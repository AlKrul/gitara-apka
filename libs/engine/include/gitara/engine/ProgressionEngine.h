#pragma once

#include "gitara/score/Score.h"
#include "gitara/theory/Chord.h"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gitara::engine {

/// Poziom natężenia swingu (z ekranu „Blues 12-taktowy -> Natężenie swingu”).
enum class SwingLevel : std::uint8_t {
    Straight = 0, ///< Równo (50% : 50%)
    Light    = 1, ///< Lekki (58% : 42%)
    Medium   = 2, ///< Średni (66.7% : 33.3% – klasyczny triolowy)
    Heavy    = 3  ///< Mocny (73% : 27%)
};

enum class GrooveStyle : std::uint8_t {
    Straight = 0,
    Shuffle  = 1,
    Slow12_8 = 2
};

/// Pojedyncze gniazdo akordu w takcie (np. A7 / I7 na 1. mierze taktu).
struct ChordSlot {
    int beatIndex{0};            ///< Miara w takcie (0..3)
    gitara::theory::Interval rootOffsetFromKey{gitara::theory::intervals::P1};
    std::string chordDefId{"7"}; ///< Rodzaj akordu z bazy (np. "7", "maj7", "m7", "m7b5")
    std::string romanNumeral{"I7"};

    /// Buduje konkretny akord dla zadanej tonacji głównej.
    [[nodiscard]] gitara::theory::Chord resolveInKey(
        gitara::theory::Note keyRoot,
        gitara::theory::AccidentalMode mode = gitara::theory::AccidentalMode::Auto) const;
};

struct ProgressionMeasure {
    int voltaNumber{0}; ///< 0 = zwykły takt, 1 = 1. wolta, 2 = 2. wolta
    std::vector<ChordSlot> chords;
};

struct ProgressionSection {
    std::string name{"A"};
    int repeatCount{1};
    std::vector<ProgressionMeasure> measures;
};

/// Pełny zapis progresji akordowej („Zmiany” / „Biblioteka”).
struct ChordProgression {
    std::string id;
    std::string title;
    std::string styleCategory; ///< "Jazz", "Blues", "Pop"
    gitara::theory::Note keyRoot{gitara::theory::Letter::C, gitara::theory::Accidental::Natural};
    bool isMinorKey{false};
    int tempoBpm{108};
    int numerator{4};
    int denominator{4};
    SwingLevel swing{SwingLevel::Straight};
    GrooveStyle groove{GrooveStyle::Straight};
    bool rhythmGuitarEnabled{true};
    bool backingBandEnabled{true}; ///< Zespół akompaniujący (kroczący bas + perkusja) – odblokowane bez Premium!
    bool loopEnabled{true};
    bool cycleThroughKeys{false};  ///< „Przechodź przez tonacje”
    std::optional<int> loopStartMeasure{};
    std::optional<int> loopEndMeasure{};
    std::vector<ProgressionSection> sections;
};

/// Zwraca bibliotekę wbudowanych standardów („Gotowe do grania”: Blues 12-taktowy, Jazz blues,
/// ii–V–I, Molowe ii–V–i, I–V–vi–IV, vi–IV–I–V, vi–IV–I–iii, I–vi–IV–V, Progresja z Kanonu, Kadencja andaluzyjska).
[[nodiscard]] const std::vector<ChordProgression>& builtinProgressions();

/// Wyszukuje standard po identyfikatorze.
[[nodiscard]] std::optional<ChordProgression> findBuiltinProgression(std::string_view id);

/// Transponuje progresję do nowej tonacji głównej (z zachowaniem poprawnej pisowni A–G).
[[nodiscard]] ChordProgression transposeProgression(
    ChordProgression prog,
    gitara::theory::Note newKeyRoot);

/// Funkcja „Reharmonizuj” (w referencji zablokowana jako Premium):
/// stosuje podstawienia trytonowe (♭II7 w miejsce V7) oraz wtrącenia ii–V.
[[nodiscard]] ChordProgression reharmonizeProgression(const ChordProgression& prog);

/// Rozwija progresję na wszystkie 12 tonacji po kole kwartowo-kwintowym („we wszystkich 12 tonacjach”).
[[nodiscard]] ChordProgression expandThroughAll12Keys(const ChordProgression& prog);

/// Generuje wielościeżkowy utwór akompaniamentu (Ścieżka 1: Gitara rytmiczna / Organy, Ścieżka 2: Kroczący bas)
/// gotowy do odtwarzania w pętli oraz eksportu do MIDI / GP5.
[[nodiscard]] gitara::score::Song generateBackingTrackSong(
    const ChordProgression& prog,
    gitara::theory::AccidentalMode mode = gitara::theory::AccidentalMode::Auto);

} // namespace gitara::engine
