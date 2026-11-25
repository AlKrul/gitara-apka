/**
 * Renderer SVG i HTML dla 🎼 Studio TAB:
 *  - renderStudioTabSvg: wielotaktowy TAB (6 str. gitara, 4 str. bas, klawisze, 4-liniowa perkusja)
 *    + opcjonalna pięciolinia (Nuty + TAB), pomarańczowy kursor slotowy, repetycje, volty, sekcje, playhead
 *  - renderStudioFretboardSvg: animowany kolorowy gryf Strefy E (aktywne nuty, przerywane następne,
 *    tło skali, bursztynowi kandydaci #f59e0b) + pady perkusyjne / podgląd klawiszy
 *  - renderArrangerMatrixHtml: dolna matryca Ścieżki × Takty (Mini-DAW)
 */
import {
  DRUM_BLOCKS,
  DRUM_GRID_LANES,
  drumBlockToGridPattern,
  expandDrumBlockToSteps,
  getDrumBlockById,
  resolveDrumMeasureSteps
} from './studioDrumBlocks.js';
import {
  MAX_STUDIO_TRACKS,
  METRONOME_PRESETS,
  TRACK_ROLE_PRESETS,
  durationToQuarterNotes,
  getMasterMeasureCount,
  getScaleHintPitchClasses,
  isBeatInSelection,
  midiToNoteName,
  midiToNoteWithOctave,
  normalizeBeatSelection,
  normalizeMetronomePattern,
  noteToMidi,
  validateMeasureMeter
} from './studioModel.js';

export function escapeHtml(str) {
  return String(str ?? '')
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;')
    .replace(/'/g, '&#39;');
}

const DIATONIC_C_STEPS = [0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6];
const CHROMATIC_SHARP_PC = [false, true, false, true, false, false, true, false, true, false, true, false];
const CHROMATIC_FLAT_PC = [false, true, false, true, false, false, true, false, true, false, true, false];

/**
 * Zwraca pisaną wysokość MIDI dla pięciolinii (gitara i bas klasycznie transponują oktawę w dół 8vb,
 * dzięki czemu puste struny mieszczą się czytelnie na pięciolinii zamiast 5 linii pod nią).
 */
export function writtenStaffMidi(track, soundingMidi) {
  if (!track || !Number.isFinite(soundingMidi)) return soundingMidi;
  if (track.instrumentType === 'guitar' || track.instrumentType === 'bass') {
    return soundingMidi + 12;
  }
  return soundingMidi;
}

/**
 * Mapuje numer MIDI na pozycję diatoniczną pięciolinii względem dolnej linii (E4=0 dla wiolinowego, G2=0 dla basowego).
 */
export function computeStaffStepFromMidi(midi, isBassClef = false, accMode = 2) {
  const m = Math.round(midi);
  const oct = Math.floor(m / 12) - 1;
  const pc = ((m % 12) + 12) % 12;
  const letterStep = DIATONIC_C_STEPS[pc];
  // Krok względem C0 = 0
  const absStep = oct * 7 + letterStep;
  // Dolna linia klucza wiolinowego to E4 (oct=4, step=2 -> 30)
  // Dolna linia klucza basowego to G2 (oct=2, step=4 -> 18)
  const refStep = isBassClef ? 18 : 30;
  const isAcc = accMode === 1 ? CHROMATIC_FLAT_PC[pc] : CHROMATIC_SHARP_PC[pc];
  return {
    step: absStep - refStep,
    accidental: isAcc ? (accMode === 1 ? '♭' : '♯') : ''
  };
}

function renderBeatOnStaff({
  track,
  beat,
  cx,
  staffTopY,
  isBassClef,
  accMode,
  isBeatPlaying,
  isBeatCursor,
  isBeatSelected,
  keyRootPc = 4,
  showNoteNames = true,
  isKeys = false
}) {
  let out = '';
  const dur = Number.isFinite(beat.duration) ? beat.duration : 0;
  const noteCol = isBeatPlaying ? '#22c55e' : (isBeatSelected ? '#fbbf24' : 'var(--text)');
  const stemCol = isBeatPlaying ? '#22c55e' : 'var(--text)';
  const accentCursor = isBeatCursor || isBeatPlaying;

  if (beat.isRest || !Array.isArray(beat.notes) || beat.notes.length === 0) {
    if (dur === -2) {
      out += `<rect x="${(cx - 5).toFixed(1)}" y="${(staffTopY + 10).toFixed(1)}" width="10" height="4.5" rx="1" fill="${accentCursor ? '#f39237' : 'var(--text-muted)'}" opacity="0.88" pointer-events="none"/>`;
    } else if (dur === -1) {
      out += `<rect x="${(cx - 5).toFixed(1)}" y="${(staffTopY + 15.5).toFixed(1)}" width="10" height="4.5" rx="1" fill="${accentCursor ? '#f39237' : 'var(--text-muted)'}" opacity="0.88" pointer-events="none"/>`;
    } else {
      out += `<text x="${cx.toFixed(1)}" y="${(staffTopY + 27).toFixed(1)}" fill="${accentCursor ? '#f39237' : 'var(--text-muted)'}" font-size="19" font-weight="700" text-anchor="middle" pointer-events="none">𝄽</text>`;
    }
    if (beat.dots >= 1) {
      out += `<circle cx="${(cx + 9.5).toFixed(1)}" cy="${(staffTopY + 17.5).toFixed(1)}" r="1.6" fill="var(--text-muted)" pointer-events="none"/>`;
    }
    return out;
  }

  const items = [];
  for (const n of beat.notes) {
    const midi = noteToMidi(track, n);
    if (midi === null) continue;
    const wMidi = writtenStaffMidi(track, midi);
    const { step, accidental } = computeStaffStepFromMidi(wMidi, isBassClef, accMode);
    const clampedStep = Math.max(-8, Math.min(13, step));
    const ny = staffTopY + (8 - clampedStep) * 5;
    items.push({ note: n, midi, wMidi, step, clampedStep, ny, accidental, headRight: false });
  }
  if (items.length === 0) return out;

  items.sort((a, b) => a.clampedStep - b.clampedStep);
  for (let i = 1; i < items.length; i++) {
    if ((items[i].clampedStep - items[i - 1].clampedStep) <= 1 && !items[i - 1].headRight) {
      items[i].headRight = true;
    }
  }

  const minStep = items[0].clampedStep;
  const maxStep = items[items.length - 1].clampedStep;
  for (let ls = -2; ls >= minStep; ls -= 2) {
    const ly = staffTopY + (8 - ls) * 5;
    out += `<line x1="${(cx - 9.5).toFixed(1)}" y1="${ly.toFixed(1)}" x2="${(cx + 9.5).toFixed(1)}" y2="${ly.toFixed(1)}" stroke="var(--text-muted)" stroke-width="1.2" opacity="0.85" pointer-events="none"/>`;
  }
  for (let ls = 10; ls <= maxStep; ls += 2) {
    const ly = staffTopY + (8 - ls) * 5;
    out += `<line x1="${(cx - 9.5).toFixed(1)}" y1="${ly.toFixed(1)}" x2="${(cx + 9.5).toFixed(1)}" y2="${ly.toFixed(1)}" stroke="var(--text-muted)" stroke-width="1.2" opacity="0.85" pointer-events="none"/>`;
  }

  let lastAccStep = -999;
  let accLane = 0;
  for (const it of items) {
    if (!it.accidental) continue;
    accLane = (it.clampedStep - lastAccStep <= 2) ? (accLane + 1) % 2 : 0;
    lastAccStep = it.clampedStep;
    const accX = cx - 8.5 - accLane * 7;
    const sym = it.accidental === '♯' ? '#' : '♭';
    out += `<text x="${accX.toFixed(1)}" y="${(it.ny + 4).toFixed(1)}" fill="${noteCol}" font-size="11.5" font-weight="800" text-anchor="end" pointer-events="none">${sym}</text>`;
  }

  const isOpenHead = dur <= -1;
  let minY = Infinity;
  let maxY = -Infinity;
  for (const it of items) {
    const nx = it.headRight ? (cx + 9.5) : cx;
    minY = Math.min(minY, it.ny);
    maxY = Math.max(maxY, it.ny);
    if (isOpenHead) {
      out += `<ellipse cx="${nx.toFixed(1)}" cy="${it.ny.toFixed(1)}" rx="5.8" ry="4.1" fill="var(--bg-card)" stroke="${noteCol}" stroke-width="1.9" pointer-events="none"/>`;
    } else {
      out += `<ellipse cx="${nx.toFixed(1)}" cy="${it.ny.toFixed(1)}" rx="5.6" ry="4.2" fill="${noteCol}" pointer-events="none"/>`;
    }
  }

  const midY = (minY + maxY) / 2;
  const stemDown = midY <= staffTopY + 20;
  let stemTipY = stemDown ? (maxY + 23) : (minY - 23);

  if (dur > -2) {
    const sx = stemDown ? (cx - 5.0) : (cx + 5.0);
    const yStart = stemDown ? minY : maxY;
    out += `<line x1="${sx.toFixed(1)}" y1="${yStart.toFixed(1)}" x2="${sx.toFixed(1)}" y2="${stemTipY.toFixed(1)}" stroke="${stemCol}" stroke-width="1.5" pointer-events="none"/>`;
    const flagCount = Math.min(3, Math.max(0, dur));
    const dir = stemDown ? -1 : 1;
    for (let f = 0; f < flagCount; f++) {
      const fy = stemTipY + f * 5 * dir;
      out += `<path d="M ${sx.toFixed(1)},${fy.toFixed(1)} Q ${(sx + 8).toFixed(1)},${(fy + 6 * dir).toFixed(1)} ${(sx + 3.5).toFixed(1)},${(fy + 11 * dir).toFixed(1)}" fill="none" stroke="${stemCol}" stroke-width="1.5" pointer-events="none"/>`;
    }
  }

  const topItem = items[items.length - 1];
  if (beat.dots >= 1) {
    const dotY = (topItem.clampedStep % 2 === 0) ? (topItem.ny - 2.5) : topItem.ny;
    const baseDotX = (topItem.headRight ? cx + 9.5 : cx) + 9.5;
    out += `<circle cx="${baseDotX.toFixed(1)}" cy="${dotY.toFixed(1)}" r="1.7" fill="${stemCol}" pointer-events="none"/>`;
    if (beat.dots >= 2) {
      out += `<circle cx="${(baseDotX + 4.5).toFixed(1)}" cy="${dotY.toFixed(1)}" r="1.7" fill="${stemCol}" pointer-events="none"/>`;
    }
  }

  if (beat.tupletNumerator > 0) {
    const tupY = (dur > -2)
      ? (stemDown ? stemTipY + 10 : stemTipY - 4)
      : (minY - 8);
    out += `<text x="${cx.toFixed(1)}" y="${tupY.toFixed(1)}" fill="#4ade80" font-size="9.5" font-weight="800" text-anchor="middle" pointer-events="none">${beat.tupletNumerator}</text>`;
  }

  if (showNoteNames) {
    const nameY = Math.min(staffTopY - 8, minY - 9);
    if (isKeys) {
      const uniq = [];
      for (const it of items) {
        const lbl = midiToNoteWithOctave(it.midi, accMode);
        if (!uniq.includes(lbl)) uniq.push(lbl);
      }
      const txt = uniq.length <= 2 ? uniq.join('·') : `${uniq.slice(0, 2).join('·')}…`;
      const hasRoot = items.some(it => ((it.midi % 12) + 12) % 12 === keyRootPc);
      const col = isBeatPlaying ? '#4ade80' : (hasRoot ? '#f39237' : 'var(--accent)');
      out += `<text x="${cx.toFixed(1)}" y="${nameY.toFixed(1)}" fill="${col}" font-size="${uniq.length > 1 ? 8.5 : 9.5}" font-weight="800" text-anchor="middle" pointer-events="none">${escapeHtml(txt)}</text>`;
    } else {
      const uniq = [];
      for (const it of items) {
        const lbl = midiToNoteName(it.midi, accMode);
        if (!uniq.includes(lbl)) uniq.push(lbl);
      }
      const txt = uniq.length <= 3 ? uniq.join('·') : `${uniq.slice(0, 3).join('·')}+`;
      const hasRoot = items.some(it => ((it.midi % 12) + 12) % 12 === keyRootPc);
      const col = isBeatPlaying ? '#4ade80' : (hasRoot ? '#f39237' : 'var(--text)');
      out += `<text x="${cx.toFixed(1)}" y="${nameY.toFixed(1)}" fill="${col}" font-size="${uniq.length > 1 ? 8.5 : 9.5}" font-weight="${hasRoot ? 800 : 700}" text-anchor="middle" pointer-events="none">${escapeHtml(txt)}</text>`;
    }
  }

  return out;
}

function getTechniqueMarkers(beat) {
  if (!beat) return [];
  const tags = new Set();
  if (beat.fermata) tags.add('𝄐');
  if (beat.pickStroke === 1) tags.add('⊓');
  else if (beat.pickStroke === 2) tags.add('∨');
  if (beat.brushStroke) tags.add('↓↑');

  if (!beat.isRest && Array.isArray(beat.notes)) {
    const bendShort = { half: '+½', full: '+1', one_half: '+1½', bend_release: 'B+R', pre_bend: 'Pre' };
    for (const n of beat.notes) {
      if (n.palmMute) tags.add('P.M.');
      if (n.letRing) tags.add('let ring');
      if (n.hammerOn) tags.add('H/P');
      if (n.slideType === 'legato') tags.add('⌒/');
      else if (n.slideType === 'in_below') tags.add('/0');
      else if (n.slideType === 'out_down') tags.add('0\\');
      else if (n.slide || n.slideType) tags.add('/');
      if (n.wideVibrato) tags.add('w~');
      else if (n.vibrato) tags.add('~');
      if (n.bendPreset && bendShort[n.bendPreset]) tags.add(`↗${bendShort[n.bendPreset]}`);
      else if (Array.isArray(n.bendPoints) && n.bendPoints.length > 0) tags.add('↗');
      if (n.harmonicType === 1) tags.add('◇');
      else if (n.harmonicType > 1) tags.add('◆');
      if (n.tied) tags.add('⌣');
      if (n.accented) tags.add('>');
      if (n.heavyAccented) tags.add('^');
      if (n.staccato) tags.add('stacc');
      if (n.tap) tags.add('TAP');
      if (n.graceNote) tags.add('♪');
      if (n.tremoloPicking) tags.add('///');
      if (n.trill) tags.add('tr');
    }
  }
  return Array.from(tags);
}

function formatVoltaText(mask) {
  if (!mask) return '';
  const nums = [];
  for (let i = 0; i < 8; i++) {
    if (mask & (1 << i)) nums.push(`${i + 1}.`);
  }
  return nums.join(' ');
}

/**
 * Renderuje główny wielotaktowy zapis TAB, Nuty (pięciolinia) lub Nuty+TAB — dla jednej lub wszystkich ścieżek naraz.
 */
export function renderStudioTabSvg(song, opts = {}) {
  const tracks = song?.tracks || [];
  if (tracks.length === 0) return '<svg width="600" height="140"></svg>';
  const activeTrackIdx = Math.max(0, Math.min(tracks.length - 1, Number(opts.activeTrackIdx) || 0));
  const primaryTrack = tracks[activeTrackIdx] || tracks[0];

  const cursor = opts.cursor || { measureIdx: 0, beatIdx: 0, stringIdx: 0 };
  const playhead = opts.playhead || { active: false, measureIdx: 0, beatIdx: 0, progress: 0 };
  const viewMode = (opts.viewMode === 'staff' || opts.viewMode === 'staff_tab') ? opts.viewMode : 'tab';
  const accMode = Number(opts.accMode) === 1 ? 1 : 2;
  const loopEnabled = Boolean(opts.loopEnabled);
  const loopStart = Math.min(Number(opts.loopStart) || 0, Number(opts.loopEnd) || 0);
  const loopEnd = Math.max(Number(opts.loopStart) || 0, Number(opts.loopEnd) || 0);
  const showNoteNames = opts.showNoteNames !== false;
  const keyRootPc = Number.isFinite(Number(song?.keyRootPc)) ? ((Number(song.keyRootPc) % 12) + 12) % 12 : 4;

  const normSel = normalizeBeatSelection(song, {
    activeTrackIdx,
    beatSelection: opts.beatSelection || null,
    selectionStart: opts.selectionStart ?? null,
    selectionEnd: opts.selectionEnd ?? null
  });
  const isAllTracks = Boolean(opts.allTracks && tracks.length > 0);
  const trackIndices = isAllTracks ? tracks.map((_, idx) => idx) : [activeTrackIdx];

  const laneModeFor = (tr) => {
    const isDrum = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
    const isKeys = tr.instrumentType === 'keys';
    const isBassClef = tr.instrumentType === 'bass';
    if (isDrum) {
      return { isDrum: true, isKeys: false, isBassClef: false, showStaff: false, showTabLines: true, nStrings: 4, tabLineGap: 18 };
    }
    if (isKeys) {
      return { isDrum: false, isKeys: true, isBassClef: false, showStaff: true, showTabLines: false, nStrings: 0, tabLineGap: 16 };
    }
    const nStrings = Math.max(4, tr.openStringMidis?.length || tr.stringCount || 6);
    if (viewMode === 'staff') {
      return { isDrum: false, isKeys: false, isBassClef, showStaff: true, showTabLines: false, nStrings, tabLineGap: 16 };
    }
    if (viewMode === 'staff_tab') {
      return { isDrum: false, isKeys: false, isBassClef, showStaff: true, showTabLines: true, nStrings, tabLineGap: 16 };
    }
    return { isDrum: false, isKeys: false, isBassClef, showStaff: false, showTabLines: true, nStrings, tabLineGap: 16 };
  };

  const primaryLayout = laneModeFor(primaryTrack);
  const totalBars = Math.max(1, getMasterMeasureCount(song));
  const containerW = Math.max(600, Number(opts.containerWidth) || 960);

  let barsPerRow = 4;
  if (opts.barsPerRow && opts.barsPerRow !== 'auto') {
    barsPerRow = Math.max(1, Math.min(8, Number(opts.barsPerRow) || 4));
  } else {
    barsPerRow = containerW >= 1020 ? 4 : (containerW >= 740 ? 3 : 2);
  }

  const leftHeadW = isAllTracks ? 112 : (primaryLayout.showStaff ? 62 : 46);
  const rightPad = 14;
  const rowContentW = Math.max(460, containerW - leftHeadW - rightPad);
  const staffLineGap = 10;

  const laneGeomList = trackIndices.map((tIdx, lanePos) => {
    const tr = tracks[tIdx];
    const lm = laneModeFor(tr);
    const isTopLane = lanePos === 0;
    const laneHeaderH = isTopLane ? (lm.showStaff ? 54 : 42) : (lm.showStaff ? 34 : 20);
    const staffH = lm.showStaff ? 4 * staffLineGap : 0;
    const staffBottomGap = (lm.showStaff && lm.showTabLines) ? 28 : 0;
    const tabH = lm.showTabLines ? Math.max(1, lm.nStrings - 1) * lm.tabLineGap : 0;
    const footerBandH = (!lm.showTabLines && lm.showStaff) ? 38 : 22;
    const totalH = laneHeaderH + staffH + staffBottomGap + tabH + footerBandH;
    return { tIdx, tr, lm, isTopLane, laneHeaderH, staffH, staffBottomGap, tabH, footerBandH, totalH };
  });

  const numRows = Math.ceil(totalBars / barsPerRow);
  const svgW = leftHeadW + rowContentW + rightPad;
  const svgParts = [];
  let rowTop = 10;
  let playheadGeom = null;

  for (let row = 0; row < numRows; row++) {
    const startBar = row * barsPerRow;
    const endBar = Math.min(totalBars, startBar + barsPerRow);
    const barsInThisRow = endBar - startBar;

    // Zsynchronizowane szerokości taktów względem najszerszej ze ścieżek w wierszu
    const rawWidths = [];
    let rawRowSum = 0;
    for (let mIdx = startBar; mIdx < endBar; mIdx++) {
      let maxPrefW = 130;
      for (const lg of laneGeomList) {
        const m = lg.tr.measures?.[mIdx];
        if (!m) continue;
        if (lg.lm.isDrum) {
          maxPrefW = Math.max(maxPrefW, 180);
        } else if (Array.isArray(m.beats) && m.beats.length > 0) {
          const w = 26 + m.beats.reduce((acc, b) => acc + Math.max(28, durationToQuarterNotes(b) * 44), 0);
          maxPrefW = Math.max(maxPrefW, w);
        }
      }
      rawWidths.push(maxPrefW);
      rawRowSum += maxPrefW;
    }

    const targetRowW = (barsInThisRow < barsPerRow)
      ? Math.min(rowContentW, (rowContentW / barsPerRow) * barsInThisRow * 1.15)
      : rowContentW;

    const barXList = [];
    const barWList = [];
    let bxAcc = leftHeadW;
    for (let i = 0; i < barsInThisRow; i++) {
      const w = (rawWidths[i] / Math.max(1, rawRowSum)) * targetRowW;
      barXList.push(bxAcc);
      barWList.push(w);
      bxAcc += w;
    }

    let laneTop = rowTop;
    let firstSystemTopY = null;
    let lastSystemBotY = null;

    laneGeomList.forEach((lg, lanePos) => {
      const { tIdx, tr, lm, isTopLane, laneHeaderH, staffH, staffBottomGap, tabH } = lg;
      const measures = tr.measures || [];
      const staffTopY = laneTop + laneHeaderH;
      const tabTopY = staffTopY + staffH + staffBottomGap;
      const systemTopY = lm.showStaff ? staffTopY : tabTopY;
      const systemBotY = lm.showTabLines ? (tabTopY + tabH) : (staffTopY + staffH);
      if (firstSystemTopY === null) firstSystemTopY = systemTopY;
      lastSystemBotY = systemBotY;

      const isActiveTrack = tIdx === activeTrackIdx;
      const laneOpacity = (isAllTracks && tr.muted) ? '0.45' : '1';
      svgParts.push(`<g class="studio-track-lane ${isActiveTrack ? 'active-lane' : ''}" data-track="${tIdx}" opacity="${laneOpacity}">`);

      if (isAllTracks) {
        const laneBgFill = isActiveTrack ? 'rgba(245,158,11,0.045)' : 'transparent';
        svgParts.push(`<rect x="4" y="${(systemTopY - 12).toFixed(1)}" width="${(leftHeadW - 8).toFixed(1)}" height="${(systemBotY - systemTopY + 24).toFixed(1)}" rx="5" fill="${laneBgFill}" pointer-events="none"/>`);
        svgParts.push(`<rect x="4" y="${(systemTopY - 8).toFixed(1)}" width="4" height="${(systemBotY - systemTopY + 16).toFixed(1)}" rx="2" fill="${escapeHtml(tr.color || '#f39237')}"/>`);
        const shortName = String(tr.name || `Ścieżka ${tIdx + 1}`).slice(0, 12);
        svgParts.push(`<text x="13" y="${(systemTopY + 10).toFixed(1)}" fill="${isActiveTrack ? 'var(--accent)' : 'var(--text)'}" font-size="10.5" font-weight="${isActiveTrack ? 800 : 700}">${escapeHtml(shortName)}</text>`);
        const stateBadge = tr.solo ? 'SOLO' : (tr.muted ? 'MUTE' : (lm.isDrum ? 'Perkusja' : (lm.isKeys ? 'Klawisze' : `${lm.nStrings}-str.`)));
        const badgeCol = tr.solo ? '#eab308' : (tr.muted ? '#ef4444' : 'var(--text-muted)');
        svgParts.push(`<text x="13" y="${(systemTopY + 23).toFixed(1)}" fill="${badgeCol}" font-size="9" font-weight="700">${escapeHtml(stateBadge)}</text>`);
      }

      if (lm.showStaff) {
        for (let l = 0; l < 5; l++) {
          const ly = staffTopY + l * staffLineGap;
          svgParts.push(`<line x1="${leftHeadW}" y1="${ly}" x2="${leftHeadW + targetRowW}" y2="${ly}" stroke="var(--border)" stroke-width="1.15" opacity="0.78"/>`);
        }
        const clefChar = lm.isBassClef ? '𝄢' : '𝄞';
        const clefX = leftHeadW - 22;
        svgParts.push(`<text x="${clefX}" y="${(staffTopY + (lm.isBassClef ? 30 : 35)).toFixed(1)}" fill="var(--text)" font-size="33" text-anchor="middle" opacity="0.90">${clefChar}</text>`);
        if (tr.instrumentType === 'guitar' || tr.instrumentType === 'bass') {
          svgParts.push(`<text x="${clefX}" y="${(staffTopY + 46).toFixed(1)}" fill="var(--text-muted)" font-size="8.5" font-weight="800" text-anchor="middle">8</text>`);
        }
      }

      if (lm.showTabLines) {
        const stringLabels = lm.isDrum
          ? ['CYM', 'SNR', 'TOM', 'KCK']
          : (tr.openStringMidis || []).slice().reverse().map(m => midiToNoteName(m, accMode));
        for (let lineIdx = 0; lineIdx < lm.nStrings; lineIdx++) {
          const ly = tabTopY + lineIdx * lm.tabLineGap;
          svgParts.push(`<line x1="${leftHeadW}" y1="${ly}" x2="${leftHeadW + targetRowW}" y2="${ly}" stroke="var(--border)" stroke-width="1.15"/>`);
          const lbl = stringLabels[lineIdx] || '';
          svgParts.push(`<text x="${leftHeadW - 8}" y="${ly + 3.5}" fill="var(--text-muted)" font-family="Segoe UI, sans-serif" font-size="10.5" font-weight="700" text-anchor="end">${escapeHtml(lbl)}</text>`);
        }
        if (!lm.isDrum && !isAllTracks) {
          svgParts.push(`<text x="12" y="${tabTopY + tabH / 2 + 4}" fill="var(--accent)" font-family="Segoe UI, sans-serif" font-size="11" font-weight="900" opacity="0.85">TAB</text>`);
        }
      }

      svgParts.push(`<line x1="${leftHeadW}" y1="${systemTopY}" x2="${leftHeadW}" y2="${systemBotY}" stroke="var(--text-muted)" stroke-width="1.8"/>`);

      for (let i = 0; i < barsInThisRow; i++) {
        const mIdx = startBar + i;
        const m = measures[mIdx] || { numerator: 4, denominator: 4, beats: [] };
        const curX = barXList[i];
        const barW = barWList[i];
        const barRight = curX + barW;
        const prevM = mIdx > 0 ? measures[mIdx - 1] : null;
        const showTimeSig = (mIdx === 0) || (prevM && (prevM.numerator !== m.numerator || prevM.denominator !== m.denominator));
        const inLoop = isTopLane && loopEnabled && mIdx >= loopStart && mIdx <= loopEnd;
        const inMeasureSel = Boolean(normSel && normSel.trackIdx === tIdx && normSel.kind === 'measures' && mIdx >= normSel.startMeasure && mIdx <= normSel.endMeasure);

        svgParts.push(`<g class="studio-bar" data-track="${tIdx}" data-measure="${mIdx}" data-x="${curX.toFixed(1)}" data-w="${barW.toFixed(1)}" data-y1="${(systemTopY - 6).toFixed(1)}" data-y2="${(systemBotY + 6).toFixed(1)}" data-tab-top="${tabTopY.toFixed(1)}" data-tab-gap="${lm.tabLineGap}" data-nstrings="${lm.nStrings}">`);

        if (inMeasureSel) {
          svgParts.push(`<rect x="${curX.toFixed(1)}" y="${(systemTopY - 4).toFixed(1)}" width="${barW.toFixed(1)}" height="${(systemBotY - systemTopY + 8).toFixed(1)}" rx="4" fill="rgba(245,158,11,0.09)" stroke="rgba(245,158,11,0.45)" stroke-width="1" pointer-events="none"/>`);
        }

        if (inLoop) {
          svgParts.push(`<rect x="${curX.toFixed(1)}" y="${(laneTop + 1).toFixed(1)}" width="${barW.toFixed(1)}" height="3" rx="1.5" fill="#f59e0b" opacity="0.88"/>`);
        }

        if (isTopLane) {
          const barNumY = lm.showStaff ? (laneTop + 26) : (systemTopY - 7);
          svgParts.push(`<text x="${(curX + 4).toFixed(1)}" y="${barNumY.toFixed(1)}" fill="var(--text-muted)" font-size="10" font-weight="700">${mIdx + 1}</text>`);
          if (m.sectionMarker) {
            const pillW = Math.max(38, m.sectionMarker.length * 6.5 + 12);
            svgParts.push(`<rect x="${(curX + 20).toFixed(1)}" y="${(laneTop + 5).toFixed(1)}" width="${pillW}" height="15" rx="4" fill="var(--accent-glow)" stroke="var(--accent)" stroke-width="1"/>`);
            svgParts.push(`<text x="${(curX + 26).toFixed(1)}" y="${(laneTop + 15.5).toFixed(1)}" fill="var(--accent)" font-size="9.5" font-weight="800">${escapeHtml(m.sectionMarker)}</text>`);
          }
          if (m.voltaMask > 0) {
            const vTxt = formatVoltaText(m.voltaMask);
            svgParts.push(`<path d="M ${curX + 2} ${laneTop + 16} L ${curX + 2} ${laneTop + 6} L ${barRight - 4} ${laneTop + 6}" fill="none" stroke="var(--text)" stroke-width="1.2"/>`);
            svgParts.push(`<text x="${(curX + 7).toFixed(1)}" y="${(laneTop + 16).toFixed(1)}" fill="var(--text)" font-size="9.5" font-weight="800">${escapeHtml(vTxt)}</text>`);
          }
        }

        let innerLeft = curX + 10;
        if (showTimeSig) {
          const tsY = lm.showStaff ? (staffTopY + staffH / 2) : (tabTopY + tabH / 2);
          svgParts.push(`<text x="${(curX + 14).toFixed(1)}" y="${(tsY - 3).toFixed(1)}" fill="var(--text)" font-size="14" font-weight="900" text-anchor="middle">${m.numerator || 4}</text>`);
          svgParts.push(`<text x="${(curX + 14).toFixed(1)}" y="${(tsY + 13).toFixed(1)}" fill="var(--text)" font-size="14" font-weight="900" text-anchor="middle">${m.denominator || 4}</text>`);
          innerLeft += 18;
        }

        if (m.repeatOpen) {
          svgParts.push(`<line x1="${(curX + 3).toFixed(1)}" y1="${systemTopY}" x2="${(curX + 3).toFixed(1)}" y2="${systemBotY}" stroke="var(--accent)" stroke-width="2.4"/>`);
          svgParts.push(`<circle cx="${(curX + 8.5).toFixed(1)}" cy="${((systemTopY + systemBotY) / 2 - 7).toFixed(1)}" r="2.2" fill="var(--accent)"/>`);
          svgParts.push(`<circle cx="${(curX + 8.5).toFixed(1)}" cy="${((systemTopY + systemBotY) / 2 + 7).toFixed(1)}" r="2.2" fill="var(--accent)"/>`);
        }

        const innerW = Math.max(40, barRight - innerLeft - 10);

        if (lm.isDrum) {
          const block = getDrumBlockById(m.drumBlockId || 'rock_8th');
          const autoFillN = Number(tr.drumAutoFill) || 0;
          const isFill = autoFillN > 0 && ((mIdx + 1) % autoFillN === 0);
          const steps = resolveDrumMeasureSteps(m, tr, mIdx);
          const capQ = Math.max(1, (m.numerator || 4) * (4 / (m.denominator || 4)));

          svgParts.push(`<rect class="studio-beat-hit" data-track="${tIdx}" data-measure="${mIdx}" data-beat="0" x="${curX.toFixed(1)}" y="${systemTopY}" width="${barW.toFixed(1)}" height="${tabH}" fill="transparent" style="cursor:pointer;"/>`);
          const barTitle = m.customDrumGrid ? `Własny (${block.name})` : `${block.name}${isFill ? ' · Fill' : ''}`;
          svgParts.push(`<text x="${(curX + barW / 2).toFixed(1)}" y="${(systemTopY - 8).toFixed(1)}" fill="var(--accent)" font-size="10" font-weight="700" text-anchor="middle">${escapeHtml(barTitle)}</text>`);

          steps.forEach((step, stIdx) => {
            const sx = innerLeft + (step.beatOffsetQ / capQ) * innerW;
            const isCur = isActiveTrack && cursor.measureIdx === mIdx && cursor.beatIdx === stIdx;
            if (isCur) {
              svgParts.push(`<rect x="${(sx - 9).toFixed(1)}" y="${(tabTopY - 8).toFixed(1)}" width="18" height="${(tabH + 16).toFixed(1)}" rx="5" fill="url(#studioCursorGrad)" stroke="#f59e0b" stroke-width="1.6"/>`);
            }
            for (const h of step.hits) {
              let rowIdx = 0;
              let sym = '×';
              let col = '#38bdf8';
              if (h.kind === 'kick') { rowIdx = 3; sym = '■'; col = '#f39237'; }
              else if (h.kind === 'tom_low' || h.kind === 'tom_high') { rowIdx = 2; sym = '◎'; col = '#a855f7'; }
              else if (h.kind === 'snare') { rowIdx = 1; sym = '●'; col = '#22c55e'; }
              else if (h.kind === 'crash') { rowIdx = 0; sym = '✳'; col = '#eab308'; }
              const py = tabTopY + rowIdx * lm.tabLineGap;
              svgParts.push(`<text x="${sx.toFixed(1)}" y="${(py + 4).toFixed(1)}" fill="${col}" font-size="12" font-weight="900" text-anchor="middle" pointer-events="none">${sym}</text>`);
            }
          });
        } else {
          const beats = m.beats || [];
          const beatUnits = beats.map(b => Math.max(0.25, durationToQuarterNotes(b)));
          const unitSum = Math.max(0.25, beatUnits.reduce((a, b) => a + b, 0));

          let bx = innerLeft;
          for (let bIdx = 0; bIdx < beats.length; bIdx++) {
            const beat = beats[bIdx];
            const bw = (beatUnits[bIdx] / unitSum) * innerW;
            const cx = bx + bw * 0.45;
            const isBeatCursor = isActiveTrack && (cursor.measureIdx === mIdx && cursor.beatIdx === bIdx);
            const isBeatPlaying = (playhead.active && playhead.measureIdx === mIdx && playhead.beatIdx === bIdx);
            const isBeatSel = Boolean(normSel && normSel.trackIdx === tIdx && isBeatInSelection(normSel, mIdx, bIdx));

            svgParts.push(`<g class="studio-beat" data-track="${tIdx}" data-measure="${mIdx}" data-beat="${bIdx}" data-x="${bx.toFixed(1)}" data-w="${bw.toFixed(1)}">`);
            svgParts.push(`<rect class="studio-beat-sel-bg" data-track="${tIdx}" data-measure="${mIdx}" data-beat="${bIdx}" x="${bx.toFixed(1)}" y="${(systemTopY - 6).toFixed(1)}" width="${bw.toFixed(1)}" height="${(systemBotY - systemTopY + 12).toFixed(1)}" rx="4" fill="rgba(245,158,11,0.20)" stroke="rgba(245,158,11,0.68)" stroke-width="1.2" opacity="${isBeatSel ? '1' : '0'}" pointer-events="none"/>`);
            svgParts.push(`<rect class="studio-beat-hit" data-track="${tIdx}" data-measure="${mIdx}" data-beat="${bIdx}" x="${bx.toFixed(1)}" y="${(systemTopY - 8).toFixed(1)}" width="${bw.toFixed(1)}" height="${(systemBotY - systemTopY + 16).toFixed(1)}" fill="transparent" style="cursor:pointer;"/>`);

            const chordY = lm.showStaff ? (staffTopY - 26) : (laneTop + 29);
            if (beat.chordSymbol) {
              svgParts.push(`<text x="${cx.toFixed(1)}" y="${chordY.toFixed(1)}" fill="var(--text)" font-size="10.5" font-weight="800" text-anchor="middle">${escapeHtml(beat.chordSymbol)}</text>`);
            }
            if (beat.text) {
              const txtY = beat.chordSymbol ? (chordY + 10) : chordY;
              svgParts.push(`<text x="${cx.toFixed(1)}" y="${txtY.toFixed(1)}" fill="var(--accent)" font-size="9" font-style="italic" text-anchor="middle">${escapeHtml(beat.text)}</text>`);
            }

            if (isBeatCursor) {
              if (lm.showTabLines) {
                const targetS = Math.max(0, Math.min(lm.nStrings - 1, Number(cursor.stringIdx) || 0));
                const slotRow = lm.nStrings - 1 - targetS;
                const slotY = tabTopY + slotRow * lm.tabLineGap;
                svgParts.push(`<rect class="studio-cursor-slot" x="${(cx - 11).toFixed(1)}" y="${(slotY - 8.5).toFixed(1)}" width="22" height="17" rx="5" fill="url(#studioCursorGrad)" stroke="#f59e0b" stroke-width="2" pointer-events="none"/>`);
              } else {
                svgParts.push(`<rect class="studio-cursor-slot" x="${(cx - 13).toFixed(1)}" y="${(staffTopY - 6).toFixed(1)}" width="26" height="${(staffH + 12).toFixed(1)}" rx="6" fill="url(#studioCursorGrad)" stroke="#f59e0b" stroke-width="2" pointer-events="none"/>`);
              }
            } else if (isBeatPlaying) {
              svgParts.push(`<rect x="${(cx - 10).toFixed(1)}" y="${(systemTopY - 4).toFixed(1)}" width="20" height="${(systemBotY - systemTopY + 8).toFixed(1)}" rx="4" fill="rgba(34,197,94,0.14)" pointer-events="none"/>`);
            }

            if (lm.showStaff) {
              svgParts.push(renderBeatOnStaff({
                track: tr,
                beat,
                cx,
                staffTopY,
                isBassClef: lm.isBassClef,
                accMode,
                isBeatPlaying,
                isBeatCursor,
                isBeatSelected: isBeatSel,
                keyRootPc,
                showNoteNames,
                isKeys: lm.isKeys
              }));
            }

            if (lm.showTabLines) {
              if (beat.isRest || !beat.notes || beat.notes.length === 0) {
                const midY = tabTopY + tabH / 2;
                svgParts.push(`<rect x="${(cx - 5).toFixed(1)}" y="${(midY - 2.5).toFixed(1)}" width="10" height="5" rx="1.5" fill="var(--text-muted)" opacity="0.72" pointer-events="none"/>`);
              } else {
                for (const n of beat.notes) {
                  if (n.string < 0 || n.string >= lm.nStrings) continue;
                  const rowIdx = lm.nStrings - 1 - n.string;
                  const ny = tabTopY + rowIdx * lm.tabLineGap;
                  const label = n.deadNote ? 'X' : (n.ghostNote ? `(${n.fret})` : String(n.fret));
                  const boxW = Math.max(14, label.length * 7.2 + 5);
                  svgParts.push(`<rect x="${(cx - boxW / 2).toFixed(1)}" y="${(ny - 6.5).toFixed(1)}" width="${boxW}" height="13" rx="3" fill="var(--bg-card)" pointer-events="none"/>`);
                  svgParts.push(`<text x="${cx.toFixed(1)}" y="${(ny + 4).toFixed(1)}" fill="${isBeatPlaying ? '#22c55e' : 'var(--text)'}" font-family="Consolas, 'JetBrains Mono', monospace" font-size="11.5" font-weight="800" text-anchor="middle" pointer-events="none">${escapeHtml(label)}</text>`);
                }
              }
            }

            const techTags = getTechniqueMarkers(beat);
            const footerOffset = lm.showTabLines ? 13 : 28;
            if (techTags.length > 0) {
              svgParts.push(`<text x="${cx.toFixed(1)}" y="${(systemBotY + footerOffset).toFixed(1)}" fill="var(--accent)" font-size="9" font-weight="700" text-anchor="middle">${escapeHtml(techTags.join(' '))}</text>`);
            }
            if (lm.showTabLines && beat.duration >= 0) {
              const stemTop = systemBotY + 4;
              svgParts.push(`<line x1="${cx.toFixed(1)}" y1="${stemTop}" x2="${cx.toFixed(1)}" y2="${stemTop + 10}" stroke="var(--text-muted)" stroke-width="1.2" opacity="0.65"/>`);
              if (beat.duration >= 1) {
                svgParts.push(`<line x1="${cx.toFixed(1)}" y1="${stemTop + 10}" x2="${(cx + 6).toFixed(1)}" y2="${stemTop + 10}" stroke="var(--text-muted)" stroke-width="1.6" opacity="0.75"/>`);
              }
              if (beat.duration >= 2) {
                svgParts.push(`<line x1="${cx.toFixed(1)}" y1="${stemTop + 7}" x2="${(cx + 6).toFixed(1)}" y2="${stemTop + 7}" stroke="var(--text-muted)" stroke-width="1.4" opacity="0.75"/>`);
              }
              if (beat.dots >= 1) {
                svgParts.push(`<circle cx="${(cx + 9).toFixed(1)}" cy="${stemTop + 5}" r="1.4" fill="var(--text-muted)"/>`);
              }
              if (beat.dots >= 2) {
                svgParts.push(`<circle cx="${(cx + 13).toFixed(1)}" cy="${stemTop + 5}" r="1.4" fill="var(--text-muted)"/>`);
              }
              if (beat.tupletNumerator > 0) {
                svgParts.push(`<text x="${(cx - 7).toFixed(1)}" y="${stemTop + 10}" fill="var(--accent)" font-size="8.5" font-weight="800" text-anchor="end">${beat.tupletNumerator}</text>`);
              }
            }

            svgParts.push(`</g>`);
            bx += bw;
          }

          const meter = validateMeasureMeter(m);
          const meterCol = meter.status === 'ok' ? '#22c55e' : (meter.status === 'under' ? '#eab308' : '#ef4444');
          const meterY = systemBotY + (lm.showTabLines ? 18 : 34);
          svgParts.push(`<line x1="${(curX + 2).toFixed(1)}" y1="${meterY.toFixed(1)}" x2="${(barRight - 2).toFixed(1)}" y2="${meterY.toFixed(1)}" stroke="${meterCol}" stroke-width="2" opacity="0.55"/>`);
        }

        if (m.repeatClose >= 2) {
          svgParts.push(`<circle cx="${(barRight - 8.5).toFixed(1)}" cy="${((systemTopY + systemBotY) / 2 - 7).toFixed(1)}" r="2.2" fill="var(--accent)"/>`);
          svgParts.push(`<circle cx="${(barRight - 8.5).toFixed(1)}" cy="${((systemTopY + systemBotY) / 2 + 7).toFixed(1)}" r="2.2" fill="var(--accent)"/>`);
          svgParts.push(`<line x1="${(barRight - 3).toFixed(1)}" y1="${systemTopY}" x2="${(barRight - 3).toFixed(1)}" y2="${systemBotY}" stroke="var(--accent)" stroke-width="2.4"/>`);
          if (isTopLane) {
            svgParts.push(`<text x="${(barRight - 14).toFixed(1)}" y="${(systemTopY - 8).toFixed(1)}" fill="var(--accent)" font-size="10" font-weight="800" text-anchor="end">×${m.repeatClose}</text>`);
          }
        }
        svgParts.push(`<line x1="${barRight.toFixed(1)}" y1="${systemTopY}" x2="${barRight.toFixed(1)}" y2="${systemBotY}" stroke="var(--text-muted)" stroke-width="${mIdx === totalBars - 1 ? 2.6 : 1.3}"/>`);
        svgParts.push(`</g>`);
      }

      svgParts.push(`</g>`);
      laneTop += lg.totalH + (isAllTracks && lanePos < laneGeomList.length - 1 ? 6 : 0);
    });

    const rowSysTop = firstSystemTopY ?? rowTop;
    const rowSysBot = lastSystemBotY ?? (laneTop - 10);
    for (let i = 0; i < barsInThisRow; i++) {
      const mIdx = startBar + i;
      const curX = barXList[i];
      const barW = barWList[i];
      svgParts.push(`<g class="studio-playhead-bounds" data-measure="${mIdx}" data-x="${curX.toFixed(1)}" data-w="${barW.toFixed(1)}" data-y1="${(rowSysTop - 6).toFixed(1)}" data-y2="${(rowSysBot + 6).toFixed(1)}"></g>`);
      if (playhead.active && playhead.measureIdx === mIdx) {
        const px = curX + Math.max(0, Math.min(1, playhead.progress || 0)) * barW;
        playheadGeom = { x: px, y1: rowSysTop - 8, y2: rowSysBot + 8 };
      }
    }
    if (isAllTracks && laneGeomList.length > 1) {
      svgParts.push(`<line x1="${leftHeadW}" y1="${rowSysTop.toFixed(1)}" x2="${leftHeadW}" y2="${rowSysBot.toFixed(1)}" stroke="var(--text-muted)" stroke-width="2.2" opacity="0.65"/>`);
    }

    rowTop = laneTop + (numRows > 1 ? 10 : 4);
  }

  const svgH = Math.max(148, Math.ceil(rowTop + 6));
  const ph = playheadGeom || { x: leftHeadW + 10, y1: 30, y2: 120 };
  const header = `<svg id="studioTabSvg" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${svgW} ${svgH}" width="${svgW}" height="${svgH}">` +
    `<defs><linearGradient id="studioCursorGrad" x1="0" y1="0" x2="0" y2="1"><stop offset="0%" stop-color="rgba(245,158,11,0.34)"/><stop offset="100%" stop-color="rgba(245,158,11,0.16)"/></linearGradient></defs>`;
  return header + svgParts.join('') +
    `<line id="studioPlayheadLine" class="studio-playhead-line" x1="${ph.x.toFixed(1)}" y1="${ph.y1.toFixed(1)}" x2="${ph.x.toFixed(1)}" y2="${ph.y2.toFixed(1)}" stroke="#f59e0b" stroke-width="2.4" opacity="${playhead.active ? '0.95' : '0'}"/></svg>`;
}

/**
 * Renderuje animowany kolorowy gryf SVG (lub pady/podgląd klawiatury) dla Strefy E.
 */
export function renderStudioFretboardSvg(track, opts = {}) {
  const activeHits = Array.isArray(opts.activeHits) ? opts.activeHits : [];
  const nextHits = Array.isArray(opts.nextHits) ? opts.nextHits : [];
  const activeNotes = Array.isArray(opts.activeNotes) ? opts.activeNotes : [];
  const nextNotes = Array.isArray(opts.nextNotes) ? opts.nextNotes : [];
  const accMode = Number(opts.accMode) === 1 ? 1 : 2;

  if (!track || track.instrumentType === 'drums' || track.isPercussion) {
    const m = opts.measure || track?.measures?.[0] || { numerator: 4, denominator: 4, drumBlockId: 'rock_8th' };
    const mIdx = Number(opts.measureIdx) || 0;
    const num = Math.max(1, Math.min(16, Number(m.numerator) || 4));
    const den = Number(m.denominator) || 4;
    const autoFillN = Number(track?.drumAutoFill) || 0;
    const isFillBar = autoFillN > 0 && ((mIdx + 1) % autoFillN === 0);
    const grid = (m.customDrumGrid && m.customDrumGrid.lanes)
      ? m.customDrumGrid
      : drumBlockToGridPattern(m.drumBlockId || 'rock_8th', {
          numerator: num,
          denominator: den,
          isBarOne: mIdx === 0,
          isFillBar,
          crashOnOne: Boolean(track?.drumCrashOnOne)
        });
    const brush = Number(opts.drumBrushLevel) || 2;
    const playStep = Number.isInteger(opts.playingStepIdx) ? opts.playingStepIdx : -1;
    const spb = grid.stepsPerBeat || 2;
    const totalSteps = grid.stepCount || (num * spb);
    const blockId = m.drumBlockId || 'rock_8th';

    const beatHeaders = Array.from({ length: totalSteps }, (_, sIdx) => {
      const isBeatStart = (sIdx % spb) === 0;
      const lbl = isBeatStart ? String(Math.floor(sIdx / spb) + 1) : '·';
      return `<div class="studio-drum-seq-colnum ${isBeatStart ? 'beat-start' : ''} ${sIdx === playStep ? 'playing-step' : ''}">${lbl}</div>`;
    }).join('');

    const laneRows = DRUM_GRID_LANES.map(lane => {
      const isAct = activeHits.includes(lane.normal) || activeHits.includes(lane.accent);
      const isNext = !isAct && (nextHits.includes(lane.normal) || nextHits.includes(lane.accent));
      const padCls = ['studio-drum-pad', isAct ? 'active' : '', isNext ? 'next' : ''].filter(Boolean).join(' ');
      const arr = grid.lanes?.[lane.id] || [];
      const cells = Array.from({ length: totalSteps }, (_, sIdx) => {
        const lvl = arr[sIdx] || 0;
        const isBeatStart = (sIdx % spb) === 0;
        const sym = lvl === 3 ? '&gt;' : (lvl === 2 ? '■' : (lvl === 1 ? '·' : ''));
        return `<button type="button" class="studio-drum-cell lvl-${lvl} ${isBeatStart ? 'beat-start' : ''} ${sIdx === playStep ? 'playing-step' : ''}" style="--lane-col:${lane.color};" data-action="drum-cell" data-lane="${lane.id}" data-step="${sIdx}" data-tip-name="${escapeHtml(lane.label)} · krok ${sIdx + 1}" data-tip-desc="Kliknij, aby włączyć/wyłączyć uderzenie bieżącym pędzlem (Ghost / Normalny / Akcent).">${sym}</button>`;
      }).join('');

      return `<div class="studio-drum-seq-row">
        <button type="button" class="${padCls}" data-action="drum-pad" data-drum-hit="${lane.normal}" data-tip-name="${escapeHtml(lane.label)}" data-tip-desc="Kliknij, aby odsłuchać brzmienie ścieżki (${escapeHtml(lane.label)}).">${lane.icon} ${escapeHtml(lane.label)}</button>
        <div class="studio-drum-seq-steps" style="grid-template-columns:repeat(${totalSteps}, minmax(0, 1fr));">${cells}</div>
      </div>`;
    }).join('');

    return `<div class="studio-drum-seq">
      <div class="studio-drum-seq-top">
        <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
          <select data-action="drum-seq-preset" data-tip-name="Bazowy rytm perkusji" data-tip-desc="Wczytuje preset rytmu dla taktu ${mIdx + 1} (możesz go później dowolnie modyfikować w kratce poniżej).">
            ${DRUM_BLOCKS.map(b => `<option value="${escapeHtml(b.id)}" ${b.id === blockId ? 'selected' : ''}>Baza: ${escapeHtml(b.name)}</option>`).join('')}
          </select>
          <span class="studio-pal-sep"></span>
          <button type="button" class="studio-btn sm ${brush === 1 ? 'active' : ''}" data-action="drum-seq-brush" data-brush="1" data-tip-name="Pędzel: Ghost" data-tip-desc="Wstawia ciche uderzenie tła (ghost note) w siatkę perkusji.">· Ghost</button>
          <button type="button" class="studio-btn sm ${brush === 2 ? 'active' : ''}" data-action="drum-seq-brush" data-brush="2" data-tip-name="Pędzel: Normalny" data-tip-desc="Wstawia standardowe uderzenie bębna / zamknięty hi-hat / ride.">■ Normalny</button>
          <button type="button" class="studio-btn sm ${brush === 3 ? 'active' : ''}" data-action="drum-seq-brush" data-brush="3" data-tip-name="Pędzel: Akcent" data-tip-desc="Wstawia mocny akcent (Crash zamiast Ride, otwarty Hi-Hat, głośniejszy werbel/stopa).">&gt; Akcent</button>
        </div>
        <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
          <div class="studio-engine-toggle" data-tip-name="Gęstość siatki perkusji" data-tip-desc="Przełącza podział taktu między ósemkami (8 kroków w 4/4) a szesnastkami (16 kroków).">
            <button type="button" class="${spb === 2 ? 'active' : ''}" data-action="drum-seq-res" data-spb="2">1/8 (${num * 2})</button>
            <button type="button" class="${spb === 4 ? 'active' : ''}" data-action="drum-seq-res" data-spb="4">1/16 (${num * 4})</button>
          </div>
          <button type="button" class="studio-btn sm" data-action="drum-seq-copy-all" data-tip-name="Zastosuj na wszystkie takty" data-tip-desc="Kopiuje ułożoną siatkę perkusji z taktu ${mIdx + 1} do wszystkich taktów o tym samym metrum.">📋 Na wszystkie</button>
          ${m.customDrumGrid ? `<button type="button" class="studio-btn sm" data-action="drum-seq-reset" data-tip-name="Przywróć automatyczny klocek" data-tip-desc="Usuwa ręczne modyfikacje i wraca do czystego presetu z Auto-Fill.">↺ Resetuj</button>` : ''}
        </div>
      </div>
      <div class="studio-drum-seq-head">
        <div class="studio-drum-seq-pad-spacer">Takt ${mIdx + 1} ${m.customDrumGrid ? '(własny)' : '(preset)'}</div>
        <div class="studio-drum-seq-steps" style="grid-template-columns:repeat(${totalSteps}, minmax(0, 1fr));">${beatHeaders}</div>
      </div>
      ${laneRows}
    </div>`;
  }

  if (track.instrumentType === 'keys') {
    const actLabels = activeNotes.map(n => midiToNoteWithOctave(n.midi, accMode));
    const nextLabels = nextNotes.map(n => midiToNoteWithOctave(n.midi, accMode));
    return `<div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:10px;padding:10px 12px;">
      <div>
        <div style="font-size:12px;font-weight:800;color:var(--text);">🎹 Klawisze / Synth — zsynchronizowane z dolną klawiaturą MIDI</div>
        <div style="font-size:11.5px;color:var(--text-muted);">Graj na klawiaturze poniżej lub QWERTY — podświetlenie działa automatycznie.</div>
      </div>
      <div style="display:flex;gap:10px;align-items:center;flex-wrap:wrap;">
        <span class="studio-pill-badge">Teraz: <strong>${actLabels.length ? escapeHtml(actLabels.join(' ')) : '—'}</strong></span>
        <span class="studio-pill-badge">Następne: <span style="color:#38bdf8;font-weight:700;">${nextLabels.length ? escapeHtml(nextLabels.join(' ')) : '—'}</span></span>
      </div>
    </div>`;
  }

  const openMidis = track.openStringMidis || [40, 45, 50, 55, 59, 64];
  const nStrings = openMidis.length;
  const capo = Math.max(0, Math.min(12, Number(track.capo) || 0));
  const fretCount = Math.max(12, Math.min(24, Number(opts.fretCount) || 17));
  const keyRootPc = Number.isFinite(Number(opts.keyRootPc)) ? ((Number(opts.keyRootPc) % 12) + 12) % 12 : 4;
  const scalePcs = new Set(Array.isArray(opts.keyScalePcs) ? opts.keyScalePcs : getScaleHintPitchClasses(keyRootPc, 'natural_minor'));
  const showScaleHints = opts.showScaleHints !== false;

  const triadPcs = new Set([
    keyRootPc,
    (keyRootPc + (scalePcs.has((keyRootPc + 3) % 12) ? 3 : 4)) % 12,
    (keyRootPc + 7) % 12
  ]);

  const activePosMap = new Map();
  const activeMidiSet = new Set();
  for (const an of activeNotes) {
    if (Number.isFinite(an.string) && Number.isFinite(an.fret)) {
      activePosMap.set(`${an.string}:${an.fret}`, an);
    }
    if (Number.isFinite(an.midi)) activeMidiSet.add(Math.round(an.midi));
  }

  const nextPosMap = new Map();
  for (const nn of nextNotes) {
    if (Number.isFinite(nn.string) && Number.isFinite(nn.fret)) {
      nextPosMap.set(`${nn.string}:${nn.fret}`, nn);
    }
  }

  const candidateSet = new Set(
    (Array.isArray(opts.candidatePositions) ? opts.candidatePositions : [])
      .map(c => `${c.string}:${c.fret}`)
  );
  const liveMidiSet = new Set(
    (Array.isArray(opts.liveDetectedMidis) ? opts.liveDetectedMidis : Array.from(opts.liveDetectedMidis || []))
      .filter(Number.isFinite)
      .map(m => Math.round(m))
  );
  const posArea = (opts.positionAreaRange && Number.isFinite(Number(opts.positionAreaRange.minFret)))
    ? {
        minFret: Math.max(0, Math.min(fretCount, Math.round(Number(opts.positionAreaRange.minFret)))),
        maxFret: Math.max(0, Math.min(fretCount, Math.round(Number(opts.positionAreaRange.maxFret))))
      }
    : null;

  const svgW = 1240;
  const topPad = 18;
  const botPad = 24;
  const stringGap = nStrings <= 4 ? 34 : 27;
  const svgH = topPad + (nStrings - 1) * stringGap + botPad;
  const nutX = 56;
  const fretW = (svgW - nutX - 18) / fretCount;

  let svg = `<svg id="studioFretboardSvg" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${svgW} ${svgH}" width="100%" height="${svgH}">`;
  svg += `<rect x="${nutX}" y="${topPad - 6}" width="${(svgW - nutX - 12).toFixed(1)}" height="${((nStrings - 1) * stringGap + 12).toFixed(1)}" rx="6" fill="var(--bg-panel)" stroke="var(--border)" stroke-width="1"/>`;

  // Podświetlenie zakresu wybranej pozycji ręki (Pozycja 1 / 2 / 3...)
  if (posArea) {
    const xStart = posArea.minFret === 0 ? (nutX - 24) : (nutX + (posArea.minFret - 1) * fretW + 2);
    const xEnd = posArea.maxFret === 0 ? (nutX - 2) : (nutX + posArea.maxFret * fretW - 2);
    svg += `<rect class="studio-fb-pos-box" x="${xStart.toFixed(1)}" y="${topPad - 8}" width="${Math.max(22, xEnd - xStart).toFixed(1)}" height="${((nStrings - 1) * stringGap + 16).toFixed(1)}" rx="6" fill="rgba(245, 158, 11, 0.14)" stroke="#f59e0b" stroke-width="1.5" stroke-dasharray="4,3"/>`;
  }

  // Progi i znaczniki progów
  const markerFrets = new Set([3, 5, 7, 9, 12, 15, 17, 19, 21, 24]);
  for (let f = 0; f <= fretCount; f++) {
    const fx = nutX + f * fretW;
    svg += `<line x1="${fx.toFixed(1)}" y1="${topPad - 4}" x2="${fx.toFixed(1)}" y2="${(topPad + (nStrings - 1) * stringGap + 4).toFixed(1)}" stroke="${f === 0 ? 'var(--text)' : 'var(--border)'}" stroke-width="${f === 0 ? 4 : 1.4}"/>`;
    if (f > 0) {
      const cx = nutX + (f - 0.5) * fretW;
      if (markerFrets.has(f)) {
        if (f === 12 || f === 24) {
          svg += `<circle cx="${cx.toFixed(1)}" cy="${(topPad + ((nStrings - 1) * stringGap) * 0.3).toFixed(1)}" r="3.6" fill="var(--border)"/>`;
          svg += `<circle cx="${cx.toFixed(1)}" cy="${(topPad + ((nStrings - 1) * stringGap) * 0.7).toFixed(1)}" r="3.6" fill="var(--border)"/>`;
        } else {
          svg += `<circle cx="${cx.toFixed(1)}" cy="${(topPad + ((nStrings - 1) * stringGap) / 2).toFixed(1)}" r="4.0" fill="var(--border)"/>`;
        }
      }
      svg += `<text x="${cx.toFixed(1)}" y="${(svgH - 6).toFixed(1)}" fill="var(--text-muted)" font-size="11" font-weight="800" text-anchor="middle">${f}</text>`;
    }
  }

  // Belka kapodastra
  if (capo > 0 && capo <= fretCount) {
    const cx = nutX + (capo - 0.5) * fretW;
    svg += `<rect x="${(cx - 5).toFixed(1)}" y="${topPad - 9}" width="10" height="${((nStrings - 1) * stringGap + 18).toFixed(1)}" rx="4" fill="#f59e0b" opacity="0.88"/>`;
  }

  // Struny (od najwyższej na górze do najniższej na dole)
  for (let s = 0; s < nStrings; s++) {
    const row = nStrings - 1 - s;
    const cy = topPad + row * stringGap;
    const thickness = 1.2 + (nStrings - 1 - s) * 0.36;
    svg += `<line x1="${nutX - 18}" y1="${cy.toFixed(1)}" x2="${svgW - 14}" y2="${cy.toFixed(1)}" stroke="var(--text-muted)" stroke-width="${thickness.toFixed(2)}" opacity="0.65"/>`;
    const openLbl = midiToNoteName(openMidis[s] + capo, accMode);
    svg += `<text x="20" y="${(cy + 4).toFixed(1)}" fill="var(--text-muted)" font-size="12" font-weight="800" text-anchor="middle">${escapeHtml(openLbl)}</text>`;
  }

  // Nuty na gryfie (tło skali + następne + aktywne + kandydaci)
  for (let s = 0; s < nStrings; s++) {
    const row = nStrings - 1 - s;
    const cy = topPad + row * stringGap;
    for (let f = 0; f <= fretCount; f++) {
      const cx = (f === 0) ? (nutX - 14) : (nutX + (f - 0.5) * fretW);
      const midi = openMidis[s] + capo + f;
      const pc = ((midi % 12) + 12) % 12;
      const noteName = midiToNoteName(midi, accMode);
      const key = `${s}:${f}`;

      const isActiveExact = activePosMap.has(key);
      const isActiveByMidi = !isActiveExact && activePosMap.size === 0 && activeMidiSet.has(midi);
      const isActive = isActiveExact || isActiveByMidi;
      const isNext = nextPosMap.has(key);
      const isCandidate = candidateSet.has(key);
      const isLiveMic = liveMidiSet.has(midi);
      const inScale = scalePcs.has(pc);

      if (isActive) {
        let fill = '#eab308';
        let stroke = '#fef08a';
        let txt = '#14100d';
        if (pc === keyRootPc) { fill = '#f39237'; stroke = '#fff5eb'; }
        else if (triadPcs.has(pc)) { fill = '#22c55e'; stroke = '#bbf7d0'; }
        else if (!inScale) { fill = '#a855f7'; stroke = '#e9d5ff'; txt = '#ffffff'; }
        svg += `<g class="studio-fb-cell active" data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" style="cursor:pointer;">
          ${isNext ? `<circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="13.8" fill="none" stroke="#38bdf8" stroke-width="1.8" stroke-dasharray="3.5,2.5"/>` : ''}
          ${isLiveMic ? `<circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="14.5" fill="none" stroke="#22c55e" stroke-width="1.9"/>` : ''}
          <circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="11.2" fill="${fill}" stroke="${stroke}" stroke-width="2"/>
          <text x="${cx.toFixed(1)}" y="${(cy + 3.8).toFixed(1)}" fill="${txt}" font-size="11" font-weight="900" text-anchor="middle">${escapeHtml(noteName)}</text>
        </g>`;
      } else if (isLiveMic) {
        svg += `<g class="studio-fb-cell detected" data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" style="cursor:pointer;">
          ${isCandidate ? `<circle class="studio-fb-candidate-ring" cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="13.0" stroke-width="1.8"/>` : ''}
          <circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="10.4" fill="#22c55e" stroke="#bbf7d0" stroke-width="2"/>
          <text x="${cx.toFixed(1)}" y="${(cy + 3.6).toFixed(1)}" fill="#052e16" font-size="10.5" font-weight="900" text-anchor="middle">${escapeHtml(noteName)}</text>
        </g>`;
      } else if (isCandidate) {
        svg += `<g class="studio-fb-cell candidate" data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" style="cursor:pointer;">
          <circle class="studio-fb-candidate-ring" cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="10.4" stroke-width="2"/>
          <text x="${cx.toFixed(1)}" y="${(cy + 3.6).toFixed(1)}" fill="#fbbf24" font-size="10.5" font-weight="800" text-anchor="middle">${escapeHtml(noteName)}</text>
        </g>`;
      } else if (isNext) {
        svg += `<g class="studio-fb-cell next" data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" style="cursor:pointer;">
          <circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="10.4" fill="rgba(15, 23, 42, 0.72)" stroke="#38bdf8" stroke-width="2" stroke-dasharray="3.5,2.5"/>
          <text x="${cx.toFixed(1)}" y="${(cy + 3.6).toFixed(1)}" fill="#7dd3fc" font-size="10.5" font-weight="800" text-anchor="middle">${escapeHtml(noteName)}</text>
        </g>`;
      } else if (showScaleHints && inScale) {
        const bg = (pc === keyRootPc) ? 'rgba(243, 146, 55, 0.24)' : (triadPcs.has(pc) ? 'rgba(34, 197, 94, 0.16)' : 'rgba(148, 163, 184, 0.12)');
        const br = (pc === keyRootPc) ? 'rgba(243, 146, 55, 0.72)' : 'var(--border)';
        svg += `<g class="studio-fb-cell" data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" style="cursor:pointer;">
          <circle cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="9.2" fill="${bg}" stroke="${br}" stroke-width="1.2"/>
          <text x="${cx.toFixed(1)}" y="${(cy + 3.4).toFixed(1)}" fill="var(--text-muted)" font-size="9.5" font-weight="700" text-anchor="middle">${escapeHtml(noteName)}</text>
        </g>`;
      } else {
        svg += `<circle data-action="fb-click" data-fb-string="${s}" data-fb-fret="${f}" cx="${cx.toFixed(1)}" cy="${cy.toFixed(1)}" r="10.2" fill="transparent" style="cursor:pointer;"/>`;
      }
    }
  }

  svg += `</svg>`;
  return svg;
}

/**
 * Renderuje dolną matrycę aranżacji Ścieżki × Takty (Strefa D — Mini-DAW).
 */
export function renderArrangerMatrixHtml(song, opts = {}) {
  const tracks = song?.tracks || [];
  const activeTrackIdx = Math.max(0, Math.min(tracks.length - 1, Number(opts.activeTrackIdx) || 0));
  const cursorMeasureIdx = Number(opts.cursor?.measureIdx) || 0;
  const playMeasureIdx = opts.playhead?.active ? Number(opts.playhead.measureIdx) : -1;
  const loopEnabled = Boolean(opts.loopEnabled);
  const loopStart = Math.min(Number(opts.loopStart) || 0, Number(opts.loopEnd) || 0);
  const loopEnd = Math.max(Number(opts.loopStart) || 0, Number(opts.loopEnd) || 0);
  const hasSel = opts.selectionStart !== null && opts.selectionStart !== undefined;
  const selA = hasSel ? Math.min(Number(opts.selectionStart), Number(opts.selectionEnd ?? opts.selectionStart)) : -1;
  const selB = hasSel ? Math.max(Number(opts.selectionStart), Number(opts.selectionEnd ?? opts.selectionStart)) : -1;
  const numMeasures = Math.max(1, getMasterMeasureCount(song));

  let headerCols = '';
  for (let m = 0; m < numMeasures; m++) {
    const sec = tracks[0]?.measures?.[m]?.sectionMarker || '';
    const isPlayingCol = playMeasureIdx === m;
    const inLoop = loopEnabled && m >= loopStart && m <= loopEnd;
    const inSel = hasSel && m >= selA && m <= selB;
    headerCols += `<th class="studio-bar-col-head ${isPlayingCol ? 'playing' : ''} ${inLoop ? 'in-loop' : ''} ${inSel ? 'selected-range' : ''}" data-action="select-bar" data-measure="${m}" title="Kliknij = skocz · Shift+Klik = zaznacz zakres taktów">
      <div>${m + 1}</div>
      ${sec ? `<div style="font-size:9px;color:var(--accent);">${escapeHtml(sec)}</div>` : ''}
    </th>`;
  }

  let rowsHtml = '';
  tracks.forEach((tr, tIdx) => {
    const isActive = tIdx === activeTrackIdx;
    const isDrum = tr.instrumentType === 'drums' || Boolean(tr.isPercussion);
    const isKeys = tr.instrumentType === 'keys';
    const metaBadge = isDrum
      ? 'Perkusja · GM Ch.10'
      : (isKeys ? `Klawisze · ${tr.tonePreset || 'rhodes'}` : `${tr.openStringMidis?.length || tr.stringCount || 6} str. · ${tr.tonePreset || 'clean'}`);

    let cellsHtml = '';
    for (let m = 0; m < numMeasures; m++) {
      const meas = tr.measures?.[m];
      const isCursorCell = isActive && cursorMeasureIdx === m;
      const isPlayCell = playMeasureIdx === m;
      if (isDrum) {
        const blockId = meas?.drumBlockId || 'rock_8th';
        const autoFillN = Number(tr.drumAutoFill) || 0;
        const isFill = autoFillN > 0 && ((m + 1) % autoFillN === 0);
        const options = DRUM_BLOCKS.map(b =>
          `<option value="${escapeHtml(b.id)}" ${b.id === blockId ? 'selected' : ''}>${escapeHtml(b.name)}</option>`
        ).join('');
        cellsHtml += `<td class="studio-matrix-cell">
          <select class="studio-drum-select" data-action="drum-block" data-track="${tIdx}" data-measure="${m}" title="Klocek perkusyjny dla taktu ${m + 1}">
            ${options}
          </select>
          ${isFill ? `<div style="font-size:9px;color:var(--accent);font-weight:700;margin-top:2px;">✨ Auto-Fill</div>` : ''}
        </td>`;
      } else {
        const beats = meas?.beats || [];
        const noteCount = beats.reduce((acc, b) => acc + (!b.isRest ? (b.notes?.length || 0) : 0), 0);
        const firstSym = beats.find(b => b.chordSymbol)?.chordSymbol || '';
        const text = noteCount > 0 ? (firstSym || `${noteCount} nut`) : '—';
        const inSel = hasSel && m >= selA && m <= selB;
        cellsHtml += `<td class="studio-matrix-cell" data-action="matrix-cell" data-track="${tIdx}" data-measure="${m}">
          <div class="studio-cell-block ${noteCount > 0 ? 'filled' : ''} ${isCursorCell ? 'cursor-cell' : ''} ${isPlayCell ? 'playing-cell' : ''} ${inSel ? 'selected-range' : ''}">
            ${escapeHtml(text)}
          </div>
        </td>`;
      }
    }

    rowsHtml += `<tr class="studio-matrix-track-row ${isActive ? 'active-track' : ''}">
      <td class="studio-matrix-sticky">
        <div class="studio-track-ctrl">
          <button type="button" class="studio-track-name-btn" data-action="select-track" data-track="${tIdx}">
            <span class="studio-track-dot" style="background:${escapeHtml(tr.color || '#f39237')};"></span>
            <span>${escapeHtml(tr.name)}</span>
          </button>
          <div style="display:flex;align-items:center;gap:4px;">
            <button type="button" class="studio-sm-btn solo ${tr.solo ? 'on' : ''}" data-action="toggle-solo" data-track="${tIdx}" title="Solo (S)">S</button>
            <button type="button" class="studio-sm-btn mute ${tr.muted ? 'on' : ''}" data-action="toggle-mute" data-track="${tIdx}" title="Wycisz (M)">M</button>
            ${tracks.length > 1 ? `<button type="button" class="studio-sm-btn" data-action="remove-track" data-track="${tIdx}" title="Usuń ścieżkę">✕</button>` : ''}
          </div>
        </div>
        <div style="display:flex;align-items:center;justify-content:space-between;gap:8px;margin-top:4px;">
          <span style="font-size:10.5px;color:var(--text-muted);">${escapeHtml(metaBadge)}</span>
          <input type="range" min="0" max="100" step="5" value="${Math.round((tr.volume ?? 0.85) * 100)}"
                 data-action="track-volume" data-track="${tIdx}" style="width:72px;" title="Głośność ścieżki">
        </div>
      </td>
      ${cellsHtml}
    </tr>`;
  });

  // Przyciski dokładania brakujących ról zespołu (max 5 ścieżek)
  const existingRoles = new Set(tracks.map(t => t.role));
  const addButtons = Object.values(TRACK_ROLE_PRESETS)
    .filter(p => !existingRoles.has(p.role))
    .map(p => `<button type="button" class="studio-btn sm" data-action="add-track" data-role="${escapeHtml(p.role)}" ${tracks.length >= MAX_STUDIO_TRACKS ? 'disabled' : ''}>+ ${escapeHtml(p.name.split(' (')[0])}</button>`)
    .join('');

  return `<div class="studio-arranger-head">
    <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
      <span class="section-label" style="margin:0;">MATRYCA ARANŻACJI (ŚCIEŻKI × TAKTY)</span>
      <span style="font-size:11px;color:var(--text-muted);">${tracks.length}/${MAX_STUDIO_TRACKS} ścieżek · ${numMeasures} taktów</span>
    </div>
    <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
      ${addButtons}
      ${tracks.length < MAX_STUDIO_TRACKS ? `<button type="button" class="studio-btn sm" data-action="duplicate-track">📋 Powiel ścieżkę</button>` : ''}
      <button type="button" class="studio-btn sm" data-action="add-measure">+ Takt</button>
      ${numMeasures > 1 ? `<button type="button" class="studio-btn sm danger" data-action="remove-measure">− Usuń bieżący takt</button>` : ''}
    </div>
  </div>
  <div class="studio-matrix-scroll">
    <table class="studio-matrix-table">
      <thead>
        <tr>
          <th class="studio-matrix-sticky" style="text-align:left;padding:6px 8px;color:var(--text-muted);font-size:11px;">ŚCIEŻKI ZESPOŁU</th>
          ${headerCols}
        </tr>
      </thead>
      <tbody>${rowsHtml}</tbody>
    </table>
  </div>`;
}

/**
 * Renderuje 3-poziomowy klockowy metronom akcentów (A / B / C × uderzenia taktu + podpodziały i presety).
 */
export function renderMetronomeBlocksHtml(pattern, numerator = 4, opts = {}) {
  const num = Math.max(1, Math.min(16, Math.round(Number(numerator) || 4)));
  const pat = normalizeMetronomePattern(pattern, num);
  const playingCol = Number.isInteger(opts.playingBeatCol) ? opts.playingBeatCol : -1;

  const colsHtml = pat.levels.map((lvl, colIdx) => {
    const isPlaying = colIdx === playingCol;
    const mkBlock = (rowChar, rowLvl, cls, desc) => {
      const active = lvl === rowLvl;
      return `<button type="button" class="studio-metro-block ${cls} ${active ? 'on' : ''}" data-action="metro-cell" data-beat-col="${colIdx}" data-level="${rowLvl}" data-tip-name="Uderzenie ${colIdx + 1} — poziom ${rowChar}" data-tip-desc="${desc} Kliknij aktywny klocek ponownie, aby wyciszyć uderzenie (Gap)."></button>`;
    };
    return `<div class="studio-metro-col ${isPlaying ? 'playing-col' : ''}">
      <div class="studio-metro-colnum">${colIdx + 1}</div>
      ${mkBlock('A', 3, 'lvl-a', 'Mocny akcent (najwyższy ton, 1320 Hz).')}
      ${mkBlock('B', 2, 'lvl-b', 'Średni akcent (ton pośredni, 980 Hz).')}
      ${mkBlock('C', 1, 'lvl-c', 'Słabszy klik bazowy (720 Hz).')}
    </div>`;
  }).join('');

  return `<div class="studio-metro-panel">
    <div class="studio-metro-head">
      <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
        <span class="section-label" style="margin:0;">KLOCKOWY METRONOM AKCENTÓW (${num} UDERZEŃ)</span>
        <div class="studio-engine-toggle" data-tip-name="Podpodział rytmiczny metronomu" data-tip-desc="Dodaje cichsze kliknięcia pomocnicze pomiędzy głównymi miarami taktu.">
          ${[1, 2, 3, 4].map(sub => {
            const lbl = sub === 1 ? '×1' : (sub === 2 ? '×2 (⅛)' : (sub === 3 ? '×3 (triola)' : '×4 (1/16)'));
            return `<button type="button" class="${pat.subdivisions === sub ? 'active' : ''}" data-action="metro-subdiv" data-sub="${sub}">${lbl}</button>`;
          }).join('')}
        </div>
      </div>
      <div style="display:flex;align-items:center;gap:5px;flex-wrap:wrap;">
        ${METRONOME_PRESETS.map(pr =>
          `<button type="button" class="studio-btn sm" data-action="metro-preset" data-preset="${escapeHtml(pr.id)}" data-tip-name="${escapeHtml(pr.name)}" data-tip-desc="Ustawia gotowy wzorzec akcentów A/B/C.">${escapeHtml(pr.name)}</button>`
        ).join('')}
      </div>
    </div>
    <div class="studio-metro-body">
      <div class="studio-metro-labels">
        <div class="studio-metro-colnum" style="visibility:hidden;">#</div>
        <span data-tip-name="Poziom A (Akcent)" data-tip-desc="Najgłośniejszy, wysoki ton (np. na raz w takcie).">A</span>
        <span data-tip-name="Poziom B (Średni)" data-tip-desc="Umiarkowany akcent np. na 3. miarę w 4/4.">B</span>
        <span data-tip-name="Poziom C (Słaby)" data-tip-desc="Delikatny klik pulsu bazowego.">C</span>
      </div>
      <div class="studio-metro-grid">${colsHtml}</div>
    </div>
  </div>`;
}
