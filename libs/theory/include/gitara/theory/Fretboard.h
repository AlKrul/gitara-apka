#pragma once

#include "gitara/theory/Chord.h"
#include "gitara/theory/Scale.h"
#include "gitara/theory/Tuning.h"
#include <array>
#include <optional>
#include <string>
#include <vector>

namespace gitara::theory {

/// Pozycja na gryfie (stringIndex: 0 = najgrubsza struna E2 .. 5 = najcieńsza E4; fret: 0 = pusta struna .. 24).
struct FretPosition {
    int stringIndex{0};
    int fret{0};

    constexpr bool operator==(const FretPosition& other) const noexcept {
        return stringIndex == other.stringIndex && fret == other.fret;
    }
};

/// Maska aktywnego obszaru gryfu – pozwala użytkownikowi wykluczyć (zamaskować) progi, struny
/// lub prostokątne obszary gryfu, które go nie interesują w trybie swobodnej gry, Warsztacie i rejestratorze tabulatury.
class FretboardMask {
public:
    explicit FretboardMask(int stringCount = 6, int maxFret = 24);

    /// Ustawia wszystkie pola gryfu jako aktywne (odmaskowane).
    void allowAll();

    /// Maskuje (wyłącza) pojedynczą pozycję lub ją włącza.
    void setAllowed(int stringIndex, int fret, bool allowed) noexcept;

    /// Maskuje lub odmaskowuje prostokątny obszar gryfu (np. zaznaczony myszką).
    void setRegionAllowed(int minString, int maxString, int minFret, int maxFret, bool allowed) noexcept;

    /// Ogranicza aktywny obszar wyłącznie do podanego zakresu progów [minFret, maxFret], maskując resztę.
    void restrictToFretRange(int minFret, int maxFret) noexcept;

    /// Sprawdza, czy pozycja (struna, próg) jest aktywna (niezamaskowana).
    [[nodiscard]] bool isAllowed(int stringIndex, int fret) const noexcept;

    [[nodiscard]] int stringCount() const noexcept { return strings_; }
    [[nodiscard]] int maxFret() const noexcept { return maxFret_; }

private:
    int strings_{6};
    int maxFret_{24};
    std::vector<bool> allowed_; // rozmiar: strings_ * (maxFret_ + 1)
};

/// System podziału gryfu na pozycje dla skal i arpeggio.
enum class PositionSystem : std::uint8_t {
    None  = 0, ///< Cały gryf
    TNPS  = 1, ///< 3 dźwięki na strunę (3NPS)
    FNPS  = 2, ///< 4 dźwięki na strunę (4NPS)
    CAGED = 3  ///< 5 pozycji systemu CAGED (kształty C, A, G, E, D)
};

/// Tryb ostrości chwytów w zakładce „Akord: Odkrywanie”.
enum class VoicingSharpness : std::uint8_t {
    All   = 0, ///< Zarówno chwyty łagodne, jak i ostre
    Mild  = 1, ///< „Łagodne”: zwarte chwyty jednej ręki bez dużych rozciągnięć i bez tapingu
    Sharp = 2  ///< „Ostre”: szerokie rozciągnięcia dłoni (>= 4 progi) lub tapping prawą ręką (T)
};

/// Pojedynczy chwyt (voicing) akordu w zakładce „Akord: Odkrywanie”.
struct ChordVoicing {
    /// Dla każdej struny: numer progu (0..24) lub -1 jeśli struna tłumiona / niegrana.
    std::vector<int> frets;
    /// Dla każdej struny: true jeśli dźwięk grany jest tappingiem prawą ręką (oznaczany jako T12, T5...).
    std::vector<bool> tapped;
    /// Lista użytych pustych strun (np. "puste E", "puste G, E").
    std::string openStringsNote;
    /// Czy chwyt wykorzystuje tapping prawą ręką.
    bool usesTapping{false};
    /// Rozpiętość przyciśniętych progów (maxFret - minFret).
    int fretSpan{0};
    /// Liczba aktywnych (brzmiących) strun.
    int activeStrings{0};
    /// Czy chwyt należy do kategorii „Ostre” (tapping lub szerokie rozciągnięcie >= 4 progi).
    bool isSharpVoicing{false};
};

/// Poziom trudności wydobycia flażoletu naturalnego (zakładka „Flażolety”).
enum class HarmonicDifficulty : std::uint8_t {
    Easy     = 0, ///< n = 2, 3, 4 (nad progami 12, 7/19, 5/24)
    Medium   = 1, ///< n = 5 (tuż pod progiem 4, nad 9, nad 16)
    Hard     = 2, ///< n = 6, 7 (okolice progu 3.2, 2.7...)
    VeryHard = 3  ///< n >= 8
};

/// Flażolet naturalny w konkretnym węźle struny.
struct NaturalHarmonic {
    int stringIndex{0};
    int harmonicNumber{2};     ///< Numer harmonicznej (n = 2..10)
    int nodeIndex{1};          ///< Numer węzła (k = 1..n-1, względnie pierwszy z n)
    double exactFret{12.0};    ///< Dokładna pozycja węzła w progach (np. 12.0, 7.02, 4.98, 3.86, 3.16)
    Pitch soundingPitch{};     ///< Rzeczywista brzmiąca wysokość flażoletu (np. E3, B3, E4, G♯4)
    Interval intervalOverOpen; ///< Interwał względem pustej struny (1, 5, 3, ♭7, 2, ♯4...)
    HarmonicDifficulty difficulty{HarmonicDifficulty::Easy};
    std::string polishLocation;///< Opis po polsku, np. "nad progiem 12", "tuż pod progiem 4", "między progami 2–3"
};

/// Interaktywna mapa gryfu (22 lub 24 progi) spięta ze strojem i maską obszaru.
class Fretboard {
public:
    explicit Fretboard(Tuning tuning = findTuning("standard"), int fretCount = 24);

    [[nodiscard]] const Tuning& tuning() const noexcept { return tuning_; }
    [[nodiscard]] int stringCount() const noexcept { return tuning_.stringCount(); }
    [[nodiscard]] int fretCount() const noexcept { return fretCount_; }

    /// Zwraca wysokość dźwięku (Pitch) na zadanej strunie i progu.
    /// Jeśli podano kontekst skali, używa pisowni enharmonicznej tej skali!
    [[nodiscard]] Pitch pitchAt(int stringIndex,
                                int fret,
                                AccidentalMode mode = AccidentalMode::Auto,
                                const Scale* scaleContext = nullptr) const;

    /// Zwraca numer MIDI na zadanej strunie i progu.
    [[nodiscard]] int midiAt(int stringIndex, int fret) const noexcept;

    /// Znajduje wszystkie pozycje na gryfie dla zadanego numeru MIDI, z uwzględnieniem opcjonalnej maski.
    [[nodiscard]] std::vector<FretPosition> findPositionsForMidi(
        int midiNote,
        const FretboardMask* mask = nullptr) const;

    /// Wybiera najbardziej ergonomiczną pozycję dla wykrytego dźwięku MIDI (najbliżej poprzedniej pozycji dłoni)
    /// w aktywnym obszarze gryfu (tryb swobodnej gry i tworzenie tabulatury).
    [[nodiscard]] std::optional<FretPosition> selectBestPosition(
        int midiNote,
        std::optional<FretPosition> previousPosition = std::nullopt,
        const FretboardMask* mask = nullptr) const;

    /// Generuje pozycje na gryfie dla wybranej skali i systemu pozycji (Brak / 3NPS / 4NPS / CAGED).
    /// positionIndex: 0..6 dla 3NPS/4NPS (7 stopni skali) lub 0..4 dla CAGED (kształty C, A, G, E, D).
    [[nodiscard]] std::vector<FretPosition> generateScalePositions(
        const Scale& scale,
        PositionSystem system = PositionSystem::None,
        int positionIndex = 0,
        const FretboardMask* mask = nullptr) const;

    /// Generuje grywalne chwyty (voicingi) akordu na gryfie (w tym z pustymi strunami, szerokimi chwytami i tappingiem T).
    [[nodiscard]] std::vector<ChordVoicing> generateChordVoicings(
        const Chord& chord,
        bool includeTapping = true,
        const FretboardMask* mask = nullptr,
        VoicingSharpness sharpness = VoicingSharpness::All,
        int maxStrings = 0) const;

    /// Wylicza wszystkie flażolety naturalne na gryfie (lub wybranej strunie) w zadanym zakresie progów.
    [[nodiscard]] std::vector<NaturalHarmonic> computeNaturalHarmonics(
        std::optional<int> onlyStringIndex = std::nullopt,
        double minFret = 0.0,
        double maxFret = 12.0,
        int maxHarmonicN = 8,
        AccidentalMode mode = AccidentalMode::Auto) const;

private:
    Tuning tuning_;
    int fretCount_{24};
};

} // namespace gitara::theory
