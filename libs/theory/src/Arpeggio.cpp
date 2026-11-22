#include "gitara/theory/Arpeggio.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <set>

namespace gitara::theory {

namespace {

ArpeggioPositionNode makeArpNode(
    const Fretboard& fretboard,
    const Chord& chord,
    int s,
    int f,
    AccidentalMode mode) {
    int midi = fretboard.midiAt(s, f);
    int pc = midi % 12;
    Interval foundIv = intervals::P1;
    Note spelled = Note::fromPitchClass(pc, mode);

    if (chord.definition) {
        for (const auto& iv : chord.definition->intervals) {
            if ((chord.root.pitchClass() + iv.semitones) % 12 == pc) {
                foundIv = iv;
                spelled = iv.applyTo(chord.root).withAccidentalMode(mode);
                break;
            }
        }
    }

    ArpeggioPositionNode node{};
    node.position = FretPosition{s, f};
    node.note = spelled;
    node.intervalFromRoot = foundIv;
    node.isRoot = (pc == chord.root.pitchClass());
    return node;
}

ArpeggioStep makeStepFromNode(
    const Fretboard& fretboard,
    const ArpeggioPositionNode& node,
    const std::string& technique,
    bool isRoll,
    int segmentIndex,
    AccidentalMode mode) {
    ArpeggioStep st{};
    st.stringIndex = node.position.stringIndex;
    st.fret = node.position.fret;
    st.midi = fretboard.midiAt(st.stringIndex, st.fret);
    st.frequencyHz = fretboard.pitchAt(st.stringIndex, st.fret, mode).frequencyHz();
    st.noteName = node.note.toString(false);
    st.interval = node.intervalFromRoot.symbol(true);
    st.isRoot = node.isRoot;
    st.technique = technique;
    st.isRoll = isRoll;
    st.segmentIndex = segmentIndex;
    return st;
}

std::vector<ArpeggioStep> buildSweepStepsFromNodes(
    const Fretboard& fretboard,
    const std::vector<ArpeggioPositionNode>& nodes,
    int direction,
    int segmentIndex,
    AccidentalMode mode,
    bool& outHasRoll) {
    outHasRoll = false;
    if (nodes.empty()) return {};

    // Grupuj po strunach (rosnąco od najgrubszej do najcieńszej) i progach rosnąco
    std::vector<ArpeggioPositionNode> sorted = nodes;
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        if (a.position.stringIndex != b.position.stringIndex)
            return a.position.stringIndex < b.position.stringIndex;
        return a.position.fret < b.position.fret;
    });

    std::vector<ArpeggioStep> asc;
    for (std::size_t i = 0; i < sorted.size(); ++i) {
        std::string tech = "D";
        bool roll = false;
        if (i > 0) {
            const auto& prev = sorted[i - 1];
            if (prev.position.stringIndex == sorted[i].position.stringIndex) {
                tech = "H"; // Hammer-on na tej samej strunie (np. zwrot na strunie E1 lub baza E6)
            } else if (prev.position.fret > 0 && prev.position.fret == sorted[i].position.fret &&
                       sorted[i].position.stringIndex == prev.position.stringIndex + 1) {
                roll = true;
                outHasRoll = true;
            }
        }
        asc.push_back(makeStepFromNode(fretboard, sorted[i], tech, roll, segmentIndex, mode));
    }

    std::vector<ArpeggioStep> desc;
    for (int idx = static_cast<int>(sorted.size()) - 1; idx >= 0; --idx) {
        std::size_t i = static_cast<std::size_t>(idx);
        std::string tech = "U";
        bool roll = false;
        if (idx < static_cast<int>(sorted.size()) - 1) {
            const auto& prev = sorted[static_cast<std::size_t>(idx + 1)];
            if (prev.position.stringIndex == sorted[i].position.stringIndex) {
                tech = "P"; // Pull-off w dół na tej samej strunie
            } else if (prev.position.fret > 0 && prev.position.fret == sorted[i].position.fret &&
                       prev.position.stringIndex == sorted[i].position.stringIndex + 1) {
                roll = true;
                outHasRoll = true;
            }
        }
        desc.push_back(makeStepFromNode(fretboard, sorted[i], tech, roll, segmentIndex, mode));
    }

    if (direction == 1) return asc;
    if (direction == 2) return desc;

    // W górę i w dół bez powtarzania najwyższego ani najniższego dźwięku zwrotnego
    std::vector<ArpeggioStep> full = asc;
    for (std::size_t i = 1; i + 1 < desc.size(); ++i) {
        full.push_back(desc[i]);
    }
    return full;
}

} // namespace

std::vector<SweepShape> generateSweepShapes(
    const Fretboard& fretboard,
    const Chord& chord,
    int stringSpan,
    int direction,
    AccidentalMode mode,
    const FretboardMask* mask) {
    std::vector<SweepShape> shapes;
    if (!chord.definition || fretboard.stringCount() < 3) {
        return shapes;
    }

    const int nStrings = fretboard.stringCount();
    const int nFrets = fretboard.fretCount();
    const int span = (stringSpan == 6 && nStrings >= 6) ? 6
                   : (stringSpan == 5 && nStrings >= 5) ? 5
                   : 3;

    // Zestaw kolejnych strun dla sweepu (zawsze kończy się na najcieńszej strunie wiolinowej)
    const int startString = nStrings - span; // 3-str: G-B-e (3..5), 5-str: A-D-G-B-e (1..5), 6-str: E-A-D-G-B-e (0..5)
    const int topString = nStrings - 1;

    const auto chordNotes = chord.notes(mode);
    const int reqCount = static_cast<int>(chordNotes.size());
    int essentialMask = 0;
    for (int i = 0; i < reqCount; ++i) {
        bool isOpt = (reqCount > 3) &&
            std::find(chord.definition->optionalIndices.begin(),
                      chord.definition->optionalIndices.end(), i) != chord.definition->optionalIndices.end();
        if (!isOpt) {
            essentialMask |= (1 << chordNotes[static_cast<std::size_t>(i)].pitchClass());
        }
    }

    // Zbieramy kandydatów na najwyższej strunie (topString): każda para (fTop1, fTop2) gdzie fTop1 < fTop2
    // to klasyczny zwrot hammer-on / pull-off na strunie E1!
    std::vector<int> topFrets;
    for (int f = 1; f <= nFrets; ++f) {
        if (mask && !mask->isAllowed(topString, f)) continue;
        if (chord.containsPitchClass(fretboard.midiAt(topString, f) % 12)) {
            topFrets.push_back(f);
        }
    }

    auto signatureOf = [](const std::vector<ArpeggioPositionNode>& ns) {
        std::vector<std::pair<int, int>> sig;
        sig.reserve(ns.size());
        for (const auto& n : ns) sig.emplace_back(n.position.stringIndex, n.position.fret);
        std::sort(sig.begin(), sig.end());
        return sig;
    };
    std::set<std::vector<std::pair<int, int>>> seen;

    auto countSkippedChordTones = [&](int lowMidi, int highMidi) {
        int skipped = 0;
        for (int mm = lowMidi + 1; mm < highMidi; ++mm) {
            if (chord.containsPitchClass(mm % 12)) ++skipped;
        }
        return skipped;
    };

    const int minDistinct = (reqCount <= 2) ? 2 : 3;
    const int maxBodySpan = (span == 3) ? 4 : 5;

    for (std::size_t ti = 0; ti + 1 < topFrets.size(); ++ti) {
        int fTop1 = topFrets[ti];
        int fTop2 = topFrets[ti + 1];
        if (fTop2 - fTop1 < 2 || fTop2 - fTop1 > 6) continue;
        if (fTop1 > 19) continue;

        // Okno pozycji lewej dłoni wokół fTop1 (rozszerzone na tyle, aby pomieścić kanoniczne kształty bez pominiętych składników)
        const int boxMin = std::max(1, fTop1 - (span == 3 ? 2 : 3));
        const int boxMax = std::min(nFrets, fTop1 + (span == 3 ? 4 : 5));

        std::vector<std::vector<int>> candFrets(static_cast<std::size_t>(nStrings));
        bool allStringsHaveCand = true;
        for (int s = startString; s < topString; ++s) {
            for (int f = boxMin; f <= boxMax; ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                int m = fretboard.midiAt(s, f);
                if (chord.containsPitchClass(m % 12)) {
                    candFrets[static_cast<std::size_t>(s)].push_back(f);
                }
            }
            if (candFrets[static_cast<std::size_t>(s)].empty()) {
                allStringsHaveCand = false;
                break;
            }
        }
        if (!allStringsHaveCand) continue;

        std::vector<int> curFrets(static_cast<std::size_t>(nStrings), -1);
        std::vector<int> bestFrets(static_cast<std::size_t>(nStrings), -1);
        curFrets[static_cast<std::size_t>(topString)] = fTop1;
        bool foundBest = false;
        int bestScore = std::numeric_limits<int>::min();
        int bestPcMask = 0;

        const int top1Midi = fretboard.midiAt(topString, fTop1);
        const int top2Midi = fretboard.midiAt(topString, fTop2);
        const int initPcMask = (1 << (top1Midi % 12)) | (1 << (top2Midi % 12));

        // Dokładne przeszukiwanie po strunach topString-1 .. startString zamiast zachłannego wyboru na jednej strunie:
        // premiuje kolejne składniki akordu (zero przeskoków), brak zbędnych przetoczeń palca i zwartą pozycję dłoni.
        auto dfs = [&](auto& self, int s, int prevMidi, int prevFret, int pcMask,
                       int bodyMinF, int bodyMaxF, int accSkipped, int accRolls, int accDist) -> void {
            if (s < startString) {
                int distinctCount = 0;
                for (int b = 0; b < 12; ++b) {
                    if (pcMask & (1 << b)) ++distinctCount;
                }
                if (distinctCount < minDistinct) return;

                if (reqCount >= 4 && span >= 5) {
                    int missing = 0;
                    for (int b = 0; b < 12; ++b) {
                        if ((essentialMask & (1 << b)) && !(pcMask & (1 << b))) ++missing;
                    }
                    if (missing > 1) return;
                }

                const int skipPenalty = (reqCount <= 3) ? 140 : 60;
                int score = 0;
                score -= accSkipped * skipPenalty;
                score += distinctCount * 35;
                if (reqCount >= 4 && (pcMask & essentialMask) == essentialMask) score += 170;
                score -= accRolls * 18;
                const int bodySpan = bodyMaxF - bodyMinF;
                score -= bodySpan * 14;
                if (bodySpan > 4) score -= (bodySpan - 4) * 20;
                score -= accDist * 5;
                const int lowPc = fretboard.midiAt(startString, curFrets[static_cast<std::size_t>(startString)]) % 12;
                if (span >= 5 && lowPc == chord.root.pitchClass()) score += 26;

                if (!foundBest || score > bestScore) {
                    foundBest = true;
                    bestScore = score;
                    bestFrets = curFrets;
                    bestPcMask = pcMask;
                }
                return;
            }

            for (int f : candFrets[static_cast<std::size_t>(s)]) {
                int m = fretboard.midiAt(s, f);
                if (m >= prevMidi) continue;
                int nMin = std::min(bodyMinF, f);
                int nMax = std::max(bodyMaxF, f);
                if (nMax - nMin > maxBodySpan) continue;
                if (std::max(nMax, fTop2) - nMin > 7) continue;

                int skipped = countSkippedChordTones(m, prevMidi);
                int roll = (f == prevFret) ? 1 : 0;
                curFrets[static_cast<std::size_t>(s)] = f;
                self(self, s - 1, m, f, pcMask | (1 << (m % 12)),
                     nMin, nMax, accSkipped + skipped, accRolls + roll, accDist + std::abs(f - fTop1));
            }
        };

        dfs(dfs, topString - 1, top1Midi, fTop1, initPcMask, fTop1, fTop1, 0, 0, 0);
        if (!foundBest) continue;

        std::vector<ArpeggioPositionNode> picked;
        picked.push_back(makeArpNode(fretboard, chord, topString, fTop2, mode));
        picked.push_back(makeArpNode(fretboard, chord, topString, fTop1, mode));
        for (int s = topString - 1; s >= startString; --s) {
            picked.push_back(makeArpNode(fretboard, chord, s, bestFrets[static_cast<std::size_t>(s)], mode));
        }
        int pcMask = bestPcMask;

        // Dla 6-strunowego sweepu sprawdź opcjonalny 2. dźwięk (hammer-on) na najgrubszej strunie E6
        // — najpierw poniżej lowFret (np. 8-12 przy kształcie A-moll), a jeśli brak, powyżej lowFret (np. 5-8 przy E-moll).
        if (span == 6) {
            int lowFret = bestFrets[static_cast<std::size_t>(startString)];
            int lowMidi = fretboard.midiAt(startString, lowFret);
            int nextStrMidi = fretboard.midiAt(startString + 1, bestFrets[static_cast<std::size_t>(startString + 1)]);

            int curMnF = 99, curMxF = 0;
            for (const auto& n : picked) {
                curMnF = std::min(curMnF, n.position.fret);
                curMxF = std::max(curMxF, n.position.fret);
            }

            bool addedSixthExtra = false;
            for (int f0 = lowFret - 2; f0 >= std::max(1, lowFret - 5); --f0) {
                if (mask && !mask->isAllowed(startString, f0)) continue;
                int m0 = fretboard.midiAt(startString, f0);
                if (!chord.containsPitchClass(m0 % 12)) continue;
                if (countSkippedChordTones(m0, lowMidi) != 0) continue;
                const bool stretchOk = (lowFret - f0 <= 4) || (f0 >= 7 && lowFret - f0 <= 5);
                if (!stretchOk) continue;
                const int spanLimit = (f0 >= fTop1) ? 7 : 6;
                if (std::max(curMxF, lowFret) - std::min(curMnF, f0) > spanLimit) continue;

                picked.push_back(makeArpNode(fretboard, chord, startString, f0, mode));
                pcMask |= (1 << (m0 % 12));
                addedSixthExtra = true;
                break;
            }

            if (!addedSixthExtra) {
                for (int f2 = lowFret + 2; f2 <= std::min(nFrets, lowFret + 5); ++f2) {
                    if (mask && !mask->isAllowed(startString, f2)) continue;
                    int m2 = fretboard.midiAt(startString, f2);
                    if (m2 > lowMidi && m2 < nextStrMidi && chord.containsPitchClass(m2 % 12) &&
                        countSkippedChordTones(lowMidi, m2) == 0 &&
                        countSkippedChordTones(m2, nextStrMidi) == 0 &&
                        std::max(curMxF, f2) - std::min(curMnF, lowFret) <= 7) {
                        picked.push_back(makeArpNode(fretboard, chord, startString, f2, mode));
                        pcMask |= (1 << (m2 % 12));
                        break;
                    }
                }
            }
        }

        // Wymagaj przynajmniej 3 różnych składników akordu (dla trójdźwięku: wszystkie 3; dla power chord: 2)
        int distinctCount = 0;
        for (int b = 0; b < 12; ++b) if (pcMask & (1 << b)) ++distinctCount;
        if (distinctCount < minDistinct) continue;
        if (reqCount >= 4 && span >= 5 && (pcMask & essentialMask) != essentialMask) {
            int missing = 0;
            for (int b = 0; b < 12; ++b) {
                if ((essentialMask & (1 << b)) && !(pcMask & (1 << b))) ++missing;
            }
            if (missing > 1) continue;
        }

        int mnF = 99, mxF = 0;
        for (const auto& n : picked) {
            mnF = std::min(mnF, n.position.fret);
            mxF = std::max(mxF, n.position.fret);
        }
        // Ogranicz całkowitą rozpiętość kształtu sweep (z włączeniem zwrotu H/P na E1)
        if (mxF - mnF > 7) continue;

        auto sig = signatureOf(picked);
        if (!seen.insert(sig).second) continue;

        std::sort(picked.begin(), picked.end(), [](const auto& a, const auto& b) {
            if (a.position.stringIndex != b.position.stringIndex)
                return a.position.stringIndex < b.position.stringIndex;
            return a.position.fret < b.position.fret;
        });

        SweepShape sh{};
        sh.stringSpan = span;
        sh.minFret = mnF;
        sh.maxFret = mxF;
        sh.nodes = std::move(picked);
        sh.steps = buildSweepStepsFromNodes(fretboard, sh.nodes, direction, 0, mode, sh.hasFingerRoll);
        shapes.push_back(std::move(sh));
    }

    // Fallback gdy akord z szerokimi interwałami nie znalazł zwrotu H/P (np. w niestandardowym stroju)
    if (shapes.empty()) {
        for (int anchor = 1; anchor <= std::min(nFrets, 16); anchor += 3) {
            std::vector<ArpeggioPositionNode> picked;
            int prevMidi = 0;
            for (int s = startString; s < nStrings; ++s) {
                for (int f = anchor; f <= std::min(nFrets, anchor + 5); ++f) {
                    if (mask && !mask->isAllowed(s, f)) continue;
                    int m = fretboard.midiAt(s, f);
                    if (m > prevMidi && chord.containsPitchClass(m % 12)) {
                        picked.push_back(makeArpNode(fretboard, chord, s, f, mode));
                        prevMidi = m;
                        break;
                    }
                }
            }
            if (static_cast<int>(picked.size()) >= 3) {
                auto sig = signatureOf(picked);
                if (seen.insert(sig).second) {
                    int mnF = 99, mxF = 0;
                    for (const auto& n : picked) { mnF = std::min(mnF, n.position.fret); mxF = std::max(mxF, n.position.fret); }
                    SweepShape sh{};
                    sh.stringSpan = span;
                    sh.minFret = mnF;
                    sh.maxFret = mxF;
                    sh.nodes = std::move(picked);
                    sh.steps = buildSweepStepsFromNodes(fretboard, sh.nodes, direction, 0, mode, sh.hasFingerRoll);
                    shapes.push_back(std::move(sh));
                }
            }
        }
    }

    std::sort(shapes.begin(), shapes.end(), [](const SweepShape& a, const SweepShape& b) {
        if (a.minFret != b.minFret) return a.minFret < b.minFret;
        return a.maxFret < b.maxFret;
    });

    for (std::size_t i = 0; i < shapes.size(); ++i) {
        shapes[i].positionIndex = static_cast<int>(i);
        std::string startTag;
        if (!shapes[i].nodes.empty()) {
            const auto& fn = shapes[i].nodes.front();
            startTag = " · od " + fn.note.toString(false) + " (" + fn.intervalFromRoot.symbol(true) + ")";
        }
        shapes[i].label = "Poz. " + std::to_string(i + 1) + " · pr. " +
                          std::to_string(shapes[i].minFret) + "–" + std::to_string(shapes[i].maxFret) + startTag;
    }

    return shapes;
}

std::vector<ArpeggioPositionNode> generateArpeggio(
    const Fretboard& fretboard,
    const Chord& chord,
    ArpeggioPattern pattern,
    int positionIndex,
    AccidentalMode mode,
    const FretboardMask* mask) {
    std::vector<ArpeggioPositionNode> nodes;
    if (!chord.definition) {
        return nodes;
    }

    const int nStrings = fretboard.stringCount();
    const int nFrets = fretboard.fretCount();

    auto dedupUnisons = [&](std::vector<ArpeggioPositionNode> raw, double idealCenterFret) {
        std::sort(raw.begin(), raw.end(), [&](const auto& a, const auto& b) {
            int ma = fretboard.midiAt(a.position.stringIndex, a.position.fret);
            int mb = fretboard.midiAt(b.position.stringIndex, b.position.fret);
            if (ma != mb) return ma < mb;
            return std::abs(a.position.fret - idealCenterFret) < std::abs(b.position.fret - idealCenterFret);
        });
        std::vector<ArpeggioPositionNode> out;
        int lastMidi = -1;
        for (const auto& n : raw) {
            int m = fretboard.midiAt(n.position.stringIndex, n.position.fret);
            if (m == lastMidi) continue;
            out.push_back(n);
            lastMidi = m;
        }
        return out;
    };

    if (pattern == ArpeggioPattern::FullNeck) {
        for (int s = 0; s < nStrings; ++s) {
            for (int f = 0; f <= nFrets; ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                int midi = fretboard.midiAt(s, f);
                if (chord.containsPitchClass(midi % 12)) {
                    nodes.push_back(makeArpNode(fretboard, chord, s, f, mode));
                }
            }
        }
        return nodes;
    }

    if (pattern == ArpeggioPattern::RootOctave) {
        return generateRootOctaveArpeggio(fretboard, chord, positionIndex, 2, mode, mask);
    }

    if (pattern == ArpeggioPattern::Inversions) {
        auto invs = generateArpeggioInversions(fretboard, chord, 0, mode, mask);
        if (!invs.empty()) {
            int idx = ((positionIndex % static_cast<int>(invs.size())) + static_cast<int>(invs.size())) %
                      static_cast<int>(invs.size());
            return invs[static_cast<std::size_t>(idx)].nodes;
        }
        return nodes;
    }

    if (pattern == ArpeggioPattern::CAGED) {
        // 5 autentycznych kształtów CAGED: 0=C, 1=A, 2=G, 3=E, 4=D zakotwiczonych na realnych progach prymy!
        int rootPc = chord.root.pitchClass();
        int rE = (rootPc - (fretboard.midiAt(0, 0) % 12) + 12) % 12;
        int rA = (nStrings > 1) ? ((rootPc - (fretboard.midiAt(1, 0) % 12) + 12) % 12) : rE;
        int rD = (nStrings > 2) ? ((rootPc - (fretboard.midiAt(2, 0) % 12) + 12) % 12) : rE;

        int box = ((positionIndex % 5) + 5) % 5;
        int minFret = 0, maxFret = 4;
        if (box == 0) { // Kształt C (pryma na strunie A -> boks [rA-3 .. rA+1])
            int anchor = (rA < 3) ? (rA + 12) : rA;
            minFret = std::max(0, anchor - 3);
            maxFret = std::min(nFrets, anchor + 1);
        } else if (box == 1) { // Kształt A (pryma na strunie A -> boks [rA .. rA+4])
            int anchor = (rA == 0) ? 12 : rA;
            minFret = anchor;
            maxFret = std::min(nFrets, anchor + 4);
        } else if (box == 2) { // Kształt G (pryma na strunie E -> boks [rE-3 .. rE+1])
            int anchor = (rE < 3) ? (rE + 12) : rE;
            minFret = std::max(0, anchor - 3);
            maxFret = std::min(nFrets, anchor + 1);
        } else if (box == 3) { // Kształt E (pryma na strunie E -> boks [rE-1 .. rE+3])
            int anchor = (rE == 0) ? 12 : rE;
            minFret = std::max(0, anchor - 1);
            maxFret = std::min(nFrets, anchor + 3);
        } else { // Kształt D (pryma na strunie D -> boks [rD .. rD+4])
            int anchor = (rD == 0) ? 12 : rD;
            minFret = anchor;
            maxFret = std::min(nFrets, anchor + 4);
        }

        std::vector<ArpeggioPositionNode> raw;
        for (int s = 0; s < nStrings; ++s) {
            for (int f = minFret; f <= maxFret; ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                if (chord.containsPitchClass(fretboard.midiAt(s, f) % 12)) {
                    raw.push_back(makeArpNode(fretboard, chord, s, f, mode));
                }
            }
        }
        return dedupUnisons(std::move(raw), 0.5 * (minFret + maxFret));
    }

    if (pattern == ArpeggioPattern::TwoNPS || pattern == ArpeggioPattern::ThreeNPS) {
        const int targetPerString = (pattern == ArpeggioPattern::TwoNPS) ? 2 : 3;
        const int maxPerStringSpan = (pattern == ArpeggioPattern::TwoNPS) ? 5 : 7;
        const auto chordNotes = chord.notes(mode);
        if (chordNotes.empty()) return nodes;

        int startIdx = ((positionIndex % static_cast<int>(chordNotes.size())) +
                        static_cast<int>(chordNotes.size())) %
                       static_cast<int>(chordNotes.size());
        int startPc = chordNotes[static_cast<std::size_t>(startIdx)].pitchClass();
        int openLowPc = fretboard.midiAt(0, 0) % 12;
        int firstFret = (startPc - openLowPc + 12) % 12;
        // Trzymaj pozycję startową na niskich/średnich progach, żeby arpeggio zmieściło się na wszystkich 6 strunach!
        if (firstFret > 9) firstFret = std::max(0, firstFret - 12);
        int minMidi = fretboard.midiAt(0, firstFret);

        for (int s = 0; s < nStrings; ++s) {
            int count = 0;
            int firstOnStr = -1;
            for (int f = 0; f <= nFrets; ++f) {
                int m = fretboard.midiAt(s, f);
                if (m >= minMidi && chord.containsPitchClass(m % 12)) {
                    if (firstOnStr >= 0 && (f - firstOnStr) > maxPerStringSpan) break;
                    if (!mask || mask->isAllowed(s, f)) {
                        nodes.push_back(makeArpNode(fretboard, chord, s, f, mode));
                    }
                    if (firstOnStr < 0) firstOnStr = f;
                    minMidi = m + 1;
                    ++count;
                    if (count >= targetPerString) break;
                }
            }
        }
        return nodes;
    }

    // ArpeggioPattern::Sweep — wykorzystuje nowy generator kształtów Sweep (domyślnie 5-strunowy z pełnym zakresem)
    auto shapes = generateSweepShapes(fretboard, chord, 5, 0, mode, mask);
    if (shapes.empty()) {
        shapes = generateSweepShapes(fretboard, chord, 3, 0, mode, mask);
    }
    if (!shapes.empty()) {
        int idx = ((positionIndex % static_cast<int>(shapes.size())) + static_cast<int>(shapes.size())) %
                  static_cast<int>(shapes.size());
        return shapes[static_cast<std::size_t>(idx)].nodes;
    }
    return nodes;
}

std::vector<ArpeggioInversionBox> generateArpeggioInversions(
    const Fretboard& fretboard,
    const Chord& chord,
    int stringSetIndex,
    AccidentalMode mode,
    const FretboardMask* mask) {
    std::vector<ArpeggioInversionBox> out;
    if (!chord.definition || fretboard.stringCount() < 3) return out;

    const int nStrings = fretboard.stringCount();
    const int nFrets = fretboard.fretCount();
    const auto chordNotes = chord.notes(mode);
    const int reqCount = static_cast<int>(chordNotes.size());
    const int setSize = (reqCount >= 4 && nStrings >= 4) ? 4 : 3;

    // 0 = najwyższe (G-B-e lub D-G-B-e), 1 = środkowe (D-G-B lub A-D-G-B), 2 = niższe (A-D-G lub E-A-D-G)
    int maxStart = nStrings - setSize;
    int s0 = std::clamp(maxStart - std::clamp(stringSetIndex, 0, 2), 0, maxStart);

    int essentialMask = 0;
    for (int i = 0; i < reqCount; ++i) {
        bool isOpt = (reqCount > setSize) &&
            std::find(chord.definition->optionalIndices.begin(),
                      chord.definition->optionalIndices.end(), i) != chord.definition->optionalIndices.end();
        if (!isOpt) essentialMask |= (1 << chordNotes[static_cast<std::size_t>(i)].pitchClass());
    }

    std::set<std::vector<int>> seenFrets;
    for (int fBase = 0; fBase <= std::min(nFrets, 19); ++fBase) {
        if (mask && !mask->isAllowed(s0, fBase)) continue;
        int baseMidi = fretboard.midiAt(s0, fBase);
        if (!chord.containsPitchClass(baseMidi % 12)) continue;

        int lo = std::max(0, fBase - 3);
        int hi = std::min(nFrets, fBase + 4);
        std::vector<ArpeggioPositionNode> picked;
        picked.push_back(makeArpNode(fretboard, chord, s0, fBase, mode));
        int prevMidi = baseMidi;
        int pcMask = (1 << (baseMidi % 12));
        bool ok = true;

        for (int k = 1; k < setSize; ++k) {
            int s = s0 + k;
            int bestF = -1;
            int bestScore = -9999;
            for (int f = lo; f <= hi; ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                int m = fretboard.midiAt(s, f);
                if (m <= prevMidi) continue;
                int pc = m % 12;
                if (!chord.containsPitchClass(pc)) continue;
                int sc = 0;
                if ((pcMask & (1 << pc)) == 0) sc += 100;
                if ((essentialMask & (1 << pc)) && !(pcMask & (1 << pc))) sc += 60;
                sc -= std::abs(f - fBase) * 10;
                if (sc > bestScore) { bestScore = sc; bestF = f; }
            }
            if (bestF < 0) { ok = false; break; }
            picked.push_back(makeArpNode(fretboard, chord, s, bestF, mode));
            prevMidi = fretboard.midiAt(s, bestF);
            pcMask |= (1 << (prevMidi % 12));
        }
        if (!ok) continue;

        int distinct = 0;
        for (int b = 0; b < 12; ++b) if (pcMask & (1 << b)) ++distinct;
        int minDist = (reqCount <= 2) ? 2 : std::min(setSize, reqCount);
        if (distinct < minDist) continue;

        int mnF = 99, mxF = 0;
        std::vector<int> sig;
        for (const auto& n : picked) {
            sig.push_back(n.position.fret);
            if (n.position.fret > 0) { mnF = std::min(mnF, n.position.fret); mxF = std::max(mxF, n.position.fret); }
        }
        if (mnF <= mxF && (mxF - mnF) > 4) continue;
        if (!seenFrets.insert(sig).second) continue;
        if (mnF > mxF) mnF = 0;

        std::string bassIv = picked.front().intervalFromRoot.symbol(true);
        std::string invName = "Przewrót (" + bassIv + " w basie)";
        if (picked.front().isRoot || bassIv == "1") invName = "Postać zasadnicza (1)";
        else if (bassIv == "3" || bassIv == "♭3" || bassIv == "b3") invName = "1. przewrót (" + bassIv + ")";
        else if (bassIv == "5" || bassIv == "♭5" || bassIv == "♯5") invName = "2. przewrót (" + bassIv + ")";
        else invName = "3. przewrót (" + bassIv + ")";

        ArpeggioInversionBox box{};
        box.inversionIndex = static_cast<int>(out.size());
        box.minFret = mnF;
        box.maxFret = mxF;
        box.label = invName + " · pr. " + std::to_string(mnF) + "–" + std::to_string(mxF);
        box.nodes = std::move(picked);
        out.push_back(std::move(box));
    }
    return out;
}

std::vector<ArpeggioPositionNode> generateRootOctaveArpeggio(
    const Fretboard& fretboard,
    const Chord& chord,
    int positionIndex,
    int octaves,
    AccidentalMode mode,
    const FretboardMask* mask) {
    std::vector<ArpeggioPositionNode> out;
    if (!chord.definition || fretboard.stringCount() < 4) return out;

    const int nStrings = fretboard.stringCount();
    const int nFrets = fretboard.fretCount();
    const int rootPc = chord.root.pitchClass();
    const int numOct = (octaves == 1) ? 1 : 2;

    // 4 ergonomiczne kotwice startowe od prymy:
    // 0 = pryma na strunie E6 (pozycja otwarta/niska), 1 = pryma na strunie A5,
    // 2 = pryma na strunie D4 (1 okt. na wiolinowych), 3 = pryma na strunie E6 (wyżej / ok. 12. progu)
    struct Anchor { int str; int fret; int boxShift; };
    int rE = (rootPc - (fretboard.midiAt(0, 0) % 12) + 12) % 12;
    int rA = (rootPc - (fretboard.midiAt(1, 0) % 12) + 12) % 12;
    int rD = (rootPc - (fretboard.midiAt(2, 0) % 12) + 12) % 12;
    std::vector<Anchor> anchors = {
        {0, rE, 0},
        {1, rA, 0},
        {2, rD, 0},
        {0, (rE <= 7 ? rE + 12 : rE), -2}
    };
    int idx = ((positionIndex % static_cast<int>(anchors.size())) + static_cast<int>(anchors.size())) %
              static_cast<int>(anchors.size());
    const auto& anc = anchors[static_cast<std::size_t>(idx)];

    int startMidi = fretboard.midiAt(anc.str, anc.fret);
    int endMidi = startMidi + 12 * ((anc.str >= 2 && numOct == 2) ? 1 : numOct);
    int boxMin = std::max(0, anc.fret + anc.boxShift - 1);
    int boxMax = std::min(nFrets, boxMin + 4);
    if (anc.fret > boxMax) { boxMax = anc.fret; boxMin = std::max(0, boxMax - 4); }
    double centerFret = 0.5 * (boxMin + boxMax);

    // Zbierz dokładnie dźwięki akordu z przedziału [startMidi .. endMidi], każdy unikalny MIDI najbliżej środka boksu!
    std::vector<int> targetMidis;
    for (int m = startMidi; m <= endMidi; ++m) {
        if (chord.containsPitchClass(m % 12)) targetMidis.push_back(m);
    }

    int lastStr = anc.str;
    for (int m : targetMidis) {
        int bestS = -1, bestF = -1;
        double bestCost = 1e9;
        for (int s = lastStr; s < nStrings; ++s) {
            int f = m - fretboard.midiAt(s, 0);
            if (f < 0 || f > nFrets) continue;
            if (mask && !mask->isAllowed(s, f)) continue;
            double cost = std::abs(f - centerFret);
            if (f < boxMin || f > boxMax) cost += 8.0 + std::min(std::abs(f - boxMin), std::abs(f - boxMax)) * 3.0;
            if (s > lastStr + 1) cost += 4.0;
            if (m == startMidi && s == anc.str && f == anc.fret) cost = -100.0;
            if (cost < bestCost) { bestCost = cost; bestS = s; bestF = f; }
        }
        if (bestS >= 0) {
            out.push_back(makeArpNode(fretboard, chord, bestS, bestF, mode));
            lastStr = bestS;
        }
    }
    return out;
}

std::vector<ArpeggioStep> buildArpeggioSequenceSteps(
    const Fretboard& fretboard,
    const std::vector<ArpeggioPositionNode>& nodes,
    ArpeggioPattern pattern,
    int direction,
    AccidentalMode mode,
    int seqStyle) {
    if (nodes.empty()) return {};

    if (pattern == ArpeggioPattern::Sweep && seqStyle == 0) {
        bool roll = false;
        return buildSweepStepsFromNodes(fretboard, nodes, direction, 0, mode, roll);
    }

    // Sortuj rosnąco po wysokości MIDI i ZAWSZE usuń zdublowane unisony (żeby nigdy nie grało dwa razy tej samej nuty)
    std::vector<ArpeggioPositionNode> sorted = nodes;
    std::sort(sorted.begin(), sorted.end(), [&](const auto& a, const auto& b) {
        int ma = fretboard.midiAt(a.position.stringIndex, a.position.fret);
        int mb = fretboard.midiAt(b.position.stringIndex, b.position.fret);
        if (ma != mb) return ma < mb;
        return a.position.stringIndex < b.position.stringIndex;
    });

    std::vector<ArpeggioPositionNode> clean;
    int lastMidi = -1;
    for (const auto& n : sorted) {
        int m = fretboard.midiAt(n.position.stringIndex, n.position.fret);
        if (m == lastMidi) continue;
        clean.push_back(n);
        lastMidi = m;
        if (pattern == ArpeggioPattern::FullNeck && clean.size() >= 14) break;
    }

    auto applySeqOrder = [&](const std::vector<ArpeggioPositionNode>& base) {
        std::vector<ArpeggioPositionNode> ordered;
        if (seqStyle == 1 && base.size() >= 3) {
            // Grupy po 3 (triole / sekwencje 3-nutowe: 0-1-2, 1-2-3, 2-3-4...)
            for (std::size_t i = 0; i + 2 < base.size(); ++i) {
                ordered.push_back(base[i]);
                ordered.push_back(base[i + 1]);
                ordered.push_back(base[i + 2]);
            }
        } else if (seqStyle == 2 && base.size() >= 3) {
            // Skoki interwałowe (1-5-3-7 / przeskoki o 2 składniki)
            for (std::size_t i = 0; i + 2 < base.size(); ++i) {
                ordered.push_back(base[i]);
                ordered.push_back(base[i + 2]);
            }
            ordered.push_back(base.back());
        } else {
            ordered = base;
        }
        return ordered;
    };

    auto ascNodes = applySeqOrder(clean);
    std::vector<ArpeggioPositionNode> revClean(clean.rbegin(), clean.rend());
    auto descNodes = applySeqOrder(revClean);

    auto toSteps = [&](const std::vector<ArpeggioPositionNode>& seq, bool descending) {
        std::vector<ArpeggioStep> out;
        for (std::size_t i = 0; i < seq.size(); ++i) {
            std::string tech = descending ? ((i % 2 == 0) ? "U" : "D") : ((i % 2 == 0) ? "D" : "U");
            if (pattern == ArpeggioPattern::ThreeNPS && i > 0 &&
                seq[i].position.stringIndex == seq[i - 1].position.stringIndex) {
                tech = (seq[i].position.fret > seq[i - 1].position.fret) ? "H" : "P";
            }
            out.push_back(makeStepFromNode(fretboard, seq[i], tech, false, 0, mode));
        }
        return out;
    };

    auto asc = toSteps(ascNodes, false);
    auto desc = toSteps(descNodes, true);

    if (direction == 1) return asc;
    if (direction == 2) return desc;
    std::vector<ArpeggioStep> full = asc;
    std::size_t startDesc = (!full.empty() && !desc.empty() && full.back().midi == desc.front().midi) ? 1 : 0;
    for (std::size_t i = startDesc; i < desc.size(); ++i) {
        if (i + 1 == desc.size() && !full.empty() && desc[i].midi == full.front().midi && seqStyle == 0) continue;
        full.push_back(desc[i]);
    }
    return full;
}

const std::vector<SweepTransitionPresetInfo>& allSweepTransitionPresets() {
    static const std::vector<SweepTransitionPresetInfo> presets = {
        {
            "neoclassical_minor",
            "Neoklasyczna kadencja (i → ♭VI → iv → V)",
            "Neoklasyka",
            "Klasyczne przejścia Yngwie / Bach w jednej strefie gryfu z minimalnym ruchem lewej dłoni (np. Am → F → Dm → E)."
        },
        {
            "dim7_climb",
            "Wspinaczka dim7 co 3 progi (i → vii°7 ↗ → i)",
            "Shred / Neoklasyka",
            "Symetryczne arpeggia zmniejszone przesuwane co tercję małą (+3 progi) ze slidem na strunie E1 i rozwiązaniem oktawę wyżej."
        },
        {
            "same_chord_inversions",
            "Inwersje jednego akordu po gryfie (Poz. 1 ↗ 2 ↗ 3)",
            "Technika zmiany pozycji",
            "Płynne łączenie 3 kolejnych inwersji wybranego arpeggio w górę gryfu przez slide na najwyższej strunie."
        },
        {
            "three_to_five_expand",
            "Przejście 3-strunowe → 5-strunowe (i → vii° → i → V)",
            "Hybryda 3↔5 strun",
            "Szybkie, zwarte 3-strunowe arpeggia przechodzące bezpośrednio w pełne, szerokie 5-strunowe sweepy."
        },
        {
            "andalusian_metal",
            "Pochód metalowy (i → ♭VII → ♭VI → V7)",
            "Metal / Shred",
            "Zstępująca progresja molowa (np. Am → G → F → E7) z ostrym rozwiązaniem dominantowym na V7."
        },
        {
            "major_I_V_vi_IV",
            "Progresja durowa (I → V → vi → IV)",
            "Rock / Melodic",
            "Płynne łączenie kształtów durowych i molowych (np. C → G → Am → F) w jednej pozycji bez skakania po gryfie."
        },
        {
            "jazz_ii_V_I",
            "Jazz / Fusion Sweep (ii7 → V7 → Imaj7 → VI7)",
            "Jazz / Economy",
            "Czterodźwiękowe arpeggia septymowe (styl Franka Gambale) z głosami prowadzącymi ♭7 → 3 między kolejnymi akordami."
        },
        {
            "diatonic_climb",
            "Wspinaczka diatoniczna w skali (I → ii → iii → IV → V)",
            "Skala → Arpeggio",
            "Kolejne trójdźwięki diatoniczne wybranej skali grane sweepem w górę gryfu (świetne do budowania melodii)."
        }
    };
    return presets;
}

SweepTransition generateSweepTransition(
    const Fretboard& fretboard,
    std::string_view presetId,
    int rootPc,
    int stringSpan,
    int startPositionIndex,
    const Chord* customChord,
    const Scale* customScale,
    AccidentalMode mode,
    bool diatonicSevenths) {
    SweepTransition tr{};
    tr.id = std::string(presetId);

    const int effSpan = (stringSpan == 6) ? 6 : (stringSpan == 5 ? 5 : 3);

    struct Spec {
        int semiOffset;
        std::string chordId;
        std::string roman;
        int overrideSpan; // 0 = użyj effSpan
        bool climbStep;   // true = wybieraj wyższą pozycję (+2..+5 progów wyżej)
    };
    std::vector<Spec> specs;

    if (presetId == "dim7_climb") {
        tr.name = "Wspinaczka dim7 co 3 progi (i → vii°7 ↗ → i)";
        tr.category = "Shred / Neoklasyka";
        tr.description = "Symetryczne przesunięcia zmniejszone co 3 progi (tercja mała) ze slidem do kolejnej pozycji.";
        tr.practiceTip = "Przy każdej zmianie dim7 przesuń palec wskazujący lub mały o dokładnie 3 progi (slide S) i zachowaj ciągły ruch kostki.";
        specs = {
            {0,  "minor", "i",     effSpan, false},
            {11, "dim7",  "vii°7", effSpan, false},
            {2,  "dim7",  "ii°7 (+3)", effSpan, true},
            {5,  "dim7",  "iv°7 (+6)", effSpan, true},
            {0,  "minor", "i (okt.)",  effSpan, true}
        };
    } else if (presetId == "same_chord_inversions") {
        std::string cid = (customChord && customChord->definition) ? customChord->definition->id : "minor";
        tr.name = "Inwersje jednego akordu po gryfie (Poz. 1 ↗ 2 ↗ 3)";
        tr.category = "Technika zmiany pozycji";
        tr.description = "Trzy kolejne inwersje tego samego akordu połączone płynnym przesunięciem w górę gryfu.";
        tr.practiceTip = "Na szczycie każdej pozycji zrób slide (S) na strunie E1 do kolejnej inwersji, a przy zejściu płynnie złap nowy kształt.";
        specs = {
            {0, cid, "Poz. 1", effSpan, false},
            {0, cid, "Poz. 2", effSpan, true},
            {0, cid, "Poz. 3", effSpan, true}
        };
    } else if (presetId == "three_to_five_expand") {
        tr.name = "Przejście 3-strunowe → 5-strunowe (i → vii° → i → V)";
        tr.category = "Hybryda 3↔5 strun";
        tr.description = "Dwa szybkie 3-strunowe sweepy rozwijające się w szerokie, 5-strunowe arpeggia w tej samej strefie gryfu.";
        tr.practiceTip = "Na przejściu z 3 strun na 5 strun nie przerywaj ruchu nadgarstka — po powrocie ↑ na strunie G od razu wejdź ↓ na strunę A.";
        specs = {
            {0,  "minor", "i (3 str.)",    3, false},
            {11, "dim",   "vii° (3 str.)", 3, false},
            {0,  "minor", "i (5 str.)",    5, false},
            {7,  "major", "V (5 str.)",    5, false}
        };
    } else if (presetId == "andalusian_metal") {
        tr.name = "Pochód metalowy (i → ♭VII → ♭VI → V7)";
        tr.category = "Metal / Shred";
        tr.description = "Zstępująca sekwencja molowa z napięciem dominantowym na akordzie V7 wracającym do i.";
        tr.practiceTip = "Zwróć uwagę na płynne obniżenie pozycji o 2 progi między i oraz ♭VII, a potem o kolejne 2 progi do ♭VI.";
        specs = {
            {0,  "minor", "i",    effSpan, false},
            {10, "major", "♭VII", effSpan, false},
            {8,  "major", "♭VI",  effSpan, false},
            {7,  "7",     "V7",   effSpan, false}
        };
    } else if (presetId == "major_I_V_vi_IV") {
        tr.name = "Progresja durowa (I → V → vi → IV)";
        tr.category = "Rock / Melodic";
        tr.description = "Popularna progresja w jednej strefie gryfu — uczy natychmiastowej zmiany kształtu durowego na molowy.";
        tr.practiceTip = "Zauważ wspólne dźwięki (pivoty) między I a vi oraz między vi a IV — zostawiaj palec kotwiczący tam, gdzie próg się powtarza.";
        specs = {
            {0, "major", "I",  effSpan, false},
            {7, "major", "V",  effSpan, false},
            {9, "minor", "vi", effSpan, false},
            {5, "major", "IV", effSpan, false}
        };
    } else if (presetId == "jazz_ii_V_I") {
        tr.name = "Jazz / Fusion Sweep (ii7 → V7 → Imaj7 → VI7)";
        tr.category = "Jazz / Economy";
        tr.description = "Sekwencja czterodźwięków septymowych z głosem prowadzącym ♭7 → 3 w zwartym obszarze gryfu.";
        tr.practiceTip = "Używaj 5 strun dla pełnego brzmienia septymy; przy przejściu ii7 → V7 septyma opada tylko o 1 półton na tercję dominanty.";
        int jazzSpan = std::max(5, effSpan);
        specs = {
            {2, "m7",   "iim7",  jazzSpan, false},
            {7, "7",    "V7",    jazzSpan, false},
            {0, "maj7", "Imaj7", jazzSpan, false},
            {9, "7",    "VI7",   jazzSpan, false}
        };
    } else if (presetId == "diatonic_climb") {
        Scale sc = (customScale && customScale->definition)
            ? *customScale
            : Scale{Note::fromPitchClass(rootPc, mode), findScaleDefinition("major")};
        tr.name = "Wspinaczka diatoniczna: " + sc.fullName(false) + (diatonicSevenths ? " (septymowe)" : " (trójdźwięki)");
        tr.category = "Skala → Arpeggio";
        tr.description = "Kolejne diatoniczne arpeggia z wybranej skali grane sweepem w górę gryfu.";
        tr.practiceTip = "Trzymaj stały puls (szesnastki lub triole) — każdy kolejny stopień skali przesuwa dłoń o 1–3 progi w górę gryfu.";
        auto diat = generateDiatonicArpeggios(sc, diatonicSevenths, mode);
        const std::size_t maxSteps = std::min<std::size_t>(diat.size(), 7);
        for (std::size_t i = 0; i < maxSteps; ++i) {
            int relSemi = (diat[i].chord.root.pitchClass() - rootPc + 12) % 12;
            specs.push_back({relSemi, diat[i].chord.definition->id, diat[i].romanNumeral, effSpan, i > 0});
        }
    } else {
        // Domyślnie: neoclassical_minor (i -> bVI -> iv -> V)
        tr.id = "neoclassical_minor";
        tr.name = "Neoklasyczna kadencja (i → ♭VI → iv → V)";
        tr.category = "Neoklasyka";
        tr.description = "Najpopularniejsze przejście sweep picking w stylu Yngwie Malmsteena / Bacha w jednej pozycji.";
        tr.practiceTip = "Między i (Am) a ♭VI (F) zmienia się praktycznie tylko 1 dźwięk (E → F o 1 półton)! Ćwicz płynne przetoczenie palca (Roll).";
        specs = {
            {0, "minor", "i",   effSpan, false},
            {8, "major", "♭VI", effSpan, false},
            {5, "minor", "iv",  effSpan, false},
            {7, "major", "V",   effSpan, false}
        };
    }

    double prevCenterFret = -1.0;
    int prevEndFret = -1;
    int prevEndString = -1;

    for (std::size_t idx = 0; idx < specs.size(); ++idx) {
        const auto& sp = specs[idx];
        int pc = (rootPc + sp.semiOffset + 120) % 12;
        const ChordDefinition* def = findChordDefinition(sp.chordId);
        if (!def) def = findChordDefinition("minor");
        Chord ch{Note::fromPitchClass(pc, mode), def, std::nullopt};

        int useSpan = sp.overrideSpan > 0 ? sp.overrideSpan : effSpan;
        auto shapes = generateSweepShapes(fretboard, ch, useSpan, 0, mode, nullptr);
        if (shapes.empty() && useSpan != 3) {
            shapes = generateSweepShapes(fretboard, ch, 3, 0, mode, nullptr);
        }
        if (shapes.empty()) continue;

        std::size_t chosenIdx = 0;
        if (prevCenterFret < 0.0) {
            // Pierwszy akord: 0 = Auto (dobór strefy startowej), >= 1 = 1-indeksowany wybór pozycji startowej
            if (startPositionIndex > 0) {
                chosenIdx = static_cast<std::size_t>(startPositionIndex - 1) % shapes.size();
            } else {
                const bool isClimbPreset = (presetId == "diatonic_climb" || presetId == "dim7_climb" || presetId == "same_chord_inversions");
                const double idealStart = isClimbPreset ? 5.0 : 8.5;
                double bestDiff = 1e9;
                for (std::size_t i = 0; i < shapes.size(); ++i) {
                    double c = 0.5 * (shapes[i].minFret + shapes[i].maxFret);
                    double d = std::abs(c - idealStart);
                    if (d < bestDiff) { bestDiff = d; chosenIdx = i; }
                }
            }
        } else if (sp.climbStep) {
            // Wspinaczka: wybierz najbliższą pozycję ściśle powyżej poprzedniej
            const double minStep = (presetId == "diatonic_climb") ? 0.4 : 1.2;
            double bestScore = 1e9;
            bool foundHigher = false;
            for (std::size_t i = 0; i < shapes.size(); ++i) {
                double c = 0.5 * (shapes[i].minFret + shapes[i].maxFret);
                if (c > prevCenterFret + minStep) {
                    double d = c - prevCenterFret;
                    if (d < bestScore) { bestScore = d; chosenIdx = i; foundHigher = true; }
                }
            }
            if (!foundHigher) {
                // Jeśli zabrakło progów na górze, wybierz najwyższą dostępną pozycję
                chosenIdx = shapes.size() - 1;
            }
        } else {
            // Voice-leading w jednej strefie: zminimalizuj skok dłoni na gryfie!
            double bestDiff = 1e9;
            for (std::size_t i = 0; i < shapes.size(); ++i) {
                double c = 0.5 * (shapes[i].minFret + shapes[i].maxFret);
                double d = std::abs(c - prevCenterFret);
                if (d < bestDiff) { bestDiff = d; chosenIdx = i; }
            }
        }

        const auto& pickedShape = shapes[chosenIdx];
        double curCenter = 0.5 * (pickedShape.minFret + pickedShape.maxFret);

        SweepTransitionSegment seg{};
        seg.romanNumeral = sp.roman;
        seg.chordSymbol = ch.symbol(false, mode);
        seg.chordId = def->id;
        seg.rootPc = pc;
        seg.stringSpan = pickedShape.stringSpan;
        seg.minFret = pickedShape.minFret;
        seg.maxFret = pickedShape.maxFret;
        seg.nodes = pickedShape.nodes;

        bool hasRoll = false;
        seg.steps = buildSweepStepsFromNodes(fretboard, seg.nodes, 0, static_cast<int>(idx), mode, hasRoll);

        // Oznacz połączenie (slide / płynne wejście) od poprzedniego segmentu
        if (idx > 0 && !seg.steps.empty() && prevEndFret > 0) {
            int startFret = seg.steps.front().fret;
            int startStr = seg.steps.front().stringIndex;
            int deltaFret = startFret - prevEndFret;
            if (startStr == prevEndString && std::abs(deltaFret) >= 1 && std::abs(deltaFret) <= 5) {
                seg.steps.front().technique = "S"; // Slide na tej samej strunie do kolejnego akordu!
                seg.transitionHint = "Slide " + std::to_string(prevEndFret) + "→" + std::to_string(startFret) +
                                     " na strunie " + std::to_string(fretboard.stringCount() - startStr);
            } else if (deltaFret == 0 && startStr == prevEndString) {
                seg.transitionHint = "Wspólny dźwięk kotwiczący (próg " + std::to_string(startFret) + ")";
            } else if (std::abs(curCenter - prevCenterFret) <= 2.5) {
                seg.transitionHint = "Ta sama pozycja (minimalny ruch dłoni)";
            } else {
                seg.transitionHint = (curCenter > prevCenterFret ? "Przesunięcie w górę gryfu (pr. " : "Przesunięcie w dół gryfu (pr. ") +
                                     std::to_string(seg.minFret) + "–" + std::to_string(seg.maxFret) + ")";
            }
        } else {
            seg.transitionHint = "Start pozycji (pr. " + std::to_string(seg.minFret) + "–" + std::to_string(seg.maxFret) + ")";
        }
        if (hasRoll) {
            seg.transitionHint += " · Roll (przetoczenie palca)";
        }

        if (!seg.steps.empty()) {
            prevEndFret = seg.steps.back().fret;
            prevEndString = seg.steps.back().stringIndex;
        }
        prevCenterFret = curCenter;
        tr.segments.push_back(std::move(seg));
    }

    return tr;
}

std::vector<DiatonicArpeggio> generateDiatonicArpeggios(
    const Scale& scale,
    bool seventhChords,
    AccidentalMode mode) {
    std::vector<DiatonicArpeggio> result;
    if (!scale.definition) {
        return result;
    }

    const auto scaleNotes = scale.notes(mode);
    const std::size_t nDeg = scaleNotes.size();
    if (nDeg < 5) {
        return result;
    }

    constexpr std::array<const char*, 8> upperRoman = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII"};
    constexpr std::array<const char*, 8> lowerRoman = {"i", "ii", "iii", "iv", "v", "vi", "vii", "viii"};

    auto findFirstFittingDef = [&](const Note& root, bool wantSevenths) -> const ChordDefinition* {
        auto matchesInCategory = [&](std::string_view cat) -> const ChordDefinition* {
            for (const auto& def : allChordDefinitions()) {
                if (!cat.empty() && def.category != cat) continue;
                Chord cand{root, &def, std::nullopt};
                bool fits = true;
                for (const auto& cn : cand.notes(mode)) {
                    if (!scale.containsPitchClass(cn.pitchClass())) {
                        fits = false;
                        break;
                    }
                }
                if (fits) return &def;
            }
            return nullptr;
        };
        if (wantSevenths) {
            if (const auto* d = matchesInCategory("Septymowe")) return d;
        }
        if (const auto* d = matchesInCategory("Trójdźwięki")) return d;
        if (const auto* d = matchesInCategory("Rozszerzone")) return d;
        if (const auto* d = matchesInCategory("")) return d;
        return findChordDefinition("5");
    };

    for (std::size_t i = 0; i < nDeg; ++i) {
        Note root = scaleNotes[i];
        std::string chordId = "";
        std::string numeral = upperRoman[std::min<std::size_t>(i, 7)];

        if (nDeg == 7) {
            Note third = scaleNotes[(i + 2) % 7];
            Note fifth = scaleNotes[(i + 4) % 7];
            Note seventh = scaleNotes[(i + 6) % 7];

            int thirdSemi = (third.pitchClass() - root.pitchClass() + 12) % 12;
            int fifthSemi = (fifth.pitchClass() - root.pitchClass() + 12) % 12;
            int seventhSemi = (seventh.pitchClass() - root.pitchClass() + 12) % 12;

            if (!seventhChords) {
                if (thirdSemi == 4 && fifthSemi == 7) {
                    chordId = "major";
                    numeral = upperRoman[i];
                } else if (thirdSemi == 3 && fifthSemi == 7) {
                    chordId = "minor";
                    numeral = lowerRoman[i];
                } else if (thirdSemi == 3 && fifthSemi == 6) {
                    chordId = "dim";
                    numeral = std::string(lowerRoman[i]) + "°";
                } else if (thirdSemi == 4 && fifthSemi == 8) {
                    chordId = "aug";
                    numeral = std::string(upperRoman[i]) + "+";
                }
            } else {
                if (thirdSemi == 4 && fifthSemi == 7 && seventhSemi == 11) {
                    chordId = "maj7";
                    numeral = std::string(upperRoman[i]) + "maj7";
                } else if (thirdSemi == 4 && fifthSemi == 7 && seventhSemi == 10) {
                    chordId = "7";
                    numeral = std::string(upperRoman[i]) + "7";
                } else if (thirdSemi == 3 && fifthSemi == 7 && seventhSemi == 10) {
                    chordId = "m7";
                    numeral = std::string(lowerRoman[i]) + "m7";
                } else if (thirdSemi == 3 && fifthSemi == 7 && seventhSemi == 11) {
                    chordId = "mMaj7";
                    numeral = std::string(lowerRoman[i]) + "m(maj7)";
                } else if (thirdSemi == 3 && fifthSemi == 6 && seventhSemi == 10) {
                    chordId = "m7b5";
                    numeral = std::string(lowerRoman[i]) + "m7♭5";
                } else if (thirdSemi == 3 && fifthSemi == 6 && seventhSemi == 9) {
                    chordId = "dim7";
                    numeral = std::string(lowerRoman[i]) + "dim7";
                } else if (thirdSemi == 4 && fifthSemi == 8 && seventhSemi == 11) {
                    chordId = "augMaj7";
                    numeral = std::string(upperRoman[i]) + "maj7♯5";
                }
            }
        }

        const ChordDefinition* def = chordId.empty() ? findFirstFittingDef(root, seventhChords) : findChordDefinition(chordId);
        if (!def) def = findFirstFittingDef(root, false);
        if (chordId.empty() && def) {
            const bool isMinFlavor = (def->id == "minor" || def->id == "m7" || def->id == "dim" || def->id == "m7b5" || def->id == "dim7" || def->id == "m6" || def->id == "madd9");
            const std::size_t rIdx = std::min<std::size_t>(i, 7);
            numeral = std::string(isMinFlavor ? lowerRoman[rIdx] : upperRoman[rIdx]) +
                      (def->id == "major" || def->id == "minor" ? "" : def->symbolSuffix);
        }

        DiatonicArpeggio da{};
        da.degreeNumber = static_cast<int>(i + 1);
        da.romanNumeral = numeral;
        da.chord = Chord{root, def, std::nullopt};
        result.push_back(std::move(da));
    }

    return result;
}

} // namespace gitara::theory
