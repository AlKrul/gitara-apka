#include "gitara/theory/Scale.h"

namespace gitara::theory {

std::vector<Note> Scale::notes(AccidentalMode mode) const {
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

bool Scale::containsPitchClass(int pitchClass) const noexcept {
    return intervalForPitchClass(pitchClass).has_value();
}

std::optional<Interval> Scale::intervalForPitchClass(int pitchClass) const noexcept {
    if (!definition) {
        return std::nullopt;
    }
    int pc = pitchClass % 12;
    if (pc < 0) pc += 12;
    int rootPc = root.pitchClass();

    for (const auto& iv : definition->intervals) {
        int notePc = (rootPc + iv.semitones) % 12;
        if (notePc == pc) {
            return iv;
        }
    }
    return std::nullopt;
}

std::string Scale::fullName(bool useUnicode) const {
    if (!definition) {
        return root.toString(useUnicode);
    }
    return root.toString(useUnicode) + " " + definition->polishName;
}

const std::vector<ScaleDefinition>& allScaleDefinitions() {
    using namespace intervals;
    static const std::vector<ScaleDefinition> defs = {
        // Podstawowe
        {"major", "Durowa", "Podstawowe",
         {P1, M2, M3, P4, P5, M6, M7}, "tonal / Wesołowski"},
        {"natural_minor", "Molowa naturalna (Eolska)", "Podstawowe",
         {P1, M2, m3, P4, P5, m6, m7}, "tonal / Wesołowski"},
        {"harmonic_minor", "Molowa harmoniczna", "Podstawowe",
         {P1, M2, m3, P4, P5, m6, M7}, "tonal / Wesołowski"},
        {"melodic_minor", "Molowa melodyczna (jazzowa)", "Podstawowe",
         {P1, M2, m3, P4, P5, M6, M7}, "tonal / Levine"},

        // Modalne
        {"ionian", "Jońska", "Modalne",
         {P1, M2, M3, P4, P5, M6, M7}, "tonal / Levine"},
        {"dorian", "Dorycka", "Modalne",
         {P1, M2, m3, P4, P5, M6, m7}, "tonal / Levine"},
        {"phrygian", "Frygijska", "Modalne",
         {P1, m2, m3, P4, P5, m6, m7}, "tonal / Levine"},
        {"lydian", "Lidyjska", "Modalne",
         {P1, M2, M3, A4, P5, M6, M7}, "tonal / Levine"},
        {"mixolydian", "Miksolidyjska", "Modalne",
         {P1, M2, M3, P4, P5, M6, m7}, "tonal / Levine"},
        {"aeolian", "Eolska", "Modalne",
         {P1, M2, m3, P4, P5, m6, m7}, "tonal / Levine"},
        {"locrian", "Lokrycka", "Modalne",
         {P1, m2, m3, P4, d5, m6, m7}, "tonal / Levine"},

        // Pentatoniki i Blues
        {"major_pentatonic", "Pentatonika durowa", "Pentatoniki i Blues",
         {P1, M2, M3, P5, M6}, "tonal"},
        {"minor_pentatonic", "Pentatonika molowa", "Pentatoniki i Blues",
         {P1, m3, P4, P5, m7}, "tonal"},
        {"blues", "Bluesowa", "Pentatoniki i Blues",
         {P1, m3, P4, d5, P5, m7}, "tonal"},
        {"major_blues", "Bluesowa durowa", "Pentatoniki i Blues",
         {P1, M2, m3, M3, P5, M6}, "tonal"},

        // Jazzowe / Modi molowej melodycznej i harmonicznej
        {"lydian_dominant", "Lidyjska dominantowa", "Jazzowe",
         {P1, M2, M3, A4, P5, M6, m7}, "Levine / Nettles-Graf"},
        {"lydian_sharp2", "Lidyjska ♯2", "Jazzowe",
         {P1, A2, M3, A4, P5, M6, M7}, "Levine / Nettles-Graf"},
        {"mixolydian_b6", "Miksolidyjska ♭6", "Jazzowe",
         {P1, M2, M3, P4, P5, m6, m7}, "Levine"},
        {"phrygian_dominant", "Frygijska dominantowa", "Jazzowe",
         {P1, m2, M3, P4, P5, m6, m7}, "Levine / Nettles-Graf"},
        {"altered", "Alterowana (Superlokrycka)", "Jazzowe",
         {P1, m2, A2, M3, d5, m6, m7}, "Levine"},
        {"locrian_nat2", "Lokrycka ♮2", "Jazzowe",
         {P1, M2, m3, P4, d5, m6, m7}, "Levine"},
        {"bebop_dominant", "Bebop dominantowa", "Jazzowe",
         {P1, M2, M3, P4, P5, M6, m7, M7}, "Levine"},

        // Symetryczne
        {"whole_tone", "Całotonowa", "Symetryczne",
         {P1, M2, M3, A4, A5, m7}, "tonal / Levine"},
        {"diminished_half_whole", "Zmniejszona (pół-cała)", "Symetryczne",
         {P1, m2, m3, M3, A4, P5, M6, m7}, "Levine"},
        {"diminished_whole_half", "Zmniejszona (cała-pół)", "Symetryczne",
         {P1, M2, m3, P4, d5, m6, M6, M7}, "Levine"},
        {"augmented", "Zwiększona", "Symetryczne",
         {P1, A2, M3, P5, A5, M7}, "tonal / Levine"},
        {"messiaen_3", "Tryb Messiaena 3", "Symetryczne",
         {P1, M2, m3, M3, A4, P5, m6, M6, M7}, "Ian Ring"},
        {"chromatic", "Chromatyczna", "Symetryczne",
         {P1, m2, M2, m3, M3, P4, A4, P5, m6, M6, m7, M7}, "tonal"},

        // Egzotyczne
        {"hungarian_minor", "Węgierska molowa", "Egzotyczne",
         {P1, M2, m3, A4, P5, m6, M7}, "Ian Ring / tonal"},
        {"double_harmonic", "Podwójnie harmoniczna (Bizantyjska)", "Egzotyczne",
         {P1, m2, M3, P4, P5, m6, M7}, "Ian Ring / tonal"},
        {"hirajoshi", "Hirajoshi (japońska)", "Egzotyczne",
         {P1, M2, m3, P5, m6}, "Ian Ring / tonal"},
        {"in_sen", "In Sen (japońska)", "Egzotyczne",
         {P1, m2, P4, P5, m7}, "Ian Ring / tonal"}
    };
    return defs;
}

const ScaleDefinition* findScaleDefinition(std::string_view id) noexcept {
    for (const auto& def : allScaleDefinitions()) {
        if (def.id == id) {
            return &def;
        }
    }
    return nullptr;
}

} // namespace gitara::theory
