/**
 * Nazywanie akordów: każda pryma × jakość jest punktowana względem słyszanych klas wysokości.
 * Obsługuje przewroty / slash chords (C/E). Port z composeApp/.../music/Chord.kt + ChordNamer.kt.
 */
import { Notes } from './music.js';

export const Quality = Object.freeze({
  MAJ:     { intervals: [0, 4, 7],          symbol: '',        prior: 0.15, fifthOptional: true },
  MIN:     { intervals: [0, 3, 7],          symbol: 'm',       prior: 0.15, fifthOptional: true },
  DIM:     { intervals: [0, 3, 6],          symbol: 'dim',     prior: 0.05, fifthOptional: false },
  AUG:     { intervals: [0, 4, 8],          symbol: 'aug',     prior: 0.0,  fifthOptional: false },
  SUS2:    { intervals: [0, 2, 7],          symbol: 'sus2',    prior: 0.0,  fifthOptional: false },
  SUS4:    { intervals: [0, 5, 7],          symbol: 'sus4',    prior: 0.05, fifthOptional: false },
  POWER:   { intervals: [0, 7],             symbol: '5',       prior: 0.0,  fifthOptional: false },
  MAJ6:    { intervals: [0, 4, 7, 9],       symbol: '6',       prior: 0.05, fifthOptional: true },
  MIN6:    { intervals: [0, 3, 7, 9],       symbol: 'm6',      prior: 0.05, fifthOptional: true },
  DOM7:    { intervals: [0, 4, 7, 10],      symbol: '7',       prior: 0.1,  fifthOptional: true },
  MAJ7:    { intervals: [0, 4, 7, 11],      symbol: 'maj7',    prior: 0.1,  fifthOptional: true },
  MIN7:    { intervals: [0, 3, 7, 10],      symbol: 'm7',      prior: 0.1,  fifthOptional: true },
  MINMAJ7: { intervals: [0, 3, 7, 11],      symbol: 'm(maj7)', prior: 0.0,  fifthOptional: true },
  DIM7:    { intervals: [0, 3, 6, 9],       symbol: 'dim7',    prior: 0.05, fifthOptional: false },
  HDIM7:   { intervals: [0, 3, 6, 10],      symbol: 'm7b5',    prior: 0.05, fifthOptional: false },
  SEVENSUS4: { intervals: [0, 5, 7, 10],    symbol: '7sus4',   prior: 0.03, fifthOptional: false },
  AUG7:    { intervals: [0, 4, 8, 10],      symbol: '7#5',     prior: 0.0,  fifthOptional: false },
  ADD9:    { intervals: [0, 2, 4, 7],       symbol: 'add9',    prior: 0.03, fifthOptional: true },
  MADD9:   { intervals: [0, 2, 3, 7],       symbol: 'm(add9)', prior: 0.02, fifthOptional: true },
  MAJ69:   { intervals: [0, 2, 4, 7, 9],    symbol: '6/9',     prior: 0.0,  fifthOptional: true },
  DOM9:    { intervals: [0, 2, 4, 7, 10],   symbol: '9',       prior: 0.03, fifthOptional: true },
  MAJ9:    { intervals: [0, 2, 4, 7, 11],   symbol: 'maj9',    prior: 0.03, fifthOptional: true },
  MIN9:    { intervals: [0, 2, 3, 7, 10],   symbol: 'm9',      prior: 0.03, fifthOptional: true },
  DOM7B9:  { intervals: [0, 1, 4, 7, 10],   symbol: '7b9',     prior: 0.0,  fifthOptional: true },
  DOM7S9:  { intervals: [0, 3, 4, 7, 10],   symbol: '7#9',     prior: 0.0,  fifthOptional: true },
  DOM13:   { intervals: [0, 4, 7, 9, 10],   symbol: '13',      prior: 0.0,  fifthOptional: true },
  MIN11:   { intervals: [0, 2, 3, 5, 7, 10], symbol: 'm11',    prior: 0.0,  fifthOptional: true },
});

export const QUALITY_LIST = Object.values(Quality);

const EXTRA_PENALTY = 1.2;
const MISSING_FIFTH_PENALTY = 0.3;
const BASS_IS_ROOT_BONUS = 0.4;
const BASS_OUTSIDE_PENALTY = 0.3;
const COMPLEXITY_PENALTY = 0.1;

export class Chord {
  constructor(root, quality, bass = root) {
    this.root = root;
    this.quality = quality;
    this.bass = bass;
  }

  get pitchClasses() {
    return new Set(this.quality.intervals.map((iv) => Notes.pc(this.root + iv)));
  }

  get isInversion() { return this.bass !== this.root; }

  symbol() {
    const base = Notes.name(this.root) + this.quality.symbol;
    return this.isInversion ? base + '/' + Notes.name(this.bass) : base;
  }
}

// Reading = { kind: 'single'|'interval'|'chord', notes: number[], chord?: Chord, semitones?: number }

export const ChordNamer = {
  required(q) {
    return q.fifthOptional ? q.intervals.filter((iv) => iv !== 7) : q.intervals;
  },

  read(notes, weights = null) {
    if (!notes || notes.length === 0) return null;
    const order = notes.map((_, i) => i).sort((a, b) => notes[a] - notes[b]);
    const sorted = order.map((i) => notes[i]);
    const pcWeight = new Float64Array(12);
    for (const i of order) {
      const w = weights ? Math.min(1.0, Math.max(0.0, weights[i])) : 1.0;
      const pc = Notes.pc(notes[i]);
      pcWeight[pc] = Math.max(pcWeight[pc], 0.5 + 0.5 * w);
    }
    const pcs = [];
    for (let pc = 0; pc < 12; pc++) if (pcWeight[pc] > 0) pcs.push(pc);
    const bass = Notes.pc(sorted[0]);

    if (pcs.length === 1) {
      return { kind: 'single', notes: sorted };
    }
    if (pcs.length === 2) {
      const interval = Notes.pc(pcs[1] - pcs[0]);
      if (interval === 7) return { kind: 'chord', notes: sorted, chord: new Chord(pcs[0], Quality.POWER, bass) };
      if (interval === 5) return { kind: 'chord', notes: sorted, chord: new Chord(pcs[1], Quality.POWER, bass) };
      return { kind: 'interval', notes: sorted, semitones: Notes.pc(sorted[sorted.length - 1] - sorted[0]) };
    }
    const best = ChordNamer.best(pcWeight, bass);
    if (best) return { kind: 'chord', notes: sorted, chord: best };
    return { kind: 'interval', notes: sorted, semitones: Notes.pc(sorted[sorted.length - 1] - sorted[0]) };
  },

  best(pcWeight, bass) {
    let bestChord = null;
    let bestScore = -Infinity;
    for (let root = 0; root < 12; root++) {
      if (pcWeight[root] <= 0) continue;
      for (const q of QUALITY_LIST) {
        if (q === Quality.POWER) continue;
        const s = ChordNamer.score(pcWeight, bass, root, q);
        if (s !== null && s > bestScore) {
          bestScore = s;
          bestChord = new Chord(root, q, bass);
        }
      }
    }
    return bestChord;
  },

  score(pcWeight, bass, root, q) {
    const required = ChordNamer.required(q);
    for (const i of required) {
      if (pcWeight[Notes.pc(root + i)] <= 0) return null;
    }
    const chordPcs = new Set(q.intervals.map((iv) => Notes.pc(root + iv)));
    let covered = 0.0;
    let extra = 0.0;
    for (let pc = 0; pc < 12; pc++) {
      const w = pcWeight[pc];
      if (w <= 0) continue;
      if (chordPcs.has(pc)) covered += w;
      else extra += w;
    }
    let s = covered - EXTRA_PENALTY * extra + q.prior;
    if (q.fifthOptional && q.intervals.includes(7) && pcWeight[Notes.pc(root + 7)] <= 0) {
      s -= MISSING_FIFTH_PENALTY;
    }
    s -= COMPLEXITY_PENALTY * Math.max(0, q.intervals.length - 3);
    if (bass === root) s += BASS_IS_ROOT_BONUS;
    else if (!chordPcs.has(bass)) s -= BASS_OUTSIDE_PENALTY;
    return s;
  },
};
