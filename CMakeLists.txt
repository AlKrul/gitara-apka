cmake_minimum_required(VERSION 3.24)
project(ApkaGitara VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GITARA_BUILD_JUCE_APP "Pobierz JUCE 8 i zbuduj natywne okno GUI (Direct2D / ASIO / WASAPI)" OFF)
option(GITARA_BUILD_TESTS "Zbuduj pakiet testow automatycznych" ON)

# ------------------------------------------------------------------------------
# [L0] libs/theory — czysty C++20 bez zaleznosci od JUCE (system A–G, bez H)
# ------------------------------------------------------------------------------
add_library(gitara_theory STATIC
    libs/theory/src/Note.cpp
    libs/theory/src/Pitch.cpp
    libs/theory/src/Interval.cpp
    libs/theory/src/Scale.cpp
    libs/theory/src/Chord.cpp
    libs/theory/src/Fretboard.cpp
)
target_include_directories(gitara_theory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/theory/include
)

# ------------------------------------------------------------------------------
# [L0] libs/score — model zapisu nut/tabulatury, TabRecorder, eksport GP5 i MIDI
# ------------------------------------------------------------------------------
add_library(gitara_score STATIC
    libs/score/src/Score.cpp
)
target_include_directories(gitara_score PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/score/include
)
target_link_libraries(gitara_score PUBLIC gitara_theory)

# ------------------------------------------------------------------------------
# [L1] libs/dsp — detekcja wysokosci (YIN + Bitstream Q), akordow (HPCP), onset,
#                 kondycjoner sygnalu i syntezator struny Karplus-Strong
# ------------------------------------------------------------------------------
add_library(gitara_dsp STATIC
    libs/dsp/src/SignalConditioner.cpp
    libs/dsp/src/PitchDetector.cpp
    libs/dsp/src/OnsetDetector.cpp
    libs/dsp/src/ChordDetector.cpp
    libs/dsp/src/KarplusStrongSynth.cpp
)
target_include_directories(gitara_dsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/dsp/include
)
target_link_libraries(gitara_dsp PUBLIC gitara_theory)

# ------------------------------------------------------------------------------
# [L2] libs/engine — kolejka lock-free SPSC, sample-accurate metronom, weryfikator
# ------------------------------------------------------------------------------
add_library(gitara_engine STATIC
    libs/engine/src/PracticeEngine.cpp
)
target_include_directories(gitara_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/engine/include
)
target_link_libraries(gitara_engine PUBLIC gitara_dsp gitara_score)

# Wspolna biblioteka „SharedCode” (wzorzec Pamplejuce) dla aplikacji i testow
add_library(gitara_shared_code INTERFACE)
target_link_libraries(gitara_shared_code INTERFACE
    gitara_theory
    gitara_score
    gitara_dsp
    gitara_engine
)
target_include_directories(gitara_shared_code INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ------------------------------------------------------------------------------
# [L3] app — Samodzielna aplikacja JUCE 8 (Windows Direct2D + ASIO/WASAPI)
# ------------------------------------------------------------------------------
if(GITARA_BUILD_JUCE_APP)
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.4
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(JUCE)

    juce_add_gui_app(GitaraApp
        PRODUCT_NAME "Apka Gitarowa"
        COMPANY_NAME "ApkaGitarowa"
    )
    target_sources(GitaraApp PRIVATE
        app/Main.cpp
    )
    target_compile_definitions(GitaraApp PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:GitaraApp,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:GitaraApp,JUCE_VERSION>"
    )
    target_link_libraries(GitaraApp PRIVATE
        gitara_shared_code
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
    )
endif()

# ------------------------------------------------------------------------------
# Testy jednostkowe i integracyjne (L0–L2)
# ------------------------------------------------------------------------------
if(GITARA_BUILD_TESTS)
    enable_testing()
    add_executable(GitaraTests
        tests/test_main.cpp
    )
    target_link_libraries(GitaraTests PRIVATE gitara_shared_code)
    add_test(NAME GitaraTests COMMAND GitaraTests)
endif()
