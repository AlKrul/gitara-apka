/**
 * Centralna konfiguracja kalibracyjna potoku DSP dla gitary w czasie rzeczywistym.
 * Wszystkie progi dB, rozmiary okien czasowych i wagi harmonicznych są w jednym miejscu,
 * aby łatwo dostroić silnik pod konkretny mikrofon, przetwornik lub interfejs audio.
 */
export const DEFAULT_GUITAR_DSP_CONFIG = Object.freeze({
  // =========================================================================
  // 1. BUFOROWANIE I LATENCJA (REAL-TIME FRAMING)
  // =========================================================================
  sampleRate: 48000,         // Częstotliwość próbkowania [Hz] (nadpisywana z AudioContext.sampleRate)
  hopSize: 256,              // Przesunięcie okna: 256 próbek ≈ 5.33 ms @ 48 kHz
  maxWindowSize: 4096,       // Maks. okno dla najniższych strun: 4096 próbek ≈ 85.3 ms @ 48 kHz
  minWindowSize: 512,        // Min. okno dla wysokich progów: 512 próbek ≈ 10.6 ms

  // =========================================================================
  // 2. PRE-PROCESSING (FILTR PASMOWOPRZEPUSTOWY BUTTERWORTHA 4. RZĘDU)
  // =========================================================================
  // 62 Hz bezpiecznie przepuszcza Drop D (73.4 Hz), Eb2 (77.8 Hz) i E2 (82.4 Hz),
  // jednocześnie odcinając przydźwięk 50 Hz i uderzenia w pudło rezonansowe.
  bandpassLowHz: 62.0,
  // 4800 Hz chroni harmoniczne h=2..6 (kluczowe dla HPS i oktaw przy słabym f0 w mikrofonie),
  // odcinając wysoki szum i trzask kostki powyżej ~5 kHz.
  bandpassHighHz: 4800.0,
  mainsNotchHz: 50.0,        // Wąski filtr Notch na przydźwięk sieciowy (50 Hz EU / 60 Hz US; 0 = wył.)

  // =========================================================================
  // 3. REPREZENTACJA WIDMOWA: VARIABLE-Q / CONSTANT-Q TRANSFORM (VQT / CQT)
  // =========================================================================
  minMidiNote: 36,           // C2 ≈ 65.4 Hz
  maxMidiNote: 108,          // C8 ≈ 4186 Hz (pełne harmoniczne aż po 24. próg E1)
  binsPerSemitone: 2,        // 2 biny / półton = siatka co 50 centów (24 biny / oktawę)
  a4ReferenceHz: 440.0,      // Kalibracja stroju A4 [Hz]

  // Zakres wyszukiwania częstotliwości podstawowej f0 gitary
  fundamentalMinHz: 66.0,    // Drop D = 73.4 Hz, Eb2 = 77.8 Hz, E2 = 82.4 Hz
  fundamentalMaxHz: 1350.0,  // 24. próg struny E1 = 1318.5 Hz

  // Parametry Variable-Q z wąskim oknem Hanna (2x węższy płat główny niż Blackman-Harris -> rozdziela półtony!)
  qFactor: 22.0,
  vqtGammaHz: 6.0,

  // =========================================================================
  // 4. DETEKCJA STARTU NUTY (POSITIVE SPECTRAL FLUX + ODD-HARMONIC HPS + MPM)
  // =========================================================================
  fluxLookbackMs: 48.0,      // Δt dla ΔS(t) = max(0, S(t) - β·S(t - Δt)) [40–60 ms]
  fluxSubtractFactor: 1.12,  // Współczynnik β eliminujący drobne wahania wybrzmiewających strun
  minRmsGate: 0.0022,        // Bramka szumu RMS (odrzuca ciszę i kliknięcia po wyciszeniu struny)
  minFluxPeakLinear: 0.011,  // Minimalny przyrost w ΔS(t) (kalibracja czułości mikrofonu!)
  minFluxScore: 0.24,        // Minimalny wynik Log-HPS
  minFluxEnergyRatio: 0.22,  // Minimalny udział przyrostu ΔS w energii pasma
  secondaryNoteMinRatio: 0.50, // Druga równoczesna nuta w ramce musi mieć ≥ 50% wyniku HPS pierwszej
  onsetConfirmHops: 2,       // 2 ramki potwierdzające (~10.7 ms) — szybka reakcja przy czystej filtracji
  maxSimultaneousOnsets: 4,  // Maks. liczba nowych nut wykrytych jednocześnie (dwudźwięki/akordy)

  // Wagi 6 harmonicznych (h=1..6): wyraźny priorytet f0 (1.15) chroni przed fałszywą sub-oktawą w akordach,
  // a h=3..6 jednoznacznie rozdzielają sąsiednie półtony na wybrzmiewaniu!
  harmonicWeights: [1.15, 0.85, 0.72, 0.54, 0.36, 0.24],
  logCompressionAlpha: 45.0,
  hpsProductBonus: 0.38,
  subharmonicGuardRatio: 0.22,
  harmonicSubtractFactor: 0.90,

  // Weryfikator MPM w dziedzinie czasu (precyzja ±0.2 ct i bezbłędne oktawy dla pojedynczych strun i szybkich przebiegów)
  mpmClarityThreshold: 0.82,
  mpmStrongClarityThreshold: 0.87,
  polyOnsetConfirmHops: 4,   // Ścieżka polifoniczna VQT+Flux wymaga 4 ramek (~21 ms), by nie strzelać na 15-ms przejściu palca na jednej strunie!

  // =========================================================================
  // 5. PODTRZYMANIE (SUSTAIN), NOTE OFF (-20 dB) I RE-TRIGGERING
  // =========================================================================
  releaseDropDb: -20.0,      // Próg wygaszania Note Off względem E_peak (-20 dB = 10% amplitudy)
  noiseFloorDb: -54.0,       // Globalny próg szumu tła [dBFS]
  attackPeakWindowMs: 55.0,  // Okno po Note On, w którym aktualizujemy E_peak po ataku kostki
  releaseConfirmHops: 3,     // Histereza Note Off (3 ciche ramki ≈ 16 ms — odporność na wibrato)
  retriggerCooldownMs: 75.0, // Minimalny odstęp między ponownymi uderzeniami tej samej nuty [ms]
  retriggerFluxBoostDb: 5.0, // Wymagany skok energii do re-triggeringu aktywnej nuty [dB]
  retriggerMinFluxRatio: 0.32,
  lowerHarmonicBleed: 0.32,  // Kompensacja przebicia 2./3. harmonicznej z niższych aktywnych strun
  maxActiveNotes: 6          // Maks. jednocześnie aktywnych dźwięków (6 strun gitary)
});

export function createGuitarDspConfig(overrides = {}) {
  return Object.assign({}, DEFAULT_GUITAR_DSP_CONFIG, overrides || {});
}
