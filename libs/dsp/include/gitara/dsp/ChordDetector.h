#pragma once

#include "gitara/theory/Chord.h"
#include <array>
#include <cstddef>

namespace gitara::dsp {

struct ChordDetectionResult {
    bool detected{false};
    gitara::theory::Chord chord{};
    double confidence{0.0}; ///< Podobieństwo kosinusowe (0.0 .. 1.0)
    std::array<float, 12> chromagram{};
};

/// Polifoniczny detektor akordów oparty o 12-klasowy profil wysokości (Chromagram / HPCP)
/// z kompensacją alikwotów i dopasowaniem kosinusowym do bazy akordów z libs/theory.
/// Obsługuje zarówno szybką weryfikację „czy zagrano oczekiwany akord X?”, jak i rozpoznawanie swobodne.
class ChordDetector {
public:
    explicit ChordDetector(double sampleRate = 48000.0);

    void prepare(double sampleRate) noexcept;

    /// Wylicza 12-elementowy znormalizowany chromagram (C=0 .. B=11) z bufora audio.
    [[nodiscard]] std::array<float, 12> computeChromagram(
        const float* samples,
        std::size_t numSamples,
        double a4ReferenceHz = 440.0) const noexcept;

    /// Sprawdza stopień dopasowania sygnału audio do konkretnego oczekiwanego akordu (0.0 .. 1.0).
    [[nodiscard]] double verifyExpectedChord(
        const float* samples,
        std::size_t numSamples,
        const gitara::theory::Chord& expectedChord,
        double a4ReferenceHz = 440.0) const noexcept;

    /// Rozpoznaje najlepiej pasujący akord z bazy (trójdźwięki i septymowe).
    [[nodiscard]] ChordDetectionResult detectChord(
        const float* samples,
        std::size_t numSamples,
        bool includeSeventhChords = true,
        double a4ReferenceHz = 440.0) const noexcept;

private:
    double sampleRate_{48000.0};
};

} // namespace gitara::dsp
