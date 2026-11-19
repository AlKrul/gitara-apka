#include "gitara/engine/WorkshopGenerator.h"
#include <algorithm>

namespace gitara::engine {

GeneratedExercise generateScaleExercise(
    const gitara::theory::Fretboard& fretboard,
    const WorkshopExerciseConfig& config,
    const gitara::theory::FretboardMask* mask) {
    GeneratedExercise result{};

    auto rawPositions = fretboard.generateScalePositions(
        config.scale, config.positionSystem, config.positionIndex, mask);

    // Filtruj pozycje poniżej wybranej struny startowej (gdy startStringIndex >= 0)
    if (config.startStringIndex >= 0) {
        rawPositions.erase(
            std::remove_if(rawPositions.begin(), rawPositions.end(),
                           [&](const gitara::theory::FretPosition& p) {
                               return p.stringIndex < config.startStringIndex;
                           }),
            rawPositions.end());
    }

    // Sortuj rosnąco wg wysokości dźwięku (a przy równym MIDI wg struny i progu)
    std::sort(rawPositions.begin(), rawPositions.end(),
        [&](const gitara::theory::FretPosition& a, const gitara::theory::FretPosition& b) {
            int ma = fretboard.midiAt(a.stringIndex, a.fret);
            int mb = fretboard.midiAt(b.stringIndex, b.fret);
            if (ma != mb) return ma < mb;
            if (a.stringIndex != b.stringIndex) return a.stringIndex < b.stringIndex;
            return a.fret < b.fret;
        });

    // Jeśli wybrano 1 oktawę, ogranicz bazową pulę do pierwszych 8 stopni skali
    if (config.octaves == 1 && rawPositions.size() > 8) {
        rawPositions.resize(8);
    }

    std::vector<gitara::theory::FretPosition> ordered;
    const std::size_t n = rawPositions.size();

    auto applyIntervalJumps = [&](std::size_t stepDist) {
        if (n > stepDist) {
            for (std::size_t i = 0; i + stepDist < n; ++i) {
                ordered.push_back(rawPositions[i]);
                ordered.push_back(rawPositions[i + stepDist]);
            }
        } else {
            ordered = rawPositions;
        }
    };

    switch (config.pattern) {
        case ExercisePattern::Ascending:
            ordered = rawPositions;
            break;

        case ExercisePattern::Descending:
            ordered.assign(rawPositions.rbegin(), rawPositions.rend());
            break;

        case ExercisePattern::UpAndDown:
            ordered = rawPositions;
            if (n > 1) {
                for (std::size_t i = n - 1; i > 0; --i) {
                    ordered.push_back(rawPositions[i - 1]);
                }
            }
            break;

        case ExercisePattern::GroupsOf3:
            if (n >= 3) {
                for (std::size_t i = 0; i + 2 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 1]);
                    ordered.push_back(rawPositions[i + 2]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::GroupsOf4:
            if (n >= 4) {
                for (std::size_t i = 0; i + 3 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 1]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i + 3]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::DiatonicThirds:
            applyIntervalJumps(2);
            break;

        case ExercisePattern::FourthIntervals:
            applyIntervalJumps(3);
            break;

        case ExercisePattern::FifthIntervals:
            applyIntervalJumps(4);
            break;

        case ExercisePattern::SixthIntervals:
            applyIntervalJumps(5);
            break;

        case ExercisePattern::OctaveJumps:
            applyIntervalJumps(7);
            break;
    }

    // Zastosuj kierunek z kreatora ( jeśli pattern nie wymusił własnego kierunku Descending/UpAndDown)
    if (config.pattern != ExercisePattern::Descending && config.pattern != ExercisePattern::UpAndDown) {
        if (config.direction == ExerciseDirection::Descending) {
            std::reverse(ordered.begin(), ordered.end());
        } else if (config.direction == ExerciseDirection::UpAndDown && ordered.size() > 1) {
            auto copy = ordered;
            for (std::size_t i = copy.size() - 1; i > 0; --i) {
                ordered.push_back(copy[i - 1]);
            }
        } else if (config.direction == ExerciseDirection::DownAndUp && ordered.size() > 1) {
            std::vector<gitara::theory::FretPosition> down(ordered.rbegin(), ordered.rend());
            for (std::size_t i = 1; i < ordered.size(); ++i) {
                down.push_back(ordered[i]);
            }
            ordered = std::move(down);
        }
    }

    const int bpm = std::max(20, config.tempoBpm);
    gitara::score::Beat protoBeat{};
    protoBeat.duration = config.noteDuration;
    const double stepDurationSec = (60.0 / static_cast<double>(bpm)) * protoBeat.quarterNoteLength();

    result.song.title = config.scale.fullName(true);
    result.song.tempoBpm = bpm;
    gitara::score::Track track{};
    track.name = "Gitara";
    track.tuning = fretboard.tuning();

    gitara::score::Measure currentMeasure{};
    currentMeasure.numerator = 4;
    currentMeasure.denominator = 4;
    double measureQuarters = 0.0;

    for (std::size_t i = 0; i < ordered.size(); ++i) {
        const auto& pos = ordered[i];
        int midi = fretboard.midiAt(pos.stringIndex, pos.fret);

        ExerciseStep step{};
        step.isChord = false;
        step.targetPosition = pos;
        step.expectedMidiNote = midi;
        step.expectedTimeSeconds = static_cast<double>(i) * stepDurationSec;
        result.steps.push_back(step);

        gitara::score::Beat b{};
        b.duration = config.noteDuration;
        b.isRest = false;
        gitara::score::TabNote tn{};
        tn.stringIndex = pos.stringIndex;
        tn.fret = pos.fret;
        tn.velocity = 95;
        b.notes.push_back(tn);

        double qLen = b.quarterNoteLength();
        if (measureQuarters + qLen > 4.0 + 1e-4 && !currentMeasure.beats.empty()) {
            track.measures.push_back(std::move(currentMeasure));
            currentMeasure = gitara::score::Measure{};
            measureQuarters = 0.0;
        }
        measureQuarters += qLen;
        currentMeasure.beats.push_back(std::move(b));
    }

    if (!currentMeasure.beats.empty() || track.measures.empty()) {
        track.measures.push_back(std::move(currentMeasure));
    }
    result.song.tracks.push_back(std::move(track));
    return result;
}

} // namespace gitara::engine
