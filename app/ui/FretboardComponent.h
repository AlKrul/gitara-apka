#pragma once

#include "gitara/theory/Fretboard.h"
#include <functional>
#include <optional>
#include <vector>

#if __has_include(<juce_gui_basics/juce_gui_basics.h>)
#include <juce_gui_basics/juce_gui_basics.h>

namespace gitara::app::ui {

/// Interaktywny komponent gryfu (22/24 progi) w stylu ciemnego motywu:
/// - wyświetla dźwięki (C D E F G A B, bez H) lub interwały (1, ♭3, 5...) względem toniki,
/// - obsługuje przełącznik znaków chromatycznych: Auto (wg teorii) / Bemole (♭) / Krzyżyki (♯),
/// - kliknięcie lewym przyciskiem myszy odtwarza dźwięk struny (Karplus-Strong),
/// - przeciągnięcie prawym przyciskiem myszy maskuje / odmaskowuje obszar gryfu (FretboardMask),
/// - podświetla wykryte na żywo dźwięki (wyróżniając pozycję najbliższą poprzedniej pozycji dłoni).
class FretboardComponent final : public juce::Component {
public:
    enum class LabelType : std::uint8_t {
        Notes     = 0,
        Intervals = 1
    };

    explicit FretboardComponent(gitara::theory::Fretboard fretboard = gitara::theory::Fretboard{})
        : fretboard_(std::move(fretboard)),
          mask_(fretboard_.stringCount(), fretboard_.fretCount()) {
        if (const auto* maj = gitara::theory::findScaleDefinition("major")) {
            activeScale_ = gitara::theory::Scale{
                gitara::theory::Note{gitara::theory::Letter::C, gitara::theory::Accidental::Natural},
                maj
            };
        }
    }

    void setScale(gitara::theory::Scale scale) {
        activeScale_ = scale;
        repaint();
    }

    void setAccidentalMode(gitara::theory::AccidentalMode mode) {
        accidentalMode_ = mode;
        repaint();
    }

    void setLabelType(LabelType type) {
        labelType_ = type;
        repaint();
    }

    void setDetectedMidiNote(std::optional<int> midiNote) {
        detectedMidi_ = midiNote;
        if (midiNote.has_value()) {
            primaryDetectedPos_ = fretboard_.selectBestPosition(*midiNote, primaryDetectedPos_, &mask_);
        }
        repaint();
    }

    std::function<void(int midiNote, double frequencyHz)> onNoteClicked;

    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat().reduced(16.0f);
        g.fillAll(juce::Colour(0xFF14110F));

        const int nStrings = fretboard_.stringCount();
        const int nFrets = fretboard_.fretCount();
        const float fretW = bounds.getWidth() / static_cast<float>(nFrets + 1);
        const float stringH = bounds.getHeight() / static_cast<float>(nStrings);

        // Rysuj progi i struny
        for (int f = 0; f <= nFrets; ++f) {
            float x = bounds.getX() + static_cast<float>(f + 1) * fretW;
            g.setColour(f == 0 ? juce::Colour(0xFFD6C7B2) : juce::Colour(0xFF3E352F));
            g.drawLine(x, bounds.getY(), x, bounds.getBottom(), f == 0 ? 3.0f : 1.2f);
        }

        for (int s = 0; s < nStrings; ++s) {
            int row = nStrings - 1 - s; // Góra = E4, dół = E2
            float y = bounds.getY() + (static_cast<float>(row) + 0.5f) * stringH;
            g.setColour(juce::Colour(0xFF6E6259));
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f + static_cast<float>(nStrings - 1 - s) * 0.25f);
        }

        // Rysuj węzły skali, maskę oraz wykryty dźwięk
        for (int s = 0; s < nStrings; ++s) {
            int row = nStrings - 1 - s;
            float cy = bounds.getY() + (static_cast<float>(row) + 0.5f) * stringH;

            for (int f = 0; f <= nFrets; ++f) {
                float cx = bounds.getX() + (static_cast<float>(f) + 0.5f) * fretW;
                bool allowed = mask_.isAllowed(s, f);
                if (!allowed) {
                    g.setColour(juce::Colour(0x88000000));
                    g.fillRect(cx - fretW * 0.48f, cy - stringH * 0.48f, fretW * 0.96f, stringH * 0.96f);
                    continue;
                }

                int midi = fretboard_.midiAt(s, f);
                bool inScale = activeScale_.containsPitchClass(midi % 12);
                bool isRoot = (midi % 12) == activeScale_.root.pitchClass();
                bool isDetected = detectedMidi_.has_value() && (*detectedMidi_ == midi);
                bool isPrimaryDetected = isDetected && primaryDetectedPos_.has_value() &&
                                         primaryDetectedPos_->stringIndex == s &&
                                         primaryDetectedPos_->fret == f;

                if (!inScale && !isDetected) {
                    continue;
                }

                float radius = std::min(fretW, stringH) * 0.36f;
                if (isPrimaryDetected) {
                    g.setColour(juce::Colour(0xFF2ECC71)); // Zielony dla głównej rozpoznanej pozycji
                } else if (isDetected) {
                    g.setColour(juce::Colour(0x882ECC71)); // Przygaszony zielony dla alternatywnych pozycji
                } else if (isRoot) {
                    g.setColour(juce::Colour(0xFFF3903F)); // Pomarańczowy dla toniki
                } else {
                    g.setColour(juce::Colour(0xFF4A3525));
                }

                g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
                g.setColour(juce::Colours::white);

                std::string label;
                if (labelType_ == LabelType::Intervals) {
                    if (auto iv = activeScale_.intervalForPitchClass(midi % 12)) {
                        label = iv->symbol(true);
                    }
                } else {
                    auto p = fretboard_.pitchAt(s, f, accidentalMode_, &activeScale_);
                    label = p.note.toString(true);
                }

                g.drawText(label,
                           juce::Rectangle<float>(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f),
                           juce::Justification::centred,
                           false);
            }
        }
    }

    void mouseDown(const juce::MouseEvent& e) override {
        auto bounds = getLocalBounds().toFloat().reduced(16.0f);
        const int nStrings = fretboard_.stringCount();
        const int nFrets = fretboard_.fretCount();
        int f = static_cast<int>((e.position.x - bounds.getX()) /
                                 (bounds.getWidth() / static_cast<float>(nFrets + 1)));
        int row = static_cast<int>((e.position.y - bounds.getY()) /
                                   (bounds.getHeight() / static_cast<float>(nStrings)));
        int s = nStrings - 1 - row;
        if (s < 0 || s >= nStrings || f < 0 || f > nFrets) {
            return;
        }

        if (e.mods.isRightButtonDown()) {
            // Prawy przycisk myszy przełącza maskę pola gryfu
            mask_.setAllowed(s, f, !mask_.isAllowed(s, f));
            repaint();
            return;
        }

        if (onNoteClicked) {
            int midi = fretboard_.midiAt(s, f);
            auto pitch = fretboard_.pitchAt(s, f, accidentalMode_, &activeScale_);
            onNoteClicked(midi, pitch.frequencyHz());
        }
    }

    [[nodiscard]] gitara::theory::FretboardMask& mask() noexcept { return mask_; }
    [[nodiscard]] const gitara::theory::FretboardMask& mask() const noexcept { return mask_; }

private:
    gitara::theory::Fretboard fretboard_;
    gitara::theory::FretboardMask mask_;
    gitara::theory::Scale activeScale_{};
    gitara::theory::AccidentalMode accidentalMode_{gitara::theory::AccidentalMode::Auto};
    LabelType labelType_{LabelType::Notes};
    std::optional<int> detectedMidi_{};
    std::optional<gitara::theory::FretPosition> primaryDetectedPos_{};
};

} // namespace gitara::app::ui
#endif
