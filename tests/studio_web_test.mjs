import assert from 'node:assert/strict';
import {
  DEFAULT_DSP_HIGH_THRESHOLD_ON,
  DEFAULT_DSP_THRESHOLD_ON,
  DEFAULT_STUDIO_PREFS,
  MAX_STUDIO_TRACKS,
  NOTE_NAMES_FLAT,
  NOTE_NAMES_SHARP,
  WHAMMY_PRESETS,
  addMeasure,
  addMeasureBefore,
  addTrack,
  analyzePhrasePositions,
  analyzeSelectionPositionsAndOctaves,
  applyBeatPropToSelection,
  applyEffectToSelection,
  applyMetronomePreset,
  applyPhrasePosition,
  buildBeatEventsForTrack,
  buildBeatsClipboardFromSteps,
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
  expandSongToLinearMeasures,
  generateStudioArpeggioOrSweepSteps,
  applyGeneratedStepsToSong,
  detectAutoChordForBeat,
  detectAutoChordsForMeasure,
  getNoteAt,
  getScaleHintPitchClasses,
  hasProRepeatStructure,
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
  reconcileLinearEditsIntoProSong,
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
  sensitivityOffsetToThresholds,
  shiftNotePitch,
  shiftSelectionPitch,
  solveBeatFretAssignments,
  thresholdToSensitivityOffset,
  tieOrExtendPreviousNote,
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
  computePlayheadXForMeasure,
  computeStaffStepFromMidi,
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
import {
  LIBRARY_FORMAT,
  createUserLibraryStore,
  exportItemPayload,
  exportPackPayload,
  makeSafeFilename,
  parseImportText,
  sanitizeItemId
} from '../web/js/userLibrary.js';

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

  assert.equal(DEFAULT_STUDIO_PREFS.uiMode, 'simple');
  assert.equal(clampStudioPrefs({ uiMode: 'pro' }).uiMode, 'pro');
  assert.equal(clampStudioPrefs({ uiMode: 'bogus' }).uiMode, 'simple');

  // Tryb uproszczony (domyślny): zostawia dynamikę + techniki brzmieniowe, usuwa notacyjne detale
  const palSimple = renderStudioPaletteHtml({
    uiMode: 'simple',
    track: song.tracks[0],
    beat: song.tracks[0].measures[0].beats[0],
    note: getNoteAt(song, 0, 0, 0, 0),
    measure: song.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 }
  });
  assert.ok(palSimple.includes('data-action="palette-dynamic"'), 'Simple mode must keep dynamics control');
  assert.ok(palSimple.includes('effect:palmMute') && palSimple.includes('bend:full') && palSimple.includes('harmonic:1'));
  for (const hiddenCmd of ['pick:1', 'pick:2', 'brush:down', 'effect:graceNote', 'effect:trill', 'effect:tremoloPicking', 'beat:fermata', 'dur:4', 'tuplet:5:4']) {
    assert.ok(!palSimple.includes(`data-cmd="${hiddenCmd}"`), `Simple mode must omit ${hiddenCmd}`);
  }

  // Tryb Pro: zawiera wszystkie detale notacyjne
  const palPro = renderStudioPaletteHtml({
    uiMode: 'pro',
    track: song.tracks[0],
    beat: song.tracks[0].measures[0].beats[0],
    note: getNoteAt(song, 0, 0, 0, 0),
    measure: song.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 }
  });
  for (const proCmd of ['pick:1', 'pick:2', 'brush:down', 'effect:graceNote', 'effect:trill', 'effect:tremoloPicking', 'beat:fermata', 'dur:4', 'tuplet:5:4']) {
    assert.ok(palPro.includes(`data-cmd="${proCmd}"`), `Pro mode must include ${proCmd}`);
  }
  console.log('[OK] Test 12: Paleta Songsterra (tryb Uproszczony/Pro, dymki), pasek inline i wyszukiwarka Ctrl+K');
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

// 25. Czas trwania nut i precyzyjne planowanie WebAudio whenSec przy prędkości 50% / 100% / 150%
{
  let fakeNowSec = 10.0;
  const triggered = [];
  const stopAllCalls = [];
  const stateObj = { guitarTonePreset: 'clean', keyboardPreset: 'rhodes', a4ReferenceHz: 440 };
  const mockHost = {
    getState: () => stateObj,
    ensureAudioCtx: () => ({ currentTime: fakeNowSec }),
    getMasterAudioOut: () => ({}),
    pluckGuitarFrequency: (freq, dur, gain, preview, opts) => {
      triggered.push({ type: 'guitar', t: fakeNowSec, freq, dur, gain, preview, opts: opts || {} });
    },
    stopAllPreviewVoices: () => stopAllCalls.push(fakeNowSec)
  };

  const song = createEmptySong({ bpm: 120, measureCount: 1 });
  song.tracks[0].measures[0].beats = [
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 0 })] }),                   // 0.0 Q
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 0, fret: 2 })] }),                   // 1.0 Q (1/16)
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 0, fret: 3 })] }),                   // 1.25 Q (1/16)
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 5, palmMute: true })] }),   // 1.5 Q (PM)
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 0, fret: 7, staccato: true })] }),   // 2.5 Q (Staccato)
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 1, fret: 2, letRing: true })] })     // 3.5 Q (Let ring)
  ];

  const runAtSpeed = (speedPct, t0) => {
    triggered.length = 0;
    const p = createStudioPlayer({ host: mockHost, clockFn: () => fakeNowSec });
    p.loadSong(song, 0);
    p.setMetronome(false);
    p.setSpeedPercent(speedPct);
    fakeNowSec = t0;
    p.play();
    const totalSec = 4.5 * (60 / (120 * (speedPct / 100)));
    for (let dt = 0.04; dt <= totalSec; dt += 0.04) {
      fakeNowSec = t0 + dt;
      p.stepSchedulerForTest(fakeNowSec);
    }
    const snapshot = triggered.slice();
    p.stop();
    p.destroy();
    return snapshot;
  };

  const at50 = runAtSpeed(50, 20.0);
  const at100 = runAtSpeed(100, 40.0);
  const at150 = runAtSpeed(150, 60.0);

  assert.equal(at50.length, 6);
  assert.equal(at100.length, 6);
  assert.equal(at150.length, 6);
  assert.ok(stopAllCalls.length >= 3, 'stop() must cancel active sustained preview voices');

  // Ćwierćnuta: 50% (0.96s) == 2x 100% (0.48s); 150% (0.32s) == 2/3x 100%
  assert.ok(Math.abs(at50[0].dur - 2 * at100[0].dur) < 1e-6, `50% (${at50[0].dur}) should be 2x 100% (${at100[0].dur})`);
  assert.ok(Math.abs(at150[0].dur - (2 / 3) * at100[0].dur) < 1e-6, `150% (${at150[0].dur}) should be 2/3x 100% (${at100[0].dur})`);
  assert.equal(at150[0].opts.strictDuration, true);

  // Szybkie 1/16 przy 150% (~0.08s) nie mogą być sztucznie wydłużone do 0.22s ani wpadać na ten sam whenSec
  assert.ok(at150[1].dur < 0.11 && at150[2].dur < 0.11, `Fast 16ths at 150% must stay short, got ${at150[1].dur}`);
  assert.ok(at150[2].opts.whenSec > at150[1].opts.whenSec + 0.06, 'Consecutive 16ths inside lookahead must get distinct whenSec');

  // Palm mute skaluje się proporcjonalnie ze zmianą prędkości
  assert.ok(at50[3].dur > at100[3].dur && at100[3].dur > at150[3].dur, 'Palm mute duration must scale inversely with speed');
  assert.equal(at50[3].opts.palmMute, true);

  // Staccato ~45% czasu taktu; Let Ring wyłącza strictDuration
  assert.ok(Math.abs(at100[4].dur - at100[0].dur * 0.45) < 1e-4, 'Staccato must shorten normal duration to ~45%');
  assert.equal(at100[5].opts.strictDuration, false, 'Let Ring must disable strictDuration cut');
  console.log('[OK] Test 25: Skalowanie czasu nut (50/100/150%), Palm Mute/Staccato i precyzja whenSec');
}

// 26. Oznaczanie oktaw na gryfie Studia TAB (gitara 6-str. + bas 4-str.) i wybór dźwięku podziału ze skali
{
  const demo = createDemoSong('rock_riff_em');
  const guitarTrack = demo.tracks[0];
  const bassTrack = demo.tracks[2]; // Bas 4-str. [28, 33, 38, 43]
  const emScale = getScaleHintPitchClasses(4, 'natural_minor');

  const gSvgDefault = renderStudioFretboardSvg(guitarTrack, {
    keyRootPc: 4,
    keyScalePcs: emScale,
    showOctaveBands: true,
    octaveAnchorPc: 4
  });
  const gSvgAltAnchor = renderStudioFretboardSvg(guitarTrack, {
    keyRootPc: 4,
    keyScalePcs: emScale,
    showOctaveBands: true,
    octaveAnchorPc: 7 // Sol (G) ze skali E moll
  });
  assert.ok(gSvgDefault.includes('studio-fb-oct-band'), 'Guitar fretboard must emit octave band shading');
  assert.ok(gSvgDefault.includes('studio-fb-oct-link'), 'Guitar fretboard must emit unison connector lines');
  assert.notEqual(gSvgDefault, gSvgAltAnchor, 'Changing octave anchor note in scale must update octave bands/links');

  const bassSvg = renderStudioFretboardSvg(bassTrack, {
    keyRootPc: 4,
    keyScalePcs: emScale,
    showOctaveBands: true,
    octaveAnchorPc: 4
  });
  assert.ok(bassSvg.includes('studio-fb-oct-band'), '4-string bass fretboard must support octave bands');
  assert.ok(bassSvg.includes('studio-fb-oct-link'), '4-string bass fretboard must draw unison links');
  assert.ok(!bassSvg.includes('NaN'), '4-string bass octave SVG must not contain NaN coordinates');

  const clamped = clampStudioPrefs({ showOctaveBands: true, octaveAnchorPc: 7 });
  assert.equal(clamped.showOctaveBands, true);
  assert.equal(clamped.octaveAnchorPc, 7);
  const rt = deserializeStudioSong(serializeStudioSong(demo, clamped));
  assert.equal(rt.studioPrefs.showOctaveBands, true);
  assert.equal(rt.studioPrefs.octaveAnchorPc, 7);
  console.log('[OK] Test 26: Pasy oktawowe i wybór dźwięku podziału (gitara 6-str. i bas 4-str.)');
}

// 27. Auto-akordy (ChordNamer A–G), pisownia bemolowa na pięciolinii, 7/16, multi-volta i pełna transpozycja akordu na klawiszach
{
  const song = createEmptySong({ title: 'Audit Regressions', bpm: 120, measureCount: 4 });
  const tr = song.tracks[0];
  // 7/16 powinien mieć 7 szesnastek (dur = 2)
  const m716 = createEmptyMeasure(7, 16, '', false);
  assert.equal(m716.beats.length, 7);
  assert.equal(m716.beats[0].duration, 2);

  // Auto-akord wielodźwiękowy B-D-F# (strict B, nigdy H) oraz bemol Bb-D-F przy accMode=1
  const bmBeat = createBeat({
    duration: 0,
    isRest: false,
    notes: [
      { string: 1, fret: 2 },  // B2 (47)
      { string: 2, fret: 4 },  // F#3 (54)
      { string: 3, fret: 4 },  // B3 (59)
      { string: 4, fret: 3 }   // D4 (62)
    ]
  });
  const bmLabel = detectAutoChordForBeat(tr, bmBeat, 2);
  assert.ok(bmLabel.startsWith('B'), `Expected B-chord (never H), got ${bmLabel}`);
  assert.ok(!bmLabel.includes('H'));

  const bbBeat = createBeat({
    duration: 0,
    isRest: false,
    notes: [
      { string: 1, fret: 1 }, // Bb2 (46)
      { string: 2, fret: 3 }, // F3 (53)
      { string: 4, fret: 3 }  // D4 (62)
    ]
  });
  const bbLabel = detectAutoChordForBeat(tr, bbBeat, 1);
  assert.ok(bbLabel.startsWith('Bb'), `Expected Bb chord with accMode=1, got ${bbLabel}`);

  // Arpeggiowany takt bez równoczesnych akordów -> auto-akord taktu + render SVG
  tr.measures[0].beats = [
    createBeat({ duration: 1, isRest: false, notes: [{ string: 0, fret: 0 }] }), // E2
    createBeat({ duration: 1, isRest: false, notes: [{ string: 1, fret: 2 }] }), // B2
    createBeat({ duration: 1, isRest: false, notes: [{ string: 2, fret: 2 }] }), // E3
    createBeat({ duration: 1, isRest: false, notes: [{ string: 3, fret: 0 }] })  // G3
  ];
  const autoM0 = detectAutoChordsForMeasure(tr, tr.measures[0], 2);
  assert.equal(autoM0.measureChord, 'Em');
  const svgWithAuto = renderStudioTabSvg(song, { activeTrackIdx: 0, showAutoChords: true, accMode: 2 });
  assert.ok(svgWithAuto.includes('studio-auto-chord') && svgWithAuto.includes('>Em<'));
  const svgNoAuto = renderStudioTabSvg(song, { activeTrackIdx: 0, showAutoChords: false, accMode: 2 });
  assert.ok(!svgNoAuto.includes('studio-auto-chord'));

  // Pisownia bemolowa na pięciolinii (Bb4 vs A#4 muszą różnić się stopniem na pięciolinii)
  const aSharpStep = computeStaffStepFromMidi(70, false, 2); // A#4 -> linia A
  const bFlatStep = computeStaffStepFromMidi(70, false, 1);  // Bb4 -> linia B
  assert.equal(aSharpStep.accidental, '♯');
  assert.equal(bFlatStep.accidental, '♭');
  assert.equal(bFlatStep.step, aSharpStep.step + 1);

  // Multi-volta (1., 2. oraz 3. zakończenie nie powtarza Volty 2 dwa razy!)
  const voltaSong = createEmptySong({ measureCount: 4 });
  voltaSong.tracks[0].measures[0].repeatOpen = true;
  voltaSong.tracks[0].measures[1].voltaMask = 1;
  voltaSong.tracks[0].measures[1].repeatClose = 3;
  voltaSong.tracks[0].measures[2].voltaMask = 2;
  voltaSong.tracks[0].measures[2].repeatClose = 3;
  voltaSong.tracks[0].measures[3].voltaMask = 4;
  const tl = expandPlaybackTimeline(voltaSong);
  assert.deepEqual(tl.map(e => e.measureIdx), [0, 1, 0, 2, 0, 3]);

  // Transpozycja akordu na ścieżce klawiszy (wszystkie dźwięki, nie tylko 1.!) + export payload (bends, drums, keys)
  const kIdx = addTrack(song, 'keys');
  const kTr = song.tracks[kIdx];
  setNoteAt(song, kIdx, 0, 0, 0, 60, { chord: true });
  setNoteAt(song, kIdx, 0, 0, 0, 64, { chord: true });
  setNoteAt(song, kIdx, 0, 0, 0, 67, { chord: true });
  assert.equal(getNoteAt(song, kIdx, 0, 0, -1), null);
  transposeRange(song, { trackIdx: kIdx, startMeasure: 0, endMeasure: 0, semitones: 2 });
  const kMidis = kTr.measures[0].beats[0].notes.map(n => n.midi);
  assert.deepEqual(kMidis, [62, 66, 69]);

  // Bend pos/val i skalowanie perkusji + klawisze w buildSongExportPayload
  tr.measures[1].beats[0] = createBeat({
    duration: 0,
    isRest: false,
    notes: [{ string: 2, fret: 7, bendPreset: 'full', bendPoints: [{ pos: 0, val: 0 }, { pos: 60, val: 100 }] }]
  });
  const exp = buildSongExportPayload(song);
  assert.equal(exp.tracks[0].measures[1].beats[0].notes[0].bendPoints[1].position, 60);
  assert.equal(exp.tracks[0].measures[1].beats[0].notes[0].bendPoints[1].value, 100);
  const expDrumNote = exp.tracks[1].measures[0].beats.find(b => !b.isRest)?.notes?.[0];
  assert.ok(expDrumNote && expDrumNote.velocity >= 10 && expDrumNote.velocity <= 127);
  const expKeys = exp.tracks[kIdx];
  assert.equal(expKeys.tuningId, 'keys');
  assert.deepEqual(expKeys.measures[0].beats[0].notes.map(n => n.midi), [62, 66, 69]);
  console.log('[OK] Test 27: Auto-akordy (ChordNamer A-G), bemole pięciolinii, multi-volta, 7/16 i eksport keys/bend/drum');
}

// 28. Generator Arpeggio / Sweep bezpośrednio w Studio TAB (Gitara 6-str., Bas 4-str., Klawisze)
{
  const song = createEmptySong({ title: 'Studio Arp Sweep Test', measureCount: 2 });
  const gSteps = generateStudioArpeggioOrSweepSteps({
    mode: 'sweep',
    rootPc: 4, // Em
    quality: 'minor',
    direction: 'up_down',
    track: song.tracks[0],
    duration: 2,
    accMode: 2
  });
  assert.ok(gSteps.length >= 6 && gSteps[0].chordSymbol === 'Em');
  const cursorRes = applyGeneratedStepsToSong(song, 0, gSteps, { insertMode: 'cursor', measureIdx: 0, beatIdx: 0 });
  assert.ok(cursorRes.ok && cursorRes.insertedBeats === gSteps.length);

  const bIdx = addTrack(song, 'bass');
  const bassSteps = generateStudioArpeggioOrSweepSteps({
    mode: 'arpeggio',
    rootPc: 9, // Am
    quality: 'minor',
    direction: 'up',
    track: song.tracks[bIdx],
    duration: 1
  });
  assert.ok(bassSteps.length >= 3 && bassSteps.every(st => st.notes[0].string < 4));
  const appRes = applyGeneratedStepsToSong(song, bIdx, bassSteps, { insertMode: 'append', measureIdx: 0 });
  assert.ok(appRes.ok && appRes.insertedBars >= 1);

  const kIdx = addTrack(song, 'keys');
  const kSteps = generateStudioArpeggioOrSweepSteps({
    mode: 'arpeggio',
    rootPc: 0, // Cmaj7
    quality: 'maj7',
    direction: 'up_down',
    track: song.tracks[kIdx],
    duration: 1
  });
  assert.ok(kSteps.length >= 4 && kSteps.every(st => Number.isFinite(st.notes[0].midi)));
  const kRes = applyGeneratedStepsToSong(song, kIdx, kSteps, { insertMode: 'cursor', measureIdx: 0, beatIdx: 0 });
  assert.ok(kRes.ok);
  console.log('[OK] Test 28: Generator Arpeggio/Sweep w Studio TAB (gitara 6-str., bas 4-str. i klawisze)');
}

// 29. Lokalna biblioteka użytkownika (Sweep, Progresje, TAB-y, Studio TAB) + eksport/import plików i paczek
await (async () => {
  assert.equal(makeSafeFilename('Łąka ąęćłńóśźż #1', 'gitara-sweep.json'), 'laka_aeclnoszz_1.gitara-sweep.json');
  assert.equal(sanitizeItemId('../evil_id'), 'evil_id');

  const serverDisk = { sweeps: new Map(), progressions: new Map(), tabs: new Map(), studio: new Map(), sessions: new Map() };
  const memStorageMap = new Map();
  const mockStorage = {
    getItem: (k) => (memStorageMap.has(k) ? memStorageMap.get(k) : null),
    setItem: (k, v) => memStorageMap.set(k, String(v))
  };
  const mockFetch = async (url, opts = {}) => {
    const u = new URL(url, 'http://localhost');
    if (u.pathname === '/api/library') {
      const payload = {};
      for (const [cat, m] of Object.entries(serverDisk)) payload[cat] = Array.from(m.values());
      return { ok: true, json: async () => payload };
    }
    if (u.pathname === '/api/library/save') {
      const cat = u.searchParams.get('category');
      const id = u.searchParams.get('id');
      const obj = JSON.parse(opts.body);
      serverDisk[cat].set(id, obj);
      return { ok: true, json: async () => ({ ok: true, category: cat, id }) };
    }
    if (u.pathname === '/api/library/delete') {
      const cat = u.searchParams.get('category');
      const id = u.searchParams.get('id');
      const ok = serverDisk[cat].delete(id);
      return { ok, json: async () => ({ ok }) };
    }
    return { ok: false, json: async () => ({}) };
  };

  const store = createUserLibraryStore({ fetchFn: mockFetch, storage: mockStorage });
  await store.load();

  const sw = await store.saveItem('sweeps', {
    id: 'sw_share_1',
    name: 'Neoklasyczny Sweep Em',
    items: [{ symbol: 'Em', rootPc: 4, chordId: 'minor', steps: [{ s: 0, f: 7 }], nodes: [] }]
  });
  const pr = await store.saveItem('progressions', {
    id: 'pr_share_1',
    title: 'Progresja Kolegi',
    bpm: 118,
    keyRootPc: 4,
    isMinor: true,
    measures: [{ volta: 0, beats: [{ offsetSemi: 0, chordId: 'minor', roman: 'i' }, null, null, null] }]
  });
  const tb = await store.saveItem('tabs', {
    id: 'tb_share_1',
    title: 'Zagrywka Pentatoniczna',
    recordedEvents: [{ s: 0, f: 5, midi: 45, note: 'A', dur: 1, tech: '-' }]
  });
  const demoStudioSong = createDemoSong('rock_riff_em');
  const st = await store.saveItem('studio', {
    id: 'st_share_1',
    name: demoStudioSong.title,
    song: demoStudioSong,
    studioPrefs: { uiMode: 'simple' }
  });

  // Trwałość po utworzeniu nowej instancji store
  const store2 = createUserLibraryStore({ fetchFn: mockFetch, storage: new Map() });
  await store2.load();
  assert.equal(store2.getItems('sweeps').length, 1);
  assert.equal(store2.getItems('progressions').length, 1);
  assert.equal(store2.getItems('tabs').length, 1);
  assert.equal(store2.getItems('studio').length, 1);

  // Wrapped studio payload zgodny z deserializeStudioSong
  const wrappedStudioJson = JSON.stringify(exportItemPayload('studio', st));
  const decodedStudio = deserializeStudioSong(wrappedStudioJson);
  assert.equal(decodedStudio.song.title, demoStudioSong.title);
  assert.equal(decodedStudio.studioPrefs.uiMode, 'simple');

  // Eksport paczki + import na czystym magazynie ("wczytaj i ma na stałe")
  const packJson = JSON.stringify(exportPackPayload(store.getAll(), { name: 'Paczka testowa' }));
  const cleanDisk = { sweeps: new Map(), progressions: new Map(), tabs: new Map(), studio: new Map(), sessions: new Map() };
  const cleanStore = createUserLibraryStore({
    fetchFn: async (url, opts = {}) => {
      const u = new URL(url, 'http://localhost');
      if (u.pathname === '/api/library') {
        const out = {};
        for (const [c, m] of Object.entries(cleanDisk)) out[c] = Array.from(m.values());
        return { ok: true, json: async () => out };
      }
      if (u.pathname === '/api/library/save') {
        cleanDisk[u.searchParams.get('category')].set(u.searchParams.get('id'), JSON.parse(opts.body));
        return { ok: true, json: async () => ({ ok: true }) };
      }
      if (u.pathname === '/api/library/delete') {
        const ok = cleanDisk[u.searchParams.get('category')].delete(u.searchParams.get('id'));
        return { ok, json: async () => ({ ok }) };
      }
      return { ok: false, json: async () => ({}) };
    },
    storage: null
  });
  const impPack = await cleanStore.importTextPermanently(packJson);
  assert.equal(impPack.count, 4);
  assert.equal(cleanStore.getItems('sweeps')[0].source, 'imported');
  assert.equal(cleanStore.getItems('progressions')[0].title, pr.title);
  assert.equal(cleanStore.getItems('tabs')[0].title, tb.title);
  assert.equal(cleanStore.getItems('studio')[0].name, sw ? st.name : '');

  // Legacy Studio .gitara.json importowany bezpośrednio do kategorii 'studio'
  const legacyStudioJson = serializeStudioSong(demoStudioSong, { uiMode: 'pro' });
  const impLegacy = await cleanStore.importTextPermanently(legacyStudioJson, 'studio');
  assert.equal(impLegacy.count, 1);
  assert.equal(impLegacy.categories[0], 'studio');

  // Usuwanie
  assert.equal(await store.deleteItem('sweeps', sw.id), true);
  assert.equal(store.getItems('sweeps').length, 0);

  // Niepoprawny JSON -> czytelny wyjątek
  assert.throws(() => parseImportText('{bad json'), /Niepoprawny plik JSON/);
  assert.equal(LIBRARY_FORMAT, 'apka-gitara-library-v1');
  console.log('[OK] Test 29: Lokalna biblioteka użytkownika (userLibrary.js), paczki i trwały import plików');
})();

// 30. Notacja Songsterr (sustain, tied, let ring, P.M., H/P, slide, bend, vibrato, wajcha), Zoom TAB/Gryfu i wrażliwość detekcji
{
  // A. WHAMMY_PRESETS, palette, input commands & export payload
  assert.ok(WHAMMY_PRESETS.dive_full && WHAMMY_PRESETS.dive_bomb && WHAMMY_PRESETS.dip_full && WHAMMY_PRESETS.scoop && WHAMMY_PRESETS.raise_half);
  const wSong = createEmptySong({ measureCount: 1 });
  setNoteAt(wSong, 0, 0, 0, 2, 12);
  assert.equal(toggleNoteEffect(wSong, 0, 0, 0, 2, 'whammyPreset', 'dive_bomb'), true);
  const wNote = getNoteAt(wSong, 0, 0, 0, 2);
  assert.equal(wNote.whammyPreset, 'dive_bomb');
  assert.ok(Array.isArray(wNote.whammyPoints) && wNote.whammyPoints.length >= 2);
  assert.equal(STUDIO_COMMANDS.some(c => c.id === 'whammy:dive_full'), true);

  const palSimpleHtml = renderStudioPaletteHtml({
    uiMode: 'simple',
    track: wSong.tracks[0],
    beat: wSong.tracks[0].measures[0].beats[0],
    note: wNote,
    measure: wSong.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 2 }
  });
  assert.equal(palSimpleHtml.includes('data-action="palette-whammy"'), false, 'Simple palette must hide whammy');
  assert.equal(palSimpleHtml.includes('data-cmd="whammy:dive_full"'), false, 'Simple palette must hide whammy button');
  assert.ok(palSimpleHtml.includes('data-cmd="effect:vibrato"') && palSimpleHtml.includes('data-cmd="effect:hammerOn"') && palSimpleHtml.includes('data-action="palette-bend"') && palSimpleHtml.includes('data-cmd="effect:tied"'), 'Simple palette must include vibrato, legato, bend, and extend-tie');

  const palProHtml = renderStudioPaletteHtml({
    uiMode: 'pro',
    track: wSong.tracks[0],
    beat: wSong.tracks[0].measures[0].beats[0],
    note: wNote,
    measure: wSong.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 2 }
  });
  assert.ok(palProHtml.includes('data-action="palette-whammy"') && palProHtml.includes('data-cmd="whammy:dive_full"'), 'Pro palette missing whammy UI');

  const cmdSong = createEmptySong({ measureCount: 1 });
  setNoteAt(cmdSong, 0, 0, 0, 0, 5);
  const cmdUi = {
    song: cmdSong,
    history: createStudioHistory(20),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    loopEnabled: false,
    loopStart: 0,
    loopEnd: 0,
    clipboard: null,
    clipboardScope: 'track',
    clipboardPasteMode: 'overwrite',
    selectionStart: null,
    selectionEnd: null,
    beatSelection: null,
    inlinePrompt: null,
    cmdPalette: { open: false, query: '', selectedIdx: 0 }
  };
  const inputCtrl = createStudioInputController({
    getState: () => cmdUi,
    pushUndo: () => cmdUi.history.push(cmdUi.song),
    onSongChanged: () => {},
    onRender: () => {}
  });
  assert.equal(inputCtrl.executeCommand('whammy:dive_full'), true);
  assert.equal(getNoteAt(cmdSong, 0, 0, 0, 0).whammyPreset, 'dive_full');
  assert.equal(inputCtrl.handleKeyDown({ key: 'w', ctrlKey: false, metaKey: false, altKey: false, shiftKey: false, preventDefault() {} }), true);
  assert.equal(getNoteAt(cmdSong, 0, 0, 0, 0).whammyPreset, '', 'Second w toggles dive_full off');

  const selSong = createEmptySong({ measureCount: 1 });
  setNoteAt(selSong, 0, 0, 0, 0, 5);
  setNoteAt(selSong, 0, 0, 1, 0, 7);
  assert.equal(applyEffectToSelection(selSong, 0, {
    trackIdx: 0,
    anchor: { measureIdx: 0, beatIdx: 0 },
    focus: { measureIdx: 0, beatIdx: 1 }
  }, 'whammyPreset', 'dive_bomb'), 2);
  const expPayload = buildSongExportPayload(selSong);
  assert.equal(expPayload.tracks[0].measures[0].beats[0].notes[0].whammyPreset, 'dive_bomb');
  assert.ok(expPayload.tracks[0].measures[0].beats[1].notes[0].whammyPoints.length >= 2);

  // B. Songsterr SVG markers/classes
  const sg = createEmptySong({ bpm: 120, measureCount: 3 });
  sg.tracks[0].measures[0].beats = [
    createBeat({ duration: -2, isRest: false, notes: [createTabNote({ string: 0, fret: 3, letRing: true })] })
  ];
  sg.tracks[0].measures[1].beats = [
    createBeat({ duration: -1, isRest: false, notes: [createTabNote({ string: 1, fret: 5, bendPreset: 'full', vibrato: true })] }),
    createBeat({ duration: 1, isRest: false, dynamic: 6, notes: [createTabNote({ string: 0, fret: 5 })] }),
    createBeat({ duration: 1, isRest: false, dynamic: 6, notes: [createTabNote({ string: 0, fret: 5, tied: true })] })
  ];
  sg.tracks[0].measures[2].beats = [
    createBeat({ duration: 2, isRest: false, dynamic: 7, notes: [createTabNote({ string: 2, fret: 7 })] }),
    createBeat({ duration: 2, isRest: false, dynamic: 1, notes: [createTabNote({ string: 2, fret: 9, hammerOn: true })] }),
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 2, fret: 9, slideType: 'shift', palmMute: true })] }),
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 2, fret: 12, whammyPreset: 'dive_bomb' })] })
  ];

  const tabSvg = renderStudioTabSvg(sg, {
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    viewMode: 'tab',
    containerWidth: 1000
  });
  for (const marker of [
    'studio-tie-arc',
    'studio-letring-span',
    'studio-pm-span',
    'studio-legato-slur',
    'studio-slide-line',
    'studio-bend-arrow',
    'studio-vibrato-wave',
    'studio-whammy-curve',
    'w/bar',
    'studio-rhythm-beam'
  ]) {
    assert.ok(tabSvg.includes(marker), `Missing Songsterr SVG marker: ${marker}`);
  }

  // C. Tied continuation audio merging + pitchCurve + dynamic gain scaling
  const evs = buildBeatEventsForTrack(sg, 0);
  const tiedStartEv = evs.find(e => e.measureIdx === 1 && e.beatIdx === 1);
  const tiedContEv = evs.find(e => e.measureIdx === 1 && e.beatIdx === 2);
  assert.ok(tiedStartEv, 'Tied start event must exist');
  assert.equal(tiedContEv, undefined, 'Tied continuation beat with same pitch must merge instead of retriggering');
  assert.ok(tiedStartEv.notes[0].tieExtraQ > 0.45, `Expected merged tieExtraQ ~0.5, got ${tiedStartEv.notes[0].tieExtraQ}`);

  let fakeNowSec = 30.0;
  const triggered = [];
  const pHost = {
    getState: () => ({ guitarTonePreset: 'lead', a4ReferenceHz: 440 }),
    ensureAudioCtx: () => ({ currentTime: fakeNowSec }),
    getMasterAudioOut: () => ({}),
    pluckGuitarFrequency: (freq, dur, gain, preview, opts) => {
      triggered.push({ freq, dur, gain, opts: opts || {} });
    },
    stopAllPreviewVoices: () => {}
  };
  const p = createStudioPlayer({ host: pHost, clockFn: () => fakeNowSec });
  p.loadSong(sg, 0);
  p.setMetronome(false);
  p.play();
  for (let dt = 0.05; dt <= 6.5; dt += 0.05) {
    fakeNowSec = 30.0 + dt;
    p.stepSchedulerForTest(fakeNowSec);
  }
  p.stop();
  p.destroy();

  assert.equal(triggered.length, 7, `Expected 7 triggered note events (with tied note merged), got ${triggered.length}`);
  const tiedAudio = triggered[2]; // M1 B1 (two tied 1/8 = 1.0 Q -> ~0.48s)
  const loudAccent = triggered[3]; // M2 B0 (fff dynamic: 7)
  const quietHammer = triggered[4]; // M2 B1 (pp dynamic: 1, hammerOn)
  const whammyAudio = triggered[6]; // M2 B3 (whammy dive_bomb)
  assert.ok(tiedAudio.dur > 0.44, `Merged tied 1/8+1/8 note must sound for ~0.48s, got ${tiedAudio.dur}`);
  assert.equal(triggered[1].opts.vibrato, true);
  assert.equal(quietHammer.opts.hammerOn, true);
  assert.ok(loudAccent.gain > quietHammer.gain * 2.0, `fff beat (${loudAccent.gain}) must be much louder than pp (${quietHammer.gain})`);
  assert.ok(Array.isArray(whammyAudio.opts.pitchCurve) && whammyAudio.opts.pitchCurve.length >= 2);
  assert.equal(whammyAudio.opts.pitchCurve[1].val, -250);

  // D. TAB Zoom & Fretboard Zoom SVG dimension scaling + prefs clamping
  const parseSvgDims = (svgStr) => {
    const m = svgStr.match(/viewBox="0 0 ([\d.]+) ([\d.]+)" width="([^"]+)" height="(\d+)"/);
    assert.ok(m, 'SVG missing viewBox/width/height attributes');
    const vbW = Number(m[1]);
    const vbH = Number(m[2]);
    const wNum = Number(m[3]);
    const hNum = Number(m[4]);
    return { vbW, vbH, w: m[3], wNum, hNum, visualScale: wNum ? (wNum / vbW) : 0 };
  };
  const tabZoom80 = parseSvgDims(renderStudioTabSvg(sg, { containerWidth: 1000, tabZoom: 80 }));
  const tabZoom100 = parseSvgDims(renderStudioTabSvg(sg, { containerWidth: 1000, tabZoom: 100 }));
  const tabZoom140 = parseSvgDims(renderStudioTabSvg(sg, { containerWidth: 1000, tabZoom: 140 }));
  assert.ok(tabZoom140.visualScale > tabZoom100.visualScale && tabZoom100.visualScale > tabZoom80.visualScale, 'tabZoom must increase SVG visual scale (width / viewBoxW) monotonically');
  assert.ok(tabZoom140.hNum > tabZoom100.hNum && tabZoom100.hNum > tabZoom80.hNum, 'tabZoom must scale SVG height monotonically');

  const fb80 = parseSvgDims(renderStudioFretboardSvg(sg.tracks[0], { fretboardZoom: 80 }));
  const fb140 = parseSvgDims(renderStudioFretboardSvg(sg.tracks[0], { fretboardZoom: 140 }));
  assert.ok(fb140.hNum > fb80.hNum, 'fretboardZoom must scale fretboard SVG height');
  assert.ok(fb140.wNum > 1240, 'fretboardZoom > 100% must emit expanded numeric SVG width');

  const clampedPrefs = clampStudioPrefs({ tabZoom: 999, fretboardZoom: 10, detectSensitivityOffset: 99 });
  assert.equal(clampedPrefs.tabZoom, 160);
  assert.equal(clampedPrefs.fretboardZoom, 75);
  assert.equal(clampedPrefs.detectSensitivityOffset, 5);

  // E. Verified detection sensitivity baseline (Domyślna = 0 -> 0.58 / 0.42) & relative offset mapping
  const baseSens = sensitivityOffsetToThresholds(0);
  assert.equal(baseSens.offset, 0);
  assert.equal(baseSens.thresholdOn, DEFAULT_DSP_THRESHOLD_ON);
  assert.equal(baseSens.highThresholdOn, DEFAULT_DSP_HIGH_THRESHOLD_ON);
  assert.equal(baseSens.thresholdOn, 0.58);
  assert.equal(baseSens.highThresholdOn, 0.42);

  const moreSens = sensitivityOffsetToThresholds(3);
  const lessSens = sensitivityOffsetToThresholds(-3);
  assert.ok(moreSens.thresholdOn < baseSens.thresholdOn && moreSens.highThresholdOn < baseSens.highThresholdOn, '+offset lowers threshold');
  assert.ok(lessSens.thresholdOn > baseSens.thresholdOn && lessSens.highThresholdOn > baseSens.highThresholdOn, '-offset raises threshold');
  assert.equal(thresholdToSensitivityOffset(0.58), 0);
  assert.ok(thresholdToSensitivityOffset(0.49) > 0);
  assert.ok(thresholdToSensitivityOffset(0.70) < 0);

  console.log('[OK] Test 30: Songsterr TAB/audio (tied/w/bar/H/P/bend/vib/dyn), Zoom TAB+Gryfu oraz wrażliwość detekcji');
}

// 31. Bezkolizyjne pozycje/oktawy zaznaczenia, schowek Arpeggio/Sweep oraz tryb Uproszczony <-> Pro
{
  // A. Zaznaczenie fragmentu akordowo-melodycznego -> rozdzielenie tej samej oktawy i innych oktaw bez kolizji strun
  const song = createEmptySong({ measureCount: 2, bpm: 120 });
  song.tracks[0].measures[0].beats = [
    createBeat({ duration: 0, isRest: false, notes: [
      createTabNote({ string: 0, fret: 5, palmMute: true }), // A2 (45)
      createTabNote({ string: 1, fret: 7, palmMute: true })  // E3 (52)
    ] }),
    createBeat({ duration: 0, isRest: false, notes: [
      createTabNote({ string: 1, fret: 3 }), // C3 (48)
      createTabNote({ string: 2, fret: 5 })  // G3 (55)
    ] }),
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 1, fret: 5, hammerOn: true })] }), // D3 (50)
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 2, fret: 2, whammyPreset: 'dive_full' })] }) // E3 (52)
  ];
  const sel = { trackIdx: 0, anchor: { measureIdx: 0, beatIdx: 0 }, focus: { measureIdx: 0, beatIdx: 3 } };

  const fullNeckAnalysis = analyzeSelectionPositionsAndOctaves(song, 0, sel, { maxFret: 24 });
  assert.ok(fullNeckAnalysis.sameOctave.length >= 2, `Should offer >=2 collision-free same-octave positions, got ${fullNeckAnalysis.sameOctave.length}`);
  for (const pos of [...fullNeckAnalysis.sameOctave, ...fullNeckAnalysis.allOctavePositions]) {
    const byBeat = new Map();
    for (const a of pos.assignments) {
      const k = `${a.measureIdx}:${a.beatIdx}`;
      if (!byBeat.has(k)) byBeat.set(k, new Set());
      const used = byBeat.get(k);
      assert.equal(used.has(a.newString), false, `String collision in ${pos.label} beat ${k}`);
      used.add(a.newString);
      assert.ok(a.newFret >= 0 && a.newFret <= 24);
    }
    assert.ok((pos.maxFret - pos.minFret) <= 7, `Span too wide in ${pos.label}: ${pos.minFret}-${pos.maxFret}`);
  }
  const octShifts24 = fullNeckAnalysis.octaveOptions.map(o => o.octaveShift);
  assert.ok(octShifts24.includes(1) && octShifts24.includes(2), '+1 i +2 okt. muszą być wykryte przy gryfie 24 progów');
  assert.equal(octShifts24.includes(-1), false, '-1 okt. poniżej E2 nie może być oferowana');

  // Ograniczenie gryfu (np. maxDisplayedFret = 14) dynamicznie ogranicza wykryte oktawy (bez +2 okt.)
  const shortNeckAnalysis = analyzeSelectionPositionsAndOctaves(song, 0, sel, { maxFret: 14 });
  assert.equal(shortNeckAnalysis.octaveOptions.some(o => o.octaveShift >= 2), false, '+2 okt. (G5=próg 15) nie mieści się na 14 progach');
  assert.ok(shortNeckAnalysis.allOctavePositions.every(p => p.maxFret <= 14));

  // Bas 4-strunowy również wykrywa pozycje i oktawy bez kolizji strun (<4 struny)
  const bassSong = createEmptySong({ measureCount: 1 });
  const bIdx = addTrack(bassSong, 'bass');
  bassSong.tracks[bIdx].measures[0].beats = [
    createBeat({ duration: 0, isRest: false, notes: [
      createTabNote({ string: 0, fret: 0 }), // E1 (28)
      createTabNote({ string: 1, fret: 2 })  // B1 (35)
    ] }),
    createBeat({ duration: 0, isRest: false, notes: [createTabNote({ string: 1, fret: 5 })] }) // D2 (38)
  ];
  const bassSel = { trackIdx: bIdx, anchor: { measureIdx: 0, beatIdx: 0 }, focus: { measureIdx: 0, beatIdx: 1 } };
  const bassAnalysis = analyzeSelectionPositionsAndOctaves(bassSong, bIdx, bassSel, { maxFret: 21 });
  assert.ok(bassAnalysis.octaveOptions.some(o => o.octaveShift === 1), 'Bas 4-str. musi wykrywać +1 okt.');
  assert.ok(bassAnalysis.allOctavePositions.every(p => p.assignments.every(a => a.newString < 4)));

  // Przesunięcie w niemożliwy zakres (-12 półt. poniżej E2) NIE niszczy ani jednej nuty (atomowa odmowa)
  const beforeSnap = JSON.stringify(song.tracks[0].measures[0]);
  const badShift = shiftSelectionPitch(song, 0, sel, -12, { maxFret: 24 });
  assert.equal(badShift.beatsChanged, 0);
  assert.equal(badShift.outOfRange, true);
  assert.equal(JSON.stringify(song.tracks[0].measures[0]), beforeSnap, 'Out-of-range shift must not partially mutate notes');

  // Zastosowanie +1 okt. zachowuje techniki (palmMute, hammerOn, whammyPreset) i podnosi MIDI o +12
  const oct1Pos = fullNeckAnalysis.octaveOptions.find(o => o.octaveShift === 1).positions[0];
  assert.equal(applyPhrasePosition(song, 0, oct1Pos), 6);
  const trG = song.tracks[0];
  assert.equal(noteToMidi(trG, trG.measures[0].beats[0].notes[0]), 57);
  assert.equal(noteToMidi(trG, trG.measures[0].beats[0].notes[1]), 64);
  assert.equal(trG.measures[0].beats[0].notes[0].palmMute, true);
  assert.equal(trG.measures[0].beats[2].notes[0].hammerOn, true);
  assert.equal(trG.measures[0].beats[3].notes[0].whammyPreset, 'dive_full');

  // B. Bezpośredni solver pojedynczego uderzenia odmawia kolizji (np. dwudźwięk niemożliwy na gryfie)
  assert.equal(solveBeatFretAssignments(trG, [createTabNote({ string: 0, fret: 0 }), createTabNote({ string: 1, fret: 2 })], -12, { maxFret: 24 }), null);

  // C. Mostek kopiowania z Arpeggio/Sweep (buildBeatsClipboardFromSteps -> pasteBeatsFromClipboard w istniejący utwór)
  const sweepClip = buildBeatsClipboardFromSteps([
    { notes: [{ string: 1, fret: 7, hammerOn: true, tap: true }], duration: 2, tupletNumerator: 6, tupletDenominator: 4, chordSymbol: 'Em' },
    { s: 2, f: 9, tech: 'H', tapped: false, duration: 2, tupletNumerator: 6, tupletDenominator: 4 }
  ], { label: 'Sweep Em' });
  assert.equal(sweepClip.kind, 'beats');
  assert.equal(sweepClip.label, 'Sweep Em');
  assert.equal(sweepClip.beatCount, 2);
  assert.equal(sweepClip.beats[0].notes[0].tap, true);
  assert.equal(sweepClip.beats[0].tupletNumerator, 6);
  const targetSong = createEmptySong({ measureCount: 2 });
  setNoteAt(targetSong, 0, 0, 0, 0, 3);
  const pasteRes = pasteBeatsFromClipboard(targetSong, sweepClip, { targetTrackIdx: 0, targetMeasureIdx: 1, targetBeatIdx: 0, mode: 'overwrite' });
  assert.ok(pasteRes && pasteRes.pastedCount === 2);
  assert.equal(getNoteAt(targetSong, 0, 0, 0, 0).fret, 3, 'Takty spoza miejsca wklejenia pozostają nietknięte');
  assert.equal(getNoteAt(targetSong, 0, 1, 0, 1).tap, true);

  // D. Tryb Uproszczony <-> Pro: rozwijanie powtórzeń na długość liniową + ukrywanie/odzyskiwanie danych Pro
  const proDemo = createDemoSong('rock_riff_em'); // 8 taktów z |: T.1..T.4 :|| ×2 -> 12 taktów po rozwinięciu
  assert.equal(hasProRepeatStructure(proDemo), true);
  proDemo.tracks[0].measures[0].beats[0].pickStroke = 1;
  proDemo.tracks[0].measures[0].beats[0].fermata = true;
  proDemo.tracks[0].measures[0].beats[0].notes[0].graceNote = true;
  proDemo.tracks[0].measures[0].beats[0].notes[0].trill = true;

  const { linearSong, sourceMap } = expandSongToLinearMeasures(proDemo);
  assert.equal(hasProRepeatStructure(linearSong), false, 'Rozwinięta wersja uproszczona nie ma znaków repetycji');
  assert.equal(linearSong.tracks[0].measures.length, 12, '4 takty powtórzone ×2 + 4 takty = 12 taktów');

  const baselineLinear = JSON.parse(JSON.stringify(linearSong));
  // Edytujemy w trybie Uproszczonym takt 1 oraz odpowiadające mu powtórzenie (takt 5), nie dotykając taktu 0
  setNoteAt(linearSong, 0, 1, 0, 0, 10);
  setNoteAt(linearSong, 0, 5, 0, 0, 10);
  const reconciled = reconcileLinearEditsIntoProSong(proDemo, linearSong, sourceMap, baselineLinear);
  assert.equal(reconciled.foldedToPro, true);
  assert.equal(reconciled.song.tracks[0].measures.length, 8, 'Powrót do Pro zwija pętlę z powrotem do 8 taktów');
  assert.equal(reconciled.song.tracks[0].measures[0].repeatOpen, true);
  assert.equal(reconciled.song.tracks[0].measures[3].repeatClose, 2);
  assert.equal(reconciled.song.tracks[0].measures[1].beats[0].notes[0].fret, 10, 'Edycje z trybu Uproszczonego zachowane');
  assert.equal(reconciled.song.tracks[0].measures[0].beats[0].pickStroke, 1, 'pickStroke z trybu Pro przetrwał');
  assert.equal(reconciled.song.tracks[0].measures[0].beats[0].fermata, true, 'fermata z trybu Pro przetrwała');
  assert.equal(reconciled.song.tracks[0].measures[0].beats[0].notes[0].trill, true, 'trill z trybu Pro przetrwał');

  const simpleSvg = renderStudioTabSvg(proDemo, { uiMode: 'simple', viewMode: 'tab', containerWidth: 960 });
  const proSvg = renderStudioTabSvg(proDemo, { uiMode: 'pro', viewMode: 'tab', containerWidth: 960 });
  assert.equal(simpleSvg.includes('×2'), false, 'Tryb Uproszczony chowa oznaczenia powtórzeń');
  assert.equal(simpleSvg.includes('𝄐'), false, 'Tryb Uproszczony chowa fermatę Pro');
  assert.ok(proSvg.includes('×2') && proSvg.includes('⊓') && proSvg.includes('𝄐'), 'Tryb Pro wyświetla pełne oznaczenia');

  const palSimple = renderStudioPaletteHtml({
    uiMode: 'simple',
    track: linearSong.tracks[0],
    beat: linearSong.tracks[0].measures[0].beats[0],
    note: linearSong.tracks[0].measures[0].beats[0].notes[0],
    measure: linearSong.tracks[0].measures[0],
    clipboard: sweepClip,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 }
  });
  assert.equal(palSimple.includes('struct:repeatOpen'), false);
  assert.equal(palSimple.includes('prompt:repeatClose'), false);
  assert.ok(palSimple.includes('Sweep Em'), 'Tooltip Wklej informuje o skopiowanej sekwencji Sweep/Arpeggio');

  const palPro = renderStudioPaletteHtml({
    uiMode: 'pro',
    track: proDemo.tracks[0],
    beat: proDemo.tracks[0].measures[0].beats[0],
    note: proDemo.tracks[0].measures[0].beats[0].notes[0],
    measure: proDemo.tracks[0].measures[0],
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 }
  });
  assert.ok(palPro.includes('struct:repeatOpen') && palPro.includes('🔁|:') && palPro.includes(':||🔁'));

  const serSimple = serializeStudioSong(linearSong, { uiMode: 'simple' }, { proBackup: { song: proDemo, sourceMap } });
  const deserSimple = deserializeStudioSong(serSimple);
  assert.ok(deserSimple.proBackup && deserSimple.proBackup.song.tracks[0].measures.length === 8);

  console.log('[OK] Test 31: Pozycje/oktawy zaznaczenia bez kolizji, kopiowanie Sweep/Arp oraz zgodność Uproszczony <-> Pro');
}

// 32. Zapamiętywanie wartości rytmicznej (1/16), Wajcha tylko w Pro, Przedłużenie dźwięku na pustym uderzeniu i dokładna synchronizacja Playhead
{
  const s32 = createEmptySong({ measureCount: 1, numerator: 4, denominator: 4 });
  // Ustaw tymczasowo 2 uderzenia w pierwszym takcie, aby przetestować wypełnianie niedopełnionego taktu 4/4 szesnastkami
  s32.tracks[0].measures[0].beats = [createBeat({ duration: 0, isRest: true }), createBeat({ duration: 0, isRest: true })];
  const ui32 = {
    song: s32,
    history: createStudioHistory(20),
    activeTrackIdx: 0,
    cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
    activeDuration: null,
    fretInputMode: 'edit',
    studioPrefs: { uiMode: 'simple' },
    loopEnabled: false,
    loopStart: 0,
    loopEnd: 0,
    clipboard: null,
    clipboardScope: 'track',
    clipboardPasteMode: 'overwrite',
    selectionStart: null,
    selectionEnd: null,
    beatSelection: null,
    inlinePrompt: null,
    cmdPalette: { open: false, query: '', selectedIdx: 0 }
  };
  const ctrl32 = createStudioInputController({
    getState: () => ui32,
    pushUndo: () => ui32.history.push(ui32.song),
    onSongChanged: () => {},
    onRender: () => {}
  });

  // A. Wybór 1/16 (dur:2) zostaje zapamiętany dla kolejnych nut, pustych pól, +B i nowego taktu
  assert.equal(ctrl32.executeCommand('dur:2'), true);
  assert.equal(ui32.activeDuration, 2);
  assert.equal(s32.tracks[0].measures[0].beats[0].duration, 2);
  assert.equal(s32.tracks[0].measures[0].beats[1].duration, 2, 'Sąsiednia pusta pauza przejmuje 1/16');

  // Wpisanie cyfry progu na 1. i przesunięcie strzałką w prawo na kolejne sloty
  ctrl32.handleKeyDown({ key: '5', ctrlKey: false, metaKey: false, altKey: false, shiftKey: false, preventDefault() {} });
  ctrl32.handleKeyDown({ key: 'ArrowRight', ctrlKey: false, metaKey: false, altKey: false, shiftKey: false, preventDefault() {} });
  assert.equal(ui32.cursor.beatIdx, 1);
  assert.equal(s32.tracks[0].measures[0].beats[1].duration, 2);
  ctrl32.handleKeyDown({ key: '7', ctrlKey: false, metaKey: false, altKey: false, shiftKey: false, preventDefault() {} });
  ctrl32.handleKeyDown({ key: 'ArrowRight', ctrlKey: false, metaKey: false, altKey: false, shiftKey: false, preventDefault() {} });
  assert.equal(ui32.cursor.measureIdx, 0, 'Niedopełniony takt 4/4 dopisuje 3. slot szesnastkowy zamiast wracać do 1/4');
  assert.equal(ui32.cursor.beatIdx, 2);
  assert.equal(s32.tracks[0].measures[0].beats[2].duration, 2);

  // Kliknięcie gryfu + "+B" + nowy takt również zachowują 1/16
  ctrl32.handleFretClick(0, 9);
  ctrl32.executeCommand('edit:insertBeat');
  assert.equal(s32.tracks[0].measures[0].beats[ui32.cursor.beatIdx].duration, 2);
  ctrl32.executeCommand('struct:addAfter');
  assert.equal(s32.tracks[0].measures[1].beats[0].duration, 2, 'Nowy takt po wyborze 1/16 zaczyna się od 1/16');
  assert.equal(s32.tracks[0].measures[1].beats.length, 16, 'Pełny takt 4/4 w trybie 1/16 ma 16 slotów');

  const palStickyHtml = renderStudioPaletteHtml({
    uiMode: 'simple',
    track: s32.tracks[0],
    beat: s32.tracks[0].measures[1].beats[0],
    note: null,
    measure: s32.tracks[0].measures[1],
    cursor: ui32.cursor,
    activeDuration: ui32.activeDuration
  });
  assert.ok(palStickyHtml.includes('class="studio-pal-btn active" data-action="palette-cmd" data-cmd="dur:2"'), 'Paleta utrzymuje podświetlenie 1/16');

  // B. Tryb uproszczony blokuje Wajchę, a pozwala na Przedłużenie dźwięku na pustym uderzeniu (tieOrExtendPreviousNote)
  setNoteAt(s32, 0, 1, 0, 0, 5);
  ui32.cursor = { measureIdx: 1, beatIdx: 0, stringIdx: 0 };
  assert.equal(ctrl32.executeCommand('whammy:dive_full'), false, 'Wajcha zablokowana w trybie uproszczonym');
  ui32.studioPrefs.uiMode = 'pro';
  assert.equal(ctrl32.executeCommand('whammy:dive_full'), true, 'Wajcha dostępna w trybie Pro');
  ui32.studioPrefs.uiMode = 'simple';

  // Przesuń kursor na puste uderzenie B.1 po nucie z B.0 i wciśnij Przedłuż (effect:tied / L)
  ui32.cursor = { measureIdx: 1, beatIdx: 1, stringIdx: 0 };
  assert.equal(ctrl32.executeCommand('effect:tied'), true);
  const extNote = getNoteAt(s32, 0, 1, 1, 0);
  assert.ok(extNote && extNote.fret === 5 && extNote.tied === true, 'Puste pole przedłuża poprzednią nutę z tie=true');

  // Przedłużenie działa również przez granicę taktu i dla całego akordu na pustym uderzeniu
  const tieSong = createEmptySong({ measureCount: 2, numerator: 4, denominator: 4 });
  const bar0LastIdx = tieSong.tracks[0].measures[0].beats.length - 1;
  setNoteAt(tieSong, 0, 0, bar0LastIdx, 0, 3);
  setNoteAt(tieSong, 0, 0, bar0LastIdx, 1, 5);
  const crossRes = tieOrExtendPreviousNote(tieSong, 0, 1, 0, 2, 1);
  assert.equal(crossRes.changed, true);
  assert.equal(crossRes.extended, true);
  assert.equal(getNoteAt(tieSong, 0, 1, 0, 0).tied, true);
  assert.equal(getNoteAt(tieSong, 0, 1, 0, 1).tied, true, 'Cały akord przeniesiony i połączony łukiem przez granicę taktu');
  const tieSvg = renderStudioTabSvg(tieSong, { uiMode: 'simple', viewMode: 'tab', containerWidth: 920 });
  assert.ok(tieSvg.includes('studio-tie-arc'), 'Przedłużony dźwięk rysuje łuk Tie w SVG');
  const tieEvs = buildBeatEventsForTrack(tieSong, 0);
  assert.equal(tieEvs.length, 1, 'Połączony akord gra jako jedno ciągłe zdarzenie audio');
  assert.ok(tieEvs[0].notes.every(n => Math.abs(n.tieExtraQ - 0.5) < 1e-5));

  // C. Dokładna synchronizacja Playhead (linii przechodzącej) z pozycjami nut `beatsCx` w pełnym i niedopełnionym takcie
  const syncSong = createEmptySong({ bpm: 120, measureCount: 1, numerator: 4, denominator: 4 });
  // Dwie krótkie szesnastki w takcie 4/4 (tak jak podczas komponowania niedopełnionego taktu)
  syncSong.tracks[0].measures[0].beats = [
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 0, fret: 5 })] }),
    createBeat({ duration: 2, isRest: false, notes: [createTabNote({ string: 0, fret: 7 })] })
  ];
  const syncSvg = renderStudioTabSvg(syncSong, { uiMode: 'simple', viewMode: 'tab', containerWidth: 900 });
  const boundsMatch = syncSvg.match(/data-beats-cx="([^"]+)"\s+data-inner-right="([^"]+)"/);
  assert.ok(boundsMatch, 'SVG musi emitować dokładne pozycje nut data-beats-cx');
  const parsedCx = boundsMatch[1].split(',').map(Number);
  const parsedInnerRight = Number(boundsMatch[2]);
  assert.equal(parsedCx.length, 2);

  let fakeSec = 0;
  let lastPosEv = null;
  const syncPlayer = createStudioPlayer({
    host: { ensureAudioCtx: () => null, pluckGuitarFrequency: () => {}, playMetronomeTickAudio: () => {}, stopAllPreviewVoices: () => {} },
    clockFn: () => fakeSec,
    onPositionChange: (p) => { lastPosEv = p; }
  });
  syncPlayer.loadSong(syncSong, 0);
  syncPlayer.play();
  // T=0 -> nuta 0 (beatProgress=0) -> px musi trafiać dokładnie w środek nuty 0 (parsedCx[0])
  assert.equal(lastPosEv.beatIdx, 0);
  assert.ok(Math.abs(lastPosEv.beatProgress - 0) < 1e-6);
  const pxBeat0 = computePlayheadXForMeasure({
    beatsCx: parsedCx, innerRight: parsedInnerRight, barX: 60, barW: 500,
    beatIdx: lastPosEv.beatIdx, beatProgress: lastPosEv.beatProgress, barProgress: lastPosEv.progress
  });
  assert.ok(Math.abs(pxBeat0 - parsedCx[0]) < 0.05, `Playhead beat 0 (${pxBeat0}) musi trafiać w cx[0] (${parsedCx[0]})`);

  // T=0.125 s przy 120 BPM (= 0.25 Q = początek nuty 1) -> px musi być dokładnie w cx[1]!
  fakeSec = 0.125;
  syncPlayer.stepSchedulerForTest(fakeSec);
  assert.equal(lastPosEv.beatIdx, 1);
  assert.ok(Math.abs(lastPosEv.beatProgress - 0) < 1e-6);
  const pxBeat1 = computePlayheadXForMeasure({
    beatsCx: parsedCx, innerRight: parsedInnerRight, barX: 60, barW: 500,
    beatIdx: lastPosEv.beatIdx, beatProgress: lastPosEv.beatProgress, barProgress: lastPosEv.progress
  });
  assert.ok(Math.abs(pxBeat1 - parsedCx[1]) < 0.05, `Playhead beat 1 (${pxBeat1}) musi trafiać dokładnie w cx[1] (${parsedCx[1]})`);

  // Weryfikacja seekTo na drugie uderzenie w trakcie odtwarzania
  syncPlayer.pause();
  syncPlayer.seekTo(0, 1);
  syncPlayer.play();
  assert.equal(lastPosEv.beatIdx, 1, 'seekTo(0, 1) musi wznawiać dokładnie od 2. uderzenia');
  syncPlayer.destroy();

  console.log('[OK] Test 32: Zapamiętywanie 1/16, Wajcha w Pro, Przedłużenie dźwięku i synchronizacja Playhead');
}

console.log('\nWszystkie testy Studio TAB (32/32) zakończone sukcesem!');
