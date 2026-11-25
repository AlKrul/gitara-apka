/**
 * Kontroler wejść i skrótów klawiaturowych dla 🎼 Studio TAB (Faza 2):
 *  - Bufor dwucyfrowego wpisywania progów 0–24 w oknie 450 ms
 *  - Pełna mapa skrótów z docs/PLAN_STUDIO_TAB.md (Sekcja 5)
 *  - Obsługa kliknięć gryfu (edycja / odsłuch), dolnej klawiatury #midiFooter oraz palety Strefy B
 */
import {
  addMeasure,
  addMeasureBefore,
  analyzePhrasePositions,
  applyBeatPropToSelection,
  applyEffectToSelection,
  applyPhrasePosition,
  clampStudioPrefs,
  clearMeasureAt,
  copyBeatsToClipboard,
  copyMeasuresToClipboard,
  createBeat,
  createEmptyMeasure,
  createTabNote,
  cutBeatsToClipboard,
  cutMeasuresToClipboard,
  cycleBeatDots,
  deleteBeatAt,
  deleteNoteAt,
  deleteSelectionNotes,
  duplicateMeasures,
  findFretCandidatesForMidi,
  getMasterMeasureCount,
  getNoteAt,
  insertBeatAfter,
  insertMidiNoteAt,
  moveBeatInMeasure,
  moveNoteToAdjacentString,
  moveSelectionToAdjacentString,
  normalizeBeatSelection,
  noteToMidi,
  pasteBeatsFromClipboard,
  pasteMeasuresFromClipboard,
  removeMeasure,
  setBeatDuration,
  setBeatDynamic,
  setBeatRest,
  setBeatTuplet,
  setMeasureRepeatClose,
  setMeasureSectionMarker,
  setNoteAt,
  setNotePosition,
  shiftNotePitch,
  shiftSelectionPitch,
  stepBeatDuration,
  toggleBeatProp,
  toggleMeasureRepeatOpen,
  toggleMeasureVolta,
  toggleNoteEffect,
  transposeRange
} from './studioModel.js';
import { GuitarEventTracker, analyzePitchPlease } from '../dsp/pitchPleaseBridge.js';
import { computeActiveAndNextNotes } from './studioPlayer.js';
import { filterStudioCommands } from './studioPalette.js';

export function createFretInputBuffer(windowMs = 450, nowFn = null) {
  const getNow = typeof nowFn === 'function'
    ? nowFn
    : () => (typeof performance !== 'undefined' ? performance.now() : Date.now());
  let lastDigit = null;
  let lastTimeMs = 0;
  let lastSlotKey = '';

  return {
    pushDigit(digit, slotKey = '') {
      const d = Math.max(0, Math.min(9, Math.floor(Number(digit))));
      const now = getNow();
      let fret = d;
      if (lastDigit !== null && lastSlotKey === slotKey && (now - lastTimeMs) <= windowMs) {
        const combined = lastDigit * 10 + d;
        if (combined <= 24) {
          fret = combined;
          // Po złożeniu pełnej liczby dwucyfrowej kolejna cyfra zacznie od nowa
          lastDigit = null;
          lastTimeMs = 0;
          lastSlotKey = '';
          return fret;
        }
      }
      lastDigit = d;
      lastTimeMs = now;
      lastSlotKey = slotKey;
      return fret;
    },
    reset() {
      lastDigit = null;
      lastTimeMs = 0;
      lastSlotKey = '';
    }
  };
}

export function createStudioInputController(deps) {
  const fretBuf = createFretInputBuffer(450, deps.nowFn);

  function getActiveBeatSelection(ui) {
    return normalizeBeatSelection(ui.song, ui);
  }

  function clearAllSelection(ui) {
    if (!ui) return;
    ui.beatSelection = null;
    ui.selectionStart = null;
    ui.selectionEnd = null;
  }

  function getSelectionRange(ui) {
    const sel = getActiveBeatSelection(ui);
    if (sel) {
      return { startIdx: sel.startMeasure, endIdx: sel.endMeasure };
    }
    const maxM = Math.max(0, getMasterMeasureCount(ui.song) - 1);
    const cur = Math.max(0, Math.min(maxM, ui.cursor.measureIdx || 0));
    return { startIdx: cur, endIdx: cur };
  }

  function moveCursorNext(addBarAtEnd = false) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr) return;
    const maxM = Math.max(0, tr.measures.length - 1);
    const beatsLen = tr.measures[ui.cursor.measureIdx]?.beats?.length || 1;
    if (ui.cursor.beatIdx + 1 < beatsLen) {
      ui.cursor.beatIdx++;
    } else if (ui.cursor.measureIdx < maxM) {
      ui.cursor.measureIdx++;
      ui.cursor.beatIdx = 0;
    } else if (addBarAtEnd) {
      deps.pushUndo();
      const inserted = addMeasure(ui.song, ui.cursor.measureIdx);
      if (inserted >= 0) {
        ui.cursor.measureIdx = inserted;
        ui.cursor.beatIdx = 0;
        deps.onSongChanged(false);
        deps.onToast?.(`Dodano takt ${inserted + 1}`);
      }
    }
    fretBuf.reset();
    deps.onSeek?.(ui.cursor.measureIdx, ui.cursor.beatIdx);
    deps.onRender();
  }

  function moveCursorPrev() {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr) return;
    if (ui.cursor.beatIdx > 0) {
      ui.cursor.beatIdx--;
    } else if (ui.cursor.measureIdx > 0) {
      ui.cursor.measureIdx--;
      const prevBeats = tr.measures[ui.cursor.measureIdx]?.beats?.length || 1;
      ui.cursor.beatIdx = Math.max(0, prevBeats - 1);
    }
    fretBuf.reset();
    deps.onSeek?.(ui.cursor.measureIdx, ui.cursor.beatIdx);
    deps.onRender();
  }

  function openInlinePrompt(type) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    const m = tr?.measures?.[ui.cursor.measureIdx];
    const b = m?.beats?.[ui.cursor.beatIdx];
    let initVal = '';
    if (type === 'section') initVal = m?.sectionMarker || '';
    else if (type === 'text') initVal = b?.text || '';
    else if (type === 'chord') initVal = b?.chordSymbol || '';
    else if (type === 'transpose') initVal = '0';
    ui.inlinePrompt = { type, value: initVal };
    deps.onRender();
  }

  function submitInlinePrompt(rawVal) {
    const ui = deps.getState();
    const prompt = ui.inlinePrompt;
    if (!prompt) return false;
    const val = rawVal !== undefined ? rawVal : prompt.value;
    const mIdx = ui.cursor.measureIdx;
    const bIdx = ui.cursor.beatIdx;

    if (prompt.type === 'section') {
      deps.pushUndo();
      setMeasureSectionMarker(ui.song, mIdx, val);
      deps.onSongChanged(true);
    } else if (prompt.type === 'text') {
      deps.pushUndo();
      toggleBeatProp(ui.song, ui.activeTrackIdx, mIdx, bIdx, 'text', val);
      deps.onSongChanged(true);
    } else if (prompt.type === 'chord') {
      deps.pushUndo();
      toggleBeatProp(ui.song, ui.activeTrackIdx, mIdx, bIdx, 'chordSymbol', val);
      deps.onSongChanged(true);
    } else if (prompt.type === 'repeatClose') {
      deps.pushUndo();
      setMeasureRepeatClose(ui.song, mIdx, Number(val) || 0);
      deps.onSongChanged(false);
    } else if (prompt.type === 'transpose') {
      const semis = Math.max(-24, Math.min(24, Math.round(Number(val) || 0)));
      if (semis !== 0) {
        const sel = getActiveBeatSelection(ui);
        deps.pushUndo();
        let cnt = 0;
        if (sel && sel.kind === 'beats' && ui.clipboardScope !== 'all') {
          cnt = shiftSelectionPitch(ui.song, ui.activeTrackIdx, sel, semis).beatsChanged;
        } else {
          const range = getSelectionRange(ui);
          cnt = transposeRange(ui.song, {
            trackIdx: ui.activeTrackIdx,
            allTracks: ui.clipboardScope === 'all',
            startMeasure: range.startIdx,
            endMeasure: range.endIdx,
            semitones: semis
          });
        }
        deps.onSongChanged(true);
        deps.onToast?.(`Przetransponowano (${cnt} uderzeń) o ${semis > 0 ? '+' : ''}${semis} półt.`);
      }
    }
    ui.inlinePrompt = null;
    deps.onRender();
    return true;
  }

  function cancelInlinePrompt() {
    const ui = deps.getState();
    if (!ui.inlinePrompt) return false;
    ui.inlinePrompt = null;
    deps.onRender();
    return true;
  }

  function executeCommand(cmdId, arg = undefined) {
    const ui = deps.getState();
    const tIdx = ui.activeTrackIdx;
    const mIdx = ui.cursor.measureIdx;
    const bIdx = ui.cursor.beatIdx;
    const sIdx = ui.cursor.stringIdx;
    const tr = ui.song.tracks[tIdx];
    if (!tr) return false;

    const sel = getActiveBeatSelection(ui);
    const hasMultiBeatSel = Boolean(sel && sel.beatCount > 1);

    if (cmdId.startsWith('dur:')) {
      const dur = Number(cmdId.slice(4));
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'duration', dur);
      else setBeatDuration(ui.song, tIdx, mIdx, bIdx, dur);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('dot:')) {
      const d = Number(cmdId.slice(4));
      const exactDot = Number.isFinite(d) ? d : null;
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'dots', exactDot);
      else cycleBeatDots(ui.song, tIdx, mIdx, bIdx, exactDot);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'rest') {
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'rest');
      else setBeatRest(ui.song, tIdx, mIdx, bIdx);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'edit:insertBeat') {
      const curBeat = tr.measures?.[mIdx]?.beats?.[bIdx];
      deps.pushUndo();
      const pos = insertBeatAfter(ui.song, tIdx, mIdx, bIdx, curBeat?.duration ?? 0, true);
      if (pos >= 0) ui.cursor.beatIdx = pos;
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('tuplet:')) {
      const [, nStr, dStr] = cmdId.split(':');
      const num = Number(nStr) || 3;
      const den = Number(dStr) || 2;
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'tuplet', { num, den });
      else setBeatTuplet(ui.song, tIdx, mIdx, bIdx, num, den);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('effect:')) {
      const eff = cmdId.slice(7);
      deps.pushUndo();
      if (hasMultiBeatSel) applyEffectToSelection(ui.song, tIdx, sel, eff);
      else toggleNoteEffect(ui.song, tIdx, mIdx, bIdx, sIdx, eff);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('harmonic:')) {
      const hType = Number(cmdId.slice(9)) || 1;
      deps.pushUndo();
      if (hasMultiBeatSel) applyEffectToSelection(ui.song, tIdx, sel, 'harmonicType', hType);
      else toggleNoteEffect(ui.song, tIdx, mIdx, bIdx, sIdx, 'harmonicType', hType);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('slide:')) {
      const sType = cmdId.slice(6) || 'shift';
      deps.pushUndo();
      if (hasMultiBeatSel) applyEffectToSelection(ui.song, tIdx, sel, 'slideType', sType);
      else toggleNoteEffect(ui.song, tIdx, mIdx, bIdx, sIdx, 'slideType', sType);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('bend:')) {
      const preset = cmdId.slice(5) || 'full';
      deps.pushUndo();
      if (hasMultiBeatSel) applyEffectToSelection(ui.song, tIdx, sel, 'bendPreset', preset);
      else toggleNoteEffect(ui.song, tIdx, mIdx, bIdx, sIdx, 'bendPreset', preset);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('pick:')) {
      const pVal = Number(cmdId.slice(5)) || 1;
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'pickStroke', pVal);
      else toggleBeatProp(ui.song, tIdx, mIdx, bIdx, 'pickStroke', pVal);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('brush:')) {
      const bVal = cmdId.slice(6) || 'down';
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'brushStroke', bVal);
      else toggleBeatProp(ui.song, tIdx, mIdx, bIdx, 'brushStroke', bVal);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'beat:fermata') {
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'fermata');
      else toggleBeatProp(ui.song, tIdx, mIdx, bIdx, 'fermata');
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'beat:dynamic') {
      const dynVal = Number(arg) || 6;
      deps.pushUndo();
      if (hasMultiBeatSel) applyBeatPropToSelection(ui.song, tIdx, sel, 'dynamic', dynVal);
      else setBeatDynamic(ui.song, tIdx, mIdx, bIdx, dynVal);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'struct:repeatOpen') {
      deps.pushUndo();
      toggleMeasureRepeatOpen(ui.song, mIdx);
      deps.onSongChanged(false);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('struct:volta:')) {
      const vNum = Number(cmdId.slice(13)) || 1;
      deps.pushUndo();
      toggleMeasureVolta(ui.song, mIdx, vNum);
      deps.onSongChanged(false);
      deps.onRender();
      return true;
    }
    if (cmdId === 'struct:addBefore') {
      deps.pushUndo();
      const pos = addMeasureBefore(ui.song, mIdx);
      if (pos >= 0) {
        ui.cursor.measureIdx = pos;
        ui.cursor.beatIdx = 0;
      }
      deps.onSongChanged(false);
      deps.onRender();
      return true;
    }
    if (cmdId === 'struct:addAfter') {
      deps.pushUndo();
      const pos = addMeasure(ui.song, mIdx);
      if (pos >= 0) {
        ui.cursor.measureIdx = pos;
        ui.cursor.beatIdx = 0;
      }
      deps.onSongChanged(false);
      deps.onRender();
      return true;
    }
    if (cmdId.startsWith('prompt:')) {
      openInlinePrompt(cmdId.slice(7));
      return true;
    }
    if (cmdId === 'transform:strUp' || cmdId === 'transform:strDown') {
      const dir = cmdId === 'transform:strUp' ? 1 : -1;
      deps.pushUndo();
      if (hasMultiBeatSel) {
        const res = moveSelectionToAdjacentString(ui.song, tIdx, sel, dir);
        if (res.movedCount > 0) {
          deps.onSongChanged(true);
          deps.onToast?.(`Przeniesiono ${res.movedCount} nut na sąsiednią strunę`);
          deps.onRender();
        } else {
          deps.onToast?.('Nie można przenieść zaznaczonych nut na sąsiednią strunę');
        }
        return true;
      }
      const res = moveNoteToAdjacentString(ui.song, tIdx, mIdx, bIdx, sIdx, dir);
      if (res.moved) {
        ui.cursor.stringIdx = res.newString;
        deps.onSongChanged(true);
        deps.onAuditionBeat?.(tr, tr.measures[mIdx]?.beats?.[bIdx]);
        deps.onRender();
      } else {
        deps.onToast?.('Nie można przenieść nuty na sąsiednią strunę (poza skalą progów lub struna zajęta)');
      }
      return true;
    }
    if (cmdId === 'transform:semiUp' || cmdId === 'transform:semiDown' ||
        cmdId === 'transform:octUp' || cmdId === 'transform:octDown') {
      const semiMap = {
        'transform:semiUp': 1,
        'transform:semiDown': -1,
        'transform:octUp': 12,
        'transform:octDown': -12
      };
      const semis = semiMap[cmdId];
      deps.pushUndo();
      if (sel) {
        const res = shiftSelectionPitch(ui.song, tIdx, sel, semis, { allTracks: false });
        if (res.beatsChanged > 0) {
          deps.onSongChanged(true);
          deps.onToast?.(`Przesunięto zaznaczenie (${res.beatsChanged} ud.) o ${semis > 0 ? '+' : ''}${semis} półt.`);
          deps.onRender();
        } else {
          deps.onToast?.('Brak nut do przesunięcia w zaznaczeniu');
        }
        return true;
      }
      if (shiftNotePitch(ui.song, tIdx, mIdx, bIdx, sIdx, semis)) {
        deps.onSongChanged(true);
        deps.onAuditionBeat?.(tr, tr.measures[mIdx]?.beats?.[bIdx]);
        deps.onRender();
      }
      return true;
    }
    if (cmdId === 'transform:moveBeatLeft' || cmdId === 'transform:moveBeatRight') {
      const delta = cmdId === 'transform:moveBeatLeft' ? -1 : 1;
      deps.pushUndo();
      ui.cursor.beatIdx = moveBeatInMeasure(ui.song, tIdx, mIdx, bIdx, delta);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'clip:copy' || cmdId === 'clip:cut') {
      const scope = arg || ui.clipboardScope || 'track';
      if (sel && sel.kind === 'beats' && scope !== 'all') {
        if (cmdId === 'clip:cut') {
          deps.pushUndo();
          ui.clipboard = cutBeatsToClipboard(ui.song, { trackIdx: tIdx, selection: sel });
          deps.onSongChanged(true);
        } else {
          ui.clipboard = copyBeatsToClipboard(ui.song, { trackIdx: tIdx, selection: sel });
        }
        const nBeats = ui.clipboard?.beatCount || 0;
        deps.onToast?.(`${cmdId === 'clip:cut' ? 'Wycięto' : 'Skopiowano'} ${nBeats} uderzeń`);
        deps.onRender();
        return true;
      }
      const range = getSelectionRange(ui);
      if (cmdId === 'clip:cut') {
        deps.pushUndo();
        ui.clipboard = cutMeasuresToClipboard(ui.song, { ...range, scope, activeTrackIdx: tIdx });
        deps.onSongChanged(true);
      } else {
        ui.clipboard = copyMeasuresToClipboard(ui.song, { ...range, scope, activeTrackIdx: tIdx });
      }
      const nBars = ui.clipboard?.barCount || 0;
      deps.onToast?.(`${cmdId === 'clip:cut' ? 'Wycięto' : 'Skopiowano'} ${nBars} takt(ów) (${scope === 'all' ? 'wszystkie ścieżki' : '1 ścieżka'})`);
      deps.onRender();
      return true;
    }
    if (cmdId === 'clip:paste') {
      if (!ui.clipboard) {
        deps.onToast?.('Schowek jest pusty');
        return false;
      }
      const mode = arg || ui.clipboardPasteMode || 'overwrite';
      deps.pushUndo();
      if (ui.clipboard.kind === 'beats') {
        const res = pasteBeatsFromClipboard(ui.song, ui.clipboard, {
          targetTrackIdx: tIdx,
          targetMeasureIdx: mIdx,
          targetBeatIdx: bIdx,
          mode
        });
        deps.onSongChanged(false);
        deps.onToast?.(`Wklejono ${res?.pastedCount || 0} uderzeń (${mode === 'insert' ? 'wstawiono' : 'nadpisano'})`);
        deps.onRender();
        return true;
      }
      pasteMeasuresFromClipboard(ui.song, ui.clipboard, {
        targetMeasureIdx: mIdx,
        mode,
        scope: ui.clipboardScope || ui.clipboard.scope,
        activeTrackIdx: tIdx
      });
      deps.onSongChanged(false);
      deps.onToast?.(`Wklejono (${mode === 'insert' ? 'wstawiono nowe takty' : 'nadpisano'})`);
      deps.onRender();
      return true;
    }
    if (cmdId === 'clip:dup1' || cmdId === 'clip:dup2' || cmdId === 'clip:dup4') {
      const reps = cmdId === 'clip:dup4' ? 4 : (cmdId === 'clip:dup2' ? 2 : 1);
      const range = getSelectionRange(ui);
      deps.pushUndo();
      duplicateMeasures(ui.song, range.startIdx, range.endIdx, reps, ui.clipboardScope === 'all', tIdx);
      deps.onSongChanged(false);
      deps.onToast?.(`Powielono ${range.endIdx - range.startIdx + 1} takt(ów) ×${reps}`);
      deps.onRender();
      return true;
    }
    if (cmdId === 'del:note') {
      deps.pushUndo();
      if (hasMultiBeatSel) {
        const cnt = deleteSelectionNotes(ui.song, tIdx, sel);
        deps.onSongChanged(true);
        deps.onToast?.(`Wyczyszczono ${cnt} zaznaczonych uderzeń`);
        deps.onRender();
        return true;
      }
      deleteNoteAt(ui.song, tIdx, mIdx, bIdx, sIdx);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'del:beat') {
      deps.pushUndo();
      ui.cursor.beatIdx = deleteBeatAt(ui.song, tIdx, mIdx, bIdx);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'del:barClear') {
      deps.pushUndo();
      clearMeasureAt(ui.song, tIdx, mIdx);
      ui.cursor.beatIdx = 0;
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (cmdId === 'del:barRemove') {
      deps.pushUndo();
      if (removeMeasure(ui.song, mIdx)) {
        ui.cursor.measureIdx = Math.max(0, Math.min(getMasterMeasureCount(ui.song) - 1, mIdx));
        ui.cursor.beatIdx = 0;
        deps.onSongChanged(false);
        deps.onRender();
      }
      return true;
    }
    return false;
  }

  function handleFretClick(stringIdx, fret, { shiftKey = false } = {}) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr) return false;
    ui.cursor.stringIdx = stringIdx;
    deps.onAuditionFret?.(tr, stringIdx, fret);
    if (ui.fretInputMode === 'edit' || shiftKey) {
      deps.pushUndo();
      setNoteAt(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, stringIdx, fret);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    deps.onRender();
    return true;
  }

  function handleMidiNote(midi) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) return false;
    deps.pushUndo();
    const res = insertMidiNoteAt(
      ui.song,
      ui.activeTrackIdx,
      ui.cursor.measureIdx,
      ui.cursor.beatIdx,
      midi,
      ui.cursor.stringIdx
    );
    if (res.inserted) {
      ui.cursor.stringIdx = res.string;
      deps.onSongChanged(true);
      deps.onAuditionBeat?.(tr, tr.measures[ui.cursor.measureIdx]?.beats?.[ui.cursor.beatIdx]);
      deps.onRender();
      return true;
    }
    return false;
  }

  function handleKeyDown(e) {
    if (!e) return false;
    const ui = deps.getState();
    const ctrl = Boolean(e.ctrlKey || e.metaKey);
    const shift = Boolean(e.shiftKey);
    const alt = Boolean(e.altKey);
    const key = e.key || '';
    const low = key.toLowerCase();

    // Gdy otwarta nakładka Ctrl+K — przejmij nawigację strzałkami i Enter/Escape
    if (ui.cmdPalette?.open) {
      if (key === 'Escape') {
        e.preventDefault?.();
        ui.cmdPalette.open = false;
        deps.onRender();
        return true;
      }
      const list = filterStudioCommands(ui.cmdPalette.query);
      if (key === 'ArrowDown') {
        e.preventDefault?.();
        ui.cmdPalette.selectedIdx = Math.min(Math.max(0, list.length - 1), (ui.cmdPalette.selectedIdx || 0) + 1);
        deps.onRender();
        return true;
      }
      if (key === 'ArrowUp') {
        e.preventDefault?.();
        ui.cmdPalette.selectedIdx = Math.max(0, (ui.cmdPalette.selectedIdx || 0) - 1);
        deps.onRender();
        return true;
      }
      if (key === 'Enter' && list.length > 0) {
        e.preventDefault?.();
        const chosen = list[Math.max(0, Math.min(list.length - 1, ui.cmdPalette.selectedIdx || 0))];
        ui.cmdPalette.open = false;
        if (chosen) executeCommand(chosen.id);
        return true;
      }
      return false;
    }

    const tag = e.target?.tagName;
    if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT') {
      if (key === 'Escape' && ui.inlinePrompt) {
        e.preventDefault?.();
        cancelInlinePrompt();
        return true;
      }
      if (key === 'Enter' && e.target?.id === 'studioInlineInput') {
        e.preventDefault?.();
        submitInlinePrompt(e.target.value);
        return true;
      }
      return false;
    }

    if (ctrl && low === 'k') {
      e.preventDefault?.();
      ui.cmdPalette = { open: true, query: '', selectedIdx: 0 };
      deps.onRender();
      return true;
    }

    if (key === 'Escape') {
      if (cancelInlinePrompt()) {
        e.preventDefault?.();
        return true;
      }
      if (ui.beatSelection !== null || ui.selectionStart !== null) {
        e.preventDefault?.();
        clearAllSelection(ui);
        deps.onRender();
        return true;
      }
    }

    // Odtwarzanie: Space, Home, End, Ctrl+Shift+Left/Right, F9, [, ]
    if (key === ' ') {
      e.preventDefault?.();
      deps.onPlayToggle?.();
      return true;
    }
    if (key === 'Home' || (ctrl && shift && key === 'ArrowLeft')) {
      e.preventDefault?.();
      ui.cursor.measureIdx = 0;
      ui.cursor.beatIdx = 0;
      deps.onStop?.();
      deps.onRender();
      return true;
    }
    if (key === 'End' || (ctrl && shift && key === 'ArrowRight')) {
      e.preventDefault?.();
      const lastM = Math.max(0, getMasterMeasureCount(ui.song) - 1);
      ui.cursor.measureIdx = lastM;
      ui.cursor.beatIdx = 0;
      deps.onSeek?.(lastM, 0);
      deps.onRender();
      return true;
    }
    if (key === 'F9') {
      e.preventDefault?.();
      deps.onSetLoop?.({ enabled: !ui.loopEnabled, start: ui.loopStart, end: ui.loopEnd });
      return true;
    }
    if (key === '[') {
      e.preventDefault?.();
      const start = ui.cursor.measureIdx;
      const end = Math.max(start, ui.loopEnd);
      deps.onSetLoop?.({ enabled: true, start, end });
      return true;
    }
    if (key === ']') {
      e.preventDefault?.();
      const end = ui.cursor.measureIdx;
      const start = Math.min(ui.loopStart, end);
      deps.onSetLoop?.({ enabled: true, start, end });
      return true;
    }

    // Undo / Redo
    if (ctrl && low === 'z') {
      e.preventDefault?.();
      const prev = ui.history.undo(ui.song);
      if (prev) {
        ui.song = prev;
        deps.onSongChanged(false);
        deps.onRender();
      }
      return true;
    }
    if (ctrl && low === 'y') {
      e.preventDefault?.();
      const next = ui.history.redo(ui.song);
      if (next) {
        ui.song = next;
        deps.onSongChanged(false);
        deps.onRender();
      }
      return true;
    }

    // Schowek Ctrl+A / Ctrl+C / Ctrl+Shift+C / Ctrl+V / Ctrl+Shift+V / Ctrl+X
    if (ctrl && low === 'a') {
      e.preventDefault?.();
      const trCur = ui.song.tracks[ui.activeTrackIdx];
      if (!trCur) return false;
      const mCur = ui.cursor.measureIdx;
      const curBeatsLen = Math.max(1, trCur.measures[mCur]?.beats?.length || 1);
      const curSel = getActiveBeatSelection(ui);
      const isSingleBarAlready = curSel && curSel.startMeasure === mCur && curSel.endMeasure === mCur &&
        curSel.start.beatIdx === 0 && curSel.end.beatIdx === curBeatsLen - 1 && curSel.trackIdx === ui.activeTrackIdx;
      if (isSingleBarAlready) {
        const lastM = Math.max(0, trCur.measures.length - 1);
        const lastB = Math.max(0, (trCur.measures[lastM]?.beats?.length || 1) - 1);
        ui.beatSelection = {
          trackIdx: ui.activeTrackIdx,
          anchor: { measureIdx: 0, beatIdx: 0 },
          focus: { measureIdx: lastM, beatIdx: lastB },
          fullMeasures: true
        };
        ui.selectionStart = 0;
        ui.selectionEnd = lastM;
        deps.onToast?.(`Zaznaczono całą ścieżkę (${lastM + 1} taktów)`);
      } else {
        ui.beatSelection = {
          trackIdx: ui.activeTrackIdx,
          anchor: { measureIdx: mCur, beatIdx: 0 },
          focus: { measureIdx: mCur, beatIdx: curBeatsLen - 1 },
          fullMeasures: true
        };
        ui.selectionStart = mCur;
        ui.selectionEnd = mCur;
        deps.onToast?.(`Zaznaczono takt ${mCur + 1}`);
      }
      deps.onRender();
      return true;
    }
    if (ctrl && low === 'c') {
      e.preventDefault?.();
      return executeCommand('clip:copy', shift ? 'all' : undefined);
    }
    if (ctrl && low === 'v') {
      e.preventDefault?.();
      return executeCommand('clip:paste', shift ? 'insert' : undefined);
    }
    if (ctrl && low === 'x') {
      e.preventDefault?.();
      return executeCommand('clip:cut', shift ? 'all' : undefined);
    }

    // Przekształcenia: Alt+Up/Down (struna bez zmiany wysokości), Shift+Arrows (półton/oktawa)
    if (alt && (key === 'ArrowUp' || key === 'ArrowDown')) {
      e.preventDefault?.();
      return executeCommand(key === 'ArrowUp' ? 'transform:strUp' : 'transform:strDown');
    }
    if (shift && !ctrl && !alt && (key === 'ArrowRight' || key === 'ArrowLeft' || key === 'ArrowUp' || key === 'ArrowDown')) {
      e.preventDefault?.();
      const map = {
        ArrowRight: 'transform:semiUp',
        ArrowLeft: 'transform:semiDown',
        ArrowUp: 'transform:octUp',
        ArrowDown: 'transform:octDown'
      };
      return executeCommand(map[key]);
    }

    // Nawigacja między ścieżkami / taktami / uderzeniami / strunami
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr) return false;
    const maxM = Math.max(0, tr.measures.length - 1);

    if (ctrl && shift && (key === 'ArrowUp' || key === 'ArrowDown')) {
      e.preventDefault?.();
      clearAllSelection(ui);
      ui.activeTrackIdx = key === 'ArrowUp'
        ? Math.max(0, ui.activeTrackIdx - 1)
        : Math.min(ui.song.tracks.length - 1, ui.activeTrackIdx + 1);
      fretBuf.reset();
      deps.onCursorMoved?.();
      deps.onRender();
      return true;
    }
    if (ctrl && (key === 'ArrowLeft' || key === 'ArrowRight')) {
      e.preventDefault?.();
      clearAllSelection(ui);
      ui.cursor.measureIdx = key === 'ArrowLeft'
        ? Math.max(0, ui.cursor.measureIdx - 1)
        : Math.min(maxM, ui.cursor.measureIdx + 1);
      ui.cursor.beatIdx = 0;
      fretBuf.reset();
      deps.onSeek?.(ui.cursor.measureIdx, 0);
      deps.onRender();
      return true;
    }
    if (key === 'ArrowRight') {
      e.preventDefault?.();
      clearAllSelection(ui);
      moveCursorNext(true);
      return true;
    }
    if (key === 'ArrowLeft') {
      e.preventDefault?.();
      clearAllSelection(ui);
      moveCursorPrev();
      return true;
    }
    if (key === 'ArrowUp' || key === 'ArrowDown') {
      e.preventDefault?.();
      clearAllSelection(ui);
      const maxS = Math.max(0, (tr.openStringMidis?.length || 6) - 1);
      ui.cursor.stringIdx = key === 'ArrowUp'
        ? Math.min(maxS, ui.cursor.stringIdx + 1)
        : Math.max(0, ui.cursor.stringIdx - 1);
      fretBuf.reset();
      deps.onRender();
      return true;
    }

    // 3-poziomowe kasowanie: Del / Ctrl+Del / Shift+Del / Ctrl+Shift+Del
    if (key === 'Delete' || key === 'Backspace') {
      e.preventDefault?.();
      if (ctrl && shift) return executeCommand('del:barRemove');
      if (ctrl) return executeCommand('del:beat');
      if (shift) return executeCommand('del:barClear');
      return executeCommand('del:note');
    }

    // Wpisywanie progów 0–24 (dwucyfrowe w oknie 450 ms)
    if (!ctrl && !alt && /^[0-9]$/.test(key)) {
      e.preventDefault?.();
      if (tr.instrumentType === 'drums' || tr.isPercussion) return false;
      const slotKey = `${ui.activeTrackIdx}:${ui.cursor.measureIdx}:${ui.cursor.beatIdx}:${ui.cursor.stringIdx}`;
      const fret = fretBuf.pushDigit(Number(key), slotKey);
      deps.pushUndo();
      setNoteAt(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, ui.cursor.stringIdx, fret);
      deps.onSongChanged(true);
      deps.onAuditionFret?.(tr, ui.cursor.stringIdx, fret);
      deps.onRender();
      return true;
    }

    // Długości nut: Alt+1..7 oraz + / - / .
    if (alt && /^[1-7]$/.test(key)) {
      e.preventDefault?.();
      return executeCommand(`dur:${Number(key) - 3}`);
    }
    if (!ctrl && !alt && (key === '+' || key === '=')) {
      e.preventDefault?.();
      deps.pushUndo();
      stepBeatDuration(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, +1);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (!ctrl && !alt && (key === '-' || key === '_')) {
      e.preventDefault?.();
      deps.pushUndo();
      stepBeatDuration(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, -1);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (!ctrl && !alt && key === '.') {
      e.preventDefault?.();
      deps.pushUndo();
      cycleBeatDots(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, null);
      deps.onSongChanged(true);
      deps.onRender();
      return true;
    }
    if (alt && low === 't') {
      e.preventDefault?.();
      return executeCommand('tuplet:3:2');
    }

    // Pojedyncze skróty literowe z Sekcji 5
    if (!ctrl && !alt) {
      if (low === 'r' || key === 'Insert') {
        e.preventDefault?.();
        return executeCommand('rest');
      }
      const letterCmdMap = {
        l: 'effect:tied',
        p: 'effect:palmMute',
        h: 'effect:hammerOn',
        s: 'slide:shift',
        b: 'bend:full',
        v: 'effect:vibrato',
        x: 'effect:deadNote',
        o: 'harmonic:1',
        g: 'effect:ghostNote',
        d: 'clip:dup1',
        m: 'prompt:section',
        t: 'prompt:text'
      };
      if (letterCmdMap[low]) {
        e.preventDefault?.();
        return executeCommand(letterCmdMap[low]);
      }
    }
    return false;
  }

  return {
    handleKeyDown,
    handleFretClick,
    handleMidiNote,
    executeCommand,
    submitInlinePrompt,
    cancelInlinePrompt,
    resetBuffer: () => fretBuf.reset()
  };
}

/**
 * Bramka minimalnego czasu trwania nuty (odrzuca losowe „strzały” szumu krótsze niż minDurationMs).
 */
export function createDurationGate(minDurationMs = 80) {
  let minMs = Math.max(0, Number(minDurationMs) || 0);
  const pendingMap = new Map();
  const confirmedSet = new Set();
  let rejectedCount = 0;

  return {
    setMinDurationMs(ms) {
      minMs = Math.max(0, Number(ms) || 0);
    },
    getMinDurationMs() {
      return minMs;
    },
    getRejectedCount() {
      return rejectedCount;
    },
    reset() {
      pendingMap.clear();
      confirmedSet.clear();
      rejectedCount = 0;
    },
    processFrame(events = [], activeNotes = [], nowMs = 0) {
      const accepted = [];

      for (const ev of (events || [])) {
        if (!ev) continue;
        if (ev.type === 'noteOn') {
          const midi = Math.round(ev.midiNote);
          if (confirmedSet.has(midi)) continue;
          if (minMs <= 0) {
            confirmedSet.add(midi);
            accepted.push({ type: 'noteOn', midiNote: midi, vel: ev.vel ?? 95, durationMs: ev.durationMs ?? 0 });
          } else {
            pendingMap.set(midi, { midiNote: midi, vel: ev.vel ?? 95, startMs: nowMs });
          }
        } else if (ev.type === 'noteOff') {
          const midi = Math.round(ev.midiNote);
          if (pendingMap.has(midi)) {
            pendingMap.delete(midi);
            rejectedCount++;
          }
          confirmedSet.delete(midi);
        }
      }

      if (minMs > 0) {
        for (const n of (activeNotes || [])) {
          if (!n) continue;
          const midi = Math.round(n.midiNote);
          if (confirmedSet.has(midi)) continue;
          const pend = pendingMap.get(midi);
          const dur = Math.max(Number(n.durationMs) || 0, pend ? (nowMs - pend.startMs) : 0);
          if (dur >= minMs) {
            confirmedSet.add(midi);
            pendingMap.delete(midi);
            accepted.push({
              type: 'noteOn',
              midiNote: midi,
              vel: n.vel ?? pend?.vel ?? 95,
              durationMs: dur
            });
          }
        }
      }

      return accepted;
    }
  };
}

function stepQuarterForGrid(gridDuration = 1) {
  if (gridDuration === -2) return 4;
  if (gridDuration === -1) return 2;
  if (gridDuration === 0) return 1;
  if (gridDuration === 1) return 0.5;
  if (gridDuration === 2) return 0.25;
  if (gridDuration === 3) return 0.125;
  return 0.5;
}

function coalesceRestBeats(slotRunLen, stepQ) {
  const totalQ = slotRunLen * stepQ;
  const candidates = [
    { q: 4, d: -2, dots: 0 },
    { q: 3, d: -1, dots: 1 },
    { q: 2, d: -1, dots: 0 },
    { q: 1.5, d: 0, dots: 1 },
    { q: 1, d: 0, dots: 0 },
    { q: 0.75, d: 1, dots: 1 },
    { q: 0.5, d: 1, dots: 0 },
    { q: 0.25, d: 2, dots: 0 },
    { q: 0.125, d: 3, dots: 0 }
  ];
  const out = [];
  let rem = totalQ;
  let guard = 0;
  while (rem > 0.06 && guard++ < 16) {
    const pick = candidates.find(c => c.q <= rem + 1e-4) || candidates[candidates.length - 1];
    out.push(createBeat({ duration: pick.d, dots: pick.dots, isRest: true, notes: [] }));
    rem -= pick.q;
  }
  return out;
}

/**
 * Kwantyzuje wykryte zdarzenia czasowe { timeSec, midi, vel } do siatki taktów Studio TAB.
 */
export function quantizeTimedNotesToMeasures(timedEvents = [], opts = {}) {
  const tempoBpm = Math.max(30, Math.min(280, Number(opts.tempoBpm) || 120));
  const numerator = Math.max(1, Math.min(16, Math.round(Number(opts.numerator) || 4)));
  const denominator = [1, 2, 4, 8, 16].includes(Number(opts.denominator)) ? Number(opts.denominator) : 4;
  const gridDuration = Number.isInteger(Number(opts.gridDuration)) ? Number(opts.gridDuration) : 1;
  const track = opts.track || { openStringMidis: [40, 45, 50, 55, 59, 64], capo: 0 };

  const stepQ = stepQuarterForGrid(gridDuration);
  const capQ = numerator * (4 / denominator);
  const stepsPerBar = Math.max(1, Math.round(capQ / stepQ));
  const qPerSec = tempoBpm / 60;

  if (!Array.isArray(timedEvents) || timedEvents.length === 0) {
    return { measures: [createEmptyMeasure(numerator, denominator)], noteRefs: [] };
  }

  const slotMap = new Map();
  let maxStepIdx = 0;

  for (const ev of timedEvents) {
    const tSec = Math.max(0, Number(ev.timeSec) || 0);
    const midi = Math.round(Number(ev.midi ?? ev.midiNote));
    if (!Number.isFinite(midi) || midi < 20 || midi > 108) continue;
    const rawQ = tSec * qPerSec;
    const stepIdx = Math.max(0, Math.round(rawQ / stepQ));
    if (stepIdx > maxStepIdx) maxStepIdx = stepIdx;
    if (!slotMap.has(stepIdx)) slotMap.set(stepIdx, []);
    const arr = slotMap.get(stepIdx);
    if (!arr.some(x => x.midi === midi)) {
      arr.push({ midi, vel: Math.max(1, Math.min(127, Math.round(ev.vel || 95))) });
    }
  }

  if (slotMap.size === 0) {
    return { measures: [createEmptyMeasure(numerator, denominator)], noteRefs: [] };
  }

  const barCount = Math.max(1, Math.ceil((maxStepIdx + 1) / stepsPerBar));
  const measures = [];
  const noteRefs = [];

  for (let mIdx = 0; mIdx < barCount; mIdx++) {
    const beats = [];
    let s = 0;
    while (s < stepsPerBar) {
      const globalStep = mIdx * stepsPerBar + s;
      const items = slotMap.get(globalStep);
      if (!items || items.length === 0) {
        let runEnd = s + 1;
        while (runEnd < stepsPerBar && !slotMap.has(mIdx * stepsPerBar + runEnd)) {
          runEnd++;
        }
        beats.push(...coalesceRestBeats(runEnd - s, stepQ));
        s = runEnd;
        continue;
      }

      const usedStrings = new Set();
      const notes = [];
      for (const item of items) {
        const cands = findFretCandidatesForMidi(track, item.midi, 0, 24);
        if (cands.length === 0) continue;
        const pick = cands.find(c => !usedStrings.has(c.string)) || cands[0];
        if (!usedStrings.has(pick.string)) {
          usedStrings.add(pick.string);
          notes.push(createTabNote({ string: pick.string, fret: pick.fret, velocity: item.vel }));
          noteRefs.push({
            measureIdx: mIdx,
            beatIdx: beats.length,
            string: pick.string,
            fret: pick.fret,
            midi: item.midi
          });
        }
      }
      notes.sort((a, b) => a.string - b.string);
      beats.push(createBeat({
        duration: gridDuration,
        dots: 0,
        isRest: notes.length === 0,
        notes
      }));
      s++;
    }

    measures.push({
      numerator,
      denominator,
      repeatOpen: false,
      repeatClose: 0,
      voltaMask: 0,
      sectionMarker: '',
      isPickup: false,
      tripletFeel: 0,
      isDoubleBar: false,
      drumBlockId: 'rock_8th',
      beats
    });
  }

  return { measures, noteRefs };
}

/**
 * Przelicza surowe ramki PitchPlease (z nagrania Offline lub bufora) na nuty i takty Studio TAB.
 */
export function recomputeOfflineRecordedFrames(frames = [], opts = {}) {
  const sens = Number.isFinite(Number(opts.sensitivity)) ? Number(opts.sensitivity) : 0.58;
  const highThr = Number.isFinite(Number(opts.highThresholdOn))
    ? Number(opts.highThresholdOn)
    : Math.max(0.32, Math.round((sens - 0.16) * 100) / 100);
  const minDurMs = Number.isFinite(Number(opts.minDurationMs)) ? Number(opts.minDurationMs) : 80;
  const track = opts.track || { openStringMidis: [40, 45, 50, 55, 59, 64], capo: 0 };

  const openMidis = track.openStringMidis || [40, 45, 50, 55, 59, 64];
  const minMidi = Math.max(28, Math.min(...openMidis) + (track.capo || 0) - 2);
  const maxMidi = Math.min(96, Math.max(...openMidis) + (track.capo || 0) + 24);

  const tracker = new GuitarEventTracker({
    minMidi,
    maxMidi,
    thresholdOn: sens,
    highThresholdOn: highThr,
    autoAdapt: false
  });
  const gate = createDurationGate(minDurMs);
  const timedEvents = [];

  for (const fr of (frames || [])) {
    const tSec = Number.isFinite(Number(fr.time)) ? Number(fr.time) : (Number(fr.emitTime) || 0);
    const tsMs = tSec * 1000;
    const res = tracker.step(fr.probs || null, fr.notes || [], tsMs, null, null, opts.sampleRate || 48000);
    const accepted = gate.processFrame(res.events, res.activeNotes, tsMs);
    for (const ev of accepted) {
      timedEvents.push({
        timeSec: Math.max(0, tSec - (ev.durationMs || 0) / 1000),
        midi: ev.midiNote,
        vel: ev.vel
      });
    }
  }

  const quant = quantizeTimedNotesToMeasures(timedEvents, {
    tempoBpm: opts.tempoBpm,
    numerator: opts.numerator,
    denominator: opts.denominator,
    gridDuration: opts.gridDuration,
    track
  });

  return {
    ...quant,
    timedEvents,
    rejectedShortCount: gate.getRejectedCount()
  };
}

/**
 * Kontroler mikrofonu dla Strefy E (Faza 3): Step-Input, Nagrywanie do metronomu (Offline/Live),
 * Graj razem (Play-Along) oraz analizator pozycji (Pozycja 1/2/3...).
 */
export function createStudioMicController(deps) {
  let mode = 'off'; // 'off' | 'step' | 'rec' | 'playalong'
  let savedGlobalConfig = null;
  let stepGate = createDurationGate(80);
  let liveRecGate = createDurationGate(80);
  let stepChordBuf = [];
  let stepChordTimer = null;
  let liveDetectedMidis = new Set();

  // Stan nagrywania do metronomu
  let recActive = false;
  let recWaitingCountIn = false;
  let recStartTimeSec = null;
  let recStartPerfMs = 0;
  let liveRecEvents = [];
  let offlineChunks = [];
  let offlineSampleRate = 48000;
  let pcmCaptureNode = null;
  let pcmSourceNode = null;
  let pcmZeroGain = null;
  let lastRecording = null; // { mode: 'offline'|'live', frames, events, sampleRate, noteRefs }

  // Stan Play-Along
  const playAlong = {
    hits: 0,
    totalExpected: 0,
    matchedBeatKeys: new Set(),
    lastFeedback: '',
    mutedTrackIdx: -1,
    prevMuted: false
  };

  function getPrefs() {
    const ui = deps.getState();
    ui.studioPrefs = clampStudioPrefs(ui.studioPrefs);
    return ui.studioPrefs;
  }

  function resolveThresholds(prefs) {
    const on = prefs.detectSensitivity;
    const highOn = prefs.detectHighThresholdOn !== null
      ? prefs.detectHighThresholdOn
      : Math.max(0.32, Math.min(on, Math.round((on - 0.16) * 100) / 100));
    return { thresholdOn: on, highThresholdOn: highOn };
  }

  function applyStudioOverride() {
    const det = deps.getDetector?.();
    if (!det || typeof det.updateConfig !== 'function') return false;
    if (!savedGlobalConfig && typeof det.getConfig === 'function') {
      savedGlobalConfig = det.getConfig();
    }
    const prefs = getPrefs();
    const { thresholdOn, highThresholdOn } = resolveThresholds(prefs);
    det.updateConfig({ thresholdOn, highThresholdOn });
    stepGate.setMinDurationMs(prefs.minNoteDurationMs);
    liveRecGate.setMinDurationMs(prefs.minNoteDurationMs);
    return true;
  }

  function restoreGlobalConfig() {
    const det = deps.getDetector?.();
    if (det && savedGlobalConfig && typeof det.updateConfig === 'function') {
      det.updateConfig(savedGlobalConfig);
    }
    savedGlobalConfig = null;
  }

  function stopPcmCapture() {
    try { pcmCaptureNode?.disconnect?.(); } catch {}
    try { pcmSourceNode?.disconnect?.(); } catch {}
    try { pcmZeroGain?.disconnect?.(); } catch {}
    pcmCaptureNode = null;
    pcmSourceNode = null;
    pcmZeroGain = null;
  }

  function startPcmCapture() {
    stopPcmCapture();
    offlineChunks = [];
    const ctx = deps.getAudioCtx?.();
    const stream = deps.getMicStream?.();
    if (!ctx || !stream || typeof ctx.createScriptProcessor !== 'function') return false;
    offlineSampleRate = ctx.sampleRate || 48000;
    try {
      pcmSourceNode = ctx.createMediaStreamSource(stream);
      pcmCaptureNode = ctx.createScriptProcessor(2048, 1, 1);
      pcmZeroGain = ctx.createGain();
      pcmZeroGain.gain.value = 0;
      pcmCaptureNode.onaudioprocess = (ev) => {
        if (!recActive || recWaitingCountIn) return;
        const ch = ev.inputBuffer.getChannelData(0);
        offlineChunks.push(new Float32Array(ch));
      };
      pcmSourceNode.connect(pcmCaptureNode);
      pcmCaptureNode.connect(pcmZeroGain);
      pcmZeroGain.connect(ctx.destination);
      return true;
    } catch {
      stopPcmCapture();
      return false;
    }
  }

  function restorePlayAlongMute() {
    if (playAlong.mutedTrackIdx >= 0) {
      const ui = deps.getState();
      const tr = ui.song.tracks[playAlong.mutedTrackIdx];
      if (tr) tr.muted = playAlong.prevMuted;
      playAlong.mutedTrackIdx = -1;
      deps.onSongChanged?.(true);
    }
  }

  function flushStepChord() {
    if (stepChordTimer) {
      clearTimeout(stepChordTimer);
      stepChordTimer = null;
    }
    if (stepChordBuf.length === 0) return;

    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) {
      stepChordBuf = [];
      return;
    }

    const mIdx = ui.cursor.measureIdx;
    const bIdx = ui.cursor.beatIdx;
    const m = tr.measures?.[mIdx];
    const beat = m?.beats?.[bIdx];
    if (!beat) {
      stepChordBuf = [];
      return;
    }

    deps.pushUndo();
    const usedStrings = new Set();
    const newNotes = [];
    let firstPlaced = null;

    for (const hit of stepChordBuf) {
      const cands = findFretCandidatesForMidi(tr, hit.midi, 0, 24);
      if (cands.length === 0) continue;
      const pick = cands.find(c => !usedStrings.has(c.string)) || cands[0];
      if (!usedStrings.has(pick.string)) {
        usedStrings.add(pick.string);
        newNotes.push(createTabNote({ string: pick.string, fret: pick.fret, velocity: hit.vel || 96 }));
        if (!firstPlaced) {
          firstPlaced = {
            measureIdx: mIdx,
            beatIdx: bIdx,
            string: pick.string,
            fret: pick.fret,
            midi: hit.midi
          };
        }
      }
    }

    if (newNotes.length > 0) {
      newNotes.sort((a, b) => a.string - b.string);
      beat.isRest = false;
      beat.notes = newNotes;
      if (firstPlaced) {
        ui.cursor.stringIdx = firstPlaced.string;
        ui.lastPlacedNote = firstPlaced;
      }
      deps.onSongChanged?.(true);
      refreshPhrasePositions();
      // Auto-przesunięcie kursora do kolejnego slotu po zakończeniu akordu
      if (ui.cursor.beatIdx + 1 < m.beats.length) {
        ui.cursor.beatIdx++;
      } else if (ui.cursor.measureIdx + 1 < tr.measures.length) {
        ui.cursor.measureIdx++;
        ui.cursor.beatIdx = 0;
      } else {
        const added = addMeasure(ui.song, ui.cursor.measureIdx);
        if (added >= 0) {
          ui.cursor.measureIdx = added;
          ui.cursor.beatIdx = 0;
          deps.onSongChanged?.(false);
        }
      }
      deps.onRender?.();
    }
    stepChordBuf = [];
  }

  function refreshPhrasePositions(noteRefs = null) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr || tr.instrumentType === 'drums' || tr.isPercussion) {
      ui.phrasePositions = [];
      ui.selectedPhrasePosIdx = -1;
      return;
    }
    ui.phrasePositions = analyzePhrasePositions(tr, noteRefs, { boxSpan: 4, maxOptions: 5 });
    ui.selectedPhrasePosIdx = -1;
  }

  function applyQuantizedResult(res) {
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    if (!tr || !res || !Array.isArray(res.measures)) return;

    deps.pushUndo();
    tr.measures = res.measures;
    // Wyrównaj długość pozostałych ścieżek do nowej liczby taktów
    const needBars = tr.measures.length;
    for (let t = 0; t < ui.song.tracks.length; t++) {
      if (t === ui.activeTrackIdx) continue;
      const other = ui.song.tracks[t];
      const refM = res.measures[0] || { numerator: 4, denominator: 4 };
      while (other.measures.length < needBars) {
        other.measures.push(createEmptyMeasure(refM.numerator, refM.denominator, 'rock_8th', other.instrumentType === 'drums'));
      }
    }
    ui.cursor.measureIdx = 0;
    ui.cursor.beatIdx = 0;
    ui.positionAreaRange = null;
    refreshPhrasePositions(res.noteRefs);
    deps.onSongChanged?.(false);
    deps.onRender?.();
  }

  async function finalizeRecording() {
    const wasActive = recActive;
    recActive = false;
    recWaitingCountIn = false;
    stopPcmCapture();
    if (!wasActive) return;

    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    const prefs = getPrefs();
    const curM = tr?.measures?.[0] || { numerator: 4, denominator: 4 };
    const bpm = Number(ui.song.tempoBpm) || 120;

    if (prefs.liveRecMode === 'offline' && offlineChunks.length > 0) {
      const totalLen = offlineChunks.reduce((acc, a) => acc + a.length, 0);
      const pcm = new Float32Array(totalLen);
      let pos = 0;
      for (const ch of offlineChunks) {
        pcm.set(ch, pos);
        pos += ch.length;
      }
      offlineChunks = [];
      try {
        deps.onToast?.('Analiza nagrania Offline (PitchPlease)...');
        const frames = await analyzePitchPlease(pcm, offlineSampleRate, { stabilityFrames: 1 });
        lastRecording = {
          mode: 'offline',
          frames,
          sampleRate: offlineSampleRate,
          events: []
        };
        const { thresholdOn, highThresholdOn } = resolveThresholds(prefs);
        const res = recomputeOfflineRecordedFrames(frames, {
          sensitivity: thresholdOn,
          highThresholdOn,
          minDurationMs: prefs.minNoteDurationMs,
          tempoBpm: bpm,
          numerator: curM.numerator,
          denominator: curM.denominator,
          gridDuration: prefs.quantizeGrid,
          sampleRate: offlineSampleRate,
          track: tr
        });
        applyQuantizedResult(res);
        deps.onToast?.(`Rozpoznano ${res.noteRefs.length} nut (odrzucono ${res.rejectedShortCount} krótkich trzasków)`);
      } catch (err) {
        deps.onToast?.(`Błąd analizy Offline: ${err?.message || err}`);
      }
      return;
    }

    // Tryb Live (lub fallback jeśli brak PCM offline)
    lastRecording = {
      mode: 'live',
      frames: null,
      sampleRate: offlineSampleRate,
      events: liveRecEvents.slice()
    };
    const quant = quantizeTimedNotesToMeasures(liveRecEvents, {
      tempoBpm: bpm,
      numerator: curM.numerator,
      denominator: curM.denominator,
      gridDuration: prefs.quantizeGrid,
      track: tr
    });
    applyQuantizedResult(quant);
    deps.onToast?.(`Nagrano Live: ${quant.noteRefs.length} nut`);
  }

  function recomputeLastRecording() {
    if (!lastRecording) return false;
    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];
    const prefs = getPrefs();
    const curM = tr?.measures?.[0] || { numerator: 4, denominator: 4 };
    const bpm = Number(ui.song.tempoBpm) || 120;

    if (lastRecording.mode === 'offline' && Array.isArray(lastRecording.frames)) {
      const { thresholdOn, highThresholdOn } = resolveThresholds(prefs);
      const res = recomputeOfflineRecordedFrames(lastRecording.frames, {
        sensitivity: thresholdOn,
        highThresholdOn,
        minDurationMs: prefs.minNoteDurationMs,
        tempoBpm: bpm,
        numerator: curM.numerator,
        denominator: curM.denominator,
        gridDuration: prefs.quantizeGrid,
        sampleRate: lastRecording.sampleRate || 48000,
        track: tr
      });
      applyQuantizedResult(res);
      deps.onToast?.(`Przeliczono Offline: ${res.noteRefs.length} nut`);
      return true;
    }
    if (Array.isArray(lastRecording.events) && lastRecording.events.length > 0) {
      const quant = quantizeTimedNotesToMeasures(lastRecording.events, {
        tempoBpm: bpm,
        numerator: curM.numerator,
        denominator: curM.denominator,
        gridDuration: prefs.quantizeGrid,
        track: tr
      });
      applyQuantizedResult(quant);
      deps.onToast?.(`Przeliczono siatkę Live: ${quant.noteRefs.length} nut`);
      return true;
    }
    return false;
  }

  async function setMode(nextMode) {
    if (nextMode === mode) {
      if (mode === 'rec') {
        await finalizeRecording();
        deps.onStopMetronome?.();
        mode = 'off';
        restoreGlobalConfig();
        deps.onRender?.();
        return 'off';
      }
      if (mode !== 'off') {
        nextMode = 'off';
      }
    }

    // Wyłączenie poprzedniego trybu
    if (recActive) {
      await finalizeRecording();
      deps.onStopMetronome?.();
    }
    flushStepChord();
    restorePlayAlongMute();

    if (nextMode === 'off') {
      mode = 'off';
      stepGate.reset();
      liveRecGate.reset();
      liveDetectedMidis.clear();
      restoreGlobalConfig();
      deps.onRender?.();
      return 'off';
    }

    // Uruchom mikrofon hosta jeśli jeszcze nie działa
    if (typeof deps.ensureMicActive === 'function') {
      const ok = await deps.ensureMicActive();
      if (!ok) {
        deps.onToast?.('Włącz mikrofon, aby korzystać z trybów nagrywania.');
        return mode;
      }
    }

    mode = nextMode;
    applyStudioOverride();
    stepGate.reset();
    liveRecGate.reset();

    const ui = deps.getState();
    const tr = ui.song.tracks[ui.activeTrackIdx];

    if (mode === 'rec') {
      recActive = true;
      liveRecEvents = [];
      recStartTimeSec = null;
      recStartPerfMs = 0;
      recWaitingCountIn = Boolean(ui.countInEnabled);
      if (getPrefs().liveRecMode === 'offline') {
        startPcmCapture();
      }
      deps.onStartMetronomeRec?.();
    } else if (mode === 'playalong') {
      playAlong.hits = 0;
      playAlong.totalExpected = 0;
      playAlong.matchedBeatKeys.clear();
      playAlong.lastFeedback = 'Czekam na start...';
      if (tr) {
        playAlong.mutedTrackIdx = ui.activeTrackIdx;
        playAlong.prevMuted = Boolean(tr.muted);
        tr.muted = true;
        deps.onSongChanged?.(true);
      }
      deps.onStartPlayAlong?.();
    }

    deps.onRender?.();
    return mode;
  }

  function onPlaybackPhaseChange(posInfo) {
    if (recActive && posInfo?.phase === 'playing' && recWaitingCountIn) {
      recWaitingCountIn = false;
      recStartTimeSec = null;
      recStartPerfMs = typeof performance !== 'undefined' ? performance.now() : Date.now();
    }
    if (mode === 'playalong' && posInfo?.phase === 'playing' && posInfo.noteChanged) {
      const ui = deps.getState();
      const { activeNotes } = computeActiveAndNextNotes(ui.song, ui.activeTrackIdx, posInfo.measureIdx, posInfo.beatIdx);
      if (activeNotes.length > 0) {
        playAlong.totalExpected += activeNotes.length;
      }
    }
  }

  function onPitchFrame(payload) {
    if (!payload) return;
    const events = payload.events || [];
    const activeNotes = payload.activeNotes || [];

    liveDetectedMidis = new Set(activeNotes.map(n => Math.round(n.midiNote)));
    deps.onLiveFretboardRefresh?.();

    const nowMs = (payload.frameTimeSec !== null && payload.frameTimeSec !== undefined)
      ? payload.frameTimeSec * 1000
      : (typeof performance !== 'undefined' ? performance.now() : Date.now());

    if (mode === 'step') {
      const accepted = stepGate.processFrame(events, activeNotes, nowMs);
      if (accepted.length > 0) {
        for (const ev of accepted) {
          stepChordBuf.push({ midi: ev.midiNote, vel: ev.vel });
        }
        if (stepChordTimer) clearTimeout(stepChordTimer);
        stepChordTimer = setTimeout(() => flushStepChord(), 90);
      }
      return;
    }

    if (mode === 'rec' && recActive && !recWaitingCountIn && getPrefs().liveRecMode === 'live') {
      const tFrameSec = (typeof payload.frameTimeSec === 'number')
        ? payload.frameTimeSec
        : ((typeof performance !== 'undefined' ? performance.now() : Date.now()) - recStartPerfMs) / 1000;
      if (recStartTimeSec === null) recStartTimeSec = tFrameSec;
      const accepted = liveRecGate.processFrame(events, activeNotes, nowMs);
      for (const ev of accepted) {
        const relSec = Math.max(0, tFrameSec - recStartTimeSec - (ev.durationMs || 0) / 1000);
        liveRecEvents.push({ timeSec: relSec, midi: ev.midiNote, vel: ev.vel });
      }
      return;
    }

    if (mode === 'playalong') {
      const accepted = stepGate.processFrame(events, activeNotes, nowMs);
      if (accepted.length > 0) {
        const ui = deps.getState();
        const mIdx = ui.lastPlayPos?.active ? ui.lastPlayPos.measureIdx : ui.cursor.measureIdx;
        const bIdx = ui.lastPlayPos?.active ? ui.lastPlayPos.beatIdx : ui.cursor.beatIdx;
        const { activeNotes: expected } = computeActiveAndNextNotes(ui.song, ui.activeTrackIdx, mIdx, bIdx);
        const expectedMidis = new Set(expected.map(x => Math.round(x.midi)));
        for (const ev of accepted) {
          const hitKey = `${mIdx}:${bIdx}:${ev.midiNote}`;
          if (expectedMidis.has(ev.midiNote) && !playAlong.matchedBeatKeys.has(hitKey)) {
            playAlong.matchedBeatKeys.add(hitKey);
            playAlong.hits++;
            playAlong.lastFeedback = '✓ Trafione';
          }
        }
        deps.onLivePlayAlongRefresh?.();
      }
    }
  }

  return {
    getMode: () => mode,
    isRecording: () => recActive,
    setMode,
    onDetectorReady: () => {
      if (mode !== 'off') applyStudioOverride();
    },
    syncThresholdToDetector: () => {
      const prefs = getPrefs();
      stepGate.setMinDurationMs(prefs.minNoteDurationMs);
      liveRecGate.setMinDurationMs(prefs.minNoteDurationMs);
      if (mode !== 'off') applyStudioOverride();
    },
    restoreGlobalConfig,
    onPitchFrame,
    onPlaybackPhaseChange,
    recomputeLastRecording,
    hasLastRecording: () => Boolean(lastRecording),
    getLiveDetectedMidis: () => liveDetectedMidis,
    getPlayAlongStats: () => ({
      hits: playAlong.hits,
      total: playAlong.totalExpected,
      percent: playAlong.totalExpected > 0 ? Math.min(100, Math.round((playAlong.hits / playAlong.totalExpected) * 100)) : 0,
      lastFeedback: playAlong.lastFeedback
    }),
    refreshPhrasePositions,
    applyPositionIndex(posIdx) {
      const ui = deps.getState();
      const list = ui.phrasePositions || [];
      const pos = list[posIdx];
      if (!pos) return false;
      deps.pushUndo();
      applyPhrasePosition(ui.song, ui.activeTrackIdx, pos);
      ui.selectedPhrasePosIdx = posIdx;
      ui.positionAreaRange = { minFret: pos.minFret, maxFret: pos.maxFret, label: pos.label };
      deps.onSongChanged?.(true);
      deps.onRender?.();
      return true;
    },
    selectCandidateForNote(toString, toFret) {
      const ui = deps.getState();
      const curNote = getNoteAt(ui.song, ui.activeTrackIdx, ui.cursor.measureIdx, ui.cursor.beatIdx, ui.cursor.stringIdx);
      const fromCursor = Boolean(curNote);
      const ref = fromCursor
        ? { measureIdx: ui.cursor.measureIdx, beatIdx: ui.cursor.beatIdx, string: curNote.string }
        : ui.lastPlacedNote;
      if (!ref) return false;
      deps.pushUndo();
      const ok = setNotePosition(ui.song, ui.activeTrackIdx, ref.measureIdx, ref.beatIdx, ref.string, toString, toFret);
      if (ok) {
        if (fromCursor) {
          ui.cursor.stringIdx = toString;
        } else {
          ui.lastPlacedNote = { ...ref, string: toString, fret: toFret };
        }
        deps.onSongChanged?.(true);
        deps.onRender?.();
      }
      return ok;
    },
    _flushStepChordForTest: flushStepChord
  };
}
