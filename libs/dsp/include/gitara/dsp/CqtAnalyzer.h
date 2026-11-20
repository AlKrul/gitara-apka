#pragma once

#include "gitara/dsp/GuitarDspConfig.h"
#include <cstddef>
#include <vector>

namespace gitara::dsp {

/// Wynik szybkiego decymowanego estymatora MPM (McLeod Pitch Method / NSDF) w dziedzinie czasu.
struct MpmFrameEstimate {
    bool valid{false};
    int midiNote{-1};
    double exactMidi{0.0};
    double frequencyHz{0.0};
    double centsOffset{0.0};
    float clarity{0.0f};
    float rms{0.0f};
    float shortRms{0.0f};
    bool isTrulyMonophonic{false};
    bool isVirtualFund{false};
};

/// Etap 2 potoku DSP:
///  - Banka filtrów Variable-Q / Constant-Q (VQT / CQT) z wąskim prawoskrętnym oknem Hanna
///    (2x węższy płat główny niż Blackman-Harris -> rozdziela sąsiednie półtony!).
///  - Szybki decymowany analizator MPM (NSDF) odporny na wytłumione f0 w mikrofonach słuchawkowych.
class CqtAnalyzer {
public:
    void prepare(const GuitarDspConfig& config);

    void analyze(const float* windowSamples,
                 std::size_t numSamples,
                 std::vector<float>& outMagnitudes) const noexcept;

    [[nodiscard]] MpmFrameEstimate analyzeMpm(const float* windowSamples,
                                              std::size_t numSamples,
                                              double minHz,
                                              double maxHz,
                                              float minRmsGate) const noexcept;

    [[nodiscard]] double refineExactMidi(const std::vector<float>& magnitudes,
                                         std::size_t binIdx) const noexcept;

    [[nodiscard]] std::size_t binCount() const noexcept { return bins_.size(); }
    [[nodiscard]] int binsPerSemitone() const noexcept { return binsPerSemitone_; }
    [[nodiscard]] int minMidiNote() const noexcept { return minMidiNote_; }
    [[nodiscard]] int maxMidiNote() const noexcept { return maxMidiNote_; }
    [[nodiscard]] double a4ReferenceHz() const noexcept { return a4ReferenceHz_; }

    [[nodiscard]] double binCenterFreqHz(std::size_t binIdx) const noexcept;
    [[nodiscard]] double binMidiFloat(std::size_t binIdx) const noexcept;
    [[nodiscard]] std::size_t midiToBinIndex(int midiNote) const noexcept;
    [[nodiscard]] double midiToFreqHz(double exactMidi) const noexcept;

private:
    struct BinKernel {
        double freqHz{0.0};
        double midiFloat{0.0};
        std::size_t windowLen{256};
        std::size_t decimation{1};
        float normScale{1.0f};
        std::vector<float> cosKernel;
        std::vector<float> sinKernel;
    };

    double sampleRate_{48000.0};
    double a4ReferenceHz_{440.0};
    int minMidiNote_{36};
    int maxMidiNote_{108};
    int binsPerSemitone_{2};
    std::size_t maxWindowSize_{4096};
    std::vector<BinKernel> bins_;
    mutable std::vector<float> mpmDecBuf_;
    mutable std::vector<float> mpmNsdfBuf_;
};

} // namespace gitara::dsp
