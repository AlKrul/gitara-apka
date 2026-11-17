#include "gitara/score/Score.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <sstream>

namespace gitara::score {

double Beat::quarterNoteLength() const noexcept {
    double base = std::pow(2.0, -static_cast<int>(duration));
    if (dotted) {
        base *= 1.5;
    }
    if (tupletNumerator > 0 && tupletDenominator > 0) {
        base *= static_cast<double>(tupletDenominator) / static_cast<double>(tupletNumerator);
    }
    return base;
}

TabRecorder::TabRecorder(gitara::theory::Fretboard fretboard,
                         int tempoBpm,
                         int timeSigNumerator,
                         int timeSigDenominator)
    : fretboard_(std::move(fretboard)),
      tempoBpm_(std::max(20, tempoBpm)),
      numerator_(std::max(1, timeSigNumerator)),
      denominator_(std::max(1, timeSigDenominator)) {}

Song TabRecorder::buildSongFromEvents(
    const std::vector<DetectedNoteEvent>& events,
    const gitara::theory::FretboardMask* mask,
    std::string title) const {
    Song song{};
    song.title = std::move(title);
    song.tempoBpm = tempoBpm_;

    Track track{};
    track.name = "Gitara";
    track.tuning = fretboard_.tuning();

    if (!enabled_ || events.empty()) {
        Measure emptyM{};
        emptyM.numerator = numerator_;
        emptyM.denominator = denominator_;
        track.measures.push_back(std::move(emptyM));
        song.tracks.push_back(std::move(track));
        return song;
    }

    const double secondsPerQuarter = 60.0 / static_cast<double>(tempoBpm_);
    const double quartersPerMeasure =
        static_cast<double>(numerator_) * (4.0 / static_cast<double>(denominator_));

    auto quantizeDuration = [&](double seconds) -> DurationValue {
        double quarters = std::max(0.125, seconds / secondsPerQuarter);
        if (quarters >= 3.0)   return DurationValue::Whole;
        if (quarters >= 1.5)   return DurationValue::Half;
        if (quarters >= 0.75)  return DurationValue::Quarter;
        if (quarters >= 0.375) return DurationValue::Eighth;
        if (quarters >= 0.18)  return DurationValue::Sixteenth;
        return DurationValue::ThirtySecond;
    };

    Measure currentMeasure{};
    currentMeasure.numerator = numerator_;
    currentMeasure.denominator = denominator_;
    double currentMeasureQuarters = 0.0;
    double previousEndSeconds = events.front().startTimeSeconds;
    std::optional<gitara::theory::FretPosition> lastHandPos{};

    auto pushBeatToMeasures = [&](Beat b) {
        double len = b.quarterNoteLength();
        if (currentMeasureQuarters + len > quartersPerMeasure + 1e-4 &&
            !currentMeasure.beats.empty()) {
            track.measures.push_back(std::move(currentMeasure));
            currentMeasure = Measure{};
            currentMeasure.numerator = numerator_;
            currentMeasure.denominator = denominator_;
            currentMeasureQuarters = 0.0;
        }
        currentMeasureQuarters += len;
        currentMeasure.beats.push_back(std::move(b));
    };

    for (std::size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];

        // Jeśli między poprzednią nutą a obecną wystąpiła wyraźna przerwa (>= pół ćwierćnuty), wstaw pauzę
        double gapSeconds = ev.startTimeSeconds - previousEndSeconds;
        if (i > 0 && gapSeconds >= secondsPerQuarter * 0.40) {
            Beat restBeat{};
            restBeat.isRest = true;
            restBeat.duration = quantizeDuration(gapSeconds);
            pushBeatToMeasures( std::move(restBeat));
        }

        // Efektywny czas do następnego uderzenia lub deklarowany czas wybrzmiewania
        double effectiveSec = ev.durationSeconds;
        if (i + 1 < events.size()) {
            double interOnset = events[i + 1].startTimeSeconds - ev.startTimeSeconds;
            if (interOnset > 0.05 && interOnset < effectiveSec * 1.4) {
                effectiveSec = interOnset;
            }
        }

        Beat noteBeat{};
        noteBeat.isRest = ev.midiNotes.empty();
        noteBeat.duration = quantizeDuration(effectiveSec);

        std::array<bool, 12> usedStrings{};
        for (int midi : ev.midiNotes) {
            auto candidates = fretboard_.findPositionsForMidi(midi, mask);
            // Wyklucz struny już zajęte w tym samym uderzeniu (dla akordów)
            candidates.erase(
                std::remove_if(candidates.begin(), candidates.end(),
                               [&](const gitara::theory::FretPosition& p) {
                                   return p.stringIndex >= 0 && p.stringIndex < 12 &&
                                          usedStrings[static_cast<std::size_t>(p.stringIndex)];
                               }),
                candidates.end());

            if (candidates.empty()) {
                continue;
            }

            gitara::theory::FretPosition chosen = candidates.front();
            if (lastHandPos.has_value()) {
                auto prev = *lastHandPos;
                chosen = *std::min_element(candidates.begin(), candidates.end(),
                    [&prev](const gitara::theory::FretPosition& a,
                            const gitara::theory::FretPosition& b) {
                        double da = std::abs(a.fret - prev.fret) +
                                    std::abs(a.stringIndex - prev.stringIndex) * 1.2;
                        double db = std::abs(b.fret - prev.fret) +
                                    std::abs(b.stringIndex - prev.stringIndex) * 1.2;
                        return da < db;
                    });
            }

            usedStrings[static_cast<std::size_t>(chosen.stringIndex)] = true;
            lastHandPos = chosen;

            TabNote tn{};
            tn.stringIndex = chosen.stringIndex;
            tn.fret = chosen.fret;
            tn.velocity = 95;
            noteBeat.notes.push_back(tn);
        }

        pushBeatToMeasures(std::move(noteBeat));
        previousEndSeconds = ev.startTimeSeconds + effectiveSec;
    }

    if (!currentMeasure.beats.empty() || track.measures.empty()) {
        track.measures.push_back(std::move(currentMeasure));
    }

    song.tracks.push_back(std::move(track));
    return song;
}

gitara::theory::Scale detectKeyFromSong(const Song& song, gitara::theory::AccidentalMode mode) {
    // Profile Krumhansla-Schmucklera dla tonacji durowych i molowych
    constexpr std::array<double, 12> kMajorProfile = {
        6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88
    };
    constexpr std::array<double, 12> kMinorProfile = {
        6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17
    };

    std::array<double, 12> histogram{};
    for (const auto& track : song.tracks) {
        gitara::theory::Fretboard fb{track.tuning};
        for (const auto& m : track.measures) {
            for (const auto& b : m.beats) {
                double w = b.quarterNoteLength();
                for (const auto& n : b.notes) {
                    int midi = fb.midiAt(n.stringIndex, n.fret);
                    if (midi >= 0) {
                        histogram[static_cast<std::size_t>(midi % 12)] += w;
                    }
                }
            }
        }
    }

    int bestRootPc = 0;
    bool bestIsMinor = false;
    double bestScore = -1e9;

    for (int rootPc = 0; rootPc < 12; ++rootPc) {
        double majScore = 0.0;
        double minScore = 0.0;
        for (int i = 0; i < 12; ++i) {
            double h = histogram[static_cast<std::size_t>((rootPc + i) % 12)];
            majScore += h * kMajorProfile[static_cast<std::size_t>(i)];
            minScore += h * kMinorProfile[static_cast<std::size_t>(i)];
        }
        if (majScore > bestScore) {
            bestScore = majScore;
            bestRootPc = rootPc;
            bestIsMinor = false;
        }
        if (minScore > bestScore) {
            bestScore = minScore;
            bestRootPc = rootPc;
            bestIsMinor = true;
        }
    }

    const auto* def = gitara::theory::findScaleDefinition(bestIsMinor ? "natural_minor" : "major");
    return gitara::theory::Scale{gitara::theory::Note::fromPitchClass(bestRootPc, mode), def};
}

namespace {

void writeVariableLength(std::vector<std::uint8_t>& buf, std::uint32_t value) {
    std::uint32_t buffer = value & 0x7Fu;
    while ((value >>= 7) > 0) {
        buffer <<= 8;
        buffer |= ((value & 0x7Fu) | 0x80u);
    }
    while (true) {
        buf.push_back(static_cast<std::uint8_t>(buffer & 0xFFu));
        if (buffer & 0x80u) {
            buffer >>= 8;
        } else {
            break;
        }
    }
}

void writeBE16(std::vector<std::uint8_t>& buf, std::uint16_t v) {
    buf.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>(v & 0xFFu));
}

void writeBE32(std::vector<std::uint8_t>& buf, std::uint32_t v) {
    buf.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>(v & 0xFFu));
}

void writeLE32(std::vector<std::uint8_t>& buf, std::int32_t v) {
    auto u = static_cast<std::uint32_t>(v);
    buf.push_back(static_cast<std::uint8_t>(u & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>((u >> 8) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>((u >> 16) & 0xFFu));
    buf.push_back(static_cast<std::uint8_t>((u >> 24) & 0xFFu));
}

void writeGpIntByteSizeString(std::vector<std::uint8_t>& buf, const std::string& s) {
    writeLE32(buf, static_cast<std::int32_t>(s.size() + 1));
    buf.push_back(static_cast<std::uint8_t>(s.size()));
    buf.insert(buf.end(), s.begin(), s.end());
}

void writeGpFixedString(std::vector<std::uint8_t>& buf, const std::string& s, std::size_t maxLen) {
    std::size_t len = std::min(s.size(), maxLen);
    buf.push_back(static_cast<std::uint8_t>(len));
    buf.insert(buf.end(), s.begin(), s.begin() + static_cast<std::ptrdiff_t>(len));
    for (std::size_t i = len; i < maxLen; ++i) {
        buf.push_back(0);
    }
}

} // namespace

std::vector<std::uint8_t> exportToMidiBytes(const Song& song) {
    constexpr std::uint16_t kTicksPerQuarter = 480;
    std::vector<std::uint8_t> out;

    // Nagłówek MThd (Format 1, 1 ścieżka na każdy Track, lub 1 pusta jeśli brak)
    const std::uint16_t numTracks = static_cast<std::uint16_t>(std::max<std::size_t>(1, song.tracks.size()));
    out.insert(out.end(), {'M', 'T', 'h', 'd'});
    writeBE32(out, 6);
    writeBE16(out, 1);
    writeBE16(out, numTracks);
    writeBE16(out, kTicksPerQuarter);

    for (std::size_t tIdx = 0; tIdx < numTracks; ++tIdx) {
        std::vector<std::uint8_t> trk;

        // Meta: Tempo (FF 51 03 tt tt tt)
        std::uint32_t usPerQuarter = static_cast<std::uint32_t>(60000000 / std::max(20, song.tempoBpm));
        writeVariableLength(trk, 0);
        trk.push_back(0xFF);
        trk.push_back(0x51);
        trk.push_back(0x03);
        trk.push_back(static_cast<std::uint8_t>((usPerQuarter >> 16) & 0xFFu));
        trk.push_back(static_cast<std::uint8_t>((usPerQuarter >> 8) & 0xFFu));
        trk.push_back(static_cast<std::uint8_t>(usPerQuarter & 0xFFu));

        if (tIdx < song.tracks.size()) {
            const auto& track = song.tracks[tIdx];
            gitara::theory::Fretboard fb{track.tuning};
            std::uint8_t ch = static_cast<std::uint8_t>(track.midiChannel & 0x0F);

            // Program Change
            writeVariableLength(trk, 0);
            trk.push_back(static_cast<std::uint8_t>(0xC0u | ch));
            trk.push_back(static_cast<std::uint8_t>(track.midiProgram & 0x7Fu));

            std::uint32_t restAccumulatorTicks = 0;
            for (const auto& m : track.measures) {
                for (const auto& b : m.beats) {
                    std::uint32_t beatTicks = static_cast<std::uint32_t>(
                        std::llround(b.quarterNoteLength() * static_cast<double>(kTicksPerQuarter)));
                    if (b.isRest || b.notes.empty()) {
                        restAccumulatorTicks += beatTicks;
                        continue;
                    }

                    // Note On dla wszystkich nut uderzenia
                    for (std::size_t nIdx = 0; nIdx < b.notes.size(); ++nIdx) {
                        const auto& n = b.notes[nIdx];
                        int midi = std::clamp(fb.midiAt(n.stringIndex, n.fret), 0, 127);
                        writeVariableLength(trk, (nIdx == 0) ? restAccumulatorTicks : 0u);
                        restAccumulatorTicks = 0;
                        trk.push_back(static_cast<std::uint8_t>(0x90u | ch));
                        trk.push_back(static_cast<std::uint8_t>(midi));
                        trk.push_back(static_cast<std::uint8_t>(std::clamp(n.velocity, 1, 127)));
                    }

                    // Note Off po czasie trwania uderzenia
                    for (std::size_t nIdx = 0; nIdx < b.notes.size(); ++nIdx) {
                        const auto& n = b.notes[nIdx];
                        int midi = std::clamp(fb.midiAt(n.stringIndex, n.fret), 0, 127);
                        writeVariableLength(trk, (nIdx == 0) ? beatTicks : 0u);
                        trk.push_back(static_cast<std::uint8_t>(0x80u | ch));
                        trk.push_back(static_cast<std::uint8_t>(midi));
                        trk.push_back(0);
                    }
                }
            }
        }

        // End of Track (FF 2F 00)
        writeVariableLength(trk, 0);
        trk.push_back(0xFF);
        trk.push_back(0x2F);
        trk.push_back(0x00);

        out.insert(out.end(), {'M', 'T', 'r', 'k'});
        writeBE32(out, static_cast<std::uint32_t>(trk.size()));
        out.insert(out.end(), trk.begin(), trk.end());
    }

    return out;
}

std::vector<std::uint8_t> exportToGp5Bytes(const Song& song) {
    std::vector<std::uint8_t> buf;
    buf.reserve(4096);

    // 1. Nagłówek wersji GP5.00 (30-znakowy string poprzedzony długością 24)
    writeGpFixedString(buf, "FICHIER GUITAR PRO v5.00", 30);

    // 2. Informacje o utworze (Title, Subtitle, Artist, Album, Words, Music, Copyright, Tab, Instructions)
    writeGpIntByteSizeString(buf, song.title);
    writeGpIntByteSizeString(buf, "");
    writeGpIntByteSizeString(buf, song.artist);
    writeGpIntByteSizeString(buf, "");
    writeGpIntByteSizeString(buf, "");
    writeGpIntByteSizeString(buf, song.artist);
    writeGpIntByteSizeString(buf, "");
    writeGpIntByteSizeString(buf, "Apka Gitarowa");
    writeGpIntByteSizeString(buf, "");
    writeLE32(buf, 0); // Notice lines = 0

    // 3. Lyrics (1 track + 5 linii pustych)
    writeLE32(buf, 0);
    for (int i = 0; i < 5; ++i) {
        writeLE32(buf, 0);
        writeLE32(buf, 0);
    }

    // 4. Page setup & Tempo
    writeLE32(buf, 210); // Page width
    writeLE32(buf, 297); // Page height
    writeLE32(buf, 10);  // Margin left
    writeLE32(buf, 10);  // Margin right
    writeLE32(buf, 15);  // Margin top
    writeLE32(buf, 10);  // Margin bottom
    writeLE32(buf, 100); // Score size %
    buf.push_back(0xFF); // Header/footer flags
    buf.push_back(0x01);
    for (int i = 0; i < 10; ++i) {
        writeGpIntByteSizeString(buf, "");
    }

    writeGpIntByteSizeString(buf, ""); // Tempo name
    writeLE32(buf, song.tempoBpm);
    buf.push_back(0); // Key signature
    writeLE32(buf, 0); // Octave

    // 5. 64 kanały MIDI (po 12 bajtów każdy: instrument, vol, bal, chorus, reverb, phaser, tremolo, 2x blank)
    for (int ch = 0; ch < 64; ++ch) {
        writeLE32(buf, (ch == 0 && !song.tracks.empty()) ? song.tracks.front().midiProgram : 24);
        buf.push_back(13); // Volume
        buf.push_back(8);  // Balance
        buf.push_back(0);  // Chorus
        buf.push_back(0);  // Reverb
        buf.push_back(0);  // Phaser
        buf.push_back(0);  // Tremolo
        buf.push_back(0);
        buf.push_back(0);
    }

    // Directions (19 x short = 38 bajtów 0xFF) + master reverb (4 bajty)
    for (int i = 0; i < 38; ++i) buf.push_back(0xFF);
    writeLE32(buf, 0);

    const std::size_t numTracks = std::max<std::size_t>(1, song.tracks.size());
    const std::size_t numMeasures =
        (!song.tracks.empty() && !song.tracks.front().measures.empty())
            ? song.tracks.front().measures.size()
            : 1;

    writeLE32(buf, static_cast<std::int32_t>(numMeasures));
    writeLE32(buf, static_cast<std::int32_t>(numTracks));

    // 6. Nagłówki taktów (Measure Headers)
    for (std::size_t mIdx = 0; mIdx < numMeasures; ++mIdx) {
        if (mIdx > 0) buf.push_back(0); // Blank byte przed kolejnymi nagłówkami taktów w GP5
        int num = 4;
        int den = 4;
        if (!song.tracks.empty() && mIdx < song.tracks.front().measures.size()) {
            num = song.tracks.front().measures[mIdx].numerator;
            den = song.tracks.front().measures[mIdx].denominator;
        }
        std::uint8_t flags = (mIdx == 0) ? 0x03u : 0x00u; // Licznik + mianownik w 1. takcie
        buf.push_back(flags);
        if (flags & 0x01u) buf.push_back(static_cast<std::uint8_t>(num));
        if (flags & 0x02u) buf.push_back(static_cast<std::uint8_t>(den));
        // Beams (4 bajty) + placeholder + tripletFeel
        buf.insert(buf.end(), {2, 2, 2, 2, 0, 0});
    }

    // 7. Nagłówki ścieżek (Tracks) z dokładnym strojem strun (od najwyższej E4 do najniższej E2 wg GP5)
    for (std::size_t tIdx = 0; tIdx < numTracks; ++tIdx) {
        buf.push_back(0); // Flags1
        std::string tName = (tIdx < song.tracks.size()) ? song.tracks[tIdx].name : "Gitara";
        writeGpFixedString(buf, tName, 40);

        gitara::theory::Tuning tuning = (tIdx < song.tracks.size())
            ? song.tracks[tIdx].tuning
            : gitara::theory::findTuning("standard");
        writeLE32(buf, tuning.stringCount());
        // W GP5 struny są zapisywane od 1. (najcieńszej, np. E4 = 64) do 7.
        for (int i = 0; i < 7; ++i) {
            int revIdx = tuning.stringCount() - 1 - i;
            int midi = (revIdx >= 0 && revIdx < tuning.stringCount())
                ? tuning.openStrings[static_cast<std::size_t>(revIdx)].midiNumber()
                : 0;
            writeLE32(buf, midi);
        }
        writeLE32(buf, 1);  // Port
        writeLE32(buf, 1);  // Channel
        writeLE32(buf, 2);  // Effects channel
        writeLE32(buf, 24); // Frets
        writeLE32(buf, 0);  // Capo
        writeLE32(buf, 0x00FF8000); // Color
        for (int i = 0; i < 49; ++i) buf.push_back(0); // RSE padding w GP5.00
    }

    // 8. Zawartość taktów (Measures -> Voices -> Beats -> Notes ze struną i progiem!)
    for (std::size_t mIdx = 0; mIdx < numMeasures; ++mIdx) {
        for (std::size_t tIdx = 0; tIdx < numTracks; ++tIdx) {
            const Measure* measurePtr = nullptr;
            int stringCount = 6;
            if (tIdx < song.tracks.size()) {
                stringCount = song.tracks[tIdx].tuning.stringCount();
                if (mIdx < song.tracks[tIdx].measures.size()) {
                    measurePtr = &song.tracks[tIdx].measures[mIdx];
                }
            }

            // Głos 1 (aktywne uderzenia)
            const std::size_t beatCount = (measurePtr && !measurePtr->beats.empty())
                ? measurePtr->beats.size()
                : 1;
            writeLE32(buf, static_cast<std::int32_t>(beatCount));

            for (std::size_t bIdx = 0; bIdx < beatCount; ++bIdx) {
                Beat b{};
                b.isRest = true;
                if (measurePtr && bIdx < measurePtr->beats.size()) {
                    b = measurePtr->beats[bIdx];
                }

                std::uint8_t beatFlags = 0;
                if (b.dotted) beatFlags |= 0x01u;
                if (b.tupletNumerator > 0) beatFlags |= 0x20u;
                if (b.isRest || b.notes.empty()) beatFlags |= 0x40u;
                buf.push_back(beatFlags);

                if (beatFlags & 0x40u) {
                    buf.push_back(0x02u); // Status: Rest
                }
                buf.push_back(static_cast<std::uint8_t>(static_cast<std::int8_t>(b.duration)));
                if (beatFlags & 0x20u) {
                    writeLE32(buf, b.tupletNumerator);
                }

                // Maska bitowa użytych strun (bit 6 = najwyższa struna E4 .. bit 1 = najniższa struna E2)
                std::uint8_t stringBits = 0;
                if (!b.isRest) {
                    for (const auto& n : b.notes) {
                        if (n.stringIndex >= 0 && n.stringIndex < stringCount) {
                            int bitPos = 1 + n.stringIndex; // 0 (E2) -> bit 1, 5 (E4) -> bit 6
                            stringBits |= static_cast<std::uint8_t>(1u << bitPos);
                        }
                    }
                }
                buf.push_back(stringBits);

                // Zapisz nuty od najwyższej struny (bit 6) do najniższej (bit 1)
                for (int s = stringCount - 1; s >= 0; --s) {
                    int bitPos = 1 + s;
                    if (!(stringBits & (1u << bitPos))) {
                        continue;
                    }
                    int fret = 0;
                    bool tied = false;
                    for (const auto& n : b.notes) {
                        if (n.stringIndex == s) {
                            fret = std::clamp(n.fret, 0, 24);
                            tied = n.tied;
                            break;
                        }
                    }
                    buf.push_back(0x20u | 0x10u); // Note flags: ma typ nuty + dynamikę
                    buf.push_back(tied ? 0x02u : 0x01u); // 1 = normal, 2 = tied
                    buf.push_back(6); // Dynamic: f (forte)
                    buf.push_back(static_cast<std::uint8_t>(fret));
                    buf.push_back(0); // Note flags2
                }

                // 2 bajty flag transpozycji/łamania w GP5
                buf.push_back(0);
                buf.push_back(0);
            }

            // Głos 2 (pusty: 0 uderzeń) + LineBreak byte
            writeLE32(buf, 0);
            buf.push_back(0);
        }
    }

    return buf;
}

std::string exportToAsciiTab(const Song& song) {
    if (song.tracks.empty()) {
        return {};
    }
    const auto& track = song.tracks.front();
    const int nStrings = track.tuning.stringCount();
    std::vector<std::string> lines(static_cast<std::size_t>(nStrings));

    for (int s = 0; s < nStrings; ++s) {
        int displayRow = nStrings - 1 - s; // Góra = najcieńsza struna E4, dół = najgrubsza E2
        std::string label = track.tuning.openStrings[static_cast<std::size_t>(s)].note.toString(false);
        if (label.size() < 2) label += " ";
        lines[static_cast<std::size_t>(displayRow)] = label + "|";
    }

    for (const auto& m : track.measures) {
        for (const auto& b : m.beats) {
            for (int s = 0; s < nStrings; ++s) {
                int displayRow = nStrings - 1 - s;
                int fret = -1;
                if (!b.isRest) {
                    for (const auto& n : b.notes) {
                        if (n.stringIndex == s) {
                            fret = n.fret;
                            break;
                        }
                    }
                }
                if (fret < 0) {
                    lines[static_cast<std::size_t>(displayRow)] += "---";
                } else if (fret < 10) {
                    lines[static_cast<std::size_t>(displayRow)] += "-" + std::to_string(fret) + "-";
                } else {
                    lines[static_cast<std::size_t>(displayRow)] += std::to_string(fret) + "-";
                }
            }
        }
        for (auto& line : lines) {
            line += "|";
        }
    }

    std::ostringstream oss;
    for (const auto& line : lines) {
        oss << line << "\n";
    }
    return oss.str();
}

} // namespace gitara::score
