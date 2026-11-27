# Apka Gitarowa

A guitar practice app in C++20: music theory, a DSP engine (pitch, chord and onset detection), tablature with GP5/MIDI export, and a GUI.

Detection runs a hybrid engine: PitchPlease (a neural WASM model) supplies the notes, onsets and velocities, while a chord namer ported from [dechorder](https://github.com/Airflame/dechorder) (28 qualities plus inversions/slash chords) names the chord — improving chord precision and expanding multi-note recognition.

## Standalone Windows Executable (No WSL / VS Code Required)

End users can download the ready-to-run Windows x64 executable directly from GitHub Releases:

- **Direct download (always latest `main`):** https://github.com/AlKrul/gitara-apka/releases/latest/download/ApkaGitarowa.exe

### Features of `ApkaGitarowa.exe`
- **Zero setup:** single self-contained `.exe` (~4–5 MB) with HTML, JS, CSS, PitchPlease WASM, and neural models embedded inside. Opens in a dedicated Microsoft Edge app window (`--app`) with no terminal window, and shuts down cleanly when you close the window.
- **Automatic pre-launch updates:** on every launch, it checks `manifest.json` from the public `AlKrul/gitara-apka` `latest` release, verifies SHA-256 checksums, swaps the executable in place, and relaunches the new version. If offline or skipped (`Pomiń` / `Esc`), it starts offline immediately.
- **Persistent user data:** settings and `user_library/` are stored in `%APPDATA%\ApkaGitarowa` so updates never overwrite your saved tabs, progressions, or sessions.
- **Windows SmartScreen notice:** because the open-source binary is not signed with a commercial EV certificate, Windows may show *"Windows protected your PC"* on first launch — click **More info → Run anyway**.

### Optional CLI Flags
- `--no-update` — skip the pre-launch GitHub update check (also respects `GITARA_NO_UPDATE=1`)
- `--portable` — store `settings.json` and `user_library/` next to the `.exe` instead of `%APPDATA%`
- `--headless-check` — validate theory API and embedded web/WASM/model assets and exit (`0` = OK)
- `--version` — print build version, commit SHA, timestamp, and update repository

## Repository layout

| Path | Contents |
|------|----------|
| `libs/theory` | Music theory (notes, intervals, scales, chords, fretboard, arpeggios), pure C++20 |
| `libs/score` | Score/tab model, GP5 and MIDI export |
| `libs/dsp` | Pitch/onset/chord detection, signal conditioning, Karplus-Strong synth |
| `libs/engine` | Practice engine, metronome, ear trainer, progressions, sessions |
| `app/` | Application entry point (`Main.cpp`), embedded GUI HTML, and `app/platform/` host/updater |
| `web/` | JS DSP modules, the hybrid chord namer ported from [dechorder](https://github.com/Airflame/dechorder) (`web/js/dsp/dechorder`), and the vendored PitchPlease WASM engine |
| `tools/` | Asset packer (`pack_assets.py`), icon generator (`make_icon.py`), release manifest generator (`make_manifest.py`) |
| `tests/` | C++ unit tests (`test_main.cpp`) and JS DSP/Studio tests |

## Requirements (Building from Source)

- A C++20 compiler: MSVC (Visual Studio 2022 or newer), GCC 13+ or Clang 16+
- CMake 3.24+ and Python 3 (for standalone embedded asset builds via CMake or `EMBED_ASSETS=1`)
- Node.js 18+, only to run the JS DSP / Studio tests

## Building Standalone App with CMake (Windows / Linux)

```sh
cmake -B build -DGITARA_BUILD_JUCE_APP=OFF -DGITARA_BUILD_DESKTOP_APP=ON -DGITARA_BUILD_TESTS=ON
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure -C Release
```

This produces `build/Release/ApkaGitarowa.exe` on Windows or `build/ApkaGitarowa` on Linux with all `web/` assets embedded. Every push to `main` automatically runs `.github/workflows/release-windows.yml` to build, test, and update the rolling `latest` GitHub Release.

## Building with Make (Linux / WSL / macOS)

```sh
make all                  # builds build/GitaraTests and build/GitaraApp (dev mode)
EMBED_ASSETS=1 make all   # builds GitaraApp with embedded web/ assets
make test                 # builds and runs the unit tests
make clean
```

You can override the compiler with `make CXX=clang++`.

## Running in Development

From the repository root, `./build/GitaraApp` live-reloads `app/ui/DesktopGuiHtml.h` and `web/` directly from disk and stores dev settings in `.apka_gitara_settings.json` and `user_library/`.

To check that the theory API and all web assets (DSP modules, PitchPlease WASM and model) can be loaded without starting the server:

```sh
./build/GitaraApp --headless-check   # exit code 0 = OK
```

## Tests

```sh
# C++ unit tests
make test

# JS DSP, dechorder, ear trainer, and Studio TAB tests (from the repository root)
node tests/dechorder_web_test.mjs
node tests/dsp_web_test.mjs
node tests/ear_web_test.mjs
node tests/studio_web_test.mjs
```
