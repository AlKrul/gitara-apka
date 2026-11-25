import assert from 'node:assert/strict';
import {
  DEFAULT_STUDIO_PREFS,
  MAX_STUDIO_TRACKS,
  NOTE_NAMES_FLAT,
  NOTE_NAMES_SHARP,
  addMeasure,
  addMeasureBefore,
  addTrack,
  analyzePhrasePositions,
  applyBeatPropToSelection,
  applyEffectToSelection,
  applyMetronomePreset,
  applyPhrasePosition,
  buildMeasuresFromSteps,
  buildSongExportPayload,
  clampStudioPrefs,
  clearMeasureAt,
  copyBeatsToClipboard,
  copyDrumGridToAllMeasures,
  copyMeasuresToClipboard,
  createBeat,
  createDemoSong,
  createEmptyMeasure,
  createEmptySong,
  createStudioHistory,
  createTabNote,
  cutBeatsToClipboard,
  cutMeasuresToClipboard,
  deleteBeatAt,
  deleteNoteAt,
  deleteSelectionNotes,
  deserializeStudioSong,
  detectKeyFromSongJs,
  duplicateMeasures,
  duplicateTrack,
  durationToQuarterNotes,
  expandPlaybackTimeline,
  getNoteAt,
  insertMidiNoteAt,
  isBeatInSelection,
  midiToNoteName,
  midiToNoteWithOctave,
  moveBeatInMeasure,
  moveNoteToAdjacentString,
  moveSelectionToAdjacentString,
  normalizeBeatSelection,
  normalizeMetronomePattern,
  noteToMidi,
  pasteBeatsFromClipboard,
  pasteMeasuresFromClipboard,
  removeMeasure,
  removeTrack,
  resetMeasureDrumGrid,
  serializeStudioSong,
  setCustomDrumCell,
  setCustomDrumResolution,
  setMeasureRepeatClose,
  setMeasureSectionMarker,
  setNoteAt,
  setNotePosition,
  setTrackTuning,
  shiftNotePitch,
  shiftSelectionPitch,
  toggleBeatProp,
  toggleMeasureRepeatOpen,
  toggleMeasureVolta,
  toggleNoteEffect,
  transposeRange,
  validateMeasureMeter
} from '../web/js/studio/studioModel.js';
import {
  STUDIO_COMMANDS,
  filterStudioCommands,
  renderCommandPaletteHtml,
  renderStudioPaletteHtml
} from '../web/js/studio/studioPalette.js';
import {
  createDurationGate,
  createFretInputBuffer,
  createStudioInputController,
  createStudioMicController,
  quantizeTimedNotesToMeasures,
  recomputeOfflineRecordedFrames
} from '../web/js/studio/studioInputs.js';
import { GuitarEventTracker } from '../web/js/dsp/pitchPleaseBridge.js';
import { findSoundFontZone, parseSoundFont2 } from '../web/js/studio/studioSoundFont.js';
import {
  DRUM_BLOCKS,
  GM_DRUM_MIDI,
  drumBlockToGridPattern,
  expandDrumBlockToSteps,
  expandDrumCustomGridToSteps
} from '../web/js/studio/studioDrumBlocks.js';
import {
  renderArrangerMatrixHtml,
  renderMetronomeBlocksHtml,
  renderStudioFretboardSvg,
  renderStudioTabSvg,
  writtenStaffMidi
} from '../web/js/studio/studioRenderer.js';
import {
  computeActiveAndNextNotes,
  createStudioPlayer
} from '../web/js/studio/studioPlayer.js';

// 1. Nazewnictwo nut A-G (bez H) i konwersje MIDI z kapodastrem oraz basem 4-str.
{
  assert.ok(!NOTE_NAMES_SHARP.includes('H'), 'NOTE_NAMES_SHARP cannot contain H');
  assert.ok(!NOTE_NAMES_FLAT.includes('H'), 'NOTE_NAMES_FLAT cannot contain H');
  assert.equal(midiToNoteName(59, 2), 'B');
  assert.equal(midiToNoteWithOctave(60, 2), 'C4');
  assert.equal(midiToNoteWithOctave(58, 1), 'Bb3');

  const guitar = { openStringMidis: [40, 45, 50, 55, 59, 64], capo: 2 };
  assert.equal(noteToMidi(guitar, createTabNote({ string: 0, fret: 3 })), 45); // E2 + 2 capo + 3 = A2 (45)

  const bass = { openStringMidis: [28, 33, 38, 43], capo: 0 };
  assert.equal(noteToMidi(bass, createTabNote({ string: 0, fret: 0 })), 28); // E1
  assert.equal(noteToMidi(bass, createTabNote({ string: 3, fret: 5 })), 48); // C3
  console.log('[OK] Test 1: Nazwy nut A-G (bez H), kapodaster i bas 4-strunowy');
}

// 2. Wartości rytmiczne 1/1..1/64, kropki, tuplety i walidacja metrum (4/4, 3/4, 6/8, 7/8)
{
  assert.equal(durationToQuarterNotes(createBeat({ duration: -2 })), 4);
  assert.equal(durationToQuarterNotes(createBeat({ duration: -1 })), 2);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 0 })), 1);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 1 })), 0.5);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 2 })), 0.25);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 3 })), 0.125);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 4 })), 0.0625);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 0, dots: 1 })), 1.5);
  assert.equal(durationToQuarterNotes(createBeat({ duration: 0, dots: 2 })), 1.75);
  assert.ok(Math.abs(durationToQuarterNotes(createBeat({ duration: 1, tupletNumerator: 3, tupletDenominator: 2 })) - 1 / 3) < 1e-6);
  assert.ok(Math.abs(durationToQuarterNotes(createBeat({ duration: 2, tupletNumerator: 5, tupletDenominator: 4 })) - 0.2) < 1e-6);

  const m44 = createEmptyMeasure(4, 4);
  assert.equal(validateMeasureMeter(m44).status, 'ok');
  const m34 = createEmptyMeasure(3, 4);
  assert.equal(validateMeasureMeter(m34).status, 'ok');
  const m68 = createEmptyMeasure(6, 8);
  assert.equal(validateMeasureMeter(m68).status, 'ok');
  const m78 = createEmptyMeasure(7, 8);
  assert.equal(validateMeasureMeter(m78).status, 'ok');

  m44.beats.pop();
  assert.equal(validateMeasureMeter(m44).status, 'under');
  m44.beats.push(createBeat({ duration: -1, isRest: true }));
  assert.equal(validateMeasureMeter(m44).status, 'over');
  console.log('[OK] Test 2: Wartości rytmiczne 1/1..1/64, kropki, tuplety i walidacja metrum');
}

// 3. Ścieżki (limit 5, min 1), takty, duplikacja, repetycje, volty i ochrona przed zapętleniem
{
  const song = createEmptySong({ measureCount: 4 });
  assert.equal(song.tracks.length, 2); // Gitara 1 + Perkusja
  assert.ok(addTrack(song, 'guitar2') >= 0);
  assert.ok(addTrack(song, 'bass') >= 0);
  assert.ok(addTrack(song, 'keys') >= 0);
  assert.equal(song.tracks.length, MAX_STUDIO_TRACKS);
  assert.equal(addTrack(song, 'guitar1'), -1); // Limit 5 ścieżek
  assert.equal(duplicateTrack(song, 0), -1);

  assert.equal(removeTrack(song, 4), true);
  assert.equal(song.tracks.length, 4);
  assert.equal(setTrackTuning(song.tracks[0], 'drop_d'), true);
  assert.deepEqual(song.tracks[0].openStringMidis, [38, 45, 50, 55, 59, 64]);

  while (song.tracks.length > 1) removeTrack(song, song.tracks.length - 1);
  assert.equal(removeTrack(song, 0), false); // Min 1 ścieżka zostaje

  // Test repetycji + volt (1. przejście gra takt 1, 2. przejście przeskakuje do taktu 2)
  const vSong = createEmptySong({ measureCount: 3 });
  const m0 = vSong.tracks[0].measures[0];
  const m1 = vSong.tracks[0].measures[1];
  const m2 = vSong.tracks[0].measures[2];
  m0.repeatOpen = true;
  m1.voltaMask = 1; // 1. zakończenie
  m1.repeatClose = 2;
  m2.voltaMask = 2; // 2. zakończenie
  const tl = expandPlaybackTimeline(vSong);
  assert.deepEqual(tl.map(e => e.measureIdx), [0, 1, 0, 2]);

  // Ochrona przed nieskończoną pętlą
  const infSong = createEmptySong({ measureCount: 1 });
  infSong.tracks[0].measures[0].repeatOpen = true;
  infSong.tracks[0].measures[0].repeatClose = 9999;
  const infTl = expandPlaybackTimeline(infSong);
  assert.equal(infTl.length, 512);

  addMeasure(vSong, 0);
  assert.equal(vSong.tracks[0].measures.length, 4);
  assert.equal(removeMeasure(vSong, 1), true);
  assert.equal(vSong.tracks[0].measures.length, 3);
  duplicateMeasures(vSong, 0, 1, 1, true, 0);
  assert.equal(vSong.tracks[0].measures.length, 5);
  console.log('[OK] Test 3: Zarządzanie ścieżkami, repetycje, volty i limit pętli');
}

// 4. Historia Undo/Redo (50 kroków) + serializacja .gitara.json + utwory demo
{
  const hist = createStudioHistory(50);
  let s = createEmptySong();
  for (let i = 0; i < 55; i++) {
    s.tempoBpm = 100 + i;
    hist.push(s);
  }
  assert.equal(hist.size, 50);
  const current = { ...s, tempoBpm: 999 };
  const u1 = hist.undo(current);
  assert.equal(u1.tempoBpm, 154);
  const r1 = hist.redo(u1);
  assert.equal(r1.tempoBpm, 999);

  const demo1 = createDemoSong('rock_riff_em');
  const demo2 = createDemoSong('blues_shuffle_a');
  for (const demo of [demo1, demo2]) {
    for (const tr of demo.tracks) {
      if (tr.instrumentType === 'drums') continue;
      tr.measures.forEach((m, idx) => {
        const v = validateMeasureMeter(m);
        assert.equal(v.status, 'ok', `Demo ${demo.title} track ${tr.name} bar ${idx + 1} meter invalid: ${JSON.stringify(v)}`);
      });
    }
  }

  const prefs = clampStudioPrefs({ detectSensitivity: 0.72, minNoteDurationMs: 110, liveRecMode: 'live' });
  const raw = serializeStudioSong(demo1, prefs);
  const loaded = deserializeStudioSong(raw);
  assert.equal(loaded.song.title, demo1.title);
  assert.equal(loaded.song.tracks.length, 4);
  assert.equal(loaded.studioPrefs.detectSensitivity, 0.72);
  assert.equal(loaded.studioPrefs.minNoteDurationMs, 110);
  assert.equal(loaded.studioPrefs.liveRecMode, 'live');
  assert.equal(clampStudioPrefs(null).detectSensitivity, DEFAULT_STUDIO_PREFS.detectSensitivity);
  assert.throws(() => deserializeStudioSong('{bad json'), /Nieprawidłowy plik projektu JSON/);
  console.log('[OK] Test 4: Undo/Redo 50 kroków, integralność dem i serializacja .gitara.json');
}

// 5. Klocki perkusyjne (10 stylów × różne metrum + Auto-Fill + Crash na 1)
{
  assert.equal(DRUM_BLOCKS.length, 10);
  const meters = [[4, 4], [3, 4], [6, 8], [7, 8]];
  for (const block of DRUM_BLOCKS) {
    for (const [num, den] of meters) {
      const capQ = num * (4 / den);
      const steps = expandDrumBlockToSteps(block.id, {
        numerator: num,
        denominator: den,
        isBarOne: true,
        isFillBar: true,
        crashOnOne: true
      });
      if (block.id === 'break') {
        assert.equal(steps.length, 0);
        continue;
      }
      assert.ok(steps.length > 0, `${block.id} in ${num}/${den} should emit steps`);
      for (const st of steps) {
        assert.ok(st.beatOffsetQ >= 0 && st.beatOffsetQ < capQ, `${block.id} offset ${st.beatOffsetQ} out of ${capQ}`);
        for (const h of st.hits) {
          assert.ok(h.midi >= 35 && h.midi <= 81, `GM drum midi ${h.midi} out of range`);
        }
      }
      const firstHits = steps[0].hits.map(h => h.midi);
      assert.ok(firstHits.includes(GM_DRUM_MIDI.crash), `${block.id} in ${num}/${den} missing Crash on 1`);
    }
  }
  console.log('[OK] Test 5: 10 klocków perkusyjnych dla 4/4, 3/4, 6/8, 7/8 + Fill + Crash');
}

// 6. Renderer SVG (TAB 6-str., Bas 4-str., Nuty+TAB, Gryf z następnym uderzeniem, Matryca)
{
  const demo = createDemoSong('rock_riff_em');
  const svgGuitar = renderStudioTabSvg(demo, { activeTrackIdx: 0, viewMode: 'tab', loopEnabled: true, loopStart: 0, loopEnd: 3 });
  assert.ok(svgGuitar.includes('studio-cursor-slot'), 'Missing orange slot cursor');
  assert.ok(svgGuitar.includes('studioPlayheadLine'), 'Missing playhead line');
  assert.ok(svgGuitar.includes('Intro') && svgGuitar.includes('Refren'), 'Missing section markers');

  const svgStaffTab = renderStudioTabSvg(demo, { activeTrackIdx: 1, viewMode: 'staff_tab' });
  assert.ok(svgStaffTab.includes('𝄞'), 'Missing treble clef in Nuty+TAB mode');

  const svgBass = renderStudioTabSvg(demo, { activeTrackIdx: 2, viewMode: 'staff_tab' });
  assert.ok(svgBass.includes('𝄢'), 'Missing bass clef for 4-string bass');

  const svgDrums = renderStudioTabSvg(demo, { activeTrackIdx: 3 });
  assert.ok(svgDrums.includes('CYM') && svgDrums.includes('KCK'), 'Missing 4-line drum notation');

  const bluesDemo = createDemoSong('blues_shuffle_a');
  const svgKeys = renderStudioTabSvg(bluesDemo, { activeTrackIdx: 2 });
  assert.ok(svgKeys.includes('A3') || svgKeys.includes('C#4'), 'Missing keys note labels');

  const { activeNotes, nextNotes } = computeActiveAndNextNotes(demo, 0, 0, 2);
  assert.ok(activeNotes.length > 0 && nextNotes.length > 0);
  const fbSvg = renderStudioFretboardSvg(demo.tracks[0], {
    activeNotes,
    nextNotes,
    candidatePositions: [{ string: 1, fret: 7 }],
    keyRootPc: 4
  });
  assert.ok(fbSvg.includes('stroke-dasharray="3.5,2.5"'), 'Missing dashed cyan outline for nextNotes');
  assert.ok(fbSvg.includes('studio-fb-candidate-ring'), 'Missing amber candidate ring hook');

  const drumPadsHtml = renderStudioFretboardSvg(demo.tracks[3], { activeHits: ['kick'], nextHits: ['snare'] });
  assert.ok(drumPadsHtml.includes('studio-drum-pad active') && drumPadsHtml.includes('studio-drum-pad next'));

  const matrixHtml = renderArrangerMatrixHtml(demo, { activeTrackIdx: 0 });
  assert.ok(matrixHtml.includes('Gitara 1') && matrixHtml.includes('Bas (4-str.') && matrixHtml.includes('studio-drum-select'));
  console.log('[OK] Test 6: Renderer SVG TAB, Nuty+TAB, Gryf z nextNotes i Matryca');
}

// 7. Player WebAudio (zegar, Count-In, prędkość 50/150%, Solo/Mute, pętla A-B + Auto-Tempo)
{
  let fakeNowSec = 10.0;
  const triggered = [];
  const stateObj = { guitarTonePreset: 'crunch', keyboardPreset: 'rhodes', a4ReferenceHz: 440 };
  const mockHost = {
    getState: () => stateObj,
    ensureAudioCtx: () => ({ currentTime: fakeNowSec }),
    getMasterAudioOut: () => ({}),
    pluckGuitarFrequency: (freq, dur, gain) => triggered.push({ type: 'guitar', t: fakeNowSec, freq, dur, gain }),
    playHarmonicBellFrequency: (freq, dur) => triggered.push({ type: 'harmonic', t: fakeNowSec, freq, dur }),
    playKeyboardPresetNote: (freq, dur) => triggered.push({ type: 'keys', t: fakeNowSec, freq, dur }),
    playDrumHit: (kind, vel) => triggered.push({ type: 'drum', t: fakeNowSec, kind, vel }),
    playMetronomeTickAudio: (isDown) => triggered.push({ type: 'metro', t: fakeNowSec, isDown })
  };

  let autoBumps = [];
  const song = createEmptySong({ bpm: 120, measureCount: 2 });
  song.tracks[0].measures[0].beats = [
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 0 })] }),
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 3 })] }),
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 5 })] }),
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 7 })] })
  ];
  song.tracks[0].measures[1].beats = [
    createBeat({ duration: -2, isRest: false, notes: [createTabNote({ string: 0, fret: 0 })] })
  ];

  const player = createStudioPlayer({
    host: mockHost,
    clockFn: () => fakeNowSec,
    onAutoTempoBump: ev => autoBumps.push(ev)
  });
  player.loadSong(song, 0);

  // Prędkość 50% / 100% / 150%
  player.setSpeedPercent(50);
  assert.equal(player.getEffectiveBpm(), 60);
  player.setSpeedPercent(150);
  assert.equal(player.getEffectiveBpm(), 180);
  player.setSpeedPercent(100);
  assert.equal(player.getEffectiveBpm(), 120);

  // Count-In 4 kliknięcia przed startem taktu 0 (120 BPM -> 0.5 s/klik, 2.0 s odliczania)
  player.setCountIn(true);
  player.setMetronome(false);
  player.play();
  assert.equal(triggered.filter(e => e.type === 'metro').length, 1);
  assert.equal(triggered.filter(e => e.type === 'guitar').length, 0);

  for (let t = 10.5; t <= 11.6; t += 0.5) {
    fakeNowSec = t;
    player.stepSchedulerForTest(fakeNowSec);
  }
  assert.equal(triggered.filter(e => e.type === 'metro').length, 4, 'Expected 4 count-in ticks before bar 0');
  assert.equal(triggered.filter(e => e.type === 'guitar').length, 0, 'Notes should not fire during count-in');

  // Teraz wchodzimy w takt 0 (od 12.0 s)
  fakeNowSec = 12.0;
  player.stepSchedulerForTest(fakeNowSec);
  assert.ok(triggered.some(e => e.type === 'guitar'), 'Expected guitar notes to start after count-in');

  // Test Solo/Mute: wyciszamy gitarę i sprawdzamy że gra tylko perkusja
  player.stop();
  triggered.length = 0;
  player.setCountIn(false);
  song.tracks[1].solo = true; // Solo na perkusję
  player.loadSong(song, 0);
  fakeNowSec = 20.0;
  player.play();
  fakeNowSec = 20.6;
  player.stepSchedulerForTest(fakeNowSec);
  assert.equal(triggered.filter(e => e.type === 'guitar').length, 0, 'Solo on drums must mute guitar');
  assert.ok(triggered.filter(e => e.type === 'drum').length > 0, 'Soloed drums must play');

  // Test Pętli A-B (takt 0..0 -> 2.0 s przy 120 BPM) + Auto-Tempo (+5 BPM na pętlę)
  player.stop();
  song.tracks[1].solo = false;
  player.loadSong(song, 0);
  player.setLoop({ enabled: true, start: 0, end: 0 });
  player.setAutoTempo(true, 6);
  fakeNowSec = 30.0;
  player.play();
  fakeNowSec = 32.05; // Przekroczenie końca 1-taktowej pętli (4 ćwierćnuty @ 120 BPM = 2.0 s)
  player.stepSchedulerForTest(fakeNowSec);
  assert.equal(autoBumps.length, 1, 'Expected Auto-Tempo bump after completing loop');
  assert.equal(autoBumps[0].newBpm, 126);
  player.destroy();
  console.log('[OK] Test 7: Player WebAudio (Count-In, prędkość, Solo/Mute, pętla A-B + Auto-Tempo)');
}

// 8. Faza 2: Wpisywanie wielodźwięków, insertMidiNoteAt i bufor dwucyfrowy 0-24 (450 ms)
{
  const song = createEmptySong({ measureCount: 2 });
  setNoteAt(song, 0, 0, 0, 0, 0);
  setNoteAt(song, 0, 0, 0, 1, 2);
  setNoteAt(song, 0, 0, 0, 2, 2);
  assert.equal(song.tracks[0].measures[0].beats[0].isRest, false);
  assert.equal(song.tracks[0].measures[0].beats[0].notes.length, 3);

  addTrack(song, 'bass');
  addTrack(song, 'keys');
  const bassRes = insertMidiNoteAt(song, 2, 0, 0, 33, 1); // A1 na strunie A basu = próg 0
  assert.deepEqual(bassRes, { inserted: true, string: 1, fret: 0, midi: 33 });
  const keysRes = insertMidiNoteAt(song, 3, 0, 0, 60);
  assert.deepEqual(keysRes, { inserted: true, string: 0, fret: 0, midi: 60 });

  let fakeMs = 1000;
  const fbuf = createFretInputBuffer(450, () => fakeMs);
  assert.equal(fbuf.pushDigit(1, 'slotA'), 1);
  fakeMs = 1250; // +250 ms <= 450 ms -> 12
  assert.equal(fbuf.pushDigit(2, 'slotA'), 12);
  fakeMs = 1350; // Nowa cyfra po złożeniu 12 -> 2
  assert.equal(fbuf.pushDigit(2, 'slotA'), 2);
  fakeMs = 1500; // 29 > 24 -> samo 9
  assert.equal(fbuf.pushDigit(9, 'slotA'), 9);
  fakeMs = 2200; // > 450 ms przerwy -> reset
  assert.equal(fbuf.pushDigit(5, 'slotA'), 5);
  console.log('[OK] Test 8: Wielodźwięki, insertMidiNoteAt (Gitara/Bas/Klawisze) i bufor progów 0-24');
}

// 9. Faza 2: Pełne techniki, struktura utworu oraz roundtrip serializacji
{
  const song = createEmptySong({ measureCount: 2 });
  setNoteAt(song, 0, 0, 0, 2, 7);
  toggleNoteEffect(song, 0, 0, 0, 2, 'palmMute');
  toggleNoteEffect(song, 0, 0, 0, 2, 'hammerOn');
  toggleNoteEffect(song, 0, 0, 0, 2, 'slideType', 'legato');
  toggleNoteEffect(song, 0, 0, 0, 2, 'bendPreset', 'bend_release');
  toggleNoteEffect(song, 0, 0, 0, 2, 'vibrato');
  toggleNoteEffect(song, 0, 0, 0, 2, 'accented');
  toggleNoteEffect(song, 0, 0, 0, 2, 'tap');
  toggleNoteEffect(song, 0, 0, 0, 2, 'harmonicType', 1);
  toggleBeatProp(song, 0, 0, 0, 'pickStroke', 1);
  toggleBeatProp(song, 0, 0, 0, 'fermata');
  toggleBeatProp(song, 0, 0, 0, 'text', 'Solo');

  toggleMeasureRepeatOpen(song, 0);
  setMeasureRepeatClose(song, 1, 3);
  toggleMeasureVolta(song, 1, 1);
  setMeasureSectionMarker(song, 0, 'Riff A');
  const beforeIdx = addMeasureBefore(song, 0);
  assert.equal(beforeIdx, 0);
  assert.equal(song.tracks[0].measures.length, 3);

  const raw = serializeStudioSong(song);
  const loaded = deserializeStudioSong(raw).song;
  const n = getNoteAt(loaded, 0, 1, 0, 2);
  assert.equal(n.palmMute, true);
  assert.equal(n.hammerOn, true);
  assert.equal(n.slideType, 'legato');
  assert.equal(n.bendPreset, 'bend_release');
  assert.equal(n.bendPoints.length, 3);
  assert.equal(n.accented, true);
  assert.equal(n.tap, true);
  assert.equal(n.harmonicType, 1);
  assert.equal(loaded.tracks[0].measures[1].beats[0].fermata, true);
  assert.equal(loaded.tracks[0].measures[1].beats[0].text, 'Solo');
  assert.equal(loaded.tracks[0].measures[2].repeatClose, 3);
  console.log('[OK] Test 9: Techniki, struktura i roundtrip JSON Fazy 2');
}

// 10. Faza 2: Przekształcenia (Alt+↑/↓, Shift+strzałki, moveBeat, transpose, detectKey)
{
  const song = createEmptySong({ measureCount: 2 });
  setNoteAt(song, 0, 0, 0, 0, 5); // Struna E6 próg 5 = A2 (MIDI 45)
  const up = moveNoteToAdjacentString(song, 0, 0, 0, 0, 1);
  assert.equal(up.moved, true);
  assert.equal(up.newString, 1);
  assert.equal(up.newFret, 0);
  assert.equal(noteToMidi(song.tracks[0], getNoteAt(song, 0, 0, 0, 1)), 45);

  // Blokada kolizji gdy docelowa struna jest już zajęta
  setNoteAt(song, 0, 0, 0, 0, 5);
  const blocked = moveNoteToAdjacentString(song, 0, 0, 0, 0, 1);
  assert.equal(blocked.moved, false);
  assert.equal(blocked.reason, 'string-occupied');

  // Shift+strzałki: +1 półton i +12 oktawa (z przejściem na wyższą strunę przy końcu gryfu)
  setNoteAt(song, 0, 0, 1, 0, 20);
  assert.equal(shiftNotePitch(song, 0, 0, 1, 0, 1), true);
  assert.equal(getNoteAt(song, 0, 0, 1, 0).fret, 21);
  assert.equal(shiftNotePitch(song, 0, 0, 1, 0, 12), true); // 21+12 > 24 -> przenosi na wyższą strunę
  const movedOct = song.tracks[0].measures[0].beats[1].notes[0];
  assert.ok(movedOct.string > 0 && movedOct.fret <= 24);

  const swappedIdx = moveBeatInMeasure(song, 0, 0, 0, 1);
  assert.equal(swappedIdx, 1);
  assert.ok(transposeRange(song, { trackIdx: 0, startMeasure: 0, endMeasure: 0, semitones: 2 }) >= 2);

  const demo = createDemoSong('rock_riff_em');
  const key = detectKeyFromSongJs(demo);
  assert.equal(key.rootPc, 4); // E
  assert.equal(key.scaleId, 'natural_minor');
  console.log('[OK] Test 10: Przekształcenia Alt+↑/↓, Shift+strzałki, transpozycja i detekcja tonacji');
}

// 11. Faza 2: Schowek wielościeżkowy bez modali oraz 3-poziomowe kasowanie
{
  const song = createEmptySong({ measureCount: 3 });
  setNoteAt(song, 0, 0, 0, 0, 7);
  setNoteAt(song, 0, 0, 0, 1, 9);
  song.tracks[1].measures[0].drumBlockId = 'hard_rock';

  // Kopiuj wszystkie ścieżki, wklej w trybie 'insert'
  const clipAll = copyMeasuresToClipboard(song, { startIdx: 0, endIdx: 0, scope: 'all', activeTrackIdx: 0 });
  assert.equal(clipAll.barCount, 1);
  assert.equal(pasteMeasuresFromClipboard(song, clipAll, { targetMeasureIdx: 1, mode: 'insert' }), true);
  assert.equal(song.tracks[0].measures.length, 4);
  assert.equal(song.tracks[1].measures.length, 4);
  assert.equal(song.tracks[1].measures[2].drumBlockId, 'hard_rock');
  assert.equal(getNoteAt(song, 0, 2, 0, 0).fret, 7);

  // Wytnij tylko 1 ścieżkę (czyści gitarę, zachowuje perkusję)
  const cutOne = cutMeasuresToClipboard(song, { startIdx: 0, endIdx: 0, scope: 'track', activeTrackIdx: 0 });
  assert.equal(cutOne.scope, 'track');
  assert.equal(song.tracks[0].measures[0].beats[0].isRest, true);
  assert.equal(song.tracks[1].measures[0].drumBlockId, 'hard_rock');

  // 3-poziomowe kasowanie: Note -> Beat -> Bar
  assert.equal(deleteNoteAt(song, 0, 2, 0, 0), true);
  assert.ok(getNoteAt(song, 0, 2, 0, 1)); // Nuta na strunie 1 przetrwała
  assert.equal(deleteNoteAt(song, 0, 2, 0, 1), true);
  assert.equal(song.tracks[0].measures[2].beats[0].isRest, true);

  const prevBeatsLen = song.tracks[0].measures[2].beats.length;
  deleteBeatAt(song, 0, 2, 0);
  assert.equal(song.tracks[0].measures[2].beats.length, prevBeatsLen - 1);

  setNoteAt(song, 0, 2, 0, 0, 5);
  clearMeasureAt(song, 0, 2);
  assert.equal(song.tracks[0].measures[2].beats.every(b => b.isRest), true);
  console.log('[OK] Test 11: Schowek wielościeżkowy (1 ścieżka/Wszystkie, Nadpisz/Wstaw) i 3-poziomowe kasowanie');
}

// 12. Faza 2: studioPalette.js (render palety Strefy B i wyszukiwarka Ctrl+K)
{
  assert.ok(STUDIO_COMMANDS.length >= 50);
  const pmMatches = filterStudioCommands('palm mute');
  assert.ok(pmMatches.some(c => c.id === 'effect:palmMute'));
  const shortcutMatches = filterStudioCommands('Alt+↑');
  assert.ok(shortcutMatches.some(c => c.id === 'transform:strUp'));

  const song = createEmptySong({ measureCount: 2 });
  setNoteAt(song, 0, 0, 0, 0, 3, { palmMute: true });
  const palHtml = renderStudioPaletteHtml({
    track: song.tracks[0],
    beat: song.tracks[0].measures[0].beats[0],
    note: getNoteAt(song, 0, 0, 0, 0),
    measure: song.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    clipboardScope: 'all',
    clipboardPasteMode: 'insert',
    inlinePrompt: { type: 'section', value: 'Intro' }
  });
  assert.ok(palHtml.includes('studio-rail-palette'), 'Right rail palette container missing');
  assert.ok(palHtml.includes('studio-pal-btn active'), 'Active technique button must be highlighted');
  assert.ok(palHtml.includes('data-tip-name=') && palHtml.includes('data-tip-kbd='), 'Hover tooltip name and shortcut data attributes missing');
  assert.ok(palHtml.includes('studio-inline-bar') && palHtml.includes('Refren'), 'Inline section bar missing');

  const cmdkHtml = renderCommandPaletteHtml({ open: true, query: 'bend', selectedIdx: 0 });
  assert.ok(cmdkHtml.includes('studio-cmdk-dialog') && cmdkHtml.includes('Bend +1 ton'));
  console.log('[OK] Test 12: Paleta Songsterra w prawym panelu z dymkami, pasek inline bez modali i wyszukiwarka Ctrl+K');
}

// 13. Faza 2: studioInputs.js (router skrótów klawiaturowych + wykonanie komend)
{
  let ui = {
    song: createEmptySong({ measureCount: 2 }),
    history: createStudioHistory(50),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    loopEnabled: false,
    loopStart: 0,
    loopEnd: 1,
    clipboard: null,
    clipboardScope: 'track',
    clipboardPasteMode: 'overwrite',
    selectionStart: null,
    selectionEnd: null,
    inlinePrompt: null,
    cmdPalette: { open: false, query: '', selectedIdx: 0 }
  };
  let nowVal = 5000;
  let rendered = 0;
  const ctrl = createStudioInputController({
    getState: () => ui,
    pushUndo: () => ui.history.push(ui.song),
    onSongChanged: () => {},
    onRender: () => { rendered++; },
    nowFn: () => nowVal
  });

  const keyEv = (key, mods = {}) => ({ key, ...mods, preventDefault() {} });

  // Wpisz 12 na strunie 0 w oknie 450 ms, następnie P (Palm Mute), Shift+Right (+1 półton -> 13), Alt+ArrowUp (struna 1 -> próg 8)
  assert.equal(ctrl.handleKeyDown(keyEv('1')), true);
  nowVal += 120;
  assert.equal(ctrl.handleKeyDown(keyEv('2')), true);
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 0).fret, 12);

  ctrl.handleKeyDown(keyEv('p'));
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 0).palmMute, true);

  ctrl.handleKeyDown(keyEv('ArrowRight', { shiftKey: true }));
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 0).fret, 13);

  ctrl.handleKeyDown(keyEv('ArrowUp', { altKey: true }));
  assert.equal(ui.cursor.stringIdx, 1);
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 1).fret, 8);

  // Ctrl+Z cofa ostatnią zmianę
  ctrl.handleKeyDown(keyEv('z', { ctrlKey: true }));
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 0).fret, 13);

  // Ctrl+Y ponawia
  ctrl.handleKeyDown(keyEv('y', { ctrlKey: true }));
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 1).fret, 8);

  // Ctrl+C -> Ctrl+Shift+V (wklej jako nowy takt)
  ctrl.handleKeyDown(keyEv('c', { ctrlKey: true }));
  ctrl.handleKeyDown(keyEv('v', { ctrlKey: true, shiftKey: true }));
  assert.equal(ui.song.tracks[0].measures.length, 3);

  // Nawigacja na sam koniec i ArrowRight -> automatycznie dokłada nowy takt
  ui.cursor.measureIdx = 2;
  ui.cursor.beatIdx = ui.song.tracks[0].measures[2].beats.length - 1;
  ctrl.handleKeyDown(keyEv('ArrowRight'));
  assert.equal(ui.song.tracks[0].measures.length, 4);

  // Ctrl+K otwiera wyszukiwarkę, Escape zamyka
  ctrl.handleKeyDown(keyEv('k', { ctrlKey: true }));
  assert.equal(ui.cmdPalette.open, true);
  ctrl.handleKeyDown(keyEv('Escape'));
  assert.equal(ui.cmdPalette.open, false);
  assert.ok(rendered > 0);
  console.log('[OK] Test 13: Router skrótów studioInputs.js (0-24, techniki, przekształcenia, schowek, Undo/Redo, Ctrl+K)');
}

// 14. Faza 3: Odrębna czułość Studia TAB (> 0.68 zachowane przy autoAdapt) + zapis/przywrócenie globalnego progu
{
  const tracker = new GuitarEventTracker({ thresholdOn: 0.82, autoAdapt: true });
  tracker.adaptLowFrames = 12;
  tracker.emaLowPeak = 1.0;
  const eff = tracker.effectiveThresholds();
  assert.ok(eff.lowOn > 0.68, `Expected lowOn > 0.68 when thresholdOn=0.82, got ${eff.lowOn}`);

  const globalCfg = { thresholdOn: 0.58, highThresholdOn: 0.42, autoAdapt: true };
  const fakeDet = {
    cfg: { ...globalCfg },
    getConfig() { return { ...this.cfg }; },
    updateConfig(next) { Object.assign(this.cfg, next); }
  };
  let ui = {
    song: createEmptySong({ measureCount: 2 }),
    history: createStudioHistory(10),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    studioPrefs: clampStudioPrefs({ detectSensitivity: 0.80 })
  };
  const mic = createStudioMicController({
    getState: () => ui,
    pushUndo: () => {},
    getDetector: () => fakeDet,
    onRender: () => {}
  });
  await mic.setMode('step');
  assert.equal(fakeDet.cfg.thresholdOn, 0.80);
  assert.equal(fakeDet.cfg.highThresholdOn, 0.64);
  await mic.setMode('off');
  assert.equal(fakeDet.cfg.thresholdOn, 0.58);
  assert.equal(fakeDet.cfg.highThresholdOn, 0.42);
  console.log('[OK] Test 14: Czułość Studia TAB + nadpisanie i bezwarunkowe przywrócenie globalnego PitchPlease');
}

// 15. Faza 3: Bramka minimalnego czasu trwania nuty (odrzucanie strzałów szumu < minNoteDurationMs)
{
  const gate = createDurationGate(80);
  // Krótki strzał 35 ms -> musi zostać odrzucony
  assert.equal(gate.processFrame([{ type: 'noteOn', midiNote: 64, vel: 90 }], [], 100).length, 0);
  assert.equal(gate.processFrame([{ type: 'noteOff', midiNote: 64, durationMs: 35 }], [], 135).length, 0);
  assert.equal(gate.getRejectedCount(), 1);

  // Dłuższa nuta >= 80 ms -> zostaje przyjęta natychmiast po przekroczeniu progu
  assert.equal(gate.processFrame([{ type: 'noteOn', midiNote: 60, vel: 100 }], [], 200).length, 0);
  const acc = gate.processFrame([], [{ midiNote: 60, vel: 100, durationMs: 88 }], 288);
  assert.equal(acc.length, 1);
  assert.equal(acc[0].midiNote, 60);
  gate.processFrame([{ type: 'noteOff', midiNote: 60, durationMs: 220 }], [], 420);
  assert.equal(gate.getRejectedCount(), 1);
  console.log('[OK] Test 15: Bramka czasu trwania createDurationGate(80 ms)');
}

// 16. Faza 3: Nagrywanie Offline (przeliczanie PitchPlease z nową czułością) i kwantyzacja do taktów
{
  const track = createEmptySong({ measureCount: 1 }).tracks[0];
  const makeProbs = (entries) => {
    const arr = new Float32Array(88);
    for (const [midi, prob] of entries) {
      arr[midi - 21] = prob;
    }
    return arr;
  };
  const frames = [];
  // 0.00-0.025 s: krótki trzask MIDI 40 (poniżej 80 ms)
  for (let i = 0; i < 3; i++) frames.push({ time: i * 0.011, probs: makeProbs([[40, 0.78]]), notes: [] });
  for (let i = 3; i < 40; i++) frames.push({ time: i * 0.011, probs: makeProbs([]), notes: [] });
  // 0.50-0.70 s: silna nuta A2 (MIDI 45, prob 0.88)
  for (let i = 45; i < 65; i++) frames.push({ time: i * 0.011, probs: makeProbs([[45, 0.88]]), notes: [] });
  // 1.00-1.20 s: słabsza nuta D3 (MIDI 50, prob 0.64)
  for (let i = 90; i < 110; i++) frames.push({ time: i * 0.011, probs: makeProbs([[50, 0.64]]), notes: [] });

  const loose = recomputeOfflineRecordedFrames(frames, {
    sensitivity: 0.55,
    highThresholdOn: 0.40,
    minDurationMs: 80,
    tempoBpm: 120,
    numerator: 4,
    denominator: 4,
    gridDuration: 0,
    track
  });
  assert.ok(loose.noteRefs.some(n => n.midi === 45) && loose.noteRefs.some(n => n.midi === 50), 'Should keep both A2 and D3 at sensitivity 0.55');
  assert.equal(validateMeasureMeter(loose.measures[0]).status, 'ok');

  const strict = recomputeOfflineRecordedFrames(frames, {
    sensitivity: 0.76,
    highThresholdOn: 0.68,
    minDurationMs: 80,
    tempoBpm: 120,
    numerator: 4,
    denominator: 4,
    gridDuration: 0,
    track
  });
  assert.ok(strict.noteRefs.some(n => n.midi === 45), 'Strong A2 kept at sensitivity 0.76');
  assert.ok(!strict.noteRefs.some(n => n.midi === 50), 'Weaker D3 filtered out at sensitivity 0.76');
  console.log('[OK] Test 16: Offline PitchPlease przeliczanie czułości + kwantyzacja taktów');
}

// 17. Faza 3: Auto-place Step-Input, wybór kandydata struna/próg oraz analizator pozycji (Pozycja 1/2/3...)
{
  let ui = {
    song: createEmptySong({ measureCount: 2 }),
    history: createStudioHistory(10),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    studioPrefs: clampStudioPrefs({ minNoteDurationMs: 80 }),
    phrasePositions: [],
    selectedPhrasePosIdx: -1,
    positionAreaRange: null,
    lastPlacedNote: null
  };
  const mic = createStudioMicController({
    getState: () => ui,
    pushUndo: () => ui.history.push(ui.song),
    onSongChanged: () => {},
    onRender: () => {}
  });
  await mic.setMode('step');
  mic.onPitchFrame({
    events: [{ type: 'noteOn', midiNote: 52, vel: 98 }],
    activeNotes: [{ midiNote: 52, vel: 98, durationMs: 95 }],
    frameTimeSec: 0.2
  });
  mic._flushStepChordForTest();
  const placed = getNoteAt(ui.song, 0, 0, 0, 2); // najniższy próg dla E3 = struna 2 (D3), próg 2
  assert.ok(placed && placed.fret === 2);
  assert.equal(ui.cursor.beatIdx, 1);

  // Zmiana kandydata z popovera na strunę 0, próg 12
  assert.equal(mic.selectCandidateForNote(0, 12), true);
  assert.equal(getNoteAt(ui.song, 0, 0, 0, 0).fret, 12);

  // Analizator pozycji dla sekwencji [E3, G3, A3] zwraca wiele pozycji i zachowuje wysokości MIDI po zmianie
  const songPos = createEmptySong({ measureCount: 1 });
  setNoteAt(songPos, 0, 0, 0, 2, 2); // E3 = 52
  setNoteAt(songPos, 0, 0, 1, 3, 0); // G3 = 55
  setNoteAt(songPos, 0, 0, 2, 3, 2); // A3 = 57
  const positions = analyzePhrasePositions(songPos.tracks[0], null, { boxSpan: 4, maxOptions: 5 });
  assert.ok(positions.length >= 2, `Expected >= 2 hand positions, got ${positions.length}`);
  const highPos = positions[positions.length - 1];
  assert.ok(highPos.minFret >= 5);
  applyPhrasePosition(songPos, 0, highPos);
  const tr = songPos.tracks[0];
  assert.equal(noteToMidi(tr, tr.measures[0].beats[0].notes[0]), 52);
  assert.equal(noteToMidi(tr, tr.measures[0].beats[1].notes[0]), 55);
  assert.equal(noteToMidi(tr, tr.measures[0].beats[2].notes[0]), 57);
  console.log('[OK] Test 17: Step-Input auto-place, wybór kandydata i analizator pozycji fraz');
}

// 18. SoundFont2 (.sf2): minimalny parser RIFF sfbk i wyszukiwanie stref (bank 0 + perkusja bank 128)
{
  function buildMinimalSf2Buffer() {
    const enc = new TextEncoder();
    const mkChunk = (id, payloadBytes) => {
      const pad = payloadBytes.byteLength & 1;
      const out = new Uint8Array(8 + payloadBytes.byteLength + pad);
      const dv = new DataView(out.buffer);
      for (let i = 0; i < 4; i++) out[i] = id.charCodeAt(i);
      dv.setUint32(4, payloadBytes.byteLength, true);
      out.set(payloadBytes, 8);
      return out;
    };
    const mkList = (listType, chunks) => {
      const innerLen = 4 + chunks.reduce((a, c) => a + c.byteLength, 0);
      const out = new Uint8Array(8 + innerLen);
      const dv = new DataView(out.buffer);
      out.set(enc.encode('LIST'), 0);
      dv.setUint32(4, innerLen, true);
      out.set(enc.encode(listType), 8);
      let off = 12;
      for (const c of chunks) { out.set(c, off); off += c.byteLength; }
      return out;
    };
    const strFixed = (txt, len) => {
      const b = new Uint8Array(len);
      b.set(enc.encode(txt).slice(0, len));
      return b;
    };

    const inam = mkChunk('INAM', enc.encode('TestKit\0'));
    const infoList = mkList('INFO', [inam]);

    const smplBytes = new Uint8Array(64 * 2);
    const smplDv = new DataView(smplBytes.buffer);
    for (let i = 0; i < 64; i++) smplDv.setInt16(i * 2, Math.round(Math.sin(i / 4) * 16000), true);
    const sdtaList = mkList('sdta', [mkChunk('smpl', smplBytes)]);

    // 2 presety (0:25 Guitar, 128:0 Drums) + EOP
    const phdr = new Uint8Array(38 * 3);
    const pdv = new DataView(phdr.buffer);
    phdr.set(strFixed('Steel Guitar', 20), 0); pdv.setUint16(20, 25, true); pdv.setUint16(22, 0, true); pdv.setUint16(24, 0, true);
    phdr.set(strFixed('Standard Kit', 20), 38); pdv.setUint16(38 + 20, 0, true); pdv.setUint16(38 + 22, 128, true); pdv.setUint16(38 + 24, 1, true);
    phdr.set(strFixed('EOP', 20), 76); pdv.setUint16(76 + 24, 2, true);

    const pbag = new Uint8Array(4 * 3);
    const pbDv = new DataView(pbag.buffer);
    pbDv.setUint16(0, 0, true); pbDv.setUint16(4, 1, true); pbDv.setUint16(8, 2, true);

    const pgen = new Uint8Array(4 * 2);
    const pgDv = new DataView(pgen.buffer);
    pgDv.setUint16(0, 41, true); pgDv.setUint16(2, 0, true); // inst 0
    pgDv.setUint16(4, 41, true); pgDv.setUint16(6, 1, true); // inst 1

    const inst = new Uint8Array(22 * 3);
    const idv = new DataView(inst.buffer);
    inst.set(strFixed('SteelInst', 20), 0); idv.setUint16(20, 0, true);
    inst.set(strFixed('DrumInst', 20), 22); idv.setUint16(22 + 20, 1, true);
    inst.set(strFixed('EOI', 20), 44); idv.setUint16(44 + 20, 2, true);

    const ibag = new Uint8Array(4 * 3);
    const ibDv = new DataView(ibag.buffer);
    ibDv.setUint16(0, 0, true); ibDv.setUint16(4, 1, true); ibDv.setUint16(8, 2, true);

    const igen = new Uint8Array(4 * 2);
    const igDv = new DataView(igen.buffer);
    igDv.setUint16(0, 53, true); igDv.setUint16(2, 0, true); // sample 0
    igDv.setUint16(4, 53, true); igDv.setUint16(6, 1, true); // sample 1

    const shdr = new Uint8Array(46 * 3);
    const sdv = new DataView(shdr.buffer);
    shdr.set(strFixed('SmpGtr', 20), 0);
    sdv.setUint32(20, 0, true); sdv.setUint32(24, 32, true); sdv.setUint32(28, 4, true); sdv.setUint32(32, 28, true);
    sdv.setUint32(36, 44100, true); sdv.setUint8(40, 60); sdv.setUint16(44, 1, true);
    shdr.set(strFixed('SmpKick', 20), 46);
    sdv.setUint32(46 + 20, 32, true); sdv.setUint32(46 + 24, 64, true); sdv.setUint32(46 + 28, 32, true); sdv.setUint32(46 + 32, 60, true);
    sdv.setUint32(46 + 36, 44100, true); sdv.setUint8(46 + 40, 36); sdv.setUint16(46 + 44, 1, true);
    shdr.set(strFixed('EOS', 20), 92);

    const pdtaList = mkList('pdta', [
      mkChunk('phdr', phdr), mkChunk('pbag', pbag), mkChunk('pgen', new Uint8Array(0)), mkChunk('pgen', pgen).slice(0, 0) // wait clean pmod:
    ].slice(0, 2));
    // Reassemble exact pdta cleanly including pmod/imod:
    const exactPdta = mkList('pdta', [
      mkChunk('phdr', phdr),
      mkChunk('pbag', pbag),
      mkChunk('pmod', new Uint8Array(10)),
      mkChunk('pgen', pgen),
      mkChunk('inst', inst),
      mkChunk('ibag', ibag),
      mkChunk('imod', new Uint8Array(10)),
      mkChunk('igen', igen),
      mkChunk('shdr', shdr)
    ]);
    void pdtaList;

    const totalSize = 4 + infoList.byteLength + sdtaList.byteLength + exactPdta.byteLength;
    const riff = new Uint8Array(8 + totalSize);
    const rdv = new DataView(riff.buffer);
    riff.set(enc.encode('RIFF'), 0);
    rdv.setUint32(4, totalSize, true);
    riff.set(enc.encode('sfbk'), 8);
    let pos = 12;
    for (const part of [infoList, sdtaList, exactPdta]) {
      riff.set(part, pos);
      pos += part.byteLength;
    }
    return riff.buffer;
  }

  const sf2 = parseSoundFont2(buildMinimalSf2Buffer());
  assert.equal(sf2.name, 'TestKit');
  assert.equal(sf2.presets.length, 2);
  const zMelodic = findSoundFontZone(sf2, 0, 25, 64, 100);
  assert.ok(zMelodic && zMelodic.sampleName === 'SmpGtr' && zMelodic.rootKey === 60 && zMelodic.sampleRate === 44100);
  const zDrum = findSoundFontZone(sf2, 128, 0, 36, 110);
  assert.ok(zDrum && zDrum.sampleName === 'SmpKick' && zDrum.bank === 128);
  assert.throws(() => parseSoundFont2(new Uint8Array(16).buffer), /RIFF\/sfbk/);
  console.log('[OK] Test 18: Parser SoundFont2 (.sf2 RIFF sfbk) i strefy melodyczne/perkusyjne');
}

// 19. Eksport payloadu JSON (Phase 4) i import kroków z innych zakładek (buildMeasuresFromSteps)
{
  const arpSteps = Array.from({ length: 8 }, (_, i) => ({ notes: [{ string: i % 3, fret: 5 + i }], duration: 1 }));
  const measures = buildMeasuresFromSteps(arpSteps, { numerator: 4, denominator: 4, defaultDuration: 1, sectionMarker: 'Arp' });
  assert.equal(measures.length, 1);
  assert.equal(measures[0].sectionMarker, 'Arp');
  assert.equal(validateMeasureMeter(measures[0]).status, 'ok');

  const demo = createDemoSong('rock_riff_em');
  const payload = buildSongExportPayload(demo);
  assert.equal(payload.tracks.length, 4);
  const drumTr = payload.tracks.find(t => t.isPercussion);
  assert.ok(drumTr && drumTr.measures.length === 8);
  assert.ok(drumTr.measures[0].beats.some(b => !b.isRest && b.notes.some(n => n.fret >= 35 && n.fret <= 81)));
  console.log('[OK] Test 19: buildMeasuresFromSteps oraz buildSongExportPayload dla eksportu GP5/MIDI');
}

// 20. Klockowy metronom akcentów (A / B / C × uderzenia + podpodziały ×2/×3 + wyciszenie Gap)
{
  const patNorm = normalizeMetronomePattern({ subdivisions: 2, levels: [3, 2, 0] }, 4);
  assert.equal(patNorm.subdivisions, 2);
  assert.deepEqual(patNorm.levels, [3, 2, 0, 1]);

  const rock4 = applyMetronomePreset('rock', 4, 1);
  assert.deepEqual(rock4.levels, [3, 1, 2, 1]);
  const gap3 = applyMetronomePreset('gap', 3, 3);
  assert.deepEqual(gap3.levels, [3, 1, 0]);
  assert.equal(gap3.subdivisions, 3);

  const metroHtml = renderMetronomeBlocksHtml(gap3, 3, { playingBeatCol: 1 });
  assert.ok(metroHtml.includes('studio-metro-panel') && metroHtml.includes('playing-col'));
  assert.ok(metroHtml.includes('lvl-a on') && metroHtml.includes('lvl-c on'));

  // Sprawdź harmonogram metronomu w Playerze (pominięcie wyciszonego uderzenia 0 + podpodział ×2)
  const ticks = [];
  const song = createEmptySong({ measureCount: 1, bpm: 120 });
  song.metronomePattern = { subdivisions: 2, subAccent: 1, levels: [3, 0, 2, 1] };
  let simSec = 0;
  const pl = createStudioPlayer({
    host: {
      ensureAudioCtx: () => ({}),
      playMetronomeTickAudio: (isDown) => ticks.push({ t: Math.round(simSec * 1000), isDown })
    },
    clockFn: () => simSec
  });
  pl.loadSong(song, 0);
  pl.setMetronome(true);
  pl.play();
  for (let ms = 0; ms <= 1650; ms += 50) {
    simSec = ms / 1000;
    pl.stepSchedulerForTest(simSec);
  }
  pl.stop();
  // 1 takt 4/4 @ 120 BPM: beat 0 (lvl 3, isDown=true), 4 podpodziały (lvl 0.5), beat 2 (lvl 2), beat 3 (lvl 1) => 7 kliknięć (beat 1 wyciszony)
  assert.equal(ticks.length, 7, `Expected 7 metronome clicks (3 main + 4 subdivs), got ${ticks.length}`);
  assert.equal(ticks[0].isDown, true);
  console.log('[OK] Test 20: Klockowy metronom akcentów A/B/C, presety, Gap i podpodziały w Playerze');
}

// 21. 5-wierszowy hybrydowy edytor perkusji (Ghost / Normalny / Akcent + 8↔16 + eksport JSON/GP5)
{
  const grid8 = drumBlockToGridPattern('rock_8th', { numerator: 4, denominator: 4 });
  assert.equal(grid8.stepCount, 8);
  assert.equal(grid8.lanes.kick[0], 2);
  assert.equal(grid8.lanes.snare[2], 2);

  const song = createEmptySong({ measureCount: 2, bpm: 120 });
  // Przełącz krok 1 na stopie jako akcent (3) -> inicjalizuje customDrumGrid
  const nextLvl = setCustomDrumCell(song, 1, 0, 'kick', 1, 3);
  assert.equal(nextLvl, 3);
  assert.ok(song.tracks[1].measures[0].customDrumGrid);

  // Dodaj Akcent na talerzu (crash = 49) w kroku 4
  assert.equal(setCustomDrumCell(song, 1, 0, 'cymbal', 4, 3), 3);
  const customSteps = expandDrumCustomGridToSteps(song.tracks[1].measures[0].customDrumGrid, { numerator: 4, denominator: 4 });
  assert.ok(customSteps.some(s => Math.abs(s.beatOffsetQ - 2.0) < 1e-3 && s.hits.some(h => h.midi === GM_DRUM_MIDI.crash)));

  // Skalowanie 8 <-> 16 kroków zachowuje akcent na nowej siatce
  assert.equal(setCustomDrumResolution(song, 1, 0, 4), true);
  assert.equal(song.tracks[1].measures[0].customDrumGrid.stepCount, 16);
  assert.equal(song.tracks[1].measures[0].customDrumGrid.lanes.cymbal[8], 3);

  // Kopiowanie siatki na pozostałe takty i serializacja .gitara.json
  assert.equal(copyDrumGridToAllMeasures(song, 1, 0), true);
  assert.ok(song.tracks[1].measures[1].customDrumGrid);
  const loaded = deserializeStudioSong(serializeStudioSong(song));
  assert.equal(loaded.song.tracks[1].measures[0].customDrumGrid.stepCount, 16);
  assert.equal(loaded.song.tracks[1].measures[1].customDrumGrid.lanes.cymbal[8], 3);

  // Eksport do C++ GP5/MIDI zawiera własne uderzenia crash (MIDI 49)
  const payload = buildSongExportPayload(song);
  const drumPayload = payload.tracks.find(t => t.isPercussion);
  assert.ok(drumPayload.measures[0].beats.some(b => b.notes.some(n => n.fret === GM_DRUM_MIDI.crash)));

  // Reset wraca do automatycznego klocka
  assert.equal(resetMeasureDrumGrid(song, 1, 0), true);
  assert.equal(song.tracks[1].measures[0].customDrumGrid, null);
  console.log('[OK] Test 21: 5-wierszowy sekwencer perkusji (Ghost/Normalny/Akcent, 8<->16, JSON i eksport GP5)');
}

// 22. Zaznaczanie fragmentu beat-to-beat na TAB oraz grupowe przekształcenia (+Okt/-Okt, Str↑/↓, efekty, rytm)
{
  const song = createEmptySong({ measureCount: 2, bpm: 120 });
  setNoteAt(song, 0, 0, 0, 0, 0); // E2 (40) - poza zakresem zaznaczenia
  setNoteAt(song, 0, 0, 1, 0, 2); // F#2 (42)
  setNoteAt(song, 0, 0, 1, 1, 2); // B2 (47)
  setNoteAt(song, 0, 0, 2, 0, 5); // A2 (45)
  setNoteAt(song, 0, 0, 3, 0, 7); // B2 (47)
  setNoteAt(song, 0, 1, 0, 0, 3); // G2 (43)

  // Odwrotny kierunek przeciągania (od taktu 1 beat 0 do taktu 0 beat 1)
  const norm = normalizeBeatSelection(song, {
    activeTrackIdx: 0,
    beatSelection: {
      trackIdx: 0,
      anchor: { measureIdx: 1, beatIdx: 0 },
      focus: { measureIdx: 0, beatIdx: 1 }
    }
  });
  assert.equal(norm.startMeasure, 0);
  assert.equal(norm.start.beatIdx, 1);
  assert.equal(norm.endMeasure, 1);
  assert.equal(norm.end.beatIdx, 0);
  assert.equal(norm.beatCount, 4);
  assert.equal(norm.kind, 'beats');
  assert.equal(isBeatInSelection(norm, 0, 0), false);
  assert.equal(isBeatInSelection(norm, 0, 1), true);
  assert.equal(isBeatInSelection(norm, 0, 3), true);
  assert.equal(isBeatInSelection(norm, 1, 0), true);
  assert.equal(isBeatInSelection(norm, 1, 1), false);

  // +12 półtonów (Oktawa w górę) działa na cały zaznaczony fragment, nie ruszając beat 0
  const tr = song.tracks[0];
  const beforeUntouchedMidi = noteToMidi(tr, getNoteAt(song, 0, 0, 0, 0));
  const beforeB1Low = noteToMidi(tr, getNoteAt(song, 0, 0, 1, 0));
  const beforeB1High = noteToMidi(tr, getNoteAt(song, 0, 0, 1, 1));
  const beforeBar2 = noteToMidi(tr, getNoteAt(song, 0, 1, 0, 0));
  const octUpRes = shiftSelectionPitch(song, 0, norm, 12);
  assert.equal(octUpRes.beatsChanged, 4);
  assert.equal(noteToMidi(tr, getNoteAt(song, 0, 0, 0, 0)), beforeUntouchedMidi);
  assert.equal(noteToMidi(tr, getNoteAt(song, 0, 0, 1, 0)), beforeB1Low + 12);
  assert.equal(noteToMidi(tr, getNoteAt(song, 0, 0, 1, 1)), beforeB1High + 12);
  assert.equal(noteToMidi(tr, getNoteAt(song, 0, 1, 0, 0)), beforeBar2 + 12);

  // -12 przywraca pierwotne wysokości
  shiftSelectionPitch(song, 0, norm, -12);
  assert.equal(noteToMidi(tr, getNoteAt(song, 0, 0, 2, 0)), 45);

  // Grupowe przeniesienie struny bez zmiany wysokości (beats 2..3)
  const strSel = { trackIdx: 0, anchor: { measureIdx: 0, beatIdx: 2 }, focus: { measureIdx: 0, beatIdx: 3 } };
  const mvRes = moveSelectionToAdjacentString(song, 0, strSel, 1);
  assert.equal(mvRes.movedCount, 2);
  assert.equal(getNoteAt(song, 0, 0, 2, 1).fret, 0);
  assert.equal(getNoteAt(song, 0, 0, 3, 1).fret, 2);

  // Grupowe włączanie/wyłączanie efektu (Palm Mute), rytmu i kasowanie nut w zaznaczeniu
  assert.ok(applyEffectToSelection(song, 0, strSel, 'palmMute') >= 2);
  assert.equal(getNoteAt(song, 0, 0, 2, 1).palmMute, true);
  assert.equal(getNoteAt(song, 0, 0, 3, 1).palmMute, true);
  applyEffectToSelection(song, 0, strSel, 'palmMute'); // Ponowne kliknięcie wyłącza oba
  assert.equal(getNoteAt(song, 0, 0, 2, 1).palmMute, false);
  assert.equal(getNoteAt(song, 0, 0, 3, 1).palmMute, false);

  applyBeatPropToSelection(song, 0, strSel, 'dynamic', 7);
  assert.equal(tr.measures[0].beats[2].dynamic, 7);
  assert.equal(tr.measures[0].beats[3].dynamic, 7);

  assert.equal(deleteSelectionNotes(song, 0, strSel), 2);
  assert.equal(tr.measures[0].beats[2].isRest, true);
  assert.equal(tr.measures[0].beats[3].isRest, true);
  console.log('[OK] Test 22: Zaznaczanie fragmentu beat-to-beat i operacje grupowe (+Okt, Str, techniki, kasowanie)');
}

// 23. Kopiuj / Wytnij / Wklej fragmentu uderzeń (TuxGuitar style) + skrót Ctrl+A i Shift+ArrowUp
{
  const song = createEmptySong({ measureCount: 2, bpm: 120 });
  // Ustaw takt 0 na [1/4, 1/8, 1/8, 1/4, 1/4] (razem 4.0Q)
  song.tracks[0].measures[0].beats = [
    createBeat({ duration: 0, isRest: true }),
    createBeat({ duration: 1, isRest: false, notes: [createTabNote({ string: 2, fret: 5 })] }),
    createBeat({ duration: 1, isRest: false, notes: [createTabNote({ string: 2, fret: 7 })] }),
    createBeat({ duration: 0, isRest: true }),
    createBeat({ duration: 0, isRest: true })
  ];
  const sel = { trackIdx: 0, anchor: { measureIdx: 0, beatIdx: 1 }, focus: { measureIdx: 0, beatIdx: 2 } };
  const clip = copyBeatsToClipboard(song, { trackIdx: 0, selection: sel });
  assert.equal(clip.kind, 'beats');
  assert.equal(clip.beatCount, 2);

  // Wklej dwie ósemki w puste miejsce taktu 1 (beat 1 = ćwierćnuta) -> metrum nadal idealne 4/4!
  const pRes = pasteBeatsFromClipboard(song, clip, { targetTrackIdx: 0, targetMeasureIdx: 1, targetBeatIdx: 1, mode: 'overwrite' });
  assert.equal(pRes.pastedCount, 2);
  assert.equal(validateMeasureMeter(song.tracks[0].measures[1]).status, 'ok');
  assert.equal(getNoteAt(song, 0, 1, 1, 2).fret, 5);
  assert.equal(getNoteAt(song, 0, 1, 2, 2).fret, 7);

  // Wklej 2 ćwierćnuty na koniec taktu 1 (beat 4) -> przelewa drugą ćwierćnutę do nowego taktu 2
  const quarterSel = { trackIdx: 0, anchor: { measureIdx: 1, beatIdx: 1 }, focus: { measureIdx: 1, beatIdx: 2 } };
  const cutClip = cutBeatsToClipboard(song, { trackIdx: 0, selection: quarterSel });
  assert.equal(song.tracks[0].measures[1].beats[1].isRest, true);
  assert.equal(cutClip.beatCount, 2);

  // End-to-end przez kontroler skrótów: Ctrl+A -> Shift+ArrowUp -> Ctrl+C fragment -> Ctrl+V w takt 1 beat 0
  let ui = {
    song: createEmptySong({ measureCount: 2 }),
    history: createStudioHistory(20),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    clipboard: null,
    clipboardScope: 'track',
    clipboardPasteMode: 'overwrite',
    selectionStart: null,
    selectionEnd: null,
    beatSelection: null,
    cmdPalette: { open: false, query: '', selectedIdx: 0 },
    inlinePrompt: null
  };
  setNoteAt(ui.song, 0, 0, 0, 0, 2);
  setNoteAt(ui.song, 0, 0, 1, 0, 3);
  setNoteAt(ui.song, 0, 0, 2, 0, 5);
  const ctrl = createStudioInputController({
    getState: () => ui,
    pushUndo: () => ui.history.push(ui.song),
    onSongChanged: () => {},
    onRender: () => {}
  });
  const keyEv = (key, mods = {}) => ({ key, ...mods, preventDefault() {} });

  ctrl.handleKeyDown(keyEv('a', { ctrlKey: true }));
  assert.ok(ui.beatSelection && ui.beatSelection.fullMeasures);
  ctrl.handleKeyDown(keyEv('ArrowUp', { shiftKey: true }));
  assert.equal(noteToMidi(ui.song.tracks[0], ui.song.tracks[0].measures[0].beats[0].notes[0]), 54);
  assert.equal(noteToMidi(ui.song.tracks[0], ui.song.tracks[0].measures[0].beats[1].notes[0]), 55);
  assert.equal(noteToMidi(ui.song.tracks[0], ui.song.tracks[0].measures[0].beats[2].notes[0]), 57);

  // Zaznacz fragment 2 uderzeń (beats 1..2), skopiuj Ctrl+C, przejdź do taktu 1 beat 0 i wklej Ctrl+V
  ui.beatSelection = { trackIdx: 0, anchor: { measureIdx: 0, beatIdx: 1 }, focus: { measureIdx: 0, beatIdx: 2 }, fullMeasures: false };
  ctrl.handleKeyDown(keyEv('c', { ctrlKey: true }));
  assert.equal(ui.clipboard.kind, 'beats');
  assert.equal(ui.clipboard.beatCount, 2);

  ui.beatSelection = null;
  ui.selectionStart = null;
  ui.selectionEnd = null;
  ui.cursor.measureIdx = 1;
  ui.cursor.beatIdx = 0;
  ctrl.handleKeyDown(keyEv('v', { ctrlKey: true }));
  assert.equal(noteToMidi(ui.song.tracks[0], ui.song.tracks[0].measures[1].beats[0].notes[0]), 55);
  assert.equal(noteToMidi(ui.song.tracks[0], ui.song.tracks[0].measures[1].beats[1].notes[0]), 57);
  console.log('[OK] Test 23: Kopiowanie i wklejanie fragmentu w kliknięty takt/uderzenie + skrót Ctrl+A i +Okt');
}

// 24. Tryb „Nuty” (sama pięciolinia 8vb a'la Ćwiczenie), widok całego zespołu naraz i podświetlenie zaznaczenia
{
  const demo = createDemoSong('rock_riff_em');
  assert.equal(writtenStaffMidi(demo.tracks[0], 40), 52); // Gitara 8vb
  assert.equal(writtenStaffMidi(demo.tracks[2], 28), 40); // Bas 8vb

  // Dodajmy pauzę do taktu 0, żeby zweryfikować symbol pauzy ćwierćnutowej '𝄽'
  demo.tracks[0].measures[0].beats[0].isRest = true;
  demo.tracks[0].measures[0].beats[0].notes = [];

  const staffOnlySvg = renderStudioTabSvg(demo, {
    activeTrackIdx: 0,
    viewMode: 'staff',
    showNoteNames: true
  });
  assert.ok(staffOnlySvg.includes('𝄞'), 'Missing treble clef in staff mode');
  assert.ok(staffOnlySvg.includes('𝄽'), 'Missing musical rest symbol in staff mode');
  assert.ok(!staffOnlySvg.includes('>TAB<'), 'Pure staff view must not draw TAB clef label');

  const multiSvg = renderStudioTabSvg(demo, {
    activeTrackIdx: 0,
    viewMode: 'staff',
    allTracks: true,
    beatSelection: { trackIdx: 0, anchor: { measureIdx: 0, beatIdx: 1 }, focus: { measureIdx: 0, beatIdx: 2 } }
  });
  assert.ok(multiSvg.includes('data-track="0"') && multiSvg.includes('data-track="1"') && multiSvg.includes('data-track="2"') && multiSvg.includes('data-track="3"'), 'All tracks must render in synchronized multitrack view');
  assert.ok(multiSvg.includes('studio-playhead-bounds'), 'Missing grand playhead bounds');
  assert.ok(multiSvg.includes('class="studio-beat-sel-bg" data-track="0" data-measure="0" data-beat="1"') && multiSvg.includes('opacity="1"'), 'Selected beats must render amber selection rect');
  console.log('[OK] Test 24: Tryb czystej pięciolinii (Nuty 8vb), pełna partytura wielościeżkowa i zaznaczenie SVG');
}

console.log('\nWszystkie testy Studio TAB (24/24) zakończone sukcesem!');
