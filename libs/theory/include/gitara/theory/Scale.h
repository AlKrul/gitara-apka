#pragma once

#include "gitara/theory/Interval.h"
#include "gitara/theory/Note.h"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gitara::theory {

/// Definicja skali muzycznej w oparciu o listę interwałów od toniki.
struct ScaleDefinition {
    std::string id;                  ///< Unikalny identyfikator (np. "major", "dorian", "lydian_dominant")
    std::string polishName;          ///< Nazwa wyświetlana w polskim UI (np. "Durowa", "Lidyjska dominantowa")
    std::string category;            ///< Kategoria (np. "Podstawowe", "Modalne", "Pentatoniki i Blues", "Jazzowe", "Symetryczne", "Egzotyczne")
    std::vector<Interval> intervals; ///< Interwały od prymy (włącznie z P1)
    std::string source;              ///< Źródło teoretyczne (np. "tonal / Levine")
};

/// Skala osadzona od konkretnej toniki (np. C Durowa, G♯ Harmoniczna molowa).
struct Scale {
    Note root{};
    const ScaleDefinition* definition{nullptr};

    /// Zwraca wszystkie dźwięki skali z poprawną pisownią enharmoniczną (lub wg wybranego AccidentalMode).
    [[nodiscard]] std::vector<Note> notes(AccidentalMode mode = AccidentalMode::Auto) const;

    /// Sprawdza, czy dana klasa wysokości (0..11) należy do skali.
    [[nodiscard]] bool containsPitchClass(int pitchClass) const noexcept;

    /// Zwraca interwał skali dla danej klasy wysokości (lub std::nullopt jeśli dźwięk spoza skali).
    [[nodiscard]] std::optional<Interval> intervalForPitchClass(int pitchClass) const noexcept;

    /// Formatuje pełną nazwę skali w konwencji „C Durowa”, „F♯ Molowa naturalna”.
    [[nodiscard]] std::string fullName(bool useUnicode = true) const;
};

/// Zwraca pełny katalog zdefiniowanych skal.
[[nodiscard]] const std::vector<ScaleDefinition>& allScaleDefinitions();

/// Wyszukuje definicję skali po identyfikatorze (np. "major", "natural_minor", "mixolydian").
[[nodiscard]] const ScaleDefinition* findScaleDefinition(std::string_view id) noexcept;

} // namespace gitara::theory
