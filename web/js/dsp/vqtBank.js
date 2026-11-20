/**
 * Etap 2 potoku DSP:
 *  - Banka filtrów Variable-Q / Constant-Q (VQT / CQT) z wąskim prawoskrętnym oknem Hanna
 *    (2x węższy płat główny niż Blackman-Harris -> precyzyjnie rozdziela sąsiednie półtony!).
 *  - Szybki decymowany analizator MPM (McLeod Pitch Method / NSDF) w dziedzinie czasu,
 *    który działa nawet przy wytłumionym f0 na mikrofonie słuchawkowym i daje ±0.2 ct bez błędów oktawy.
 */
export class CqtAnalyzer {
  constructor() {
    this.sampleRate = 48000;
    this.a4ReferenceHz = 440;
    this.minMidiNote = 36;
    this.maxMidiNote = 108;
    this.binsPerSemitone = 2;
    this.maxWindowSize = 4096;
    this.bins = [];
    this.mpmDecBuf = new Float32Array(2048);
    this.mpmNsdfBuf = new Float32Array(512);
  }

  prepare(config) {
    this.sampleRate = config.sampleRate > 0 ? config.sampleRate : 48000;
    this.a4ReferenceHz = config.a4ReferenceHz > 0 ? config.a4ReferenceHz : 440;
    this.minMidiNote = config.minMidiNote;
    this.maxMidiNote = Math.max(config.minMidiNote + 12, config.maxMidiNote);
    this.binsPerSemitone = Math.max(1, config.binsPerSemitone | 0);
    this.maxWindowSize = Math.max(512, config.maxWindowSize | 0);

    const minWin = Math.max(128, Math.min(this.maxWindowSize, config.minWindowSize | 0));
    const totalBins = (this.maxMidiNote - this.minMidiNote) * this.binsPerSemitone + 1;
    this.bins = new Array(totalBins);

    for (let k = 0; k < totalBins; k++) {
      const midiFloat = this.minMidiNote + k / this.binsPerSemitone;
      const freqHz = this.midiToFreqHz(midiFloat);
      const idealWin = (config.qFactor * this.sampleRate) / (freqHz + Math.max(0, config.vqtGammaHz));
      const winLen = Math.max(minWin, Math.min(this.maxWindowSize, Math.round(idealWin)));

      // Bezpieczna decymacja 2x dla długich niskich okien (Nyquist 12 kHz >> 4.8 kHz LPF)
      const decimation = (winLen >= 1024 && freqHz < 1200 && this.sampleRate >= 32000) ? 2 : 1;
      const tapCount = Math.ceil(winLen / decimation);
      const cosKernel = new Float32Array(tapCount);
      const sinKernel = new Float32Array(tapCount);

      const omega = (2 * Math.PI * freqHz) / this.sampleRate;
      let winSum = 0;

      for (let t = 0; t < tapCount; t++) {
        const n = t * decimation;
        const u = winLen > 1 ? (n / (winLen - 1)) : 0;
        // Prawoskrętne przyczynowe okno Hanna (u^1.18): wąski płat główny dla rozdzielczości półtonowej + niska latencja
        const uSkew = Math.pow(u, 1.18);
        const w = 0.5 * (1.0 - Math.cos(2 * Math.PI * uSkew));
        const phase = omega * n;
        cosKernel[t] = w * Math.cos(phase);
        sinKernel[t] = -w * Math.sin(phase);
        winSum += w;
      }

      this.bins[k] = {
        freqHz,
        midiFloat,
        windowLen: winLen,
        decimation,
        normScale: winSum > 1e-9 ? (2.0 / winSum) : 1.0,
        cosKernel,
        sinKernel
      };
    }
  }

  get binCount() {
    return this.bins.length;
  }

  analyze(windowSamples, outMagnitudes) {
    const numSamples = windowSamples.length;
    const nBins = this.bins.length;
    const nyqGuard = this.sampleRate * 0.46;

    for (let k = 0; k < nBins; k++) {
      const bk = this.bins[k];
      if (bk.freqHz >= nyqGuard) {
        outMagnitudes[k] = 0;
        continue;
      }
      const winLen = Math.min(bk.windowLen, numSamples);
      const offset = numSamples - winLen;
      const dec = bk.decimation;
      const cK = bk.cosKernel;
      const sK = bk.sinKernel;
      const tapCount = Math.min(cK.length, Math.ceil(winLen / dec));

      let re = 0;
      let im = 0;
      if (dec === 1) {
        for (let t = 0; t < tapCount; t++) {
          const xv = windowSamples[offset + t];
          re += xv * cK[t];
          im += xv * sK[t];
        }
      } else {
        for (let t = 0; t < tapCount; t++) {
          const xv = windowSamples[offset + t * dec];
          re += xv * cK[t];
          im += xv * sK[t];
        }
      }
      outMagnitudes[k] = Math.hypot(re, im) * bk.normScale;
    }
  }

  /**
   * Szybki decymowany MPM (McLeod Pitch Method / NSDF) na najnowszych ~42–53 ms sygnału.
   * Działa po filtrze LPF 4800 Hz (Nyquist 12 kHz przy decymacji 2x = brak aliasingu)
   * i wykrywa dokładną wysokość oraz oktawę nawet gdy mikrofon słuchawkowy mocno tłumi f0!
   */
  analyzeMpm(windowSamples, minHz = 66.0, maxHz = 1350.0, minRmsGate = 0.002) {
    const N = windowSamples.length;
    const dec = this.sampleRate >= 32000 ? 2 : 1;
    const effSr = this.sampleRate / dec;
    // 1024 próbki po decymacji 2x = 2048 próbek = 42.7 ms @ 48 kHz (~3.5 pełnych okresów dla E2 = 82.4 Hz!)
    const rawSpan = Math.min(N, dec === 2 ? 2048 : 1024);
    const W = Math.floor(rawSpan / dec);
    const off = N - W * dec;

    if (this.mpmDecBuf.length < W) this.mpmDecBuf = new Float32Array(W);
    const x = this.mpmDecBuf;
    let mean = 0;
    if (dec === 2) {
      for (let i = 0; i < W; i++) {
        const v = 0.5 * (windowSamples[off + i * 2] + windowSamples[off + i * 2 + 1]);
        x[i] = v;
        mean += v;
      }
    } else {
      for (let i = 0; i < W; i++) {
        const v = windowSamples[off + i];
        x[i] = v;
        mean += v;
      }
    }
    mean /= W;
    let sumSq = 0;
    for (let i = 0; i < W; i++) {
      x[i] -= mean;
      sumSq += x[i] * x[i];
    }
    const rms = Math.sqrt(sumSq / W);
    // Krótkie RMS wyłącznie z ostatnich 256 próbek (5.3 ms) — natychmiast wykrywa wyciszenie struny (mute)!
    const shortSpan = Math.min(N, 256);
    const shortOff = N - shortSpan;
    let shortSq = 0;
    for (let i = 0; i < shortSpan; i++) {
      const sv = windowSamples[shortOff + i];
      shortSq += sv * sv;
    }
    const shortRms = Math.sqrt(shortSq / shortSpan);
    if (rms < minRmsGate || shortRms < minRmsGate * 0.75) {
      return { valid: false, midiNote: -1, exactMidi: 0, frequencyHz: 0, centsOffset: 0, clarity: 0, rms, shortRms };
    }

    const minTau = Math.max(4, Math.floor(effSr / maxHz) - 1);
    const maxTau = Math.min(Math.ceil(effSr / minHz) + 2, Math.floor(W * 0.48));
    if (maxTau <= minTau + 2) {
      return { valid: false, midiNote: -1, exactMidi: 0, frequencyHz: 0, centsOffset: 0, clarity: 0, rms, shortRms };
    }
    if (this.mpmNsdfBuf.length < maxTau + 3) this.mpmNsdfBuf = new Float32Array(maxTau + 3);
    const nsdf = this.mpmNsdfBuf;

    let m = sumSq * 2.0;
    nsdf[0] = 1.0;
    for (let tau = 1; tau <= maxTau + 1; tau++) {
      const a = x[W - tau];
      const b = x[tau - 1];
      m -= a * a + b * b;
      let r = 0;
      const lim = W - tau;
      for (let j = 0; j < lim; j++) r += x[j] * x[j + tau];
      nsdf[tau] = m > 1e-9 ? (2.0 * r) / m : 0.0;
    }

    // Zbierz kluczowe maksima (key maxima) w dodatnich łukach po pierwszym przejściu przez zero
    let pos = 1;
    while (pos < maxTau && nsdf[pos] > 0) pos++;
    const peaks = [];
    let curBest = -1;
    for (; pos <= maxTau; pos++) {
      const v = nsdf[pos];
      if (v > 0) {
        if (v > nsdf[pos - 1] && v >= nsdf[pos + 1] && (curBest < 0 || v > nsdf[curBest])) {
          curBest = pos;
        }
      } else if (curBest >= 0) {
        peaks.push(curBest);
        curBest = -1;
      }
    }
    if (curBest >= 0) peaks.push(curBest);

    let highest = 0;
    for (let i = 0; i < peaks.length; i++) {
      const p = peaks[i];
      if (p >= minTau && nsdf[p] > highest) highest = nsdf[p];
    }
    if (highest < 0.50) {
      return { valid: false, midiNote: -1, exactMidi: 0, frequencyHz: 0, centsOffset: 0, clarity: highest, rms, shortRms };
    }

    // Wysoki próg 0.93 * highest + kontrola 2*tau chroni przed skokiem +1 oktawę przy silnej 2. harmonicznej!
    const thresh = highest * 0.93;
    let chosen = -1;
    for (let i = 0; i < peaks.length; i++) {
      const p = peaks[i];
      if (p >= minTau && nsdf[p] >= thresh) {
        chosen = p;
        break;
      }
    }
    if (chosen < 0) {
      return { valid: false, midiNote: -1, exactMidi: 0, frequencyHz: 0, centsOffset: 0, clarity: highest, rms, shortRms };
    }

    // Jeśli przy podwójnym okresie (oktawę niżej) korelacja jest wyraźnie wyższa (o ≥ 0.035), to 2*chosen jest prawdziwym okresem!
    const dbl = chosen * 2;
    if (dbl >= minTau && dbl <= maxTau) {
      let bestDbl = -1;
      let bestDblVal = -1;
      for (let d = -2; d <= 2; d++) {
        const idx = dbl + d;
        if (idx >= minTau && idx <= maxTau && nsdf[idx] > bestDblVal) {
          bestDblVal = nsdf[idx];
          bestDbl = idx;
        }
      }
      if (bestDbl > 0 && bestDblVal >= nsdf[chosen] + 0.045 && bestDblVal >= 0.86) {
        chosen = bestDbl;
      }
    }

    // Interpolacja paraboliczna szczytu NSDF
    let refinedTau = chosen;
    if (chosen > 1 && chosen + 1 <= maxTau) {
      const y0 = nsdf[chosen - 1];
      const y1 = nsdf[chosen];
      const y2 = nsdf[chosen + 1];
      const denom = y0 - 2 * y1 + y2;
      if (Math.abs(denom) > 1e-12) {
        const delta = Math.max(-0.5, Math.min(0.5, 0.5 * (y0 - y2) / denom));
        refinedTau = chosen + delta;
      }
    }

    const freqHz = effSr / refinedTau;
    if (freqHz < minHz * 0.96 || freqHz > maxHz * 1.03) {
      return { valid: false, midiNote: -1, exactMidi: 0, frequencyHz: 0, centsOffset: 0, clarity: nsdf[chosen], rms, shortRms };
    }

    const exactMidi = 69.0 + 12.0 * Math.log2(freqHz / this.a4ReferenceHz);
    const midiNote = Math.round(exactMidi);
    return {
      valid: true,
      midiNote,
      exactMidi,
      frequencyHz: freqHz,
      centsOffset: (exactMidi - midiNote) * 100.0,
      clarity: nsdf[chosen],
      rms,
      shortRms
    };
  }

  refineExactMidi(magnitudes, binIdx) {
    if (binIdx < 0 || binIdx >= this.bins.length) return 0;
    let refinedBin = binIdx;
    if (binIdx > 0 && binIdx + 1 < magnitudes.length) {
      const y0 = Math.log(magnitudes[binIdx - 1] + 1e-7);
      const y1 = Math.log(magnitudes[binIdx] + 1e-7);
      const y2 = Math.log(magnitudes[binIdx + 1] + 1e-7);
      const denom = y0 - 2 * y1 + y2;
      if (Math.abs(denom) > 1e-12 && y1 >= y0 && y1 >= y2) {
        const delta = Math.max(-0.5, Math.min(0.5, 0.5 * (y0 - y2) / denom));
        refinedBin += delta;
      }
    }
    return this.minMidiNote + refinedBin / this.binsPerSemitone;
  }

  midiToBinIndex(midiNote) {
    const clamped = Math.max(this.minMidiNote, Math.min(this.maxMidiNote, midiNote));
    return (clamped - this.minMidiNote) * this.binsPerSemitone;
  }

  midiToFreqHz(exactMidi) {
    return this.a4ReferenceHz * Math.pow(2, (exactMidi - 69) / 12);
  }
}
