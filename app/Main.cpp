#include "app/MainWindow.h"
#if !defined(GITARA_EMBEDDED_ASSETS)
#include "app/ui/DesktopGuiHtml.h"
#endif
#include "gitara/engine/EarTrainer.h"
#include "gitara/engine/ProgressionEngine.h"
#include "gitara/engine/SessionManager.h"
#include "gitara/engine/WorkshopGenerator.h"
#include "gitara/score/Score.h"
#include "gitara/score/StudioJsonCodec.h"
#include "gitara/score/UserLibrary.h"
#include "gitara/theory/Arpeggio.h"
#include "gitara/theory/Chord.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
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

#include "app/BuildInfo.h"
#include "app/platform/AppPaths.h"
#include "app/platform/AutoUpdater.h"
#include "app/platform/EmbeddedAssets.h"
#include "app/platform/NetSocket.h"
#include "app/platform/UpdateLogic.h"
#include "app/platform/WindowsHost.h"

#include <atomic>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <system_error>
#include <thread>

#ifdef _WIN32
#include <shellapi.h>
#endif

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
            if (!tok.empty()) {
                int midi = std::clamp(std::atoi(tok.c_str()), 12, 108);
                customPitches.push_back(Pitch::fromMidiNumber(midi, AccidentalMode::Sharps));
            }
            if (comma == std::string::npos) break;
            pos = comma + 1;
        }
        if (customPitches.size() >= 4 && customPitches.size() <= 8) {
            t.id = "custom";
            t.polishName = "Własny strój";
            t.openStrings = std::move(customPitches);
        }
    }
    return t;
}

std::string sanitizeAsciiFilenameSlug(std::string_view raw, std::string_view fallback = "progresja") {
    std::string out;
    out.reserve(48);
    for (unsigned char c : raw) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_') {
            out.push_back(static_cast<char>(c));
        } else if (c == ' ' && (out.empty() || out.back() != '_')) {
            out.push_back('_');
        }
        if (out.size() >= 48) break;
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    return out.empty() ? std::string(fallback) : out;
}

ChordProgression buildCustomProgressionFromJson(std::string_view json) {
    auto skipWs = [&](std::size_t& p) {
        while (p < json.size() && (json[p] == ' ' || json[p] == '\t' || json[p] == '\r' || json[p] == '\n')) ++p;
    };
    auto matchString = [&](std::size_t p, std::string& out, std::size_t& nextPos) -> bool {
        if (p >= json.size() || json[p] != '"') return false;
        ++p;
        out.clear();
        while (p < json.size()) {
            char c = json[p++];
            if (c == '\\' && p < json.size()) {
                char esc = json[p++];
                if (esc == 'n') out.push_back('\n');
                else if (esc == 'r') out.push_back('\r');
                else if (esc == 't') out.push_back('\t');
                else out.push_back(esc);
            } else if (c == '"') {
                nextPos = p;
                return true;
            } else {
                out.push_back(c);
            }
        }
        return false;
    };
    auto findMatching = [&](std::size_t openPos, char openCh, char closeCh) -> std::size_t {
        if (openPos >= json.size() || json[openPos] != openCh) return std::string_view::npos;
        int depth = 0;
        bool inStr = false;
        for (std::size_t p = openPos; p < json.size(); ++p) {
            char c = json[p];
            if (inStr) {
                if (c == '\\' && p + 1 < json.size()) { ++p; continue; }
                if (c == '"') inStr = false;
                continue;
            }
            if (c == '"') { inStr = true; continue; }
            if (c == openCh) ++depth;
            else if (c == closeCh) {
                --depth;
                if (depth == 0) return p;
            }
        }
        return std::string_view::npos;
    };
    auto findTopValueStart = [&](std::size_t objStart, std::size_t objEnd, std::string_view targetKey) -> std::size_t {
        if (objStart >= objEnd || json[objStart] != '{') return std::string_view::npos;
        std::size_t p = objStart + 1;
        while (p < objEnd) {
            skipWs(p);
            if (p >= objEnd || json[p] == '}') break;
            if (json[p] == ',') { ++p; continue; }
            std::string key;
            std::size_t afterKey = p;
            if (!matchString(p, key, afterKey)) break;
            p = afterKey;
            skipWs(p);
            if (p >= objEnd || json[p] != ':') break;
            ++p;
            skipWs(p);
            if (key == targetKey) return p;
            if (p >= objEnd) break;
            if (json[p] == '{') {
                auto e = findMatching(p, '{', '}');
                if (e == std::string_view::npos) break;
                p = e + 1;
            } else if (json[p] == '[') {
                auto e = findMatching(p, '[', ']');
                if (e == std::string_view::npos) break;
                p = e + 1;
            } else if (json[p] == '"') {
                std::string dummy;
                if (!matchString(p, dummy, p)) break;
            } else {
                while (p < objEnd && json[p] != ',' && json[p] != '}') ++p;
            }
        }
        return std::string_view::npos;
    };
    auto getStrField = [&](std::size_t s, std::size_t e, std::string_view key, std::string def) -> std::string {
        auto v = findTopValueStart(s, e, key);
        if (v == std::string_view::npos) return def;
        std::string out;
        std::size_t np = v;
        return matchString(v, out, np) ? out : def;
    };
    auto getIntField = [&](std::size_t s, std::size_t e, std::string_view key, int def) -> int {
        auto v = findTopValueStart(s, e, key);
        if (v == std::string_view::npos || v >= e) return def;
        char c = json[v];
        if (c != '-' && (c < '0' || c > '9')) return def;
        return std::atoi(json.substr(v, e - v).data());
    };
    auto getBoolField = [&](std::size_t s, std::size_t e, std::string_view key, bool def) -> bool {
        auto v = findTopValueStart(s, e, key);
        if (v == std::string_view::npos || v >= e) return def;
        if (json.substr(v, 4) == "true") return true;
        if (json.substr(v, 5) == "false") return false;
        return def;
    };

    ChordProgression prog{};
    prog.id = "custom_progression";
    prog.title = "Wlasna progresja";
    prog.styleCategory = "Custom";
    prog.tempoBpm = 108;

    std::size_t rootStart = json.find('{');
    if (rootStart == std::string_view::npos) return builtinProgressions().front();
    std::size_t rootEnd = findMatching(rootStart, '{', '}');
    if (rootEnd == std::string_view::npos) return builtinProgressions().front();

    prog.title = getStrField(rootStart, rootEnd, "title", getStrField(rootStart, rootEnd, "name", "Wlasna progresja"));
    prog.tempoBpm = std::clamp(getIntField(rootStart, rootEnd, "bpm", 108), 40, 240);
    int keyRootPc = ((getIntField(rootStart, rootEnd, "keyRootPc", 0) % 12) + 12) % 12;
    prog.isMinorKey = getBoolField(rootStart, rootEnd, "isMinor", false);
    prog.keyRoot = Note::fromPitchClass(keyRootPc);

    std::string timeSig = getStrField(rootStart, rootEnd, "timeSig", "4/4");
    auto slashPos = timeSig.find('/');
    if (slashPos != std::string::npos) {
        prog.numerator = std::clamp(std::atoi(timeSig.substr(0, slashPos).c_str()), 1, 12);
        int den = std::atoi(timeSig.substr(slashPos + 1).c_str());
        prog.denominator = (den == 2 || den == 4 || den == 8 || den == 16) ? den : 4;
    }

    std::string feel = getStrField(rootStart, rootEnd, "feel", "Prosto");
    std::string grooveStr = getStrField(rootStart, rootEnd, "groove", "straight");
    if (feel == "Blues" && grooveStr == "shuffle") {
        prog.groove = GrooveStyle::Shuffle;
        prog.swing = SwingLevel::Medium;
    } else if (feel == "Blues" && grooveStr == "slow12_8") {
        prog.groove = GrooveStyle::Slow12_8;
    } else if (feel == "Swing Jazz") {
        prog.swing = SwingLevel::Medium;
    }

    auto parseSlotObj = [&](std::size_t objS, std::size_t objE, int beatIdx) -> ChordSlot {
        int off = ((getIntField(objS, objE, "offsetSemi", 0) % 12) + 12) % 12;
        std::string cid = getStrField(objS, objE, "chordId", "major");
        if (!findChordDefinition(cid)) cid = "major";
        std::string roman = getStrField(objS, objE, "roman", "I");
        return ChordSlot{beatIdx, intervalFromSemitones(off), cid, roman};
    };

    ProgressionSection sec{};
    sec.name = "A";
    sec.repeatCount = 1;

    std::size_t measuresVal = findTopValueStart(rootStart, rootEnd, "measures");
    if (measuresVal != std::string_view::npos && json[measuresVal] == '[') {
        std::size_t mArrEnd = findMatching(measuresVal, '[', ']');
        if (mArrEnd != std::string_view::npos) {
            std::size_t p = measuresVal + 1;
            while (p < mArrEnd) {
                skipWs(p);
                if (p >= mArrEnd || json[p] == ']') break;
                if (json[p] == ',') { ++p; continue; }
                if (json[p] == '{') {
                    std::size_t mEnd = findMatching(p, '{', '}');
                    if (mEnd == std::string_view::npos) break;
                    ProgressionMeasure pm{};
                    pm.voltaNumber = std::clamp(getIntField(p, mEnd, "volta", 0), 0, 2);
                    std::size_t beatsVal = findTopValueStart(p, mEnd, "beats");
                    if (beatsVal != std::string_view::npos && json[beatsVal] == '[') {
                        std::size_t bArrEnd = findMatching(beatsVal, '[', ']');
                        if (bArrEnd != std::string_view::npos) {
                            std::size_t bp = beatsVal + 1;
                            int beatIdx = 0;
                            while (bp < bArrEnd) {
                                skipWs(bp);
                                if (bp >= bArrEnd || json[bp] == ']') break;
                                if (json[bp] == ',') { ++bp; continue; }
                                if (json[bp] == '{') {
                                    std::size_t bEnd = findMatching(bp, '{', '}');
                                    if (bEnd == std::string_view::npos) break;
                                    pm.chords.push_back(parseSlotObj(bp, bEnd, beatIdx));
                                    bp = bEnd + 1;
                                    ++beatIdx;
                                } else {
                                    while (bp < bArrEnd && json[bp] != ',' && json[bp] != ']') ++bp;
                                    ++beatIdx;
                                }
                            }
                        }
                    }
                    sec.measures.push_back(std::move(pm));
                    p = mEnd + 1;
                } else {
                    ++p;
                }
            }
        }
    }

    if (sec.measures.empty()) {
        std::size_t barsVal = findTopValueStart(rootStart, rootEnd, "bars");
        if (barsVal != std::string_view::npos && json[barsVal] == '[') {
            std::size_t bArrEnd = findMatching(barsVal, '[', ']');
            if (bArrEnd != std::string_view::npos) {
                std::size_t p = barsVal + 1;
                while (p < bArrEnd) {
                    skipWs(p);
                    if (p >= bArrEnd || json[p] == ']') break;
                    if (json[p] == ',') { ++p; continue; }
                    if (json[p] == '{') {
                        std::size_t bEnd = findMatching(p, '{', '}');
                        if (bEnd == std::string_view::npos) break;
                        ProgressionMeasure pm{};
                        pm.chords.push_back(parseSlotObj(p, bEnd, 0));
                        sec.measures.push_back(std::move(pm));
                        p = bEnd + 1;
                    } else {
                        ++p;
                    }
                }
            }
        }
    }

    if (sec.measures.empty()) {
        ProgressionMeasure pm{};
        pm.chords.push_back(ChordSlot{0, intervals::P1, prog.isMinorKey ? "minor" : "major", prog.isMinorKey ? "i" : "I"});
        sec.measures.push_back(std::move(pm));
    }
    prog.sections.push_back(std::move(sec));
    return prog;
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
    int capo = std::clamp(getQueryInt(query, "capo", 0), 0, 12);

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
        if (!d) {
            d = findChordDefinition("major");
            if (d) chordId = d->id;
        }
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
        << ",\"capo\":" << capo
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
        std::string shapeSym;
        if (capo > 0) {
            Note shapeRoot = Note::fromPitchClass(((rootPc - capo) % 12 + 12) % 12, mode);
            Chord shapeCh{shapeRoot, d, std::nullopt};
            shapeSym = shapeCh.symbol(false, mode);
        }
        auto list = fb.generateChordVoicings(ch, sharpness != VoicingSharpness::Mild, nullptr, sharpness, maxStrings, capo);
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
            if (capo > 0 && !shapeSym.empty()) {
                info += " · kapo " + std::to_string(capo) + " (kształt " + shapeSym + ")";
            }
            if (!v.openStringsNote.empty()) info += " · " + v.openStringsNote;

            oss << "{\"symbol\":\"" << jsonEscape(ch.symbol(false, mode))
                << "\",\"shapeSymbol\":\"" << jsonEscape(shapeSym)
                << "\",\"capo\":" << capo
                << ",\"rootPc\":" << rootPc
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
    std::optional<int> optStr = (stringIdx >= 0 && stringIdx < fb.stringCount())
        ? std::optional<int>(stringIdx)
        : std::nullopt;
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
            << ",\"stringNum\":" << (fb.stringCount() - h.stringIndex)
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

// Wykrywanie skali na podstawie zagranych klas wysokości (tryb „Wykryj skalę" w zakładce Szukaj).
// pcs: lista klas 0..11 rozdzielona przecinkami; mode: all | major | minor.
std::string handleDetectScaleApi(std::string_view query) {
    std::string pcsParam = getQueryParam(query, "pcs", "");
    std::string mode = getQueryParam(query, "mode", "all");
    int rootHint = getQueryInt(query, "rootHint", -1);
    AccidentalMode accMode = toAccMode(getQueryInt(query, "accMode", 2));

    bool present[12] = {false};
    int pcCount = 0;
    {
        std::istringstream ss(pcsParam);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            if (tok.empty()) continue;
            int pc = std::atoi(tok.c_str());
            if (pc >= 0 && pc < 12 && !present[pc]) { present[pc] = true; ++pcCount; }
        }
    }
    if (pcCount == 0) {
        return "{\"scales\":[]}";
    }

    struct Match { int rootPc; const ScaleDefinition* def; int extra; int categoryRank; bool rootIsHint; };
    std::vector<Match> matches;

    auto categoryRank = [](std::string_view cat) -> int {
        if (cat == "Podstawowe") return 0;
        if (cat == "Modalne") return 1;
        if (cat == "Pentatoniki i Blues") return 2;
        if (cat == "Jazzowe") return 3;
        if (cat == "Symetryczne") return 4;
        return 5; // Egzotyczne
    };

    for (const auto& def : allScaleDefinitions()) {
        bool hasM3 = false, hasm3 = false;
        for (const auto& iv : def.intervals) {
            if (iv.semitones == 4) hasM3 = true;
            if (iv.semitones == 3) hasm3 = true;
        }
        if (mode == "major" && (!hasM3 || hasm3)) continue;
        if (mode == "minor" && (!hasm3 || hasM3)) continue;

        for (int root = 0; root < 12; ++root) {
            Scale sc{Note::fromPitchClass(root, accMode), &def};
            bool allPresent = true;
            for (int pc = 0; pc < 12; ++pc) {
                if (present[pc] && !sc.containsPitchClass(pc)) { allPresent = false; break; }
            }
            if (!allPresent) continue;

            int extra = 0;
            for (const auto& sn : sc.notes(accMode)) {
                if (!present[sn.pitchClass()]) ++extra;
            }
            matches.push_back({root, &def, extra, categoryRank(def.category), (rootHint >= 0 && root == rootHint)});
        }
    }

    std::sort(matches.begin(), matches.end(), [](const Match& a, const Match& b) {
        if (a.rootIsHint != b.rootIsHint) return a.rootIsHint > b.rootIsHint;
        if (a.categoryRank != b.categoryRank) return a.categoryRank < b.categoryRank;
        if (a.extra != b.extra) return a.extra < b.extra;
        if (a.rootPc != b.rootPc) return a.rootPc < b.rootPc;
        return a.def->id < b.def->id;
    });

    std::ostringstream oss;
    oss << "{\"scales\":[";
    for (std::size_t i = 0; i < matches.size(); ++i) {
        if (i > 0) oss << ",";
        const auto& m = matches[i];
        Scale sc{Note::fromPitchClass(m.rootPc, accMode), m.def};
        oss << "{\"scaleId\":\"" << jsonEscape(m.def->id) << "\""
            << ",\"rootPc\":" << m.rootPc
            << ",\"scaleName\":\"" << jsonEscape(sc.fullName(false)) << "\""
            << ",\"category\":\"" << jsonEscape(m.def->category) << "\""
            << ",\"extra\":" << m.extra << "}";
    }
    oss << "]}";
    return oss.str();
}

void writePossibleChordsInScaleJson(
    std::ostringstream& oss,
    const Scale& sc,
    const Note& root,
    AccidentalMode mode) {
    oss << "[";
    bool first = true;
    for (const auto& def : allChordDefinitions()) {
        Chord cand{root, &def, std::nullopt};
        bool fits = true;
        for (const auto& cn : cand.notes(mode)) {
            if (!sc.containsPitchClass(cn.pitchClass())) {
                fits = false;
                break;
            }
        }
        if (!fits) continue;
        if (!first) oss << ",";
        first = false;
        oss << "{\"id\":\"" << jsonEscape(def.id) << "\""
            << ",\"symbol\":\"" << jsonEscape(cand.symbol(false, mode)) << "\""
            << ",\"name\":\"" << jsonEscape(def.polishName) << "\""
            << ",\"category\":\"" << jsonEscape(def.category) << "\"}";
    }
    oss << "]";
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
            << ",\"chordId\":\"" << jsonEscape(d.chord.definition ? d.chord.definition->id : "major") << "\""
            << ",\"pcs\":[" << pcs.str() << "]"
            << ",\"possible\":";
        writePossibleChordsInScaleJson(oss, sc, d.chord.root, mode);
        oss << "}";
    }
    oss << "]}";
    return oss.str();
}

std::string handleArpeggioApi(std::string_view query) {
    int rootPc = ((getQueryInt(query, "rootPc", 0) % 12) + 12) % 12;
    std::string chordId = getQueryParam(query, "chordId", "minor");
    int patternInt = getQueryInt(query, "pattern", 6);
    int posIdx = getQueryInt(query, "posIdx", 0);
    int sweepStrings = getQueryInt(query, "sweepStrings", 3);
    int direction = getQueryInt(query, "direction", 0);
    int seqStyle = getQueryInt(query, "seqStyle", 0);
    int stringSet = getQueryInt(query, "stringSet", 0);
    int octaves = getQueryInt(query, "octaves", 2);
    int startString = getQueryInt(query, "startStr", -1);   // -1 = od najniższej struny
    int returnString = getQueryInt(query, "returnStr", -1); // -1 = pełny powrót do najniższej
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
        auto shapes = generateSweepShapes(fb, ch, sweepStrings, direction, mode, nullptr, stringSet);
        if (!shapes.empty()) {
            resolvedPosIdx = std::clamp(posIdx, 0, static_cast<int>(shapes.size()) - 1);
            for (const auto& sh : shapes) {
                positions.push_back({sh.positionIndex, sh.label, sh.minFret, sh.maxFret, sh.hasFingerRoll});
            }
            activeNodes = shapes[static_cast<std::size_t>(resolvedPosIdx)].nodes;
            activeSteps = (seqStyle == 0 && startString < 0 && returnString < 0)
                ? shapes[static_cast<std::size_t>(resolvedPosIdx)].steps
                : buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
        }
    } else if (pat == ArpeggioPattern::RootOctave) {
        constexpr std::array<const char*, 4> octNames = {"Od str. E6 (niska)", "Od str. A5", "Od str. D4", "Od str. E6 (wysoka)"};
        resolvedPosIdx = std::clamp(posIdx, 0, 3);
        for (int i = 0; i < 4; ++i) {
            auto ns = generateRootOctaveArpeggio(fb, ch, i, octaves, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, std::string(octNames[static_cast<std::size_t>(i)]) + " · pr. " + std::to_string(mn) + "–" + std::to_string(mx), mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
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
                    auto segSteps = buildArpeggioSequenceSteps(fb, invs[i].nodes, pat, direction == 2 ? 2 : 1, mode, seqStyle, startString, returnString);
                    for (auto& st : segSteps) {
                        st.segmentIndex = static_cast<int>(i);
                        activeSteps.push_back(std::move(st));
                    }
                }
            } else {
                resolvedPosIdx = std::clamp(posIdx, 0, static_cast<int>(invs.size()) - 1);
                activeNodes = invs[static_cast<std::size_t>(resolvedPosIdx)].nodes;
                activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
            }
        }
    } else if (pat == ArpeggioPattern::CAGED) {
        constexpr std::array<const char*, 5> cagedNames = {"Kształt C", "Kształt A", "Kształt G", "Kształt E", "Kształt D"};
        resolvedPosIdx = std::clamp(posIdx, 0, 4);
        for (int i = 0; i < 5; ++i) {
            auto ns = generateArpeggio(fb, ch, ArpeggioPattern::CAGED, i, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, std::string(cagedNames[static_cast<std::size_t>(i)]) + " (pr. " + std::to_string(mn) + "–" + std::to_string(mx) + ")", mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
    } else if (pat == ArpeggioPattern::TwoNPS || pat == ArpeggioPattern::ThreeNPS) {
        int countPos = std::max<int>(3, static_cast<int>(ch.notes(mode).size()));
        resolvedPosIdx = std::clamp(posIdx, 0, countPos - 1);
        for (int i = 0; i < countPos; ++i) {
            auto ns = generateArpeggio(fb, ch, pat, i, mode, nullptr);
            int mn = 99, mx = 0;
            for (const auto& n : ns) { mn = std::min(mn, n.position.fret); mx = std::max(mx, n.position.fret); }
            if (mn > mx) mn = 0;
            positions.push_back({i, "Poz. " + std::to_string(i + 1) + " (pr. " + std::to_string(mn) + "–" + std::to_string(mx) + ")", mn, mx, false});
            if (i == resolvedPosIdx) activeNodes = std::move(ns);
        }
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
    } else {
        resolvedPosIdx = 0;
        activeNodes = fullNeckNodes;
        activeSteps = buildArpeggioSequenceSteps(fb, activeNodes, pat, direction, mode, seqStyle, startString, returnString);
    }

    auto transition = generateSweepTransition(fb, transitionId, rootPc, sweepStrings, posIdx, &ch, &sc, mode, diatonicSevenths, direction, stringSet);

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
        << ",\"scaleRootPc\":" << scaleRootPc
        << ",\"scalePcs\":[";
    {
        const auto scNotes = sc.notes(mode);
        for (std::size_t si = 0; si < scNotes.size(); ++si) {
            if (si > 0) oss << ",";
            oss << scNotes[si].pitchClass();
        }
    }
    oss << "]"
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
    oss << "],\"scaleDegrees\":[";
    {
        const auto scNotes = sc.notes(mode);
        for (std::size_t si = 0; si < scNotes.size(); ++si) {
            if (si > 0) oss << ",";
            std::string roman = (si < diatonic.size())
                ? diatonic[si].romanNumeral
                : std::to_string(si + 1);
            oss << "{\"degree\":" << (si + 1)
                << ",\"pc\":" << scNotes[si].pitchClass()
                << ",\"note\":\"" << jsonEscape(scNotes[si].toString(false)) << "\""
                << ",\"roman\":\"" << jsonEscape(roman) << "\"}";
        }
    }
    oss << "],\"possibleForRoot\":";
    writePossibleChordsInScaleJson(oss, sc, Note::fromPitchClass(rootPc, mode), mode);
    oss << ",\"diatonic\":[";
    for (std::size_t i = 0; i < diatonic.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"roman\":\"" << jsonEscape(diatonic[i].romanNumeral) << "\""
            << ",\"symbol\":\"" << jsonEscape(diatonic[i].chord.symbol(false, mode)) << "\""
            << ",\"rootPc\":" << diatonic[i].chord.root.pitchClass()
            << ",\"chordId\":\"" << jsonEscape(diatonic[i].chord.definition ? diatonic[i].chord.definition->id : "major") << "\""
            << ",\"possible\":";
        writePossibleChordsInScaleJson(oss, sc, diatonic[i].chord.root, mode);
        oss << "}";
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

Song buildSongFromJsonEvents(std::string_view eventsJson, int bpm, const Tuning& tuning = findTuning("standard")) {
    Song song{};
    song.title = "Twoje Cwiczenie";
    song.tempoBpm = std::max(40, bpm);

    Track track{};
    track.name = tuning.stringCount() <= 4 ? "Bas" : "Gitara";
    track.tuning = tuning;
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
    Song song = buildSongFromJsonEvents(evs, bpm, resolveRequestedTuning(query));
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
        auto p = fb.pitchAt(st.targetPosition.stringIndex, st.targetPosition.fret, mode, &cfg.scale);
        oss << "{\"s\":" << st.targetPosition.stringIndex
            << ",\"f\":" << st.targetPosition.fret
            << ",\"midi\":" << st.expectedMidiNote
            << ",\"note\":\"" << jsonEscape(p.note.toString(false)) << "\""
            << ",\"freq\":" << p.frequencyHz() << "}";
    }
    oss << "]}";
    return oss.str();
}

bool tryReadWebAsset(
    std::string_view urlPath,
    bool allowDiskDev,
    std::string& outBody,
    std::string_view& outMime) {
    constexpr std::string_view kPrefix = "/web/";
    if (urlPath.size() <= kPrefix.size() || urlPath.substr(0, kPrefix.size()) != kPrefix) {
        return false;
    }
    std::string_view rel = urlPath.substr(kPrefix.size());
    if (rel.empty() || rel.front() == '/' || rel.find("..") != std::string_view::npos ||
        rel.find('\\') != std::string_view::npos || rel.find('\0') != std::string_view::npos) {
        return false;
    }
    for (char c : rel) {
        const bool ok = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
                        c == '/' || c == '.' || c == '-' || c == '_';
        if (!ok) return false;
    }

    auto endsWith = [&](std::string_view suffix) {
        return rel.size() >= suffix.size() && rel.substr(rel.size() - suffix.size()) == suffix;
    };
    if (endsWith(".js") || endsWith(".mjs")) {
        outMime = "application/javascript; charset=utf-8";
    } else if (endsWith(".css")) {
        outMime = "text/css; charset=utf-8";
    } else if (endsWith(".html")) {
        outMime = "text/html; charset=utf-8";
    } else if (endsWith(".svg")) {
        outMime = "image/svg+xml";
    } else if (endsWith(".json")) {
        outMime = "application/json; charset=utf-8";
    } else if (endsWith(".wasm")) {
        outMime = "application/wasm";
    } else if (endsWith(".bin")) {
        outMime = "application/octet-stream";
    } else if (endsWith(".sf2")) {
        outMime = "application/octet-stream";
    } else {
        return false;
    }

    if (allowDiskDev) {
        const std::filesystem::path relPath(rel);
        const std::filesystem::path candidates[2] = {
            std::filesystem::path("web") / relPath,
            std::filesystem::path("..") / "web" / relPath
        };
        for (const auto& filePath : candidates) {
            std::ifstream ifs(filePath, std::ios::binary);
            if (ifs) {
                outBody.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                return true;
            }
        }
    }

    if (auto embedded = gitara::platform::globalEmbeddedAssets().find(urlPath)) {
        outBody.assign(embedded->body.data(), embedded->body.size());
        outMime = embedded->mime;
        return true;
    }
    return false;
}

bool tryReadIndexHtml(bool allowDiskDev, std::string& outBody) {
    if (allowDiskDev) {
        static std::mutex sHtmlCacheMutex;
        static std::string sCachedLiveHtml;
        static std::filesystem::file_time_type sCachedHtmlMtime{};
        static bool sHasCachedMtime = false;

        std::lock_guard<std::mutex> lock(sHtmlCacheMutex);
        const std::filesystem::path candidates[2] = {
            std::filesystem::path("app") / "ui" / "DesktopGuiHtml.h",
            std::filesystem::path("..") / "app" / "ui" / "DesktopGuiHtml.h"
        };
        for (const auto& cand : candidates) {
            std::error_code ec;
            if (!std::filesystem::exists(cand, ec) || ec) continue;
            auto mtime = std::filesystem::last_write_time(cand, ec);
            if (!ec && (sCachedLiveHtml.empty() || !sHasCachedMtime || mtime != sCachedHtmlMtime)) {
                std::ifstream ifs(cand, std::ios::binary);
                if (ifs) {
                    std::string fileContent((std::istreambuf_iterator<char>(ifs)),
                                            std::istreambuf_iterator<char>());
                    const std::string startTag = "R\"HTML(";
                    const std::string endTag = ")HTML\"";
                    const auto p1 = fileContent.find(startTag);
                    const auto p2 = fileContent.rfind(endTag);
                    if (p1 != std::string::npos && p2 != std::string::npos && p2 > p1 + startTag.size()) {
                        sCachedLiveHtml = fileContent.substr(p1 + startTag.size(), p2 - (p1 + startTag.size()));
                        sCachedHtmlMtime = mtime;
                        sHasCachedMtime = true;
                    }
                }
            }
            if (!sCachedLiveHtml.empty()) {
                outBody = sCachedLiveHtml;
                return true;
            }
        }
    }

    if (auto embedded = gitara::platform::globalEmbeddedAssets().find("/index.html")) {
        outBody.assign(embedded->body.data(), embedded->body.size());
        return true;
    }

#if !defined(GITARA_EMBEDDED_ASSETS)
    outBody = gitara::app::ui::kDesktopGuiHtml;
    return true;
#else
    return false;
#endif
}

struct RuntimeConfig {
    gitara::platform::AppPaths paths;
    bool allowDiskDevAssets = false;
    bool embeddedOnly = false;
    bool autoShutdown = false;
    bool noBrowser = false;
    bool noUpdate = false;
    bool skipUpdateOnce = false;
    bool forceUpdateCheck = false;
    bool portable = false;
};

std::mutex gStorageMutex;
std::atomic<bool> gStopRequested{false};
std::atomic<bool> gHasClientConnected{false};
std::atomic<long long> gLastClientActivityMs{0};
std::atomic<long long> gClosingSinceMs{0};
std::atomic<int> gActiveClientThreads{0};

long long nowSteadyMs() noexcept {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

void markClientActivity() noexcept {
    gHasClientConnected.store(true);
    gLastClientActivityMs.store(nowSteadyMs());
    gClosingSinceMs.store(0);
}

#ifdef _WIN32
bool isEnvFlagSet(const char* name) noexcept {
    char buf[32]{};
    const DWORD len = ::GetEnvironmentVariableA(name, buf, static_cast<DWORD>(sizeof(buf)));
    return len > 0 && buf[0] != '\0' && std::string_view(buf) != "0";
}
#endif

std::string buildVersionJson() {
    std::ostringstream oss;
    oss << "{\"version\":\"" << jsonEscape(GITARA_APP_VERSION)
        << "\",\"commit\":\"" << jsonEscape(GITARA_BUILD_COMMIT)
        << "\",\"shortCommit\":\"" << jsonEscape(GITARA_BUILD_SHORT_COMMIT)
        << "\",\"builtAt\":\"" << jsonEscape(GITARA_BUILD_TIMESTAMP)
        << "\",\"repo\":\"" << jsonEscape(GITARA_UPDATE_REPO) << "\"}";
    return oss.str();
}

std::string readSettingsJson(const std::filesystem::path& settingsFile) {
    std::lock_guard<std::mutex> lock(gStorageMutex);
    std::ifstream sfs(settingsFile, std::ios::binary);
    std::string body = "{}";
    if (sfs) {
        body.assign((std::istreambuf_iterator<char>(sfs)), std::istreambuf_iterator<char>());
        if (body.empty()) body = "{}";
    }
    return body;
}

void writeSettingsJson(const gitara::platform::AppPaths& paths, std::string_view payload) {
    if (payload.empty() || payload.front() != '{') return;
    std::lock_guard<std::mutex> lock(gStorageMutex);
    gitara::platform::ensureAppDirectories(paths);

    std::filesystem::path tmpPath = paths.settingsFile;
    tmpPath += ".tmp";
    bool wroteTmp = false;
    {
        std::ofstream ofs(tmpPath, std::ios::binary | std::ios::trunc);
        if (ofs) {
            ofs.write(payload.data(), static_cast<std::streamsize>(payload.size()));
            ofs.flush();
            wroteTmp = ofs.good();
        }
    }
    if (wroteTmp) {
        std::error_code ec;
        std::filesystem::rename(tmpPath, paths.settingsFile, ec);
        if (!ec) return;
        std::error_code rmEc;
        std::filesystem::remove(tmpPath, rmEc);
    }

    std::ofstream ofs(paths.settingsFile, std::ios::binary | std::ios::trunc);
    if (ofs) {
        ofs.write(payload.data(), static_cast<std::streamsize>(payload.size()));
    }
}

#ifdef _WIN32
int readActivePortFile(const std::filesystem::path& portFile) {
    std::ifstream ifs(portFile);
    if (!ifs) return 0;
    int port = 0;
    ifs >> port;
    return (port >= 1024 && port <= 65535) ? port : 0;
}
#endif

void writeActivePortFile(const gitara::platform::AppPaths& paths, int port) {
    gitara::platform::ensureAppDirectories(paths);
    std::ofstream ofs(paths.activePortFile, std::ios::trunc);
    if (ofs) {
        ofs << port << "\n";
    }
}

void handleClientConnection(gitara::platform::SocketHandle clientFd, const RuntimeConfig& cfg) {
    struct ThreadGuard {
        gitara::platform::SocketHandle sock;
        ~ThreadGuard() {
            gitara::platform::closeSocket(sock);
            gActiveClientThreads.fetch_sub(1);
        }
    } guard{clientFd};

    gitara::platform::setRecvTimeoutMs(clientFd, 2000);
    gitara::platform::setSendTimeoutMs(clientFd, 5000);

    // Odczyt pełnego żądania HTTP (nagłówki + opcjonalne ciało POST) bez limitu 8 KB!
    std::string reqStr;
    reqStr.reserve(16384);
    constexpr std::size_t kMaxReqBytes = 4 * 1024 * 1024;
    std::size_t headerEnd = std::string::npos;
    std::size_t contentLength = 0;
    char chunk[8192];
    while (reqStr.size() < kMaxReqBytes) {
        const int nRead = gitara::platform::socketRecv(clientFd, chunk, static_cast<int>(sizeof(chunk)));
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
                const std::size_t clPos = lowerHdr.find("content-length:");
                if (clPos != std::string::npos) {
                    std::size_t valStart = clPos + 15;
                    while (valStart < headerEnd && (reqStr[valStart] == ' ' || reqStr[valStart] == '\t')) ++valStart;
                    contentLength = static_cast<std::size_t>(std::max<long long>(0, std::atoll(reqStr.c_str() + valStart)));
                }
            }
        }
        if (headerEnd != std::string::npos) {
            const std::size_t bodyRead = reqStr.size() - (headerEnd + 4);
            if (bodyRead >= contentLength) break;
        }
    }
    if (reqStr.empty()) {
        return;
    }

    std::string_view req(reqStr);
    std::string_view reqBody = (headerEnd != std::string::npos && reqStr.size() >= headerEnd + 4)
        ? std::string_view(reqStr.data() + headerEnd + 4,
                           std::min(contentLength, reqStr.size() - (headerEnd + 4)))
        : std::string_view{};

    const std::size_t firstSpace = req.find(' ');
    const std::size_t secondSpace = (firstSpace != std::string_view::npos)
        ? req.find(' ', firstSpace + 1)
        : std::string_view::npos;

    const std::string_view fullTarget = (firstSpace != std::string_view::npos && secondSpace != std::string_view::npos)
        ? req.substr(firstSpace + 1, secondSpace - firstSpace - 1)
        : "/";

    const std::size_t qPos = fullTarget.find('?');
    const std::string_view path = (qPos == std::string_view::npos) ? fullTarget : fullTarget.substr(0, qPos);
    const std::string_view query = (qPos == std::string_view::npos) ? "" : fullTarget.substr(qPos + 1);

    auto writeAll = [&](const char* ptr, std::size_t len) {
        std::size_t sent = 0;
        while (sent < len) {
            const int toSend = static_cast<int>(std::min<std::size_t>(len - sent, 1u << 20));
            const int w = gitara::platform::socketSend(clientFd, ptr + sent, toSend);
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
            << "Content-Length: " << size << "\r\n";
        if (extraHeaders.find("Cache-Control:") == std::string_view::npos) {
            hdr << "Cache-Control: no-store\r\n";
        }
        hdr << extraHeaders
            << "Connection: close\r\n\r\n";
        const std::string hStr = hdr.str();
        writeAll(hStr.data(), hStr.size());
        if (size > 0 && data != nullptr) {
            writeAll(static_cast<const char*>(data), size);
        }
    };

    if (path == "/" || path == "/index.html") {
        markClientActivity();
        std::string htmlBody;
        if (tryReadIndexHtml(cfg.allowDiskDevAssets, htmlBody)) {
            sendResponse("200 OK", "text/html; charset=utf-8", htmlBody.data(), htmlBody.size());
        } else {
            const char* notFound = "Index HTML unavailable";
            sendResponse("500 Internal Server Error", "text/plain; charset=utf-8", notFound, std::strlen(notFound));
        }
    } else if (path.size() > 5 && path.substr(0, 5) == "/web/") {
        std::string assetBody;
        std::string_view assetMime;
        if (tryReadWebAsset(path, cfg.allowDiskDevAssets, assetBody, assetMime)) {
            const bool isHeavyModelOrWasm =
                (path.size() >= 4 && path.substr(path.size() - 4) == ".bin") ||
                (path.size() >= 5 && path.substr(path.size() - 5) == ".wasm");
            sendResponse("200 OK", assetMime, assetBody.data(), assetBody.size(),
                         isHeavyModelOrWasm ? "Cache-Control: public, max-age=3600\r\n" : "");
        } else {
            const char* notFound = "Not Found";
            sendResponse("404 Not Found", "text/plain; charset=utf-8", notFound, std::strlen(notFound));
        }
    } else if (path == "/api/ping") {
        const char* ok = "{\"ok\":true}";
        sendResponse("200 OK", "application/json; charset=utf-8", ok, std::strlen(ok));
    } else if (path == "/api/version") {
        const std::string body = buildVersionJson();
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/heartbeat") {
        markClientActivity();
        const char* ok = "{\"ok\":true}";
        sendResponse("200 OK", "application/json; charset=utf-8", ok, std::strlen(ok));
    } else if (path == "/api/closing") {
        long long expected = 0;
        gClosingSinceMs.compare_exchange_strong(expected, nowSteadyMs());
        const char* ok = "{\"ok\":true}";
        sendResponse("200 OK", "application/json; charset=utf-8", ok, std::strlen(ok));
    } else if (path == "/api/init") {
        const std::string body = handleInitApi();
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/settings") {
        const std::string body = readSettingsJson(cfg.paths.settingsFile);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/save_settings") {
        std::string jsonPayload = (!reqBody.empty() && reqBody.front() == '{')
            ? std::string(reqBody)
            : getQueryParam(query, "data", "{}");
        writeSettingsJson(cfg.paths, jsonPayload);
        const char* ok = "{\"ok\":true}";
        sendResponse("200 OK", "application/json; charset=utf-8", ok, std::strlen(ok));
    } else if (path == "/api/fretboard") {
        const std::string body = handleFretboardApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/voicings") {
        const std::string body = handleVoicingsApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/harmonics") {
        const std::string body = handleHarmonicsApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/search") {
        const std::string body = handleSearchApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/detect-scale") {
        const std::string body = handleDetectScaleApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/diatonic") {
        const std::string body = handleDiatonicApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/arpeggio") {
        const std::string body = handleArpeggioApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/tab_preview") {
        const std::string body = handleTabPreviewApi(query, reqBody);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/workshop_events") {
        const std::string body = handleWorkshopEventsApi(query);
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/export/gp5") {
        std::string evs = (!reqBody.empty() && reqBody.front() == '[')
            ? std::string(reqBody)
            : getQueryParam(query, "events", "[]");
        Song song = buildSongFromJsonEvents(evs, getQueryInt(query, "bpm", 120), resolveRequestedTuning(query));
        auto bytes = exportToGp5Bytes(song);
        sendResponse("200 OK", "application/octet-stream", bytes.data(), bytes.size(),
                     "Content-Disposition: attachment; filename=\"cwiczenie_tuxguitar.gp5\"\r\n");
    } else if (path == "/api/export/mid") {
        std::string evs = (!reqBody.empty() && reqBody.front() == '[')
            ? std::string(reqBody)
            : getQueryParam(query, "events", "[]");
        Song song = buildSongFromJsonEvents(evs, getQueryInt(query, "bpm", 120), resolveRequestedTuning(query));
        auto bytes = exportToMidiBytes(song);
        sendResponse("200 OK", "audio/midi", bytes.data(), bytes.size(),
                     "Content-Disposition: attachment; filename=\"cwiczenie_tuxguitar.mid\"\r\n");
    } else if (path == "/api/export/progression_gp5") {
        ChordProgression prog{};
        bool hasBodyJson = false;
        for (char c : reqBody) {
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
            hasBodyJson = (c == '{');
            break;
        }
        if (hasBodyJson) {
            prog = buildCustomProgressionFromJson(reqBody);
        } else {
            auto progOpt = findBuiltinProgression(getQueryParam(query, "id", "blues_12"));
            prog = progOpt.value_or(builtinProgressions().front());
            int reqKey = getQueryInt(query, "keyRootPc", -1);
            int reqBpm = getQueryInt(query, "bpm", 0);
            if (reqKey >= 0 && reqKey < 12) {
                prog = transposeProgression(prog, Note::fromPitchClass(reqKey));
            }
            if (reqBpm >= 40 && reqBpm <= 240) {
                prog.tempoBpm = reqBpm;
            }
        }
        Song song = generateBackingTrackSong(prog);
        auto bytes = exportToGp5Bytes(song);
        std::string fname = sanitizeAsciiFilenameSlug(hasBodyJson ? prog.title : prog.id, "progresja") + "_tuxguitar.gp5";
        std::string disp = "Content-Disposition: attachment; filename=\"" + fname + "\"\r\n";
        sendResponse("200 OK", "application/octet-stream", bytes.data(), bytes.size(), disp);
    } else if (path == "/api/library") {
        std::string body;
        {
            std::lock_guard<std::mutex> lock(gStorageMutex);
            body = gitara::score::listAllLibraryItemsJson(cfg.paths.userLibraryDir);
        }
        sendResponse("200 OK", "application/json; charset=utf-8", body.data(), body.size());
    } else if (path == "/api/library/save") {
        std::string cat = getQueryParam(query, "category", "");
        std::string id = getQueryParam(query, "id", "");
        std::string errMsg;
        bool saved = false;
        {
            std::lock_guard<std::mutex> lock(gStorageMutex);
            saved = gitara::score::saveLibraryItem(cfg.paths.userLibraryDir, cat, id, reqBody, &errMsg);
        }
        if (saved) {
            std::string okJson = "{\"ok\":true,\"category\":\"" + jsonEscape(cat) +
                                 "\",\"id\":\"" + jsonEscape(gitara::score::sanitizeLibraryId(id)) + "\"}";
            sendResponse("200 OK", "application/json; charset=utf-8", okJson.data(), okJson.size());
        } else {
            std::string errJson = "{\"ok\":false,\"error\":\"" + jsonEscape(errMsg) + "\"}";
            sendResponse("400 Bad Request", "application/json; charset=utf-8", errJson.data(), errJson.size());
        }
    } else if (path == "/api/library/delete") {
        std::string cat = getQueryParam(query, "category", "");
        std::string id = getQueryParam(query, "id", "");
        std::string errMsg;
        bool deleted = false;
        {
            std::lock_guard<std::mutex> lock(gStorageMutex);
            deleted = gitara::score::deleteLibraryItem(cfg.paths.userLibraryDir, cat, id, &errMsg);
        }
        if (deleted) {
            const char* okJson = "{\"ok\":true}";
            sendResponse("200 OK", "application/json; charset=utf-8", okJson, std::strlen(okJson));
        } else {
            const bool badInput = !gitara::score::isAllowedLibraryCategory(cat) ||
                                  gitara::score::sanitizeLibraryId(id).empty();
            std::string errJson = "{\"ok\":false,\"error\":\"" + jsonEscape(errMsg) + "\"}";
            sendResponse(badInput ? "400 Bad Request" : "404 Not Found",
                         "application/json; charset=utf-8", errJson.data(), errJson.size());
        }
    } else if (path == "/api/studio/export_gp5") {
        Song studioSong{};
        std::string parseErr;
        if (!gitara::score::parseStudioSongJson(reqBody, studioSong, &parseErr)) {
            sendResponse("400 Bad Request", "text/plain; charset=utf-8", parseErr.data(), parseErr.size());
        } else {
            auto bytes = exportToGp5Bytes(studioSong);
            sendResponse("200 OK", "application/octet-stream", bytes.data(), bytes.size(),
                         "Content-Disposition: attachment; filename=\"studio_tab.gp5\"\r\n");
        }
    } else if (path == "/api/studio/export_mid") {
        Song studioSong{};
        std::string parseErr;
        if (!gitara::score::parseStudioSongJson(reqBody, studioSong, &parseErr)) {
            sendResponse("400 Bad Request", "text/plain; charset=utf-8", parseErr.data(), parseErr.size());
        } else {
            auto bytes = exportToMidiBytes(studioSong);
            sendResponse("200 OK", "audio/midi", bytes.data(), bytes.size(),
                         "Content-Disposition: attachment; filename=\"studio_tab.mid\"\r\n");
        }
    } else {
        const char* notFound = "{}";
        sendResponse("404 Not Found", "application/json", notFound, 2);
    }
}

int runHeadlessCheck(bool embeddedOnly) {
#if defined(GITARA_EMBEDDED_ASSETS)
    const bool useEmbeddedDirect = true;
    (void)embeddedOnly;
#else
    const bool useEmbeddedDirect = embeddedOnly;
#endif

    const auto initJson = handleInitApi();
    const auto fbJson = handleFretboardApi("rootPc=0&scaleId=major");

    std::string htmlBody;
    const bool htmlOk = useEmbeddedDirect
        ? [&]() {
              if (auto found = gitara::platform::globalEmbeddedAssets().find("/index.html")) {
                  htmlBody.assign(found->body.data(), found->body.size());
                  return !htmlBody.empty();
              }
              return false;
          }()
        : tryReadIndexHtml(true, htmlBody) && !htmlBody.empty();

    auto verifyWebAsset = [&](std::string_view urlPath, std::string_view expectedMimeSubstr) -> bool {
        std::string body;
        std::string_view mime;
        if (useEmbeddedDirect) {
            if (auto found = gitara::platform::globalEmbeddedAssets().find(urlPath)) {
                body.assign(found->body.data(), found->body.size());
                mime = found->mime;
            } else {
                return false;
            }
        } else {
            if (!tryReadWebAsset(urlPath, true, body, mime)) {
                return false;
            }
        }
        return !body.empty() && mime.find(expectedMimeSubstr) != std::string_view::npos;
    };

    const bool webDspOk = verifyWebAsset("/web/js/dsp/guitarDspEngine.js", "javascript");
    const bool pitchPleaseOk =
        verifyWebAsset("/web/vendor/pitchplease/pitchplease.js", "javascript") &&
        verifyWebAsset("/web/vendor/pitchplease/engine/incport.wasm", "application/wasm") &&
        verifyWebAsset("/web/vendor/pitchplease/models/piano.bin", "application/octet-stream") &&
        verifyWebAsset("/web/js/dsp/pitchPleaseBridge.js", "javascript");
    const bool studioAssetsOk =
        verifyWebAsset("/web/css/studio.css", "css") &&
        verifyWebAsset("/web/js/studio/studioModel.js", "javascript") &&
        verifyWebAsset("/web/js/studio/studioPalette.js", "javascript") &&
        verifyWebAsset("/web/js/studio/studioInputs.js", "javascript") &&
        verifyWebAsset("/web/js/studio/studioSoundFont.js", "javascript") &&
        verifyWebAsset("/web/js/studio/studioView.js", "javascript");

    return (!initJson.empty() && !fbJson.empty() && htmlOk &&
            webDspOk && pitchPleaseOk && studioAssetsOk) ? 0 : 1;
}

int appMain(int argc, char** argv) {
    bool headlessCheck = false;
    bool printVersion = false;
    RuntimeConfig cfg{};
#ifdef _WIN32
    cfg.autoShutdown = true;
#else
    cfg.autoShutdown = false;
#endif

    std::vector<std::string> forwardArgs;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg(argv[i]);
        forwardArgs.emplace_back(arg);
        if (arg == "--headless-check") headlessCheck = true;
        else if (arg == "--version") printVersion = true;
        else if (arg == "--no-update") cfg.noUpdate = true;
        else if (arg == "--skip-update-once") cfg.skipUpdateOnce = true;
        else if (arg == "--force-update-check") cfg.forceUpdateCheck = true;
        else if (arg == "--portable") cfg.portable = true;
        else if (arg == "--no-browser") cfg.noBrowser = true;
        else if (arg == "--auto-shutdown") cfg.autoShutdown = true;
        else if (arg == "--no-auto-shutdown") cfg.autoShutdown = false;
        else if (arg == "--embedded-only") cfg.embeddedOnly = true;
    }

    if (printVersion) {
        gitara::platform::attachConsoleIfPresent();
        std::cout << "Apka Gitarowa v" << GITARA_APP_VERSION
                  << " (" << GITARA_BUILD_SHORT_COMMIT
                  << ", built " << GITARA_BUILD_TIMESTAMP
                  << ", repo " << GITARA_UPDATE_REPO << ")\n";
        return 0;
    }

    if (headlessCheck) {
        gitara::platform::attachConsoleIfPresent();
        return runHeadlessCheck(cfg.embeddedOnly);
    }

    cfg.paths = gitara::platform::resolveAppPaths(cfg.portable);
#if defined(GITARA_EMBEDDED_ASSETS)
    cfg.allowDiskDevAssets = cfg.paths.isDevCheckout && !cfg.embeddedOnly;
#else
    cfg.allowDiskDevAssets = !cfg.embeddedOnly;
#endif

    gitara::platform::cleanupPreviousUpdateArtifacts();

    gitara::platform::SocketInit socketInit;
    if (!socketInit.ok()) {
        std::cerr << "Blad inicjalizacji warstwy sieciowej.\n";
        return 1;
    }

    gitara::platform::SingleInstanceLock singleInstance;
#ifdef _WIN32
    const int lockRetryMs = cfg.skipUpdateOnce ? 2500 : 0;
    if (!singleInstance.acquire(lockRetryMs)) {
        const int existingPort = readActivePortFile(cfg.paths.activePortFile);
        if (existingPort > 0 && gitara::platform::probeLocalhostPing(existingPort, 500)) {
            (void)gitara::platform::launchAppWindow(existingPort, cfg.paths);
            return 0;
        }
        return 0;
    }

    const bool envNoUpdate = isEnvFlagSet("GITARA_NO_UPDATE");
    if (!cfg.noUpdate && !cfg.skipUpdateOnce && !envNoUpdate) {
        gitara::platform::AutoUpdateOptions updOpts{};
        updOpts.currentCommit = GITARA_BUILD_COMMIT;
        updOpts.repo = GITARA_UPDATE_REPO;
        updOpts.tag = GITARA_UPDATE_TAG;
        updOpts.forceCheck = cfg.forceUpdateCheck;
        updOpts.forwardArgs = forwardArgs;

        const auto outcome = gitara::platform::runPreLaunchAutoUpdate(updOpts);
        if (outcome.result == gitara::platform::AutoUpdateResult::UpdatedAndRelaunchReady) {
            singleInstance.release();
            if (gitara::platform::spawnRelaunchedApp(outcome.relaunchExePath, forwardArgs)) {
                return 0;
            }
            (void)singleInstance.acquire(0);
        }
    }
#endif

    gitara::platform::ensureAppDirectories(cfg.paths);
    // Wstępna inicjalizacja statycznych słowników przed startem wątków HTTP
    (void)handleInitApi();

    gitara::platform::SocketHandle serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == gitara::platform::kInvalidSocket) {
        std::cerr << "Blad utworzenia gniazda GUI.\n";
        return 1;
    }

    gitara::platform::setReuseAddr(serverFd);

    int port = 8765;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool bound = false;
    for (int p = 8765; p < 8785; ++p) {
        addr.sin_port = htons(static_cast<std::uint16_t>(p));
        if (::bind(serverFd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) == 0) {
            port = p;
            bound = true;
            break;
        }
    }
    if (!bound || ::listen(serverFd, 32) < 0) {
        std::cerr << "Nie udalo sie uruchomic lokalnego serwera GUI.\n";
        gitara::platform::closeSocket(serverFd);
        return 1;
    }

    writeActivePortFile(cfg.paths, port);

    const auto dataDirU8 = cfg.paths.dataDir.u8string();
    std::cout << "====================================================================\n";
    std::cout << " Apka Gitarowa (GUI) uruchomiona!\n";
    std::cout << " -> Adres lokalny: http://127.0.0.1:" << port << "\n";
    std::cout << " -> Katalog danych: "
              << std::string_view(reinterpret_cast<const char*>(dataDirU8.data()), dataDirU8.size())
              << "\n";
    std::cout << "====================================================================\n";

    gitara::platform::BrowserLaunchResult browserLaunch{};
    if (!cfg.noBrowser) {
        browserLaunch = gitara::platform::launchAppWindow(port, cfg.paths);
    }

    const long long startTimeMs = nowSteadyMs();
    gLastClientActivityMs.store(startTimeMs);
    long long browserExitedAtMs = 0;

    while (!gStopRequested.load()) {
        const int ready = gitara::platform::waitReadableMs(serverFd, 200);
        if (ready > 0) {
            sockaddr_in clientAddr{};
#ifdef _WIN32
            int clientLen = sizeof(clientAddr);
#else
            socklen_t clientLen = sizeof(clientAddr);
#endif
            const gitara::platform::SocketHandle clientFd =
                ::accept(serverFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
            if (clientFd != gitara::platform::kInvalidSocket) {
                gActiveClientThreads.fetch_add(1);
                std::thread([clientFd, &cfg]() {
                    handleClientConnection(clientFd, cfg);
                }).detach();
            }
        }

        if (cfg.autoShutdown) {
            const long long nowMs = nowSteadyMs();
            const bool connected = gHasClientConnected.load();
            const long long lastAct = gLastClientActivityMs.load();
            const long long closingAt = gClosingSinceMs.load();

            if (browserLaunch.dedicatedProcess && gitara::platform::isBrowserProcessExited(browserLaunch)) {
                if (browserExitedAtMs == 0) {
                    browserExitedAtMs = nowMs;
                } else if (nowMs - browserExitedAtMs >= 1500) {
                    // Jeśli aplikacja przekazała okno do istniejącego procesu Edge i nadal wysyła heartbeat,
                    // nie zamykamy serwera — przechodzimy na kontrolę przez /api/closing i heartbeat.
                    if (connected && (nowMs - lastAct) < 2500) {
                        gitara::platform::closeBrowserHandle(browserLaunch);
                        browserExitedAtMs = 0;
                    } else if (connected || (nowMs - startTimeMs) >= 5000) {
                        gStopRequested.store(true);
                    }
                }
            }

            if (closingAt > 0 && (nowMs - closingAt) >= 4000) {
                gStopRequested.store(true);
            } else if (connected && (nowMs - lastAct) >= 15000) {
                gStopRequested.store(true);
            } else if (!connected && (nowMs - startTimeMs) >= 60000) {
                gStopRequested.store(true);
            }
        }
    }

    gitara::platform::closeSocket(serverFd);
    for (int waitIter = 0; waitIter < 100 && gActiveClientThreads.load() > 0; ++waitIter) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::error_code rmEc;
    std::filesystem::remove(cfg.paths.activePortFile, rmEc);
    gitara::platform::closeBrowserHandle(browserLaunch);
    singleInstance.release();
    return 0;
}

} // namespace

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    int wargc = 0;
    LPWSTR* wargv = ::CommandLineToArgvW(::GetCommandLineW(), &wargc);
    std::vector<std::string> storage;
    std::vector<char*> argvPtrs;
    if (wargv != nullptr && wargc > 0) {
        storage.reserve(static_cast<std::size_t>(wargc));
        argvPtrs.reserve(static_cast<std::size_t>(wargc) + 1);
        for (int i = 0; i < wargc; ++i) {
            const int len = ::WideCharToMultiByte(
                CP_UTF8, 0, wargv[i], -1, nullptr, 0, nullptr, nullptr);
            std::string s(len > 0 ? static_cast<std::size_t>(len - 1) : 0u, '\0');
            if (len > 1) {
                ::WideCharToMultiByte(
                    CP_UTF8, 0, wargv[i], -1, s.data(), len, nullptr, nullptr);
            }
            storage.push_back(std::move(s));
        }
        ::LocalFree(wargv);
        for (auto& s : storage) {
            argvPtrs.push_back(s.data());
        }
    } else {
        storage.emplace_back("ApkaGitarowa.exe");
        argvPtrs.push_back(storage.back().data());
    }
    argvPtrs.push_back(nullptr);
    return appMain(static_cast<int>(storage.size()), argvPtrs.data());
}
#else
int main(int argc, char** argv) {
    return appMain(argc, argv);
}
#endif

#endif
