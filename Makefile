CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O3 -Wall -Wextra -Wpedantic -pthread \
            -Ilibs/theory/include \
            -Ilibs/score/include \
            -Ilibs/dsp/include \
            -Ilibs/engine/include \
            -I.
LDFLAGS  ?= -pthread -lm

EMBED_ASSETS ?= 0

LIB_SRCS := \
    libs/theory/src/Note.cpp \
    libs/theory/src/Pitch.cpp \
    libs/theory/src/Interval.cpp \
    libs/theory/src/Scale.cpp \
    libs/theory/src/Chord.cpp \
    libs/theory/src/Fretboard.cpp \
    libs/theory/src/Arpeggio.cpp \
    libs/score/src/Score.cpp \
    libs/score/src/StudioJsonCodec.cpp \
    libs/score/src/UserLibrary.cpp \
    libs/dsp/src/SignalConditioner.cpp \
    libs/dsp/src/PitchDetector.cpp \
    libs/dsp/src/OnsetDetector.cpp \
    libs/dsp/src/ChordDetector.cpp \
    libs/dsp/src/KarplusStrongSynth.cpp \
    libs/dsp/src/BandpassPreFilter.cpp \
    libs/dsp/src/CqtAnalyzer.cpp \
    libs/dsp/src/SpectralOnsetHps.cpp \
    libs/dsp/src/ActiveNoteTracker.cpp \
    libs/dsp/src/GuitarNoteEngine.cpp \
    libs/engine/src/PracticeEngine.cpp \
    libs/engine/src/WorkshopGenerator.cpp \
    libs/engine/src/EarTrainer.cpp \
    libs/engine/src/ProgressionEngine.cpp \
    libs/engine/src/SessionManager.cpp

PLATFORM_CORE_SRCS := \
    app/platform/UpdateLogic.cpp \
    app/platform/EmbeddedAssets.cpp \
    app/platform/AppPaths.cpp

APP_HOST_SRCS := \
    app/platform/AutoUpdater.cpp \
    app/platform/WindowsHost.cpp \
    app/platform/EmbeddedAssetsGlobal.cpp

BUILD_DIR := build

ifeq ($(EMBED_ASSETS),1)
GEN_DIR := $(BUILD_DIR)/generated
PAK_FILE := $(GEN_DIR)/gitara_assets.pak
EMBED_ASM := $(GEN_DIR)/gitara_assets.S
WEB_DEPS := $(shell find web -type f 2>/dev/null)
APP_EMBED_FLAGS := -DGITARA_EMBEDDED_ASSETS=1
APP_EMBED_INPUTS := $(EMBED_ASM)
else
APP_EMBED_FLAGS :=
APP_EMBED_INPUTS :=
endif

.PHONY: all test clean

all: $(BUILD_DIR)/GitaraTests $(BUILD_DIR)/GitaraApp

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

ifeq ($(EMBED_ASSETS),1)
$(GEN_DIR):
	mkdir -p $(GEN_DIR)

$(PAK_FILE) $(EMBED_ASM): tools/pack_assets.py app/ui/DesktopGuiHtml.h $(WEB_DEPS) | $(GEN_DIR)
	python3 tools/pack_assets.py \
		--html app/ui/DesktopGuiHtml.h \
		--web-dir web \
		--out-pak $(PAK_FILE) \
		--mode asm \
		--out-embed $(EMBED_ASM)
endif

$(BUILD_DIR)/GitaraTests: $(LIB_SRCS) $(PLATFORM_CORE_SRCS) tests/test_main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_SRCS) $(PLATFORM_CORE_SRCS) tests/test_main.cpp -o $(BUILD_DIR)/GitaraTests $(LDFLAGS)

$(BUILD_DIR)/GitaraApp: $(LIB_SRCS) $(PLATFORM_CORE_SRCS) $(APP_HOST_SRCS) $(APP_EMBED_INPUTS) app/Main.cpp app/ui/DesktopGuiHtml.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(APP_EMBED_FLAGS) $(LIB_SRCS) $(PLATFORM_CORE_SRCS) $(APP_HOST_SRCS) $(APP_EMBED_INPUTS) app/Main.cpp -o $(BUILD_DIR)/GitaraApp $(LDFLAGS)

test: $(BUILD_DIR)/GitaraTests
	./$(BUILD_DIR)/GitaraTests

clean:
	rm -rf $(BUILD_DIR)
