#include "gitara/dsp/BandpassPreFilter.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace gitara::dsp {

BiquadSection BandpassPreFilter::makeHighpass(double sampleRate, double cutoffHz, double q) noexcept {
    BiquadSection sec{};
    const double clampedFc = std::clamp(cutoffHz, 5.0, sampleRate * 0.45);
    const double w0 = 2.0 * std::numbers::pi * clampedFc / sampleRate;
    const double cosw0 = std::cos(w0);
    const double sinw0 = std::sin(w0);
    const double alpha = sinw0 / (2.0 * q);
    const double a0 = 1.0 + alpha;

    sec.b0 = ((1.0 + cosw0) * 0.5) / a0;
    sec.b1 = (-(1.0 + cosw0)) / a0;
    sec.b2 = ((1.0 + cosw0) * 0.5) / a0;
    sec.a1 = (-2.0 * cosw0) / a0;
    sec.a2 = (1.0 - alpha) / a0;
    return sec;
}

BiquadSection BandpassPreFilter::makeLowpass(double sampleRate, double cutoffHz, double q) noexcept {
    BiquadSection sec{};
    const double clampedFc = std::clamp(cutoffHz, 20.0, sampleRate * 0.45);
    const double w0 = 2.0 * std::numbers::pi * clampedFc / sampleRate;
    const double cosw0 = std::cos(w0);
    const double sinw0 = std::sin(w0);
    const double alpha = sinw0 / (2.0 * q);
    const double a0 = 1.0 + alpha;

    sec.b0 = ((1.0 - cosw0) * 0.5) / a0;
    sec.b1 = (1.0 - cosw0) / a0;
    sec.b2 = ((1.0 - cosw0) * 0.5) / a0;
    sec.a1 = (-2.0 * cosw0) / a0;
    sec.a2 = (1.0 - alpha) / a0;
    return sec;
}

BiquadSection BandpassPreFilter::makeNotch(double sampleRate, double centerHz, double q) noexcept {
    BiquadSection sec{};
    const double clampedFc = std::clamp(centerHz, 10.0, sampleRate * 0.45);
    const double w0 = 2.0 * std::numbers::pi * clampedFc / sampleRate;
    const double cosw0 = std::cos(w0);
    const double sinw0 = std::sin(w0);
    const double alpha = sinw0 / (2.0 * q);
    const double a0 = 1.0 + alpha;

    sec.b0 = 1.0 / a0;
    sec.b1 = (-2.0 * cosw0) / a0;
    sec.b2 = 1.0 / a0;
    sec.a1 = (-2.0 * cosw0) / a0;
    sec.a2 = (1.0 - alpha) / a0;
    return sec;
}

void BandpassPreFilter::prepare(double sampleRate,
                                double lowCutoffHz,
                                double highCutoffHz,
                                double mainsNotchHz) noexcept {
    if (sampleRate <= 0.0) sampleRate = 48000.0;

    // Kaskada Butterwortha 4. rzędu (maksymalnie płaska w pasmie przepustowym, -24 dB/okt):
    // Q1 = 1 / (2·cos(π/8)) ≈ 0.5411961, Q2 = 1 / (2·cos(3π/8)) ≈ 1.3065630
    constexpr double kButter4Q1 = 0.541196100146197;
    constexpr double kButter4Q2 = 1.3065629648763766;

    hp1_ = makeHighpass(sampleRate, lowCutoffHz, kButter4Q1);
    hp2_ = makeHighpass(sampleRate, lowCutoffHz, kButter4Q2);
    lp1_ = makeLowpass(sampleRate, highCutoffHz, kButter4Q1);
    lp2_ = makeLowpass(sampleRate, highCutoffHz, kButter4Q2);

    notchEnabled_ = (mainsNotchHz >= 20.0 && mainsNotchHz < sampleRate * 0.45);
    if (notchEnabled_) {
        notch_ = makeNotch(sampleRate, mainsNotchHz, 10.0);
    }
    reset();
}

void BandpassPreFilter::reset() noexcept {
    hp1_.reset();
    hp2_.reset();
    lp1_.reset();
    lp2_.reset();
    notch_.reset();
}

float BandpassPreFilter::processSample(float x) noexcept {
    float y = hp1_.processSample(x);
    y = hp2_.processSample(y);
    if (notchEnabled_) {
        y = notch_.processSample(y);
    }
    y = lp1_.processSample(y);
    y = lp2_.processSample(y);
    return y;
}

void BandpassPreFilter::processBlock(const float* input, float* output, std::size_t numSamples) noexcept {
    if (!input || !output || numSamples == 0) return;
    for (std::size_t i = 0; i < numSamples; ++i) {
        output[i] = processSample(input[i]);
    }
}

void AudioRingBuffer::prepare(std::size_t capacity) {
    buffer_.assign(std::max<std::size_t>(capacity, 256), 0.0f);
    writePos_ = 0;
    totalWritten_ = 0;
}

void AudioRingBuffer::reset() noexcept {
    std::fill(buffer_.begin(), buffer_.end(), 0.0f);
    writePos_ = 0;
    totalWritten_ = 0;
}

void AudioRingBuffer::pushSample(float sample) noexcept {
    if (buffer_.empty()) return;
    buffer_[writePos_] = sample;
    writePos_ = (writePos_ + 1) % buffer_.size();
    ++totalWritten_;
}

void AudioRingBuffer::pushBlock(const float* samples, std::size_t numSamples) noexcept {
    if (!samples || buffer_.empty()) return;
    for (std::size_t i = 0; i < numSamples; ++i) {
        buffer_[writePos_] = samples[i];
        writePos_ = (writePos_ + 1) % buffer_.size();
    }
    totalWritten_ += numSamples;
}

void AudioRingBuffer::copyLatest(float* dest, std::size_t length) const noexcept {
    if (!dest || buffer_.empty() || length == 0) return;
    const std::size_t cap = buffer_.size();
    const std::size_t useLen = std::min(length, cap);
    const std::size_t start = (writePos_ + cap - useLen) % cap;

    const std::size_t firstPart = std::min(useLen, cap - start);
    std::copy_n(buffer_.data() + start, firstPart, dest);
    if (firstPart < useLen) {
        std::copy_n(buffer_.data(), useLen - firstPart, dest + firstPart);
    }
}

} // namespace gitara::dsp
