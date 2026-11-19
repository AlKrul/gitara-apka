#include "gitara/theory/Arpeggio.h"
#include <algorithm>
#include <array>

namespace gitara::theory {

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

    auto makeNode = [&](int s, int f) -> ArpeggioPositionNode {
        int midi = fretboard.midiAt(s, f);
        int pc = midi % 12;
        Interval foundIv = intervals::P1;
        Note spelled = Note::fromPitchClass(pc, mode);

        for (const auto& iv : chord.definition->intervals) {
            if ((chord.root.pitchClass() + iv.semitones) % 12 == pc) {
                foundIv = iv;
                spelled = iv.applyTo(chord.root).withAccidentalMode(mode);
                break;
            }
        }

        ArpeggioPositionNode node{};
        node.position = FretPosition{s, f};
        node.note = spelled;
        node.intervalFromRoot = foundIv;
        node.isRoot = (pc == chord.root.pitchClass());
        return node;
    };

    const int nStrings = fretboard.stringCount();
    const int nFrets = fretboard.fretCount();

    if (pattern == ArpeggioPattern::FullNeck) {
        for (int s = 0; s < nStrings; ++s) {
            for (int f = 0; f <= nFrets; ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                int midi = fretboard.midiAt(s, f);
                if (chord.containsPitchClass(midi % 12)) {
                    nodes.push_back(makeNode(s, f));
                }
            }
        }
        return nodes;
    }

    if (pattern == ArpeggioPattern::CAGED) {
        int rootPc = chord.root.pitchClass();
        int openE = fretboard.midiAt(0, 0) % 12;
        int eRootFret = (rootPc - openE + 12) % 12;
        constexpr std::array<int, 5> offsets = {0, 2, 5, 7, 9};
        int box = ((positionIndex % 5) + 5) % 5;
        int minFret = (eRootFret + offsets[static_cast<std::size_t>(box)]) % 12;
        int maxFret = minFret + 4;

        for (int s = 0; s < nStrings; ++s) {
            for (int f = minFret; f <= std::min(nFrets, maxFret); ++f) {
                if (mask && !mask->isAllowed(s, f)) continue;
                if (chord.containsPitchClass(fretboard.midiAt(s, f) % 12)) {
                    nodes.push_back(makeNode(s, f));
                }
            }
        }
        return nodes;
    }

    if (pattern == ArpeggioPattern::TwoNPS || pattern == ArpeggioPattern::ThreeNPS) {
        const int targetPerString = (pattern == ArpeggioPattern::TwoNPS) ? 2 : 3;
        const auto chordNotes = chord.notes(mode);
        if (chordNotes.empty()) return nodes;

        int startIdx = ((positionIndex % static_cast<int>(chordNotes.size())) +
                        static_cast<int>(chordNotes.size())) %
                       static_cast<int>(chordNotes.size());
        int startPc = chordNotes[static_cast<std::size_t>(startIdx)].pitchClass();
        int openLowPc = fretboard.midiAt(0, 0) % 12;
        int firstFret = (startPc - openLowPc + 12) % 12;
        int minMidi = fretboard.midiAt(0, firstFret);

        for (int s = 0; s < nStrings; ++s) {
            int count = 0;
            for (int f = 0; f <= nFrets; ++f) {
                int m = fretboard.midiAt(s, f);
                if (m >= minMidi && chord.containsPitchClass(m % 12)) {
                    if (!mask || mask->isAllowed(s, f)) {
                        nodes.push_back(makeNode(s, f));
                    }
                    minMidi = m + 1;
                    ++count;
                    if (count >= targetPerString) break;
                }
            }
        }
        return nodes;
    }

    // ArpeggioPattern::Sweep — 1 dźwięk na każdą kolejną strunę w zwartym oknie 4–5 progów
    int anchorFret = ((positionIndex * 3) % 12) + 1;
    int prevMidi = 0;
    for (int s = 0; s < nStrings; ++s) {
        for (int f = anchorFret; f <= std::min(nFrets, anchorFret + 4); ++f) {
            if (mask && !mask->isAllowed(s, f)) continue;
            int m = fretboard.midiAt(s, f);
            if (m > prevMidi && chord.containsPitchClass(m % 12)) {
                nodes.push_back(makeNode(s, f));
                prevMidi = m;
                break;
            }
        }
    }
    return nodes;
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
    if (scaleNotes.size() < 7) {
        return result;
    }

    constexpr std::array<const char*, 7> upperRoman = {"I", "II", "III", "IV", "V", "VI", "VII"};
    constexpr std::array<const char*, 7> lowerRoman = {"i", "ii", "iii", "iv", "v", "vi", "vii"};

    for (std::size_t i = 0; i < 7; ++i) {
        Note root = scaleNotes[i];
        Note third = scaleNotes[(i + 2) % 7];
        Note fifth = scaleNotes[(i + 4) % 7];
        Note seventh = scaleNotes[(i + 6) % 7];

        int thirdSemi = (third.pitchClass() - root.pitchClass() + 12) % 12;
        int fifthSemi = (fifth.pitchClass() - root.pitchClass() + 12) % 12;
        int seventhSemi = (seventh.pitchClass() - root.pitchClass() + 12) % 12;

        std::string chordId = "major";
        std::string numeral = upperRoman[i];

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

        DiatonicArpeggio da{};
        da.degreeNumber = static_cast<int>(i + 1);
        da.romanNumeral = numeral;
        da.chord = Chord{root, findChordDefinition(chordId), std::nullopt};
        result.push_back(std::move(da));
    }

    return result;
}

} // namespace gitara::theory
