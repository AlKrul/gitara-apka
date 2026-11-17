#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace gitara::dsp {

/// Polifoniczny syntezator szarpanej struny gitarowej (6 głosów Karplus-Strong).
/// Służy do odtwarzania dźwięku po kliknięciu w dowolny próg na wizualizacji gryfu,
/// odgrywania interwałów/akordów w Kształceniu Słuchu oraz gry z wirtualnej klawiatury MIDI.
class KarplusStrongSynth {
public:
    explicit KarplusStrongSynth(double sampleRate = 48000.0);

    void prepare(double sampleRate);
    void allNotesOff() noexcept;

    /// Wzbudza strunę o zadanej częstotliwości (Hz) i sile uderzenia (velocity 0.0..1.0).
    void pluckFrequency(double frequencyHz, float velocity = 0.85f) noexcept;

    /// Wzbudza strunę dla numeru nuty MIDI (np. 40 = E2 .. 88 = E6).
    void noteOn(int midiNote, float velocity = 0.85f, double a4ReferenceHz = 440.0) noexcept;

    /// Tłumi głos odpowiadający podanej nucie MIDI.
    void noteOff(int midiNote) noexcept;

    /// Renderuje i sumuje dźwięk aktywnych strun do bufora wyjściowego. Zero alokacji w wątku audio.
    void renderNextBlock(float* outputBuffer, std::size_t numSamples) noexcept;

private:
    struct Voice {
        bool active{false};
        int midiNote{-1};
        std::vector<float> delayLine;
        std::size_t periodSamples{100};
        std::size_t writePos{0};
        float prevSample{0.0f};
        float damping{0.996f};
    };

    double sampleRate_{48000.0};
    std::uint32_t rngState_{0x12345678u};
    std::array<Voice, 6> voices_{};
    std::size_t nextVoiceIndex_{0};
};

} // namespace gitara::dsp
