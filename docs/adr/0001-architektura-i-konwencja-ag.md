# ADR 0001: Architektura warstwowa L0–L3 i międzynarodowa konwencja nazewnictwa A–G

## Status
Zaakceptowane

## Kontekst
Budujemy wysokowydajną aplikację desktopową (Windows Direct2D + ASIO/WASAPI, z gotowością pod Androida) do nauki gry na gitarze z detekcją dźwięku i akordów w czasie rzeczywistym, interaktywnym gryfem, edytorem ćwiczeń z automatycznym tworzeniem tabulatury i eksportem do TuxGuitar (`.gp5` / `.mid`).

## Decyzje
1. **Podział na warstwy z zależnościami wyłącznie w dół:**
   - **L0 (`libs/theory`, `libs/score`)**: Czysty C++20 bez zależności od JUCE i systemu operacyjnego. Zawiera logikę teorii muzyki, mapowania gryfu, maski obszaru (`FretboardMask`), model zapisu tabulatury oraz eksportery `.gp5` i `.mid`.
   - **L1 (`libs/dsp`)**: Algorytmy przetwarzania sygnału (kondycjoner sygnału HPF + Notch 50 Hz + Noise Gate, detektory wysokości YIN i Bitstream Autocorrelation, detektor ataku kostki, chromagram HPCP i syntezator Karplus-Strong). Zero alokacji w wątku audio.
   - **L2 (`libs/engine`)**: Kolejka bezblokadowa SPSC (`LockFreeSpscQueue`), sample-accurate metronom i maszyna stanów weryfikacji ćwiczeń (`ExerciseVerifier`).
   - **L3 (`app/`)**: Natywny interfejs JUCE 8 (`FretboardComponent`, `TunerComponent`, `MidiKeyboardPanel`).
2. **Konwencja nazewnictwa dźwięków A–G (bez H):**
   - Używamy wyłącznie liter `C D E F G A B` (`B` = dawne polskie H, `B♭` = B obniżone o półton). Symbol `H` jest odrzucany przez parser.
   - Każda 7-stopniowa skala ma przypisane 7 kolejnych liter diatonicznych `A–G` bez powtórzeń (np. `G♯` moll harmoniczna ma `F𝄪` na 7. stopniu).
   - Przełącznik **„Znaki chromatyczne: Auto / Bemole (`♭`) / Krzyżyki (`♯`)”** zastępuje błędną kalkę tłumaczeniową „Ostrość: Łagodne / Ostre” z aplikacji referencyjnej.
