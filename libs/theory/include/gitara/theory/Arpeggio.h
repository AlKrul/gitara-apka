#pragma once

#include "gitara/theory/Chord.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"
#include <string>
#include <vector>

namespace gitara::theory {

/// Wzorzec ogrywania arpeggio (zakładka „Praktyka -> Arpeggio”).
enum class ArpeggioPattern : std::uint8_t {
    FullNeck   = 0, ///< Mapa wszystkich składników akordu na całym gryfie
    CAGED      = 1, ///< 5 pozycji systemu CAGED (kształty C, A, G, E, D bez zdublowanych unisonów)
    TwoNPS     = 2, ///< 2 dźwięki na strunę (2NPS – kostkowanie naprzemienne / economy)
    ThreeNPS   = 3, ///< 3 dźwięki na strunę (3NPS – szerokie rozciągnięcia / legato)
    Sweep      = 4, ///< Sweep Picking (klasyczne kształty 3-, 5- i 6-strunowe z kierunkiem kostkowania)
    Inversions = 5, ///< Poziome inwersje (przewroty) na wybranym zestawie strun (np. G-B-e, D-G-B-e)
    RootOctave = 6  ///< Czyste arpeggio od prymy do prymy (1 lub 2 oktawy w zwartej pozycji)
};

/// Zestaw pozycji pojedynczego arpeggio na gryfie wraz z rolą każdego dźwięku (pryma, tercja, kwinta, septyma...).
struct ArpeggioPositionNode {
    FretPosition position{};
    Note note{};
    Interval intervalFromRoot{};
    bool isRoot{false};
};

/// Pojedynczy przewrót (inwersja) arpeggio na wybranym zestawie strun.
struct ArpeggioInversionBox {
    int inversionIndex{0};
    std::string label;
    int minFret{0};
    int maxFret{0};
    std::vector<ArpeggioPositionNode> nodes;
};

/// Pojedynczy krok odgrywanego arpeggio / sweep pickingu (do tabulatury, animacji gryfu i syntezy).
struct ArpeggioStep {
    int stringIndex{0};
    int fret{0};
    int midi{0};
    double frequencyHz{0.0};
    std::string noteName;
    std::string interval;
    bool isRoot{false};
    std::string technique; ///< "D" (↓), "U" (↑), "H" (hammer-on), "P" (pull-off), "S" (slide)
    bool isRoll{false};    ///< true gdy dwie sąsiednie struny leżą na tym samym progu (przetoczenie palca / roll)
    int segmentIndex{0};   ///< Indeks akordu w przejściu sweep (0, 1, 2...)
};

/// Gotowy kształt (pozycja) Sweep Pickingu z listą węzłów na gryfie i kolejnością kostkowania.
struct SweepShape {
    int positionIndex{0};
    int stringSpan{3};     ///< 3, 5 lub 6 strun
    int minFret{0};
    int maxFret{0};
    std::string label;     ///< np. "Poz. 1 · pr. 5–8"
    std::vector<ArpeggioPositionNode> nodes;
    std::vector<ArpeggioStep> steps; ///< Sekwencja w górę i w dół (albo wg kierunku)
    bool hasFingerRoll{false};
};

/// Pojedynczy segment (akord) w przejściu Sweep Picking.
struct SweepTransitionSegment {
    std::string romanNumeral;
    std::string chordSymbol;
    std::string chordId;
    int rootPc{0};
    int stringSpan{3};
    int minFret{0};
    int maxFret{0};
    std::string transitionHint; ///< Wskazówka techniczna przejścia do kolejnego akordu (np. slide / wspólny dźwięk)
    std::vector<ArpeggioPositionNode> nodes;
    std::vector<ArpeggioStep> steps;
};

/// Kompletne przejście / progresja do ćwiczenia Sweep Pickingu.
struct SweepTransition {
    std::string id;
    std::string name;
    std::string category;
    std::string description;
    std::string practiceTip;
    std::vector<SweepTransitionSegment> segments;
};

/// Metadane presetu przejścia Sweep Picking (do listy w UI).
struct SweepTransitionPresetInfo {
    std::string id;
    std::string name;
    std::string category;
    std::string description;
};

/// Diatoniczne arpeggio zbudowane na kolejnym stopniu skali (np. I=Cmaj7, ii=Dm7, iii=Em7, IV=Fmaj7, V=G7, vi=Am7, vii=Bm7♭5).
struct DiatonicArpeggio {
    int degreeNumber{1};          ///< Stopień skali (1..7)
    std::string romanNumeral;     ///< Oznaczenie rzymskie (np. "Imaj7", "iim7", "V7", "viim7♭5")
    Chord chord{};
};

/// Generuje węzły arpeggio na gryfie dla wybranego akordu, wzorca (FullNeck, CAGED, 2NPS, 3NPS, Sweep) i pozycji.
[[nodiscard]] std::vector<ArpeggioPositionNode> generateArpeggio(
    const Fretboard& fretboard,
    const Chord& chord,
    ArpeggioPattern pattern = ArpeggioPattern::FullNeck,
    int positionIndex = 0,
    AccidentalMode mode = AccidentalMode::Auto,
    const FretboardMask* mask = nullptr);

/// Generuje wszystkie grywalne kształty Sweep Pickingu (3-, 4-, 5- lub 6-strunowe) dla zadanego akordu na całym gryfie.
/// direction: 0 = w górę i w dół, 1 = tylko w górę, 2 = tylko w dół.
/// stringSetOffset: 0 = najwyższe struny (do e1), 1 = zestaw o 1 strunę niżej (np. D-G-B), 2 = o 2 niżej (A-D-G / E-A-D-G), 3 = E-A-D.
[[nodiscard]] std::vector<SweepShape> generateSweepShapes(
    const Fretboard& fretboard,
    const Chord& chord,
    int stringSpan = 3,
    int direction = 0,
    AccidentalMode mode = AccidentalMode::Auto,
    const FretboardMask* mask = nullptr,
    int stringSetOffset = 0);

/// Generuje poziome inwersje (przewroty) arpeggio na wybranym zestawie sąsiednich strun.
/// stringSetIndex: 0 = najwyższe (G-B-e / D-G-B-e), 1 = środkowe (D-G-B / A-D-G-B), 2 = niższe (A-D-G / E-A-D-G).
[[nodiscard]] std::vector<ArpeggioInversionBox> generateArpeggioInversions(
    const Fretboard& fretboard,
    const Chord& chord,
    int stringSetIndex = 0,
    AccidentalMode mode = AccidentalMode::Auto,
    const FretboardMask* mask = nullptr);

/// Generuje czyste 1- lub 2-oktawowe arpeggio od prymy do prymy (Root-to-Root) w zwartej pozycji.
[[nodiscard]] std::vector<ArpeggioPositionNode> generateRootOctaveArpeggio(
    const Fretboard& fretboard,
    const Chord& chord,
    int positionIndex = 0,
    int octaves = 2,
    AccidentalMode mode = AccidentalMode::Auto,
    const FretboardMask* mask = nullptr);

/// Generuje kroki odtwarzania (TAB + kostkowanie) dla dowolnego wzorca arpeggio (FullNeck/CAGED/2NPS/3NPS/Sweep/Inversions/RootOctave).
/// seqStyle: 0 = Prosto (1-3-5-7), 1 = Grupy po 3, 2 = Skoki interwałowe (1-5-3-7).
/// startString/returnString: -1 = auto (od najniższej / pełny powrót), 0..5 = struna (0=E6 najgrubsza .. 5=e1 najcieńsza)
/// od której zaczynamy arpeggio w górę i do której wracamy w dół (kontrola długości frazy pod metronom).
[[nodiscard]] std::vector<ArpeggioStep> buildArpeggioSequenceSteps(
    const Fretboard& fretboard,
    const std::vector<ArpeggioPositionNode>& nodes,
    ArpeggioPattern pattern,
    int direction = 0,
    AccidentalMode mode = AccidentalMode::Auto,
    int seqStyle = 0,
    int startString = -1,
    int returnString = -1);

/// Zwraca listę wbudowanych przejść / progresji Sweep Picking.
[[nodiscard]] const std::vector<SweepTransitionPresetInfo>& allSweepTransitionPresets();

/// Generuje konkretne przejście Sweep Picking transponowane do zadanej tonacji (rootPc) i stroju.
[[nodiscard]] SweepTransition generateSweepTransition(
    const Fretboard& fretboard,
    std::string_view presetId,
    int rootPc,
    int stringSpan = 3,
    int startPositionIndex = 0,
    const Chord* customChord = nullptr,
    const Scale* customScale = nullptr,
    AccidentalMode mode = AccidentalMode::Auto,
    bool diatonicSevenths = false,
    int direction = 0,
    int stringSetOffset = 0);

/// Generuje rodzinę diatonicznych arpeggio (trójdźwięków lub czterodźwięków septymowych) dla zadanej skali.
[[nodiscard]] std::vector<DiatonicArpeggio> generateDiatonicArpeggios(
    const Scale& scale,
    bool seventhChords = true,
    AccidentalMode mode = AccidentalMode::Auto);

} // namespace gitara::theory
