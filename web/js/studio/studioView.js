/**
 * Główny kontroler widoku 🎼 Studio TAB (Faza 1: Kompozytor + Player + Matryca Ścieżki × Takty).
 * Łączy studioModel.js, studioDrumBlocks.js, studioRenderer.js i studioPlayer.js z hostem aplikacji.
 */
import {
  DEFAULT_STUDIO_PREFS,
  MAX_STUDIO_TRACKS,
  NOTE_NAMES_SHARP,
  SCALE_HINT_OPTIONS,
  STUDIO_TUNING_PRESETS,
  TIME_SIGNATURE_PRESETS,
  TRACK_ROLE_PRESETS,
  addMeasure,
  addTrack,
  analyzePhrasePositions,
  analyzeSelectionPositionsAndOctaves,
  applyMetronomePreset,
  applyPhrasePosition,
  buildBeatsClipboardFromSteps,
  buildMeasuresFromSteps,
  buildSongExportPayload,
  clampStudioPrefs,
  cloneSong,
  copyDrumGridToAllMeasures,
  createDemoSong,
  createEmptyMeasure,
  createEmptySong,
  createStudioHistory,
  createTabNote,
  deserializeStudioSong,
  detectKeyFromSongJs,
  duplicateTrack,
  expandSongToLinearMeasures,
  findFretCandidatesForMidi,
  getMasterMeasureCount,
  getNoteAt,
  getScaleHintPitchClasses,
  hasProRepeatStructure,
  isBeatInSelection,
  midiToNoteName,
  normalizeBeatSelection,
  normalizeMetronomePattern,
  noteToMidi,
  reconcileLinearEditsIntoProSong,
  removeMeasure,
  removeTrack,
  resetMeasureDrumGrid,
  serializeStudioSong,
  sensitivityOffsetToThresholds,
  setCustomDrumCell,
  setCustomDrumResolution,
  setDrumBlockForMeasure,
  setMeasureTimeSignature,
  setTrackTuning,
  thresholdToSensitivityOffset,
  validateMeasureMeter
} from './studioModel.js';
import { DRUM_BLOCKS, DRUM_GRID_LANES, playStudioDrumHit } from './studioDrumBlocks.js';
import { computeActiveAndNextNotes, createStudioPlayer } from './studioPlayer.js';
import { createStudioInputController, createStudioMicController } from './studioInputs.js';
import { createSoundFontSynth } from './studioSoundFont.js';
import {
  filterStudioCommands,
  renderCommandPaletteHtml,
  renderInlinePromptBar,
  renderStudioPaletteHtml
} from './studioPalette.js';
import {
  computePlayheadXForMeasure,
  escapeHtml,
  renderArrangerMatrixHtml,
  renderMetronomeBlocksHtml,
  renderStudioFretboardSvg,
  renderStudioTabSvg
} from './studioRenderer.js';

const SPEED_PRESETS = [25, 50, 75, 90, 100, 125, 150];
const GUITAR_TONE_OPTIONS = [
  { id: 'acoustic', label: 'Akustyczna (Steel)' },
  { id: 'nylon', label: 'Klasyczna (Nylon)' },
  { id: 'clean', label: 'Elektryczna Clean' },
  { id: 'crunch', label: 'Blues Overdrive (Crunch)' },
  { id: 'lead', label: 'Rock / Metal Lead' },
  { id: 'chorus', label: 'Ambient Chorus' }
];
const KEYBOARD_TONE_OPTIONS = [
  { id: 'piano', label: 'Fortepian (Grand)' },
  { id: 'rhodes', label: 'Fender Rhodes' },
  { id: 'hammond', label: 'Organy Hammonda' },
  { id: 'organ', label: 'Organy klasyczne' },
  { id: 'synth', label: 'Syntezator Lead' }
];

const boundContainers = new WeakSet();

function getHost() {
  return (typeof window !== 'undefined' && window.__gitaraStudioHost) ? window.__gitaraStudioHost : null;
}

const studioUi = {
  song: createDemoSong('rock_riff_em'),
  demoId: 'rock_riff_em',
  savedProjectId: null,
  history: createStudioHistory(50),
  activeTrackIdx: 0,
  cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
  activeDuration: null,
  viewMode: 'tab',
  barsPerRow: 'auto',
  showScaleHints: true,
  loopEnabled: false,
  loopStart: 0,
  loopEnd: 3,
  speedPercent: 100,
  countInEnabled: false,
  metronomeEnabled: true,
  autoTempoEnabled: false,
  autoTempoStep: 4,
  countInText: '',
  statusMsg: '',
  fretInputMode: 'edit',
  clipboard: null,
  clipboardScope: 'track',
  clipboardPasteMode: 'overwrite',
  selectionStart: null,
  selectionEnd: null,
  beatSelection: null,
  scoreAllTracks: false,
  showScoreNoteNames: true,
  inlinePrompt: null,
  cmdPalette: { open: false, query: '', selectedIdx: 0 },
  rightPanelTab: 'tools',
  metroPanelOpen: false,
  arrangerOpen: false,
  drumBrushLevel: 2,
  lastMetroBeatCol: -1,
  studioPrefs: { ...DEFAULT_STUDIO_PREFS },
  proBackup: null,
  phrasePositions: [],
  selPosAnalysis: null,
  selectedPhrasePosIdx: -1,
  positionAreaRange: null,
  savedPositionAreaRange: null,
  lastPlacedNote: null,
  lastStageEl: null,
  lastInspEl: null,
  lastPlayPos: { active: false, measureIdx: 0, beatIdx: 0, progress: 0, beatProgress: 0 }
};

let statusTimer = null;
function showToast(msg) {
  studioUi.statusMsg = msg || '';
  const el = document.getElementById('studioToastMsg');
  if (el) {
    el.textContent = studioUi.statusMsg;
    el.style.display = studioUi.statusMsg ? 'inline-flex' : 'none';
  }
  if (statusTimer) clearTimeout(statusTimer);
  if (msg) {
    statusTimer = setTimeout(() => {
      studioUi.statusMsg = '';
      const tEl = document.getElementById('studioToastMsg');
      if (tEl) tEl.style.display = 'none';
    }, 3000);
  }
}

function syncPrefsFromHost() {
  const host = getHost();
  const st = host?.getState?.();
  if (st && st.studioPrefs) {
    studioUi.studioPrefs = clampStudioPrefs(st.studioPrefs);
  }
}

function persistPrefsToHost() {
  const host = getHost();
  const st = host?.getState?.();
  if (st) {
    st.studioPrefs = clampStudioPrefs(studioUi.studioPrefs);
    host.saveSettings?.();
  }
}

function formatSensitivityOffsetLabel(offset) {
  const off = Math.max(-5, Math.min(5, Math.round(Number(offset) || 0)));
  if (off === 0) return 'Domyślna';
  return off > 0 ? `+${off} (czulsza)` : `${off} (mniej czuła)`;
}

function getStudioMaxFret() {
  const raw = getHost()?.getState?.()?.maxDisplayedFret;
  return Math.max(12, Math.min(24, Math.round(Number(raw) || 24)));
}

function enterSimpleModeProjection() {
  if (!studioUi.song) return false;
  if (!studioUi.proBackup) {
    studioUi.proBackup = {
      song: cloneSong(studioUi.song),
      loopEnabled: Boolean(studioUi.loopEnabled),
      loopStart: Number(studioUi.loopStart) || 0,
      loopEnd: Number(studioUi.loopEnd) || 0,
      autoTempoEnabled: Boolean(studioUi.autoTempoEnabled),
      sourceMap: null,
      baselineLinearSong: null
    };
  }
  if (hasProRepeatStructure(studioUi.song)) {
    const { linearSong, sourceMap } = expandSongToLinearMeasures(studioUi.song);
    studioUi.proBackup.sourceMap = sourceMap;
    studioUi.proBackup.baselineLinearSong = cloneSong(linearSong);
    studioUi.song = linearSong;
  }
  studioUi.loopEnabled = false;
  studioUi.autoTempoEnabled = false;
  clampCursor();
  return true;
}

function exitSimpleModeToPro() {
  if (!studioUi.proBackup) return false;
  const bk = studioUi.proBackup;
  if (Array.isArray(bk.sourceMap) && bk.sourceMap.length > 0) {
    const res = reconcileLinearEditsIntoProSong(bk.song, studioUi.song, bk.sourceMap, bk.baselineLinearSong);
    studioUi.song = res.song;
  }
  const maxBar = Math.max(0, getMasterMeasureCount(studioUi.song) - 1);
  studioUi.loopEnabled = Boolean(bk.loopEnabled);
  studioUi.loopStart = Math.max(0, Math.min(maxBar, Number(bk.loopStart) || 0));
  studioUi.loopEnd = Math.max(studioUi.loopStart, Math.min(maxBar, Number(bk.loopEnd) || 0));
  studioUi.autoTempoEnabled = Boolean(bk.autoTempoEnabled);
  studioUi.proBackup = null;
  clampCursor();
  return true;
}

const STUDIO_IV_LABELS = ['1', '♭2', '2', '♭3', '3', '4', '♭5', '5', '♯5', '6', '♭7', '7'];

function resolveStudioOctaveAnchorPc(keyRootPc, keyScalePcs) {
  const scaleArr = Array.isArray(keyScalePcs) && keyScalePcs.length > 0 ? keyScalePcs : [keyRootPc];
  const raw = studioUi.studioPrefs?.octaveAnchorPc;
  return (raw !== null && raw !== undefined && Number.isInteger(Number(raw)) && scaleArr.includes(Number(raw)))
    ? Number(raw)
    : keyRootPc;
}

function getAccMode() {
  const host = getHost();
  return host?.getState?.()?.accMode === 1 ? 1 : 2;
}

function clampCursor() {
  const tracks = studioUi.song.tracks || [];
  if (tracks.length === 0) return;
  studioUi.activeTrackIdx = Math.max(0, Math.min(tracks.length - 1, studioUi.activeTrackIdx));
  const tr = tracks[studioUi.activeTrackIdx];
  const maxM = Math.max(0, (tr.measures?.length || 1) - 1);
  studioUi.cursor.measureIdx = Math.max(0, Math.min(maxM, studioUi.cursor.measureIdx));
  const m = tr.measures?.[studioUi.cursor.measureIdx];
  const maxB = Math.max(0, (m?.beats?.length || 1) - 1);
  studioUi.cursor.beatIdx = Math.max(0, Math.min(maxB, studioUi.cursor.beatIdx));
  const maxS = Math.max(0, (tr.openStringMidis?.length || 6) - 1);
  studioUi.cursor.stringIdx = Math.max(0, Math.min(maxS, studioUi.cursor.stringIdx));
  studioUi.loopStart = Math.max(0, Math.min(maxM, studioUi.loopStart));
  studioUi.loopEnd = Math.max(0, Math.min(maxM, studioUi.loopEnd));
}

function resolveSelectedNoteContext() {
  const track = studioUi.song.tracks[studioUi.activeTrackIdx];
  if (!track || track.instrumentType === 'drums' || track.isPercussion) return null;
  const curNote = getNoteAt(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, studioUi.cursor.beatIdx, studioUi.cursor.stringIdx);
  if (curNote) {
    const midi = noteToMidi(track, curNote);
    if (midi !== null) {
      return {
        measureIdx: studioUi.cursor.measureIdx,
        beatIdx: studioUi.cursor.beatIdx,
        string: curNote.string,
        fret: curNote.fret,
        midi
      };
    }
  }
  const lp = studioUi.lastPlacedNote;
  if (lp) {
    const lpNote = getNoteAt(studioUi.song, studioUi.activeTrackIdx, lp.measureIdx, lp.beatIdx, lp.string);
    if (lpNote) {
      const midi = noteToMidi(track, lpNote);
      if (midi !== null) {
        return {
          measureIdx: lp.measureIdx,
          beatIdx: lp.beatIdx,
          string: lpNote.string,
          fret: lpNote.fret,
          midi
        };
      }
    }
  }
  return null;
}

function updateLiveFretboard() {
  const mount = document.getElementById('studioFretboardMount');
  if (!mount) return;
  const track = studioUi.song.tracks[studioUi.activeTrackIdx];
  if (!track) return;
  const mIdx = studioUi.lastPlayPos.active ? studioUi.lastPlayPos.measureIdx : studioUi.cursor.measureIdx;
  const bIdx = studioUi.lastPlayPos.active ? studioUi.lastPlayPos.beatIdx : studioUi.cursor.beatIdx;
  const { activeNotes, nextNotes, activeHits, nextHits } = computeActiveAndNextNotes(
    studioUi.song,
    studioUi.activeTrackIdx,
    mIdx,
    bIdx
  );
  const keyRootPc = studioUi.song.keyRootPc ?? 4;
  const keyScalePcs = getScaleHintPitchClasses(keyRootPc, studioUi.song.keyScaleId || 'natural_minor');
  const selCtx = resolveSelectedNoteContext();
  const curMaxFret = getStudioMaxFret();
  const cands = selCtx ? findFretCandidatesForMidi(track, selCtx.midi, 0, curMaxFret) : [];
  const fbZoom = Math.max(75, Math.min(150, Math.round(Number(studioUi.studioPrefs?.fretboardZoom) || 100)));
  mount.classList.toggle('zoomed', fbZoom > 100);
  mount.innerHTML = renderStudioFretboardSvg(track, {
    measure: track.measures?.[mIdx],
    measureIdx: mIdx,
    drumBrushLevel: studioUi.drumBrushLevel,
    playingStepIdx: -1,
    activeNotes,
    nextNotes,
    activeHits,
    nextHits,
    candidatePositions: cands,
    liveDetectedMidis: micCtrl.getLiveDetectedMidis(),
    positionAreaRange: studioUi.positionAreaRange,
    keyRootPc,
    keyScalePcs,
    showScaleHints: studioUi.showScaleHints,
    showOctaveBands: Boolean(studioUi.studioPrefs?.showOctaveBands),
    octaveAnchorPc: resolveStudioOctaveAnchorPc(keyRootPc, keyScalePcs),
    accMode: getAccMode(),
    fretCount: curMaxFret,
    fretboardZoom: fbZoom
  });
}

function updatePlayheadOnly(posInfo) {
  const svg = document.getElementById('studioTabSvg');
  const phLine = document.getElementById('studioPlayheadLine');
  const active = posInfo.phase === 'playing';
  studioUi.lastPlayPos = {
    active,
    measureIdx: posInfo.measureIdx ?? 0,
    beatIdx: posInfo.beatIdx ?? 0,
    progress: posInfo.progress ?? 0,
    beatProgress: posInfo.beatProgress ?? 0,
    metroBeatCol: active ? (posInfo.metroBeatCol ?? -1) : -1
  };

  if (studioUi.metroPanelOpen) {
    const bCol = active ? (posInfo.metroBeatCol ?? -1) : -1;
    if (bCol !== studioUi.lastMetroBeatCol) {
      studioUi.lastMetroBeatCol = bCol;
      document.querySelectorAll('.studio-metro-col').forEach((el, idx) => {
        el.classList.toggle('playing-col', active && idx === bCol);
      });
    }
  }

  const seqRoot = document.querySelector('.studio-drum-seq');
  if (seqRoot) {
    const stepCount = seqRoot.querySelectorAll('.studio-drum-seq-colnum').length || 8;
    const liveStep = active ? Math.max(0, Math.min(stepCount - 1, Math.floor((posInfo.progress || 0) * stepCount))) : -1;
    seqRoot.querySelectorAll('.studio-drum-seq-colnum').forEach((el, idx) => {
      el.classList.toggle('playing-step', idx === liveStep);
    });
    seqRoot.querySelectorAll('.studio-drum-seq-row').forEach(rowEl => {
      rowEl.querySelectorAll('.studio-drum-cell').forEach((cEl, idx) => {
        cEl.classList.toggle('playing-step', idx === liveStep);
      });
    });
  }

  const countInBadge = document.getElementById('studioCountInBadge');
  if (countInBadge) {
    if (posInfo.phase === 'countin') {
      countInBadge.textContent = `Odliczanie: ${posInfo.countInBeat} / ${posInfo.countInTotal}`;
      countInBadge.style.display = 'inline-flex';
    } else {
      countInBadge.style.display = 'none';
    }
  }

  if (svg && phLine) {
    if (!active) {
      phLine.setAttribute('opacity', '0');
    } else {
      const barGroup = svg.querySelector(`.studio-playhead-bounds[data-measure="${posInfo.measureIdx}"]`)
        || svg.querySelector(`.studio-bar[data-measure="${posInfo.measureIdx}"]`);
      if (barGroup) {
        const bx = Number(barGroup.getAttribute('data-x')) || 0;
        const bw = Number(barGroup.getAttribute('data-w')) || 80;
        const y1 = Number(barGroup.getAttribute('data-y1')) || 20;
        const y2 = Number(barGroup.getAttribute('data-y2')) || 120;
        const rawBeatsCx = barGroup.getAttribute('data-beats-cx') || '';
        const beatsCx = rawBeatsCx ? rawBeatsCx.split(',').map(Number).filter(Number.isFinite) : [];
        const innerRightAttr = barGroup.getAttribute('data-inner-right');
        const innerRight = innerRightAttr !== null ? Number(innerRightAttr) : (bx + bw - 8);
        const px = computePlayheadXForMeasure({
          beatsCx,
          innerRight,
          barX: bx,
          barW: bw,
          beatIdx: posInfo.beatIdx ?? 0,
          beatProgress: posInfo.beatProgress ?? 0,
          barProgress: posInfo.progress ?? 0
        });
        phLine.setAttribute('x1', px.toFixed(1));
        phLine.setAttribute('x2', px.toFixed(1));
        phLine.setAttribute('y1', y1.toFixed(1));
        phLine.setAttribute('y2', y2.toFixed(1));
        phLine.setAttribute('opacity', '0.95');
      }
    }
  }

  if (posInfo.noteChanged) {
    updateLiveFretboard();
    // Delikatne podświetlenie aktywnej kolumny w matrycy bez pełnego przerysowywania DOM
    const matHeads = document.querySelectorAll('.studio-bar-col-head');
    matHeads.forEach(th => {
      const m = Number(th.getAttribute('data-measure'));
      th.classList.toggle('playing', active && m === posInfo.measureIdx);
    });
    const matBlocks = document.querySelectorAll('.studio-matrix-cell');
    matBlocks.forEach(td => {
      const m = Number(td.getAttribute('data-measure'));
      const blk = td.querySelector('.studio-cell-block');
      if (blk) blk.classList.toggle('playing-cell', active && m === posInfo.measureIdx);
    });
  }
}

const soundFontSynth = createSoundFontSynth({
  host: {
    ensureAudioCtx: () => getHost()?.ensureAudioCtx?.()
  }
});

const player = createStudioPlayer({
  host: {
    getState: () => getHost()?.getState?.(),
    ensureAudioCtx: () => getHost()?.ensureAudioCtx?.(),
    getMasterAudioOut: () => getHost()?.getMasterAudioOut?.(),
    pluckGuitarFrequency: (...args) => getHost()?.pluckGuitarFrequency?.(...args),
    playHarmonicBellFrequency: (...args) => getHost()?.playHarmonicBellFrequency?.(...args),
    playKeyboardPresetNote: (...args) => getHost()?.playKeyboardPresetNote?.(...args),
    playDrumHit: (...args) => getHost()?.playDrumHit?.(...args),
    playMetronomeTickAudio: (...args) => getHost()?.playMetronomeTickAudio?.(...args),
    stopAllPreviewVoices: () => getHost()?.stopAllPreviewVoices?.()
  },
  soundFontSynth,
  soundFontEnabled: false,
  onPositionChange: (info) => {
    updatePlayheadOnly(info);
    micCtrl.onPlaybackPhaseChange(info);
  },
  onStateChange: (snap) => {
    studioUi.speedPercent = snap.speedPercent;
    const playBtn = document.getElementById('studioPlayToggleBtn');
    if (playBtn) {
      playBtn.textContent = (snap.isPlaying && !snap.isPaused) ? '⏸ Pauza' : '▶ Start';
    }
    const bpmBadge = document.getElementById('studioEffectiveBpmLabel');
    if (bpmBadge) {
      bpmBadge.textContent = `${snap.effectiveBpm} BPM (${snap.speedPercent}%)`;
    }
    if (!snap.isPlaying) {
      studioUi.lastPlayPos.active = false;
      updateLiveFretboard();
    }
  },
  onAutoTempoBump: (ev) => {
    showToast(`⚡ Auto-Tempo: ${ev.newBpm} BPM (${ev.newSpeedPercent}%)`);
    const slider = document.getElementById('studioSpeedSlider');
    if (slider) slider.value = String(ev.newSpeedPercent);
    document.querySelectorAll('.studio-speed-pill').forEach(btn => {
      btn.classList.toggle('active', Number(btn.getAttribute('data-speed')) === ev.newSpeedPercent);
    });
  }
});

function renderInspectorHtml(ctx = {}) {
  const track = ctx.track || studioUi.song.tracks[studioUi.activeTrackIdx] || studioUi.song.tracks[0];
  if (!track) return '';
  const isDrum = track.instrumentType === 'drums' || Boolean(track.isPercussion);
  const isKeys = track.instrumentType === 'keys';
  const curM = ctx.curMeasure || track.measures?.[studioUi.cursor.measureIdx] || track.measures?.[0];
  const curBeat = ctx.curBeat ?? (curM?.beats?.[studioUi.cursor.beatIdx] || null);
  const curNote = ctx.curNote ?? getNoteAt(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, studioUi.cursor.beatIdx, studioUi.cursor.stringIdx);
  const isProUi = studioUi.studioPrefs?.uiMode === 'pro';
  const accMode = getAccMode();
  const rootOptions = NOTE_NAMES_SHARP.map((nm, pc) =>
    `<option value="${pc}" ${pc === studioUi.song.keyRootPc ? 'selected' : ''}>${escapeHtml(midiToNoteName(pc, accMode))}</option>`
  ).join('');
  const scaleOptions = SCALE_HINT_OPTIONS.map(sc =>
    `<option value="${escapeHtml(sc.id)}" ${sc.id === studioUi.song.keyScaleId ? 'selected' : ''}>${escapeHtml(sc.name)}</option>`
  ).join('');

  const topTabs = `<div class="studio-insp-tabs">
    <button type="button" class="studio-insp-tab-btn ${studioUi.rightPanelTab !== 'settings' ? 'active' : ''}" data-action="set-right-tab" data-tab="tools" data-tip-name="Panel narzędzi" data-tip-desc="Kafelki rytmu, artykulacji, technik, struktury i schowka.">🎛 Narzędzia</button>
    <button type="button" class="studio-insp-tab-btn ${studioUi.rightPanelTab === 'settings' ? 'active' : ''}" data-action="set-right-tab" data-tab="settings" data-tip-name="Ścieżki i ustawienia" data-tip-desc="Lista ścieżek zespołu (Solo/Mute/głośność/dodawanie), brzmienie, strój, kapodaster i tonacja.">⚙️ Ścieżki</button>
    <button type="button" class="studio-btn sm" data-action="open-cmdk" data-tip-name="Szybka wyszukiwarka komend" data-tip-desc="Znajdź i uruchom dowolną akcję lub skrót Studia TAB." data-tip-kbd="Ctrl+K">🔍</button>
  </div>`;

  if (studioUi.rightPanelTab !== 'settings') {
    const railPalette = renderStudioPaletteHtml({
      uiMode: isProUi ? 'pro' : 'simple',
      track,
      beat: curBeat,
      note: curNote,
      measure: curM,
      cursor: studioUi.cursor,
      activeDuration: studioUi.activeDuration,
      clipboard: studioUi.clipboard,
      clipboardScope: studioUi.clipboardScope,
      clipboardPasteMode: studioUi.clipboardPasteMode,
      selectionStart: studioUi.selectionStart,
      selectionEnd: studioUi.selectionEnd,
      beatSelection: ctx.beatSelection || null,
      includeInlinePrompt: false
    });
    return `<div class="studio-insp-wrap">${topTabs}${railPalette}</div>`;
  }

  let instrumentCard = '';
  if (isDrum) {
    const curBlock = curM?.drumBlockId || 'rock_8th';
    instrumentCard = `<div class="studio-insp-card">
      <div class="section-label" style="margin:0;">ŚCIEŻKA PERKUSJI</div>
      <div class="studio-insp-row">
        <span>Klocek (takt ${studioUi.cursor.measureIdx + 1})</span>
        <select class="studio-insp-input" style="width:auto;" data-action="insp-drum-block">
          ${DRUM_BLOCKS.map(b => `<option value="${escapeHtml(b.id)}" ${b.id === curBlock ? 'selected' : ''}>${escapeHtml(b.name)}</option>`).join('')}
        </select>
      </div>
      <div class="studio-insp-row">
        <span>Auto-Fill</span>
        <select class="studio-insp-input" style="width:auto;" data-action="insp-drum-autofill">
          <option value="0" ${!track.drumAutoFill ? 'selected' : ''}>Wyłączony</option>
          <option value="4" ${track.drumAutoFill === 4 ? 'selected' : ''}>Co 4 takty</option>
          <option value="8" ${track.drumAutoFill === 8 ? 'selected' : ''}>Co 8 taktów</option>
        </select>
      </div>
      <label class="studio-insp-row" style="cursor:pointer;">
        <span>Crash na „1” w 1. takcie</span>
        <input type="checkbox" data-action="insp-drum-crash1" ${track.drumCrashOnOne ? 'checked' : ''}>
      </label>
    </div>`;
  } else {
    const toneList = isKeys ? KEYBOARD_TONE_OPTIONS : GUITAR_TONE_OPTIONS;
    const tuningList = track.instrumentType === 'bass' ? STUDIO_TUNING_PRESETS.bass : STUDIO_TUNING_PRESETS.guitar;
    instrumentCard = `<div class="studio-insp-card">
      <div class="section-label" style="margin:0;">AKTYWNA ŚCIEŻKA</div>
      <input type="text" class="studio-insp-input" data-action="insp-track-name" value="${escapeHtml(track.name)}" placeholder="Nazwa ścieżki">
      <div class="studio-insp-row">
        <span>Brzmienie</span>
        <select class="studio-insp-input" style="width:auto;" data-action="insp-track-tone">
          ${toneList.map(o => `<option value="${escapeHtml(o.id)}" ${o.id === track.tonePreset ? 'selected' : ''}>${escapeHtml(o.label)}</option>`).join('')}
        </select>
      </div>
      ${!isKeys ? `<div class="studio-insp-row">
        <span>Strój</span>
        <select class="studio-insp-input" style="width:auto;max-width:165px;" data-action="insp-track-tuning">
          ${tuningList.map(t => `<option value="${escapeHtml(t.id)}" ${t.id === track.tuningId ? 'selected' : ''}>${escapeHtml(t.name)}</option>`).join('')}
        </select>
      </div>
      <div class="studio-insp-row">
        <span>Kapodaster (próg)</span>
        <input type="number" class="studio-insp-input" style="width:62px;" min="0" max="12" value="${track.capo || 0}" data-action="insp-track-capo">
      </div>` : ''}
    </div>`;
  }

  const tracks = studioUi.song.tracks || [];
  const numMeasures = Math.max(1, getMasterMeasureCount(studioUi.song));
  const existingRoles = new Set(tracks.map(t => t.role));
  const addTrackBtns = Object.values(TRACK_ROLE_PRESETS)
    .filter(p => !existingRoles.has(p.role))
    .map(p => `<button type="button" class="studio-btn sm" data-action="add-track" data-role="${escapeHtml(p.role)}" ${tracks.length >= MAX_STUDIO_TRACKS ? 'disabled' : ''}>+ ${escapeHtml(p.name.split(' (')[0])}</button>`)
    .join('');

  const mixerRows = tracks.map((tr, tIdx) => {
    const isAct = tIdx === studioUi.activeTrackIdx;
    const isD = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
    const isK = tr.instrumentType === 'keys';
    const subMeta = isD ? 'Perkusja' : (isK ? 'Klawisze' : `${tr.openStringMidis?.length || tr.stringCount || 6} str.`);
    return `<div class="studio-insp-mixer-row ${isAct ? 'active' : ''}">
      <div style="display:flex;align-items:center;justify-content:space-between;gap:6px;">
        <button type="button" class="studio-track-name-btn" data-action="select-track" data-track="${tIdx}" style="flex:1;min-width:0;justify-content:flex-start;">
          <span class="studio-track-dot" style="background:${escapeHtml(tr.color || '#f39237')};"></span>
          <span style="overflow:hidden;text-overflow:ellipsis;white-space:nowrap;">${escapeHtml(tr.name)}</span>
        </button>
        <div style="display:flex;align-items:center;gap:3px;flex-shrink:0;">
          <button type="button" class="studio-sm-btn solo ${tr.solo ? 'on' : ''}" data-action="toggle-solo" data-track="${tIdx}" title="Solo">S</button>
          <button type="button" class="studio-sm-btn mute ${tr.muted ? 'on' : ''}" data-action="toggle-mute" data-track="${tIdx}" title="Wycisz">M</button>
          ${tracks.length > 1 ? `<button type="button" class="studio-sm-btn" data-action="remove-track" data-track="${tIdx}" title="Usuń ścieżkę">✕</button>` : ''}
        </div>
      </div>
      <div style="display:flex;align-items:center;justify-content:space-between;gap:6px;margin-top:4px;">
        <span style="font-size:10.5px;color:var(--text-muted);">${escapeHtml(subMeta)} · ${escapeHtml(tr.tonePreset || 'clean')}</span>
        <input type="range" min="0" max="100" step="5" value="${Math.round((tr.volume ?? 0.85) * 100)}"
               data-action="track-volume" data-track="${tIdx}" style="width:74px;" title="Głośność ścieżki">
      </div>
    </div>`;
  }).join('');

  const teamMixerCard = `<div class="studio-insp-card">
    <div style="display:flex;align-items:center;justify-content:space-between;gap:6px;">
      <div class="section-label" style="margin:0;">ŚCIEŻKI ZESPOŁU</div>
      <span style="font-size:10.5px;color:var(--text-muted);">${tracks.length}/${MAX_STUDIO_TRACKS} · ${numMeasures} t.</span>
    </div>
    <div style="display:flex;flex-direction:column;gap:5px;">${mixerRows}</div>
    <div style="display:flex;flex-wrap:wrap;gap:4px;margin-top:4px;">
      ${addTrackBtns}
      ${tracks.length < MAX_STUDIO_TRACKS ? `<button type="button" class="studio-btn sm" data-action="duplicate-track">📋 Powiel</button>` : ''}
      <button type="button" class="studio-btn sm" data-action="add-measure">+ Takt</button>
    </div>
  </div>`;

  const sfSt = soundFontSynth.getStatus();
  const sfOn = studioUi.studioPrefs.soundMode === 'sf2' && sfSt.loaded;
  return `<div class="studio-insp-wrap">
    ${topTabs}
    ${teamMixerCard}
    ${instrumentCard}
    <div class="studio-insp-card">
      <div class="section-label" style="margin:0;">KOMPOZYCJA I SKALA GRYFU</div>
      <input type="text" class="studio-insp-input" data-action="insp-song-title" value="${escapeHtml(studioUi.song.title)}" placeholder="Tytuł utworu">
      <input type="text" class="studio-insp-input" data-action="insp-song-artist" value="${escapeHtml(studioUi.song.artist)}" placeholder="Artysta">
      <div class="studio-insp-row">
        <span>Tempo bazowe</span>
        <input type="number" class="studio-insp-input" style="width:72px;" min="30" max="280" value="${studioUi.song.tempoBpm}" data-action="insp-song-bpm">
      </div>
      <div class="studio-insp-row">
        <span>Tonacja skali</span>
        <select class="studio-insp-input" style="width:auto;" data-action="insp-key-root">${rootOptions}</select>
      </div>
      <div class="studio-insp-row">
        <span>Skala podpowiedzi</span>
        <select class="studio-insp-input" style="width:auto;max-width:165px;" data-action="insp-key-scale">${scaleOptions}</select>
      </div>
      <div class="studio-insp-row">
        <span>Silnik audio</span>
        <div style="display:flex;align-items:center;gap:4px;">
          <button type="button" class="studio-btn sm" data-action="import-sf2-trigger" title="Wczytaj bank .sf2">🎹 ${sfSt.loaded ? escapeHtml(sfSt.name.slice(0, 10)) : '.SF2'}</button>
          <div class="studio-engine-toggle">
            <button type="button" class="${!sfOn ? 'active' : ''}" data-action="set-sound-mode" data-mode="webaudio">WebAudio</button>
            <button type="button" class="${sfOn ? 'active' : ''}" data-action="set-sound-mode" data-mode="sf2" ${sfSt.loaded ? '' : 'disabled'}>SF2</button>
          </div>
        </div>
      </div>
      <div style="display:flex;flex-wrap:wrap;gap:5px;margin-top:4px;padding-top:6px;border-top:1px dashed var(--border);">
        <button type="button" class="studio-btn sm primary" data-action="save-project-local">💾 Zapisz</button>
        <button type="button" class="studio-btn sm" data-action="save-project-as-new">📑 Jako nowy</button>
        ${studioUi.savedProjectId ? `<button type="button" class="studio-btn sm danger" data-action="delete-project-local">🗑️ Usuń</button>` : ''}
      </div>
    </div>
  </div>`;
}

function renderAllStudio() {
  const stageEl = studioUi.lastStageEl;
  const inspEl = studioUi.lastInspEl;
  if (!stageEl) return;

  hideHoverTip();
  clampCursor();
  const track = studioUi.song.tracks[studioUi.activeTrackIdx] || studioUi.song.tracks[0];
  const curMeasure = track?.measures?.[studioUi.cursor.measureIdx];
  const meterInfo = (track && track.instrumentType !== 'drums' && curMeasure)
    ? validateMeasureMeter(curMeasure)
    : null;
  const snap = player.getSnapshot();
  const playingNow = snap.isPlaying && !snap.isPaused;
  const containerW = Math.max(580, stageEl.clientWidth ? stageEl.clientWidth - 46 : 960);

  const curTimeSig = curMeasure ? `${curMeasure.numerator}/${curMeasure.denominator}` : '4/4';
  const timeSigOptions = TIME_SIGNATURE_PRESETS.map(ts =>
    `<option value="${ts.label}" ${ts.label === curTimeSig ? 'selected' : ''}>${ts.label}</option>`
  ).join('');

  const { activeNotes, nextNotes, activeHits, nextHits } = computeActiveAndNextNotes(
    studioUi.song,
    studioUi.activeTrackIdx,
    studioUi.lastPlayPos.active ? studioUi.lastPlayPos.measureIdx : studioUi.cursor.measureIdx,
    studioUi.lastPlayPos.active ? studioUi.lastPlayPos.beatIdx : studioUi.cursor.beatIdx
  );
  const keyRootPc = studioUi.song.keyRootPc ?? 4;
  const keyScalePcs = getScaleHintPitchClasses(keyRootPc, studioUi.song.keyScaleId || 'natural_minor');

  const activeSel = normalizeBeatSelection(studioUi.song, studioUi);
  const showAutoChords = studioUi.studioPrefs?.showAutoChords !== false;
  const isProUi = studioUi.studioPrefs?.uiMode === 'pro';
  const curMaxFret = getStudioMaxFret();
  const tabZoom = Math.max(70, Math.min(160, Math.round(Number(studioUi.studioPrefs?.tabZoom) || 100)));
  const fbZoom = Math.max(75, Math.min(150, Math.round(Number(studioUi.studioPrefs?.fretboardZoom) || 100)));
  const tabSvg = renderStudioTabSvg(studioUi.song, {
    activeTrackIdx: studioUi.activeTrackIdx,
    cursor: studioUi.cursor,
    playhead: studioUi.lastPlayPos,
    viewMode: studioUi.viewMode,
    uiMode: isProUi ? 'pro' : 'simple',
    allTracks: studioUi.scoreAllTracks,
    showNoteNames: studioUi.showScoreNoteNames,
    showAutoChords,
    barsPerRow: studioUi.barsPerRow,
    containerWidth: containerW,
    tabZoom,
    loopEnabled: studioUi.loopEnabled,
    loopStart: studioUi.loopStart,
    loopEnd: studioUi.loopEnd,
    selectionStart: studioUi.selectionStart,
    selectionEnd: studioUi.selectionEnd,
    beatSelection: studioUi.beatSelection,
    accMode: getAccMode()
  });

  const curBeat = curMeasure?.beats?.[studioUi.cursor.beatIdx] || null;
  const curNote = getNoteAt(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, studioUi.cursor.beatIdx, studioUi.cursor.stringIdx);
  const inlinePromptHtml = renderInlinePromptBar(studioUi.inlinePrompt);

  const cmdkHtml = renderCommandPaletteHtml({
    ...studioUi.cmdPalette,
    results: studioUi.cmdPalette.open ? filterStudioCommands(studioUi.cmdPalette.query) : []
  });

  const selCtx = resolveSelectedNoteContext();
  const cands = selCtx ? findFretCandidatesForMidi(track, selCtx.midi, 0, curMaxFret) : [];
  const fbContent = renderStudioFretboardSvg(track, {
    measure: curMeasure,
    measureIdx: studioUi.cursor.measureIdx,
    drumBrushLevel: studioUi.drumBrushLevel,
    playingStepIdx: -1,
    activeNotes,
    nextNotes,
    activeHits,
    nextHits,
    candidatePositions: cands,
    liveDetectedMidis: micCtrl.getLiveDetectedMidis(),
    positionAreaRange: studioUi.positionAreaRange,
    keyRootPc,
    keyScalePcs,
    showScaleHints: studioUi.showScaleHints,
    showOctaveBands: Boolean(studioUi.studioPrefs?.showOctaveBands),
    octaveAnchorPc: resolveStudioOctaveAnchorPc(keyRootPc, keyScalePcs),
    accMode: getAccMode(),
    fretCount: curMaxFret,
    fretboardZoom: fbZoom
  });
  const metroPanelHtml = studioUi.metroPanelOpen
    ? renderMetronomeBlocksHtml(studioUi.song.metronomePattern, curMeasure?.numerator || 4, {
        playingBeatCol: studioUi.lastPlayPos.active ? (studioUi.lastPlayPos.metroBeatCol ?? -1) : -1
      })
    : '';

  const micMode = micCtrl.getMode();
  const sensOffset = (studioUi.studioPrefs.detectSensitivityOffset !== undefined && studioUi.studioPrefs.detectSensitivityOffset !== null)
    ? Math.max(-5, Math.min(5, Math.round(Number(studioUi.studioPrefs.detectSensitivityOffset) || 0)))
    : thresholdToSensitivityOffset(studioUi.studioPrefs.detectSensitivity);
  const sensLabel = formatSensitivityOffsetLabel(sensOffset);
  const minDurMs = studioUi.studioPrefs.minNoteDurationMs || 80;
  const recMode = studioUi.studioPrefs.liveRecMode || 'offline';
  const gridVal = studioUi.studioPrefs.quantizeGrid ?? 1;
  const sfStatus = soundFontSynth.getStatus();
  const sfEnabled = studioUi.studioPrefs.soundMode === 'sf2' && sfStatus.loaded;
  const paStats = micCtrl.getPlayAlongStats();

  let selPosPanelHtml = '';
  studioUi.selPosAnalysis = null;
  if (activeSel && track && track.instrumentType !== 'drums' && !track.isPercussion) {
    const selAnalysis = analyzeSelectionPositionsAndOctaves(studioUi.song, studioUi.activeTrackIdx, activeSel, { maxFret: curMaxFret });
    studioUi.selPosAnalysis = selAnalysis;
    if (selAnalysis.noteCount > 0) {
      const sameHtml = track.instrumentType === 'keys'
        ? `<span class="studio-mic-hint">(klawisze — bez podziału na progi)</span>`
        : (selAnalysis.sameOctave.length > 0
            ? selAnalysis.sameOctave.map((pos, idx) =>
                `<button type="button" class="studio-cand-pill ${pos.isCurrent ? 'active' : ''}" data-action="apply-sel-pos" data-pos-kind="same" data-pos-idx="${idx}" data-pos-min="${pos.minFret}" data-pos-max="${pos.maxFret}" data-tip-name="${escapeHtml(pos.label)}" data-tip-desc="Przenosi zaznaczony fragment w tej samej oktawie w bezkolizyjny układ ${escapeHtml(pos.shortRange)}.">${escapeHtml(pos.shortRange)}${pos.isCurrent ? ' ✓' : ''}</button>`
              ).join('')
            : `<span class="studio-mic-hint">Brak innych wariantów w tej oktawie na gryfie 0–${curMaxFret}.</span>`);

      const octCount = selAnalysis.octaveOptions.length;
      const octHtml = selAnalysis.allOctavePositions.length > 0
        ? selAnalysis.allOctavePositions.map((pos, idx) =>
            `<button type="button" class="studio-cand-pill octave" data-action="apply-sel-pos" data-pos-kind="octave" data-pos-idx="${idx}" data-pos-min="${pos.minFret}" data-pos-max="${pos.maxFret}" data-tip-name="${escapeHtml(pos.label)}" data-tip-desc="Przenosi całe zaznaczenie o ${pos.octaveShift > 0 ? `+${pos.octaveShift}` : pos.octaveShift} okt. (${pos.semitones > 0 ? `+${pos.semitones}` : pos.semitones} półt.) w układ ${escapeHtml(pos.shortRange)} bez nakładania nut na struny.">${escapeHtml(pos.octaveShift > 0 ? `+${pos.octaveShift}` : pos.octaveShift)} okt.${pos.maxFret > 0 || pos.minFret > 0 ? ` · ${escapeHtml(pos.shortRange)}` : ''}</button>`
          ).join('')
        : `<span class="studio-mic-hint">Brak innych oktaw mieszczących się w pełni na gryfie 0–${curMaxFret} przy tym stroju.</span>`;

      selPosPanelHtml = `<div class="studio-sel-pos-panel">
        <div class="studio-sel-pos-row">
          <span class="studio-sel-pos-title">📍 Ta sama oktawa (gryf 0–${curMaxFret}):</span>
          ${sameHtml}
        </div>
        <div class="studio-sel-pos-row">
          <span class="studio-sel-pos-title">⇅ Inne oktawy (wykryto ${octCount} ${octCount === 1 ? 'oktawę' : (octCount >= 2 && octCount <= 4 ? 'oktawy' : 'oktaw')}):</span>
          ${octHtml}
        </div>
      </div>`;
    }
  }

  const phrasePositions = (!activeSel && track && track.instrumentType !== 'drums')
    ? (studioUi.phrasePositions.length > 0 ? studioUi.phrasePositions : analyzePhrasePositions(track, null, { boxSpan: 4, maxOptions: 5, maxFret: curMaxFret }))
    : [];
  studioUi.phrasePositions = phrasePositions;

  const candPopoverHtml = (!activeSel && selCtx && cands.length > 0)
    ? `<div style="display:flex;align-items:center;gap:5px;flex-wrap:wrap;">
         <span class="studio-mic-hint" style="font-style:normal;font-weight:800;color:#fbbf24;">Pozycja (${escapeHtml(midiToNoteName(selCtx.midi, getAccMode()))}):</span>
         ${cands.map(c => {
           const active = c.string === selCtx.string && c.fret === selCtx.fret;
           const nStr = (track.openStringMidis?.length || 6) - c.string;
           return `<button type="button" class="studio-cand-pill ${active ? 'active' : ''}" data-action="pick-note-cand" data-cand-string="${c.string}" data-cand-fret="${c.fret}" data-tip-name="Pozycja S${nStr}:P${c.fret}" data-tip-desc="Przenosi wybraną nutę na strunę ${nStr}, próg ${c.fret} bez zmiany wysokości dźwięku.">S${nStr}:P${c.fret}</button>`;
         }).join('')}
       </div>`
    : '';

  const phrasePosHtml = (!activeSel && phrasePositions.length > 0)
    ? `<div style="display:flex;align-items:center;gap:5px;flex-wrap:wrap;margin-left:auto;">
         <span class="studio-mic-hint" style="font-style:normal;font-weight:700;">Układ fraz:</span>
         ${phrasePositions.map((pos, idx) =>
           `<button type="button" class="studio-cand-pill ${studioUi.selectedPhrasePosIdx === idx ? 'active' : ''}" data-action="pick-phrase-pos" data-pos-idx="${idx}" data-pos-min="${pos.minFret}" data-pos-max="${pos.maxFret}" data-tip-name="${escapeHtml(pos.label)}" data-tip-desc="Przepisuje nagraną lub edytowaną frazę w zakres progów ${pos.minFret}–${pos.maxFret} z zachowaniem wysokości MIDI.">${escapeHtml(pos.label)}</button>`
         ).join('')}
       </div>`
    : '';

  const micPanelHtml = (track && track.instrumentType !== 'drums')
    ? `<div class="studio-mic-bar">
         <div class="studio-mic-row">
           <button type="button" class="studio-btn sm ${micMode === 'step' ? 'active' : ''}" data-action="mic-mode" data-mic-mode="step" data-tip-name="Wprowadzanie krokowe (Step-Input)" data-tip-desc="Każda wykryta z mikrofonu nuta lub akord trafia automatycznie pod kursor i przesuwa go dalej.">🎙 Step-Input</button>
           <button type="button" class="studio-btn sm ${micMode === 'rec' ? 'rec-active' : ''}" data-action="mic-mode" data-mic-mode="rec" data-tip-name="Nagraj do metronomu" data-tip-desc="Odtwarza metronom/podkład i zapisuje zagraną partię do TAB (tryb Offline lub Live).">${micMode === 'rec' ? '⏹ Stop + Rozpoznaj TAB' : '● Nagraj do metronomu'}</button>
           <button type="button" class="studio-btn sm ${micMode === 'playalong' ? 'active' : ''}" data-action="mic-mode" data-mic-mode="playalong" data-tip-name="Graj razem (Play-Along)" data-tip-desc="Wycisza aktywną ścieżkę i zlicza trafienia granych nut względem partytury.">🎧 Graj razem</button>
           <button type="button" class="studio-btn sm" data-action="insert-live-chord" data-tip-name="Wstaw akord z gryfu" data-tip-desc="Wpisuje aktualnie słyszane przez mikrofon lub podświetlone dźwięki jako akord w bieżącym slocie.">➕ Wstaw akord</button>
           ${micMode === 'playalong' ? `<span id="studioPlayAlongStats" class="studio-playalong-badge">Trafienia: <strong>${paStats.hits}/${paStats.total} (${paStats.percent}%)</strong> · ${escapeHtml(paStats.lastFeedback)}</span>` : ''}
           <div style="margin-left:auto;display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
             <div class="studio-mic-slider-wrap" data-tip-name="Wrażliwość wykrywania nut" data-tip-desc="Sprawdzony próg domyślny (0) + szybka korekta od -5 do +5 tylko dla Studia TAB. Plus = wyższa czułość (cichsze nuty), minus = mniejsza czułość (mniej szumu).">
               <span>Wrażliwość</span>
               <button type="button" class="studio-zoom-btn" data-action="studio-sens-step" data-delta="-1" ${sensOffset <= -5 ? 'disabled' : ''} title="Mniejsza czułość">−</button>
               <input id="studioSensitivitySlider" type="range" min="-5" max="5" step="1" value="${sensOffset}" data-action="studio-sensitivity" style="width:74px;">
               <button type="button" class="studio-zoom-btn" data-action="studio-sens-step" data-delta="1" ${sensOffset >= 5 ? 'disabled' : ''} title="Większa czułość">+</button>
               <strong id="studioSensitivityVal" style="min-width:66px;text-align:center;">${escapeHtml(sensLabel)}</strong>
               ${sensOffset !== 0 ? `<button type="button" class="studio-zoom-btn" data-action="studio-sens-reset" title="Przywróć sprawdzoną wartość domyślną">↺</button>` : ''}
             </div>
             <label class="studio-mic-slider-wrap" data-tip-name="Minimalny czas nuty" data-tip-desc="Bramka czasu trwania odfiltrowująca krótkie trzaski i uderzenia krótsze niż próg.">
               <span>Min.</span>
               <input id="studioMinDurSlider" type="range" min="20" max="240" step="10" value="${minDurMs}" data-action="studio-mindur" style="width:62px;">
               <strong id="studioMinDurVal">${minDurMs} ms</strong>
             </label>
             <div class="studio-engine-toggle" data-tip-name="Tryb rozpoznawania nagrania" data-tip-desc="Offline analizuje pełne audio po Stop (najdokładniej); Live wpisuje nuty na bieżąco.">
               <button type="button" class="${recMode === 'offline' ? 'active' : ''}" data-action="set-rec-engine" data-engine="offline">Offline</button>
               <button type="button" class="${recMode === 'live' ? 'active' : ''}" data-action="set-rec-engine" data-engine="live">Live</button>
             </div>
             <select data-action="studio-grid" class="studio-pal-select" data-tip-name="Siatka kwantyzacji" data-tip-desc="Najmniejsza jednostka rytmiczna przy kwantyzacji nagrania mikrofonowego.">
               <option value="0" ${gridVal === 0 ? 'selected' : ''}>Siatka 1/4</option>
               <option value="1" ${gridVal === 1 ? 'selected' : ''}>Siatka 1/8</option>
               <option value="2" ${gridVal === 2 ? 'selected' : ''}>Siatka 1/16</option>
               <option value="3" ${gridVal === 3 ? 'selected' : ''}>Siatka 1/32</option>
             </select>
             <button type="button" class="studio-btn sm" data-action="mic-recompute" ${micCtrl.hasLastRecording() ? '' : 'disabled'} data-tip-name="Przelicz nagranie" data-tip-desc="Ponownie przelicza ostatnie nagranie Offline z aktualną czułością, bramką ms i siatką bez ponownego grania.">↻ Przelicz</button>
           </div>
         </div>
       </div>
       ${(candPopoverHtml || phrasePosHtml) ? `<div class="studio-pos-bar">${candPopoverHtml}${phrasePosHtml}</div>` : ''}`
    : '';

  const tracks = studioUi.song.tracks || [];
  const numMeasures = Math.max(1, getMasterMeasureCount(studioUi.song));
  const trackPillsHtml = `<div class="studio-track-switcher">
    ${tracks.map((tr, tIdx) => {
      const isAct = tIdx === studioUi.activeTrackIdx;
      return `<button type="button" class="studio-track-pill ${isAct ? 'active' : ''}" data-action="select-track" data-track="${tIdx}" title="Przełącz aktywną ścieżkę na ${escapeHtml(tr.name)}">
        <span class="studio-track-dot" style="background:${escapeHtml(tr.color || '#f39237')};"></span>
        <span>${escapeHtml(tr.name)}</span>
        ${tr.solo ? '<span class="studio-track-flag s">S</span>' : ''}
        ${tr.muted ? '<span class="studio-track-flag m">M</span>' : ''}
      </button>`;
    }).join('')}
    <button type="button" class="studio-btn sm ${studioUi.scoreAllTracks ? 'active' : ''}" data-action="toggle-all-tracks-score" data-tip-name="Wszystkie ścieżki naraz" data-tip-desc="Wyświetla zsynchronizowane takty wszystkich ścieżek zespołu jedna pod drugą.">👥 Wszystkie</button>
    <button type="button" class="studio-btn sm" style="margin-left:auto;" data-action="set-right-tab" data-tab="settings" data-tip-name="Ustawienia ścieżek" data-tip-desc="Otwiera mikser Ścieżki zespołu (S/M/✕, głośność, dodawanie instrumentów) po prawej stronie.">⚙️ Ścieżki (${tracks.length})</button>
  </div>`;

  const matrixHtml = studioUi.arrangerOpen
    ? renderArrangerMatrixHtml(studioUi.song, {
        activeTrackIdx: studioUi.activeTrackIdx,
        cursor: studioUi.cursor,
        playhead: studioUi.lastPlayPos,
        loopEnabled: studioUi.loopEnabled,
        loopStart: studioUi.loopStart,
        loopEnd: studioUi.loopEnd,
        selectionStart: studioUi.selectionStart,
        selectionEnd: studioUi.selectionEnd
      })
    : '';

  const meterStatusHtml = meterInfo
    ? `<span class="studio-pill-badge" data-tip-name="Kontrola metrum taktu" data-tip-desc="Suma wartości rytmicznych w bieżącym takcie względem jego metrum." style="border-color:${meterInfo.status === 'ok' ? '#22c55e' : (meterInfo.status === 'under' ? '#eab308' : '#ef4444')};">
         Metrum T.${studioUi.cursor.measureIdx + 1}: <strong>${meterInfo.filled.toFixed(2).replace(/\.00$/, '')}/${meterInfo.capacity}</strong> ${meterInfo.status === 'ok' ? '✓' : (meterInfo.status === 'under' ? '(niepełny)' : '(nadmiar)')}
       </span>`
    : '';

  const isFretboardTrack = Boolean(track && track.instrumentType !== 'drums' && track.instrumentType !== 'keys');
  const showOctBands = Boolean(studioUi.studioPrefs?.showOctaveBands);
  const octAnchorPc = resolveStudioOctaveAnchorPc(keyRootPc, keyScalePcs);
  const octAnchorOptionsHtml = (isFretboardTrack && showOctBands)
    ? keyScalePcs.map(pc => {
        const semi = (((pc - keyRootPc) % 12) + 12) % 12;
        const nLbl = midiToNoteName(pc, getAccMode());
        return `<option value="${pc}" ${pc === octAnchorPc ? 'selected' : ''}>Od: ${escapeHtml(nLbl)} (${STUDIO_IV_LABELS[semi] || '1'})</option>`;
      }).join('')
    : '';
  const octaveControlsHtml = isFretboardTrack
    ? `<button type="button" class="studio-btn sm ${showOctBands ? 'active' : ''}" data-action="toggle-octave-bands" data-tip-name="Oznacz oktawy na gryfie" data-tip-desc="Dzieli gryf (gitara 6-str. i bas 4-str.) ukośnymi pasami oktaw i łączy dźwięk bazowy liniami unisono.">◈ Oznacz oktawy</button>${showOctBands ? `<select class="studio-pal-select" style="height:26px;font-size:11px;padding:2px 6px;" data-action="set-octave-anchor" data-tip-name="Dźwięk podziału oktaw" data-tip-desc="Wybierz dźwięk z aktywnej skali, względem którego dzielone są oktawy.">${octAnchorOptionsHtml}</select>` : ''}`
    : '';

  const savedProjects = getHost()?.getLibraryItems?.('studio') || [];
  const selectedProjVal = studioUi.savedProjectId
    ? `saved:${studioUi.savedProjectId}`
    : `demo:${studioUi.demoId || 'rock_riff_em'}`;
  const projectSelectHtml = `<select data-action="load-project" style="max-width:190px;" data-tip-name="Biblioteka projektów Studia TAB" data-tip-desc="Wybierz przykład demo lub jeden z zapisanych/wczytanych utworów lokalnych.">
    <optgroup label="Przykłady (Demo)">
      <option value="demo:rock_riff_em" ${selectedProjVal === 'demo:rock_riff_em' ? 'selected' : ''}>Demo: Neon Riff (Em)</option>
      <option value="demo:blues_shuffle_a" ${selectedProjVal === 'demo:blues_shuffle_a' ? 'selected' : ''}>Demo: Blues Shuffle (A)</option>
    </optgroup>
    ${savedProjects.length > 0 ? `<optgroup label="Moje i wczytane (${savedProjects.length})">
      ${savedProjects.map(pr => `<option value="saved:${escapeHtml(pr.id)}" ${selectedProjVal === `saved:${pr.id}` ? 'selected' : ''}>${pr.source === 'imported' ? '📥 ' : '💾 '}${escapeHtml(pr.name || pr.song?.title || 'Projekt')}</option>`).join('')}
    </optgroup>` : ''}
  </select>`;

  stageEl.innerHTML = `<div class="studio-shell">
    <!-- Strefa A: Zwarty 2-wierszowy pasek transportu, tempa i plików -->
    <div class="studio-zone-a">
      <div class="studio-zone-a-row">
        <div class="studio-group">
          <button id="studioPlayToggleBtn" type="button" class="studio-btn sm primary" data-action="play-toggle" data-tip-name="Odtwarzanie" data-tip-desc="Uruchamia lub zatrzymuje (pauza) odtwarzanie wszystkich słyszalnych ścieżek." data-tip-kbd="Spacja">${playingNow ? '⏸ Pauza' : '▶ Start'}</button>
          <button type="button" class="studio-btn sm" data-action="stop" data-tip-name="Stop" data-tip-desc="Zatrzymuje odtwarzanie i cofa kursor na początek pętli lub utworu.">⏹ Stop</button>
          <button type="button" class="studio-btn sm ${studioUi.countInEnabled ? 'active' : ''}" data-action="toggle-countin" data-tip-name="Count-In (1-2-3-4)" data-tip-desc="Odlicza jeden pełny takt metronomem przed rozpoczęciem odtwarzania lub nagrywania.">⏱ 1-2-3-4</button>
          <button type="button" class="studio-btn sm ${studioUi.metronomeEnabled ? 'active' : ''}" data-action="toggle-metronome" data-tip-name="Metronom" data-tip-desc="Włącza lub wyłącza klik metronomu podczas odtwarzania.">🔔 Metr.</button>
          <button type="button" class="studio-btn sm ${studioUi.metroPanelOpen ? 'active' : ''}" data-action="toggle-metro-panel" data-tip-name="Klockowy metronom akcentów (A / B / C)" data-tip-desc="Rozwija siatkę ustawiania akcentów A/B/C dla każdej miary taktu oraz podpodziałów ×1–×4.">▦</button>
          <span id="studioCountInBadge" class="studio-countin-badge" style="display:none;"></span>
        </div>

        ${isProUi ? `<div class="studio-group">
          <button type="button" class="studio-btn sm ${studioUi.loopEnabled ? 'active' : ''}" data-action="toggle-loop" data-tip-name="Zapętl odtwarzanie (Pętla A–B)" data-tip-desc="Powtarza w kółko fragment między flagą startu (🚩) i końca (🏁)." data-tip-kbd="F9">🔁 Pętla</button>
          <button type="button" class="studio-btn sm" data-action="set-loop-a" data-tip-name="Początek pętli" data-tip-desc="Ustawia start pętli (🚩) na bieżącym takcie." data-tip-kbd="[">🚩 T.${studioUi.loopStart + 1}</button>
          <button type="button" class="studio-btn sm" data-action="set-loop-b" data-tip-name="Koniec pętli" data-tip-desc="Ustawia koniec pętli (🏁) na bieżącym takcie." data-tip-kbd="]">🏁 T.${studioUi.loopEnd + 1}</button>
        </div>` : ''}

        <div class="studio-group" style="border-right:none;padding-right:0;">
          ${SPEED_PRESETS.map(pct =>
            `<button type="button" class="studio-btn sm studio-speed-pill ${studioUi.speedPercent === pct ? 'active' : ''}" data-action="set-speed" data-speed="${pct}" data-tip-name="Prędkość ${pct}%" data-tip-desc="Skaluje tempo ćwiczenia do ${pct}% wartości bazowej.">${pct}%</button>`
          ).join('')}
          <input id="studioSpeedSlider" type="range" min="25" max="150" step="5" value="${studioUi.speedPercent}" data-action="speed-slider" data-tip-name="Suwak tempa (25–150%)" data-tip-desc="Płynna zmiana prędkości odtwarzania bez zmiany wysokości dźwięku." style="width:72px;">
          <span id="studioEffectiveBpmLabel" class="studio-pill-badge" style="padding:3px 7px;font-size:11px;"><strong>${snap.effectiveBpm} BPM (${studioUi.speedPercent}%)</strong></span>
          ${isProUi ? `<button type="button" class="studio-btn sm ${studioUi.autoTempoEnabled ? 'active' : ''}" data-action="toggle-autotempo" data-tip-name="Auto-Tempo pętli" data-tip-desc="Automatycznie zwiększa tempo o +${studioUi.autoTempoStep} BPM po każdym przejściu pętli A–B.">⚡ Auto</button>` : ''}
        </div>
        <div class="studio-uimode-switch" style="margin-left:auto;" data-tip-name="Tryb interfejsu Studia TAB" data-tip-desc="Uproszczony: prosty liniowy układ taktów bez pętli i zbędnych oznaczeń. Pro: pełne repetycje, pętla A–B i narzędzia notacyjne.">
          <button type="button" class="${!isProUi ? 'active' : ''}" data-action="set-studio-ui-mode" data-mode="simple">Uproszczony</button>
          <button type="button" class="${isProUi ? 'active' : ''}" data-action="set-studio-ui-mode" data-mode="pro">Pro</button>
        </div>
        <span id="studioToastMsg" class="studio-toast" style="display:${studioUi.statusMsg ? 'inline-flex' : 'none'};">${escapeHtml(studioUi.statusMsg)}</span>
      </div>

      <div class="studio-zone-a-row">
        <div class="studio-group">
          <select data-action="set-timesig" data-tip-name="Metrum taktu" data-tip-desc="Ustawia metrum (np. 4/4, 3/4, 6/8) od bieżącego taktu.">${timeSigOptions}</select>
          ${isProUi ? `
            <button type="button" class="studio-btn sm ${studioUi.viewMode === 'tab' ? 'active' : ''}" data-action="set-view-mode" data-mode="tab" data-tip-name="Widok TAB" data-tip-desc="Wyświetla samą tabulaturę gitarową/basową.">🎸 TAB</button>
            <button type="button" class="studio-btn sm ${studioUi.viewMode === 'staff' ? 'active' : ''}" data-action="set-view-mode" data-mode="staff" data-tip-name="Widok Nuty (pięciolinia)" data-tip-desc="Przejrzysta pięciolinia bez TAB. Przy kilku instrumentach włącza widok całego zespołu naraz.">🎼 Nuty</button>
            <button type="button" class="studio-btn sm ${studioUi.viewMode === 'staff_tab' ? 'active' : ''}" data-action="set-view-mode" data-mode="staff_tab" data-tip-name="Widok Nuty + TAB" data-tip-desc="Wyświetla pięciolinię w kluczu wiolinowym/basowym nad tabulaturą.">🎼 Nuty+TAB</button>
            <button type="button" class="studio-btn sm ${studioUi.showScoreNoteNames ? 'active' : ''}" data-action="toggle-score-note-names" data-tip-name="Nazwy nut (A–G)" data-tip-desc="Pokazuje czytelne nazwy dźwięków nad pięciolinią.">ABC</button>
            <button type="button" class="studio-btn sm ${showAutoChords ? 'active' : ''}" data-action="toggle-auto-chords" data-tip-name="Auto-akordy nad TAB" data-tip-desc="Automatycznie rozpoznaje i pokazuje akordy (z ChordNamer, A–G) nad wielodźwiękami oraz arpeggiami.">🔍 Auto-akordy</button>
            <select data-action="set-bars-per-row" data-tip-name="Takty w wierszu" data-tip-desc="Liczba taktów przypadająca na jeden wiersz partytury.">
              <option value="auto" ${studioUi.barsPerRow === 'auto' ? 'selected' : ''}>Wiersz: Auto</option>
              <option value="2" ${String(studioUi.barsPerRow) === '2' ? 'selected' : ''}>2 takty</option>
              <option value="4" ${String(studioUi.barsPerRow) === '4' ? 'selected' : ''}>4 takty</option>
              <option value="6" ${String(studioUi.barsPerRow) === '6' ? 'selected' : ''}>6 taktów</option>
            </select>
            <button type="button" class="studio-btn sm" data-action="detect-key" data-tip-name="Wykryj tonację" data-tip-desc="Analizuje zapisane nuty algorytmem Krumhansla-Schmucklera i ustawia skalę gryfu.">🔍 Tonacja</button>
          ` : `
            <button type="button" class="studio-btn sm ${studioUi.viewMode === 'staff_tab' ? 'active' : ''}" data-action="toggle-simple-staff" data-tip-name="Przełącz TAB / Nuty+TAB" data-tip-desc="Szybkie przełączenie między samym TAB-em a widokiem Nuty+TAB.">${studioUi.viewMode === 'staff_tab' ? '🎼 Nuty+TAB' : '🎸 TAB'}</button>
            <button type="button" class="studio-btn sm ${showAutoChords ? 'active' : ''}" data-action="toggle-auto-chords" data-tip-name="Auto-akordy nad TAB" data-tip-desc="Automatycznie rozpoznaje i pokazuje akordy (z ChordNamer, A–G) nad wielodźwiękami oraz arpeggiami.">🔍 Auto-akordy</button>
            <button type="button" class="studio-btn sm" data-action="detect-key" data-tip-name="Wykryj tonację" data-tip-desc="Analizuje zapisane nuty algorytmem Krumhansla-Schmucklera i ustawia skalę gryfu.">🔍 Tonacja</button>
          `}
        </div>

        <div class="studio-group" style="border-right:none;padding-right:0;">
          ${projectSelectHtml}
          <button type="button" class="studio-btn sm" data-action="new-song" data-tip-name="Nowy projekt" data-tip-desc="Tworzy czysty projekt z jedną ścieżką gitary i perkusją.">📄 Nowy</button>
          <button type="button" class="studio-btn sm primary" data-action="save-project-local" data-tip-name="Zapisz w bibliotece lokalnej" data-tip-desc="Zapisuje ten utwór trwale w lokalnym folderze user_library/studio/.">💾 Zapisz</button>
          <button type="button" class="studio-btn sm" data-action="export-json" data-tip-name="Eksportuj plik .gitara-studio.json" data-tip-desc="Pobiera plik projektu gotowy do wysłania znajomym.">📤 Eksportuj</button>
          <button type="button" class="studio-btn sm" data-action="import-json-trigger" data-tip-name="Wczytaj plik na stałe" data-tip-desc="Wczytuje plik projektu (lub paczkę) i zapisuje go na stałe w lokalnej bibliotece.">📂 Wczytaj</button>
          <button type="button" class="studio-btn sm" data-action="export-gp5" data-tip-name="Eksportuj Guitar Pro 5" data-tip-desc="Generuje binarny plik .GP5 ze wszystkimi ścieżkami, technikami, markerami i perkusją GM.">⬇ .GP5</button>
          ${isProUi ? `
            <button type="button" class="studio-btn sm" data-action="export-mid" data-tip-name="Eksportuj Standard MIDI" data-tip-desc="Generuje wielościeżkowy plik .MID (Format 1) z tempem, tonacją i kanałem perkusji 10.">⬇ .MID</button>
            <button type="button" class="studio-btn sm" data-action="import-sf2-trigger" data-tip-name="Wczytaj SoundFont2 (.sf2)" data-tip-desc="Ładuje lokalny bank brzmień .sf2 (bank 0 + perkusja 128) do odtwarzania offline w WebAudio.">🎹 ${sfStatus.loaded ? escapeHtml(sfStatus.name.slice(0, 14)) : '.SF2'}</button>
            <div class="studio-engine-toggle" data-tip-name="Silnik dźwięku" data-tip-desc="Przełącza syntezę pomiędzy wbudowanym WebAudio a wczytanym bankiem SoundFont2 (.sf2).">
              <button type="button" class="${!sfEnabled ? 'active' : ''}" data-action="set-sound-mode" data-mode="webaudio">WebAudio</button>
              <button type="button" class="${sfEnabled ? 'active' : ''}" data-action="set-sound-mode" data-mode="sf2" ${sfStatus.loaded ? '' : 'disabled'}>.SF2</button>
            </div>
          ` : ''}
          <input id="studioJsonFileInput" type="file" accept=".json,.gitara.json,.gitara-studio.json,.gitara-pack.json,application/json" style="display:none;" data-action="import-json-file">
          <input id="studioSf2FileInput" type="file" accept=".sf2" style="display:none;" data-action="import-sf2-file">
          <button type="button" class="studio-btn sm" data-action="undo" ${studioUi.history.canUndo() ? '' : 'disabled'} data-tip-name="Cofnij" data-tip-desc="Cofa ostatnią operację edycji (historia 50 kroków)." data-tip-kbd="Ctrl+Z">↩</button>
          <button type="button" class="studio-btn sm" data-action="redo" ${studioUi.history.canRedo() ? '' : 'disabled'} data-tip-name="Ponów" data-tip-desc="Przywraca cofniętą zmianę." data-tip-kbd="Ctrl+Y">↪</button>
        </div>
      </div>
    </div>

    ${metroPanelHtml}
    ${inlinePromptHtml}

    <!-- Strefa C: Wielotaktowa partytura TAB -->
    <div class="studio-card">
      <div class="studio-zone-c-header">
        <div class="studio-track-badge">
          <span class="studio-track-dot" style="background:${escapeHtml(track?.color || '#f39237')};"></span>
          <span>${escapeHtml(studioUi.song.title)}</span>
          <span style="color:var(--text-muted);font-weight:500;">· ${escapeHtml(track?.name || '')}</span>
        </div>
        <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
          <div class="studio-zoom-pill" data-tip-name="Skalowanie tabulatury (Zoom)" data-tip-desc="Powiększa lub zmniejsza widok partytury i TAB-u (70%–160%).">
            <span>🔍 TAB</span>
            <button type="button" class="studio-zoom-btn" data-action="tab-zoom-step" data-delta="-10" ${tabZoom <= 70 ? 'disabled' : ''}>−</button>
            <button type="button" class="studio-zoom-val" data-action="tab-zoom-reset" title="Przywróć zoom TAB 100%">${tabZoom}%</button>
            <button type="button" class="studio-zoom-btn" data-action="tab-zoom-step" data-delta="10" ${tabZoom >= 160 ? 'disabled' : ''}>+</button>
          </div>
          ${activeSel ? `<span class="studio-sel-pill">Zaznaczenie: <strong>T.${activeSel.startMeasure + 1}·B.${activeSel.start.beatIdx + 1} – T.${activeSel.endMeasure + 1}·B.${activeSel.end.beatIdx + 1}</strong> (${activeSel.beatCount} ud.)<button type="button" class="studio-sel-clear" data-action="clear-selection" title="Wyczyść zaznaczenie (Esc)">✕</button></span>` : ''}
          ${meterStatusHtml}
          <span class="studio-pill-badge">Kursor: <strong>T.${studioUi.cursor.measureIdx + 1} · B.${studioUi.cursor.beatIdx + 1}</strong></span>
        </div>
      </div>
      ${trackPillsHtml}
      <div class="studio-tab-scroll ${studioUi.scoreAllTracks ? 'multitrack' : ''} ${tabZoom > 100 ? 'zoomed' : ''}" id="studioTabScrollWrap">${tabSvg}</div>
    </div>

    <!-- Strefa E: Animowany kolorowy gryf + mikrofon -->
    <div class="studio-card">
      <div class="studio-zone-e-head">
        <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
          <span class="section-label" style="margin:0;">GRYF (STREFA E)</span>
          <button type="button" class="studio-btn sm ${studioUi.fretInputMode === 'edit' ? 'active' : ''}" data-action="set-fret-mode" data-mode="edit" data-tip-name="Tryb gryfu: Wpisuj" data-tip-desc="Kliknięcie progu na gryfie wpisuje nutę w aktualnym slocie i ją odsłuchuje.">✏️ Wpisuj</button>
          <button type="button" class="studio-btn sm ${studioUi.fretInputMode === 'audition' ? 'active' : ''}" data-action="set-fret-mode" data-mode="audition" data-tip-name="Tryb gryfu: Tylko odsłuch" data-tip-desc="Kliknięcie progu jedynie odsłuchuje dźwięk bez modyfikacji partytury.">🔊 Odsłuch</button>
          <button type="button" class="studio-btn sm ${studioUi.showScaleHints ? 'active' : ''}" data-action="toggle-scale-hints" data-tip-name="Podpowiedzi skali na gryfie" data-tip-desc="Pokazuje na gryfie stopnie wybranej tonacji utworu.">Skala: ${studioUi.showScaleHints ? 'Wł.' : 'Wył.'}</button>
          ${octaveControlsHtml}
          <div class="studio-zoom-pill" data-tip-name="Skalowanie gryfu (Zoom)" data-tip-desc="Powiększa lub zmniejsza podstrunnicę w Strefie E (75%–150%).">
            <span>🔍 Gryf</span>
            <button type="button" class="studio-zoom-btn" data-action="fb-zoom-step" data-delta="-10" ${fbZoom <= 75 ? 'disabled' : ''}>−</button>
            <button type="button" class="studio-zoom-val" data-action="fb-zoom-reset" title="Przywróć zoom gryfu 100%">${fbZoom}%</button>
            <button type="button" class="studio-zoom-btn" data-action="fb-zoom-step" data-delta="10" ${fbZoom >= 150 ? 'disabled' : ''}>+</button>
          </div>
        </div>
        <div class="studio-legend">
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;background:#f39237;display:inline-block;"></span>Tonika</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;background:#22c55e;display:inline-block;"></span>Tercja/Mic</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;border:2px solid #f59e0b;display:inline-block;"></span>Kandydaci</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;border:2px dashed #38bdf8;display:inline-block;"></span>Następne</span>
        </div>
      </div>
      ${selPosPanelHtml}
      <div id="studioFretboardMount" class="studio-fretboard-wrap ${fbZoom > 100 ? 'zoomed' : ''}">${fbContent}</div>
      ${micPanelHtml}
    </div>

    <!-- Strefa D: Dolna matryca aranżacji Ścieżki × Takty (zwijana domyślnie) -->
    <div class="studio-card">
      <button type="button" class="studio-arranger-toggle" data-action="toggle-arranger">
        <span>📊 Matryca aranżacji (${tracks.length}/${MAX_STUDIO_TRACKS} ścieżek × ${numMeasures} taktów)</span>
        <span>${studioUi.arrangerOpen ? 'Zwiń ▴' : 'Rozwiń ▾'}</span>
      </button>
      ${matrixHtml}
    </div>
    ${cmdkHtml}
  </div>`;

  if (inspEl) {
    inspEl.innerHTML = renderInspectorHtml({ track, curMeasure, curBeat, curNote, beatSelection: activeSel });
  }
  if (studioUi.cmdPalette?.open) {
    const inp = document.getElementById('studioCmdkInput');
    if (inp) {
      inp.focus();
      const len = inp.value.length;
      inp.setSelectionRange?.(len, len);
    }
  } else if (studioUi.inlinePrompt) {
    const inp = document.getElementById('studioInlineInput');
    if (inp) {
      inp.focus();
      inp.select?.();
    }
  }
}

function pushUndoSnapshot() {
  studioUi.history.push(studioUi.song);
}

function reloadPlayerFromSong(preservePlayback = false) {
  player.loadSong(studioUi.song, studioUi.activeTrackIdx, { preservePlayback });
  if (!preservePlayback) {
    player.setSpeedPercent(studioUi.speedPercent);
    player.setCountIn(studioUi.countInEnabled);
    player.setMetronome(studioUi.metronomeEnabled);
    player.setLoop({ enabled: studioUi.loopEnabled, start: studioUi.loopStart, end: studioUi.loopEnd });
    player.setAutoTempo(studioUi.autoTempoEnabled, studioUi.autoTempoStep);
  }
}

function applyLoadedStudioItem(rawItem, { savedId = null, demoId = null } = {}) {
  const { song, studioPrefs, proBackup: loadedBackup } = deserializeStudioSong(rawItem);
  player.stop();
  pushUndoSnapshot();
  studioUi.proBackup = null;
  studioUi.selPosAnalysis = null;
  studioUi.savedPositionAreaRange = null;
  studioUi.activeDuration = null;
  studioUi.song = song;
  const keepUiMode = studioUi.studioPrefs?.uiMode || 'simple';
  studioUi.studioPrefs = clampStudioPrefs({ ...studioPrefs, uiMode: studioPrefs.uiMode || keepUiMode });
  studioUi.savedProjectId = savedId;
  if (demoId !== undefined && demoId !== null) studioUi.demoId = demoId;
  if (loadedBackup && loadedBackup.song) {
    if (studioUi.studioPrefs.uiMode === 'pro' && Array.isArray(loadedBackup.sourceMap) && loadedBackup.sourceMap.length > 0) {
      const rec = reconcileLinearEditsIntoProSong(loadedBackup.song, studioUi.song, loadedBackup.sourceMap, loadedBackup.baselineLinearSong);
      studioUi.song = rec.song;
    } else if (studioUi.studioPrefs.uiMode !== 'pro') {
      studioUi.proBackup = loadedBackup;
    }
  }
  if (studioUi.studioPrefs.uiMode !== 'pro') {
    enterSimpleModeProjection();
  }
  player.setSoundFontEnabled(studioUi.studioPrefs.soundMode === 'sf2' && soundFontSynth.isLoaded());
  persistPrefsToHost();
  studioUi.activeTrackIdx = 0;
  studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
  studioUi.beatSelection = null;
  studioUi.selectionStart = null;
  studioUi.selectionEnd = null;
  studioUi.lastPlacedNote = null;
  studioUi.positionAreaRange = null;
  studioUi.phrasePositions = [];
  studioUi.selectedPhrasePosIdx = -1;
  studioUi.loopEnd = Math.min(3, Math.max(0, getMasterMeasureCount(studioUi.song) - 1));
  reloadPlayerFromSong();
}

function saveCurrentStudioProject({ asNew = false } = {}) {
  const host = getHost();
  if (!host || typeof host.saveLibraryItem !== 'function') {
    triggerJsonDownload();
    return;
  }
  const title = (studioUi.song.title || 'Projekt Studio TAB').trim() || 'Projekt Studio TAB';
  const payload = {
    id: (!asNew && studioUi.savedProjectId) ? studioUi.savedProjectId : undefined,
    name: title,
    title,
    song: JSON.parse(JSON.stringify(studioUi.song)),
    studioPrefs: studioUi.studioPrefs,
    ...(studioUi.proBackup ? { proBackup: cloneSong(studioUi.proBackup) } : {})
  };
  Promise.resolve(host.saveLibraryItem('studio', payload)).then(saved => {
    if (saved && saved.id) {
      studioUi.savedProjectId = saved.id;
      showToast(`Zapisano lokalnie „${saved.name || title}”`);
      renderAllStudio();
    }
  }).catch(err => {
    showToast(err?.message || 'Nie udało się zapisać projektu lokalnie');
  });
}

function triggerJsonDownload() {
  const host = getHost();
  const safeTitle = (studioUi.song.title || 'projekt').toLowerCase().replace(/[^a-z0-9_-]+/gi, '_');
  if (host && typeof host.exportLibraryItem === 'function') {
    host.exportLibraryItem('studio', {
      id: studioUi.savedProjectId || undefined,
      name: studioUi.song.title || 'Projekt Studio TAB',
      song: studioUi.song,
      studioPrefs: studioUi.studioPrefs,
      ...(studioUi.proBackup ? { proBackup: cloneSong(studioUi.proBackup) } : {})
    }, `${safeTitle}.gitara-studio.json`);
    showToast('Wyeksportowano .gitara-studio.json');
    return;
  }
  const json = serializeStudioSong(studioUi.song, studioUi.studioPrefs, studioUi.proBackup ? { proBackup: studioUi.proBackup } : null);
  const blob = new Blob([json], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `${safeTitle}.gitara.json`;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
  showToast('Zapisano projekt .gitara.json');
}

async function triggerBinaryExport(endpoint, ext) {
  try {
    const payload = buildSongExportPayload(studioUi.song);
    if (!payload) return;
    showToast(`Generowanie pliku .${ext.toUpperCase()}...`);
    const resp = await fetch(endpoint, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(payload)
    });
    if (!resp.ok) {
      const msg = await resp.text().catch(() => '');
      throw new Error(msg || `Błąd serwera (${resp.status})`);
    }
    const blob = await resp.blob();
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    const safeTitle = (studioUi.song.title || 'projekt').toLowerCase().replace(/[^a-z0-9_-]+/gi, '_');
    a.href = url;
    a.download = `${safeTitle}.${ext}`;
    document.body.appendChild(a);
    a.click();
    a.remove();
    URL.revokeObjectURL(url);
    showToast(`Wyeksportowano ${safeTitle}.${ext}`);
  } catch (err) {
    showToast(err?.message || `Błąd eksportu .${ext}`);
  }
}

const micCtrl = createStudioMicController({
  getState: () => studioUi,
  pushUndo: () => pushUndoSnapshot(),
  getDetector: () => getHost()?.getMicDetector?.(),
  getMicStream: () => getHost()?.getMicStream?.(),
  getAudioCtx: () => getHost()?.ensureAudioCtx?.(),
  ensureMicActive: async () => {
    const h = getHost();
    if (h && typeof h.ensureMicActive === 'function') {
      return await h.ensureMicActive();
    }
    return Boolean(h?.getMicDetector?.());
  },
  onStartMetronomeRec: () => {
    studioUi.metronomeEnabled = true;
    player.setMetronome(true);
    player.setCountIn(studioUi.countInEnabled);
    player.play(studioUi.cursor.measureIdx);
  },
  onStopMetronome: () => {
    player.stop();
  },
  onStartPlayAlong: () => {
    player.play(studioUi.cursor.measureIdx);
  },
  onSongChanged: (preservePlayback) => reloadPlayerFromSong(preservePlayback),
  onLiveFretboardRefresh: () => updateLiveFretboard(),
  onLivePlayAlongRefresh: () => {
    const el = document.getElementById('studioPlayAlongStats');
    if (el) {
      const s = micCtrl.getPlayAlongStats();
      el.innerHTML = `Trafienia: <strong>${s.hits}/${s.total} (${s.percent}%)</strong> · ${escapeHtml(s.lastFeedback)}`;
    }
  },
  onToast: (msg) => showToast(msg),
  onRender: () => renderAllStudio()
});

function insertLiveChordFromMic() {
  const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
  if (!tr || tr.instrumentType === 'drums') return false;
  const hostSt = getHost()?.getState?.();
  const midisSet = new Set(micCtrl.getLiveDetectedMidis());
  if (Array.isArray(hostSt?.detectedMidis)) {
    for (const m of hostSt.detectedMidis) {
      if (Number.isFinite(m)) midisSet.add(Math.round(m));
    }
  }
  const midis = Array.from(midisSet).sort((a, b) => a - b);
  if (midis.length === 0) {
    showToast('Brak wykrytych dźwięków z mikrofonu lub gryfu.');
    return false;
  }
  const mIdx = studioUi.cursor.measureIdx;
  const bIdx = studioUi.cursor.beatIdx;
  const beat = tr.measures?.[mIdx]?.beats?.[bIdx];
  if (!beat) return false;
  pushUndoSnapshot();
  const used = new Set();
  const notes = [];
  for (const midi of midis) {
    const cands = findFretCandidatesForMidi(tr, midi, 0, 24);
    const pick = cands.find(c => !used.has(c.string)) || cands[0];
    if (pick && !used.has(pick.string)) {
      used.add(pick.string);
      notes.push(createTabNote({ string: pick.string, fret: pick.fret, velocity: 98 }));
    }
  }
  if (notes.length === 0) return false;
  notes.sort((a, b) => a.string - b.string);
  beat.isRest = false;
  beat.notes = notes;
  studioUi.lastPlacedNote = { measureIdx: mIdx, beatIdx: bIdx, string: notes[0].string, fret: notes[0].fret, midi: midis[0] };
  reloadPlayerFromSong(true);
  renderAllStudio();
  showToast(`Wstawiono akord (${notes.length} str.)`);
  return true;
}

const inputCtrl = createStudioInputController({
  getState: () => studioUi,
  getMaxFret: getStudioMaxFret,
  pushUndo: () => pushUndoSnapshot(),
  onSongChanged: (preservePlayback) => reloadPlayerFromSong(preservePlayback),
  onCursorMoved: () => {
    player.setActiveTrack(studioUi.activeTrackIdx);
    clampCursor();
  },
  onPlayToggle: () => player.togglePlay(),
  onStop: () => { player.stop(); renderAllStudio(); },
  onSeek: (mIdx, bIdx) => { clampCursor(); player.seekTo(mIdx, bIdx); },
  onSetLoop: ({ enabled, start, end }) => {
    studioUi.loopEnabled = enabled;
    studioUi.loopStart = start;
    studioUi.loopEnd = end;
    player.setLoop({ enabled, start, end });
    renderAllStudio();
  },
  onAuditionFret: (tr, s, f) => player.auditionFret(tr, s, f),
  onAuditionBeat: (tr, beat) => player.auditionBeat(tr, beat),
  onToast: (msg) => showToast(msg),
  onRender: () => renderAllStudio()
});

function resolveClickedStringFromEvent(beatHit, origEvent) {
  const bar = beatHit.closest('.studio-bar');
  const svg = beatHit.closest('svg');
  if (!bar || !svg || !origEvent || typeof svg.getScreenCTM !== 'function') return null;
  const nStrings = Number(bar.getAttribute('data-nstrings')) || 0;
  const tabTop = Number(bar.getAttribute('data-tab-top'));
  const tabGap = Number(bar.getAttribute('data-tab-gap')) || 16;
  if (nStrings <= 0 || !Number.isFinite(tabTop)) return null;
  const ctm = svg.getScreenCTM();
  if (!ctm || ctm.d === 0) return null;
  const svgY = (origEvent.clientY - ctm.f) / ctm.d;
  const row = Math.round((svgY - tabTop) / tabGap);
  if (row < 0 || row >= nStrings) return null;
  return nStrings - 1 - row;
}

function handleStageAction(target, eventType, origEvent = null) {
  // Obsługa kliknięcia bezpośrednio w uderzenie/takt na SVG TAB (z wyborem ścieżki, uderzenia i struny z Y)
  const beatHit = target.closest('.studio-beat-hit');
  if (beatHit && eventType === 'click') {
    if (suppressNextBeatClick) {
      suppressNextBeatClick = false;
      return;
    }
    const hitTrackAttr = beatHit.getAttribute('data-track');
    const tIdx = hitTrackAttr !== null ? (Number(hitTrackAttr) || 0) : studioUi.activeTrackIdx;
    const mIdx = Number(beatHit.getAttribute('data-measure')) || 0;
    const bIdx = Number(beatHit.getAttribute('data-beat')) || 0;
    const clickedStr = resolveClickedStringFromEvent(beatHit, origEvent);
    studioUi.beatSelection = null;
    studioUi.selectionStart = null;
    studioUi.selectionEnd = null;
    if (studioUi.activeTrackIdx !== tIdx) {
      studioUi.activeTrackIdx = tIdx;
      studioUi.lastPlacedNote = null;
      studioUi.positionAreaRange = null;
      studioUi.phrasePositions = [];
      studioUi.selectedPhrasePosIdx = -1;
      player.setActiveTrack(tIdx);
    }
    studioUi.cursor.measureIdx = mIdx;
    studioUi.cursor.beatIdx = bIdx;
    if (clickedStr !== null) studioUi.cursor.stringIdx = clickedStr;
    inputCtrl.resetBuffer();
    clampCursor();
    player.seekTo(mIdx, bIdx);
    const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
    const beat = tr?.measures?.[mIdx]?.beats?.[bIdx];
    if (beat) player.auditionBeat(tr, beat);
    renderAllStudio();
    return;
  }

  const actionEl = target.closest('[data-action]');
  if (!actionEl) return;
  const action = actionEl.getAttribute('data-action');

  if (eventType === 'click') {
    switch (action) {
      case 'play-toggle':
        player.togglePlay();
        return;
      case 'stop':
        player.stop();
        renderAllStudio();
        return;
      case 'toggle-countin':
        studioUi.countInEnabled = !studioUi.countInEnabled;
        player.setCountIn(studioUi.countInEnabled);
        renderAllStudio();
        return;
      case 'toggle-metronome':
        studioUi.metronomeEnabled = !studioUi.metronomeEnabled;
        player.setMetronome(studioUi.metronomeEnabled);
        renderAllStudio();
        return;
      case 'toggle-metro-panel':
        studioUi.metroPanelOpen = !studioUi.metroPanelOpen;
        renderAllStudio();
        return;
      case 'metro-cell': {
        const col = Number(actionEl.getAttribute('data-beat-col'));
        const lvl = Number(actionEl.getAttribute('data-level'));
        const curNum = studioUi.song.tracks?.[0]?.measures?.[studioUi.cursor.measureIdx]?.numerator || 4;
        const pat = normalizeMetronomePattern(studioUi.song.metronomePattern, curNum);
        if (Number.isFinite(col) && col >= 0 && col < pat.levels.length) {
          const nextLvl = (pat.levels[col] === lvl) ? 0 : lvl;
          pat.levels[col] = nextLvl;
          studioUi.song.metronomePattern = pat;
          player.setMetronomePattern(pat);
          if (nextLvl > 0) player.auditionMetronomeLevel(nextLvl);
          renderAllStudio();
        }
        return;
      }
      case 'metro-subdiv': {
        const sub = Number(actionEl.getAttribute('data-sub')) || 1;
        const curNum = studioUi.song.tracks?.[0]?.measures?.[studioUi.cursor.measureIdx]?.numerator || 4;
        const pat = normalizeMetronomePattern(studioUi.song.metronomePattern, curNum);
        pat.subdivisions = sub;
        studioUi.song.metronomePattern = pat;
        player.setMetronomePattern(pat);
        renderAllStudio();
        return;
      }
      case 'metro-preset': {
        const pId = actionEl.getAttribute('data-preset') || 'standard';
        const curNum = studioUi.song.tracks?.[0]?.measures?.[studioUi.cursor.measureIdx]?.numerator || 4;
        const curPat = normalizeMetronomePattern(studioUi.song.metronomePattern, curNum);
        const nextPat = applyMetronomePreset(pId, curNum, curPat.subdivisions);
        studioUi.song.metronomePattern = nextPat;
        player.setMetronomePattern(nextPat);
        showToast('Zmieniono akcenty metronomu');
        renderAllStudio();
        return;
      }
      case 'drum-seq-brush': {
        const b = Number(actionEl.getAttribute('data-brush')) || 2;
        studioUi.drumBrushLevel = Math.max(1, Math.min(3, b));
        renderAllStudio();
        return;
      }
      case 'drum-cell': {
        const laneId = actionEl.getAttribute('data-lane');
        const stepIdx = Number(actionEl.getAttribute('data-step'));
        const lane = DRUM_GRID_LANES.find(l => l.id === laneId);
        if (!lane || !Number.isFinite(stepIdx)) return;
        pushUndoSnapshot();
        const nextLvl = setCustomDrumCell(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, laneId, stepIdx, studioUi.drumBrushLevel);
        if (nextLvl > 0) {
          const hitKind = nextLvl === 3 ? lane.accent : lane.normal;
          const vel = nextLvl === 3 ? 1.0 : (nextLvl === 1 ? 0.45 : 0.85);
          playStudioDrumHit(hitKind, vel, getHost());
        }
        reloadPlayerFromSong(true);
        renderAllStudio();
        return;
      }
      case 'drum-seq-res': {
        const spb = Number(actionEl.getAttribute('data-spb')) === 4 ? 4 : 2;
        pushUndoSnapshot();
        setCustomDrumResolution(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, spb);
        reloadPlayerFromSong(true);
        renderAllStudio();
        return;
      }
      case 'drum-seq-copy-all': {
        pushUndoSnapshot();
        copyDrumGridToAllMeasures(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx);
        reloadPlayerFromSong(true);
        showToast('Skopiowano rytm perkusji na pasujące takty');
        renderAllStudio();
        return;
      }
      case 'drum-seq-reset': {
        pushUndoSnapshot();
        resetMeasureDrumGrid(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx);
        reloadPlayerFromSong(true);
        showToast('Przywrócono bazowy klocek perkusyjny');
        renderAllStudio();
        return;
      }
      case 'toggle-loop':
        studioUi.loopEnabled = !studioUi.loopEnabled;
        player.setLoop({ enabled: studioUi.loopEnabled, start: studioUi.loopStart, end: studioUi.loopEnd });
        renderAllStudio();
        return;
      case 'set-loop-a':
        studioUi.loopStart = studioUi.cursor.measureIdx;
        if (studioUi.loopEnd < studioUi.loopStart) studioUi.loopEnd = studioUi.loopStart;
        studioUi.loopEnabled = true;
        player.setLoop({ enabled: true, start: studioUi.loopStart, end: studioUi.loopEnd });
        showToast(`Pętla A = takt ${studioUi.loopStart + 1}`);
        renderAllStudio();
        return;
      case 'set-loop-b':
        studioUi.loopEnd = studioUi.cursor.measureIdx;
        if (studioUi.loopEnd < studioUi.loopStart) studioUi.loopStart = studioUi.loopEnd;
        studioUi.loopEnabled = true;
        player.setLoop({ enabled: true, start: studioUi.loopStart, end: studioUi.loopEnd });
        showToast(`Pętla B = takt ${studioUi.loopEnd + 1}`);
        renderAllStudio();
        return;
      case 'set-speed': {
        const pct = Number(actionEl.getAttribute('data-speed')) || 100;
        studioUi.speedPercent = pct;
        player.setSpeedPercent(pct);
        renderAllStudio();
        return;
      }
      case 'toggle-autotempo':
        studioUi.autoTempoEnabled = !studioUi.autoTempoEnabled;
        player.setAutoTempo(studioUi.autoTempoEnabled, studioUi.autoTempoStep);
        renderAllStudio();
        return;
      case 'set-studio-ui-mode': {
        const nextUiMode = actionEl.getAttribute('data-mode') === 'pro' ? 'pro' : 'simple';
        const prevUiMode = studioUi.studioPrefs.uiMode === 'pro' ? 'pro' : 'simple';
        if (nextUiMode !== prevUiMode) {
          player.stop();
          studioUi.studioPrefs.uiMode = nextUiMode;
          if (nextUiMode === 'simple') {
            enterSimpleModeProjection();
            if (studioUi.viewMode === 'staff') {
              studioUi.viewMode = 'tab';
              studioUi.scoreAllTracks = false;
            }
          } else {
            exitSimpleModeToPro();
          }
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          studioUi.positionAreaRange = null;
          studioUi.savedPositionAreaRange = null;
          studioUi.selPosAnalysis = null;
          reloadPlayerFromSong();
          showToast(nextUiMode === 'simple'
            ? 'Tryb Uproszczony (liniowe takty; ustawienia Pro zapamiętane w tle)'
            : 'Tryb Pro (przywrócono repetycje, pętlę i pełne narzędzia)');
        }
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'toggle-simple-staff': {
        studioUi.viewMode = (studioUi.viewMode === 'staff_tab') ? 'tab' : 'staff_tab';
        studioUi.scoreAllTracks = false;
        renderAllStudio();
        return;
      }
      case 'save-project-local':
        saveCurrentStudioProject({ asNew: false });
        return;
      case 'save-project-as-new':
        saveCurrentStudioProject({ asNew: true });
        return;
      case 'delete-project-local': {
        const host = getHost();
        if (studioUi.savedProjectId && host && typeof host.deleteLibraryItem === 'function') {
          const delId = studioUi.savedProjectId;
          Promise.resolve(host.deleteLibraryItem('studio', delId)).then(() => {
            studioUi.savedProjectId = null;
            showToast('Usunięto projekt z biblioteki lokalnej');
            renderAllStudio();
          });
        }
        return;
      }
      case 'set-view-mode': {
        const mode = actionEl.getAttribute('data-mode');
        if (mode === 'staff') {
          studioUi.viewMode = 'staff';
          if ((studioUi.song.tracks?.length || 0) > 1) {
            studioUi.scoreAllTracks = true;
          }
        } else if (mode === 'staff_tab') {
          studioUi.viewMode = 'staff_tab';
          studioUi.scoreAllTracks = false;
        } else {
          studioUi.viewMode = 'tab';
          studioUi.scoreAllTracks = false;
        }
        renderAllStudio();
        return;
      }
      case 'toggle-all-tracks-score':
        studioUi.scoreAllTracks = !studioUi.scoreAllTracks;
        renderAllStudio();
        return;
      case 'toggle-score-note-names':
        studioUi.showScoreNoteNames = !studioUi.showScoreNoteNames;
        renderAllStudio();
        return;
      case 'toggle-auto-chords':
        studioUi.studioPrefs.showAutoChords = !(studioUi.studioPrefs.showAutoChords !== false);
        persistPrefsToHost();
        renderAllStudio();
        return;
      case 'toggle-arranger':
        studioUi.arrangerOpen = !studioUi.arrangerOpen;
        renderAllStudio();
        return;
      case 'clear-selection':
        studioUi.beatSelection = null;
        studioUi.selectionStart = null;
        studioUi.selectionEnd = null;
        studioUi.positionAreaRange = null;
        studioUi.savedPositionAreaRange = null;
        studioUi.selPosAnalysis = null;
        renderAllStudio();
        return;
      case 'toggle-scale-hints':
        studioUi.showScaleHints = !studioUi.showScaleHints;
        updateLiveFretboard();
        renderAllStudio();
        return;
      case 'toggle-octave-bands':
        studioUi.studioPrefs.showOctaveBands = !studioUi.studioPrefs.showOctaveBands;
        persistPrefsToHost();
        updateLiveFretboard();
        renderAllStudio();
        return;
      case 'tab-zoom-step': {
        const delta = Number(actionEl.getAttribute('data-delta')) || 0;
        const cur = Number(studioUi.studioPrefs.tabZoom) || 100;
        studioUi.studioPrefs.tabZoom = Math.max(70, Math.min(160, Math.round(cur + delta)));
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'tab-zoom-reset':
        studioUi.studioPrefs.tabZoom = 100;
        persistPrefsToHost();
        renderAllStudio();
        return;
      case 'fb-zoom-step': {
        const delta = Number(actionEl.getAttribute('data-delta')) || 0;
        const cur = Number(studioUi.studioPrefs.fretboardZoom) || 100;
        studioUi.studioPrefs.fretboardZoom = Math.max(75, Math.min(150, Math.round(cur + delta)));
        persistPrefsToHost();
        updateLiveFretboard();
        renderAllStudio();
        return;
      }
      case 'fb-zoom-reset':
        studioUi.studioPrefs.fretboardZoom = 100;
        persistPrefsToHost();
        updateLiveFretboard();
        renderAllStudio();
        return;
      case 'studio-sens-step': {
        const delta = Number(actionEl.getAttribute('data-delta')) || 0;
        const curOff = (studioUi.studioPrefs.detectSensitivityOffset !== undefined && studioUi.studioPrefs.detectSensitivityOffset !== null)
          ? Number(studioUi.studioPrefs.detectSensitivityOffset)
          : thresholdToSensitivityOffset(studioUi.studioPrefs.detectSensitivity);
        const mapped = sensitivityOffsetToThresholds(curOff + delta);
        studioUi.studioPrefs.detectSensitivityOffset = mapped.offset;
        studioUi.studioPrefs.detectSensitivity = mapped.thresholdOn;
        studioUi.studioPrefs.detectHighThresholdOn = mapped.highThresholdOn;
        micCtrl.syncThresholdToDetector();
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'studio-sens-reset': {
        const mapped = sensitivityOffsetToThresholds(0);
        studioUi.studioPrefs.detectSensitivityOffset = mapped.offset;
        studioUi.studioPrefs.detectSensitivity = mapped.thresholdOn;
        studioUi.studioPrefs.detectHighThresholdOn = mapped.highThresholdOn;
        micCtrl.syncThresholdToDetector();
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'new-song':
        player.stop();
        pushUndoSnapshot();
        studioUi.savedProjectId = null;
        studioUi.proBackup = null;
        studioUi.selPosAnalysis = null;
        studioUi.activeDuration = null;
        studioUi.song = createEmptySong({ title: 'Nowy projekt', bpm: 120, measureCount: 4 });
        studioUi.activeTrackIdx = 0;
        studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
        studioUi.beatSelection = null;
        studioUi.selectionStart = null;
        studioUi.selectionEnd = null;
        studioUi.lastPlacedNote = null;
        studioUi.positionAreaRange = null;
        studioUi.savedPositionAreaRange = null;
        studioUi.phrasePositions = [];
        studioUi.selectedPhrasePosIdx = -1;
        if (studioUi.studioPrefs?.uiMode !== 'pro') enterSimpleModeProjection();
        reloadPlayerFromSong();
        showToast('Utworzono pusty projekt (Gitara + Perkusja)');
        renderAllStudio();
        return;
      case 'export-json':
        triggerJsonDownload();
        return;
      case 'export-gp5':
        triggerBinaryExport('/api/studio/export_gp5', 'gp5');
        return;
      case 'export-mid':
        triggerBinaryExport('/api/studio/export_mid', 'mid');
        return;
      case 'import-sf2-trigger': {
        const sfInp = document.getElementById('studioSf2FileInput');
        if (sfInp) sfInp.click();
        return;
      }
      case 'set-sound-mode': {
        const nextEngine = actionEl.getAttribute('data-mode') === 'sf2' ? 'sf2' : 'webaudio';
        if (nextEngine === 'sf2' && !soundFontSynth.isLoaded()) {
          showToast('Wczytaj najpierw plik SoundFont2 (.sf2).');
          return;
        }
        studioUi.studioPrefs.soundMode = nextEngine;
        player.setSoundFontEnabled(nextEngine === 'sf2');
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'mic-mode': {
        const nextMicMode = actionEl.getAttribute('data-mic-mode') || 'off';
        micCtrl.setMode(nextMicMode);
        return;
      }
      case 'set-rec-engine': {
        const eng = actionEl.getAttribute('data-engine') === 'live' ? 'live' : 'offline';
        studioUi.studioPrefs.liveRecMode = eng;
        persistPrefsToHost();
        renderAllStudio();
        return;
      }
      case 'mic-recompute':
        micCtrl.recomputeLastRecording();
        return;
      case 'insert-live-chord':
        insertLiveChordFromMic();
        return;
      case 'pick-note-cand': {
        const sTo = Number(actionEl.getAttribute('data-cand-string'));
        const fTo = Number(actionEl.getAttribute('data-cand-fret'));
        if (Number.isFinite(sTo) && Number.isFinite(fTo)) {
          micCtrl.selectCandidateForNote(sTo, fTo);
        }
        return;
      }
      case 'pick-phrase-pos': {
        const posIdx = Number(actionEl.getAttribute('data-pos-idx'));
        if (Number.isFinite(posIdx)) {
          micCtrl.applyPositionIndex(posIdx);
          studioUi.savedPositionAreaRange = studioUi.positionAreaRange ? { ...studioUi.positionAreaRange } : null;
        }
        return;
      }
      case 'apply-sel-pos': {
        const kind = actionEl.getAttribute('data-pos-kind') === 'octave' ? 'octave' : 'same';
        const posIdx = Number(actionEl.getAttribute('data-pos-idx'));
        const list = kind === 'octave'
          ? (studioUi.selPosAnalysis?.allOctavePositions || [])
          : (studioUi.selPosAnalysis?.sameOctave || []);
        const pos = list[posIdx];
        if (!pos) return;
        pushUndoSnapshot();
        const applied = applyPhrasePosition(studioUi.song, studioUi.activeTrackIdx, pos);
        if (applied > 0) {
          studioUi.positionAreaRange = (pos.maxFret > 0 || pos.minFret > 0)
            ? { minFret: pos.minFret, maxFret: pos.maxFret, label: pos.label }
            : null;
          studioUi.savedPositionAreaRange = studioUi.positionAreaRange ? { ...studioUi.positionAreaRange } : null;
          reloadPlayerFromSong(true);
          showToast(kind === 'octave'
            ? `Przeniesiono zaznaczenie: ${pos.label}`
            : `Zmieniono pozycję zaznaczenia na ${pos.shortRange}`);
          renderAllStudio();
        } else {
          showToast('Nie można zastosować tej pozycji (kolizja lub zakres strun).');
        }
        return;
      }
      case 'import-json-trigger': {
        const fileInp = document.getElementById('studioJsonFileInput');
        if (fileInp) fileInp.click();
        return;
      }
      case 'undo': {
        const prev = studioUi.history.undo(studioUi.song);
        if (prev) {
          studioUi.song = prev;
          studioUi.activeTrackIdx = Math.max(0, Math.min((studioUi.song.tracks?.length || 1) - 1, studioUi.activeTrackIdx));
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          studioUi.lastPlacedNote = null;
          studioUi.positionAreaRange = null;
          studioUi.savedPositionAreaRange = null;
          studioUi.selPosAnalysis = null;
          studioUi.phrasePositions = [];
          studioUi.selectedPhrasePosIdx = -1;
          clampCursor();
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'redo': {
        const next = studioUi.history.redo(studioUi.song);
        if (next) {
          studioUi.song = next;
          studioUi.activeTrackIdx = Math.max(0, Math.min((studioUi.song.tracks?.length || 1) - 1, studioUi.activeTrackIdx));
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          studioUi.lastPlacedNote = null;
          studioUi.positionAreaRange = null;
          studioUi.savedPositionAreaRange = null;
          studioUi.selPosAnalysis = null;
          studioUi.phrasePositions = [];
          studioUi.selectedPhrasePosIdx = -1;
          clampCursor();
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'fb-click': {
        const s = Number(actionEl.getAttribute('data-fb-string')) || 0;
        const f = Number(actionEl.getAttribute('data-fb-fret')) || 0;
        inputCtrl.handleFretClick(s, f, { shiftKey: Boolean(origEvent?.shiftKey) });
        return;
      }
      case 'palette-cmd': {
        const cmd = actionEl.getAttribute('data-cmd');
        if (cmd) inputCtrl.executeCommand(cmd);
        return;
      }
      case 'set-clip-scope':
        studioUi.clipboardScope = actionEl.getAttribute('data-scope') === 'all' ? 'all' : 'track';
        renderAllStudio();
        return;
      case 'set-paste-mode':
        studioUi.clipboardPasteMode = actionEl.getAttribute('data-mode') === 'insert' ? 'insert' : 'overwrite';
        renderAllStudio();
        return;
      case 'set-fret-mode':
        studioUi.fretInputMode = actionEl.getAttribute('data-mode') === 'audition' ? 'audition' : 'edit';
        renderAllStudio();
        return;
      case 'detect-key': {
        const res = detectKeyFromSongJs(studioUi.song, getAccMode());
        studioUi.song.keyRootPc = res.rootPc;
        studioUi.song.keyScaleId = res.scaleId;
        showToast(`Wykryto tonację: ${res.label}`);
        renderAllStudio();
        return;
      }
      case 'inline-prompt-preset':
        inputCtrl.submitInlinePrompt(actionEl.getAttribute('data-value') ?? '');
        return;
      case 'inline-prompt-submit': {
        const inp = document.getElementById('studioInlineInput');
        inputCtrl.submitInlinePrompt(inp ? inp.value : studioUi.inlinePrompt?.value);
        return;
      }
      case 'inline-prompt-cancel':
        inputCtrl.cancelInlinePrompt();
        return;
      case 'open-cmdk':
        studioUi.cmdPalette = { open: true, query: '', selectedIdx: 0 };
        renderAllStudio();
        return;
      case 'set-right-tab':
        studioUi.rightPanelTab = actionEl.getAttribute('data-tab') === 'settings' ? 'settings' : 'tools';
        renderAllStudio();
        return;
      case 'cmdk-close':
        studioUi.cmdPalette.open = false;
        renderAllStudio();
        return;
      case 'cmdk-backdrop':
        if (origEvent && origEvent.target === actionEl) {
          studioUi.cmdPalette.open = false;
          renderAllStudio();
        }
        return;
      case 'cmdk-run': {
        const cmd = actionEl.getAttribute('data-cmd');
        studioUi.cmdPalette.open = false;
        if (cmd) inputCtrl.executeCommand(cmd);
        return;
      }
      case 'drum-pad': {
        const kind = actionEl.getAttribute('data-drum-hit') || 'snare';
        playStudioDrumHit(kind, 0.95, getHost());
        return;
      }
      case 'select-track': {
        studioUi.activeTrackIdx = Number(actionEl.getAttribute('data-track')) || 0;
        studioUi.beatSelection = null;
        studioUi.selectionStart = null;
        studioUi.selectionEnd = null;
        studioUi.lastPlacedNote = null;
        studioUi.positionAreaRange = null;
        studioUi.phrasePositions = [];
        studioUi.selectedPhrasePosIdx = -1;
        player.setActiveTrack(studioUi.activeTrackIdx);
        clampCursor();
        renderAllStudio();
        return;
      }
      case 'select-bar': {
        const mIdx = Number(actionEl.getAttribute('data-measure')) || 0;
        if (origEvent?.shiftKey) {
          const sA = (studioUi.selectionStart !== null) ? studioUi.selectionStart : studioUi.cursor.measureIdx;
          const sB = mIdx;
          studioUi.selectionStart = sA;
          studioUi.selectionEnd = sB;
          const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
          const lo = Math.min(sA, sB);
          const hi = Math.max(sA, sB);
          const endBeats = Math.max(1, tr?.measures?.[hi]?.beats?.length || 1);
          studioUi.beatSelection = {
            trackIdx: studioUi.activeTrackIdx,
            anchor: { measureIdx: lo, beatIdx: 0 },
            focus: { measureIdx: hi, beatIdx: endBeats - 1 },
            fullMeasures: true
          };
        } else {
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          studioUi.cursor.measureIdx = mIdx;
          studioUi.cursor.beatIdx = 0;
          clampCursor();
          player.seekTo(mIdx, 0);
        }
        renderAllStudio();
        return;
      }
      case 'matrix-cell': {
        const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
        const mIdx = Number(actionEl.getAttribute('data-measure')) || 0;
        if (studioUi.activeTrackIdx !== tIdx) {
          studioUi.beatSelection = null;
          studioUi.lastPlacedNote = null;
          studioUi.positionAreaRange = null;
          studioUi.phrasePositions = [];
          studioUi.selectedPhrasePosIdx = -1;
        }
        studioUi.activeTrackIdx = tIdx;
        player.setActiveTrack(tIdx);
        if (origEvent?.shiftKey) {
          const sA = (studioUi.selectionStart !== null) ? studioUi.selectionStart : studioUi.cursor.measureIdx;
          const sB = mIdx;
          studioUi.selectionStart = sA;
          studioUi.selectionEnd = sB;
          const tr = studioUi.song.tracks[tIdx];
          const lo = Math.min(sA, sB);
          const hi = Math.max(sA, sB);
          const endBeats = Math.max(1, tr?.measures?.[hi]?.beats?.length || 1);
          studioUi.beatSelection = {
            trackIdx: tIdx,
            anchor: { measureIdx: lo, beatIdx: 0 },
            focus: { measureIdx: hi, beatIdx: endBeats - 1 },
            fullMeasures: true
          };
        } else {
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          studioUi.cursor.measureIdx = mIdx;
          studioUi.cursor.beatIdx = 0;
          player.seekTo(mIdx, 0);
        }
        clampCursor();
        renderAllStudio();
        return;
      }
      case 'toggle-solo': {
        const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
        const tr = studioUi.song.tracks[tIdx];
        if (tr) {
          tr.solo = !tr.solo;
          reloadPlayerFromSong(true);
          renderAllStudio();
        }
        return;
      }
      case 'toggle-mute': {
        const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
        const tr = studioUi.song.tracks[tIdx];
        if (tr) {
          tr.muted = !tr.muted;
          reloadPlayerFromSong(true);
          renderAllStudio();
        }
        return;
      }
      case 'add-track': {
        const role = actionEl.getAttribute('data-role') || 'guitar2';
        pushUndoSnapshot();
        const added = addTrack(studioUi.song, role);
        if (added >= 0) {
          studioUi.activeTrackIdx = added;
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'duplicate-track': {
        pushUndoSnapshot();
        const added = duplicateTrack(studioUi.song, studioUi.activeTrackIdx);
        if (added >= 0) {
          studioUi.activeTrackIdx = added;
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'remove-track': {
        const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
        pushUndoSnapshot();
        if (removeTrack(studioUi.song, tIdx)) {
          if (studioUi.activeTrackIdx >= studioUi.song.tracks.length) {
            studioUi.activeTrackIdx = studioUi.song.tracks.length - 1;
          }
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'add-measure': {
        pushUndoSnapshot();
        const inserted = addMeasure(studioUi.song, studioUi.cursor.measureIdx, Number.isInteger(studioUi.activeDuration) ? studioUi.activeDuration : null);
        if (inserted >= 0) {
          studioUi.cursor.measureIdx = inserted;
          studioUi.cursor.beatIdx = 0;
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'remove-measure': {
        pushUndoSnapshot();
        if (removeMeasure(studioUi.song, studioUi.cursor.measureIdx)) {
          clampCursor();
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      default:
        return;
    }
  }

  if (eventType === 'input') {
    if (action === 'speed-slider') {
      const pct = Number(actionEl.value) || 100;
      studioUi.speedPercent = pct;
      player.setSpeedPercent(pct);
      const lbl = document.getElementById('studioEffectiveBpmLabel');
      if (lbl) lbl.textContent = `${player.getEffectiveBpm()} BPM (${pct}%)`;
      document.querySelectorAll('.studio-speed-pill').forEach(btn => {
        btn.classList.toggle('active', Number(btn.getAttribute('data-speed')) === pct);
      });
      return;
    }
    if (action === 'track-volume') {
      const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
      const tr = studioUi.song.tracks[tIdx];
      if (tr) tr.volume = Math.max(0, Math.min(1, Number(actionEl.value) / 100));
      return;
    }
    if (action === 'studio-sensitivity') {
      const offset = Math.max(-5, Math.min(5, Math.round(Number(actionEl.value) || 0)));
      const mapped = sensitivityOffsetToThresholds(offset);
      studioUi.studioPrefs.detectSensitivityOffset = mapped.offset;
      studioUi.studioPrefs.detectSensitivity = mapped.thresholdOn;
      studioUi.studioPrefs.detectHighThresholdOn = mapped.highThresholdOn;
      const valEl = document.getElementById('studioSensitivityVal');
      if (valEl) valEl.textContent = formatSensitivityOffsetLabel(mapped.offset);
      micCtrl.syncThresholdToDetector();
      persistPrefsToHost();
      return;
    }
    if (action === 'studio-mindur') {
      const ms = Math.max(20, Math.min(300, Math.round(Number(actionEl.value) || 80)));
      studioUi.studioPrefs.minNoteDurationMs = ms;
      const valEl = document.getElementById('studioMinDurVal');
      if (valEl) valEl.textContent = `${ms} ms`;
      micCtrl.syncThresholdToDetector();
      persistPrefsToHost();
      return;
    }
    if (action === 'inline-prompt-input' && studioUi.inlinePrompt) {
      studioUi.inlinePrompt.value = actionEl.value;
      return;
    }
    if (action === 'cmdk-input' && studioUi.cmdPalette) {
      studioUi.cmdPalette.query = actionEl.value;
      studioUi.cmdPalette.selectedIdx = 0;
      const listEl = document.querySelector('.studio-cmdk-list');
      if (listEl) {
        const items = filterStudioCommands(studioUi.cmdPalette.query);
        listEl.innerHTML = items.length === 0
          ? `<div style="padding:18px;text-align:center;color:var(--text-muted);font-size:12.5px;">Brak pasujących akcji dla „${escapeHtml(studioUi.cmdPalette.query)}”.</div>`
          : items.slice(0, 14).map((cmd, i) => `
              <button type="button" class="studio-cmdk-item ${i === 0 ? 'selected' : ''}" data-action="cmdk-run" data-cmd="${escapeHtml(cmd.id)}">
                <div style="display:flex;flex-direction:column;align-items:flex-start;gap:2px;">
                  <span style="font-weight:700;font-size:12.5px;">${escapeHtml(cmd.label)}</span>
                  <span style="font-size:10.5px;color:var(--text-muted);">${escapeHtml(cmd.category)}</span>
                </div>
                ${cmd.shortcut ? `<kbd class="studio-cmdk-kbd">${escapeHtml(cmd.shortcut)}</kbd>` : ''}
              </button>
            `).join('');
      }
      return;
    }
  }

  if (eventType === 'change') {
    if (action === 'palette-dynamic') {
      inputCtrl.executeCommand('beat:dynamic', Number(actionEl.value) || 6);
      return;
    }
    if (action === 'palette-bend') {
      inputCtrl.executeCommand(`bend:${actionEl.value || 'clear'}`);
      return;
    }
    if (action === 'palette-whammy') {
      inputCtrl.executeCommand(`whammy:${actionEl.value || 'clear'}`);
      return;
    }
    if (action === 'studio-sensitivity') {
      renderAllStudio();
      return;
    }
    if (action === 'set-timesig') {
      const parts = String(actionEl.value || '4/4').split('/');
      const num = Number(parts[0]) || 4;
      const den = Number(parts[1]) || 4;
      pushUndoSnapshot();
      setMeasureTimeSignature(studioUi.song, studioUi.cursor.measureIdx, num, den, true);
      studioUi.song.metronomePattern = normalizeMetronomePattern(studioUi.song.metronomePattern, num);
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'drum-seq-preset') {
      const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
      const m = tr?.measures?.[studioUi.cursor.measureIdx];
      if (tr && m) {
        pushUndoSnapshot();
        m.drumBlockId = actionEl.value;
        m.customDrumGrid = null;
        reloadPlayerFromSong(true);
        renderAllStudio();
      }
      return;
    }
    if (action === 'set-bars-per-row') {
      const val = actionEl.value;
      studioUi.barsPerRow = (val === 'auto') ? 'auto' : (Number(val) || 4);
      renderAllStudio();
      return;
    }
    if (action === 'studio-grid') {
      studioUi.studioPrefs.quantizeGrid = Number(actionEl.value);
      persistPrefsToHost();
      return;
    }
    if (action === 'import-sf2-file') {
      const file = actionEl.files?.[0];
      if (!file) return;
      file.arrayBuffer().then(buf => {
        try {
          const st = soundFontSynth.loadFromArrayBuffer(buf, file.name);
          studioUi.studioPrefs.soundMode = 'sf2';
          player.setSoundFontEnabled(true);
          persistPrefsToHost();
          showToast(`Załadowano SoundFont2: ${st.name} (${st.presetCount} brzmień)`);
          renderAllStudio();
        } catch (err) {
          showToast(err?.message || 'Błąd odczytu pliku .sf2');
        }
      });
      return;
    }
    if (action === 'load-project') {
      const val = String(actionEl.value || '');
      if (val.startsWith('saved:')) {
        const targetId = val.slice(6);
        const found = (getHost()?.getLibraryItems?.('studio') || []).find(it => it.id === targetId);
        if (found) {
          applyLoadedStudioItem(found, { savedId: found.id });
          showToast(`Wczytano „${studioUi.song.title}”`);
          renderAllStudio();
        }
        return;
      }
      const demoKey = val.startsWith('demo:') ? val.slice(5) : (val || 'rock_riff_em');
      player.stop();
      pushUndoSnapshot();
      studioUi.savedProjectId = null;
      studioUi.proBackup = null;
      studioUi.selPosAnalysis = null;
      studioUi.savedPositionAreaRange = null;
      studioUi.demoId = demoKey;
      studioUi.song = createDemoSong(demoKey);
      studioUi.activeTrackIdx = 0;
      studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
      studioUi.beatSelection = null;
      studioUi.selectionStart = null;
      studioUi.selectionEnd = null;
      studioUi.lastPlacedNote = null;
      studioUi.positionAreaRange = null;
      studioUi.phrasePositions = [];
      studioUi.selectedPhrasePosIdx = -1;
      if (studioUi.studioPrefs?.uiMode !== 'pro') enterSimpleModeProjection();
      studioUi.loopEnd = Math.min(3, Math.max(0, getMasterMeasureCount(studioUi.song) - 1));
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'load-demo') {
      player.stop();
      pushUndoSnapshot();
      studioUi.savedProjectId = null;
      studioUi.proBackup = null;
      studioUi.selPosAnalysis = null;
      studioUi.savedPositionAreaRange = null;
      studioUi.demoId = actionEl.value;
      studioUi.song = createDemoSong(studioUi.demoId);
      studioUi.activeTrackIdx = 0;
      studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
      studioUi.beatSelection = null;
      studioUi.selectionStart = null;
      studioUi.selectionEnd = null;
      studioUi.lastPlacedNote = null;
      studioUi.positionAreaRange = null;
      studioUi.phrasePositions = [];
      studioUi.selectedPhrasePosIdx = -1;
      if (studioUi.studioPrefs?.uiMode !== 'pro') enterSimpleModeProjection();
      studioUi.loopEnd = Math.min(3, Math.max(0, getMasterMeasureCount(studioUi.song) - 1));
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'drum-block') {
      const tIdx = Number(actionEl.getAttribute('data-track')) || 0;
      const mIdx = Number(actionEl.getAttribute('data-measure')) || 0;
      pushUndoSnapshot();
      setDrumBlockForMeasure(studioUi.song, tIdx, mIdx, actionEl.value);
      reloadPlayerFromSong(true);
      renderAllStudio();
      return;
    }
    if (action === 'import-json-file') {
      const file = actionEl.files?.[0];
      if (!file) return;
      file.text().then(async text => {
        try {
          const host = getHost();
          if (host && typeof host.importLibraryText === 'function') {
            const res = await host.importLibraryText(text, 'studio');
            host.refreshAfterLibraryImport?.(res);
            const studioEntry = res.entries.find(e => e.category === 'studio');
            if (studioEntry) {
              applyLoadedStudioItem(studioEntry.item, { savedId: studioEntry.item.id });
              showToast(`Wczytano i zapisano na stałe „${studioUi.song.title}”`);
            } else {
              showToast(`Zapisano w bibliotece lokalnej (${res.count} elem.)`);
            }
          } else {
            applyLoadedStudioItem(text, { savedId: null });
            showToast(`Wczytano „${studioUi.song.title}”`);
          }
          renderAllStudio();
        } catch (err) {
          showToast(err.message || 'Błąd wczytywania pliku JSON');
        }
      }).finally(() => {
        actionEl.value = '';
      });
      return;
    }
    // Akcje prawego inspektora
    const track = studioUi.song.tracks[studioUi.activeTrackIdx];
    if (action === 'insp-track-name' && track) {
      pushUndoSnapshot();
      track.name = actionEl.value || 'Ścieżka';
      renderAllStudio();
      return;
    }
    if (action === 'insp-track-tone' && track) {
      track.tonePreset = actionEl.value;
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-track-tuning' && track) {
      pushUndoSnapshot();
      setTrackTuning(track, actionEl.value);
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-track-capo' && track) {
      pushUndoSnapshot();
      track.capo = Math.max(0, Math.min(12, Number(actionEl.value) || 0));
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-drum-block' && track) {
      pushUndoSnapshot();
      setDrumBlockForMeasure(studioUi.song, studioUi.activeTrackIdx, studioUi.cursor.measureIdx, actionEl.value);
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-drum-autofill' && track) {
      pushUndoSnapshot();
      track.drumAutoFill = Number(actionEl.value) || 0;
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-drum-crash1' && track) {
      pushUndoSnapshot();
      track.drumCrashOnOne = Boolean(actionEl.checked);
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-song-title') {
      studioUi.song.title = actionEl.value || 'Bez tytułu';
      renderAllStudio();
      return;
    }
    if (action === 'insp-song-artist') {
      studioUi.song.artist = actionEl.value || '';
      return;
    }
    if (action === 'insp-song-bpm') {
      studioUi.song.tempoBpm = Math.max(30, Math.min(280, Math.round(Number(actionEl.value) || 120)));
      reloadPlayerFromSong();
      renderAllStudio();
      return;
    }
    if (action === 'insp-key-root') {
      studioUi.song.keyRootPc = Number(actionEl.value) || 0;
      renderAllStudio();
      return;
    }
    if (action === 'insp-key-scale') {
      studioUi.song.keyScaleId = actionEl.value || 'natural_minor';
      renderAllStudio();
      return;
    }
    if (action === 'set-octave-anchor') {
      const parsed = Number(actionEl.value);
      studioUi.studioPrefs.octaveAnchorPc = Number.isInteger(parsed) ? (((parsed % 12) + 12) % 12) : null;
      persistPrefsToHost();
      updateLiveFretboard();
      renderAllStudio();
      return;
    }
  }
}

let hoverTipEl = null;
let hoverTipTarget = null;
let resizeBound = false;
let resizeTimer = null;
let tabDragSel = null;
let suppressNextBeatClick = false;

function refreshSvgBeatSelectionLive() {
  const svg = document.getElementById('studioTabSvg');
  if (!svg) return;
  const norm = normalizeBeatSelection(studioUi.song, studioUi);
  svg.querySelectorAll('.studio-beat-sel-bg').forEach(rect => {
    const tIdx = Number(rect.getAttribute('data-track'));
    const mIdx = Number(rect.getAttribute('data-measure'));
    const bIdx = Number(rect.getAttribute('data-beat'));
    const active = Boolean(norm && tIdx === norm.trackIdx && isBeatInSelection(norm, mIdx, bIdx));
    rect.setAttribute('opacity', active ? '1' : '0');
  });
}

function ensureHoverTipEl() {
  if (typeof document === 'undefined') return null;
  if (!hoverTipEl || !hoverTipEl.isConnected) {
    hoverTipEl = document.getElementById('studioHoverTip');
    if (!hoverTipEl) {
      hoverTipEl = document.createElement('div');
      hoverTipEl.id = 'studioHoverTip';
      hoverTipEl.className = 'studio-hover-tip';
      document.body.appendChild(hoverTipEl);
    }
  }
  return hoverTipEl;
}

function hideHoverTip() {
  hoverTipTarget = null;
  if (hoverTipEl) hoverTipEl.classList.remove('show');
}

function showHoverTipFor(targetEl) {
  if (!targetEl || typeof window === 'undefined') return;
  const tipEl = ensureHoverTipEl();
  if (!tipEl) return;
  const name = targetEl.getAttribute('data-tip-name') || '';
  const kbd = targetEl.getAttribute('data-tip-kbd') || '';
  const desc = targetEl.getAttribute('data-tip-desc') || '';
  if (!name && !desc) return;

  hoverTipTarget = targetEl;
  tipEl.innerHTML = `<div class="studio-tip-head">
    <span class="studio-tip-name">${escapeHtml(name)}</span>
    ${kbd ? `<kbd class="studio-tip-kbd">${escapeHtml(kbd)}</kbd>` : ''}
  </div>${desc ? `<div class="studio-tip-desc">${escapeHtml(desc)}</div>` : ''}`;
  tipEl.classList.add('show');

  const rect = targetEl.getBoundingClientRect();
  const tipRect = tipEl.getBoundingClientRect();
  const pad = 8;
  const inRightRail = Boolean(targetEl.closest('.right-inspector'));

  let left = rect.left + (rect.width - tipRect.width) / 2;
  let top = rect.top - tipRect.height - 8;

  if (inRightRail && rect.left - tipRect.width - 10 >= pad) {
    left = rect.left - tipRect.width - 10;
    top = rect.top + (rect.height - tipRect.height) / 2;
  } else if (top < pad) {
    top = rect.bottom + 8;
  }

  left = Math.max(pad, Math.min(window.innerWidth - tipRect.width - pad, left));
  top = Math.max(pad, Math.min(window.innerHeight - tipRect.height - pad, top));
  tipEl.style.left = `${Math.round(left)}px`;
  tipEl.style.top = `${Math.round(top)}px`;
}

function bindContainerDelegates(el) {
  if (!el || boundContainers.has(el)) return;
  boundContainers.add(el);
  el.addEventListener('click', e => {
    hideHoverTip();
    handleStageAction(e.target, 'click', e);
  });
  el.addEventListener('mousedown', e => {
    hideHoverTip();
    if (e.button !== 0) return;
    const hit = e.target?.closest?.('.studio-beat-hit');
    if (!hit) return;
    e.preventDefault();
    const hitTrackAttr = hit.getAttribute('data-track');
    const tIdx = hitTrackAttr !== null ? (Number(hitTrackAttr) || 0) : studioUi.activeTrackIdx;
    const mIdx = Number(hit.getAttribute('data-measure')) || 0;
    const bIdx = Number(hit.getAttribute('data-beat')) || 0;
    if (e.shiftKey) {
      const prevNorm = normalizeBeatSelection(studioUi.song, studioUi);
      const anchor = (prevNorm && prevNorm.trackIdx === tIdx)
        ? (studioUi.beatSelection?.anchor || prevNorm.start)
        : (studioUi.activeTrackIdx === tIdx ? { measureIdx: studioUi.cursor.measureIdx, beatIdx: studioUi.cursor.beatIdx } : { measureIdx: mIdx, beatIdx: bIdx });
      const hitTrack = studioUi.song.tracks?.[tIdx];
      const isDrumHit = Boolean(hitTrack && (hitTrack.instrumentType === 'drums' || hitTrack.isPercussion));
      studioUi.activeTrackIdx = tIdx;
      studioUi.beatSelection = {
        trackIdx: tIdx,
        anchor,
        focus: { measureIdx: mIdx, beatIdx: bIdx },
        fullMeasures: isDrumHit
      };
      const norm = normalizeBeatSelection(studioUi.song, studioUi);
      if (norm) {
        studioUi.selectionStart = norm.startMeasure;
        studioUi.selectionEnd = norm.endMeasure;
      }
      studioUi.cursor.measureIdx = mIdx;
      studioUi.cursor.beatIdx = bIdx;
      suppressNextBeatClick = true;
      setTimeout(() => { suppressNextBeatClick = false; }, 30);
      player.setActiveTrack(tIdx);
      clampCursor();
      renderAllStudio();
      return;
    }
    tabDragSel = {
      active: true,
      moved: false,
      trackIdx: tIdx,
      anchor: { measureIdx: mIdx, beatIdx: bIdx },
      focus: { measureIdx: mIdx, beatIdx: bIdx }
    };
  });
  el.addEventListener('input', e => handleStageAction(e.target, 'input', e));
  el.addEventListener('change', e => handleStageAction(e.target, 'change', e));
  el.addEventListener('mouseover', e => {
    const posTarget = e.target?.closest?.('[data-pos-min][data-pos-max]');
    if (posTarget) {
      const pMin = Number(posTarget.getAttribute('data-pos-min'));
      const pMax = Number(posTarget.getAttribute('data-pos-max'));
      if (Number.isFinite(pMin) && Number.isFinite(pMax) && (pMax > 0 || pMin > 0)) {
        studioUi.positionAreaRange = { minFret: pMin, maxFret: pMax, label: posTarget.getAttribute('data-tip-name') || '' };
        updateLiveFretboard();
      }
    }
    const tipTarget = e.target?.closest?.('[data-tip-name]');
    if (!tipTarget) {
      if (hoverTipTarget) hideHoverTip();
      return;
    }
    if (tipTarget !== hoverTipTarget) showHoverTipFor(tipTarget);
  });
  el.addEventListener('mouseout', e => {
    const posTarget = e.target?.closest?.('[data-pos-min][data-pos-max]');
    if (posTarget) {
      const relatedPos = e.relatedTarget?.closest?.('[data-pos-min][data-pos-max]');
      if (!relatedPos) {
        studioUi.positionAreaRange = studioUi.savedPositionAreaRange ? { ...studioUi.savedPositionAreaRange } : null;
        updateLiveFretboard();
      }
    }
    if (!hoverTipTarget) return;
    const related = e.relatedTarget?.closest?.('[data-tip-name]');
    if (related !== hoverTipTarget) hideHoverTip();
  });
  el.addEventListener('focusin', e => {
    const tipTarget = e.target?.closest?.('[data-tip-name]');
    if (tipTarget) showHoverTipFor(tipTarget);
  });
  el.addEventListener('focusout', () => hideHoverTip());
  el.addEventListener('scroll', () => hideHoverTip(), { passive: true });
}

function ensureResizeBinding() {
  if (resizeBound || typeof window === 'undefined') return;
  resizeBound = true;
  window.addEventListener('resize', () => {
    hideHoverTip();
    if (!studioUi.lastStageEl || !studioUi.lastStageEl.isConnected) return;
    if (window.__gitaraStudioHost?.getState?.()?.mainView !== 'studio') return;
    if (resizeTimer) clearTimeout(resizeTimer);
    resizeTimer = setTimeout(() => renderAllStudio(), 120);
  });
  window.addEventListener('mousemove', e => {
    if (!tabDragSel?.active || typeof document === 'undefined') return;
    const el = document.elementFromPoint(e.clientX, e.clientY);
    const hit = el?.closest?.('.studio-beat-hit');
    if (!hit) return;
    const hitTrackAttr = hit.getAttribute('data-track');
    const tIdx = hitTrackAttr !== null ? (Number(hitTrackAttr) || 0) : tabDragSel.trackIdx;
    if (tIdx !== tabDragSel.trackIdx) return;
    const mIdx = Number(hit.getAttribute('data-measure')) || 0;
    const bIdx = Number(hit.getAttribute('data-beat')) || 0;
    if (mIdx !== tabDragSel.focus.measureIdx || bIdx !== tabDragSel.focus.beatIdx) {
      tabDragSel.moved = true;
      tabDragSel.focus = { measureIdx: mIdx, beatIdx: bIdx };
      const dragTr = studioUi.song.tracks?.[tabDragSel.trackIdx];
      const isDrumDrag = Boolean(dragTr && (dragTr.instrumentType === 'drums' || dragTr.isPercussion));
      studioUi.activeTrackIdx = tabDragSel.trackIdx;
      studioUi.beatSelection = {
        trackIdx: tabDragSel.trackIdx,
        anchor: tabDragSel.anchor,
        focus: tabDragSel.focus,
        fullMeasures: isDrumDrag
      };
      const norm = normalizeBeatSelection(studioUi.song, studioUi);
      if (norm) {
        studioUi.selectionStart = norm.startMeasure;
        studioUi.selectionEnd = norm.endMeasure;
      }
      refreshSvgBeatSelectionLive();
    }
  });
  window.addEventListener('mouseup', () => {
    if (!tabDragSel?.active) return;
    const wasMoved = tabDragSel.moved;
    const endFocus = tabDragSel.focus;
    const dragTrack = tabDragSel.trackIdx;
    tabDragSel = null;
    if (wasMoved) {
      studioUi.activeTrackIdx = dragTrack;
      studioUi.cursor.measureIdx = endFocus.measureIdx;
      studioUi.cursor.beatIdx = endFocus.beatIdx;
      suppressNextBeatClick = true;
      setTimeout(() => { suppressNextBeatClick = false; }, 30);
      player.setActiveTrack(dragTrack);
      clampCursor();
      renderAllStudio();
    }
  });
}

function render(stageEl, inspEl) {
  syncPrefsFromHost();
  if (studioUi.studioPrefs?.uiMode !== 'pro' && !studioUi.proBackup) {
    enterSimpleModeProjection();
  }
  player.setSoundFontEnabled(studioUi.studioPrefs.soundMode === 'sf2' && soundFontSynth.isLoaded());
  studioUi.lastStageEl = stageEl;
  studioUi.lastInspEl = inspEl;
  bindContainerDelegates(stageEl);
  bindContainerDelegates(inspEl);
  ensureResizeBinding();
  reloadPlayerFromSong();
  renderAllStudio();
}

function stopPlayback() {
  hideHoverTip();
  player.stop();
  micCtrl.setMode('off');
  micCtrl.restoreGlobalConfig();
}

function handleKeyDown(e) {
  inputCtrl.handleKeyDown(e);
}

function onMidiNote(midi) {
  inputCtrl.handleMidiNote(midi);
}

function onPitchFrame(payload) {
  micCtrl.onPitchFrame(payload);
}

function onDetectorReady() {
  micCtrl.onDetectorReady();
}

function importSteps(steps, opts = {}) {
  const num = Math.max(1, Math.min(16, Number(opts.numerator) || 4));
  const den = [1, 2, 4, 8, 16].includes(Number(opts.denominator)) ? Number(opts.denominator) : 4;
  const measures = buildMeasuresFromSteps(steps, {
    numerator: num,
    denominator: den,
    defaultDuration: opts.defaultDuration ?? 1,
    sectionMarker: opts.sectionMarker || opts.title || ''
  });
  pushUndoSnapshot();
  studioUi.proBackup = null;
  studioUi.selPosAnalysis = null;
  studioUi.savedPositionAreaRange = null;
  if (opts.replaceSong) {
    studioUi.savedProjectId = null;
    studioUi.song = createEmptySong({
      title: opts.title || 'Importowane ćwiczenie',
      bpm: Number(opts.bpm) || studioUi.song.tempoBpm || 110,
      numerator: num,
      denominator: den,
      measureCount: measures.length
    });
    if (Array.isArray(opts.tuningMidis) && opts.tuningMidis.length >= 4) {
      studioUi.song.tracks[0].openStringMidis = opts.tuningMidis.slice();
      studioUi.song.tracks[0].stringCount = opts.tuningMidis.length;
      if (opts.tuningMidis.length === 4) {
        studioUi.song.tracks[0].instrumentType = 'bass';
        studioUi.song.tracks[0].tuningId = 'bass_standard';
      }
    }
    studioUi.song.tracks[0].measures = measures;
    studioUi.song.tracks[1].measures = measures.map(() => createEmptyMeasure(num, den, 'rock_8th', true));
    studioUi.activeTrackIdx = 0;
  } else {
    const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
    if (tr && tr.instrumentType !== 'drums') {
      if (Array.isArray(opts.tuningMidis) && opts.tuningMidis.length >= 4 && tr.instrumentType !== 'keys') {
        tr.openStringMidis = opts.tuningMidis.slice();
        tr.stringCount = opts.tuningMidis.length;
      }
      tr.measures = measures;
      for (let t = 0; t < studioUi.song.tracks.length; t++) {
        if (t === studioUi.activeTrackIdx) continue;
        const other = studioUi.song.tracks[t];
        while (other.measures.length < measures.length) {
          other.measures.push(createEmptyMeasure(num, den, 'rock_8th', other.instrumentType === 'drums'));
        }
      }
    }
  }
  studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
  studioUi.positionAreaRange = null;
  if (studioUi.studioPrefs?.uiMode !== 'pro') enterSimpleModeProjection();
  micCtrl.refreshPhrasePositions();
  reloadPlayerFromSong();
  if (studioUi.lastStageEl) renderAllStudio();
  showToast(`Zaimportowano ${measures.length} taktów do Studia TAB`);
}

function importChordProgression(chords, opts = {}) {
  const steps = (Array.isArray(chords) ? chords : []).map((ch, idx) => {
    const notes = [];
    if (Array.isArray(ch.frets)) {
      ch.frets.forEach((f, sIdx) => {
        if (Number.isFinite(f) && f >= 0) {
          notes.push({ string: sIdx, fret: f });
        }
      });
    } else if (Array.isArray(ch.notes)) {
      ch.notes.forEach(n => notes.push(n));
    }
    return {
      notes,
      duration: ch.duration ?? 0,
      chordSymbol: ch.symbol || ch.chordSymbol || `Akord ${idx + 1}`
    };
  });
  return importSteps(steps, {
    title: opts.title || 'Progresja akordów',
    bpm: opts.bpm || 100,
    numerator: opts.numerator || 4,
    denominator: opts.denominator || 4,
    defaultDuration: 0,
    tuningMidis: opts.tuningMidis,
    replaceSong: opts.replaceSong !== false
  });
}

function copyStepsToClipboard(steps, opts = {}) {
  const clip = buildBeatsClipboardFromSteps(steps, opts);
  if (!clip || clip.beatCount === 0) return { ok: false, beatCount: 0 };
  studioUi.clipboard = clip;
  if (studioUi.lastStageEl && studioUi.lastStageEl.isConnected) {
    renderAllStudio();
  }
  showToast(`Skopiowano ${clip.label || 'sekwencję'} (${clip.beatCount} nut) — użyj Ctrl+V w Studio TAB`);
  return { ok: true, beatCount: clip.beatCount };
}

if (typeof window !== 'undefined') {
  window.StudioTab = {
    render,
    stopPlayback,
    handleKeyDown,
    onMidiNote,
    onPitchFrame,
    onDetectorReady,
    importSteps,
    importChordProgression,
    copyStepsToClipboard,
    getState: () => studioUi
  };
}

export { studioUi, render, stopPlayback, handleKeyDown };
