#include "gitara/engine/SessionManager.h"
#include <algorithm>

namespace gitara::engine {

SessionManager::SessionManager() {
    sessions_ = createFirstWeekPlan();
}

void SessionManager::addSession(PracticeSession session) {
    sessions_.push_back(std::move(session));
}

bool SessionManager::removeSessionById(std::string_view id) {
    auto it = std::remove_if(sessions_.begin(), sessions_.end(),
                             [&](const PracticeSession& s) { return s.id == id; });
    if (it == sessions_.end()) {
        return false;
    }
    sessions_.erase(it, sessions_.end());
    return true;
}

void SessionManager::recordCompletedSession(int minutesPracticed, double accuracyPercent) {
    int mins = std::max(1, minutesPracticed);
    stats_.totalMinutesPracticed += mins;
    stats_.todayMinutesPracticed += mins;

    double prevTotalAcc = stats_.averageAccuracyPercent *
                          static_cast<double>(stats_.completedSessionsCount);
    ++stats_.completedSessionsCount;
    stats_.averageAccuracyPercent =
        (prevTotalAcc + std::clamp(accuracyPercent, 0.0, 100.0)) /
        static_cast<double>(stats_.completedSessionsCount);

    if (stats_.firstWeekPlanDayCompleted < 7) {
        ++stats_.firstWeekPlanDayCompleted;
    }
}

std::vector<PracticeSession> SessionManager::createFirstWeekPlan() {
    using namespace gitara::theory;
    std::vector<PracticeSession> plan;

    // Sesja 1 (wzorowana bezpośrednio na zrzucie ekranu: G# moll naturalna + Blues 12-taktowy)
    {
        PracticeSession s1{};
        s1.id = "session_1";
        s1.name = "Sesja 1 – G♯ moll naturalna + Rozgrzewka";
        s1.transitionMode = SessionTransitionMode::BreakAndCountdown;
        s1.breakBars = 1;
        s1.usingHeadphones = true;
        s1.metronomeEnabled = true;

        RoutineItem item1{};
        item1.title = "G♯ Molowa naturalna (3NPS)";
        item1.kind = RoutineItemKind::ScaleExercise;
        item1.scaleConfig.scale = Scale{
            Note{Letter::G, Accidental::Sharp},
            findScaleDefinition("natural_minor")
        };
        item1.scaleConfig.positionSystem = PositionSystem::TNPS;
        item1.scaleConfig.pattern = ExercisePattern::UpAndDown;
        item1.durationMinutes = 10;
        s1.items.push_back(std::move(item1));

        if (auto blues = findBuiltinProgression("blues_12")) {
            RoutineItem item2{};
            item2.title = "Blues 12-taktowy w A dur";
            item2.kind = RoutineItemKind::ChordProgression;
            item2.progressionConfig = *blues;
            item2.durationMinutes = 10;
            s1.items.push_back(std::move(item2));
        }

        plan.push_back(std::move(s1));
    }

    // Sesja 2 – C dur (CAGED) + ii–V–I
    {
        PracticeSession s2{};
        s2.id = "session_2";
        s2.name = "Sesja 2 – C dur (CAGED) i ii–V–I";
        s2.transitionMode = SessionTransitionMode::BreakAndCountdown;
        s2.breakBars = 2;

        RoutineItem item1{};
        item1.title = "C Durowa – Grupy po 4";
        item1.kind = RoutineItemKind::ScaleExercise;
        item1.scaleConfig.scale = Scale{
            Note{Letter::C, Accidental::Natural},
            findScaleDefinition("major")
        };
        item1.scaleConfig.positionSystem = PositionSystem::CAGED;
        item1.scaleConfig.pattern = ExercisePattern::GroupsOf4;
        item1.durationMinutes = 10;
        s2.items.push_back(std::move(item1));

        plan.push_back(std::move(s2));
    }

    return plan;
}

} // namespace gitara::engine
