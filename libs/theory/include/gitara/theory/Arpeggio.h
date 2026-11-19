#pragma once

#include "gitara/theory/Chord.h"
#include "gitara/theory/Fretboard.h"
#include "gitara/theory/Scale.h"
#include <string>
#include <vector>

namespace gitara::theory {

/// Wzorzec ogrywania arpeggio (zakładka „Praktyka -> Arpeggio”).
enum class ArpeggioPattern : std::uint8_t {
    FullNeck = 0, ///< Wszystkie składniki akordu na całym gryfie
    CAGED    = 1, ///< 5 pozycji systemu CAGED
    TwoNPS   = 2, ///< 2 dźwięki na strunę (2NPS)
    ThreeNPS = 3, ///< 3 dźwięki na strunę (3NPS – szerokie rozciągnięcia / legato)
    Sweep    = 4  ///< Sweep Picking (1 dźwięk na każdą kolejną strunę – klasyczne kształty 3-, 5- i 6-strunowe)
};

/// Zestaw pozycji pojedynczego arpeggio na gryfie wraz z rolą każdego dźwięku (pryma, tercja, kwinta, septyma...).
struct ArpeggioPositionNode {
    FretPosition position{};
    Note note{};
    Interval intervalFromRoot{};
    bool isRoot{false};
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

/// Generuje rodzinę diatonicznych arpeggio (trójdźwięków lub czterodźwięków septymowych) dla zadanej skali.
[[nodiscard]] std::vector<DiatonicArpeggio> generateDiatonicArpeggios(
    const Scale& scale,
    bool seventhChords = true,
    AccidentalMode mode = AccidentalMode::Auto);

} // namespace gitara::theory
