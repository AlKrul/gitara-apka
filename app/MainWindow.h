#pragma once

#include "app/ui/FretboardComponent.h"
#include "app/ui/MidiKeyboardPanel.h"
#include "app/ui/TunerComponent.h"
#include "gitara/dsp/KarplusStrongSynth.h"
#include "gitara/dsp/PitchDetector.h"
#include "gitara/dsp/SignalConditioner.h"
#include "gitara/engine/PracticeEngine.h"

#if __has_include(<juce_audio_utils/juce_audio_utils.h>)
#include <juce_audio_utils/juce_audio_utils.h>

namespace gitara::app {

/// Główny komponent roboczy łączący silnik audio (ASIO / WASAPI / mikrofon / Hotone Ampero One)
/// z wizualizacją gryfu, stroikiem igłowo-stroboskopowym oraz wirtualną klawiaturą MIDI.
class MainContentComponent final : public juce::AudioAppComponent,
                                   private juce::Timer {
public:
    MainContentComponent()
        : midiPanel_(keyboardState_) {
        addAndMakeVisible(tunerComponent_);
        addAndMakeVisible(fretboardComponent_);
        addAndMakeVisible(midiPanel_);

        accidentalCombo_.addItem("Znaki chromatyczne: Auto (wg teorii A–G)", 1);
        accidentalCombo_.addItem("Znaki chromatyczne: Bemole (♭)", 2);
        accidentalCombo_.addItem("Znaki chromatyczne: Krzyżyki (♯)", 3);
        accidentalCombo_.setSelectedId(1, juce::dontSendNotification);
        accidentalCombo_.onChange = [this] {
            int id = accidentalCombo_.getSelectedId();
            auto mode = (id == 2) ? gitara::theory::AccidentalMode::Flats
                      : (id == 3) ? gitara::theory::AccidentalMode::Sharps
                                  : gitara::theory::AccidentalMode::Auto;
            fretboardComponent_.setAccidentalMode(mode);
        };
        addAndMakeVisible(accidentalCombo_);

        fretboardComponent_.onNoteClicked = [this](int midiNote, double freqHz) {
            (void)midiNote;
            synth_.pluckFrequency(freqHz, 0.9f);
        };

        setSize(1280, 800);
        setAudioChannels(1, 2); // 1 wejście (gitara: Ampero One / mikrofon), 2 wyjścia (stereo)
        startTimerHz(60);
    }

    ~MainContentComponent() override {
        shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        conditioner_.prepare(sampleRate);
        pitchDetector_.prepare(sampleRate, std::max(2048, samplesPerBlockExpected * 4));
        synth_.prepare(sampleRate);
        metronome_.prepare(sampleRate);
        analysisWindow_.assign(2048, 0.0f);
        tempMonoOut_.assign(static_cast<std::size_t>(samplesPerBlockExpected), 0.0f);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override {
        auto* device = deviceManager.getCurrentAudioDevice();
        const int numSamples = bufferToFill.numSamples;
        if (!device || numSamples <= 0) {
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        // 1. Pobierz wejście z gitary (kanał 0) i skondycjonuj sygnał
        if (bufferToFill.buffer->getNumChannels() > 0) {
            const float* in = bufferToFill.buffer->getReadPointer(
                0, bufferToFill.startSample);
            for (int i = 0; i < numSamples; ++i) {
                analysisWindow_[analysisWritePos_] = in[i];
                analysisWritePos_ = (analysisWritePos_ + 1) % analysisWindow_.size();
            }
            auto res = pitchDetector_.detect(analysisWindow_.data(), analysisWindow_.size());
            pitchQueue_.push(res);
        }

        // 2. Wyczyść wyjście i zrenderuj syntezator Karplus-Strong oraz metronom
        bufferToFill.clearActiveBufferRegion();
        if (tempMonoOut_.size() < static_cast<std::size_t>(numSamples)) {
            tempMonoOut_.resize(static_cast<std::size_t>(numSamples), 0.0f);
        }
        std::fill_n(tempMonoOut_.begin(), numSamples, 0.0f);

        juce::MidiBuffer incomingMidi;
        keyboardState_.processNextMidiBuffer(incomingMidi, 0, numSamples, true);
        for (const auto metadata : incomingMidi) {
            const auto msg = metadata.getMessage();
            if (msg.isNoteOn()) {
                synth_.noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
            } else if (msg.isNoteOff()) {
                synth_.noteOff(msg.getNoteNumber());
            }
        }

        synth_.renderNextBlock(tempMonoOut_.data(), static_cast<std::size_t>(numSamples));
        metronome_.renderNextBlock(tempMonoOut_.data(), static_cast<std::size_t>(numSamples));

        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch) {
            float* out = bufferToFill.buffer->getWritePointer(ch, bufferToFill.startSample);
            for (int i = 0; i < numSamples; ++i) {
                out[i] = tempMonoOut_[static_cast<std::size_t>(i)];
            }
        }
    }

    void releaseResources() override {}

    void resized() override {
        auto area = getLocalBounds().reduced(10);
        auto topRow = area.removeFromTop(64);
        accidentalCombo_.setBounds(topRow.removeFromRight(320).reduced(4, 14));
        tunerComponent_.setBounds(topRow);

        auto bottomKeyboard = area.removeFromBottom(130);
        midiPanel_.setBounds(bottomKeyboard);
        fretboardComponent_.setBounds(area);
    }

private:
    void timerCallback() override {
        gitara::dsp::PitchResult latest{};
        bool gotAny = false;
        while (pitchQueue_.pop(latest)) {
            gotAny = true;
        }
        if (gotAny) {
            tunerComponent_.updatePitch(latest);
            if (latest.pitched) {
                fretboardComponent_.setDetectedMidiNote(latest.deviation.midiNote);
            } else {
                fretboardComponent_.setDetectedMidiNote(std::nullopt);
            }
        }
    }

    juce::MidiKeyboardState keyboardState_;
    juce::ComboBox accidentalCombo_;
    ui::TunerComponent tunerComponent_;
    ui::FretboardComponent fretboardComponent_;
    ui::MidiKeyboardPanel midiPanel_;

    gitara::dsp::SignalConditioner conditioner_;
    gitara::dsp::PitchDetector pitchDetector_;
    gitara::dsp::KarplusStrongSynth synth_;
    gitara::engine::Metronome metronome_;
    gitara::engine::LockFreeSpscQueue<gitara::dsp::PitchResult, 64> pitchQueue_;

    std::vector<float> analysisWindow_;
    std::size_t analysisWritePos_{0};
    std::vector<float> tempMonoOut_;
};

} // namespace gitara::app
#endif
