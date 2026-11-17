#pragma once

#include "gitara/dsp/PitchDetector.h"
#include <cmath>

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)
#include <juce_gui_basics/juce_gui_basics.h>

namespace gitara::app::ui {

/// Komponent stroika gitarowego (Tuner) z dwoma trybami wizualizacji:
/// 1. Igłowy (Needle) – czytelny wskaźnik odchyłki w centach (-50 .. +50 ct),
/// 2. Stroboskopowy (Strobe) – przesuwające się pasy fazowe o najwyższej precyzji (do ustawiania menzury).
class TunerComponent final : public juce::Component {
public:
    enum class DisplayMode : std::uint8_t {
        NeedleAndStrobe = 0,
        NeedleOnly      = 1,
        StrobeOnly      = 2
    };

    void updatePitch(const gitara::dsp::PitchResult& result) {
        lastResult_ = result;
        if (result.pitched) {
            // Prędkość przesuwu pasów stroboskopu jest proporcjonalna do odchyłki w centach
            strobePhase_ += static_cast<float>(result.deviation.cents * 0.015);
            strobePhase_ = std::fmod(strobePhase_, 1.0f);
        }
        repaint();
    }

    void paint(juce::Graphics& g) override {
        auto area = getLocalBounds().toFloat().reduced(8.0f);
        g.setColour(juce::Colour(0xFF1C1815));
        g.fillRoundedRectangle(area, 8.0f);

        auto left = area.removeFromLeft(180.0f);
        g.setColour(juce::Colours::white);
        g.setFont(28.0f);

        if (lastResult_.pitched) {
            std::string noteText = lastResult_.deviation.closestPitch.toString(true);
            char centsBuf[64];
            std::snprintf(centsBuf, sizeof(centsBuf), "%s  (%+.1f ct, %.1f Hz)",
                          noteText.c_str(), lastResult_.deviation.cents, lastResult_.frequencyHz);
            g.drawText(centsBuf, left, juce::Justification::centred, false);
        } else {
            g.drawText("Stroik: zagraj strunę", left, juce::Justification::centred, false);
        }

        // Pasek stroboskopowy + znacznik igłowy
        auto bar = area.reduced(12.0f, 14.0f);
        g.setColour(juce::Colour(0xFF2A2420));
        g.fillRoundedRectangle(bar, 6.0f);

        if (lastResult_.pitched) {
            bool inTune = std::abs(lastResult_.deviation.cents) <= 2.0;
            g.setColour(inTune ? juce::Colour(0xFF2ECC71) : juce::Colour(0xFFF3903F));

            // Pasy stroboskopowe
            const int numStripes = 12;
            const float stripeW = bar.getWidth() / static_cast<float>(numStripes);
            for (int i = -1; i <= numStripes; ++i) {
                float x = bar.getX() + (static_cast<float>(i) + strobePhase_) * stripeW;
                auto stripeRect = juce::Rectangle<float>(x, bar.getY(), stripeW * 0.45f, bar.getHeight())
                                      .getIntersection(bar);
                if (!stripeRect.isEmpty()) {
                    g.fillRect(stripeRect);
                }
            }

            // Igła odchyłki (-50 .. +50 ct)
            float norm = std::clamp(static_cast<float>(lastResult_.deviation.cents) / 50.0f, -1.0f, 1.0f);
            float needleX = bar.getCentreX() + norm * (bar.getWidth() * 0.45f);
            g.setColour(juce::Colours::white);
            g.drawLine(needleX, bar.getY() - 4.0f, needleX, bar.getBottom() + 4.0f, 3.0f);
        }
    }

private:
    gitara::dsp::PitchResult lastResult_{};
    float strobePhase_{0.0f};
};

} // namespace gitara::app::ui
#endif
