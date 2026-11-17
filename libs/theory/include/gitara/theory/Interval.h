#pragma once

#include "gitara/theory/Note.h"
#include "gitara/theory/Pitch.h"
#include <cstdint>
#include <string_view>
#include <vector>

namespace gitara::theory {

/// Interwał muzyczny zdefiniowany przez stopień diatoniczny (1 = pryma, 2 = sekunda, ..., 13 = tercdecyma)
/// oraz rozmiar w półtonach (0..21). Pozwala na bezbłędną pisownię enharmoniczną (np. ♯2 vs ♭3, ♯4 vs ♭5, 𝄫7 vs 6).
struct Interval {
    int degree{1};    ///< Stopień diatoniczny (1-indeksowany: 1=pryma, 2=sekunda, 3=tercja, ..., 8=oktawa, 9=nona...)
    int semitones{0}; ///< Odległość w półtonach

    constexpr Interval() = default;
    constexpr Interval(int deg, int semi) : degree(deg), semitones(semi) {}

    /// Dodaje interwał do dźwięku z zachowaniem poprawnej litery stopnia diatonicznego!
    /// Przykład: C + (stopień 3, półtony 3 -> ♭3) = E♭; C + (stopień 2, półtony 3 -> ♯2) = D♯; G♯ + (7, 11) = F𝄪.
    [[nodiscard]] Note applyTo(Note root) const noexcept;

    /// Dodaje interwał do Pitch (z uwzględnieniem oktawy).
    [[nodiscard]] Pitch applyTo(Pitch root) const noexcept;

    /// Zwraca symbol stopniowy interwału (np. "1", "♭2", "2", "♯2", "♭3", "3", "4", "♯4", "♭5", "5", "♭6", "6", "♭7", "7").
    [[nodiscard]] std::string symbol(bool useUnicode = true) const;

    /// Zwraca skrót międzynarodowy (np. "P1", "m2", "M2", "m3", "M3", "P4", "TT", "P5", "m6", "M6", "m7", "M7", "P8").
    [[nodiscard]] std::string_view shortCode() const noexcept;

    /// Zwraca polską nazwę interwału (np. "Pryma czysta", "Tercja mała", "Kwinta czysta").
    [[nodiscard]] std::string_view polishName() const noexcept;

    /// Zwraca podpowiedź piosenki-skojarzenia do modułu kształcenia słuchu.
    [[nodiscard]] std::string_view songHintAscending() const noexcept;

    constexpr bool operator==(const Interval& other) const noexcept {
        return degree == other.degree && semitones == other.semitones;
    }
};

namespace intervals {
    inline constexpr Interval P1  {1,  0}; // Pryma czysta (1)
    inline constexpr Interval m2  {2,  1}; // Sekunda mała (♭2)
    inline constexpr Interval M2  {2,  2}; // Sekunda wielka (2)
    inline constexpr Interval A2  {2,  3}; // Sekunda zwiększona (♯2)
    inline constexpr Interval m3  {3,  3}; // Tercja mała (♭3)
    inline constexpr Interval M3  {3,  4}; // Tercja wielka (3)
    inline constexpr Interval d4  {4,  4}; // Kwarta zmniejszona (♭4)
    inline constexpr Interval P4  {4,  5}; // Kwarta czysta (4)
    inline constexpr Interval A4  {4,  6}; // Kwarta zwiększona (♯4)
    inline constexpr Interval d5  {5,  6}; // Kwinta zmniejszona (♭5)
    inline constexpr Interval P5  {5,  7}; // Kwinta czysta (5)
    inline constexpr Interval A5  {5,  8}; // Kwinta zwiększona (♯5)
    inline constexpr Interval m6  {6,  8}; // Seksta mała (♭6)
    inline constexpr Interval M6  {6,  9}; // Seksta wielka (6)
    inline constexpr Interval d7  {7,  9}; // Septyma zmniejszona (𝄫7)
    inline constexpr Interval m7  {7, 10}; // Septyma mała (♭7)
    inline constexpr Interval M7  {7, 11}; // Septyma wielka (7)
    inline constexpr Interval P8  {8, 12}; // Oktawa czysta (8)
    inline constexpr Interval m9  {9, 13}; // Nona mała (♭9)
    inline constexpr Interval M9  {9, 14}; // Nona wielka (9)
    inline constexpr Interval A9  {9, 15}; // Nona zwiększona (♯9)
    inline constexpr Interval P11 {11, 17}; // Undecyma czysta (11)
    inline constexpr Interval A11 {11, 18}; // Undecyma zwiększona (♯11)
    inline constexpr Interval m13 {13, 20}; // Tercdecyma mała (♭13)
    inline constexpr Interval M13 {13, 21}; // Tercdecyma wielka (13)
} // namespace intervals

/// Zwraca listę 13 podstawowych interwałów (P1..P8) używanych w module Kształcenia Słuchu.
[[nodiscard]] const std::vector<Interval>& earTrainingIntervals();

/// Wyznacza prosty interwał (0..12 półtonów) na podstawie różnicy półtonów.
[[nodiscard]] Interval intervalFromSemitones(int semitones) noexcept;

} // namespace gitara::theory
