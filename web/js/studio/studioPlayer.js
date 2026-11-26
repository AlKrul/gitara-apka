/**
 * Silnik odtwarzania wielościeżkowego WebAudio dla 🎼 Studio TAB:
 *  - Lookahead scheduler (120 ms naprzód) na zegarze AudioContext (lub clockFn w testach)
 *  - Count-In (1-2-3-4), Metronom, Pętla A-B, Prędkość 25%-150% + Auto-Tempo
 *  - Obsługa Solo / Mute oraz obliczanie nut aktywnych i następnych (gryf Strefy E)
 */
import {
  BEND_PRESETS,
  WHAMMY_PRESETS,
  buildBeatEventsForTrack,
  durationToQuarterNotes,
  expandPlaybackTimeline,
  getMasterMeasureCount,
  isTrackAudible,
  midiToHz,
  midiToNoteName,
  normalizeMetronomePattern,
  noteToMidi
} from './studioModel.js';
import { GM_DRUM_MIDI, playStudioDrumHit, resolveDrumMeasureSteps } from './studioDrumBlocks.js';

const LOOKAHEAD_SEC = 0.12;
const TIMER_INTERVAL_MS = 25;

function findFirstAudibleMelodicTarget(song, activeTrackIdx) {
  const tr = song?.tracks?.[activeTrackIdx];
  if (!tr) return null;
  return tr;
}

/**
 * Zwraca nuty bieżącego uderzenia oraz następnego niepauzowego uderzenia (do animacji gryfu w Strefie E).
 */
export function computeActiveAndNextNotes(song, activeTrackIdx, measureIdx, beatIdx) {
  const track = findFirstAudibleMelodicTarget(song, activeTrackIdx);
  if (!track) return { activeNotes: [], nextNotes: [], activeHits: [], nextHits: [] };

  const accMode = 2;
  if (track.instrumentType === 'drums' || track.isPercussion) {
    const m = track.measures?.[measureIdx];
    const steps = resolveDrumMeasureSteps(m, track, measureIdx);
    const curStep = steps[beatIdx] || null;
    const nextStep = steps[beatIdx + 1] || null;
    return {
      activeNotes: [],
      nextNotes: [],
      activeHits: curStep ? curStep.hits.map(h => h.kind) : [],
      nextHits: nextStep ? nextStep.hits.map(h => h.kind) : []
    };
  }

  const mapBeatNotes = (beat) => {
    if (!beat || beat.isRest || !Array.isArray(beat.notes)) return [];
    const out = [];
    for (const n of beat.notes) {
      const midi = noteToMidi(track, n);
      if (midi === null) continue;
      out.push({
        string: n.string,
        fret: n.fret,
        midi,
        note: midiToNoteName(midi, accMode),
        deadNote: Boolean(n.deadNote),
        harmonicType: Number(n.harmonicType) || 0
      });
    }
    return out;
  };

  const measures = track.measures || [];
  const curMeasure = measures[measureIdx];
  const curBeat = curMeasure?.beats?.[beatIdx];
  const activeNotes = mapBeatNotes(curBeat);

  let nextNotes = [];
  if (curMeasure && Array.isArray(curMeasure.beats)) {
    for (let b = beatIdx + 1; b < curMeasure.beats.length; b++) {
      if (!curMeasure.beats[b].isRest && curMeasure.beats[b].notes.length > 0) {
        nextNotes = mapBeatNotes(curMeasure.beats[b]);
        break;
      }
    }
  }
  if (nextNotes.length === 0) {
    for (let m = measureIdx + 1; m < measures.length; m++) {
      const beats = measures[m]?.beats || [];
      for (let b = 0; b < beats.length; b++) {
        if (!beats[b].isRest && beats[b].notes.length > 0) {
          nextNotes = mapBeatNotes(beats[b]);
          break;
        }
      }
      if (nextNotes.length > 0) break;
    }
  }

  return { activeNotes, nextNotes, activeHits: [], nextHits: [] };
}

function buildBeatPositionsForMeasure(measure) {
  if (!measure || !Array.isArray(measure.beats) || measure.beats.length === 0) {
    return [{ beatIdx: 0, startOffsetQ: 0, durQ: 4 }];
  }
  const out = [];
  let acc = 0;
  for (let i = 0; i < measure.beats.length; i++) {
    const durQ = durationToQuarterNotes(measure.beats[i]);
    out.push({ beatIdx: i, startOffsetQ: acc, durQ });
    acc += durQ;
  }
  return out;
}

/**
 * Tworzy odtwarzacz Studia TAB z precyzyjnym harmonogramem WebAudio i obsługą pętli/Auto-Tempo.
 */
export function createStudioPlayer({
  host = null,
  soundFontSynth = null,
  soundFontEnabled = false,
  clockFn = null,
  onPositionChange = null,
  onStateChange = null,
  onAutoTempoBump = null
} = {}) {
  let song = null;
  let activeTrackIdx = 0;
  let useSoundFont = Boolean(soundFontEnabled);

  let isPlaying = false;
  let isPaused = false;
  let speedPercent = 100;
  let countInEnabled = false;
  let metronomeEnabled = true;
  let loopEnabled = false;
  let loopStart = 0;
  let loopEnd = 3;
  let autoTempoEnabled = false;
  let autoTempoStep = 4;

  let cursorMeasureIdx = 0;
  let cursorBeatIdx = 0;

  // Harmonogram wewnętrzny
  let timerId = null;
  let rafId = null;
  let activeSlice = [];
  let sliceTotalQ = 4;
  let scheduledEvents = [];
  let metronomeEvents = [];
  let countInTicks = [];
  let nextEventIdx = 0;
  let nextMetroIdx = 0;
  let nextCountInIdx = 0;
  let epochSec = 0;
  let pausedAtQ = 0;
  let loopPassCount = 0;
  let lastReportedKey = '';

  function getNowSec() {
    if (typeof clockFn === 'function') return Number(clockFn()) || 0;
    const ctx = host && typeof host.ensureAudioCtx === 'function' ? host.ensureAudioCtx() : null;
    if (ctx && Number.isFinite(ctx.currentTime)) return ctx.currentTime;
    return (typeof performance !== 'undefined' ? performance.now() : Date.now()) / 1000;
  }

  function getVisualOutputLatencySec() {
    if (typeof clockFn === 'function') return 0;
    const ctx = host && typeof host.ensureAudioCtx === 'function' ? host.ensureAudioCtx() : null;
    const raw = Number(ctx?.outputLatency ?? 0);
    return Number.isFinite(raw) && raw > 0 ? Math.min(0.15, raw) : 0;
  }

  function getA4Hz() {
    const st = host && typeof host.getState === 'function' ? host.getState() : null;
    return (st && Number(st.a4ReferenceHz) > 300) ? Number(st.a4ReferenceHz) : 440.0;
  }

  function getEffectiveBpm() {
    const base = song ? (Number(song.tempoBpm) || 120) : 120;
    return Math.max(25, Math.min(280, Math.round(base * (speedPercent / 100))));
  }

  function getSecPerQuarter() {
    return 60.0 / getEffectiveBpm();
  }

  function emitState() {
    if (typeof onStateChange === 'function') {
      onStateChange(getSnapshot());
    }
  }

  function playMetronomeLevel(level = 1, whenSec = undefined) {
    if (level <= 0 || !host) return;
    const hasWhen = Number.isFinite(whenSec);
    const ctx = typeof host.ensureAudioCtx === 'function' ? host.ensureAudioCtx() : null;
    if (ctx && typeof ctx.createOscillator === 'function' && typeof ctx.createGain === 'function') {
      const out = (typeof host.getMasterAudioOut === 'function' ? host.getMasterAudioOut() : null) || ctx.destination;
      const st = typeof host.getState === 'function' ? host.getState() : null;
      const mVol = (st && st.metronomeVolume !== undefined) ? Number(st.metronomeVolume) : 0.75;
      const now = hasWhen ? Math.max(ctx.currentTime, whenSec) : ctx.currentTime;
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      let freq = 720;
      let peak = 0.14;
      let dur = 0.036;
      osc.type = 'triangle';
      if (level >= 3) {
        freq = 1320; peak = 0.26; dur = 0.048;
      } else if (level >= 2) {
        freq = 980; peak = 0.19; dur = 0.042;
      } else if (level < 1) {
        osc.type = 'sine';
        freq = 560; peak = 0.08; dur = 0.024;
      }
      osc.frequency.setValueAtTime(freq, now);
      g.gain.setValueAtTime(Math.max(0.01, peak * mVol), now);
      g.gain.exponentialRampToValueAtTime(0.001, now + dur);
      osc.connect(g);
      g.connect(out);
      osc.start(now);
      osc.stop(now + dur + 0.008);
      return;
    }
    if (typeof host.playMetronomeTickAudio === 'function') {
      if (hasWhen) host.playMetronomeTickAudio(level >= 3, whenSec);
      else host.playMetronomeTickAudio(level >= 3);
    }
  }

  function rebuildScheduleData(startFromMeasureIdx = null) {
    if (!song) return { startQOffset: 0 };
    const fullTimeline = expandPlaybackTimeline(song);
    if (fullTimeline.length === 0) return { startQOffset: 0 };

    const maxM = Math.max(0, getMasterMeasureCount(song) - 1);
    const lA = Math.max(0, Math.min(maxM, Math.min(loopStart, loopEnd)));
    const lB = Math.max(0, Math.min(maxM, Math.max(loopStart, loopEnd)));

    let slice = loopEnabled
      ? fullTimeline.filter(e => e.measureIdx >= lA && e.measureIdx <= lB)
      : fullTimeline;
    let usedFallbackSlice = false;

    if (slice.length === 0 && loopEnabled) {
      // Fallback gdy zakres pętli dotyczy wyłączonej volty
      usedFallbackSlice = true;
      const master = song.tracks[0]?.measures || [];
      let q = 0;
      slice = [];
      for (let m = lA; m <= lB; m++) {
        const ref = master[m] || { numerator: 4, denominator: 4 };
        const lenQ = (ref.numerator || 4) * (4 / (ref.denominator || 4));
        slice.push({
          seqIdx: slice.length,
          measureIdx: m,
          startQ: q,
          lengthQ: lenQ,
          numerator: ref.numerator || 4,
          denominator: ref.denominator || 4,
          pass: 1
        });
        q += lenQ;
      }
    }

    // Przelicz spójną oś startQ dla wycinka (od 0)
    let runningQ = 0;
    const seqMap = new Map();
    activeSlice = slice.map((entry, idx) => {
      const refM = song.tracks[activeTrackIdx]?.measures?.[entry.measureIdx] || song.tracks[0]?.measures?.[entry.measureIdx];
      const remapped = {
        ...entry,
        sliceIdx: idx,
        startQ: runningQ,
        beatPositions: buildBeatPositionsForMeasure(refM)
      };
      seqMap.set(entry.seqIdx, remapped);
      runningQ += entry.lengthQ;
      return remapped;
    });
    sliceTotalQ = Math.max(0.25, runningQ);

    // Zdarzenia nutowe i perkusyjne dla wszystkich słyszalnych ścieżek
    const sourceTimeline = usedFallbackSlice ? slice : fullTimeline;
    const allEvs = [];
    for (let tIdx = 0; tIdx < song.tracks.length; tIdx++) {
      if (!isTrackAudible(song.tracks, tIdx)) continue;
      const trackEvs = buildBeatEventsForTrack(song, tIdx, sourceTimeline);
      for (const ev of trackEvs) {
        const mappedEntry = seqMap.get(ev.seqIdx);
        if (!mappedEntry) continue;
        const origEntry = sourceTimeline[ev.seqIdx];
        const localQ = ev.startQ - (origEntry ? origEntry.startQ : 0);
        allEvs.push({
          ...ev,
          startQ: mappedEntry.startQ + localQ
        });
      }
    }
    allEvs.sort((a, b) => a.startQ - b.startQ);
    scheduledEvents = allEvs;

    // Zdarzenia metronomu na siatce taktowej wycinka (poziomy A=3, B=2, C=1, brak=0 + podpodziały)
    const metro = [];
    for (const entry of activeSlice) {
      const stepQ = 4 / (entry.denominator || 4);
      const count = Math.max(1, entry.numerator || 4);
      const pat = normalizeMetronomePattern(song.metronomePattern, count);
      for (let b = 0; b < count; b++) {
        const lvl = pat.levels[b] ?? (b === 0 ? 3 : 1);
        if (lvl > 0) {
          metro.push({
            startQ: entry.startQ + b * stepQ,
            beatCol: b,
            level: lvl,
            isDownbeat: lvl >= 3,
            measureIdx: entry.measureIdx
          });
        }
        if (pat.subdivisions > 1 && pat.subAccent > 0) {
          const subQ = stepQ / pat.subdivisions;
          for (let sub = 1; sub < pat.subdivisions; sub++) {
            metro.push({
              startQ: entry.startQ + b * stepQ + sub * subQ,
              beatCol: b,
              level: 0.5,
              isDownbeat: false,
              measureIdx: entry.measureIdx
            });
          }
        }
      }
    }
    metro.sort((a, b) => a.startQ - b.startQ);
    metronomeEvents = metro;

    let startQOffset = 0;
    if (startFromMeasureIdx !== null) {
      const target = activeSlice.find(e => e.measureIdx === startFromMeasureIdx);
      if (target) {
        const bp = target.beatPositions[cursorBeatIdx];
        startQOffset = target.startQ + (bp ? bp.startOffsetQ : 0);
      }
    }
    return { startQOffset };
  }

  function computeScheduledNoteDurationSec(evDurationQ, secPerQ, note, curSpeedPercent) {
    const tieExtraQ = Math.max(0, Number(note?.tieExtraQ) || 0);
    const totalDurQ = evDurationQ + tieExtraQ;
    const baseDurSec = Math.max(0.045, totalDurQ * secPerQ * 0.96);
    const speedScale = 100 / Math.max(25, curSpeedPercent || 100);
    if (note?.deadNote) {
      return Math.max(0.035, Math.min(0.08 * speedScale, baseDurSec * 0.35));
    }
    if (note?.palmMute) {
      return Math.max(0.05, Math.min(0.24 * speedScale, baseDurSec * 0.62));
    }
    if (note?.staccato) {
      return Math.max(0.045, baseDurSec * 0.45);
    }
    if (note?.letRing) {
      return Math.max(baseDurSec, 1.45 * speedScale);
    }
    return Math.max(0.05, baseDurSec);
  }

  function triggerAudioEvent(ev, secPerQ, whenSec = undefined) {
    if (!host || !song) return;
    const track = song.tracks[ev.trackIdx];
    if (!track || !isTrackAudible(song.tracks, ev.trackIdx)) return;

    const scheduledWhen = Number.isFinite(whenSec) ? whenSec : undefined;
    const sfReady = useSoundFont && soundFontSynth && typeof soundFontSynth.playNote === 'function' && soundFontSynth.isLoaded?.();

    if (ev.kind === 'drum') {
      const gain = Math.max(0.1, Math.min(1.2, track.volume ?? 0.85));
      for (const hit of ev.hits) {
        if (sfReady) {
          const midi = GM_DRUM_MIDI[hit.kind] ?? 36;
          const rawVel = Number(hit.velocity);
          const sfVel = Number.isFinite(rawVel)
            ? (rawVel <= 1.0 ? Math.max(1, Math.min(127, Math.round(rawVel * 127))) : Math.max(1, Math.min(127, Math.round(rawVel))))
            : 96;
          if (soundFontSynth.playNote({
            bank: 128,
            program: 0,
            midi,
            velocity: sfVel,
            durationSec: 0.35,
            gain,
            whenSec: scheduledWhen ?? null
          })) {
            continue;
          }
        }
        playStudioDrumHit(hit.kind, hit.velocity * gain, host, scheduledWhen);
      }
      return;
    }

    const notes = ev.notes || [];
    const dynMulMap = { 1: 0.42, 2: 0.54, 3: 0.68, 4: 0.80, 5: 0.92, 6: 1.0, 7: 1.15 };
    const dynMul = dynMulMap[ev.beatDynamic ?? 6] ?? 1.0;
    const polyScale = ((track.volume ?? 0.85) * dynMul) / Math.sqrt(Math.max(1, notes.length));
    const a4 = getA4Hz();

    for (const n of notes) {
      const noteDur = computeScheduledNoteDurationSec(ev.durationQ, secPerQ, n, speedPercent);
      const noteGain = n.deadNote ? polyScale * 0.35 : (n.palmMute ? polyScale * 0.72 : (n.ghostNote ? polyScale * 0.45 : polyScale));

      if (sfReady) {
        if (soundFontSynth.playNote({
          bank: 0,
          program: track.midiProgram ?? 25,
          midi: n.midi,
          velocity: n.velocity ?? 95,
          durationSec: noteDur,
          gain: noteGain,
          whenSec: scheduledWhen ?? null
        })) {
          continue;
        }
      }

      const freq = midiToHz(n.midi, a4);
      if (track.instrumentType === 'keys') {
        if (typeof host.playKeyboardPresetNote === 'function') {
          const st = host.getState?.();
          const prevKbd = st ? st.keyboardPreset : null;
          if (st && track.tonePreset) st.keyboardPreset = track.tonePreset;
          if (scheduledWhen !== undefined) {
            host.playKeyboardPresetNote(freq, noteDur, polyScale, scheduledWhen);
          } else {
            host.playKeyboardPresetNote(freq, noteDur, polyScale);
          }
          if (st && prevKbd !== null) st.keyboardPreset = prevKbd;
        }
        continue;
      }

      if (n.harmonicType > 0 && typeof host.playHarmonicBellFrequency === 'function') {
        const harmDur = Math.max(0.09, noteDur * 1.15);
        if (scheduledWhen !== undefined) {
          host.playHarmonicBellFrequency(freq, harmDur, scheduledWhen);
        } else {
          host.playHarmonicBellFrequency(freq, harmDur);
        }
        continue;
      }

      if (typeof host.pluckGuitarFrequency === 'function') {
        const st = host.getState?.();
        const prevTone = st ? st.guitarTonePreset : null;
        if (st && track.tonePreset) st.guitarTonePreset = track.tonePreset;
        const whammyPts = (Array.isArray(n.whammyPoints) && n.whammyPoints.length >= 2)
          ? n.whammyPoints
          : (n.whammyPreset && WHAMMY_PRESETS[n.whammyPreset] ? WHAMMY_PRESETS[n.whammyPreset].points : null);
        const bendPts = (Array.isArray(n.bendPoints) && n.bendPoints.length >= 2)
          ? n.bendPoints
          : (n.bendPreset && BEND_PRESETS[n.bendPreset] ? BEND_PRESETS[n.bendPreset].points : null);
        const pitchCurve = whammyPts || bendPts || null;
        host.pluckGuitarFrequency(freq, noteDur, noteGain, true, {
          whenSec: scheduledWhen,
          strictDuration: !n.letRing,
          palmMute: Boolean(n.palmMute),
          hammerOn: Boolean(n.hammerOn),
          vibrato: Boolean(n.vibrato),
          wideVibrato: Boolean(n.wideVibrato),
          pitchCurve
        });
        if (st && prevTone !== null) st.guitarTonePreset = prevTone;
      }
    }
  }

  function locatePositionAtQ(nowQ) {
    if (activeSlice.length === 0) {
      return { measureIdx: 0, beatIdx: 0, progress: 0, beatProgress: 0 };
    }
    const clampedQ = Math.max(0, Math.min(sliceTotalQ - 1e-6, nowQ));
    let entry = activeSlice[0];
    for (let i = 0; i < activeSlice.length; i++) {
      const e = activeSlice[i];
      if (clampedQ >= e.startQ - 1e-6) entry = e;
      else break;
    }
    const localQ = Math.max(0, clampedQ - entry.startQ);
    let beatIdx = 0;
    const bps = entry.beatPositions || [];
    for (let b = 0; b < bps.length; b++) {
      if (localQ >= bps[b].startOffsetQ - 1e-5) beatIdx = bps[b].beatIdx;
      else break;
    }
    const curBp = bps[beatIdx] || { startOffsetQ: 0, durQ: entry.lengthQ || 1 };
    const nextBpStartQ = (beatIdx + 1 < bps.length)
      ? bps[beatIdx + 1].startOffsetQ
      : Math.max(curBp.startOffsetQ + curBp.durQ, entry.lengthQ || curBp.durQ);
    const beatSpanQ = Math.max(1e-4, nextBpStartQ - curBp.startOffsetQ);
    const beatProgress = Math.max(0, Math.min(1, (localQ - curBp.startOffsetQ) / beatSpanQ));
    const progress = entry.lengthQ > 0 ? Math.max(0, Math.min(1, localQ / entry.lengthQ)) : 0;
    const stepQ = 4 / (entry.denominator || 4);
    const metroBeatCol = Math.max(0, Math.min((entry.numerator || 4) - 1, Math.floor((localQ + 1e-5) / stepQ)));
    return { measureIdx: entry.measureIdx, beatIdx, progress, beatProgress, sliceIdx: entry.sliceIdx, metroBeatCol };
  }

  function advanceTimers(nowSec) {
    if (!isPlaying || isPaused || !song) return;

    // 1. Obsługa odliczania (Count-In)
    while (nextCountInIdx < countInTicks.length && countInTicks[nextCountInIdx].timeSec <= nowSec + LOOKAHEAD_SEC) {
      const tick = countInTicks[nextCountInIdx];
      const tickWhen = Math.max(nowSec, tick.timeSec);
      if (host && typeof host.playMetronomeTickAudio === 'function') {
        host.playMetronomeTickAudio(tick.isDownbeat, tickWhen);
      }
      if (typeof onPositionChange === 'function') {
        onPositionChange({
          phase: 'countin',
          countInBeat: tick.beatNum,
          countInTotal: tick.total,
          measureIdx: cursorMeasureIdx,
          beatIdx: cursorBeatIdx,
          progress: 0
        });
      }
      nextCountInIdx++;
    }

    if (nowSec < epochSec) {
      return;
    }

    const secPerQ = getSecPerQuarter();
    let nowQ = (nowSec - epochSec) / secPerQ;
    const horizonQ = nowQ + LOOKAHEAD_SEC / secPerQ;

    // 2. Planowanie zdarzeń metronomu i ścieżek w oknie LOOKAHEAD
    while (nextMetroIdx < metronomeEvents.length && metronomeEvents[nextMetroIdx].startQ <= horizonQ) {
      const mEv = metronomeEvents[nextMetroIdx];
      if (metronomeEnabled) {
        const mWhen = Math.max(nowSec, epochSec + mEv.startQ * secPerQ);
        playMetronomeLevel(mEv.level ?? (mEv.isDownbeat ? 3 : 1), mWhen);
      }
      nextMetroIdx++;
    }

    while (nextEventIdx < scheduledEvents.length && scheduledEvents[nextEventIdx].startQ <= horizonQ) {
      const ev = scheduledEvents[nextEventIdx];
      const evWhen = Math.max(nowSec, epochSec + ev.startQ * secPerQ);
      triggerAudioEvent(ev, secPerQ, evWhen);
      nextEventIdx++;
    }

    // 3. Obsługa końca sekwencji lub zawinięcia pętli A-B (+ Auto-Tempo)
    if (nowQ >= sliceTotalQ) {
      if (loopEnabled) {
        loopPassCount++;
        const oldSecPerQ = secPerQ;
        epochSec += sliceTotalQ * oldSecPerQ;

        if (autoTempoEnabled && speedPercent < 150) {
          const oldPct = speedPercent;
          const oldBpm = getEffectiveBpm();
          const targetBpm = Math.min(280, oldBpm + autoTempoStep);
          const baseBpm = Number(song.tempoBpm) || 120;
          let nextPct = Math.min(150, Math.round((targetBpm / baseBpm) * 100));
          if (nextPct <= oldPct && oldPct < 150) nextPct = Math.min(150, oldPct + 1);
          if (nextPct > oldPct) {
            speedPercent = nextPct;
            if (typeof onAutoTempoBump === 'function') {
              onAutoTempoBump({
                oldSpeedPercent: oldPct,
                newSpeedPercent: speedPercent,
                oldBpm,
                newBpm: getEffectiveBpm(),
                pass: loopPassCount
              });
            }
            emitState();
          }
        }

        nextEventIdx = 0;
        nextMetroIdx = 0;
        // Ponownie przeleć od początku pętli w bieżącym kroku
        const newSecPerQ = getSecPerQuarter();
        nowQ = Math.max(0, (nowSec - epochSec) / newSecPerQ);
        const newHorizonQ = nowQ + LOOKAHEAD_SEC / newSecPerQ;
        while (nextMetroIdx < metronomeEvents.length && metronomeEvents[nextMetroIdx].startQ <= newHorizonQ) {
          const mEv = metronomeEvents[nextMetroIdx];
          if (metronomeEnabled) {
            const mWhen = Math.max(nowSec, epochSec + mEv.startQ * newSecPerQ);
            playMetronomeLevel(mEv.level ?? (mEv.isDownbeat ? 3 : 1), mWhen);
          }
          nextMetroIdx++;
        }
        while (nextEventIdx < scheduledEvents.length && scheduledEvents[nextEventIdx].startQ <= newHorizonQ) {
          const ev = scheduledEvents[nextEventIdx];
          const evWhen = Math.max(nowSec, epochSec + ev.startQ * newSecPerQ);
          triggerAudioEvent(ev, newSecPerQ, evWhen);
          nextEventIdx++;
        }
      } else {
        stop();
        return;
      }
    }

    const visualNowSec = Math.max(epochSec, nowSec - getVisualOutputLatencySec());
    const visualQ = Math.max(0, (visualNowSec - epochSec) / getSecPerQuarter());
    const pos = locatePositionAtQ(visualQ);
    cursorMeasureIdx = pos.measureIdx;
    cursorBeatIdx = pos.beatIdx;
    const key = `${pos.measureIdx}:${pos.beatIdx}`;
    const noteChanged = key !== lastReportedKey;
    lastReportedKey = key;
    if (typeof onPositionChange === 'function') {
      onPositionChange({
        phase: 'playing',
        measureIdx: pos.measureIdx,
        beatIdx: pos.beatIdx,
        progress: pos.progress,
        beatProgress: pos.beatProgress,
        metroBeatCol: pos.metroBeatCol,
        noteChanged
      });
    }
  }

  function startLoops() {
    stopLoops();
    if (typeof setInterval === 'function' && typeof clockFn !== 'function') {
      timerId = setInterval(() => advanceTimers(getNowSec()), TIMER_INTERVAL_MS);
    }
    if (typeof requestAnimationFrame === 'function' && typeof clockFn !== 'function') {
      const tickRaf = () => {
        if (!isPlaying || isPaused) return;
        const nowSec = getNowSec();
        const visualNowSec = nowSec - getVisualOutputLatencySec();
        if (visualNowSec >= epochSec) {
          const nowQ = (visualNowSec - epochSec) / getSecPerQuarter();
          const pos = locatePositionAtQ(nowQ);
          cursorMeasureIdx = pos.measureIdx;
          cursorBeatIdx = pos.beatIdx;
          const key = `${pos.measureIdx}:${pos.beatIdx}`;
          const noteChanged = key !== lastReportedKey;
          lastReportedKey = key;
          if (typeof onPositionChange === 'function') {
            onPositionChange({
              phase: 'playing',
              measureIdx: pos.measureIdx,
              beatIdx: pos.beatIdx,
              progress: pos.progress,
              beatProgress: pos.beatProgress,
              metroBeatCol: pos.metroBeatCol,
              noteChanged
            });
          }
        }
        rafId = requestAnimationFrame(tickRaf);
      };
      rafId = requestAnimationFrame(tickRaf);
    }
  }

  function stopLoops() {
    if (timerId !== null) {
      clearInterval(timerId);
      timerId = null;
    }
    if (rafId !== null && typeof cancelAnimationFrame === 'function') {
      cancelAnimationFrame(rafId);
      rafId = null;
    }
  }

  function loadSong(nextSong, trackIdx = 0, opts = {}) {
    song = nextSong;
    activeTrackIdx = Math.max(0, Number(trackIdx) || 0);
    if (opts.preservePlayback && isPlaying) {
      const nowSec = getNowSec();
      const curQ = (!isPaused && nowSec >= epochSec)
        ? Math.max(0, (nowSec - epochSec) / getSecPerQuarter())
        : pausedAtQ;
      rebuildScheduleData(cursorMeasureIdx);
      nextEventIdx = scheduledEvents.findIndex(e => e.startQ >= curQ - 1e-6);
      if (nextEventIdx < 0) nextEventIdx = scheduledEvents.length;
      nextMetroIdx = metronomeEvents.findIndex(e => e.startQ >= curQ - 1e-6);
      if (nextMetroIdx < 0) nextMetroIdx = metronomeEvents.length;
      return;
    }
    if (isPlaying) stop();
    else rebuildScheduleData(cursorMeasureIdx);
  }

  function setActiveTrack(trackIdx) {
    activeTrackIdx = Math.max(0, Number(trackIdx) || 0);
    if (song && activeSlice.length > 0) {
      activeSlice = activeSlice.map(entry => {
        const refM = song.tracks[activeTrackIdx]?.measures?.[entry.measureIdx] || song.tracks[0]?.measures?.[entry.measureIdx];
        return { ...entry, beatPositions: buildBeatPositionsForMeasure(refM) };
      });
    }
  }

  function play() {
    if (!song) return false;
    if (host && typeof host.ensureAudioCtx === 'function') host.ensureAudioCtx();

    const nowSec = getNowSec();
    if (isPlaying && isPaused) {
      // Wznowienie z pauzy dokładnie z zapamiętanej pozycji
      isPaused = false;
      epochSec = nowSec - pausedAtQ * getSecPerQuarter();
      startLoops();
      emitState();
      return true;
    }

    const startTarget = loopEnabled
      ? Math.max(0, Math.min(loopStart, loopEnd))
      : cursorMeasureIdx;
    const { startQOffset } = rebuildScheduleData(startTarget);

    countInTicks = [];
    nextCountInIdx = 0;
    let countInOffsetSec = 0;
    if (countInEnabled && activeSlice.length > 0) {
      const refEntry = activeSlice[0];
      const total = Math.max(1, refEntry.numerator || 4);
      const stepSec = (4 / (refEntry.denominator || 4)) * getSecPerQuarter();
      for (let i = 0; i < total; i++) {
        countInTicks.push({
          timeSec: nowSec + i * stepSec,
          beatNum: i + 1,
          total,
          isDownbeat: i === 0
        });
      }
      countInOffsetSec = total * stepSec;
    }

    epochSec = nowSec + countInOffsetSec - startQOffset * getSecPerQuarter();
    nextEventIdx = scheduledEvents.findIndex(e => e.startQ >= startQOffset - 1e-6);
    if (nextEventIdx < 0) nextEventIdx = scheduledEvents.length;
    nextMetroIdx = metronomeEvents.findIndex(e => e.startQ >= startQOffset - 1e-6);
    if (nextMetroIdx < 0) nextMetroIdx = metronomeEvents.length;

    loopPassCount = 0;
    lastReportedKey = '';
    isPlaying = true;
    isPaused = false;

    advanceTimers(nowSec);
    startLoops();
    emitState();
    return true;
  }

  function pause() {
    if (!isPlaying || isPaused) return;
    const nowSec = getNowSec();
    pausedAtQ = Math.max(0, (nowSec - epochSec) / getSecPerQuarter());
    countInTicks = [];
    nextCountInIdx = 0;
    isPaused = true;
    stopLoops();
    if (host && typeof host.stopAllPreviewVoices === 'function') host.stopAllPreviewVoices();
    emitState();
  }

  function togglePlay() {
    if (!isPlaying || isPaused) return play();
    pause();
    return false;
  }

  function stop() {
    stopLoops();
    if (host && typeof host.stopAllPreviewVoices === 'function') host.stopAllPreviewVoices();
    isPlaying = false;
    isPaused = false;
    pausedAtQ = 0;
    countInTicks = [];
    nextCountInIdx = 0;
    lastReportedKey = '';
    const resetMeasure = loopEnabled ? Math.max(0, Math.min(loopStart, loopEnd)) : 0;
    cursorMeasureIdx = resetMeasure;
    cursorBeatIdx = 0;
    if (typeof onPositionChange === 'function') {
      onPositionChange({
        phase: 'stopped',
        measureIdx: cursorMeasureIdx,
        beatIdx: 0,
        progress: 0,
        beatProgress: 0,
        noteChanged: true
      });
    }
    emitState();
  }

  function seekTo(measureIdx, beatIdx = 0) {
    cursorMeasureIdx = Math.max(0, Number(measureIdx) || 0);
    cursorBeatIdx = Math.max(0, Number(beatIdx) || 0);
    if (isPlaying) {
      const { startQOffset } = rebuildScheduleData(cursorMeasureIdx);
      const entry = activeSlice.find(e => e.measureIdx === cursorMeasureIdx);
      const beatStart = entry?.beatPositions?.[cursorBeatIdx]?.startOffsetQ ?? 0;
      const targetQ = startQOffset + beatStart;
      if (isPaused) {
        pausedAtQ = targetQ;
        countInTicks = [];
        nextCountInIdx = 0;
      } else {
        const nowSec = getNowSec();
        epochSec = nowSec - targetQ * getSecPerQuarter();
      }
      nextEventIdx = scheduledEvents.findIndex(e => e.startQ >= targetQ - 1e-6);
      if (nextEventIdx < 0) nextEventIdx = scheduledEvents.length;
      nextMetroIdx = metronomeEvents.findIndex(e => e.startQ >= targetQ - 1e-6);
      if (nextMetroIdx < 0) nextMetroIdx = metronomeEvents.length;
      lastReportedKey = '';
    }
    if (typeof onPositionChange === 'function') {
      onPositionChange({
        phase: isPlaying && !isPaused ? 'playing' : 'stopped',
        measureIdx: cursorMeasureIdx,
        beatIdx: cursorBeatIdx,
        progress: 0,
        beatProgress: 0,
        noteChanged: true
      });
    }
  }

  function setSpeedPercent(pct) {
    const nextPct = Math.max(25, Math.min(150, Math.round(Number(pct) || 100)));
    if (nextPct === speedPercent) return;
    const nowSec = getNowSec();
    if (isPlaying && !isPaused && nowSec >= epochSec) {
      const currentQ = (nowSec - epochSec) / getSecPerQuarter();
      speedPercent = nextPct;
      epochSec = nowSec - currentQ * getSecPerQuarter();
    } else {
      speedPercent = nextPct;
    }
    emitState();
  }

  function setLoop({ enabled, start, end } = {}) {
    if (typeof enabled === 'boolean') loopEnabled = enabled;
    if (Number.isFinite(start)) loopStart = Math.max(0, Math.round(start));
    if (Number.isFinite(end)) loopEnd = Math.max(0, Math.round(end));
    if (isPlaying) {
      const { startQOffset } = rebuildScheduleData(cursorMeasureIdx);
      const nowSec = getNowSec();
      epochSec = nowSec - startQOffset * getSecPerQuarter();
      nextEventIdx = scheduledEvents.findIndex(e => e.startQ >= startQOffset - 1e-6);
      if (nextEventIdx < 0) nextEventIdx = scheduledEvents.length;
      nextMetroIdx = metronomeEvents.findIndex(e => e.startQ >= startQOffset - 1e-6);
      if (nextMetroIdx < 0) nextMetroIdx = metronomeEvents.length;
    }
    emitState();
  }

  function setCountIn(on) {
    countInEnabled = Boolean(on);
    emitState();
  }

  function setMetronome(on) {
    metronomeEnabled = Boolean(on);
    emitState();
  }

  function setAutoTempo(on, stepBpm = autoTempoStep) {
    autoTempoEnabled = Boolean(on);
    if (Number.isFinite(stepBpm)) autoTempoStep = Math.max(1, Math.min(20, Math.round(stepBpm)));
    emitState();
  }

  function auditionFret(track, stringIdx, fret) {
    if (!host || !track || track.instrumentType === 'drums') return;
    if (host.ensureAudioCtx) host.ensureAudioCtx();
    const midi = noteToMidi(track, { string: stringIdx, fret });
    if (midi === null) return;
    if (useSoundFont && soundFontSynth && typeof soundFontSynth.playNote === 'function' && soundFontSynth.isLoaded?.()) {
      if (soundFontSynth.playNote({
        bank: 0,
        program: track.midiProgram ?? 25,
        midi,
        velocity: 100,
        durationSec: 1.0,
        gain: 0.9
      })) {
        return;
      }
    }
    const freq = midiToHz(midi, getA4Hz());
    if (track.instrumentType === 'keys' && typeof host.playKeyboardPresetNote === 'function') {
      host.playKeyboardPresetNote(freq, 0.9, 0.85);
    } else if (typeof host.pluckGuitarFrequency === 'function') {
      const st = host.getState?.();
      const prevTone = st ? st.guitarTonePreset : null;
      if (st && track.tonePreset) st.guitarTonePreset = track.tonePreset;
      host.pluckGuitarFrequency(freq, 1.15, 0.9, true);
      if (st && prevTone !== null) st.guitarTonePreset = prevTone;
    }
  }

  function auditionBeat(track, beat) {
    if (!host || !track || !beat || beat.isRest || !Array.isArray(beat.notes)) return;
    const resolved = beat.notes
      .map(n => ({ ...n, midi: noteToMidi(track, n) }))
      .filter(n => n.midi !== null);
    if (resolved.length === 0) return;
    const tIdx = song ? song.tracks.indexOf(track) : -1;
    triggerAudioEvent({
      kind: 'notes',
      trackIdx: tIdx >= 0 ? tIdx : 0,
      durationQ: durationToQuarterNotes(beat),
      notes: resolved
    }, getSecPerQuarter());
  }

  function getSnapshot() {
    return {
      isPlaying,
      isPaused,
      speedPercent,
      effectiveBpm: getEffectiveBpm(),
      countInEnabled,
      metronomeEnabled,
      loopEnabled,
      loopStart,
      loopEnd,
      autoTempoEnabled,
      autoTempoStep,
      loopPassCount,
      measureIdx: cursorMeasureIdx,
      beatIdx: cursorBeatIdx
    };
  }

  function destroy() {
    stopLoops();
    isPlaying = false;
    isPaused = false;
  }

  function setSoundFontEnabled(enabled) {
    useSoundFont = Boolean(enabled);
    emitState();
  }

  function setMetronomePattern(pattern) {
    if (!song) return;
    const maxNum = (song.tracks?.[0]?.measures || []).reduce((mx, m) => Math.max(mx, Number(m?.numerator) || 4), 4);
    song.metronomePattern = normalizeMetronomePattern(pattern, maxNum);
    loadSong(song, activeTrackIdx, { preservePlayback: isPlaying });
  }

  return {
    loadSong,
    setActiveTrack,
    play,
    pause,
    togglePlay,
    stop,
    seekTo,
    setSpeedPercent,
    setLoop,
    setCountIn,
    setMetronome,
    setMetronomePattern,
    auditionMetronomeLevel: (lvl) => playMetronomeLevel(lvl),
    setAutoTempo,
    setSoundFontEnabled,
    getSoundFontEnabled: () => useSoundFont,
    auditionFret,
    auditionBeat,
    getEffectiveBpm,
    getSnapshot,
    destroy,
    stepSchedulerForTest: advanceTimers
  };
}
