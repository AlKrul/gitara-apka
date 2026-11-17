#pragma once

#include <cmath>
#include <cstddef>

namespace gitara::dsp {

/// Kondycjoner sygnału wejściowego (z interfejsu Hotone Ampero One lub mikrofonu):
/// 1. Filtr górnoprzepustowy 2. rzędu (HPF ~60 Hz) usuwający dudnienie podniskotonowe,
/// 2. Filtr wycinający przydźwięk sieciowy (Notch 50 Hz),
/// 3. Bramka szumów (Noise Gate) z histerezą.
/// W pełni deterministyczny, zero alokacji w wątku audio.
class SignalConditioner {
public:
    explicit SignalConditioner(double sampleRate = 48000.0,
                               double hpfCutoffHz = 60.0,
                               double notchFreqHz = 50.0,
                               float gateThresholdDb = -55.0f);

    void prepare(double sampleRate,
                 double hpfCutoffHz = 60.0,
                 double notchFreqHz = 50.0,
                 float gateThresholdDb = -55.0f) noexcept;

    void reset() noexcept;

    /// Przetwarza bufor próbek w miejscu i zwraca wartość RMS bloku (przed bramką).
    float processBlock(float* samples, std::size_t numSamples) noexcept;

    /// Zwraca true, jeśli bramka szumów jest aktualnie otwarta (sygnał powyżej progu).
    [[nodiscard]] bool isGateOpen() const noexcept { return gateOpen_; }

private:
    struct Biquad {
        float b0{1.0f}, b1{0.0f}, b2{0.0f};
        float a1{0.0f}, a2{0.0f};
        float z1{0.0f}, z2{0.0f};

        void reset() noexcept { z1 = 0.0f; z2 = 0.0f; }

        float processSample(float x) noexcept {
            float y = b0 * x + z1;
            z1 = b1 * x - a1 * y + z2;
            z2 = b2 * x - a2 * y;
            return y;
        }
    };

    Biquad hpf_{};
    Biquad notch_{};
    float openThresholdLinear_{0.0018f};
    float closeThresholdLinear_{0.0010f};
    bool gateOpen_{false};
};

} // namespace gitara::dsp
