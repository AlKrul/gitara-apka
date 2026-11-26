/**
 * Most łączący silnik polifoniczny PitchPlease (streamingowy CNN w AudioWorklet + WASM SIMD)
 * z interfejsem Apki Gitarowej:
 *  - kondycjonowanie pasma pod wzmacniacz i mikrofon (HPF 68 Hz + tilt EQ +6 dB na cienkie struny + lekki leveler),
 *  - niezależny próg detekcji dla grubych strun (E6–D4) i cienkich strun (G3–E1) + szybkie potwierdzenie f0 Goertzlem,
 *  - filtrowanie dokładnie do zakresu stroju gitary + kalibrowana bramka ciszy RMS,
 *  - natychmiastowe zdarzenia MIDI NoteOn / Sustain / NoteOff + re-triggering tej samej struny,
 *  - 3-etapowa kalibracja na żywo z gitary użytkownika (cisza → grube struny → cienkie struny).
 */
import { create as createPitchPlease, analyze as analyzePitchPlease, models as PITCHPLEASE_MODELS, version as PITCHPLEASE_VERSION } from '../../vendor/pitchplease/pitchplease.js';

export { analyzePitchPlease, PITCHPLEASE_MODELS, PITCHPLEASE_VERSION };

export const GUITAR_MODELS = Object.freeze([
  { id: 'fast',  labelPl: 'Uniwersalny / Wzmacniacz (34 ms · 1× CPU)', labelEn: 'Universal / Amp (34 ms · 1× CPU)' },
  { id: 'voice', labelPl: 'Gitara / Głos (34 ms · GuitarSet)',         labelEn: 'Guitar / Voice (34 ms · GuitarSet)' },
  { id: 'piano', labelPl: 'Czysty akustyk / Clean (33 ms)',            labelEn: 'Clean Acoustic (33 ms)' }
]);

const LOWEST_PP_MIDI = 21; // A0
const PP_KEY_COUNT = 88;   // A0..C8
const THIN_STRING_SPLIT_MIDI = 54; // F#3/G3 i wyżej: struny nieowijane (cienkie G3, B2, E1)

export function defaultThresholdForModel(modelId) {
  if (modelId === 'piano') return { on: 0.60, highOn: 0.45, off: 0.34 };
  if (modelId === 'voice') return { on: 0.58, highOn: 0.42, off: 0.32 };
  return { on: 0.58, highOn: 0.42, off: 0.32 }; // 'fast' — najbardziej odporny na wzmacniacz/crunch i najlżejszy CPU
}

/**
 * Szybki, bezalokacyjny pomiar amplitudy Hann-Goertzla w pojedynczym punkcie częstotliwości.
 * Używany wyłącznie do potwierdzenia granicznych pików cienkich strun (MIDI >= 54) przy przesterze/wzmacniaczu.
 */
export function goertzelHannMag(pcm, sampleRate, freqHz) {
  if (!pcm || pcm.length < 256 || !(sampleRate > 0) || !(freqHz > 0)) return 0;
  const N = Math.min(2048, pcm.length);
  const start = pcm.length - N;
  const w = (2 * Math.PI * freqHz) / sampleRate;
  const cosW = Math.cos(w);
  const sinW = Math.sin(w);
  const invN = (2 * Math.PI) / N;
  const cosStep = Math.cos(invN);
  const sinStep = Math.sin(invN);

  // Rekurencyjny obrót wektora okna Hanna i oscylatora (zero Math.cos/sin w pętli!)
  let hCos = 1.0, hSin = 0.0;
  let oCos = 1.0, oSin = 0.0;
  let re = 0.0, im = 0.0;
  for (let i = 0; i < N; i++) {
    const win = 0.5 - 0.5 * hCos;
    const x = pcm[start + i] * win;
    re += x * oCos;
    im -= x * oSin;

    const nhCos = hCos * cosStep - hSin * sinStep;
    hSin = hSin * cosStep + hCos * sinStep;
    hCos = nhCos;

    const noCos = oCos * cosW - oSin * sinW;
    oSin = oSin * cosW + oCos * sinW;
    oCos = noCos;
  }
  return Math.hypot(re, im) / (N * 0.25);
}

export function measureThinStringF0Sharpness(pcm, sampleRate, midi, a4Hz = 440.0) {
  const f0 = a4Hz * Math.pow(2, (midi - 69) / 12);
  const N = pcm ? Math.min(2048, pcm.length) : 2048;
  // Boczne częstotliwości odniesienia muszą leżeć poza płatem głównym okna Hanna (~2.1 * fs/N)
  const df = Math.max(f0 * 0.055, (2.1 * sampleRate) / Math.max(256, N));
  const fL = Math.max(40, f0 - df);
  const fR = f0 + df;
  const m0 = goertzelHannMag(pcm, sampleRate, f0);
  const mSide = Math.max(
    goertzelHannMag(pcm, sampleRate, fL),
    goertzelHannMag(pcm, sampleRate, fR),
    1e-6
  );
  return { mag: m0, ratio: m0 / mSide };
}

export class GuitarEventTracker {
  constructor(options = {}) {
    this.minMidi = options.minMidi ?? 38;
    this.maxMidi = options.maxMidi ?? 88;
    this.a4Hz = options.a4Hz ?? 440.0;
    this.thresholdOn = options.thresholdOn ?? 0.58;
    this.highThresholdOn = options.highThresholdOn ?? Math.max(0.34, Math.round((this.thresholdOn - 0.16) * 100) / 100);
    this.thresholdOff = options.thresholdOff ?? Math.min(this.thresholdOn * 0.55, this.thresholdOn - 0.06);
    this.noiseGateRms = options.noiseGateRms ?? 0.0020;
    this.maxActiveNotes = options.maxActiveNotes ?? 6;
    this.retriggerCooldownMs = options.retriggerCooldownMs ?? 85;
    this.enableThinHybridBoost = options.enableThinHybridBoost ?? true;
    this.autoAdapt = options.autoAdapt !== false;
    this.reset();
  }

  updateConfig(cfg = {}) {
    if (typeof cfg.minMidi === 'number') this.minMidi = Math.max(LOWEST_PP_MIDI, Math.round(cfg.minMidi));
    if (typeof cfg.maxMidi === 'number') this.maxMidi = Math.min(LOWEST_PP_MIDI + PP_KEY_COUNT - 1, Math.round(cfg.maxMidi));
    if (typeof cfg.a4Hz === 'number' && cfg.a4Hz > 300) this.a4Hz = cfg.a4Hz;
    if (typeof cfg.thresholdOn === 'number') {
      this.thresholdOn = Math.max(0.36, Math.min(0.88, cfg.thresholdOn));
      if (typeof cfg.highThresholdOn !== 'number') {
        // Zachowaj spójny margines -0.16 dla cienkich strun przy ręcznej zmianie głównego suwaka
        this.highThresholdOn = Math.max(0.32, Math.min(this.thresholdOn, Math.round((this.thresholdOn - 0.16) * 100) / 100));
      }
      this.thresholdOff = typeof cfg.thresholdOff === 'number'
        ? Math.max(0.15, Math.min(this.thresholdOn - 0.04, cfg.thresholdOff))
        : Math.max(0.18, Math.min(this.thresholdOn - 0.06, this.thresholdOn * 0.55));
    }
    if (typeof cfg.highThresholdOn === 'number') {
      this.highThresholdOn = Math.max(0.30, Math.min(0.84, cfg.highThresholdOn));
    }
    if (typeof cfg.noiseGateRms === 'number') {
      this.noiseGateRms = Math.max(0.0, Math.min(0.1, cfg.noiseGateRms));
    }
    if (typeof cfg.enableThinHybridBoost === 'boolean') {
      this.enableThinHybridBoost = cfg.enableThinHybridBoost;
    }
    if (typeof cfg.autoAdapt === 'boolean') {
      this.autoAdapt = cfg.autoAdapt;
    }
    if (!this.emaLowPeak) this.emaLowPeak = this.thresholdOn / 0.68;
    if (!this.emaHighPeak) this.emaHighPeak = this.highThresholdOn / 0.58;
  }

  reset() {
    this.activeMap = new Map(); // midi -> noteState
    this.prevProbs = new Float32Array(PP_KEY_COUNT);
    this.quietFrames = 0;
    this.rmsSampleFrames = 0;
    this.prevRms = 0;
    this.emaRms = 0;
    this.autoNoiseFloorRms = this.noiseGateRms * 0.45;
    this.emaLowPeak = this.thresholdOn / 0.68;
    this.emaHighPeak = this.highThresholdOn / 0.58;
    this.adaptLowFrames = 0;
    this.adaptHighFrames = 0;
  }

  midiToHz(midi) {
    return this.a4Hz * Math.pow(2, (midi - 69) / 12);
  }

  effectiveThresholds() {
    if (!this.autoAdapt) {
      return { lowOn: this.thresholdOn, highOn: this.highThresholdOn };
    }
    const lowCap = Math.max(0.68, this.thresholdOn);
    const lowOn = (this.adaptLowFrames >= 6)
      ? Math.max(
          Math.max(0.42, this.thresholdOn - 0.12),
          Math.min(Math.min(lowCap, this.thresholdOn + 0.06), this.emaLowPeak * 0.68)
        )
      : this.thresholdOn;
    const highOn = (this.adaptHighFrames >= 6)
      ? Math.max(
          Math.max(0.30, this.highThresholdOn - 0.10),
          Math.min(lowOn - 0.08, this.emaHighPeak * 0.58)
        )
      : Math.min(lowOn - 0.06, this.highThresholdOn);
    return { lowOn, highOn };
  }

  getConfig() {
    return {
      minMidi: this.minMidi,
      maxMidi: this.maxMidi,
      a4Hz: this.a4Hz,
      thresholdOn: this.thresholdOn,
      highThresholdOn: this.highThresholdOn,
      thresholdOff: this.thresholdOff,
      noiseGateRms: this.noiseGateRms,
      maxActiveNotes: this.maxActiveNotes,
      retriggerCooldownMs: this.retriggerCooldownMs,
      enableThinHybridBoost: this.enableThinHybridBoost,
      autoAdapt: this.autoAdapt
    };
  }

  /**
   * Płynna krzywa progu wg rejestru gitary:
   *  - grube struny (<= 52): próg lowOn (chroni przed buczeniem i IMD),
   *  - cienkie struny (>= 62): niższy próg highOn (samoadaptujący się lub z kalibracji).
   */
  thresholdOnForMidi(midi) {
    const { lowOn, highOn } = this.effectiveThresholds();
    const t = Math.max(0, Math.min(1, (midi - 52) / 10));
    return lowOn - t * (lowOn - highOn);
  }

  thresholdOffForMidi(midi) {
    const on = this.thresholdOnForMidi(midi);
    return Math.max(0.18, Math.min(on - 0.06, on * 0.56));
  }

  probToVelocity(prob, rms) {
    const pNorm = Math.max(0, Math.min(1, (prob - 0.30) / 0.70));
    const rNorm = (rms !== null && rms > 0)
      ? Math.max(0, Math.min(1, (20 * Math.log10(Math.max(1e-5, rms)) + 52) / 42))
      : pNorm;
    return Math.max(16, Math.min(127, Math.round(24 + (0.55 * pNorm + 0.45 * rNorm) * 103)));
  }

  /**
   * Przetwarza jedną ramkę z PitchPlease (co ~11.6 ms).
   * @param {Float32Array|null} probs - 88 prawdopodobieństw klawiszy (A0..C8)
   * @param {Array<{midi:number, prob:number}>} fallbackNotes - używane tylko gdy probs === null
   * @param {number} timestampMs - znacznik czasu w ms
   * @param {number|null} currentRms - krótkie RMS mikrofonu (lub null w trybie offline bez bramki)
   * @param {Float32Array|null} pcmBuf - opcjonalny bufor PCM do szybkiego potwierdzenia f0 cienkich strun
   * @param {number} sampleRate - próbkowanie bufora pcmBuf
   */
  step(probs, fallbackNotes, timestampMs, currentRms = null, pcmBuf = null, sampleRate = 48000) {
    const events = [];
    const loIdx = Math.max(0, this.minMidi - LOWEST_PP_MIDI);
    const hiIdx = Math.min(PP_KEY_COUNT - 1, this.maxMidi - LOWEST_PP_MIDI);

    let maxGuitarProb = 0;
    let maxLowProb = 0;
    let maxHighProb = 0;
    const strongNotes = [];
    let noteProbMap = null;
    if (probs) {
      for (let idx = loIdx; idx <= hiIdx; idx++) {
        const p = probs[idx];
        const midi = idx + LOWEST_PP_MIDI;
        if (p > maxGuitarProb) maxGuitarProb = p;
        if (midi < THIN_STRING_SPLIT_MIDI) {
          if (p > maxLowProb) maxLowProb = p;
        } else {
          if (p > maxHighProb) maxHighProb = p;
        }
        if (p >= this.thresholdOn) strongNotes.push({ midi, prob: p });
      }
    } else {
      noteProbMap = new Map();
      for (const n of (fallbackNotes || [])) {
        if (n.midi >= this.minMidi && n.midi <= this.maxMidi) {
          const p = n.prob ?? 0.8;
          noteProbMap.set(n.midi, p);
          if (p > maxGuitarProb) maxGuitarProb = p;
          if (n.midi < THIN_STRING_SPLIT_MIDI) {
            if (p > maxLowProb) maxLowProb = p;
          } else {
            if (p > maxHighProb) maxHighProb = p;
          }
        }
      }
    }

    // 1. Samoadaptacyjna bramka ciszy RMS (Minimum-Statistics Noise Floor Tracker)
    let isQuiet = false;
    let rmsJumpRatio = 1.0;
    let effNoiseGate = this.noiseGateRms;
    if (currentRms !== null && Number.isFinite(currentRms)) {
      this.rmsSampleFrames++;
      if (this.autoAdapt) {
        const preGate = Math.max(this.noiseGateRms * 0.65, this.autoNoiseFloorRms * 2.25);
        const preRef = Math.max(this.emaRms, this.prevRms, preGate * 0.5, 1e-5);
        const preJump = currentRms / preRef;
        const steadyLowFloor =
          this.activeMap.size === 0 &&
          this.rmsSampleFrames <= 8 &&
          currentRms < 0.022 &&
          preJump < 1.35 &&
          maxGuitarProb < 0.65;
        if (steadyLowFloor) {
          this.autoNoiseFloorRms = (this.rmsSampleFrames <= 1)
            ? currentRms
            : (0.50 * this.autoNoiseFloorRms + 0.50 * currentRms);
        } else if (maxGuitarProb < 0.22) {
          this.autoNoiseFloorRms = (currentRms < this.autoNoiseFloorRms)
            ? (0.80 * this.autoNoiseFloorRms + 0.20 * currentRms)
            : (0.988 * this.autoNoiseFloorRms + 0.012 * currentRms);
        }
        effNoiseGate = (this.noiseGateRms <= 0)
          ? 0
          : Math.max(0.0008, Math.min(0.025, Math.max(this.noiseGateRms * 0.65, this.autoNoiseFloorRms * 2.25)));
      }
      const refRms = Math.max(this.emaRms, this.prevRms, effNoiseGate * 0.5, 1e-5);
      rmsJumpRatio = currentRms / refRms;
      this.prevRms = currentRms;
      this.emaRms = this.emaRms === 0 ? currentRms : (0.82 * this.emaRms + 0.18 * currentRms);

      if (currentRms < effNoiseGate) {
        this.quietFrames++;
        if (this.quietFrames >= 2 || (this.autoAdapt && this.activeMap.size === 0 && this.rmsSampleFrames <= 3)) {
          isQuiet = true;
        }
      } else {
        this.quietFrames = 0;
      }
    }

    if (isQuiet) {
      if (probs) this.prevProbs.set(probs);
      for (const note of this.activeMap.values()) {
        events.push({
          type: 'noteOff',
          midiNote: note.midiNote,
          exactMidi: note.exactMidi,
          frequencyHz: note.frequencyHz,
          centsOffset: 0,
          velocity: 0,
          timestampMs,
          durationMs: Math.max(0, timestampMs - note.onsetTimeMs),
          isRetrigger: false
        });
      }
      this.activeMap.clear();
      return { events, activeNotes: [], maxGuitarProb: 0, maxLowProb: 0, maxHighProb: 0 };
    }

    // Samopoziomowanie obwiedni grubych i cienkich strun podczas ataku kostką
    // (nie obniżaj adaptacyjnego progu cienkich strun na podstawie słabego alikwotu głośnej grubej struny)
    if (this.autoAdapt && (rmsJumpRatio >= 1.30 || maxGuitarProb >= 0.48)) {
      if (maxLowProb >= 0.48) {
        this.emaLowPeak = 0.88 * this.emaLowPeak + 0.12 * maxLowProb;
        this.adaptLowFrames++;
      }
      if (maxHighProb >= 0.34 && (maxLowProb < 0.45 || maxHighProb >= maxLowProb * 0.85)) {
        this.emaHighPeak = 0.85 * this.emaHighPeak + 0.15 * maxHighProb;
        this.adaptHighFrames++;
      }
    }

    const isHighOvertoneGhost = (midi) => {
      if (midi < 70) return false;
      for (let i = 0; i < strongNotes.length; i++) {
        const s = strongNotes[i];
        if (s.midi >= midi || s.prob < 0.64) continue;
        const diff = midi - s.midi;
        // 5., 6. i 8. harmoniczna (+28, +31, +36 półt.) bardzo głośnej grubej struny
        if (diff === 28 || diff === 31 || diff === 36) return true;
      }
      return false;
    };

    const isOctaveOrTwelfthOfStrongLower = (midi) => {
      for (let i = 0; i < strongNotes.length; i++) {
        const s = strongNotes[i];
        if (s.midi >= midi || s.prob < 0.78) continue;
        const diff = midi - s.midi;
        if (diff === 12 || diff === 19) return true;
      }
      return false;
    };

    let cachedPcmRms = -1;
    const getPcmRms = () => {
      if (cachedPcmRms >= 0) return cachedPcmRms;
      if (!pcmBuf || pcmBuf.length < 256) { cachedPcmRms = 0; return 0; }
      const N = Math.min(2048, pcmBuf.length);
      const start = pcmBuf.length - N;
      let sum = 0;
      for (let i = start; i < pcmBuf.length; i++) {
        const v = pcmBuf[i];
        sum += v * v;
      }
      cachedPcmRms = Math.sqrt(sum / N);
      return cachedPcmRms;
    };

    const isThinConfirmedByGoertzel = (midi, p) => {
      if (!this.enableThinHybridBoost || !pcmBuf || midi < THIN_STRING_SPLIT_MIDI || p < 0.24) return false;
      if (isHighOvertoneGhost(midi)) return false;
      const frameRms = getPcmRms();
      if (frameRms < 1e-4) return false;
      const g = measureThinStringF0Sharpness(pcmBuf, sampleRate, midi, this.a4Hz);
      const relMag = g.mag / frameRms;
      const isOctOfLower = strongNotes.some(s => (s.midi === midi - 12 || s.midi === midi - 19) && s.prob >= 0.70);
      const needRatio = isOctOfLower ? 2.8 : 2.25;
      const needRelMag = isOctOfLower ? 0.16 : 0.10;
      return g.ratio >= needRatio && relMag >= needRelMag;
    };

    // 2. Aktualizuj istniejące nuty (NoteOff / Sustain / Re-trigger)
    for (const [midi, note] of Array.from(this.activeMap.entries())) {
      const idx = midi - LOWEST_PP_MIDI;
      const outOfRange = midi < this.minMidi || midi > this.maxMidi;
      const p = outOfRange
        ? 0
        : (probs ? probs[idx] : (noteProbMap.get(midi) ?? 0));
      const prevP = (idx >= 0 && idx < PP_KEY_COUNT) ? this.prevProbs[idx] : 0;
      const offThr = this.thresholdOffForMidi(midi);
      const onThr = this.thresholdOnForMidi(midi);

      const keptByThinF0 = !outOfRange && p < offThr && p >= 0.24 && isThinConfirmedByGoertzel(midi, p);
      if (outOfRange || (p < offThr && !keptByThinF0)) {
        events.push({
          type: 'noteOff',
          midiNote: note.midiNote,
          exactMidi: note.exactMidi,
          frequencyHz: note.frequencyHz,
          centsOffset: 0,
          velocity: 0,
          timestampMs,
          durationMs: Math.max(0, timestampMs - note.onsetTimeMs),
          isRetrigger: false
        });
        this.activeMap.delete(midi);
        continue;
      }

      note.prob = p;
      note.durationMs = Math.max(0, timestampMs - note.onsetTimeMs);
      if (p > note.peakProb) note.peakProb = p;
      if (p < note.minProbSincePeak) note.minProbSincePeak = p;

      const sinceTrigger = timestampMs - note.lastTriggerTimeMs;
      if (sinceTrigger >= this.retriggerCooldownMs) {
        const probDipRebound =
          note.minProbSincePeak <= (note.peakProb - 0.16) &&
          (p - note.minProbSincePeak) >= 0.17 &&
          p >= onThr;
        const rmsRetrigger =
          currentRms !== null &&
          rmsJumpRatio >= 1.85 &&
          currentRms >= this.noiseGateRms * 2.5 &&
          p >= onThr &&
          (this.activeMap.size === 1 || p >= prevP + 0.03);

        if (probDipRebound || rmsRetrigger) {
          events.push({
            type: 'noteOff',
            midiNote: note.midiNote,
            exactMidi: note.exactMidi,
            frequencyHz: note.frequencyHz,
            centsOffset: 0,
            velocity: 0,
            timestampMs,
            durationMs: note.durationMs,
            isRetrigger: true
          });
          note.onsetTimeMs = timestampMs;
          note.lastTriggerTimeMs = timestampMs;
          note.durationMs = 0;
          note.peakProb = p;
          note.minProbSincePeak = p;
          note.velocity = this.probToVelocity(p, currentRms);
          events.push({
            type: 'noteOn',
            midiNote: note.midiNote,
            exactMidi: note.exactMidi,
            frequencyHz: note.frequencyHz,
            centsOffset: 0,
            velocity: note.velocity,
            timestampMs,
            durationMs: 0,
            isRetrigger: true
          });
        }
      }
    }

    // 3. Wykryj nowe nuty (natychmiastowy NoteOn + wsparcie cienkich strun)
    const newCandidates = [];
    if (probs) {
      for (let idx = loIdx; idx <= hiIdx; idx++) {
        const midi = idx + LOWEST_PP_MIDI;
        if (this.activeMap.has(midi)) continue;
        const p = probs[idx];
        const isLocalMax = p >= (idx > 0 ? probs[idx - 1] : 0) && p >= (idx + 1 < PP_KEY_COUNT ? probs[idx + 1] : 0);

        let effThr = this.thresholdOnForMidi(midi);
        // Obniżony próg cienkich strun dotyczy niezależnych dźwięków — nie fałszywych oktaw (+12/+19) głośnego basu!
        if (effThr < this.thresholdOn && isOctaveOrTwelfthOfStrongLower(midi)) {
          effThr = this.thresholdOn;
        }
        if (!isLocalMax) effThr = Math.max(effThr, this.thresholdOn + 0.08);
        if (isHighOvertoneGhost(midi)) effThr = Math.max(effThr, 0.93);

        let accepted = p >= effThr;
        const deltaP = p - this.prevProbs[idx];
        if (!accepted && isLocalMax && midi >= THIN_STRING_SPLIT_MIDI &&
            ((deltaP >= 0.18 && p >= 0.26) || isThinConfirmedByGoertzel(midi, p))) {
          if (!isHighOvertoneGhost(midi) && isThinConfirmedByGoertzel(midi, p)) {
            accepted = true;
          }
        }
        if (accepted) {
          newCandidates.push({ midi, prob: p });
        }
      }
      this.prevProbs.set(probs);
    } else if (noteProbMap) {
      for (const [midi, p] of noteProbMap.entries()) {
        if (!this.activeMap.has(midi) && p >= this.thresholdOnForMidi(midi)) {
          newCandidates.push({ midi, prob: p });
        }
      }
    }

    newCandidates.sort((a, b) => b.prob - a.prob);
    for (const c of newCandidates) {
      if (this.activeMap.size >= this.maxActiveNotes) {
        let weakest = null;
        for (const n of this.activeMap.values()) {
          if (!weakest || n.prob < weakest.prob) weakest = n;
        }
        if (weakest && c.prob > weakest.prob + 0.12) {
          events.push({
            type: 'noteOff',
            midiNote: weakest.midiNote,
            exactMidi: weakest.exactMidi,
            frequencyHz: weakest.frequencyHz,
            centsOffset: 0,
            velocity: 0,
            timestampMs,
            durationMs: Math.max(0, timestampMs - weakest.onsetTimeMs),
            isRetrigger: false
          });
          this.activeMap.delete(weakest.midiNote);
        } else {
          continue;
        }
      }

      const freq = this.midiToHz(c.midi);
      const vel = this.probToVelocity(c.prob, currentRms);
      const noteObj = {
        midiNote: c.midi,
        exactMidi: c.midi,
        frequencyHz: freq,
        centsOffset: 0,
        velocity: vel,
        prob: c.prob,
        peakProb: c.prob,
        minProbSincePeak: c.prob,
        onsetTimeMs: timestampMs,
        lastTriggerTimeMs: timestampMs,
        durationMs: 0
      };
      this.activeMap.set(c.midi, noteObj);
      events.push({
        type: 'noteOn',
        midiNote: c.midi,
        exactMidi: c.midi,
        frequencyHz: freq,
        centsOffset: 0,
        velocity: vel,
        timestampMs,
        durationMs: 0,
        isRetrigger: false
      });
    }

    const activeNotes = Array.from(this.activeMap.values()).sort((a, b) => a.midiNote - b.midiNote);
    return { events, activeNotes, maxGuitarProb, maxLowProb, maxHighProb };
  }
}

/**
 * Tworzy gotowy detektor gitarowy na żywo:
 *  - Surowa gałąź (HPF 55 Hz -> tunerAnalyserNode): nietknięty fazowo sygnał do Stroika MPM (±0.2 ct).
 *  - Gałąź wzmacniaczowo-mikrofonowa (HPF 68 Hz -> LowShelf -4.5 dB -> HighShelf +6 dB -> lekki leveler):
 *    wyrównuje głośność cienkich strun G3/B2/E1 względem grubych E6/A5/D4 przed wejściem do PitchPlease WASM SIMD.
 */
export function createGuitarPitchDetector(options = {}) {
  const modelId = options.model || 'fast';
  const defT = defaultThresholdForModel(modelId);
  const tracker = new GuitarEventTracker({
    minMidi: options.minMidi ?? 38,
    maxMidi: options.maxMidi ?? 88,
    a4Hz: options.a4Hz ?? 440.0,
    thresholdOn: options.thresholdOn ?? defT.on,
    highThresholdOn: options.highThresholdOn ?? defT.highOn,
    thresholdOff: options.thresholdOff ?? defT.off,
    noiseGateRms: options.noiseGateRms ?? 0.0020,
    enableThinHybridBoost: options.ampPreEq !== false
  });

  const assetBaseUrl = options.assetBaseUrl ?? new URL('../../vendor/pitchplease/', import.meta.url).href;
  let ampPreEqEnabled = options.ampPreEq !== false;

  let rawSrcNode = null;
  let tunerHpFilter = null;
  let tunerAnalyserNode = null;
  let tunerPcmBuf = new Float32Array(2048);

  let condHpFilter = null;
  let condLowShelf = null;
  let condHighShelf = null;
  let condLeveler = null;
  let condAnalyserNode = null;
  let condPcmBuf = new Float32Array(2048);
  let condDestNode = null;

  let analyserSampleRate = 48000;
  let lastMaxGuitarProb = 0;
  let lastMaxLowProb = 0;
  let lastMaxHighProb = 0;
  let lastActiveNotes = [];
  let lastChord = null;

  const applyEqGains = () => {
    if (condLowShelf) condLowShelf.gain.value = ampPreEqEnabled ? -4.5 : 0.0;
    if (condHighShelf) condHighShelf.gain.value = ampPreEqEnabled ? 6.0 : 0.0;
    if (condLeveler) condLeveler.ratio.value = ampPreEqEnabled ? 3.0 : 1.0;
  };

  let cachedPcmStamp = -1;
  const refreshFramePcm = (stamp) => {
    const an = condAnalyserNode || tunerAnalyserNode;
    const buf = condAnalyserNode ? condPcmBuf : tunerPcmBuf;
    if (!an) return null;
    if (stamp === undefined || stamp === null || stamp !== cachedPcmStamp) {
      an.getFloatTimeDomainData(buf);
      if (stamp !== undefined && stamp !== null) cachedPcmStamp = stamp;
    }
    return buf;
  };

  const readShortRms = (stamp) => {
    const buf = refreshFramePcm(stamp);
    if (!buf) return null;
    const start = buf.length - 512;
    let sum = 0;
    for (let i = start; i < buf.length; i++) {
      const v = buf[i];
      sum += v * v;
    }
    return Math.sqrt(sum / 512);
  };

  const detector = createPitchPlease({
    model: modelId,
    assetBaseUrl,
    stabilityFrames: 1,
    clearAfterMs: 180,
    onNotes: (notes, frame) => {
      const stamp = frame && typeof frame.emitTime === 'number' ? frame.emitTime : performance.now();
      const buf = refreshFramePcm(stamp);
      const rms = readShortRms(stamp);
      const tsMs = frame && typeof frame.emitTime === 'number' ? frame.emitTime * 1000 : stamp;
      const hybridPcm = (condAnalyserNode && ampPreEqEnabled) ? buf : (tunerAnalyserNode ? tunerPcmBuf : null);
      const res = tracker.step(frame ? frame.probs : null, notes, tsMs, rms, hybridPcm, analyserSampleRate);
      lastMaxGuitarProb = res.maxGuitarProb;
      lastMaxLowProb = res.maxLowProb;
      lastMaxHighProb = res.maxHighProb;
      lastActiveNotes = res.activeNotes;
      if (typeof options.onFrame === 'function') {
        options.onFrame({
          events: res.events,
          activeNotes: res.activeNotes,
          chord: frame ? frame.chord : lastChord,
          rms: rms ?? 0,
          maxGuitarProb: res.maxGuitarProb,
          maxLowProb: res.maxLowProb,
          maxHighProb: res.maxHighProb,
          frameTimeSec: (frame && typeof frame.time === 'number') ? frame.time : null,
          frame
        });
      }
    },
    onChord: (chord) => {
      lastChord = chord;
    },
    onChordEnd: () => {
      lastChord = null;
    },
    onError: (err) => {
      if (typeof options.onError === 'function') options.onError(err);
      else console.error('[PitchPleaseBridge]', err);
    }
  });

  return {
    async start(stream, sharedAudioCtx) {
      const disconnectNodes = () => {
        const nodes = [rawSrcNode, tunerHpFilter, tunerAnalyserNode, condHpFilter, condLowShelf, condHighShelf, condLeveler, condAnalyserNode, condDestNode];
        for (const n of nodes) {
          try { if (n && typeof n.disconnect === 'function') n.disconnect(); } catch {}
        }
        rawSrcNode = tunerHpFilter = tunerAnalyserNode = null;
        condHpFilter = condLowShelf = condHighShelf = condLeveler = condAnalyserNode = condDestNode = null;
      };
      disconnectNodes();
      tracker.reset();
      lastActiveNotes = [];
      lastChord = null;
      cachedPcmStamp = -1;
      lastMaxGuitarProb = 0;
      lastMaxLowProb = 0;
      lastMaxHighProb = 0;

      let detectorInputStream = stream;
      if (sharedAudioCtx && stream) {
        try {
          analyserSampleRate = sharedAudioCtx.sampleRate || 48000;
          const fftSize = analyserSampleRate > 54000 ? 4096 : 2048;

          rawSrcNode = sharedAudioCtx.createMediaStreamSource(stream);

          // 1. Surowa gałąź pod Stroik (Tuner MPM)
          tunerHpFilter = sharedAudioCtx.createBiquadFilter();
          tunerHpFilter.type = 'highpass';
          tunerHpFilter.frequency.value = 55;
          tunerHpFilter.Q.value = 0.707;
          tunerAnalyserNode = sharedAudioCtx.createAnalyser();
          tunerAnalyserNode.fftSize = fftSize;
          tunerAnalyserNode.smoothingTimeConstant = 0;
          tunerPcmBuf = new Float32Array(fftSize);
          rawSrcNode.connect(tunerHpFilter);
          tunerHpFilter.connect(tunerAnalyserNode);

          // 2. Gałąź kondycjonowania pod wzmacniacz / mikrofon (wyrównanie cienkich strun G3/B2/E1)
          condHpFilter = sharedAudioCtx.createBiquadFilter();
          condHpFilter.type = 'highpass';
          condHpFilter.frequency.value = 68;
          condHpFilter.Q.value = 0.707;

          condLowShelf = sharedAudioCtx.createBiquadFilter();
          condLowShelf.type = 'lowshelf';
          condLowShelf.frequency.value = 270;

          condHighShelf = sharedAudioCtx.createBiquadFilter();
          condHighShelf.type = 'highshelf';
          condHighShelf.frequency.value = 580;

          condLeveler = sharedAudioCtx.createDynamicsCompressor();
          condLeveler.threshold.value = -24;
          condLeveler.knee.value = 18;
          condLeveler.attack.value = 0.008;
          condLeveler.release.value = 0.11;

          applyEqGains();

          condAnalyserNode = sharedAudioCtx.createAnalyser();
          condAnalyserNode.fftSize = fftSize;
          condAnalyserNode.smoothingTimeConstant = 0;
          condPcmBuf = new Float32Array(fftSize);

          rawSrcNode.connect(condHpFilter);
          condHpFilter.connect(condLowShelf);
          condLowShelf.connect(condHighShelf);
          condHighShelf.connect(condLeveler);
          condLeveler.connect(condAnalyserNode);

          if (typeof sharedAudioCtx.createMediaStreamDestination === 'function') {
            condDestNode = sharedAudioCtx.createMediaStreamDestination();
            condLeveler.connect(condDestNode);
            if (condDestNode.stream && condDestNode.stream.getAudioTracks().length > 0) {
              detectorInputStream = condDestNode.stream;
            }
          }
        } catch (e) {
          disconnectNodes();
          console.warn('[PitchPleaseBridge] Kondycjonowanie WebAudio niedostępne, używam surowego strumienia:', e);
        }
      }
      try {
        return await detector.start(detectorInputStream);
      } catch (err) {
        disconnectNodes();
        throw err;
      }
    },

    stop() {
      detector.stop();
      tracker.reset();
      lastActiveNotes = [];
      lastChord = null;
      cachedPcmStamp = -1;
      lastMaxGuitarProb = 0;
      lastMaxLowProb = 0;
      lastMaxHighProb = 0;
      const nodes = [rawSrcNode, tunerHpFilter, tunerAnalyserNode, condHpFilter, condLowShelf, condHighShelf, condLeveler, condAnalyserNode, condDestNode];
      for (const n of nodes) {
        try { if (n && typeof n.disconnect === 'function') n.disconnect(); } catch {}
      }
      rawSrcNode = tunerHpFilter = tunerAnalyserNode = null;
      condHpFilter = condLowShelf = condHighShelf = condLeveler = condAnalyserNode = condDestNode = null;
    },

    async setModel(newModelId) {
      return detector.setModel(newModelId);
    },

    setAmpPreEq(enabled) {
      ampPreEqEnabled = !!enabled;
      applyEqGains();
      tracker.updateConfig({ enableThinHybridBoost: ampPreEqEnabled });
    },

    updateConfig(cfg) {
      if (cfg && typeof cfg.ampPreEq === 'boolean') {
        ampPreEqEnabled = cfg.ampPreEq;
        applyEqGains();
        tracker.updateConfig({ enableThinHybridBoost: ampPreEqEnabled });
      }
      tracker.updateConfig(cfg);
    },

    getConfig() {
      return {
        ...tracker.getConfig(),
        ampPreEq: ampPreEqEnabled
      };
    },

    getLatestPcm() {
      if (!tunerAnalyserNode) return null;
      tunerAnalyserNode.getFloatTimeDomainData(tunerPcmBuf);
      return tunerPcmBuf;
    },

    getAnalyserSampleRate() {
      return analyserSampleRate;
    },

    getCurrentRms() {
      return readShortRms() ?? 0;
    },

    getLastMaxProb() {
      return lastMaxGuitarProb;
    },

    getLastRegisterProbs() {
      return { low: lastMaxLowProb, high: lastMaxHighProb, all: lastMaxGuitarProb };
    },

    get activeNotes() {
      return lastActiveNotes;
    },

    get stats() {
      return detector.stats;
    },

    get running() {
      return detector.running;
    }
  };
}

/**
 * 3-etapowa kalibracja na żywo z gitary i wzmacniacza użytkownika (~5.2 s):
 *  - Etap 1 (1.2 s): cisza -> pomiar szumu tła / wzmacniacza -> noiseGateRms
 *  - Etap 2 (1.8 s): grube struny (E6–D4) -> kalibracja progu grubych strun (thresholdOn)
 *  - Etap 3 (2.2 s): cienkie struny (G3–E1, np. dwudźwięk) -> kalibracja progu cienkich strun (highThresholdOn)
 */
export async function calibrateGuitarFromLiveInput({
  getRms,
  getRegisterProbs,
  getMaxProb,
  onProgress,
  silenceMs = 1200,
  lowPlayMs = 1800,
  highPlayMs = 2200
}) {
  const sleep = (ms) => new Promise(r => setTimeout(r, ms));
  const percentile = (arr, q) => {
    if (!arr.length) return 0;
    const s = arr.slice().sort((a, b) => a - b);
    return s[Math.min(s.length - 1, Math.floor(s.length * q))];
  };
  const readProbs = () => {
    if (typeof getRegisterProbs === 'function') return getRegisterProbs();
    const p = typeof getMaxProb === 'function' ? getMaxProb() : 0;
    return { low: p, high: p, all: p };
  };

  // Etap 1: Cisza (szum wzmacniacza / pokoju)
  const silenceRms = [];
  const t0 = performance.now();
  while (performance.now() - t0 < silenceMs) {
    silenceRms.push(Math.max(0, getRms()));
    if (onProgress) onProgress({ stage: 'silence', progress: Math.min(1, (performance.now() - t0) / silenceMs) });
    await sleep(35);
  }
  const noiseP90 = percentile(silenceRms, 0.90);
  const calibratedNoiseGate = Math.max(0.0012, Math.min(0.030, Math.max(noiseP90 * 2.5, 0.0014)));

  // Etap 2: Grube struny (E6–D4)
  const lowRms = [];
  const lowProbs = [];
  const t1 = performance.now();
  while (performance.now() - t1 < lowPlayMs) {
    const rms = Math.max(0, getRms());
    const pr = readProbs();
    if (rms >= calibratedNoiseGate * 1.45) {
      lowRms.push(rms);
      const pUse = Math.max(pr.low, pr.all * 0.85);
      if (pUse >= 0.22) lowProbs.push(pUse);
    }
    if (onProgress) onProgress({ stage: 'low', progress: Math.min(1, (performance.now() - t1) / lowPlayMs), rms });
    await sleep(35);
  }

  // Etap 3: Cienkie struny (G3–B2–E1, np. dwudźwięk)
  const highRms = [];
  const highProbs = [];
  const t2 = performance.now();
  while (performance.now() - t2 < highPlayMs) {
    const rms = Math.max(0, getRms());
    const pr = readProbs();
    if (rms >= calibratedNoiseGate * 1.35) {
      highRms.push(rms);
      const pUse = Math.max(pr.high, pr.all * 0.80);
      if (pUse >= 0.18) highProbs.push(pUse);
    }
    if (onProgress) onProgress({ stage: 'high', progress: Math.min(1, (performance.now() - t2) / highPlayMs), rms });
    await sleep(35);
  }

  const allRms = lowRms.concat(highRms);
  const guitarPeakRms = percentile(allRms, 0.90);
  const snrDb = (guitarPeakRms > 0 && noiseP90 > 1e-6)
    ? (20 * Math.log10(guitarPeakRms / noiseP90))
    : (guitarPeakRms > 0 ? 35 : 0);

  let thresholdOn = 0.58;
  let highThresholdOn = 0.42;
  let ok = false;

  if (lowProbs.length >= 5) {
    const p75Low = percentile(lowProbs, 0.75);
    thresholdOn = Math.round(Math.max(0.46, Math.min(0.68, p75Low * 0.68)) * 100) / 100;
    ok = true;
  }
  if (highProbs.length >= 5) {
    const p70High = percentile(highProbs, 0.70);
    // Dla cienkich strun celujemy w 60% typowego piku — żeby dwudźwięki i 3. cieńsza struna zawsze wskakiwały!
    highThresholdOn = Math.round(Math.max(0.32, Math.min(thresholdOn - 0.06, p70High * 0.60)) * 100) / 100;
    ok = true;
  } else if (ok) {
    highThresholdOn = Math.round(Math.max(0.34, thresholdOn - 0.16) * 100) / 100;
  }

  const minCalibratedFloor = noiseP90 > 0 ? Math.max(calibratedNoiseGate, noiseP90 * 1.15) : calibratedNoiseGate;
  const finalNoiseGate = ok
    ? Math.max(noiseP90 * 1.15, Math.min(minCalibratedFloor, Math.max(0.0012, Math.min(percentile(highRms, 0.75) || guitarPeakRms, guitarPeakRms) * 0.20)))
    : minCalibratedFloor;

  return {
    ok,
    noiseGateRms: Math.round(finalNoiseGate * 100000) / 100000,
    thresholdOn,
    highThresholdOn,
    thresholdOff: Math.round(thresholdOn * 0.55 * 100) / 100,
    noiseP90,
    guitarPeakRms,
    snrDb: Math.round(snrDb * 10) / 10,
    lowFrames: lowProbs.length,
    highFrames: highProbs.length
  };
}
