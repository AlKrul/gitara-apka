#include "gitara/dsp/KarplusStrongSynth.h"
#include <algorithm>
#include <cmath>

namespace gitara::dsp {

KarplusStrongSynth::KarplusStrongSynth(double sampleRate) {
    prepare(sampleRate);
}

void KarplusStrongSynth::prepare(double sampleRate) {
    if (sampleRate <= 0.0) sampleRate = 48000.0;
    sampleRate_ = sampleRate;
    const std::size_t maxDelay = static_cast<std::size_t>(sampleRate_ / 45.0) + 8;
    for (auto& v : voices_) {
        v.active = false;
        v.midiNote = -1;
        v.delayLine.assign(maxDelay, 0.0f);
        v.periodSamples = 100;
        v.writePos = 0;
        v.prevSample = 0.0f;
        v.damping = 0.996f;
    }
}

void KarplusStrongSynth::allNotesOff() noexcept {
    for (auto& v : voices_) {
        v.active = false;
        v.midiNote = -1;
        v.prevSample = 0.0f;
    }
}

void KarplusStrongSynth::pluckFrequency(double frequencyHz, float velocity) noexcept {
    if (frequencyHz < 50.0 || frequencyHz > 2500.0) {
        return;
    }

    auto& v = voices_[nextVoiceIndex_];
    nextVoiceIndex_ = (nextVoiceIndex_ + 1) % voices_.size();

    std::size_t period = static_cast<std::size_t>(std::llround(sampleRate_ / frequencyHz));
    period = std::clamp<std::size_t>(period, 8, v.delayLine.size() - 1);

    v.periodSamples = period;
    v.writePos = 0;
    v.prevSample = 0.0f;
    v.damping = 0.9965f;
    v.active = true;

    // Wzbudzenie wygładzonym impulsem szumu o zerowej składowej stałej (ciepłe brzmienie struny)
    float prevNoise = 0.0f;
    float amp = std::clamp(velocity, 0.05f, 1.0f) * 0.45f;
    for (std::size_t i = 0; i < period; ++i) {
        rngState_ = rngState_ * 1664525u + 1013904223u;
        float white = (static_cast<float>(rngState_ >> 8) / static_cast<float>(0x00FFFFFFu)) * 2.0f - 1.0f;
        float filtered = 0.5f * (white + prevNoise);
        prevNoise = white;
        v.delayLine[i] = filtered * amp;
    }
}

void KarplusStrongSynth::noteOn(int midiNote, float velocity, double a4ReferenceHz) noexcept {
    double freq = a4ReferenceHz * std::pow(2.0, (midiNote - 69) / 12.0);
    std::size_t targetIdx = nextVoiceIndex_;
    pluckFrequency(freq, velocity);
    voices_[targetIdx].midiNote = midiNote;
}

void KarplusStrongSynth::noteOff(int midiNote) noexcept {
    for (auto& v : voices_) {
        if (v.active && v.midiNote == midiNote) {
            v.damping = 0.94f; // Szybkie wygaszenie struny po puszczeniu klawisza
        }
    }
}

void KarplusStrongSynth::renderNextBlock(float* outputBuffer, std::size_t numSamples) noexcept {
    if (!outputBuffer || numSamples == 0) {
        return;
    }

    for (std::size_t i = 0; i < numSamples; ++i) {
        float sum = 0.0f;
        for (auto& v : voices_) {
            if (!v.active) continue;
            float current = v.delayLine[v.writePos];
            sum += current;
            float nextVal = v.damping * 0.5f * (current + v.prevSample);
            v.prevSample = current;
            v.delayLine[v.writePos] = nextVal;
            v.writePos = (v.writePos + 1) % v.periodSamples;
        }
        outputBuffer[i] += sum;
    }
}

} // namespace gitara::dsp
