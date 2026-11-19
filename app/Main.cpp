#include "app/MainWindow.h"
#include "app/ui/DesktopGuiHtml.h"
#include "gitara/engine/EarTrainer.h"
#include "gitara/engine/ProgressionEngine.h"
#include "gitara/engine/SessionManager.h"
#include "gitara/engine/WorkshopGenerator.h"
#include "gitara/score/Score.h"
#include "gitara/theory/Arpeggio.h"
#include "gitara/theory/Chord.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)

namespace gitara::app {

class GitaraApplication final : public juce::JUCEApplication {
public:
    const juce::String getApplicationName() override { return "Apka Gitarowa"; }
    const juce::String getApplicationVersion() override { return "0.3.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        mainWindow_ = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override {
        mainWindow_.reset();
    }

    void systemRequestedQuit() override {
        quit();
    }

    class MainWindow final : public juce::DocumentWindow {
    public:
        explicit MainWindow(const juce::String& name)
            : DocumentWindow(name,
                             juce::Colour(0xFF14110F),
                             DocumentWindow::allButtons) {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainContentComponent(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow_;
};

} // namespace gitara::app

START_JUCE_APPLICATION(gitara::app::GitaraApplication)

#else

#include <arpa/inet.h>
#include <cctype>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

namespace {

using namespace gitara::theory;
using namespace gitara::engine;
using namespace gitara::score;

std::string jsonEscape(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') continue;
        else out.push_back(c);
    }
    return out;
}

std::string urlDecode(std::string_view in) {
    std::string out;
    for (std::size_t i = 0; i < in.size(); ++i) {
        if (in[i] == '%' && i + 2 < in.size()) {
            char hex[3] = {in[i + 1], in[i + 2], 0};
            out.push_back(static_cast<char>(std::strtol(hex, nullptr, 16)));
            i += 2;
        } else if (in[i] == '+') {
            out.push_back(' ');
        } else {
            out.push_back(in[i]);
        }
    }
    return out;
}

std::string getQueryParam(std::string_view query, std::string_view key, std::string_view defVal = "") {
    std::string pattern = std::string(key) + "=";
    std::size_t pos = 0;
    while (pos < query.size()) {
        std::size_t amp = query.find('&', pos);
        std::string_view pair = (amp == std::string_view::npos)
            ? query.substr(pos)
            : query.substr(pos, amp - pos);
        if (pair.substr(0, pattern.size()) == pattern) {
            return urlDecode(pair.substr(pattern.size()));
        }
        if (amp == std::string_view::npos) break;
        pos = amp + 1;
    }
    return std::string(defVal);
}

int getQueryInt(std::string_view query, std::string_view key, int defVal) {
    auto s = getQueryParam(query, key);
    if (s.empty()) return defVal;
    return std::atoi(s.c_str());
}

AccidentalMode toAccMode(int m) {
    if (m == 1) return AccidentalMode::Flats;
    if (m == 2) return AccidentalMode::Sharps;
    return AccidentalMode::Auto;
}

Tuning resolveRequestedTuning(std::string_view query) {
    std::string tuningId = getQueryParam(query, "tuning", "standard");
    std::string customStr = getQueryParam(query, "customMidis", "");
    Tuning t = findTuning(tuningId);
    if (!customStr.empty()) {
        std::vector<Pitch> customPitches;
        std::size_t pos = 0;
        while (pos < customStr.size()) {
            std::size_t comma = customStr.find(',', pos);
            std::string tok = (comma == std::string::npos)
                ? customStr.substr(pos)
                : customStr.substr(pos, comma - pos);
            int midi = std::clamp(std::atoi(tok.c_str()), 24, 88);
            customPitches.push_back(Pitch::fromMidiNumber(midi, AccidentalMode::Sharps));
            if (comma == std::string::npos) break;
            pos = comma + 1;
        }
        if (customPitches.size() == 6) {
            t.id = "custom";
            t.polishName = "Własny strój";
            t.openStrings = std::move(customPitches);
        }
    }
    return t;
}

std::string handleInitApi() {
    std::ostringstream oss;
    oss << "{";

    oss << "\"scales\":[";
    const auto& scales = allScaleDefinitions();
    for (std::size_t i = 0; i < scales.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << jsonEscape(scales[i].id)
            << "\",\"name\":\"" << jsonEscape(scales[i].polishName)
            << "\",\"category\":\"" << jsonEscape(scales[i].category) << "\"}";
    }
    oss << "],";

    oss << "\"chords\":[";
    const auto& chords = allChordDefinitions();
    for (std::size_t i = 0; i < chords.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << jsonEscape(chords[i].id)
            << "\",\"name\":\"" << jsonEscape(chords[i].polishName)
            << "\",\"category\":\"" << jsonEscape(chords[i].category) << "\"}";
    }
    oss << "],";

    oss << "\"tunings\":[";
    const auto& tunings = builtinTunings();
    for (std::size_t i = 0; i < tunings.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << jsonEscape(tunings[i].id)
            << "\",\"name\":\"" << jsonEscape(tunings[i].polishName)
            << "\",\"midis\":[";
        for (int s = 0; s < tunings[i].stringCount(); ++s) {
            if (s > 0) oss << ",";
            oss << tunings[i].openStrings[static_cast<std::size_t>(s)].midiNumber();
        }
        oss << "]}";
    }
    oss << "],";

    oss << "\"intervals\":[";
    const auto& ivs = earTrainingIntervals();
    for (std::size_t i = 0; i < ivs.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"semitones\":" << ivs[i].semitones
            << ",\"symbol\":\"" << jsonEscape(ivs[i].symbol(true))
            << "\",\"shortCode\":\"" << jsonEscape(ivs[i].shortCode())
            << "\",\"name\":\"" << jsonEscape(ivs[i].polishName())
            << "\",\"hint\":\"" << jsonEscape(ivs[i].songHintAscending()) << "\"}";
    }
    oss << "],";

    oss << "\"progressions\":[";
    const auto& progs = builtinProgressions();
    for (std::size_t i = 0; i < progs.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& p = progs[i];
        oss << "{\"id\":\"" << jsonEscape(p.id)
            << "\",\"title\":\"" << jsonEscape(p.title)
            << "\",\"style\":\"" << jsonEscape(p.styleCategory)
            << "\",\"keyRootPc\":" << p.keyRoot.pitchClass()
            << ",\"isMinor\":" << (p.isMinorKey ? "true" : "false")
            << ",\"swing\":" << static_cast<int>(p.swing)
            << ",\"groove\":" << static_cast<int>(p.groove)
            << ",\"cycleKeys\":" << (p.cycleThroughKeys ? "true" : "false")
            << ",\"bpm\":" << p.tempoBpm << ",\"bars\":[";
        if (!p.sections.empty()) {
            const auto& measures = p.sections.front().measures;
            for (std::size_t m = 0; m < measures.size(); ++m) {
                if (m > 0) oss << ",";
                const auto& slot = measures[m].chords.front();
                auto ch = slot.resolveInKey(p.keyRoot, AccidentalMode::Sharps);
                Pitch rootP{ch.root, 3};
                oss << "{\"symbol\":\"" << jsonEscape(ch.symbol(false, AccidentalMode::Sharps))
                    << "\",\"roman\":\"" << jsonEscape(slot.romanNumeral)
                    << "\",\"offsetSemi\":" << slot.rootOffsetFromKey.semitones
                    << ",\"chordId\":\"" << jsonEscape(slot.chordDefId) << "\""
                    << ",\"freq\":" << rootP.frequencyHz() << "}";
            }
        }
        oss << "]}";
    }
    oss << "],";

    SessionManager sm;
    oss << "\"sessions\":[";
    const auto& sessions = sm.sessions();
    for (std::size_t i = 0; i < sessions.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << jsonEscape(sessions[i].id)
            << "\",\"name\":\"" << jsonEscape(sessions[i].name)
            << "\",\"breakBars\":" << sessions[i].breakBars << "}";
    }
    oss << "]}";
    return oss.str();
}

std::string handleFretboardApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string scaleId = getQueryParam(query, "scaleId", "major");
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));
    int posSysInt = getQueryInt(query, "posSystem", 0);
    int posIdx = getQueryInt(query, "posIdx", 0);

    Fretboard fb{resolveRequestedTuning(query), 24};
    const auto* scaleDef = findScaleDefinition(scaleId);
    if (!scaleDef) scaleDef = findScaleDefinition("major");
    Scale scale{Note::fromPitchClass(rootPc, mode), scaleDef};

    auto activePositions = fb.generateScalePositions(
        scale, static_cast<PositionSystem>(posSysInt), posIdx);

    auto isInPos = [&](int s, int f) {
        return std::any_of(activePositions.begin(), activePositions.end(),
                           [&](const FretPosition& p) { return p.stringIndex == s && p.fret == f; });
    };

    std::ostringstream oss;
    oss << "{\"scaleFullName\":\"" << jsonEscape(scale.fullName(false)) << "\",\"scaleNotes\":[";
    auto sNotes = scale.notes(mode);
    for (std::size_t i = 0; i < sNotes.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "\"" << jsonEscape(sNotes[i].toString(false)) << "\"";
    }
    oss << "],\"scaleDegrees\":[";
    auto diatonicTriads = generateDiatonicArpeggios(scale, false, mode);
    auto scaleFuncName = [](int semitones) -> const char* {
        switch (((semitones % 12) + 12) % 12) {
            case 0:  return "Tonika (I)";
            case 1:  return "Stopień frygijski (♭II)";
            case 2:  return "Supertonika (II)";
            case 3:  return "Medianta molowa (♭III)";
            case 4:  return "Medianta durowa (III)";
            case 5:  return "Subdominanta (IV)";
            case 6:  return "Tryton / Kwarta lidyjska (♯IV/♭V)";
            case 7:  return "Dominanta (V)";
            case 8:  return "Submedianta molowa (♭VI)";
            case 9:  return "Submedianta durowa (VI)";
            case 10: return "Subtonika (♭VII)";
            case 11: return "Dźwięk prowadzący (VII)";
            default: return "Stopień skali";
        }
    };
    for (std::size_t i = 0; i < sNotes.size(); ++i) {
        if (i > 0) oss << ",";
        Interval iv = (i < scaleDef->intervals.size()) ? scaleDef->intervals[i] : intervals::P1;
        std::string triadSym;
        std::string triadRoman;
        if (i < diatonicTriads.size()) {
            triadSym = diatonicTriads[i].chord.symbol(false, mode);
            triadRoman = diatonicTriads[i].romanNumeral;
        }
        int degPc = ((scale.root.pitchClass() + iv.semitones) % 12 + 12) % 12;
        oss << "{\"note\":\"" << jsonEscape(sNotes[i].toString(false)) << "\""
            << ",\"pc\":" << degPc
            << ",\"semitones\":" << iv.semitones
            << ",\"deg\":" << (i + 1)
            << ",\"interval\":\"" << jsonEscape(iv.symbol(true)) << "\""
            << ",\"intervalName\":\"" << jsonEscape(iv.polishName()) << "\""
            << ",\"functionName\":\"" << jsonEscape(scaleFuncName(iv.semitones)) << "\""
            << ",\"triad\":\"" << jsonEscape(triadSym) << "\""
            << ",\"roman\":\"" << jsonEscape(triadRoman) << "\"}";
    }
    oss << "],\"cells\":[";

    bool first = true;
    for (int s = 0; s < fb.stringCount(); ++s) {
        for (int f = 0; f <= fb.fretCount(); ++f) {
            if (!first) oss << ",";
            first = false;
            int midi = fb.midiAt(s, f);
            auto pitch = fb.pitchAt(s, f, mode, &scale);
            std::string ivStr;
            if (auto iv = scale.intervalForPitchClass(midi % 12)) {
                ivStr = iv->symbol(true);
            }
            bool isRoot = ((midi % 12) == scale.root.pitchClass());
            oss << "{\"s\":" << s
                << ",\"f\":" << f
                << ",\"midi\":" << midi
                << ",\"freq\":" << pitch.frequencyHz()
                << ",\"note\":\"" << jsonEscape(pitch.note.toString(false)) << "\""
                << ",\"interval\":\"" << jsonEscape(ivStr) << "\""
                << ",\"isRoot\":" << (isRoot ? "true" : "false")
                << ",\"inPosition\":" << (isInPos(s, f) ? "true" : "false") << "}";
        }
    }
    oss << "]}";
    return oss.str();
}

std::string handleVoicingsApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string chordId = getQueryParam(query, "chordId", "major");
    std::string group = getQueryParam(query, "group", "all");
    std::string sharpnessStr = getQueryParam(query, "sharpness", "mild");
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));
    bool allTypes = getQueryInt(query, "allTypes", 0) != 0;
    bool filterByScale = getQueryInt(query, "filterByScale", 0) != 0;
    int scaleRootPc = getQueryInt(query, "scaleRootPc", rootPc);
    std::string scaleId = getQueryParam(query, "scaleId", "major");

    VoicingSharpness sharpness = VoicingSharpness::Mild;
    if (sharpnessStr == "sharp") sharpness = VoicingSharpness::Sharp;
    else if (sharpnessStr == "all") sharpness = VoicingSharpness::All;
    const int maxStrings = (group == "triad") ? 3 : 0;

    Fretboard fb{resolveRequestedTuning(query), 24};

    const auto* scaleDef = findScaleDefinition(scaleId);
    if (!scaleDef) scaleDef = findScaleDefinition("major");
    Scale filterScale{Note::fromPitchClass(scaleRootPc, mode), scaleDef};

    auto chordFitsScale = [&](int rPc, const ChordDefinition* d) {
        if (!filterByScale) return true;
        Chord testCh{Note::fromPitchClass(rPc, mode), d, std::nullopt};
        for (const auto& n : testCh.notes()) {
            if (!filterScale.containsPitchClass(n.pitchClass())) return false;
        }
        return true;
    };

    std::vector<int> matchingRootPcs;
    for (int pc = 0; pc < 12; ++pc) {
        if (!filterByScale || filterScale.containsPitchClass(pc)) {
            matchingRootPcs.push_back(pc);
        }
    }
    if (filterByScale && !filterScale.containsPitchClass(rootPc) && !matchingRootPcs.empty()) {
        rootPc = matchingRootPcs.front();
    }

    std::vector<const ChordDefinition*> matchingDefsForRoot;
    for (const auto& d : allChordDefinitions()) {
        if (chordFitsScale(rootPc, &d)) {
            matchingDefsForRoot.push_back(&d);
        }
    }

    std::vector<const ChordDefinition*> groupDefs;
    for (const auto* d : matchingDefsForRoot) {
        if (chordMatchesGroup(*d, group)) {
            groupDefs.push_back(d);
        }
    }
    if (groupDefs.empty()) {
        groupDefs = matchingDefsForRoot;
    }

    Note root = Note::fromPitchClass(rootPc, mode);
    std::vector<const ChordDefinition*> defs;
    if (allTypes) {
        defs = groupDefs;
    } else {
        const auto* d = findChordDefinition(chordId);
        if (!d) d = findChordDefinition("major");
        bool fitsGroup = std::find(groupDefs.begin(), groupDefs.end(), d) != groupDefs.end();
        if (!fitsGroup && !groupDefs.empty()) {
            d = groupDefs.front();
            chordId = d->id;
        } else if (filterByScale && !chordFitsScale(rootPc, d) && !matchingDefsForRoot.empty()) {
            d = matchingDefsForRoot.front();
            chordId = d->id;
        }
        defs.push_back(d);
    }

    std::ostringstream oss;
    oss << "{\"resolvedRootPc\":" << rootPc
        << ",\"resolvedChordId\":\"" << jsonEscape(chordId) << "\""
        << ",\"matchingRootPcs\":[";
    for (std::size_t i = 0; i < matchingRootPcs.size(); ++i) {
        if (i > 0) oss << ",";
        oss << matchingRootPcs[i];
    }
    oss << "],\"matchingChordIds\":[";
    for (std::size_t i = 0; i < matchingDefsForRoot.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "\"" << jsonEscape(matchingDefsForRoot[i]->id) << "\"";
    }
    oss << "],\"voicings\":[";
    bool first = true;
    int count = 0;
    const int globalCap = allTypes ? 84 : 28;

    for (const auto* d : defs) {
        Chord ch{root, d, std::nullopt};
        auto list = fb.generateChordVoicings(ch, sharpness != VoicingSharpness::Mild, nullptr, sharpness, maxStrings);
        std::size_t limit = (!allTypes || defs.size() == 1)
            ? std::min<std::size_t>(28, list.size())
            : std::min<std::size_t>(defs.size() <= 4 ? 6 : 4, list.size());
        for (std::size_t i = 0; i < limit; ++i) {
            if (!first) oss << ",";
            first = false;
            const auto& v = list[i];
            std::string info = v.usesTapping
                ? "T = tapping prawą ręką"
                : (v.isSharpVoicing ? "Chwyt ostry (rozciągnięty)" : "Chwyt łagodny");
            if (!v.openStringsNote.empty()) info += " · " + v.openStringsNote;

            oss << "{\"symbol\":\"" << jsonEscape(ch.symbol(false, mode))
                << "\",\"rootPc\":" << rootPc
                << ",\"activeStrings\":" << v.activeStrings
                << ",\"fretSpan\":" << v.fretSpan
                << ",\"info\":\"" << jsonEscape(info)
                << "\",\"usesTapping\":" << (v.usesTapping ? "true" : "false")
                << ",\"isSharp\":" << (v.isSharpVoicing ? "true" : "false")
                << ",\"openNote\":\"" << jsonEscape(v.openStringsNote) << "\""
                << ",\"frets\":[";
            for (int s = 0; s < fb.stringCount(); ++s) {
                if (s > 0) oss << ",";
                oss << v.frets[static_cast<std::size_t>(s)];
            }
            oss << "],\"tapped\":[";
            for (int s = 0; s < fb.stringCount(); ++s) {
                if (s > 0) oss << ",";
                oss << (v.tapped[static_cast<std::size_t>(s)] ? "true" : "false");
            }
            oss << "],\"freqs\":[";
            bool fFirst = true;
            for (int s = 0; s < fb.stringCount(); ++s) {
                int fr = v.frets[static_cast<std::size_t>(s)];
                if (fr >= 0) {
                    if (!fFirst) oss << ",";
                    fFirst = false;
                    oss << fb.pitchAt(s, fr, mode).frequencyHz();
                }
            }
            oss << "]}";
            if (++count >= globalCap) break;
        }
        if (count >= globalCap) break;
    }

    oss << "]}";
    return oss.str();
}

std::string handleHarmonicsApi(std::string_view query) {
    int stringIdx = getQueryInt(query, "stringIdx", -1);
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));

    Fretboard fb{resolveRequestedTuning(query), 24};
    std::optional<int> optStr = (stringIdx >= 0 && stringIdx < 6) ? std::optional<int>(stringIdx) : std::nullopt;
    auto list = fb.computeNaturalHarmonics(optStr, 0.0, 12.0, 15, mode);

    std::ostringstream oss;
    oss << "{\"harmonics\":[";
    for (std::size_t i = 0; i < list.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& h = list[i];
        const char* color = "#10b981";
        if (h.difficulty == HarmonicDifficulty::Medium) color = "#eab308";
        else if (h.difficulty == HarmonicDifficulty::Hard) color = "#f97316";
        else if (h.difficulty == HarmonicDifficulty::VeryHard) color = "#ef4444";

        int diffLevel = 0;
        if (h.difficulty == HarmonicDifficulty::Medium) diffLevel = 1;
        else if (h.difficulty == HarmonicDifficulty::Hard) diffLevel = 2;
        else if (h.difficulty == HarmonicDifficulty::VeryHard) diffLevel = 3;

        oss << "{\"pitch\":\"" << jsonEscape(h.soundingPitch.toString(false))
            << "\",\"noteOnly\":\"" << jsonEscape(h.soundingPitch.note.toString(false))
            << "\",\"location\":\"" << jsonEscape(h.polishLocation)
            << "\",\"interval\":\"" << jsonEscape(h.intervalOverOpen.symbol(false))
            << "\",\"stringIdx\":" << h.stringIndex
            << ",\"stringNum\":" << (6 - h.stringIndex)
            << ",\"stringName\":\"" << jsonEscape(fb.tuning().openStrings[static_cast<std::size_t>(h.stringIndex)].note.toString(false))
            << "\",\"exactFret\":" << h.exactFret
            << ",\"midi\":" << h.soundingPitch.midiNumber()
            << ",\"freq\":" << h.soundingPitch.frequencyHz()
            << ",\"diff\":" << diffLevel
            << ",\"color\":\"" << color << "\"}";
    }
    oss << "]}";
    return oss.str();
}

std::string handleSearchApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string chordId = getQueryParam(query, "chordId", "major");
    std::string scaleId = getQueryParam(query, "scaleId", "major");
    std::string dir = getQueryParam(query, "dir", "chord_to_scale");
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));

    if (dir == "scale_to_chord") {
        const auto* sDef = findScaleDefinition(scaleId);
        if (!sDef) sDef = findScaleDefinition("major");
        Scale sc{Note::fromPitchClass(rootPc, mode), sDef};
        auto chords = findChordsForScale(sc, true, mode);

        std::ostringstream oss;
        oss << "{\"scaleFullName\":\"" << jsonEscape(sc.fullName(true)) << "\",\"chords\":[";
        for (std::size_t i = 0; i < chords.size(); ++i) {
            if (i > 0) oss << ",";
            const auto& c = chords[i];
            std::string notesStr;
            auto cNotes = c.notes(mode);
            std::ostringstream pcs;
            for (std::size_t n = 0; n < cNotes.size(); ++n) {
                if (n > 0) {
                    notesStr += " ";
                    pcs << ",";
                }
                notesStr += cNotes[n].toString(true);
                pcs << cNotes[n].pitchClass();
            }
            oss << "{\"symbol\":\"" << jsonEscape(c.symbol(true, mode))
                << "\",\"name\":\"" << jsonEscape(c.definition->polishName)
                << "\",\"notes\":\"" << jsonEscape(notesStr)
                << "\",\"chordPcs\":[" << pcs.str() << "]}";
        }
        oss << "]}";
        return oss.str();
    }

    const auto* def = findChordDefinition(chordId);
    if (!def) def = findChordDefinition("major");
    Chord ch{Note::fromPitchClass(rootPc, mode), def, std::nullopt};
    auto matches = findScalesForChord(ch, mode);

    std::ostringstream oss;
    oss << "{\"chordSymbol\":\"" << jsonEscape(ch.symbol(false, mode)) << "\",\"chordPcs\":[";
    auto baseChordNotes = ch.notes(mode);
    for (std::size_t i = 0; i < baseChordNotes.size(); ++i) {
        if (i > 0) oss << ",";
        oss << baseChordNotes[i].pitchClass();
    }
    oss << "],\"matches\":[";
    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& m = matches[i];
        std::string tens;
        for (std::size_t t = 0; t < m.tensions.size(); ++t) {
            if (t > 0) tens += " ";
            tens += m.tensions[t].toString(false);
        }
        std::string av;
        for (std::size_t a = 0; a < m.avoidNotes.size(); ++a) {
            if (a > 0) av += " ";
            av += m.avoidNotes[a].toString(false);
        }
        oss << "{\"scaleId\":\"" << jsonEscape(m.scale.definition->id)
            << "\",\"scaleName\":\"" << jsonEscape(m.scale.fullName(false))
            << "\",\"tensions\":\"" << jsonEscape(tens)
            << "\",\"avoid\":\"" << jsonEscape(av)
            << "\",\"chordPcs\":[";
        for (std::size_t c = 0; c < m.chordTones.size(); ++c) {
            if (c > 0) oss << ",";
            oss << m.chordTones[c].pitchClass();
        }
        oss << "],\"tensionPcs\":[";
        for (std::size_t t = 0; t < m.tensions.size(); ++t) {
            if (t > 0) oss << ",";
            oss << m.tensions[t].pitchClass();
        }
        oss << "],\"avoidPcs\":[";
        for (std::size_t a = 0; a < m.avoidNotes.size(); ++a) {
            if (a > 0) oss << ",";
            oss << m.avoidNotes[a].pitchClass();
        }
        oss << "]}";
    }
    oss << "]}";
    return oss.str();
}

std::string handleDiatonicApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string scaleId = getQueryParam(query, "scaleId", "major");
    bool sevenths = getQueryInt(query, "sevenths", 0) != 0;
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));

    const auto* sDef = findScaleDefinition(scaleId);
    if (!sDef) sDef = findScaleDefinition("major");
    Scale sc{Note::fromPitchClass(rootPc, mode), sDef};

    auto diatonic = generateDiatonicArpeggios(sc, sevenths, mode);

    std::ostringstream oss;
    oss << "{\"scaleFullName\":\"" << jsonEscape(sc.fullName(false)) << "\",\"chords\":[";
    for (std::size_t i = 0; i < diatonic.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& d = diatonic[i];
        auto notes = d.chord.notes(mode);
        std::ostringstream pcs;
        for (std::size_t n = 0; n < notes.size(); ++n) {
            if (n > 0) pcs << ",";
            pcs << notes[n].pitchClass();
        }
        oss << "{\"degree\":" << d.degreeNumber
            << ",\"roman\":\"" << jsonEscape(d.romanNumeral) << "\""
            << ",\"symbol\":\"" << jsonEscape(d.chord.symbol(false, mode)) << "\""
            << ",\"rootPc\":" << d.chord.root.pitchClass()
            << ",\"chordId\":\"" << jsonEscape(d.chord.definition->id) << "\""
            << ",\"pcs\":[" << pcs.str() << "]}";
    }
    oss << "]}";
    return oss.str();
}

std::string handleArpeggioApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string chordId = getQueryParam(query, "chordId", "minor");
    int patternInt = getQueryInt(query, "pattern", 6);
    int posIdx = getQueryInt(query, "posIdx", 0);
    int sweepStrings = getQueryInt(query, "sweepStrings", 3);
    int direction = getQueryInt(query, "direction", 0);
    int seqStyle = getQueryInt(query, "seqStyle", 0);
    int stringSet = getQueryInt(query, "stringSet", 0);
    int octaves = getQueryInt(query, "octaves", 2);
    std::string viewMode = getQueryParam(query, "viewMode", "single"); // "single" | "transition"
    std::string transitionId = getQueryParam(query, "transitionId", "neoclassical_minor");
    int scaleRootPc = getQueryInt(query, "scaleRootPc", rootPc);
    std::string scaleId = getQueryParam(query, "scaleId", "major");
    bool diatonicSevenths = getQueryInt(query, "sevenths", 1) != 0;
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));

    Fretboard fb{resolveRequestedTuning(query), 24};
    const ChordDefinition* chDef = findChordDefinition(chordId);
    if (!chDef) chDef = findChordDefinition("minor");
    Chord ch{Note::fromPitchClass(rootPc, mode), chDef, std::nullopt};

    const ScaleDefinition* scDef = findScaleDefinition(scaleId);
    if (!scDef) scDef = findScaleDefinition("major");
    Scale sc{Note::fromPitchClass(scaleRootPc, mode), scDef};

    auto diatonic = generateDiatonicArpeggios(sc, diatonicSevenths, mode);
    auto fullNeckNodes = generateArpeggio(fb, ch, ArpeggioPattern::FullNeck, 0, mode, nullptr);

    auto writeNodeJson = [&](std::ostringstream& out, const ArpeggioPositionNode& n) {
        int m = fb.midiAt(n.position.stringIndex, n.position.fret);
        out << "{\"s\":" << n.position.stringIndex
            << ",\"f\":" << n.position.fret
            << ",\"midi\":" << m
            << ",\"freq\":" << fb.pitchAt(n.position.stringIndex, n.position.fret, mode).frequencyHz()
            << ",\"note\":\"" << jsonEscape(n.note.toString(false)) << "\""
            << ",\"interval\":\"" << jsonEscape(n.intervalFromRoot.symbol(true)) << "\""
            << ",\"isRoot\":" << (n.isRoot ? "true" : "false") << "}";
    };
    auto writeStepJson = [&](std::ostringstream& out, const ArpeggioStep& st) {
        out << "{\"s\":" << st.stringIndex
            << ",\"f\":" << st.fret
            << ",\"midi\":" << st.midi
            << ",\"freq\":" << st.frequencyHz
            << ",\"note\":\"" << jsonEscape(st.noteName) << "\""
            << ",\"interval\":\"" << jsonEscape(st.interval) << "\""
            << ",\"isRoot\":" << (st.isRoot ? "true" : "false")
            << ",\"tech\":\"" << jsonEscape(st.technique) << "\""
            << ",\"roll\":" << (st.isRoll ? "true" : "false")
            << ",\"seg\":" << st.segmentIndex << "}";
    };

    ArpeggioPattern pat = static_cast<ArpeggioPattern>(std::clamp(patternInt, 0, 6));
    std::vector<ArpeggioPositionNode> activeNodes;
    std::vector<ArpeggioStep> activeSteps;
    struct PosMeta { int idx; std::string label; int minFret; int maxFret; bool hasRoll; };
    std::vector<PosMeta> positions;
    int resolvedPosIdx = posIdx;

    if (pat == ArpeggioPattern::Sweep) {
        auto shapes = generateSweepShapes(fb, ch, sweepStrings, direction, mode, nullptr);
        if (!shapes.empty()) {
            resolvedPosIdx = ((posIdx % static_cast<int>(shapes.size())) + static_cast<int>(shapes.size())) %
                             static_cast<int>(shapes.size());
            for (const auto& sh : shapes) {
                positions.push_back({sh.positionIndex, sh.label, sh.minFret, sh.maxFret, sh.hasFingerRoll});
            }
            activeNodes = shapes[static_cast<std::size_t>(resolvedPosIdx)].nodes;
            activeSteps = (seqStyle == 0)
                ? shapes[static_cast<std::size_t>(resolvedPosIdx)].steps
                : buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
        }
    } else if (pat == ArpeggioPattern::RootOctave) {
        constexpr std::array<const char*, 4> octNames = {"Od str. E6 (niska)", "Od str. A5", "Od str. D4", "Od str. E6 (wysoka)"};
        resolvedPosIdx = ((posIdx % 4) + 4) % 4;
        for (int i = 0; i < 4; ++i) {
            auto ns = generateRootOctaveArpeggio(fb, ch, i, octaves, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, std::string(octNames[static_cast<std::size_t>(i)]) + " · pr. " + std::to_string(mn) + "–" + std::to_string(mx), mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
    } else if (pat == ArpeggioPattern::Inversions) {
        auto invs = generateArpeggioInversions(fb, ch, stringSet, mode, nullptr);
        if (!invs.empty()) {
            positions.push_back({-1, "Wszystkie po kolei (w górę gryfu)", invs.front().minFret, invs.back().maxFret, false});
            for (const auto& inv : invs) {
                positions.push_back({inv.inversionIndex, inv.label, inv.minFret, inv.maxFret, false});
            }
            if (posIdx < 0) {
                resolvedPosIdx = -1;
                std::size_t maxInv = std::min<std::size_t>(invs.size(), 5);
                for (std::size_t i = 0; i < maxInv; ++i) {
                    for (const auto& n : invs[i].nodes) activeNodes.push_back(n);
                    auto segSteps = buildArpeggioSequenceSteps(fb, invs[i].nodes, pat, direction == 2 ? 2 : 1, mode, seqStyle);
                    for (auto& st : segSteps) {
                        st.segmentIndex = static_cast<int>(i);
                        activeSteps.push_back(std::move(st));
                    }
                }
            } else {
                resolvedPosIdx = ((posIdx % static_cast<int>(invs.size())) + static_cast<int>(invs.size())) %
                                 static_cast<int>(invs.size());
                activeNodes = invs[static_cast<std::size_t>(resolvedPosIdx)].nodes;
                activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
            }
        }
    } else if (pat == ArpeggioPattern::CAGED) {
        constexpr std::array<const char*, 5> cagedNames = {"Kształt C", "Kształt A", "Kształt G", "Kształt E", "Kształt D"};
        resolvedPosIdx = ((posIdx % 5) + 5) % 5;
        for (int i = 0; i < 5; ++i) {
            auto ns = generateArpeggio(fb, ch, ArpeggioPattern::CAGED, i, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, std::string(cagedNames[static_cast<std::size_t>(i)]) + " (pr. " + std::to_string(mn) + "–" + std::to_string(mx) + ")", mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
    } else if (pat == ArpeggioPattern::TwoNPS || pat == ArpeggioPattern::ThreeNPS) {
        int countPos = std::max<int>(3, static_cast<int>(ch.notes(mode).size()));
        resolvedPosIdx = ((posIdx % countPos) + countPos) % countPos;
        for (int i = 0; i < countPos; ++i) {
            auto ns = generateArpeggio(fb, ch, pat, i, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, "Poz. " + std::to_string(i + 1) + " (pr. " + std::to_string(mn) + "–" + std::to_string(mx) + ")", mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
    } else {
        resolvedPosIdx = 0;
        activeNodes = fullNeckNodes;
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle);
    }

    auto transition = generateSweepTransition(fb, transitionId, rootPc, sweepStrings, posIdx, &ch, &sc, mode, diatonicSevenths);

    std::ostringstream oss;
    oss << "{\"chordSymbol\":\"" << jsonEscape(ch.symbol(false, mode)) << "\""
        << ",\"chordName\":\"" << jsonEscape(chDef->polishName) << "\""
        << ",\"rootPc\":" << rootPc
        << ",\"chordId\":\"" << jsonEscape(chDef->id) << "\""
        << ",\"pattern\":" << static_cast<int>(pat)
        << ",\"posIdx\":" << resolvedPosIdx
        << ",\"sweepStrings\":" << sweepStrings
        << ",\"seqStyle\":" << seqStyle
        << ",\"stringSet\":" << stringSet
        << ",\"octaves\":" << octaves
        << ",\"viewMode\":\"" << jsonEscape(viewMode) << "\""
        << ",\"scaleFullName\":\"" << jsonEscape(sc.fullName(false)) << "\""
        << ",\"formula\":[";
    for (std::size_t i = 0; i < chDef->intervals.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& iv = chDef->intervals[i];
        Note n = iv.applyTo(ch.root).withAccidentalMode(mode);
        int midi = 48 + rootPc + iv.semitones;
        double hz = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
        oss << "{\"note\":\"" << jsonEscape(n.toString(false)) << "\""
            << ",\"interval\":\"" << jsonEscape(iv.symbol(true)) << "\""
            << ",\"isRoot\":" << (i == 0 ? "true" : "false")
            << ",\"midi\":" << midi
            << ",\"freq\":" << hz << "}";
    }
    oss << "],\"positions\":[";
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"idx\":" << positions[i].idx
            << ",\"label\":\"" << jsonEscape(positions[i].label) << "\""
            << ",\"minFret\":" << positions[i].minFret
            << ",\"maxFret\":" << positions[i].maxFret
            << ",\"hasRoll\":" << (positions[i].hasRoll ? "true" : "false") << "}";
    }
    oss << "],\"nodes\":[";
    for (std::size_t i = 0; i < activeNodes.size(); ++i) {
        if (i > 0) oss << ",";
        writeNodeJson(oss, activeNodes[i]);
    }
    oss << "],\"fullNeckNodes\":[";
    for (std::size_t i = 0; i < fullNeckNodes.size(); ++i) {
        if (i > 0) oss << ",";
        writeNodeJson(oss, fullNeckNodes[i]);
    }
    oss << "],\"steps\":[";
    for (std::size_t i = 0; i < activeSteps.size(); ++i) {
        if (i > 0) oss << ",";
        writeStepJson(oss, activeSteps[i]);
    }
    oss << "],\"diatonic\":[";
    for (std::size_t i = 0; i < diatonic.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"roman\":\"" << jsonEscape(diatonic[i].romanNumeral) << "\""
            << ",\"symbol\":\"" << jsonEscape(diatonic[i].chord.symbol(false, mode)) << "\""
            << ",\"rootPc\":" << diatonic[i].chord.root.pitchClass()
            << ",\"chordId\":\"" << jsonEscape(diatonic[i].chord.definition->id) << "\"}";
    }
    oss << "],\"transitionPresets\":[";
    const auto& presets = allSweepTransitionPresets();
    for (std::size_t i = 0; i < presets.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":\"" << jsonEscape(presets[i].id) << "\""
            << ",\"name\":\"" << jsonEscape(presets[i].name) << "\""
            << ",\"category\":\"" << jsonEscape(presets[i].category) << "\""
            << ",\"description\":\"" << jsonEscape(presets[i].description) << "\"}";
    }
    oss << "],\"transition\":{\"id\":\"" << jsonEscape(transition.id) << "\""
        << ",\"name\":\"" << jsonEscape(transition.name) << "\""
        << ",\"category\":\"" << jsonEscape(transition.category) << "\""
        << ",\"description\":\"" << jsonEscape(transition.description) << "\""
        << ",\"practiceTip\":\"" << jsonEscape(transition.practiceTip) << "\""
        << ",\"segments\":[";
    for (std::size_t i = 0; i < transition.segments.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& seg = transition.segments[i];
        oss << "{\"roman\":\"" << jsonEscape(seg.romanNumeral) << "\""
            << ",\"symbol\":\"" << jsonEscape(seg.chordSymbol) << "\""
            << ",\"chordId\":\"" << jsonEscape(seg.chordId) << "\""
            << ",\"rootPc\":" << seg.rootPc
            << ",\"stringSpan\":" << seg.stringSpan
            << ",\"minFret\":" << seg.minFret
            << ",\"maxFret\":" << seg.maxFret
            << ",\"hint\":\"" << jsonEscape(seg.transitionHint) << "\""
            << ",\"nodes\":[";
        for (std::size_t ni = 0; ni < seg.nodes.size(); ++ni) {
            if (ni > 0) oss << ",";
            writeNodeJson(oss, seg.nodes[ni]);
        }
        oss << "],\"steps\":[";
        for (std::size_t si = 0; si < seg.steps.size(); ++si) {
            if (si > 0) oss << ",";
            writeStepJson(oss, seg.steps[si]);
        }
        oss << "]}";
    }
    oss << "]}}";
    return oss.str();
}

Song buildSongFromJsonEvents(std::string_view eventsJson, int bpm) {
    Song song{};
    song.title = "Twoje Cwiczenie";
    song.tempoBpm = std::max(40, bpm);

    Track track{};
    track.name = "Gitara";
    track.tuning = findTuning("standard");
    Fretboard fb{track.tuning, 24};

    Measure currentM{};
    currentM.numerator = 4;
    currentM.denominator = 4;
    double measureQuarters = 0.0;

    auto pushBeat = [&](Beat b) {
        double qLen = b.quarterNoteLength();
        if (measureQuarters + qLen > 4.0 + 1e-4 && !currentM.beats.empty()) {
            track.measures.push_back(std::move(currentM));
            currentM = Measure{};
            measureQuarters = 0.0;
        }
        measureQuarters += qLen;
        currentM.beats.push_back(std::move(b));
    };

    std::size_t pos = 0;
    while ((pos = eventsJson.find('{', pos)) != std::string_view::npos) {
        std::size_t endObj = eventsJson.find('}', pos);
        if (endObj == std::string_view::npos) break;
        std::string_view obj = eventsJson.substr(pos, endObj - pos + 1);
        pos = endObj + 1;

        auto extractInt = [&](std::string_view key, int defVal) -> int {
            std::size_t kPos = obj.find(key);
            if (kPos == std::string_view::npos) return defVal;
            return std::atoi(obj.substr(kPos + key.size()).data());
        };

        int isRest = extractInt("\"rest\":", 0);
        int durCode = extractInt("\"dur\":", 0); // -2=Whole, -1=Half, 0=Quarter, 1=Eighth, 2=Sixteenth
        int tuplet = extractInt("\"tuplet\":", 0);
        int s = extractInt("\"s\":", -1);
        int f = extractInt("\"f\":", -1);
        int midi = extractInt("\"midi\":", -1);

        Beat b{};
        b.duration = static_cast<DurationValue>(std::clamp(durCode, -2, 3));
        if (tuplet == 3) {
            b.tupletNumerator = 3;
            b.tupletDenominator = 2;
        }

        if (isRest != 0) {
            b.isRest = true;
            pushBeat(std::move(b));
            continue;
        }

        if (s >= 0 && f >= 0) {
            b.isRest = false;
            b.notes.push_back(TabNote{s, f, 95});
            pushBeat(std::move(b));
        } else if (midi > 0) {
            if (auto best = fb.selectBestPosition(midi)) {
                b.isRest = false;
                b.notes.push_back(TabNote{best->stringIndex, best->fret, 95});
                pushBeat(std::move(b));
            }
        }
    }

    if (!currentM.beats.empty() || track.measures.empty()) {
        track.measures.push_back(std::move(currentM));
    }
    song.tracks.push_back(std::move(track));
    return song;
}

std::string handleTabPreviewApi(std::string_view query, std::string_view body = "") {
    std::string evs = (!body.empty() && body.front() == '[')
        ? std::string(body)
        : getQueryParam(query, "events", "[]");
    int bpm = getQueryInt(query, "bpm", 120);
    Song song = buildSongFromJsonEvents(evs, bpm);
    std::string tab = exportToAsciiTab(song);
    Scale key = detectKeyFromSong(song);
    std::ostringstream oss;
    oss << "{\"asciiTab\":\"" << jsonEscape(tab)
        << "\",\"detectedKey\":\"" << jsonEscape(key.fullName(true)) << "\"}";
    return oss.str();
}

std::string handleWorkshopEventsApi(std::string_view query) {
    int rootPc = getQueryInt(query, "rootPc", 0);
    std::string scaleId = getQueryParam(query, "scaleId", "major");
    int posSystem = getQueryInt(query, "posSystem", 1);
    int posIdx = getQueryInt(query, "posIdx", 0);
    int startStr = getQueryInt(query, "startString", -1);
    int dir = getQueryInt(query, "direction", 0);
    int octaves = getQueryInt(query, "octaves", 2);
    int pattern = getQueryInt(query, "pattern", 0);
    int traversal = getQueryInt(query, "traversal", 0);
    bool startOnRoot = getQueryInt(query, "startOnRoot", 0) != 0;
    int minFret = getQueryInt(query, "minFret", 0);
    int maxFret = getQueryInt(query, "maxFret", 24);
    int bpm = getQueryInt(query, "bpm", 120);
    int dur = getQueryInt(query, "dur", 1);
    bool tuplet = getQueryInt(query, "tuplet", 0) != 0;
    AccidentalMode mode = toAccMode(getQueryInt(query, "accMode", 2));
    if (query.find("group=") != std::string_view::npos) {
        int group = getQueryInt(query, "group", 3);
        pattern = (group == 4) ? 4 : 3;
    }

    Fretboard fb{resolveRequestedTuning(query), 24};
    const auto* scDef = findScaleDefinition(scaleId);
    if (!scDef) scDef = findScaleDefinition("major");
    WorkshopExerciseConfig cfg{};
    cfg.scale = Scale{Note::fromPitchClass(rootPc, mode), scDef};
    cfg.positionSystem = static_cast<PositionSystem>(std::clamp(posSystem, 0, 3));
    cfg.positionIndex = std::max(0, posIdx);
    cfg.startStringIndex = startStr;
    cfg.direction = static_cast<ExerciseDirection>(std::clamp(dir, 0, 3));
    cfg.octaves = std::clamp(octaves, 1, 3);
    cfg.pattern = static_cast<ExercisePattern>(std::clamp(pattern, 0, 18));
    cfg.traversalMode = static_cast<ScaleTraversalMode>(std::clamp(traversal, 0, 2));
    cfg.startOnRoot = startOnRoot;
    cfg.minFret = std::clamp(minFret, 0, 24);
    cfg.maxFret = std::clamp(maxFret, cfg.minFret, 24);
    cfg.tempoBpm = std::clamp(bpm, 30, 260);
    cfg.tuplet = tuplet;
    if (dur == 0) cfg.noteDuration = DurationValue::Quarter;
    else if (dur == 2) cfg.noteDuration = DurationValue::Sixteenth;
    else cfg.noteDuration = DurationValue::Eighth;

    auto gen = generateScaleExercise(fb, cfg);

    int outMinFret = 99, outMaxFret = 0;
    std::size_t limit = std::min<std::size_t>(256, gen.steps.size());
    for (std::size_t i = 0; i < limit; ++i) {
        outMinFret = std::min(outMinFret, gen.steps[i].targetPosition.fret);
        outMaxFret = std::max(outMaxFret, gen.steps[i].targetPosition.fret);
    }
    if (outMinFret > outMaxFret) outMinFret = 0;

    std::ostringstream oss;
    oss << "{\"title\":\"" << jsonEscape(gen.song.title) << "\""
        << ",\"stepCount\":" << limit
        << ",\"minFret\":" << outMinFret
        << ",\"maxFret\":" << outMaxFret
        << ",\"events\":[";
    for (std::size_t i = 0; i < limit; ++i) {
        if (i > 0) oss << ",";
        const auto& st = gen.steps[i];
        auto p = fb.pitchAt(st.targetPosition.stringIndex, st.targetPosition.fret, mode);
        oss << "{\"s\":" << st.targetPosition.stringIndex
            << ",\"f\":" << st.targetPosition.fret
            << ",\"midi\":" << st.expectedMidiNote
            << ",\"note\":\"" << jsonEscape(p.note.toString(false)) << "\""
            << ",\"freq\":" << p.frequencyHz() << "}";
    }
    oss << "]}";
    return oss.str();
}

void openDesktopAppWindow(int port) {
    std::string url = "http://127.0.0.1:" + std::to_string(port);
    // W środowisku Windows + WSL2 otwieramy natywne okno aplikacyjne bez paska adresu (--app=...)
    std::string cmd =
        "( \"/mnt/c/Program Files (x86)/Microsoft/Edge/Application/msedge.exe\" --app=" + url +
        " >/dev/null 2>&1 || cmd.exe /c start " + url +
        " >/dev/null 2>&1 || xdg-open " + url + " >/dev/null 2>&1 ) &";
    int rc = std::system(cmd.c_str());
    (void)rc;
}

} // namespace

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::string_view(argv[i]) == "--headless-check") {
            auto initJson = handleInitApi();
            auto fbJson = handleFretboardApi("rootPc=0&scaleId=major");
            return (!initJson.empty() && !fbJson.empty()) ? 0 : 1;
        }
    }

    int serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "Blad utworzenia gniazda GUI.\n";
        return 1;
    }

    int opt = 1;
    ::setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int port = 8765;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool bound = false;
    for (int p = 8765; p < 8785; ++p) {
        addr.sin_port = htons(static_cast<std::uint16_t>(p));
        if (::bind(serverFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0) {
            port = p;
            bound = true;
            break;
        }
    }
    if (!bound || ::listen(serverFd, 16) < 0) {
        std::cerr << "Nie udalo sie uruchomic lokalnego serwera GUI.\n";
        ::close(serverFd);
        return 1;
    }

    std::cout << "====================================================================\n";
    std::cout << " 🎸 Apka Gitarowa (GUI) uruchomiona!\n";
    std::cout << " -> Okno graficzne otwiera sie automatycznie: http://127.0.0.1:" << port << "\n";
    std::cout << " -> Nacisnij Ctrl+C, aby zamknac aplikacje.\n";
    std::cout << "====================================================================\n";

    openDesktopAppWindow(port);

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = ::accept(serverFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientFd < 0) continue;

        timeval tv{};
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        ::setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // Odczyt pełnego żądania HTTP (nagłówki + opcjonalne ciało POST) bez limitu 8 KB!
        std::string reqStr;
        reqStr.reserve(16384);
        constexpr std::size_t kMaxReqBytes = 4 * 1024 * 1024;
        std::size_t headerEnd = std::string::npos;
        std::size_t contentLength = 0;
        char chunk[8192];
        while (reqStr.size() < kMaxReqBytes) {
            ssize_t nRead = ::read(clientFd, chunk, sizeof(chunk));
            if (nRead <= 0) break;
            reqStr.append(chunk, static_cast<std::size_t>(nRead));
            if (headerEnd == std::string::npos) {
                headerEnd = reqStr.find("\r\n\r\n");
                if (headerEnd != std::string::npos) {
                    std::string lowerHdr;
                    lowerHdr.reserve(headerEnd);
                    for (std::size_t i = 0; i < headerEnd; ++i) {
                        lowerHdr.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(reqStr[i]))));
                    }
                    std::size_t clPos = lowerHdr.find("content-length:");
                    if (clPos != std::string::npos) {
                        std::size_t valStart = clPos + 15;
                        while (valStart < headerEnd && (reqStr[valStart] == ' ' || reqStr[valStart] == '\t')) ++valStart;
                        contentLength = static_cast<std::size_t>(std::max<long long>(0, std::atoll(reqStr.c_str() + valStart)));
                    }
                }
            }
            if (headerEnd != std::string::npos) {
                std::size_t bodyRead = reqStr.size() - (headerEnd + 4);
                if (bodyRead >= contentLength) break;
            }
        }
        if (reqStr.empty()) {
            ::close(clientFd);
            continue;
        }

        std::string_view req(reqStr);
        std::string_view reqBody = (headerEnd != std::string::npos && reqStr.size() >= headerEnd + 4)
            ? std::string_view(reqStr.data() + headerEnd + 4,
                               std::min(contentLength, reqStr.size() - (headerEnd + 4)))
            : std::string_view{};

        std::size_t firstSpace = req.find(' ');
        std::size_t secondSpace = (firstSpace != std::string_view::npos)
            ? req.find(' ', firstSpace + 1)
            : std::string_view::npos;

        std::string_view fullTarget = (firstSpace != std::string_view::npos && secondSpace != std::string_view::npos)
            ? req.substr(firstSpace + 1, secondSpace - firstSpace - 1)
            : "/";

        std::size_t qPos = fullTarget.find('?');
        std::string_view path = (qPos == std::string_view::npos) ? fullTarget : fullTarget.substr(0, qPos);
        std::string_view query = (qPos == std::string_view::npos) ? "" : fullTarget.substr(qPos + 1);

        auto writeAll = [&](const char* ptr, std::size_t len) {
            std::size_t sent = 0;
            while (sent < len) {
                ssize_t w = ::write(clientFd, ptr + sent, len - sent);
                if (w <= 0) break;
                sent += static_cast<std::size_t>(w);
            }
        };

        auto sendResponse = [&](std::string_view status,
                                std::string_view contentType,
                                const void* data,
                                std::size_t size,
                                std::string_view extraHeaders = "") {
            std::ostringstream hdr;
            hdr << "HTTP/1.1 " << status << "\r\n"
                << "Content-Type: " << contentType << "\r\n"
                << "Content-Length: " << size << "\r\n"
                << "Cache-Control: no-store\r\n"
                << extraHeaders
                << "Connection: close\r\n\r\n";
            std::string hStr = hdr.str();
            writeAll(hStr.data(), hStr.size());
            if (size > 0 && data != nullptr) {
                writeAll(static_cast<const char*>(data), size);
            }
        };

        if (path == "/" || path == "/index.html") {
            std::string liveHtml;
            std::ifstream ifs("app/ui/DesktopGuiHtml.h", std::ios::binary);
            if (ifs) {
                std::string fileContent((std::istreambuf_iterator<char>(ifs)),
                                        std::istreambuf_iterator<char>());
                const std::string startTag = "R\"HTML(";
                const std::string endTag = ")HTML\"";
                auto p1 = fileContent.find(startTag);
                auto p2 = fileContent.rfind(endTag);
                if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1 + startTag.size()) {
                    liveHtml = fileContent.substr(p1 + startTag.size(), p2 - (p1 + startTag.size()));
                }
            }
            if (!liveHtml.empty()) {
                sendResponse("200 OK", "text/html; charset=utf-8", liveHtml.data(), liveHtml.size());
            } else {
                const char* html = gitara::app::ui::kDesktopGuiHtml;
                sendResponse("200 OK", "text/html; charset=utf-8", html, std::strlen(html));
            }
        } else if (path == "/api/init") {
            std::string body = handleInitApi();
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/settings") {
            std::ifstream sfs(".apka_gitara_settings.json", std::ios::binary);
            std::string body = "{}";
            if (sfs) {
                body.assign((std::istreambuf_iterator<char>(sfs)),
                            std::istreambuf_iterator<char>());
                if (body.empty()) body = "{}";
            }
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/save_settings") {
            std::string jsonPayload = (!reqBody.empty() && reqBody.front() == '{')
                ? std::string(reqBody)
                : getQueryParam(query, "data", "{}");
            if (!jsonPayload.empty() && jsonPayload.front() == '{') {
                std::ofstream ofs(".apka_gitara_settings.json", std::ios::binary | std::ios::trunc);
                if (ofs) ofs.write(jsonPayload.data(), static_cast<std::streamsize>(jsonPayload.size()));
            }
            const char* ok = "{\"ok\":true}";
            sendResponse("200 OK", "application/json; charset=utf-8", ok, std::strlen(ok));
        } else if (path == "/api/fretboard") {
            std::string body = handleFretboardApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/voicings") {
            std::string body = handleVoicingsApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/harmonics") {
            std::string body = handleHarmonicsApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/search") {
            std::string body = handleSearchApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/diatonic") {
            std::string body = handleDiatonicApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/arpeggio") {
            std::string body = handleArpeggioApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/tab_preview") {
            std::string body = handleTabPreviewApi(query, reqBody);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/workshop_events") {
            std::string body = handleWorkshopEventsApi(query);
            sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
        } else if (path == "/api/export/gp5") {
            std::string evs = (!reqBody.empty() && reqBody.front() == '[')
                ? std::string(reqBody)
                : getQueryParam(query, "events", "[]");
            Song song = buildSongFromJsonEvents(evs, getQueryInt(query, "bpm", 120));
            auto bytes = exportToGp5Bytes(song);
            sendResponse("200 OK", "application/octet-stream", bytes.data(), bytes.size(),
                         "Content-Disposition: attachment; filename=\"cwiczenie_tuxguitar.gp5\"\r\n");
        } else if (path == "/api/export/mid") {
            std::string evs = (!reqBody.empty() && reqBody.front() == '[')
                ? std::string(reqBody)
                : getQueryParam(query, "events", "[]");
            Song song = buildSongFromJsonEvents(evs, getQueryInt(query, "bpm", 120));
            auto bytes = exportToMidiBytes(song);
            sendResponse("200 OK", "audio/midi", bytes.data(), bytes.size(),
                         "Content-Disposition: attachment; filename=\"cwiczenie_tuxguitar.mid\"\r\n");
        } else if (path == "/api/export/progression_gp5") {
            auto prog = findBuiltinProgression(getQueryParam(query, "id", "blues_12"));
            Song song = generateBackingTrackSong(prog.value_or(builtinProgressions().front()));
            auto bytes = exportToGp5Bytes(song);
            sendResponse("200 OK", "application/octet-stream", bytes.data(), bytes.size(),
                         "Content-Disposition: attachment; filename=\"blues_12_tuxguitar.gp5\"\r\n");
        } else {
            const char* notFound = "{}";
            sendResponse("404 Not Found", "application/json", notFound, 2);
        }

        ::close(clientFd);
    }

    ::close(serverFd);
    return 0;
}

#endif
