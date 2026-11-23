/**
 * Polifoniczny detektor nut: harmoniczna salience + iteracyjna estymacja i kasacja.
 * Port z composeApp/.../dsp/NoteDetector.kt.
 */
import { Notes } from './music.js';

export class DetectedNote {
  constructor(midi, salience, freq) {
    this.midi = midi;
    this.salience = salience;
    this.freq = freq;
  }
}

export class DetectionParams {
  constructor({
    minPeakProminenceDb = 10.0,
    relativeThreshold = 0.22,
    minSalience = 20.0,
    minFundamentalDb = 6.0,
    maxNotes = 8,
  } = {}) {
    this.minPeakProminenceDb = minPeakProminenceDb;
    this.relativeThreshold = relativeThreshold;
    this.minSalience = minSalience;
    this.minFundamentalDb = minFundamentalDb;
    this.maxNotes = maxNotes;
  }

  static forSensitivity(sensitivity) {
    const s = Math.min(1.0, Math.max(0.0, sensitivity));
    const lerp = (a, b) => a + (b - a) * s;
    return new DetectionParams({
      minPeakProminenceDb: lerp(14.0, 6.0),
      relativeThreshold: lerp(0.35, 0.10),
      minSalience: lerp(30.0, 10.0),
    });
  }
}

export class NoteDetector {
  constructor(minMidi = 28, maxMidi = 100, maxHarmonics = 16, salienceHarmonics = 8) {
    this.minMidi = minMidi;
    this.maxMidi = maxMidi;
    this.maxHarmonics = maxHarmonics;
    this.salienceHarmonics = salienceHarmonics;
  }

  detect(peaks, a4, params) {
    if (!peaks || peaks.length === 0) return [];
    const sorted = peaks.slice().sort((a, b) => a.freq - b.freq);
    const freq = new Float64Array(sorted.length);
    const residual = new Float64Array(sorted.length);
    for (let i = 0; i < sorted.length; i++) {
      freq[i] = sorted[i].freq;
      residual[i] = sorted[i].prominence;
    }
    const overtone = new Int32Array(sorted.length);

    const isCandidate = new Uint8Array(this.maxMidi + 1);
    for (let i = 0; i < freq.length; i++) {
      const m = Notes.midiOf(freq[i], a4);
      const r = Math.round(m);
      if (Math.abs(m - r) <= 0.35 && r >= this.minMidi && r <= this.maxMidi) isCandidate[r] = 1;
    }

    const result = [];
    let first = 0.0;
    const matched = new Int32Array(this.maxHarmonics);
    const bestMatched = new Int32Array(this.maxHarmonics);

    while (result.length < params.maxNotes) {
      let bestMidi = -1;
      let bestSal = 0.0;
      let bestCount = 0;
      for (let m = this.minMidi; m <= this.maxMidi; m++) {
        if (!isCandidate[m]) continue;
        const count = this.match(m, a4, freq, residual, matched);
        if (count === 0) continue;
        const needed = params.minFundamentalDb * (overtone[matched[0]] > 3 ? 2 : 1);
        if (residual[matched[0]] < needed) continue;
        let sal = 0.0;
        const upTo = Math.min(count, this.salienceHarmonics);
        for (let h = 0; h < upTo; h++) {
          if (matched[h] >= 0) sal += residual[matched[h]] / Math.sqrt(h + 1.0);
        }
        if (sal > bestSal) {
          bestSal = sal;
          bestMidi = m;
          bestCount = count;
          bestMatched.set(matched.subarray(0, count));
        }
      }
      if (bestMidi < 0 || bestSal < params.minSalience) break;
      if (result.length > 0 && bestSal < params.relativeThreshold * first) break;
      if (result.length === 0) first = bestSal;
      result.push(new DetectedNote(bestMidi, bestSal, freq[bestMatched[0]]));
      isCandidate[bestMidi] = 0;
      this.cancel(bestMatched, bestCount, residual, overtone);
    }
    return result;
  }

  match(midi, a4, freq, residual, out) {
    const f0 = Notes.frequency(midi, a4);
    const top = freq[freq.length - 1];
    let count = 0;
    for (let h = 1; h <= this.maxHarmonics; h++) {
      const target = h * f0 * Math.sqrt(1 + 1e-4 * h * h);
      if (target > top * 1.03) break;
      const tolerance = Math.pow(2, (35.0 + 4.0 * (h - 1)) / 1200.0);
      out[h - 1] = this.strongestIn(freq, residual, target / tolerance, target * tolerance);
      count = h;
      if (h === 1 && out[0] < 0) return 0;
    }
    return count;
  }

  strongestIn(freq, residual, lo, hi) {
    let i = this.lowerBound(freq, lo);
    let best = -1;
    while (i < freq.length && freq[i] <= hi) {
      if (best < 0 || residual[i] > residual[best]) best = i;
      i++;
    }
    return best;
  }

  lowerBound(a, x) {
    let lo = 0;
    let hi = a.length;
    while (lo < hi) {
      const mid = (lo + hi) >>> 1;
      if (a[mid] < x) lo = mid + 1;
      else hi = mid;
    }
    return lo;
  }

  cancel(matched, count, residual, overtone) {
    const level = new Float64Array(count);
    for (let h = 0; h < count; h++) {
      level[h] = matched[h] >= 0 ? residual[matched[h]] : -1.0;
    }
    for (let h = 0; h < count; h++) {
      const j = matched[h];
      if (j < 0) continue;
      let sum = 0.0;
      let n = 0;
      for (const k of [h - 1, h + 1]) {
        if (k >= 0 && k < count && level[k] >= 0) { sum += level[k]; n++; }
      }
      const envelope = n === 0 ? level[h] : sum / n;
      residual[j] = Math.max(0.0, level[h] - Math.max(envelope, 0.7 * level[h]));
      if (h === 0) continue;
      let odd = h + 1;
      while (odd % 2 === 0) odd /= 2;
      overtone[j] = Math.max(overtone[j], odd);
    }
  }
}
