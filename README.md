# Apka Gitarowa

A guitar practice app in C++20: music theory, a DSP engine (pitch, chord and onset detection), tablature with GP5/MIDI export, and a GUI.

## Repository layout

| Path | Contents |
|------|----------|
| `libs/theory` | Music theory (notes, intervals, scales, chords, fretboard, arpeggios), pure C++20 |
| `libs/score` | Score/tab model, GP5 and MIDI export |
| `libs/dsp` | Pitch/onset/chord detection, signal conditioning, Karplus-Strong synth |
| `libs/engine` | Practice engine, metronome, ear trainer, progressions, sessions |
| `app/` | Application entry point (`Main.cpp`) and the embedded HTML GUI |
| `web/` | JS DSP modules and the vendored PitchPlease WASM engine, served at runtime |
| `tests/` | C++ unit tests (`test_main.cpp`) and the JS DSP test (`dsp_web_test.mjs`) |

## Requirements

- A C++20 compiler: MSVC (Visual Studio 2022 or newer), GCC 13+ or Clang 16+
- CMake 3.24+ (for the CMake build)
- Git and network access, only if you build the JUCE app (JUCE 8 is downloaded at configure time)
- Node.js 18+, only to run the JS DSP test

## Building with CMake (recommended)

CMake options:

| Option | Default | Description |
|--------|---------|-------------|
| `GITARA_BUILD_TESTS` | `ON` | Build the `GitaraTests` test executable |
| `GITARA_BUILD_JUCE_APP` | `OFF` | Download JUCE 8.0.4 and build the native `GitaraApp` GUI (Direct2D / ASIO / WASAPI) |

### Core libraries and tests

```sh
cmake -B build -DGITARA_BUILD_JUCE_APP=OFF -DGITARA_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

This is the same sequence CI runs on Windows and Linux (`.github/workflows/ci.yml`).

### Native JUCE app (Windows)

```sh
cmake -B build-juce -DGITARA_BUILD_JUCE_APP=ON
cmake --build build-juce --config Release
```

The first configure clones JUCE, so it takes a while. With Visual Studio generators the executable is `build-juce/GitaraApp_artefacts/Release/Apka Gitarowa.exe`.

To open the project in Visual Studio, pick a generator explicitly, for example `-G "Visual Studio 17 2022"`, then open the generated solution in `build-juce/`. CLion can open the root `CMakeLists.txt` directly. Set `GITARA_BUILD_JUCE_APP=ON` in the CMake profile options to get the GUI target.

## Building with Make (Linux / WSL / macOS)

The `Makefile` builds without CMake or JUCE. Without JUCE, `app/Main.cpp` uses POSIX sockets, so this route does not work with MSVC or MinGW on Windows; use WSL there.

```sh
make all     # builds build/GitaraTests and build/GitaraApp
make test    # builds and runs the unit tests
make clean
```

You can override the compiler with `make CXX=clang++`.

Without JUCE, `GitaraApp` runs as a lightweight local server. It serves the GUI at `http://127.0.0.1:8765` (or the next free port up to 8784) and tries to open it in a browser window automatically (Edge `--app` on WSL, otherwise `start` or `xdg-open`). Press Ctrl+C to quit.

## Running

The app loads its JS and WASM assets from `web/` (or `../web/`) **relative to the current working directory**, so start it from the repository root:

```sh
./build/GitaraApp
```

To check that the theory API and all web assets (DSP modules, PitchPlease WASM and model) can be found, without starting the server:

```sh
./build/GitaraApp --headless-check   # exit code 0 = OK
```

## Tests

```sh
# C++ unit tests (either build system)
ctest --test-dir build --output-on-failure -C Release
# or
make test

# JS DSP engine test (from the repository root)
node tests/dsp_web_test.mjs
```
