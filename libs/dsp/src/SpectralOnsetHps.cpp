#include "gitara/dsp/SpectralOnsetHps.h"
#include "gitara/dsp/ActiveNoteTracker.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace gitara::dsp {

void SpectralOnsetHps::prepare(const GuitarDspConfig& config, const CqtAnalyzer& cqt) {
    cfg_ = config;
    binCount_ = cqt.binCount();

    const double hopMs = (cfg_.sampleRate > 0.0)
        ? (1000.0 * static_cast<double>(cfg_.hopSize) / cfg_.sampleRate)
        : 5.3333;
    lookbackHops_ = static_cast<std::size_t>(
        std::clamp(std::round(cfg_.fluxLookbackMs / std::max(1.0, hopMs)), 2.0, 24.0));

    const std::size_t histLen = lookbackHops_ + 6;
    spectrumHistory_.assign(histLen, std::vector<float>(binCount_, 0.0f));
    rmsHistory_.assign(histLen, 0.0f);
    deltaSpec_.assign(binCount_, 0.0f);
    workSpec_.assign(binCount_, 0.0f);
    workDelta_.assign(binCount_, 0.0f);
    hpsScores_.assign(binCount_, 0.0f);

    minFundBin_ = 0;
    maxFundBin_ = (binCount_ > 0) ? (binCount_ - 1) : 0;
    for (std::size_t k = 0; k < binCount_; ++k) {
        if (cqt.binCenterFreqHz(k) >= cfg_.fundamentalMinHz) {
            minFundBin_ = k;
            break;
        }
    }
    for (std::size_t k = binCount_; k > 0; --k) {
        if (cqt.binCenterFreqHz(k - 1) <= cfg_.fundamentalMaxHz) {
            maxFundBin_ = k - 1;
            break;
        }
    }

    const double binsPerOct = static_cast<double>(cqt.binsPerSemitone()) * 12.0;
    for (int h = 0; h < GuitarDspConfig::kMaxHarmonics; ++h) {
        harmonicBinOffsets_[static_cast<std::size_t>(h)] =
            static_cast<std::size_t>(std::llround(binsPerOct * std::log2(static_cast<double>(h + 1))));
    }

    reset();
}

void SpectralOnsetHps::reset() noexcept {
    for (auto& frame : spectrumHistory_) {
        std::fill(frame.begin(), frame.end(), 0.0f);
    }
    std::fill(rmsHistory_.begin(), rmsHistory_.end(), 0.0f);
    std::fill(deltaSpec_.begin(), deltaSpec_.end(), 0.0f);
    std::fill(workSpec_.begin(), workSpec_.end(), 0.0f);
    std::fill(workDelta_.begin(), workDelta_.end(), 0.0f);
    std::fill(hpsScores_.begin(), hpsScores_.end(), 0.0f);
    historyWriteIdx_ = 0;
    framesSeen_ = 0;
    totalFlux_ = 0.0f;
}

float SpectralOnsetHps::readBand(const std::vector<float>& spec, std::size_t binIdx) const noexcept {
    if (binIdx >= binCount_ || binIdx >= spec.size()) return 0.0f;
    float v = spec[binIdx];
    if (binIdx > 0 && spec[binIdx - 1] > v) v = spec[binIdx - 1];
    if (binIdx + 1 < binCount_ && spec[binIdx + 1] > v) v = spec[binIdx + 1];
    return v;
}

std::size_t SpectralOnsetHps::resolveOctaveBin(const CqtAnalyzer& cqt,
                                               const std::vector<float>& spec,
                                               const std::vector<float>& deltaSpec,
                                               std::size_t candidateBin) const noexcept {
    const std::size_t octBins = static_cast<std::size_t>(12 * cqt.binsPerSemitone());
    const std::size_t h3Off = harmonicBinOffsets_[2];
    const std::size_t h5Off = harmonicBinOffsets_[4];
    std::size_t bin = candidateBin;

    // 1. Kontrola oktawy niżej: wymagaj rzeczywistego piku na downBin (nie zbocza innej nuty akordu) + silnej h3
    if (bin >= octBins + minFundBin_) {
        const std::size_t downBin = bin - octBins;
        const float sCand = readBand(spec, bin);
        const float sDownCenter = spec[downBin];
        const bool downIsPeak = (sDownCenter > 0.0f) &&
            (downBin == 0 || sDownCenter >= spec[downBin - 1]) &&
            (downBin + 1 >= binCount_ || sDownCenter >= spec[downBin + 1]);
        if (downIsPeak) {
            const float sDown3 = (downBin + h3Off < binCount_) ? readBand(spec, downBin + h3Off) : 0.0f;
            const float dDown1 = deltaSpec[downBin];
            const float dDown3 = (downBin + h3Off < binCount_) ? readBand(deltaSpec, downBin + h3Off) : 0.0f;
            if (sDownCenter >= sCand * 0.25f && dDown1 >= sCand * 0.22f &&
                sDown3 >= sCand * 0.35f && dDown3 >= sCand * 0.32f) {
                bin = downBin;
            }
        }
    }

    // 2. Kontrola fałszywej sub-oktawy (brak nieparzystych harmonicznych h1, h3, h5)
    if (bin + octBins <= maxFundBin_) {
        const std::size_t upBin = bin + octBins;
        const float s1 = readBand(spec, bin);
        const float s2 = readBand(spec, upBin);
        const float s3 = (bin + h3Off < binCount_) ? readBand(spec, bin + h3Off) : 0.0f;
        const float s5 = (bin + h5Off < binCount_) ? readBand(spec, bin + h5Off) : 0.0f;
        const float oddSum = s1 + s3 + s5;
        if (s2 > cfg_.minFluxPeakLinear && s1 < s2 * 0.18f && s3 < s2 * 0.18f && oddSum < s2 * 0.34f) {
            bin = upBin;
        }
    }
    return bin;
}

void SpectralOnsetHps::classifyMpmMonophony(const CqtAnalyzer& cqt,
                                            const std::vector<float>& currentSpectrum,
                                            MpmFrameEstimate& mpmInfo) const noexcept {
    mpmInfo.isTrulyMonophonic = false;
    mpmInfo.isVirtualFund = false;
    if (!mpmInfo.valid || binCount_ == 0) return;

    const std::size_t mBin = cqt.midiToBinIndex(mpmInfo.midiNote);
    const std::size_t h2Off = harmonicBinOffsets_[1];
    const std::size_t h3Off = harmonicBinOffsets_[2];
    const std::size_t h5Off = harmonicBinOffsets_[4];

    const float s1 = readBand(currentSpectrum, mBin);
    const float s2 = (mBin + h2Off < binCount_) ? readBand(currentSpectrum, mBin + h2Off) : 0.0f;
    const float s3 = (mBin + h3Off < binCount_) ? readBand(currentSpectrum, mBin + h3Off) : 0.0f;
    const float s5 = (mBin + h5Off < binCount_) ? readBand(currentSpectrum, mBin + h5Off) : 0.0f;

    if (s2 > cfg_.minFluxPeakLinear && s3 > s2 * 0.30f && s1 < s2 * 0.04f && s5 < s3 * 0.14f) {
        mpmInfo.isVirtualFund = true;
        return;
    }

    std::vector<std::uint8_t> onHarm(binCount_, 0);
    const double bps = static_cast<double>(cqt.binsPerSemitone());
    for (int h = 1; h <= 8; ++h) {
        const std::size_t hk = mBin + static_cast<std::size_t>(std::llround(bps * 12.0 * std::log2(static_cast<double>(h))));
        if (hk >= binCount_) break;
        const std::size_t lo = (hk > 2) ? (hk - 2) : 0;
        const std::size_t hi = std::min(binCount_ - 1, hk + 2);
        for (std::size_t j = lo; j <= hi; ++j) onHarm[j] = 1;
    }

    float harmSum = 0.0f;
    float otherSum = 0.0f;
    const std::size_t scanHi = std::min(binCount_ - 2, maxFundBin_ + static_cast<std::size_t>(12 * cqt.binsPerSemitone()));
    for (std::size_t k = std::max<std::size_t>(1, minFundBin_); k <= scanHi; ++k) {
        const float v = currentSpectrum[k];
        if (v < cfg_.minFluxPeakLinear * 0.70f) continue;
        if (v >= currentSpectrum[k - 1] && v >= currentSpectrum[k + 1]) {
            if (onHarm[k]) harmSum += v;
            else otherSum += v;
        }
    }

    const float otherRatio = otherSum / (harmSum + 1e-6f);
    mpmInfo.isTrulyMonophonic = (otherRatio < 0.24f);
}

void SpectralOnsetHps::processFrame(const CqtAnalyzer& cqt,
                                    const std::vector<float>& currentSpectrum,
                                    MpmFrameEstimate& mpmInfo,
                                    const std::vector<ActiveNote>& activeNotes,
                                    std::vector<OnsetCandidate>& outCandidates) {
    outCandidates.clear();
    if (binCount_ == 0 || currentSpectrum.size() != binCount_ || spectrumHistory_.empty()) {
        return;
    }

    const std::size_t histLen = spectrumHistory_.size();
    const std::size_t effectiveBack = std::min(lookbackHops_, framesSeen_);
    const std::size_t back2 = std::min(lookbackHops_ + 1, framesSeen_);
    const std::size_t refIdx1 = (historyWriteIdx_ + histLen - std::max<std::size_t>(1, effectiveBack)) % histLen;
    const std::size_t refIdx2 = (historyWriteIdx_ + histLen - std::max<std::size_t>(1, back2)) % histLen;
    const auto& ref1 = spectrumHistory_[refIdx1];
    const auto& ref2 = spectrumHistory_[refIdx2];

    const std::size_t prevRmsIdx = (historyWriteIdx_ + histLen - std::max<std::size_t>(1, std::min<std::size_t>(3, framesSeen_))) % histLen;
    const float recentRms = (framesSeen_ > 0) ? rmsHistory_[prevRmsIdx] : 0.0f;
    const float curRms = mpmInfo.rms;
    const float shortRms = mpmInfo.shortRms;

    float sumFlux = 0.0f;
    float maxDelta = 0.0f;
    for (std::size_t k = 0; k < binCount_; ++k) {
        const float sVal = currentSpectrum[k];
        const float bg = (framesSeen_ == 0) ? 0.0f : std::max(ref1[k], ref2[k]);
        const float d = std::max(0.0f, sVal - cfg_.fluxSubtractFactor * bg);
        deltaSpec_[k] = d;
        workDelta_[k] = d;
        workSpec_[k] = sVal;
        if (k >= minFundBin_) {
            sumFlux += d;
            if (d > maxDelta) maxDelta = d;
        }
    }
    totalFlux_ = sumFlux;

    std::copy(currentSpectrum.begin(), currentSpectrum.end(), spectrumHistory_[historyWriteIdx_].begin());
    rmsHistory_[historyWriteIdx_] = shortRms;
    historyWriteIdx_ = (historyWriteIdx_ + 1) % histLen;
    ++framesSeen_;

    if (curRms < cfg_.minRmsGate || shortRms < cfg_.minRmsGate * 0.80f) return;
    if (recentRms > cfg_.minRmsGate * 3.0f && shortRms < recentRms * 0.38f) return;

    const bool mpmValid = mpmInfo.valid &&
        mpmInfo.midiNote >= cfg_.minMidiNote && mpmInfo.midiNote <= cfg_.maxMidiNote;
    if (mpmValid) {
        classifyMpmMonophony(cqt, currentSpectrum, mpmInfo);
    } else {
        mpmInfo.isTrulyMonophonic = false;
        mpmInfo.isVirtualFund = false;
    }
    const bool mpmStrong = mpmValid && !mpmInfo.isVirtualFund && mpmInfo.isTrulyMonophonic &&
        (mpmInfo.clarity >= cfg_.mpmClarityThreshold);
    const bool mpmAlreadyActive = mpmValid && std::any_of(activeNotes.begin(), activeNotes.end(),
        [&](const ActiveNote& n) { return n.midiNote == mpmInfo.midiNote; });

    // ŚCIEŻKA A (MONOFONICZNA / KROKI O PÓŁTON / BEZBŁĘDNE OKTAWY MPM)
    if (mpmStrong) {
        const std::size_t mBin = cqt.midiToBinIndex(mpmInfo.midiNote);
        const float rawBandE = readBand(currentSpectrum, mBin);
        const float rawH2 = (mBin + harmonicBinOffsets_[1] < binCount_) ? readBand(currentSpectrum, mBin + harmonicBinOffsets_[1]) : 0.0f;
        const float rawH3 = (mBin + harmonicBinOffsets_[2] < binCount_) ? readBand(currentSpectrum, mBin + harmonicBinOffsets_[2]) : 0.0f;
        const float effectiveBandE = std::max({rawBandE, 0.75f * rawH2, 0.65f * rawH3});

        float combFlux = 0.0f;
        for (int h = 0; h < GuitarDspConfig::kMaxHarmonics; ++h) {
            const std::size_t hk = mBin + harmonicBinOffsets_[static_cast<std::size_t>(h)];
            if (hk >= binCount_) break;
            const float dH = readBand(deltaSpec_, hk);
            if (dH > combFlux) combFlux = dH;
        }

        // Nowa nuta monofoniczna wymaga realnego przyrostu energii w grzebieniu harmonicznych (chroni przed ogonem po NoteOff!)
        const float minMonoFlux = cfg_.minFluxPeakLinear * 0.40f;
        const bool isCleanNewPitch = !mpmAlreadyActive && (combFlux >= minMonoFlux);
        const bool isRetriggerJump = mpmAlreadyActive && (combFlux >= cfg_.minFluxPeakLinear * 1.25f);

        if ((isCleanNewPitch || isRetriggerJump) && effectiveBandE >= cfg_.minFluxPeakLinear * 0.65f) {
            OnsetCandidate cand{};
            cand.midiNote = mpmInfo.midiNote;
            cand.exactMidi = mpmInfo.exactMidi;
            cand.frequencyHz = mpmInfo.frequencyHz;
            cand.centsOffset = mpmInfo.centsOffset;
            cand.fluxPeakEnergy = std::max(combFlux, cfg_.minFluxPeakLinear * 1.1f);
            cand.currentBandEnergy = effectiveBandE;
            cand.hpsScore = 2.0f + mpmInfo.clarity;
            cand.confidence = mpmInfo.clarity;
            cand.fromMpm = true;
            outCandidates.push_back(cand);
        }
        return;
    }

    // ŚCIEŻKA B (POLIFONICZNA: NAKŁADAJĄCE SIĘ STRUNY I AKORDY)
    if (maxDelta < cfg_.minFluxPeakLinear) return;

    const float alpha = cfg_.logCompressionAlpha;
    auto readHarm = [&](const std::vector<float>& arr, std::size_t baseBin, int hIdx) -> float {
        const std::size_t hk = baseBin + harmonicBinOffsets_[static_cast<std::size_t>(hIdx)];
        if (hk >= binCount_) return 0.0f;
        float v = arr[hk];
        if (hIdx >= 2) {
            if (hk + 1 < binCount_) v = std::max(v, 0.85f * arr[hk + 1]);
            if (hk > 0)             v = std::max(v, 0.70f * arr[hk - 1]);
        }
        return v;
    };

    auto isLocalPeak = [&](const std::vector<float>& arr, std::size_t k) -> bool {
        const float v = arr[k];
        if (v <= 0.0f) return false;
        const float l = (k > 0) ? arr[k - 1] : 0.0f;
        const float r = (k + 1 < binCount_) ? arr[k + 1] : 0.0f;
        return v >= l && v >= r && (v > l || v > r);
    };

    std::vector<std::uint8_t> suppressed(binCount_, 0);
    constexpr float kLobe[5] = {0.45f, 0.85f, 1.00f, 0.85f, 0.45f};
    constexpr float kHarmCap[GuitarDspConfig::kMaxHarmonics] = {1.10f, 0.88f, 0.78f, 0.56f, 0.38f, 0.25f};

    // Addytywne wstępne odjęcie harmonicznych h=2..6 już aktywnych strun (eliminuje fałszywe +12/+19 w akordach 3-strunowych!)
    for (const auto& an : activeNotes) {
        // Odejmujemy harmoniczne tylko świeżo uderzonych aktywnych strun (<= 80 ms), żeby nie tłumić re-triggeringu starej struny!
        if (an.durationMs > 80.0) continue;
        const std::size_t aBin = cqt.midiToBinIndex(an.midiNote);
        const float aFlux = readBand(deltaSpec_, aBin);
        if (aFlux < cfg_.minFluxPeakLinear * 0.45f) continue;
        for (int h = 1; h < GuitarDspConfig::kMaxHarmonics; ++h) {
            const std::size_t hk = aBin + harmonicBinOffsets_[static_cast<std::size_t>(h)];
            if (hk >= binCount_) break;
            const float subFlux = std::min(workDelta_[hk], aFlux * kHarmCap[h]);
            for (int d = -2; d <= 2; ++d) {
                const int jj = static_cast<int>(hk) + d;
                if (jj >= 0 && jj < static_cast<int>(binCount_)) {
                    workDelta_[static_cast<std::size_t>(jj)] =
                        std::max(0.0f, workDelta_[static_cast<std::size_t>(jj)] - subFlux * kLobe[d + 2]);
                }
            }
        }
    }

    float firstBestScore = 0.0f;

    for (int iter = 0; iter < cfg_.maxSimultaneousOnsets; ++iter) {
        std::fill(hpsScores_.begin(), hpsScores_.end(), 0.0f);

        for (std::size_t k = minFundBin_; k <= maxFundBin_; ++k) {
            if (suppressed[k]) continue;

            const std::size_t h2k = k + harmonicBinOffsets_[1];
            const bool peakAtFund = isLocalPeak(currentSpectrum, k);
            const bool truePeak = peakAtFund || (h2k < binCount_ && isLocalPeak(currentSpectrum, h2k));
            if (!truePeak) continue;
            if (!peakAtFund && k > 0 && currentSpectrum[k] < currentSpectrum[k - 1] * 0.72f) continue;

            float maxHarmFlux = 0.0f;
            float highHarmFlux = 0.0f;
            for (int h = 0; h < GuitarDspConfig::kMaxHarmonics; ++h) {
                const float dh = readHarm(workDelta_, k, h);
                if (dh > maxHarmFlux) maxHarmFlux = dh;
                if (h >= 2 && dh > highHarmFlux) highHarmFlux = dh;
            }
            if (maxHarmFlux < cfg_.minFluxPeakLinear * 0.80f) continue;

            float sumScore = 0.0f;
            float wSum = 0.0f;
            for (int h = 0; h < GuitarDspConfig::kMaxHarmonics; ++h) {
                const std::size_t hk = k + harmonicBinOffsets_[static_cast<std::size_t>(h)];
                if (hk >= binCount_) break;
                const float dh = readHarm(workDelta_, k, h);
                const float sh = readHarm(workSpec_, k, h);
                const float effVal = (h <= 1) ? std::max(dh, std::min(sh, highHarmFlux * 1.65f)) : dh;
                const float w = cfg_.harmonicWeights[static_cast<std::size_t>(h)];
                sumScore += w * std::log1p(alpha * effVal);
                wSum += w;
            }
            if (wSum > 0.0f) sumScore /= wSum;

            const float s1 = workSpec_[k];
            const float s2 = readHarm(workSpec_, k, 1);
            const float s3 = readHarm(workSpec_, k, 2);
            const float d1 = workDelta_[k];
            const float d2 = readHarm(workDelta_, k, 1);
            const float d3 = readHarm(workDelta_, k, 2);

            float subGuard = 1.0f;
            if (s1 < s2 * cfg_.subharmonicGuardRatio && s3 < s2 * 0.24f) {
                subGuard = 0.14f;
            } else if (!peakAtFund && s1 < s2 * 0.26f) {
                subGuard = 0.22f;
            } else if (d1 < cfg_.minFluxPeakLinear * 0.45f && d3 < cfg_.minFluxPeakLinear * 0.55f) {
                subGuard = 0.30f;
            }

            const float l1 = std::log1p(alpha * std::max(d1, std::min(s1, highHarmFlux * 1.15f)));
            const float l2 = std::log1p(alpha * std::max(d2, std::min(s2, highHarmFlux * 1.15f)));
            const float l3 = std::log1p(alpha * d3);
            const float prodBonus = cfg_.hpsProductBonus * std::sqrt(l1 * std::max(l2, 0.80f * l3));

            hpsScores_[k] = (sumScore + prodBonus) * subGuard;
        }

        std::size_t bestBin = binCount_;
        float bestScore = 0.0f;
        for (std::size_t k = minFundBin_; k <= maxFundBin_; ++k) {
            if (!suppressed[k] && hpsScores_[k] > bestScore) {
                bestScore = hpsScores_[k];
                bestBin = k;
            }
        }

        if (bestBin >= binCount_ || bestScore < cfg_.minFluxScore) break;
        if (iter == 0) firstBestScore = bestScore;
        else if (bestScore < firstBestScore * cfg_.secondaryNoteMinRatio) break;

        const std::size_t resolvedBin = resolveOctaveBin(cqt, workSpec_, workDelta_, bestBin);

        const std::size_t sLo = (resolvedBin > 2) ? (resolvedBin - 2) : 0;
        const std::size_t sHi = std::min(binCount_ - 1, resolvedBin + 2);
        for (std::size_t j = sLo; j <= sHi; ++j) suppressed[j] = 1;

        const float fluxAtFund = std::max({
            readBand(workDelta_, resolvedBin),
            0.75f * readHarm(workDelta_, resolvedBin, 1),
            0.65f * readHarm(workDelta_, resolvedBin, 2)
        });
        const float rawBandE = std::max({
            readBand(currentSpectrum, resolvedBin),
            0.70f * readHarm(currentSpectrum, resolvedBin, 1),
            0.60f * readHarm(currentSpectrum, resolvedBin, 2)
        });

        if (fluxAtFund < cfg_.minFluxPeakLinear) {
            workDelta_[resolvedBin] = 0.0f;
            continue;
        }

        const double exactMidi = cqt.refineExactMidi(currentSpectrum, resolvedBin);
        const int midiNote = static_cast<int>(std::llround(exactMidi));

        const bool duplicate = std::any_of(outCandidates.begin(), outCandidates.end(),
            [&](const OnsetCandidate& existing) { return existing.midiNote == midiNote; });
        if (!duplicate) {
            OnsetCandidate cand{};
            cand.midiNote = midiNote;
            cand.exactMidi = exactMidi;
            cand.frequencyHz = cqt.midiToFreqHz(exactMidi);
            cand.centsOffset = (exactMidi - static_cast<double>(midiNote)) * 100.0;
            cand.fluxPeakEnergy = fluxAtFund;
            cand.currentBandEnergy = rawBandE;
            cand.hpsScore = bestScore;
            cand.confidence = std::clamp(bestScore / (bestScore + 0.65f), 0.0f, 1.0f);
            cand.fromMpm = false;
            outCandidates.push_back(cand);
        }

        const float refFluxBase = std::max(workDelta_[resolvedBin], 0.72f * readHarm(workDelta_, resolvedBin, 1));
        const float refSpecBase = std::max(workSpec_[resolvedBin], 0.72f * readHarm(workSpec_, resolvedBin, 1));
        for (int h = 0; h < GuitarDspConfig::kMaxHarmonics; ++h) {
            const std::size_t hk = resolvedBin + harmonicBinOffsets_[static_cast<std::size_t>(h)];
            if (hk >= binCount_) break;
            const float subFluxAmp = (h == 0) ? workDelta_[hk] : std::min(workDelta_[hk] * cfg_.harmonicSubtractFactor, refFluxBase * kHarmCap[h]);
            const float subSpecAmp = (h == 0) ? (workSpec_[hk] * 0.95f) : std::min(workSpec_[hk] * 0.82f, refSpecBase * kHarmCap[h]);
            for (int d = -2; d <= 2; ++d) {
                const int jj = static_cast<int>(hk) + d;
                if (jj < 0 || jj >= static_cast<int>(binCount_)) continue;
                const float lw = kLobe[d + 2];
                workDelta_[static_cast<std::size_t>(jj)] =
                    std::max(0.0f, workDelta_[static_cast<std::size_t>(jj)] - subFluxAmp * lw);
                workSpec_[static_cast<std::size_t>(jj)] =
                    std::max(0.0f, workSpec_[static_cast<std::size_t>(jj)] - subSpecAmp * lw);
            }
        }
    }
}

} // namespace gitara::dsp
