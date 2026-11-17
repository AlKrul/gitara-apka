#pragma once

#if __has_include(<juce_audio_utils/juce_audio_utils.h>)
#include <juce_audio_utils/juce_audio_utils.h>

namespace gitara::app::ui {

/// Moduł wirtualnej klawiatury MIDI oparty o juce::MidiKeyboardComponent oraz juce::MidiKeyboardState.
/// Realizuje wymagania specyfikacji:
/// 1. Mapowanie QWERTY (A=C4, W=C#4, S=D4, E=D#4, D=E4, F=F4, T=F#4, G=G4, Y=G#4, H=A4, U=A#4, J=B4, K=C5),
/// 2. Przełącznik focusu (Toggle): kliknięcie wywołuje grabKeyboardFocus(), ponowne kliknięcie giveAwayKeyboardFocus(),
/// 3. Przechwytywanie pełnego wejścia z klawiatury komputera (z pominięciem skrótów UI),
/// 4. Wysłanie allNotesOff() przy utracie focusu (ochrona przed „zawieszonymi” nutami),
/// 5. Informacja w UI o sprzętowym limicie Key Ghosting (2–3 klawisze na zwykłych klawiaturach biurowych).
class MidiKeyboardPanel final : public juce::Component {
public:
    explicit MidiKeyboardPanel(juce::MidiKeyboardState& state)
        : keyboardState_(state),
          keyboardComponent_(state, juce::MidiKeyboardComponent::horizontalKeyboard) {
        keyboardComponent_.setKeyPressBaseOctave(4); // A = C4, W = C#4
        keyboardComponent_.setAvailableRange(36, 84); // C2 .. C6
        addAndMakeVisible(keyboardComponent_);

        focusToggleButton_.setButtonText("🎹 Klawiatura QWERTY: WYŁ. (kliknij, aby przejąć focus)");
        focusToggleButton_.onClick = [this] { toggleKeyboardCapture(); };
        addAndMakeVisible(focusToggleButton_);

        ghostingHintLabel_.setText(
            "Uwaga (Key Ghosting): zwykłe klawiatury sprzętowo limitują wciśnięcie do ~3 klawiszy jednocześnie.",
            juce::dontSendNotification);
        ghostingHintLabel_.setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(ghostingHintLabel_);
    }

    void toggleKeyboardCapture() {
        if (keyboardCaptured_) {
            keyboardState_.allNotesOff(0);
            keyboardComponent_.giveAwayKeyboardFocus();
            keyboardCaptured_ = false;
        } else {
            keyboardComponent_.grabKeyboardFocus();
            keyboardCaptured_ = true;
        }
        updateToggleAppearance();
    }

    void focusOfChildComponentChanged(FocusChangeType) override {
        const bool hasFocusNow = keyboardComponent_.hasKeyboardFocus(true);
        if (!hasFocusNow && keyboardCaptured_) {
            keyboardState_.allNotesOff(0);
            keyboardCaptured_ = false;
            updateToggleAppearance();
        } else if (hasFocusNow && !keyboardCaptured_) {
            keyboardCaptured_ = true;
            updateToggleAppearance();
        }
    }

    bool keyPressed(const juce::KeyPress& key) override {
        if (keyboardCaptured_) {
            return keyboardComponent_.keyPressed(key);
        }
        return false;
    }

    bool keyStateChanged(bool isKeyDown) override {
        if (keyboardCaptured_) {
            return keyboardComponent_.keyStateChanged(isKeyDown);
        }
        return false;
    }

    void resized() override {
        auto area = getLocalBounds().reduced(6);
        auto topBar = area.removeFromTop(28);
        focusToggleButton_.setBounds(topBar.removeFromLeft(360));
        ghostingHintLabel_.setBounds(topBar);
        area.removeFromTop(4);
        keyboardComponent_.setBounds(area);
    }

private:
    void updateToggleAppearance() {
        if (keyboardCaptured_) {
            focusToggleButton_.setButtonText("🎹 Klawiatura QWERTY: AKTYWNA (kliknij, aby zwolnić focus)");
        } else {
            focusToggleButton_.setButtonText("🎹 Klawiatura QWERTY: WYŁ. (kliknij, aby przejąć focus)");
        }
        repaint();
    }

    juce::MidiKeyboardState& keyboardState_;
    juce::MidiKeyboardComponent keyboardComponent_;
    juce::TextButton focusToggleButton_;
    juce::Label ghostingHintLabel_;
    bool keyboardCaptured_{false};
};

} // namespace gitara::app::ui
#endif
