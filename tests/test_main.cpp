#include "app/platform/AppPaths.h"
#include "app/platform/EmbeddedAssets.h"
#include "app/platform/UpdateLogic.h"
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
#include "gitara/score/StudioJsonCodec.h"
#include "gitara/score/UserLibrary.h"
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
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>
#include <set>
#include <string>
#include <vector>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

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

    // Weryfikacja kapodastra (Capo 2, 5, 7, 10) dla brzmiącego akordu D dur (D=2, F#=6, A=9)
    Chord dMajChord{Note{Letter::D, Accidental::Natural}, findChordDefinition("major")};
    for (int capo : {2, 5, 7, 10}) {
        auto capoMild = fb.generateChordVoicings(dMajChord, false, nullptr, VoicingSharpness::Mild, 0, capo);
        auto capoSharp = fb.generateChordVoicings(dMajChord, true, nullptr, VoicingSharpness::Sharp, 0, capo);
        auto capoTriad = fb.generateChordVoicings(dMajChord, false, nullptr, VoicingSharpness::Mild, 3, capo);
        CHECK(!capoMild.empty());
        CHECK(!capoSharp.empty());
        CHECK(!capoTriad.empty());
        bool hasCapoOpenFullTriad = false;
        for (const auto& v : capoMild) {
            bool hasD = false, hasFs = false, hasA = false, usesCapoStr = false;
            for (int s = 0; s < fb.stringCount(); ++s) {
                int fr = v.frets[static_cast<std::size_t>(s)];
                if (fr >= 0) {
                    CHECK(fr >= capo);
                    CHECK(fr <= 24);
                    if (fr == capo) usesCapoStr = true;
                    int pc = fb.midiAt(s, fr) % 12;
                    CHECK(pc == 2 || pc == 6 || pc == 9);
                    if (pc == 2) hasD = true;
                    if (pc == 6) hasFs = true;
                    if (pc == 9) hasA = true;
                }
            }
            CHECK(hasD && hasFs);
            if (usesCapoStr && hasD && hasFs && hasA) hasCapoOpenFullTriad = true;
        }
        CHECK(hasCapoOpenFullTriad);
        for (const auto& tv : capoTriad) {
            bool hasD = false, hasFs = false, hasA = false;
            for (int s = 0; s < fb.stringCount(); ++s) {
                int fr = tv.frets[static_cast<std::size_t>(s)];
                if (fr >= 0) {
                    CHECK(fr >= capo);
                    int pc = fb.midiAt(s, fr) % 12;
                    if (pc == 2) hasD = true;
                    if (pc == 6) hasFs = true;
                    if (pc == 9) hasA = true;
                }
            }
            CHECK(hasD && hasFs && hasA);
        }
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

void testStudioJsonCodecAndGp5V2() {
    using namespace gitara::score;

    const std::string json = R"JSON({
      "title": "Neon Studio Test",
      "artist": "Test Band",
      "album": "Studio V2",
      "tempoBpm": 132,
      "keyRootPc": 4,
      "keyIsMinor": true,
      "tracks": [
        {
          "name": "Gitara Lead",
          "instrumentType": "guitar",
          "isPercussion": false,
          "openStringMidis": [40, 45, 50, 55, 59, 64],
          "capo": 2,
          "midiChannel": 0,
          "midiProgram": 29,
          "measures": [
            {
              "numerator": 4,
              "denominator": 4,
              "repeatOpen": true,
              "repeatClose": 2,
              "voltaMask": 1,
              "sectionMarker": "Refren",
              "tripletFeel": 0,
              "beats": [
                {
                  "duration": 0,
                  "dots": 0,
                  "isRest": false,
                  "chordSymbol": "Em",
                  "notes": [
                    {
                      "string": 2,
                      "fret": 7,
                      "velocity": 110,
                      "hammerOn": true,
                      "palmMute": true,
                      "harmonicType": 1,
                      "slideType": "legato",
                      "bendPoints": [
                        {"position": 0, "value": 0, "vibrato": false},
                        {"position": 30, "value": 100, "vibrato": true},
                        {"position": 60, "value": 100, "vibrato": true}
                      ]
                    }
                  ]
                }
              ]
            }
          ]
        },
        {
          "name": "Perkusja GM",
          "instrumentType": "drums",
          "isPercussion": true,
          "midiChannel": 9,
          "measures": [
            {
              "numerator": 4,
              "denominator": 4,
              "beats": [
                {
                  "duration": 0,
                  "isRest": false,
                  "notes": [
                    {"string": 0, "fret": 36, "velocity": 120},
                    {"string": 1, "fret": 38, "velocity": 105}
                  ]
                }
              ]
            }
          ]
        }
      ]
    })JSON";

    Song song{};
    std::string err;
    CHECK(parseStudioSongJson(json, song, &err));
    CHECK(err.empty());
    CHECK(song.title == "Neon Studio Test");
    CHECK(song.tempoBpm == 132);
    CHECK(song.tracks.size() == 2);
    CHECK(song.tracks[0].capo == 2);
    CHECK(song.tracks[0].measures[0].repeatOpen);
    CHECK(song.tracks[0].measures[0].repeatClose == 2);
    CHECK(song.tracks[0].measures[0].voltaMask == 1);
    CHECK(song.tracks[0].measures[0].sectionMarker == "Refren");
    CHECK(song.tracks[0].measures[0].beats[0].notes[0].bendPoints.size() == 3);
    CHECK(song.tracks[1].isPercussion);
    CHECK(song.tracks[1].measures[0].beats[0].notes[0].fret == 36);

    const auto gp5 = exportToGp5Bytes(song);
    CHECK(gp5.size() > 512);
    CHECK(std::string(reinterpret_cast<const char*>(gp5.data() + 1), 24) == "FICHIER GUITAR PRO v5.00");
    std::string gp5AsText(reinterpret_cast<const char*>(gp5.data()), gp5.size());
    CHECK(gp5AsText.find("Refren") != std::string::npos);
    // Sprawdzamy, że MIDI bębnów (36/38) w perkusji GP5 nie zostały ucięte do 24
    CHECK(std::find(gp5.begin(), gp5.end(), static_cast<std::uint8_t>(36)) != gp5.end());
    CHECK(std::find(gp5.begin(), gp5.end(), static_cast<std::uint8_t>(38)) != gp5.end());

    const auto mid = exportToMidiBytes(song);
    CHECK(mid.size() > 64);
    CHECK(mid[0] == 'M' && mid[1] == 'T' && mid[2] == 'h' && mid[3] == 'd');
    // Kanał 10 perkusji (0x99) + Kick 36 (50+2 dla gitary w kanale 0 => 52 + capo 2 => fret 7 na D3=50 => 59)
    bool hasDrumCh10Kick = false;
    bool hasKeySigMeta = false;
    for (std::size_t i = 0; i + 2 < mid.size(); ++i) {
        if (mid[i] == 0x99 && mid[i + 1] == 36) hasDrumCh10Kick = true;
        if (mid[i] == 0xFF && mid[i + 1] == 0x59 && mid[i + 2] == 0x02) hasKeySigMeta = true;
    }
    CHECK(hasDrumCh10Kick);
    CHECK(hasKeySigMeta);

    Song bad{};
    std::string badErr;
    CHECK(!parseStudioSongJson("{broken", bad, &badErr));
    CHECK(!badErr.empty());
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

    // Zakres strun arpeggia (startStr / returnStr): skraca frazę i trzyma się wybranych strun (parzysta liczba nut pod metronom)
    {
        auto fullSeq = buildArpeggioSequenceSteps(fb, rootOct2, ArpeggioPattern::RootOctave, 0, AccidentalMode::Sharps, 0);
        auto cutSeq = buildArpeggioSequenceSteps(fb, rootOct2, ArpeggioPattern::RootOctave, 0, AccidentalMode::Sharps, 0, 2, 2);
        CHECK(!fullSeq.empty());
        CHECK(cutSeq.size() < fullSeq.size());
        for (const auto& st : cutSeq) {
            CHECK(st.stringIndex >= 2);
        }
        // Powrót do tej samej struny nie może zdublować nuty granicowej (unison na końcu zjazdu)
        if (cutSeq.size() >= 2) {
            CHECK(cutSeq.back().midi != cutSeq[0].midi);
        }
        // Kierunek „tylko w górę" z filtrem startStr również się skraca
        auto upFull = buildArpeggioSequenceSteps(fb, rootOct2, ArpeggioPattern::RootOctave, 1, AccidentalMode::Sharps, 0);
        auto upCut = buildArpeggioSequenceSteps(fb, rootOct2, ArpeggioPattern::RootOctave, 1, AccidentalMode::Sharps, 0, 2, -1);
        CHECK(upCut.size() < upFull.size());
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

    // Regresje kanonicznych kształtów Sweep (G#m w Eb Standard i Am w Standard, bez pomijanych dźwięków)
    {
        auto hasExactNodeSet = [](const SweepShape& sh, std::vector<std::pair<int, int>> expected) {
            if (sh.nodes.size() != expected.size()) return false;
            std::vector<std::pair<int, int>> actual;
            actual.reserve(sh.nodes.size());
            for (const auto& n : sh.nodes) {
                actual.emplace_back(n.position.stringIndex, n.position.fret);
            }
            std::sort(actual.begin(), actual.end());
            std::sort(expected.begin(), expected.end());
            return actual == expected;
        };

        auto hasNoSkippedChordTone = [&](const Fretboard& fbd, const Chord& ch, const SweepShape& sh) {
            std::vector<int> midis;
            for (const auto& n : sh.nodes) {
                midis.push_back(fbd.midiAt(n.position.stringIndex, n.position.fret));
            }
            std::sort(midis.begin(), midis.end());
            for (std::size_t i = 1; i < midis.size(); ++i) {
                for (int mm = midis[i - 1] + 1; mm < midis[i]; ++mm) {
                    if (ch.containsPitchClass(mm % 12)) return false;
                }
            }
            return true;
        };

        Fretboard fbEb{findTuning("eb_standard"), 24};
        Chord gSharpMin{Note{Letter::G, Accidental::Sharp}, findChordDefinition("minor")};
        auto gs5 = generateSweepShapes(fbEb, gSharpMin, 5, 0, AccidentalMode::Sharps);
        CHECK(!gs5.empty());
        bool foundGs5A = false;
        for (const auto& sh : gs5) {
            if (hasExactNodeSet(sh, {{1, 12}, {2, 10}, {3, 9}, {4, 10}, {5, 8}, {5, 12}})) {
                foundGs5A = true;
                CHECK(!sh.hasFingerRoll);
                CHECK(sh.label.find("od G# (1)") != std::string::npos);
                CHECK(hasNoSkippedChordTone(fbEb, gSharpMin, sh));
            }
        }
        CHECK(foundGs5A);

        auto am5 = generateSweepShapes(fb, aMinChord, 5, 0, AccidentalMode::Sharps);
        bool foundAm5A = false;
        for (const auto& sh : am5) {
            if (hasExactNodeSet(sh, {{1, 12}, {2, 10}, {3, 9}, {4, 10}, {5, 8}, {5, 12}})) {
                foundAm5A = true;
                CHECK(!sh.hasFingerRoll);
                CHECK(hasNoSkippedChordTone(fb, aMinChord, sh));
            }
        }
        CHECK(foundAm5A);

        auto gs6 = generateSweepShapes(fbEb, gSharpMin, 6, 0, AccidentalMode::Sharps);
        bool foundGs6E = false;
        bool foundGs6A = false;
        for (const auto& sh : gs6) {
            if (hasExactNodeSet(sh, {{0, 5}, {0, 8}, {1, 7}, {2, 7}, {3, 5}, {4, 5}, {5, 5}, {5, 8}})) {
                foundGs6E = true;
                CHECK(hasNoSkippedChordTone(fbEb, gSharpMin, sh));
            }
            if (hasExactNodeSet(sh, {{0, 8}, {0, 12}, {1, 12}, {2, 10}, {3, 9}, {4, 10}, {5, 8}, {5, 12}})) {
                foundGs6A = true;
                CHECK(hasNoSkippedChordTone(fbEb, gSharpMin, sh));
            }
        }
        CHECK(foundGs6E);
        CHECK(foundGs6A);

        // Wspinaczka diatoniczna (G# molowa naturalna w Eb Standard, 5 i 6 strun) - monotonicznie w górę gryfu
        Scale gsMinScale{Note{Letter::G, Accidental::Sharp}, findScaleDefinition("natural_minor")};
        for (int span : {5, 6}) {
            auto climb = generateSweepTransition(fbEb, "diatonic_climb", 8, span, 0, &gSharpMin, &gsMinScale, AccidentalMode::Sharps, false);
            CHECK(climb.segments.size() == 7);
            for (std::size_t i = 1; i < climb.segments.size(); ++i) {
                double prevC = 0.5 * (climb.segments[i - 1].minFret + climb.segments[i - 1].maxFret);
                double curC = 0.5 * (climb.segments[i].minFret + climb.segments[i].maxFret);
                CHECK(curC > prevC);
            }
        }

        // startPositionIndex (1-based: 1 -> shapes[0], 2 -> shapes[1])
        if (gs5.size() >= 2) {
            auto c1 = generateSweepTransition(fbEb, "diatonic_climb", 8, 5, 1, &gSharpMin, &gsMinScale, AccidentalMode::Sharps, false);
            auto c2 = generateSweepTransition(fbEb, "diatonic_climb", 8, 5, 2, &gSharpMin, &gsMinScale, AccidentalMode::Sharps, false);
            CHECK(!c1.segments.empty() && !c2.segments.empty());
            CHECK(c1.segments[0].minFret == gs5[0].minFret && c1.segments[0].maxFret == gs5[0].maxFret);
            CHECK(c2.segments[0].minFret == gs5[1].minFret && c2.segments[0].maxFret == gs5[1].maxFret);
        }

        // 4-strunowe sweepy (D-G-B-e oraz przesunięty zestaw basowy E-A-D-G z stringSetOffset = 2)
        Chord aMin7{Note{Letter::A, Accidental::Natural}, findChordDefinition("m7")};
        auto sw4High = generateSweepShapes(fb, aMin7, 4, 0, AccidentalMode::Sharps, nullptr, 0);
        CHECK(!sw4High.empty());
        for (const auto& n : sw4High[0].nodes) {
            CHECK(n.position.stringIndex >= 2 && n.position.stringIndex <= 5);
        }
        auto sw4Bass = generateSweepShapes(fb, aMin7, 4, 0, AccidentalMode::Sharps, nullptr, 2);
        CHECK(!sw4Bass.empty());
        for (const auto& n : sw4Bass[0].nodes) {
            CHECK(n.position.stringIndex >= 0 && n.position.stringIndex <= 3);
        }
        auto sw3Low = generateSweepShapes(fb, aMin7, 3, 1, AccidentalMode::Sharps, nullptr, 3);
        CHECK(!sw3Low.empty());
        for (const auto& n : sw3Low[0].nodes) {
            CHECK(n.position.stringIndex >= 0 && n.position.stringIndex <= 2);
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

void testAuditRegressions() {
    using namespace gitara::theory;
    using namespace gitara::score;
    using namespace gitara::engine;

    // 1. Messiaen 3 powinien zawierać m7 (10 półtonów), nie M6 (9)
    const ScaleDefinition* m3 = findScaleDefinition("messiaen_3");
    CHECK(m3 != nullptr);
    if (m3) {
        Scale sMess{Note{Letter::C, Accidental::Natural}, m3};
        CHECK(sMess.containsPitchClass(10));
        CHECK(!sMess.containsPitchClass(9));
    }

    // 2. d4 symbol/nazwa i P8 songHintAscending
    CHECK(intervals::d4.symbol(true) == "♭4");
    CHECK(intervals::d4.polishName() == "Kwarta zmniejszona");
    CHECK(!intervals::P8.songHintAscending().empty());

    // 3. spellAtLetter przy dużym odchyleniu chromatycznym zachowuje klasę wysokości
    Note extremeSpelled = spellAtLetter(Letter::C, 6); // F#
    CHECK(extremeSpelled.pitchClass() == 6);

    // 4. EarTrainer odrzuca zagranie interwału w złym kierunku (asc vs desc)
    EarTrainingConfig eCfg{};
    eCfg.direction = EarDirection::Ascending;
    eCfg.activeIntervals = {intervals::P5};
    EarTrainer earCheck(eCfg);
    auto eq = earCheck.nextQuestion();
    int wrongDirMidi = eq.rootPitch.midiNumber() - eq.interval.semitones;
    CHECK(!earCheck.submitPlayedGuitarMidi(wrongDirMidi, 250.0));

    // 5. detectKeyFromSong z kapodastrem + pomijanie perkusji i pauz (C dur -> tonika 0)
    Song keySong{};
    Track gtrTrack{};
    gtrTrack.tuning = findTuning("standard");
    gtrTrack.capo = 2; // Z capo 2: A(47)+1=48(C), D(52)+0=52(E), E4(66)+1=67(G)
    Measure kBar{};
    for (int i = 0; i < 4; ++i) {
        Beat b{};
        b.duration = DurationValue::Quarter;
        b.isRest = false;
        b.notes.push_back(TabNote{1, 1, 95}); // 45 + 2 + 1 = 48 (C)
        b.notes.push_back(TabNote{2, 0, 95}); // 50 + 2 + 0 = 52 (E)
        b.notes.push_back(TabNote{5, 1, 95}); // 64 + 2 + 1 = 67 (G)
        kBar.beats.push_back(b);
    }
    Beat restB{};
    restB.isRest = true;
    restB.notes.push_back(TabNote{0, 1, 95}); // resztkowe dane ignorowane przy isRest
    kBar.beats.push_back(restB);
    gtrTrack.measures.push_back(kBar);
    Track drumIgn{};
    drumIgn.isPercussion = true;
    Measure dBar{};
    Beat dBt{};
    dBt.notes.push_back(TabNote{0, 37, 100}); // C# perkusyjne nie psuje detekcji tonacji
    dBar.beats.push_back(dBt);
    drumIgn.measures.push_back(dBar);
    keySong.tracks = {gtrTrack, drumIgn};
    auto detKey = detectKeyFromSong(keySong);
    CHECK(detKey.root.pitchClass() == 0);
    CHECK(detKey.definition != nullptr && detKey.definition->id == "major");

    // 6. Arpeggio 2NPS dla Dm zwraca różne pozycje; Sweep na 4-str. basie oraz transition stringSetOffset działają poprawnie
    Fretboard fb6{findTuning("standard"), 24};
    Chord dmChord{Note{Letter::D, Accidental::Natural}, findChordDefinition("minor")};
    auto dm0 = generateArpeggio(fb6, dmChord, ArpeggioPattern::TwoNPS, 0);
    auto dm2 = generateArpeggio(fb6, dmChord, ArpeggioPattern::TwoNPS, 2);
    CHECK(!dm0.empty() && !dm2.empty());
    bool twoNpsDiff = (dm0.size() != dm2.size());
    if (!twoNpsDiff && !dm0.empty()) {
        for (std::size_t i = 0; i < dm0.size(); ++i) {
            if (dm0[i].position.fret != dm2[i].position.fret) { twoNpsDiff = true; break; }
        }
    }
    CHECK(twoNpsDiff);

    Tuning bassTuning{"bass_standard", "Bas 4-str (E A D G)", {
        Pitch{{Letter::E, Accidental::Natural}, 1},
        Pitch{{Letter::A, Accidental::Natural}, 1},
        Pitch{{Letter::D, Accidental::Natural}, 2},
        Pitch{{Letter::G, Accidental::Natural}, 2}
    }};
    Fretboard fbBass4{bassTuning, 24};
    auto bassSw = generateSweepShapes(fbBass4, dmChord, 5, 0, AccidentalMode::Sharps);
    CHECK(!bassSw.empty());
    for (const auto& sh : bassSw) {
        for (const auto& n : sh.nodes) {
            CHECK(n.position.stringIndex >= 0 && n.position.stringIndex < 4);
        }
    }
    Scale dmScale{Note{Letter::D, Accidental::Natural}, findScaleDefinition("natural_minor")};
    auto swTrUp = generateSweepTransition(fbBass4, "diatonic_climb", 2, 4, 0, &dmChord, &dmScale, AccidentalMode::Sharps, false, 0, 2);
    CHECK(!swTrUp.segments.empty());
    for (const auto& seg : swTrUp.segments) {
        for (const auto& n : seg.nodes) {
            CHECK(n.position.stringIndex >= 0 && n.position.stringIndex < 4);
        }
    }

    // 7. GP5 4-strunowy bas + MIDI Time Signature (FF 58 04)
    Song b4Song{};
    b4Song.tempoBpm = 115;
    Track b4{};
    b4.name = "Bass 4";
    b4.midiChannel = 1;
    b4.midiProgram = 33;
    b4.tuning = bassTuning;
    Measure b4m{};
    b4m.numerator = 7;
    b4m.denominator = 8;
    Beat b4bt{};
    b4bt.duration = DurationValue::Eighth;
    b4bt.notes.push_back(TabNote{0, 3, 100}); // G1 (28+3=31) na najniższej strunie basu
    b4m.beats.push_back(b4bt);
    b4.measures.push_back(b4m);
    b4Song.tracks.push_back(b4);
    const auto gp5B4 = exportToGp5Bytes(b4Song);
    CHECK(gp5B4.size() > 256);
    const auto midB4 = exportToMidiBytes(b4Song);
    bool has7_8TimeSig = false;
    for (std::size_t i = 0; i + 4 < midB4.size(); ++i) {
        if (midB4[i] == 0xFF && midB4[i + 1] == 0x58 && midB4[i + 2] == 0x04 &&
            midB4[i + 3] == 7 && midB4[i + 4] == 3) {
            has7_8TimeSig = true;
            break;
        }
    }
    CHECK(has7_8TimeSig);
}

void testUserLibraryPersistenceAndSecurity() {
    using namespace gitara::score;

#ifdef _WIN32
    const auto pid = static_cast<long>(_getpid());
#else
    const auto pid = static_cast<long>(::getpid());
#endif
    const std::filesystem::path tmpRoot =
        std::filesystem::temp_directory_path() / ("apka_gitara_test_lib_" + std::to_string(pid));
    std::error_code ec;
    std::filesystem::remove_all(tmpRoot, ec);

    CHECK(isAllowedLibraryCategory("sweeps"));
    CHECK(isAllowedLibraryCategory("progressions"));
    CHECK(isAllowedLibraryCategory("tabs"));
    CHECK(isAllowedLibraryCategory("studio"));
    CHECK(isAllowedLibraryCategory("sessions"));
    CHECK(!isAllowedLibraryCategory("other"));
    CHECK(!isAllowedLibraryCategory("../etc"));

    CHECK(sanitizeLibraryId("my_sweep-01") == "my_sweep-01");
    CHECK(sanitizeLibraryId("../evil").empty());
    CHECK(sanitizeLibraryId("a/b").empty());
    CHECK(sanitizeLibraryId("..").empty());
    CHECK(sanitizeLibraryId("a.b").empty());
    CHECK(sanitizeLibraryId("").empty());

    std::string err;
    CHECK(saveLibraryItem(tmpRoot, "sweeps", "sw1", "{\"id\":\"sw1\",\"name\":\"Sweep Alpha\",\"items\":[{}]}", &err));
    CHECK(saveLibraryItem(tmpRoot, "progressions", "pr1", "{\"id\":\"pr1\",\"title\":\"Prog One\",\"measures\":[{}]}", &err));
    CHECK(saveLibraryItem(tmpRoot, "tabs", "tb1", "{\"id\":\"tb1\",\"title\":\"Tab One\",\"recordedEvents\":[{}]}", &err));
    CHECK(saveLibraryItem(tmpRoot, "studio", "st1", "{\"id\":\"st1\",\"name\":\"Studio One\",\"song\":{\"tracks\":[{}]}}", &err));

    const std::string listed = listAllLibraryItemsJson(tmpRoot);
    CHECK(listed.find("\"sweeps\":[") != std::string::npos);
    CHECK(listed.find("\"progressions\":[") != std::string::npos);
    CHECK(listed.find("\"tabs\":[") != std::string::npos);
    CHECK(listed.find("\"studio\":[") != std::string::npos);
    CHECK(listed.find("Sweep Alpha") != std::string::npos);
    CHECK(listed.find("Prog One") != std::string::npos);
    CHECK(listed.find("Tab One") != std::string::npos);
    CHECK(listed.find("Studio One") != std::string::npos);

    // Atomowe nadpisanie pod tym samym ID
    CHECK(saveLibraryItem(tmpRoot, "sweeps", "sw1", "{\"id\":\"sw1\",\"name\":\"Sweep Updated\",\"items\":[{}]}", &err));
    const std::string listedUpdated = listAllLibraryItemsJson(tmpRoot);
    CHECK(listedUpdated.find("Sweep Updated") != std::string::npos);
    CHECK(listedUpdated.find("Sweep Alpha") == std::string::npos);

    // Bezpieczeństwo: odrzucanie path traversal, złej kategorii i nie-obiektowego JSON
    CHECK(!saveLibraryItem(tmpRoot, "sweeps", "../evil", "{\"id\":\"x\"}", &err));
    CHECK(!saveLibraryItem(tmpRoot, "sweeps", "a/b", "{\"id\":\"x\"}", &err));
    CHECK(!saveLibraryItem(tmpRoot, "unknown", "id1", "{\"id\":\"x\"}", &err));
    CHECK(!saveLibraryItem(tmpRoot, "sweeps", "badjson", "not json", &err));
    CHECK(!saveLibraryItem(tmpRoot, "sweeps", "badarr", "[1,2,3]", &err));

    // Usuwanie
    CHECK(deleteLibraryItem(tmpRoot, "sweeps", "sw1", &err));
    CHECK(!deleteLibraryItem(tmpRoot, "sweeps", "sw1", &err));
    CHECK(listAllLibraryItemsJson(tmpRoot).find("\"id\":\"sw1\"") == std::string::npos);

    std::filesystem::remove_all(tmpRoot, ec);
}

void testPlatformUpdaterAndEmbeddedAssets() {
    using namespace gitara::platform;

    // 1. Wektory testowe SHA-256 (NIST FIPS 180-4)
    CHECK(sha256Hex("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CHECK(sha256Hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CHECK(sha256Hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") ==
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");

    // Wielokawałkowe hashowanie 100 KB strumienia
    const std::string bigData(100000, 'a');
    const std::string expectedBig = sha256Hex(bigData);
    Sha256 streamed;
    std::size_t off = 0;
    const std::size_t chunkLens[] = {1, 7, 63, 64, 65, 513, 4096};
    std::size_t ci = 0;
    while (off < bigData.size()) {
        const std::size_t take = std::min(chunkLens[ci % 7], bigData.size() - off);
        streamed.update(bigData.data() + off, take);
        off += take;
        ++ci;
    }
    CHECK(streamed.finalHex() == expectedBig);

    // 2. Parser manifest.json
    const std::string validCommit = "0123456789abcdef0123456789abcdef01234567";
    const std::string newerCommit = "fedcba9876543210fedcba9876543210fedcba98";
    const std::string validSha = "a1b2c3d4e5f60718293a4b5c6d7e8f90a1b2c3d4e5f60718293a4b5c6d7e8f90";
    const std::string validUrl = "https://github.com/AlKrul/gitara-apka/releases/download/latest/ApkaGitarowa.exe";
    const std::string manifestJson =
        "{\n"
        "  \"schema\": 1,\n"
        "  \"version\": \"0.1.0\",\n"
        "  \"commit\": \"" + newerCommit + "\",\n"
        "  \"shortCommit\": \"fedcba9\",\n"
        "  \"builtAt\": \"2026-11-27T00:00:00Z\",\n"
        "  \"assetName\": \"ApkaGitarowa.exe\",\n"
        "  \"downloadUrl\": \"" + validUrl + "\",\n"
        "  \"sha256\": \"" + validSha + "\",\n"
        "  \"size\": 4567890\n"
        "}\n";
    auto parsed = parseUpdateManifest(manifestJson);
    CHECK(parsed.has_value());
    if (parsed.has_value()) {
        CHECK(parsed->schema == 1);
        CHECK(parsed->version == "0.1.0");
        CHECK(parsed->commit == newerCommit);
        CHECK(parsed->downloadUrl == validUrl);
        CHECK(parsed->sha256 == validSha);
        CHECK(parsed->size == 4567890ull);
    }

    CHECK(!parseUpdateManifest("{\"schema\":2,\"version\":\"0.1.0\",\"commit\":\"" + newerCommit + "\",\"downloadUrl\":\"" + validUrl + "\",\"sha256\":\"" + validSha + "\"}").has_value());
    CHECK(!parseUpdateManifest("{\"schema\":1,\"version\":\"0.1.0\",\"downloadUrl\":\"" + validUrl + "\"}").has_value());
    CHECK(!parseUpdateManifest("not json").has_value());
    CHECK(!parseUpdateManifest("{\"schema\":1,\"version\":\"0.1.0\",}").has_value());

    // 3. Walidacja adresów pobierania
    CHECK(isAllowedDownloadUrl(validUrl, "AlKrul/gitara-apka"));
    CHECK(isAllowedDownloadUrl("https://github.com/alkrul/GITARA-APKA/releases/latest/download/ApkaGitarowa.exe", "AlKrul/gitara-apka"));
    CHECK(isAllowedDownloadUrl("https://objects.githubusercontent.com/github-production-release-asset/12345", "AlKrul/gitara-apka"));
    CHECK(!isAllowedDownloadUrl("http://github.com/AlKrul/gitara-apka/releases/download/latest/ApkaGitarowa.exe", "AlKrul/gitara-apka"));
    CHECK(!isAllowedDownloadUrl("https://github.com/OtherUser/gitara-apka/releases/download/latest/ApkaGitarowa.exe", "AlKrul/gitara-apka"));
    CHECK(!isAllowedDownloadUrl("https://github.com.evil.com/AlKrul/gitara-apka/releases/download/latest/ApkaGitarowa.exe", "AlKrul/gitara-apka"));
    CHECK(!isAllowedDownloadUrl("https://user@github.com/AlKrul/gitara-apka/releases/download/latest/ApkaGitarowa.exe", "AlKrul/gitara-apka"));
    CHECK(!isAllowedDownloadUrl("https://evil.example/ApkaGitarowa.exe", "AlKrul/gitara-apka"));

    // 4. Decyzje auto-update
    CHECK(parsed.has_value());
    if (parsed.has_value()) {
        CHECK(evaluateUpdate("dev", *parsed, "AlKrul/gitara-apka", false) == UpdateCheckDecision::SkippedDevBuild);
        CHECK(evaluateUpdate("dev", *parsed, "AlKrul/gitara-apka", true) == UpdateCheckDecision::UpdateAvailable);
        CHECK(evaluateUpdate(validCommit, *parsed, "AlKrul/gitara-apka") == UpdateCheckDecision::UpdateAvailable);
        CHECK(evaluateUpdate(newerCommit, *parsed, "AlKrul/gitara-apka") == UpdateCheckDecision::UpToDate);
        CHECK(evaluateUpdate("FEDCBA9876543210FEDCBA9876543210FEDCBA98", *parsed, "AlKrul/gitara-apka") == UpdateCheckDecision::UpToDate);

        UpdateManifest badSha = *parsed;
        badSha.sha256 = "abcd";
        CHECK(evaluateUpdate(validCommit, badSha, "AlKrul/gitara-apka") == UpdateCheckDecision::InvalidManifest);

        UpdateManifest badHost = *parsed;
        badHost.downloadUrl = "https://evil.example/ApkaGitarowa.exe";
        CHECK(evaluateUpdate(validCommit, badHost, "AlKrul/gitara-apka") == UpdateCheckDecision::InvalidManifest);
    }

    // 5. Parser GITAPAK1 dla zasobów wbudowanych
    auto appendU32 = [](std::vector<std::uint8_t>& buf, std::uint32_t v) {
        buf.push_back(static_cast<std::uint8_t>(v & 0xffu));
        buf.push_back(static_cast<std::uint8_t>((v >> 8) & 0xffu));
        buf.push_back(static_cast<std::uint8_t>((v >> 16) & 0xffu));
        buf.push_back(static_cast<std::uint8_t>((v >> 24) & 0xffu));
    };
    auto appendU64 = [](std::vector<std::uint8_t>& buf, std::uint64_t v) {
        for (int i = 0; i < 8; ++i) {
            buf.push_back(static_cast<std::uint8_t>((v >> (i * 8)) & 0xffu));
        }
    };
    auto appendStr = [](std::vector<std::uint8_t>& buf, std::string_view s) {
        buf.insert(buf.end(), s.begin(), s.end());
    };

    const std::string p1 = "/index.html";
    const std::string m1 = "text/html; charset=utf-8";
    const std::string d1 = "<html>ok</html>";
    const std::string p2 = "/web/test.js";
    const std::string m2 = "application/javascript; charset=utf-8";
    const std::string d2 = "export const x = 42;";

    const std::uint64_t tocBytes =
        (4 + p1.size() + 4 + m1.size() + 16) +
        (4 + p2.size() + 4 + m2.size() + 16);
    const std::uint64_t off1 = 12 + tocBytes;
    const std::uint64_t off2 = off1 + d1.size();

    std::vector<std::uint8_t> pak;
    appendStr(pak, "GITAPAK1");
    appendU32(pak, 2);
    appendU32(pak, static_cast<std::uint32_t>(p1.size()));
    appendStr(pak, p1);
    appendU32(pak, static_cast<std::uint32_t>(m1.size()));
    appendStr(pak, m1);
    appendU64(pak, off1);
    appendU64(pak, d1.size());
    appendU32(pak, static_cast<std::uint32_t>(p2.size()));
    appendStr(pak, p2);
    appendU32(pak, static_cast<std::uint32_t>(m2.size()));
    appendStr(pak, m2);
    appendU64(pak, off2);
    appendU64(pak, d2.size());
    appendStr(pak, d1);
    appendStr(pak, d2);

    EmbeddedAssetPack pack;
    CHECK(pack.parse(pak.data(), pak.size()));
    CHECK(pack.count() == 2);
    auto v1 = pack.find("/index.html");
    CHECK(v1.has_value() && v1->body == d1 && v1->mime == m1);
    auto v2 = pack.find("/web/test.js");
    CHECK(v2.has_value() && v2->body == d2 && v2->mime == m2);
    CHECK(!pack.find("/missing.js").has_value());

    // Uszkodzone paczki musza byc bezpiecznie odrzucane
    std::vector<std::uint8_t> badMagic = pak;
    badMagic[0] = 'X';
    CHECK(!pack.parse(badMagic.data(), badMagic.size()));
    CHECK(pack.empty());

    std::vector<std::uint8_t> truncated(pak.begin(), pak.begin() + 14);
    CHECK(!pack.parse(truncated.data(), truncated.size()));

    std::vector<std::uint8_t> hugeCount = pak;
    hugeCount[8] = 0xff;
    hugeCount[9] = 0xff;
    hugeCount[10] = 0xff;
    hugeCount[11] = 0x7f;
    CHECK(!pack.parse(hugeCount.data(), hugeCount.size()));

    // 6. Rozwiazywanie sciezek aplikacji
    std::error_code ec;
    const std::filesystem::path tmpBase = std::filesystem::temp_directory_path() / "gitara_paths_test";
    std::filesystem::remove_all(tmpBase, ec);
    const std::filesystem::path fakeExe = tmpBase / "bin" / "ApkaGitarowa.exe";
    const std::filesystem::path emptyCwd = tmpBase / "empty_cwd";
    std::filesystem::create_directories(fakeExe.parent_path(), ec);
    std::filesystem::create_directories(emptyCwd, ec);

    const AppPaths portablePaths = resolveAppPaths(true, fakeExe, emptyCwd);
    CHECK(!portablePaths.isDevCheckout);
    CHECK(portablePaths.dataDir == fakeExe.parent_path());
    CHECK(portablePaths.settingsFile == fakeExe.parent_path() / "settings.json");
    CHECK(portablePaths.userLibraryDir == fakeExe.parent_path() / "user_library");

    const std::filesystem::path fakeDevCwd = tmpBase / "dev_repo";
    std::filesystem::create_directories(fakeDevCwd / "web" / "js" / "dsp", ec);
    {
        std::ofstream ofs(fakeDevCwd / "web" / "js" / "dsp" / "guitarDspEngine.js");
        ofs << "// test\n";
    }
    CHECK(isDevCheckoutDir(fakeDevCwd));
    CHECK(!isDevCheckoutDir(emptyCwd));
#ifndef _WIN32
    const AppPaths devPaths = resolveAppPaths(false, fakeExe, fakeDevCwd);
    CHECK(devPaths.isDevCheckout);
    CHECK(devPaths.settingsFile == fakeDevCwd / ".apka_gitara_settings.json");
    CHECK(devPaths.userLibraryDir == fakeDevCwd / "user_library");
#endif
    std::filesystem::remove_all(tmpBase, ec);
}

} // namespace

int main() {
    testTheoryConventionsAG();
    testFretboardMaskVoicingsAndHarmonics();
    testDspPitchChordAndSynth();
    testGuitarNoteEngineCqtFluxHpsAndSustain();
    testScoreTabRecorderAndTuxGuitarExport();
    testStudioJsonCodecAndGp5V2();
    testExerciseVerifierAndMetronome();
    testArpeggioWorkshopEarTrainerProgressionsAndSessions();
    testAuditRegressions();
    testUserLibraryPersistenceAndSecurity();
    testPlatformUpdaterAndEmbeddedAssets();

    std::cout << "========================================\n"
              << "Wynik testow: " << g_passed << " zaliczonych, "
              << g_failed << " bledow.\n"
              << "========================================\n";
    return (g_failed == 0) ? 0 : 1;
}
