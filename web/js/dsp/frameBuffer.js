/**
 * Etap 1 potoku DSP:
 *  - BandpassPreFilter: filtr pasmowoprzepustowy Butterwortha 4. rzędu (2× HPF + 2× LPF) + opcjonalny Notch 50/60 Hz.
 *  - AudioRingBuffer: bezalokacyjny bufor kołowy dla okien przyczynowych czasu rzeczywistego.
 */

class BiquadSection {
  constructor() {
    this.b0 = 1; this.b1 = 0; this.b2 = 0;
    this.a1 = 0; this.a2 = 0;
    this.z1 = 0; this.z2 = 0;
  }

  reset() {
    this.z1 = 0;
    this.z2 = 0;
  }

  processSample(x) {
    const y = this.b0 * x + this.z1;
    this.z1 = this.b1 * x - this.a1 * y + this.z2;
    this.z2 = this.b2 * x - this.a2 * y;
    return y;
  }
}

function makeHighpass(sampleRate, cutoffHz, q) {
  const sec = new BiquadSection();
  const fc = Math.max(5, Math.min(sampleRate * 0.45, cutoffHz));
  const w0 = (2 * Math.PI * fc) / sampleRate;
  const cosw0 = Math.cos(w0);
  const sinw0 = Math.sin(w0);
  const alpha = sinw0 / (2 * q);
  const a0 = 1 + alpha;
  sec.b0 = ((1 + cosw0) * 0.5) / a0;
  sec.b1 = -(1 + cosw0) / a0;
  sec.b2 = ((1 + cosw0) * 0.5) / a0;
  sec.a1 = (-2 * cosw0) / a0;
  sec.a2 = (1 - alpha) / a0;
  return sec;
}

function makeLowpass(sampleRate, cutoffHz, q) {
  const sec = new BiquadSection();
  const fc = Math.max(20, Math.min(sampleRate * 0.45, cutoffHz));
  const w0 = (2 * Math.PI * fc) / sampleRate;
  const cosw0 = Math.cos(w0);
  const sinw0 = Math.sin(w0);
  const alpha = sinw0 / (2 * q);
  const a0 = 1 + alpha;
  sec.b0 = ((1 - cosw0) * 0.5) / a0;
  sec.b1 = (1 - cosw0) / a0;
  sec.b2 = ((1 - cosw0) * 0.5) / a0;
  sec.a1 = (-2 * cosw0) / a0;
  sec.a2 = (1 - alpha) / a0;
  return sec;
}

function makeNotch(sampleRate, centerHz, q) {
  const sec = new BiquadSection();
  const fc = Math.max(10, Math.min(sampleRate * 0.45, centerHz));
  const w0 = (2 * Math.PI * fc) / sampleRate;
  const cosw0 = Math.cos(w0);
  const sinw0 = Math.sin(w0);
  const alpha = sinw0 / (2 * q);
  const a0 = 1 + alpha;
  sec.b0 = 1 / a0;
  sec.b1 = (-2 * cosw0) / a0;
  sec.b2 = 1 / a0;
  sec.a1 = (-2 * cosw0) / a0;
  sec.a2 = (1 - alpha) / a0;
  return sec;
}

export class BandpassPreFilter {
  constructor() {
    this.hp1 = new BiquadSection();
    this.hp2 = new BiquadSection();
    this.lp1 = new BiquadSection();
    this.lp2 = new BiquadSection();
    this.notch = new BiquadSection();
    this.notchEnabled = false;
  }

  prepare(sampleRate, lowCutoffHz, highCutoffHz, mainsNotchHz = 50.0) {
    const sr = sampleRate > 0 ? sampleRate : 48000;
    const Q1 = 0.541196100146197;
    const Q2 = 1.3065629648763766;
    this.hp1 = makeHighpass(sr, lowCutoffHz, Q1);
    this.hp2 = makeHighpass(sr, lowCutoffHz, Q2);
    this.lp1 = makeLowpass(sr, highCutoffHz, Q1);
    this.lp2 = makeLowpass(sr, highCutoffHz, Q2);
    this.notchEnabled = mainsNotchHz >= 20 && mainsNotchHz < sr * 0.45;
    if (this.notchEnabled) {
      this.notch = makeNotch(sr, mainsNotchHz, 10.0);
    }
    this.reset();
  }

  reset() {
    this.hp1.reset();
    this.hp2.reset();
    this.lp1.reset();
    this.lp2.reset();
    this.notch.reset();
  }

  processSample(x) {
    let y = this.hp1.processSample(x);
    y = this.hp2.processSample(y);
    if (this.notchEnabled) y = this.notch.processSample(y);
    y = this.lp1.processSample(y);
    y = this.lp2.processSample(y);
    return y;
  }
}

export class AudioRingBuffer {
  constructor(capacity = 4096) {
    this.prepare(capacity);
  }

  prepare(capacity) {
    this.buffer = new Float32Array(Math.max(256, capacity | 0));
    this.writePos = 0;
    this.totalWritten = 0;
  }

  reset() {
    this.buffer.fill(0);
    this.writePos = 0;
    this.totalWritten = 0;
  }

  pushSample(sample) {
    this.buffer[this.writePos] = sample;
    this.writePos = (this.writePos + 1) % this.buffer.length;
    this.totalWritten++;
  }

  copyLatest(dest, length = dest.length) {
    const cap = this.buffer.length;
    const useLen = Math.min(length, cap);
    const start = (this.writePos + cap - useLen) % cap;
    const firstPart = Math.min(useLen, cap - start);
    dest.set(this.buffer.subarray(start, start + firstPart), 0);
    if (firstPart < useLen) {
      dest.set(this.buffer.subarray(0, useLen - firstPart), firstPart);
    }
  }
}
