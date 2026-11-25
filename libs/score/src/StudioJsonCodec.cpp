#include "gitara/score/StudioJsonCodec.h"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <map>
#include <utility>
#include <vector>

namespace gitara::score {
namespace {

enum class JsonType { Null, Bool, Number, String, Array, Object };

struct JsonValue {
    JsonType type{JsonType::Null};
    bool boolVal{false};
    double numVal{0.0};
    std::string strVal{};
    std::vector<JsonValue> arrVal{};
    std::map<std::string, JsonValue, std::less<>> objVal{};

    [[nodiscard]] const JsonValue* find(std::string_view key) const noexcept {
        if (type != JsonType::Object) return nullptr;
        auto it = objVal.find(key);
        return (it != objVal.end()) ? &it->second : nullptr;
    }

    [[nodiscard]] std::string getString(std::string_view key, std::string def = {}) const {
        const auto* v = find(key);
        return (v && v->type == JsonType::String) ? v->strVal : def;
    }

    [[nodiscard]] double getDouble(std::string_view key, double def = 0.0) const noexcept {
        const auto* v = find(key);
        return (v && v->type == JsonType::Number) ? v->numVal : def;
    }

    [[nodiscard]] int getInt(std::string_view key, int def = 0) const noexcept {
        const auto* v = find(key);
        return (v && v->type == JsonType::Number) ? static_cast<int>(std::llround(v->numVal)) : def;
    }

    [[nodiscard]] bool getBool(std::string_view key, bool def = false) const noexcept {
        const auto* v = find(key);
        if (!v) return def;
        if (v->type == JsonType::Bool) return v->boolVal;
        if (v->type == JsonType::Number) return v->numVal != 0.0;
        return def;
    }
};

class JsonParser {
public:
    explicit JsonParser(std::string_view src) : src_(src) {}

    bool parse(JsonValue& out, std::string& err) {
        skipWs();
        if (!parseValue(out, 0, err)) return false;
        skipWs();
        if (pos_ != src_.size()) {
            err = "Nieoczekiwane znaki po końcu JSON.";
            return false;
        }
        return true;
    }

private:
    std::string_view src_;
    std::size_t pos_{0};

    void skipWs() noexcept {
        while (pos_ < src_.size()) {
            char c = src_[pos_];
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                ++pos_;
            } else {
                break;
            }
        }
    }

    bool parseValue(JsonValue& out, int depth, std::string& err) {
        if (depth > 40) {
            err = "Przekroczona maksymalna głębokość JSON.";
            return false;
        }
        skipWs();
        if (pos_ >= src_.size()) {
            err = "Nieoczekiwany koniec danych JSON.";
            return false;
        }
        char c = src_[pos_];
        if (c == '{') return parseObject(out, depth, err);
        if (c == '[') return parseArray(out, depth, err);
        if (c == '"') {
            out.type = JsonType::String;
            return parseString(out.strVal, err);
        }
        if (c == 't' && src_.substr(pos_, 4) == "true") {
            pos_ += 4;
            out.type = JsonType::Bool;
            out.boolVal = true;
            return true;
        }
        if (c == 'f' && src_.substr(pos_, 5) == "false") {
            pos_ += 5;
            out.type = JsonType::Bool;
            out.boolVal = false;
            return true;
        }
        if (c == 'n' && src_.substr(pos_, 4) == "null") {
            pos_ += 4;
            out.type = JsonType::Null;
            return true;
        }
        if (c == '-' || (c >= '0' && c <= '9')) {
            return parseNumber(out, err);
        }
        err = "Nieprawidłowy token w JSON.";
        return false;
    }

    static void appendUtf8(std::string& dst, std::uint32_t cp) {
        if (cp <= 0x7Fu) {
            dst.push_back(static_cast<char>(cp));
        } else if (cp <= 0x7FFu) {
            dst.push_back(static_cast<char>(0xC0u | ((cp >> 6) & 0x1Fu)));
            dst.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        } else if (cp <= 0xFFFFu) {
            dst.push_back(static_cast<char>(0xE0u | ((cp >> 12) & 0x0Fu)));
            dst.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        } else {
            dst.push_back(static_cast<char>(0xF0u | ((cp >> 18) & 0x07u)));
            dst.push_back(static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
            dst.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
        }
    }

    bool parseHex4(std::uint32_t& out) noexcept {
        if (pos_ + 4 > src_.size()) return false;
        std::uint32_t val = 0;
        for (int i = 0; i < 4; ++i) {
            char c = src_[pos_++];
            val <<= 4;
            if (c >= '0' && c <= '9') val |= static_cast<std::uint32_t>(c - '0');
            else if (c >= 'a' && c <= 'f') val |= static_cast<std::uint32_t>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') val |= static_cast<std::uint32_t>(c - 'A' + 10);
            else return false;
        }
        out = val;
        return true;
    }

    bool parseString(std::string& out, std::string& err) {
        if (pos_ >= src_.size() || src_[pos_] != '"') {
            err = "Oczekiwano cudzysłowu.";
            return false;
        }
        ++pos_;
        out.clear();
        while (pos_ < src_.size()) {
            char c = src_[pos_++];
            if (c == '"') return true;
            if (c == '\\') {
                if (pos_ >= src_.size()) {
                    err = "Niekompletna sekwencja ucieczki JSON.";
                    return false;
                }
                char esc = src_[pos_++];
                switch (esc) {
                    case '"': out.push_back('"'); break;
                    case '\\': out.push_back('\\'); break;
                    case '/': out.push_back('/'); break;
                    case 'b': out.push_back('\b'); break;
                    case 'f': out.push_back('\f'); break;
                    case 'n': out.push_back('\n'); break;
                    case 'r': out.push_back('\r'); break;
                    case 't': out.push_back('\t'); break;
                    case 'u': {
                        std::uint32_t cp = 0;
                        if (!parseHex4(cp)) {
                            err = "Nieprawidłowa sekwencja \\uXXXX.";
                            return false;
                        }
                        if (cp >= 0xD800u && cp <= 0xDBFFu && pos_ + 2 <= src_.size() &&
                            src_[pos_] == '\\' && src_[pos_ + 1] == 'u') {
                            pos_ += 2;
                            std::uint32_t lo = 0;
                            if (parseHex4(lo) && lo >= 0xDC00u && lo <= 0xDFFFu) {
                                cp = 0x10000u + (((cp - 0xD800u) << 10) | (lo - 0xDC00u));
                            }
                        }
                        appendUtf8(out, cp);
                        break;
                    }
                    default:
                        err = "Nieznana sekwencja ucieczki w JSON.";
                        return false;
                }
            } else {
                out.push_back(c);
            }
        }
        err = "Niedomknięty łańcuch znaków JSON.";
        return false;
    }

    bool parseNumber(JsonValue& out, std::string& err) {
        std::size_t start = pos_;
        if (src_[pos_] == '-') ++pos_;
        if (pos_ >= src_.size() || !std::isdigit(static_cast<unsigned char>(src_[pos_]))) {
            err = "Nieprawidłowa liczba JSON.";
            return false;
        }
        while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        if (pos_ < src_.size() && src_[pos_] == '.') {
            ++pos_;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        }
        if (pos_ < src_.size() && (src_[pos_] == 'e' || src_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < src_.size() && (src_[pos_] == '+' || src_[pos_] == '-')) ++pos_;
            while (pos_ < src_.size() && std::isdigit(static_cast<unsigned char>(src_[pos_]))) ++pos_;
        }
        std::string tmp(src_.substr(start, pos_ - start));
        char* endPtr = nullptr;
        double val = std::strtod(tmp.c_str(), &endPtr);
        if (endPtr == tmp.c_str() || !std::isfinite(val)) {
            err = "Nieprawidłowa wartość liczbowa JSON.";
            return false;
        }
        out.type = JsonType::Number;
        out.numVal = val;
        return true;
    }

    bool parseArray(JsonValue& out, int depth, std::string& err) {
        out.type = JsonType::Array;
        out.arrVal.clear();
        ++pos_;
        skipWs();
        if (pos_ < src_.size() && src_[pos_] == ']') {
            ++pos_;
            return true;
        }
        while (true) {
            JsonValue elem;
            if (!parseValue(elem, depth + 1, err)) return false;
            out.arrVal.push_back(std::move(elem));
            skipWs();
            if (pos_ >= src_.size()) {
                err = "Niedomknięta tablica JSON.";
                return false;
            }
            if (src_[pos_] == ']') {
                ++pos_;
                return true;
            }
            if (src_[pos_] != ',') {
                err = "Oczekiwano ',' lub ']' w tablicy JSON.";
                return false;
            }
            ++pos_;
        }
    }

    bool parseObject(JsonValue& out, int depth, std::string& err) {
        out.type = JsonType::Object;
        out.objVal.clear();
        ++pos_;
        skipWs();
        if (pos_ < src_.size() && src_[pos_] == '}') {
            ++pos_;
            return true;
        }
        while (true) {
            skipWs();
            std::string key;
            if (!parseString(key, err)) return false;
            skipWs();
            if (pos_ >= src_.size() || src_[pos_] != ':') {
                err = "Oczekiwano ':' po kluczu JSON.";
                return false;
            }
            ++pos_;
            JsonValue val;
            if (!parseValue(val, depth + 1, err)) return false;
            out.objVal.insert_or_assign(std::move(key), std::move(val));
            skipWs();
            if (pos_ >= src_.size()) {
                err = "Niedomknięty obiekt JSON.";
                return false;
            }
            if (src_[pos_] == '}') {
                ++pos_;
                return true;
            }
            if (src_[pos_] != ',') {
                err = "Oczekiwano ',' lub '}' w obiekcie JSON.";
                return false;
            }
            ++pos_;
        }
    }
};

} // namespace

bool parseStudioSongJson(std::string_view jsonText, Song& outSong, std::string* outError) {
    JsonParser parser(jsonText);
    JsonValue root;
    std::string err;
    if (!parser.parse(root, err) || root.type != JsonType::Object) {
        if (outError) *outError = err.empty() ? "Korzeń JSON musi być obiektem." : err;
        return false;
    }

    Song song{};
    song.title = root.getString("title", "Nowe ćwiczenie");
    song.artist = root.getString("artist", "Apka Gitarowa");
    song.album = root.getString("album", "Studio TAB");
    song.tempoBpm = std::clamp(root.getInt("tempoBpm", 120), 20, 320);
    song.keyRootPc = ((root.getInt("keyRootPc", 0) % 12) + 12) % 12;
    song.keyIsMinor = root.getBool("keyIsMinor", false);

    const auto* tracksVal = root.find("tracks");
    if (!tracksVal || tracksVal->type != JsonType::Array || tracksVal->arrVal.empty()) {
        if (outError) *outError = "Brak niepustej tablicy ścieżek 'tracks' w JSON.";
        return false;
    }

    for (std::size_t tIdx = 0; tIdx < tracksVal->arrVal.size(); ++tIdx) {
        const auto& tj = tracksVal->arrVal[tIdx];
        if (tj.type != JsonType::Object) continue;

        Track tr{};
        tr.name = tj.getString("name", "Ścieżka");
        std::string instType = tj.getString("instrumentType", "guitar");
        tr.isPercussion = tj.getBool("isPercussion", false) || (instType == "drums");
        tr.capo = tr.isPercussion ? 0 : std::clamp(tj.getInt("capo", 0), 0, 12);
        tr.midiChannel = tr.isPercussion ? 9 : std::clamp(tj.getInt("midiChannel", static_cast<int>(tIdx)), 0, 15);
        tr.midiProgram = tr.isPercussion ? 0 : std::clamp(tj.getInt("midiProgram", 25), 0, 127);

        if (tr.isPercussion) {
            tr.tuning = gitara::theory::findTuning("standard");
        } else {
            const auto* openArr = tj.find("openStringMidis");
            if (openArr && openArr->type == JsonType::Array && !openArr->arrVal.empty()) {
                tr.tuning.id = tj.getString("tuningId", "custom");
                tr.tuning.polishName = tr.tuning.id;
                tr.tuning.openStrings.clear();
                for (const auto& mVal : openArr->arrVal) {
                    if (mVal.type == JsonType::Number) {
                        int m = std::clamp(static_cast<int>(std::llround(mVal.numVal)), 12, 108);
                        tr.tuning.openStrings.push_back(
                            gitara::theory::Pitch::fromMidiNumber(m, gitara::theory::AccidentalMode::Sharps));
                    }
                }
            }
            if (tr.tuning.openStrings.empty()) {
                tr.tuning = gitara::theory::findTuning(tj.getString("tuningId", "standard"));
            }
        }

        const auto* measuresVal = tj.find("measures");
        if (measuresVal && measuresVal->type == JsonType::Array) {
            for (const auto& mj : measuresVal->arrVal) {
                if (mj.type != JsonType::Object) continue;
                Measure meas{};
                meas.numerator = std::clamp(mj.getInt("numerator", 4), 1, 32);
                meas.denominator = std::clamp(mj.getInt("denominator", 4), 1, 16);
                meas.repeatOpen = mj.getBool("repeatOpen", false);
                meas.repeatClose = std::clamp(mj.getInt("repeatClose", 0), 0, 99);
                meas.voltaMask = std::clamp(mj.getInt("voltaMask", 0), 0, 255);
                meas.sectionMarker = mj.getString("sectionMarker", "");
                meas.tripletFeel = std::clamp(mj.getInt("tripletFeel", 0), 0, 3);
                meas.isDoubleBar = mj.getBool("isDoubleBar", false);

                const auto* beatsVal = mj.find("beats");
                if (beatsVal && beatsVal->type == JsonType::Array) {
                    for (const auto& bj : beatsVal->arrVal) {
                        if (bj.type != JsonType::Object) continue;
                        Beat beat{};
                        int durInt = std::clamp(bj.getInt("duration", 0), -2, 4);
                        beat.duration = static_cast<DurationValue>(durInt);
                        beat.dots = std::clamp(bj.getInt("dots", 0), 0, 2);
                        beat.dotted = beat.dots > 0 || bj.getBool("dotted", false);
                        beat.tupletNumerator = std::max(0, bj.getInt("tupletNumerator", 0));
                        beat.tupletDenominator = std::max(0, bj.getInt("tupletDenominator", 0));
                        beat.isRest = bj.getBool("isRest", false);
                        beat.text = bj.getString("text", "");
                        beat.chordSymbol = bj.getString("chordSymbol", "");
                        beat.dynamic = std::clamp(bj.getInt("dynamic", 6), 1, 8);
                        beat.brushStroke = bj.getString("brushStroke", "");
                        beat.pickStroke = std::clamp(bj.getInt("pickStroke", 0), 0, 2);
                        beat.fermata = bj.getBool("fermata", false);

                        const auto* notesVal = bj.find("notes");
                        if (notesVal && notesVal->type == JsonType::Array) {
                            for (const auto& nj : notesVal->arrVal) {
                                if (nj.type != JsonType::Object) continue;
                                TabNote note{};
                                note.stringIndex = std::max(0, nj.getInt("string", nj.getInt("stringIndex", 0)));
                                note.fret = std::clamp(nj.getInt("fret", 0), 0, 127);
                                note.velocity = std::clamp(nj.getInt("velocity", 95), 1, 127);
                                note.tied = nj.getBool("tied", false);
                                note.hammerOn = nj.getBool("hammerOn", false);
                                note.slideType = nj.getString("slideType", "");
                                note.slide = nj.getBool("slide", false) || !note.slideType.empty();
                                note.palmMute = nj.getBool("palmMute", false);
                                note.letRing = nj.getBool("letRing", false);
                                note.vibrato = nj.getBool("vibrato", false);
                                note.wideVibrato = nj.getBool("wideVibrato", false);
                                note.deadNote = nj.getBool("deadNote", false);
                                note.ghostNote = nj.getBool("ghostNote", false);
                                note.accented = nj.getBool("accented", false);
                                note.heavyAccented = nj.getBool("heavyAccented", false);
                                note.staccato = nj.getBool("staccato", false);
                                note.tap = nj.getBool("tap", false);
                                note.graceNote = nj.getBool("graceNote", false);
                                note.tremoloPicking = nj.getBool("tremoloPicking", false);
                                note.trill = nj.getBool("trill", false);
                                note.harmonicType = std::clamp(nj.getInt("harmonicType", 0), 0, 5);

                                const auto* bendsVal = nj.find("bendPoints");
                                if (bendsVal && bendsVal->type == JsonType::Array) {
                                    for (const auto& bp : bendsVal->arrVal) {
                                        if (bp.type != JsonType::Object) continue;
                                        BendPoint pt{};
                                        pt.position = std::clamp(bp.getInt("position", 0), 0, 60);
                                        pt.value = std::clamp(bp.getInt("value", 0), -200, 400);
                                        pt.vibrato = bp.getBool("vibrato", false);
                                        note.bendPoints.push_back(pt);
                                    }
                                }
                                beat.notes.push_back(std::move(note));
                            }
                        }
                        if (beat.notes.empty()) {
                            beat.isRest = true;
                        }
                        meas.beats.push_back(std::move(beat));
                    }
                }
                if (meas.beats.empty()) {
                    Beat r{};
                    r.duration = DurationValue::Whole;
                    r.isRest = true;
                    meas.beats.push_back(r);
                }
                tr.measures.push_back(std::move(meas));
            }
        }
        if (tr.measures.empty()) {
            Measure m{};
            Beat r{};
            r.duration = DurationValue::Whole;
            r.isRest = true;
            m.beats.push_back(r);
            tr.measures.push_back(std::move(m));
        }
        song.tracks.push_back(std::move(tr));
    }

    if (song.tracks.empty()) {
        if (outError) *outError = "Brak poprawnych ścieżek w JSON.";
        return false;
    }

    outSong = std::move(song);
    return true;
}

} // namespace gitara::score
