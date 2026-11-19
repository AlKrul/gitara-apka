#include "gitara/engine/EarTrainer.h"
#include <algorithm>
#include <numeric>

namespace gitara::engine {

EarTrainer::EarTrainer(EarTrainingConfig config)
    : config_(std::move(config)) {
    if (config_.activeIntervals.empty()) {
        config_.activeIntervals = gitara::theory::earTrainingIntervals();
    }
}

void EarTrainer::setConfig(EarTrainingConfig config) {
    config_ = std::move(config);
    if (config_.activeIntervals.empty()) {
        config_.activeIntervals = gitara::theory::earTrainingIntervals();
    }
    completedRounds_ = 0;
    correctAnswers_ = 0;
}

void EarTrainer::setupSurpriseMeSession(int rounds) {
    const auto& all = gitara::theory::earTrainingIntervals();
    std::vector<std::size_t> indices(all.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sortuj interwały rosnąco wg dotychczasowej skuteczności (najmniej przećwiczone / najsłabsze na początku)
    std::sort(indices.begin(), indices.end(), [&](std::size_t a, std::size_t b) {
        int sa = std::clamp(all[a].semitones, 0, 12);
        int sb = std::clamp(all[b].semitones, 0, 12);
        const auto& stA = intervalStats_[static_cast<std::size_t>(sa)];
        const auto& stB = intervalStats_[static_cast<std::size_t>(sb)];
        if (stA.attempts == 0 && stB.attempts > 0) return true;
        if (stA.attempts > 0 && stB.attempts == 0) return false;
        return stA.accuracyPercent() < stB.accuracyPercent();
    });

    config_.activeIntervals.clear();
    const std::size_t count = std::min<std::size_t>(5, indices.size());
    for (std::size_t i = 0; i < count; ++i) {
        config_.activeIntervals.push_back(all[indices[i]]);
    }
    config_.endlessSession = false;
    config_.targetRounds = std::max(1, rounds);
    completedRounds_ = 0;
    correctAnswers_ = 0;
}

EarQuestion EarTrainer::nextQuestion() {
    auto nextRand = [&]() -> std::uint32_t {
        rngState_ = rngState_ * 1664525u + 1013904223u;
        return rngState_;
    };

    const auto& pool = config_.activeIntervals.empty()
        ? gitara::theory::earTrainingIntervals()
        : config_.activeIntervals;
    const auto& chosenIv = pool[nextRand() % pool.size()];

    int rootMidi = 48; // C3
    if (config_.rootMode == RootNoteMode::Random) {
        rootMidi = 45 + static_cast<int>(nextRand() % 18); // A2 (45) .. D4 (62)
    } else if (config_.rootMode == RootNoteMode::FixedNote) {
        int pc = config_.fixedRoot.note.pitchClass();
        int oct = 2 + static_cast<int>(nextRand() % 2);
        rootMidi = (oct + 1) * 12 + pc;
    } else {
        rootMidi = config_.fixedRoot.midiNumber();
    }

    bool asc = true;
    if (config_.direction == EarDirection::Descending) {
        asc = false;
    } else if (config_.direction == EarDirection::Random) {
        asc = ((nextRand() & 1u) == 0u);
    }

    int secondMidi = asc ? (rootMidi + chosenIv.semitones) : (rootMidi - chosenIv.semitones);

    EarQuestion q{};
    q.roundNumber = completedRounds_ + 1;
    q.rootPitch = gitara::theory::Pitch::fromMidiNumber(rootMidi);
    q.secondPitch = gitara::theory::Pitch::fromMidiNumber(secondMidi);
    q.interval = chosenIv;
    q.isAscending = asc;
    if (config_.showSongHints) {
        q.songHint = std::string(chosenIv.songHintAscending());
    }
    currentQuestion_ = q;
    return q;
}

bool EarTrainer::submitIntervalAnswer(int guessedSemitones, double reactionTimeMs) {
    int actualSemi = std::clamp(currentQuestion_.interval.semitones, 0, 12);
    auto& st = intervalStats_[static_cast<std::size_t>(actualSemi)];
    ++st.attempts;
    st.totalReactionTimeMs += std::max(0.0, reactionTimeMs);

    ++completedRounds_;
    bool ok = (guessedSemitones == currentQuestion_.interval.semitones);
    if (ok) {
        ++st.correct;
        ++correctAnswers_;
    }
    return ok;
}

bool EarTrainer::submitPlayedGuitarMidi(int detectedMidiNote, double reactionTimeMs) {
    int expectedMidi = currentQuestion_.secondPitch.midiNumber();
    int guessedSemitones = std::abs(detectedMidiNote - currentQuestion_.rootPitch.midiNumber());
    if (detectedMidiNote == expectedMidi) {
        return submitIntervalAnswer(currentQuestion_.interval.semitones, reactionTimeMs);
    }
    return submitIntervalAnswer(guessedSemitones, reactionTimeMs);
}

bool EarTrainer::isSessionFinished() const noexcept {
    if (config_.endlessSession) {
        return false;
    }
    return completedRounds_ >= config_.targetRounds;
}

double EarTrainer::sessionAccuracyPercent() const noexcept {
    if (completedRounds_ <= 0) {
        return 0.0;
    }
    return 100.0 * static_cast<double>(correctAnswers_) / static_cast<double>(completedRounds_);
}

const IntervalStats& EarTrainer::statsForSemitones(int semitones) const noexcept {
    int idx = std::clamp(semitones, 0, 12);
    return intervalStats_[static_cast<std::size_t>(idx)];
}

} // namespace gitara::engine
