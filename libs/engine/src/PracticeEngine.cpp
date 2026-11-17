#include "gitara/engine/PracticeEngine.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace gitara::engine {

void Metronome::prepare(double sampleRate) noexcept {
    if (sampleRate <= 0.0) sampleRate = 48000.0;
    sampleRate_ = sampleRate;
    reset();
}

void Metronome::reset() noexcept {
    sampleCounter_ = 0;
    nextSubClickSample_ = 0;
    currentBeatInBar_ = 0;
    currentSubInBeat_ = 0;
    clickSamplesRemaining_ = 0;
    clickPhase_ = 0.0;
}

void Metronome::setTempoBpm(double bpm) noexcept {
    tempoBpm_ = std::clamp(bpm, 20.0, 300.0);
}

void Metronome::setTimeSignature(int numerator, int denominator) noexcept {
    numerator_ = std::clamp(numerator, 1, 16);
    denominator_ = std::clamp(denominator, 1, 16);
}

void Metronome::setSubdivision(int clicksPerBeat) noexcept {
    subdivision_ = std::clamp(clicksPerBeat, 1, 6);
}

void Metronome::renderNextBlock(float* outputBuffer, std::size_t numSamples) noexcept {
    if (!outputBuffer || numSamples == 0 || !enabled_) {
        return;
    }

    const double beatSeconds = (60.0 / tempoBpm_) * (4.0 / static_cast<double>(denominator_));
    const double subSeconds = beatSeconds / static_cast<double>(subdivision_);
    const std::uint64_t samplesPerSub = static_cast<std::uint64_t>(
        std::max(1LL, std::llround(subSeconds * sampleRate_)));
    const std::size_t clickDurationSamples = static_cast<std::size_t>(0.015 * sampleRate_);

    for (std::size_t i = 0; i < numSamples; ++i) {
        if (sampleCounter_ >= nextSubClickSample_) {
            clickSamplesRemaining_ = clickDurationSamples;
            clickPhase_ = 0.0;
            if (currentSubInBeat_ == 0 && currentBeatInBar_ == 0) {
                clickFreqHz_ = 1600.0; // Akcent na „raz” w takcie
                clickGain_ = 0.35f;
            } else if (currentSubInBeat_ == 0) {
                clickFreqHz_ = 1100.0; // Główne miary taktu
                clickGain_ = 0.25f;
            } else {
                clickFreqHz_ = 800.0;  // Podziały rytmiczne (ósemki, triole, szesnastki)
                clickGain_ = 0.14f;
            }

            ++currentSubInBeat_;
            if (currentSubInBeat_ >= subdivision_) {
                currentSubInBeat_ = 0;
                currentBeatInBar_ = (currentBeatInBar_ + 1) % numerator_;
            }
            nextSubClickSample_ = sampleCounter_ + samplesPerSub;
        }

        if (clickSamplesRemaining_ > 0) {
            float env = static_cast<float>(clickSamplesRemaining_) /
                        static_cast<float>(clickDurationSamples);
            float sample = clickGain_ * env * static_cast<float>(std::sin(clickPhase_));
            outputBuffer[i] += sample;
            clickPhase_ += 2.0 * std::numbers::pi * clickFreqHz_ / sampleRate_;
            --clickSamplesRemaining_;
        }

        ++sampleCounter_;
    }
}

void ExerciseVerifier::setSteps(std::vector<ExerciseStep> steps, double centsTolerance) {
    steps_ = std::move(steps);
    centsTolerance_ = std::max(5.0, centsTolerance);
    reset();
}

void ExerciseVerifier::reset() noexcept {
    evaluations_.clear();
    currentStepIndex_ = 0;
}

bool ExerciseVerifier::onMonophonicDetection(
    const gitara::dsp::PitchResult& pitch,
    double currentTimeSeconds) {
    if (isFinished() || !pitch.pitched) {
        return false;
    }

    const auto& step = steps_[currentStepIndex_];
    if (step.isChord) {
        return false;
    }

    if (pitch.deviation.midiNote == step.expectedMidiNote &&
        std::abs(pitch.deviation.cents) <= centsTolerance_) {
        StepEvaluation eval{};
        eval.stepIndex = currentStepIndex_;
        eval.pitchCorrect = true;
        eval.intonationErrorCents = pitch.deviation.cents;
        eval.timingErrorMs = (currentTimeSeconds - step.expectedTimeSeconds) * 1000.0;
        evaluations_.push_back(eval);
        ++currentStepIndex_;
        return true;
    }
    return false;
}

bool ExerciseVerifier::onChordConfidence(double chordConfidence, double currentTimeSeconds) {
    if (isFinished()) {
        return false;
    }

    const auto& step = steps_[currentStepIndex_];
    if (!step.isChord) {
        return false;
    }

    if (chordConfidence >= 0.68) {
        StepEvaluation eval{};
        eval.stepIndex = currentStepIndex_;
        eval.pitchCorrect = true;
        eval.intonationErrorCents = 0.0;
        eval.timingErrorMs = (currentTimeSeconds - step.expectedTimeSeconds) * 1000.0;
        evaluations_.push_back(eval);
        ++currentStepIndex_;
        return true;
    }
    return false;
}

double ExerciseVerifier::overallAccuracyPercent() const noexcept {
    if (steps_.empty()) {
        return 100.0;
    }
    std::size_t correctCount = 0;
    for (const auto& ev : evaluations_) {
        if (ev.pitchCorrect) {
            ++correctCount;
        }
    }
    return 100.0 * static_cast<double>(correctCount) / static_cast<double>(steps_.size());
}

} // namespace gitara::engine
