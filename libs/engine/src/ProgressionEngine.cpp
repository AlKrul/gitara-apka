#include "gitara/engine/ProgressionEngine.h"
#include "gitara/theory/Fretboard.h"
#include <algorithm>
#include <array>

namespace gitara::engine {

gitara::theory::Chord ChordSlot::resolveInKey(
    gitara::theory::Note keyRoot,
    gitara::theory::AccidentalMode mode) const {
    gitara::theory::Note chordRoot = rootOffsetFromKey.applyTo(keyRoot).withAccidentalMode(mode);
    const auto* def = gitara::theory::findChordDefinition(chordDefId);
    return gitara::theory::Chord{chordRoot, def, std::nullopt};
}

namespace {

ProgressionMeasure singleChordBar(gitara::theory::Interval iv,
                                  const char* defId,
                                  const char* numeral) {
    ProgressionMeasure m{};
    m.chords.push_back(ChordSlot{0, iv, defId, numeral});
    return m;
}

} // namespace

const std::vector<ChordProgression>& builtinProgressions() {
    using namespace gitara::theory;
    using namespace gitara::theory::intervals;

    static const std::vector<ChordProgression> list = [] {
        std::vector<ChordProgression> v;

        // 1. Blues 12-taktowy (A dur, Shuffle, 108 BPM – dokładnie jak na screenshocie!)
        {
            ChordProgression p{};
            p.id = "blues_12";
            p.title = "Blues 12-taktowy";
            p.styleCategory = "Blues";
            p.keyRoot = Note{Letter::A, Accidental::Natural};
            p.tempoBpm = 108;
            p.swing = SwingLevel::Heavy;
            p.groove = GrooveStyle::Shuffle;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P4, "7", "IV7"),
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P4, "7", "IV7"),
                singleChordBar(P4, "7", "IV7"),
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P5, "7", "V7"),
                singleChordBar(P4, "7", "IV7"),
                singleChordBar(P1, "7", "I7"),
                singleChordBar(P5, "7", "V7")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 2. Jazz blues (F dur)
        {
            ChordProgression p{};
            p.id = "jazz_blues";
            p.title = "Jazz blues";
            p.styleCategory = "Blues";
            p.keyRoot = Note{Letter::F, Accidental::Natural};
            p.tempoBpm = 120;
            p.swing = SwingLevel::Medium;
            p.groove = GrooveStyle::Shuffle;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "7",    "I7"),
                singleChordBar(P4, "7",    "IV7"),
                singleChordBar(P1, "7",    "I7"),
                singleChordBar(P1, "7",    "I7"),
                singleChordBar(P4, "7",    "IV7"),
                singleChordBar(A4, "dim7", "♯IVdim7"),
                singleChordBar(P1, "7",    "I7"),
                singleChordBar(M6, "7",    "VI7"),
                singleChordBar(M2, "m7",   "iim7"),
                singleChordBar(P5, "7",    "V7"),
                singleChordBar(P1, "7",    "I7"),
                singleChordBar(P5, "7",    "V7")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 3. ii–V–I (C dur)
        {
            ChordProgression p{};
            p.id = "ii_v_i_major";
            p.title = "ii–V–I we wszystkich 12 tonacjach";
            p.styleCategory = "Jazz";
            p.keyRoot = Note{Letter::C, Accidental::Natural};
            p.tempoBpm = 120;
            p.swing = SwingLevel::Medium;
            p.cycleThroughKeys = true;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(M2, "m7",   "iim7"),
                singleChordBar(P5, "7",    "V7"),
                singleChordBar(P1, "maj7", "Imaj7"),
                singleChordBar(P1, "maj7", "Imaj7")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 4. Molowe ii–V–i (C moll)
        {
            ChordProgression p{};
            p.id = "ii_v_i_minor";
            p.title = "Molowe ii–V–i we wszystkich 12 tonacjach";
            p.styleCategory = "Jazz";
            p.keyRoot = Note{Letter::C, Accidental::Natural};
            p.isMinorKey = true;
            p.tempoBpm = 116;
            p.swing = SwingLevel::Medium;
            p.cycleThroughKeys = true;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(M2, "m7b5", "iim7♭5"),
                singleChordBar(P5, "7b9",  "V7♭9"),
                singleChordBar(P1, "m7",   "im7"),
                singleChordBar(P1, "m7",   "im7")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 5. I–V–vi–IV (Pop, G dur)
        {
            ChordProgression p{};
            p.id = "pop_1564";
            p.title = "I–V–vi–IV";
            p.styleCategory = "Pop";
            p.keyRoot = Note{Letter::G, Accidental::Natural};
            p.tempoBpm = 112;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "major", "I"),
                singleChordBar(P5, "major", "V"),
                singleChordBar(M6, "minor", "vi"),
                singleChordBar(P4, "major", "IV")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 6. vi–IV–I–V (C dur: Am F C G)
        {
            ChordProgression p{};
            p.id = "pop_6415";
            p.title = "vi–IV–I–V";
            p.styleCategory = "Pop";
            p.keyRoot = Note{Letter::C, Accidental::Natural};
            p.tempoBpm = 110;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(M6, "minor", "vi"),
                singleChordBar(P4, "major", "IV"),
                singleChordBar(P1, "major", "I"),
                singleChordBar(P5, "major", "V")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 7. I–vi–IV–V (lata 50., C dur: C Am F G)
        {
            ChordProgression p{};
            p.id = "fifties_1645";
            p.title = "I–vi–IV–V (lata 50.)";
            p.styleCategory = "Pop";
            p.keyRoot = Note{Letter::C, Accidental::Natural};
            p.tempoBpm = 100;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "major", "I"),
                singleChordBar(M6, "minor", "vi"),
                singleChordBar(P4, "major", "IV"),
                singleChordBar(P5, "major", "V")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 8. Progresja z Kanonu Pachelbela (D dur: D A Bm F#m G D G A)
        {
            ChordProgression p{};
            p.id = "canon";
            p.title = "Progresja z Kanonu";
            p.styleCategory = "Pop";
            p.keyRoot = Note{Letter::D, Accidental::Natural};
            p.tempoBpm = 92;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "major", "I"),
                singleChordBar(P5, "major", "V"),
                singleChordBar(M6, "minor", "vi"),
                singleChordBar(M3, "minor", "iii"),
                singleChordBar(P4, "major", "IV"),
                singleChordBar(P1, "major", "I"),
                singleChordBar(P4, "major", "IV"),
                singleChordBar(P5, "major", "V")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        // 9. Kadencja andaluzyjska (A moll: Am G F E7)
        {
            ChordProgression p{};
            p.id = "andalusian";
            p.title = "Kadencja andaluzyjska";
            p.styleCategory = "Pop";
            p.keyRoot = Note{Letter::A, Accidental::Natural};
            p.isMinorKey = true;
            p.tempoBpm = 105;

            ProgressionSection secA{};
            secA.name = "A";
            secA.measures = {
                singleChordBar(P1, "minor", "i"),
                singleChordBar(m7, "major", "♭VII"),
                singleChordBar(m6, "major", "♭VI"),
                singleChordBar(P5, "7",     "V7")
            };
            p.sections.push_back(std::move(secA));
            v.push_back(std::move(p));
        }

        return v;
    }();
    return list;
}

std::optional<ChordProgression> findBuiltinProgression(std::string_view id) {
    for (const auto& p : builtinProgressions()) {
        if (p.id == id) {
            return p;
        }
    }
    return std::nullopt;
}

ChordProgression transposeProgression(ChordProgression prog, gitara::theory::Note newKeyRoot) {
    prog.keyRoot = newKeyRoot;
    return prog;
}

ChordProgression reharmonizeProgression(const ChordProgression& prog) {
    using namespace gitara::theory::intervals;
    ChordProgression out = prog;
    for (auto& sec : out.sections) {
        for (auto& m : sec.measures) {
            for (auto& slot : m.chords) {
                // Podstawienie trytonowe (♭II7 w miejsce dominanty V7)
                if (slot.rootOffsetFromKey == P5 &&
                    (slot.chordDefId == "7" || slot.chordDefId == "7b9")) {
                    slot.rootOffsetFromKey = m2;
                    slot.chordDefId = "7";
                    slot.romanNumeral = "♭II7";
                }
            }
        }
    }
    return out;
}

ChordProgression expandThroughAll12Keys(const ChordProgression& prog) {
    using namespace gitara::theory;
    ChordProgression out = prog;
    if (prog.sections.empty()) {
        return out;
    }

    const auto baseSection = prog.sections.front();
    out.sections.clear();

    // Przejście po kole kwart (co +5 półtonów: C, F, Bb, Eb, Ab, Db, Gb, B, E, A, D, G)
    for (int k = 0; k < 12; ++k) {
        int semiShift = (k * 5) % 12;
        Note currentKey = Note::fromPitchClass((prog.keyRoot.pitchClass() + semiShift) % 12);
        ProgressionSection sec = baseSection;
        sec.name = currentKey.toString(true);
        for (auto& m : sec.measures) {
            for (auto& slot : m.chords) {
                int newSemi = (slot.rootOffsetFromKey.semitones + semiShift) % 12;
                slot.rootOffsetFromKey = intervalFromSemitones(newSemi);
            }
        }
        out.sections.push_back(std::move(sec));
    }

    return out;
}

gitara::score::Song generateBackingTrackSong(
    const ChordProgression& prog,
    gitara::theory::AccidentalMode mode) {
    gitara::score::Song song{};
    song.title = prog.title;
    song.tempoBpm = prog.tempoBpm;

    gitara::score::Track compTrack{};
    compTrack.name = "Akompaniament (Gitara rytm. / Organy)";
    compTrack.midiChannel = 0;
    compTrack.midiProgram = 27; // Electric Guitar (clean)
    gitara::theory::Fretboard guitarFb{compTrack.tuning};

    gitara::score::Track bassTrack{};
    bassTrack.name = "Bas elektryczny (Walking Bass)";
    bassTrack.midiChannel = 1;
    bassTrack.midiProgram = 33; // Electric Bass (finger)
    bassTrack.tuning = gitara::theory::findTuning("standard");

    for (const auto& sec : prog.sections) {
        const int repeats = std::max(1, sec.repeatCount);
        for (int r = 0; r < repeats; ++r) {
            for (const auto& pm : sec.measures) {
                if (pm.voltaNumber > 0 && pm.voltaNumber != (r + 1)) {
                    continue;
                }

                gitara::score::Measure compM{};
                compM.numerator = prog.numerator;
                compM.denominator = prog.denominator;

                gitara::score::Measure bassM{};
                bassM.numerator = prog.numerator;
                bassM.denominator = prog.denominator;

                gitara::theory::Chord activeChord = pm.chords.empty()
                    ? gitara::theory::Chord{prog.keyRoot, gitara::theory::findChordDefinition("major")}
                    : pm.chords.front().resolveInKey(prog.keyRoot, mode);

                auto voicings = guitarFb.generateChordVoicings(activeChord, false);
                const auto chordNotes = activeChord.notes(mode);

                // 4 ćwierćnuty w takcie: akompaniament + linia kroczącego basu (pryma -> tercja -> kwinta -> dźwięk prowadzący)
                for (int beatIdx = 0; beatIdx < prog.numerator; ++beatIdx) {
                    for (const auto& slot : pm.chords) {
                        if (slot.beatIndex == beatIdx) {
                            activeChord = slot.resolveInKey(prog.keyRoot, mode);
                            voicings = guitarFb.generateChordVoicings(activeChord, false);
                        }
                    }

                    gitara::score::Beat compBeat{};
                    compBeat.duration = gitara::score::DurationValue::Quarter;
                    if (!voicings.empty()) {
                        const auto& v = voicings.front();
                        for (std::size_t s = 0; s < v.frets.size(); ++s) {
                            if (v.frets[s] >= 0) {
                                compBeat.notes.push_back(
                                    gitara::score::TabNote{static_cast<int>(s), v.frets[s], 85});
                            }
                        }
                    }
                    compM.beats.push_back(std::move(compBeat));

                    // Kroczący bas na najniższych strunach (E2 / A2)
                    gitara::score::Beat bassBeat{};
                    bassBeat.duration = gitara::score::DurationValue::Quarter;
                    int targetPc = activeChord.root.pitchClass();
                    if (!chordNotes.empty()) {
                        targetPc = chordNotes[static_cast<std::size_t>(beatIdx) % chordNotes.size()].pitchClass();
                    }
                    int targetMidi = 40 + ((targetPc - 4 + 12) % 12); // Zakres E2 (40) .. D#3 (51)
                    if (auto pos = guitarFb.selectBestPosition(targetMidi)) {
                        bassBeat.notes.push_back(gitara::score::TabNote{pos->stringIndex, pos->fret, 95});
                    }
                    bassM.beats.push_back(std::move(bassBeat));
                }

                compTrack.measures.push_back(std::move(compM));
                bassTrack.measures.push_back(std::move(bassM));
            }
        }
    }

    song.tracks.push_back(std::move(compTrack));
    if (prog.backingBandEnabled) {
        song.tracks.push_back(std::move(bassTrack));
    }
    return song;
}

} // namespace gitara::engine
