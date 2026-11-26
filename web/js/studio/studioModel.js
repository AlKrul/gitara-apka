/**
 * Model danych 🎼 Studio TAB:
 *  - Struktura StudioSong (Tracks -> Measures -> Beats -> TabNotes)
 *  - Walidacja metrum, przeliczanie wartości rytmicznych (1/1 .. 1/64, kropki, tuplety)
 *  - Rozwijanie repetycji i volt, spójne nazewnictwo A–G (bez H)
 *  - Zarządzanie ścieżkami (2–5 ścieżek zespołu), Undo/Redo (50 kroków), serializacja .gitara.json
 *  - Dwa gotowe utwory demo + kreator pustego projektu
 */
import {
  DRUM_GRID_LANES,
  GM_DRUM_MIDI,
  createEmptyDrumGrid,
  drumBlockToGridPattern,
  expandDrumBlockToSteps,
  getDrumBlockById,
  resolveDrumMeasureSteps
} from './studioDrumBlocks.js';
import { ChordNamer } from '../dsp/dechorder/chordNamer.js';

export const NOTE_NAMES_SHARP = Object.freeze(['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']);
export const NOTE_NAMES_FLAT = Object.freeze(['C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G', 'Ab', 'A', 'Bb', 'B']);

export const DURATION_OPTIONS = Object.freeze([
  { value: -2, label: '1/1',  namePl: 'Cała nuta' },
  { value: -1, label: '1/2',  namePl: 'Półnuta' },
  { value:  0, label: '1/4',  namePl: 'Ćwierćnuta' },
  { value:  1, label: '1/8',  namePl: 'Ósemka' },
  { value:  2, label: '1/16', namePl: 'Szesnastka' },
  { value:  3, label: '1/32', namePl: 'Trzydziestodwójka' },
  { value:  4, label: '1/64', namePl: 'Sześćdziesięcioczwórka' }
]);

export const TIME_SIGNATURE_PRESETS = Object.freeze([
  { numerator: 4, denominator: 4, label: '4/4' },
  { numerator: 3, denominator: 4, label: '3/4' },
  { numerator: 2, denominator: 4, label: '2/4' },
  { numerator: 6, denominator: 8, label: '6/8' },
  { numerator: 12, denominator: 8, label: '12/8' },
  { numerator: 5, denominator: 4, label: '5/4' },
  { numerator: 7, denominator: 8, label: '7/8' }
]);

export const STUDIO_TUNING_PRESETS = Object.freeze({
  guitar: [
    { id: 'standard',    name: 'Standard (E A D G B E)',    midis: [40, 45, 50, 55, 59, 64] },
    { id: 'drop_d',      name: 'Drop D (D A D G B E)',      midis: [38, 45, 50, 55, 59, 64] },
    { id: 'eb_standard', name: 'Eb Standard (Eb Ab Db Gb Bb Eb)', midis: [39, 44, 49, 54, 58, 63] },
    { id: 'd_standard',  name: 'D Standard (D G C F A D)',  midis: [38, 43, 48, 53, 57, 62] },
    { id: 'drop_c',      name: 'Drop C (C G C F A D)',      midis: [36, 43, 48, 53, 57, 62] },
    { id: 'open_g',      name: 'Open G (D G D G B D)',      midis: [38, 43, 50, 55, 59, 62] },
    { id: 'dadgad',      name: 'DADGAD (D A D G A D)',      midis: [38, 45, 50, 55, 57, 62] }
  ],
  bass: [
    { id: 'bass_standard', name: 'Bas 4-str. Standard (E A D G)', midis: [28, 33, 38, 43] },
    { id: 'bass_drop_d',   name: 'Bas Drop D (D A D G)',          midis: [26, 33, 38, 43] },
    { id: 'bass_eb',       name: 'Bas Eb Standard (Eb Ab Db Gb)', midis: [27, 32, 37, 42] }
  ]
});

export const SCALE_HINT_OPTIONS = Object.freeze([
  { id: 'natural_minor', name: 'Moll naturalna (Eolska)', intervals: [0, 2, 3, 5, 7, 8, 10] },
  { id: 'minor_pentatonic', name: 'Pentatonika molowa', intervals: [0, 3, 5, 7, 10] },
  { id: 'blues', name: 'Skala bluesowa', intervals: [0, 3, 5, 6, 7, 10] },
  { id: 'major', name: 'Durowa (Jońska)', intervals: [0, 2, 4, 5, 7, 9, 11] },
  { id: 'major_pentatonic', name: 'Pentatonika durowa', intervals: [0, 2, 4, 7, 9] },
  { id: 'dorian', name: 'Dorycka', intervals: [0, 2, 3, 5, 7, 9, 10] },
  { id: 'phrygian', name: 'Frygijska', intervals: [0, 1, 3, 5, 7, 8, 10] },
  { id: 'harmonic_minor', name: 'Moll harmoniczna', intervals: [0, 2, 3, 5, 7, 8, 11] },
  { id: 'mixolydian', name: 'Miksolidyjska', intervals: [0, 2, 4, 5, 7, 9, 10] }
]);

export const TRACK_ROLE_PRESETS = Object.freeze({
  guitar1: {
    role: 'guitar1',
    name: 'Gitara 1 (Rytm / Riff)',
    instrumentType: 'guitar',
    tonePreset: 'crunch',
    stringCount: 6,
    tuningId: 'standard',
    openStringMidis: [40, 45, 50, 55, 59, 64],
    capo: 0,
    midiChannel: 0,
    midiProgram: 30,
    isPercussion: false,
    color: '#f39237'
  },
  guitar2: {
    role: 'guitar2',
    name: 'Gitara 2 (Lead / Harmonia)',
    instrumentType: 'guitar',
    tonePreset: 'lead',
    stringCount: 6,
    tuningId: 'standard',
    openStringMidis: [40, 45, 50, 55, 59, 64],
    capo: 0,
    midiChannel: 1,
    midiProgram: 29,
    isPercussion: false,
    color: '#38bdf8'
  },
  bass: {
    role: 'bass',
    name: 'Bas (4-str. E–A–D–G)',
    instrumentType: 'bass',
    tonePreset: 'clean',
    stringCount: 4,
    tuningId: 'bass_standard',
    openStringMidis: [28, 33, 38, 43],
    capo: 0,
    midiChannel: 2,
    midiProgram: 33,
    isPercussion: false,
    color: '#a855f7'
  },
  keys: {
    role: 'keys',
    name: 'Klawisze / Synth',
    instrumentType: 'keys',
    tonePreset: 'rhodes',
    stringCount: 0,
    tuningId: 'keys',
    openStringMidis: [],
    capo: 0,
    midiChannel: 3,
    midiProgram: 4,
    isPercussion: false,
    color: '#22c55e'
  },
  drums: {
    role: 'drums',
    name: 'Perkusja (Klocki)',
    instrumentType: 'drums',
    tonePreset: 'drums',
    stringCount: 0,
    tuningId: 'drums',
    openStringMidis: [],
    capo: 0,
    midiChannel: 9,
    midiProgram: 0,
    isPercussion: true,
    drumAutoFill: 4,
    drumCrashOnOne: true,
    color: '#eab308'
  }
});

export const DEFAULT_DSP_THRESHOLD_ON = 0.58;
export const DEFAULT_DSP_HIGH_THRESHOLD_ON = 0.42;

/**
 * Przelicza względną korektę wrażliwości (-5 .. 0 .. +5, gdzie 0 = sprawdzona domyślna 0.58/0.42)
 * na parę progów detekcji PitchPlease (wyższe + = niższy próg = łatwiejsze wyłapywanie cichych dźwięków).
 */
export function sensitivityOffsetToThresholds(offset = 0, fineHighOffset = 0) {
  const off = Number.isFinite(Number(offset)) ? Math.max(-5, Math.min(5, Math.round(Number(offset)))) : 0;
  const fine = Number.isFinite(Number(fineHighOffset)) ? Math.max(-3, Math.min(3, Math.round(Number(fineHighOffset)))) : 0;
  const rawOn = off === 0
    ? DEFAULT_DSP_THRESHOLD_ON
    : (off > 0 ? (DEFAULT_DSP_THRESHOLD_ON - off * 0.03) : (DEFAULT_DSP_THRESHOLD_ON - off * 0.04));
  const thresholdOn = Math.round(Math.max(0.40, Math.min(0.78, rawOn)) * 100) / 100;
  const baseHigh = off === 0
    ? DEFAULT_DSP_HIGH_THRESHOLD_ON
    : (off > 0 ? (DEFAULT_DSP_HIGH_THRESHOLD_ON - off * 0.025) : (DEFAULT_DSP_HIGH_THRESHOLD_ON - off * 0.04));
  const rawHigh = fine === 0
    ? baseHigh
    : (fine > 0 ? (baseHigh - fine * 0.025) : (baseHigh - fine * 0.035));
  const highThresholdOn = Math.round(Math.max(0.28, Math.min(thresholdOn, Math.min(0.66, rawHigh))) * 100) / 100;
  return { offset: off, fineHighOffset: fine, thresholdOn, highThresholdOn };
}

/**
 * Odtwarza względną korektę (-5..+5) z progu detekcji (0.58 -> 0).
 */
export function thresholdToSensitivityOffset(thresholdOn = DEFAULT_DSP_THRESHOLD_ON) {
  const v = Number(thresholdOn);
  if (!Number.isFinite(v) || Math.abs(v - DEFAULT_DSP_THRESHOLD_ON) < 0.015) return 0;
  const raw = v < DEFAULT_DSP_THRESHOLD_ON
    ? (DEFAULT_DSP_THRESHOLD_ON - v) / 0.03
    : (DEFAULT_DSP_THRESHOLD_ON - v) / 0.04;
  return Math.max(-5, Math.min(5, Math.round(raw)));
}

export const DEFAULT_STUDIO_PREFS = Object.freeze({
  detectSensitivity: DEFAULT_DSP_THRESHOLD_ON,
  detectSensitivityOffset: 0,
  detectHighThresholdOn: null,
  minNoteDurationMs: 80,
  liveRecMode: 'offline',
  quantizeGrid: 1,
  soundMode: 'webaudio',
  showOctaveBands: false,
  octaveAnchorPc: null,
  showAutoChords: true,
  tabZoom: 100,
  fretboardZoom: 100,
  uiMode: 'simple'
});

export const DEFAULT_METRONOME_PATTERN = Object.freeze({
  subdivisions: 1,
  subAccent: 1,
  levels: Object.freeze([3, 1, 1, 1])
});

export const METRONOME_PRESETS = Object.freeze([
  { id: 'standard', name: 'Domyślny (A-C-C-C)', pattern: [3, 1, 1, 1] },
  { id: 'rock',     name: 'Rock (A-C-B-C)',     pattern: [3, 1, 2, 1] },
  { id: 'backbeat', name: 'Backbeat 2&4 (C-A)', pattern: [1, 3, 1, 3] },
  { id: 'gap',      name: 'Gap (bez 3.)',       pattern: [3, 1, 0, 1] }
]);

export function normalizeMetronomePattern(raw, numerator = 4) {
  const num = Math.max(1, Math.min(16, Math.round(Number(numerator) || 4)));
  const src = (raw && typeof raw === 'object') ? raw : {};
  const rawSub = Number(src.subdivisions);
  const subdivisions = [1, 2, 3, 4].includes(rawSub) ? rawSub : 1;
  const subAccent = src.subAccent === 0 ? 0 : 1;
  const levels = [];
  for (let i = 0; i < num; i++) {
    const val = Array.isArray(src.levels) && src.levels[i] !== undefined
      ? Number(src.levels[i])
      : (i === 0 ? 3 : 1);
    levels.push(Number.isFinite(val) ? Math.max(0, Math.min(3, Math.round(val))) : (i === 0 ? 3 : 1));
  }
  return { subdivisions, subAccent, levels };
}

export function applyMetronomePreset(presetId = 'standard', numerator = 4, subdivisions = 1) {
  const num = Math.max(1, Math.min(16, Math.round(Number(numerator) || 4)));
  const preset = METRONOME_PRESETS.find(p => p.id === presetId) || METRONOME_PRESETS[0];
  const levels = [];
  for (let i = 0; i < num; i++) {
    if (presetId === 'backbeat' && num === 3) {
      levels.push(i === 1 ? 3 : 1);
    } else {
      levels.push(preset.pattern[i % preset.pattern.length]);
    }
  }
  return normalizeMetronomePattern({ subdivisions, subAccent: 1, levels }, num);
}

function normalizeCustomDrumGrid(rawGrid, numerator = 4, denominator = 4) {
  if (!rawGrid || typeof rawGrid !== 'object' || !rawGrid.lanes) return null;
  const spb = Number(rawGrid.stepsPerBeat) === 4 ? 4 : 2;
  const base = createEmptyDrumGrid(numerator, denominator, spb);
  for (const lane of DRUM_GRID_LANES) {
    const srcArr = rawGrid.lanes[lane.id];
    if (!Array.isArray(srcArr)) continue;
    for (let i = 0; i < base.stepCount; i++) {
      base.lanes[lane.id][i] = Math.max(0, Math.min(3, Math.round(Number(srcArr[i]) || 0)));
    }
  }
  return base;
}

export const MAX_STUDIO_TRACKS = 5;
const MAX_PLAYBACK_BARS = 512;

export function clampStudioPrefs(raw) {
  const src = (raw && typeof raw === 'object') ? raw : {};
  const hasExplicitOffset = src.detectSensitivityOffset !== undefined && src.detectSensitivityOffset !== null && Number.isFinite(Number(src.detectSensitivityOffset));
  const hasExplicitSens = src.detectSensitivity !== undefined && src.detectSensitivity !== null && Number.isFinite(Number(src.detectSensitivity));
  let detectSensitivityOffset = 0;
  let sens = DEFAULT_DSP_THRESHOLD_ON;
  if (hasExplicitOffset) {
    detectSensitivityOffset = Math.max(-5, Math.min(5, Math.round(Number(src.detectSensitivityOffset))));
    sens = hasExplicitSens
      ? Math.max(0.36, Math.min(0.88, Math.round(Number(src.detectSensitivity) * 100) / 100))
      : sensitivityOffsetToThresholds(detectSensitivityOffset).thresholdOn;
  } else if (hasExplicitSens) {
    sens = Math.max(0.36, Math.min(0.88, Math.round(Number(src.detectSensitivity) * 100) / 100));
    detectSensitivityOffset = thresholdToSensitivityOffset(sens);
  }
  const highThr = (src.detectHighThresholdOn !== null && src.detectHighThresholdOn !== undefined && Number.isFinite(Number(src.detectHighThresholdOn)))
    ? Math.max(0.28, Math.min(0.84, Math.round(Number(src.detectHighThresholdOn) * 100) / 100))
    : null;
  const minDur = Number.isFinite(Number(src.minNoteDurationMs))
    ? Math.max(20, Math.min(300, Math.round(Number(src.minNoteDurationMs))))
    : DEFAULT_STUDIO_PREFS.minNoteDurationMs;
  const mode = (src.liveRecMode === 'live' || src.liveRecMode === 'offline') ? src.liveRecMode : 'offline';
  const rawGrid = Number(src.quantizeGrid);
  const quantizeGrid = Number.isInteger(rawGrid) && rawGrid >= -1 && rawGrid <= 3 ? rawGrid : DEFAULT_STUDIO_PREFS.quantizeGrid;
  const soundMode = src.soundMode === 'sf2' ? 'sf2' : 'webaudio';
  const showOctaveBands = Boolean(src.showOctaveBands);
  const rawAnchor = src.octaveAnchorPc;
  const octaveAnchorPc = (rawAnchor !== null && rawAnchor !== undefined && Number.isInteger(Number(rawAnchor)))
    ? (((Number(rawAnchor) % 12) + 12) % 12)
    : null;
  const showAutoChords = src.showAutoChords !== undefined ? Boolean(src.showAutoChords) : DEFAULT_STUDIO_PREFS.showAutoChords;
  const tabZoom = Number.isFinite(Number(src.tabZoom))
    ? Math.max(70, Math.min(160, Math.round(Number(src.tabZoom))))
    : DEFAULT_STUDIO_PREFS.tabZoom;
  const fretboardZoom = Number.isFinite(Number(src.fretboardZoom))
    ? Math.max(75, Math.min(150, Math.round(Number(src.fretboardZoom))))
    : DEFAULT_STUDIO_PREFS.fretboardZoom;
  const uiMode = src.uiMode === 'pro' ? 'pro' : DEFAULT_STUDIO_PREFS.uiMode;
  return {
    detectSensitivity: sens,
    detectSensitivityOffset,
    detectHighThresholdOn: highThr,
    minNoteDurationMs: minDur,
    liveRecMode: mode,
    quantizeGrid,
    soundMode,
    showOctaveBands,
    octaveAnchorPc,
    showAutoChords,
    tabZoom,
    fretboardZoom,
    uiMode
  };
}

export function cloneSong(song) {
  return JSON.parse(JSON.stringify(song));
}

export function midiToNoteName(midi, accMode = 2) {
  const pc = ((Math.round(midi) % 12) + 12) % 12;
  const arr = (accMode === 1) ? NOTE_NAMES_FLAT : NOTE_NAMES_SHARP;
  return arr[pc];
}

export function midiToNoteWithOctave(midi, accMode = 2) {
  const m = Math.round(midi);
  const oct = Math.floor(m / 12) - 1;
  return `${midiToNoteName(m, accMode)}${oct}`;
}

export function midiToHz(midi, a4Hz = 440.0) {
  return a4Hz * Math.pow(2, (midi - 69) / 12);
}

export function getScaleHintPitchClasses(rootPc = 0, scaleId = 'natural_minor') {
  const sc = SCALE_HINT_OPTIONS.find(s => s.id === scaleId) || SCALE_HINT_OPTIONS[0];
  const root = ((Number(rootPc) % 12) + 12) % 12;
  return sc.intervals.map(iv => (root + iv) % 12);
}

export const BEND_PRESETS = Object.freeze({
  half:         { id: 'half',         label: '+1/2',         shortLabel: '+½',  points: [{ pos: 0, val: 0 }, { pos: 60, val: 50 }] },
  full:         { id: 'full',         label: '+1',           shortLabel: '+1',  points: [{ pos: 0, val: 0 }, { pos: 60, val: 100 }] },
  one_half:     { id: 'one_half',     label: '+1½',          shortLabel: '+1½', points: [{ pos: 0, val: 0 }, { pos: 60, val: 150 }] },
  bend_release: { id: 'bend_release', label: 'Bend+Release', shortLabel: 'B+R', points: [{ pos: 0, val: 0 }, { pos: 30, val: 100 }, { pos: 60, val: 0 }] },
  pre_bend:     { id: 'pre_bend',     label: 'Pre-bend',     shortLabel: 'Pre', points: [{ pos: 0, val: 100 }, { pos: 60, val: 100 }] }
});

export const WHAMMY_PRESETS = Object.freeze({
  dive_half:  { id: 'dive_half',  label: 'Dive -½',       shortLabel: '-½',  points: [{ pos: 0, val: 0 }, { pos: 60, val: -50 }] },
  dive_full:  { id: 'dive_full',  label: 'Dive -1 ton',   shortLabel: '-1',  points: [{ pos: 0, val: 0 }, { pos: 60, val: -100 }] },
  dive_bomb:  { id: 'dive_bomb',  label: 'Dive Bomb -2½', shortLabel: '-2½', points: [{ pos: 0, val: 0 }, { pos: 60, val: -250 }] },
  dip_half:   { id: 'dip_half',   label: 'Dip v -½',      shortLabel: 'v-½', points: [{ pos: 0, val: 0 }, { pos: 30, val: -50 }, { pos: 60, val: 0 }] },
  dip_full:   { id: 'dip_full',   label: 'Dip v -1',      shortLabel: 'v-1', points: [{ pos: 0, val: 0 }, { pos: 30, val: -100 }, { pos: 60, val: 0 }] },
  scoop:      { id: 'scoop',      label: 'Scoop /-1→0',   shortLabel: '/-1', points: [{ pos: 0, val: -100 }, { pos: 30, val: 0 }, { pos: 60, val: 0 }] },
  raise_half: { id: 'raise_half', label: 'Pull Up +½',    shortLabel: '+½',  points: [{ pos: 0, val: 0 }, { pos: 60, val: 50 }] }
});

export const DYNAMIC_LEVELS = Object.freeze([
  { value: 1, label: 'pp' },
  { value: 2, label: 'p' },
  { value: 3, label: 'mp' },
  { value: 4, label: 'mf' },
  { value: 5, label: 'f' },
  { value: 6, label: 'ff' },
  { value: 7, label: 'fff' }
]);

export function createTabNote(opts = {}) {
  const slideType = String(opts.slideType || (opts.slide ? 'shift' : ''));
  const bendPreset = String(opts.bendPreset || '');
  const bendPoints = (Array.isArray(opts.bendPoints) && opts.bendPoints.length > 0)
    ? opts.bendPoints.map(p => ({ ...p }))
    : (bendPreset && BEND_PRESETS[bendPreset] ? BEND_PRESETS[bendPreset].points.map(p => ({ ...p })) : []);
  const whammyPreset = String(opts.whammyPreset || '');
  const whammyPoints = (Array.isArray(opts.whammyPoints) && opts.whammyPoints.length > 0)
    ? opts.whammyPoints.map(p => ({ ...p }))
    : (whammyPreset && WHAMMY_PRESETS[whammyPreset] ? WHAMMY_PRESETS[whammyPreset].points.map(p => ({ ...p })) : []);
  return {
    string: Number.isFinite(opts.string) ? opts.string : 0,
    fret: Number.isFinite(opts.fret) ? Math.max(0, Math.min(24, Math.round(opts.fret))) : 0,
    midi: Number.isFinite(opts.midi) ? Math.round(opts.midi) : undefined,
    velocity: Number.isFinite(opts.velocity) ? Math.max(1, Math.min(127, Math.round(opts.velocity))) : 96,
    tied: Boolean(opts.tied),
    hammerOn: Boolean(opts.hammerOn),
    slide: Boolean(opts.slide || slideType),
    slideType,
    palmMute: Boolean(opts.palmMute),
    letRing: Boolean(opts.letRing),
    vibrato: Boolean(opts.vibrato),
    wideVibrato: Boolean(opts.wideVibrato),
    deadNote: Boolean(opts.deadNote),
    ghostNote: Boolean(opts.ghostNote),
    accented: Boolean(opts.accented),
    heavyAccented: Boolean(opts.heavyAccented),
    staccato: Boolean(opts.staccato),
    tap: Boolean(opts.tap),
    graceNote: Boolean(opts.graceNote),
    tremoloPicking: Boolean(opts.tremoloPicking),
    trill: Boolean(opts.trill),
    harmonicType: Number.isFinite(opts.harmonicType) ? opts.harmonicType : 0,
    bendPreset,
    bendPoints,
    whammyPreset,
    whammyPoints
  };
}

export function createBeat(opts = {}) {
  const notes = Array.isArray(opts.notes) ? opts.notes.map(n => createTabNote(n)) : [];
  const isRest = opts.isRest !== undefined ? Boolean(opts.isRest) : (notes.length === 0);
  return {
    duration: Number.isFinite(opts.duration) ? Math.max(-2, Math.min(4, Math.round(opts.duration))) : 0,
    dots: Number.isFinite(opts.dots) ? Math.max(0, Math.min(2, Math.round(opts.dots))) : (opts.dotted ? 1 : 0),
    tupletNumerator: Number.isFinite(opts.tupletNumerator) ? Math.max(0, Math.round(opts.tupletNumerator)) : 0,
    tupletDenominator: Number.isFinite(opts.tupletDenominator) ? Math.max(0, Math.round(opts.tupletDenominator)) : 0,
    isRest,
    dynamic: Number.isFinite(opts.dynamic) ? opts.dynamic : 6,
    pickStroke: opts.pickStroke || 0,
    brushStroke: String(opts.brushStroke || ''),
    fermata: Boolean(opts.fermata),
    chordSymbol: opts.chordSymbol || '',
    text: opts.text || '',
    notes: isRest ? [] : notes
  };
}

export function createEmptyBeat(duration = 0, isRest = true) {
  return createBeat({ duration, isRest, notes: [] });
}

export function createEmptyMeasure(numerator = 4, denominator = 4, drumBlockId = 'rock_8th', isDrumTrack = false, defaultBeatDuration = null) {
  const num = Math.max(1, Math.min(16, Number(numerator) || 4));
  const den = [2, 4, 8, 16].includes(Number(denominator)) ? Number(denominator) : 4;
  const beats = [];
  if (!isDrumTrack) {
    if (Number.isInteger(defaultBeatDuration)) {
      const beatDur = Math.max(-2, Math.min(4, defaultBeatDuration));
      const beatQ = durationToQuarterNotes({ duration: beatDur });
      const capQ = num * (4 / den);
      const count = Math.max(1, Math.min(16, Math.round(capQ / Math.max(0.0625, beatQ))));
      for (let i = 0; i < count; i++) {
        beats.push(createEmptyBeat(beatDur, true));
      }
    } else {
      // Wypełniamy takty pojedynczymi pauzami o wartości odpowiadającej metrum (np. 4x ćwierćnuta w 4/4)
      const beatDur = (den === 16) ? 2 : (den === 8 ? 1 : (den === 2 ? -1 : 0));
      for (let i = 0; i < num; i++) {
        beats.push(createEmptyBeat(beatDur, true));
      }
    }
  }
  return {
    numerator: num,
    denominator: den,
    isPickup: false,
    repeatOpen: false,
    repeatClose: 0,
    voltaMask: 0,
    sectionMarker: '',
    tripletFeel: 0,
    drumBlockId: isDrumTrack ? (drumBlockId || 'rock_8th') : '',
    customDrumGrid: null,
    beats
  };
}

export function durationToQuarterNotes(beat) {
  if (!beat) return 1;
  const d = Number.isFinite(beat.duration) ? beat.duration : 0;
  let q = 4 * Math.pow(2, -(d + 2));
  const dots = Number.isFinite(beat.dots) ? beat.dots : (beat.dotted ? 1 : 0);
  if (dots === 1) q *= 1.5;
  else if (dots === 2) q *= 1.75;
  if (beat.tupletNumerator > 0 && beat.tupletDenominator > 0) {
    q *= (beat.tupletDenominator / beat.tupletNumerator);
  }
  return q;
}

export function measureCapacityQuarterNotes(measure) {
  const num = Math.max(1, Number(measure?.numerator) || 4);
  const den = Math.max(1, Number(measure?.denominator) || 4);
  return num * (4 / den);
}

export function measureFilledQuarterNotes(measure) {
  if (!measure || !Array.isArray(measure.beats)) return 0;
  let sum = 0;
  for (const b of measure.beats) {
    sum += durationToQuarterNotes(b);
  }
  return Math.round(sum * 10000) / 10000;
}

export function validateMeasureMeter(measure) {
  const capacity = Math.round(measureCapacityQuarterNotes(measure) * 10000) / 10000;
  const filled = measureFilledQuarterNotes(measure);
  const delta = Math.round((filled - capacity) * 10000) / 10000;
  let status = 'ok';
  if (delta < -0.002) status = 'under';
  else if (delta > 0.002) status = 'over';
  return { filled, capacity, delta, status };
}

export function noteToMidi(track, note) {
  if (!note) return null;
  if (track && Array.isArray(track.openStringMidis) && track.openStringMidis.length > 0 && Number.isFinite(note.fret)) {
    const sIdx = Number(note.string);
    if (sIdx >= 0 && sIdx < track.openStringMidis.length) {
      return track.openStringMidis[sIdx] + (track.capo || 0) + Math.round(note.fret);
    }
  }
  if (Number.isFinite(note.midi)) {
    return Math.round(note.midi);
  }
  return null;
}

export function createTrackPreset(role = 'guitar1', measureCount = 4, numerator = 4, denominator = 4) {
  const base = TRACK_ROLE_PRESETS[role] || TRACK_ROLE_PRESETS.guitar1;
  const isDrum = base.instrumentType === 'drums';
  const measures = [];
  for (let i = 0; i < Math.max(1, measureCount); i++) {
    measures.push(createEmptyMeasure(numerator, denominator, 'rock_8th', isDrum));
  }
  return {
    id: `${base.role}_${Date.now().toString(36)}_${Math.floor(Math.random() * 1000)}`,
    role: base.role,
    name: base.name,
    instrumentType: base.instrumentType,
    tonePreset: base.tonePreset,
    stringCount: base.stringCount,
    tuningId: base.tuningId,
    openStringMidis: base.openStringMidis.slice(),
    capo: base.capo || 0,
    midiChannel: base.midiChannel,
    midiProgram: base.midiProgram,
    isPercussion: Boolean(base.isPercussion),
    drumAutoFill: base.drumAutoFill || 0,
    drumCrashOnOne: base.drumCrashOnOne !== undefined ? Boolean(base.drumCrashOnOne) : false,
    muted: false,
    solo: false,
    volume: 0.85,
    color: base.color,
    measures
  };
}

export function isTrackAudible(tracks, trackIdx) {
  if (!Array.isArray(tracks) || trackIdx < 0 || trackIdx >= tracks.length) return false;
  const t = tracks[trackIdx];
  if (!t || t.muted) return false;
  const anySolo = tracks.some(tr => Boolean(tr && tr.solo));
  return anySolo ? Boolean(t.solo) : true;
}

export function getMasterMeasureCount(song) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length === 0) return 0;
  return song.tracks.reduce((mx, tr) => Math.max(mx, Array.isArray(tr.measures) ? tr.measures.length : 0), 0);
}

/**
 * Rozwija sekwencję taktów z uwzględnieniem repetycji (`repeatOpen` / `repeatClose`) oraz volt (`voltaMask`).
 * Chroni przed nieskończoną pętlą limitem MAX_PLAYBACK_BARS.
 */
export function expandPlaybackTimeline(song) {
  const numMeasures = getMasterMeasureCount(song);
  if (numMeasures <= 0) return [];
  const masterMeasures = song.tracks[0]?.measures || [];

  const timeline = [];
  const repeatPassCount = new Map();
  let repeatStartIdx = 0;
  let lastRepeatCloseIdx = -1;
  let currentPass = 1;
  let justJumpedBack = false;
  let startQ = 0;
  let idx = 0;

  while (idx < numMeasures && timeline.length < MAX_PLAYBACK_BARS) {
    const m = masterMeasures[idx] || { numerator: 4, denominator: 4, repeatOpen: false, repeatClose: 0, voltaMask: 0 };
    if (m.repeatOpen && !justJumpedBack) {
      repeatStartIdx = idx;
      currentPass = 1;
    }
    justJumpedBack = false;

    const voltaMask = Number(m.voltaMask) || 0;
    if (voltaMask === 0 && idx > lastRepeatCloseIdx && currentPass > 1) {
      currentPass = 1;
      repeatStartIdx = idx;
    }
    if (voltaMask > 0) {
      const bit = 1 << Math.max(0, currentPass - 1);
      if ((voltaMask & bit) === 0) {
        idx++;
        continue;
      }
    }

    const lenQ = measureCapacityQuarterNotes(m);
    timeline.push({
      seqIdx: timeline.length,
      measureIdx: idx,
      startQ,
      lengthQ: lenQ,
      numerator: m.numerator || 4,
      denominator: m.denominator || 4,
      pass: currentPass
    });
    startQ += lenQ;

    const repeatTarget = Number(m.repeatClose) || 0;
    if (repeatTarget >= 2) {
      lastRepeatCloseIdx = Math.max(lastRepeatCloseIdx, idx);
      if (currentPass < repeatTarget) {
        currentPass += 1;
        justJumpedBack = true;
        idx = Math.max(0, Math.min(idx, repeatStartIdx));
        continue;
      } else {
        const nextVolta = Number(masterMeasures[idx + 1]?.voltaMask) || 0;
        if (nextVolta === 0) {
          currentPass = 1;
          repeatStartIdx = idx + 1;
        }
      }
    }

    idx++;
  }

  return timeline;
}

/**
 * Buduje płaską listę zdarzeń audio dla pojedynczej ścieżki na podstawie rozwiniętej osi czasu.
 */
export function buildBeatEventsForTrack(song, trackIdx, timeline = null) {
  if (!song || !Array.isArray(song.tracks) || trackIdx < 0 || trackIdx >= song.tracks.length) return [];
  const track = song.tracks[trackIdx];
  const tl = timeline || expandPlaybackTimeline(song);
  const events = [];

  if (track.instrumentType === 'drums' || track.isPercussion) {
    for (const entry of tl) {
      const m = track.measures[entry.measureIdx];
      const steps = resolveDrumMeasureSteps({ ...m, numerator: entry.numerator, denominator: entry.denominator }, track, entry.measureIdx);
      steps.forEach((step, stepIdx) => {
        events.push({
          kind: 'drum',
          trackIdx,
          seqIdx: entry.seqIdx,
          measureIdx: entry.measureIdx,
          beatIdx: stepIdx,
          startQ: entry.startQ + step.beatOffsetQ,
          durationQ: 0.25,
          hits: step.hits
        });
      });
    }
    return events;
  }

  const isKeysTrack = track.instrumentType === 'keys';
  let prevNoteByKey = new Map();

  for (const entry of tl) {
    const m = track.measures[entry.measureIdx];
    if (!m || !Array.isArray(m.beats)) continue;
    let offsetQ = 0;
    for (let bIdx = 0; bIdx < m.beats.length; bIdx++) {
      const beat = m.beats[bIdx];
      const durQ = durationToQuarterNotes(beat);
      if (!beat.isRest && Array.isArray(beat.notes) && beat.notes.length > 0) {
        const resolved = [];
        const currNoteByKey = new Map();
        for (const n of beat.notes) {
          const midi = noteToMidi(track, n);
          if (midi !== null && midi >= 0 && midi <= 127) {
            const tieKey = isKeysTrack ? `m:${midi}` : `s:${n.string}:m:${midi}`;
            const prevMatched = prevNoteByKey.get(tieKey);
            if (n.tied && prevMatched) {
              prevMatched.tieExtraQ = (prevMatched.tieExtraQ || 0) + durQ;
              currNoteByKey.set(tieKey, prevMatched);
            } else {
              const outNote = { ...n, midi, tieExtraQ: 0 };
              resolved.push(outNote);
              currNoteByKey.set(tieKey, outNote);
            }
          }
        }
        prevNoteByKey = currNoteByKey;
        if (resolved.length > 0) {
          events.push({
            kind: 'notes',
            trackIdx,
            seqIdx: entry.seqIdx,
            measureIdx: entry.measureIdx,
            beatIdx: bIdx,
            startQ: entry.startQ + offsetQ,
            durationQ: durQ,
            beatDynamic: Math.max(1, Math.min(7, Number(beat.dynamic) || 6)),
            beat,
            notes: resolved
          });
        }
      } else {
        prevNoteByKey = new Map();
      }
      offsetQ += durQ;
    }
  }
  return events;
}

export function addTrack(song, role = 'guitar2') {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length >= MAX_STUDIO_TRACKS) return -1;
  const masterLen = Math.max(1, getMasterMeasureCount(song));
  const refMeasures = song.tracks[0]?.measures || [];
  const tr = createTrackPreset(role, masterLen, refMeasures[0]?.numerator || 4, refMeasures[0]?.denominator || 4);
  const isDrum = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
  for (let i = 0; i < masterLen; i++) {
    const ref = refMeasures[i];
    if (ref && tr.measures[i]) {
      const rebuilt = createEmptyMeasure(ref.numerator, ref.denominator, isDrum ? 'rock_8th' : '', isDrum);
      rebuilt.repeatOpen = Boolean(ref.repeatOpen);
      rebuilt.repeatClose = Number(ref.repeatClose) || 0;
      rebuilt.voltaMask = Number(ref.voltaMask) || 0;
      rebuilt.sectionMarker = ref.sectionMarker || '';
      tr.measures[i] = rebuilt;
    }
  }
  // Uniknij kolizji kanału MIDI jeśli rola była już użyta
  const usedChannels = new Set(song.tracks.map(t => t.midiChannel));
  if (tr.isPercussion) {
    tr.midiChannel = 9;
  } else if (usedChannels.has(tr.midiChannel)) {
    for (let ch = 0; ch < 16; ch++) {
      if (ch !== 9 && !usedChannels.has(ch)) {
        tr.midiChannel = ch;
        break;
      }
    }
  }
  song.tracks.push(tr);
  return song.tracks.length - 1;
}

export function duplicateTrack(song, trackIdx) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length >= MAX_STUDIO_TRACKS) return -1;
  if (trackIdx < 0 || trackIdx >= song.tracks.length) return -1;
  const copy = cloneSong(song.tracks[trackIdx]);
  copy.id = `${copy.role || 'track'}_${Date.now().toString(36)}`;
  copy.name = `${copy.name} (kopia)`;
  const usedChannels = new Set(song.tracks.map(t => t.midiChannel));
  if (!copy.isPercussion && usedChannels.has(copy.midiChannel)) {
    for (let ch = 0; ch < 16; ch++) {
      if (ch !== 9 && !usedChannels.has(ch)) {
        copy.midiChannel = ch;
        break;
      }
    }
  }
  song.tracks.push(copy);
  return song.tracks.length - 1;
}

export function removeTrack(song, trackIdx) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length <= 1) return false;
  if (trackIdx < 0 || trackIdx >= song.tracks.length) return false;
  song.tracks.splice(trackIdx, 1);
  return true;
}

export function addMeasure(song, afterIdx = -1, defaultBeatDuration = null) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length === 0) return -1;
  const masterLen = getMasterMeasureCount(song);
  const insertPos = (afterIdx >= 0 && afterIdx < masterLen) ? afterIdx + 1 : masterLen;
  const refIdx = Math.max(0, Math.min(masterLen - 1, insertPos - 1));
  const refM = song.tracks[0]?.measures[refIdx] || { numerator: 4, denominator: 4 };

  for (const tr of song.tracks) {
    const isDrum = tr.instrumentType === 'drums' || tr.isPercussion;
    const prevBlock = tr.measures[refIdx]?.drumBlockId || 'rock_8th';
    const newM = createEmptyMeasure(refM.numerator, refM.denominator, prevBlock, isDrum, defaultBeatDuration);
    tr.measures.splice(insertPos, 0, newM);
  }
  return insertPos;
}

export function removeMeasure(song, measureIdx) {
  if (!song || !Array.isArray(song.tracks) || getMasterMeasureCount(song) <= 1) return false;
  if (measureIdx < 0 || measureIdx >= getMasterMeasureCount(song)) return false;
  for (const tr of song.tracks) {
    if (measureIdx < tr.measures.length) {
      tr.measures.splice(measureIdx, 1);
    }
  }
  return true;
}

export function duplicateMeasures(song, startIdx, endIdx, repeatCount = 1, allTracks = true, activeTrackIdx = 0) {
  if (!song || !Array.isArray(song.tracks)) return false;
  const maxM = getMasterMeasureCount(song);
  const s = Math.max(0, Math.min(startIdx, endIdx));
  const e = Math.min(maxM - 1, Math.max(startIdx, endIdx));
  const reps = Math.max(1, Math.min(8, Math.round(repeatCount)));
  if (s > e) return false;

  const targetTracks = allTracks
    ? song.tracks
    : (song.tracks[activeTrackIdx] ? [song.tracks[activeTrackIdx]] : []);

  for (const tr of targetTracks) {
    const slice = tr.measures.slice(s, e + 1);
    const toInsert = [];
    for (let r = 0; r < reps; r++) {
      for (const m of slice) {
        const cm = cloneSong(m);
        cm.repeatOpen = false;
        cm.repeatClose = 0;
        cm.voltaMask = 0;
        toInsert.push(cm);
      }
    }
    tr.measures.splice(e + 1, 0, ...toInsert);
  }
  // Utrzymaj równą długość wszystkich ścieżek przy duplikacji pojedynczej ścieżki
  const newMax = getMasterMeasureCount(song);
  for (const tr of song.tracks) {
    while (tr.measures.length < newMax) {
      const ref = song.tracks[0]?.measures[tr.measures.length] || { numerator: 4, denominator: 4 };
      tr.measures.push(createEmptyMeasure(ref.numerator, ref.denominator, 'break', tr.instrumentType === 'drums'));
    }
  }
  return true;
}

export function setTrackTuning(track, tuningId, customMidis = null) {
  if (!track || track.instrumentType === 'drums' || track.instrumentType === 'keys') return false;
  const list = track.instrumentType === 'bass' ? STUDIO_TUNING_PRESETS.bass : STUDIO_TUNING_PRESETS.guitar;
  const found = list.find(t => t.id === tuningId);
  if (found) {
    track.tuningId = found.id;
    track.openStringMidis = found.midis.slice();
    track.stringCount = found.midis.length;
    return true;
  }
  if (Array.isArray(customMidis) && customMidis.length === track.stringCount) {
    track.tuningId = 'custom';
    track.openStringMidis = customMidis.slice();
    return true;
  }
  return false;
}

export function setDrumBlockForMeasure(song, trackIdx, measureIdx, blockId) {
  const tr = song?.tracks?.[trackIdx];
  if (!tr || (tr.instrumentType !== 'drums' && !tr.isPercussion)) return false;
  const m = tr.measures?.[measureIdx];
  if (!m) return false;
  m.drumBlockId = getDrumBlockById(blockId).id;
  m.customDrumGrid = null;
  return true;
}

export function setMeasureTimeSignature(song, measureIdx, numerator, denominator, applyFromHere = false) {
  if (!song || !Array.isArray(song.tracks)) return false;
  const num = Math.max(1, Math.min(16, Number(numerator) || 4));
  const den = [2, 4, 8, 16].includes(Number(denominator)) ? Number(denominator) : 4;
  const maxM = getMasterMeasureCount(song);
  if (measureIdx < 0 || measureIdx >= maxM) return false;
  const endM = applyFromHere ? maxM : measureIdx + 1;
  for (const tr of song.tracks) {
    for (let i = measureIdx; i < endM && i < tr.measures.length; i++) {
      tr.measures[i].numerator = num;
      tr.measures[i].denominator = den;
    }
  }
  return true;
}

export function createStudioHistory(maxSteps = 50) {
  const undoStack = [];
  const redoStack = [];
  return {
    push(snapshot) {
      if (!snapshot) return;
      undoStack.push(cloneSong(snapshot));
      if (undoStack.length > maxSteps) undoStack.shift();
      redoStack.length = 0;
    },
    undo(currentSong) {
      if (undoStack.length === 0) return null;
      if (currentSong) redoStack.push(cloneSong(currentSong));
      return undoStack.pop();
    },
    redo(currentSong) {
      if (redoStack.length === 0) return null;
      if (currentSong) undoStack.push(cloneSong(currentSong));
      return redoStack.pop();
    },
    canUndo() { return undoStack.length > 0; },
    canRedo() { return redoStack.length > 0; },
    clear() { undoStack.length = 0; redoStack.length = 0; },
    get size() { return undoStack.length; }
  };
}

export function serializeStudioSong(song, prefs = null, extraMeta = null) {
  const payload = {
    format: 'apka-gitara-studio',
    version: 1,
    savedAt: new Date().toISOString(),
    studioPrefs: clampStudioPrefs(prefs),
    song
  };
  if (extraMeta && typeof extraMeta === 'object' && extraMeta.proBackup) {
    payload.proBackup = extraMeta.proBackup;
  }
  return JSON.stringify(payload, null, 2);
}

export function deserializeStudioSong(rawInput) {
  let parsed;
  try {
    parsed = typeof rawInput === 'string' ? JSON.parse(rawInput) : rawInput;
  } catch (err) {
    throw new Error(`Nieprawidłowy plik projektu JSON: ${err.message}`);
  }
  const sourceObj = (parsed && parsed.format === 'apka-gitara-library-v1' && parsed.item && typeof parsed.item === 'object')
    ? parsed.item
    : parsed;
  const songObj = (sourceObj && sourceObj.song && Array.isArray(sourceObj.song.tracks))
    ? sourceObj.song
    : (sourceObj && Array.isArray(sourceObj.tracks) ? sourceObj : null);
  if (!songObj || songObj.tracks.length === 0) {
    throw new Error('Plik nie zawiera poprawnej struktury utworu Studio TAB (brak ścieżek).');
  }
  const normalizedTracks = songObj.tracks.slice(0, MAX_STUDIO_TRACKS).map((tr, idx) => {
    const preset = TRACK_ROLE_PRESETS[tr.role] || TRACK_ROLE_PRESETS.guitar1;
    const isDrum = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
    const isKeys = tr.instrumentType === 'keys';
    const openMidis = Array.isArray(tr.openStringMidis) && tr.openStringMidis.length > 0
      ? tr.openStringMidis.slice()
      : preset.openStringMidis.slice();
    const measures = (Array.isArray(tr.measures) && tr.measures.length > 0 ? tr.measures : [createEmptyMeasure(4, 4, 'rock_8th', isDrum)])
      .map(m => ({
        numerator: Math.max(1, Math.min(16, Number(m.numerator) || 4)),
        denominator: [2, 4, 8, 16].includes(Number(m.denominator)) ? Number(m.denominator) : 4,
        isPickup: Boolean(m.isPickup),
        repeatOpen: Boolean(m.repeatOpen),
        repeatClose: Number(m.repeatClose) || 0,
        voltaMask: Number(m.voltaMask) || 0,
        sectionMarker: String(m.sectionMarker || ''),
        tripletFeel: Number(m.tripletFeel) || 0,
        drumBlockId: isDrum ? getDrumBlockById(m.drumBlockId || 'rock_8th').id : '',
        customDrumGrid: isDrum ? normalizeCustomDrumGrid(m.customDrumGrid, Number(m.numerator) || 4, Number(m.denominator) || 4) : null,
        beats: isDrum ? [] : (Array.isArray(m.beats) && m.beats.length > 0 ? m.beats.map(b => createBeat(b)) : [createEmptyBeat(-2, true)])
      }));
    return {
      id: tr.id || `tr_${idx}`,
      role: tr.role || preset.role,
      name: String(tr.name || preset.name),
      instrumentType: isDrum ? 'drums' : (isKeys ? 'keys' : (tr.instrumentType === 'bass' ? 'bass' : 'guitar')),
      tonePreset: tr.tonePreset || preset.tonePreset,
      stringCount: (isDrum || isKeys) ? 0 : openMidis.length,
      tuningId: tr.tuningId || preset.tuningId,
      openStringMidis: (isDrum || isKeys) ? [] : openMidis,
      capo: Number(tr.capo) || 0,
      midiChannel: Number.isFinite(tr.midiChannel) ? tr.midiChannel : idx,
      midiProgram: Number.isFinite(tr.midiProgram) ? tr.midiProgram : preset.midiProgram,
      isPercussion: isDrum,
      drumAutoFill: Number(tr.drumAutoFill) || 0,
      drumCrashOnOne: tr.drumCrashOnOne !== undefined ? Boolean(tr.drumCrashOnOne) : isDrum,
      muted: Boolean(tr.muted),
      solo: Boolean(tr.solo),
      volume: Number.isFinite(Number(tr.volume)) ? Math.max(0, Math.min(1, Number(tr.volume))) : 0.85,
      color: tr.color || preset.color,
      measures
    };
  });

  return {
    song: {
      title: String(songObj.title || 'Bez tytułu'),
      artist: String(songObj.artist || 'Apka Gitarowa'),
      album: String(songObj.album || ''),
      tempoBpm: Math.max(30, Math.min(280, Math.round(Number(songObj.tempoBpm) || 120))),
      keyRootPc: Number.isFinite(Number(songObj.keyRootPc)) ? ((Number(songObj.keyRootPc) % 12) + 12) % 12 : 4,
      keyScaleId: String(songObj.keyScaleId || 'natural_minor'),
      metronomePattern: normalizeMetronomePattern(songObj.metronomePattern, normalizedTracks[0]?.measures?.[0]?.numerator || 4),
      tracks: normalizedTracks
    },
    studioPrefs: clampStudioPrefs(sourceObj?.studioPrefs || parsed?.studioPrefs),
    proBackup: (sourceObj?.proBackup || parsed?.proBackup) || null
  };
}

export function createEmptySong(opts = {}) {
  const num = Number(opts.numerator) || 4;
  const den = Number(opts.denominator) || 4;
  const count = Math.max(1, Number(opts.measureCount) || 4);
  return {
    title: opts.title || 'Nowy utwór',
    artist: opts.artist || 'Apka Gitarowa',
    album: opts.album || '',
    tempoBpm: Math.max(30, Math.min(280, Number(opts.bpm) || 120)),
    keyRootPc: Number.isFinite(opts.keyRootPc) ? opts.keyRootPc : 4,
    keyScaleId: opts.keyScaleId || 'natural_minor',
    metronomePattern: normalizeMetronomePattern(opts.metronomePattern, num),
    tracks: [
      createTrackPreset('guitar1', count, num, den),
      createTrackPreset('drums', count, num, den)
    ]
  };
}

// Krótki pomocnik do budowania riffów demo z dokładnym metrum 4/4
function nBeat(notesArr, dur = 1, extra = {}) {
  return createBeat({
    duration: dur,
    isRest: false,
    notes: notesArr,
    ...extra
  });
}

function rBeat(dur = 0) {
  return createEmptyBeat(dur, true);
}

/**
 * Tworzy w pełni muzyczny utwór demonstracyjny gotowy do odtworzenia od razu po wejściu do Studia.
 */
export function createDemoSong(presetId = 'rock_riff_em') {
  if (presetId === 'blues_shuffle_a') {
    const g1 = createTrackPreset('guitar1', 8, 4, 4);
    g1.tonePreset = 'clean';
    g1.name = 'Gitara 1 (Blues Clean)';
    const chordBar = (rootStr, rootFret, fifthFret, sixthFret, symbol, sec = '') => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = sec;
      m.beats = [
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: fifthFret }], 1, { chordSymbol: symbol }),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: fifthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: sixthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: sixthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: fifthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: fifthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: sixthFret }], 1),
        nBeat([{ string: rootStr, fret: rootFret }, { string: rootStr + 1, fret: fifthFret }], 1)
      ];
      return m;
    };
    g1.measures = [
      chordBar(1, 0, 2, 4, 'A5', 'Zwrotka A'),
      chordBar(1, 0, 2, 4, 'A6'),
      chordBar(2, 0, 2, 4, 'D5', 'Subdominanta'),
      chordBar(1, 0, 2, 4, 'A5'),
      chordBar(0, 0, 2, 4, 'E5', 'Turnaround'),
      chordBar(2, 0, 2, 4, 'D5'),
      chordBar(1, 0, 2, 4, 'A5'),
      chordBar(0, 0, 2, 4, 'E7')
    ];

    const bass = createTrackPreset('bass', 8, 4, 4);
    const walkBar = (str, baseFret) => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: str, fret: baseFret }], 0),
        nBeat([{ string: str, fret: baseFret + 4 }], 0),
        nBeat([{ string: str + 1, fret: baseFret + 2 }], 0),
        nBeat([{ string: str + 1, fret: baseFret + 4 }], 0)
      ];
      return m;
    };
    bass.measures = [
      walkBar(1, 0), walkBar(1, 0), walkBar(2, 0), walkBar(1, 0),
      walkBar(0, 0), walkBar(2, 0), walkBar(1, 0), walkBar(0, 0)
    ];

    const keys = createTrackPreset('keys', 8, 4, 4);
    const keysBar = (midis, sym) => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat(midis.map(mi => ({ midi: mi })), -1, { chordSymbol: sym }),
        rBeat(0),
        nBeat(midis.map(mi => ({ midi: mi })), 0)
      ];
      return m;
    };
    keys.measures = [
      keysBar([57, 61, 64, 67], 'A7'), keysBar([57, 61, 64, 67], 'A7'),
      keysBar([62, 66, 69, 72], 'D7'), keysBar([57, 61, 64, 67], 'A7'),
      keysBar([52, 56, 59, 62], 'E7'), keysBar([62, 66, 69, 72], 'D7'),
      keysBar([57, 61, 64, 67], 'A7'), keysBar([52, 56, 59, 62], 'E7')
    ];

    const drums = createTrackPreset('drums', 8, 4, 4);
    drums.measures.forEach((m, i) => {
      m.drumBlockId = (i === 7) ? 'rock_8th' : 'blues_shuffle';
    });

    return {
      title: 'Blues Shuffle w A',
      artist: 'Apka Gitarowa Demo',
      album: 'Studio Sessions',
      tempoBpm: 96,
      keyRootPc: 9,
      keyScaleId: 'blues',
      metronomePattern: normalizeMetronomePattern(null, 4),
      tracks: [g1, bass, keys, drums]
    };
  }

  // Domyślny preset: 'rock_riff_em' — 8-taktowy riff E moll (4 ścieżki: Gitara 1, Gitara 2, Bas 4-str., Perkusja)
  const g1 = createTrackPreset('guitar1', 8, 4, 4);
  g1.measures = [
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = 'Intro';
      m.repeatOpen = true;
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1, { chordSymbol: 'Em' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1, { chordSymbol: 'G5' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 5 }, { string: 1, fret: 7 }], 1, { chordSymbol: 'A5' }),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1, { chordSymbol: 'Em' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1, { chordSymbol: 'G5' }),
        nBeat([{ string: 1, fret: 2 }, { string: 2, fret: 4 }], 0, { chordSymbol: 'B5', vibrato: true }),
        nBeat([{ string: 1, fret: 0 }, { string: 2, fret: 2 }], 0, { chordSymbol: 'A5', slide: true })
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = 'Riff';
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1, { chordSymbol: 'Em' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 2 }, { string: 1, fret: 4 }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1, { chordSymbol: 'G5' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 5 }, { string: 1, fret: 7 }], 1, { chordSymbol: 'A5' }),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1),
        nBeat([{ string: 0, fret: 2 }, { string: 1, fret: 4 }], 1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.repeatClose = 2;
      m.beats = [
        nBeat([{ string: 1, fret: 3 }, { string: 2, fret: 5 }], 0, { chordSymbol: 'C5' }),
        nBeat([{ string: 1, fret: 5 }, { string: 2, fret: 7 }], 0, { chordSymbol: 'D5' }),
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }, { string: 2, fret: 2 }], -1, { chordSymbol: 'Em', letRing: true })
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = 'Refren';
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }, { string: 2, fret: 2 }], 0, { chordSymbol: 'Em' }),
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1),
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }, { string: 2, fret: 5 }], 0, { chordSymbol: 'G5' }),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 1, fret: 3 }, { string: 2, fret: 5 }, { string: 3, fret: 5 }], 0, { chordSymbol: 'C5' }),
        nBeat([{ string: 1, fret: 3 }, { string: 2, fret: 5 }], 1),
        nBeat([{ string: 1, fret: 3 }, { string: 2, fret: 5 }], 1),
        nBeat([{ string: 1, fret: 5 }, { string: 2, fret: 7 }, { string: 3, fret: 7 }], 0, { chordSymbol: 'D5' }),
        nBeat([{ string: 1, fret: 2 }, { string: 2, fret: 4 }], 0, { chordSymbol: 'B5' })
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }], 1, { chordSymbol: 'Em' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 5 }, { string: 1, fret: 7 }], 1, { chordSymbol: 'A5' }),
        nBeat([{ string: 0, fret: 0, palmMute: true }], 1),
        nBeat([{ string: 0, fret: 7 }, { string: 1, fret: 9 }], 1, { chordSymbol: 'B5' }),
        nBeat([{ string: 0, fret: 5 }, { string: 1, fret: 7 }], 1),
        nBeat([{ string: 0, fret: 3 }, { string: 1, fret: 5 }], 1, { chordSymbol: 'G5' }),
        nBeat([{ string: 0, fret: 2 }, { string: 1, fret: 4 }], 1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 0, fret: 0 }, { string: 1, fret: 2 }, { string: 2, fret: 2 }], -2, { chordSymbol: 'Em', letRing: true })
      ];
      return m;
    })()
  ];

  const g2 = createTrackPreset('guitar2', 8, 4, 4);
  g2.measures = [
    (() => { const m = createEmptyMeasure(4, 4); m.sectionMarker = 'Intro'; m.repeatOpen = true; m.beats = [rBeat(-2)]; return m; })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        rBeat(-1),
        nBeat([{ string: 3, fret: 12 }], 1),
        nBeat([{ string: 3, fret: 14, slide: true }], 1),
        nBeat([{ string: 4, fret: 12 }], 1),
        nBeat([{ string: 4, fret: 15, vibrato: true }], 1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = 'Riff';
      m.beats = [
        nBeat([{ string: 5, fret: 12, vibrato: true }], 0),
        nBeat([{ string: 4, fret: 15 }], 1),
        nBeat([{ string: 4, fret: 12, hammerOn: true }], 1),
        nBeat([{ string: 3, fret: 14 }], 1),
        nBeat([{ string: 3, fret: 12 }], 1),
        nBeat([{ string: 2, fret: 14, vibrato: true }], 0)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.repeatClose = 2;
      m.beats = [
        nBeat([{ string: 3, fret: 12 }], 1),
        nBeat([{ string: 3, fret: 14, hammerOn: true }], 1),
        nBeat([{ string: 4, fret: 12 }], 1),
        nBeat([{ string: 5, fret: 12 }], 1),
        nBeat([{ string: 5, fret: 15, bendPoints: [{ pos: 0, val: 0 }, { pos: 60, val: 100 }], vibrato: true }], -1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.sectionMarker = 'Refren';
      m.beats = [
        nBeat([{ string: 4, fret: 12 }], 0),
        nBeat([{ string: 5, fret: 12 }], 0),
        nBeat([{ string: 5, fret: 14, slide: true }], 0),
        nBeat([{ string: 5, fret: 15, vibrato: true }], 0)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 5, fret: 17 }], 0),
        nBeat([{ string: 5, fret: 15 }], 1),
        nBeat([{ string: 5, fret: 14 }], 1),
        nBeat([{ string: 4, fret: 15 }], 0),
        nBeat([{ string: 4, fret: 12, vibrato: true }], 0)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 3, fret: 12, harmonicType: 1 }], 0),
        nBeat([{ string: 4, fret: 12, harmonicType: 1 }], 0),
        nBeat([{ string: 5, fret: 12, harmonicType: 1 }], -1)
      ];
      return m;
    })(),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 5, fret: 12, vibrato: true }], -2)
      ];
      return m;
    })()
  ];

  const bass = createTrackPreset('bass', 8, 4, 4);
  const bassBar = (str, f1, f2, sec = '', ro = false, rc = 0) => {
    const m = createEmptyMeasure(4, 4);
    m.sectionMarker = sec;
    m.repeatOpen = ro;
    m.repeatClose = rc;
    m.beats = [
      nBeat([{ string: str, fret: f1 }], 1),
      nBeat([{ string: str, fret: f1 }], 1),
      nBeat([{ string: str, fret: f1 }], 1),
      nBeat([{ string: str, fret: f2 }], 1),
      nBeat([{ string: str, fret: f1 }], 1),
      nBeat([{ string: str, fret: f1 }], 1),
      nBeat([{ string: str, fret: f2 }], 1),
      nBeat([{ string: str, fret: f1 }], 1)
    ];
    return m;
  };
  bass.measures = [
    bassBar(0, 0, 3, 'Intro', true, 0),
    bassBar(0, 0, 5),
    bassBar(0, 0, 3, 'Riff'),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.repeatClose = 2;
      m.beats = [
        nBeat([{ string: 1, fret: 3 }], 0),
        nBeat([{ string: 1, fret: 5 }], 0),
        nBeat([{ string: 0, fret: 0 }], -1)
      ];
      return m;
    })(),
    bassBar(0, 0, 3, 'Refren'),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [
        nBeat([{ string: 1, fret: 3 }], 0),
        nBeat([{ string: 1, fret: 3 }], 0),
        nBeat([{ string: 1, fret: 5 }], 0),
        nBeat([{ string: 1, fret: 2 }], 0)
      ];
      return m;
    })(),
    bassBar(0, 0, 7),
    (() => {
      const m = createEmptyMeasure(4, 4);
      m.beats = [nBeat([{ string: 0, fret: 0 }], -2)];
      return m;
    })()
  ];

  const drums = createTrackPreset('drums', 8, 4, 4);
  drums.drumAutoFill = 4;
  drums.drumCrashOnOne = true;
  drums.measures.forEach((m, i) => {
    if (i === 0) { m.sectionMarker = 'Intro'; m.repeatOpen = true; }
    if (i === 2) m.sectionMarker = 'Riff';
    if (i === 3) m.repeatClose = 2;
    if (i === 4) m.sectionMarker = 'Refren';
    m.drumBlockId = i >= 4 ? 'hard_rock' : 'rock_8th';
    if (i === 7) m.drumBlockId = 'half_time';
  });

  return {
    title: 'Neon Riff w E moll',
    artist: 'Apka Gitarowa Demo',
    album: 'Studio Sessions',
    tempoBpm: 118,
    keyRootPc: 4,
    keyScaleId: 'natural_minor',
    metronomePattern: normalizeMetronomePattern(null, 4),
    tracks: [g1, g2, bass, drums]
  };
}

// ============================================================================
// Faza 2: Edycja nut, techniki, struktura, przekształcenia i schowek bez modali
// ============================================================================

export function getBeatAt(song, trackIdx, measureIdx, beatIdx) {
  return song?.tracks?.[trackIdx]?.measures?.[measureIdx]?.beats?.[beatIdx] || null;
}

export function getNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || beat.isRest || !Array.isArray(beat.notes)) return null;
  if (tr.instrumentType === 'keys') return stringIdx < 0 ? null : (beat.notes[stringIdx] || beat.notes[0] || null);
  return beat.notes.find(n => n.string === stringIdx) || null;
}

export function findFretCandidatesForMidi(track, midi, minFret = 0, maxFret = 24) {
  if (!track || !Array.isArray(track.openStringMidis) || track.openStringMidis.length === 0) return [];
  const capo = Number(track.capo) || 0;
  const target = Math.round(midi);
  const lo = Math.max(0, Number(minFret) || 0);
  const hi = Math.min(24, Number.isFinite(Number(maxFret)) ? Number(maxFret) : 24);
  const out = [];
  for (let s = 0; s < track.openStringMidis.length; s++) {
    const fret = target - (track.openStringMidis[s] + capo);
    if (fret >= lo && fret <= hi) {
      out.push({ string: s, fret, midi: target });
    }
  }
  out.sort((a, b) => (a.fret - b.fret) || (a.string - b.string));
  return out;
}

export function setNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx, fretOrMidi, extraOpts = {}) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || tr.instrumentType === 'drums' || tr.isPercussion) return null;

  if (tr.instrumentType === 'keys') {
    const midi = Math.max(21, Math.min(108, Math.round(fretOrMidi)));
    const note = createTabNote({ string: 0, fret: 0, midi, ...extraOpts });
    if (extraOpts.chord) {
      beat.notes = beat.notes.filter(n => n.midi !== midi);
      beat.notes.push(note);
      beat.notes.sort((a, b) => (a.midi || 0) - (b.midi || 0));
    } else {
      beat.notes = [note];
    }
    beat.isRest = false;
    return note;
  }

  const nStrings = tr.openStringMidis?.length || tr.stringCount || 6;
  const s = Math.max(0, Math.min(nStrings - 1, Math.round(stringIdx)));
  const f = Math.max(0, Math.min(24, Math.round(fretOrMidi)));
  const existing = beat.notes.find(n => n.string === s) || {};
  const note = createTabNote({ ...existing, ...extraOpts, string: s, fret: f });
  beat.notes = beat.notes.filter(n => n.string !== s);
  beat.notes.push(note);
  beat.notes.sort((a, b) => a.string - b.string);
  beat.isRest = false;
  return note;
}

export function insertMidiNoteAt(song, trackIdx, measureIdx, beatIdx, midi, preferredString = null) {
  const tr = song?.tracks?.[trackIdx];
  if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return { inserted: false };
  if (tr.instrumentType === 'keys') {
    const note = setNoteAt(song, trackIdx, measureIdx, beatIdx, 0, midi);
    return note ? { inserted: true, string: 0, fret: 0, midi: note.midi } : { inserted: false };
  }
  const candidates = findFretCandidatesForMidi(tr, midi, 0, 24);
  if (candidates.length === 0) return { inserted: false };
  const chosen = (preferredString !== null && preferredString !== undefined)
    ? (candidates.find(c => c.string === preferredString) || candidates[0])
    : candidates[0];
  setNoteAt(song, trackIdx, measureIdx, beatIdx, chosen.string, chosen.fret);
  return { inserted: true, string: chosen.string, fret: chosen.fret, midi: chosen.midi };
}

export function insertBeatAfter(song, trackIdx, measureIdx, beatIdx, duration = 0, isRest = true) {
  const m = song?.tracks?.[trackIdx]?.measures?.[measureIdx];
  if (!m || !Array.isArray(m.beats)) return -1;
  const pos = Math.max(0, Math.min(m.beats.length, (Number(beatIdx) || 0) + 1));
  m.beats.splice(pos, 0, createEmptyBeat(duration, isRest));
  return pos;
}

export function setBeatRest(song, trackIdx, measureIdx, beatIdx) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  beat.notes = [];
  beat.isRest = true;
  return true;
}

export function deleteNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || beat.isRest) return false;
  if (tr.instrumentType === 'keys') {
    beat.notes = [];
    beat.isRest = true;
    return true;
  }
  const prevLen = beat.notes.length;
  beat.notes = beat.notes.filter(n => n.string !== stringIdx);
  if (beat.notes.length === 0) beat.isRest = true;
  return beat.notes.length !== prevLen;
}

export function deleteBeatAt(song, trackIdx, measureIdx, beatIdx) {
  const m = song?.tracks?.[trackIdx]?.measures?.[measureIdx];
  if (!m || !Array.isArray(m.beats) || beatIdx < 0 || beatIdx >= m.beats.length) return -1;
  m.beats.splice(beatIdx, 1);
  if (m.beats.length === 0) {
    const restDur = (m.denominator === 16) ? 2 : (m.denominator === 8 ? 1 : (m.denominator === 2 ? -1 : 0));
    m.beats.push(createEmptyBeat(restDur, true));
  }
  return Math.max(0, Math.min(m.beats.length - 1, beatIdx));
}

export function clearMeasureAt(song, trackIdx, measureIdx) {
  const tr = song?.tracks?.[trackIdx];
  const m = tr?.measures?.[measureIdx];
  if (!tr || !m) return false;
  if (tr.instrumentType === 'drums' || tr.isPercussion) {
    m.drumBlockId = 'break';
    m.customDrumGrid = null;
    return true;
  }
  const fresh = createEmptyMeasure(m.numerator, m.denominator, '', false);
  m.beats = fresh.beats;
  return true;
}

export function setBeatDuration(song, trackIdx, measureIdx, beatIdx, duration) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  beat.duration = Math.max(-2, Math.min(4, Math.round(Number(duration) || 0)));
  return true;
}

/**
 * Zmienia długość nuty o jeden stopień: delta > 0 wydłuża nutę (duration - 1), delta < 0 skraca (duration + 1).
 */
export function stepBeatDuration(song, trackIdx, measureIdx, beatIdx, delta) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  const next = delta > 0 ? beat.duration - 1 : beat.duration + 1;
  beat.duration = Math.max(-2, Math.min(4, next));
  return true;
}

export function cycleBeatDots(song, trackIdx, measureIdx, beatIdx, exactDots = null) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  if (exactDots === 1 || exactDots === 2) {
    beat.dots = (beat.dots === exactDots) ? 0 : exactDots;
  } else {
    beat.dots = ((beat.dots || 0) + 1) % 3;
  }
  return true;
}

export function setBeatTuplet(song, trackIdx, measureIdx, beatIdx, num = 3, den = 2) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  if (beat.tupletNumerator === num && beat.tupletDenominator === den) {
    beat.tupletNumerator = 0;
    beat.tupletDenominator = 0;
  } else {
    beat.tupletNumerator = num;
    beat.tupletDenominator = den;
  }
  return true;
}

export function setBeatDynamic(song, trackIdx, measureIdx, beatIdx, dynVal) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  beat.dynamic = Math.max(1, Math.min(7, Math.round(Number(dynVal) || 6)));
  return true;
}

export function toggleBeatProp(song, trackIdx, measureIdx, beatIdx, prop, val = undefined) {
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!beat) return false;
  if (prop === 'fermata') {
    beat.fermata = !beat.fermata;
    return true;
  }
  if (prop === 'pickStroke') {
    const target = Number(val) || 1;
    beat.pickStroke = (beat.pickStroke === target) ? 0 : target;
    return true;
  }
  if (prop === 'brushStroke') {
    const target = String(val || 'down');
    beat.brushStroke = (beat.brushStroke === target) ? '' : target;
    return true;
  }
  if (prop === 'chordSymbol') {
    beat.chordSymbol = String(val ?? '').trim();
    return true;
  }
  if (prop === 'text') {
    beat.text = String(val ?? '').trim();
    return true;
  }
  return false;
}

export function toggleNoteEffect(song, trackIdx, measureIdx, beatIdx, stringIdx, effect, value = undefined) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || beat.isRest || !Array.isArray(beat.notes) || beat.notes.length === 0) return false;

  const exact = getNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx);
  const targets = exact ? [exact] : beat.notes;
  const ref = targets[0];

  if (effect === 'harmonicType') {
    const nextType = (ref.harmonicType === (Number(value) || 1)) ? 0 : (Number(value) || 1);
    targets.forEach(n => { n.harmonicType = nextType; });
    return true;
  }
  if (effect === 'slide' || effect === 'slideType') {
    const targetType = String(value || 'shift');
    const nextType = (ref.slideType === targetType) ? '' : targetType;
    targets.forEach(n => {
      n.slideType = nextType;
      n.slide = Boolean(nextType);
    });
    return true;
  }
  if (effect === 'bendPreset') {
    const presetKey = String(value || 'full');
    const preset = BEND_PRESETS[presetKey] || BEND_PRESETS.full;
    const clear = (ref.bendPreset === preset.id) || (presetKey === 'clear');
    targets.forEach(n => {
      n.bendPreset = clear ? '' : preset.id;
      n.bendPoints = clear ? [] : preset.points.map(p => ({ ...p }));
    });
    return true;
  }
  if (effect === 'whammyPreset') {
    const presetKey = String(value || 'dive_full');
    const preset = WHAMMY_PRESETS[presetKey] || WHAMMY_PRESETS.dive_full;
    const clear = (ref.whammyPreset === preset.id) || (presetKey === 'clear');
    targets.forEach(n => {
      n.whammyPreset = clear ? '' : preset.id;
      n.whammyPoints = clear ? [] : preset.points.map(p => ({ ...p }));
    });
    return true;
  }

  const boolEffects = new Set([
    'tied', 'palmMute', 'letRing', 'hammerOn', 'vibrato', 'wideVibrato',
    'deadNote', 'ghostNote', 'accented', 'heavyAccented', 'staccato',
    'tap', 'graceNote', 'tremoloPicking', 'trill'
  ]);
  if (!boolEffects.has(effect)) return false;
  const nextVal = !Boolean(ref[effect]);
  targets.forEach(n => { n[effect] = nextVal; });
  return true;
}

export function toggleMeasureRepeatOpen(song, measureIdx) {
  if (!song?.tracks?.length) return false;
  const cur = Boolean(song.tracks[0]?.measures?.[measureIdx]?.repeatOpen);
  const next = !cur;
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].repeatOpen = next;
  }
  return true;
}

export function setMeasureRepeatClose(song, measureIdx, count = 2) {
  if (!song?.tracks?.length) return false;
  const cur = Number(song.tracks[0]?.measures?.[measureIdx]?.repeatClose) || 0;
  const req = Number(count) || 0;
  const next = (req <= 1 || req === cur) ? 0 : Math.max(2, Math.min(8, Math.round(req)));
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].repeatClose = next;
  }
  return true;
}

export function toggleMeasureVolta(song, measureIdx, voltaNum = 1) {
  if (!song?.tracks?.length) return false;
  const bit = 1 << Math.max(0, Math.min(7, (Number(voltaNum) || 1) - 1));
  const cur = Number(song.tracks[0]?.measures?.[measureIdx]?.voltaMask) || 0;
  const next = (cur & bit) ? (cur & ~bit) : (cur | bit);
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].voltaMask = next;
  }
  return true;
}

export function setMeasureSectionMarker(song, measureIdx, text) {
  if (!song?.tracks?.length) return false;
  const clean = String(text || '').trim();
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].sectionMarker = clean;
  }
  return true;
}

export function toggleMeasurePickup(song, measureIdx) {
  if (!song?.tracks?.length) return false;
  const cur = Boolean(song.tracks[0]?.measures?.[measureIdx]?.isPickup);
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].isPickup = !cur;
  }
  return true;
}

export function setMeasureTripletFeel(song, measureIdx, feel = 0) {
  if (!song?.tracks?.length) return false;
  const next = Math.max(0, Math.min(2, Number(feel) || 0));
  for (const tr of song.tracks) {
    if (tr.measures?.[measureIdx]) tr.measures[measureIdx].tripletFeel = next;
  }
  return true;
}

export function tieOrExtendPreviousNote(song, trackIdx, measureIdx, beatIdx, stringIdx = 0, defaultDuration = null) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || tr.instrumentType === 'drums' || tr.isPercussion) {
    return { changed: false, extended: false, reason: 'invalid' };
  }

  const existing = getNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx);
  if (existing) {
    existing.tied = !existing.tied;
    return { changed: true, extended: false, tied: Boolean(existing.tied) };
  }

  // Znajdź bezpośrednio poprzednie uderzenie (w obrębie taktu lub z końca poprzedniego taktu)
  let prevBeat = null;
  if (beatIdx > 0) {
    prevBeat = getBeatAt(song, trackIdx, measureIdx, beatIdx - 1);
  } else if (measureIdx > 0) {
    const prevM = tr.measures?.[measureIdx - 1];
    if (prevM && Array.isArray(prevM.beats) && prevM.beats.length > 0) {
      prevBeat = prevM.beats[prevM.beats.length - 1];
    }
  }
  if (!prevBeat || prevBeat.isRest || !Array.isArray(prevBeat.notes) || prevBeat.notes.length === 0) {
    return { changed: false, extended: false, reason: 'prev_rest' };
  }

  if (beat.isRest && Number.isInteger(defaultDuration)) {
    beat.duration = Math.max(-2, Math.min(4, defaultDuration));
  }

  if (tr.instrumentType === 'keys') {
    let added = 0;
    for (const pn of prevBeat.notes) {
      const midi = noteToMidi(tr, pn);
      if (midi !== null) {
        setNoteAt(song, trackIdx, measureIdx, beatIdx, 0, midi, { chord: added > 0, tied: true });
        added++;
      }
    }
    return added > 0 ? { changed: true, extended: true, tied: true } : { changed: false, extended: false, reason: 'no_notes' };
  }

  const sameStringPrev = prevBeat.notes.find(n => n.string === stringIdx);
  if (sameStringPrev) {
    setNoteAt(song, trackIdx, measureIdx, beatIdx, sameStringPrev.string, sameStringPrev.fret, { tied: true });
    return { changed: true, extended: true, tied: true };
  }

  if (beat.isRest) {
    for (const pn of prevBeat.notes) {
      setNoteAt(song, trackIdx, measureIdx, beatIdx, pn.string, pn.fret, { tied: true });
    }
    return { changed: true, extended: true, tied: true };
  }

  return { changed: false, extended: false, reason: 'no_matching_string' };
}

export function addMeasureBefore(song, measureIdx, defaultBeatDuration = null) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length === 0) return -1;
  const maxM = getMasterMeasureCount(song);
  const insertPos = Math.max(0, Math.min(maxM, Number(measureIdx) || 0));
  const refM = song.tracks[0]?.measures[insertPos] || { numerator: 4, denominator: 4 };
  for (const tr of song.tracks) {
    const isDrum = tr.instrumentType === 'drums' || tr.isPercussion;
    const prevBlock = tr.measures[insertPos]?.drumBlockId || 'rock_8th';
    tr.measures.splice(insertPos, 0, createEmptyMeasure(refM.numerator, refM.denominator, prevBlock, isDrum, defaultBeatDuration));
  }
  return insertPos;
}

/**
 * Przenosi nutę na wyższą (dir=+1) lub niższą (dir=-1) strunę bez zmiany wysokości MIDI (Alt+↑/↓).
 */
export function moveNoteToAdjacentString(song, trackIdx, measureIdx, beatIdx, stringIdx, dir) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || beat.isRest || !Array.isArray(tr.openStringMidis) || tr.openStringMidis.length === 0) {
    return { moved: false, reason: 'no-string-track' };
  }
  const note = beat.notes.find(n => n.string === stringIdx);
  if (!note) return { moved: false, reason: 'no-note' };

  const newString = stringIdx + (dir >= 0 ? 1 : -1);
  if (newString < 0 || newString >= tr.openStringMidis.length) {
    return { moved: false, reason: 'out-of-strings' };
  }
  if (beat.notes.some(n => n !== note && n.string === newString)) {
    return { moved: false, reason: 'string-occupied' };
  }
  const midi = noteToMidi(tr, note);
  if (midi === null) return { moved: false, reason: 'invalid-midi' };
  const newFret = midi - (tr.openStringMidis[newString] + (tr.capo || 0));
  if (newFret < 0 || newFret > 24) {
    return { moved: false, reason: 'out-of-frets' };
  }
  note.string = newString;
  note.fret = newFret;
  beat.notes.sort((a, b) => a.string - b.string);
  return { moved: true, newString, newFret, midi };
}

/**
 * Przesuwa wysokość nuty pod kursorem (lub całego uderzenia) o `semitones` półtonów (Shift+strzałki).
 */
export function shiftNotePitch(song, trackIdx, measureIdx, beatIdx, stringIdx, semitones) {
  const tr = song?.tracks?.[trackIdx];
  const beat = getBeatAt(song, trackIdx, measureIdx, beatIdx);
  if (!tr || !beat || beat.isRest || !beat.notes.length || tr.instrumentType === 'drums') {
    return false;
  }
  const exact = getNoteAt(song, trackIdx, measureIdx, beatIdx, stringIdx);
  const targets = exact
    ? [exact]
    : beat.notes.slice().sort((a, b) => (semitones > 0 ? (b.string - a.string) : (a.string - b.string)));
  let changed = false;

  if (tr.instrumentType === 'keys') {
    for (const n of targets) {
      const next = Math.max(21, Math.min(108, (n.midi ?? 60) + semitones));
      if (next !== n.midi) { n.midi = next; changed = true; }
    }
    return changed;
  }

  const capo = Number(tr.capo) || 0;
  for (const n of targets) {
    const curMidi = noteToMidi(tr, n);
    if (curMidi === null) continue;
    const targetMidi = curMidi + semitones;
    const sameStringFret = targetMidi - (tr.openStringMidis[n.string] + capo);
    if (sameStringFret >= 0 && sameStringFret <= 24) {
      n.fret = sameStringFret;
      changed = true;
    } else {
      const cands = findFretCandidatesForMidi(tr, targetMidi, 0, 24)
        .filter(c => !beat.notes.some(other => other !== n && other.string === c.string));
      if (cands.length > 0) {
        cands.sort((a, b) => Math.abs(a.string - n.string) - Math.abs(b.string - n.string) || a.fret - b.fret);
        n.string = cands[0].string;
        n.fret = cands[0].fret;
        changed = true;
      }
    }
  }
  beat.notes.sort((a, b) => a.string - b.string);
  return changed;
}

export function moveBeatInMeasure(song, trackIdx, measureIdx, beatIdx, delta) {
  const m = song?.tracks?.[trackIdx]?.measures?.[measureIdx];
  if (!m || !Array.isArray(m.beats)) return beatIdx;
  const target = beatIdx + (delta >= 0 ? 1 : -1);
  if (beatIdx < 0 || beatIdx >= m.beats.length || target < 0 || target >= m.beats.length) return beatIdx;
  const tmp = m.beats[beatIdx];
  m.beats[beatIdx] = m.beats[target];
  m.beats[target] = tmp;
  return target;
}

export function transposeRange(song, { trackIdx = 0, allTracks = false, startMeasure = 0, endMeasure = 0, semitones = 0 } = {}) {
  if (!song || !Array.isArray(song.tracks) || !semitones) return 0;
  const maxM = getMasterMeasureCount(song) - 1;
  const s = Math.max(0, Math.min(startMeasure, endMeasure));
  const e = Math.min(maxM, Math.max(startMeasure, endMeasure));
  const list = allTracks ? song.tracks.map((_, idx) => idx) : [trackIdx];
  let count = 0;

  for (const tIdx of list) {
    const tr = song.tracks[tIdx];
    if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) continue;
    for (let mIdx = s; mIdx <= e && mIdx < tr.measures.length; mIdx++) {
      const beats = tr.measures[mIdx]?.beats || [];
      for (let bIdx = 0; bIdx < beats.length; bIdx++) {
        if (!beats[bIdx].isRest && beats[bIdx].notes.length > 0) {
          if (shiftNotePitch(song, tIdx, mIdx, bIdx, -1, semitones)) count++;
        }
      }
    }
  }
  return count;
}

export function copyMeasuresToClipboard(song, { startIdx = 0, endIdx = 0, scope = 'track', activeTrackIdx = 0 } = {}) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length === 0) return null;
  const maxM = getMasterMeasureCount(song) - 1;
  const s = Math.max(0, Math.min(startIdx, endIdx));
  const e = Math.min(maxM, Math.max(startIdx, endIdx));
  if (s > e) return null;

  const isAll = scope === 'all';
  const indices = isAll ? song.tracks.map((_, i) => i) : [Math.max(0, Math.min(song.tracks.length - 1, activeTrackIdx))];
  return {
    kind: 'measures',
    scope: isAll ? 'all' : 'track',
    sourceTrackIdx: activeTrackIdx,
    barCount: e - s + 1,
    measuresByTrack: indices.map(tIdx => ({
      trackIdx: tIdx,
      measures: song.tracks[tIdx].measures.slice(s, e + 1).map(m => cloneSong(m))
    }))
  };
}

export function cutMeasuresToClipboard(song, opts = {}) {
  const clip = copyMeasuresToClipboard(song, opts);
  if (!clip) return null;
  const maxM = getMasterMeasureCount(song) - 1;
  const s = Math.max(0, Math.min(opts.startIdx || 0, opts.endIdx || 0));
  const e = Math.min(maxM, Math.max(opts.startIdx || 0, opts.endIdx || 0));
  const indices = clip.scope === 'all'
    ? song.tracks.map((_, i) => i)
    : [Math.max(0, Math.min(song.tracks.length - 1, opts.activeTrackIdx || 0))];
  for (const tIdx of indices) {
    for (let mIdx = s; mIdx <= e; mIdx++) {
      clearMeasureAt(song, tIdx, mIdx);
    }
  }
  return clip;
}

export function pasteMeasuresFromClipboard(song, clipboard, { targetMeasureIdx = 0, mode = 'overwrite', scope = null, activeTrackIdx = 0 } = {}) {
  if (!song || !clipboard || !Array.isArray(clipboard.measuresByTrack) || clipboard.measuresByTrack.length === 0) {
    return false;
  }
  const effScope = scope || clipboard.scope || 'track';
  const count = clipboard.barCount || clipboard.measuresByTrack[0].measures.length;
  const targetStart = Math.max(0, Number(targetMeasureIdx) || 0);

  if (mode === 'insert') {
    const insertPos = Math.min(getMasterMeasureCount(song), targetStart + 1);
    const refM = song.tracks[0]?.measures[Math.max(0, insertPos - 1)] || { numerator: 4, denominator: 4 };
    song.tracks.forEach((tr, tIdx) => {
      let srcList = null;
      if (effScope === 'all') {
        const entry = clipboard.measuresByTrack.find(e => e.trackIdx === tIdx) || clipboard.measuresByTrack[0];
        srcList = entry?.measures;
      } else if (tIdx === activeTrackIdx) {
        srcList = clipboard.measuresByTrack[0]?.measures;
      }
      const isDrum = tr.instrumentType === 'drums' || tr.isPercussion;
      const toInsert = [];
      for (let i = 0; i < count; i++) {
        if (srcList && srcList[i]) {
          const cm = cloneSong(srcList[i]);
          if (isDrum && !cm.drumBlockId) cm.drumBlockId = 'rock_8th';
          if (!isDrum && (!cm.beats || cm.beats.length === 0)) {
            cm.beats = createEmptyMeasure(cm.numerator, cm.denominator).beats;
          }
          toInsert.push(cm);
        } else {
          toInsert.push(createEmptyMeasure(refM.numerator, refM.denominator, 'rock_8th', isDrum));
        }
      }
      tr.measures.splice(insertPos, 0, ...toInsert);
    });
    return true;
  }

  // Tryb 'overwrite'
  while (getMasterMeasureCount(song) < targetStart + count) {
    addMeasure(song);
  }
  song.tracks.forEach((tr, tIdx) => {
    let srcList = null;
    if (effScope === 'all') {
      const entry = clipboard.measuresByTrack.find(e => e.trackIdx === tIdx) || clipboard.measuresByTrack[0];
      srcList = entry?.measures;
    } else if (tIdx === activeTrackIdx) {
      srcList = clipboard.measuresByTrack[0]?.measures;
    }
    if (!srcList) return;
    const isDrum = tr.instrumentType === 'drums' || tr.isPercussion;
    for (let i = 0; i < count; i++) {
      if (!srcList[i]) continue;
      const cm = cloneSong(srcList[i]);
      if (isDrum) {
        tr.measures[targetStart + i].drumBlockId = cm.drumBlockId || 'rock_8th';
        tr.measures[targetStart + i].customDrumGrid = cm.customDrumGrid ? cloneSong(cm.customDrumGrid) : null;
      } else {
        if (!Array.isArray(cm.beats) || cm.beats.length === 0) {
          cm.beats = createEmptyMeasure(cm.numerator, cm.denominator).beats;
        }
        const nStr = tr.instrumentType === 'keys' ? 0 : (tr.openStringMidis?.length || tr.stringCount || 6);
        if (nStr > 0) {
          for (const b of cm.beats) {
            if (!Array.isArray(b.notes)) continue;
            const seen = new Set();
            b.notes = b.notes.filter(n => {
              const s = Math.max(0, Math.min(nStr - 1, Number(n.string) || 0));
              if (seen.has(s)) return false;
              n.string = s;
              seen.add(s);
              return true;
            });
          }
        }
        tr.measures[targetStart + i] = cm;
      }
    }
  });
  return true;
}

const KS_MAJOR_PROFILE = [6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88];
const KS_MINOR_PROFILE = [6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17];

export function detectKeyFromSongJs(song, accMode = 2) {
  const weights = new Array(12).fill(0);
  let total = 0;
  for (const tr of (song?.tracks || [])) {
    if (tr.instrumentType === 'drums' || tr.isPercussion) continue;
    for (const m of (tr.measures || [])) {
      for (const b of (m.beats || [])) {
        if (b.isRest || !Array.isArray(b.notes)) continue;
        const w = durationToQuarterNotes(b);
        for (const n of b.notes) {
          const midi = noteToMidi(tr, n);
          if (midi === null) continue;
          const pc = ((midi % 12) + 12) % 12;
          weights[pc] += w;
          total += w;
        }
      }
    }
  }
  if (total <= 0) {
    return { rootPc: song?.keyRootPc ?? 4, scaleId: song?.keyScaleId || 'natural_minor', label: 'Brak nut' };
  }

  let bestScore = -Infinity;
  let bestRoot = 0;
  let bestScale = 'major';
  for (let r = 0; r < 12; r++) {
    let majScore = 0;
    let minScore = 0;
    for (let i = 0; i < 12; i++) {
      const val = weights[(r + i) % 12];
      majScore += val * KS_MAJOR_PROFILE[i];
      minScore += val * KS_MINOR_PROFILE[i];
    }
    if (majScore > bestScore) { bestScore = majScore; bestRoot = r; bestScale = 'major'; }
    if (minScore > bestScore) { bestScore = minScore; bestRoot = r; bestScale = 'natural_minor'; }
  }
  const rootName = midiToNoteName(bestRoot, accMode);
  const scaleName = bestScale === 'major' ? 'dur' : 'moll';
  return { rootPc: bestRoot, scaleId: bestScale, label: `${rootName} ${scaleName}` };
}

/**
 * Przestawia wybraną nutę w uderzeniu na konkretną parę { string, fret } zachowując efekty.
 */
export function setNotePosition(song, trackIdx, measureIdx, beatIdx, fromString, toString, toFret) {
  if (!song || !Array.isArray(song.tracks)) return false;
  const tr = song.tracks[trackIdx];
  const m = tr?.measures?.[measureIdx];
  const beat = m?.beats?.[beatIdx];
  if (!tr || !beat || beat.isRest || !Array.isArray(beat.notes)) return false;
  const maxStr = Math.max(0, (tr.openStringMidis?.length || 6) - 1);
  const sTo = Math.max(0, Math.min(maxStr, Math.round(toString)));
  const fTo = Math.max(0, Math.min(24, Math.round(toFret)));

  const srcIdx = beat.notes.findIndex(n => n.string === fromString);
  if (srcIdx < 0) return false;
  const srcNote = beat.notes[srcIdx];
  const cloned = { ...srcNote, string: sTo, fret: fTo };
  beat.notes = beat.notes.filter((n, idx) => idx !== srcIdx && n.string !== sTo);
  beat.notes.push(cloned);
  beat.notes.sort((a, b) => a.string - b.string);
  return true;
}

/**
 * Szuka bezkolizyjnego przypisania wszystkich nut pojedynczego uderzenia do osobnych strun w zakresie gryfu.
 * Zwraca null, jeśli dowolna nuta nie mieści się w [minFret..maxFret] lub dwie nuty musiałyby zająć tę samą strunę.
 */
export function solveBeatFretAssignments(track, beatNotes, midiShift = 0, opts = {}) {
  if (!track || !Array.isArray(beatNotes) || beatNotes.length === 0) return null;
  if (!Array.isArray(track.openStringMidis) || track.openStringMidis.length === 0) return null;

  const minFret = Math.max(0, Math.min(24, Math.round(Number(opts.minFret) || 0)));
  const maxFret = Math.max(minFret, Math.min(24, Math.round(Number(opts.maxFret) ?? 24)));
  const boxStart = Math.max(minFret, Math.min(maxFret, Math.round(Number(opts.boxStart) || 0)));
  const boxEnd = Math.max(boxStart, Math.min(maxFret, Math.round(Number(opts.boxEnd) ?? maxFret)));
  const prevFret = Number.isFinite(Number(opts.prevFret)) ? Number(opts.prevFret) : null;
  const prevString = Number.isFinite(Number(opts.prevString)) ? Number(opts.prevString) : null;
  const midF = (boxStart + boxEnd) / 2;

  const entries = [];
  for (let i = 0; i < beatNotes.length; i++) {
    const n = beatNotes[i];
    const baseMidi = Number.isFinite(Number(n.midi)) && !Array.isArray(track.openStringMidis)
      ? Math.round(Number(n.midi))
      : noteToMidi(track, n);
    if (baseMidi === null || !Number.isFinite(baseMidi)) return null;
    const targetMidi = baseMidi + midiShift;
    const rawCands = findFretCandidatesForMidi(track, targetMidi, minFret, maxFret);
    if (rawCands.length === 0) return null;
    const orderedCands = rawCands.slice().sort((a, b) => {
      const aIn = (a.fret >= boxStart && a.fret <= boxEnd) || (a.fret === 0 && boxStart <= 2);
      const bIn = (b.fret >= boxStart && b.fret <= boxEnd) || (b.fret === 0 && boxStart <= 2);
      if (aIn !== bIn) return aIn ? -1 : 1;
      return Math.abs(a.fret - midF) - Math.abs(b.fret - midF) || a.fret - b.fret;
    });
    entries.push({ origIdx: i, note: n, targetMidi, cands: orderedCands });
  }

  entries.sort((a, b) => a.cands.length - b.cands.length || a.origIdx - b.origIdx);

  let bestScore = Infinity;
  let bestPicked = null;
  let visited = 0;
  const current = new Array(entries.length);
  const usedStrings = new Set();

  function backtrack(depth) {
    if (visited++ > 2500) return;
    if (depth === entries.length) {
      let outCount = 0;
      let boxDist = 0;
      let sumFret = 0;
      let sumStr = 0;
      let nonOpenMin = 99;
      let nonOpenMax = -1;
      let shapeBias = 0;

      for (let k = 0; k < current.length; k++) {
        const pick = current[k];
        const f = pick.cand.fret;
        const s = pick.cand.string;
        const inBox = (f >= boxStart && f <= boxEnd);
        const openShortcut = (f === 0 && boxStart <= 2);
        if (!inBox && !openShortcut) {
          outCount++;
          boxDist += Math.abs(f - midF) * 4.5 + 12;
        } else if (inBox) {
          boxDist += Math.abs(f - midF) * 0.35;
        } else {
          boxDist += 0.5;
        }
        if (f > 0) {
          if (f < nonOpenMin) nonOpenMin = f;
          if (f > nonOpenMax) nonOpenMax = f;
        }
        if (s === pick.note.string) shapeBias -= 0.25;
        sumFret += f;
        sumStr += s;
      }

      const chordSpan = nonOpenMax >= 0 ? (nonOpenMax - nonOpenMin) : 0;
      const stretchPenalty = chordSpan > 4 ? (chordSpan - 4) * 14 : 0;
      const avgF = sumFret / current.length;
      const avgS = sumStr / current.length;
      const jumpCost = (prevFret !== null && prevString !== null)
        ? (Math.abs(avgF - prevFret) + Math.abs(avgS - prevString) * 1.1)
        : 0;
      const score = outCount * 50 + stretchPenalty + boxDist + jumpCost + shapeBias;
      if (score < bestScore) {
        bestScore = score;
        bestPicked = current.slice();
      }
      return;
    }

    const item = entries[depth];
    for (let cIdx = 0; cIdx < item.cands.length; cIdx++) {
      const cand = item.cands[cIdx];
      if (usedStrings.has(cand.string)) continue;
      usedStrings.add(cand.string);
      current[depth] = { origIdx: item.origIdx, note: item.note, targetMidi: item.targetMidi, cand };
      backtrack(depth + 1);
      usedStrings.delete(cand.string);
    }
  }

  backtrack(0);
  if (!bestPicked) return null;

  bestPicked.sort((a, b) => a.origIdx - b.origIdx);
  let actMin = 99;
  let actMax = 0;
  let outOfBoxCount = 0;
  let sumF = 0;
  let sumS = 0;
  const assignments = bestPicked.map(p => {
    const f = p.cand.fret;
    const s = p.cand.string;
    if (f < actMin) actMin = f;
    if (f > actMax) actMax = f;
    if (!((f >= boxStart && f <= boxEnd) || (f === 0 && boxStart <= 2))) {
      outOfBoxCount++;
    }
    sumF += f;
    sumS += s;
    return {
      measureIdx: Number.isFinite(Number(p.note.measureIdx)) ? Number(p.note.measureIdx) : 0,
      beatIdx: Number.isFinite(Number(p.note.beatIdx)) ? Number(p.note.beatIdx) : 0,
      oldString: p.note.string,
      oldFret: p.note.fret,
      newString: s,
      newFret: f,
      midi: p.targetMidi
    };
  });

  return {
    assignments,
    outOfBoxCount,
    cost: bestScore,
    minFret: actMin <= 24 ? actMin : 0,
    maxFret: actMax,
    avgFret: sumF / bestPicked.length,
    avgStr: sumS / bestPicked.length
  };
}

/**
 * Analizuje bezkolizyjne pozycje ręki na gryfie (Pozycja 1, 2, 3...) dla sekwencji nut lub zaznaczonego fragmentu.
 */
export function analyzePhrasePositions(track, noteRefs = null, opts = {}) {
  if (!track || track.instrumentType === 'drums' || track.isPercussion) return [];
  if (!Array.isArray(track.openStringMidis) || track.openStringMidis.length === 0) return [];

  const maxFret = Math.max(12, Math.min(24, Math.round(Number(opts.maxFret) || 24)));
  const octaveShift = Math.round(Number(opts.octaveShift) || 0);
  const midiShift = octaveShift * 12;

  const beatMap = new Map();
  const pushItem = (mIdx, bIdx, refObj) => {
    const midi = Number.isFinite(refObj.midi) ? Math.round(refObj.midi) : noteToMidi(track, refObj);
    if (midi === null) return;
    const key = `${mIdx}:${bIdx}`;
    if (!beatMap.has(key)) beatMap.set(key, { measureIdx: mIdx, beatIdx: bIdx, notes: [] });
    beatMap.get(key).notes.push({
      measureIdx: mIdx,
      beatIdx: bIdx,
      string: refObj.string,
      fret: refObj.fret ?? 0,
      midi
    });
  };

  if (Array.isArray(noteRefs) && noteRefs.length > 0) {
    for (const ref of noteRefs) {
      const m = track.measures?.[ref.measureIdx];
      const b = m?.beats?.[ref.beatIdx];
      const n = b?.notes?.find(x => x.string === ref.string);
      pushItem(ref.measureIdx, ref.beatIdx, {
        string: ref.string,
        fret: n ? n.fret : (ref.fret ?? 0),
        midi: Number.isFinite(ref.midi) ? Math.round(ref.midi) : (n ? noteToMidi(track, n) : null)
      });
    }
  } else {
    (track.measures || []).forEach((m, mIdx) => {
      (m.beats || []).forEach((b, bIdx) => {
        if (b.isRest || !Array.isArray(b.notes)) return;
        for (const n of b.notes) {
          pushItem(mIdx, bIdx, n);
        }
      });
    });
  }

  const beatGroups = Array.from(beatMap.values());
  if (beatGroups.length === 0) return [];

  // Najpierw sprawdź twardą wykonalność każdego uderzenia w tej oktawie i zakresie gryfu
  for (const bg of beatGroups) {
    const feasible = solveBeatFretAssignments(track, bg.notes, midiShift, { minFret: 0, maxFret, boxStart: 0, boxEnd: maxFret });
    if (!feasible) return [];
  }

  const baseSpan = Math.max(3, Math.min(6, Math.round(Number(opts.boxSpan) || 4)));
  const maxOptions = Math.max(1, Math.min(8, Math.round(Number(opts.maxOptions) || 6)));
  const bySignature = new Map();

  const evaluateWindowSpan = (span) => {
    const maxStart = Math.max(0, maxFret - span);
    for (let startF = 0; startF <= maxStart; startF++) {
      const endF = Math.min(maxFret, startF + span);
      let prevFret = (startF + endF) / 2;
      let prevString = Math.min(2, Math.max(0, Math.floor((track.openStringMidis.length - 1) / 2)));
      let outOfBoxCount = 0;
      let cost = 0;
      let actualMin = 99;
      let actualMax = 0;
      const assignments = [];
      let failed = false;

      for (const bg of beatGroups) {
        const solved = solveBeatFretAssignments(track, bg.notes, midiShift, {
          minFret: 0,
          maxFret,
          boxStart: startF,
          boxEnd: endF,
          prevFret,
          prevString
        });
        if (!solved) {
          failed = true;
          break;
        }
        outOfBoxCount += solved.outOfBoxCount;
        cost += solved.cost;
        if (solved.minFret < actualMin) actualMin = solved.minFret;
        if (solved.maxFret > actualMax) actualMax = solved.maxFret;
        prevFret = solved.avgFret;
        prevString = solved.avgStr;
        for (const a of solved.assignments) assignments.push(a);
      }
      if (failed || assignments.length === 0) continue;

      const sig = assignments.map(a => `${a.newString}:${a.newFret}`).join('|');
      const areaMin = actualMin <= maxFret ? actualMin : startF;
      const areaMax = Math.max(areaMin, actualMax);
      const fretSpan = areaMax - areaMin;
      const prev = bySignature.get(sig);
      if (!prev || outOfBoxCount < prev.outOfBoxCount || (outOfBoxCount === prev.outOfBoxCount && cost < prev.cost)) {
        bySignature.set(sig, {
          minFret: areaMin,
          maxFret: areaMax,
          fretSpan,
          outOfBoxCount,
          cost,
          assignments
        });
      }
    }
  };

  evaluateWindowSpan(baseSpan);
  const hasZeroOut = Array.from(bySignature.values()).some(x => x.outOfBoxCount === 0);
  if (!hasZeroOut && baseSpan < 6) {
    evaluateWindowSpan(baseSpan + 1);
  }

  const allUnique = Array.from(bySignature.values());
  if (allUnique.length === 0) return [];
  const bestOut = Math.min(...allUnique.map(x => x.outOfBoxCount));
  let filtered = (bestOut === 0)
    ? allUnique.filter(x => x.outOfBoxCount === 0)
    : allUnique.filter(x => x.outOfBoxCount <= bestOut + Math.max(1, Math.floor(beatGroups.length * 0.25)));

  const minSpanFound = Math.min(...filtered.map(x => x.fretSpan));
  const tightFiltered = filtered.filter(x => x.fretSpan <= Math.max(baseSpan + 2, minSpanFound + 4));
  if (tightFiltered.length > 0) filtered = tightFiltered;

  filtered.sort((a, b) => a.minFret - b.minFret || a.maxFret - b.maxFret || a.outOfBoxCount - b.outOfBoxCount || a.cost - b.cost);

  return filtered.slice(0, maxOptions).map((pos, idx) => {
    const isCurrent = octaveShift === 0 && pos.assignments.every(a => a.newString === a.oldString && a.newFret === a.oldFret);
    const shortRange = `Pr. ${pos.minFret}–${pos.maxFret}`;
    const label = octaveShift === 0
      ? `Pozycja ${idx + 1} (${pos.minFret}–${pos.maxFret})`
      : `${octaveShift > 0 ? '+' : ''}${octaveShift} okt. (${pos.minFret}–${pos.maxFret})`;
    return {
      index: idx + 1,
      label,
      shortRange,
      minFret: pos.minFret,
      maxFret: pos.maxFret,
      octaveShift,
      semitones: midiShift,
      isCurrent,
      outOfBoxCount: pos.outOfBoxCount,
      cost: Math.round(pos.cost * 100) / 100,
      assignments: pos.assignments
    };
  });
}

/**
 * Dla zaznaczonego fragmentu wyznacza osobno:
 *  - możliwe pozycje na gryfie w TEJ SAMEJ oktawie (`sameOctave`),
 *  - wszystkie realnie wykonalne przeniesienia między OKTAWAMI (`octaveOptions` / `allOctavePositions`)
 *    przy aktualnym stroju, kapodastrze i maksymalnej liczbie progów.
 */
export function analyzeSelectionPositionsAndOctaves(song, trackIdx, selection, opts = {}) {
  const tr = song?.tracks?.[trackIdx];
  const maxFret = Math.max(12, Math.min(24, Math.round(Number(opts.maxFret) || 24)));
  const norm = normalizeBeatSelection(song, selection ? { ...selection, trackIdx } : null);
  if (!tr || !norm || tr.instrumentType === 'drums' || tr.isPercussion) {
    return { noteCount: 0, beatCount: 0, maxFret, sameOctave: [], octaveOptions: [], allOctavePositions: [] };
  }

  const coords = getSelectedBeatCoords(song, trackIdx, norm);
  const noteRefs = [];
  for (const { measureIdx, beatIdx } of coords) {
    const beat = tr.measures?.[measureIdx]?.beats?.[beatIdx];
    if (!beat || beat.isRest || !Array.isArray(beat.notes)) continue;
    for (const n of beat.notes) {
      const midi = noteToMidi(tr, n);
      if (midi !== null) {
        noteRefs.push({ measureIdx, beatIdx, string: n.string, fret: n.fret, midi });
      }
    }
  }

  if (noteRefs.length === 0) {
    return { noteCount: 0, beatCount: norm.beatCount, maxFret, sameOctave: [], octaveOptions: [], allOctavePositions: [] };
  }

  if (tr.instrumentType === 'keys') {
    const octaveOptions = [];
    const allOctavePositions = [];
    for (const oct of [-3, -2, -1, 1, 2, 3]) {
      const semis = oct * 12;
      if (noteRefs.every(r => r.midi + semis >= 21 && r.midi + semis <= 108)) {
        const pos = {
          index: octaveOptions.length + 1,
          label: `${oct > 0 ? '+' : ''}${oct} okt.`,
          shortRange: `${oct > 0 ? '+' : ''}${oct} okt.`,
          minFret: 0,
          maxFret: 0,
          octaveShift: oct,
          semitones: semis,
          isCurrent: false,
          assignments: noteRefs.map(r => ({
            measureIdx: r.measureIdx,
            beatIdx: r.beatIdx,
            oldString: r.string,
            oldFret: 0,
            newString: r.string,
            newFret: 0,
            midi: r.midi + semis
          }))
        };
        octaveOptions.push({ octaveShift: oct, semitones: semis, label: pos.label, positions: [pos] });
        allOctavePositions.push(pos);
      }
    }
    return { noteCount: noteRefs.length, beatCount: norm.beatCount, maxFret, sameOctave: [], octaveOptions, allOctavePositions };
  }

  const sameOctave = analyzePhrasePositions(tr, noteRefs, { octaveShift: 0, maxFret, boxSpan: 4, maxOptions: 6 });
  const octaveOptions = [];
  const allOctavePositions = [];
  for (const oct of [-3, -2, -1, 1, 2, 3]) {
    const positions = analyzePhrasePositions(tr, noteRefs, { octaveShift: oct, maxFret, boxSpan: 4, maxOptions: 3 });
    if (positions.length > 0) {
      octaveOptions.push({
        octaveShift: oct,
        semitones: oct * 12,
        label: `${oct > 0 ? '+' : ''}${oct} okt.`,
        positions
      });
      for (const p of positions) allOctavePositions.push(p);
    }
  }

  return {
    noteCount: noteRefs.length,
    beatCount: norm.beatCount,
    maxFret,
    sameOctave,
    octaveOptions,
    allOctavePositions
  };
}

/**
 * Aplikuje wybraną pozycję / przeniesienie oktawowe do ścieżki bez nadpisywania sąsiednich strun (atomowo na uderzenie).
 */
export function applyPhrasePosition(song, trackIdx, position) {
  if (!song || !position || !Array.isArray(position.assignments)) return 0;
  const tr = song.tracks?.[trackIdx];
  if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return 0;

  const isKeys = tr.instrumentType === 'keys';
  const maxStr = isKeys ? 0 : Math.max(0, (tr.openStringMidis?.length || 6) - 1);
  const byBeat = new Map();
  for (const a of position.assignments) {
    const key = `${a.measureIdx}:${a.beatIdx}`;
    if (!byBeat.has(key)) byBeat.set(key, []);
    byBeat.get(key).push(a);
  }

  // Walidacja przed zapisem (żadne uderzenie nie może mieć dwóch nut na tej samej strunie)
  const prepared = [];
  for (const [key, list] of byBeat.entries()) {
    const [mStr, bStr] = key.split(':');
    const beat = tr.measures?.[Number(mStr)]?.beats?.[Number(bStr)];
    if (!beat || beat.isRest || !Array.isArray(beat.notes)) continue;

    if (isKeys) {
      const nextNotes = beat.notes.map((n, idx) => {
        const match = list.find((a, lIdx) => lIdx === idx || a.oldMidi === n.midi);
        const midi = match && Number.isFinite(match.midi) ? Math.max(21, Math.min(108, Math.round(match.midi))) : n.midi;
        return { ...n, midi };
      });
      prepared.push({ beat, nextNotes, count: list.length });
      continue;
    }

    const assignedOldStrings = new Set(list.map(x => x.oldString));
    const keptNotes = beat.notes.filter(n => !assignedOldStrings.has(n.string));
    const usedStrings = new Set(keptNotes.map(n => n.string));

    for (const a of list) {
      const orig = beat.notes.find(n => n.string === a.oldString);
      if (!orig) return 0;
      const sTo = Math.round(a.newString);
      const fTo = Math.round(a.newFret);
      if (sTo < 0 || sTo > maxStr || fTo < 0 || fTo > 24 || usedStrings.has(sTo)) {
        return 0;
      }
      keptNotes.push({ ...orig, string: sTo, fret: fTo });
      usedStrings.add(sTo);
    }
    keptNotes.sort((a, b) => a.string - b.string);
    prepared.push({ beat, nextNotes: keptNotes, count: list.length });
  }

  let applied = 0;
  for (const item of prepared) {
    item.beat.notes = item.nextNotes;
    applied += item.count;
  }
  return applied;
}

function splitQuarterDurationToRestBeats(remQ) {
  const candidates = [
    { q: 4, d: -2, dots: 0 },
    { q: 3, d: -1, dots: 1 },
    { q: 2, d: -1, dots: 0 },
    { q: 1.5, d: 0, dots: 1 },
    { q: 1, d: 0, dots: 0 },
    { q: 0.75, d: 1, dots: 1 },
    { q: 0.5, d: 1, dots: 0 },
    { q: 0.25, d: 2, dots: 0 }
  ];
  const rests = [];
  let left = remQ;
  let guard = 0;
  while (left > 0.24 && guard++ < 12) {
    const pick = candidates.find(c => c.q <= left + 1e-4) || candidates[candidates.length - 1];
    rests.push(createBeat({ duration: pick.d, dots: pick.dots, isRest: true, notes: [] }));
    left -= pick.q;
  }
  return rests;
}

/**
 * Konwertuje listę kroków ćwiczeniowych (akordy, arpeggia, sweepy, progresje) na takty Studio TAB.
 */
export function buildMeasuresFromSteps(steps, opts = {}) {
  const numerator = Math.max(1, Math.min(16, Math.round(Number(opts.numerator) || 4)));
  const denominator = [1, 2, 4, 8, 16].includes(Number(opts.denominator)) ? Number(opts.denominator) : 4;
  const capQ = numerator * (4 / denominator);
  const defaultDur = Number.isInteger(Number(opts.defaultDuration)) ? Number(opts.defaultDuration) : 1;

  const rawBeats = (Array.isArray(steps) ? steps : []).map(st => {
    const dur = Number.isInteger(Number(st?.duration)) ? Number(st.duration) : defaultDur;
    const notesSrc = Array.isArray(st?.notes)
      ? st.notes
      : (Number.isFinite(st?.string) && Number.isFinite(st?.fret) ? [st] : []);
    const notes = [];
    const seen = new Set();
    for (const n of notesSrc) {
      const s = Math.max(0, Math.min(7, Math.round(Number(n.string) || 0)));
      const f = Math.max(0, Math.min(24, Math.round(Number(n.fret) || 0)));
      if (seen.has(s)) continue;
      seen.add(s);
      notes.push(createTabNote({
        string: s,
        fret: f,
        midi: Number.isFinite(Number(n.midi)) ? Math.round(Number(n.midi)) : undefined,
        velocity: Number(n.velocity) || 96,
        tap: Boolean(n.tapped || n.tap),
        hammerOn: Boolean(n.hammerOn),
        slide: Boolean(n.slide),
        harmonicType: Number(n.harmonicType) || (n.harmonic ? 1 : 0)
      }));
    }
    notes.sort((a, b) => a.string - b.string);
    return createBeat({
      duration: dur,
      dots: Number(st?.dots) || 0,
      tupletNumerator: Number(st?.tupletNumerator) || 0,
      tupletDenominator: Number(st?.tupletDenominator) || 0,
      isRest: notes.length === 0,
      chordSymbol: st?.chordSymbol ? String(st.chordSymbol) : '',
      text: st?.text ? String(st.text) : '',
      notes
    });
  });

  if (rawBeats.length === 0) {
    return [createEmptyMeasure(numerator, denominator)];
  }

  const measures = [];
  let curBeats = [];
  let curQ = 0;

  for (const beat of rawBeats) {
    const bQ = durationToQuarterNotes(beat);
    if (curBeats.length > 0 && curQ + bQ > capQ + 1e-4) {
      const rem = capQ - curQ;
      if (rem > 0.24) curBeats.push(...splitQuarterDurationToRestBeats(rem));
      measures.push({
        numerator,
        denominator,
        repeatOpen: false,
        repeatClose: 0,
        voltaMask: 0,
        sectionMarker: measures.length === 0 && opts.sectionMarker ? String(opts.sectionMarker) : '',
        isPickup: false,
        tripletFeel: 0,
        isDoubleBar: false,
        drumBlockId: 'rock_8th',
        beats: curBeats
      });
      curBeats = [];
      curQ = 0;
    }
    curBeats.push(beat);
    curQ += bQ;
  }

  if (curBeats.length > 0) {
    const rem = capQ - curQ;
    if (rem > 0.24) curBeats.push(...splitQuarterDurationToRestBeats(rem));
    measures.push({
      numerator,
      denominator,
      repeatOpen: false,
      repeatClose: 0,
      voltaMask: 0,
      sectionMarker: measures.length === 0 && opts.sectionMarker ? String(opts.sectionMarker) : '',
      isPickup: false,
      tripletFeel: 0,
      isDoubleBar: false,
      drumBlockId: 'rock_8th',
      beats: curBeats
    });
  }

  return measures;
}

function quantizeDeltaToDurationFields(deltaQ) {
  const table = [
    { q: 4, duration: -2, dots: 0, tn: 0, td: 0 },
    { q: 3, duration: -1, dots: 1, tn: 0, td: 0 },
    { q: 2, duration: -1, dots: 0, tn: 0, td: 0 },
    { q: 1.5, duration: 0, dots: 1, tn: 0, td: 0 },
    { q: 1, duration: 0, dots: 0, tn: 0, td: 0 },
    { q: 0.75, duration: 1, dots: 1, tn: 0, td: 0 },
    { q: 2 / 3, duration: 0, dots: 0, tn: 3, td: 2 },
    { q: 0.5, duration: 1, dots: 0, tn: 0, td: 0 },
    { q: 1 / 3, duration: 1, dots: 0, tn: 3, td: 2 },
    { q: 0.25, duration: 2, dots: 0, tn: 0, td: 0 },
    { q: 0.125, duration: 3, dots: 0, tn: 0, td: 0 }
  ];
  let best = table[4];
  let bestDiff = Infinity;
  for (const row of table) {
    const diff = Math.abs(deltaQ - row.q);
    if (diff < bestDiff) {
      bestDiff = diff;
      best = row;
    }
  }
  return best;
}

/**
 * Buduje znormalizowany ładunek JSON utworu dla endpointów C++ (/api/studio/export_gp5 i /api/studio/export_mid).
 */
export function buildSongExportPayload(song) {
  if (!song) return null;
  const masterCount = getMasterMeasureCount(song);
  const tempoBpm = Math.max(30, Math.min(300, Math.round(Number(song.tempoBpm) || 120)));

  const tracks = (song.tracks || []).map((tr, tIdx) => {
    const isPerc = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
    const measures = [];

    for (let mIdx = 0; mIdx < masterCount; mIdx++) {
      const m = tr.measures?.[mIdx] || createEmptyMeasure(4, 4);
      const num = Math.max(1, Math.min(32, Math.round(Number(m.numerator) || 4)));
      const den = [1, 2, 4, 8, 16].includes(Number(m.denominator)) ? Number(m.denominator) : 4;
      const capQ = num * (4 / den);

      let outBeats = [];
      if (isPerc) {
        const steps = resolveDrumMeasureSteps({ ...m, numerator: num, denominator: den }, tr, mIdx);
        if (steps.length === 0) {
          outBeats = [{ duration: -2, dots: 0, tupletNumerator: 0, tupletDenominator: 0, isRest: true, notes: [] }];
        } else {
          if (steps[0].beatOffsetQ > 0.05) {
            const rf = quantizeDeltaToDurationFields(steps[0].beatOffsetQ);
            outBeats.push({ duration: rf.duration, dots: rf.dots, tupletNumerator: rf.tn, tupletDenominator: rf.td, isRest: true, notes: [] });
          }
          for (let i = 0; i < steps.length; i++) {
            const nextQ = (i + 1 < steps.length) ? steps[i + 1].beatOffsetQ : capQ;
            const deltaQ = Math.max(0.125, nextQ - steps[i].beatOffsetQ);
            const df = quantizeDeltaToDurationFields(deltaQ);
            const notes = (steps[i].hits || []).map((h, hIdx) => {
              const rv = Number(h.velocity);
              const scaledVel = Number.isFinite(rv)
                ? (rv <= 1 ? Math.max(1, Math.min(127, Math.round(rv * 127))) : Math.max(1, Math.min(127, Math.round(rv))))
                : 96;
              return {
                string: hIdx,
                fret: GM_DRUM_MIDI[h.kind] ?? 36,
                velocity: scaledVel
              };
            });
            outBeats.push({
              duration: df.duration,
              dots: df.dots,
              tupletNumerator: df.tn,
              tupletDenominator: df.td,
              isRest: notes.length === 0,
              notes
            });
          }
        }
      } else {
        outBeats = (Array.isArray(m.beats) && m.beats.length > 0 ? m.beats : createEmptyMeasure(num, den).beats).map(b => ({
          duration: Number.isInteger(Number(b.duration)) ? Number(b.duration) : 0,
          dots: Math.max(0, Math.min(2, Math.round(Number(b.dots) || 0))),
          tupletNumerator: Math.round(Number(b.tupletNumerator) || 0),
          tupletDenominator: Math.round(Number(b.tupletDenominator) || 0),
          isRest: Boolean(b.isRest || !b.notes || b.notes.length === 0),
          text: String(b.text || ''),
          chordSymbol: String(b.chordSymbol || ''),
          dynamic: Math.max(1, Math.min(8, Math.round(Number(b.dynamic) || 6))),
          brushStroke: String(b.brushStroke || ''),
          pickStroke: Math.round(Number(b.pickStroke) || 0),
          fermata: Boolean(b.fermata),
          notes: (Array.isArray(b.notes) ? b.notes : []).map(n => ({
            string: Math.max(0, Math.round(Number(n.string) || 0)),
            fret: Math.max(0, Math.min(127, Math.round(Number(n.fret) || 0))),
            ...(Number.isFinite(Number(n.midi)) ? { midi: Math.max(12, Math.min(127, Math.round(Number(n.midi)))) } : {}),
            velocity: Math.max(1, Math.min(127, Math.round(Number(n.velocity) || 95))),
            tied: Boolean(n.tied),
            hammerOn: Boolean(n.hammerOn),
            slide: Boolean(n.slide || n.slideType),
            slideType: String(n.slideType || (n.slide ? 'shift' : '')),
            palmMute: Boolean(n.palmMute),
            letRing: Boolean(n.letRing),
            vibrato: Boolean(n.vibrato),
            wideVibrato: Boolean(n.wideVibrato),
            deadNote: Boolean(n.deadNote),
            ghostNote: Boolean(n.ghostNote),
            accented: Boolean(n.accented),
            heavyAccented: Boolean(n.heavyAccented),
            staccato: Boolean(n.staccato),
            tap: Boolean(n.tap),
            graceNote: Boolean(n.graceNote),
            tremoloPicking: Boolean(n.tremoloPicking),
            trill: Boolean(n.trill),
            harmonicType: Math.max(0, Math.min(5, Math.round(Number(n.harmonicType) || 0))),
            bendPreset: String(n.bendPreset || ''),
            bendPoints: Array.isArray(n.bendPoints)
              ? n.bendPoints.map(bp => ({
                  position: Math.max(0, Math.min(60, Math.round(Number(bp.position ?? bp.pos) || 0))),
                  value: Math.max(-200, Math.min(400, Math.round(Number(bp.value ?? bp.val) || 0))),
                  vibrato: Boolean(bp.vibrato)
                }))
              : [],
            whammyPreset: String(n.whammyPreset || ''),
            whammyPoints: Array.isArray(n.whammyPoints)
              ? n.whammyPoints.map(wp => ({
                  position: Math.max(0, Math.min(60, Math.round(Number(wp.position ?? wp.pos) || 0))),
                  value: Math.max(-400, Math.min(400, Math.round(Number(wp.value ?? wp.val) || 0)))
                }))
              : []
          }))
        }));
      }

      measures.push({
        numerator: num,
        denominator: den,
        repeatOpen: Boolean(m.repeatOpen),
        repeatClose: Math.max(0, Math.min(99, Math.round(Number(m.repeatClose) || 0))),
        voltaMask: Math.max(0, Math.min(255, Math.round(Number(m.voltaMask) || 0))),
        sectionMarker: String(m.sectionMarker || ''),
        tripletFeel: Math.max(0, Math.min(3, Math.round(Number(m.tripletFeel) || 0))),
        isDoubleBar: Boolean(m.isDoubleBar),
        beats: outBeats
      });
    }

    const isKeys = tr.instrumentType === 'keys';
    return {
      name: String(tr.name || `Ścieżka ${tIdx + 1}`),
      instrumentType: String(tr.instrumentType || 'guitar'),
      isPercussion: isPerc,
      tuningId: isPerc ? '' : (isKeys ? 'keys' : String(tr.tuningId || 'standard')),
      openStringMidis: (isPerc || isKeys) ? [] : (Array.isArray(tr.openStringMidis) ? tr.openStringMidis.slice() : [40, 45, 50, 55, 59, 64]),
      capo: isPerc ? 0 : Math.max(0, Math.min(12, Math.round(Number(tr.capo) || 0))),
      midiChannel: isPerc ? 9 : Math.max(0, Math.min(15, Math.round(Number(tr.midiChannel ?? tIdx)))),
      midiProgram: isPerc ? 0 : Math.max(0, Math.min(127, Math.round(Number(tr.midiProgram ?? 25)))),
      measures
    };
  });

  return {
    title: String(song.title || 'Nowe ćwiczenie'),
    artist: String(song.artist || 'Apka Gitarowa'),
    album: String(song.album || 'Studio TAB'),
    tempoBpm,
    keyRootPc: Math.max(0, Math.min(11, Math.round(Number(song.keyRootPc ?? 0)))),
    keyIsMinor: song.keyScaleId !== 'major',
    tracks
  };
}

function ensureMeasureCustomGrid(track, measure, measureIdx) {
  if (!measure) return null;
  if (!measure.customDrumGrid || !measure.customDrumGrid.lanes) {
    const autoFillN = Number(track?.drumAutoFill) || 0;
    const isFillBar = autoFillN > 0 && ((measureIdx + 1) % autoFillN === 0);
    measure.customDrumGrid = drumBlockToGridPattern(measure.drumBlockId || 'rock_8th', {
      numerator: measure.numerator || 4,
      denominator: measure.denominator || 4,
      isBarOne: measureIdx === 0,
      isFillBar,
      crashOnOne: Boolean(track?.drumCrashOnOne)
    });
  }
  return measure.customDrumGrid;
}

/**
 * Przełącza lub ustawia krok na 5-wierszowej siatce perkusji (0=wył, 1=ghost, 2=normalny, 3=akcent).
 */
export function setCustomDrumCell(song, trackIdx, measureIdx, laneId, stepIdx, brushLevel = 2) {
  const tr = song?.tracks?.[trackIdx];
  const m = tr?.measures?.[measureIdx];
  if (!tr || !m) return -1;
  const grid = ensureMeasureCustomGrid(tr, m, measureIdx);
  if (!grid || !Array.isArray(grid.lanes?.[laneId])) return -1;
  const s = Math.max(0, Math.min(grid.stepCount - 1, Math.round(Number(stepIdx) || 0)));
  const bLvl = Math.max(1, Math.min(3, Math.round(Number(brushLevel) || 2)));
  const cur = grid.lanes[laneId][s] || 0;
  const next = cur === bLvl ? 0 : bLvl;
  grid.lanes[laneId][s] = next;
  return next;
}

/**
 * Zmienia rozdzielczość siatki perkusji (8 <-> 16 kroków na takt 4/4) zachowując wpisane uderzenia.
 */
export function setCustomDrumResolution(song, trackIdx, measureIdx, stepsPerBeat = 2) {
  const tr = song?.tracks?.[trackIdx];
  const m = tr?.measures?.[measureIdx];
  if (!tr || !m) return false;
  const spb = Number(stepsPerBeat) === 4 ? 4 : 2;
  const oldGrid = ensureMeasureCustomGrid(tr, m, measureIdx);
  if (!oldGrid || oldGrid.stepsPerBeat === spb) return true;
  const nextGrid = createEmptyDrumGrid(m.numerator || 4, m.denominator || 4, spb);
  for (const lane of DRUM_GRID_LANES) {
    const src = oldGrid.lanes[lane.id] || [];
    const dst = nextGrid.lanes[lane.id];
    if (spb === 4) {
      for (let i = 0; i < src.length; i++) {
        const dIdx = i * 2;
        if (dIdx < dst.length) dst[dIdx] = src[i] || 0;
      }
    } else {
      for (let i = 0; i < dst.length; i++) {
        const a = src[i * 2] || 0;
        const b = src[i * 2 + 1] || 0;
        dst[i] = Math.max(a, b);
      }
    }
  }
  m.customDrumGrid = nextGrid;
  return true;
}

export function copyDrumGridToAllMeasures(song, trackIdx, sourceMeasureIdx) {
  const tr = song?.tracks?.[trackIdx];
  const srcM = tr?.measures?.[sourceMeasureIdx];
  if (!tr || !srcM) return false;
  const srcGrid = ensureMeasureCustomGrid(tr, srcM, sourceMeasureIdx);
  if (!srcGrid) return false;
  for (let i = 0; i < tr.measures.length; i++) {
    if (i === sourceMeasureIdx) continue;
    const dstM = tr.measures[i];
    if (dstM.numerator === srcM.numerator && dstM.denominator === srcM.denominator) {
      dstM.customDrumGrid = JSON.parse(JSON.stringify(srcGrid));
    }
  }
  return true;
}

export function resetMeasureDrumGrid(song, trackIdx, measureIdx) {
  const m = song?.tracks?.[trackIdx]?.measures?.[measureIdx];
  if (!m) return false;
  m.customDrumGrid = null;
  return true;
}

// ============================================================================
// Zaznaczanie fragmentów beat-to-beat na TAB, grupowe operacje i schowek uderzeń
// ============================================================================

/**
 * Normalizuje obiekt zaznaczenia (beatSelection lub legacy selectionStart/selectionEnd)
 * do spójnego zakresu { trackIdx, start: {measureIdx, beatIdx}, end: {measureIdx, beatIdx}, beatCount, kind }.
 */
export function normalizeBeatSelection(song, uiOrSel) {
  if (!song || !Array.isArray(song.tracks) || song.tracks.length === 0 || !uiOrSel) return null;

  let raw = null;
  let trackIdx = 0;

  const isUiWrapper = ('beatSelection' in uiOrSel) || ('selectionStart' in uiOrSel) || ('activeTrackIdx' in uiOrSel);
  if (isUiWrapper) {
    trackIdx = Math.max(0, Math.min(song.tracks.length - 1, Number(uiOrSel.activeTrackIdx) || 0));
    if (uiOrSel.beatSelection) {
      raw = uiOrSel.beatSelection;
      if (Number.isFinite(Number(raw.trackIdx))) {
        trackIdx = Math.max(0, Math.min(song.tracks.length - 1, Number(raw.trackIdx)));
      }
    } else if (uiOrSel.selectionStart !== null && uiOrSel.selectionStart !== undefined) {
      const tr = song.tracks[trackIdx];
      const maxM = Math.max(0, (tr?.measures?.length || 1) - 1);
      const a = Math.max(0, Math.min(maxM, Number(uiOrSel.selectionStart) || 0));
      const b = Math.max(0, Math.min(maxM, Number(uiOrSel.selectionEnd ?? uiOrSel.selectionStart) || 0));
      const sm = Math.min(a, b);
      const em = Math.max(a, b);
      const endBeatsLen = Math.max(1, tr?.measures?.[em]?.beats?.length || 1);
      raw = {
        trackIdx,
        anchor: { measureIdx: sm, beatIdx: 0 },
        focus: { measureIdx: em, beatIdx: endBeatsLen - 1 },
        fullMeasures: true
      };
    } else {
      return null;
    }
  } else {
    raw = uiOrSel;
    if (Number.isFinite(Number(raw.trackIdx))) {
      trackIdx = Math.max(0, Math.min(song.tracks.length - 1, Number(raw.trackIdx)));
    }
  }

  const tr = song.tracks[trackIdx];
  if (!tr || !Array.isArray(tr.measures) || tr.measures.length === 0) return null;
  const srcA = raw.anchor || raw.start;
  const srcB = raw.focus || raw.end || srcA;
  if (!srcA || !srcB) return null;

  const clampCoord = (c) => {
    const mIdx = Math.max(0, Math.min(tr.measures.length - 1, Math.round(Number(c.measureIdx) || 0)));
    const beatsLen = tr.measures[mIdx]?.beats?.length || 0;
    const bIdx = beatsLen > 0
      ? Math.max(0, Math.min(beatsLen - 1, Math.round(Number(c.beatIdx) || 0)))
      : 0;
    return { measureIdx: mIdx, beatIdx: bIdx };
  };

  const c1 = clampCoord(srcA);
  const c2 = clampCoord(srcB);
  const swap = (c1.measureIdx > c2.measureIdx) || (c1.measureIdx === c2.measureIdx && c1.beatIdx > c2.beatIdx);
  const start = swap ? c2 : c1;
  const end = swap ? c1 : c2;

  let beatCount = 0;
  for (let m = start.measureIdx; m <= end.measureIdx; m++) {
    const beats = tr.measures[m]?.beats || [];
    if (beats.length === 0) {
      beatCount += 1;
      continue;
    }
    const lo = (m === start.measureIdx) ? start.beatIdx : 0;
    const hi = (m === end.measureIdx) ? Math.min(beats.length - 1, end.beatIdx) : (beats.length - 1);
    beatCount += Math.max(0, hi - lo + 1);
  }

  const kind = (raw.fullMeasures || raw.kind === 'measures') ? 'measures' : 'beats';
  return {
    trackIdx,
    start,
    end,
    startMeasure: start.measureIdx,
    endMeasure: end.measureIdx,
    beatCount,
    kind
  };
}

export function isBeatInSelection(sel, measureIdx, beatIdx) {
  if (!sel || !sel.start || !sel.end) return false;
  const m = Number(measureIdx);
  const b = Number(beatIdx);
  if (m < sel.start.measureIdx || m > sel.end.measureIdx) return false;
  if (m === sel.start.measureIdx && b < sel.start.beatIdx) return false;
  if (m === sel.end.measureIdx && b > sel.end.beatIdx) return false;
  return true;
}

export function getSelectedBeatCoords(song, trackIdx, sel) {
  const tr = song?.tracks?.[trackIdx];
  const norm = normalizeBeatSelection(song, sel ? { ...sel, trackIdx } : null);
  if (!tr || !norm) return [];
  const coords = [];
  for (let m = norm.start.measureIdx; m <= norm.end.measureIdx && m < tr.measures.length; m++) {
    const beats = tr.measures[m]?.beats || [];
    const lo = (m === norm.start.measureIdx) ? norm.start.beatIdx : 0;
    const hi = (m === norm.end.measureIdx) ? Math.min(beats.length - 1, norm.end.beatIdx) : (beats.length - 1);
    for (let b = lo; b <= hi; b++) {
      coords.push({ measureIdx: m, beatIdx: b });
    }
  }
  return coords;
}

/**
 * Przesuwa wysokość wszystkich nut w zaznaczonym fragmencie o `semitones` półtonów (+/-1, +/-12 itd.).
 * Dla ścieżek strunowych rozwiązuje przesunięcie atomowo na bezkolizyjnym gryfie — jeśli część nut nie mieści się w zakresie progów,
 * nie modyfikuje utworu połowicznie i zwraca `outOfRange: true`.
 */
function shiftTrackCoordsAtomic(song, tIdx, coords, semitones, maxFret = 24) {
  const tr = song?.tracks?.[tIdx];
  if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return { beatsChanged: 0, notesChanged: 0, outOfRange: false };

  const beatEntries = [];
  for (const { measureIdx, beatIdx } of coords) {
    const b = tr.measures?.[measureIdx]?.beats?.[beatIdx];
    if (b && !b.isRest && Array.isArray(b.notes) && b.notes.length > 0) {
      beatEntries.push({ measureIdx, beatIdx, beat: b });
    }
  }
  if (beatEntries.length === 0) return { beatsChanged: 0, notesChanged: 0, outOfRange: false };

  if (tr.instrumentType === 'keys') {
    for (const entry of beatEntries) {
      for (const n of entry.beat.notes) {
        const m = (n.midi ?? 60) + semitones;
        if (m < 21 || m > 108) return { beatsChanged: 0, notesChanged: 0, outOfRange: true };
      }
    }
    let notesChanged = 0;
    for (const entry of beatEntries) {
      for (const n of entry.beat.notes) {
        n.midi = (n.midi ?? 60) + semitones;
        notesChanged++;
      }
    }
    return { beatsChanged: beatEntries.length, notesChanged, outOfRange: false };
  }

  let fretSum = 0;
  let noteCount = 0;
  for (const entry of beatEntries) {
    for (const n of entry.beat.notes) {
      fretSum += Number(n.fret) || 0;
      noteCount++;
    }
  }
  const avgOrigFret = noteCount > 0 ? (fretSum / noteCount) : 5;
  const maxNeck = Math.max(12, Math.min(24, Math.round(Number(maxFret) || 24)));
  const capo = Number(tr.capo) || 0;
  const openMidis = Array.isArray(tr.openStringMidis) ? tr.openStringMidis : [];

  // Szybka ścieżka bezkolizyjna: jeśli wszystkie nuty mieszczą się na swoich oryginalnych strunach w [0..maxNeck], zachowaj układ strun
  const sameStringAssignments = [];
  let canKeepOriginalStrings = openMidis.length > 0;
  for (const entry of beatEntries) {
    if (!canKeepOriginalStrings) break;
    const usedStrings = new Set();
    for (const n of entry.beat.notes) {
      const baseMidi = noteToMidi(tr, n);
      if (baseMidi === null || n.string < 0 || n.string >= openMidis.length || usedStrings.has(n.string)) {
        canKeepOriginalStrings = false;
        break;
      }
      const targetMidi = baseMidi + semitones;
      const sameFret = targetMidi - (openMidis[n.string] + capo);
      if (sameFret < 0 || sameFret > maxNeck) {
        canKeepOriginalStrings = false;
        break;
      }
      usedStrings.add(n.string);
      sameStringAssignments.push({
        measureIdx: entry.measureIdx,
        beatIdx: entry.beatIdx,
        oldString: n.string,
        oldFret: n.fret,
        newString: n.string,
        newFret: sameFret,
        midi: targetMidi
      });
    }
  }
  if (canKeepOriginalStrings && sameStringAssignments.length > 0) {
    const appliedSame = applyPhrasePosition(song, tIdx, { assignments: sameStringAssignments });
    if (appliedSame > 0) {
      return { beatsChanged: beatEntries.length, notesChanged: appliedSame, outOfRange: false };
    }
  }

  // Najpierw sprawdź, czy każde uderzenie ma w ogóle bezkolizyjne rozwiązanie w [0..maxNeck]
  for (const entry of beatEntries) {
    const taggedNotes = entry.beat.notes.map(n => ({ ...n, measureIdx: entry.measureIdx, beatIdx: entry.beatIdx }));
    if (!solveBeatFretAssignments(tr, taggedNotes, semitones, { minFret: 0, maxFret: maxNeck, boxStart: 0, boxEnd: maxNeck })) {
      return { beatsChanged: 0, notesChanged: 0, outOfRange: true };
    }
  }

  let bestPlan = null;
  let bestRank = Infinity;
  const span = 5;
  for (let startF = 0; startF <= Math.max(0, maxNeck - span); startF++) {
    const endF = Math.min(maxNeck, startF + span);
    let prevFret = (startF + endF) / 2;
    let prevString = Math.min(2, Math.max(0, Math.floor(((tr.openStringMidis?.length || 6) - 1) / 2)));
    let outCount = 0;
    let cost = 0;
    const assignments = [];
    let ok = true;

    for (const entry of beatEntries) {
      const taggedNotes = entry.beat.notes.map(n => ({ ...n, measureIdx: entry.measureIdx, beatIdx: entry.beatIdx }));
      const solved = solveBeatFretAssignments(tr, taggedNotes, semitones, {
        minFret: 0,
        maxFret: maxNeck,
        boxStart: startF,
        boxEnd: endF,
        prevFret,
        prevString
      });
      if (!solved) { ok = false; break; }
      outCount += solved.outOfBoxCount;
      cost += solved.cost;
      prevFret = solved.avgFret;
      prevString = solved.avgStr;
      for (const a of solved.assignments) assignments.push(a);
    }
    if (!ok) continue;

    const boxCenter = (startF + endF) / 2;
    const anchorBias = Math.abs(semitones) < 12 ? Math.abs(boxCenter - avgOrigFret) * 1.4 : boxCenter * 0.15;
    const rank = outCount * 60 + cost + anchorBias;
    if (rank < bestRank) {
      bestRank = rank;
      bestPlan = { assignments };
    }
  }

  if (!bestPlan) return { beatsChanged: 0, notesChanged: 0, outOfRange: true };
  const applied = applyPhrasePosition(song, tIdx, bestPlan);
  if (applied <= 0) return { beatsChanged: 0, notesChanged: 0, outOfRange: true };
  return { beatsChanged: beatEntries.length, notesChanged: applied, outOfRange: false };
}

export function shiftSelectionPitch(song, trackIdx, selection, semitones, { allTracks = false, maxFret = 24 } = {}) {
  const sel = normalizeBeatSelection(song, selection ? { ...selection, trackIdx } : null);
  if (!sel || !semitones) return { beatsChanged: 0, notesChanged: 0, outOfRange: false };

  let beatsChanged = 0;
  let notesChanged = 0;
  let anyOutOfRange = false;

  if (allTracks) {
    song.tracks.forEach((tr, tIdx) => {
      if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return;
      const coords = (tIdx === trackIdx && sel.kind === 'beats')
        ? getSelectedBeatCoords(song, tIdx, sel)
        : (() => {
            const arr = [];
            for (let mIdx = sel.startMeasure; mIdx <= sel.endMeasure && mIdx < tr.measures.length; mIdx++) {
              const beats = tr.measures[mIdx]?.beats || [];
              for (let bIdx = 0; bIdx < beats.length; bIdx++) arr.push({ measureIdx: mIdx, beatIdx: bIdx });
            }
            return arr;
          })();
      const res = shiftTrackCoordsAtomic(song, tIdx, coords, semitones, maxFret);
      beatsChanged += res.beatsChanged;
      notesChanged += res.notesChanged;
      if (res.outOfRange) anyOutOfRange = true;
    });
    return { beatsChanged, notesChanged, outOfRange: anyOutOfRange && beatsChanged === 0 };
  }

  const coords = getSelectedBeatCoords(song, trackIdx, sel);
  return shiftTrackCoordsAtomic(song, trackIdx, coords, semitones, maxFret);
}

/**
 * Przenosi wszystkie nuty zaznaczonego fragmentu na sąsiednią strunę bez zmiany wysokości MIDI.
 */
export function moveSelectionToAdjacentString(song, trackIdx, selection, dir) {
  const coords = getSelectedBeatCoords(song, trackIdx, selection);
  const tr = song?.tracks?.[trackIdx];
  if (!tr || coords.length === 0) return { movedCount: 0, skippedCount: 0 };

  let movedCount = 0;
  let skippedCount = 0;
  for (const { measureIdx, beatIdx } of coords) {
    const beat = tr.measures?.[measureIdx]?.beats?.[beatIdx];
    if (!beat || beat.isRest || !Array.isArray(beat.notes) || beat.notes.length === 0) continue;
    const ordered = beat.notes.slice().sort((a, b) => (dir >= 0 ? (b.string - a.string) : (a.string - b.string)));
    for (const note of ordered) {
      const res = moveNoteToAdjacentString(song, trackIdx, measureIdx, beatIdx, note.string, dir);
      if (res.moved) movedCount++;
      else skippedCount++;
    }
  }
  return { movedCount, skippedCount };
}

/**
 * Grupowo przełącza efekt/artykulację na wszystkich nutach w zaznaczonym fragmencie.
 */
export function applyEffectToSelection(song, trackIdx, selection, effect, value = undefined) {
  const coords = getSelectedBeatCoords(song, trackIdx, selection);
  const tr = song?.tracks?.[trackIdx];
  if (!tr || coords.length === 0) return 0;

  const allNotes = [];
  for (const { measureIdx, beatIdx } of coords) {
    const b = tr.measures?.[measureIdx]?.beats?.[beatIdx];
    if (b && !b.isRest && Array.isArray(b.notes)) {
      for (const n of b.notes) allNotes.push(n);
    }
  }
  if (allNotes.length === 0) return 0;

  if (effect === 'harmonicType') {
    const req = Number(value) || 1;
    const allHave = allNotes.every(n => n.harmonicType === req);
    const next = allHave ? 0 : req;
    allNotes.forEach(n => { n.harmonicType = next; });
    return allNotes.length;
  }
  if (effect === 'slide' || effect === 'slideType') {
    const req = String(value || 'shift');
    const allHave = allNotes.every(n => n.slideType === req);
    const next = allHave ? '' : req;
    allNotes.forEach(n => {
      n.slideType = next;
      n.slide = Boolean(next);
    });
    return allNotes.length;
  }
  if (effect === 'bendPreset') {
    const presetKey = String(value || 'full');
    const preset = BEND_PRESETS[presetKey] || BEND_PRESETS.full;
    const clear = (presetKey === 'clear') || allNotes.every(n => n.bendPreset === preset.id);
    allNotes.forEach(n => {
      n.bendPreset = clear ? '' : preset.id;
      n.bendPoints = clear ? [] : preset.points.map(p => ({ ...p }));
    });
    return allNotes.length;
  }
  if (effect === 'whammyPreset') {
    const presetKey = String(value || 'dive_full');
    const preset = WHAMMY_PRESETS[presetKey] || WHAMMY_PRESETS.dive_full;
    const clear = (presetKey === 'clear') || allNotes.every(n => n.whammyPreset === preset.id);
    allNotes.forEach(n => {
      n.whammyPreset = clear ? '' : preset.id;
      n.whammyPoints = clear ? [] : preset.points.map(p => ({ ...p }));
    });
    return allNotes.length;
  }

  const boolEffects = new Set([
    'tied', 'palmMute', 'letRing', 'hammerOn', 'vibrato', 'wideVibrato',
    'deadNote', 'ghostNote', 'accented', 'heavyAccented', 'staccato',
    'tap', 'graceNote', 'tremoloPicking', 'trill'
  ]);
  if (!boolEffects.has(effect)) return 0;
  const allOn = allNotes.every(n => Boolean(n[effect]));
  const nextVal = !allOn;
  allNotes.forEach(n => { n[effect] = nextVal; });
  return allNotes.length;
}

/**
 * Grupowo zmienia długość rytmiczną, kropki, tuplet, dynamikę lub właściwości uderzeń w zaznaczeniu.
 */
export function applyBeatPropToSelection(song, trackIdx, selection, prop, value = undefined) {
  const coords = getSelectedBeatCoords(song, trackIdx, selection);
  const tr = song?.tracks?.[trackIdx];
  if (!tr || coords.length === 0) return 0;

  const beats = coords.map(({ measureIdx, beatIdx }) => tr.measures?.[measureIdx]?.beats?.[beatIdx]).filter(Boolean);
  if (beats.length === 0) return 0;

  if (prop === 'duration') {
    coords.forEach(({ measureIdx, beatIdx }) => setBeatDuration(song, trackIdx, measureIdx, beatIdx, value));
    return beats.length;
  }
  if (prop === 'dots') {
    const req = (value === 1 || value === 2) ? value : 1;
    const allHave = beats.every(b => (b.dots || 0) === req);
    const nextDots = (value === null || value === undefined)
      ? (((beats[0].dots || 0) + 1) % 3)
      : (allHave ? 0 : req);
    beats.forEach(b => { b.dots = nextDots; });
    return beats.length;
  }
  if (prop === 'tuplet') {
    const num = Number(value?.num) || 3;
    const den = Number(value?.den) || 2;
    const allHave = beats.every(b => b.tupletNumerator === num && b.tupletDenominator === den);
    beats.forEach(b => {
      b.tupletNumerator = allHave ? 0 : num;
      b.tupletDenominator = allHave ? 0 : den;
    });
    return beats.length;
  }
  if (prop === 'dynamic') {
    coords.forEach(({ measureIdx, beatIdx }) => setBeatDynamic(song, trackIdx, measureIdx, beatIdx, value));
    return beats.length;
  }
  if (prop === 'rest') {
    const allRest = beats.every(b => b.isRest);
    if (allRest) {
      beats.forEach(b => { if (Array.isArray(b.notes) && b.notes.length > 0) b.isRest = false; });
    } else {
      beats.forEach(b => { b.isRest = true; b.notes = []; });
    }
    return beats.length;
  }
  if (prop === 'fermata') {
    const nextVal = !beats.every(b => Boolean(b.fermata));
    beats.forEach(b => { b.fermata = nextVal; });
    return beats.length;
  }
  if (prop === 'pickStroke') {
    const req = Number(value) || 1;
    const nextVal = beats.every(b => b.pickStroke === req) ? 0 : req;
    beats.forEach(b => { b.pickStroke = nextVal; });
    return beats.length;
  }
  if (prop === 'brushStroke') {
    const req = String(value || 'down');
    const nextVal = beats.every(b => b.brushStroke === req) ? '' : req;
    beats.forEach(b => { b.brushStroke = nextVal; });
    return beats.length;
  }
  return 0;
}

/**
 * Zamienia wszystkie zaznaczone uderzenia w pauzy (czyści nuty zachowując rytm).
 */
export function deleteSelectionNotes(song, trackIdx, selection) {
  const coords = getSelectedBeatCoords(song, trackIdx, selection);
  const tr = song?.tracks?.[trackIdx];
  if (!tr || coords.length === 0) return 0;
  let cleared = 0;
  for (const { measureIdx, beatIdx } of coords) {
    const b = tr.measures?.[measureIdx]?.beats?.[beatIdx];
    if (b) {
      b.isRest = true;
      b.notes = [];
      cleared++;
    }
  }
  return cleared;
}

function durationToRestBeats(quarterNotes) {
  const table = [
    { q: 4, d: -2, dots: 0 },
    { q: 3, d: -1, dots: 1 },
    { q: 2, d: -1, dots: 0 },
    { q: 1.5, d: 0, dots: 1 },
    { q: 1, d: 0, dots: 0 },
    { q: 0.75, d: 1, dots: 1 },
    { q: 0.5, d: 1, dots: 0 },
    { q: 0.25, d: 2, dots: 0 },
    { q: 0.125, d: 3, dots: 0 },
    { q: 0.0625, d: 4, dots: 0 }
  ];
  const out = [];
  let rem = Math.max(0, Number(quarterNotes) || 0);
  let guard = 0;
  while (rem > 0.05 && guard++ < 24) {
    const pick = table.find(c => c.q <= rem + 1e-4) || table[table.length - 1];
    out.push(createBeat({ duration: pick.d, dots: pick.dots, isRest: true, notes: [] }));
    rem -= pick.q;
  }
  return out;
}

export function copyBeatsToClipboard(song, { trackIdx = 0, selection } = {}) {
  const tr = song?.tracks?.[trackIdx];
  const coords = getSelectedBeatCoords(song, trackIdx, selection);
  if (!tr || coords.length === 0) return null;
  const beats = coords
    .map(({ measureIdx, beatIdx }) => tr.measures?.[measureIdx]?.beats?.[beatIdx])
    .filter(Boolean)
    .map(b => cloneSong(b));
  if (beats.length === 0) return null;
  return {
    kind: 'beats',
    scope: 'track',
    sourceTrackIdx: trackIdx,
    beatCount: beats.length,
    barCount: 0,
    beats
  };
}

export function cutBeatsToClipboard(song, { trackIdx = 0, selection } = {}) {
  const clip = copyBeatsToClipboard(song, { trackIdx, selection });
  if (!clip) return null;
  deleteSelectionNotes(song, trackIdx, selection);
  return clip;
}

/**
 * Wkleja skopiowany fragment uderzeń od wskazanego taktu i uderzenia (jak w TuxGuitar):
 * w trybie 'overwrite' zachowuje metrum taktu i przy nadmiarze przechodzi na kolejne takty.
 */
export function pasteBeatsFromClipboard(song, clipboard, { targetTrackIdx = 0, targetMeasureIdx = 0, targetBeatIdx = 0, mode = 'overwrite' } = {}) {
  if (!song || !clipboard || clipboard.kind !== 'beats' || !Array.isArray(clipboard.beats) || clipboard.beats.length === 0) {
    return null;
  }
  const tr = song.tracks?.[targetTrackIdx];
  if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return null;

  let mIdx = Math.max(0, Number(targetMeasureIdx) || 0);
  while (mIdx >= getMasterMeasureCount(song)) {
    addMeasure(song);
  }
  const startMeasure = tr.measures[mIdx];
  let bIdx = Math.max(0, Math.min(startMeasure?.beats?.length || 0, Number(targetBeatIdx) || 0));

  if (mode === 'insert') {
    const copies = clipboard.beats.map(b => cloneSong(b));
    startMeasure.beats.splice(bIdx, 0, ...copies);
    return {
      pastedCount: copies.length,
      endMeasureIdx: mIdx,
      endBeatIdx: bIdx + copies.length - 1
    };
  }

  let srcIdx = 0;
  let lastEndM = mIdx;
  let lastEndB = bIdx;
  let guard = 0;

  while (srcIdx < clipboard.beats.length && guard++ < 256) {
    while (mIdx >= getMasterMeasureCount(song)) {
      addMeasure(song);
    }
    const m = tr.measures[mIdx];
    if (!Array.isArray(m.beats)) m.beats = [];
    const capQ = Math.max(0.25, (m.numerator || 4) * (4 / (m.denominator || 4)));
    let prefixQ = 0;
    const clampedB = Math.max(0, Math.min(m.beats.length, bIdx));
    for (let k = 0; k < clampedB; k++) {
      prefixQ += durationToQuarterNotes(m.beats[k]);
    }
    const availQ = Math.max(0, capQ - prefixQ);

    const chunk = [];
    let chunkQ = 0;
    while (srcIdx + chunk.length < clipboard.beats.length) {
      const cand = clipboard.beats[srcIdx + chunk.length];
      const dQ = Math.max(0.0625, durationToQuarterNotes(cand));
      if (chunk.length > 0 && chunkQ + dQ > availQ + 1e-4) break;
      if (chunk.length === 0 && clampedB > 0 && dQ > availQ + 1e-4) break;
      chunk.push(cand);
      chunkQ += dQ;
      if (chunkQ >= availQ - 1e-4) break;
    }

    if (chunk.length === 0) {
      mIdx++;
      bIdx = 0;
      continue;
    }

    let consumedQ = 0;
    let consumeCount = 0;
    while (clampedB + consumeCount < m.beats.length && consumedQ < chunkQ - 1e-4) {
      consumedQ += durationToQuarterNotes(m.beats[clampedB + consumeCount]);
      consumeCount++;
    }

    const remQ = consumedQ - chunkQ;
    const padRests = remQ > 0.05 ? durationToRestBeats(remQ) : [];
    const nStr = tr.instrumentType === 'keys' ? 0 : (tr.openStringMidis?.length || tr.stringCount || 6);
    const pastedClones = chunk.map(b => {
      const cb = cloneSong(b);
      if (nStr > 0 && Array.isArray(cb.notes)) {
        const seen = new Set();
        cb.notes = cb.notes.filter(n => {
          const s = Math.max(0, Math.min(nStr - 1, Number(n.string) || 0));
          if (seen.has(s)) return false;
          n.string = s;
          seen.add(s);
          return true;
        });
      }
      return cb;
    });
    m.beats.splice(clampedB, consumeCount, ...pastedClones, ...padRests);
    if (m.beats.length === 0) {
      m.beats = createEmptyMeasure(m.numerator, m.denominator).beats;
    }

    lastEndM = mIdx;
    lastEndB = clampedB + pastedClones.length - 1;
    srcIdx += chunk.length;
    if (srcIdx < clipboard.beats.length) {
      mIdx++;
      bIdx = 0;
    }
  }

  return {
    pastedCount: clipboard.beats.length,
    endMeasureIdx: lastEndM,
    endBeatIdx: lastEndB
  };
}

// ============================================================================
// Auto-akordy (ChordNamer) oraz Generator Arpeggio / Sweep dla Studio TAB
// ============================================================================

function formatDetectedChordSymbol(reading, accMode = 2) {
  if (!reading || reading.kind !== 'chord' || !reading.chord) return '';
  const ch = reading.chord;
  const rootName = midiToNoteName(ch.root, accMode);
  const qSym = ch.quality?.symbol || '';
  if (ch.isInversion) {
    return `${rootName}${qSym}/${midiToNoteName(ch.bass, accMode)}`;
  }
  return `${rootName}${qSym}`;
}

export function detectAutoChordForBeat(track, beat, accMode = 2) {
  if (!track || track.instrumentType === 'drums' || track.isPercussion) return '';
  if (!beat || beat.isRest || !Array.isArray(beat.notes) || beat.notes.length < 2) return '';
  const midis = [];
  const pcs = new Set();
  for (const n of beat.notes) {
    if (n.deadNote) continue;
    const m = noteToMidi(track, n);
    if (m !== null) {
      midis.push(m);
      pcs.add(((m % 12) + 12) % 12);
    }
  }
  if (pcs.size < 2) return '';
  const reading = ChordNamer.read(midis);
  return formatDetectedChordSymbol(reading, accMode);
}

export function detectAutoChordsForMeasure(track, measure, accMode = 2) {
  if (!track || !measure || track.instrumentType === 'drums' || track.isPercussion) {
    return { perBeat: [], measureChord: '' };
  }
  const beats = Array.isArray(measure.beats) ? measure.beats : [];
  const perBeat = beats.map(b => detectAutoChordForBeat(track, b, accMode));
  const hasPolyBeat = perBeat.some(Boolean);
  if (hasPolyBeat) {
    return { perBeat, measureChord: '' };
  }
  const allMidis = [];
  const weights = [];
  const pcs = new Set();
  for (const b of beats) {
    if (b.isRest || !Array.isArray(b.notes)) continue;
    const w = Math.max(0.25, Math.min(2.0, durationToQuarterNotes(b)));
    for (const n of b.notes) {
      if (n.deadNote) continue;
      const m = noteToMidi(track, n);
      if (m !== null) {
        allMidis.push(m);
        weights.push(w);
        pcs.add(((m % 12) + 12) % 12);
      }
    }
  }
  if (pcs.size < 2) return { perBeat, measureChord: '' };
  const reading = ChordNamer.read(allMidis, weights);
  return { perBeat, measureChord: formatDetectedChordSymbol(reading, accMode) };
}

export const STUDIO_ARP_QUALITIES = Object.freeze([
  { id: 'minor', name: 'Moll (m)',       symbol: 'm',    intervals: [0, 3, 7] },
  { id: 'major', name: 'Dur',            symbol: '',     intervals: [0, 4, 7] },
  { id: 'min7',  name: 'Moll 7 (m7)',    symbol: 'm7',   intervals: [0, 3, 7, 10] },
  { id: 'maj7',  name: 'Dur 7 (maj7)',   symbol: 'maj7', intervals: [0, 4, 7, 11] },
  { id: 'dom7',  name: 'Dominant (7)',   symbol: '7',    intervals: [0, 4, 7, 10] },
  { id: 'dim',   name: 'Zmniejsz. (dim)',symbol: 'dim',  intervals: [0, 3, 6] },
  { id: 'dim7',  name: 'Dim7',           symbol: 'dim7', intervals: [0, 3, 6, 9] },
  { id: 'sus4',  name: 'Sus4',           symbol: 'sus4', intervals: [0, 5, 7] },
  { id: 'sus2',  name: 'Sus2',           symbol: 'sus2', intervals: [0, 2, 7] },
  { id: 'power', name: 'Power (5)',      symbol: '5',    intervals: [0, 7] }
]);

export function generateStudioArpeggioOrSweepSteps({
  mode = 'arpeggio',
  rootPc = 0,
  quality = 'minor',
  direction = 'up_down',
  track = null,
  duration = 1,
  accMode = 2
} = {}) {
  const qDef = STUDIO_ARP_QUALITIES.find(q => q.id === quality) || STUDIO_ARP_QUALITIES[0];
  const root = ((Math.round(Number(rootPc) || 0) % 12) + 12) % 12;
  const chordPcs = new Set(qDef.intervals.map(iv => (root + iv) % 12));
  const chordLabel = `${midiToNoteName(root, accMode)}${qDef.symbol}`;
  const isKeys = track?.instrumentType === 'keys';

  let rawSeq = [];
  if (isKeys) {
    const baseMidi = 48 + root;
    const pool = [];
    for (let oct = 0; oct <= 1; oct++) {
      for (const iv of qDef.intervals) {
        pool.push({ midi: baseMidi + oct * 12 + iv, string: 0, fret: 0 });
      }
    }
    pool.push({ midi: baseMidi + 24, string: 0, fret: 0 });
    const asc = pool.slice(0, Math.min(6, pool.length));
    if (direction === 'up') rawSeq = asc;
    else if (direction === 'down') rawSeq = asc.slice().reverse();
    else rawSeq = asc.concat(asc.slice(1, -1).reverse());
  } else {
    const openMidis = Array.isArray(track?.openStringMidis) && track.openStringMidis.length > 0
      ? track.openStringMidis
      : [40, 45, 50, 55, 59, 64];
    const capo = Number(track?.capo) || 0;
    const nStrings = openMidis.length;
    const basePc0 = ((openMidis[0] + capo) % 12 + 12) % 12;
    let anchorFret = (root - basePc0 + 12) % 12;
    if (anchorFret === 0) anchorFret = 12;
    if (anchorFret > 12) anchorFret = anchorFret - 12;
    const boxMin = Math.max(0, anchorFret - 2);
    const boxMax = Math.min(19, anchorFret + 4);

    if (mode === 'sweep') {
      const startStr = (nStrings >= 6 && anchorFret >= 3) ? 1 : 0;
      const perString = [];
      for (let s = startStr; s < nStrings; s++) {
        const cands = [];
        for (let f = boxMin; f <= boxMax; f++) {
          const midi = openMidis[s] + capo + f;
          const pc = ((midi % 12) + 12) % 12;
          if (chordPcs.has(pc)) cands.push({ string: s, fret: f, midi });
        }
        if (cands.length > 0) {
          cands.sort((a, b) => Math.abs(a.fret - anchorFret) - Math.abs(b.fret - anchorFret));
          perString.push({ list: cands, pick: cands[0] });
        }
      }
      const asc = perString.map(x => ({ ...x.pick, hammerOn: false }));
      const topEntry = perString[perString.length - 1];
      const topExtra = topEntry?.list?.find(c => c.fret > topEntry.pick.fret);
      if (direction === 'up') {
        rawSeq = asc;
      } else if (direction === 'down') {
        rawSeq = asc.slice().reverse();
      } else {
        rawSeq = asc.slice();
        if (topExtra) rawSeq.push({ ...topExtra, hammerOn: true });
        for (let i = asc.length - (topExtra ? 1 : 2); i >= 0; i--) {
          if (i === 0 && rawSeq.length >= 2) continue;
          rawSeq.push({ ...asc[i], hammerOn: false });
        }
      }
    } else {
      const byMidi = new Map();
      for (let s = 0; s < nStrings; s++) {
        for (let f = boxMin; f <= boxMax; f++) {
          const midi = openMidis[s] + capo + f;
          const pc = ((midi % 12) + 12) % 12;
          if (!chordPcs.has(pc)) continue;
          const prev = byMidi.get(midi);
          if (!prev || Math.abs(f - anchorFret) < Math.abs(prev.fret - anchorFret)) {
            byMidi.set(midi, { string: s, fret: f, midi });
          }
        }
      }
      const asc = Array.from(byMidi.values()).sort((a, b) => a.midi - b.midi);
      if (asc.length >= 2) {
        if (direction === 'up') {
          rawSeq = asc;
        } else if (direction === 'down') {
          rawSeq = asc.slice().reverse();
        } else if (direction === 'broken') {
          for (let i = 0; i + 2 < asc.length; i++) {
            rawSeq.push(asc[i], asc[i + 2], asc[i + 1]);
          }
          if (rawSeq.length === 0) rawSeq = asc;
        } else {
          rawSeq = asc.concat(asc.slice(1, -1).reverse());
        }
      } else {
        rawSeq = asc;
      }
    }
  }

  return rawSeq.map((n, idx) => ({
    duration: Number.isInteger(Number(duration)) ? Number(duration) : 1,
    chordSymbol: idx === 0 ? chordLabel : '',
    notes: [n]
  }));
}

export function applyGeneratedStepsToSong(song, trackIdx, steps, {
  insertMode = 'cursor',
  measureIdx = 0,
  beatIdx = 0,
  sectionMarker = ''
} = {}) {
  if (!song || !Array.isArray(song.tracks) || !Array.isArray(steps) || steps.length === 0) {
    return { ok: false, insertedBars: 0, insertedBeats: 0 };
  }
  const tr = song.tracks[trackIdx];
  if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) {
    return { ok: false, insertedBars: 0, insertedBeats: 0 };
  }
  const refM = tr.measures?.[Math.max(0, Math.min(tr.measures.length - 1, measureIdx))] || { numerator: 4, denominator: 4 };
  if (insertMode === 'append') {
    const newMeasures = buildMeasuresFromSteps(steps, {
      numerator: refM.numerator || 4,
      denominator: refM.denominator || 4,
      sectionMarker: sectionMarker || steps[0]?.chordSymbol || ''
    });
    const baseCount = tr.measures.length;
    tr.measures.push(...newMeasures);
    const newLen = tr.measures.length;
    for (let t = 0; t < song.tracks.length; t++) {
      if (t === trackIdx) continue;
      const other = song.tracks[t];
      const isDrum = other.instrumentType === 'drums' || other.isPercussion;
      while (other.measures.length < newLen) {
        const ref = tr.measures[other.measures.length] || refM;
        other.measures.push(createEmptyMeasure(ref.numerator, ref.denominator, 'rock_8th', isDrum));
      }
    }
    return { ok: true, insertedBars: newMeasures.length, insertedBeats: steps.length, startMeasureIdx: baseCount };
  }

  const built = buildMeasuresFromSteps(steps, { numerator: refM.numerator || 4, denominator: refM.denominator || 4 });
  const beats = [];
  for (const bm of built) {
    for (const b of (bm.beats || [])) {
      if (!b.isRest) beats.push(b);
    }
  }
  const pasteRes = pasteBeatsFromClipboard(song, {
    kind: 'beats',
    scope: 'track',
    sourceTrackIdx: trackIdx,
    beatCount: beats.length,
    beats
  }, {
    targetTrackIdx: trackIdx,
    targetMeasureIdx: measureIdx,
    targetBeatIdx: beatIdx,
    mode: 'overwrite'
  });
  return {
    ok: Boolean(pasteRes),
    insertedBars: 0,
    insertedBeats: pasteRes?.pastedCount || 0,
    endMeasureIdx: pasteRes?.endMeasureIdx ?? measureIdx,
    endBeatIdx: pasteRes?.endBeatIdx ?? beatIdx
  };
}

/**
 * Konwertuje surowe kroki z zakładek Arpeggio / Sweep (lub dowolnej listy kroków) do schowka uderzeń Studia TAB.
 */
export function buildBeatsClipboardFromSteps(steps, opts = {}) {
  const srcList = Array.isArray(steps) ? steps : [];
  const defaultDur = Number.isInteger(Number(opts.defaultDuration)) ? Number(opts.defaultDuration) : 2;
  const beats = [];

  for (const st of srcList) {
    if (!st) continue;
    const rawNotes = Array.isArray(st.notes)
      ? st.notes
      : (Number.isFinite(Number(st.s)) && Number.isFinite(Number(st.f))
          ? [{
              string: Number(st.s),
              fret: Number(st.f),
              hammerOn: st.tech === 'H' || st.tech === 'P' || st.tech === 'Hammer-on' || st.tech === 'Pull-off',
              slide: st.tech === 'S' || st.tech === 'Slide',
              tap: Boolean(st.tapped || st.tap)
            }]
          : (Number.isFinite(Number(st.string)) && Number.isFinite(Number(st.fret)) ? [st] : []));

    const seenStr = new Set();
    const notes = [];
    for (const n of rawNotes) {
      const s = Math.max(0, Math.min(7, Math.round(Number(n.string) || 0)));
      if (seenStr.has(s)) continue;
      seenStr.add(s);
      notes.push(createTabNote({
        string: s,
        fret: Math.max(0, Math.min(24, Math.round(Number(n.fret) || 0))),
        midi: Number.isFinite(Number(n.midi)) ? Math.round(Number(n.midi)) : undefined,
        velocity: Number(n.velocity) || 96,
        tap: Boolean(n.tap || n.tapped),
        hammerOn: Boolean(n.hammerOn),
        slide: Boolean(n.slide),
        harmonicType: Number(n.harmonicType) || (n.harmonic ? 1 : 0)
      }));
    }
    notes.sort((a, b) => a.string - b.string);
    beats.push(createBeat({
      duration: Number.isInteger(Number(st.duration)) ? Number(st.duration) : defaultDur,
      dots: Number(st.dots) || 0,
      tupletNumerator: Number(st.tupletNumerator) || 0,
      tupletDenominator: Number(st.tupletDenominator) || 0,
      isRest: notes.length === 0,
      chordSymbol: st.chordSymbol ? String(st.chordSymbol) : '',
      text: st.text ? String(st.text) : '',
      notes
    }));
  }

  return {
    kind: 'beats',
    scope: 'track',
    sourceTrackIdx: 0,
    beatCount: beats.length,
    barCount: 0,
    label: String(opts.label || ''),
    beats
  };
}

/**
 * Sprawdza, czy utwór zawiera struktury repetycji lub volt trybu Pro (`repeatOpen`, `repeatClose`, `voltaMask`).
 */
export function hasProRepeatStructure(song) {
  const master = song?.tracks?.[0]?.measures;
  if (!Array.isArray(master)) return false;
  return master.some(m => Boolean(m && (m.repeatOpen || (Number(m.repeatClose) || 0) >= 2 || (Number(m.voltaMask) || 0) > 0)));
}

/**
 * Rozwija repetycje i volty utworu Pro w prostą, liniową sekwencję taktów dla trybu Uproszczonego.
 */
export function expandSongToLinearMeasures(song) {
  const linearSong = cloneSong(song);
  const tl = expandPlaybackTimeline(song);
  if (!tl.length) {
    return { linearSong, sourceMap: [] };
  }

  const seenPrimary = new Set();
  const sourceMap = tl.map(entry => {
    const isPrimary = !seenPrimary.has(entry.measureIdx);
    seenPrimary.add(entry.measureIdx);
    return {
      sourceMeasureIdx: entry.measureIdx,
      pass: entry.pass || 1,
      isPrimary
    };
  });

  linearSong.tracks = (song.tracks || []).map(tr => {
    const measures = sourceMap.map(info => {
      const srcM = tr.measures?.[info.sourceMeasureIdx]
        || createEmptyMeasure(4, 4, 'rock_8th', tr.instrumentType === 'drums' || Boolean(tr.isPercussion));
      const copyM = cloneSong(srcM);
      copyM.repeatOpen = false;
      copyM.repeatClose = 0;
      copyM.voltaMask = 0;
      return copyM;
    });
    return { ...cloneSong(tr), measures };
  });

  return { linearSong, sourceMap };
}

function measureContentSignature(m) {
  if (!m) return '';
  return JSON.stringify({
    numerator: m.numerator || 4,
    denominator: m.denominator || 4,
    sectionMarker: m.sectionMarker || '',
    drumBlockId: m.drumBlockId || '',
    customDrumGrid: m.customDrumGrid || null,
    beats: (m.beats || []).map(b => ({
      duration: b.duration,
      dots: b.dots || 0,
      tupletNumerator: b.tupletNumerator || 0,
      tupletDenominator: b.tupletDenominator || 0,
      isRest: Boolean(b.isRest),
      dynamic: b.dynamic ?? 6,
      chordSymbol: b.chordSymbol || '',
      text: b.text || '',
      notes: (b.notes || []).map(n => ({
        s: n.string, f: n.fret, midi: n.midi,
        tied: Boolean(n.tied), pm: Boolean(n.palmMute), lr: Boolean(n.letRing),
        ho: Boolean(n.hammerOn), sl: n.slideType || '', vib: Boolean(n.vibrato),
        wv: Boolean(n.wideVibrato), bp: n.bendPreset || '', wp: n.whammyPreset || '',
        tap: Boolean(n.tap), ht: n.harmonicType || 0, dn: Boolean(n.deadNote),
        gn: Boolean(n.ghostNote), acc: Boolean(n.accented), ha: Boolean(n.heavyAccented),
        st: Boolean(n.staccato)
      }))
    }))
  });
}

function mergeProBeatMetadata(targetMeasure, origProMeasure) {
  if (!targetMeasure || !origProMeasure || !Array.isArray(targetMeasure.beats) || !Array.isArray(origProMeasure.beats)) return;
  const count = Math.min(targetMeasure.beats.length, origProMeasure.beats.length);
  for (let bIdx = 0; bIdx < count; bIdx++) {
    const tb = targetMeasure.beats[bIdx];
    const pb = origProMeasure.beats[bIdx];
    if (!tb || !pb) continue;
    if (pb.pickStroke) tb.pickStroke = pb.pickStroke;
    if (pb.brushStroke) tb.brushStroke = pb.brushStroke;
    if (pb.fermata) tb.fermata = true;
    if (tb.text === '' && pb.text) tb.text = pb.text;
    if (!tb.isRest && !pb.isRest && Array.isArray(tb.notes) && Array.isArray(pb.notes)) {
      for (const tn of tb.notes) {
        const pn = pb.notes.find(x => x.string === tn.string && x.fret === tn.fret) || pb.notes.find(x => x.string === tn.string);
        if (pn) {
          if (pn.graceNote) tn.graceNote = true;
          if (pn.tremoloPicking) tn.tremoloPicking = true;
          if (pn.trill) tn.trill = true;
        }
      }
    }
  }
}

/**
 * Scala zmiany wykonane w trybie Uproszczonym (na rozwiniętej partyturze) z powrotem do utworu Pro,
 * bezstratnie przywracając repetycje, volty oraz ukryte oznaczenia Pro (kostkowanie, przednutki, tryle, fermatę).
 */
export function reconcileLinearEditsIntoProSong(proSong, linearSong, sourceMap = [], baselineLinearSong = null) {
  if (!proSong || !linearSong) return { song: cloneSong(linearSong || proSong), foldedToPro: false };
  const mapList = Array.isArray(sourceMap) ? sourceMap : [];
  const proMasterLen = proSong.tracks?.[0]?.measures?.length || 0;
  const linMasterLen = linearSong.tracks?.[0]?.measures?.length || 0;

  if (mapList.length === 0 || linMasterLen !== mapList.length) {
    // Nie można zwinąć pętli, jeśli użytkownik dodał/usunął takty; zachowujemy przynajmniej ocalałe metadane Pro
    const outSong = cloneSong(linearSong);
    if (mapList.length > 0 && Array.isArray(outSong.tracks) && Array.isArray(proSong.tracks)) {
      for (let t = 0; t < outSong.tracks.length && t < proSong.tracks.length; t++) {
        const lMs = outSong.tracks[t].measures || [];
        const pMs = proSong.tracks[t].measures || [];
        for (let li = 0; li < lMs.length && li < mapList.length; li++) {
          const origM = pMs[mapList[li].sourceMeasureIdx];
          if (origM) mergeProBeatMetadata(lMs[li], origM);
        }
      }
    }
    return { song: outSong, foldedToPro: false };
  }

  // Sprawdź czy powtórzone przebiegi tej samej miary źródłowej nie rozeszły się wskutek różnych edycji
  const trackCount = Math.min(proSong.tracks?.length || 0, linearSong.tracks?.length || 0);
  const chosenLinearIdxByTrackAndSrc = Array.from({ length: trackCount }, () => new Map());

  for (let t = 0; t < trackCount; t++) {
    const lMs = linearSong.tracks[t].measures || [];
    const baseMs = baselineLinearSong?.tracks?.[t]?.measures || [];
    const grouped = new Map();
    for (let li = 0; li < mapList.length; li++) {
      const srcM = mapList[li].sourceMeasureIdx;
      if (!grouped.has(srcM)) grouped.set(srcM, []);
      grouped.get(srcM).push(li);
    }

    for (const [srcM, indices] of grouped.entries()) {
      const sigs = indices.map(li => measureContentSignature(lMs[li]));
      const baseSigs = indices.map(li => (baseMs[li] ? measureContentSignature(baseMs[li]) : sigs[0]));
      const editedFlags = sigs.map((s, k) => s !== baseSigs[k]);
      const editedSigs = new Set(sigs.filter((_, k) => editedFlags[k]));
      const allSameNow = new Set(sigs).size === 1;

      if (allSameNow) {
        chosenLinearIdxByTrackAndSrc[t].set(srcM, indices[0]);
      } else if (editedSigs.size === 1) {
        const idxEdited = indices.find((_, k) => editedFlags[k]);
        chosenLinearIdxByTrackAndSrc[t].set(srcM, idxEdited);
      } else {
        // Rozbieżne edycje między powtórzeniami tej samej miary -> zachowaj rozwiniętą wersję bez utraty edycji
        const outSong = cloneSong(linearSong);
        for (let tt = 0; tt < outSong.tracks.length && tt < proSong.tracks.length; tt++) {
          const outMs = outSong.tracks[tt].measures || [];
          const pMs = proSong.tracks[tt].measures || [];
          for (let li = 0; li < outMs.length && li < mapList.length; li++) {
            const origM = pMs[mapList[li].sourceMeasureIdx];
            if (origM) mergeProBeatMetadata(outMs[li], origM);
          }
        }
        return { song: outSong, foldedToPro: false };
      }
    }
  }

  const restored = cloneSong(proSong);
  restored.title = linearSong.title || restored.title;
  restored.artist = linearSong.artist ?? restored.artist;
  restored.album = linearSong.album ?? restored.album;
  restored.tempoBpm = linearSong.tempoBpm || restored.tempoBpm;
  restored.keyRootPc = linearSong.keyRootPc ?? restored.keyRootPc;
  restored.keyScaleId = linearSong.keyScaleId || restored.keyScaleId;
  restored.metronomePattern = cloneSong(linearSong.metronomePattern || restored.metronomePattern);

  for (let t = 0; t < linearSong.tracks.length; t++) {
    if (t >= restored.tracks.length) {
      restored.tracks.push(cloneSong(linearSong.tracks[t]));
      continue;
    }
    const proTr = proSong.tracks[t];
    const linTr = linearSong.tracks[t];
    const resTr = restored.tracks[t];
    resTr.name = linTr.name || resTr.name;
    resTr.tonePreset = linTr.tonePreset || resTr.tonePreset;
    resTr.muted = Boolean(linTr.muted);
    resTr.solo = Boolean(linTr.solo);
    resTr.volume = linTr.volume ?? resTr.volume;
    resTr.capo = linTr.capo ?? resTr.capo;
    resTr.drumAutoFill = linTr.drumAutoFill ?? resTr.drumAutoFill;
    resTr.drumCrashOnOne = linTr.drumCrashOnOne ?? resTr.drumCrashOnOne;

    for (let srcM = 0; srcM < proMasterLen; srcM++) {
      const li = chosenLinearIdxByTrackAndSrc[t]?.get(srcM);
      if (li === undefined) continue;
      const origProM = proTr.measures?.[srcM];
      const editedLinM = linTr.measures?.[li];
      if (!origProM || !editedLinM) continue;
      const mergedM = cloneSong(editedLinM);
      mergedM.repeatOpen = Boolean(origProM.repeatOpen);
      mergedM.repeatClose = Number(origProM.repeatClose) || 0;
      mergedM.voltaMask = Number(origProM.voltaMask) || 0;
      mergedM.tripletFeel = Number(origProM.tripletFeel) || 0;
      mergeProBeatMetadata(mergedM, origProM);
      resTr.measures[srcM] = mergedM;
    }
  }

  return { song: restored, foldedToPro: true };
}
