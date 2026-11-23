# Apka Gitarowa

A guitar practice app in C++20: music theory, a DSP engine (pitch, chord and onset detection), tablature with GP5/MIDI export, and a GUI.

Detection runs a hybrid engine: PitchPlease (a neural WASM model) supplies the notes, onsets and velocities, while a chord namer ported from [dechorder](https://github.com/Airflame/dechorder) (28 qualities plus inversions/slash chords) names the chord — improving chord precision and expanding multi-note recognition.

## Repository layout

| Path | Contents |
|------|----------|
| `libs/theory` | Music theory (notes, intervals, scales, chords, fretboard, arpeggios), pure C++20 |
| `libs/score` | Score/tab model, GP5 and MIDI export |
| `libs/dsp` | Pitch/onset/chord detection, signal conditioning, Karplus-Strong synth |
| `libs/engine` | Practice engine, metronome, ear trainer, progressions, sessions |
| `app/` | Application entry point (`Main.cpp`) and the embedded HTML GUI |
| `web/` | JS DSP modules, the hybrid chord namer ported from [dechorder](https://github.com/Airflame/dechorder) (`web/js/dsp/dechorder`), and the vendored PitchPlease WASM engine, served at runtime |
| `tests/` | C++ unit tests (`test_main.cpp`) and JS DSP tests (`dsp_web_test.mjs`, `dechorder_web_test.mjs`) |

## Requirements

- A C++20 compiler: MSVC (Visual Studio 2022 or newer), GCC 13+ or Clang 16+
- Node.js 18+, only to run the JS DSP test

## Building with Make (Linux / WSL / macOS)

The `Makefile` builds without JUCE. Without JUCE, `app/Main.cpp` uses POSIX sockets, so this route does not work with MSVC or MinGW on Windows; use WSL there.

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
# C++ unit tests
make test

# JS DSP engine test (from the repository root)
node tests/dsp_web_test.mjs

# Hybrid chord namer test (dechorder port)
node tests/dechorder_web_test.mjs
```
