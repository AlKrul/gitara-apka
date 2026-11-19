#include "gitara/theory/Chord.h"

namespace gitara::theory {

std::vector<Note> Chord::notes(AccidentalMode mode) const {
    std::vector<Note> result;
    if (!definition) {
        return result;
    }
    result.reserve(definition->intervals.size());
    for (const auto& iv : definition->intervals) {
        Note spelled = iv.applyTo(root);
        result.push_back(spelled.withAccidentalMode(mode));
    }
    return result;
}

std::string Chord::symbol(bool useUnicode, AccidentalMode mode) const {
    Note displayedRoot = root.withAccidentalMode(mode);
    std::string out = displayedRoot.toString(useUnicode);
    if (definition) {
        out += definition->symbolSuffix;
    }
    if (bassNote.has_value()) {
        out += "/" + bassNote->withAccidentalMode(mode).toString(useUnicode);
    }
    return out;
}

bool Chord::containsPitchClass(int pitchClass) const noexcept {
    if (!definition) {
        return false;
    }
    int pc = pitchClass % 12;
    if (pc < 0) pc += 12;
    int rootPc = root.pitchClass();
    for (const auto& iv : definition->intervals) {
        if ((rootPc + iv.semitones) % 12 == pc) {
            return true;
        }
    }
    return false;
}

const std::vector<ChordDefinition>& allChordDefinitions() {
    using namespace intervals;
    static const std::vector<ChordDefinition> defs = {
        // Trójdźwięki (dokładnie 3 składniki) oraz dwudźwięk power chord
        {"major",  "maj",   "Durowy",              "Trójdźwięki", {P1, M3, P5}, {2}},
        {"minor",  "m",     "Molowy",              "Trójdźwięki", {P1, m3, P5}, {2}},
        {"dim",    "dim",   "Zmniejszony",         "Trójdźwięki", {P1, m3, d5}, {}},
        {"aug",    "aug",   "Zwiększony",          "Trójdźwięki", {P1, M3, A5}, {}},
        {"sus2",   "sus2",  "Zawieszony sus2",     "Trójdźwięki", {P1, M2, P5}, {}},
        {"sus4",   "sus4",  "Zawieszony sus4",     "Trójdźwięki", {P1, P4, P5}, {}},
        {"5",      "5",     "Power chord (kwinta)","Dwudźwięki",  {P1, P5},     {}},

        // Septymowe
        {"7",      "7",     "Dominantowy septymowy", "Septymowe", {P1, M3, P5, m7}, {2}},
        {"maj7",   "maj7",  "Durowy wielki septymowy","Septymowe", {P1, M3, P5, M7}, {2}},
        {"m7",     "m7",    "Molowy septymowy",      "Septymowe", {P1, m3, P5, m7}, {2}},
        {"mMaj7",  "m(maj7)","Molowy z wielką septymą","Septymowe",{P1, m3, P5, M7}, {2}},
        {"m7b5",   "m7♭5",  "Półzmniejszony",        "Septymowe", {P1, m3, d5, m7}, {}},
        {"dim7",   "dim7",  "Zmniejszony septymowy", "Septymowe", {P1, m3, d5, d7}, {}},
        {"7sus4",  "7sus4", "Dominantowy sus4",      "Septymowe", {P1, P4, P5, m7}, {2}},
        {"aug7",   "aug7",  "Zwiększony septymowy",  "Septymowe", {P1, M3, A5, m7}, {}},
        {"augMaj7","maj7♯5","Zwiększony z wielką septymą","Septymowe",{P1, M3, A5, M7},{}},

        // Rozszerzone (sekstowe, add, nonowe, undecymowe, tercdecymowe)
        {"6",      "6",     "Durowy sekstowy",       "Rozszerzone", {P1, M3, P5, M6}, {2}},
        {"m6",     "m6",    "Molowy sekstowy",       "Rozszerzone", {P1, m3, P5, M6}, {2}},
        {"add9",   "add9",  "Durowy z dodaną noną",  "Rozszerzone", {P1, M3, P5, M9}, {2}},
        {"madd9",  "m(add9)","Molowy z dodaną noną", "Rozszerzone", {P1, m3, P5, M9}, {2}},
        {"9",      "9",     "Dominantowy nonowy",    "Rozszerzone", {P1, M3, P5, m7, M9}, {2}},
        {"maj9",   "maj9",  "Durowy wielki nonowy",  "Rozszerzone", {P1, M3, P5, M7, M9}, {2}},
        {"m9",     "m9",    "Molowy nonowy",         "Rozszerzone", {P1, m3, P5, m7, M9}, {2}},
        {"11",     "11",    "Dominantowy undecymowy","Rozszerzone", {P1, M3, P5, m7, M9, P11}, {1, 2, 4}},
        {"m11",    "m11",   "Molowy undecymowy",     "Rozszerzone", {P1, m3, P5, m7, M9, P11}, {2, 4}},
        {"maj7#11","maj7♯11","Lidyjski septymowy",   "Rozszerzone", {P1, M3, P5, M7, A11}, {2}},
        {"13",     "13",    "Dominantowy tercdecymowy","Rozszerzone",{P1, M3, P5, m7, M9, M13}, {2, 4}},
        {"maj13",  "maj13", "Durowy wielki tercdecymowy","Rozszerzone",{P1, M3, P5, M7, M9, M13}, {2, 4}},
        {"m13",    "m13",   "Molowy tercdecymowy",   "Rozszerzone", {P1, m3, P5, m7, M9, M13}, {2, 4}},

        // Alterowane
        {"7b5",    "7♭5",   "Dominantowy ♭5",        "Alterowane", {P1, M3, d5, m7}, {}},
        {"7b9",    "7♭9",   "Dominantowy ♭9",        "Alterowane", {P1, M3, P5, m7, m9}, {2}},
        {"7#9",    "7♯9",   "Dominantowy ♯9 (Hendrix)","Alterowane",{P1, M3, P5, m7, A9}, {2}},
        {"7#11",   "7♯11",  "Dominantowy ♯11",       "Alterowane", {P1, M3, P5, m7, A11}, {2}},
        {"7b13",   "7♭13",  "Dominantowy ♭13",       "Alterowane", {P1, M3, P5, m7, m13}, {2}},
        {"7alt",   "7alt",  "Dominantowy alterowany","Alterowane", {P1, M3, d5, m7, m9, m13}, {2, 4}}
    };
    return defs;
}

const ChordDefinition* findChordDefinition(std::string_view id) noexcept {
    for (const auto& def : allChordDefinitions()) {
        if (def.id == id || def.symbolSuffix == id) {
            return &def;
        }
    }
    return nullptr;
}

bool chordMatchesGroup(const ChordDefinition& def, std::string_view group) noexcept {
    if (group.empty() || group == "all") return true;
    if (group == "basic") {
        return def.id == "major" || def.id == "minor" || def.id == "7" ||
               def.id == "maj7"  || def.id == "m7"    || def.id == "sus2" ||
               def.id == "sus4"  || def.id == "5"     || def.id == "dim" ||
               def.id == "add9";
    }
    if (group == "triad")    return def.category == "Trójdźwięki";
    if (group == "seventh")  return def.category == "Septymowe";
    if (group == "extended") return def.category == "Rozszerzone";
    if (group == "altered")  return def.category == "Alterowane";
    return true;
}

std::vector<ChordScaleMatch> findScalesForChord(const Chord& chord, AccidentalMode mode) {
    std::vector<ChordScaleMatch> matches;
    if (!chord.definition) {
        return matches;
    }

    const auto chordNotes = chord.notes(mode);

    for (const auto& scaleDef : allScaleDefinitions()) {
        Scale candidate{chord.root, &scaleDef};

        bool allChordTonesPresent = true;
        for (const auto& cn : chordNotes) {
            if (!candidate.containsPitchClass(cn.pitchClass())) {
                allChordTonesPresent = false;
                break;
            }
        }
        if (!allChordTonesPresent) {
            continue;
        }

        ChordScaleMatch match{};
        match.scale = candidate;
        const auto scaleNotes = candidate.notes(mode);

        for (const auto& sn : scaleNotes) {
            if (chord.containsPitchClass(sn.pitchClass())) {
                match.chordTones.push_back(sn);
            } else {
                // Sprawdź, czy dźwięk spoza akordu leży o 1 półton powyżej składnika akordu (avoid note)
                bool isAvoid = false;
                for (const auto& cn : chordNotes) {
                    int diff = (sn.pitchClass() - cn.pitchClass() + 12) % 12;
                    if (diff == 1) {
                        isAvoid = true;
                        break;
                    }
                }
                if (isAvoid) {
                    match.avoidNotes.push_back(sn);
                } else {
                    match.tensions.push_back(sn);
                }
            }
        }

        matches.push_back(std::move(match));
    }

    return matches;
}

std::vector<Chord> findChordsForScale(const Scale& scale, bool allChordTypes, AccidentalMode mode) {
    std::vector<Chord> result;
    if (!scale.definition) {
        return result;
    }

    const auto scaleNotes = scale.notes(mode);
    for (const auto& degreeNote : scaleNotes) {
        for (const auto& chordDef : allChordDefinitions()) {
            if (!allChordTypes && chordDef.category != "Trójdźwięki") {
                continue;
            }

            Chord candidate{degreeNote, &chordDef, std::nullopt};
            bool fits = true;
            for (const auto& cn : candidate.notes(mode)) {
                if (!scale.containsPitchClass(cn.pitchClass())) {
                    fits = false;
                    break;
                }
            }
            if (fits) {
                result.push_back(candidate);
            }
        }
    }

    return result;
}

} // namespace gitara::theory
