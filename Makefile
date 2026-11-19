CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O3 -Wall -Wextra -Wpedantic \
            -Ilibs/theory/include \
            -Ilibs/score/include \
            -Ilibs/dsp/include \
            -Ilibs/engine/include \
            -I.

LIB_SRCS := \
    libs/theory/src/Note.cpp \
    libs/theory/src/Pitch.cpp \
    libs/theory/src/Interval.cpp \
    libs/theory/src/Scale.cpp \
    libs/theory/src/Chord.cpp \
    libs/theory/src/Fretboard.cpp \
    libs/theory/src/Arpeggio.cpp \
    libs/score/src/Score.cpp \
    libs/dsp/src/SignalConditioner.cpp \
    libs/dsp/src/PitchDetector.cpp \
    libs/dsp/src/OnsetDetector.cpp \
    libs/dsp/src/ChordDetector.cpp \
    libs/dsp/src/KarplusStrongSynth.cpp \
    libs/engine/src/PracticeEngine.cpp \
    libs/engine/src/WorkshopGenerator.cpp \
    libs/engine/src/EarTrainer.cpp \
    libs/engine/src/ProgressionEngine.cpp \
    libs/engine/src/SessionManager.cpp

BUILD_DIR := build

.PHONY: all test clean

all: $(BUILD_DIR)/GitaraTests $(BUILD_DIR)/GitaraApp

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/GitaraTests: $(LIB_SRCS) tests/test_main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_SRCS) tests/test_main.cpp -o $(BUILD_DIR)/GitaraTests -lm

$(BUILD_DIR)/GitaraApp: $(LIB_SRCS) app/Main.cpp app/ui/DesktopGuiHtml.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_SRCS) app/Main.cpp -o $(BUILD_DIR)/GitaraApp -lm

test: $(BUILD_DIR)/GitaraTests
	./$(BUILD_DIR)/GitaraTests

clean:
	rm -rf $(BUILD_DIR)
