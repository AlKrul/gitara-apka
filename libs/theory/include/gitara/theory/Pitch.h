#pragma once

#include "gitara/theory/Note.h"
#include <cmath>
#include <optional>
#include <string>
#include <string_view>

namespace gitara::theory {

/// Wysokość dźwięku z oktawą w notacji naukowej (C4 = środkowe C = MIDI 60, A4 = MIDI 69 = 440 Hz, E2 = MIDI 40).
struct Pitch {
    Note note{Letter::A, Accidental::Natural};
    int octave{4};

    constexpr Pitch() = default;
    constexpr Pitch(Note n, int oct) : note(n), octave(oct) {}

    /// Numer nuty MIDI (C4 = 60, E2 = 40, A4 = 69).
    /// Uwzględnia przekroczenie granicy oktawy dla znaków chromatycznych (np. Cb4 = MIDI 59, B#3 = MIDI 60).
    [[nodiscard]] constexpr int midiNumber() const noexcept {
        int rawSemitone = letterBaseSemitones(note.letter) + static_cast<int>(note.accidental);
        return (octave + 1) * 12 + rawSemitone;
    }

    /// Tworzy Pitch z numeru MIDI (0..127) z pisownią wg wybranego trybu znaków chromatycznych.
    [[nodiscard]] static Pitch fromMidiNumber(int midi,
                                              AccidentalMode mode = AccidentalMode::Auto) noexcept;

    /// Częstotliwość w Hz w stroju równomiernie temperowanym 12-TET dla zadanego A4 (domyślnie 440.0 Hz).
    [[nodiscard]] double frequencyHz(double a4ReferenceHz = 440.0) const noexcept {
        return a4ReferenceHz * std::pow(2.0, (midiNumber() - 69) / 12.0);
    }

    /// Formatuje do notacji naukowej, np. "E2", "F♯4", "B♭3".
    [[nodiscard]] std::string toString(bool useUnicode = true) const {
        return note.toString(useUnicode) + std::to_string(octave);
    }

    /// Parsuje zapis naukowy (np. "E2", "C#4", "Bb3").
    [[nodiscard]] static std::optional<Pitch> parse(std::string_view text) noexcept;

    constexpr bool operator==(const Pitch& other) const noexcept {
        return note == other.note && octave == other.octave;
    }
};

/// Wynik kwantyzacji częstotliwości (Hz) do najbliższej nuty i odchyłki w centach (-50..+50).
struct PitchDeviation {
    Pitch closestPitch{};
    int midiNote{69};
    double exactMidi{69.0};
    double cents{0.0}; ///< Odchyłka w centach względem closestPitch (-50.0 .. +50.0)
    double targetFrequencyHz{440.0};
};

/// Wyznacza najbliższą nutę i odchyłkę w centach dla zmierzonej częstotliwości (Hz).
[[nodiscard]] PitchDeviation analyzeFrequency(double frequencyHz,
                                              double a4ReferenceHz = 440.0,
                                              AccidentalMode mode = AccidentalMode::Auto) noexcept;

} // namespace gitara::theory
