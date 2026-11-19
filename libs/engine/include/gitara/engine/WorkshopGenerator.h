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

/// Wzorzec sekwencyjny ćwiczenia w Warsztacie / Praktyce skal (wszystkie wzorce odblokowane!).
enum class ExercisePattern : std::uint8_t {
    Ascending                 = 0,  ///< Cała skala liniowo (1-2-3-4-5-6-7)
    Descending                = 1,  ///< Cała skala malejąco (kompatybilność API)
    UpAndDown                 = 2,  ///< Cała skala góra-dół (kompatybilność API)
    GroupsOf3                 = 3,  ///< Grupy po 3 dźwięki (1-2-3, 2-3-4, 3-4-5...)
    GroupsOf4                 = 4,  ///< Grupy po 4 dźwięki (1-2-3-4, 2-3-4-5, 3-4-5-6...)
    DiatonicThirds            = 5,  ///< Skoki tercjowe (1-3, 2-4, 3-5, 4-6...)
    FourthIntervals           = 6,  ///< Skoki kwartowe (1-4, 2-5, 3-6...)
    FifthIntervals            = 7,  ///< Skoki kwintowe (1-5, 2-6, 3-7...)
    SixthIntervals            = 8,  ///< Skoki sekstowe (1-6, 2-7...)
    OctaveJumps               = 9,  ///< Skoki oktawowe (1-8, 2-9...)
    GroupsOf6                 = 10, ///< Sekstole / grupy po 6 (1-2-3-4-5-6, 2-3-4-5-6-7...)
    BackAndForth              = 11, ///< Krok w tył (1-2-3-2, 2-3-4-3...)
    PaulGilbert4              = 12, ///< Zwrot shred 4-nutowy (1-2-3-1, 2-3-4-2...)
    BrokenThirdsZigZag        = 13, ///< Tercje naprzemienne zig-zag (1-3, 4-2, 3-5, 6-4...)
    DiatonicTriads            = 14, ///< Trójdźwięki po skali (1-3-5, 2-4-6, 3-5-7...)
    DiatonicTriadsAlternating = 15, ///< Trójdźwięki naprzemienne (1-3-5, 6-4-2, 3-5-7, 8-6-4...)
    DiatonicSevenths          = 16, ///< Czterodźwięki septymowe po skali (1-3-5-7, 2-4-6-8...)
    PedalPointRoot            = 17, ///< Nuta pedałowa na prymie (1-2-1-3-1-4-1-5...)
    StringSkipping            = 18  ///< Przeskok struny (skip-a-string)
};

/// Tryb prowadzenia przebiegu skali po gryfie.
enum class ScaleTraversalMode : std::uint8_t {
    Standard      = 0, ///< Wg wybranego positionSystem (Cały gryf / 3NPS / 4NPS / CAGED)
    DiagonalShift = 1, ///< Przebieg diagonalny w górę gryfu (płynne 2–3 oktawy przez wszystkie struny)
    SingleString  = 2  ///< Bieg poziomy po jednej wybranej strunie (0–24 próg)
};

struct WorkshopExerciseConfig {
    gitara::theory::Scale scale{};
    gitara::theory::PositionSystem positionSystem{gitara::theory::PositionSystem::TNPS};
    int positionIndex{0};
    int startStringIndex{0}; ///< -1 = Dowolna, 0..5 = konkretna struna startowa (E, A, D, G, B, E)
    ExerciseDirection direction{ExerciseDirection::Ascending};
    int octaves{2};          ///< 1, 2 lub 3 oktawy
    ExercisePattern pattern{ExercisePattern::Ascending};
    ScaleTraversalMode traversalMode{ScaleTraversalMode::Standard};
    bool startOnRoot{false}; ///< true = rozpocznij bieg od toniki (prymy) skali
    int minFret{0};          ///< Dolna granica progu (0..24)
    int maxFret{24};         ///< Górna granica progu (0..24)
    int tempoBpm{120};
    gitara::score::DurationValue noteDuration{gitara::score::DurationValue::Eighth};
    bool tuplet{false};      ///< true = triole (3 w czasie 2)
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
