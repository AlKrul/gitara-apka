/**
 * Widmo: okno Hanna + zero-padding + odporny lokalny szum tła + interpolacja paraboliczna pików.
 * Port z composeApp/.../dsp/Spectrum.kt.
 */
import { Fft } from './fft.js';

export class Peak {
  constructor(freq, db, prominence) {
    this.freq = freq;
    this.db = db;
    this.prominence = prominence;
  }
}

export class Spectrum {
  constructor(sampleRate, windowSize, fftSize = windowSize * 2) {
    this.sampleRate = sampleRate;
    this.windowSize = windowSize;
    this.fftSize = fftSize;
    this.fft = new Fft(fftSize);
    this.window = new Float64Array(windowSize);
    for (let i = 0; i < windowSize; i++) {
      this.window[i] = 0.5 - 0.5 * Math.cos((2 * Math.PI * i) / (windowSize - 1));
    }
    this.re = new Float64Array(fftSize);
    this.im = new Float64Array(fftSize);
    this.bins = (fftSize >> 1) + 1;
    this.binHz = sampleRate / fftSize;

    this.db = new Float64Array(this.bins);
    this.floor = new Float64Array(this.bins);
    this.prefix = new Float64Array(this.bins + 1);
    this.clipped = new Float64Array(this.bins);
  }

  /** Poziom każdego binu w dBFS (sygnał pełnej skali ≈ 0 dB). */
  analyze(frame, offset = 0) {
    const { windowSize, fftSize, window, re, im, bins } = this;
    for (let i = 0; i < windowSize; i++) {
      re[i] = frame[offset + i] * window[i];
      im[i] = 0.0;
    }
    for (let i = windowSize; i < fftSize; i++) {
      re[i] = 0.0;
      im[i] = 0.0;
    }
    this.fft.transform(re, im);
    const scale = 4.0 / windowSize;
    for (let k = 0; k < bins; k++) {
      const m = Math.sqrt(re[k] * re[k] + im[k] * im[k]) * scale;
      this.db[k] = 20 * Math.log10(m + 1e-9);
    }
  }

  computeFloor(lowBin, highBin) {
    const { bins, db, floor, prefix, clipped } = this;
    const halfWidth = (k) => Math.max(24, Math.floor(k * 0.2));
    const windowMean = (src, dst) => {
      prefix[0] = 0.0;
      for (let k = 0; k < bins; k++) prefix[k + 1] = prefix[k] + src[k];
      for (let k = lowBin; k <= highBin; k++) {
        const h = halfWidth(k);
        const a = Math.max(0, k - h);
        const b = Math.min(bins - 1, k + h);
        dst[k] = (prefix[b + 1] - prefix[a]) / (b - a + 1);
      }
    };
    windowMean(db, floor);
    for (let k = 0; k < bins; k++) {
      clipped[k] = (k >= lowBin && k <= highBin) ? Math.min(db[k], floor[k]) : db[k];
    }
    windowMean(clipped, floor);
  }

  peaks(minHz, maxHz, minProminenceDb, dynamicRangeDb, minDb) {
    const { bins, binHz, db, floor } = this;
    const lo = Math.max(3, Math.floor(minHz / binHz));
    const hi = Math.min(bins - 4, Math.floor(maxHz / binHz) + 1);
    this.computeFloor(lo, hi);
    const candidates = [];
    let k = lo;
    while (k <= hi) {
      const v = db[k];
      if (v > db[k - 1] && v >= db[k + 1] && v >= db[k - 2] && v >= db[k + 2] && v >= db[k - 3] && v >= db[k + 3]) {
        const a = db[k - 1];
        const c = db[k + 1];
        const denom = a - 2 * v + c;
        let p = 0.0;
        if (denom < 0) {
          p = Math.min(0.5, Math.max(-0.5, (0.5 * (a - c)) / denom));
        }
        const peakDb = v - 0.25 * (a - c) * p;
        const prom = peakDb - floor[k];
        if (prom >= minProminenceDb && peakDb >= minDb) {
          candidates.push(new Peak((k + p) * binHz, peakDb, prom));
        }
        k += 2;
      } else {
        k++;
      }
    }
    if (candidates.length === 0) return candidates;
    const top = candidates.reduce((m, c) => Math.max(m, c.db), -Infinity);
    return candidates.filter((c) => c.db >= top - dynamicRangeDb);
  }

  static rmsDb(samples, from = 0, to = samples.length) {
    if (to <= from) return -120.0;
    let sum = 0.0;
    for (let i = from; i < to; i++) {
      sum += samples[i] * samples[i];
    }
    return 10 * Math.log10(sum / (to - from) + 1e-12);
  }
}
