#pragma once

#include "gitara/engine/ProgressionEngine.h"
#include "gitara/engine/WorkshopGenerator.h"
#include <cstdint>
#include <string>
#include <vector>

namespace gitara::engine {

enum class SessionTransitionMode : std::uint8_t {
    BreakAndCountdown = 0, ///< „Przerwa i odliczanie” (przerwa i 3-sekundowe odliczanie przed każdym ćwiczeniem)
    ImmediateNext     = 1  ///< „Od razu dalej”
};

enum class RoutineItemKind : std::uint8_t {
    ScaleExercise    = 0,
    ChordProgression = 1,
    EarTraining      = 2
};

struct RoutineItem {
    std::string title;
    RoutineItemKind kind{RoutineItemKind::ScaleExercise};
    WorkshopExerciseConfig scaleConfig{};
    ChordProgression progressionConfig{};
    int durationMinutes{5};
};

/// Konfiguracja przebiegu sesji w zakładce „Warsztat -> Sesje” (dokładnie wg zrzutu ekranu).
struct PracticeSession {
    std::string id;
    std::string name{"Sesja 1"};
    std::vector<RoutineItem> items;
    SessionTransitionMode transitionMode{SessionTransitionMode::BreakAndCountdown};
    int breakBars{1};             ///< „Takty przerwy” przed kolejnym ćwiczeniem, gdy gra metronom
    std::string audioInputDevice; ///< Wybrane wejście audio (np. Hotone Ampero One ASIO / Mikrofon)
    bool usingHeadphones{true};   ///< „Używam słuchawek” (gdy wyłączone: tłumienie własnego akompaniamentu w detekcji)
    bool metronomeEnabled{true};  ///< „Metronom”
    bool referenceNoteEnabled{false}; ///< „Nuta referencyjna” (dron tonalny w tle)
    bool backingChordEnabled{false};  ///< „Akord w tle”
};

/// Statystyki na Pulpicie (Dashboard – Etap 5.1).
struct DashboardStats {
    int totalMinutesPracticed{0};
    int todayMinutesPracticed{0};
    int currentStreakDays{1};
    int completedSessionsCount{0};
    double averageAccuracyPercent{0.0};
    int firstWeekPlanDayCompleted{0}; ///< Postęp planu „Pierwszy tydzień” (0..7 dni)
};

/// Menedżer sesji treningowych (rutyn) i statystyk postępów.
class SessionManager {
public:
    SessionManager();

    void addSession(PracticeSession session);
    bool removeSessionById(std::string_view id);
    [[nodiscard]] const std::vector<PracticeSession>& sessions() const noexcept { return sessions_; }

    /// Rejestruje ukończoną sesję treningową i aktualizuje statystyki Dashboardu.
    void recordCompletedSession(int minutesPracticed, double accuracyPercent);

    [[nodiscard]] const DashboardStats& dashboardStats() const noexcept { return stats_; }

    /// Tworzy domyślną 7-dniową ścieżkę wdrożeniową „Pierwszy tydzień”.
    [[nodiscard]] static std::vector<PracticeSession> createFirstWeekPlan();

private:
    std::vector<PracticeSession> sessions_;
    DashboardStats stats_{};
};

} // namespace gitara::engine
