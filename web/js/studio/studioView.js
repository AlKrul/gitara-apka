/**
 * Główny kontroler widoku 🎼 Studio TAB (Faza 1: Kompozytor + Player + Matryca Ścieżki × Takty).
 * Łączy studioModel.js, studioDrumBlocks.js, studioRenderer.js i studioPlayer.js z hostem aplikacji.
 */
import {
  DEFAULT_STUDIO_PREFS,
  NOTE_NAMES_SHARP,
  SCALE_HINT_OPTIONS,
  STUDIO_TUNING_PRESETS,
  TIME_SIGNATURE_PRESETS,
  addMeasure,
  addTrack,
  analyzePhrasePositions,
  applyMetronomePreset,
  buildMeasuresFromSteps,
  buildSongExportPayload,
  clampStudioPrefs,
  copyDrumGridToAllMeasures,
  createDemoSong,
  createEmptyMeasure,
  createEmptySong,
  createStudioHistory,
  createTabNote,
  deserializeStudioSong,
  detectKeyFromSongJs,
  duplicateTrack,
  findFretCandidatesForMidi,
  getMasterMeasureCount,
  getNoteAt,
  getScaleHintPitchClasses,
  isBeatInSelection,
  midiToNoteName,
  normalizeBeatSelection,
  normalizeMetronomePattern,
  noteToMidi,
  removeMeasure,
  removeTrack,
  resetMeasureDrumGrid,
  serializeStudioSong,
  setCustomDrumCell,
  setCustomDrumResolution,
  setDrumBlockForMeasure,
  setMeasureTimeSignature,
  setTrackTuning,
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
  history: createStudioHistory(50),
  activeTrackIdx: 0,
  cursor: { measureIdx: 0, beatIdx: 0, stringIdx: 0 },
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
  drumBrushLevel: 2,
  lastMetroBeatCol: -1,
  studioPrefs: { ...DEFAULT_STUDIO_PREFS },
  phrasePositions: [],
  selectedPhrasePosIdx: -1,
  positionAreaRange: null,
  lastPlacedNote: null,
  lastStageEl: null,
  lastInspEl: null,
  lastPlayPos: { active: false, measureIdx: 0, beatIdx: 0, progress: 0 }
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
  const cands = selCtx ? findFretCandidatesForMidi(track, selCtx.midi, 0, 24) : [];
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
    accMode: getAccMode(),
    fretCount: 17
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
        const px = bx + Math.max(0, Math.min(1, posInfo.progress || 0)) * bw;
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
    playMetronomeTickAudio: (...args) => getHost()?.playMetronomeTickAudio?.(...args)
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
  const accMode = getAccMode();
  const rootOptions = NOTE_NAMES_SHARP.map((nm, pc) =>
    `<option value="${pc}" ${pc === studioUi.song.keyRootPc ? 'selected' : ''}>${escapeHtml(midiToNoteName(pc, accMode))}</option>`
  ).join('');
  const scaleOptions = SCALE_HINT_OPTIONS.map(sc =>
    `<option value="${escapeHtml(sc.id)}" ${sc.id === studioUi.song.keyScaleId ? 'selected' : ''}>${escapeHtml(sc.name)}</option>`
  ).join('');

  const topTabs = `<div class="studio-insp-tabs">
    <button type="button" class="studio-insp-tab-btn ${studioUi.rightPanelTab !== 'settings' ? 'active' : ''}" data-action="set-right-tab" data-tab="tools" data-tip-name="Panel narzędzi" data-tip-desc="Kafelki rytmu, artykulacji, technik, struktury i schowka.">🎛 Narzędzia</button>
    <button type="button" class="studio-insp-tab-btn ${studioUi.rightPanelTab === 'settings' ? 'active' : ''}" data-action="set-right-tab" data-tab="settings" data-tip-name="Ustawienia ścieżki" data-tip-desc="Brzmienie, strój, kapodaster, tytuł, tempo i tonacja utworu.">⚙️ Ścieżka</button>
    <button type="button" class="studio-btn sm" data-action="open-cmdk" data-tip-name="Szybka wyszukiwarka komend" data-tip-desc="Znajdź i uruchom dowolną akcję lub skrót Studia TAB." data-tip-kbd="Ctrl+K">🔍</button>
  </div>`;

  if (studioUi.rightPanelTab !== 'settings') {
    const railPalette = renderStudioPaletteHtml({
      track,
      beat: curBeat,
      note: curNote,
      measure: curM,
      cursor: studioUi.cursor,
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

  return `<div class="studio-insp-wrap">
    ${topTabs}
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
  const tabSvg = renderStudioTabSvg(studioUi.song, {
    activeTrackIdx: studioUi.activeTrackIdx,
    cursor: studioUi.cursor,
    playhead: studioUi.lastPlayPos,
    viewMode: studioUi.viewMode,
    allTracks: studioUi.scoreAllTracks,
    showNoteNames: studioUi.showScoreNoteNames,
    barsPerRow: studioUi.barsPerRow,
    containerWidth: containerW,
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
  const cands = selCtx ? findFretCandidatesForMidi(track, selCtx.midi, 0, 24) : [];
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
    accMode: getAccMode(),
    fretCount: 17
  });
  const metroPanelHtml = studioUi.metroPanelOpen
    ? renderMetronomeBlocksHtml(studioUi.song.metronomePattern, curMeasure?.numerator || 4, {
        playingBeatCol: studioUi.lastPlayPos.active ? (studioUi.lastPlayPos.metroBeatCol ?? -1) : -1
      })
    : '';

  const micMode = micCtrl.getMode();
  const sensPct = Math.round((studioUi.studioPrefs.detectSensitivity || 0.58) * 100);
  const minDurMs = studioUi.studioPrefs.minNoteDurationMs || 80;
  const recMode = studioUi.studioPrefs.liveRecMode || 'offline';
  const gridVal = studioUi.studioPrefs.quantizeGrid ?? 1;
  const sfStatus = soundFontSynth.getStatus();
  const sfEnabled = studioUi.studioPrefs.soundMode === 'sf2' && sfStatus.loaded;
  const paStats = micCtrl.getPlayAlongStats();
  const phrasePositions = (track && track.instrumentType !== 'drums')
    ? (studioUi.phrasePositions.length > 0 ? studioUi.phrasePositions : analyzePhrasePositions(track, null, { boxSpan: 4, maxOptions: 5 }))
    : [];
  studioUi.phrasePositions = phrasePositions;

  const candPopoverHtml = (selCtx && cands.length > 0)
    ? `<div style="display:flex;align-items:center;gap:5px;flex-wrap:wrap;">
         <span class="studio-mic-hint" style="font-style:normal;font-weight:800;color:#fbbf24;">Pozycja (${escapeHtml(midiToNoteName(selCtx.midi, getAccMode()))}):</span>
         ${cands.map(c => {
           const active = c.string === selCtx.string && c.fret === selCtx.fret;
           const nStr = (track.openStringMidis?.length || 6) - c.string;
           return `<button type="button" class="studio-cand-pill ${active ? 'active' : ''}" data-action="pick-note-cand" data-cand-string="${c.string}" data-cand-fret="${c.fret}" data-tip-name="Pozycja S${nStr}:P${c.fret}" data-tip-desc="Przenosi wybraną nutę na strunę ${nStr}, próg ${c.fret} bez zmiany wysokości dźwięku.">S${nStr}:P${c.fret}</button>`;
         }).join('')}
       </div>`
    : '';

  const phrasePosHtml = phrasePositions.length > 0
    ? `<div style="display:flex;align-items:center;gap:5px;flex-wrap:wrap;margin-left:auto;">
         <span class="studio-mic-hint" style="font-style:normal;font-weight:700;">Układ fraz:</span>
         ${phrasePositions.map((pos, idx) =>
           `<button type="button" class="studio-cand-pill ${studioUi.selectedPhrasePosIdx === idx ? 'active' : ''}" data-action="pick-phrase-pos" data-pos-idx="${idx}" data-tip-name="${escapeHtml(pos.label)}" data-tip-desc="Przepisuje nagraną lub edytowaną frazę w zakres progów ${pos.minFret}–${pos.maxFret} z zachowaniem wysokości MIDI.">${escapeHtml(pos.label)}</button>`
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
             <label class="studio-mic-slider-wrap" data-tip-name="Czułość detekcji melodii" data-tip-desc="Niezależny próg Studia TAB (nie zmienia Stroika ani ćwiczeń). Wyżej = mniej przypadkowych nut i szumu.">
               <span>Czułość</span>
               <input id="studioSensitivitySlider" type="range" min="0.36" max="0.88" step="0.02" value="${studioUi.studioPrefs.detectSensitivity}" data-action="studio-sensitivity" style="width:80px;">
               <strong id="studioSensitivityVal">${sensPct}%</strong>
             </label>
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

  const matrixHtml = renderArrangerMatrixHtml(studioUi.song, {
    activeTrackIdx: studioUi.activeTrackIdx,
    cursor: studioUi.cursor,
    playhead: studioUi.lastPlayPos,
    loopEnabled: studioUi.loopEnabled,
    loopStart: studioUi.loopStart,
    loopEnd: studioUi.loopEnd,
    selectionStart: studioUi.selectionStart,
    selectionEnd: studioUi.selectionEnd
  });

  const meterStatusHtml = meterInfo
    ? `<span class="studio-pill-badge" data-tip-name="Kontrola metrum taktu" data-tip-desc="Suma wartości rytmicznych w bieżącym takcie względem jego metrum." style="border-color:${meterInfo.status === 'ok' ? '#22c55e' : (meterInfo.status === 'under' ? '#eab308' : '#ef4444')};">
         Metrum T.${studioUi.cursor.measureIdx + 1}: <strong>${meterInfo.filled.toFixed(2).replace(/\.00$/, '')}/${meterInfo.capacity}</strong> ${meterInfo.status === 'ok' ? '✓' : (meterInfo.status === 'under' ? '(niepełny)' : '(nadmiar)')}
       </span>`
    : '';

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

        <div class="studio-group">
          <button type="button" class="studio-btn sm ${studioUi.loopEnabled ? 'active' : ''}" data-action="toggle-loop" data-tip-name="Pętla A–B" data-tip-desc="Zapętla odtwarzanie pomiędzy taktami A i B." data-tip-kbd="F9">🔁 A–B</button>
          <button type="button" class="studio-btn sm" data-action="set-loop-a" data-tip-name="Początek pętli (A)" data-tip-desc="Ustawia początek pętli na bieżącym takcie." data-tip-kbd="[">A:T.${studioUi.loopStart + 1}</button>
          <button type="button" class="studio-btn sm" data-action="set-loop-b" data-tip-name="Koniec pętli (B)" data-tip-desc="Ustawia koniec pętli na bieżącym takcie." data-tip-kbd="]">B:T.${studioUi.loopEnd + 1}</button>
        </div>

        <div class="studio-group" style="border-right:none;padding-right:0;">
          ${SPEED_PRESETS.map(pct =>
            `<button type="button" class="studio-btn sm studio-speed-pill ${studioUi.speedPercent === pct ? 'active' : ''}" data-action="set-speed" data-speed="${pct}" data-tip-name="Prędkość ${pct}%" data-tip-desc="Skaluje tempo ćwiczenia do ${pct}% wartości bazowej.">${pct}%</button>`
          ).join('')}
          <input id="studioSpeedSlider" type="range" min="25" max="150" step="5" value="${studioUi.speedPercent}" data-action="speed-slider" data-tip-name="Suwak tempa (25–150%)" data-tip-desc="Płynna zmiana prędkości odtwarzania bez zmiany wysokości dźwięku." style="width:72px;">
          <span id="studioEffectiveBpmLabel" class="studio-pill-badge" style="padding:3px 7px;font-size:11px;"><strong>${snap.effectiveBpm} BPM (${studioUi.speedPercent}%)</strong></span>
          <button type="button" class="studio-btn sm ${studioUi.autoTempoEnabled ? 'active' : ''}" data-action="toggle-autotempo" data-tip-name="Auto-Tempo" data-tip-desc="Automatycznie zwiększa tempo o +${studioUi.autoTempoStep} BPM po każdym przejściu pętli A–B.">⚡ Auto</button>
        </div>
        <span id="studioToastMsg" class="studio-toast" style="display:${studioUi.statusMsg ? 'inline-flex' : 'none'};">${escapeHtml(studioUi.statusMsg)}</span>
      </div>

      <div class="studio-zone-a-row">
        <div class="studio-group">
          <select data-action="set-timesig" data-tip-name="Metrum taktu" data-tip-desc="Ustawia metrum (np. 4/4, 3/4, 6/8) od bieżącego taktu.">${timeSigOptions}</select>
          <button type="button" class="studio-btn sm ${studioUi.viewMode === 'tab' ? 'active' : ''}" data-action="set-view-mode" data-mode="tab" data-tip-name="Widok TAB" data-tip-desc="Wyświetla samą tabulaturę gitarową/basową.">🎸 TAB</button>
          <button type="button" class="studio-btn sm ${studioUi.viewMode === 'staff' ? 'active' : ''}" data-action="set-view-mode" data-mode="staff" data-tip-name="Widok Nuty (pięciolinia)" data-tip-desc="Przejrzysta pięciolinia bez TAB (jak w zakładce Ćwiczenie). Przy kilku instrumentach włącza widok całego zespołu naraz.">🎼 Nuty</button>
          <button type="button" class="studio-btn sm ${studioUi.viewMode === 'staff_tab' ? 'active' : ''}" data-action="set-view-mode" data-mode="staff_tab" data-tip-name="Widok Nuty + TAB" data-tip-desc="Wyświetla pięciolinię w kluczu wiolinowym/basowym nad tabulaturą.">🎼 Nuty+TAB</button>
          <button type="button" class="studio-btn sm ${studioUi.scoreAllTracks ? 'active' : ''}" data-action="toggle-all-tracks-score" data-tip-name="Wszystkie instrumenty naraz" data-tip-desc="Wyświetla zsynchronizowane takty wszystkich ścieżek zespołu jedna pod drugą.">👥 Wszystkie</button>
          <button type="button" class="studio-btn sm ${studioUi.showScoreNoteNames ? 'active' : ''}" data-action="toggle-score-note-names" data-tip-name="Nazwy nut (A–G)" data-tip-desc="Pokazuje czytelne nazwy dźwięków nad pięciolinią.">ABC</button>
          <select data-action="set-bars-per-row" data-tip-name="Takty w wierszu" data-tip-desc="Liczba taktów przypadająca na jeden wiersz partytury.">
            <option value="auto" ${studioUi.barsPerRow === 'auto' ? 'selected' : ''}>Wiersz: Auto</option>
            <option value="2" ${String(studioUi.barsPerRow) === '2' ? 'selected' : ''}>2 takty</option>
            <option value="4" ${String(studioUi.barsPerRow) === '4' ? 'selected' : ''}>4 takty</option>
            <option value="6" ${String(studioUi.barsPerRow) === '6' ? 'selected' : ''}>6 taktów</option>
          </select>
          <button type="button" class="studio-btn sm" data-action="detect-key" data-tip-name="Wykryj tonację" data-tip-desc="Analizuje zapisane nuty algorytmem Krumhansla-Schmucklera i ustawia skalę gryfu.">🔍 Tonacja</button>
        </div>

        <div class="studio-group" style="border-right:none;padding-right:0;">
          <select data-action="load-demo" data-tip-name="Projekt demo" data-tip-desc="Wczytuje jeden z gotowych wielościeżkowych utworów demonstracyjnych.">
            <option value="rock_riff_em" ${studioUi.demoId === 'rock_riff_em' ? 'selected' : ''}>Demo: Neon Riff (Em)</option>
            <option value="blues_shuffle_a" ${studioUi.demoId === 'blues_shuffle_a' ? 'selected' : ''}>Demo: Blues Shuffle (A)</option>
          </select>
          <button type="button" class="studio-btn sm" data-action="new-song" data-tip-name="Nowy projekt" data-tip-desc="Tworzy czysty projekt z jedną ścieżką gitary i perkusją.">📄 Nowy</button>
          <button type="button" class="studio-btn sm" data-action="export-json" data-tip-name="Zapisz .gitara.json" data-tip-desc="Pobiera pełny projekt Studia TAB w formacie JSON.">💾 .JSON</button>
          <button type="button" class="studio-btn sm" data-action="import-json-trigger" data-tip-name="Wczytaj .gitara.json" data-tip-desc="Otwiera zapisany wcześniej projekt z dysku.">📂 Wczytaj</button>
          <button type="button" class="studio-btn sm" data-action="export-gp5" data-tip-name="Eksportuj Guitar Pro 5" data-tip-desc="Generuje binarny plik .GP5 ze wszystkimi ścieżkami, technikami, markerami i perkusją GM.">⬇ .GP5</button>
          <button type="button" class="studio-btn sm" data-action="export-mid" data-tip-name="Eksportuj Standard MIDI" data-tip-desc="Generuje wielościeżkowy plik .MID (Format 1) z tempem, tonacją i kanałem perkusji 10.">⬇ .MID</button>
          <button type="button" class="studio-btn sm" data-action="import-sf2-trigger" data-tip-name="Wczytaj SoundFont2 (.sf2)" data-tip-desc="Ładuje lokalny bank brzmień .sf2 (bank 0 + perkusja 128) do odtwarzania offline w WebAudio.">🎹 ${sfStatus.loaded ? escapeHtml(sfStatus.name.slice(0, 14)) : '.SF2'}</button>
          <div class="studio-engine-toggle" data-tip-name="Silnik dźwięku" data-tip-desc="Przełącza syntezę pomiędzy wbudowanym WebAudio a wczytanym bankiem SoundFont2 (.sf2).">
            <button type="button" class="${!sfEnabled ? 'active' : ''}" data-action="set-sound-mode" data-mode="webaudio">WebAudio</button>
            <button type="button" class="${sfEnabled ? 'active' : ''}" data-action="set-sound-mode" data-mode="sf2" ${sfStatus.loaded ? '' : 'disabled'}>.SF2</button>
          </div>
          <input id="studioJsonFileInput" type="file" accept=".json,.gitara.json,application/json" style="display:none;" data-action="import-json-file">
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
          ${activeSel ? `<span class="studio-sel-pill">Zaznaczenie: <strong>T.${activeSel.startMeasure + 1}·B.${activeSel.start.beatIdx + 1} – T.${activeSel.endMeasure + 1}·B.${activeSel.end.beatIdx + 1}</strong> (${activeSel.beatCount} ud.)<button type="button" class="studio-sel-clear" data-action="clear-selection" title="Wyczyść zaznaczenie (Esc)">✕</button></span>` : ''}
          ${meterStatusHtml}
          <span class="studio-pill-badge">Kursor: <strong>T.${studioUi.cursor.measureIdx + 1} · B.${studioUi.cursor.beatIdx + 1}</strong></span>
        </div>
      </div>
      <div class="studio-tab-scroll ${studioUi.scoreAllTracks ? 'multitrack' : ''}" id="studioTabScrollWrap">${tabSvg}</div>
    </div>

    <!-- Strefa E: Animowany kolorowy gryf + mikrofon -->
    <div class="studio-card">
      <div class="studio-zone-e-head">
        <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
          <span class="section-label" style="margin:0;">GRYF (STREFA E)</span>
          <button type="button" class="studio-btn sm ${studioUi.fretInputMode === 'edit' ? 'active' : ''}" data-action="set-fret-mode" data-mode="edit" data-tip-name="Tryb gryfu: Wpisuj" data-tip-desc="Kliknięcie progu na gryfie wpisuje nutę w aktualnym slocie i ją odsłuchuje.">✏️ Wpisuj</button>
          <button type="button" class="studio-btn sm ${studioUi.fretInputMode === 'audition' ? 'active' : ''}" data-action="set-fret-mode" data-mode="audition" data-tip-name="Tryb gryfu: Tylko odsłuch" data-tip-desc="Kliknięcie progu jedynie odsłuchuje dźwięk bez modyfikacji partytury.">🔊 Odsłuch</button>
          <button type="button" class="studio-btn sm ${studioUi.showScaleHints ? 'active' : ''}" data-action="toggle-scale-hints" data-tip-name="Podpowiedzi skali na gryfie" data-tip-desc="Pokazuje na gryfie stopnie wybranej tonacji utworu.">Skala: ${studioUi.showScaleHints ? 'Wł.' : 'Wył.'}</button>
        </div>
        <div class="studio-legend">
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;background:#f39237;display:inline-block;"></span>Tonika</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;background:#22c55e;display:inline-block;"></span>Tercja/Mic</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;border:2px solid #f59e0b;display:inline-block;"></span>Kandydaci</span>
          <span class="studio-legend-item"><span style="width:9px;height:9px;border-radius:99px;border:2px dashed #38bdf8;display:inline-block;"></span>Następne</span>
        </div>
      </div>
      <div id="studioFretboardMount" class="studio-fretboard-wrap">${fbContent}</div>
      ${micPanelHtml}
    </div>

    <!-- Strefa D: Dolna matryca aranżacji Ścieżki × Takty -->
    <div class="studio-card">${matrixHtml}</div>
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

function triggerJsonDownload() {
  const json = serializeStudioSong(studioUi.song, studioUi.studioPrefs);
  const blob = new Blob([json], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  const safeTitle = (studioUi.song.title || 'projekt').toLowerCase().replace(/[^a-z0-9_-]+/gi, '_');
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
      case 'clear-selection':
        studioUi.beatSelection = null;
        studioUi.selectionStart = null;
        studioUi.selectionEnd = null;
        renderAllStudio();
        return;
      case 'toggle-scale-hints':
        studioUi.showScaleHints = !studioUi.showScaleHints;
        updateLiveFretboard();
        renderAllStudio();
        return;
      case 'new-song':
        player.stop();
        pushUndoSnapshot();
        studioUi.song = createEmptySong({ title: 'Nowy projekt', bpm: 120, measureCount: 4 });
        studioUi.activeTrackIdx = 0;
        studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
        studioUi.beatSelection = null;
        studioUi.selectionStart = null;
        studioUi.selectionEnd = null;
        studioUi.lastPlacedNote = null;
        studioUi.positionAreaRange = null;
        studioUi.phrasePositions = [];
        studioUi.selectedPhrasePosIdx = -1;
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
          reloadPlayerFromSong();
          renderAllStudio();
        }
        return;
      }
      case 'redo': {
        const next = studioUi.history.redo(studioUi.song);
        if (next) {
          studioUi.song = next;
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
        const inserted = addMeasure(studioUi.song, studioUi.cursor.measureIdx);
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
      const val = Math.max(0.36, Math.min(0.88, Math.round(Number(actionEl.value) * 100) / 100));
      studioUi.studioPrefs.detectSensitivity = val;
      studioUi.studioPrefs.detectHighThresholdOn = Math.max(0.32, Math.round((val - 0.16) * 100) / 100);
      const valEl = document.getElementById('studioSensitivityVal');
      if (valEl) valEl.textContent = `${Math.round(val * 100)}%`;
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
    if (action === 'load-demo') {
      player.stop();
      pushUndoSnapshot();
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
      file.text().then(text => {
        try {
          const { song, studioPrefs } = deserializeStudioSong(text);
          player.stop();
          pushUndoSnapshot();
          studioUi.song = song;
          studioUi.studioPrefs = studioPrefs;
          persistPrefsToHost();
          studioUi.activeTrackIdx = 0;
          studioUi.cursor = { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
          studioUi.beatSelection = null;
          studioUi.selectionStart = null;
          studioUi.selectionEnd = null;
          reloadPlayerFromSong();
          showToast(`Wczytano „${song.title}”`);
          renderAllStudio();
        } catch (err) {
          showToast(err.message || 'Błąd wczytywania pliku JSON');
        }
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
      studioUi.activeTrackIdx = tIdx;
      studioUi.beatSelection = {
        trackIdx: tIdx,
        anchor,
        focus: { measureIdx: mIdx, beatIdx: bIdx },
        fullMeasures: false
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
    const tipTarget = e.target?.closest?.('[data-tip-name]');
    if (!tipTarget) {
      if (hoverTipTarget) hideHoverTip();
      return;
    }
    if (tipTarget !== hoverTipTarget) showHoverTipFor(tipTarget);
  });
  el.addEventListener('mouseout', e => {
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
      studioUi.activeTrackIdx = tabDragSel.trackIdx;
      studioUi.beatSelection = {
        trackIdx: tabDragSel.trackIdx,
        anchor: tabDragSel.anchor,
        focus: tabDragSel.focus,
        fullMeasures: false
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
  if (opts.replaceSong) {
    studioUi.song = createEmptySong({
      title: opts.title || 'Importowane ćwiczenie',
      bpm: Number(opts.bpm) || studioUi.song.tempoBpm || 110,
      numerator: num,
      denominator: den,
      measureCount: measures.length
    });
    if (Array.isArray(opts.tuningMidis) && opts.tuningMidis.length >= 4) {
      studioUi.song.tracks[0].openStringMidis = opts.tuningMidis.slice();
    }
    studioUi.song.tracks[0].measures = measures;
    studioUi.song.tracks[1].measures = measures.map(() => createEmptyMeasure(num, den, 'rock_8th', true));
    studioUi.activeTrackIdx = 0;
  } else {
    const tr = studioUi.song.tracks[studioUi.activeTrackIdx];
    if (tr && tr.instrumentType !== 'drums') {
      if (Array.isArray(opts.tuningMidis) && opts.tuningMidis.length === (tr.openStringMidis?.length || 6)) {
        tr.openStringMidis = opts.tuningMidis.slice();
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
    getState: () => studioUi
  };
}

export { studioUi, render, stopPlayback, handleKeyDown };
