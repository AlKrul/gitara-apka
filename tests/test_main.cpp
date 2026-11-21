#include "gitara/dsp/ChordDetector.h"
#include "gitara/dsp/GuitarNoteEngine.h"
#include "gitara/dsp/KarplusStrongSynth.h"
#include "gitara/dsp/OnsetDetector.h"
#include "gitara/dsp/PitchDetector.h"
#include "gitara/dsp/SignalConditioner.h"
#include "gitara/engine/EarTrainer.h"
#include "gitara/engine/PracticeEngine.h"
#include "gitara/engine/ProgressionEngine.h"
#include "gitara/engine/SessionManager.h"
#include "gitara/engine/WorkshopGenerator.h"
#include "gitara/score/Score.h"
#include "gitara/theory/Arpeggio.h"
#include "gitara/theory/Chord.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Interval.h"
#include "gitara/theory/Note.h"
#include "gitara/theory/Pitch.h"
#include "gitara/theory/Scale.h"
#include "gitara/theory/Tuning.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numbers>
#include <set>
#include <string>
#include <vector>

namespace {

int g_passed = 0;
int g_failed = 0;

void check(bool condition, const char* expr, const char* file, int line) {
    if (condition) {
        ++g_passed;
    } else {
        ++g_failed;
        std::cerr << "[BLAD TESTU] " << file << ":" << line << " -> " << expr << "\n";
    }
}

#define CHECK(expr) check((expr), #expr, __FILE__, __LINE__)

/// Generuje realistyczny sygnał struny gitarowej z silnymi wyższymi harmonicznymi
/// (na niskiej strunie E2 druga i trzecia harmoniczna bywają silniejsze od tonu podstawowego).
std::vector<float> synthesizeGuitarNote(double freqHz, double sampleRate, std::size_t numSamples) {
    std::vector<float> buf(numSamples, 0.0f);
    for (std::size_t i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        double sample =
            0.45 * std::sin(2.0 * std::numbers::pi * freqHz * t) +
            0.55 * std::sin(2.0 * std::numbers::pi * (2.0 * freqHz) * t + 0.3) +
            0.35 * std::sin(2.0 * std::numbers::pi * (3.0 * freqHz) * t + 0.7) +
            0.20 * std::sin(2.0 * std::numbers::pi * (4.0 * freqHz) * t + 1.1);
        buf[i] = static_cast<float>(sample * 0.5);
    }
    return buf;
}

void testTheoryConventionsAG() {
    using namespace gitara::theory;

    // 1. System A–G: 'B' = 11 półtonów, 'Bb' = 10 półtonów, 'H' odrzucane
    auto bNat = Note::parse("B");
    auto bFlat = Note::parse("Bb");
    auto invalidH = Note::parse("H");
    CHECK(bNat.has_value() && bNat->pitchClass() == 11);
    CHECK(bFlat.has_value() && bFlat->pitchClass() == 10);
    CHECK(!invalidH.has_value());

    // 2. Przełącznik „Znaki chromatyczne: Auto / Bemole (♭) / Krzyżyki (♯)”
    Note eb = Note::fromPitchClass(3, AccidentalMode::Flats);
    Note dSharp = Note::fromPitchClass(3, AccidentalMode::Sharps);
    CHECK(eb.toString(false) == "Eb");
    CHECK(dSharp.toString(false) == "D#");

    // 3. Każda 7-dźwiękowa skala używa dokładnie 7 różnych liter A–G (brak duplikatów liter)
    const std::vector<std::string> heptatonicIds = {
        "major", "natural_minor", "harmonic_minor", "melodic_minor",
        "dorian", "phrygian", "lydian", "mixolydian", "locrian",
        "lydian_dominant", "mixolydian_b6", "phrygian_dominant"
    };
    const std::vector<Note> standardRoots = {
        {Letter::C, Accidental::Natural},
        {Letter::D, Accidental::Flat},
        {Letter::D, Accidental::Natural},
        {Letter::E, Accidental::Flat},
        {Letter::E, Accidental::Natural},
        {Letter::F, Accidental::Natural},
        {Letter::F, Accidental::Sharp},
        {Letter::G, Accidental::Natural},
        {Letter::A, Accidental::Flat},
        {Letter::A, Accidental::Natural},
        {Letter::B, Accidental::Flat},
        {Letter::B, Accidental::Natural}
    };

    for (const auto& id : heptatonicIds) {
        const auto* def = findScaleDefinition(id);
        CHECK(def != nullptr);
        for (const auto& root : standardRoots) {
            Scale sc{root, def};
            auto notes = sc.notes(AccidentalMode::Auto);
            CHECK(notes.size() == 7);
            std::set<Letter> uniqueLetters;
            for (const auto& n : notes) {
                uniqueLetters.insert(n.letter);
                // Żaden napis w systemie nie może zawierać 'H'
                CHECK(n.toString(true).find('H') == std::string::npos);
            }
            CHECK(uniqueLetters.size() == 7);
        }
    }

    // 4. Test szczególny: G# moll harmoniczna ma F𝄪 (Fx) na 7. stopniu, a nie G!
    Scale gSharpHarmMinor{Note{Letter::G, Accidental::Sharp}, findScaleDefinition("harmonic_minor")};
    auto gshNotes = gSharpHarmMinor.notes(AccidentalMode::Auto);
    CHECK(gshNotes.size() == 7);
    CHECK(gshNotes[6].toString(true) == "F𝄪");
    CHECK(gshNotes[6].toString(false) == "Fx");

    // 5. Test szczególny: Cdim7 = C, Eb, Gb, Bbb (B𝄫)
    Chord cDim7{Note{Letter::C, Accidental::Natural}, findChordDefinition("dim7")};
    auto cDimNotes = cDim7.notes(AccidentalMode::Auto);
    CHECK(cDimNotes.size() == 4);
    CHECK(cDimNotes[0].toString(false) == "C");
    CHECK(cDimNotes[1].toString(false) == "Eb");
    CHECK(cDimNotes[2].toString(false) == "Gb");
    CHECK(cDimNotes[3].toString(false) == "Bbb");

    // 6. Test zakładki „Szukaj (Akord -> skale)”:
    // W aplikacji referencyjnej dla Cmaj -> C Miksolidyjska był błąd pisowni napięć „D A A#”.
    // U nas napięcia C Miksolidyjskiej to poprawnie „D A Bb” (B♭)!
    Chord cMaj{Note{Letter::C, Accidental::Natural}, findChordDefinition("major")};
    auto scaleMatches = findScalesForChord(cMaj, AccidentalMode::Auto);
    CHECK(!scaleMatches.empty());

    bool foundMixolydian = false;
    bool foundMajorWithAvoidF = false;
    for (const auto& m : scaleMatches) {
        if (m.scale.definition->id == "mixolydian") {
            foundMixolydian = true;
            CHECK(m.tensions.size() == 3);
            CHECK(m.tensions[0].toString(false) == "D");
            CHECK(m.tensions[1].toString(false) == "A");
            CHECK(m.tensions[2].toString(false) == "Bb");
        }
        if (m.scale.definition->id == "major") {
            foundMajorWithAvoidF = (!m.avoidNotes.empty() && m.avoidNotes[0].toString(false) == "F");
        }
    }
    CHECK(foundMixolydian);
    CHECK(foundMajorWithAvoidF);
}

void testFretboardMaskVoicingsAndHarmonics() {
    using namespace gitara::theory;

    Fretboard fb{findTuning("standard"), 24};
    CHECK(fb.midiAt(0, 0) == 40); // E2
    CHECK(fb.midiAt(5, 0) == 64); // E4
    CHECK(fb.midiAt(5, 24) == 88);// E6

    // Maska obszaru gryfu: wykluczamy progi 0..4, więc dźwięk E4 (MIDI 64) nie może użyć pustej struny E4 (próg 0),
    // lecz wybierze próg 5 na strunie B3 (stringIndex=4, fret=5)
    FretboardMask mask(6, 24);
    mask.restrictToFretRange(5, 12);
    auto bestPos = fb.selectBestPosition(64, std::nullopt, &mask);
    CHECK(bestPos.has_value());
    CHECK(bestPos->stringIndex == 4 && bestPos->fret == 5);

    // Śledzenie dłoni: jeśli poprzednia nuta była na (string=3, fret=9), to dla E4 (MIDI 64)
    // najbliższą pozycją jest (string=3, fret=9)!
    auto trackedPos = fb.selectBestPosition(64, FretPosition{3, 8}, &mask);
    CHECK(trackedPos.has_value());
    CHECK(trackedPos->stringIndex == 3 && trackedPos->fret == 9);

    // Systemy 3NPS i CAGED
    Scale cMajScale{Note{Letter::C, Accidental::Natural}, findScaleDefinition("major")};
    auto tnpsPos = fb.generateScalePositions(cMajScale, PositionSystem::TNPS, 0);
    CHECK(tnpsPos.size() == 18); // 6 strun * 3 dźwięki na strunę = 18
    auto cagedPos = fb.generateScalePositions(cMajScale, PositionSystem::CAGED, 0);
    CHECK(!cagedPos.empty());

    // Generator voicingów akordu Cmaj (w tym warianty z tappingiem T i pustymi strunami)
    Chord cMajChord{Note{Letter::C, Accidental::Natural}, findChordDefinition("major")};
    auto voicings = fb.generateChordVoicings(cMajChord, true);
    CHECK(voicings.size() >= 10);
    bool hasTapped = false;
    bool hasOpenString = false;
    for (const auto& v : voicings) {
        if (v.usesTapping) hasTapped = true;
        if (!v.openStringsNote.empty()) hasOpenString = true;
    }
    CHECK(hasTapped);
    CHECK(hasOpenString);

    // Weryfikacja ostrości (Łagodne vs Ostre), ścisłych 3-strunowych trójdźwięków oraz kompletu 35 akordów
    int triadDefs = 0, seventhDefs = 0, extDefs = 0, altDefs = 0, basicDefs = 0;
    for (const auto& def : allChordDefinitions()) {
        if (chordMatchesGroup(def, "basic")) ++basicDefs;
        if (chordMatchesGroup(def, "triad")) {
            ++triadDefs;
            CHECK(def.intervals.size() == 3);
        }
        if (chordMatchesGroup(def, "seventh")) ++seventhDefs;
        if (chordMatchesGroup(def, "extended")) ++extDefs;
        if (chordMatchesGroup(def, "altered")) ++altDefs;

        Chord ch{Note{Letter::C, Accidental::Natural}, &def};
        auto mildList = fb.generateChordVoicings(ch, false, nullptr, VoicingSharpness::Mild, 0);
        auto sharpList = fb.generateChordVoicings(ch, true, nullptr, VoicingSharpness::Sharp, 0);
        CHECK(!mildList.empty());
        CHECK(!sharpList.empty());
        for (const auto& mv : mildList) {
            CHECK(!mv.usesTapping);
        }
        bool anySharpFlag = false;
        for (const auto& sv : sharpList) {
            CHECK(sv.isSharpVoicing);
            if (sv.usesTapping || sv.fretSpan >= 4) anySharpFlag = true;
        }
        CHECK(anySharpFlag);

        if (chordMatchesGroup(def, "triad")) {
            auto triadMild = fb.generateChordVoicings(ch, false, nullptr, VoicingSharpness::Mild, 3);
            auto triadSharp = fb.generateChordVoicings(ch, true, nullptr, VoicingSharpness::Sharp, 3);
            CHECK(!triadMild.empty());
            CHECK(!triadSharp.empty());
            for (const auto& tv : triadMild) {
                CHECK(tv.activeStrings == 3);
                CHECK(!tv.usesTapping);
            }
            for (const auto& tv : triadSharp) {
                CHECK(tv.activeStrings == 3);
                CHECK(tv.isSharpVoicing);
            }
        }
    }
    CHECK(basicDefs == 10);
    CHECK(triadDefs == 6);
    CHECK(seventhDefs == 9);
    CHECK(extDefs == 13);
    CHECK(altDefs == 6);

    // Weryfikacja, że akord C7 w Łagodnych i Ostrych ma zawsze dźwięki charakterystyczne (C, E, Bb)
    Chord c7Chord{Note{Letter::C, Accidental::Natural}, findChordDefinition("7")};
    for (const auto& v : fb.generateChordVoicings(c7Chord, true, nullptr, VoicingSharpness::Mild, 0)) {
        bool hasC = false, hasE = false, hasBb = false;
        for (int s = 0; s < fb.stringCount(); ++s) {
            if (v.frets[static_cast<std::size_t>(s)] >= 0) {
                int pc = fb.midiAt(s, v.frets[static_cast<std::size_t>(s)]) % 12;
                if (pc == 0) hasC = true;
                if (pc == 4) hasE = true;
                if (pc == 10) hasBb = true;
            }
        }
        CHECK(hasC && hasE && hasBb);
    }

    // Flażolety naturalne na 6. strunie (E2): węzły nad progami 12 (E3), 7 (B3), 5 (E4) i tuż pod 4 (G#4)
    auto harmonics = fb.computeNaturalHarmonics(0, 0.0, 12.0, 6);
    CHECK(!harmonics.empty());
    CHECK(harmonics.front().soundingPitch.toString(false) == "E3");
    CHECK(harmonics.front().polishLocation == "nad progiem 12");
}

void testDspPitchChordAndSynth() {
    using namespace gitara::dsp;
    using namespace gitara::theory;

    constexpr double kSampleRate = 48000.0;
    PitchDetector detector(kSampleRate, 2048);

    // 1. Test wszystkich 6 pustych strun gitary (z silnymi alikwotami) dla obu algorytmów: YIN oraz Bitstream Q
    const auto stdTuning = findTuning("standard");
    for (const auto& openPitch : stdTuning.openStrings) {
        double targetHz = openPitch.frequencyHz(440.0);
        auto signal = synthesizeGuitarNote(targetHz, kSampleRate, 2048);

        auto yinRes = detector.detect(signal.data(), signal.size(), PitchAlgorithm::YIN);
        CHECK(yinRes.pitched);
        CHECK(yinRes.deviation.midiNote == openPitch.midiNumber());
        CHECK(std::abs(yinRes.deviation.cents) < 1.0);

        auto bitRes = detector.detect(signal.data(), signal.size(), PitchAlgorithm::BitstreamAutocorrelation);
        CHECK(bitRes.pitched);
        CHECK(bitRes.deviation.midiNote == openPitch.midiNumber());
        CHECK(std::abs(bitRes.deviation.cents) < 1.5);
    }

    // 2. Test precyzji stroboskopowej: struna A2 (110 Hz) odstrojona o +14.0 centów
    double detunedA2Hz = 110.0 * std::pow(2.0, 14.0 / 1200.0);
    auto detunedSig = synthesizeGuitarNote(detunedA2Hz, kSampleRate, 2048);
    auto strobeRes = detector.detect(detunedSig.data(), detunedSig.size(), PitchAlgorithm::YIN);
    CHECK(strobeRes.pitched);
    CHECK(strobeRes.deviation.midiNote == 45);
    CHECK(std::abs(strobeRes.deviation.cents - 14.0) < 0.8);

    // 3. Test pętli zamkniętej: KarplusStrongSynth -> SignalConditioner -> PitchDetector
    KarplusStrongSynth synth(kSampleRate);
    SignalConditioner conditioner(kSampleRate);
    synth.noteOn(50, 0.9f); // D3 = 146.83 Hz
    std::vector<float> synthBuf(4096, 0.0f);
    synth.renderNextBlock(synthBuf.data(), synthBuf.size());
    conditioner.processBlock(synthBuf.data(), synthBuf.size());
    // Analizujemy ustabilizowaną część wybrzmiewania struny (od próbki 1024)
    auto ksPitch = detector.detect(synthBuf.data() + 1024, 2048, PitchAlgorithm::YIN);
    CHECK(ksPitch.pitched);
    CHECK(ksPitch.deviation.midiNote == 50);

    // 4. Test polifonicznej detekcji akordów (Chromagram / HPCP): akord Cmaj (C3 + E3 + G3 + C4 + E4)
    std::vector<float> chordBuf(4096, 0.0f);
    const std::vector<int> cMajorMidis = {48, 52, 55, 60, 64};
    for (int m : cMajorMidis) {
        double f = Pitch::fromMidiNumber(m).frequencyHz();
        auto noteWave = synthesizeGuitarNote(f, kSampleRate, chordBuf.size());
        for (std::size_t i = 0; i < chordBuf.size(); ++i) {
            chordBuf[i] += noteWave[i] * 0.25f;
        }
    }

    ChordDetector chordDetector(kSampleRate);
    Chord expectedCMaj{Note{Letter::C, Accidental::Natural}, findChordDefinition("major")};
    Chord wrongFSharpMin{Note{Letter::F, Accidental::Sharp}, findChordDefinition("minor")};

    double cMajConf = chordDetector.verifyExpectedChord(chordBuf.data(), chordBuf.size(), expectedCMaj);
    double wrongConf = chordDetector.verifyExpectedChord(chordBuf.data(), chordBuf.size(), wrongFSharpMin);
    CHECK(cMajConf > 0.80);
    CHECK(wrongConf < 0.35);

    auto detectedChord = chordDetector.detectChord(chordBuf.data(), chordBuf.size(), false);
    CHECK(detectedChord.detected);
    CHECK(detectedChord.chord.root.pitchClass() == 0);
    CHECK(detectedChord.chord.definition != nullptr && detectedChord.chord.definition->id == "major");
}

void testScoreTabRecorderAndTuxGuitarExport() {
    using namespace gitara::score;
    using namespace gitara::theory;

    Fretboard fb{findTuning("standard"), 24};
    FretboardMask firstPosMask(6, 24);
    firstPosMask.restrictToFretRange(0, 5);

    TabRecorder recorder(fb, 120, 4, 4);
    recorder.setTabCreationEnabled(true);

    // Symulujemy zagranie 4 ćwierćnut gamy C dur (C3, D3, E3, G3) w tempie 120 BPM (co 0.5 s)
    std::vector<DetectedNoteEvent> events = {
        {0.0, 0.48, {48}}, // C3 (struna A, próg 3)
        {0.5, 0.48, {50}}, // D3 (struna D, próg 0)
        {1.0, 0.48, {52}}, // E3 (struna D, próg 2)
        {1.5, 0.48, {55}}  // G3 (struna G, próg 0)
    };

    Song song = recorder.buildSongFromEvents(events, &firstPosMask, "Cwiczenie C dur");
    CHECK(song.tracks.size() == 1);
    CHECK(song.tracks[0].measures.size() == 1);
    CHECK(song.tracks[0].measures[0].beats.size() == 4);
    CHECK(song.tracks[0].measures[0].beats[0].notes[0].stringIndex == 1);
    CHECK(song.tracks[0].measures[0].beats[0].notes[0].fret == 3);

    // Wykrywanie tonacji z nut („Wykryj z nut”)
    Scale detectedKey = detectKeyFromSong(song);
    CHECK(detectedKey.root.pitchClass() == 0);
    CHECK(detectedKey.definition != nullptr && detectedKey.definition->id == "major");

    // Eksport ASCII Tab
    std::string asciiTab = exportToAsciiTab(song);
    CHECK(asciiTab.find("-3-") != std::string::npos);

    // Eksport MIDI (.mid dla TuxGuitar)
    auto midiBytes = exportToMidiBytes(song);
    CHECK(midiBytes.size() > 32);
    CHECK(midiBytes[0] == 'M' && midiBytes[1] == 'T' && midiBytes[2] == 'h' && midiBytes[3] == 'd');

    // Eksport Guitar Pro 5 (.gp5 dla TuxGuitar z zachowaniem struny i progu)
    auto gp5Bytes = exportToGp5Bytes(song);
    CHECK(gp5Bytes.size() > 256);
    CHECK(gp5Bytes[0] == 24);
    std::string header(reinterpret_cast<const char*>(gp5Bytes.data() + 1), 24);
    CHECK(header == "FICHIER GUITAR PRO v5.00");
}

void testExerciseVerifierAndMetronome() {
    using namespace gitara::engine;
    using namespace gitara::dsp;
    using namespace gitara::theory;

    ExerciseVerifier verifier;
    std::vector<ExerciseStep> steps = {
        {false, FretPosition{0, 0}, 40, {}, 0.0},
        {false, FretPosition{0, 3}, 43, {}, 0.5}
    };
    verifier.setSteps(steps, 35.0);
    CHECK(!verifier.isFinished());

    PitchResult p1{};
    p1.pitched = true;
    p1.frequencyHz = 82.41;
    p1.deviation = analyzeFrequency(82.41);
    CHECK(verifier.onMonophonicDetection(p1, 0.01));
    CHECK(verifier.currentStepIndex() == 1);

    PitchResult p2{};
    p2.pitched = true;
    p2.frequencyHz = 98.0;
    p2.deviation = analyzeFrequency(98.0);
    CHECK(verifier.onMonophonicDetection(p2, 0.51));
    CHECK(verifier.isFinished());
    CHECK(std::abs(verifier.overallAccuracyPercent() - 100.0) < 1e-6);

    Metronome metro;
    metro.prepare(48000.0);
    metro.setTempoBpm(120.0);
    metro.setTimeSignature(4, 4);
    metro.setSubdivision(2);
    metro.setEnabled(true);
    std::vector<float> clickBuf(2048, 0.0f);
    metro.renderNextBlock(clickBuf.data(), clickBuf.size());
    float maxAbs = 0.0f;
    for (float s : clickBuf) maxAbs = std::max(maxAbs, std::abs(s));
    CHECK(maxAbs > 0.1f);
}

void testArpeggioWorkshopEarTrainerProgressionsAndSessions() {
    using namespace gitara::theory;
    using namespace gitara::engine;
    using namespace gitara::score;

    Fretboard fb{findTuning("standard"), 24};
    Chord cMaj7{Note{Letter::C, Accidental::Natural}, findChordDefinition("maj7")};

    // 1. Zakładka Arpeggio (FullNeck, CAGED bez unisonów, 2NPS, 3NPS, Sweep 3/5/6-str., Inwersje, Od prymy + 8 przejść Sweep + diatoniczne)
    auto fullArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::FullNeck);
    auto cagedArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::CAGED, 0);
    auto twoNpsArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::TwoNPS, 0);
    auto threeNpsArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::ThreeNPS, 0);
    auto sweepArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::Sweep, 0);
    auto rootOct1 = generateRootOctaveArpeggio(fb, cMaj7, 0, 1);
    auto rootOct2 = generateRootOctaveArpeggio(fb, cMaj7, 0, 2);
    CHECK(fullArp.size() > cagedArp.size());
    CHECK(!cagedArp.empty());
    CHECK(!twoNpsArp.empty());
    CHECK(!threeNpsArp.empty());
    CHECK(!sweepArp.empty());
    CHECK(rootOct1.size() >= 4);
    CHECK(rootOct2.size() > rootOct1.size());
    CHECK(rootOct1.front().isRoot);
    CHECK(rootOct1.back().isRoot);

    // Weryfikacja braku zdublowanych unisonów MIDI w CAGED (wszystkie 5 kształtów)
    for (int box = 0; box < 5; ++box) {
        auto boxNodes = generateArpeggio(fb, cMaj7, ArpeggioPattern::CAGED, box);
        CHECK(!boxNodes.empty());
        for (std::size_t i = 1; i < boxNodes.size(); ++i) {
            int mPrev = fb.midiAt(boxNodes[i - 1].position.stringIndex, boxNodes[i - 1].position.fret);
            int mCur = fb.midiAt(boxNodes[i].position.stringIndex, boxNodes[i].position.fret);
            CHECK(mCur > mPrev);
        }
    }

    // Inwersje poziome na wszystkich 3 zestawach strun (trójdźwięk i czterodźwięk) + sekwencje (Prosto, Grupy po 3, Skoki)
    Chord aMinTriad{Note{Letter::A, Accidental::Natural}, findChordDefinition("minor")};
    for (int ss = 0; ss < 3; ++ss) {
        auto invTriad = generateArpeggioInversions(fb, aMinTriad, ss);
        auto inv7 = generateArpeggioInversions(fb, cMaj7, ss);
        CHECK(invTriad.size() >= 3);
        CHECK(inv7.size() >= 3);
    }
    for (int seq = 0; seq <= 2; ++seq) {
        auto st = buildArpeggioSequenceSteps(fb, rootOct2, ArpeggioPattern::RootOctave, 0, AccidentalMode::Sharps, seq);
        CHECK(!st.empty());
    }

    // Sweep shapes 3-, 5- i 6-strunowe dla Am, Cmaj, G#dim7, Cmaj7
    for (const char* cid : {"minor", "major", "dim7", "maj7", "7"}) {
        Chord testCh{Note{Letter::A, Accidental::Natural}, findChordDefinition(cid)};
        for (int span : {3, 5, 6}) {
            auto shapes = generateSweepShapes(fb, testCh, span, 0);
            CHECK(!shapes.empty());
            for (const auto& sh : shapes) {
                CHECK(!sh.nodes.empty());
                CHECK(!sh.steps.empty());
                CHECK(sh.maxFret >= sh.minFret);
            }
        }
    }

    // Wszystkie 8 presetów przejść Sweep Picking w tonacji A i C (3- i 5-strunowe)
    const auto& trPresets = allSweepTransitionPresets();
    CHECK(trPresets.size() == 8);
    Scale aMinScale{Note{Letter::A, Accidental::Natural}, findScaleDefinition("natural_minor")};
    Chord aMinChord{Note{Letter::A, Accidental::Natural}, findChordDefinition("minor")};
    for (const auto& p : trPresets) {
        auto tr3 = generateSweepTransition(fb, p.id, 9, 3, 0, &aMinChord, &aMinScale);
        auto tr5 = generateSweepTransition(fb, p.id, 0, 5, 0, &cMaj7, nullptr);
        CHECK(tr3.segments.size() >= 3);
        CHECK(tr5.segments.size() >= 3);
        for (const auto& seg : tr3.segments) {
            CHECK(!seg.nodes.empty());
            CHECK(!seg.steps.empty());
        }
        for (const auto& seg : tr5.segments) {
            CHECK(!seg.nodes.empty());
            CHECK(!seg.steps.empty());
        }
    }

    Scale cMajScale{Note{Letter::C, Accidental::Natural}, findScaleDefinition("major")};
    auto diatonicArps = generateDiatonicArpeggios(cMajScale, true);
    CHECK(diatonicArps.size() == 7);
    CHECK(diatonicArps[0].romanNumeral == "Imaj7");
    CHECK(diatonicArps[1].romanNumeral == "iim7");
    CHECK(diatonicArps[4].romanNumeral == "V7");
    CHECK(diatonicArps[6].romanNumeral == "viim7♭5");
    CHECK(diatonicArps[6].chord.symbol(false) == "Bm7♭5");

    // 2. Warsztat: generowanie ćwiczeń sekwencyjnych (Grupy po 3/4/6, Paul Gilbert, Triady, Septymy, Pedal, 3 oktawy, Diagonal, Jedna struna)
    WorkshopExerciseConfig exCfg{};
    exCfg.scale = cMajScale;
    exCfg.positionSystem = PositionSystem::TNPS;
    exCfg.pattern = ExercisePattern::GroupsOf4;
    auto genEx = generateScaleExercise(fb, exCfg);
    CHECK(genEx.steps.size() == (18 - 3) * 4); // 15 grup czwórkowych z 18 dźwięków 3NPS = 60 kroków
    CHECK(!genEx.song.tracks.empty());

    // 3 oktawy + tryb DiagonalShift (przesuwany po przekątnej gryfu) + start od toniki
    {
        WorkshopExerciseConfig diag3{};
        diag3.scale = Scale{Note{Letter::A, Accidental::Natural}, findScaleDefinition("natural_minor")};
        diag3.traversalMode = ScaleTraversalMode::DiagonalShift;
        diag3.octaves = 3;
        diag3.startOnRoot = true;
        diag3.direction = ExerciseDirection::Ascending;
        diag3.pattern = ExercisePattern::Ascending;
        auto exDiag = generateScaleExercise(fb, diag3);
        CHECK(exDiag.steps.size() >= 15);
        CHECK(exDiag.steps.front().expectedMidiNote % 12 == 9); // Start od A (tonika)
        int spanSemitones = exDiag.steps.back().expectedMidiNote - exDiag.steps.front().expectedMidiNote;
        CHECK(spanSemitones >= 24); // Co najmniej 2+ oktawy w górę (do 3 oktaw)
    }

    // Tryb SingleString (poziomo na jednej strunie 0..24 progi)
    {
        WorkshopExerciseConfig singleCfg{};
        singleCfg.scale = cMajScale;
        singleCfg.traversalMode = ScaleTraversalMode::SingleString;
        singleCfg.startStringIndex = 0; // E6
        singleCfg.minFret = 0;
        singleCfg.maxFret = 24;
        singleCfg.octaves = 2;
        singleCfg.direction = ExerciseDirection::Ascending;
        singleCfg.pattern = ExercisePattern::Ascending;
        auto exSingle = generateScaleExercise(fb, singleCfg);
        CHECK(exSingle.steps.size() >= 10);
        for (const auto& st : exSingle.steps) {
            CHECK(st.targetPosition.stringIndex == 0);
        }
    }

    // Wszystkie nowe wzorce treningowe skal (GroupsOf6 .. StringSkipping)
    for (int p = static_cast<int>(ExercisePattern::GroupsOf6);
         p <= static_cast<int>(ExercisePattern::StringSkipping); ++p) {
        WorkshopExerciseConfig patCfg{};
        patCfg.scale = cMajScale;
        patCfg.positionSystem = PositionSystem::TNPS;
        patCfg.octaves = 2;
        patCfg.direction = ExerciseDirection::Ascending;
        patCfg.pattern = static_cast<ExercisePattern>(p);
        auto exPat = generateScaleExercise(fb, patCfg);
        CHECK(!exPat.steps.empty());
        CHECK(!exPat.song.tracks.empty());
    }

    // 3. Kształcenie Słuchu: pytania, odpowiedź przyciskiem, odpowiedź na gitarze i „Zaskocz mnie”
    EarTrainer ear;
    ear.setupSurpriseMeSession(5);
    CHECK(ear.config().activeIntervals.size() == 5);
    auto q1 = ear.nextQuestion();
    CHECK(!q1.songHint.empty());
    CHECK(ear.submitIntervalAnswer(q1.interval.semitones, 420.0));
    auto q2 = ear.nextQuestion();
    CHECK(ear.submitPlayedGuitarMidi(q2.secondPitch.midiNumber(), 510.0));
    CHECK(ear.completedRounds() == 2);
    CHECK(std::abs(ear.sessionAccuracyPercent() - 100.0) < 1e-6);

    // 4. Zmiany / Biblioteka: 10 standardów, transpozycja, 12 tonacji, reharmonizacja, podkład (Gitara + Bas)
    CHECK(builtinProgressions().size() == 10);
    auto blues = findBuiltinProgression("blues_12");
    CHECK(blues.has_value());
    CHECK(blues->sections.front().measures.size() == 12);

    auto reharmBlues = reharmonizeProgression(*blues);
    CHECK(reharmBlues.sections.front().measures[8].chords[0].romanNumeral == "♭II7");

    auto iiVi = findBuiltinProgression("ii_v_i_major");
    CHECK(iiVi.has_value());
    auto all12 = expandThroughAll12Keys(*iiVi);
    CHECK(all12.sections.size() == 12);

    auto backingSong = generateBackingTrackSong(*blues);
    CHECK(backingSong.tracks.size() == 2); // Gitara rytmiczna + Bas elektryczny (Walking Bass)
    CHECK(backingSong.tracks[0].measures.size() == 12);
    CHECK(backingSong.tracks[1].measures.size() == 12);

    // 5. Warsztat -> Sesje i Dashboard („Pierwszy tydzień”)
    SessionManager sessionMgr;
    CHECK(!sessionMgr.sessions().empty());
    sessionMgr.recordCompletedSession(20, 94.5);
    CHECK(sessionMgr.dashboardStats().todayMinutesPracticed == 20);
    CHECK(sessionMgr.dashboardStats().firstWeekPlanDayCompleted == 1);
}

void testGuitarNoteEngineCqtFluxHpsAndSustain() {
    using namespace gitara::dsp;
    constexpr double kSr = 48000.0;

    // Pomocnik: realistyczna harmoniczna struna gitarowa z naturalnym wygaszaniem (h1..h5)
    auto addPluckedString = [&](std::vector<float>& buf, double startSec, int midi, double decayTauSec, float amp = 0.25f) {
        const double f0 = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
        const std::size_t startIdx = static_cast<std::size_t>(std::llround(startSec * kSr));
        const double hAmps[5] = {1.0, 0.72, 0.45, 0.28, 0.16};
        for (std::size_t i = startIdx; i < buf.size(); ++i) {
            const double t = static_cast<double>(i - startIdx) / kSr;
            const double atk = std::min(1.0, t / 0.004);
            double s = 0.0;
            for (int h = 0; h < 5; ++h) {
                const double fh = f0 * static_cast<double>(h + 1);
                if (fh >= kSr * 0.45) break;
                const double env = std::exp(-t / (decayTauSec / (1.0 + 0.35 * h)));
                s += hAmps[h] * env * std::sin(2.0 * std::numbers::pi * fh * t);
            }
            buf[i] += static_cast<float>(amp * atk * s / 2.2);
        }
    };

    GuitarDspConfig cfg{};
    cfg.sampleRate = kSr;
    GuitarNoteEngine engine(cfg);

    // 1. Odrzucanie przydźwięku 50 Hz i niskiego uderzenia w pudło (< 60 Hz)
    {
        engine.reset();
        std::vector<float> hum(static_cast<std::size_t>(0.4 * kSr), 0.0f);
        for (std::size_t i = 0; i < hum.size(); ++i) {
            const double t = static_cast<double>(i) / kSr;
            hum[i] = 0.22f * static_cast<float>(std::sin(2.0 * std::numbers::pi * 50.0 * t)) +
                     0.18f * static_cast<float>(std::sin(2.0 * std::numbers::pi * 35.0 * t));
        }
        std::vector<NoteEvent> evs;
        engine.pushSamples(hum.data(), hum.size(), evs);
        CHECK(evs.empty());
    }

    // 2. Pełny zakres gryfu gitary (od Eb2 = 77.8 Hz aż po A5 = 880 Hz — dowód że LPF 4800 Hz nie zabija HPS!)
    for (int testMidi : {39, 40, 45, 50, 55, 59, 64, 76, 81}) {
        engine.reset();
        std::vector<float> pcm(static_cast<std::size_t>(0.35 * kSr), 0.0f);
        addPluckedString(pcm, 0.02, testMidi, 0.9, 0.28f);
        std::vector<NoteEvent> evs;
        engine.pushSamples(pcm.data(), pcm.size(), evs);
        bool sawExpectedOn = false;
        for (const auto& e : evs) {
            if (e.type == NoteEventType::NoteOn && e.midiNote == testMidi) {
                sawExpectedOn = true;
                CHECK(e.timestampMs < 110.0); // niska latencja detekcji ataku
                break;
            }
        }
        CHECK(sawExpectedOn);
    }

    // 3. KLUCZOWY SCENARIUSZ (Krok 1): Szybkie nakładające się nuty („dźwięk na dźwięku”)
    //    E2 (40) wybrzmiewa bardzo długo (tau = 2.5 s), a na jego tle co 130 ms uderzamy G2 (43), B2 (47), D3 (50), E3 (52)!
    {
        engine.reset();
        std::vector<float> pcm(static_cast<std::size_t>(1.05 * kSr), 0.0f);
        const std::vector<int> seq = {40, 43, 47, 50, 52};
        for (std::size_t i = 0; i < seq.size(); ++i) {
            addPluckedString(pcm, 0.03 + static_cast<double>(i) * 0.14, seq[i], 2.5, 0.26f);
        }
        std::vector<NoteEvent> evs;
        engine.pushSamples(pcm.data(), pcm.size(), evs);

        std::vector<int> detectedOnOrder;
        for (const auto& e : evs) {
            if (e.type == NoteEventType::NoteOn) {
                detectedOnOrder.push_back(e.midiNote);
            }
        }
        for (int targetM : seq) {
            bool found = std::find(detectedOnOrder.begin(), detectedOnOrder.end(), targetM) != detectedOnOrder.end();
            CHECK(found);
        }
    }

    // 4. Śledzenie podtrzymania (Sustain) oraz wygaszania -20 dB (Note Off + czas trzymania)
    {
        engine.reset();
        // tau = 0.22 s -> spadek o -20 dB (do 10% amplitudy) następuje przy t = 0.22 * ln(10) ≈ 506 ms!
        std::vector<float> pcm(static_cast<std::size_t>(1.10 * kSr), 0.0f);
        addPluckedString(pcm, 0.03, 57, 0.22, 0.30f); // A3 (220 Hz)
        std::vector<NoteEvent> evs;
        engine.pushSamples(pcm.data(), pcm.size(), evs);

        bool sawOn = false;
        bool sawOff = false;
        double heldDurationMs = 0.0;
        for (const auto& e : evs) {
            if (e.midiNote == 57 && e.type == NoteEventType::NoteOn) sawOn = true;
            if (e.midiNote == 57 && e.type == NoteEventType::NoteOff) {
                sawOff = true;
                heldDurationMs = e.durationMs;
            }
        }
        CHECK(sawOn);
        CHECK(sawOff);
        CHECK(heldDurationMs >= 320.0 && heldDurationMs <= 780.0);
        CHECK(engine.activeNotes().empty());
    }

    // 5. Re-triggering: ponowne uderzenie tej samej aktywnej struny A2 (45) podczas jej wybrzmiewania
    {
        engine.reset();
        std::vector<float> pcm(static_cast<std::size_t>(0.90 * kSr), 0.0f);
        addPluckedString(pcm, 0.03, 45, 0.45, 0.22f); // 1. uderzenie A2
        addPluckedString(pcm, 0.38, 45, 0.90, 0.32f); // 2. głośniejsze uderzenie A2 gdy 1. jeszcze wybrzmiewa!
        std::vector<NoteEvent> evs;
        engine.pushSamples(pcm.data(), pcm.size(), evs);

        int onCount = 0;
        bool sawRetriggerOff = false;
        bool sawRetriggerOn = false;
        for (const auto& e : evs) {
            if (e.midiNote == 45 && e.type == NoteEventType::NoteOn) {
                ++onCount;
                if (e.isRetrigger) sawRetriggerOn = true;
            }
            if (e.midiNote == 45 && e.type == NoteEventType::NoteOff && e.isRetrigger) {
                sawRetriggerOff = true;
            }
        }
        CHECK(onCount >= 2);
        CHECK(sawRetriggerOff);
        CHECK(sawRetriggerOn);
    }

    // 6. Równoczesny dwudźwięk / akord (E2 + B2 + G3 uderzone jednocześnie)
    {
        engine.reset();
        std::vector<float> pcm(static_cast<std::size_t>(0.40 * kSr), 0.0f);
        addPluckedString(pcm, 0.03, 40, 1.5, 0.24f); // E2
        addPluckedString(pcm, 0.03, 47, 1.5, 0.24f); // B2
        addPluckedString(pcm, 0.03, 55, 1.5, 0.24f); // G3
        std::vector<NoteEvent> evs;
        engine.pushSamples(pcm.data(), pcm.size(), evs);

        std::set<int> detectedSet;
        for (const auto& e : evs) {
            if (e.type == NoteEventType::NoteOn) detectedSet.insert(e.midiNote);
        }
        CHECK(detectedSet.count(40) == 1);
        CHECK(detectedSet.count(47) == 1);
        CHECK(detectedSet.count(55) == 1);
    }

    // 7. Kroki o półton na jednej strunie i skoki oktawowe przy tłumieniu dołu mikrofonu słuchawkowego (150 Hz HPF)
    {
        auto addRealMicNote = [&](std::vector<float>& buf, double startSec, double endSec, int midi, float amp) {
            const double f0 = 440.0 * std::pow(2.0, (static_cast<double>(midi) - 69.0) / 12.0);
            const std::size_t sIdx = static_cast<std::size_t>(std::llround(startSec * kSr));
            const std::size_t eIdx = std::min(buf.size(), static_cast<std::size_t>(std::llround(endSec * kSr)));
            const double hW[6] = {1.0, 1.35, 1.10, 0.75, 0.45, 0.25};
            for (std::size_t i = sIdx; i < eIdx; ++i) {
                const double t = static_cast<double>(i - sIdx) / kSr;
                const double env = std::min(1.0, t / 0.005) * std::exp(-t / 1.2);
                double s = 0.0;
                for (int h = 0; h < 6; ++h) {
                    const double fh = f0 * static_cast<double>(h + 1);
                    if (fh >= kSr * 0.45) break;
                    const double micGain = fh / std::sqrt(fh * fh + 150.0 * 150.0);
                    s += hW[h] * micGain * std::exp(-t * 0.35 * h) * std::sin(2.0 * std::numbers::pi * fh * t);
                }
                buf[i] += static_cast<float>(amp * env * s);
            }
        };

        const std::pair<int, int> semiPairs[5] = {{40, 41}, {45, 46}, {50, 51}, {55, 56}, {46, 45}};
        for (const auto& [m1, m2] : semiPairs) {
            engine.reset();
            std::vector<float> pcm(static_cast<std::size_t>(1.0 * kSr), 0.0f);
            addRealMicNote(pcm, 0.08, 0.36, m1, 0.25f);
            addRealMicNote(pcm, 0.36, 0.85, m2, 0.25f);
            std::vector<NoteEvent> evs;
            engine.pushSamples(pcm.data(), pcm.size(), evs);
            std::vector<int> ons;
            for (const auto& e : evs) {
                if (e.type == NoteEventType::NoteOn) ons.push_back(e.midiNote);
            }
            CHECK(ons.size() == 2);
            if (ons.size() == 2) {
                CHECK(ons[0] == m1);
                CHECK(ons[1] == m2);
            }
        }

        const std::array<int, 3> octSeqs[2] = {{40, 52, 64}, {64, 52, 40}};
        for (const auto& seq : octSeqs) {
            engine.reset();
            std::vector<float> pcm(static_cast<std::size_t>(1.2 * kSr), 0.0f);
            addRealMicNote(pcm, 0.08, 0.40, seq[0], 0.25f);
            addRealMicNote(pcm, 0.40, 0.72, seq[1], 0.25f);
            addRealMicNote(pcm, 0.72, 1.10, seq[2], 0.25f);
            std::vector<NoteEvent> evs;
            engine.pushSamples(pcm.data(), pcm.size(), evs);
            std::vector<int> ons;
            for (const auto& e : evs) {
                if (e.type == NoteEventType::NoteOn) ons.push_back(e.midiNote);
            }
            CHECK(ons.size() == 3);
            if (ons.size() == 3) {
                CHECK(ons[0] == seq[0]);
                CHECK(ons[1] == seq[1]);
                CHECK(ons[2] == seq[2]);
            }
        }

        // 8. 3-strunowe akordy brzmiące jednocześnie (Em, A5, D, G, C) — brak wirtualnej podstawy D2 i brak zrywania Sustainu!
        const std::array<int, 3> strumChords[5] = {
            {40, 47, 55}, {45, 52, 57}, {50, 57, 62}, {55, 59, 62}, {48, 52, 55}
        };
        for (const auto& ch : strumChords) {
            engine.reset();
            std::vector<float> pcm(static_cast<std::size_t>(0.65 * kSr), 0.0f);
            for (std::size_t idx = 0; idx < 3; ++idx) {
                const double f0 = 440.0 * std::pow(2.0, (static_cast<double>(ch[idx]) - 69.0) / 12.0);
                const std::size_t sIdx = static_cast<std::size_t>(std::llround((0.08 + idx * 0.020) * kSr));
                const double hW[6] = {1.0, 1.35, 1.10, 0.75, 0.45, 0.25};
                const double phase0 = std::fmod(static_cast<double>(ch[idx]) * 1.732, 2.0 * std::numbers::pi);
                for (std::size_t i = sIdx; i < pcm.size(); ++i) {
                    const double t = static_cast<double>(i - sIdx) / kSr;
                    const double env = std::min(1.0, t / 0.006) * std::exp(-t / 2.0);
                    double s = 0.0;
                    for (int h = 0; h < 6; ++h) {
                        const double fh = f0 * static_cast<double>(h + 1) * (1.0 + 0.00015 * h * h);
                        if (fh >= kSr * 0.45) break;
                        const double micGain = fh / std::sqrt(fh * fh + 150.0 * 150.0);
                        s += hW[h] * micGain * std::exp(-t * 0.35 * h) *
                             std::sin(2.0 * std::numbers::pi * fh * t + phase0 * static_cast<double>(h + 1));
                    }
                    pcm[i] += static_cast<float>(0.22 * env * s);
                }
            }
            std::vector<NoteEvent> evs;
            engine.pushSamples(pcm.data(), pcm.size(), evs);
            std::vector<int> active;
            for (const auto& a : engine.activeNotes()) active.push_back(a.midiNote);
            CHECK(active.size() == 3);
            if (active.size() == 3) {
                CHECK(active[0] == ch[0]);
                CHECK(active[1] == ch[1]);
                CHECK(active[2] == ch[2]);
            }
        }
    }
}

} // namespace

int main() {
    testTheoryConventionsAG();
    testFretboardMaskVoicingsAndHarmonics();
    testDspPitchChordAndSynth();
    testGuitarNoteEngineCqtFluxHpsAndSustain();
    testScoreTabRecorderAndTuxGuitarExport();
    testExerciseVerifierAndMetronome();
    testArpeggioWorkshopEarTrainerProgressionsAndSessions();

    std::cout << "========================================\n"
              << "Wynik testow: " << g_passed << " zaliczonych, "
              << g_failed << " bledow.\n"
              << "========================================\n";
    return (g_failed == 0) ? 0 : 1;
}
