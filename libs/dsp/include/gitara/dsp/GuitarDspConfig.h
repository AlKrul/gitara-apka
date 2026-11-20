#pragma once

#include <array>
#include <cstddef>

namespace gitara::dsp {

/// Centralna konfiguracja kalibracyjna potoku DSP dla gitary w czasie rzeczywistym (V2).
/// Wszystkie progi dB, rozmiary okien czasowych i wagi harmonicznych są zebrane tutaj,
/// aby łatwo dostroić silnik pod konkretny mikrofon, przetwornik lub interfejs audio.
struct GuitarDspConfig {
    // =========================================================================
    // 1. BUFOROWANIE I LATENCJA (REAL-TIME FRAMING)
    // =========================================================================
    double sampleRate{48000.0};        ///< Częstotliwość próbkowania [Hz]
    std::size_t hopSize{256};          ///< Przesunięcie okna: 256 próbek ≈ 5.33 ms @ 48 kHz
    std::size_t maxWindowSize{4096};   ///< Maks. okno dla najniższych strun: 4096 próbek ≈ 85.3 ms @ 48 kHz
    std::size_t minWindowSize{512};    ///< Min. okno: 512 próbek ≈ 10.6 ms

    // =========================================================================
    // 2. PRE-PROCESSING (FILTR PASMOWOPRZEPUSTOWY BUTTERWORTHA 4. RZĘDU)
    // =========================================================================
    /// Dolna granica HPF [Hz]: 62 Hz bezpiecznie przepuszcza Drop D (73.4 Hz), Eb2 (77.8 Hz) i E2 (82.4 Hz),
    /// jednocześnie mocno tłumiąc przydźwięk 50 Hz i uderzenia w pudło.
    double bandpassLowHz{62.0};

    /// Górna granica LPF [Hz]: chroni harmoniczne h=2..6 (kluczowe dla HPS i oktaw przy słabym f0 w mikrofonie),
    /// odcinając jednocześnie wysoki szum i trzask kostki powyżej ~5 kHz.
    double bandpassHighHz{4800.0};

    /// Opcjonalny wąski filtr wycinający przydźwięk sieciowy (50 Hz w Europie, 60 Hz w USA; 0 = wył.)
    double mainsNotchHz{50.0};

    // =========================================================================
    // 3. REPREZENTACJA WIDMOWA: VARIABLE-Q / CONSTANT-Q TRANSFORM (VQT / CQT)
    // =========================================================================
    int minMidiNote{36};               ///< C2 ≈ 65.4 Hz (dolny zapas pod obniżone stroje)
    int maxMidiNote{108};              ///< C8 ≈ 4186 Hz (obejmuje harmoniczne aż po 24. próg E1)
    int binsPerSemitone{2};            ///< 2 biny / półton = siatka co 50 centów (24 biny / oktawę)
    double a4ReferenceHz{440.0};       ///< Kalibracja stroju A4 [Hz]

    /// Zakres wyszukiwania częstotliwości podstawowej f0 gitary
    double fundamentalMinHz{66.0};     ///< Drop D = 73.4 Hz, Eb2 = 77.8 Hz, E2 = 82.4 Hz
    double fundamentalMaxHz{1350.0};   ///< 24. próg E1 = 1318.5 Hz

    /// Parametry Variable-Q z wąskim prawoskrętnym oknem Hanna (2x węższy płat główny -> rozdziela półtony!)
    double qFactor{22.0};
    double vqtGammaHz{6.0};

    // =========================================================================
    // 4. DETEKCJA STARTU NUTY (POSITIVE SPECTRAL FLUX + ODD-HARMONIC HPS + MPM)
    // =========================================================================
    double fluxLookbackMs{48.0};       ///< Δt do obliczenia ΔS(t) = max(0, S(t) - β·S(t - Δt)) [40–60 ms]
    float fluxSubtractFactor{1.12f};   ///< Współczynnik β tłumiący drobne wahania wybrzmiewającej struny
    float minRmsGate{0.0022f};         ///< Bramka szumu RMS (odrzuca ciszę i kliknięcia po wyciszeniu struny)
    float minFluxPeakLinear{0.011f};   ///< Minimalna amplituda przyrostu w ΔS(t) (kalibracja czułości)
    float minFluxScore{0.24f};         ///< Minimalny wynik Log-HPS dla wykrycia nowej nuty
    float minFluxEnergyRatio{0.22f};   ///< Min. udział przyrostu ΔS w energii pasma
    float secondaryNoteMinRatio{0.50f};///< Druga równoczesna nuta w ramce musi mieć ≥ 50% wyniku pierwszej
    int onsetConfirmHops{2};           ///< Potwierdzenie dla czystej ścieżki MPM (2 × 5.3 ms ≈ 10.7 ms)
    int polyOnsetConfirmHops{4};       ///< Potwierdzenie dla ścieżki polifonicznej VQT+Flux (~21 ms)
    int maxSimultaneousOnsets{4};      ///< Maks. liczba nowych nut wykrytych jednocześnie (dwudźwięki/akordy)

    /// Parametry algorytmu HPS (6 harmonicznych: f0 + h=2..6 rozdzielają sąsiednie półtony i oktawy)
    static constexpr int kMaxHarmonics = 6;
    std::array<float, kMaxHarmonics> harmonicWeights{1.15f, 0.85f, 0.72f, 0.54f, 0.36f, 0.24f};
    float logCompressionAlpha{45.0f};
    float hpsProductBonus{0.38f};
    float subharmonicGuardRatio{0.22f};
    float harmonicSubtractFactor{0.90f};

    /// Weryfikator czasowy MPM (McLeod Pitch Method / NSDF)
    float mpmClarityThreshold{0.82f};
    float mpmStrongClarityThreshold{0.87f};

    // =========================================================================
    // 5. ŚLEDZENIE PODTRZYMANIA (SUSTAIN), NOTE OFF (-20 dB) I RE-TRIGGERING
    // =========================================================================
    float releaseDropDb{-20.0f};       ///< Próg wygaszania Note Off względem E_peak (-20 dB = 10% ampl.)
    float noiseFloorDb{-54.0f};        ///< Globalny próg szumu poniżej którego nuta wygasa [dBFS]
    double attackPeakWindowMs{55.0};   ///< Okno początkowe po Note On, w którym aktualizujemy E_peak
    int releaseConfirmHops{3};         ///< Histereza Note Off: 3 ciche ramki (~16 ms) chronią przed wibrato
    double retriggerCooldownMs{75.0};  ///< Minimalny odstęp między dwoma uderzeniami tej samej nuty [ms]
    float retriggerFluxBoostDb{5.0f};  ///< Wymagany skok energii do re-triggeringu aktywnej nuty [dB]
    float retriggerMinFluxRatio{0.32f};
    float lowerHarmonicBleed{0.32f};   ///< Kompensacja przebicia 2./3. harmonicznej z niższych aktywnych strun
    std::size_t maxActiveNotes{6};     ///< Maks. jednocześnie aktywnych dźwięków (6 strun gitary)
};

} // namespace gitara::dsp
