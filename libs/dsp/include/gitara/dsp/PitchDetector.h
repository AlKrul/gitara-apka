#pragma once

#include "gitara/theory/Pitch.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace gitara::dsp {

enum class PitchAlgorithm : std::uint8_t {
    YIN                    = 0, ///< Algorytm YIN z interpolacją paraboliczną (referencyjna dokładność centowa)
    BitstreamAutocorrelation = 1 ///< Bitstream Autocorrelation w stylu cycfi/Q (bardzo szybki, odporny na alikwoty)
};

struct PitchResult {
    bool pitched{false};
    double frequencyHz{0.0};
    double confidence{0.0};
    gitara::theory::PitchDeviation deviation{};
};

/// Monofoniczny detektor wysokości dźwięku gitary (zakres od Drop C = 65 Hz / E2 = 82.4 Hz do 24. progu E6 = 1318.5 Hz).
/// Bufory robocze są prealokowane w prepare(), dzięki czemu detect() nie wykonuje żadnych alokacji w wątku audio.
class PitchDetector {
public:
    explicit PitchDetector(double sampleRate = 48000.0,
                           std::size_t windowSize = 2048,
                           double minFreqHz = 60.0,
                           double maxFreqHz = 1400.0);

    void prepare(double sampleRate,
                 std::size_t windowSize = 2048,
                 double minFreqHz = 60.0,
                 double maxFreqHz = 1400.0);

    /// Wykrywa częstotliwość podstawową (F0) z okna próbek. Zero alokacji pamięci.
    [[nodiscard]] PitchResult detect(
        const float* samples,
        std::size_t numSamples,
        PitchAlgorithm algorithm = PitchAlgorithm::YIN,
        double a4ReferenceHz = 440.0,
        gitara::theory::AccidentalMode accidentalMode = gitara::theory::AccidentalMode::Auto) noexcept;

private:
    [[nodiscard]] PitchResult detectYin(const float* samples,
                                        std::size_t n,
                                        double a4ReferenceHz,
                                        gitara::theory::AccidentalMode mode) noexcept;

    [[nodiscard]] PitchResult detectBitstream(const float* samples,
                                              std::size_t n,
                                              double a4ReferenceHz,
                                              gitara::theory::AccidentalMode mode) noexcept;

    double sampleRate_{48000.0};
    std::size_t maxWindowSize_{2048};
    std::size_t minTau_{34};
    std::size_t maxTau_{800};
    std::vector<double> yinBuffer_;
    std::vector<std::uint64_t> bitWords_;
};

} // namespace gitara::dsp
