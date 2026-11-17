#pragma once

#if __has_include(<juce_data_structures/juce_data_structures.h>)
#include <juce_data_structures/juce_data_structures.h>

namespace gitara::app::IDs {
    inline const juce::Identifier ROOT            {"ROOT"};
    inline const juce::Identifier SETTINGS        {"SETTINGS"};
    inline const juce::Identifier ACCIDENTAL_MODE {"accidentalMode"};
    inline const juce::Identifier TUNING_ID       {"tuningId"};
    inline const juce::Identifier A4_HZ           {"a4Hz"};
    inline const juce::Identifier TAB_RECORDING   {"tabRecordingEnabled"};
} // namespace gitara::app::IDs
#endif
