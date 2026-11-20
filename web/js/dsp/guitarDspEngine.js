/**
 * Główna fasada potoku DSP czasu rzeczywistego dla gitary:
 *   BandpassPreFilter + AudioRingBuffer -> CqtAnalyzer (VQT/CQT) -> SpectralOnsetHps -> ActiveNoteTracker (MIDI)
 */
import { createGuitarDspConfig, DEFAULT_GUITAR_DSP_CONFIG } from './config.js';
import { AudioRingBuffer, BandpassPreFilter } from './frameBuffer.js';
import { CqtAnalyzer } from './vqtBank.js';
import { SpectralOnsetHps } from './onsetDetector.js';
import { ActiveNoteTracker } from './noteTracker.js';

export { DEFAULT_GUITAR_DSP_CONFIG, createGuitarDspConfig };

export class GuitarNoteEngine {
  constructor(overrides = {}) {
    this.preFilter = new BandpassPreFilter();
    this.ringBuffer = new AudioRingBuffer(4096);
    this.cqt = new CqtAnalyzer();
    this.onsetHps = new SpectralOnsetHps();
    this.tracker = new ActiveNoteTracker();
    this.prepare(overrides);
  }

  prepare(overrides = {}) {
    this.cfg = createGuitarDspConfig(overrides);
    this.cfg.hopSize = Math.max(64, this.cfg.hopSize | 0);
    this.cfg.maxWindowSize = Math.max(this.cfg.maxWindowSize | 0, this.cfg.hopSize * 2);

    this.preFilter.prepare(this.cfg.sampleRate, this.cfg.bandpassLowHz, this.cfg.bandpassHighHz, this.cfg.mainsNotchHz);
    this.ringBuffer.prepare(this.cfg.maxWindowSize);
    this.cqt.prepare(this.cfg);
    this.onsetHps.prepare(this.cfg, this.cqt);
    this.tracker.prepare(this.cfg);

    this.windowWorkBuf = new Float32Array(this.cfg.maxWindowSize);
    this.currentSpectrum = new Float32Array(this.cqt.binCount);
    this.reset();
  }

  reset() {
    this.preFilter.reset();
    this.ringBuffer.reset();
    this.onsetHps.reset();
    this.tracker.reset();
    this.windowWorkBuf.fill(0);
    this.currentSpectrum.fill(0);
    this.samplesUntilNextHop = this.cfg.hopSize;
    this.totalProcessedSamples = 0;
    this.currentTimeMs = 0;
  }

  /**
   * Przetwarza kolejny blok próbek PCM (np. 128 lub 256 z AudioWorklet) i zwraca zdarzenia MIDI (noteOn / noteOff).
   */
  pushSamples(samples) {
    const events = [];
    if (!samples || samples.length === 0) return events;

    for (let i = 0; i < samples.length; i++) {
      const filtered = this.preFilter.processSample(samples[i]);
      this.ringBuffer.pushSample(filtered);
      this.totalProcessedSamples++;

      if (--this.samplesUntilNextHop === 0) {
        this.samplesUntilNextHop = this.cfg.hopSize;
        this.currentTimeMs = (1000 * this.totalProcessedSamples) / this.cfg.sampleRate;
        this.processSingleHop(events);
      }
    }
    return events;
  }

  processSingleHop(outEvents) {
    this.ringBuffer.copyLatest(this.windowWorkBuf, this.windowWorkBuf.length);
    this.cqt.analyze(this.windowWorkBuf, this.currentSpectrum);
    const mpmInfo = this.cqt.analyzeMpm(
      this.windowWorkBuf,
      this.cfg.fundamentalMinHz,
      this.cfg.fundamentalMaxHz,
      this.cfg.minRmsGate
    );
    const candidates = this.onsetHps.processFrame(
      this.cqt,
      this.currentSpectrum,
      mpmInfo,
      this.tracker.activeNotes
    );
    this.tracker.processHop(
      this.cqt,
      this.currentSpectrum,
      this.onsetHps.deltaSpec,
      candidates,
      mpmInfo,
      this.currentTimeMs,
      outEvents
    );
  }

  get activeNotes() {
    return this.tracker.activeNotes;
  }

  get latestSpectrum() {
    return this.currentSpectrum;
  }

  get latestFluxSpectrum() {
    return this.onsetHps.deltaSpec;
  }

  get latestWindow() {
    return this.windowWorkBuf;
  }
}

// Udostępnienie globalne dla aplikacji webowej (DesktopGuiHtml) przy zachowaniu pełnej modularności ES6
if (typeof window !== 'undefined') {
  window.GuitarDsp = {
    DEFAULT_GUITAR_DSP_CONFIG,
    createGuitarDspConfig,
    GuitarNoteEngine
  };
}
