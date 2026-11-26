import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const htmlSrc = fs.readFileSync(new URL('../app/ui/DesktopGuiHtml.h', import.meta.url), 'utf8');
const scripts = [...htmlSrc.matchAll(/<script(?:\s+[^>]*)?>([\s\S]*?)<\/script>/g)]
  .map(m => m[1])
  .filter(s => s.trim().length > 0);
assert.ok(scripts.length >= 1, 'Expected inline <script> inside DesktopGuiHtml.h');
const scriptSrc = scripts.find(s => s.includes('const state =')) || scripts[0];

// 1. Statyczna weryfikacja braku skopiowanych fragmentów, zakazanej nazwy dźwięku H i zachowania wspólnych klas
assert.equal(/Zrzut(?:y)?\s*1[–-]7|Losowe ćwiczenie dla Ciebie/i.test(htmlSrc), false,
  'Ear Training must not contain screenshot-clone comments or marketing copy');
assert.equal(/\.ear-config-layout|\.ear-summary-box|\.ear-intervals-grid|\.ear-trainer-screen|\.ear-answer-grid/.test(htmlSrc), false,
  'Legacy cloned .ear-* layout classes must be replaced by .et-*');
assert.ok(htmlSrc.includes('.ear-seg-row') && htmlSrc.includes('.ear-seg-btn') && htmlSrc.includes('.ear-toggle-switch'),
  'Shared .ear-seg-row/.ear-seg-btn/.ear-toggle-switch CSS must stay intact for other views');

const elementsById = new Map();
function makeElement(id) {
  const classes = new Set();
  return {
    id,
    innerHTML: '',
    textContent: '',
    value: '',
    style: {},
    dataset: {},
    classList: {
      add: (...c) => c.forEach(x => classes.add(x)),
      remove: (...c) => c.forEach(x => classes.delete(x)),
      toggle: (c, force) => {
        if (force === undefined) {
          if (classes.has(c)) classes.delete(c); else classes.add(c);
        } else if (force) classes.add(c); else classes.delete(c);
      },
      contains: (c) => classes.has(c)
    },
    getAttribute: () => '',
    setAttribute: () => {}
  };
}

function getEl(id) {
  if (!elementsById.has(id)) elementsById.set(id, makeElement(id));
  return elementsById.get(id);
}

const storageMap = new Map();
const keydownListeners = [];
const audioCalls = [];

const sandbox = {
  console,
  Math,
  Date,
  Set,
  Map,
  Array,
  Object,
  Number,
  String,
  Boolean,
  JSON,
  Error,
  performance: { now: () => 1000 },
  setTimeout: (fn) => { if (typeof fn === 'function') fn(); return 1; },
  clearTimeout: () => {},
  setInterval: () => 1,
  clearInterval: () => {},
  requestAnimationFrame: () => 1,
  cancelAnimationFrame: () => {},
  localStorage: {
    getItem: (k) => storageMap.has(k) ? storageMap.get(k) : null,
    setItem: (k, v) => { storageMap.set(k, String(v)); },
    removeItem: (k) => { storageMap.delete(k); }
  },
  document: {
    documentElement: {
      setAttribute: () => {},
      getAttribute: () => ''
    },
    body: { classList: makeElement('body').classList },
    getElementById: (id) => getEl(id),
    querySelectorAll: () => [],
    addEventListener: () => {}
  },
  window: {
    addEventListener: (ev, fn) => {
      if (ev === 'keydown' && typeof fn === 'function') keydownListeners.push(fn);
    },
    removeEventListener: () => {},
    matchMedia: () => ({ matches: false })
  },
  navigator: {},
  ResizeObserver: class { observe() {} disconnect() {} },
  fetch: async () => ({ ok: true, json: async () => ({}) }),
  alert: () => {},
  confirm: () => true
};
sandbox.globalThis = sandbox;

const ctx = vm.createContext(sandbox);
const harnessSrc = `
  window.__testAudioCalls = [];
  window.__testMicToggleCount = 0;
  pluckGuitarFrequency = function(freqHz, dur, gain) {
    window.__testAudioCalls.push({ inst: 'guitar', freqHz, dur, gain });
  };
  playKeyboardPresetNote = function(freqHz, dur, gain) {
    window.__testAudioCalls.push({ inst: 'piano', preset: state.keyboardPreset, freqHz, dur, gain });
  };
  playMetronomeTickAudio = function(downbeat) {
    window.__testAudioCalls.push({ inst: 'metro', downbeat: !!downbeat });
  };
  playDrumHit = function(kind, vel) {
    window.__testAudioCalls.push({ inst: 'drum', kind, vel });
  };
  toggleLiveAudio = function() {
    window.__testMicToggleCount++;
  };
  renderStageAndInspector = function() {
    if (state.mainView === 'ear') {
      renderEarView(document.getElementById('mainStage'));
    }
  };
  globalThis.__earTestResults = (() => {
    const out = {};
    const stage = document.getElementById('mainStage');
    state.mainView = 'ear';
    state.earScreen = 'config';
    state.earMode = 'identify';
    renderStageAndInspector();
    out.configHtml = stage.innerHTML;

    // Zakazana nazwa 'H' nie może pojawić się przy formatowaniu żadnej nuty (oba tryby znaków przygodnych)
    state.accMode = 2;
    const sharpAll = Array.from({ length: 24 }, (_, i) => earFormatMidi(40 + i)).join(' ');
    state.accMode = 1;
    const flatAll = Array.from({ length: 24 }, (_, i) => earFormatMidi(40 + i)).join(' ');
    out.hasLetterHInNotes = /\\bH/.test(sharpAll) || /\\bH/.test(flatAll);
    state.accMode = 2;

    // Presety i losowanie zestawu
    applyEarPreset('diatonic');
    out.diatonicCount = state.earSelectedSemis.size;
    applyEarPreset('rock_blues');
    out.rockCount = state.earSelectedSemis.size;
    state.earMode = 'chords';
    applyEarPreset('triads');
    out.triadsCount = state.earSelectedChords.size;
    state.earMode = 'scales';
    applyEarPreset('modes');
    out.modesCount = state.earSelectedScales.size;
    randomizeEarMaterial();
    out.randScalesCount = state.earSelectedScales.size;

    // Test playEarTone z instrumentem piano poza trybem akordów
    window.__testAudioCalls = [];
    state.earMode = 'identify';
    state.earInstrument = 'piano';
    state.keyboardPreset = 'synth';
    playEarTone(440, 1.0);
    out.identifyPianoCall = window.__testAudioCalls[0];
    out.restoredKeyboardPreset = state.keyboardPreset;

    // Test 5 trybów + podsumowania + odsłuchu porównawczego
    // A) Rozpoznaj (identify)
    state.earMode = 'identify';
    state.earInstrument = 'guitar';
    state.earTargetRounds = 2;
    state.earSessionType = 'fixed';
    applyEarPreset('basic');
    startEarTrainingSession();
    out.activeIdentifyHtml = stage.innerHTML;
    const q1 = state.earCurrentQuestion;
    submitEarIdentifyAnswer(q1.semi);
    out.answeredIdentifyHtml = stage.innerHTML;
    const otherSemi = q1.semi === 7 ? 5 : 7;
    auditionEarAnswerChoice('interval', otherSemi);
    out.auditionLabel = state.earCurrentQuestion.auditionLabel;
    nextEarActiveRound();
    const q2 = state.earCurrentQuestion;
    submitEarIdentifyAnswer(q2.semi === 7 ? 5 : 7); // celowo błędna odpowiedź w 2. rundzie
    nextEarActiveRound();
    out.summaryScreenAfterIdentify = state.earScreen;
    out.summaryHtml = stage.innerHTML;

    // B) Słabsze elementy z sesji i z localStorage
    const weakCount = getEarWeakListForMode('identify').length;
    out.persistentWeakCount = weakCount;
    startEarWeakItemsSession();
    out.weakSessionScreen = state.earScreen;
    out.weakSelectedCount = state.earSelectedSemis.size;

    // C) Nauka (learn) + A/B compare
    state.earMode = 'learn';
    startEarTrainingSession();
    onEarLearnTileClick(7);
    out.learnHtml = stage.innerHTML;
    toggleEarCompareMode();
    onEarLearnTileClick(3);
    onEarLearnTileClick(4);
    out.compareCount = state.earCompareSelection.length;

    // D) Zagraj (play) + #earPlayDetectedBox + pominięcie + mikrofon
    state.earMode = 'play';
    state.earTargetRounds = 3;
    startEarTrainingSession();
    out.playHtmlBeforeHit = stage.innerHTML;
    checkEarPlayGuitarMidi(state.earCurrentQuestion.targetMidi);
    out.playHtmlAfterHit = stage.innerHTML;
    nextEarActiveRound();
    skipEarPlayRound();
    out.playSkippedAnswered = state.earCurrentQuestion.answered && !state.earCurrentQuestion.wasCorrect;

    // E) Akordy (1 przewrót -> klik natychmiastowy; >1 przewrót -> Sprawdź)
    state.earMode = 'chords';
    state.earSelectedChords = new Set(['major', 'minor', '7']);
    state.earSelectedInversions = new Set([0]);
    startEarTrainingSession();
    onEarChordTileClick(state.earCurrentQuestion.chordId);
    out.chordSingleInvAutoSubmitted = state.earCurrentQuestion.answered;
    state.earSelectedInversions = new Set([0, 1]);
    startEarTrainingSession();
    onEarChordTileClick('major');
    out.chordMultiInvNeedsSubmit = !state.earCurrentQuestion.answered;
    submitEarChordAnswer();
    out.chordMultiInvSubmitted = state.earCurrentQuestion.answered;

    // F) Skale i tryby + SVG gryfu (gitara 6-str. + bas 4-str.)
    state.earMode = 'scales';
    applyEarPreset('basic');
    startEarTrainingSession();
    submitEarScaleAnswer(state.earCurrentQuestion.scaleId);
    out.scalesAnsweredHtml = stage.innerHTML;

    state.bass4Mode = true;
    state.customStringMidis = [28, 33, 38, 43, 59, 64];
    out.bassSvg = renderEarMiniFretboardSvg({ kind: 'interval', rootMidi: 40, targetMidis: [47] });
    state.bass4Mode = false;
    state.customStringMidis = [40, 45, 50, 55, 59, 64];

    // G) Uczciwe „Losowe ćwiczenie” + skróty klawiszowe
    startEarRandomSession();
    out.randomRollInfo = state.earRandomRollInfo;
    out.randomMode = state.earMode;

    // H) Tryb 6: Funkcje w tonacji (degrees) — dur/moll, kadencja I-IV-V-I, A/B odsłuch, SVG
    state.earMode = 'degrees';
    applyEarPreset('deg_major');
    out.degMajorCount = state.earSelectedDegrees.size;
    applyEarPreset('deg_minor');
    out.degMinorCount = state.earSelectedDegrees.size;
    window.__testAudioCalls = [];
    startEarTrainingSession();
    out.degCadenceAudioCount = window.__testAudioCalls.length;
    out.degConfigPromptHtml = stage.innerHTML;
    submitEarDegreeAnswer(state.earCurrentQuestion.semi);
    out.degWasCorrect = state.earCurrentQuestion.wasCorrect;
    auditionEarAnswerChoice('degree', 7);
    out.degAuditionOk = (state.earCurrentQuestion.auditionLabel || '').includes('Odsłuch porównawczy');

    // I) Tryb 7: Dyktando motywów (dictation) — generowanie, klik SVG, live mic i Backspace
    state.earMode = 'dictation';
    state.earDictationLength = 4;
    state.earDictationScaleId = 'pent_minor';
    startEarTrainingSession();
    out.dictMotifLen = state.earCurrentQuestion.motifMidis.length;
    out.dictSvgHasClick = stage.innerHTML.includes("submitEarDictationNote(");
    const targetMotif = state.earCurrentQuestion.motifMidis.slice();
    submitEarDictationNote(targetMotif[0], 'click');
    submitEarDictationNote(targetMotif[1], 'click');
    handleEarKeyDown({ key: 'Backspace', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => {} });
    out.dictAfterUndoLen = state.earCurrentQuestion.userMidis.length;
    handleEarLiveInputMidi(targetMotif[1]);
    submitEarDictationNote(targetMotif[2], 'click');
    submitEarDictationNote(targetMotif[3], 'click');
    out.dictSolvedCorrectly = state.earCurrentQuestion.answered && state.earCurrentQuestion.wasCorrect;

    // J) Tryb 8: Progresje akordów (progressions) — sloty, Backspace, sprawdzenie i nazwy A–G
    state.earMode = 'progressions';
    applyEarPreset('prog_all');
    startEarTrainingSession();
    out.progConfigHas4Slots = stage.innerHTML.includes('earProgSlot_3');
    const trueProg = state.earCurrentQuestion.stepIds.slice();
    pickEarProgFunction(trueProg[0]);
    pickEarProgFunction(trueProg[1]);
    handleEarKeyDown({ key: 'Backspace', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => {} });
    out.progSlotsAfterUndo = state.earProgUserSlots.length;
    pickEarProgFunction(trueProg[1]);
    pickEarProgFunction(trueProg[2]);
    pickEarProgFunction(trueProg[3]);
    submitEarProgressionAnswer();
    out.progSolved = state.earCurrentQuestion.answered && state.earCurrentQuestion.wasCorrect;
    out.progHtmlHasH = /\\bH[m°]?\\b/.test(stage.innerHTML.replace(/<[^>]+>/g, ' '));
    playEarProgSlotChord(0);

    // K) Tryb 9: Trening rytmiczny (rhythm) — bez natychmiastowego wygaszania timerów
    state.earMode = 'rhythm';
    state.earRhythmPatternId = 'quarters';
    state.earRhythmBpm = 120;
    state.earRhythmBars = 2;
    startEarTrainingSession();
    const origSetTimeout = setTimeout;
    const savedRhythmTimers = [];
    setTimeout = (fn) => { savedRhythmTimers.push(fn); return savedRhythmTimers.length; };
    try {
      startEarRhythmRun();
      const rs = state.earRhythmState;
      out.rhythmRunning = state.earRhythmRunning;
      out.rhythmSlotCount = rs.slots.length;
      registerEarRhythmTap('keyboard', rs.slots[0].expectedMs + 10);  // perfect
      registerEarRhythmTap('keyboard', rs.slots[1].expectedMs - 50);  // close
      registerEarRhythmTap('keyboard', rs.slots[2].expectedMs - 95);  // early/miss
      registerEarRhythmTap('keyboard', rs.slots[3].expectedMs + 95);  // late/miss
      out.rhythmStatuses = rs.slots.slice(0, 4).map(s => s.status);
      out.rhythmHasSvg = stage.innerHTML.includes('earRhythmOffsetSvgWrap') && renderEarRhythmTimingSvg(rs.hits).includes('<svg');
      const prevPerfNow = performance.now;
      performance.now = () => rs.slots[4].expectedMs + 5;
      try {
        handleEarLiveInputMidi(64); // live guitar hit routing on beat 5
      } finally {
        performance.now = prevPerfNow;
      }
      out.rhythmGuitarBeat5Status = rs.slots[4].status;
      finishEarSessionEarly();
      out.rhythmSummaryHtml = stage.innerHTML;
      out.rhythmStoppedOnFinish = !state.earRhythmRunning;
    } finally {
      setTimeout = origSetTimeout;
    }

    state.earMode = 'identify';
    startEarTrainingSession();
    const evSpace = { key: ' ', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => { out.spacePrevented = true; } };
    out.spaceHandled = handleEarKeyDown(evSpace);
    const evKey1 = { key: '1', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => { out.key1Prevented = true; } };
    out.key1Handled = handleEarKeyDown(evKey1);
    out.answeredByKey1 = state.earCurrentQuestion.answered;
    const evEnter = { key: 'Enter', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => {} };
    const rBefore = state.earRound;
    handleEarKeyDown(evEnter);
    out.roundAdvancedByEnter = state.earRound === rBefore + 1;
    const evEsc = { key: 'Escape', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => {} };
    handleEarKeyDown(evEsc);
    out.exitedByEsc = state.earScreen === 'config';

    // Sprawdź że w trybie 'play' klawisze numeryczne/QWERTY nie są przechwytywane
    state.earMode = 'play';
    startEarTrainingSession();
    out.playKey1NotHandled = handleEarKeyDown({ key: '1', ctrlKey: false, metaKey: false, altKey: false, repeat: false, preventDefault: () => {} }) === false;

    clearEarPersistentStats();
    out.clearedWeakCount = getEarWeakListForMode('identify').length;
    return out;
  })();
`;
vm.runInContext(scriptSrc + '\n;\n' + harnessSrc, ctx, { filename: 'DesktopGuiHtml_ear_test.js' });
const testResults = sandbox.__earTestResults;

assert.equal(testResults.hasLetterHInNotes, false, 'Strict A-G naming must never emit H');
assert.ok(testResults.configHtml.includes('Losowe ćwiczenie'), 'Top bar must offer honest Losowe ćwiczenie');
assert.equal(testResults.configHtml.includes('Zaskocz mnie'), false, 'Config must not show Zaskocz mnie');
assert.equal(testResults.configHtml.includes('Zrzut'), false, 'Config must not reference Zrzuty');
assert.equal(testResults.diatonicCount, 8);
assert.equal(testResults.rockCount, 7);
assert.equal(testResults.triadsCount, 4);
assert.equal(testResults.modesCount, 6);
assert.equal(testResults.randScalesCount, 4);
assert.equal(testResults.identifyPianoCall?.inst, 'piano', 'Piano instrument must work in interval mode too');
assert.equal(testResults.restoredKeyboardPreset, 'synth', 'keyboardPreset must be restored after playEarTone');
assert.ok(testResults.answeredIdentifyHtml.includes('<svg '), 'Answered identify question should render mini-fretboard SVG');
assert.ok(testResults.auditionLabel.includes('Odsłuch porównawczy'), 'Post-answer click must audition alternative choice');
assert.equal(testResults.summaryScreenAfterIdentify, 'summary');
assert.ok(testResults.summaryHtml.includes('PODSUMOWANIE SESJI'));
assert.equal(testResults.persistentWeakCount, 1);
assert.equal(testResults.weakSessionScreen, 'active');
assert.equal(testResults.weakSelectedCount, 2);
assert.ok(testResults.learnHtml.includes('<svg '));
assert.equal(testResults.compareCount, 2);
assert.ok(testResults.playHtmlBeforeHit.includes('id="earPlayDetectedBox"'), 'Play mode must expose earPlayDetectedBox');
assert.ok(testResults.playHtmlBeforeHit.includes('Włącz mikrofon'), 'Play mode must show mic CTA when micStream is off');
assert.ok(testResults.playHtmlAfterHit.includes('data-answered="1"'), 'Answered play box must include data-answered="1"');
assert.equal(testResults.playSkippedAnswered, true);
assert.equal(testResults.chordSingleInvAutoSubmitted, true);
assert.equal(testResults.chordMultiInvNeedsSubmit, true);
assert.equal(testResults.chordMultiInvSubmitted, true);
assert.ok(testResults.scalesAnsweredHtml.includes('<svg '), 'Scales quiz must render scale mini-fretboard SVG');
assert.ok(testResults.bassSvg.includes('<svg ') && !testResults.bassSvg.includes('>H<'), '4-string bass SVG must render with A-G notes');
assert.ok(testResults.randomRollInfo.startsWith('Losowe ćwiczenie:'), 'Random session must report rolled parameters');
assert.ok(['identify', 'chords', 'scales', 'degrees', 'progressions'].includes(testResults.randomMode));
assert.equal(testResults.degMajorCount, 7);
assert.equal(testResults.degMinorCount, 7);
assert.ok(testResults.degCadenceAudioCount >= 5, 'Cadence + degree must trigger chord + note audio');
assert.equal(testResults.degWasCorrect, true);
assert.equal(testResults.degAuditionOk, true);
assert.equal(testResults.dictMotifLen, 4);
assert.equal(testResults.dictSvgHasClick, true);
assert.equal(testResults.dictAfterUndoLen, 1);
assert.equal(testResults.dictSolvedCorrectly, true);
assert.equal(testResults.progConfigHas4Slots, true);
assert.equal(testResults.progSlotsAfterUndo, 1);
assert.equal(testResults.progSolved, true);
assert.equal(testResults.progHtmlHasH, false, 'Progression chord names must strictly use A-G');
assert.equal(testResults.rhythmRunning, true);
assert.equal(testResults.rhythmSlotCount, 8);
assert.equal(testResults.rhythmStatuses.join(','), 'perfect,close,miss,miss');
assert.equal(testResults.rhythmHasSvg, true);
assert.equal(testResults.rhythmGuitarBeat5Status, 'perfect');
assert.equal(testResults.rhythmStoppedOnFinish, true);
assert.ok(testResults.rhythmSummaryHtml.includes('PODSUMOWANIE SESJI'));
assert.equal(testResults.spaceHandled, true);
assert.equal(testResults.key1Handled, true);
assert.equal(testResults.answeredByKey1, true);
assert.equal(testResults.roundAdvancedByEnter, true);
assert.equal(testResults.exitedByEsc, true);
assert.equal(testResults.playKey1NotHandled, true);
assert.equal(testResults.clearedWeakCount, 0);

console.log('ear_web_test.mjs: ALL PASS');
