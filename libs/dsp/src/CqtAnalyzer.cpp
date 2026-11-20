#include "gitara/dsp/CqtAnalyzer.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace gitara::dsp {

void CqtAnalyzer::prepare(const GuitarDspConfig& config) {
    sampleRate_ = (config.sampleRate > 0.0) ? config.sampleRate : 48000.0;
    a4ReferenceHz_ = (config.a4ReferenceHz > 0.0) ? config.a4ReferenceHz : 440.0;
    minMidiNote_ = config.minMidiNote;
    maxMidiNote_ = std::max(config.minMidiNote + 12, config.maxMidiNote);
    binsPerSemitone_ = std::max(1, config.binsPerSemitone);
    maxWindowSize_ = std::max<std::size_t>(512, config.maxWindowSize);

    const std::size_t totalBins = static_cast<std::size_t>((maxMidiNote_ - minMidiNote_) * binsPerSemitone_ + 1);
    bins_.clear();
    bins_.resize(totalBins);
    mpmDecBuf_.assign(2048, 0.0f);
    mpmNsdfBuf_.assign(512, 0.0f);

    const std::size_t minWin = std::clamp<std::size_t>(config.minWindowSize, 128, maxWindowSize_);

    for (std::size_t k = 0; k < totalBins; ++k) {
        BinKernel& bk = bins_[k];
        bk.midiFloat = static_cast<double>(minMidiNote_) +
                       static_cast<double>(k) / static_cast<double>(binsPerSemitone_);
        bk.freqHz = midiToFreqHz(bk.midiFloat);

        const double idealWin = (config.qFactor * sampleRate_) / (bk.freqHz + std::max(0.0, config.vqtGammaHz));
        const std::size_t winLen = static_cast<std::size_t>(
            std::clamp(std::round(idealWin), static_cast<double>(minWin), static_cast<double>(maxWindowSize_)));

        const std::size_t dec = (winLen >= 1024 && bk.freqHz < 1200.0 && sampleRate_ >= 32000.0) ? 2 : 1;
        bk.windowLen = winLen;
        bk.decimation = dec;

        const std::size_t tapCount = (winLen + dec - 1) / dec;
        bk.cosKernel.resize(tapCount);
        bk.sinKernel.resize(tapCount);

        const double omega = 2.0 * std::numbers::pi * bk.freqHz / sampleRate_;
        double winSum = 0.0;

        for (std::size_t t = 0; t < tapCount; ++t) {
            const std::size_t n = t * dec;
            const double u = (winLen > 1) ? (static_cast<double>(n) / static_cast<double>(winLen - 1)) : 0.0;
            // Prawoskrętne przyczynowe okno Hanna (u^1.18): wąski płat główny dla rozdzielczości półtonowej
            const double uSkew = std::pow(u, 1.18);
            const double w = 0.5 * (1.0 - std::cos(2.0 * std::numbers::pi * uSkew));
            const double phase = omega * static_cast<double>(n);
            bk.cosKernel[t] = static_cast<float>(w * std::cos(phase));
            bk.sinKernel[t] = static_cast<float>(-w * std::sin(phase));
            winSum += w;
        }
        bk.normScale = (winSum > 1e-9) ? static_cast<float>(2.0 / winSum) : 1.0f;
    }
}

void CqtAnalyzer::analyze(const float* windowSamples,
                          std::size_t numSamples,
                          std::vector<float>& outMagnitudes) const noexcept {
    const std::size_t nBins = bins_.size();
    if (outMagnitudes.size() != nBins) {
        outMagnitudes.assign(nBins, 0.0f);
    }
    if (!windowSamples || numSamples == 0 || nBins == 0) {
        std::fill(outMagnitudes.begin(), outMagnitudes.end(), 0.0f);
        return;
    }

    for (std::size_t k = 0; k < nBins; ++k) {
        const BinKernel& bk = bins_[k];
        if (bk.freqHz >= sampleRate_ * 0.46) {
            outMagnitudes[k] = 0.0f;
            continue;
        }

        const std::size_t winLen = std::min(bk.windowLen, numSamples);
        const std::size_t offset = numSamples - winLen;
        const std::size_t dec = bk.decimation;
        const std::size_t tapCount = std::min(bk.cosKernel.size(), (winLen + dec - 1) / dec);

        const float* xPtr = windowSamples + offset;
        const float* cPtr = bk.cosKernel.data();
        const float* sPtr = bk.sinKernel.data();

        double re = 0.0;
        double im = 0.0;
        if (dec == 1) {
            for (std::size_t t = 0; t < tapCount; ++t) {
                const double xv = static_cast<double>(xPtr[t]);
                re += xv * static_cast<double>(cPtr[t]);
                im += xv * static_cast<double>(sPtr[t]);
            }
        } else {
            for (std::size_t t = 0; t < tapCount; ++t) {
                const double xv = static_cast<double>(xPtr[t * dec]);
                re += xv * static_cast<double>(cPtr[t]);
                im += xv * static_cast<double>(sPtr[t]);
            }
        }

        outMagnitudes[k] = static_cast<float>(std::hypot(re, im)) * bk.normScale;
    }
}

MpmFrameEstimate CqtAnalyzer::analyzeMpm(const float* windowSamples,
                                         std::size_t numSamples,
                                         double minHz,
                                         double maxHz,
                                         float minRmsGate) const noexcept {
    MpmFrameEstimate res{};
    if (!windowSamples || numSamples < 256) return res;

    const std::size_t dec = (sampleRate_ >= 32000.0) ? 2 : 1;
    const double effSr = sampleRate_ / static_cast<double>(dec);
    const std::size_t rawSpan = std::min<std::size_t>(numSamples, dec == 2 ? 2048 : 1024);
    const std::size_t W = rawSpan / dec;
    const std::size_t off = numSamples - W * dec;

    if (mpmDecBuf_.size() < W) mpmDecBuf_.resize(W, 0.0f);
    double mean = 0.0;
    if (dec == 2) {
        for (std::size_t i = 0; i < W; ++i) {
            const float v = 0.5f * (windowSamples[off + i * 2] + windowSamples[off + i * 2 + 1]);
            mpmDecBuf_[i] = v;
            mean += static_cast<double>(v);
        }
    } else {
        for (std::size_t i = 0; i < W; ++i) {
            const float v = windowSamples[off + i];
            mpmDecBuf_[i] = v;
            mean += static_cast<double>(v);
        }
    }
    mean /= static_cast<double>(W);
    double sumSq = 0.0;
    for (std::size_t i = 0; i < W; ++i) {
        mpmDecBuf_[i] -= static_cast<float>(mean);
        sumSq += static_cast<double>(mpmDecBuf_[i]) * static_cast<double>(mpmDecBuf_[i]);
    }
    res.rms = static_cast<float>(std::sqrt(sumSq / static_cast<double>(W)));

    const std::size_t shortSpan = std::min<std::size_t>(numSamples, 256);
    const std::size_t shortOff = numSamples - shortSpan;
    double shortSq = 0.0;
    for (std::size_t i = 0; i < shortSpan; ++i) {
        const double sv = static_cast<double>(windowSamples[shortOff + i]);
        shortSq += sv * sv;
    }
    res.shortRms = static_cast<float>(std::sqrt(shortSq / static_cast<double>(shortSpan)));

    if (res.rms < minRmsGate || res.shortRms < minRmsGate * 0.75f) {
        return res;
    }

    const std::size_t minTau = static_cast<std::size_t>(std::max(4.0, std::floor(effSr / maxHz) - 1.0));
    const std::size_t maxTau = static_cast<std::size_t>(
        std::min(std::ceil(effSr / minHz) + 2.0, std::floor(static_cast<double>(W) * 0.48)));
    if (maxTau <= minTau + 2) return res;

    if (mpmNsdfBuf_.size() < maxTau + 3) mpmNsdfBuf_.resize(maxTau + 3, 0.0f);
    double m = sumSq * 2.0;
    mpmNsdfBuf_[0] = 1.0f;
    for (std::size_t tau = 1; tau <= maxTau + 1; ++tau) {
        const double a = static_cast<double>(mpmDecBuf_[W - tau]);
        const double b = static_cast<double>(mpmDecBuf_[tau - 1]);
        m -= a * a + b * b;
        double r = 0.0;
        const std::size_t lim = W - tau;
        for (std::size_t j = 0; j < lim; ++j) {
            r += static_cast<double>(mpmDecBuf_[j]) * static_cast<double>(mpmDecBuf_[j + tau]);
        }
        mpmNsdfBuf_[tau] = (m > 1e-9) ? static_cast<float>((2.0 * r) / m) : 0.0f;
    }

    std::size_t pos = 1;
    while (pos < maxTau && mpmNsdfBuf_[pos] > 0.0f) ++pos;
    std::vector<std::size_t> peaks;
    peaks.reserve(32);
    int curBest = -1;
    for (; pos <= maxTau; ++pos) {
        const float v = mpmNsdfBuf_[pos];
        if (v > 0.0f) {
            if (v > mpmNsdfBuf_[pos - 1] && v >= mpmNsdfBuf_[pos + 1] &&
                (curBest < 0 || v > mpmNsdfBuf_[static_cast<std::size_t>(curBest)])) {
                curBest = static_cast<int>(pos);
            }
        } else if (curBest >= 0) {
            peaks.push_back(static_cast<std::size_t>(curBest));
            curBest = -1;
        }
    }
    if (curBest >= 0) peaks.push_back(static_cast<std::size_t>(curBest));

    float highest = 0.0f;
    for (std::size_t p : peaks) {
        if (p >= minTau && mpmNsdfBuf_[p] > highest) highest = mpmNsdfBuf_[p];
    }
    res.clarity = highest;
    if (highest < 0.50f) return res;

    const float thresh = highest * 0.93f;
    int chosen = -1;
    for (std::size_t p : peaks) {
        if (p >= minTau && mpmNsdfBuf_[p] >= thresh) {
            chosen = static_cast<int>(p);
            break;
        }
    }
    if (chosen < 0) return res;

    const std::size_t dbl = static_cast<std::size_t>(chosen) * 2;
    if (dbl >= minTau && dbl <= maxTau) {
        int bestDbl = -1;
        float bestDblVal = -1.0f;
        for (int d = -2; d <= 2; ++d) {
            const int idx = static_cast<int>(dbl) + d;
            if (idx >= static_cast<int>(minTau) && idx <= static_cast<int>(maxTau) &&
                mpmNsdfBuf_[static_cast<std::size_t>(idx)] > bestDblVal) {
                bestDblVal = mpmNsdfBuf_[static_cast<std::size_t>(idx)];
                bestDbl = idx;
            }
        }
        if (bestDbl > 0 && bestDblVal >= mpmNsdfBuf_[static_cast<std::size_t>(chosen)] + 0.045f && bestDblVal >= 0.86f) {
            chosen = bestDbl;
        }
    }

    const std::size_t cIdx = static_cast<std::size_t>(chosen);
    double refinedTau = static_cast<double>(cIdx);
    if (cIdx > 1 && cIdx + 1 <= maxTau) {
        const double y0 = static_cast<double>(mpmNsdfBuf_[cIdx - 1]);
        const double y1 = static_cast<double>(mpmNsdfBuf_[cIdx]);
        const double y2 = static_cast<double>(mpmNsdfBuf_[cIdx + 1]);
        const double denom = y0 - 2.0 * y1 + y2;
        if (std::abs(denom) > 1e-12) {
            const double delta = std::clamp(0.5 * (y0 - y2) / denom, -0.5, 0.5);
            refinedTau = static_cast<double>(cIdx) + delta;
        }
    }

    const double freqHz = effSr / refinedTau;
    res.clarity = mpmNsdfBuf_[cIdx];
    if (freqHz < minHz * 0.96 || freqHz > maxHz * 1.03) return res;

    res.valid = true;
    res.frequencyHz = freqHz;
    res.exactMidi = 69.0 + 12.0 * std::log2(freqHz / a4ReferenceHz_);
    res.midiNote = static_cast<int>(std::llround(res.exactMidi));
    res.centsOffset = (res.exactMidi - static_cast<double>(res.midiNote)) * 100.0;
    return res;
}

double CqtAnalyzer::refineExactMidi(const std::vector<float>& magnitudes,
                                    std::size_t binIdx) const noexcept {
    if (binIdx >= bins_.size() || binIdx >= magnitudes.size()) {
        return 0.0;
    }
    double refinedBin = static_cast<double>(binIdx);
    if (binIdx > 0 && binIdx + 1 < magnitudes.size()) {
        const double y0 = std::log(static_cast<double>(magnitudes[binIdx - 1]) + 1e-7);
        const double y1 = std::log(static_cast<double>(magnitudes[binIdx]) + 1e-7);
        const double y2 = std::log(static_cast<double>(magnitudes[binIdx + 1]) + 1e-7);
        const double denom = y0 - 2.0 * y1 + y2;
        if (std::abs(denom) > 1e-12 && y1 >= y0 && y1 >= y2) {
            const double delta = std::clamp(0.5 * (y0 - y2) / denom, -0.5, 0.5);
            refinedBin += delta;
        }
    }
    return static_cast<double>(minMidiNote_) + refinedBin / static_cast<double>(binsPerSemitone_);
}

double CqtAnalyzer::binCenterFreqHz(std::size_t binIdx) const noexcept {
    return (binIdx < bins_.size()) ? bins_[binIdx].freqHz : 0.0;
}

double CqtAnalyzer::binMidiFloat(std::size_t binIdx) const noexcept {
    return (binIdx < bins_.size()) ? bins_[binIdx].midiFloat : 0.0;
}

std::size_t CqtAnalyzer::midiToBinIndex(int midiNote) const noexcept {
    const int clamped = std::clamp(midiNote, minMidiNote_, maxMidiNote_);
    return static_cast<std::size_t>((clamped - minMidiNote_) * binsPerSemitone_);
}

double CqtAnalyzer::midiToFreqHz(double exactMidi) const noexcept {
    return a4ReferenceHz_ * std::pow(2.0, (exactMidi - 69.0) / 12.0);
}

} // namespace gitara::dsp
