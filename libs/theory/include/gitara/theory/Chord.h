#pragma once

#include "gitara/theory/Interval.h"
#include "gitara/theory/Note.h"
#include "gitara/theory/Scale.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gitara::theory {

/// Definicja rodzaju akordu (trójdźwięki, septymowe, rozszerzone, alterowane).
struct ChordDefinition {
    std::string id;                  ///< Identyfikator (np. "major", "m7", "maj7", "dim7", "7b9")
    std::string symbolSuffix;        ///< Przyrostek symbolu międzynarodowego (np. "", "m", "maj7", "m7♭5")
    std::string polishName;          ///< Polska nazwa (np. "Durowy", "Molowy septymowy", "Półzmniejszony")
    std::string category;            ///< Kategoria ("Trójdźwięki", "Septymowe", "Rozszerzone", "Alterowane")
    std::vector<Interval> intervals; ///< Interwały od prymy
    std::vector<int> optionalIndices;///< Indeksy składników, które można pominąć w szerokich voicingach (np. czysta kwinta P5)
};

/// Akord osadzony od konkretnej prymy (z opcjonalnym dźwiękiem w basie dla przewrotów / akordów ukośnikowych).
struct Chord {
    Note root{};
    const ChordDefinition* definition{nullptr};
    std::optional<Note> bassNote{};

    /// Zwraca dźwięki akordu z poprawną pisownią interwałową (np. Cdim7 -> C, E♭, G♭, B𝄫).
    [[nodiscard]] std::vector<Note> notes(AccidentalMode mode = AccidentalMode::Auto) const;

    /// Zwraca międzynarodowy symbol akordu w systemie A–G (np. "Cmaj", "Cmaj7", "Bm7♭5", "C/E").
    [[nodiscard]] std::string symbol(bool useUnicode = true,
                                     AccidentalMode mode = AccidentalMode::Auto) const;

    /// Sprawdza, czy podana klasa wysokości należy do składników akordu.
    [[nodiscard]] bool containsPitchClass(int pitchClass) const noexcept;
};

/// Wynik dopasowania skali do akordu (zakładka „Szukaj: Akord -> skale”).
struct ChordScaleMatch {
    Scale scale;
    std::vector<Note> chordTones; ///< Dźwięki akordu obecne w skali
    std::vector<Note> tensions;   ///< Pozostałe dźwięki skali (napięcia), poprawnie zapisane wg stopni skali!
    std::vector<Note> avoidNotes; ///< Dźwięki unikane (leżące o 1 półton powyżej składnika akordu, np. F w C dur nad Cmaj)
};

/// Zwraca pełną bazę definicji akordów (obsługuje przełącznik „Wszystkie typy”).
[[nodiscard]] const std::vector<ChordDefinition>& allChordDefinitions();

/// Wyszukuje definicję akordu po identyfikatorze (np. "major", "minor", "maj7", "7", "m7", "m7b5", "dim7").
[[nodiscard]] const ChordDefinition* findChordDefinition(std::string_view id) noexcept;

/// Zakładka „Szukaj (Akord -> skale)”: znajduje wszystkie skale zawierające dźwięki akordu
/// oraz wylicza ich napięcia (z poprawną pisownią A–G!) i dźwięki unikane.
[[nodiscard]] std::vector<ChordScaleMatch> findScalesForChord(
    const Chord& chord,
    AccidentalMode mode = AccidentalMode::Auto);

/// Zakładka „Szukaj (Skala -> akordy)”: znajduje akordy zawierające się w dźwiękach skali.
[[nodiscard]] std::vector<Chord> findChordsForScale(
    const Scale& scale,
    bool allChordTypes = true,
    AccidentalMode mode = AccidentalMode::Auto);

} // namespace gitara::theory
