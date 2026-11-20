/**
 * Etap 3 potoku DSP (V2 — odporny na krok o półton, mikrofon słuchawkowy i błędy oktawy):
 *  - Positive Spectral Flux ΔS(t) = max(0, S(t) - β·S(t - Δt)) wykrywa świeży atak i izoluje nowe pasma
 *    od wybrzmiewającego tła, ALE lokalizacja wysokości (f0) kotwiczona jest na:
 *      (a) rzeczywistym widmie S(t) oraz nierozmytych wyższych harmonicznych h=3..6 (eliminuje przesunięcie +1 -> +2 półtony!),
 *      (b) arbitrze nieparzystych harmonicznych h=3, h=5 (jednoznaczna identyfikacja oktawy nawet przy słabym f0!),
 *      (c) weryfikatorze czasowym MPM (NSDF), gdy sygnał ma wysoką klarowność (pojedyncze nuty, szybkie skale chromatyczne).
 */
export class SpectralOnsetHps {
  constructor() {
    this.cfg = null;
    this.binCount = 0;
    this.lookbackHops = 9;
    this.historyWriteIdx = 0;
    this.framesSeen = 0;
    this.minFundBin = 0;
    this.maxFundBin = 0;
    this.harmonicBinOffsets = [];
    this.spectrumHistory = [];
    this.rmsHistory = [];
    this.deltaSpec = new Float32Array(0);
    this.workSpec = new Float32Array(0);
    this.workDelta = new Float32Array(0);
    this.hpsScores = new Float32Array(0);
    this.totalFlux = 0;
  }

  prepare(config, cqt) {
    this.cfg = config;
    this.binCount = cqt.binCount;

    const hopMs = config.sampleRate > 0 ? (1000 * config.hopSize) / config.sampleRate : 5.3333;
    this.lookbackHops = Math.max(2, Math.min(24, Math.round(config.fluxLookbackMs / Math.max(1, hopMs))));

    const histLen = this.lookbackHops + 6;
    this.spectrumHistory = Array.from({ length: histLen }, () => new Float32Array(this.binCount));
    this.rmsHistory = new Float32Array(histLen);
    this.deltaSpec = new Float32Array(this.binCount);
    this.workSpec = new Float32Array(this.binCount);
    this.workDelta = new Float32Array(this.binCount);
    this.hpsScores = new Float32Array(this.binCount);

    this.minFundBin = 0;
    this.maxFundBin = Math.max(0, this.binCount - 1);
    for (let k = 0; k < this.binCount; k++) {
      if (cqt.bins[k].freqHz >= config.fundamentalMinHz) {
        this.minFundBin = k;
        break;
      }
    }
    for (let k = this.binCount - 1; k >= 0; k--) {
      if (cqt.bins[k].freqHz <= config.fundamentalMaxHz) {
        this.maxFundBin = k;
        break;
      }
    }

    const binsPerOct = cqt.binsPerSemitone * 12;
    this.harmonicBinOffsets = config.harmonicWeights.map((_, h) => Math.round(binsPerOct * Math.log2(h + 1)));
    this.reset();
  }

  reset() {
    for (const f of this.spectrumHistory) f.fill(0);
    this.rmsHistory.fill(0);
    this.deltaSpec.fill(0);
    this.workSpec.fill(0);
    this.workDelta.fill(0);
    this.hpsScores.fill(0);
    this.historyWriteIdx = 0;
    this.framesSeen = 0;
    this.totalFlux = 0;
  }

  /**
   * Oblicza energię pasma +/-1 bin w zadanym widmie dla danego binu.
   */
  readBand(spec, binIdx) {
    if (binIdx < 0 || binIdx >= this.binCount) return 0;
    let v = spec[binIdx];
    if (binIdx > 0 && spec[binIdx - 1] > v) v = spec[binIdx - 1];
    if (binIdx + 1 < this.binCount && spec[binIdx + 1] > v) v = spec[binIdx + 1];
    return v;
  }

  /**
   * Arbiter oktawy oparty o nieparzyste harmoniczne (h=1, h=3, h=5):
   *  - Wykrywa, czy z powodu słabego f0 (mikrofon słuchawkowy / przetwornik mostkowy) nie wybrano +1 oktawy za wysoko.
   *  - Wykrywa, czy z powodu fałszywej sub-oktawy nie wybrano -1 oktawy za nisko (brak energii w h=1, h=3, h=5).
   */
  resolveOctaveBin(cqt, spec, deltaSpec, candidateBin) {
    const bps = cqt.binsPerSemitone;
    const octBins = 12 * bps;
    const h3Off = this.harmonicBinOffsets[2]; // +19 półt. (3*f0)
    const h5Off = this.harmonicBinOffsets[4]; // +28 półt. (5*f0)
    let bin = candidateBin;

    // 1. Sprawdź, czy rzeczywista nuta nie leży OKTAWĘ NIŻEJ (bin - 12 półtonów):
    //    Wymagamy RZECZYWISTEGO lokalnego piku na downBin (nie opadającego zbocza sąsiedniej struny w akordzie!)
    const downBin = bin - octBins;
    if (downBin >= this.minFundBin) {
      const sCand = this.readBand(spec, bin);
      const sDownCenter = spec[downBin];
      const downIsPeak = sDownCenter > 0 &&
        (downBin === 0 || sDownCenter >= spec[downBin - 1]) &&
        (downBin + 1 >= this.binCount || sDownCenter >= spec[downBin + 1]);
      if (downIsPeak) {
        const sDown3 = (downBin + h3Off < this.binCount) ? this.readBand(spec, downBin + h3Off) : 0;
        const dDown1 = deltaSpec[downBin];
        const dDown3 = (downBin + h3Off < this.binCount) ? this.readBand(deltaSpec, downBin + h3Off) : 0;
        if (sDownCenter >= sCand * 0.25 && dDown1 >= sCand * 0.22 &&
            sDown3 >= sCand * 0.35 && dDown3 >= sCand * 0.32) {
          bin = downBin;
        }
      }
    }

    // 2. Sprawdź, czy wybrany bin nie jest FAŁSZYWĄ SUB-OKTAWĄ nuty leżącej OKTAWĘ WYŻEJ (bin + 12 półtonów):
    //    Prawdziwa nuta na `bin` MUSI posiadać energię nieparzystych harmonicznych (h1, h3 lub h5).
    const upBin = bin + octBins;
    if (upBin <= this.maxFundBin) {
      const s1 = this.readBand(spec, bin);
      const s2 = this.readBand(spec, upBin);
      const s3 = (bin + h3Off < this.binCount) ? this.readBand(spec, bin + h3Off) : 0;
      const s5 = (bin + h5Off < this.binCount) ? this.readBand(spec, bin + h5Off) : 0;
      const oddSum = s1 + s3 + s5;
      if (s2 > this.cfg.minFluxPeakLinear && s1 < s2 * 0.18 && s3 < s2 * 0.18 && oddSum < s2 * 0.34) {
        bin = upBin;
      }
    }
    return bin;
  }

  /**
   * Sprawdza, czy bieżąca ramka jest RZECZYWIŚCIE monofoniczna (jedna struna), czy polifoniczna (≥2 struny / power-chord).
   *  - Wykrywa „wirtualną podstawę” power-chordu (np. D3+A3 = 50+57 daje w MPM wirtualne D2 = 38 bez f0 i bez 5*f0!).
   *  - Mierzy udział energii spoza szeregu harmonicznego nuty MPM (jeśli > 24% -> brzmią co najmniej 2 niezależne struny!).
   */
  classifyMpmMonophony(cqt, currentSpectrum, mpmInfo) {
    if (!mpmInfo || !mpmInfo.valid) {
      return { isTrulyMonophonic: false, isVirtualFund: false };
    }
    const nBins = this.binCount;
    const mBin = cqt.midiToBinIndex(mpmInfo.midiNote);
    const h2Off = this.harmonicBinOffsets[1];
    const h3Off = this.harmonicBinOffsets[2];
    const h5Off = this.harmonicBinOffsets[4];

    const s1 = this.readBand(currentSpectrum, mBin);
    const s2 = (mBin + h2Off < nBins) ? this.readBand(currentSpectrum, mBin + h2Off) : 0;
    const s3 = (mBin + h3Off < nBins) ? this.readBand(currentSpectrum, mBin + h3Off) : 0;
    const s5 = (mBin + h5Off < nBins) ? this.readBand(currentSpectrum, mBin + h5Off) : 0;

    // Power-chord 2*f0 + 3*f0 (np. D3 + A3) tworzy w MPM wirtualne f0 oktawę niżej (D2),
    // ale w widmie nie ma ani f0 (s1 < 4% s2), ani 5. harmonicznej (s5 < 14% s3 — prawdziwa struna ją posiada!).
    const isVirtualFund = (s2 > this.cfg.minFluxPeakLinear) && (s3 > s2 * 0.30) &&
                          (s1 < s2 * 0.04) && (s5 < s3 * 0.14);
    if (isVirtualFund) {
      return { isTrulyMonophonic: false, isVirtualFund: true };
    }

    // Zlicz energię lokalnych pików w pasmie f0 gitary należących do harmonicznych nuty MPM vs spoza nich
    const onHarm = new Uint8Array(nBins);
    const bps = cqt.binsPerSemitone;
    for (let h = 1; h <= 8; h++) {
      const hk = mBin + Math.round(bps * 12 * Math.log2(h));
      if (hk >= nBins) break;
      for (let d = -2; d <= 2; d++) {
        const j = hk + d;
        if (j >= 0 && j < nBins) onHarm[j] = 1;
      }
    }

    let harmSum = 0;
    let otherSum = 0;
    const scanHi = Math.min(nBins - 2, this.maxFundBin + 12 * bps);
    for (let k = Math.max(1, this.minFundBin); k <= scanHi; k++) {
      const v = currentSpectrum[k];
      if (v < this.cfg.minFluxPeakLinear * 0.70) continue;
      if (v >= currentSpectrum[k - 1] && v >= currentSpectrum[k + 1]) {
        if (onHarm[k]) harmSum += v;
        else otherSum += v;
      }
    }

    const otherRatio = otherSum / (harmSum + 1e-6);
    // Prawdziwa 1 struna ma >80% energii pików na własnych harmonicznych (otherRatio < 0.24)
    return { isTrulyMonophonic: otherRatio < 0.24, isVirtualFund: false };
  }

  processFrame(cqt, currentSpectrum, mpmInfo, activeNotes = []) {
    const outCandidates = [];
    const nBins = this.binCount;
    if (nBins === 0 || currentSpectrum.length !== nBins) return outCandidates;

    const histLen = this.spectrumHistory.length;
    const effectiveBack = Math.min(this.lookbackHops, this.framesSeen);
    const back2 = Math.min(this.lookbackHops + 1, this.framesSeen);
    const refIdx1 = (this.historyWriteIdx + histLen - Math.max(1, effectiveBack)) % histLen;
    const refIdx2 = (this.historyWriteIdx + histLen - Math.max(1, back2)) % histLen;
    const ref1 = this.spectrumHistory[refIdx1];
    const ref2 = this.spectrumHistory[refIdx2];

    // Średnie krótkie RMS sprzed ~20 ms: natychmiast wykrywa wyciszenie struny (mute/release click!)
    const prevRmsIdx = (this.historyWriteIdx + histLen - Math.max(1, Math.min(3, this.framesSeen))) % histLen;
    const recentRms = this.framesSeen > 0 ? this.rmsHistory[prevRmsIdx] : 0;
    const curRms = mpmInfo ? mpmInfo.rms : 0;
    const shortRms = mpmInfo && mpmInfo.shortRms !== undefined ? mpmInfo.shortRms : curRms;

    const beta = this.cfg.fluxSubtractFactor;
    let sumFlux = 0;
    let maxDelta = 0;
    let maxSpec = 0;
    for (let k = 0; k < nBins; k++) {
      const sVal = currentSpectrum[k];
      const bg = this.framesSeen === 0 ? 0 : Math.max(ref1[k], ref2[k]);
      const d = Math.max(0, sVal - beta * bg);
      this.deltaSpec[k] = d;
      this.workDelta[k] = d;
      this.workSpec[k] = sVal;
      if (k >= this.minFundBin) {
        sumFlux += d;
        if (d > maxDelta) maxDelta = d;
        if (sVal > maxSpec) maxSpec = sVal;
      }
    }
    this.totalFlux = sumFlux;

    // Zapisz bieżące widmo i krótkie RMS do historii kołowej
    this.spectrumHistory[this.historyWriteIdx].set(currentSpectrum);
    this.rmsHistory[this.historyWriteIdx] = shortRms;
    this.historyWriteIdx = (this.historyWriteIdx + 1) % histLen;
    this.framesSeen++;

    // Odrzuć ciszę oraz gwałtowny zanik po wyciszeniu struny (mute click: shortRms spada natychmiast w 5 ms!)
    if (curRms < this.cfg.minRmsGate || shortRms < this.cfg.minRmsGate * 0.80) return outCandidates;
    const isMuteCollapse = (recentRms > this.cfg.minRmsGate * 3.0) && (shortRms < recentRms * 0.38);
    if (isMuteCollapse) return outCandidates;

    const mpmValid = !!(mpmInfo && mpmInfo.valid && mpmInfo.midiNote >= this.cfg.minMidiNote && mpmInfo.midiNote <= this.cfg.maxMidiNote);
    const monoCheck = mpmValid ? this.classifyMpmMonophony(cqt, currentSpectrum, mpmInfo) : { isTrulyMonophonic: false, isVirtualFund: false };
    if (mpmInfo) {
      mpmInfo.isTrulyMonophonic = monoCheck.isTrulyMonophonic && !monoCheck.isVirtualFund;
      mpmInfo.isVirtualFund = monoCheck.isVirtualFund;
    }
    const mpmStrong = mpmValid && !monoCheck.isVirtualFund && monoCheck.isTrulyMonophonic && mpmInfo.clarity >= this.cfg.mpmClarityThreshold;
    const mpmAlreadyActive = mpmValid && activeNotes.some(n => n.midiNote === mpmInfo.midiNote);

    // =========================================================================
    // ŚCIEŻKA A (MONOFONICZNA / SZYBKIE PRZEBIEGI / KROKI O PÓŁTON / LEGATO):
    // Działa WYŁĄCZNIE gdy MPM ma wysoką klarowność ORAZ widmo potwierdza 1 strunę (brak polifonii i wirtualnych kwint!).
    // =========================================================================
    if (mpmStrong) {
      const mBin = cqt.midiToBinIndex(mpmInfo.midiNote);
      const rawBandE = this.readBand(currentSpectrum, mBin);
      const rawH2 = (mBin + this.harmonicBinOffsets[1] < nBins) ? this.readBand(currentSpectrum, mBin + this.harmonicBinOffsets[1]) : 0;
      const rawH3 = (mBin + this.harmonicBinOffsets[2] < nBins) ? this.readBand(currentSpectrum, mBin + this.harmonicBinOffsets[2]) : 0;
      const effectiveBandE = Math.max(rawBandE, 0.75 * rawH2, 0.65 * rawH3);

      let combFlux = 0;
      for (let h = 0; h < this.harmonicBinOffsets.length; h++) {
        const hk = mBin + this.harmonicBinOffsets[h];
        if (hk >= nBins) break;
        const dH = this.readBand(this.deltaSpec, hk);
        if (dH > combFlux) combFlux = dH;
      }

      // Nowa nuta monofoniczna ZAWSZE wymaga realnego przyrostu energii w swoim grzebieniu harmonicznych (combFlux > 0),
      // dzięki czemu wygasający ogon nuty po Note Off (-20 dB) NIGDY nie odpali fałszywego drugiego Note On!
      const minMonoFlux = this.cfg.minFluxPeakLinear * 0.40;
      const isCleanNewPitch = !mpmAlreadyActive && combFlux >= minMonoFlux;
      const isRetriggerJump = mpmAlreadyActive && combFlux >= this.cfg.minFluxPeakLinear * 1.25;

      if ((isCleanNewPitch || isRetriggerJump) && effectiveBandE >= this.cfg.minFluxPeakLinear * 0.65) {
        outCandidates.push({
          midiNote: mpmInfo.midiNote,
          exactMidi: mpmInfo.exactMidi,
          frequencyHz: mpmInfo.frequencyHz,
          centsOffset: mpmInfo.centsOffset,
          fluxPeakEnergy: Math.max(combFlux, this.cfg.minFluxPeakLinear * 1.1),
          currentBandEnergy: effectiveBandE,
          hpsScore: 2.0 + mpmInfo.clarity,
          confidence: mpmInfo.clarity,
          fromMpm: true
        });
      }
      // Wysoka okresowość (clarity >= 0.82) = jedna dominująca struna -> zwróć wyłącznie czystą nutę MPM!
      return outCandidates;
    }

    // =========================================================================
    // ŚCIEŻKA B (POLIFONICZNA — NAKŁADAJĄCE SIĘ WYBRZMIEWAJĄCE STRUNY I AKORDY):
    // ΔS(t) wykrywa świeżo uderzoną strunę ponad wybrzmiewającym tłem,
    // a kotwiczenie piku na rzeczywistym widmie S(t) + wysokich harmonicznych h=3..6
    // zapobiega przesunięciu półtonu o +1 półton w bok!
    // =========================================================================
    if (maxDelta < this.cfg.minFluxPeakLinear) return outCandidates;

    const alpha = this.cfg.logCompressionAlpha;
    const weights = this.cfg.harmonicWeights;
    const offsets = this.harmonicBinOffsets;
    const workD = this.workDelta;
    const workS = this.workSpec;
    const origS = currentSpectrum;
    const origD = this.deltaSpec;

    const readHarm = (arr, baseBin, hIdx) => {
      const hk = baseBin + offsets[hIdx];
      if (hk >= nBins) return 0;
      let v = arr[hk];
      if (hIdx >= 2) {
        // Lekka sztywność struny (inharmoniczność) w górę dla h>=3
        if (hk + 1 < nBins) v = Math.max(v, 0.85 * arr[hk + 1]);
        if (hk > 0) v = Math.max(v, 0.70 * arr[hk - 1]);
      }
      return v;
    };

    const isLocalPeak = (arr, k) => {
      const v = arr[k];
      if (v <= 0) return false;
      const l = k > 0 ? arr[k - 1] : 0;
      const r = k + 1 < nBins ? arr[k + 1] : 0;
      return v >= l && v >= r && (v > l || v > r);
    };

    const suppressed = new Uint8Array(nBins);
    for (const c of outCandidates) {
      const cb = cqt.midiToBinIndex(c.midiNote);
      for (let j = Math.max(0, cb - 2); j <= Math.min(nBins - 1, cb + 2); j++) suppressed[j] = 1;
    }

    const lobeWeights = [0.45, 0.85, 1.0, 0.85, 0.45];
    const harmCap = [1.10, 0.88, 0.78, 0.56, 0.38, 0.25];

    // Kluczowe dla 3-strunowych akordów i arpeggiów:
    // Jeśli niższa struna jest JUŻ aktywna (w `activeNotes`) i wciąż ma dodatni flux ΔS z ostatnich ~50 ms,
    // odejmij addytywnie jej własne harmoniczne h=2..6 z `workD`!
    // Dzięki temu jej 2./3. harmoniczna (+12/+19) NIE wywoła fałszywego dźwięku, a prawdziwa wyższa struna
    // (np. E3 w power-chordzie E5 = 40+47+52) zachowa swój własny nadmiarowy flux!
    for (let aIdx = 0; aIdx < activeNotes.length; aIdx++) {
      const an = activeNotes[aIdx];
      // Odejmujemy harmoniczne tylko świeżo uderzonych aktywnych strun (<= 80 ms), żeby nie tłumić re-triggeringu starej struny!
      if (an.durationMs > 80) continue;
      const aBin = cqt.midiToBinIndex(an.midiNote);
      const aFlux = this.readBand(origD, aBin);
      if (aFlux < this.cfg.minFluxPeakLinear * 0.45) continue;
      for (let h = 1; h < weights.length; h++) {
        const hk = aBin + offsets[h];
        if (hk >= nBins) break;
        const subFlux = Math.min(workD[hk], aFlux * harmCap[h]);
        for (let d = -2; d <= 2; d++) {
          const j = hk + d;
          if (j >= 0 && j < nBins) {
            workD[j] = Math.max(0, workD[j] - subFlux * lobeWeights[d + 2]);
          }
        }
      }
    }

    let firstBestScore = outCandidates.length > 0 ? outCandidates[0].hpsScore : 0;
    const secRatio = this.cfg.secondaryNoteMinRatio || 0.50;

    for (let iter = outCandidates.length; iter < this.cfg.maxSimultaneousOnsets; iter++) {
      this.hpsScores.fill(0);

      for (let k = this.minFundBin; k <= this.maxFundBin; k++) {
        if (suppressed[k]) continue;

        // Sprawdź, czy kandydat ma RZECZYWISTY pik w widmie S(t) (na f0 lub 2*f0)
        // oraz czy samo f0 nie leży na stromym opadającym zboczu sąsiedniej niższej nuty!
        const h2k = k + offsets[1];
        const truePeak = isLocalPeak(origS, k) || (h2k < nBins && isLocalPeak(origS, h2k));
        if (!truePeak) continue;
        if (!isLocalPeak(origS, k) && k > 0 && origS[k] < origS[k - 1] * 0.72) continue;

        // Musi istnieć realna świeża energia (flux ΔS) w grzebieniu harmonicznych (h=1..6),
        // przy czym dla niskich strun z nakładającym się półtonem decydują nierozmyte h=3..6!
        let maxHarmFlux = 0;
        let highHarmFlux = 0;
        for (let h = 0; h < weights.length; h++) {
          const dh = readHarm(workD, k, h);
          if (dh > maxHarmFlux) maxHarmFlux = dh;
          if (h >= 2 && dh > highHarmFlux) highHarmFlux = dh;
        }
        if (maxHarmFlux < this.cfg.minFluxPeakLinear * 0.80) continue;

        // Hybrydowy wynik Log-HPS:
        //  - dla h=1,2 łączymy rzeczywistą amplitudę S(t) z nowością ΔS(t) (brak zniekształcenia półtonowego!),
        //  - dla h=3..6 używamy czystego ΔS(t) (doskonała separacja półtonów i wybrzmiewającego tła!).
        let sumScore = 0;
        let wSum = 0;
        for (let h = 0; h < weights.length; h++) {
          const hk = k + offsets[h];
          if (hk >= nBins) break;
          const dh = readHarm(workD, k, h);
          const sh = readHarm(workS, k, h);
          // Jeśli h>=2 ma silny przyrost ΔS, dopuszczamy udział prawdziwego S(t) dla h=0,1 (krok o półton na wybrzmiewaniu!)
          const effVal = (h <= 1)
            ? Math.max(dh, Math.min(sh, highHarmFlux * 1.65))
            : dh;
          const w = weights[h];
          sumScore += w * Math.log1p(alpha * effVal);
          wSum += w;
        }
        if (wSum > 0) sumScore /= wSum;

        // Strażnik sub-harmonicznych w ścieżce polifonicznej (pojedyncze nuty z mikrofonu obsługuje już MPM!):
        // Jeśli na f0 nie ma lokalnego piku i energia s1 < 28% s2 (2*f0), mocno tłumimy fałszywą sub-oktawę (np. 43 zamiast 55 w akordzie)!
        const s1 = workS[k];
        const s2 = readHarm(workS, k, 1);
        const s3 = readHarm(workS, k, 2);
        const s5 = readHarm(workS, k, 4);
        const d1 = workD[k];
        const d2 = readHarm(workD, k, 1);
        const d3 = readHarm(workD, k, 2);
        const peakAtFund = isLocalPeak(origS, k);
        let subGuard = 1.0;
        if (s1 < s2 * this.cfg.subharmonicGuardRatio && s3 < s2 * 0.24) {
          subGuard = 0.14;
        } else if (!peakAtFund && s1 < s2 * 0.26) {
          subGuard = 0.22;
        } else if (d1 < this.cfg.minFluxPeakLinear * 0.45 && d3 < this.cfg.minFluxPeakLinear * 0.55) {
          subGuard = 0.30;
        }

        const l1 = Math.log1p(alpha * Math.max(d1, Math.min(s1, highHarmFlux * 1.15)));
        const l2 = Math.log1p(alpha * Math.max(d2, Math.min(s2, highHarmFlux * 1.15)));
        const l3 = Math.log1p(alpha * d3);
        const prodBonus = this.cfg.hpsProductBonus * Math.sqrt(l1 * Math.max(l2, 0.80 * l3));

        this.hpsScores[k] = (sumScore + prodBonus) * subGuard;
      }

      let bestBin = -1;
      let bestScore = 0;
      for (let k = this.minFundBin; k <= this.maxFundBin; k++) {
        if (!suppressed[k] && this.hpsScores[k] > bestScore) {
          bestScore = this.hpsScores[k];
          bestBin = k;
        }
      }

      if (bestBin < 0 || bestScore < this.cfg.minFluxScore) break;
      if (iter === 0) {
        firstBestScore = bestScore;
      } else if (bestScore < firstBestScore * secRatio) {
        break;
      }

      // Arbiter oktawy na rezydualnym widmie (workS, workD): nie podkrada harmonicznych już wykrytych nut akordu!
      const resolvedBin = this.resolveOctaveBin(cqt, workS, workD, bestBin);

      // Zablokuj promień ±2 biny (±1 półton), aby ta sama nuta nie powtórzyła się w tej samej ramce
      for (let j = Math.max(0, resolvedBin - 2); j <= Math.min(nBins - 1, resolvedBin + 2); j++) {
        suppressed[j] = 1;
      }

      const fluxAtFund = Math.max(this.readBand(workD, resolvedBin), 0.75 * readHarm(workD, resolvedBin, 1), 0.65 * readHarm(workD, resolvedBin, 2));
      const rawBandE = Math.max(this.readBand(origS, resolvedBin), 0.70 * readHarm(origS, resolvedBin, 1), 0.60 * readHarm(origS, resolvedBin, 2));

      if (fluxAtFund < this.cfg.minFluxPeakLinear) {
        workD[resolvedBin] = 0;
        continue;
      }

      // Kotwicz dokładną wysokość (centy) na RZECZYWISTYM widmie S(t) (a nie przesuniętym ΔS(t)!)
      const exactMidi = cqt.refineExactMidi(origS, resolvedBin);
      const midiNote = Math.round(exactMidi);

      if (!outCandidates.some(c => c.midiNote === midiNote)) {
        outCandidates.push({
          midiNote,
          exactMidi,
          frequencyHz: cqt.midiToFreqHz(exactMidi),
          centsOffset: (exactMidi - midiNote) * 100,
          fluxPeakEnergy: fluxAtFund,
          currentBandEnergy: rawBandE,
          hpsScore: bestScore,
          confidence: Math.max(0, Math.min(1, bestScore / (bestScore + 0.65))),
          fromMpm: false
        });
      }

      // Addytywne odejmowanie grzebienia harmonicznych (Klapuri): odejmujemy WYŁĄCZNIE oczekiwaną energię
      // harmonicznych wykrytej nuty, dzięki czemu w akordach typu E5 (40+47+52) lub D (50+57+62)
      // prawdziwa oktawa na wyższej strunie NIE zostaje wyzerowana!
      const refFluxBase = Math.max(workD[resolvedBin], 0.72 * readHarm(workD, resolvedBin, 1));
      const refSpecBase = Math.max(workS[resolvedBin], 0.72 * readHarm(workS, resolvedBin, 1));
      for (let h = 0; h < weights.length; h++) {
        const hk = resolvedBin + offsets[h];
        if (hk >= nBins) break;
        const subFluxAmp = (h === 0) ? workD[hk] : Math.min(workD[hk] * this.cfg.harmonicSubtractFactor, refFluxBase * harmCap[h]);
        const subSpecAmp = (h === 0) ? workS[hk] * 0.95 : Math.min(workS[hk] * 0.82, refSpecBase * harmCap[h]);
        for (let d = -2; d <= 2; d++) {
          const j = hk + d;
          if (j < 0 || j >= nBins) continue;
          const lw = lobeWeights[d + 2];
          workD[j] = Math.max(0, workD[j] - subFluxAmp * lw);
          workS[j] = Math.max(0, workS[j] - subSpecAmp * lw);
        }
      }
    }

    return outCandidates;
  }
}
