#include "gitara/dsp/OnsetDetector.h"
#include <algorithm>
#include <cmath>

namespace gitara::dsp {

OnsetDetector::OnsetDetector(double sampleRate, double minInterOnsetSeconds) {
    prepare(sampleRate, minInterOnsetSeconds);
}

void OnsetDetector::prepare(double sampleRate, double minInterOnsetSeconds) noexcept {
    if (sampleRate <= 0.0) sampleRate = 48000.0;
    sampleRate_ = sampleRate;
    minCooldownSamples_ = static_cast<std::size_t>(std::max(1.0, sampleRate_ * minInterOnsetSeconds));
    reset();
}

void OnsetDetector::reset() noexcept {
    samplesSinceLastOnset_ = minCooldownSamples_ + 1;
    previousEnergy_ = 0.0f;
    previousHfc_ = 0.0f;
}

bool OnsetDetector::processBlock(const float* samples, std::size_t numSamples) noexcept {
    if (!samples || numSamples == 0) {
        return false;
    }

    samplesSinceLastOnset_ += numSamples;

    double sumSq = 0.0;
    double hfcSum = 0.0;
    for (std::size_t i = 0; i < numSamples; ++i) {
        double s = static_cast<double>(samples[i]);
        sumSq += s * s;
        if (i > 0) {
            double diff = s - static_cast<double>(samples[i - 1]);
            hfcSum += diff * diff;
        }
    }

    float energy = static_cast<float>(sumSq / static_cast<double>(numSamples));
    float hfc = static_cast<float>(hfcSum / static_cast<double>(numSamples));

    bool isOnset = false;
    if (samplesSinceLastOnset_ >= minCooldownSamples_) {
        bool energyJump = (energy > 0.0005f) && (energy > previousEnergy_ * 2.6f + 0.0002f);
        bool hfcJump = (hfc > 0.0002f) && (hfc > previousHfc_ * 3.0f + 0.0001f);
        if (energyJump || hfcJump) {
            isOnset = true;
            samplesSinceLastOnset_ = 0;
        }
    }

    // Wygładzanie obwiedni odniesienia
    previousEnergy_ = 0.65f * previousEnergy_ + 0.35f * energy;
    previousHfc_ = 0.65f * previousHfc_ + 0.35f * hfc;
    return isOnset;
}

} // namespace gitara::dsp
