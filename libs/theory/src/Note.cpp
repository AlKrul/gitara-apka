#include "gitara/theory/Note.h"

namespace gitara::theory {

Note spellAtLetter(Letter targetLetter, int targetPitchClass) noexcept {
    int pc = targetPitchClass % 12;
    if (pc < 0) pc += 12;
    int base = letterBaseSemitones(targetLetter);
    int diff = pc - base;
    if (diff > 6) diff -= 12;
    if (diff < -6) diff += 12;
    if (diff < -2) diff = -2;
    if (diff > 2) diff = 2;
    return Note{targetLetter, static_cast<Accidental>(diff)};
}

Note Note::fromPitchClass(int pitchClass, AccidentalMode mode) noexcept {
    int pc = pitchClass % 12;
    if (pc < 0) pc += 12;

    if (mode == AccidentalMode::Sharps) {
        constexpr Note sharpTable[12] = {
            {Letter::C, Accidental::Natural},
            {Letter::C, Accidental::Sharp},
            {Letter::D, Accidental::Natural},
            {Letter::D, Accidental::Sharp},
            {Letter::E, Accidental::Natural},
            {Letter::F, Accidental::Natural},
            {Letter::F, Accidental::Sharp},
            {Letter::G, Accidental::Natural},
            {Letter::G, Accidental::Sharp},
            {Letter::A, Accidental::Natural},
            {Letter::A, Accidental::Sharp},
            {Letter::B, Accidental::Natural}
        };
        return sharpTable[pc];
    }

    if (mode == AccidentalMode::Flats) {
        constexpr Note flatTable[12] = {
            {Letter::C, Accidental::Natural},
            {Letter::D, Accidental::Flat},
            {Letter::D, Accidental::Natural},
            {Letter::E, Accidental::Flat},
            {Letter::E, Accidental::Natural},
            {Letter::F, Accidental::Natural},
            {Letter::G, Accidental::Flat},
            {Letter::G, Accidental::Natural},
            {Letter::A, Accidental::Flat},
            {Letter::A, Accidental::Natural},
            {Letter::B, Accidental::Flat},
            {Letter::B, Accidental::Natural}
        };
        return flatTable[pc];
    }

    // AccidentalMode::Auto — standardowa pisownia koła kwintowego (Db, Eb, F#, Ab, Bb)
    constexpr Note autoTable[12] = {
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
    return autoTable[pc];
}

Note Note::withAccidentalMode(AccidentalMode mode) const noexcept {
    if (mode == AccidentalMode::Auto) {
        return *this;
    }
    return fromPitchClass(pitchClass(), mode);
}

std::string Note::toString(bool useUnicode) const {
    constexpr char letterChars[7] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
    std::string out(1, letterChars[static_cast<int>(letter)]);

    switch (accidental) {
        case Accidental::DoubleFlat:
            out += useUnicode ? "𝄫" : "bb";
            break;
        case Accidental::Flat:
            out += useUnicode ? "♭" : "b";
            break;
        case Accidental::Natural:
            break;
        case Accidental::Sharp:
            out += useUnicode ? "♯" : "#";
            break;
        case Accidental::DoubleSharp:
            out += useUnicode ? "𝄪" : "x";
            break;
    }
    return out;
}

std::optional<Note> Note::parse(std::string_view text) noexcept {
    if (text.empty()) {
        return std::nullopt;
    }

    Letter l{};
    switch (text[0]) {
        case 'C': case 'c': l = Letter::C; break;
        case 'D': case 'd': l = Letter::D; break;
        case 'E': case 'e': l = Letter::E; break;
        case 'F': case 'f': l = Letter::F; break;
        case 'G': case 'g': l = Letter::G; break;
        case 'A': case 'a': l = Letter::A; break;
        case 'B': case 'b': l = Letter::B; break;
        default:
            // Symbol 'H' / 'h' jest celowo odrzucany w systemie A–G
            return std::nullopt;
    }

    std::string_view rest = text.substr(1);
    if (rest.empty() || rest == "♮") {
        return Note{l, Accidental::Natural};
    }
    if (rest == "#" || rest == "♯") {
        return Note{l, Accidental::Sharp};
    }
    if (rest == "b" || rest == "♭") {
        return Note{l, Accidental::Flat};
    }
    if (rest == "x" || rest == "##" || rest == "♯♯" || rest == "𝄪") {
        return Note{l, Accidental::DoubleSharp};
    }
    if (rest == "bb" || rest == "♭♭" || rest == "𝄫") {
        return Note{l, Accidental::DoubleFlat};
    }

    return std::nullopt;
}

} // namespace gitara::theory
