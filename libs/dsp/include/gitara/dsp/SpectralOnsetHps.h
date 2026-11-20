#pragma once

#include "gitara/dsp/CqtAnalyzer.h"
#include "gitara/dsp/GuitarDspConfig.h"
#include <cstddef>
#include <vector>

namespace gitara::dsp {

struct ActiveNote;

/// Kandydat nowego uderzenia (ataku) wyizolowany hybrydowo (MPM + Positive Spectral Flux + Odd-Harmonic HPS).
struct OnsetCandidate {
    int midiNote{0};
    double exactMidi{0.0};
    double frequencyHz{0.0};
    double centsOffset{0.0};
    float fluxPeakEnergy{0.0f};
    float currentBandEnergy{0.0f};
    float hpsScore{0.0f};
    float confidence{0.0f};
    bool fromMpm{false};
};

class SpectralOnsetHps {
public:
    void prepare(const GuitarDspConfig& config, const CqtAnalyzer& cqt);
    void reset() noexcept;

    void processFrame(const CqtAnalyzer& cqt,
                      const std::vector<float>& currentSpectrum,
                      MpmFrameEstimate& mpmInfo,
                      const std::vector<ActiveNote>& activeNotes,
                      std::vector<OnsetCandidate>& outCandidates);

    [[nodiscard]] const std::vector<float>& latestFluxSpectrum() const noexcept { return deltaSpec_; }
    [[nodiscard]] float totalPositiveFlux() const noexcept { return totalFlux_; }

private:
    void classifyMpmMonophony(const CqtAnalyzer& cqt,
                              const std::vector<float>& currentSpectrum,
                              MpmFrameEstimate& mpmInfo) const noexcept;
    [[nodiscard]] float readBand(const std::vector<float>& spec, std::size_t binIdx) const noexcept;
    [[nodiscard]] std::size_t resolveOctaveBin(const CqtAnalyzer& cqt,
                                               const std::vector<float>& spec,
                                               const std::vector<float>& deltaSpec,
                                               std::size_t candidateBin) const noexcept;

    GuitarDspConfig cfg_{};
    std::size_t binCount_{0};
    std::size_t lookbackHops_{9};
    std::size_t historyWriteIdx_{0};
    std::size_t framesSeen_{0};
    std::size_t minFundBin_{0};
    std::size_t maxFundBin_{0};

    std::array<std::size_t, GuitarDspConfig::kMaxHarmonics> harmonicBinOffsets_{};

    std::vector<std::vector<float>> spectrumHistory_;
    std::vector<float> rmsHistory_;
    std::vector<float> deltaSpec_;
    std::vector<float> workSpec_;
    std::vector<float> workDelta_;
    std::vector<float> hpsScores_;
    float totalFlux_{0.0f};
};

} // namespace gitara::dsp
