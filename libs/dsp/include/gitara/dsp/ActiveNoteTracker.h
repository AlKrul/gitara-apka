#pragma once

#include "gitara/dsp/CqtAnalyzer.h"
#include "gitara/dsp/GuitarDspConfig.h"
#include "gitara/dsp/SpectralOnsetHps.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace gitara::dsp {

enum class NoteEventType : std::uint8_t {
    NoteOn = 0,
    NoteOff = 1
};

struct NoteEvent {
    NoteEventType type{NoteEventType::NoteOn};
    int midiNote{0};
    double exactMidi{0.0};
    double frequencyHz{0.0};
    double centsOffset{0.0};
    int velocity{100};
    double timestampMs{0.0};
    double durationMs{0.0};
    bool isRetrigger{false};
};

struct ActiveNote {
    int midiNote{0};
    double exactMidi{0.0};
    double frequencyHz{0.0};
    double centsOffset{0.0};
    float peakEnergy{0.0f};
    float currentEnergy{0.0f};
    double onsetTimeMs{0.0};
    double durationMs{0.0};
    double lastTriggerTimeMs{0.0};
    int velocity{100};
    int quietHopCount{0};
};

class ActiveNoteTracker {
public:
    void prepare(const GuitarDspConfig& config);
    void reset() noexcept;

    void processHop(const CqtAnalyzer& cqt,
                    const std::vector<float>& currentSpectrum,
                    const std::vector<float>& fluxSpectrum,
                    const std::vector<OnsetCandidate>& candidates,
                    const MpmFrameEstimate& mpmInfo,
                    double currentTimeMs,
                    std::vector<NoteEvent>& outEvents);

    [[nodiscard]] const std::vector<ActiveNote>& activeNotes() const noexcept { return activeNotes_; }
    [[nodiscard]] float noiseFloorLinear() const noexcept { return noiseFloorLinear_; }

private:
    struct PendingOnset {
        int midiNote{0};
        int hits{0};
        double exactMidi{0.0};
        double frequencyHz{0.0};
        double centsOffset{0.0};
        float fluxPeakEnergy{0.0f};
        float bandEnergy{0.0f};
        float confidence{0.0f};
    };

    [[nodiscard]] float readBandPeak(const CqtAnalyzer& cqt,
                                     const std::vector<float>& spec,
                                     int midiNote) const noexcept;
    [[nodiscard]] float readSustainEnergy(const CqtAnalyzer& cqt,
                                          const std::vector<float>& spec,
                                          int midiNote) const noexcept;
    [[nodiscard]] int energyToVelocity(float energy) const noexcept;

    GuitarDspConfig cfg_{};
    float releaseRatioLinear_{0.1f};
    float baseNoiseFloorLinear_{0.002f};
    float noiseFloorLinear_{0.002f};
    float retriggerBoostLinear_{1.78f};

    std::vector<ActiveNote> activeNotes_;
    std::vector<PendingOnset> pending_;
};

} // namespace gitara::dsp
