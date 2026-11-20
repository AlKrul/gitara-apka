/**
 * Etap 4 potoku DSP (V2): Maszyna stanów aktywnych dźwięków MIDI (Note On / Sustain / -20 dB Note Off / Re-trigger).
 *  - Sustain monitoruje surowe widmo S(t) z uwzględnieniem 3. harmonicznej dla niskich strun (mikrofony z obciętym basem!).
 *  - Brak blokady absDiff === 1: szybkie przejścia o półton (np. 40->41, 45->46) działają natychmiast i bez przesunięć!
 *  - Gdy MPM jednoznacznie potwierdza zmianę pojedynczej nuty (np. przejście na tej samej strunie), stara nuta gaśnie czysto.
 */
export class ActiveNoteTracker {
  constructor() {
    this.cfg = null;
    this.releaseRatioLinear = 0.1;
    this.baseNoiseFloorLinear = 0.002;
    this.noiseFloorLinear = 0.002;
    this.retriggerBoostLinear = 1.78;
    this.activeNotes = [];
    this.pending = [];
  }

  prepare(config) {
    this.cfg = config;
    this.releaseRatioLinear = Math.pow(10, Math.min(-3.0, config.releaseDropDb) / 20);
    this.baseNoiseFloorLinear = Math.pow(10, config.noiseFloorDb / 20);
    this.noiseFloorLinear = this.baseNoiseFloorLinear;
    this.retriggerBoostLinear = Math.pow(10, Math.max(1.0, config.retriggerFluxBoostDb) / 20);
    this.reset();
  }

  reset() {
    this.activeNotes = [];
    this.pending = [];
    this.noiseFloorLinear = this.baseNoiseFloorLinear;
  }

  readBandPeak(cqt, spec, midiNote) {
    if (!spec || spec.length === 0) return 0;
    const center = cqt.midiToBinIndex(midiNote);
    const lo = Math.max(0, center - 1);
    const hi = Math.min(spec.length - 1, center + 1);
    let peak = 0;
    for (let k = lo; k <= hi; k++) {
      if (spec[k] > peak) peak = spec[k];
    }
    return peak;
  }

  /**
   * Efektywna energia podtrzymania dla nuty (spójna z onsetDetector: f0 + wsparcie 2./3. harmonicznej,
   * żeby słabe f0 przy grze przy mostku lub przez mikrofon słuchawkowy nie zrywało Sustainu!).
   */
  readSustainEnergy(cqt, spec, midiNote) {
    const e1 = this.readBandPeak(cqt, spec, midiNote);
    const e2 = (midiNote + 12 <= cqt.maxMidiNote) ? this.readBandPeak(cqt, spec, midiNote + 12) : 0;
    const e3 = (midiNote + 19 <= cqt.maxMidiNote) ? this.readBandPeak(cqt, spec, midiNote + 19) : 0;
    return Math.max(e1, 0.55 * e2, 0.60 * e3);
  }

  energyToVelocity(energy) {
    const db = 20 * Math.log10(Math.max(1e-6, energy));
    const norm = Math.max(0, Math.min(1, (db + 52) / 44));
    return Math.max(1, Math.min(127, Math.round(18 + norm * 109)));
  }

  processHop(cqt, currentSpectrum, fluxSpectrum, candidates, mpmInfo, currentTimeMs, outEvents) {
    const curRms = mpmInfo ? mpmInfo.rms : 0;
    const shortRms = (mpmInfo && mpmInfo.shortRms !== undefined) ? mpmInfo.shortRms : curRms;

    // 1. Delikatna adaptacja globalnego progu szumu tła gdy nic nie gra
    if (this.activeNotes.length === 0 && currentSpectrum.length > 0) {
      let sum = 0;
      for (let k = 0; k < currentSpectrum.length; k++) sum += currentSpectrum[k];
      const avg = sum / currentSpectrum.length;
      const targetFloor = Math.max(this.baseNoiseFloorLinear, avg * 2.2);
      this.noiseFloorLinear = 0.94 * this.noiseFloorLinear + 0.06 * targetFloor;
    }

    // 2. SUSTAIN & NOTE OFF na surowym widmie S(t)
    // `mpmPureSingle` może wygaszać stare nuty WYŁĄCZNIE gdy widmo potwierdziło prawdziwą 1 strunę (brak polifonii!)
    const mpmPureSingle = !!(mpmInfo && mpmInfo.valid && mpmInfo.isTrulyMonophonic && mpmInfo.clarity >= 0.91 && curRms >= this.cfg.minRmsGate);

    this.activeNotes.sort((a, b) => a.midiNote - b.midiNote);
    for (let i = 0; i < this.activeNotes.length;) {
      const note = this.activeNotes[i];
      const rawEnergy = this.readSustainEnergy(cqt, currentSpectrum, note.midiNote);

      let bleed = 0;
      for (let j = 0; j < i; j++) {
        const diff = note.midiNote - this.activeNotes[j].midiNote;
        if (diff === 12) bleed += this.cfg.lowerHarmonicBleed * this.activeNotes[j].currentEnergy;
        else if (diff === 19) bleed += this.cfg.lowerHarmonicBleed * 0.65 * this.activeNotes[j].currentEnergy;
      }
      const effectiveEnergy = Math.max(0, rawEnergy - bleed);
      note.currentEnergy = effectiveEnergy;
      note.durationMs = Math.max(0, currentTimeMs - note.onsetTimeMs);

      if ((currentTimeMs - note.lastTriggerTimeMs) <= this.cfg.attackPeakWindowMs && effectiveEnergy > note.peakEnergy) {
        note.peakEnergy = effectiveEnergy;
      }

      // Płynna aktualizacja wysokości (centów): jeśli MPM pewnie śledzi tę nutę, użyj precyzji MPM (±0.2 ct)
      if (mpmInfo && mpmInfo.valid && mpmInfo.clarity >= 0.82 && mpmInfo.midiNote === note.midiNote) {
        note.exactMidi = 0.55 * note.exactMidi + 0.45 * mpmInfo.exactMidi;
        note.frequencyHz = cqt.midiToFreqHz(note.exactMidi);
        note.centsOffset = (note.exactMidi - note.midiNote) * 100;
      } else if (effectiveEnergy > this.noiseFloorLinear * 1.5) {
        const cBin = cqt.midiToBinIndex(note.midiNote);
        const refined = cqt.refineExactMidi(currentSpectrum, cBin);
        if (Math.abs(refined - note.midiNote) <= 0.65) {
          note.exactMidi = 0.75 * note.exactMidi + 0.25 * refined;
          note.frequencyHz = cqt.midiToFreqHz(note.exactMidi);
          note.centsOffset = (note.exactMidi - note.midiNote) * 100;
        }
      }

      // Jeśli sygnał jest czysto monofoniczny na ZUPEŁNIE INNEJ nucie (np. przesunięcie palca na sąsiedni próg tej samej struny!),
      // stara nuta z tej struny już nie brzmi — pozwól jej wygasnąć szybciej zamiast wisieć!
      const replacedByPureMono = mpmPureSingle && mpmInfo.midiNote !== note.midiNote &&
        (Math.abs(mpmInfo.midiNote - note.midiNote) <= 2 || effectiveEnergy < note.peakEnergy * 0.35);

      const hasBeatingNeighborSustain = this.activeNotes.some(
        n => n.midiNote !== note.midiNote && Math.abs(n.midiNote - note.midiNote) <= 2
      );
      const releaseHopsNeeded = hasBeatingNeighborSustain
        ? Math.max(10, this.cfg.releaseConfirmHops * 3)
        : Math.max(1, this.cfg.releaseConfirmHops);

      const releaseThreshold = Math.max(this.noiseFloorLinear, note.peakEnergy * this.releaseRatioLinear);
      if (effectiveEnergy < releaseThreshold || shortRms < this.cfg.minRmsGate * 0.70 || replacedByPureMono) {
        note.quietHopCount++;
      } else {
        note.quietHopCount = 0;
      }

      if (note.quietHopCount >= releaseHopsNeeded) {
        outEvents.push({
          type: 'noteOff',
          midiNote: note.midiNote,
          exactMidi: note.exactMidi,
          frequencyHz: note.frequencyHz,
          centsOffset: note.centsOffset,
          velocity: 0,
          timestampMs: currentTimeMs,
          durationMs: note.durationMs,
          isRetrigger: false
        });
        this.activeNotes.splice(i, 1);
      } else {
        i++;
      }
    }

    // 3. POTWIERDZANIE ATAKÓW + NOTE ON + RE-TRIGGERING
    const nextPending = [];
    for (const cand of candidates) {
      const prev = this.pending.find(p => p.midiNote === cand.midiNote);
      // W ścieżce widmowej VQT (bez MPM) wstrzymaj krawędziowe półtonowe wahania ataku (>±35 ct) aż pik się ustabilizuje
      const nearBoundary = !cand.fromMpm && Math.abs(cand.centsOffset) > 35;
      const pitchDrift = prev ? Math.abs(cand.exactMidi - prev.exactMidi) : 0;
      const transientUnsettled = prev && nearBoundary && (pitchDrift > 0.10 || cand.fluxPeakEnergy > prev.fluxPeakEnergy * 1.20);
      const hits = transientUnsettled
        ? Math.min(prev.hits, Math.max(1, (this.cfg.polyOnsetConfirmHops || 4) - 1))
        : ((prev ? prev.hits : 0) + 1);

      // Jeśli kandydat NIE pochodzi z MPM, wstrzymaj górną harmoniczną (+12 / +19) gdy niżej JUŻ rośnie
      // nieaktywna jeszcze podstawa f0 (krótsze okno VQT na wyższej harmonicznej dobiega 10–25 ms wcześniej niż f0!)
      let lowerFundRising = false;
      if (!cand.fromMpm) {
        const isStrongFluxPeakAt = (m) => {
          if (m < this.cfg.minMidiNote) return false;
          if (this.activeNotes.some(n => n.midiNote === m)) return false;
          const b = cqt.midiToBinIndex(m);
          const v = this.readBandPeak(cqt, fluxSpectrum, m);
          if (v < this.cfg.minFluxPeakLinear * 0.65 || v < cand.fluxPeakEnergy * 0.14) return false;
          const sCenter = currentSpectrum[b] || 0;
          const sL = b > 0 ? currentSpectrum[b - 1] : 0;
          const sR = b + 1 < currentSpectrum.length ? currentSpectrum[b + 1] : 0;
          return sCenter >= sL * 0.90 && sCenter >= sR * 0.90;
        };
        lowerFundRising = isStrongFluxPeakAt(cand.midiNote - 12) || isStrongFluxPeakAt(cand.midiNote - 19);
      }

      // Ścieżka A (MPM) ma natychmiastową precyzję czasową (2 ramki = 10.6 ms; przy skoku -12 półt. o niskiej klarowności +1 ramka);
      // Ścieżka B (polifoniczne VQT+Flux) wymaga 4 ramek (~21 ms), a gdy niżej rośnie podstawa f0 — czeka dodatkowe 4 ramki (~21 ms)!
      const isSubOctaveDip = cand.fromMpm && cand.confidence < 0.90 &&
        this.activeNotes.some(n => n.midiNote === cand.midiNote + 12);
      const baseConfirm = cand.fromMpm
        ? (Math.max(1, this.cfg.onsetConfirmHops) + (isSubOctaveDip ? 2 : 0))
        : Math.max(3, this.cfg.polyOnsetConfirmHops || 4);
      const requiredHops = baseConfirm + (lowerFundRising ? 4 : 0);
      if (hits < requiredHops) {
        nextPending.push({ ...cand, hits });
        continue;
      }

      const existing = this.activeNotes.find(n => n.midiNote === cand.midiNote);
      if (existing) {
        const sinceLastMs = currentTimeMs - existing.lastTriggerTimeMs;
        const fluxBand = Math.max(
          this.readBandPeak(cqt, fluxSpectrum, cand.midiNote),
          cand.fluxPeakEnergy
        );
        // Gdy aktywny jest bardzo bliski sąsiad (≤ 2 półtony, np. nakładające się E2 + F2), dudnienia (beating 5–15 Hz)
        // cyklicznie podbijają amplitudę do E_peak. Prawdziwy re-trigger wymaga wtedy realnego przebicia ponad E_peak lub czystego MPM!
        const hasBeatingNeighbor = this.activeNotes.some(
          n => n.midiNote !== existing.midiNote && Math.abs(n.midiNote - existing.midiNote) === 1
        );
        const strongFluxJump = hasBeatingNeighbor
          ? (cand.fromMpm && fluxBand >= existing.peakEnergy * 0.55 && cand.currentBandEnergy >= existing.peakEnergy * 1.15)
          : (fluxBand >= this.cfg.minFluxPeakLinear * 1.30 &&
             (fluxBand >= existing.currentEnergy * this.cfg.retriggerMinFluxRatio ||
              cand.currentBandEnergy >= existing.currentEnergy * this.retriggerBoostLinear));

        if (sinceLastMs >= this.cfg.retriggerCooldownMs && strongFluxJump) {
          outEvents.push({
            type: 'noteOff',
            midiNote: existing.midiNote,
            exactMidi: existing.exactMidi,
            frequencyHz: existing.frequencyHz,
            centsOffset: existing.centsOffset,
            velocity: 0,
            timestampMs: currentTimeMs,
            durationMs: Math.max(0, currentTimeMs - existing.onsetTimeMs),
            isRetrigger: true
          });

          const newPeak = Math.max(cand.currentBandEnergy, cand.fluxPeakEnergy);
          existing.exactMidi = cand.exactMidi;
          existing.frequencyHz = cand.frequencyHz;
          existing.centsOffset = cand.centsOffset;
          existing.peakEnergy = newPeak;
          existing.currentEnergy = newPeak;
          existing.onsetTimeMs = currentTimeMs;
          existing.lastTriggerTimeMs = currentTimeMs;
          existing.durationMs = 0;
          existing.velocity = this.energyToVelocity(newPeak);
          existing.quietHopCount = 0;

          outEvents.push({
            type: 'noteOn',
            midiNote: existing.midiNote,
            exactMidi: existing.exactMidi,
            frequencyHz: existing.frequencyHz,
            centsOffset: existing.centsOffset,
            velocity: existing.velocity,
            timestampMs: currentTimeMs,
            durationMs: 0,
            isRetrigger: true
          });
        } else {
          nextPending.push({ ...cand, hits });
        }
        continue;
      }

      // Ochrona przed harmonicznymi duchami (+12/+19/+24/+28 półt.) z już aktywnych niższych strun
      // (np. dudnienia 4./5. harmonicznej w 3-strunowych akordach), przy pełnym przepuszczeniu realnych strun power-chordu (E5, A5, D)!
      let isFreshHarmonicGhost = false;
      if (!cand.fromMpm) {
        for (const other of this.activeNotes) {
          const diff = cand.midiNote - other.midiNote;
          const ageMs = currentTimeMs - other.lastTriggerTimeMs;
          if (diff === 24 || diff === 28) {
            // 4. / 5. harmoniczna (2+ oktawy wyżej): dudnienia akordu mają mały flux — realna struna musi mieć wyraźny atak!
            if (cand.fluxPeakEnergy < other.peakEnergy * 0.65) {
              isFreshHarmonicGhost = true;
              break;
            }
          } else if (diff === 12 || diff === 19) {
            if (ageMs <= 60 && cand.fluxPeakEnergy < other.peakEnergy * 0.34) {
              isFreshHarmonicGhost = true;
              break;
            }
            if (ageMs > 60 && cand.fluxPeakEnergy < other.currentEnergy * 0.45) {
              isFreshHarmonicGhost = true;
              break;
            }
          }
        }
      }
      if (isFreshHarmonicGhost) continue;

      // Jeśli chwilę wcześniej (< 45 ms) odpaliła szybsza harmoniczna (+12 lub +19 półt.) TEGO SAMEGO ataku,
      // usuń ten prekursor harmoniczny teraz, gdy dotarła właściwa podstawa f0!
      for (let pIdx = this.activeNotes.length - 1; pIdx >= 0; pIdx--) {
        const up = this.activeNotes[pIdx];
        const diffUp = up.midiNote - cand.midiNote;
        if ((diffUp === 12 || diffUp === 19) && (currentTimeMs - up.lastTriggerTimeMs) <= 45) {
          outEvents.push({
            type: 'noteOff',
            midiNote: up.midiNote,
            exactMidi: up.exactMidi,
            frequencyHz: up.frequencyHz,
            centsOffset: up.centsOffset,
            velocity: 0,
            timestampMs: currentTimeMs,
            durationMs: up.durationMs,
            isRetrigger: false
          });
          this.activeNotes.splice(pIdx, 1);
        }
      }

      if (this.activeNotes.length >= this.cfg.maxActiveNotes) {
        let qIdx = 0;
        for (let k = 1; k < this.activeNotes.length; k++) {
          if (this.activeNotes[k].currentEnergy < this.activeNotes[qIdx].currentEnergy) qIdx = k;
        }
        const ev = this.activeNotes.splice(qIdx, 1)[0];
        outEvents.push({
          type: 'noteOff',
          midiNote: ev.midiNote,
          exactMidi: ev.exactMidi,
          frequencyHz: ev.frequencyHz,
          centsOffset: ev.centsOffset,
          velocity: 0,
          timestampMs: currentTimeMs,
          durationMs: ev.durationMs,
          isRetrigger: false
        });
      }

      const initPeak = Math.max(cand.currentBandEnergy, cand.fluxPeakEnergy);
      const vel = this.energyToVelocity(initPeak);
      this.activeNotes.push({
        midiNote: cand.midiNote,
        exactMidi: cand.exactMidi,
        frequencyHz: cand.frequencyHz,
        centsOffset: cand.centsOffset,
        peakEnergy: initPeak,
        currentEnergy: initPeak,
        onsetTimeMs: currentTimeMs,
        lastTriggerTimeMs: currentTimeMs,
        durationMs: 0,
        velocity: vel,
        quietHopCount: 0
      });

      outEvents.push({
        type: 'noteOn',
        midiNote: cand.midiNote,
        exactMidi: cand.exactMidi,
        frequencyHz: cand.frequencyHz,
        centsOffset: cand.centsOffset,
        velocity: vel,
        timestampMs: currentTimeMs,
        durationMs: 0,
        isRetrigger: false
      });
    }

    this.pending = nextPending;
  }
}
