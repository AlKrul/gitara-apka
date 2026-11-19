#pragma once

#include "gitara/engine/PracticeEngine.h"
#include "gitara/score/Score.h"
#include "gitara/theory/Arpeggio.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"
#include <vector>

namespace gitara::engine {

/// Kierunek ćwiczenia w Warsztacie (zgodnie ze zrzutem „Warsztat -> Dodaj ćwiczenie”).
enum class ExerciseDirection : std::uint8_t {
    Ascending  = 0, ///< Rosnąco
    Descending = 1, ///< Malejąco
    UpAndDown  = 2, ///< Rosnąco -> Malejąco
    DownAndUp  = 3  ///< Malejąco -> Rosnąco
};

/// Wzorzec sekwencyjny ćwiczenia w Warsztacie (wszystkie wzorce odblokowane bez Premium!).
enum class ExercisePattern : std::uint8_t {
    Ascending      = 0, ///< Cała skala (po kolei)
    Descending     = 1, ///< Cała skala malejąco (zachowane dla kompatybilności API)
    UpAndDown      = 2, ///< Cała skala góra-dół
    GroupsOf3      = 3, ///< Grupy po 3 dźwięki (1-2-3, 2-3-4, 3-4-5...)
    GroupsOf4      = 4, ///< Grupy po 4 dźwięki (1-2-3-4, 2-3-4-5, 3-4-5-6...)
    DiatonicThirds = 5, ///< Skoki tercjowe (1-3, 2-4, 3-5, 4-6...)
    FourthIntervals = 6,///< Skoki kwartowe (1-4, 2-5, 3-6...)
    FifthIntervals = 7, ///< Skoki kwintowe (1-5, 2-6, 3-7...)
    SixthIntervals = 8, ///< Skoki sekstowe (1-6, 2-7...)
    OctaveJumps    = 9  ///< Skoki oktawowe (1-8, 2-9...)
};

struct WorkshopExerciseConfig {
    gitara::theory::Scale scale{};
    gitara::theory::PositionSystem positionSystem{gitara::theory::PositionSystem::TNPS};
    int positionIndex{0};
    int startStringIndex{0}; ///< -1 = Dowolna, 0..5 = konkretna struna startowa (E, A, D, G, B, E)
    ExerciseDirection direction{ExerciseDirection::Ascending};
    int octaves{2};          ///< 1 oktawa (8 dźwięków) lub 2 oktawy (15+ dźwięków)
    ExercisePattern pattern{ExercisePattern::Ascending};
    int tempoBpm{120};
    gitara::score::DurationValue noteDuration{gitara::score::DurationValue::Eighth};
};

struct GeneratedExercise {
    std::vector<ExerciseStep> steps; ///< Kroki dla modułu weryfikacji na żywo (ExerciseVerifier)
    gitara::score::Song song;        ///< Gotowa tabulatura do podglądu w edytorze i eksportu (.gp5 / .mid)
};

/// Kreator ćwiczeń („Warsztat”): generuje sekwencję kroków na gryfie oraz tabulaturę
/// z uwzględnieniem wybranej skali, pozycji (3NPS / 4NPS / CAGED), struny startowej,
/// wzorca sekwencyjnego oraz maski aktywnego obszaru gryfu (FretboardMask).
[[nodiscard]] GeneratedExercise generateScaleExercise(
    const gitara::theory::Fretboard& fretboard,
    const WorkshopExerciseConfig& config,
    const gitara::theory::FretboardMask* mask = nullptr);

} // namespace gitara::engine
