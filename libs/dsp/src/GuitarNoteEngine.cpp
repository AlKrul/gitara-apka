#include "gitara/dsp/GuitarNoteEngine.h"
#include <algorithm>

namespace gitara::dsp {

GuitarNoteEngine::GuitarNoteEngine(const GuitarDspConfig& config) {
    prepare(config);
}

void GuitarNoteEngine::prepare(const GuitarDspConfig& config) {
    cfg_ = config;
    if (cfg_.sampleRate <= 0.0) cfg_.sampleRate = 48000.0;
    cfg_.hopSize = std::max<std::size_t>(64, cfg_.hopSize);
    cfg_.maxWindowSize = std::max<std::size_t>(cfg_.maxWindowSize, cfg_.hopSize * 2);

    preFilter_.prepare(cfg_.sampleRate, cfg_.bandpassLowHz, cfg_.bandpassHighHz, cfg_.mainsNotchHz);
    ringBuffer_.prepare(cfg_.maxWindowSize);
    cqt_.prepare(cfg_);
    onsetHps_.prepare(cfg_, cqt_);
    tracker_.prepare(cfg_);

    windowWorkBuf_.assign(cfg_.maxWindowSize, 0.0f);
    currentSpectrum_.assign(cqt_.binCount(), 0.0f);
    hopCandidates_.reserve(static_cast<std::size_t>(cfg_.maxSimultaneousOnsets + 2));

    reset();
}

void GuitarNoteEngine::reset() noexcept {
    preFilter_.reset();
    ringBuffer_.reset();
    onsetHps_.reset();
    tracker_.reset();
    std::fill(windowWorkBuf_.begin(), windowWorkBuf_.end(), 0.0f);
    std::fill(currentSpectrum_.begin(), currentSpectrum_.end(), 0.0f);
    hopCandidates_.clear();
    samplesUntilNextHop_ = cfg_.hopSize;
    totalProcessedSamples_ = 0;
    currentTimeMs_ = 0.0;
}

void GuitarNoteEngine::pushSamples(const float* samples,
                                   std::size_t numSamples,
                                   std::vector<NoteEvent>& outEvents) {
    if (!samples || numSamples == 0) return;

    for (std::size_t i = 0; i < numSamples; ++i) {
        // Etap 1: Pre-processing (filtracja pasmowoprzepustowa) + bufor kołowy
        const float filtered = preFilter_.processSample(samples[i]);
        ringBuffer_.pushSample(filtered);
        ++totalProcessedSamples_;

        if (--samplesUntilNextHop_ == 0) {
            samplesUntilNextHop_ = cfg_.hopSize;
            currentTimeMs_ = 1000.0 * static_cast<double>(totalProcessedSamples_) / cfg_.sampleRate;
            processSingleHop(outEvents);
        }
    }
}

void GuitarNoteEngine::processSingleHop(std::vector<NoteEvent>& outEvents) {
    // Wyciągnij najnowsze `maxWindowSize` przefiltrowanych próbek (causal, zero alokacji)
    ringBuffer_.copyLatest(windowWorkBuf_.data(), windowWorkBuf_.size());

    // Etap 2: Analiza widmowa VQT/CQT + szybki weryfikator czasowy MPM (NSDF)
    cqt_.analyze(windowWorkBuf_.data(), windowWorkBuf_.size(), currentSpectrum_);
    MpmFrameEstimate mpmInfo = cqt_.analyzeMpm(
        windowWorkBuf_.data(),
        windowWorkBuf_.size(),
        cfg_.fundamentalMinHz,
        cfg_.fundamentalMaxHz,
        cfg_.minRmsGate);

    // Etap 3: Positive Spectral Flux ΔS(t) + hybrydowy Odd-Harmonic HPS + MPM (uzupełnia flagi monofonii)
    onsetHps_.processFrame(cqt_, currentSpectrum_, mpmInfo, tracker_.activeNotes(), hopCandidates_);

    // Etap 4: Maszyna stanów MIDI (Sustain na S(t), -20 dB Note Off, Note On i Re-triggering)
    tracker_.processHop(
        cqt_,
        currentSpectrum_,
        onsetHps_.latestFluxSpectrum(),
        hopCandidates_,
        mpmInfo,
        currentTimeMs_,
        outEvents);
}

} // namespace gitara::dsp
