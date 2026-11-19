#include "gitara/dsp/ChordDetector.h"
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

    // 1. Zakładka Arpeggio (FullNeck, CAGED, 2NPS, 3NPS, Sweep 3/5/6-str. + 8 przejść Sweep + diatoniczne)
    auto fullArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::FullNeck);
    auto cagedArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::CAGED, 0);
    auto twoNpsArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::TwoNPS, 0);
    auto threeNpsArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::ThreeNPS, 0);
    auto sweepArp = generateArpeggio(fb, cMaj7, ArpeggioPattern::Sweep, 0);
    CHECK(fullArp.size() > cagedArp.size());
    CHECK(!cagedArp.empty());
    CHECK(!twoNpsArp.empty());
    CHECK(!threeNpsArp.empty());
    CHECK(!sweepArp.empty());

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

    // 2. Warsztat: generowanie ćwiczeń sekwencyjnych (Grupy po 3, Grupy po 4, Skoki tercjowe)
    WorkshopExerciseConfig exCfg{};
    exCfg.scale = cMajScale;
    exCfg.positionSystem = PositionSystem::TNPS;
    exCfg.pattern = ExercisePattern::GroupsOf4;
    auto genEx = generateScaleExercise(fb, exCfg);
    CHECK(genEx.steps.size() == (18 - 3) * 4); // 15 grup czwórkowych z 18 dźwięków 3NPS = 60 kroków
    CHECK(!genEx.song.tracks.empty());

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

    // 4. Zmiany / Biblioteka: 9 standardów, transpozycja, 12 tonacji, reharmonizacja, podkład (Gitara + Bas)
    CHECK(builtinProgressions().size() == 9);
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

} // namespace

int main() {
    testTheoryConventionsAG();
    testFretboardMaskVoicingsAndHarmonics();
    testDspPitchChordAndSynth();
    testScoreTabRecorderAndTuxGuitarExport();
    testExerciseVerifierAndMetronome();
    testArpeggioWorkshopEarTrainerProgressionsAndSessions();

    std::cout << "========================================\n"
              << "Wynik testow: " << g_passed << " zaliczonych, "
              << g_failed << " bledow.\n"
              << "========================================\n";
    return (g_failed == 0) ? 0 : 1;
}
