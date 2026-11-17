#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace gitara::theory {

/// Litera diatoniczna w międzynarodowym systemie anglosaskim A–G (B = dawne polskie H).
enum class Letter : std::uint8_t {
    C = 0,
    D = 1,
    E = 2,
    F = 3,
    G = 4,
    A = 5,
    B = 6
};

/// Znak chromatyczny (w półtonach względem dźwięku naturalnego).
enum class Accidental : std::int8_t {
    DoubleFlat  = -2, // 𝄫 / bb
    Flat        = -1, // ♭ / b
    Natural     =  0, // ♮ / brak
    Sharp       = +1, // ♯ / #
    DoubleSharp = +2  // 𝄪 / x
};

/// Tryb wyświetlania znaków chromatycznych (w aplikacji referencyjnej nazwany błędnie „Ostrość: Łagodne / Ostre”).
enum class AccidentalMode : std::uint8_t {
    Auto   = 0, ///< Pisownia ściśle wg teorii muzyki (stopnie skali / interwały / koło kwintowe)
    Flats  = 1, ///< Wymuszenie bemoli (♭: Db, Eb, Gb, Ab, Bb) – w referencji „Łagodne”
    Sharps = 2  ///< Wymuszenie krzyżyków (♯: C#, D#, F#, G#, A#) – w referencji „Ostre”
};

/// Dźwięk z jawną pisownią diatoniczną (litera A–G + znak chromatyczny).
/// Rozróżnia enharmonicznie np. C# i Db oraz F𝄪 i G.
struct Note {
    Letter letter{Letter::C};
    Accidental accidental{Accidental::Natural};

    constexpr Note() = default;
    constexpr Note(Letter l, Accidental a = Accidental::Natural)
        : letter(l), accidental(a) {}

    /// Klasa wysokości dźwięku (0..11, gdzie C=0, C#/Db=1, ..., B=11).
    [[nodiscard]] constexpr int pitchClass() const noexcept {
        constexpr int baseSemitones[7] = {0, 2, 4, 5, 7, 9, 11};
        int pc = (baseSemitones[static_cast<int>(letter)] + static_cast<int>(accidental)) % 12;
        return pc < 0 ? pc + 12 : pc;
    }

    /// Zwraca dźwięk z ewentualną konwersją enharmoniczną dla trybu Flats / Sharps.
    /// W trybie Auto zachowuje oryginalną pisownię teoretyczną.
    [[nodiscard]] Note withAccidentalMode(AccidentalMode mode) const noexcept;

    /// Tworzy dźwięk z klasy wysokości (0..11) zgodnie z wybranym trybem znaków chromatycznych.
    /// Dla AccidentalMode::Auto używa konwencji koła kwintowego (C, Db, D, Eb, E, F, F#, G, Ab, A, Bb, B).
    [[nodiscard]] static Note fromPitchClass(int pitchClass,
                                             AccidentalMode mode = AccidentalMode::Auto) noexcept;

    /// Formatuje nazwę dźwięku (np. "C", "F♯", "B♭", "F𝄪" lub w ASCII "F#", "Bb", "Fx").
    [[nodiscard]] std::string toString(bool useUnicode = true) const;

    /// Parsuje zapis tekstowy dźwięku w systemie A–G (np. "C", "Eb", "E♭", "F#", "F♯", "Fx", "F𝄪", "Bb").
    /// Zwraca std::nullopt dla niepoprawnego napisu (w tym dla odrzuconego symbolu 'H').
    [[nodiscard]] static std::optional<Note> parse(std::string_view text) noexcept;

    constexpr bool operator==(const Note& other) const noexcept {
        return letter == other.letter && accidental == other.accidental;
    }
    constexpr bool operator!=(const Note& other) const noexcept {
        return !(*this == other);
    }

    /// Sprawdza równoważność enharmoniczną (np. C# == Db).
    [[nodiscard]] constexpr bool isEnharmonicWith(const Note& other) const noexcept {
        return pitchClass() == other.pitchClass();
    }
};

/// Zwraca bazową wysokość półtonową litery C..B (0, 2, 4, 5, 7, 9, 11).
[[nodiscard]] constexpr int letterBaseSemitones(Letter l) noexcept {
    constexpr int table[7] = {0, 2, 4, 5, 7, 9, 11};
    return table[static_cast<int>(l)];
}

/// Zwraca literę przesuniętą o zadaną liczbę stopni diatonicznych (np. C + 2 -> E).
[[nodiscard]] constexpr Letter advanceLetter(Letter l, int steps) noexcept {
    int idx = (static_cast<int>(l) + steps) % 7;
    if (idx < 0) idx += 7;
    return static_cast<Letter>(idx);
}

/// Wylicza poprawną pisownię dźwięku na zadanym stopniu diatonicznym i w zadanej klasie wysokości.
[[nodiscard]] Note spellAtLetter(Letter targetLetter, int targetPitchClass) noexcept;

} // namespace gitara::theory
