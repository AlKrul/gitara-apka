#include "gitara/engine/WorkshopGenerator.h"
#include <algorithm>
#include <cmath>
#include <map>

namespace gitara::engine {

namespace {

bool isAllowedByBounds(
    const gitara::theory::FretboardMask* mask,
    int s,
    int f,
    int minFret,
    int maxFret) {
    if (f < minFret || f > maxFret) return false;
    if (mask && !mask->isAllowed(s, f)) return false;
    return true;
}

std::vector<gitara::theory::FretPosition> buildDiagonalScalePositions(
    const gitara::theory::Fretboard& fretboard,
    const WorkshopExerciseConfig& config,
    const gitara::theory::FretboardMask* mask,
    int minFret,
    int maxFret) {
    std::vector<gitara::theory::FretPosition> out;
    if (!config.scale.definition) return out;

    const int nStrings = fretboard.stringCount();
    const int nFrets = std::min(fretboard.fretCount(), maxFret);
    const int effStartStr = (config.startStringIndex >= 0 && config.startStringIndex < nStrings)
        ? config.startStringIndex
        : 0;
    const int rootPc = config.scale.root.pitchClass();

    // Znajdź najniższą nutę startową (tonikę, gdy startOnRoot lub pierwszą w skali na strunie startowej)
    int startFret = -1;
    int startStr = effStartStr;
    for (int s = effStartStr; s < nStrings && startFret < 0; ++s) {
        for (int f = minFret; f <= nFrets; ++f) {
            if (!isAllowedByBounds(mask, s, f, minFret, maxFret)) continue;
            int pc = fretboard.midiAt(s, f) % 12;
            if (config.startOnRoot ? (pc == rootPc) : config.scale.containsPitchClass(pc)) {
                startStr = s;
                startFret = f;
                break;
            }
        }
    }
    if (startFret < 0) return out;

    const std::size_t degCount = std::max<std::size_t>(5, config.scale.definition->intervals.size());
    const int effOctaves = std::clamp(config.octaves, 1, 3);
    const std::size_t targetNotes = static_cast<std::size_t>(effOctaves) * degCount + 1;

    // Płynny diagonalny przebieg po 6 strunach: ~2-3 dźwięki na strunę z przesunięciem w górę gryfu
    // Dla 3 oktaw z 6 strun: na niższych strunach bierzemy 3-4 dźwięki (z małym slidem), na ostatniej kończymy oktawę.
    int curMidi = fretboard.midiAt(startStr, startFret);
    int curStr = startStr;
    int curFret = startFret;
    out.push_back({curStr, curFret});

    const int notesPerStrBase = (effOctaves >= 3) ? 4 : 3;
    int notesOnCurStr = 1;

    while (out.size() < targetNotes) {
        // Znajdź następną wysokość MIDI należącą do skali (ściśle wyższą niż curMidi)
        int nextMidi = -1;
        for (int m = curMidi + 1; m <= curMidi + 6; ++m) {
            if (config.scale.containsPitchClass(m % 12)) {
                nextMidi = m;
                break;
            }
        }
        if (nextMidi < 0) break;

        // Czy przejść na kolejną strunę, czy kontynuować na obecnej?
        const bool isLastStr = (curStr >= nStrings - 1);
        int chosenS = -1;
        int chosenF = -1;

        auto fretForMidiOnString = [&](int s, int targetM) -> int {
            int openM = fretboard.midiAt(s, 0);
            int f = targetM - openM;
            if (f < minFret || f > nFrets) return -1;
            if (!isAllowedByBounds(mask, s, f, minFret, maxFret)) return -1;
            return f;
        };

        int fSame = fretForMidiOnString(curStr, nextMidi);
        int fNext = (!isLastStr) ? fretForMidiOnString(curStr + 1, nextMidi) : -1;

        // Preferuj przejście na kolejną strunę, gdy mamy już zaplanowaną liczbę dźwięków na strunie
        // albo gdy próg na tej samej strunie wymagałby zbyt dużego skoku (>4 progi), a następna struna jest ergonomiczna
        bool wantShiftString = !isLastStr && fNext >= 0 &&
            (notesOnCurStr >= notesPerStrBase ||
             (fSame < 0) ||
             (fSame - curFret > 4) ||
             (notesOnCurStr >= 2 && fNext >= std::max(minFret, curFret - 1) && fNext <= curFret + 2));

        if (wantShiftString) {
            chosenS = curStr + 1;
            chosenF = fNext;
            notesOnCurStr = 1;
        } else if (fSame >= 0) {
            chosenS = curStr;
            chosenF = fSame;
            ++notesOnCurStr;
        } else if (fNext >= 0) {
            chosenS = curStr + 1;
            chosenF = fNext;
            notesOnCurStr = 1;
        } else {
            // Szukaj na dowolnej wyższej strunie
            for (int s = curStr + 1; s < nStrings; ++s) {
                int fCand = fretForMidiOnString(s, nextMidi);
                if (fCand >= 0) {
                    chosenS = s;
                    chosenF = fCand;
                    notesOnCurStr = 1;
                    break;
                }
            }
        }

        if (chosenS < 0 || chosenF < 0) break;
        curStr = chosenS;
        curFret = chosenF;
        curMidi = nextMidi;
        out.push_back({curStr, curFret});
    }

    return out;
}

std::vector<gitara::theory::FretPosition> buildSingleStringPositions(
    const gitara::theory::Fretboard& fretboard,
    const WorkshopExerciseConfig& config,
    const gitara::theory::FretboardMask* mask,
    int minFret,
    int maxFret) {
    std::vector<gitara::theory::FretPosition> out;
    if (!config.scale.definition) return out;

    const int nStrings = fretboard.stringCount();
    const int nFrets = std::min(fretboard.fretCount(), maxFret);
    const int s = (config.startStringIndex >= 0 && config.startStringIndex < nStrings)
        ? config.startStringIndex
        : (nStrings - 1); // Domyślnie struna E1 (najcieńsza, najbardziej czytelna do ćwiczeń poziomych)

    const int rootPc = config.scale.root.pitchClass();
    bool started = !config.startOnRoot;

    for (int f = minFret; f <= nFrets; ++f) {
        if (!isAllowedByBounds(mask, s, f, minFret, maxFret)) continue;
        int pc = fretboard.midiAt(s, f) % 12;
        if (!config.scale.containsPitchClass(pc)) continue;
        if (!started) {
            if (pc == rootPc) started = true;
            else continue;
        }
        out.push_back({s, f});
    }

    // Fallback: jeśli tonika była za wysoko i zostało < 4 nuty, weź od początku zakresu progów
    if (out.size() < 4 && config.startOnRoot) {
        out.clear();
        for (int f = minFret; f <= nFrets; ++f) {
            if (!isAllowedByBounds(mask, s, f, minFret, maxFret)) continue;
            if (config.scale.containsPitchClass(fretboard.midiAt(s, f) % 12)) {
                out.push_back({s, f});
            }
        }
    }

    const std::size_t degCount = std::max<std::size_t>(5, config.scale.definition->intervals.size());
    const int effOctaves = std::clamp(config.octaves, 1, 3);
    const std::size_t maxNotes = static_cast<std::size_t>(effOctaves) * degCount + 1;
    if (out.size() > maxNotes) {
        out.resize(maxNotes);
    }
    return out;
}

} // namespace

GeneratedExercise generateScaleExercise(
    const gitara::theory::Fretboard& fretboard,
    const WorkshopExerciseConfig& config,
    const gitara::theory::FretboardMask* mask) {
    GeneratedExercise result{};

    const int effMinFret = std::clamp(config.minFret, 0, fretboard.fretCount());
    const int effMaxFret = std::clamp(config.maxFret, effMinFret, fretboard.fretCount());
    const int effOctaves = std::clamp(config.octaves, 1, 3);
    const std::size_t degCount = std::max<std::size_t>(
        5, config.scale.definition ? config.scale.definition->intervals.size() : 7);

    std::vector<gitara::theory::FretPosition> rawPositions;

    if (config.traversalMode == ScaleTraversalMode::DiagonalShift) {
        rawPositions = buildDiagonalScalePositions(fretboard, config, mask, effMinFret, effMaxFret);
    } else if (config.traversalMode == ScaleTraversalMode::SingleString) {
        rawPositions = buildSingleStringPositions(fretboard, config, mask, effMinFret, effMaxFret);
    } else {
        rawPositions = fretboard.generateScalePositions(
            config.scale, config.positionSystem, config.positionIndex, mask);

        // Filtruj wg zakresu progów i wybranej struny startowej
        rawPositions.erase(
            std::remove_if(rawPositions.begin(), rawPositions.end(),
                           [&](const gitara::theory::FretPosition& p) {
                               if (p.fret < effMinFret || p.fret > effMaxFret) return true;
                               if (config.startStringIndex >= 0 && p.stringIndex < config.startStringIndex) return true;
                               return false;
                           }),
            rawPositions.end());

        // Sortuj rosnąco wg wysokości MIDI
        std::sort(rawPositions.begin(), rawPositions.end(),
            [&](const gitara::theory::FretPosition& a, const gitara::theory::FretPosition& b) {
                int ma = fretboard.midiAt(a.stringIndex, a.fret);
                int mb = fretboard.midiAt(b.stringIndex, b.fret);
                if (ma != mb) return ma < mb;
                if (a.stringIndex != b.stringIndex) return a.stringIndex < b.stringIndex;
                return a.fret < b.fret;
            });

        // Usuń unisono (ta sama wysokość MIDI na dwóch strunach) wybierając próg najbliższy środkowi pozycji
        if (!rawPositions.empty()) {
            double avgFret = 0.0;
            for (const auto& p : rawPositions) avgFret += p.fret;
            avgFret /= static_cast<double>(rawPositions.size());

            std::vector<gitara::theory::FretPosition> dedup;
            dedup.reserve(rawPositions.size());
            std::size_t i = 0;
            while (i < rawPositions.size()) {
                int m = fretboard.midiAt(rawPositions[i].stringIndex, rawPositions[i].fret);
                std::size_t best = i;
                double bestDist = std::abs(rawPositions[i].fret - avgFret);
                std::size_t j = i + 1;
                while (j < rawPositions.size() &&
                       fretboard.midiAt(rawPositions[j].stringIndex, rawPositions[j].fret) == m) {
                    double d = std::abs(rawPositions[j].fret - avgFret);
                    if (d < bestDist) {
                        bestDist = d;
                        best = j;
                    }
                    ++j;
                }
                dedup.push_back(rawPositions[best]);
                i = j;
            }
            rawPositions = std::move(dedup);
        }

        // Opcja Start od toniki (prymy): przesuń początek na pierwszą prymę w zakresie
        if (config.startOnRoot && !rawPositions.empty()) {
            int rootPc = config.scale.root.pitchClass();
            auto itRoot = std::find_if(rawPositions.begin(), rawPositions.end(),
                [&](const gitara::theory::FretPosition& p) {
                    return (fretboard.midiAt(p.stringIndex, p.fret) % 12) == rootPc;
                });
            if (itRoot != rawPositions.end() && std::distance(itRoot, rawPositions.end()) >= 4) {
                rawPositions.erase(rawPositions.begin(), itRoot);
            }
        }

        const std::size_t targetNotes = static_cast<std::size_t>(effOctaves) * degCount + 1;

        // Dla 3 oktaw: jeśli statyczny boks (np. 3NPS/CAGED) kończy się wcześniej, dobuduj końcówkę ergonomicznie na najwyższej strunie E1!
        if (effOctaves >= 3 && !rawPositions.empty() && rawPositions.size() < targetNotes) {
            int topStr = fretboard.stringCount() - 1;
            int lastMidi = fretboard.midiAt(rawPositions.back().stringIndex, rawPositions.back().fret);
            for (int f = effMinFret; f <= effMaxFret && rawPositions.size() < targetNotes; ++f) {
                if (!isAllowedByBounds(mask, topStr, f, effMinFret, effMaxFret)) continue;
                int m = fretboard.midiAt(topStr, f);
                if (m > lastMidi && config.scale.containsPitchClass(m % 12)) {
                    rawPositions.push_back({topStr, f});
                    lastMidi = m;
                }
            }
        }

        // Ogranicz pulę do wybranej liczby oktaw:
        // - 1 oktawa = dokładnie 1 oktawa (deg+1)
        // - 2 oktawy: dla pełnego gryfu (None) przycinamy do 2*deg+1; dla boksów (3NPS/4NPS/CAGED) zachowujemy pełny 6-strunowy kształt!
        // - 3 oktawy = do 3*deg+1
        if (effOctaves == 1 && rawPositions.size() > degCount + 1) {
            rawPositions.resize(degCount + 1);
        } else if (effOctaves == 2) {
            if (config.positionSystem == gitara::theory::PositionSystem::None &&
                rawPositions.size() > 2 * degCount + 1) {
                rawPositions.resize(2 * degCount + 1);
            }
        } else if (rawPositions.size() > targetNotes) {
            rawPositions.resize(targetNotes);
        }
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

    auto applyGroupRun = [&](std::size_t grp) {
        if (n >= grp) {
            for (std::size_t i = 0; i + grp <= n; ++i) {
                for (std::size_t k = 0; k < grp; ++k) {
                    ordered.push_back(rawPositions[i + k]);
                }
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
            applyGroupRun(3);
            break;

        case ExercisePattern::GroupsOf4:
            applyGroupRun(4);
            break;

        case ExercisePattern::GroupsOf6:
            applyGroupRun(6);
            break;

        case ExercisePattern::BackAndForth:
            if (n >= 3) {
                for (std::size_t i = 0; i + 2 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 1]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i + 1]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::PaulGilbert4:
            if (n >= 3) {
                for (std::size_t i = 0; i + 2 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 1]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::DiatonicThirds:
            applyIntervalJumps(2);
            break;

        case ExercisePattern::BrokenThirdsZigZag:
            if (n >= 4) {
                for (std::size_t i = 0; i + 2 < n; ++i) {
                    if (i % 2 == 0) {
                        ordered.push_back(rawPositions[i]);
                        ordered.push_back(rawPositions[i + 2]);
                    } else {
                        ordered.push_back(rawPositions[i + 2]);
                        ordered.push_back(rawPositions[i]);
                    }
                }
            } else {
                applyIntervalJumps(2);
            }
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
            applyIntervalJumps(degCount);
            break;

        case ExercisePattern::DiatonicTriads:
            if (n >= 5) {
                for (std::size_t i = 0; i + 4 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i + 4]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::DiatonicTriadsAlternating:
            if (n >= 5) {
                for (std::size_t i = 0; i + 4 < n; ++i) {
                    if (i % 2 == 0) {
                        ordered.push_back(rawPositions[i]);
                        ordered.push_back(rawPositions[i + 2]);
                        ordered.push_back(rawPositions[i + 4]);
                    } else {
                        ordered.push_back(rawPositions[i + 4]);
                        ordered.push_back(rawPositions[i + 2]);
                        ordered.push_back(rawPositions[i]);
                    }
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::DiatonicSevenths:
            if (n >= 7) {
                for (std::size_t i = 0; i + 6 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i + 4]);
                    ordered.push_back(rawPositions[i + 6]);
                }
            } else if (n >= 5) {
                for (std::size_t i = 0; i + 4 < n; ++i) {
                    ordered.push_back(rawPositions[i]);
                    ordered.push_back(rawPositions[i + 2]);
                    ordered.push_back(rawPositions[i + 4]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::PedalPointRoot:
            if (n >= 2) {
                const auto& anchor = rawPositions.front();
                for (std::size_t i = 1; i < n; ++i) {
                    ordered.push_back(anchor);
                    ordered.push_back(rawPositions[i]);
                }
            } else {
                ordered = rawPositions;
            }
            break;

        case ExercisePattern::StringSkipping: {
            // Grupuj dźwięki wg strun i przeplataj z przeskokiem o 1 strunę (0->2, 1->3, 2->4, 3->5)
            std::map<int, std::vector<gitara::theory::FretPosition>> byStr;
            for (const auto& p : rawPositions) {
                byStr[p.stringIndex].push_back(p);
            }
            std::vector<int> strKeys;
            for (const auto& kv : byStr) strKeys.push_back(kv.first);
            if (strKeys.size() >= 3) {
                for (std::size_t k = 0; k + 2 < strKeys.size(); ++k) {
                    const auto& lowS = byStr[strKeys[k]];
                    const auto& skipS = byStr[strKeys[k + 2]];
                    for (const auto& p : lowS) ordered.push_back(p);
                    for (const auto& p : skipS) ordered.push_back(p);
                }
            } else {
                ordered = rawPositions;
            }
            break;
        }
    }

    // Zastosuj kierunek z kreatora (jeśli pattern nie wymusił własnego kierunku Descending/UpAndDown)
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
    if (config.tuplet) {
        protoBeat.tupletNumerator = 3;
        protoBeat.tupletDenominator = 2;
    }
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
        if (config.tuplet) {
            b.tupletNumerator = 3;
            b.tupletDenominator = 2;
        }
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
