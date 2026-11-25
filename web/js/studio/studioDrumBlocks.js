/**
 * Katalog gotowych klocków perkusyjnych oraz ekspander rytmiczny i lekka synteza WebAudio
 * dla ścieżki perkusyjnej w 🎼 Studio TAB.
 *
 * Nazewnictwo instrumentów zgodne z General MIDI Percussion (kanał 10, nuty MIDI 35..81).
 */

export const GM_DRUM_MIDI = Object.freeze({
  kick: 36,         // Bass Drum 1 (C2)
  snare: 38,        // Acoustic Snare (D2)
  hihat_closed: 42, // Closed Hi-Hat (F#2)
  hihat_open: 46,   // Open Hi-Hat (A#2)
  crash: 49,        // Crash Cymbal 1 (C#3)
  ride: 51,         // Ride Cymbal 1 (D#3)
  tom_high: 48,     // Hi-Mid Tom (C3)
  tom_low: 43       // High Floor Tom (G2)
});

export const DRUM_GRID_LANES = Object.freeze([
  { id: 'cymbal', label: 'Talerz', icon: '✳️', color: '#eab308', normal: 'ride', accent: 'crash', ghost: 'ride' },
  { id: 'hihat',  label: 'Hi-Hat', icon: '⚡', color: '#38bdf8', normal: 'hihat_closed', accent: 'hihat_open', ghost: 'hihat_closed' },
  { id: 'snare',  label: 'Werbel', icon: '🥁', color: '#22c55e', normal: 'snare', accent: 'snare', ghost: 'snare' },
  { id: 'tom',    label: 'Tom',    icon: '🪘', color: '#a855f7', normal: 'tom_high', accent: 'tom_low', ghost: 'tom_high' },
  { id: 'kick',   label: 'Stopa',  icon: '🦶', color: '#f59e0b', normal: 'kick', accent: 'kick', ghost: 'kick' }
]);

export const DRUM_BLOCKS = Object.freeze([
  {
    id: 'rock_8th',
    name: 'Rock 8th',
    category: 'Rock',
    desc: 'Klasyczny rock na ósemkach (stopa 1 & 3, werbel 2 & 4)',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'hihat_closed'], vel: 0.95 },
      { beatOffsetQ: 0.5, hits: ['hihat_closed'], vel: 0.65 },
      { beatOffsetQ: 1.0, hits: ['snare', 'hihat_closed'], vel: 0.92 },
      { beatOffsetQ: 1.5, hits: ['hihat_closed'], vel: 0.65 },
      { beatOffsetQ: 2.0, hits: ['kick', 'hihat_closed'], vel: 0.92 },
      { beatOffsetQ: 2.5, hits: ['hihat_closed'], vel: 0.68 },
      { beatOffsetQ: 3.0, hits: ['snare', 'hihat_closed'], vel: 0.95 },
      { beatOffsetQ: 3.5, hits: ['hihat_closed'], vel: 0.70 }
    ]
  },
  {
    id: 'hard_rock',
    name: 'Hard Rock',
    category: 'Rock',
    desc: 'Mocny groove z podwójną stopą na 3-and oraz otwartym hi-hatem',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'hihat_closed'], vel: 1.0 },
      { beatOffsetQ: 0.5, hits: ['hihat_closed'], vel: 0.74 },
      { beatOffsetQ: 1.0, hits: ['snare', 'hihat_closed'], vel: 0.98 },
      { beatOffsetQ: 1.5, hits: ['kick', 'hihat_closed'], vel: 0.80 },
      { beatOffsetQ: 2.0, hits: ['kick', 'hihat_closed'], vel: 0.96 },
      { beatOffsetQ: 2.5, hits: ['kick', 'hihat_closed'], vel: 0.84 },
      { beatOffsetQ: 3.0, hits: ['snare', 'hihat_closed'], vel: 1.0 },
      { beatOffsetQ: 3.5, hits: ['hihat_open'], vel: 0.82 }
    ]
  },
  {
    id: 'metal_16th',
    name: 'Metal Double-Kick 16th',
    category: 'Metal',
    desc: 'Szybkie szesnastki podwójnej stopy pod riffy thrash/metalowe',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'ride'], vel: 1.0 },
      { beatOffsetQ: 0.25, hits: ['kick'], vel: 0.80 },
      { beatOffsetQ: 0.5, hits: ['kick', 'ride'], vel: 0.90 },
      { beatOffsetQ: 0.75, hits: ['kick'], vel: 0.80 },
      { beatOffsetQ: 1.0, hits: ['snare', 'kick', 'ride'], vel: 1.0 },
      { beatOffsetQ: 1.25, hits: ['kick'], vel: 0.78 },
      { beatOffsetQ: 1.5, hits: ['kick', 'ride'], vel: 0.90 },
      { beatOffsetQ: 1.75, hits: ['kick'], vel: 0.80 },
      { beatOffsetQ: 2.0, hits: ['kick', 'ride'], vel: 1.0 },
      { beatOffsetQ: 2.25, hits: ['kick'], vel: 0.82 },
      { beatOffsetQ: 2.5, hits: ['kick', 'ride'], vel: 0.92 },
      { beatOffsetQ: 2.75, hits: ['kick'], vel: 0.80 },
      { beatOffsetQ: 3.0, hits: ['snare', 'kick', 'ride'], vel: 1.0 },
      { beatOffsetQ: 3.25, hits: ['kick'], vel: 0.82 },
      { beatOffsetQ: 3.5, hits: ['kick', 'hihat_open'], vel: 0.92 },
      { beatOffsetQ: 3.75, hits: ['kick'], vel: 0.84 }
    ]
  },
  {
    id: 'half_time',
    name: 'Half-Time Breakdown',
    category: 'Metal / Modern',
    desc: 'Ciężki groove half-time (werbel na 3) z akcentami chiny/crash',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'crash'], vel: 1.0 },
      { beatOffsetQ: 0.5, hits: ['hihat_open'], vel: 0.76 },
      { beatOffsetQ: 1.0, hits: ['hihat_open'], vel: 0.72 },
      { beatOffsetQ: 1.5, hits: ['kick', 'hihat_open'], vel: 0.88 },
      { beatOffsetQ: 2.0, hits: ['snare', 'hihat_open'], vel: 1.0 },
      { beatOffsetQ: 2.5, hits: ['hihat_open'], vel: 0.72 },
      { beatOffsetQ: 3.0, hits: ['kick', 'hihat_open'], vel: 0.90 },
      { beatOffsetQ: 3.5, hits: ['kick', 'hihat_open'], vel: 0.85 }
    ]
  },
  {
    id: 'punk_dbeat',
    name: 'Punk D-Beat',
    category: 'Punk',
    desc: 'Energetyczny d-beat z szybkim werblem na „and”',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'hihat_open'], vel: 0.98 },
      { beatOffsetQ: 0.5, hits: ['hihat_open'], vel: 0.74 },
      { beatOffsetQ: 0.75, hits: ['snare'], vel: 0.86 },
      { beatOffsetQ: 1.0, hits: ['hihat_open'], vel: 0.76 },
      { beatOffsetQ: 1.5, hits: ['kick', 'hihat_open'], vel: 0.92 },
      { beatOffsetQ: 2.0, hits: ['snare', 'hihat_open'], vel: 1.0 },
      { beatOffsetQ: 2.5, hits: ['kick', 'hihat_open'], vel: 0.92 },
      { beatOffsetQ: 3.0, hits: ['snare', 'hihat_open'], vel: 0.98 },
      { beatOffsetQ: 3.5, hits: ['kick', 'hihat_open'], vel: 0.88 }
    ]
  },
  {
    id: 'blues_shuffle',
    name: 'Blues / Shuffle',
    category: 'Blues',
    desc: 'Triolowy shuffle na ride/hi-hacie z pulsem bluesowym',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'ride'], vel: 0.92 },
      { beatOffsetQ: 0.667, hits: ['ride'], vel: 0.66 },
      { beatOffsetQ: 1.0, hits: ['snare', 'ride'], vel: 0.88 },
      { beatOffsetQ: 1.667, hits: ['ride'], vel: 0.66 },
      { beatOffsetQ: 2.0, hits: ['kick', 'ride'], vel: 0.90 },
      { beatOffsetQ: 2.667, hits: ['kick', 'ride'], vel: 0.74 },
      { beatOffsetQ: 3.0, hits: ['snare', 'ride'], vel: 0.92 },
      { beatOffsetQ: 3.667, hits: ['ride'], vel: 0.68 }
    ]
  },
  {
    id: 'funk',
    name: 'Funk',
    category: 'Funk / Groove',
    desc: 'Synkopowany funk z duchami na werblu i otwarciem hi-hatu',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'hihat_closed'], vel: 0.96 },
      { beatOffsetQ: 0.5, hits: ['hihat_closed'], vel: 0.68 },
      { beatOffsetQ: 0.75, hits: ['kick'], vel: 0.78 },
      { beatOffsetQ: 1.0, hits: ['snare', 'hihat_closed'], vel: 0.98 },
      { beatOffsetQ: 1.5, hits: ['hihat_closed'], vel: 0.70 },
      { beatOffsetQ: 1.75, hits: ['snare'], vel: 0.42 },
      { beatOffsetQ: 2.0, hits: ['hihat_closed'], vel: 0.72 },
      { beatOffsetQ: 2.25, hits: ['kick'], vel: 0.84 },
      { beatOffsetQ: 2.5, hits: ['kick', 'hihat_closed'], vel: 0.90 },
      { beatOffsetQ: 3.0, hits: ['snare', 'hihat_closed'], vel: 0.98 },
      { beatOffsetQ: 3.5, hits: ['hihat_open'], vel: 0.78 },
      { beatOffsetQ: 3.75, hits: ['snare'], vel: 0.45 }
    ]
  },
  {
    id: 'pop',
    name: 'Pop',
    category: 'Pop',
    desc: 'Przejrzysty groove popowy pod śpiew i gitarę akustyczną',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'hihat_closed'], vel: 0.90 },
      { beatOffsetQ: 0.5, hits: ['hihat_closed'], vel: 0.60 },
      { beatOffsetQ: 1.0, hits: ['snare', 'hihat_closed'], vel: 0.88 },
      { beatOffsetQ: 1.5, hits: ['hihat_closed'], vel: 0.62 },
      { beatOffsetQ: 2.0, hits: ['kick', 'hihat_closed'], vel: 0.85 },
      { beatOffsetQ: 2.5, hits: ['kick', 'hihat_closed'], vel: 0.76 },
      { beatOffsetQ: 3.0, hits: ['snare', 'hihat_closed'], vel: 0.90 },
      { beatOffsetQ: 3.5, hits: ['hihat_closed'], vel: 0.64 }
    ]
  },
  {
    id: 'ballad',
    name: 'Ballad',
    category: 'Ballada',
    desc: 'Delikatny puls balladowy na ride i miękkim werblu',
    steps: [
      { beatOffsetQ: 0.0, hits: ['kick', 'ride'], vel: 0.78 },
      { beatOffsetQ: 0.5, hits: ['ride'], vel: 0.52 },
      { beatOffsetQ: 1.0, hits: ['ride'], vel: 0.56 },
      { beatOffsetQ: 1.5, hits: ['ride'], vel: 0.50 },
      { beatOffsetQ: 2.0, hits: ['snare', 'ride'], vel: 0.76 },
      { beatOffsetQ: 2.5, hits: ['ride'], vel: 0.54 },
      { beatOffsetQ: 3.0, hits: ['kick', 'ride'], vel: 0.70 },
      { beatOffsetQ: 3.5, hits: ['ride'], vel: 0.52 }
    ]
  },
  {
    id: 'break',
    name: 'Break / Cisza',
    category: 'Struktura',
    desc: 'Pusty takt perkusji (zatrzymanie zespołu przed wejściem riffu)',
    steps: []
  }
]);

export function getDrumBlockById(blockId) {
  return DRUM_BLOCKS.find(b => b.id === blockId) || DRUM_BLOCKS[0];
}

function makeCompound68Steps(blockId, capacityQ) {
  // Dla 6/8 (3.0 Q) oraz 12/8 (6.0 Q): akcenty co 1.5 ćwierćnuty (grupy po 3 ósemki)
  const base = [];
  const groups = Math.max(1, Math.round(capacityQ / 1.5));
  for (let g = 0; g < groups; g++) {
    const start = g * 1.5;
    const isSnareGroup = (g % 2 === 1);
    const pulse = (blockId === 'ballad') ? 'ride' : 'hihat_closed';
    if (!isSnareGroup) {
      base.push({ beatOffsetQ: start, hits: ['kick', pulse], vel: 0.92 });
    } else {
      base.push({ beatOffsetQ: start, hits: ['snare', pulse], vel: 0.90 });
    }
    if (start + 0.5 < capacityQ - 1e-6) {
      base.push({ beatOffsetQ: start + 0.5, hits: [pulse], vel: 0.62 });
    }
    if (start + 1.0 < capacityQ - 1e-6) {
      const thirdHits = (blockId === 'hard_rock' || blockId === 'rock_8th') && !isSnareGroup && groups > 1
        ? [pulse]
        : (isSnareGroup && blockId === 'blues_shuffle' ? ['kick', pulse] : [pulse]);
      base.push({ beatOffsetQ: start + 1.0, hits: thirdHits, vel: 0.66 });
    }
  }
  return base;
}

/**
 * Rozwija klocek perkusyjny do posortowanych kroków w obrębie jednego taktu.
 * @param {string} blockId
 * @param {object} opts
 * @returns {Array<{beatOffsetQ:number, hits:Array<{kind:string, midi:number, velocity:number}>}>}
 */
export function expandDrumBlockToSteps(blockId, opts = {}) {
  const block = getDrumBlockById(blockId);
  const numerator = Math.max(1, Number(opts.numerator) || 4);
  const denominator = Math.max(1, Number(opts.denominator) || 4);
  const capacityQ = numerator * (4 / denominator);
  const isBarOne = Boolean(opts.isBarOne);
  const isFillBar = Boolean(opts.isFillBar);
  const crashOnOne = Boolean(opts.crashOnOne);
  const swingRatio = Math.max(0, Math.min(0.45, Number(opts.swingRatio) || 0));

  if (block.id === 'break' || block.steps.length === 0) {
    return [];
  }

  let rawSteps = [];
  if (denominator === 8 && numerator % 3 === 0) {
    rawSteps = makeCompound68Steps(block.id, capacityQ);
  } else {
    // Zawijanie / obcinanie wzorca 4/4 do dowolnego metrum (2/4, 3/4, 5/4, 7/8 itp.)
    const copies = Math.max(1, Math.ceil(capacityQ / 4));
    for (let c = 0; c < copies; c++) {
      const baseOff = c * 4;
      for (const st of block.steps) {
        let q = baseOff + st.beatOffsetQ;
        if (swingRatio > 0 && Math.abs((q % 1) - 0.5) < 0.02) {
          q += swingRatio * 0.35;
        }
        if (q >= -1e-6 && q < capacityQ - 1e-6) {
          rawSteps.push({
            beatOffsetQ: Math.round(q * 1000) / 1000,
            hits: st.hits.slice(),
            vel: st.vel
          });
        }
      }
    }
  }

  // Przejście (Auto-Fill) na końcówce taktu
  if (isFillBar && capacityQ >= 1.5) {
    const fillLen = capacityQ >= 3.0 ? 1.5 : 1.0;
    const fillStartQ = Math.round((capacityQ - fillLen) * 1000) / 1000;
    rawSteps = rawSteps.filter(s => s.beatOffsetQ < fillStartQ - 1e-6);

    const fillTemplate = capacityQ >= 3.0
      ? [
          { dt: 0.0,  hits: ['snare'], vel: 0.90 },
          { dt: 0.25, hits: ['snare'], vel: 0.82 },
          { dt: 0.5,  hits: ['tom_high'], vel: 0.92 },
          { dt: 0.75, hits: ['tom_high'], vel: 0.85 },
          { dt: 1.0,  hits: ['tom_low'], vel: 0.96 },
          { dt: 1.25, hits: ['snare', 'kick'], vel: 1.0 }
        ]
      : [
          { dt: 0.0,  hits: ['snare'], vel: 0.92 },
          { dt: 0.25, hits: ['tom_high'], vel: 0.90 },
          { dt: 0.5,  hits: ['tom_low'], vel: 0.95 },
          { dt: 0.75, hits: ['snare', 'kick'], vel: 1.0 }
        ];

    for (const ft of fillTemplate) {
      const q = Math.round((fillStartQ + ft.dt) * 1000) / 1000;
      if (q < capacityQ - 1e-6) {
        rawSteps.push({ beatOffsetQ: q, hits: ft.hits.slice(), vel: ft.vel });
      }
    }
  }

  // Opcjonalny Crash na raz w 1. takcie
  if (crashOnOne && isBarOne && rawSteps.length > 0) {
    const atZero = rawSteps.find(s => Math.abs(s.beatOffsetQ) < 1e-4);
    if (atZero) {
      if (!atZero.hits.includes('crash')) {
        atZero.hits.push('crash');
      }
    } else {
      rawSteps.unshift({ beatOffsetQ: 0, hits: ['crash'], vel: 0.88 });
    }
  }

  rawSteps.sort((a, b) => a.beatOffsetQ - b.beatOffsetQ);

  return rawSteps.map(st => ({
    beatOffsetQ: st.beatOffsetQ,
    hits: st.hits.map(kind => ({
      kind,
      midi: GM_DRUM_MIDI[kind] ?? GM_DRUM_MIDI.snare,
      velocity: Math.max(0.1, Math.min(1.0, st.vel ?? 0.85))
    }))
  }));
}

let cachedNoiseBuf = null;

function getDrumNoiseBuffer(ctx) {
  if (cachedNoiseBuf && cachedNoiseBuf.sampleRate === ctx.sampleRate) {
    return cachedNoiseBuf;
  }
  const len = Math.max(1, Math.floor(ctx.sampleRate * 0.45));
  const buf = ctx.createBuffer(1, len, ctx.sampleRate);
  const data = buf.getChannelData(0);
  for (let i = 0; i < len; i++) data[i] = Math.random() * 2 - 1;
  cachedNoiseBuf = buf;
  return buf;
}

/**
 * Odtwarza pojedyncze uderzenie perkusyjne przez hosta (playDrumHit) lub własną syntezę WebAudio
 * dla brakujących instrumentów (ride, tom_high, tom_low).
 */
export function playStudioDrumHit(kind, velocity = 1.0, host = null) {
  const vel = Math.max(0.05, Math.min(1.4, Number(velocity) || 1.0));
  const hostNativeKinds = ['kick', 'snare', 'hihat_closed', 'hihat_open', 'crash'];
  if (host && typeof host.playDrumHit === 'function' && hostNativeKinds.includes(kind)) {
    host.playDrumHit(kind, vel);
    return;
  }
  if (!host || typeof host.ensureAudioCtx !== 'function' || typeof host.getMasterAudioOut !== 'function') {
    return;
  }

  const ctx = host.ensureAudioCtx();
  const out = host.getMasterAudioOut();
  if (!ctx || !out) return;
  const now = ctx.currentTime;

  if (kind === 'ride') {
    const src = ctx.createBufferSource();
    src.buffer = getDrumNoiseBuffer(ctx);
    const bp = ctx.createBiquadFilter();
    bp.type = 'bandpass';
    bp.frequency.setValueAtTime(5400, now);
    bp.Q.setValueAtTime(2.6, now);
    const g = ctx.createGain();
    g.gain.setValueAtTime(0.24 * vel, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.36);

    const ping = ctx.createOscillator();
    const pg = ctx.createGain();
    ping.type = 'triangle';
    ping.frequency.setValueAtTime(920, now);
    pg.gain.setValueAtTime(0.06 * vel, now);
    pg.gain.exponentialRampToValueAtTime(0.0005, now + 0.14);

    src.connect(bp); bp.connect(g); g.connect(out);
    ping.connect(pg); pg.connect(out);
    src.start(now); src.stop(now + 0.38);
    ping.start(now); ping.stop(now + 0.16);
  } else if (kind === 'tom_high' || kind === 'tom_low') {
    const startHz = (kind === 'tom_high') ? 185 : 125;
    const endHz = (kind === 'tom_high') ? 105 : 68;
    const osc = ctx.createOscillator();
    const g = ctx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(startHz, now);
    osc.frequency.exponentialRampToValueAtTime(endHz, now + 0.18);
    g.gain.setValueAtTime(0.70 * vel, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.26);
    osc.connect(g); g.connect(out);
    osc.start(now); osc.stop(now + 0.28);
  }
}

export function createEmptyDrumGrid(numerator = 4, denominator = 4, stepsPerBeat = 2) {
  const num = Math.max(1, Math.min(16, Math.round(Number(numerator) || 4)));
  const spb = Number(stepsPerBeat) === 4 ? 4 : 2;
  const stepCount = Math.max(2, Math.min(32, num * spb));
  const lanes = {};
  for (const lane of DRUM_GRID_LANES) {
    lanes[lane.id] = new Array(stepCount).fill(0);
  }
  return {
    stepsPerBeat: spb,
    stepCount,
    numerator: num,
    denominator: [2, 4, 8, 16].includes(Number(denominator)) ? Number(denominator) : 4,
    lanes
  };
}

/**
 * Konwertuje wybrany preset klocka perkusyjnego na 5-wierszową siatkę sekwencera (0=brak, 1=ghost, 2=normalny, 3=akcent).
 */
export function drumBlockToGridPattern(blockId = 'rock_8th', opts = {}) {
  const num = Math.max(1, Math.min(16, Math.round(Number(opts.numerator) || 4)));
  const den = [2, 4, 8, 16].includes(Number(opts.denominator)) ? Number(opts.denominator) : 4;
  const capQ = num * (4 / den);
  const steps = expandDrumBlockToSteps(blockId, {
    numerator: num,
    denominator: den,
    isBarOne: Boolean(opts.isBarOne),
    isFillBar: Boolean(opts.isFillBar),
    crashOnOne: Boolean(opts.crashOnOne)
  });

  const beatQ = 4 / den;
  let spb = Number(opts.stepsPerBeat) === 4 ? 4 : 2;
  if (!opts.stepsPerBeat) {
    for (const st of steps) {
      const idx8 = (st.beatOffsetQ / beatQ) * 2;
      if (Math.abs(idx8 - Math.round(idx8)) > 0.12) {
        spb = 4;
        break;
      }
    }
  }

  const grid = createEmptyDrumGrid(num, den, spb);
  const stepQ = Math.max(0.0625, capQ / grid.stepCount);

  for (const st of steps) {
    const idx = Math.max(0, Math.min(grid.stepCount - 1, Math.round(st.beatOffsetQ / stepQ)));
    for (const h of (st.hits || [])) {
      const v = h.velocity ?? 0.85;
      if (h.kind === 'crash') {
        grid.lanes.cymbal[idx] = 3;
      } else if (h.kind === 'ride') {
        grid.lanes.cymbal[idx] = Math.max(grid.lanes.cymbal[idx], v < 0.65 ? 1 : 2);
      } else if (h.kind === 'hihat_open') {
        grid.lanes.hihat[idx] = 3;
      } else if (h.kind === 'hihat_closed') {
        grid.lanes.hihat[idx] = Math.max(grid.lanes.hihat[idx], v < 0.68 ? 1 : 2);
      } else if (h.kind === 'tom_low') {
        grid.lanes.tom[idx] = 3;
      } else if (h.kind === 'tom_high') {
        grid.lanes.tom[idx] = Math.max(grid.lanes.tom[idx], v < 0.65 ? 1 : 2);
      } else if (h.kind === 'snare') {
        const lvl = v >= 0.96 ? 3 : (v < 0.60 ? 1 : 2);
        grid.lanes.snare[idx] = Math.max(grid.lanes.snare[idx], lvl);
      } else if (h.kind === 'kick') {
        const lvl = v >= 0.96 ? 3 : (v < 0.60 ? 1 : 2);
        grid.lanes.kick[idx] = Math.max(grid.lanes.kick[idx], lvl);
      }
    }
  }
  return grid;
}

/**
 * Rozwija edytowaną siatkę 5-wierszową perkusji do kroków audio/eksportu { beatOffsetQ, hits }.
 */
export function expandDrumCustomGridToSteps(customGrid, { numerator = 4, denominator = 4 } = {}) {
  if (!customGrid || !customGrid.lanes) return [];
  const num = Math.max(1, Math.min(16, Math.round(Number(numerator) || 4)));
  const den = [2, 4, 8, 16].includes(Number(denominator)) ? Number(denominator) : 4;
  const capQ = num * (4 / den);
  const stepCount = Math.max(2, Math.min(32, Math.round(Number(customGrid.stepCount) || (num * (customGrid.stepsPerBeat === 4 ? 4 : 2)))));
  const stepQ = capQ / stepCount;

  const velByLevel = { 1: 0.42, 2: 0.82, 3: 1.0 };
  const steps = [];

  for (let i = 0; i < stepCount; i++) {
    const hits = [];
    for (const lane of DRUM_GRID_LANES) {
      const arr = customGrid.lanes[lane.id];
      const lvl = Array.isArray(arr) ? Math.max(0, Math.min(3, Math.round(Number(arr[i]) || 0))) : 0;
      if (lvl === 0) continue;
      const kind = lvl === 3 ? lane.accent : (lvl === 1 ? lane.ghost : lane.normal);
      hits.push({
        kind,
        midi: GM_DRUM_MIDI[kind] ?? GM_DRUM_MIDI.snare,
        velocity: velByLevel[lvl] || 0.82
      });
    }
    if (hits.length > 0) {
      steps.push({
        beatOffsetQ: Math.round(i * stepQ * 1000) / 1000,
        hits
      });
    }
  }
  return steps;
}

/**
 * Rozwiązuje kroki perkusyjne taktu: własna siatka krokowa ma pierwszeństwo przed presetywnym klockiem.
 */
export function resolveDrumMeasureSteps(measure, track = null, measureIdx = 0) {
  const num = Number(measure?.numerator) || 4;
  const den = Number(measure?.denominator) || 4;
  if (measure?.customDrumGrid && measure.customDrumGrid.lanes) {
    return expandDrumCustomGridToSteps(measure.customDrumGrid, { numerator: num, denominator: den });
  }
  const autoFillN = Number(track?.drumAutoFill) || 0;
  const isFillBar = autoFillN > 0 && ((measureIdx + 1) % autoFillN === 0);
  return expandDrumBlockToSteps(measure?.drumBlockId || 'rock_8th', {
    numerator: num,
    denominator: den,
    isBarOne: measureIdx === 0,
    isFillBar,
    crashOnOne: Boolean(track?.drumCrashOnOne)
  });
}
