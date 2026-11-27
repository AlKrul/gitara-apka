cmake_minimum_required(VERSION 3.24)
project(ApkaGitara VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(GITARA_BUILD_JUCE_APP "Pobierz JUCE 8 i zbuduj natywne okno GUI (Direct2D / ASIO / WASAPI)" OFF)
option(GITARA_BUILD_DESKTOP_APP "Zbuduj samodzielna aplikacje desktopowa (HTTP GUI + wbudowane zasoby + auto-update)" ON)
option(GITARA_BUILD_TESTS "Zbuduj pakiet testow automatycznych" ON)

set(GITARA_BUILD_COMMIT "" CACHE STRING "Pelny SHA commita umieszczany w binarce")
set(GITARA_BUILD_TIMESTAMP "" CACHE STRING "Znacznik czasu buildu (UTC)")
set(GITARA_UPDATE_REPO "AlKrul/gitara-apka" CACHE STRING "Repozytorium GitHub dla auto-update")
set(GITARA_UPDATE_TAG "latest" CACHE STRING "Tag wydania GitHub dla auto-update")

if(NOT GITARA_BUILD_COMMIT)
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE GITARA_GIT_HEAD
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
    if(GITARA_GIT_HEAD)
        set(GITARA_BUILD_COMMIT "${GITARA_GIT_HEAD}")
    else()
        set(GITARA_BUILD_COMMIT "dev")
    endif()
endif()

if(GITARA_BUILD_COMMIT STREQUAL "dev")
    set(GITARA_BUILD_SHORT_COMMIT "dev")
else()
    string(SUBSTRING "${GITARA_BUILD_COMMIT}" 0 7 GITARA_BUILD_SHORT_COMMIT)
endif()

if(NOT GITARA_BUILD_TIMESTAMP)
    string(TIMESTAMP GITARA_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ" UTC)
endif()

if(MSVC)
    add_compile_options(/utf-8)
endif()
if(WIN32)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

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
    libs/theory/src/Arpeggio.cpp
)
target_include_directories(gitara_theory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/theory/include
)

# ------------------------------------------------------------------------------
# [L0] libs/score — model zapisu nut/tabulatury, TabRecorder, eksport GP5 i MIDI
# ------------------------------------------------------------------------------
add_library(gitara_score STATIC
    libs/score/src/Score.cpp
    libs/score/src/StudioJsonCodec.cpp
    libs/score/src/UserLibrary.cpp
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
    libs/dsp/src/BandpassPreFilter.cpp
    libs/dsp/src/CqtAnalyzer.cpp
    libs/dsp/src/SpectralOnsetHps.cpp
    libs/dsp/src/ActiveNoteTracker.cpp
    libs/dsp/src/GuitarNoteEngine.cpp
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
    libs/engine/src/WorkshopGenerator.cpp
    libs/engine/src/EarTrainer.cpp
    libs/engine/src/ProgressionEngine.cpp
    libs/engine/src/SessionManager.cpp
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

# Warstwa platformowa (sciezki danych, zasoby wbudowane, logika auto-update)
add_library(gitara_platform_core STATIC
    app/platform/UpdateLogic.cpp
    app/platform/EmbeddedAssets.cpp
    app/platform/AppPaths.cpp
)
target_include_directories(gitara_platform_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(gitara_platform_core PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:$<IF:$<CXX_COMPILER_ID:MSVC>,/W4,-Wall -Wextra>>
)
if(WIN32)
    target_link_libraries(gitara_platform_core PUBLIC shell32 ole32 uuid)
endif()

# ------------------------------------------------------------------------------
# [L3] app — Samodzielna aplikacja JUCE 8 (opcjonalna) lub Desktop App (HTTP + Edge)
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
elseif(GITARA_BUILD_DESKTOP_APP)
    find_package(Threads REQUIRED)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)

    file(GLOB_RECURSE GITARA_WEB_ASSETS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/web/*"
    )
    set(GITARA_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(GITARA_PAK_FILE "${GITARA_GENERATED_DIR}/gitara_assets.pak")
    set(GITARA_PACK_EXTRA_ARGS)
    set(GITARA_PACK_EXTRA_DEPS)

    if(WIN32)
        set(GITARA_PACK_MODE "rc")
        set(GITARA_EMBED_FILE "${GITARA_GENERATED_DIR}/gitara_resources.rc")
        set(GITARA_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/app/resources/apka_gitara.ico")
        list(APPEND GITARA_PACK_EXTRA_ARGS --icon "${GITARA_ICON_FILE}")
        list(APPEND GITARA_PACK_EXTRA_DEPS "${GITARA_ICON_FILE}")
    elseif(UNIX AND NOT APPLE)
        enable_language(ASM)
        set(GITARA_PACK_MODE "asm")
        set(GITARA_EMBED_FILE "${GITARA_GENERATED_DIR}/gitara_assets.S")
    else()
        set(GITARA_PACK_MODE "cpp")
        set(GITARA_EMBED_FILE "${GITARA_GENERATED_DIR}/gitara_assets_blob.cpp")
    endif()

    add_custom_command(
        OUTPUT "${GITARA_PAK_FILE}" "${GITARA_EMBED_FILE}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/pack_assets.py"
            --html "${CMAKE_CURRENT_SOURCE_DIR}/app/ui/DesktopGuiHtml.h"
            --web-dir "${CMAKE_CURRENT_SOURCE_DIR}/web"
            --out-pak "${GITARA_PAK_FILE}"
            --mode "${GITARA_PACK_MODE}"
            --out-embed "${GITARA_EMBED_FILE}"
            --version "${PROJECT_VERSION}"
            --commit "${GITARA_BUILD_COMMIT}"
            ${GITARA_PACK_EXTRA_ARGS}
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tools/pack_assets.py"
            "${CMAKE_CURRENT_SOURCE_DIR}/app/ui/DesktopGuiHtml.h"
            ${GITARA_WEB_ASSETS}
            ${GITARA_PACK_EXTRA_DEPS}
        COMMENT "Pakowanie zasobow GUI i web/ do binarki ApkaGitarowa"
        VERBATIM
    )

    set_source_files_properties("${GITARA_EMBED_FILE}" PROPERTIES
        OBJECT_DEPENDS "${GITARA_PAK_FILE};${GITARA_PACK_EXTRA_DEPS}"
    )

    add_executable(ApkaGitarowa
        app/Main.cpp
        app/platform/AutoUpdater.cpp
        app/platform/WindowsHost.cpp
        app/platform/EmbeddedAssetsGlobal.cpp
        "${GITARA_EMBED_FILE}"
    )
    if(WIN32)
        set_target_properties(ApkaGitarowa PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()

    target_compile_definitions(ApkaGitarowa PRIVATE
        GITARA_EMBEDDED_ASSETS=1
        GITARA_APP_VERSION="${PROJECT_VERSION}"
        GITARA_BUILD_COMMIT="${GITARA_BUILD_COMMIT}"
        GITARA_BUILD_SHORT_COMMIT="${GITARA_BUILD_SHORT_COMMIT}"
        GITARA_BUILD_TIMESTAMP="${GITARA_BUILD_TIMESTAMP}"
        GITARA_UPDATE_REPO="${GITARA_UPDATE_REPO}"
        GITARA_UPDATE_TAG="${GITARA_UPDATE_TAG}"
    )
    if(GITARA_PACK_MODE STREQUAL "cpp")
        target_compile_definitions(ApkaGitarowa PRIVATE GITARA_EMBEDDED_ASSETS_CPP=1)
    endif()

    target_compile_options(ApkaGitarowa PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:$<IF:$<CXX_COMPILER_ID:MSVC>,/W4,-Wall -Wextra>>
    )

    target_link_libraries(ApkaGitarowa PRIVATE
        gitara_shared_code
        gitara_platform_core
        Threads::Threads
    )
    if(WIN32)
        target_link_libraries(ApkaGitarowa PRIVATE
            ws2_32
            winhttp
            shell32
            ole32
            user32
            gdi32
            advapi32
        )
    endif()
endif()

# ------------------------------------------------------------------------------
# Testy jednostkowe i integracyjne (L0–L2 + platform core)
# ------------------------------------------------------------------------------
if(GITARA_BUILD_TESTS)
    enable_testing()
    add_executable(GitaraTests
        tests/test_main.cpp
    )
    target_link_libraries(GitaraTests PRIVATE
        gitara_shared_code
        gitara_platform_core
    )
    add_test(NAME GitaraTests COMMAND GitaraTests)
endif()
