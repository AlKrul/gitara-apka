/**
 * Silnik dechorder (port do JS): polifoniczna detekcja nut + bogate nazywanie akordów.
 * Eksporty spójne z istniejącym window.GuitarDsp.
 */
export { Fft } from './fft.js';
export { Spectrum, Peak } from './spectrum.js';
export { NoteDetector, DetectedNote, DetectionParams } from './noteDetector.js';
export { ChordNamer, Chord, Quality, QUALITY_LIST } from './chordNamer.js';
export { ChordEngine, ActiveNote, matchChordFromMidis, readingLabel, WINDOW, HOP } from './chordEngine.js';
export { Notes } from './music.js';

import { ChordEngine } from './chordEngine.js';
import { ChordNamer, Quality } from './chordNamer.js';
import { NoteDetector, DetectionParams } from './noteDetector.js';
import { Spectrum, Peak } from './spectrum.js';
import { Fft } from './fft.js';
import { Notes } from './music.js';
import { matchChordFromMidis, readingLabel } from './chordEngine.js';

if (typeof window !== 'undefined') {
  window.DechorderDsp = {
    Fft,
    Spectrum,
    Peak,
    NoteDetector,
    DetectionParams,
    ChordNamer,
    Chord,
    Quality,
    ChordEngine,
    matchChordFromMidis,
    readingLabel,
    Notes,
  };
}
