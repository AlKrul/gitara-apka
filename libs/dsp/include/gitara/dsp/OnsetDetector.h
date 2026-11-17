#pragma once

#include <cstddef>

namespace gitara::dsp {

/// Detektor ataku kostki / szarpnięcia struny (Onset Detector) oparty o skok obwiedni energii
/// oraz zawartości wysokich częstotliwości (High-Frequency Content).
/// Używany do oceny dokładności rytmicznej oraz automatycznego tworzenia tabulatury.
class OnsetDetector {
public:
    explicit OnsetDetector(double sampleRate = 48000.0,
                           double minInterOnsetSeconds = 0.055);

    void prepare(double sampleRate, double minInterOnsetSeconds = 0.055) noexcept;
    void reset() noexcept;

    /// Zwraca true, jeśli w podanym bloku próbek wystąpił atak nowej nuty/akordu.
    [[nodiscard]] bool processBlock(const float* samples, std::size_t numSamples) noexcept;

private:
    double sampleRate_{48000.0};
    std::size_t minCooldownSamples_{2640};
    std::size_t samplesSinceLastOnset_{100000};
    float previousEnergy_{0.0f};
    float previousHfc_{0.0f};
};

} // namespace gitara::dsp
