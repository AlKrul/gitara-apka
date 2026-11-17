#include "gitara/dsp/SignalConditioner.h"
#include <numbers>

namespace gitara::dsp {

SignalConditioner::SignalConditioner(double sampleRate,
                                     double hpfCutoffHz,
                                     double notchFreqHz,
                                     float gateThresholdDb) {
    prepare(sampleRate, hpfCutoffHz, notchFreqHz, gateThresholdDb);
}

void SignalConditioner::prepare(double sampleRate,
                                double hpfCutoffHz,
                                double notchFreqHz,
                                float gateThresholdDb) noexcept {
    if (sampleRate <= 0.0) {
        sampleRate = 48000.0;
    }

    // 1. Butterworth HPF 2. rzędu (Q = 1 / sqrt(2))
    {
        double w0 = 2.0 * std::numbers::pi * hpfCutoffHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double alpha = sinw0 / std::sqrt(2.0);
        double a0 = 1.0 + alpha;

        hpf_.b0 = static_cast<float>(((1.0 + cosw0) * 0.5) / a0);
        hpf_.b1 = static_cast<float>(-(1.0 + cosw0) / a0);
        hpf_.b2 = static_cast<float>(((1.0 + cosw0) * 0.5) / a0);
        hpf_.a1 = static_cast<float>((-2.0 * cosw0) / a0);
        hpf_.a2 = static_cast<float>((1.0 - alpha) / a0);
        hpf_.reset();
    }

    // 2. Wąski filtr wycinający przydźwięk sieciowy (Notch 50 Hz, Q = 10)
    {
        double w0 = 2.0 * std::numbers::pi * notchFreqHz / sampleRate;
        double cosw0 = std::cos(w0);
        double sinw0 = std::sin(w0);
        double q = 10.0;
        double alpha = sinw0 / (2.0 * q);
        double a0 = 1.0 + alpha;

        notch_.b0 = static_cast<float>(1.0 / a0);
        notch_.b1 = static_cast<float>((-2.0 * cosw0) / a0);
        notch_.b2 = static_cast<float>(1.0 / a0);
        notch_.a1 = static_cast<float>((-2.0 * cosw0) / a0);
        notch_.a2 = static_cast<float>((1.0 - alpha) / a0);
        notch_.reset();
    }

    openThresholdLinear_ = std::pow(10.0f, gateThresholdDb / 20.0f);
    closeThresholdLinear_ = std::pow(10.0f, (gateThresholdDb - 4.0f) / 20.0f);
    gateOpen_ = false;
}

void SignalConditioner::reset() noexcept {
    hpf_.reset();
    notch_.reset();
    gateOpen_ = false;
}

float SignalConditioner::processBlock(float* samples, std::size_t numSamples) noexcept {
    if (!samples || numSamples == 0) {
        return 0.0f;
    }

    double sumSq = 0.0;
    for (std::size_t i = 0; i < numSamples; ++i) {
        float y = hpf_.processSample(samples[i]);
        y = notch_.processSample(y);
        samples[i] = y;
        sumSq += static_cast<double>(y) * static_cast<double>(y);
    }

    float rms = static_cast<float>(std::sqrt(sumSq / static_cast<double>(numSamples)));
    if (gateOpen_) {
        if (rms < closeThresholdLinear_) {
            gateOpen_ = false;
        }
    } else {
        if (rms >= openThresholdLinear_) {
            gateOpen_ = true;
        }
    }

    if (!gateOpen_) {
        for (std::size_t i = 0; i < numSamples; ++i) {
            samples[i] = 0.0f;
        }
    }
    return rms;
}

} // namespace gitara::dsp
