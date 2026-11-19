#pragma once

#include "gitara/theory/Chord.h"
#include "gitara/theory/Interval.h"
#include "gitara/theory/Pitch.h"
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace gitara::engine {

/// Tryb ćwiczenia w Kształceniu Słuchu (zgodnie ze screenshotem „Kształcenie słuchu -> Interwały / Akordy”).
enum class EarTrainingMode : std::uint8_t {
    Learn    = 0, ///< Nauka (odsłuchiwanie interwałów/akordów i ich piosenek-skojarzeń)
    Identify = 1, ///< Rozpoznaj (wybór nazwy usłyszanego interwału/akordu)
    PlayBack = 2, ///< Zagraj (odpowiedź na gitarze przez detekcję wysokości dźwięku!)
    Chords   = 3  ///< Akordy (rozpoznawanie barwy akordu: dur, moll, zmn., zw., maj7, 7, m7...)
};

enum class EarPlaybackStyle : std::uint8_t {
    Melodic  = 0, ///< Melodycznie (dźwięki po kolei)
    Harmonic = 1  ///< Harmonicznie (dźwięki jednocześnie)
};

enum class EarDirection : std::uint8_t {
    Ascending  = 0, ///< Rosnąco
    Descending = 1, ///< Malejąco
    Random     = 2  ///< Losowo
};

enum class RootNoteMode : std::uint8_t {
    Random      = 0, ///< Losowa nuta i oktawa (w zakresie gitary)
    FixedNote   = 1, ///< Stała klasa wysokości (np. C), losowa oktawa
    FixedPitch  = 2  ///< Stała nuta i oktawa (np. C3 = MIDI 48)
};

struct EarTrainingConfig {
    EarTrainingMode mode{EarTrainingMode::Identify};
    EarPlaybackStyle playbackStyle{EarPlaybackStyle::Melodic};
    EarDirection direction{EarDirection::Ascending};
    RootNoteMode rootMode{RootNoteMode::Random};
    gitara::theory::Pitch fixedRoot{
        gitara::theory::Note{gitara::theory::Letter::C, gitara::theory::Accidental::Natural}, 3
    };
    bool endlessSession{false}; ///< false = Ustalony (np. 10, 20, 50 rund), true = Bez końca (odblokowane bez Premium!)
    int targetRounds{10};
    bool measureReactionTime{true};
    bool showSongHints{true};
    std::vector<gitara::theory::Interval> activeIntervals = gitara::theory::earTrainingIntervals();
};

struct EarQuestion {
    int roundNumber{1};
    gitara::theory::Pitch rootPitch{};
    gitara::theory::Pitch secondPitch{};
    gitara::theory::Interval interval{};
    bool isAscending{true};
    std::string songHint;
};

struct IntervalStats {
    int attempts{0};
    int correct{0};
    double totalReactionTimeMs{0.0};

    [[nodiscard]] double accuracyPercent() const noexcept {
        return attempts > 0 ? (100.0 * static_cast<double>(correct) / static_cast<double>(attempts)) : 0.0;
    }
};

/// Silnik Kształcenia Słuchu obsługujący quizy interwałowe i akordowe, odpowiedzi kliknięciem
/// lub zagraniem na gitarze, pomiar czasu reakcji oraz funkcję „Zaskocz mnie” (adaptacyjny wybór najsłabszych interwałów).
class EarTrainer {
public:
    explicit EarTrainer(EarTrainingConfig config = {});

    void setConfig(EarTrainingConfig config);
    [[nodiscard]] const EarTrainingConfig& config() const noexcept { return config_; }

    /// Konfiguruje losową sesję („Zaskocz mnie”), wybierając 5 interwałów, w których użytkownik ma najniższą skuteczność.
    void setupSurpriseMeSession(int rounds = 10);

    /// Generuje kolejne pytanie w sesji.
    [[nodiscard]] EarQuestion nextQuestion();

    /// Weryfikuje odpowiedź wybraną przyciskiem w UI (tryb „Rozpoznaj”).
    bool submitIntervalAnswer(int guessedSemitones, double reactionTimeMs = 0.0);

    /// Weryfikuje odpowiedź zagraną na gitarze (tryb „Zagraj” – sprawdza wykryty numer MIDI drugiej nuty).
    bool submitPlayedGuitarMidi(int detectedMidiNote, double reactionTimeMs = 0.0);

    [[nodiscard]] bool isSessionFinished() const noexcept;
    [[nodiscard]] int completedRounds() const noexcept { return completedRounds_; }
    [[nodiscard]] int correctAnswers() const noexcept { return correctAnswers_; }
    [[nodiscard]] double sessionAccuracyPercent() const noexcept;
    [[nodiscard]] const IntervalStats& statsForSemitones(int semitones) const noexcept;

private:
    EarTrainingConfig config_{};
    EarQuestion currentQuestion_{};
    int completedRounds_{0};
    int correctAnswers_{0};
    std::uint32_t rngState_{0x9E3779B9u};
    std::array<IntervalStats, 13> intervalStats_{};
};

} // namespace gitara::engine
