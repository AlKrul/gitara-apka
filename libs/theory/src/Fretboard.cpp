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
                if (mask && !mask->isAllowed(s, f)) continue;
                int m = midiAt(s, f);
                if (m >= currentMinMidi && scale.containsPitchClass(m % 12)) {
                    out.push_back(FretPosition{s, f});
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
    const FretboardMask* mask,
    VoicingSharpness sharpness,
    int maxStrings,
    int capoFret) const {
    std::vector<ChordVoicing> voicings;
    if (!chord.definition || stringCount() < 3) {
        return voicings;
    }

    // Kapodaster na progu safeCapo: traktujemy próg kapodastra jako nowe siodełko (puste struny brzmią o +safeCapo półtonów wyżej),
    // dzięki czemu generator odkrywa otwarte kształty oparte o kapodaster oraz wyższe pozycje na gryfie,
    // a wynikowe progi zwracane są w bezwzględnych współrzędnych fizycznego gryfu!
    const int safeCapo = std::clamp(capoFret, 0, std::max(0, fretCount_ - 4));
    if (safeCapo > 0) {
        Tuning shiftedTuning = tuning_;
        for (auto& openP : shiftedTuning.openStrings) {
            openP = Pitch::fromMidiNumber(openP.midiNumber() + safeCapo);
        }
        const int remFrets = std::max(4, fretCount_ - safeCapo);
        Fretboard shiftedFb{std::move(shiftedTuning), remFrets};

        std::optional<FretboardMask> shiftedMask;
        if (mask) {
            shiftedMask.emplace(stringCount(), remFrets);
            for (int s = 0; s < stringCount(); ++s) {
                for (int relF = 0; relF <= remFrets; ++relF) {
                    shiftedMask->setAllowed(s, relF, mask->isAllowed(s, relF + safeCapo));
                }
            }
        }

        auto shiftedVoicings = shiftedFb.generateChordVoicings(
            chord, includeTapping, shiftedMask ? &(*shiftedMask) : nullptr, sharpness, maxStrings, 0);
        for (auto& v : shiftedVoicings) {
            v.openStringsNote.clear();
            for (int s = 0; s < stringCount(); ++s) {
                int& f = v.frets[static_cast<std::size_t>(s)];
                if (f >= 0) {
                    const bool isCapoOpen = (f == 0);
                    f += safeCapo;
                    if (isCapoOpen) {
                        if (v.openStringsNote.empty()) {
                            v.openStringsNote = "kapo " + std::to_string(safeCapo) + " (";
                        } else {
                            v.openStringsNote += ",";
                        }
                        v.openStringsNote += pitchAt(s, f).note.toString(false);
                    }
                }
            }
            if (!v.openStringsNote.empty()) {
                v.openStringsNote += ")";
            }
        }
        return shiftedVoicings;
    }

    const auto requiredNotes = chord.notes();
    const int nStrings = stringCount();
    const int reqCount = static_cast<int>(requiredNotes.size());
    const bool strictTriadMode = (maxStrings == 3 && reqCount == 3);

    // Maska wymaganych klas wysokości (wszystkie składniki akordu oprócz optionalIndices;
    // w trybie ścisłych trójdźwięków wymagamy kompletnego trójdźwięku na dokładnie 3 strunach!)
    int requiredPcMask = 0;
    for (int i = 0; i < reqCount; ++i) {
        bool isOpt = false;
        if (!strictTriadMode) {
            isOpt = std::find(chord.definition->optionalIndices.begin(),
                              chord.definition->optionalIndices.end(),
                              i) != chord.definition->optionalIndices.end();
        }
        if (!isOpt) {
            requiredPcMask |= (1 << requiredNotes[static_cast<std::size_t>(i)].pitchClass());
        }
    }

    const int minActiveStrings = (maxStrings > 0)
        ? std::min(nStrings, maxStrings)
        : (reqCount <= 2 ? 2 : 3);
    const int maxActiveStrings = (maxStrings > 0)
        ? std::min(nStrings, maxStrings)
        : nStrings;

    std::vector<ChordVoicing> mildCompact;
    std::vector<ChordVoicing> mildSpan4Fallback;
    std::vector<ChordVoicing> sharpStretch;
    std::vector<ChordVoicing> sharpTapped;

    auto recordVoicing = [&](std::vector<int> frets, std::vector<bool> explicitTapped, bool isTappedMode) -> bool {
        int minFretPos = 99;
        int maxFretPos = 0;
        int activeCount = 0;
        int presentPcMask = 0;

        for (int s = 0; s < nStrings; ++s) {
            int f = frets[static_cast<std::size_t>(s)];
            if (f >= 0) {
                ++activeCount;
                int pc = midiAt(s, f) % 12;
                if (!chord.containsPitchClass(pc)) return false;
                presentPcMask |= (1 << pc);
                if (f > 0) {
                    minFretPos = std::min(minFretPos, f);
                    maxFretPos = std::max(maxFretPos, f);
                }
            }
        }
        if (activeCount < minActiveStrings || activeCount > maxActiveStrings) return false;
        if ((presentPcMask & requiredPcMask) != requiredPcMask) return false;

        int span = (minFretPos <= maxFretPos) ? (maxFretPos - minFretPos) : 0;
        bool usesTapping = false;
        std::vector<bool> tapped(static_cast<std::size_t>(nStrings), false);

        if (isTappedMode) {
            int tappedCount = 0;
            int lhMin = 99, lhMax = 0;
            for (int s = 0; s < nStrings; ++s) {
                int f = frets[static_cast<std::size_t>(s)];
                if (f > 0) {
                    if (explicitTapped[static_cast<std::size_t>(s)]) {
                        tapped[static_cast<std::size_t>(s)] = true;
                        usesTapping = true;
                        ++tappedCount;
                    } else {
                        lhMin = std::min(lhMin, f);
                        lhMax = std::max(lhMax, f);
                    }
                }
            }
            if (!usesTapping || tappedCount == 0 || tappedCount > 2) return false;
            if (lhMin <= lhMax && (lhMax - lhMin) > 3) return false;
        } else {
            if (span > 5) return false;
        }

        auto hasDuplicate = [&](const std::vector<ChordVoicing>& list) {
            return std::any_of(list.begin(), list.end(), [&](const ChordVoicing& e) {
                return e.frets == frets;
            });
        };
        if (hasDuplicate(mildCompact) || hasDuplicate(mildSpan4Fallback) ||
            hasDuplicate(sharpStretch) || hasDuplicate(sharpTapped)) {
            return false;
        }

        ChordVoicing v{};
        v.frets = std::move(frets);
        v.tapped = std::move(tapped);
        v.usesTapping = usesTapping;
        v.fretSpan = span;
        v.activeStrings = activeCount;
        v.isSharpVoicing = usesTapping || (span >= 4);
        for (int s = 0; s < nStrings; ++s) {
            if (v.frets[static_cast<std::size_t>(s)] == 0) {
                if (v.openStringsNote.empty()) v.openStringsNote = "puste ";
                else v.openStringsNote += ",";
                v.openStringsNote += tuning_.openStrings[static_cast<std::size_t>(s)].note.toString(false);
            }
        }

        if (!usesTapping && span <= 3) {
            mildCompact.push_back(std::move(v));
        } else if (!usesTapping && span == 4) {
            // Rozpiętość 4 progi jest zarówno w fallbacku łagodnych, jak i w ostrych rozciągnięciach
            mildSpan4Fallback.push_back(v);
            sharpStretch.push_back(std::move(v));
        } else if (!usesTapping && span >= 5) {
            sharpStretch.push_back(std::move(v));
        } else {
            sharpTapped.push_back(std::move(v));
        }
        return true;
    };

    // Zestawy strun: ciągłe okna (np. 6, 5, 4, 3 struny) + popularne chwyty z pominięciem 1 struny (drop-3 / shell)
    std::vector<std::vector<int>> subsets;
    for (int len = maxActiveStrings; len >= minActiveStrings; --len) {
        for (int base = 0; base <= nStrings - len; ++base) {
            std::vector<int> ss;
            for (int o = 0; o < len; ++o) ss.push_back(base + o);
            subsets.push_back(std::move(ss));
        }
    }
    if (nStrings == 6) {
        if (maxActiveStrings >= 4 && minActiveStrings <= 4) {
            subsets.push_back({0, 2, 3, 4});
            subsets.push_back({1, 3, 4, 5});
            subsets.push_back({0, 1, 3, 4});
            subsets.push_back({1, 2, 4, 5});
        }
        if (maxActiveStrings >= 3 && minActiveStrings <= 3) {
            subsets.push_back({0, 2, 3});
            subsets.push_back({1, 3, 4});
            subsets.push_back({2, 4, 5});
            subsets.push_back({0, 1, 3});
            subsets.push_back({1, 2, 4});
            subsets.push_back({2, 3, 5});
        }
    }

    auto exploreBox = [&](int maxBoxSpan, bool allowUpperOpen, int minRequiredSpan = 0) {
        for (int startFret = 0; startFret <= std::min(fretCount_, 19); ++startFret) {
            int lo = std::max(1, startFret);
            int hi = std::min(fretCount_, lo + maxBoxSpan);
            for (const auto& ss : subsets) {
                if (maxBoxSpan >= 5 && static_cast<int>(ss.size()) > 5) continue;
                std::vector<std::vector<int>> cands(ss.size());
                bool ok = true;
                for (std::size_t idx = 0; idx < ss.size(); ++idx) {
                    int s = ss[idx];
                    if ((startFret <= 2 || (allowUpperOpen && s >= nStrings - 3)) &&
                        (!mask || mask->isAllowed(s, 0)) &&
                        chord.containsPitchClass(midiAt(s, 0) % 12)) {
                        cands[idx].push_back(0);
                    }
                    int firstMatch = -1, lastMatch = -1;
                    for (int f = lo; f <= hi; ++f) {
                        if (mask && !mask->isAllowed(s, f)) continue;
                        if (chord.containsPitchClass(midiAt(s, f) % 12)) {
                            if (firstMatch < 0) firstMatch = f;
                            lastMatch = f;
                        }
                    }
                    if (firstMatch >= 0) cands[idx].push_back(firstMatch);
                    if (lastMatch > firstMatch) cands[idx].push_back(lastMatch);
                    if (cands[idx].empty()) {
                        ok = false;
                        break;
                    }
                }
                if (!ok) continue;

                std::vector<int> frets(static_cast<std::size_t>(nStrings), -1);
                std::vector<bool> noTap(static_cast<std::size_t>(nStrings), false);
                int emittedHere = 0;
                auto dfs = [&](auto& self, std::size_t idx) -> void {
                    if (emittedHere >= 4) return;
                    if (idx == ss.size()) {
                        if (minRequiredSpan > 0) {
                            int mn = 99, mx = 0;
                            for (int f : frets) if (f > 0) { mn = std::min(mn, f); mx = std::max(mx, f); }
                            if (mn > mx || (mx - mn) < minRequiredSpan) return;
                        }
                        if (recordVoicing(frets, noTap, false)) ++emittedHere;
                        return;
                    }
                    for (int f : cands[idx]) {
                        frets[static_cast<std::size_t>(ss[idx])] = f;
                        self(self, idx + 1);
                        if (emittedHere >= 4) break;
                    }
                };
                dfs(dfs, 0);
            }
        }
    };

    // Pass A: „Łagodne” zwarte chwyty jednej ręki (rozpiętość <= 3 progi)
    if (sharpness != VoicingSharpness::Sharp) {
        exploreBox(3, true, 0);
        if (mildCompact.size() < 2) {
            exploreBox(4, true, 0);
        }
    }

    // Pass B i C: „Ostre” (rozciągnięcia 4–5 progów bez tapingu ORAZ opcjonalny tapping prawą ręką T)
    if (sharpness != VoicingSharpness::Mild) {
        exploreBox(5, true, 4);

        if (includeTapping) {
            // Pass C: Tapping prawą ręką (T) na 3–5 strunowych zestawach
            for (const auto& ss : subsets) {
                if (static_cast<int>(ss.size()) < 3 || static_cast<int>(ss.size()) > 5) continue;
                for (int anchorFret = 1; anchorFret <= std::min(fretCount_, 14); ++anchorFret) {
                    int leftHi = std::min(fretCount_, anchorFret + 3);
                    std::size_t k = ss.size();

                    // Wariant C1: niższe struny w lewej ręce, najwyższa (k-1) tapowana prawą ręką powyżej progu leftHi + 2
                    {
                        std::vector<int> frets(static_cast<std::size_t>(nStrings), -1);
                        std::vector<bool> tapFlags(static_cast<std::size_t>(nStrings), false);
                        bool ok = true;
                        for (std::size_t i = 0; i + 1 < k; ++i) {
                            int s = ss[i];
                            int fPick = -1;
                            if (i >= 1 && (!mask || mask->isAllowed(s, 0)) && chord.containsPitchClass(midiAt(s, 0) % 12)) {
                                fPick = 0;
                            }
                            if (fPick < 0) {
                                for (int f = anchorFret; f <= leftHi; ++f) {
                                    if (mask && !mask->isAllowed(s, f)) continue;
                                    if (chord.containsPitchClass(midiAt(s, f) % 12)) { fPick = f; break; }
                                }
                            }
                            if (fPick < 0) { ok = false; break; }
                            frets[static_cast<std::size_t>(s)] = fPick;
                        }
                        if (ok) {
                            int tapS = ss.back();
                            int tapF = -1;
                            for (int f = leftHi + 2; f <= std::min(fretCount_, leftHi + 9); ++f) {
                                if (mask && !mask->isAllowed(tapS, f)) continue;
                                if (chord.containsPitchClass(midiAt(tapS, f) % 12)) { tapF = f; break; }
                            }
                            if (tapF > 0) {
                                frets[static_cast<std::size_t>(tapS)] = tapF;
                                tapFlags[static_cast<std::size_t>(tapS)] = true;
                                recordVoicing(frets, tapFlags, true);
                            }
                        }
                    }

                    // Wariant C2: dolne struny w lewej ręce (nisko), najwyższa struna szeroko rozciągnięta i tapowana prawą ręką wyżej na gryfie
                    if (anchorFret >= 6) {
                        std::vector<int> frets(static_cast<std::size_t>(nStrings), -1);
                        std::vector<bool> tapFlags(static_cast<std::size_t>(nStrings), false);
                        int sHigh = ss.back();
                        int fHigh = -1;
                        for (int f = anchorFret + 4; f <= std::min(fretCount_, anchorFret + 10); ++f) {
                            if (mask && !mask->isAllowed(sHigh, f)) continue;
                            if (chord.containsPitchClass(midiAt(sHigh, f) % 12)) { fHigh = f; break; }
                        }
                        if (fHigh > 0) {
                            frets[static_cast<std::size_t>(sHigh)] = fHigh;
                            tapFlags[static_cast<std::size_t>(sHigh)] = true;
                            bool ok = true;
                            for (std::size_t i = 0; i + 1 < k; ++i) {
                                int s = ss[i];
                                int fLow = -1;
                                if (i == k - 2 && (!mask || mask->isAllowed(s, 0)) && chord.containsPitchClass(midiAt(s, 0) % 12)) {
                                    fLow = 0;
                                } else {
                                    for (int f = std::max(1, anchorFret - 3); f <= anchorFret; ++f) {
                                        if (mask && !mask->isAllowed(s, f)) continue;
                                        if (chord.containsPitchClass(midiAt(s, f) % 12)) { fLow = f; break; }
                                    }
                                }
                                if (fLow < 0) { ok = false; break; }
                                frets[static_cast<std::size_t>(s)] = fLow;
                            }
                            if (ok) recordVoicing(frets, tapFlags, true);
                        }
                    }
                }
            }
        }
    }

    auto minPosOf = [](const ChordVoicing& v) {
        int m = 99;
        for (int f : v.frets) if (f > 0 && f < m) m = f;
        return m == 99 ? 0 : m;
    };

    if (sharpness == VoicingSharpness::Mild) {
        voicings = !mildCompact.empty() ? std::move(mildCompact) : std::move(mildSpan4Fallback);
        std::stable_sort(voicings.begin(), voicings.end(), [&](const ChordVoicing& a, const ChordVoicing& b) {
            return minPosOf(a) < minPosOf(b);
        });
    } else if (sharpness == VoicingSharpness::Sharp) {
        std::stable_sort(sharpStretch.begin(), sharpStretch.end(), [&](const ChordVoicing& a, const ChordVoicing& b) {
            return minPosOf(a) < minPosOf(b);
        });
        std::stable_sort(sharpTapped.begin(), sharpTapped.end(), [&](const ChordVoicing& a, const ChordVoicing& b) {
            return minPosOf(a) < minPosOf(b);
        });
        // Przeplataj chwyty rozciągnięte (np. 8-5-3, 13-10-8) oraz tappingowane (T), jak na referencyjnym widoku „Ostre”
        std::size_t maxN = std::max(sharpStretch.size(), sharpTapped.size());
        for (std::size_t i = 0; i < maxN; ++i) {
            if (i < sharpTapped.size()) voicings.push_back(std::move(sharpTapped[i]));
            if (i < sharpStretch.size()) voicings.push_back(std::move(sharpStretch[i]));
        }
    } else {
        const bool usedSpan4Fallback = mildCompact.empty();
        auto& base = !usedSpan4Fallback ? mildCompact : mildSpan4Fallback;
        for (auto& v : base) voicings.push_back(std::move(v));
        for (auto& v : sharpStretch) {
            if (usedSpan4Fallback && v.fretSpan == 4) continue; // unika duplikacji po fallbacku span==4
            voicings.push_back(std::move(v));
        }
        if (includeTapping) {
            for (auto& v : sharpTapped) voicings.push_back(std::move(v));
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

    // Dokładne przesunięcia w półtonach dla szeregu harmonicznego n = 2..15:
    // n=2 (+12, P8), n=3 (+19, P5), n=4 (+24, P8), n=5 (+28, M3), n=6 (+31, P5),
    // n=7 (+34, m7), n=8 (+36, P8), n=9 (+38, M2), n=10 (+40, M3), n=11 (+42, A4/#4),
    // n=12 (+43, P5), n=13 (+44, m6/b6), n=14 (+46, m7/b7), n=15 (+47, M7)
    auto harmonicSemitoneOffset = [](int n) -> int {
        switch (n) {
            case 2:  return 12;
            case 3:  return 19;
            case 4:  return 24;
            case 5:  return 28;
            case 6:  return 31;
            case 7:  return 34;
            case 8:  return 36;
            case 9:  return 38;
            case 10: return 40;
            case 11: return 42;
            case 12: return 43;
            case 13: return 44;
            case 14: return 46;
            case 15: return 47;
            default: return static_cast<int>(std::llround(12.0 * std::log2(static_cast<double>(n))));
        }
    };

    // Klasyfikacja trudności zgodna ze zrzutami referencyjnymi:
    // Łatwy (zielony): n = 2..6 (6 klasycznych węzłów: progi 3.2, 3.9, 5, 7, 8.8, 12)
    // Średni (żółty):  n = 7..8
    // Trudny (pomar.): n = 9..11
    // Bardzo trudny:   n >= 12
    auto difficultyForN = [](int n) -> HarmonicDifficulty {
        if (n <= 6)  return HarmonicDifficulty::Easy;
        if (n <= 8)  return HarmonicDifficulty::Medium;
        if (n <= 11) return HarmonicDifficulty::Hard;
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
