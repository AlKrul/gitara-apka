#include "gitara/dsp/ChordDetector.h"
#include "gitara/theory/Pitch.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace gitara::dsp {

ChordDetector::ChordDetector(double sampleRate) {
    prepare(sampleRate);
}

void ChordDetector::prepare(double sampleRate) noexcept {
    if (sampleRate <= 0.0) sampleRate = 48000.0;
    sampleRate_ = sampleRate;
}

std::array<float, 12> ChordDetector::computeChromagram(
    const float* samples,
    std::size_t numSamples,
    double a4ReferenceHz) const noexcept {
    std::array<float, 12> chroma{};
    if (!samples || numSamples < 256) {
        return chroma;
    }

    // Analiza Goertzela dla oktaw gitarowych MIDI 40 (E2 = 82.4 Hz) .. MIDI 76 (E5 = 659.3 Hz)
    // z oknem Hanna i tłumieniem 3. harmonicznej (kwinty), aby wzmocnić tony podstawowe akordu.
    std::array<double, 37> noteEnergies{};
    for (int midi = 40; midi <= 76; ++midi) {
        double freq = a4ReferenceHz * std::pow(2.0, (midi - 69) / 12.0);
        double omega = 2.0 * std::numbers::pi * freq / sampleRate_;
        double coeff = 2.0 * std::cos(omega);
        double s1 = 0.0;
        double s2 = 0.0;

        for (std::size_t n = 0; n < numSamples; ++n) {
            double window = 0.5 * (1.0 - std::cos(2.0 * std::numbers::pi * static_cast<double>(n) /
                                                  static_cast<double>(numSamples - 1)));
            double x = static_cast<double>(samples[n]) * window;
            double s0 = x + coeff * s1 - s2;
            s2 = s1;
            s1 = s0;
        }

        double power = s1 * s1 + s2 * s2 - coeff * s1 * s2;
        noteEnergies[static_cast<std::size_t>(midi - 40)] = std::sqrt(std::max(0.0, power));
    }

    // Redukcja przebicia 3. harmonicznej (+19 półtonów = oktawa + kwinta czysta)
    for (int i = 36; i >= 19; --i) {
        noteEnergies[static_cast<std::size_t>(i)] = std::max(
            0.0,
            noteEnergies[static_cast<std::size_t>(i)] -
                0.35 * noteEnergies[static_cast<std::size_t>(i - 19)]);
    }

    for (int midi = 40; midi <= 76; ++midi) {
        int pc = midi % 12;
        chroma[static_cast<std::size_t>(pc)] +=
            static_cast<float>(noteEnergies[static_cast<std::size_t>(midi - 40)]);
    }

    float norm = 0.0f;
    for (float v : chroma) {
        norm += v * v;
    }
    norm = std::sqrt(norm);
    if (norm > 1e-6f) {
        for (float& v : chroma) {
            v /= norm;
        }
    }
    return chroma;
}

double ChordDetector::verifyExpectedChord(
    const float* samples,
    std::size_t numSamples,
    const gitara::theory::Chord& expectedChord,
    double a4ReferenceHz) const noexcept {
    if (!expectedChord.definition) {
        return 0.0;
    }

    const auto chroma = computeChromagram(samples, numSamples, a4ReferenceHz);
    std::array<float, 12> tpl{};
    const auto notes = expectedChord.notes();
    for (std::size_t i = 0; i < notes.size(); ++i) {
        int pc = notes[i].pitchClass();
        // Pryma i tercja mają nieco wyższą wagę niż opcjonalna kwinta
        tpl[static_cast<std::size_t>(pc)] = (i == 0) ? 1.15f : ((i == 1) ? 1.1f : 0.95f);
    }

    float tplNorm = 0.0f;
    for (float v : tpl) tplNorm += v * v;
    tplNorm = std::sqrt(tplNorm);
    if (tplNorm <= 1e-6f) return 0.0;

    double dot = 0.0;
    for (std::size_t pc = 0; pc < 12; ++pc) {
        dot += static_cast<double>(chroma[pc]) * static_cast<double>(tpl[pc] / tplNorm);
    }
    return std::clamp(dot, 0.0, 1.0);
}

ChordDetectionResult ChordDetector::detectChord(
    const float* samples,
    std::size_t numSamples,
    bool includeSeventhChords,
    double a4ReferenceHz) const noexcept {
    ChordDetectionResult best{};
    best.chromagram = computeChromagram(samples, numSamples, a4ReferenceHz);

    float energyCheck = 0.0f;
    for (float v : best.chromagram) energyCheck += v;
    if (energyCheck <= 1e-4f) {
        return best;
    }

    for (const auto& def : gitara::theory::allChordDefinitions()) {
        if (def.id != "major" && def.id != "minor" && def.id != "dim" && def.id != "aug" &&
            (!includeSeventhChords || (def.id != "7" && def.id != "maj7" && def.id != "m7"))) {
            continue;
        }

        for (int rootPc = 0; rootPc < 12; ++rootPc) {
            gitara::theory::Chord candidate{
                gitara::theory::Note::fromPitchClass(rootPc, gitara::theory::AccidentalMode::Auto),
                &def,
                std::nullopt
            };

            std::array<float, 12> tpl{};
            const auto notes = candidate.notes();
            for (std::size_t i = 0; i < notes.size(); ++i) {
                int pc = notes[i].pitchClass();
                tpl[static_cast<std::size_t>(pc)] = 1.0f;
            }
            float tplNorm = std::sqrt(static_cast<float>(notes.size()));
            double dot = 0.0;
            for (std::size_t pc = 0; pc < 12; ++pc) {
                dot += static_cast<double>(best.chromagram[pc]) *
                       static_cast<double>(tpl[pc] / tplNorm);
            }
            // Lekka kara za złożoność 4-dźwięku, by trójdźwięk nie był mylony z akordem 7 bez septymy
            if (notes.size() == 4) {
                dot *= 0.96;
            }

            if (dot > best.confidence) {
                best.confidence = dot;
                best.chord = candidate;
                best.detected = (dot >= 0.65);
            }
        }
    }

    return best;
}

} // namespace gitara::dsp
