/**
 * Potok analizy na żywo: piki widma → nuty (wygładzane) → nazwa akordu → akord na wyświetlaczu,
 * który zmienia się dopiero gdy nowy brzmi przez ustalony czas (change-delay). Port ChordEngine.kt.
 */
import { Spectrum } from './spectrum.js';
import { NoteDetector, DetectionParams } from './noteDetector.js';
import { ChordNamer } from './chordNamer.js';
import { Notes } from './music.js';

export const WINDOW = 8192;
export const HOP = 2048;
const MIN_HZ = 38.0;
const MAX_HZ = 6000.0;
const DYNAMIC_RANGE_DB = 50.0;
const SILENCE_SECONDS = 0.4;
const SUSPECT_WEIGHT = 0.7;
const OVERTONE_OFFSETS = new Set([19, 28, 31, 34, 38, 40]);

export class ActiveNote {
  constructor(midi, strength) {
    this.midi = midi;
    this.strength = strength;
  }
}

export class ChordEngine {
  constructor() {
    this.sampleRate = 44100;
    this.spectrum = new Spectrum(this.sampleRate, WINDOW);
    this.detector = new NoteDetector();
    this.frame = new Float32Array(WINDOW);
    this.filled = 0;
    this.sinceHop = 0;

    this.evidence = new Float64Array(128);
    this.salience = new Float64Array(128);
    this.active = new Uint8Array(128);
    this.lastActiveFrame = new Int32Array(128).fill(-1000);
    this.frameIndex = 0;

    this.recentLevels = new Float64Array(4).fill(-120.0);
    this.framesSinceOnset = 1000;
    this.settled = null;
    this.pending = null;
    this.pendingFrames = 0;
    this.silentFrames = 0;
  }

  begin(rate) {
    this.sampleRate = rate;
    if (this.spectrum.sampleRate !== rate) this.spectrum = new Spectrum(rate, WINDOW);
    this.filled = 0;
    this.sinceHop = 0;
    this.frame.fill(0);
    this.evidence.fill(0.0);
    this.salience.fill(0.0);
    this.active.fill(0);
    this.lastActiveFrame.fill(-1000);
    this.recentLevels.fill(-120.0);
    this.framesSinceOnset = 1000;
    this.settled = null;
    this.pending = null;
    this.pendingFrames = 0;
    this.silentFrames = 0;
  }

  /** Podaje [count] próbek (-1..1); onFrame(state) wołany po każdej przeanalizowanej ramce. */
  push(samples, count, config, onFrame) {
    let i = 0;
    while (i < count) {
      const n = Math.min(count - i, HOP - this.sinceHop);
      this.frame.copyWithin(0, n, WINDOW);
      this.frame.set(samples.subarray(i, i + n), WINDOW - n);
      this.filled = Math.min(WINDOW, this.filled + n);
      this.sinceHop += n;
      i += n;
      if (this.sinceHop >= HOP) {
        this.sinceHop = 0;
        onFrame(this.analyze(config));
      }
    }
  }

  analyze(config) {
    const levelDb = Spectrum.rmsDb(this.frame, WINDOW - HOP, WINDOW);
    const frameDb = Spectrum.rmsDb(this.frame, WINDOW - WINDOW / 2, WINDOW);
    const params = DetectionParams.forSensitivity(config.sensitivity);

    let detected;
    if (this.filled >= WINDOW / 2 && frameDb >= config.noiseGateDb) {
      this.spectrum.analyze(this.frame);
      const minDb = config.noiseGateDb - 25.0;
      const peaks = this.spectrum.peaks(MIN_HZ, MAX_HZ, params.minPeakProminenceDb, DYNAMIC_RANGE_DB, minDb);
      detected = this.detector.detect(peaks, config.a4, params);
    } else {
      detected = [];
    }

    let onset = levelDb - Math.min(...this.recentLevels) >= 4.0 && levelDb >= config.noiseGateDb;
    this.recentLevels.copyWithin(0, 1, this.recentLevels.length);
    this.recentLevels[this.recentLevels.length - 1] = levelDb;
    this.frameIndex++;

    const hit = new Uint8Array(128);
    for (const d of detected) {
      hit[d.midi] = 1;
      this.salience[d.midi] = this.active[d.midi]
        ? 0.6 * this.salience[d.midi] + 0.4 * d.salience
        : d.salience;
    }
    for (let m = 0; m < 128; m++) {
      this.evidence[m] = this.evidence[m] * 0.5 + (hit[m] ? 1.0 : 0.0);
      if (!this.active[m] && this.evidence[m] >= 1.4) {
        this.active[m] = 1;
        if (this.frameIndex - this.lastActiveFrame[m] > 10) onset = true;
      } else if (this.active[m] && this.evidence[m] < 0.3) {
        this.active[m] = 0;
      }
      if (this.active[m]) this.lastActiveFrame[m] = this.frameIndex;
      if (!hit[m]) this.salience[m] *= 0.7;
    }
    this.framesSinceOnset = onset ? 0 : this.framesSinceOnset + 1;

    const notes = [];
    for (let m = 0; m < 128; m++) if (this.active[m]) notes.push(m);
    let maxSal = 1.0;
    for (const m of notes) maxSal = Math.max(maxSal, this.salience[m]);
    const weights = notes.map((m) => Math.min(1.0, Math.max(0.0, this.salience[m] / maxSal)));

    this.settle(this.name(notes, weights), config);

    return {
      listening: true,
      sampleRate: this.sampleRate,
      levelDb,
      notes: notes.map((m, idx) => new ActiveNote(m, weights[idx])),
      reading: this.settled,
    };
  }

  name(notes, weights) {
    const full = ChordNamer.read(notes, weights);
    const suspect = [];
    for (let i = 0; i < notes.length; i++) {
      if (weights[i] >= SUSPECT_WEIGHT) continue;
      const off = notes.some((low) => low < notes[i] && OVERTONE_OFFSETS.has(notes[i] - low));
      if (off) suspect.push(i);
    }
    if (suspect.length === 0) return full;
    const keep = [];
    for (let i = 0; i < notes.length; i++) if (!suspect.includes(i)) keep.push(i);
    const reduced = ChordNamer.read(keep.map((i) => notes[i]), keep.map((i) => weights[i]));
    if (reduced && reduced.kind === 'chord' && reduced.chord.pitchClasses.size >= 3) return reduced;
    return full;
  }

  framesFor(ms) {
    return Math.max(1, Math.round((ms / 1000.0) * (this.sampleRate / HOP)));
  }

  signature(r) {
    if (!r) return '';
    if (r.kind === 'chord') return `c${r.chord.root}:${r.chord.quality.symbol}:${r.chord.bass}`;
    if (r.kind === 'single') return `n${Notes.pc(r.notes[0])}`;
    return 'i' + [...new Set(r.notes.map((n) => Notes.pc(n)))].sort().join(',');
  }

  pitchClasses(r) {
    if (r.kind === 'chord') {
      const s = new Set(r.chord.pitchClasses);
      s.add(r.chord.bass);
      return s;
    }
    return new Set(r.notes.map((n) => Notes.pc(n)));
  }

  settle(reading, config) {
    const current = this.settled;
    if (reading == null) {
      this.pending = null;
      this.pendingFrames = 0;
      if (current != null && ++this.silentFrames * (HOP / this.sampleRate) >= SILENCE_SECONDS) {
        this.settled = null;
      }
      return;
    }
    this.silentFrames = 0;
    const sig = this.signature(reading);
    if (current != null && sig === this.signature(current)) {
      this.settled = reading;
      this.pending = null;
      this.pendingFrames = 0;
      return;
    }
    if (this.pending != null && this.signature(this.pending) === sig) {
      this.pendingFrames++;
    } else {
      this.pendingFrames = 1;
    }
    this.pending = reading;
    const fading = current != null &&
      this.framesSinceOnset > this.framesFor(config.changeDelayMs) + 2 &&
      this.isSuperset(this.pitchClasses(current), this.pitchClasses(reading));
    if (!fading && this.pendingFrames >= this.framesFor(config.changeDelayMs)) {
      this.settled = reading;
      this.pending = null;
      this.pendingFrames = 0;
    }
  }

  isSuperset(a, b) {
    for (const x of b) if (!a.has(x)) return false;
    return true;
  }
}

/** Skrót: nazwij akord bezpośrednio z MIDI nut (używany w trybie hybrydowym). */
export function matchChordFromMidis(midis, weights = null) {
  return ChordNamer.read(midis, weights);
}

/** Czytelny opis odczytu (do badge/UI). */
export function readingLabel(r) {
  if (!r) return '';
  if (r.kind === 'chord') return r.chord.symbol();
  if (r.kind === 'single') return Notes.nameWithOctave(r.notes[0]);
  if (r.kind === 'interval') return 'Interwał: ' + Notes.intervalName(r.semitones);
  return '';
}
