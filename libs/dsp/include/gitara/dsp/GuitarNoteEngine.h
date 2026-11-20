#pragma once

#include "gitara/dsp/ActiveNoteTracker.h"
#include "gitara/dsp/BandpassPreFilter.h"
#include "gitara/dsp/CqtAnalyzer.h"
#include "gitara/dsp/GuitarDspConfig.h"
#include "gitara/dsp/SpectralOnsetHps.h"
#include <cstddef>
#include <vector>

namespace gitara::dsp {

/// Kompletna fasada czasu rzeczywistego łącząca wszystkie 4 etapy potoku DSP gitary:
///   1) BandpassPreFilter + AudioRingBuffer (buforowanie hop-by-hop)
///   2) CqtAnalyzer (Variable-Q / Constant-Q logarytmiczna banka filtrów)
///   3) SpectralOnsetHps (Positive Spectral Flux ΔS(t) + Log-HPS z iteracyjnym odejmowaniem)
///   4) ActiveNoteTracker (maszyna stanów MIDI: Note On / Sustain / -20 dB Note Off / Re-trigger)
class GuitarNoteEngine {
public:
    GuitarNoteEngine() = default;
    explicit GuitarNoteEngine(const GuitarDspConfig& config);

    void prepare(const GuitarDspConfig& config);
    void reset() noexcept;

    /// Podaje dowolny blok próbek PCM (np. 128, 256, 512 próbek z interfejsu audio).
    /// Wewnętrznie wyzwala analizę co dokładnie `config.hopSize` próbek i dopisuje zdarzenia do `outEvents`.
    void pushSamples(const float* samples,
                     std::size_t numSamples,
                     std::vector<NoteEvent>& outEvents);

    [[nodiscard]] const GuitarDspConfig& config() const noexcept { return cfg_; }
    [[nodiscard]] const std::vector<ActiveNote>& activeNotes() const noexcept { return tracker_.activeNotes(); }
    [[nodiscard]] const std::vector<float>& latestSpectrum() const noexcept { return currentSpectrum_; }
    [[nodiscard]] const std::vector<float>& latestFluxSpectrum() const noexcept { return onsetHps_.latestFluxSpectrum(); }
    [[nodiscard]] double currentTimeMs() const noexcept { return currentTimeMs_; }

private:
    void processSingleHop(std::vector<NoteEvent>& outEvents);

    GuitarDspConfig cfg_{};
    BandpassPreFilter preFilter_{};
    AudioRingBuffer ringBuffer_{};
    CqtAnalyzer cqt_{};
    SpectralOnsetHps onsetHps_{};
    ActiveNoteTracker tracker_{};

    std::size_t samplesUntilNextHop_{256};
    std::size_t totalProcessedSamples_{0};
    double currentTimeMs_{0.0};

    std::vector<float> windowWorkBuf_;
    std::vector<float> currentSpectrum_;
    std::vector<OnsetCandidate> hopCandidates_;
};

} // namespace gitara::dsp
