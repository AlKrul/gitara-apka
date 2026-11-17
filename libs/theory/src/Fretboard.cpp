#include "gitara/theory/Fretboard.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace gitara::theory {

const std::vector<Tuning>& builtinTunings() {
    static const std::vector<Tuning> tunings = {
        {"standard", "Standardowy (E A D G B E)", {
            Pitch{{Letter::E, Accidental::Natural}, 2},
            Pitch{{Letter::A, Accidental::Natural}, 2},
            Pitch{{Letter::D, Accidental::Natural}, 3},
            Pitch{{Letter::G, Accidental::Natural}, 3},
            Pitch{{Letter::B, Accidental::Natural}, 3},
            Pitch{{Letter::E, Accidental::Natural}, 4}
        }},
        {"drop_d", "Drop D (D A D G B E)", {
            Pitch{{Letter::D, Accidental::Natural}, 2},
            Pitch{{Letter::A, Accidental::Natural}, 2},
            Pitch{{Letter::D, Accidental::Natural}, 3},
            Pitch{{Letter::G, Accidental::Natural}, 3},
            Pitch{{Letter::B, Accidental::Natural}, 3},
            Pitch{{Letter::E, Accidental::Natural}, 4}
        }},
        {"drop_c", "Drop C (C G C F A D)", {
            Pitch{{Letter::C, Accidental::Natural}, 2},
            Pitch{{Letter::G, Accidental::Natural}, 2},
            Pitch{{Letter::C, Accidental::Natural}, 3},
            Pitch{{Letter::F, Accidental::Natural}, 3},
            Pitch{{Letter::A, Accidental::Natural}, 3},
            Pitch{{Letter::D, Accidental::Natural}, 4}
        }},
        {"open_g", "Open G (D G D G B D)", {
            Pitch{{Letter::D, Accidental::Natural}, 2},
            Pitch{{Letter::G, Accidental::Natural}, 2},
            Pitch{{Letter::D, Accidental::Natural}, 3},
            Pitch{{Letter::G, Accidental::Natural}, 3},
            Pitch{{Letter::B, Accidental::Natural}, 3},
            Pitch{{Letter::D, Accidental::Natural}, 4}
        }},
        {"open_d", "Open D (D A D F♯ A D)", {
            Pitch{{Letter::D, Accidental::Natural}, 2},
            Pitch{{Letter::A, Accidental::Natural}, 2},
            Pitch{{Letter::D, Accidental::Natural}, 3},
            Pitch{{Letter::F, Accidental::Sharp},   3},
            Pitch{{Letter::A, Accidental::Natural}, 3},
            Pitch{{Letter::D, Accidental::Natural}, 4}
        }},
        {"open_e", "Open E (E B E G♯ B E)", {
            Pitch{{Letter::E, Accidental::Natural}, 2},
            Pitch{{Letter::B, Accidental::Natural}, 2},
            Pitch{{Letter::E, Accidental::Natural}, 3},
            Pitch{{Letter::G, Accidental::Sharp},   3},
            Pitch{{Letter::B, Accidental::Natural}, 3},
            Pitch{{Letter::E, Accidental::Natural}, 4}
        }},
        {"dadgad", "DADGAD (D A D G A D)", {
            Pitch{{Letter::D, Accidental::Natural}, 2},
            Pitch{{Letter::A, Accidental::Natural}, 2},
            Pitch{{Letter::D, Accidental::Natural}, 3},
            Pitch{{Letter::G, Accidental::Natural}, 3},
            Pitch{{Letter::A, Accidental::Natural}, 3},
            Pitch{{Letter::D, Accidental::Natural}, 4}
        }},
        {"eb_standard", "E♭ Standard (E♭ A♭ D♭ G♭ B♭ E♭)", {
            Pitch{{Letter::E, Accidental::Flat}, 2},
            Pitch{{Letter::A, Accidental::Flat}, 2},
            Pitch{{Letter::D, Accidental::Flat}, 3},
            Pitch{{Letter::G, Accidental::Flat}, 3},
            Pitch{{Letter::B, Accidental::Flat}, 3},
            Pitch{{Letter::E, Accidental::Flat}, 4}
        }},
        {"d_standard", "D Standard (D G C F A D)", {
            Pitch{{Letter::D, Accidental::Natural}, 2},
            Pitch{{Letter::G, Accidental::Natural}, 2},
            Pitch{{Letter::C, Accidental::Natural}, 3},
            Pitch{{Letter::F, Accidental::Natural}, 3},
            Pitch{{Letter::A, Accidental::Natural}, 3},
            Pitch{{Letter::D, Accidental::Natural}, 4}
        }}
    };
    return tunings;
}

const Tuning& findTuning(std::string_view id) noexcept {
    const auto& all = builtinTunings();
    for (const auto& t : all) {
        if (t.id == id) {
            return t;
        }
    }
    return all.front();
}

FretboardMask::FretboardMask(int stringCount, int maxFret)
    : strings_(stringCount),
      maxFret_(maxFret),
      allowed_(static_cast<std::size_t>(stringCount * (maxFret + 1)), true) {}

void FretboardMask::allowAll() {
    std::fill(allowed_.begin(), allowed_.end(), true);
}

void FretboardMask::setAllowed(int stringIndex, int fret, bool allowed) noexcept {
    if (stringIndex < 0 || stringIndex >= strings_ || fret < 0 || fret > maxFret_) {
        return;
    }
    allowed_[static_cast<std::size_t>(stringIndex * (maxFret_ + 1) + fret)] = allowed;
}

void FretboardMask::setRegionAllowed(int minString, int maxString, int minFret, int maxFret, bool allowed) noexcept {
    for (int s = std::max(0, minString); s <= std::min(strings_ - 1, maxString); ++s) {
        for (int f = std::max(0, minFret); f <= std::min(maxFret_, maxFret); ++f) {
            setAllowed(s, f, allowed);
        }
    }
}

void FretboardMask::restrictToFretRange(int minFret, int maxFret) noexcept {
    for (int s = 0; s < strings_; ++s) {
        for (int f = 0; f <= maxFret_; ++f) {
            setAllowed(s, f, f >= minFret && f <= maxFret);
        }
    }
}

bool FretboardMask::isAllowed(int stringIndex, int fret) const noexcept {
    if (stringIndex < 0 || stringIndex >= strings_ || fret < 0 || fret > maxFret_) {
        return false;
    }
    return allowed_[static_cast<std::size_t>(stringIndex * (maxFret_ + 1) + fret)];
}

Fretboard::Fretboard(Tuning tuning, int fretCount)
    : tuning_(std::move(tuning)), fretCount_(fretCount) {}

int Fretboard::midiAt(int stringIndex, int fret) const noexcept {
    if (stringIndex < 0 || stringIndex >= stringCount() || fret < 0 || fret > fretCount_) {
        return -1;
    }
    return tuning_.openStrings[static_cast<std::size_t>(stringIndex)].midiNumber() + fret;
}

Pitch Fretboard::pitchAt(int stringIndex,
                         int fret,
                         AccidentalMode mode,
                         const Scale* scaleContext) const {
    int midi = midiAt(stringIndex, fret);
    if (midi < 0) {
        return Pitch{};
    }

    if (scaleContext && scaleContext->definition) {
        if (auto iv = scaleContext->intervalForPitchClass(midi % 12)) {
            Note spelled = iv->applyTo(scaleContext->root).withAccidentalMode(mode);
            int rawSemi = letterBaseSemitones(spelled.letter) + static_cast<int>(spelled.accidental);
            int oct = (midi - rawSemi) / 12 - 1;
            return Pitch{spelled, oct};
        }
    }

    return Pitch::fromMidiNumber(midi, mode);
}

std::vector<FretPosition> Fretboard::findPositionsForMidi(
    int midiNote,
    const FretboardMask* mask) const {
    std::vector<FretPosition> positions;
    for (int s = 0; s < stringCount(); ++s) {
        int openMidi = tuning_.openStrings[static_cast<std::size_t>(s)].midiNumber();
        int fret = midiNote - openMidi;
        if (fret >= 0 && fret <= fretCount_) {
            if (!mask || mask->isAllowed(s, fret)) {
                positions.push_back(FretPosition{s, fret});
            }
        }
    }
    return positions;
}

std::optional<FretPosition> Fretboard::selectBestPosition(
    int midiNote,
    std::optional<FretPosition> previousPosition,
    const FretboardMask* mask) const {
    auto candidates = findPositionsForMidi(midiNote, mask);
    if (candidates.empty()) {
        return std::nullopt;
    }
    if (!previousPosition.has_value()) {
        // Domyślnie preferuj niższe progi (środek dłoni ok. 3. progu)
        return *std::min_element(candidates.begin(), candidates.end(),
            [](const FretPosition& a, const FretPosition& b) {
                return a.fret < b.fret;
            });
    }

    const auto prev = *previousPosition;
    return *std::min_element(candidates.begin(), candidates.end(),
        [&prev](const FretPosition& a, const FretPosition& b) {
            auto cost = [&prev](const FretPosition& p) {
                // Pusta struna wymaga małego ruchu lewej ręki, ale lekko faworyzujemy bliskie struny
                double fretDist = (p.fret == 0) ? 1.5 : std::abs(p.fret - prev.fret);
                double stringDist = std::abs(p.stringIndex - prev.stringIndex) * 1.25;
                return fretDist + stringDist;
            };
            return cost(a) < cost(b);
        });
}

std::vector<FretPosition> Fretboard::generateScalePositions(
    const Scale& scale,
    PositionSystem system,
    int positionIndex,
    const FretboardMask* mask) const {
    std::vector<FretPosition> out;
    if (!scale.definition) {
        return out;
    }

    if (system == PositionSystem::None) {
        for (int s = 0; s < stringCount(); ++s) {
            for (int f = 0; f <= fretCount_; ++f) {
                int midi = midiAt(s, f);
                if (scale.containsPitchClass(midi % 12)) {
                    if (!mask || mask->isAllowed(s, f)) {
                        out.push_back(FretPosition{s, f});
                    }
                }
            }
        }
        return out;
    }

    if (system == PositionSystem::TNPS || system == PositionSystem::FNPS) {
        const int notesPerString = (system == PositionSystem::TNPS) ? 3 : 4;
        const auto scaleNotes = scale.notes();
        if (scaleNotes.empty()) {
            return out;
        }

        int degIdx = ((positionIndex % static_cast<int>(scaleNotes.size())) +
                      static_cast<int>(scaleNotes.size())) % static_cast<int>(scaleNotes.size());

        // Znajdź pierwszy próg (>= 0) na najniższej strunie odpowiadający stopniowi startowemu
        int startPc = scaleNotes[static_cast<std::size_t>(degIdx)].pitchClass();
        int openLowPc = midiAt(0, 0) % 12;
        int firstFretOnLow = (startPc - openLowPc + 12) % 12;
        int currentMinMidi = midiAt(0, firstFretOnLow);

        for (int s = 0; s < stringCount(); ++s) {
            int addedOnString = 0;
            for (int f = 0; f <= fretCount_; ++f) {
                int m = midiAt(s, f);
                if (m >= currentMinMidi && scale.containsPitchClass(m % 12)) {
                    if (!mask || mask->isAllowed(s, f)) {
                        out.push_back(FretPosition{s, f});
                    }
                    currentMinMidi = m + 1;
                    ++addedOnString;
                    if (addedOnString >= notesPerString) {
                        break;
                    }
                }
            }
        }
        return out;
    }

    // System CAGED: 5 nakładających się okien (boxów) wzdłuż gryfu względem prymy na strunach E i A
    int rootPc = scale.root.pitchClass();
    int openE = midiAt(0, 0) % 12;
    int eStringRootFret = (rootPc - openE + 12) % 12;
    // Standardowe przesunięcia 5 okien CAGED względem prymy na 6. strunie
    constexpr std::array<int, 5> cagedOffsets = {0, 2, 5, 7, 9};
    int boxIdx = ((positionIndex % 5) + 5) % 5;
    int minFret = (eStringRootFret + cagedOffsets[static_cast<std::size_t>(boxIdx)]) % 12;
    int maxFret = minFret + 4;

    for (int s = 0; s < stringCount(); ++s) {
        for (int f = minFret; f <= std::min(fretCount_, maxFret); ++f) {
            int midi = midiAt(s, f);
            if (scale.containsPitchClass(midi % 12)) {
                if (!mask || mask->isAllowed(s, f)) {
                    out.push_back(FretPosition{s, f});
                }
            }
        }
    }
    return out;
}

std::vector<ChordVoicing> Fretboard::generateChordVoicings(
    const Chord& chord,
    bool includeTapping,
    const FretboardMask* mask) const {
    std::vector<ChordVoicing> voicings;
    if (!chord.definition || stringCount() < 3) {
        return voicings;
    }

    const auto requiredNotes = chord.notes();
    const int nStrings = stringCount();

    // Generujemy zwarte chwyty na sąsiednich zestawach 3..4 strun (w tym z pustymi strunami i tappingiem T)
    for (int baseString = 0; baseString <= nStrings - 3; ++baseString) {
        for (int anchorFret = 1; anchorFret <= std::min(fretCount_, 19); ++anchorFret) {
            std::vector<int> frets(static_cast<std::size_t>(nStrings), -1);
            std::vector<bool> tapped(static_cast<std::size_t>(nStrings), false);
            bool valid = true;

            for (int offset = 0; offset < 3; ++offset) {
                int s = baseString + offset;
                int chosenFret = -1;
                for (int f = anchorFret; f <= std::min(fretCount_, anchorFret + 4); ++f) {
                    if (mask && !mask->isAllowed(s, f)) {
                        continue;
                    }
                    if (chord.containsPitchClass(midiAt(s, f) % 12)) {
                        chosenFret = f;
                        break;
                    }
                }
                if (chosenFret < 0) {
                    valid = false;
                    break;
                }
                frets[static_cast<std::size_t>(s)] = chosenFret;
                if (includeTapping && (chosenFret - anchorFret >= 3)) {
                    tapped[static_cast<std::size_t>(s)] = true;
                }
            }

            if (!valid) {
                continue;
            }

            // Sprawdź, czy chwyt pokrywa wszystkie wymagane klasy wysokości dla trójdźwięków
            bool coversAll = true;
            for (const auto& req : requiredNotes) {
                bool found = false;
                for (int s = 0; s < nStrings; ++s) {
                    if (frets[static_cast<std::size_t>(s)] >= 0 &&
                        (midiAt(s, frets[static_cast<std::size_t>(s)]) % 12) == req.pitchClass()) {
                        found = true;
                        break;
                    }
                }
                if (!found && requiredNotes.size() <= 3) {
                    coversAll = false;
                    break;
                }
            }
            if (!coversAll) {
                continue;
            }

            ChordVoicing v{};
            v.frets = frets;
            v.tapped = tapped;
            v.usesTapping = std::any_of(tapped.begin(), tapped.end(), [](bool b) { return b; });
            voicings.push_back(v);

            // Jeśli sąsiednia wyższa struna jako pusta (fret=0) należy do akordu, dodaj wariant z pustą struną
            if (baseString + 3 < nStrings) {
                int openS = baseString + 3;
                if ((!mask || mask->isAllowed(openS, 0)) &&
                    chord.containsPitchClass(midiAt(openS, 0) % 12)) {
                    ChordVoicing openV = v;
                    openV.frets[static_cast<std::size_t>(openS)] = 0;
                    openV.openStringsNote = "puste " +
                        tuning_.openStrings[static_cast<std::size_t>(openS)].note.toString(true);
                    voicings.push_back(std::move(openV));
                }
            }
        }
    }

    return voicings;
}

std::vector<NaturalHarmonic> Fretboard::computeNaturalHarmonics(
    std::optional<int> onlyStringIndex,
    double minFret,
    double maxFret,
    int maxHarmonicN,
    AccidentalMode mode) const {
    std::vector<NaturalHarmonic> harmonics;

    // Przybliżenie części ułamkowej log2(n) w półtonach dla szeregu harmonicznego:
    // n=2 (+12), n=3 (+19), n=4 (+24), n=5 (+28), n=6 (+31), n=7 (+34), n=8 (+36)
    auto harmonicSemitoneOffset = [](int n) -> int {
        return static_cast<int>(std::llround(12.0 * std::log2(static_cast<double>(n))));
    };

    auto difficultyForN = [](int n) -> HarmonicDifficulty {
        if (n <= 4) return HarmonicDifficulty::Easy;
        if (n == 5) return HarmonicDifficulty::Medium;
        if (n <= 7) return HarmonicDifficulty::Hard;
        return HarmonicDifficulty::VeryHard;
    };

    auto formatPolishLocation = [](double exactFret) -> std::string {
        int nearest = static_cast<int>(std::llround(exactFret));
        double diff = exactFret - static_cast<double>(nearest);
        if (std::abs(diff) <= 0.08) {
            return "nad progiem " + std::to_string(nearest);
        }
        if (diff < 0.0 && diff >= -0.30) {
            return "tuż pod progiem " + std::to_string(nearest);
        }
        if (diff > 0.0 && diff <= 0.30) {
            return "tuż nad progiem " + std::to_string(nearest);
        }
        int lowFret = static_cast<int>(std::floor(exactFret));
        return "między progami " + std::to_string(lowFret) + "–" + std::to_string(lowFret + 1);
    };

    int startS = onlyStringIndex.value_or(0);
    int endS = onlyStringIndex.has_value() ? *onlyStringIndex : (stringCount() - 1);

    for (int s = std::max(0, startS); s <= std::min(stringCount() - 1, endS); ++s) {
        int openMidi = midiAt(s, 0);
        for (int n = 2; n <= maxHarmonicN; ++n) {
            for (int k = 1; k < n; ++k) {
                if (std::gcd(k, n) != 1) {
                    continue; // Węzeł redukuje się do niższej harmonicznej
                }
                // Węzeł w odległości k/n od siodełka -> pozostała długość struny L_rem = 1 - k/n = (n-k)/n
                // Próg f spełnia 2^(-f/12) = (n-k)/n => f = 12 * log2(n / (n - k))
                double ratio = static_cast<double>(n) / static_cast<double>(n - k);
                double exactFret = 12.0 * std::log2(ratio);
                if (exactFret < minFret - 0.05 || exactFret > maxFret + 0.05) {
                    continue;
                }

                int semiOffset = harmonicSemitoneOffset(n);
                int soundingMidi = openMidi + semiOffset;

                NaturalHarmonic h{};
                h.stringIndex = s;
                h.harmonicNumber = n;
                h.nodeIndex = k;
                h.exactFret = exactFret;
                h.soundingPitch = Pitch::fromMidiNumber(soundingMidi, mode);
                h.intervalOverOpen = intervalFromSemitones(semiOffset % 12);
                h.difficulty = difficultyForN(n);
                h.polishLocation = formatPolishLocation(exactFret);
                harmonics.push_back(std::move(h));
            }
        }
    }

    std::sort(harmonics.begin(), harmonics.end(),
        [](const NaturalHarmonic& a, const NaturalHarmonic& b) {
            if (a.soundingPitch.midiNumber() != b.soundingPitch.midiNumber()) {
                return a.soundingPitch.midiNumber() < b.soundingPitch.midiNumber();
            }
            return a.exactFret < b.exactFret;
        });

    return harmonics;
}

} // namespace gitara::theory
