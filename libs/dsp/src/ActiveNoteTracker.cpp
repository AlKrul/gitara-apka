#include "gitara/dsp/ActiveNoteTracker.h"
#include <algorithm>
#include <cmath>

namespace gitara::dsp {

void ActiveNoteTracker::prepare(const GuitarDspConfig& config) {
    cfg_ = config;
    releaseRatioLinear_ = std::pow(10.0f, std::min(-3.0f, cfg_.releaseDropDb) / 20.0f);
    baseNoiseFloorLinear_ = std::pow(10.0f, cfg_.noiseFloorDb / 20.0f);
    noiseFloorLinear_ = baseNoiseFloorLinear_;
    retriggerBoostLinear_ = std::pow(10.0f, std::max(1.0f, cfg_.retriggerFluxBoostDb) / 20.0f);
    reset();
}

void ActiveNoteTracker::reset() noexcept {
    activeNotes_.clear();
    pending_.clear();
    noiseFloorLinear_ = baseNoiseFloorLinear_;
}

float ActiveNoteTracker::readBandPeak(const CqtAnalyzer& cqt,
                                      const std::vector<float>& spec,
                                      int midiNote) const noexcept {
    if (spec.empty()) return 0.0f;
    const std::size_t center = cqt.midiToBinIndex(midiNote);
    const std::size_t lo = (center > 0) ? (center - 1) : 0;
    const std::size_t hi = std::min(spec.size() - 1, center + 1);
    float peak = 0.0f;
    for (std::size_t k = lo; k <= hi; ++k) {
        if (spec[k] > peak) peak = spec[k];
    }
    return peak;
}

float ActiveNoteTracker::readSustainEnergy(const CqtAnalyzer& cqt,
                                           const std::vector<float>& spec,
                                           int midiNote) const noexcept {
    const float e1 = readBandPeak(cqt, spec, midiNote);
    const float e2 = (midiNote + 12 <= cqt.maxMidiNote()) ? readBandPeak(cqt, spec, midiNote + 12) : 0.0f;
    const float e3 = (midiNote + 19 <= cqt.maxMidiNote()) ? readBandPeak(cqt, spec, midiNote + 19) : 0.0f;
    return std::max({e1, 0.55f * e2, 0.60f * e3});
}

int ActiveNoteTracker::energyToVelocity(float energy) const noexcept {
    const float db = 20.0f * std::log10(std::max(1e-6f, energy));
    const float norm = std::clamp((db + 52.0f) / 44.0f, 0.0f, 1.0f);
    return std::clamp(static_cast<int>(std::lround(18.0f + norm * 109.0f)), 1, 127);
}

void ActiveNoteTracker::processHop(const CqtAnalyzer& cqt,
                                   const std::vector<float>& currentSpectrum,
                                   const std::vector<float>& fluxSpectrum,
                                   const std::vector<OnsetCandidate>& candidates,
                                   const MpmFrameEstimate& mpmInfo,
                                   double currentTimeMs,
                                   std::vector<NoteEvent>& outEvents) {
    const float curRms = mpmInfo.rms;
    const float shortRms = mpmInfo.shortRms;

    // 1. Adaptacja progu szumu tła gdy nic nie gra
    if (activeNotes_.empty() && !currentSpectrum.empty()) {
        float sum = 0.0f;
        for (float v : currentSpectrum) sum += v;
        const float avg = sum / static_cast<float>(currentSpectrum.size());
        const float targetFloor = std::max(baseNoiseFloorLinear_, avg * 2.2f);
        noiseFloorLinear_ = 0.94f * noiseFloorLinear_ + 0.06f * targetFloor;
    }

    // 2. SUSTAIN & NOTE OFF na surowym widmie S(t)
    const bool mpmPureSingle = mpmInfo.valid && mpmInfo.isTrulyMonophonic &&
        mpmInfo.clarity >= 0.91f && curRms >= cfg_.minRmsGate;
    std::sort(activeNotes_.begin(), activeNotes_.end(),
              [](const ActiveNote& a, const ActiveNote& b) { return a.midiNote < b.midiNote; });

    for (std::size_t i = 0; i < activeNotes_.size();) {
        ActiveNote& note = activeNotes_[i];
        const float rawEnergy = readSustainEnergy(cqt, currentSpectrum, note.midiNote);

        float bleed = 0.0f;
        for (std::size_t j = 0; j < i; ++j) {
            const int diff = note.midiNote - activeNotes_[j].midiNote;
            if (diff == 12) bleed += cfg_.lowerHarmonicBleed * activeNotes_[j].currentEnergy;
            else if (diff == 19) bleed += (cfg_.lowerHarmonicBleed * 0.65f) * activeNotes_[j].currentEnergy;
        }
        const float effectiveEnergy = std::max(0.0f, rawEnergy - bleed);
        note.currentEnergy = effectiveEnergy;
        note.durationMs = std::max(0.0, currentTimeMs - note.onsetTimeMs);

        if ((currentTimeMs - note.lastTriggerTimeMs) <= cfg_.attackPeakWindowMs &&
            effectiveEnergy > note.peakEnergy) {
            note.peakEnergy = effectiveEnergy;
        }

        if (mpmInfo.valid && mpmInfo.clarity >= 0.82f && mpmInfo.midiNote == note.midiNote) {
            note.exactMidi = 0.55 * note.exactMidi + 0.45 * mpmInfo.exactMidi;
            note.frequencyHz = cqt.midiToFreqHz(note.exactMidi);
            note.centsOffset = (note.exactMidi - static_cast<double>(note.midiNote)) * 100.0;
        } else if (effectiveEnergy > noiseFloorLinear_ * 1.5f) {
            const std::size_t cBin = cqt.midiToBinIndex(note.midiNote);
            const double refined = cqt.refineExactMidi(currentSpectrum, cBin);
            if (std::abs(refined - static_cast<double>(note.midiNote)) <= 0.65) {
                note.exactMidi = 0.75 * note.exactMidi + 0.25 * refined;
                note.frequencyHz = cqt.midiToFreqHz(note.exactMidi);
                note.centsOffset = (note.exactMidi - static_cast<double>(note.midiNote)) * 100.0;
            }
        }

        const bool replacedByPureMono = mpmPureSingle && mpmInfo.midiNote != note.midiNote &&
            (std::abs(mpmInfo.midiNote - note.midiNote) <= 2 || effectiveEnergy < note.peakEnergy * 0.35f);

        const bool hasBeatingNeighborSustain = std::any_of(activeNotes_.begin(), activeNotes_.end(),
            [&](const ActiveNote& n) { return n.midiNote != note.midiNote && std::abs(n.midiNote - note.midiNote) <= 2; });
        const int releaseHopsNeeded = hasBeatingNeighborSustain
            ? std::max(10, cfg_.releaseConfirmHops * 3)
            : std::max(1, cfg_.releaseConfirmHops);

        const float releaseThreshold = std::max(noiseFloorLinear_, note.peakEnergy * releaseRatioLinear_);
        if (effectiveEnergy < releaseThreshold || shortRms < cfg_.minRmsGate * 0.70f || replacedByPureMono) {
            ++note.quietHopCount;
        } else {
            note.quietHopCount = 0;
        }

        if (note.quietHopCount >= releaseHopsNeeded) {
            NoteEvent offEv{};
            offEv.type = NoteEventType::NoteOff;
            offEv.midiNote = note.midiNote;
            offEv.exactMidi = note.exactMidi;
            offEv.frequencyHz = note.frequencyHz;
            offEv.centsOffset = note.centsOffset;
            offEv.velocity = 0;
            offEv.timestampMs = currentTimeMs;
            offEv.durationMs = note.durationMs;
            offEv.isRetrigger = false;
            outEvents.push_back(offEv);
            activeNotes_.erase(activeNotes_.begin() + static_cast<std::ptrdiff_t>(i));
        } else {
            ++i;
        }
    }

    // 3. POTWIERDZANIE ATAKÓW + NOTE ON + RE-TRIGGERING
    std::vector<PendingOnset> nextPending;
    nextPending.reserve(candidates.size());

    for (const auto& cand : candidates) {
        const PendingOnset* prev = nullptr;
        for (const auto& p : pending_) {
            if (p.midiNote == cand.midiNote) {
                prev = &p;
                break;
            }
        }
        const bool nearBoundary = !cand.fromMpm && std::abs(cand.centsOffset) > 35.0;
        const double pitchDrift = prev ? std::abs(cand.exactMidi - prev->exactMidi) : 0.0;
        const bool transientUnsettled = prev && nearBoundary &&
            (pitchDrift > 0.10 || cand.fluxPeakEnergy > prev->fluxPeakEnergy * 1.20f);
        const int hits = transientUnsettled
            ? std::min(prev->hits, std::max(1, cfg_.polyOnsetConfirmHops - 1))
            : ((prev ? prev->hits : 0) + 1);

        bool lowerFundRising = false;
        if (!cand.fromMpm) {
            auto isStrongFluxPeakAt = [&](int m) -> bool {
                if (m < cfg_.minMidiNote || fluxSpectrum.empty()) return false;
                const bool alreadyActive = std::any_of(activeNotes_.begin(), activeNotes_.end(),
                    [&](const ActiveNote& n) { return n.midiNote == m; });
                if (alreadyActive) return false;
                const std::size_t b = cqt.midiToBinIndex(m);
                if (b >= fluxSpectrum.size() || b >= currentSpectrum.size()) return false;
                const float v = readBandPeak(cqt, fluxSpectrum, m);
                if (v < cfg_.minFluxPeakLinear * 0.65f || v < cand.fluxPeakEnergy * 0.14f) return false;
                const float sCenter = currentSpectrum[b];
                const float sL = (b > 0) ? currentSpectrum[b - 1] : 0.0f;
                const float sR = (b + 1 < currentSpectrum.size()) ? currentSpectrum[b + 1] : 0.0f;
                return sCenter >= sL * 0.90f && sCenter >= sR * 0.90f;
            };
            lowerFundRising = isStrongFluxPeakAt(cand.midiNote - 12) || isStrongFluxPeakAt(cand.midiNote - 19);
        }

        const bool isSubOctaveDip = cand.fromMpm && cand.confidence < 0.90f &&
            std::any_of(activeNotes_.begin(), activeNotes_.end(),
                [&](const ActiveNote& n) { return n.midiNote == cand.midiNote + 12; });
        const int baseConfirm = cand.fromMpm
            ? (std::max(1, cfg_.onsetConfirmHops) + (isSubOctaveDip ? 2 : 0))
            : std::max(3, cfg_.polyOnsetConfirmHops);
        const int requiredHops = baseConfirm + (lowerFundRising ? 4 : 0);

        if (hits < requiredHops) {
            PendingOnset po{};
            po.midiNote = cand.midiNote;
            po.hits = hits;
            po.exactMidi = cand.exactMidi;
            po.frequencyHz = cand.frequencyHz;
            po.centsOffset = cand.centsOffset;
            po.fluxPeakEnergy = cand.fluxPeakEnergy;
            po.bandEnergy = cand.currentBandEnergy;
            po.confidence = cand.confidence;
            nextPending.push_back(po);
            continue;
        }

        auto itActive = std::find_if(activeNotes_.begin(), activeNotes_.end(),
                                     [&](const ActiveNote& n) { return n.midiNote == cand.midiNote; });

        if (itActive != activeNotes_.end()) {
            const double sinceLastMs = currentTimeMs - itActive->lastTriggerTimeMs;
            const float fluxBand = std::max(readBandPeak(cqt, fluxSpectrum, cand.midiNote), cand.fluxPeakEnergy);
            const bool hasBeatingNeighbor = std::any_of(activeNotes_.begin(), activeNotes_.end(),
                [&](const ActiveNote& n) { return n.midiNote != itActive->midiNote && std::abs(n.midiNote - itActive->midiNote) == 1; });
            const bool strongFluxJump = hasBeatingNeighbor
                ? (cand.fromMpm && fluxBand >= itActive->peakEnergy * 0.55f && cand.currentBandEnergy >= itActive->peakEnergy * 1.15f)
                : ((fluxBand >= cfg_.minFluxPeakLinear * 1.30f) &&
                   (fluxBand >= itActive->currentEnergy * cfg_.retriggerMinFluxRatio ||
                    cand.currentBandEnergy >= itActive->currentEnergy * retriggerBoostLinear_));

            if (sinceLastMs >= cfg_.retriggerCooldownMs && strongFluxJump) {
                NoteEvent offEv{};
                offEv.type = NoteEventType::NoteOff;
                offEv.midiNote = itActive->midiNote;
                offEv.exactMidi = itActive->exactMidi;
                offEv.frequencyHz = itActive->frequencyHz;
                offEv.centsOffset = itActive->centsOffset;
                offEv.velocity = 0;
                offEv.timestampMs = currentTimeMs;
                offEv.durationMs = std::max(0.0, currentTimeMs - itActive->onsetTimeMs);
                offEv.isRetrigger = true;
                outEvents.push_back(offEv);

                const float newPeak = std::max(cand.currentBandEnergy, cand.fluxPeakEnergy);
                itActive->exactMidi = cand.exactMidi;
                itActive->frequencyHz = cand.frequencyHz;
                itActive->centsOffset = cand.centsOffset;
                itActive->peakEnergy = newPeak;
                itActive->currentEnergy = newPeak;
                itActive->onsetTimeMs = currentTimeMs;
                itActive->lastTriggerTimeMs = currentTimeMs;
                itActive->durationMs = 0.0;
                itActive->velocity = energyToVelocity(newPeak);
                itActive->quietHopCount = 0;

                NoteEvent onEv{};
                onEv.type = NoteEventType::NoteOn;
                onEv.midiNote = itActive->midiNote;
                onEv.exactMidi = itActive->exactMidi;
                onEv.frequencyHz = itActive->frequencyHz;
                onEv.centsOffset = itActive->centsOffset;
                onEv.velocity = itActive->velocity;
                onEv.timestampMs = currentTimeMs;
                onEv.durationMs = 0.0;
                onEv.isRetrigger = true;
                outEvents.push_back(onEv);
            } else {
                PendingOnset po{};
                po.midiNote = cand.midiNote;
                po.hits = hits;
                po.exactMidi = cand.exactMidi;
                po.frequencyHz = cand.frequencyHz;
                po.centsOffset = cand.centsOffset;
                po.fluxPeakEnergy = cand.fluxPeakEnergy;
                po.bandEnergy = cand.currentBandEnergy;
                po.confidence = cand.confidence;
                nextPending.push_back(po);
            }
            continue;
        }

        bool isFreshHarmonicGhost = false;
        if (!cand.fromMpm) {
            for (const auto& other : activeNotes_) {
                const int diff = cand.midiNote - other.midiNote;
                const double ageMs = currentTimeMs - other.lastTriggerTimeMs;
                if (diff == 24 || diff == 28) {
                    if (cand.fluxPeakEnergy < other.peakEnergy * 0.65f) {
                        isFreshHarmonicGhost = true;
                        break;
                    }
                } else if (diff == 12 || diff == 19) {
                    if (ageMs <= 60.0 && cand.fluxPeakEnergy < other.peakEnergy * 0.34f) {
                        isFreshHarmonicGhost = true;
                        break;
                    }
                    if (ageMs > 60.0 && cand.fluxPeakEnergy < other.currentEnergy * 0.45f) {
                        isFreshHarmonicGhost = true;
                        break;
                    }
                }
            }
        }
        if (isFreshHarmonicGhost) continue;

        // Usuń ewentualny szybszy prekursor harmoniczny (+12 / +19 półt.) z ostatnich 45 ms tego samego ataku
        for (std::size_t pIdx = activeNotes_.size(); pIdx > 0; --pIdx) {
            const ActiveNote& up = activeNotes_[pIdx - 1];
            const int diffUp = up.midiNote - cand.midiNote;
            if ((diffUp == 12 || diffUp == 19) && (currentTimeMs - up.lastTriggerTimeMs) <= 45.0) {
                NoteEvent preOff{};
                preOff.type = NoteEventType::NoteOff;
                preOff.midiNote = up.midiNote;
                preOff.exactMidi = up.exactMidi;
                preOff.frequencyHz = up.frequencyHz;
                preOff.centsOffset = up.centsOffset;
                preOff.velocity = 0;
                preOff.timestampMs = currentTimeMs;
                preOff.durationMs = up.durationMs;
                preOff.isRetrigger = false;
                outEvents.push_back(preOff);
                activeNotes_.erase(activeNotes_.begin() + static_cast<std::ptrdiff_t>(pIdx - 1));
            }
        }

        if (activeNotes_.size() >= cfg_.maxActiveNotes) {
            auto quietest = std::min_element(activeNotes_.begin(), activeNotes_.end(),
                                             [](const ActiveNote& a, const ActiveNote& b) {
                                                 return a.currentEnergy < b.currentEnergy;
                                             });
            if (quietest != activeNotes_.end()) {
                NoteEvent evictOff{};
                evictOff.type = NoteEventType::NoteOff;
                evictOff.midiNote = quietest->midiNote;
                evictOff.exactMidi = quietest->exactMidi;
                evictOff.frequencyHz = quietest->frequencyHz;
                evictOff.centsOffset = quietest->centsOffset;
                evictOff.velocity = 0;
                evictOff.timestampMs = currentTimeMs;
                evictOff.durationMs = quietest->durationMs;
                outEvents.push_back(evictOff);
                activeNotes_.erase(quietest);
            }
        }

        const float initPeak = std::max(cand.currentBandEnergy, cand.fluxPeakEnergy);
        ActiveNote active{};
        active.midiNote = cand.midiNote;
        active.exactMidi = cand.exactMidi;
        active.frequencyHz = cand.frequencyHz;
        active.centsOffset = cand.centsOffset;
        active.peakEnergy = initPeak;
        active.currentEnergy = initPeak;
        active.onsetTimeMs = currentTimeMs;
        active.lastTriggerTimeMs = currentTimeMs;
        active.durationMs = 0.0;
        active.velocity = energyToVelocity(initPeak);
        active.quietHopCount = 0;
        activeNotes_.push_back(active);

        NoteEvent onEv{};
        onEv.type = NoteEventType::NoteOn;
        onEv.midiNote = active.midiNote;
        onEv.exactMidi = active.exactMidi;
        onEv.frequencyHz = active.frequencyHz;
        onEv.centsOffset = active.centsOffset;
        onEv.velocity = active.velocity;
        onEv.timestampMs = currentTimeMs;
        onEv.durationMs = 0.0;
        onEv.isRetrigger = false;
        outEvents.push_back(onEv);
    }

    pending_ = std::move(nextPending);
}

} // namespace gitara::dsp
