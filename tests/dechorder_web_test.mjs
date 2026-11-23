import assert from 'node:assert/strict';
import { ChordNamer, Chord, matchChordFromMidis, readingLabel } from '../web/js/dsp/dechorder/index.js';
import { ChordEngine } from '../web/js/dsp/dechorder/chordEngine.js';
import { NoteDetector, DetectionParams } from '../web/js/dsp/dechorder/noteDetector.js';
import { Spectrum } from '../web/js/dsp/dechorder/spectrum.js';
import { Notes } from '../web/js/dsp/dechorder/music.js';

const SR = 44100;

function synthNote(buf, startSec, midi, amp, decayTauSec, harmonics = 6) {
  const f0 = 440 * Math.pow(2, (midi - 69) / 12);
  const startIdx = Math.floor(startSec * SR);
  for (let i = startIdx; i < buf.length; i++) {
    const t = (i - startIdx) / SR;
    const env = Math.min(1.0, t / 0.004) * Math.exp(-t / decayTauSec);
    if (t > 0.01 && env < 1e-5) break;
    let s = 0;
    for (let h = 1; h <= harmonics; h++) {
      const fh = f0 * h;
      if (fh >= SR * 0.45) break;
      s += (1 / h) * Math.sin(2 * Math.PI * fh * t);
    }
    buf[i] += amp * env * s;
  }
}

function runEngine(buf, engine, config) {
  let last = null;
  for (let off = 0; off < buf.length; off += 1024) {
    engine.push(buf.subarray(off, Math.min(buf.length, off + 1024)), Math.min(1024, buf.length - off), config, (st) => { last = st; });
  }
  return last;
}

// 1. Nazywanie akordów (bezpośrednio z MIDI)
{
  const t = (notes, weights, expected) => {
    const r = ChordNamer.read(notes, weights);
    assert.ok(r, `read(${notes}) -> null`);
    assert.equal(readingLabel(r), expected, `read(${notes}) = "${readingLabel(r)}", oczekiwano "${expected}"`);
  };
  t([60, 64, 67], null, 'C');          // C dur
  t([57, 60, 64], null, 'Am');         // A mol
  t([60, 64, 67, 71], null, 'Cmaj7');  // Cmaj7
  t([62, 65, 69], null, 'Dm');         // Dm
  t([52, 59], null, 'E5');             // E power chord (E+B)
  t([52, 55, 60], null, 'C/E');        // C/E (inwersja: E w basie)
  t([50, 53, 56], null, 'Ddim');       // D diminished
  t([60, 62, 67], null, 'Csus2');      // C sus2
  t([60, 65, 67], null, 'Csus4');      // C sus4
  t([60], null, 'C4');                 // pojedyncza nuta
  console.log('[OK] Test 1: ChordNamer (10 nazw, w tym inwersja C/E i power chord E5)');
}

// 2. matchChordFromMidis (helper hybrydowy)
{
  const r = matchChordFromMidis([60, 64, 67]);
  assert.equal(r.chord.symbol(), 'C');
  console.log('[OK] Test 2: matchChordFromMidis');
}

// 3. Detekcja polifoniczna przez cały potok (syntetyczny akord C dur)
{
  const engine = new ChordEngine();
  engine.begin(SR);
  const buf = new Float32Array(Math.floor(SR * 1.6));
  synthNote(buf, 0.15, 60, 0.35, 1.2); // C4
  synthNote(buf, 0.15, 64, 0.35, 1.2); // E4
  synthNote(buf, 0.15, 67, 0.35, 1.2); // G4
  const cfg = { a4: 440, sensitivity: 0.5, noiseGateDb: -60, changeDelayMs: 150 };
  const last = runEngine(buf, engine, cfg);
  assert.ok(last && last.reading, 'Brak odczytu akordu');
  assert.equal(last.reading.kind, 'chord');
  assert.equal(last.reading.chord.symbol(), 'C', `Oczekiwano "C", dostałem "${last.reading.chord.symbol()}"`);
  console.log('[OK] Test 3: ChordEngine wykrywa syntetyczny C dur');
}

// 4. Detekcja akordu molowego septymowego (Am7)
{
  const engine = new ChordEngine();
  engine.begin(SR);
  const buf = new Float32Array(Math.floor(SR * 1.6));
  [57, 60, 64, 67].forEach((m) => synthNote(buf, 0.15, m, 0.32, 1.1)); // A C E G
  const cfg = { a4: 440, sensitivity: 0.5, noiseGateDb: -60, changeDelayMs: 150 };
  const last = runEngine(buf, engine, cfg);
  assert.ok(last && last.reading && last.reading.kind === 'chord');
  assert.equal(last.reading.chord.symbol(), 'Am7', `Oczekiwano "Am7", dostałem "${last.reading.chord.symbol()}"`);
  console.log('[OK] Test 4: ChordEngine wykrywa syntetyczny Am7');
}

// 5. Piki widma + detektor nut na pojedynczym akordzie
{
  const spectrum = new Spectrum(SR, 8192);
  const buf = new Float32Array(8192);
  [60, 64, 67].forEach((m) => synthNote(buf, 0.0, m, 0.5, 10.0));
  spectrum.analyze(buf);
  const peaks = spectrum.peaks(38, 6000, 10.0, 50.0, -85.0);
  const detector = new NoteDetector();
  const params = new DetectionParams({ minPeakProminenceDb: 10, relativeThreshold: 0.22, minSalience: 20, minFundamentalDb: 6 });
  const notes = detector.detect(peaks, 440, params).map((n) => n.midi).sort((a, b) => a - b);
  assert.ok(notes.includes(60), `Brak C4 w [${notes}]`);
  assert.ok(notes.includes(64), `Brak E4 w [${notes}]`);
  assert.ok(notes.includes(67), `Brak G4 w [${notes}]`);
  console.log(`[OK] Test 5: NoteDetector znalazł nuty akordu: [${notes.join(', ')}]`);
}

// 6. Inwersja nie psuje nazwy (C/E)
{
  const r = matchChordFromMidis([52, 55, 60, 64]); // E3 G3 C4 E4
  assert.equal(r.chord.symbol(), 'C/E', `Oczekiwano "C/E", dostałem "${r.chord.symbol()}"`);
  console.log('[OK] Test 6: Inwersja C/E z basem E');
}

console.log('\nWszystkie testy dechordera przeszły pomyślnie.');
