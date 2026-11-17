#include "gitara/theory/Pitch.h"
#include <charconv>

namespace gitara::theory {

Pitch Pitch::fromMidiNumber(int midi, AccidentalMode mode) noexcept {
    int pc = midi % 12;
    if (pc < 0) pc += 12;
    int oct = (midi - pc) / 12 - 1;
    Note n = Note::fromPitchClass(pc, mode);
    return Pitch{n, oct};
}

std::optional<Pitch> Pitch::parse(std::string_view text) noexcept {
    if (text.size() < 2) {
        return std::nullopt;
    }

    // Szukamy miejsca, gdzie zaczyna się liczba oktawy (opcjonalny minus + cyfry na końcu)
    std::size_t digitStart = text.size();
    while (digitStart > 0 && (text[digitStart - 1] >= '0' && text[digitStart - 1] <= '9')) {
        --digitStart;
    }
    if (digitStart == text.size() || digitStart == 0) {
        return std::nullopt;
    }
    if (digitStart > 1 && text[digitStart - 1] == '-') {
        --digitStart;
    }

    auto parsedNote = Note::parse(text.substr(0, digitStart));
    if (!parsedNote) {
        return std::nullopt;
    }

    int oct = 0;
    auto octView = text.substr(digitStart);
    auto [ptr, ec] = std::from_chars(octView.data(), octView.data() + octView.size(), oct);
    if (ec != std::errc{} || ptr != octView.data() + octView.size()) {
        return std::nullopt;
    }

    return Pitch{*parsedNote, oct};
}

PitchDeviation analyzeFrequency(double frequencyHz,
                                double a4ReferenceHz,
                                AccidentalMode mode) noexcept {
    PitchDeviation result{};
    if (frequencyHz <= 0.0 || a4ReferenceHz <= 0.0) {
        return result;
    }

    double exactMidi = 69.0 + 12.0 * std::log2(frequencyHz / a4ReferenceHz);
    int nearestMidi = static_cast<int>(std::llround(exactMidi));
    double cents = (exactMidi - static_cast<double>(nearestMidi)) * 100.0;

    result.closestPitch = Pitch::fromMidiNumber(nearestMidi, mode);
    result.midiNote = nearestMidi;
    result.exactMidi = exactMidi;
    result.cents = cents;
    result.targetFrequencyHz = result.closestPitch.frequencyHz(a4ReferenceHz);
    return result;
}

} // namespace gitara::theory
