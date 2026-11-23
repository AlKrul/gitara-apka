/**
 * Muzyczne helpery dechordera: klasy wysokości, częstotliwości, nazwy nut.
 * Port z composeApp/.../music/Notes.kt (instrument-obojętny, działa też na gitarze).
 */

export const SHARP_NAMES = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
export const FLAT_NAMES = ['C', 'Db', 'D', 'Eb', 'E', 'F', 'Gb', 'G', 'Ab', 'A', 'Bb', 'B'];
export const COMMON_NAMES = ['C', 'C#', 'D', 'Eb', 'E', 'F', 'F#', 'G', 'Ab', 'A', 'Bb', 'B'];

const BLACK = new Set([1, 3, 6, 8, 10]);

const INTERVAL_NAMES = [
  'unison', 'minor second', 'major second', 'minor third', 'major third', 'perfect fourth',
  'tritone', 'perfect fifth', 'minor sixth', 'major sixth', 'minor seventh', 'major seventh',
];

export const Notes = {
  PIANO_LOWEST: 21,
  PIANO_HIGHEST: 108,

  pc(midi) { return ((midi % 12) + 12) % 12; },
  octave(midi) { return Math.floor(midi / 12) - 1; },
  isBlack(midi) { return BLACK.has(Notes.pc(midi)); },

  frequency(midi, a4 = 440.0) { return a4 * Math.pow(2, (midi - 69.0) / 12.0); },
  midiOf(freq, a4 = 440.0) { return 69.0 + 12.0 * Math.log(freq / a4) / Math.log(2.0); },

  name(pc) { return COMMON_NAMES[Notes.pc(pc)]; },
  nameWithOctave(midi) { return Notes.name(Notes.pc(midi)) + Notes.octave(midi); },
  intervalName(semitones) { return INTERVAL_NAMES[Notes.pc(semitones)]; },
};
