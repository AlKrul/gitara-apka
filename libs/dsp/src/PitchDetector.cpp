#include "gitara/dsp/PitchDetector.h"
#include <algorithm>
#include <bit>
#include <cmath>

namespace gitara::dsp {

PitchDetector::PitchDetector(double sampleRate,
                             std::size_t windowSize,
                             double minFreqHz,
                             double maxFreqHz) {
    prepare(sampleRate, windowSize, minFreqHz, maxFreqHz);
}

void PitchDetector::prepare(double sampleRate,
                            std::size_t windowSize,
                            double minFreqHz,
                            double maxFreqHz) {
    if (sampleRate <= 0.0) sampleRate = 48000.0;
    sampleRate_ = sampleRate;
    maxWindowSize_ = std::max<std::size_t>(windowSize, 512);

    minTau_ = static_cast<std::size_t>(std::max(2.0, std::floor(sampleRate_ / maxFreqHz)));
    maxTau_ = static_cast<std::size_t>(std::min(
        static_cast<double>(maxWindowSize_ / 2),
        std::ceil(sampleRate_ / minFreqHz)));

    yinBuffer_.assign(maxTau_ + 2, 0.0);
    bitWords_.assign((maxWindowSize_ + 63) / 64 + 2, 0ULL);
}

PitchResult PitchDetector::detect(const float* samples,
                                  std::size_t numSamples,
                                  PitchAlgorithm algorithm,
                                  double a4ReferenceHz,
                                  gitara::theory::AccidentalMode accidentalMode) noexcept {
    if (!samples || numSamples < minTau_ * 2) {
        return PitchResult{};
    }

    // Sprawdzenie energii sygnału (cisza nie jest dźwiękiem)
    double sumSq = 0.0;
    for (std::size_t i = 0; i < numSamples; ++i) {
        sumSq += static_cast<double>(samples[i]) * static_cast<double>(samples[i]);
    }
    double rms = std::sqrt(sumSq / static_cast<double>(numSamples));
    if (rms < 1e-4) {
        return PitchResult{};
    }

    if (algorithm == PitchAlgorithm::BitstreamAutocorrelation) {
        return detectBitstream(samples, numSamples, a4ReferenceHz, accidentalMode);
    }
    return detectYin(samples, numSamples, a4ReferenceHz, accidentalMode);
}

PitchResult PitchDetector::detectYin(const float* samples,
                                     std::size_t n,
                                     double a4ReferenceHz,
                                     gitara::theory::AccidentalMode mode) noexcept {
    const std::size_t halfN = std::min(n / 2, maxTau_);
    if (halfN <= minTau_) {
        return PitchResult{};
    }

    yinBuffer_[0] = 1.0;
    double runningSum = 0.0;

    for (std::size_t tau = 1; tau <= halfN; ++tau) {
        double diffSum = 0.0;
        const std::size_t limit = n - halfN;
        for (std::size_t j = 0; j < limit; ++j) {
            double delta = static_cast<double>(samples[j]) - static_cast<double>(samples[j + tau]);
            diffSum += delta * delta;
        }
        runningSum += diffSum;
        if (runningSum > 1e-12) {
            yinBuffer_[tau] = diffSum * static_cast<double>(tau) / runningSum;
        } else {
            yinBuffer_[tau] = 1.0;
        }
    }

    // Krok 4 YIN: próg bezwzględny zapobiegający błędom oktawy (wybór pierwszego minimum poniżej progu)
    constexpr double kYinThreshold = 0.15;
    std::size_t bestTau = 0;

    for (std::size_t tau = minTau_; tau < halfN; ++tau) {
        if (yinBuffer_[tau] < kYinThreshold) {
            while (tau + 1 < halfN && yinBuffer_[tau + 1] < yinBuffer_[tau]) {
                ++tau;
            }
            bestTau = tau;
            break;
        }
    }

    if (bestTau == 0) {
        // Jeśli nie przekroczono progu 0.15, szukamy globalnego minimum (o ile jest wyraźne < 0.35)
        double minVal = 1.0;
        for (std::size_t tau = minTau_; tau < halfN; ++tau) {
            if (yinBuffer_[tau] < minVal) {
                minVal = yinBuffer_[tau];
                bestTau = tau;
            }
        }
        if (minVal > 0.35) {
            return PitchResult{};
        }
    }

    // Krok 5 YIN: interpolacja paraboliczna wokół bestTau dla dokładności ułamka centa
    double refinedTau = static_cast<double>(bestTau);
    if (bestTau > 1 && bestTau + 1 <= halfN) {
        double s0 = yinBuffer_[bestTau - 1];
        double s1 = yinBuffer_[bestTau];
        double s2 = yinBuffer_[bestTau + 1];
        double denom = 2.0 * (2.0 * s1 - s2 - s0);
        if (std::abs(denom) > 1e-12) {
            refinedTau += (s2 - s0) / denom;
        }
    }

    PitchResult res{};
    res.pitched = true;
    res.frequencyHz = sampleRate_ / refinedTau;
    res.confidence = std::clamp(1.0 - yinBuffer_[bestTau], 0.0, 1.0);
    res.deviation = gitara::theory::analyzeFrequency(res.frequencyHz, a4ReferenceHz, mode);
    return res;
}

PitchResult PitchDetector::detectBitstream(const float* samples,
                                           std::size_t n,
                                           double a4ReferenceHz,
                                           gitara::theory::AccidentalMode mode) noexcept {
    const std::size_t clampedN = std::min(n, maxWindowSize_);
    const std::size_t halfN = std::min(clampedN / 2, maxTau_);
    if (halfN <= minTau_) {
        return PitchResult{};
    }

    // 1. Konwersja sygnału na strumień bitów (zero-crossing sign bit) w słowach 64-bitowych
    const std::size_t numWords = (clampedN + 63) / 64;
    std::fill_n(bitWords_.begin(), numWords + 1, 0ULL);
    for (std::size_t i = 0; i < clampedN; ++i) {
        if (samples[i] >= 0.0f) {
            bitWords_[i / 64] |= (1ULL << (i % 64));
        }
    }

    // 2. Autokorelacja strumienia bitów (XOR + popcount)
    const std::size_t compareWords = (clampedN - halfN) / 64;
    if (compareWords == 0) {
        return detectYin(samples, n, a4ReferenceHz, mode);
    }

    for (std::size_t tau = minTau_; tau <= halfN; ++tau) {
        const std::size_t wordShift = tau / 64;
        const std::size_t bitShift = tau % 64;
        std::size_t mismatchBits = 0;

        for (std::size_t w = 0; w < compareWords; ++w) {
            std::uint64_t shifted = 0ULL;
            if (bitShift == 0) {
                shifted = bitWords_[w + wordShift];
            } else {
                shifted = (bitWords_[w + wordShift] >> bitShift) |
                          (bitWords_[w + wordShift + 1] << (64 - bitShift));
            }
            mismatchBits += static_cast<std::size_t>(std::popcount(bitWords_[w] ^ shifted));
        }
        yinBuffer_[tau] = static_cast<double>(mismatchBits) /
                          static_cast<double>(compareWords * 64);
    }

    // Znajdź pierwsze głębokie minimum autokorelacji bitowej i doprecyzuj na próbkach rzeczywistych
    double globalMin = 1.0;
    std::size_t bestTau = minTau_;
    for (std::size_t tau = minTau_; tau <= halfN; ++tau) {
        if (yinBuffer_[tau] < globalMin) {
            globalMin = yinBuffer_[tau];
            bestTau = tau;
        }
    }
    if (globalMin > 0.25) {
        return PitchResult{};
    }

    // Sprawdź, czy wcześniejszy podokres nie był drugą harmoniczną (szukamy najkrótszego właściwego okresu powyżej progu)
    double threshold = std::max(0.12, globalMin * 1.15);
    for (std::size_t tau = minTau_ + 1; tau < halfN; ++tau) {
        if (yinBuffer_[tau] <= threshold &&
            yinBuffer_[tau] <= yinBuffer_[tau - 1] &&
            yinBuffer_[tau] <= yinBuffer_[tau + 1]) {
            bestTau = tau;
            break;
        }
    }

    // Dokładna interpolacja paraboliczna na korelacji zmiennoprzecinkowej w sąsiedztwie bestTau
    auto floatDiff = [&](std::size_t t) -> double {
        double sum = 0.0;
        const std::size_t limit = clampedN - halfN;
        for (std::size_t i = 0; i < limit; ++i) {
            double d = static_cast<double>(samples[i]) - static_cast<double>(samples[i + t]);
            sum += d * d;
        }
        return sum;
    };

    double refinedTau = static_cast<double>(bestTau);
    if (bestTau > minTau_ && bestTau + 1 <= halfN) {
        double d0 = floatDiff(bestTau - 1);
        double d1 = floatDiff(bestTau);
        double d2 = floatDiff(bestTau + 1);
        if (d0 < d1 && bestTau > minTau_ + 1) {
            --bestTau;
            d2 = d1;
            d1 = d0;
            d0 = floatDiff(bestTau - 1);
        } else if (d2 < d1 && bestTau + 2 <= halfN) {
            ++bestTau;
            d0 = d1;
            d1 = d2;
            d2 = floatDiff(bestTau + 1);
        }
        refinedTau = static_cast<double>(bestTau);
        double denom = 2.0 * (2.0 * d1 - d2 - d0);
        if (std::abs(denom) > 1e-12) {
            refinedTau += (d2 - d0) / denom;
        }
    }

    PitchResult res{};
    res.pitched = true;
    res.frequencyHz = sampleRate_ / refinedTau;
    res.confidence = std::clamp(1.0 - globalMin * 2.0, 0.0, 1.0);
    res.deviation = gitara::theory::analyzeFrequency(res.frequencyHz, a4ReferenceHz, mode);
    return res;
}

} // namespace gitara::dsp
