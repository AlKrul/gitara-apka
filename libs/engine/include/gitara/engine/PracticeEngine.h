#pragma once

#include "gitara/dsp/ChordDetector.h"
#include "gitara/dsp/KarplusStrongSynth.h"
#include "gitara/dsp/OnsetDetector.h"
#include "gitara/dsp/PitchDetector.h"
#include "gitara/dsp/SignalConditioner.h"
#include "gitara/score/Score.h"
#include "gitara/theory/Fretboard.h"
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace gitara::engine {

/// Jednokierunkowa kolejka bezblokadowa (Lock-Free Single-Producer Single-Consumer)
/// do przesyłania zdarzeń detekcji z wątku audio czasu rzeczywistego do wątku UI.
template <typename T, std::size_t Capacity>
class LockFreeSpscQueue {
public:
    bool push(const T& item) noexcept {
        const std::size_t w = writeIndex_.load(std::memory_order_relaxed);
        const std::size_t nextW = (w + 1) % Capacity;
        if (nextW == readIndex_.load(std::memory_order_acquire)) {
            return false; // Kolejka pełna
        }
        buffer_[w] = item;
        writeIndex_.store(nextW, std::memory_order_release);
        return true;
    }

    bool pop(T& outItem) noexcept {
        const std::size_t r = readIndex_.load(std::memory_order_relaxed);
        if (r == writeIndex_.load(std::memory_order_acquire)) {
            return false; // Kolejka pusta
        }
        outItem = buffer_[r];
        readIndex_.store((r + 1) % Capacity, std::memory_order_release);
        return true;
    }

private:
    std::array<T, Capacity> buffer_{};
    std::atomic<std::size_t> writeIndex_{0};
    std::atomic<std::size_t> readIndex_{0};
};

/// Sample-accurate metronom z obsługą dowolnego metrum (4/4, 3/4, 6/8, 5/4, 7/8...) i podziałów (ćwierćnuty, ósemki, triole, szesnastki).
class Metronome {
public:
    void prepare(double sampleRate) noexcept;
    void reset() noexcept;

    void setEnabled(bool enabled) noexcept { enabled_ = enabled; }
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_; }

    void setTempoBpm(double bpm) noexcept;
    [[nodiscard]] double tempoBpm() const noexcept { return tempoBpm_; }

    void setTimeSignature(int numerator, int denominator) noexcept;
    void setSubdivision(int clicksPerBeat) noexcept;

    /// Renderuje klik metronomu z dokładnością co do próbki i dodaje do bufora wyjściowego.
    void renderNextBlock(float* outputBuffer, std::size_t numSamples) noexcept;

private:
    double sampleRate_{48000.0};
    double tempoBpm_{120.0};
    int numerator_{4};
    int denominator_{4};
    int subdivision_{1};
    bool enabled_{false};

    std::uint64_t sampleCounter_{0};
    std::uint64_t nextSubClickSample_{0};
    int currentBeatInBar_{0};
    int currentSubInBeat_{0};

    std::size_t clickSamplesRemaining_{0};
    double clickPhase_{0.0};
    double clickFreqHz_{1000.0};
    float clickGain_{0.25f};
};

/// Pojedynczy krok ćwiczenia w Warsztacie (oczekiwana nuta na gryfie lub oczekiwany akord).
struct ExerciseStep {
    bool isChord{false};
    gitara::theory::FretPosition targetPosition{}; ///< Dla pojedynczej nuty: dokładna struna i próg
    int expectedMidiNote{60};
    gitara::theory::Chord expectedChord{};
    double expectedTimeSeconds{0.0};
};

/// Ocena pojedynczego kroku ćwiczenia po zagraniu przez użytkownika.
struct StepEvaluation {
    std::size_t stepIndex{0};
    bool pitchCorrect{false};
    double intonationErrorCents{0.0}; ///< Odchyłka w centach względem docelowej nuty
    double timingErrorMs{0.0};        ///< Odchyłka w milisekundach względem siatki rytmicznej
};

/// Maszyna stanów weryfikacji ćwiczenia na żywo („Gra na żywo” w Warsztacie / Edytorze ćwiczeń).
/// Jak ustalono w specyfikacji: w ćwiczeniu kolejność i pozycje są znane z góry, więc wystarczy
/// sprawdzić, czy wysokość dźwięku (lub akord) się zgadza z tolerancją w centach.
class ExerciseVerifier {
public:
    void setSteps(std::vector<ExerciseStep> steps, double centsTolerance = 35.0);
    void reset() noexcept;

    [[nodiscard]] bool isFinished() const noexcept {
        return currentStepIndex_ >= steps_.size();
    }
    [[nodiscard]] std::size_t currentStepIndex() const noexcept {
        return currentStepIndex_;
    }
    [[nodiscard]] const std::vector<StepEvaluation>& evaluations() const noexcept {
        return evaluations_;
    }

    /// Przetwarza wykrytą nutę monofoniczną i jeśli zgadza się z aktualnym krokiem, zalicza go (zielony) i przechodzi dalej.
    bool onMonophonicDetection(const gitara::dsp::PitchResult& pitch, double currentTimeSeconds);

    /// Przetwarza wynik weryfikacji akordu i jeśli przekracza próg pewności, zalicza krok i przechodzi dalej.
    bool onChordConfidence(double chordConfidence, double currentTimeSeconds);

    /// Wylicza łączną procentową dokładność (0.0 .. 100.0%).
    [[nodiscard]] double overallAccuracyPercent() const noexcept;

private:
    std::vector<ExerciseStep> steps_;
    std::vector<StepEvaluation> evaluations_;
    std::size_t currentStepIndex_{0};
    double centsTolerance_{35.0};
};

} // namespace gitara::engine
