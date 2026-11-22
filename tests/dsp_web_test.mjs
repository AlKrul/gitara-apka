import assert from 'node:assert/strict';
import { performance } from 'node:perf_hooks';
import { GuitarNoteEngine } from '../web/js/dsp/guitarDspEngine.js';
import { analyzePitchPlease, GuitarEventTracker } from '../web/js/dsp/pitchPleaseBridge.js';

const SR = 48000;

function synthPluck(buf, startSec, midi, amp, decayTauSec) {
  const f0 = 440 * Math.pow(2, (midi - 69) / 12);
  const startIdx = Math.floor(startSec * SR);
  const weights = [1.0, 0.72, 0.45, 0.25, 0.14];
  for (let i = startIdx; i < buf.length; i++) {
    const t = (i - startIdx) / SR;
    const attack = Math.min(1.0, t / 0.004);
    const env = attack * Math.exp(-t / decayTauSec);
    if (t > 0.01 && env < 1e-5) break;
    let s = 0;
    for (let h = 0; h < weights.length; h++) {
      const fh = f0 * (h + 1);
      if (fh >= SR * 0.45) break;
      const hDecay = Math.exp(-t * (0.45 * h));
      s += weights[h] * hDecay * Math.sin(2 * Math.PI * fh * t);
    }
    buf[i] += amp * env * s;
  }
}

function runEngine(buf, engine) {
  const allEvents = [];
  for (let off = 0; off < buf.length; off += 128) {
    const evs = engine.pushSamples(buf.subarray(off, Math.min(buf.length, off + 128)));
    for (const ev of evs) allEvents.push(ev);
  }
  return allEvents;
}

// 1. Odrzucanie przydźwięku sieciowego 50 Hz oraz uderzeń w pudło 35 Hz
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const buf = new Float32Array(Math.floor(SR * 0.7));
  for (let i = 0; i < buf.length; i++) {
    const t = i / SR;
    buf[i] = 0.20 * Math.sin(2 * Math.PI * 50 * t) + 0.15 * Math.sin(2 * Math.PI * 35 * t);
  }
  const evs = runEngine(buf, engine);
  assert.equal(evs.filter(e => e.type === 'noteOn').length, 0, '50 Hz + 35 Hz hum must not trigger NoteOn');
  console.log('[OK] Test 1: Odrzucanie 50 Hz + 35 Hz');
}

// 2. Pełna skala gitary (Eb2 .. A5): dokładne wykrycie każdej pojedynczej nuty bez fałszywych alarmów
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const testMidis = [39, 40, 45, 50, 55, 59, 64, 72, 81];
  for (const m of testMidis) {
    engine.reset();
    const buf = new Float32Array(Math.floor(SR * 0.65));
    synthPluck(buf, 0.10, m, 0.30, 0.7);
    const evs = runEngine(buf, engine);
    const ons = evs.filter(e => e.type === 'noteOn');
    assert.equal(ons.length, 1, `MIDI ${m}: expected 1 NoteOn, got ${JSON.stringify(ons)}`);
    assert.equal(ons[0].midiNote, m, `MIDI ${m}: detected wrong note ${ons[0].midiNote}`);
    const latencyMs = ons[0].timestampMs - 100;
    assert.ok(latencyMs > 0 && latencyMs <= 75, `MIDI ${m}: latency ${latencyMs.toFixed(1)} ms too high`);
  }
  console.log('[OK] Test 2: Pełna skala gitary Eb2..A5 (9/9 dokładnych trafień)');
}

// 3. Szybkie nakładające się nuty na tle długo wybrzmiewających poprzednich strun
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const buf = new Float32Array(Math.floor(SR * 1.5));
  const seq = [40, 43, 47, 50, 52];
  seq.forEach((m, idx) => synthPluck(buf, 0.10 + idx * 0.175, m, 0.28, 2.5));
  const t0 = performance.now();
  const evs = runEngine(buf, engine);
  const elapsedMs = performance.now() - t0;
  const ons = evs.filter(e => e.type === 'noteOn').map(e => e.midiNote);
  for (const m of seq) {
    assert.ok(ons.includes(m), `Missing overlapping note MIDI ${m} in [${ons.join(', ')}]`);
  }
  const hops = Math.floor(buf.length / 256);
  console.log(`[OK] Test 3: Nakładające się szybkie nuty [${ons.join(', ')}] (${(elapsedMs / hops).toFixed(2)} ms/hop)`);
}

// 4. Śledzenie Sustain i wygaszenie NoteOff przy -20 dB
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const buf = new Float32Array(Math.floor(SR * 1.6));
  synthPluck(buf, 0.10, 57, 0.32, 0.25);
  const evs = runEngine(buf, engine);
  const on = evs.find(e => e.type === 'noteOn' && e.midiNote === 57);
  const off = evs.find(e => e.type === 'noteOff' && e.midiNote === 57);
  assert.ok(on, 'Expected NoteOn for MIDI 57');
  assert.ok(off, 'Expected NoteOff for MIDI 57');
  assert.ok(off.durationMs >= 260 && off.durationMs <= 1150, `Unexpected sustain duration ${off.durationMs} ms`);
  console.log(`[OK] Test 4: Sustain & NoteOff -20 dB (dur=${off.durationMs.toFixed(0)} ms)`);
}

// 5. Re-triggering aktywnej nuty (NoteOff + natychmiastowe NoteOn z flagą isRetrigger)
// (Na prawdziwej gitarze ponowne uderzenie kostką tej samej struny tłumi stare drganie w chwili kontaktu)
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const buf = new Float32Array(Math.floor(SR * 1.2));
  synthPluck(buf, 0.10, 45, 0.30, 0.9);
  // Kostka dotyka struny ok. 0.42 s (krótkie przytłumienie o 60% tuż przed ponownym szarpnięciem o 0.44 s)
  const dampStart = Math.floor(0.42 * SR);
  for (let i = dampStart; i < buf.length; i++) {
    const dt = (i - dampStart) / SR;
    buf[i] *= Math.exp(-dt / 0.015);
  }
  synthPluck(buf, 0.44, 45, 0.34, 1.2);
  const evs = runEngine(buf, engine);
  const ons = evs.filter(e => e.type === 'noteOn' && e.midiNote === 45);
  const reOff = evs.find(e => e.type === 'noteOff' && e.midiNote === 45 && e.isRetrigger);
  const reOn = evs.find(e => e.type === 'noteOn' && e.midiNote === 45 && e.isRetrigger);
  assert.ok(ons.length >= 2, `Expected >=2 NoteOn for MIDI 45, got ${ons.length}`);
  assert.ok(reOff && reOn, 'Expected re-trigger NoteOff + NoteOn pair');
  console.log('[OK] Test 5: Re-triggering aktywnej nuty (NoteOff + NoteOn)');
}

// 6. Równoczesny trójdźwięk E2 + B2 + G3 (40 + 47 + 55)
{
  const engine = new GuitarNoteEngine({ sampleRate: SR });
  const buf = new Float32Array(Math.floor(SR * 0.7));
  synthPluck(buf, 0.10, 40, 0.24, 1.2);
  synthPluck(buf, 0.10, 47, 0.24, 1.2);
  synthPluck(buf, 0.10, 55, 0.24, 1.2);
  const evs = runEngine(buf, engine);
  const ons = evs.filter(e => e.type === 'noteOn').map(e => e.midiNote);
  assert.ok(ons.includes(40) && ons.includes(47) && ons.includes(55), `Chord 40+47+55 missed note(s): [${ons.join(', ')}]`);
  console.log(`[OK] Test 6: Równoczesny akord 40+47+55 -> [${ons.join(', ')}]`);
}

// 7. Kroki o półton (w górę i w dół, jak na prawdziwej gitarze z mikrofonem słuchawkowym 150 Hz HPF)
{
  const synthRealMic = (buf, startSec, endSec, midi, amp) => {
    const f0 = 440 * Math.pow(2, (midi - 69) / 12);
    const sIdx = Math.floor(startSec * SR);
    const eIdx = Math.min(buf.length, Math.floor(endSec * SR));
    const hW = [1.0, 1.35, 1.10, 0.75, 0.45, 0.25];
    for (let i = sIdx; i < eIdx; i++) {
      const t = (i - sIdx) / SR;
      const env = Math.min(1, t / 0.005) * Math.exp(-t / 1.2);
      let s = 0;
      for (let h = 0; h < hW.length; h++) {
        const fh = f0 * (h + 1);
        if (fh >= SR * 0.45) break;
        const micGain = fh / Math.sqrt(fh * fh + 150 * 150);
        s += hW[h] * micGain * Math.exp(-t * 0.35 * h) * Math.sin(2 * Math.PI * fh * t);
      }
      buf[i] += amp * env * s;
    }
  };

  for (const [m1, m2] of [[40, 41], [45, 46], [50, 51], [55, 56], [64, 65], [46, 45], [56, 55]]) {
    const engine = new GuitarNoteEngine({ sampleRate: SR });
    const buf = new Float32Array(SR);
    synthRealMic(buf, 0.08, 0.36, m1, 0.25);
    synthRealMic(buf, 0.36, 0.85, m2, 0.25);
    const ons = runEngine(buf, engine).filter(e => e.type === 'noteOn').map(e => e.midiNote);
    assert.deepEqual(ons, [m1, m2], `Semitone step ${m1}->${m2} got [${ons.join(', ')}]`);
  }
  console.log('[OK] Test 7: Kroki o półton (7/7 w górę i w dół, zero przesunięć o cały ton)');

  // 8. Skoki oktawowe (E2->E3->E4 oraz E4->E3->E2) na mikrofonie z tłumieniem basu
  for (const seq of [[40, 52, 64], [64, 52, 40]]) {
    const engine = new GuitarNoteEngine({ sampleRate: SR });
    const buf = new Float32Array(Math.floor(SR * 1.2));
    synthRealMic(buf, 0.08, 0.40, seq[0], 0.25);
    synthRealMic(buf, 0.40, 0.72, seq[1], 0.25);
    synthRealMic(buf, 0.72, 1.10, seq[2], 0.25);
    const ons = runEngine(buf, engine).filter(e => e.type === 'noteOn').map(e => e.midiNote);
    assert.deepEqual(ons, seq, `Octave jump ${seq.join('->')} got [${ons.join(', ')}]`);
  }
  console.log('[OK] Test 8: Skoki oktawowe E2->E3->E4 i E4->E3->E2 (mikrofon słuchawkowy)');

  // 9. Ekstremalnie słaba podstawowa f0 (5%) + szum tła pokoju
  let seed = 12345;
  const rand = () => { seed = (seed * 1664525 + 1013904223) >>> 0; return (seed / 0xffffffff) * 2 - 1; };
  for (const m of [39, 40, 45, 50, 55, 59, 64]) {
    const engine = new GuitarNoteEngine({ sampleRate: SR });
    const buf = new Float32Array(Math.floor(SR * 0.6));
    for (let i = 0; i < buf.length; i++) buf[i] = 0.002 * rand();
    const f0 = 440 * Math.pow(2, (m - 69) / 12);
    const hW = [0.05, 1.0, 0.95, 0.70, 0.45, 0.25];
    for (let i = Math.floor(0.08 * SR); i < Math.floor(0.52 * SR); i++) {
      const t = (i - Math.floor(0.08 * SR)) / SR;
      const env = Math.min(1, t / 0.005) * Math.exp(-t / 1.1);
      let s = 0;
      for (let h = 0; h < hW.length; h++) {
        const fh = f0 * (h + 1);
        if (fh >= SR * 0.45) break;
        s += hW[h] * Math.exp(-t * 0.35 * h) * Math.sin(2 * Math.PI * fh * t);
      }
      buf[i] += 0.12 * env * s;
    }
    const ons = runEngine(buf, engine).filter(e => e.type === 'noteOn').map(e => e.midiNote);
    assert.deepEqual(ons, [m], `Missing f0 (5%) MIDI ${m} got [${ons.join(', ')}]`);
  }
  console.log('[OK] Test 9: Słabe f0 (5%) + szum tła pokoju (7/7 dokładnych oktaw)');

  // 10. 3-strunowe akordy i arpeggia brzmiące jednocześnie (realistyczny mikrofon 150 Hz HPF + sztywność strun)
  const synthChordString = (buf, startSec, endSec, midi, amp) => {
    const f0 = 440 * Math.pow(2, (midi - 69) / 12);
    const sIdx = Math.floor(startSec * SR);
    const eIdx = Math.min(buf.length, Math.floor(endSec * SR));
    const hW = [1.0, 1.35, 1.10, 0.75, 0.45, 0.25];
    const phase0 = (midi * 1.732) % (2 * Math.PI);
    for (let i = sIdx; i < eIdx; i++) {
      const t = (i - sIdx) / SR;
      const env = Math.min(1, t / 0.006) * Math.exp(-t / 2.0);
      let s = 0;
      for (let h = 0; h < hW.length; h++) {
        const fh = f0 * (h + 1) * (1 + 0.00015 * h * h);
        if (fh >= SR * 0.45) break;
        const micGain = fh / Math.sqrt(fh * fh + 150 * 150);
        s += hW[h] * micGain * Math.exp(-t * 0.35 * h) * Math.sin(2 * Math.PI * fh * t + phase0 * (h + 1));
      }
      buf[i] += amp * env * s;
    }
  };
  for (const [name, expected] of [
    ['Em (40,47,55)', [40, 47, 55]],
    ['A5 (45,52,57)', [45, 52, 57]],
    ['D  (50,57,62)', [50, 57, 62]],
    ['G  (55,59,62)', [55, 59, 62]],
    ['C  (48,52,55)', [48, 52, 55]]
  ]) {
    const engine = new GuitarNoteEngine({ sampleRate: SR });
    const buf = new Float32Array(Math.floor(SR * 0.65));
    expected.forEach((m, idx) => synthChordString(buf, 0.08 + idx * 0.020, 0.65, m, 0.22));
    runEngine(buf, engine);
    const active = engine.activeNotes.map(n => n.midiNote);
    assert.deepEqual(active, expected, `3-string strum ${name}: expected [${expected}], got [${active}]`);
  }
  console.log('[OK] Test 10: 3-strunowe akordy na realnym mikrofonie (Em, A5, D, G, C — 5/5 dokładnie)');
}

// =========================================================================
// TESTY 11-14: GŁÓWNY SILNIK POLIFONICZNY PITCHPLEASE (WASM SIMD + CNN) + BRIDGE
// =========================================================================
{
  const synthHeadsetString = (buf, startSec, durationSec, midi, amp) => {
    const f0Base = 440 * Math.pow(2, (midi - 69) / 12);
    const startIdx = Math.floor(startSec * SR);
    const len = Math.floor(durationSec * SR);
    const maxHarm = Math.min(10, Math.floor((SR * 0.44) / f0Base));
    const phases = new Float64Array(maxHarm + 1);
    for (let i = 0; i < len && startIdx + i < buf.length; i++) {
      const t = i / SR;
      const attack = Math.min(1, t / 0.004);
      const glide = 18 * Math.exp(-t / 0.025);
      const f0 = f0Base * Math.pow(2, glide / 1200);
      let s = 0;
      for (let h = 1; h <= maxHarm; h++) {
        const fH = f0 * h * Math.sqrt(1 + 0.00018 * h * h);
        phases[h] += (2 * Math.PI * fH) / SR;
        const r = fH / 150;
        const hpGain = (r * r) / Math.sqrt(Math.pow(1 - r * r, 2) + Math.pow(1.414 * r, 2));
        s += Math.pow(h, -0.82) * Math.exp(-t * (1.2 + 0.65 * h)) * hpGain * Math.sin(phases[h]);
      }
      buf[startIdx + i] += amp * attack * s;
    }
  };

  // 11. Dwudźwięki (2 struny jednocześnie: sekunda mała, power-chord, kwinta, tercja, oktawa, cichy mikrofon)
  const dyads = [
    ['E2+F2', [40, 41], [0.28, 0.25]],
    ['D3+A3', [50, 57], [0.30, 0.22]],
    ['E2+B2', [40, 47], [0.30, 0.20]],
    ['G3+B3', [55, 59], [0.28, 0.20]],
    ['A2+A3', [45, 57], [0.28, 0.20]],
    ['C3+E3 (cichy)', [48, 52], [0.04, 0.035]]
  ];
  for (const [name, exp, amps] of dyads) {
    const buf = new Float32Array(Math.floor(0.95 * SR));
    synthHeadsetString(buf, 0.10, 0.75, exp[0], amps[0]);
    synthHeadsetString(buf, 0.115, 0.75, exp[1], amps[1]);
    const frames = await analyzePitchPlease(buf, SR, { model: 'piano' });
    const tracker = new GuitarEventTracker({ minMidi: 38, maxMidi: 88, thresholdOn: 0.60, noiseGateRms: 0.0 });
    const counts = new Map();
    let midFrames = 0;
    for (const fr of frames) {
      const { activeNotes } = tracker.step(fr.probs, fr.notes, fr.emitTime * 1000, null);
      if (fr.time >= 0.22 && fr.time <= 0.50) {
        midFrames++;
        for (const n of activeNotes) counts.set(n.midiNote, (counts.get(n.midiNote) || 0) + 1);
      }
    }
    const got = [...counts.entries()].filter(([, c]) => c >= midFrames * 0.5).map(([m]) => m).sort((a, b) => a - b);
    assert.deepEqual(got, exp, `PitchPlease dyad ${name}: expected [${exp}], got [${got}]`);
  }
  console.log('[OK] Test 11: PitchPlease (piano) dwudźwięki 2 struny naraz (6/6 dokładnie, także E2+F2 i cichy mikrofon)');

  // 12. 3-strunowe akordy (Em, A5, D, G, C, Am — 6/6 dokładnie!)
  for (const [name, exp] of [
    ['Em', [40, 47, 55]],
    ['A5', [45, 52, 57]],
    ['D',  [50, 57, 62]],
    ['G',  [55, 59, 62]],
    ['C',  [48, 52, 55]],
    ['Am', [45, 52, 60]]
  ]) {
    const buf = new Float32Array(Math.floor(0.95 * SR));
    exp.forEach((m, idx) => synthHeadsetString(buf, 0.10 + idx * 0.015, 0.75, m, 0.26 - idx * 0.03));
    const frames = await analyzePitchPlease(buf, SR, { model: 'piano' });
    const tracker = new GuitarEventTracker({ minMidi: 38, maxMidi: 88, thresholdOn: 0.60, noiseGateRms: 0.0 });
    const counts = new Map();
    let midFrames = 0;
    for (const fr of frames) {
      const { activeNotes } = tracker.step(fr.probs, fr.notes, fr.emitTime * 1000, null);
      if (fr.time >= 0.22 && fr.time <= 0.55) {
        midFrames++;
        for (const n of activeNotes) counts.set(n.midiNote, (counts.get(n.midiNote) || 0) + 1);
      }
    }
    const got = [...counts.entries()].filter(([, c]) => c >= midFrames * 0.5).map(([m]) => m).sort((a, b) => a - b);
    assert.deepEqual(got, exp, `PitchPlease triad ${name}: expected [${exp}], got [${got}]`);
  }
  console.log('[OK] Test 12: PitchPlease (piano) 3-strunowe akordy (Em, A5, D, G, C, Am — 6/6 dokładnie)');

  // 13. Bardzo szybkie szesnastki (80 ms/nuta = 187 BPM!) — 12/12 bez duchów, średnia latencja < 45 ms
  const ultraSeq = [45, 46, 47, 48, 50, 52, 53, 55, 53, 52, 50, 48];
  const stepSec = 0.080;
  const uBuf = new Float32Array(Math.floor((0.15 + ultraSeq.length * stepSec + 0.30) * SR));
  ultraSeq.forEach((m, i) => synthHeadsetString(uBuf, 0.10 + i * stepSec, 0.10, m, 0.28));
  const uFrames = await analyzePitchPlease(uBuf, SR, { model: 'piano' });
  const uTracker = new GuitarEventTracker({ minMidi: 38, maxMidi: 88, thresholdOn: 0.60, noiseGateRms: 0.0 });
  const uOnsets = [];
  for (const fr of uFrames) {
    const { events } = uTracker.step(fr.probs, fr.notes, fr.emitTime * 1000, null);
    for (const ev of events) if (ev.type === 'noteOn') uOnsets.push(ev);
  }
  assert.deepEqual(uOnsets.map(o => o.midiNote), ultraSeq, `Ultra-fast 16ths got [${uOnsets.map(o => o.midiNote)}]`);
  const avgLatMs = uOnsets.reduce((acc, o, idx) => acc + (o.timestampMs - (100 + idx * 80)), 0) / uOnsets.length;
  assert.ok(avgLatMs > 0 && avgLatMs < 48, `Expected fast emit latency < 48 ms, got ${avgLatMs.toFixed(1)} ms`);
  console.log(`[OK] Test 13: PitchPlease szybkie szesnastki 187 BPM (80 ms/nutę — 12/12, 0 duchów, latencja=${avgLatMs.toFixed(1)} ms)`);

  // 14. GuitarEventTracker: bramka ciszy RMS + re-triggering tej samej struny
  const tr = new GuitarEventTracker({ minMidi: 38, maxMidi: 88, thresholdOn: 0.60, noiseGateRms: 0.004 });
  const probs = new Float32Array(88);
  probs[45 - 21] = 0.92;
  // Cisza (poniżej bramki) — nie powinna włączyć nuty
  tr.step(probs, null, 0, 0.001);
  const q2 = tr.step(probs, null, 12, 0.001);
  assert.equal(q2.activeNotes.length, 0, 'Silence below noiseGateRms must suppress notes');
  // Uderzenie struny A2 (MIDI 45)
  const on1 = tr.step(probs, null, 50, 0.05);
  assert.equal(on1.events.filter(e => e.type === 'noteOn' && e.midiNote === 45).length, 1);
  // Spadek prawdopodobieństwa i ponowne uderzenie tej samej struny po 120 ms
  probs[45 - 21] = 0.56;
  tr.step(probs, null, 110, 0.02);
  probs[45 - 21] = 0.88;
  const ret = tr.step(probs, null, 170, 0.08);
  assert.ok(ret.events.some(e => e.type === 'noteOff' && e.isRetrigger), 'Expected re-trigger NoteOff');
  assert.ok(ret.events.some(e => e.type === 'noteOn' && e.isRetrigger && e.midiNote === 45), 'Expected re-trigger NoteOn');
  console.log('[OK] Test 14: GuitarEventTracker bramka ciszy RMS + re-triggering tej samej struny');

  // 15. Wzmacniacz gitarowy (crunch + LPF kolumny + mikrofon) — cienkie struny (dwudźwięki i 3. cieńsza struna)!
  const biquadFilter = (buf, type, fc, Q = 0.707, gainDb = 0) => {
    const w0 = 2 * Math.PI * fc / SR;
    const c = Math.cos(w0), s = Math.sin(w0), a = s / (2 * Q);
    const A = Math.pow(10, gainDb / 40);
    let b0, b1, b2, a0, a1, a2;
    if (type === 'lowpass') { b0 = (1 - c) / 2; b1 = 1 - c; b2 = (1 - c) / 2; a0 = 1 + a; a1 = -2 * c; a2 = 1 - a; }
    else if (type === 'highpass') { b0 = (1 + c) / 2; b1 = -(1 + c); b2 = (1 + c) / 2; a0 = 1 + a; a1 = -2 * c; a2 = 1 - a; }
    else if (type === 'highshelf') {
      const sq = 2 * Math.sqrt(A) * a;
      b0 = A * ((A + 1) + (A - 1) * c + sq); b1 = -2 * A * ((A - 1) + (A + 1) * c); b2 = A * ((A + 1) + (A - 1) * c - sq);
      a0 = (A + 1) - (A - 1) * c + sq; a1 = 2 * ((A - 1) - (A + 1) * c); a2 = (A + 1) - (A - 1) * c - sq;
    } else {
      const sq = 2 * Math.sqrt(A) * a;
      b0 = A * ((A + 1) - (A - 1) * c + sq); b1 = 2 * A * ((A - 1) - (A + 1) * c); b2 = A * ((A + 1) - (A - 1) * c - sq);
      a0 = (A + 1) + (A - 1) * c + sq; a1 = -2 * ((A - 1) + (A + 1) * c); a2 = (A + 1) + (A - 1) * c - sq;
    }
    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0;
    let x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    const out = new Float32Array(buf.length);
    for (let i = 0; i < buf.length; i++) {
      const x0 = buf[i];
      const y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
      out[i] = y0; x2 = x1; x1 = x0; y2 = y1; y1 = y0;
    }
    return out;
  };
  const synthAmpAndPreEq = (notes, drive, thinAmp = 0.15) => {
    const raw = new Float32Array(Math.floor(0.85 * SR));
    notes.forEach((m, idx) => {
      const isThin = m >= 54;
      const f0 = 440 * Math.pow(2, (m - 69) / 12);
      const sIdx = Math.floor((0.10 + idx * 0.015) * SR);
      const len = Math.floor(0.65 * SR);
      const H = Math.min(isThin ? 5 : 8, Math.floor((SR * 0.44) / f0));
      const ph = new Float64Array(H + 1);
      const amp = isThin ? thinAmp : 0.34;
      for (let i = 0; i < len && sIdx + i < raw.length; i++) {
        const t = i / SR;
        const att = Math.min(1, t / 0.004);
        let s = 0;
        for (let h = 1; h <= H; h++) {
          const fH = f0 * h * Math.sqrt(1 + 0.00018 * h * h);
          ph[h] += (2 * Math.PI * fH) / SR;
          s += Math.pow(h, isThin ? -1.65 : -0.90) * Math.exp(-t * (1.3 + 0.7 * h)) * Math.sin(ph[h]);
        }
        raw[sIdx + i] += amp * att * s;
      }
    });
    const amp = new Float32Array(raw.length);
    const norm = Math.tanh(drive);
    for (let i = 0; i < raw.length; i++) {
      const x = raw[i] * drive;
      amp[i] = drive <= 1.05 ? raw[i] : (x >= 0 ? Math.tanh(x) : Math.tanh(x * 0.85)) / norm;
    }
    let y = biquadFilter(biquadFilter(amp, 'lowpass', 2400), 'highpass', 140);
    // Nasz filtr wzmacniacza/mikrofonu (taki sam jak w createGuitarPitchDetector)
    y = biquadFilter(y, 'highpass', 68);
    y = biquadFilter(y, 'lowshelf', 270, 0.707, -4.5);
    y = biquadFilter(y, 'highshelf', 580, 0.707, 6.0);
    return y;
  };

  for (const [name, exp, drive, thinAmp] of [
    ['Crunch Thin Gb3+Bb3', [54, 58], 5.0, 0.15],
    ['Crunch Thin Bb3+Eb4', [58, 63], 5.0, 0.15],
    ['Crunch Thin B3+E4',   [59, 64], 5.0, 0.15],
    ['Crunch Thin G4+B4',   [67, 71], 5.0, 0.15],
    ['Crunch Thin B4+E5',   [71, 76], 5.0, 0.15],
    ['Crunch 3-str Ebm',    [39, 46, 54], 5.0, 0.15],
    ['Crunch 3-str Abm',    [44, 51, 59], 5.0, 0.15],
    ['Crunch 3-str Am',     [45, 52, 60], 5.0, 0.15],
    ['Clean 3-str D (G-B-E)', [57, 62, 66], 1.0, 0.16]
  ]) {
    const sig = synthAmpAndPreEq(exp, drive, thinAmp);
    const frames = await analyzePitchPlease(sig, SR, { model: 'fast' });
    const trk = new GuitarEventTracker({
      minMidi: 38, maxMidi: 88, thresholdOn: 0.58, highThresholdOn: 0.42, noiseGateRms: 0.0
    });
    const counts = new Map();
    let midFrames = 0;
    for (const fr of frames) {
      const endIdx = Math.min(sig.length, Math.floor(fr.time * SR));
      const slice = endIdx >= 2048 ? sig.subarray(endIdx - 2048, endIdx) : null;
      const { activeNotes } = trk.step(fr.probs, fr.notes, fr.emitTime * 1000, null, slice, SR);
      if (fr.time >= 0.18 && fr.time <= 0.55) {
        midFrames++;
        for (const n of activeNotes) counts.set(n.midiNote, (counts.get(n.midiNote) || 0) + 1);
      }
    }
    const got = [...counts.entries()].filter(([, c]) => c >= midFrames * 0.35).map(([m]) => m);
    for (const m of exp) {
      assert.ok(got.includes(m), `${name}: missing thin/chord MIDI ${m} (got [${got.sort((a,b)=>a-b)}])`);
    }
  }
  console.log('[OK] Test 15: Wzmacniacz Crunch + EQ + cienkie struny (5/5 dwudźwięków G–B–E + 4/4 akordy 3-strunowe z 3. cienką)');

  // 16. Tryb Zero-Calibration (samoadaptująca się bramka szumu wzmacniacza i czułość cienkich strun bez kalibracji)
  {
    const zeroSig = synthAmpAndPreEq([55, 59, 64], 4.2, 0.14); // Cienkie struny G3-B3-E4 na wzmacniaczu
    // Dodaj realistyczny szum tła wzmacniacza w pierwszym 100 ms przed atakiem i w całym buforze
    let zSeed = 9911;
    const zRand = () => { zSeed = (zSeed * 1664525 + 1013904223) >>> 0; return (zSeed / 0xffffffff) * 2 - 1; };
    for (let i = 0; i < zeroSig.length; i++) {
      zeroSig[i] += 0.0055 * zRand();
    }
    const zFrames = await analyzePitchPlease(zeroSig, SR, { model: 'fast' });
    const zTrk = new GuitarEventTracker({
      minMidi: 38, maxMidi: 88, thresholdOn: 0.58, highThresholdOn: 0.42, noiseGateRms: 0.0020, autoAdapt: true
    });
    const zCounts = new Map();
    let midCount = 0;
    let earlySilenceFalse = 0;
    for (const fr of zFrames) {
      const endIdx = Math.min(zeroSig.length, Math.floor(fr.time * SR));
      const slice = endIdx >= 2048 ? zeroSig.subarray(endIdx - 2048, endIdx) : null;
      const rmsWin = endIdx >= 256 ? zeroSig.subarray(Math.max(0, endIdx - 512), endIdx) : null;
      let rms = null;
      if (rmsWin) {
        let s = 0;
        for (let k = 0; k < rmsWin.length; k++) s += rmsWin[k] * rmsWin[k];
        rms = Math.sqrt(s / rmsWin.length);
      }
      const { activeNotes, events } = zTrk.step(fr.probs, fr.notes, fr.emitTime * 1000, rms, slice, SR);
      if (fr.time < 0.08 && events.some(e => e.type === 'noteOn')) earlySilenceFalse++;
      if (fr.time >= 0.18 && fr.time <= 0.55) {
        midCount++;
        for (const n of activeNotes) zCounts.set(n.midiNote, (zCounts.get(n.midiNote) || 0) + 1);
      }
    }
    assert.equal(earlySilenceFalse, 0, 'Zero-Calibration noise floor must reject amp hum before attack');
    const zGot = [...zCounts.entries()].filter(([, c]) => c >= midCount * 0.30).map(([m]) => m);
    for (const m of [55, 59, 64]) {
      assert.ok(zGot.includes(m), `Zero-Calibration missed thin string MIDI ${m}, got [${zGot.sort((a,b)=>a-b)}]`);
    }
    console.log('[OK] Test 16: Silnik Zero-Calibration (szum wzmacniacza + cienkie struny G3-B3-E4 bez kalibracji)');
  }
}

console.log('Wszystkie 16 testow Web DSP (ES6 + PitchPlease WASM) zaliczone!');
