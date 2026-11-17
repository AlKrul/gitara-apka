#pragma once

#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace gitara::score {

/// Wartość rytmiczna uderzenia (zgodna z kodowaniem Guitar Pro / TuxGuitar:
/// -2 = cała nuta, -1 = półnuta, 0 = ćwierćnuta, 1 = ósemka, 2 = szesnastka, 3 = trzydziestodwójka).
enum class DurationValue : std::int8_t {
    Whole       = -2,
    Half        = -1,
    Quarter     =  0,
    Eighth      =  1,
    Sixteenth   =  2,
    ThirtySecond = 3
};

/// Pojedyncza nuta na strunie i progu w obrębie uderzenia (Beat).
struct TabNote {
    int stringIndex{0};  ///< 0 = najniższa struna (np. E2) .. 5 = najwyższa struna (np. E4)
    int fret{0};         ///< 0 = pusta struna .. 24
    int velocity{95};    ///< Dynamika MIDI (1..127)
    bool tied{false};    ///< Łuk (przetrzymanie z poprzedniej nuty)
    bool hammerOn{false};///< Legato (hammer-on / pull-off)
    bool slide{false};   ///< Slide
};

/// Uderzenie (Beat) w takcie – może być pauzą, pojedynczą nutą lub akordem (wieloma TabNote na różnych strunach).
struct Beat {
    DurationValue duration{DurationValue::Quarter};
    bool dotted{false};
    int tupletNumerator{0};   ///< 0 = brak tupletu, 3 = triola (3:2)
    int tupletDenominator{0};
    bool isRest{false};
    std::vector<TabNote> notes;

    /// Czas trwania uderzenia w jednostkach ćwierćnuty (1.0 = ćwierćnuta, 0.5 = ósemka, 0.25 = szesnastka).
    [[nodiscard]] double quarterNoteLength() const noexcept;
};

/// Takt (Measure) w ścieżce tabulatury.
struct Measure {
    int numerator{4};
    int denominator{4};
    std::vector<Beat> beats;
};

/// Ścieżka instrumentu (domyślnie 6-strunowa gitara).
struct Track {
    std::string name{"Gitara"};
    gitara::theory::Tuning tuning = gitara::theory::findTuning("standard");
    int midiChannel{0};
    int midiProgram{24}; ///< 24 = Acoustic Guitar (nylon), 25 = Steel, 27 = Electric Clean
    std::vector<Measure> measures;
};

/// Pełny utwór / ćwiczenie z metadanymi i ścieżkami (model kompatybilny z TuxGuitar / GP5 / przyszłym importem Rocksmith).
struct Song {
    std::string title{"Nowe ćwiczenie"};
    std::string artist{"Apka Gitarowa"};
    int tempoBpm{120};
    std::vector<Track> tracks;
};

/// Zdarzenie wykryte na żywo podczas grania z włączoną opcją „Tworzenie tabulatury”.
struct DetectedNoteEvent {
    double startTimeSeconds{0.0};   ///< Czas ataku (po kompensacji latencji)
    double durationSeconds{0.25};   ///< Czas wybrzmiewania
    std::vector<int> midiNotes;     ///< Wykryte nuty MIDI (1 dla melodii, wiele dla akordu)
};

/// Rejestrator tabulatury: przekształca zagrane dźwięki i odstępy między nimi w skwantyzowany zapis
/// tabulatury, dobierając ergonomiczne pozycje na gryfie z uwzględnieniem maski obszaru (FretboardMask).
class TabRecorder {
public:
    TabRecorder(gitara::theory::Fretboard fretboard = gitara::theory::Fretboard{},
                int tempoBpm = 120,
                int timeSigNumerator = 4,
                int timeSigDenominator = 4);

    void setTabCreationEnabled(bool enabled) noexcept { enabled_ = enabled; }
    [[nodiscard]] bool isTabCreationEnabled() const noexcept { return enabled_; }

    /// Buduje obiekt Song (gotowy do podglądu i eksportu do TuxGuitar) z listy wykrytych dźwięków.
    [[nodiscard]] Song buildSongFromEvents(
        const std::vector<DetectedNoteEvent>& events,
        const gitara::theory::FretboardMask* mask = nullptr,
        std::string title = "Nagrana melodia") const;

private:
    gitara::theory::Fretboard fretboard_;
    int tempoBpm_{120};
    int numerator_{4};
    int denominator_{4};
    bool enabled_{true};
};

/// Wykrywa najbardziej prawdopodobną tonację (np. "C dur", "A moll") z zapisanych nut w ćwiczeniu
/// przy użyciu profili tonalnych Krumhansla-Schmucklera (przycisk „Wykryj z nut” w edytorze ćwiczeń).
[[nodiscard]] gitara::theory::Scale detectKeyFromSong(
    const Song& song,
    gitara::theory::AccidentalMode mode = gitara::theory::AccidentalMode::Auto);

/// Eksportuje utwór do standardowego pliku MIDI (.mid, Format 1) z tempem i metrum (do importu w TuxGuitar).
[[nodiscard]] std::vector<std::uint8_t> exportToMidiBytes(const Song& song);

/// Eksportuje utwór do binarnego formatu Guitar Pro 5 (.gp5) zachowującego struny i progi w TuxGuitar!
[[nodiscard]] std::vector<std::uint8_t> exportToGp5Bytes(const Song& song);

/// Renderuje czytelny tekstowy zapis tabulatury ASCII (6 linii strun E B G D A E).
[[nodiscard]] std::string exportToAsciiTab(const Song& song);

} // namespace gitara::score
