#pragma once

#include <cstddef>
#include <vector>

namespace gitara::dsp {

/// Dwubiegunowa sekcja filtru Biquad (Direct Form II Transposed — stabilna numerycznie w float/double).
struct BiquadSection {
    double b0{1.0}, b1{0.0}, b2{0.0};
    double a1{0.0}, a2{0.0};
    double z1{0.0}, z2{0.0};

    void reset() noexcept {
        z1 = 0.0;
        z2 = 0.0;
    }

    [[nodiscard]] float processSample(float x) noexcept {
        const double xd = static_cast<double>(x);
        const double y = b0 * xd + z1;
        z1 = b1 * xd - a1 * y + z2;
        z2 = b2 * xd - a2 * y;
        return static_cast<float>(y);
    }
};

/// Etap 1 potoku DSP: Filtr pasmowoprzepustowy Butterwortha 4. rzędu (2× HPF + 2× LPF)
/// oraz opcjonalny filtr wycinający przydźwięk sieciowy (Notch 50/60 Hz).
/// Zero alokacji dynamicznych w ścieżce audio.
class BandpassPreFilter {
public:
    void prepare(double sampleRate,
                 double lowCutoffHz,
                 double highCutoffHz,
                 double mainsNotchHz = 50.0) noexcept;

    void reset() noexcept;

    [[nodiscard]] float processSample(float x) noexcept;
    void processBlock(const float* input, float* output, std::size_t numSamples) noexcept;

private:
    static BiquadSection makeHighpass(double sampleRate, double cutoffHz, double q) noexcept;
    static BiquadSection makeLowpass(double sampleRate, double cutoffHz, double q) noexcept;
    static BiquadSection makeNotch(double sampleRate, double centerHz, double q) noexcept;

    BiquadSection hp1_{};
    BiquadSection hp2_{};
    BiquadSection lp1_{};
    BiquadSection lp2_{};
    BiquadSection notch_{};
    bool notchEnabled_{false};
};

/// Bufor kołowy (Ring Buffer) dla ramek audio czasu rzeczywistego.
/// Pozwala bez alokacji odkładać pojedyncze próbki i wyciągać najnowsze N próbek
/// wyrównane przyczynowo (causal) do chwili t_now.
class AudioRingBuffer {
public:
    void prepare(std::size_t capacity);
    void reset() noexcept;

    void pushSample(float sample) noexcept;
    void pushBlock(const float* samples, std::size_t numSamples) noexcept;

    /// Kopiuje `length` najnowszych próbek (chronologicznie: najstarsza -> najnowsza) do `dest`.
    void copyLatest(float* dest, std::size_t length) const noexcept;

    [[nodiscard]] std::size_t capacity() const noexcept { return buffer_.size(); }
    [[nodiscard]] std::size_t totalSamplesWritten() const noexcept { return totalWritten_; }

private:
    std::vector<float> buffer_;
    std::size_t writePos_{0};
    std::size_t totalWritten_{0};
};

} // namespace gitara::dsp
