#include "gitara/theory/Interval.h"

namespace gitara::theory {

Note Interval::applyTo(Note root) const noexcept {
    int letterSteps = degree - 1;
    Letter targetLetter = advanceLetter(root.letter, letterSteps);
    int targetPc = (root.pitchClass() + semitones) % 12;
    if (targetPc < 0) targetPc += 12;
    return spellAtLetter(targetLetter, targetPc);
}

Pitch Interval::applyTo(Pitch root) const noexcept {
    int targetMidi = root.midiNumber() + semitones;
    Note spelledNote = applyTo(root.note);
    // Wyliczamy oktawę tak, aby spelledNote.midiNumber w tej oktawie równał się targetMidi
    int rawSemi = letterBaseSemitones(spelledNote.letter) + static_cast<int>(spelledNote.accidental);
    int oct = (targetMidi - rawSemi) / 12 - 1;
    return Pitch{spelledNote, oct};
}

std::string Interval::symbol(bool useUnicode) const {
    // Oczekiwana liczba półtonów w skali durowej dla stopnia (1..7)
    constexpr int majorSemitones[7] = {0, 2, 4, 5, 7, 9, 11};
    int simpleDegIndex = ((degree - 1) % 7 + 7) % 7;
    int octaves = (degree - 1) / 7;
    int expectedSemi = majorSemitones[simpleDegIndex] + octaves * 12;
    int diff = semitones - expectedSemi;

    std::string prefix;
    if (diff == -2) prefix = useUnicode ? "𝄫" : "bb";
    else if (diff == -1) prefix = useUnicode ? "♭" : "b";
    else if (diff == +1) prefix = useUnicode ? "♯" : "#";
    else if (diff == +2) prefix = useUnicode ? "𝄪" : "x";

    return prefix + std::to_string(degree);
}

std::string_view Interval::shortCode() const noexcept {
    switch (semitones) {
        case 0:  return "P1";
        case 1:  return "m2";
        case 2:  return "M2";
        case 3:  return degree == 2 ? "A2" : "m3";
        case 4:  return degree == 4 ? "d4" : "M3";
        case 5:  return "P4";
        case 6:  return degree == 4 ? "A4" : (degree == 5 ? "d5" : "TT");
        case 7:  return "P5";
        case 8:  return degree == 5 ? "A5" : "m6";
        case 9:  return degree == 7 ? "d7" : "M6";
        case 10: return "m7";
        case 11: return "M7";
        case 12: return "P8";
        case 13: return "m9";
        case 14: return "M9";
        case 15: return "A9";
        case 17: return "P11";
        case 18: return "A11";
        case 20: return "m13";
        case 21: return "M13";
        default: return "?";
    }
}

std::string_view Interval::polishName() const noexcept {
    switch (semitones) {
        case 0:  return "Pryma czysta";
        case 1:  return "Sekunda mała";
        case 2:  return "Sekunda wielka";
        case 3:  return degree == 2 ? "Sekunda zwiększona" : "Tercja mała";
        case 4:  return degree == 4 ? "Kwarta zmniejszona" : "Tercja wielka";
        case 5:  return "Kwarta czysta";
        case 6:  return degree == 4 ? "Kwarta zwiększona (tryton)" : "Kwinta zmniejszona (tryton)";
        case 7:  return "Kwinta czysta";
        case 8:  return degree == 5 ? "Kwinta zwiększona" : "Seksta mała";
        case 9:  return degree == 7 ? "Septyma zmniejszona" : "Seksta wielka";
        case 10: return "Septyma mała";
        case 11: return "Septyma wielka";
        case 12: return "Oktawa czysta";
        case 13: return "Nona mała";
        case 14: return "Nona wielka";
        case 15: return "Nona zwiększona";
        case 17: return "Undecyma czysta";
        case 18: return "Undecyma zwiększona";
        case 20: return "Tercdecyma mała";
        case 21: return "Tercdecyma wielka";
        default: return "Interwał";
    }
}

std::string_view Interval::songHintAscending() const noexcept {
    if (semitones > 0 && semitones % 12 == 0) {
        return "Somewhere Over the Rainbow (oktawa)";
    }
    switch (semitones % 12) {
        case 0:  return "Powtórzony ten sam dźwięk";
        case 1:  return "Szczęki (Jaws) – temat główny";
        case 2:  return "Sto lat / Happy Birthday";
        case 3:  return "Smoke on the Water / Greensleeves";
        case 4:  return "Gdy wszyscy święci idą do nieba (Oh When the Saints)";
        case 5:  return "Marsz weselny (Here Comes the Bride)";
        case 6:  return "The Simpsons – temat tytułowy";
        case 7:  return "Gwiezdne Wojny (Star Wars) – temat główny";
        case 8:  return "The Entertainer (skok po wstępie)";
        case 9:  return "My Bonnie Lies Over the Ocean";
        case 10: return "Star Trek – temat oryginalny / The Winner Takes It All";
        case 11: return "Take On Me (a-ha) – początek refrenu";
        default: return "Somewhere Over the Rainbow (oktawa)";
    }
}

const std::vector<Interval>& earTrainingIntervals() {
    static const std::vector<Interval> list = {
        intervals::P1,
        intervals::m2,
        intervals::M2,
        intervals::m3,
        intervals::M3,
        intervals::P4,
        intervals::A4, // Tryton
        intervals::P5,
        intervals::m6,
        intervals::M6,
        intervals::m7,
        intervals::M7,
        intervals::P8
    };
    return list;
}

Interval intervalFromSemitones(int semitones) noexcept {
    int s = semitones % 12;
    if (s < 0) s += 12;
    switch (s) {
        case 0:  return semitones == 12 ? intervals::P8 : intervals::P1;
        case 1:  return intervals::m2;
        case 2:  return intervals::M2;
        case 3:  return intervals::m3;
        case 4:  return intervals::M3;
        case 5:  return intervals::P4;
        case 6:  return intervals::A4;
        case 7:  return intervals::P5;
        case 8:  return intervals::m6;
        case 9:  return intervals::M6;
        case 10: return intervals::m7;
        case 11: return intervals::M7;
        default: return intervals::P1;
    }
}

} // namespace gitara::theory
