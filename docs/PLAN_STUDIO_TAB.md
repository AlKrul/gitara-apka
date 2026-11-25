# Pełna Specyfikacja i Plan Wdrożenia dla Agenta: `🎼 Studio TAB` (Kompozytor + Player)

> [!IMPORTANT]
> **Instrukcja przekazania dla Agenta Wdrożeniowego (Handoff Context):**
> Ten dokument jest kompletną, samowystarczalną specyfikacją architektoniczną i wykonawczą. Został przygotowany tak, abyś mógł rozpocząć wdrażanie bez przeszukiwania całego repozytorium od zera. Przeczytaj sekcję **„0. Żelazne Reguły i Pułapki Środowiska”** przed wykonaniem jakiejkolwiek edycji plików.

---

## 0. Żelazne Reguły i Pułapki Środowiska (PRZECZYTAJ NAJPIERW)

1. **100% Izolacji — Zero Regresji w istniejących zakładkach (`Wymóg A1`):**
   - `🎼 Studio TAB` powstaje jako **całkowicie osobna główna zakładka** (`state.mainView === 'studio'`).
   - **Nie wolno usuwać, przebudowywać ani psuć żadnej z istniejących zakładek:** `🎵 Praktyka` (`Skala`, `Akord`, `Flażolety`, `Szukaj`, `Arpeggio`, `Sweep Picking`, `Ćwiczenie TAB`), `👂 Kształcenie słuchu`, `🔄 Zmiany`, `🛠️ Warsztat`, `🎛️ Stroik`, `⚙️ Ustawienia`.
   - Obecny endpoint `POST /api/export/gp5` dla starego `Ćwiczenia TAB` musi pozostać nietknięty. Dla Studia tworzymy nowe endpointy `POST /api/studio/export_gp5` oraz `POST /api/studio/export_mid`.
2. **ZAKAZ dopisywania tysięcy linii do `app/ui/DesktopGuiHtml.h` (`~905 KB`, `19 003` linii):**
   - **Pułapka IDE:** Podgląd diffa w edytorze obcina bufor dla plików `> 512 KB` tekstem `... [truncated for diff preview]`. Duże edycje w `DesktopGuiHtml.h` grożą ucięciem pliku przy akceptacji zmian.
   - **Rozwiązanie architektoniczne:** Serwer HTTP w `app/Main.cpp` (`linie 1479–1491`) natywnie serwuje statyczne moduły ES6 i arkusze CSS z katalogu `/web/...` z nagłówkiem `Cache-Control: no-store` (tak samo działa już `/web/js/dsp/pitchPleaseBridge.js`). Cały kod `Studio TAB` umieszczamy w modularnych plikach `web/js/studio/*.js` oraz `web/css/studio.css`.
   - W `app/ui/DesktopGuiHtml.h` dodajemy **wyłącznie minimalny punkt wpięcia (~35–50 linii)**, a po każdej edycji tego pliku obowiązkowo weryfikujemy `wc -l app/ui/DesktopGuiHtml.h` (`>= 19003`) oraz `git diff --stat`.
3. **Język i kompilacja:**
   - Cały interfejs, komentarze i komunikaty w języku **polskim** (nazwy nut anglosaskie `A–G`, bez `H`).
   - Kompilacja i testy weryfikacyjne po każdej fazie:
     - `make all && make test` — buduje `./build/GitaraTests` (`39 273+` testów C++20) oraz `./build/GitaraApp`
     - `./build/GitaraApp --headless-check` — test bezgłowego startu (`exit 0`)
     - `node tests/dsp_web_test.mjs` — `16/16` testów Web DSP (`PitchPlease` WASM)
     - `node tests/studio_web_test.mjs` — nowy zestaw testów jednostkowych modelu, edytora i zegara `Studio TAB`
4. **Spójność ze wszystkimi motywami UI:**
   - `web/css/studio.css` musi korzystać ze zmiennych CSS aplikacji (`var(--bg-main)`, `var(--bg-panel)`, `var(--bg-card)`, `var(--bg-hover)`, `var(--border)`, `var(--accent)`, `var(--accent-glow)`, `var(--text)`, `var(--text-muted)`), tak aby automatycznie pasować do wszystkich **4 stylów wizualnych** (`classic`, `modern`, `rack`, `bento`), obu trybów (`dark` / `light`) oraz obu układów nawigacji (`sidebar` i `topdeck`).

---

## 1. Wizja Produktu: Synteza Songsterr + TuxGuitar + Naszych Silników

Celem wersji testowej `🎼 Studio TAB` jest obniżenie do minimum bariery przenoszenia riffów i kawałków **„z głowy / z gitary na TAB”**, ćwiczenie ich z animowanym kolorowym gryfem i wyeksportowanie gotowego wielościeżkowego pliku **`.GP5`** do dopracowania w **TuxGuitarze** lub wrzucenia na **Songsterr**.

Analiza porównawcza referencyjnych interfejsów **Songsterr Editor** oraz **TuxGuitar** wyznacza dokładny podział tego, co bierzemy, co ulepszamy, a co odrzucamy:

| Element | Jak robi to TuxGuitar | Jak robi to Songsterr | Jak robimy to w `🎼 Studio TAB` (Najlepsze z obu + Nasze silniki) |
| :--- | :--- | :--- | :--- |
| **Paleta narzędzi i menu** | 6 rozwijanych menu z lat 2000. (`Kompozycja`, `Ścieżka`, `Takt`, `Nuta`, `Narzędzia`, `Odtwarzanie`) + mikroskopijne szare ikonki po lewej. | Ciemna, zwarta paleta czytelnych ikon z bursztynowym podświetleniem aktywnych stanów + szybka lupka wyszukiwania komend (`🔍`). | **Ciemna paleta ikon w stylu Songsterra** (podzielona na sekcje: *Rytm, Artykulacja, Legato/Slide/Bend, Struktura, Przekształcenia/Schowek*) + wyszukiwarka akcji `Ctrl+K` + pełne skróty klawiaturowe z TuxGuitara. |
| **Kursor edycji na TAB** | Cienki, mało czytelny kwadracik na białym tle. | Wyraźny **pomarańczowy zaokrąglony kursor slotowy** na przecięciu uderzenia i struny. | **Pomarańczowy kursor slotowy (`Beat + String`)** z podglądem wartości rytmicznej i obsługą wielodźwięków (akordów/power chordów) w jednym pionowym uderzeniu. |
| **Kopiuj / Wklej / Powiel takty** | Wyskakujące modalne okienka Windows (`Kopiuj Od..Do + Wszystkie ścieżki`, `Wstaw N razy + W bieżącym / nowym takcie`), które wybijają z rytmu pracy. | Przyciski schowka na dole palety + osobne kasowanie **`NOTE 🗑`**, **`BEAT 🗑`**, **`BAR 🗑`**. | **Zero okienek modalnych!** Zaznaczanie taktów myszą (`Shift+Klik` / przeciągnięcie) + przełącznik na pasku **`[1 Ścieżka \| Wszystkie ścieżki]`**, tryb wklejania **`[Nadpisz \| Wstaw nowe takty]`**, szybkie przyciski **`Duplikuj ×1 / ×2 / ×4`** (`D`) oraz 3-poziomowe kasowanie **`Nuta / Uderzenie / Takt`**. |
| **Przegląd całego utworu i ścieżek** | **Świetna dolna matryca `Ścieżki × Takty`** (wiersze = ścieżki z `S/M`, kolumny = czerwone/szare prostokąty taktów `1..32` z czarnym wskaźnikiem pozycji). | Brak widoku wielościeżkowej siatki aranżacyjnej na jednym ekranie (przełączasz pojedynczą ścieżkę z listy). | **Nowoczesna dolna Matryca Aranżacji `Ścieżki × Takty` (Mini-DAW)** dla `2–5` ścieżek zespołu: widać jednocześnie gdzie gra Gitara 1, Gitara 2, Bas, Klawisze i jaki klocek bębnów gra w którym takcie! |
| **Gryf, teoria i wejście audio** | Czarny pasek na dole z nieczytelnymi kropkami; zero mikrofonu; zero generatorów sweep/arpeggio. | Dobry odtwarzacz, ale brak podpowiedzi ról harmonicznych przy pisaniu i brak rozpoznawania gry z mikrofonu. | **Nasz kolorowy, geometryczny gryf SVG** (tonika/trójdźwięk/napięcia, tryb 4-strunowy dla basu), **polifoniczny mikrofon (`PitchPlease` WASM)** w trybie krokowym i na żywo, dolne pianino `#midiFooter` oraz **wstawianie gotowych klocków z `Akordów / Arpeggio / Sweep / Zmian`**. |

---

## 2. Mapa Istniejących Zasobów w Kodzie (Punkty Wpięcia dla Agenta)

Nie pisz od nowa syntezatorów ani DSP — podepnij poniższe istniejące funkcje globalne z `app/ui/DesktopGuiHtml.h` i endpointy z `app/Main.cpp`:

### 2.1. Istniejące punkty w `app/ui/DesktopGuiHtml.h`
- **Nawigacja główna (`linie 2300–2323` oraz `switchMainView` w `linii 7862`):**
  - Przyciski `.nav-btn` przełączają `state.mainView` (`'practice'`, `'ear'`, `'changes'`, `'workshop'`, `'tuner'`, `'settings'`).
  - Dodaj przycisk `<button id="navStudioBtn" class="nav-btn" onclick="switchMainView('studio', this)">🎼 Studio TAB</button>` tuż pod `🎵 Praktyka` (lub obok `🛠️ Warsztat`), a w `switchMainView` i `renderStageAndInspector` (`linia 8664`) przekieruj `state.mainView === 'studio'` do `window.StudioTab.render(stage, insp)`.
  - Przy wychodzeniu ze `studio` do innej zakładki wywołuj `window.StudioTab?.stopPlayback?.()`.
- **Syntezatory WebAudio gotowe do użycia przez `studioPlayer.js`:**
  - **Gitara (6 presetów: `acoustic`, `nylon`, `clean`, `crunch`, `lead`, `chorus`):** `pluckGuitarFrequency(freqHz, durationSec, gainScale, trackAsPreview)` (`linia 3634`) — czyta `state.guitarTonePreset` (można tymczasowo podmienić na preset danej ścieżki na czas wywołania, identycznie jak robi to `previewChangesChordAudio` w `linii 15018`).
  - **Flażolety naturalne/sztuczne (`◇` / `◆`):** `playHarmonicBellFrequency(freqHz, durationSec)` (`linia 3664`).
  - **Klawisze / Synth (`piano`, `hammond`, `organ`, `rhodes`, `synth`):** `playKeyboardPresetNote(freqHz, durationSec, gainScale)` (`linia 3699`) — czyta `state.keyboardPreset`.
  - **Perkusja:** `playDrumHit(kind, velocity)` (`linia 14888`, obsługuje `'kick'`, `'snare'`, `'hihat_closed'`, `'hihat_open'` — w `studioDrumBlocks.js` dodaj też lekką syntezę WebAudio dla `'crash'`, `'ride'`, `'tom_high'`, `'tom_low'` korzystającą z tego samego `audioCtx`).
- **Wejścia muzyczne gotowe do przekierowania do kursora `Studio TAB`:**
  - **Polifoniczna detekcja gitary z mikrofonu (`PitchPlease` WASM + `Dechorder`):** most ładowany w `linii 5290` (`/web/js/dsp/pitchPleaseBridge.js`), aktualizujący aktywne nuty MIDI (`activeNotes`) i pojedynczy pitch.
  - **Dolna klawiatura MIDI / QWERTY:** `<footer id="midiFooter" class="midi-footer">` (`linia 2443`).
  - **Dane skali i gryfu z C++:** `fetchJson('/api/fretboard?...')`, `/api/voicings`, `/api/arpeggio` oraz konwerter kroków `exportStepsToExerciseTab(steps, title)` (`linia 14088`), obok którego dodaj `sendStepsToStudioTab(steps, meta)`.

### 2.2. Istniejące punkty w C++20 (`libs/score/include/gitara/score/Score.h`, `libs/score/src/Score.cpp`, `app/Main.cpp`)
- `libs/score/include/gitara/score/Score.h` (`linie 14–74`): zawiera już `DurationValue` (`Whole = -2` .. `ThirtySecond = 3` — rozszerzyć o `SixtyFourth = 4`), `TabNote`, `Beat`, `Measure`, `Track`, `Song`, `exportToMidiBytes`, `exportToGp5Bytes`.
- `libs/score/src/Score.cpp` (`linie 353–553`): zawiera działający szkielet `exportToGp5Bytes(const Song&)` (`FICHIER GUITAR PRO v5.00`), który rozbudujemy o pełną specyfikację efektów, sekcji, volt, wielokanałowego MIDI i perkusji na kanale 10.
- `app/Main.cpp` (`linie 1479–1576`): zawiera już obsługę statycznych plików `/web/*` (`linia 1479`) oraz odczyt pełnego ciała żądania `POST` do `4 MB` (`linia 1370`: `reqBody`), więc przesyłanie dużych projektów JSON do `POST /api/studio/export_gp5` działa od ręki bez zmian w serwerze gniazd TCP.
- Pamiętaj o dodaniu każdego nowego pliku `.cpp` jednocześnie do `Makefile` (`LIB_SRCS`) oraz `CMakeLists.txt` (`gitara_score`).

---

## 3. Architektura Plików `🎼 Studio TAB`

1. **`web/css/studio.css`** — style układu Studia, palety ikon Songsterra, pomarańczowego kursora i matrycy `Ścieżki × Takty`.
2. **`web/js/studio/studioModel.js`** — drzewo `StudioSong`, przeliczanie i walidacja metrum, schowek wielościeżkowy (`1 ścieżka` / `Wszystkie ścieżki`, `Nadpisz` / `Wstaw nowe takty`), przekształcenia strun i wysokości, `Undo/Redo` (50 kroków) i serializacja `.gitara.json`.
3. **`web/js/studio/studioDrumBlocks.js`** — katalog klocków perkusyjnych + rozwinięcie klocka do kroków WebAudio oraz do nut General MIDI Percussion (kanał 10) przy eksporcie `.gp5`.
4. **`web/js/studio/studioPalette.js`** — kompaktowa ciemna paleta ikon a'la Songsterr + szybka wyszukiwarka komend `🔍 Ctrl+K`.
5. **`web/js/studio/studioRenderer.js`** — szybki renderer SVG wielotaktowego TAB (oraz opcjonalnego widoku `Nuty + TAB`) z pomarańczowym kursorem slotowym, oznaczeniami technik, voltami, repetycjami i znacznikami sekcji.
6. **`web/js/studio/studioPlayer.js`** — precyzyjny odtwarzacz wielościeżkowy WebAudio: `Space` = Start/Pauza, `Count-In 1-2-3-4`, `Metronom`, pętla `A–B`, prędkość `25%–150%` + `Auto-Tempo`, klik = skok oraz animacja aktywnego i następnego uderzenia na kolorowym gryfie.
7. **`web/js/studio/studioInputs.js`** — obsługa wszystkich skrótów klawiaturowych, kliknięć gryfu, dolnej klawiatury `#midiFooter`, mikrofonu (`Step-Input`, `Live Rec`, `Graj razem`) oraz wstawiania klocków z `Akord / Arpeggio / Sweep / Zmiany`.
8. **`web/js/studio/studioView.js`** — główny kontroler widoku spinający powyższe moduły z `window.state`.
9. **`libs/score/include/gitara/score/StudioJsonCodec.h` + `libs/score/src/StudioJsonCodec.cpp`** — konwersja JSON `StudioSong` do `gitara::score::Song`.

---

## 4. Układ Interfejsu `🎼 Studio TAB` (5 Stref Roboczych)

### Strefa A — Górny Pasek Transportu, Odtwarzania i Kompozycji
1. **Transport:**
   - `▶ Start / ⏸ Pauza` (**skrót: `Space`** — w `Studio TAB` klawisz `Space` ZAWSZE uruchamia/zatrzymuje odtwarzanie tak jak w TuxGuitarze i Songsterr! Pauzę wstawia `R` lub `Insert`).
   - `⏹ Stop` (`Home` — powrót na początek utworu lub początek pętli `A`).
   - `⏱ Odliczanie (Count-In 1-2-3-4)` — nabicie przed startem odtwarzania lub nagrywania.
   - `🔔 Metronom` — klik metronomu włączony/wyłączony podczas gry zespołu.
2. **Pętla i Trener Prędkości (`F9`):**
   - Przyciski `🔁 Pętla A–B`, `[Ustaw A]`, `[Ustaw B]` (lub zaznaczenie taktów myszą).
   - Prędkość odtwarzania: **`25% · 50% · 75% · 90% · 100% · 125% · 150%`** + suwak płynny + **`⚡ Auto-Tempo`** (`+2..+5 BPM` co czysto zagraną pętlę).
3. **Parametry kompozycji i widoku:**
   - Klikalny badge **`♩ = 120 BPM`** i **`4/4`** (wybór metrum taktu: `4/4`, `3/4`, `2/4`, `6/8`, `12/8`, `5/4`, `7/8` + `Przedtakt / Pickup Bar` + `Swing / Triplet Feel`).
   - Selektor **Tonacji / Skali podpowiedzi** na gryfie + `🔍 Wykryj tonację z nut`.
   - Przełącznik zapisu: **`🎸 Tylko TAB`** (domyślny) | **`🎼 Nuty + TAB`** (pięciolinia z kluczem wiolinowym/basowym nad TAB) + liczba taktów w wierszu (`Auto / 2 / 4 / 6`).
   - Pliki: `📄 Nowy`, `💾 Zapisz .JSON`, `📂 Wczytaj .JSON`, **`⬇ Eksport .GP5 (TuxGuitar / Songsterr)`**, `⬇ .MID`, `⚙ Właściwości (F5: Tytuł, Artysta, Album)`.

### Strefa B — Ciemna Paleta Narzędzi Kompozytora (wg Songsterr Editor)
Kompaktowa paleta ciemnych kafelków z ikonami SVG, bursztynowym podświetleniem aktywnych opcji (`#f59e0b`) i wyszukiwarką komend **`🔍 Szukaj akcji (Ctrl+K)`**:

1. **Rytm, pauzy, łuki, dynamika:** `1/64`..`1/1`, kropka `|.`, podwójna kropka `|..`, pauza (`R`/`Ins`), triola `└3┘` (`T` + `5:4`, `6:4`, `7:4`), ligatura / Tie `⌣` (`L`), fermata `𝄐`, dynamika (`pp..ff`).
2. **Artykulacja, tłumienia i flażolety:** Ghost Note `(0)` (`G`), Akcent `>`, Mocny akcent `^`, Staccato, `let ring`, **`P.M.` (`P`)**, **Dead Note `-X-` (`X`)**, **Flażolet naturalny `◇` (`O`)**, **Flażolet sztuczny/pinch `◆`**.
3. **Legato, Slide'y, Bendy, Kostkowanie, Tapping:** **Hammer-On / Pull-Off `0⌒1` (`H`)**, Legato Slide `0⌒/1`, **Shift Slide `0/1` (`S`)**, Slide-in (`/0`, `\0`), Slide-out (`0\`, `0/`), **Bend `↗` (`B`: `+1/2`, `+1`, `+1½`, `Bend+Release`, `Pre-bend`)**, **Vibrato `~` (`V`)**, szerokie vibrato `w`, kostkowanie **W dół `⊓`** / **W górę `∨`**, rozbicie akordu `↓↑`, `TAP`, przednutka, tremolo picking, trill `tr`.
4. **Struktura utworu:** Repetycja `||:` / `:||` (z licznikiem `×2..×8`), **Volta `┌1` (`1.`, `2.`)**, podwójna kreska `||`, dodaj takt przed `+▬` / po `▬+`, **`sect.` (Znacznik sekcji: `Intro`, `Riff`, `Zwrotka`, `Refren`, `Bridge`, `Solo`, `Outro`)**, **`TXT`**, **`[A]` (symbol akordu)**.
5. **Przekształcenia strun/wysokości, schowek bez modali i 3-poziomowe kasowanie:**
   - **Przenieś nutę na wyższą/niższą strunę bez zmiany wysokości MIDI** (`Alt+↑` / `Alt+↓`), **Pół tonu w górę/dół** (`Shift+→` / `Shift+←`), **Oktawa w górę/dół** (`Shift+↑` / `Shift+↓`), **Przesuń uderzenie w lewo/prawo**, **Transpozycja (`A⇅`)**.
   - **Schowek bez wyskakujących okienek:** przełącznik **`[1 Ścieżka | Wszystkie ścieżki]`**, tryb **`[Nadpisz | Wstaw nowe takty]`**, `✂ Wytnij`, `📋 Kopiuj`, `📥 Wklej`, **`⚡ Duplikuj ×1 (D) · ×2 · ×4`**, `↩ Cofnij (Ctrl+Z)`, `↪ Ponów (Ctrl+Y)`.
   - **3-poziomowe kasowanie:** **`NOTE 🗑` (`Del`)**, **`BEAT 🗑` (`Ctrl+Del`)**, **`BAR 🗑` (`Shift+Del` wyczyść / `Ctrl+Shift+Del` usuń takt)**.

### Strefa C — Główna Scena Wielotaktowego TAB (z pomarańczowym kursorem slotowym)
- Zawijane wiersze taktów z szerokością proporcjonalną do rytmu, wyraźnym **pomarańczowym zaokrąglonym kursorem `(takt, uderzenie, struna)`**, klamrami sekcji, `P.M.`, `let ring`, voltami i wskaźnikiem wypełnienia metrum taktu.
- Podczas odtwarzania: płynny pionowy playhead + auto-scroll + klik w dowolne uderzenie = natychmiastowy skok.

### Strefa D — Dolna Matryca Aranżacji `Ścieżki × Takty` (Mini-DAW wg TuxGuitara)
Obsługuje **kuratorowany zespół 2–5 ścieżek** (startowo 2 aktywne: `🎸 Gitara 1` + `🥁 Perkusja`, dokładane przyciskiem `+ Dodaj ścieżkę` lub `Powiel ścieżkę`):
1. `🎸 Gitara 1 (Rytm / Riff)` — 6 strun, dowolny strój + `Capo`, brzmienie `Crunch`.
2. `🎸 Gitara 2 (Lead / Harmonia)` — 6 strun, brzmienie `Lead`.
3. `🎸 Bas (4-str. E–A–D–G)` — wybranie tej ścieżki **automatycznie przełącza dolny gryf i TAB na 4 struny**!
4. `🎹 Klawisze / Synth` — automatycznie współpracuje z dolną klawiaturą `#midiFooter` (*Piano, Rhodes, Hammond, Organ, Synth*).
5. `🥁 Perkusja (Klocki rytmiczne)` — w siatce taktów wybierasz gotowy klocek na takt/sekcję (`Rock 8th`, `Hard Rock`, `Metal Double-Kick 16th`, `Half-Time Breakdown`, `Punk D-Beat`, `Blues / Shuffle`, `Funk`, `Pop`, `Ballad`, `Break`) + `Auto-Fill co 4/8 taktów` i `Crash na 1`.

### Strefa E — Animowany Kolorowy Gryf + Mikrofon + Klocki Teorii
- Interaktywny gryf SVG pokazujący w tle wybraną skalę/tonację; podczas odtwarzania animuje aktywne nuty pełnym kolorem roli harmonicznej, a następne uderzenie pokazuje przerywanym obrysem.
- **🎤 Mikrofon Krokowy (*Step-Input*)**, **🔴 Nagrywanie Na Żywo do Perkusji**, **🎤 Graj razem (wycisz moją ścieżkę)** oraz wstawianie gotowych klocków z `Akordów`, `Arpeggio`, `Sweep Picking` i `Zmian` (zarówno z inspektora Studia, jak i przyciskiem **`➕ Wyślij do Studia TAB`** w tamtych zakładkach).

---

## 5. Kompletna Mapa Skrótów Klawiaturowych

| Kategoria | Klawisz / Skrót | Akcja |
| :--- | :--- | :--- |
| **Odtwarzanie** | `Space` | **Start / Pauza** odtwarzania |
| | `Home` / `End` | Pierwszy / ostatni takt utworu (`Ctrl+Shift+Left / Right`) |
| | `F9` , `[` , `]` | Włącz pętlę `A–B`, ustaw początek `A` / koniec `B` |
| **Nawigacja** | `←` / `→` , `↑` / `↓` | Poprzednie/następne uderzenie (`→` na końcu dodaje takt), wyższa/niższa struna |
| | `Ctrl+Left` / `Ctrl+Right` | Poprzedni / następny takt |
| | `Ctrl+Shift+Up` / `Down` | Poprzednia / następna ścieżka zespołu |
| **Nuty i Kasowanie** | `0` – `9` | Wpisanie progu `0–24` (dwucyfrowe w oknie `450 ms`) |
| | `R` / `Insert` | Wstaw pauzę |
| | `Del` / `Ctrl+Del` / `Shift+Del` | **NOTE 🗑** (usuń nutę) / **BEAT 🗑** (usuń uderzenie) / **BAR 🗑** (wyczyść takt) |
| **Rytm i Łuki** | `+` / `-` (`Alt+1..7`), `.`, `L` | Długość nuty, kropka, **Ligatura / Tie (`L`)** |
| **Techniki** | `P`, `H`, `S`, `B`, `V`, `X`, `O`, `G` | `PM`, `H/P`, `Slide`, `Bend`, `Vibrato`, `Dead Note`, `Flażolet`, `Ghost Note` |
| **Przekształcenia** | `Alt+Up` / `Alt+Down` | **Przenieś nutę na sąsiednią strunę bez zmiany wysokości MIDI** |
| | `Shift+Right` / `Shift+Left` | **Pół tonu w górę (`+1` próg) / w dół (`-1` próg)** |
| | `Shift+Up` / `Shift+Down` | **Oktawa w górę / oktawa w dół** |
| **Takty i Schowek** | `D` | **Duplikuj bieżący takt / zaznaczony blok** |
| | `Ctrl+C` / `Ctrl+Shift+C` | Kopiuj takty (aktywna ścieżka / **wszystkie ścieżki**) |
| | `Ctrl+V` / `Ctrl+Shift+V` | Wklej takty (nadpisz / **wstaw jako nowe takty**) |
| | `M` / `T` / `Ctrl+K` | Znacznik sekcji `sect.` / tekst `TXT` / wyszukiwarka akcji |
| | `Ctrl+Z` / `Ctrl+Y` | `Cofnij` / `Ponów` (`Undo / Redo`) |

---

## 6. Specyfikacja Binarna GP5.00 w C++ (`libs/score`)

1. **`TabNote`:** dodaj pola z domyślnymi `false / 0`: `deadNote` (`noteType = 0x03`), `ghostNote` (`noteFlags |= 0x04`), `accented` (`0x40`), `heavyAccented` (`0x02`), `palmMute` (`fx2 |= 0x02`), `letRing` (`fx1 |= 0x08`), `vibrato` (`fx2 |= 0x40`), `slideMask` (`fx2 |= 0x08` + 1 bajt), `harmonicType` (`fx2 |= 0x10` + 1 bajt `1` = naturalny), `bendPointsSemitoneCents` (`fx1 |= 0x01` + blok krzywej bendu).
2. **`Beat`:** `dots`, `dynamic` (`1..8`), `pickStroke` (`0/1/2` w bloku `BeatEffects` `beatFlags |= 0x08`), `text` (`beatFlags |= 0x04`).
3. **`Measure`:** `isPickup`, `repeatOpen` (`0x04`), `repeatClose` (`0x10` + bajt liczby powtórzeń), `voltaMask` (`0x20` + bajt maski volt), `sectionMarker` (`0x08` + string + kolor `LE32`), `tripletFeel` (`0/1/2`), oraz zapis metrum (`0x01 | 0x02`) zawsze gdy metrum zmienia się względem poprzedniego taktu.
4. **`Track` i Perkusja Ch.10:** w tablicy 64 kanałów MIDI przypisz programy na właściwych kanałach ścieżek; dla `isPercussion == true` ustaw w nagłówku ścieżki `Flags1 = 0x01`, `Channel = 10`, `Effects channel = 10` i nie przycinaj `fret` do `24` (nuty GM Percussion `35..81`).
5. **Kolejność bajtów nuty w GP5.00 (`Score.cpp:534–539`):** `noteFlags` → `[noteType jeśli 0x20]` → `[dynamic jeśli 0x10]` → `fret` → `noteFlags2 (0)` → `[blok NoteEffects (fx1, fx2, Bend, Grace, TremoloPicking, Slide, Harmonic, Trill) jeśli 0x08]`.

---

## 7. Kolejność Wdrożenia (4 Fazy)

- **Faza 1:** Moduły `studioModel.js`, `studioDrumBlocks.js`, `studioRenderer.js`, `studioPlayer.js`, `studioView.js` + `studio.css`, wpięcie zakładki `🎼 Studio TAB`, matryca `Ścieżki × Takty` (2–5 ścieżek + klocki perkusji), pełny Player „Songsterr-like” z animowanym gryfem i utworem demo + `tests/studio_web_test.mjs`.
- **Faza 2:** `studioPalette.js` + `studioInputs.js` + `Ctrl+K`, pomarańczowy kursor slotowy, wpisywanie z gryfu / klawiatury `0–24` / `#midiFooter`, pełne techniki i struktura (`PM`, `H/P`, `slide`, `bend`, `vibrato`, `sect.`, `Volta`, repetycje), przekształcenia (`Alt+↑/↓` zmiana struny bez zmiany wysokości, `Shift+strzałki`), schowek wielościeżkowy bez modali, 3-poziomowe kasowanie, `Undo/Redo` i `.gitara.json`.
- **Faza 3:** Podpięcie mikrofonu (`PitchPlease` WASM: *Step-Input*, *Live Rec*, *Graj razem*) oraz wstawianie klocków z `Akord / Arpeggio / Sweep / Zmiany` (`➕ Wyślij do Studia TAB`) *(szczegóły wymagań mikrofonu, analizy po nagraniu i wyboru pozycji — patrz Sekcja 8)*.
- **Faza 4:** Rozbudowa `libs/score` (`StudioJsonCodec` + `exportToGp5Bytes` v2 z perkusją Ch.10, sekcjami, voltami i artykulacją), endpointy `POST /api/studio/export_gp5` i `/api/studio/export_mid` w `app/Main.cpp` oraz testy binarne GP5 w `tests/test_main.cpp`.

---

## 8. Dodatek do Fazy 3: Czułość Melodii, Nagrywanie do Metronomu (Offline + Live) i Wybór Pozycji

### 8.1. Wymaganie A — Oddzielna czułość wykrywania melodii + bramka czasu trwania
1. **Stan i serializacja (`web/js/studio/studioModel.js`, `app/ui/DesktopGuiHtml.h`):**
   - Pole ustawień Studia `studioPrefs` (przechowywane w `state.studioPrefs` i zapisywane obok `state.dspThresholdOn` w `saveSettings()`):
     - `detectSensitivity` (domyślnie `0.58`, zakres `0.36–0.88`),
     - `detectHighThresholdOn` (opcjonalny próg dla cienkich strun; gdy `null`, wyliczany z automatycznym marginesem `-0.16` zgodnie z `pitchPleaseBridge.js:106`),
     - `minNoteDurationMs` (domyślnie `80` ms — bramka czasu trwania odcinająca krótkie „strzały” szumu),
     - `liveRecMode` (`'offline' | 'live'`, domyślnie `'offline'`).
2. **UI w Strefie E (panel mikrofonu):**
   - Suwak **„Czułość wykrywania melodii”** (`36%–88%`) z odczytem `%` i podpowiedzią *„wyżej = mniej przypadkowych nut”*.
   - Kontrolka bramki czasu trwania (`minNoteDurationMs`, np. `40–160 ms`) oraz przełącznik trybu nagrywania do metronomu **`[Po nagraniu (Offline) | Na żywo (Live)]`**.
3. **Izolacja progów i poszanowanie podniesionego progu:**
   - Globalny silnik `PitchPlease` oraz jego `autoAdapt` pozostają **w 100% bez zmian** w pozostałych zakładkach (`Stroik`, `Praktyka`, `Kształcenie słuchu`, `Zmiany`).
   - Przy starcie sesji *Step-Input* / *Live Rec* w Studio TAB zapamiętujemy bieżącą konfigurację `livePitchPleaseDetector` (poprzez pomocnicze `getConfig()` w `pitchPleaseBridge.js`) i tymczasowo nadpisujemy ją przez `updateConfig({ thresholdOn: studioPrefs.detectSensitivity, highThresholdOn, ... })`.
   - Gdy użytkownik w Studio podniesie próg dokładnego zbierania ponad `0.68`, `GuitarEventTracker.effectiveThresholds()` w trakcie sesji Studia musi respektować podniesiony próg (nie może go zbijać w dół do `0.68`).
   - Przy zatrzymaniu sesji lub wyjściu z zakładki `Studio TAB` bezwarunkowo przywracamy poprzednią konfigurację globalną (wzorzec `previewChangesChordAudio` z `DesktopGuiHtml.h:15018`). Obsługujemy też sytuację, w której mikrofon jest włączany już po wejściu w tryb nagrywania Studia.
4. **Detekcja polifoniczna / monofoniczna:**
   - Zostawiamy obecne zachowanie silnika `PitchPlease` bez wymuszonego filtra monofonicznego (ewentualne dodatkowe zawężanie dopiero po testach na żywo).
5. **Bramka czasu trwania (`minNoteDurationMs`):**
   - W `GuitarEventTracker` na zdarzeniu `noteOn` pole `durationMs` ma wartość `0` (`pitchPleaseBridge.js:528`), rośnie co ramkę (~`11.6 ms`) w `activeNotes[].durationMs` (`linia 391`) i zamyka się w `noteOff` (`linia 383`).
   - Dlatego w `studioInputs.js` stosujemy kolejkę *pending*: po otrzymaniu `noteOn` odkładamy nutę do bufora oczekującego, zatwierdzamy ją natychmiast, gdy `activeNotes[].durationMs >= minNoteDurationMs`, a odrzucamy jako przypadkowy strzał szumu, jeśli wcześniej nadejdzie `noteOff`.

### 8.2. Wymaganie A2 — Nagrywanie do metronomu: Tryb Offline (`analyzePitchPlease`) + Tryb Live
W panelu mikrofonu współistnieją dwa tryby nagrywania do metronomu/perkusji (do bezpośredniego porównania w testach):
1. **Tryb Offline — rozpoznawanie po zakończeniu nagrania (zalecany do precyzyjnego TAB):**
   - Podczas nagrywania do metronomu/perkusji rejestrujemy zsynchronizowany bufor PCM mikrofonu (z lekkim podglądem wykrywanych dźwięków na gryfie w trakcie gry).
   - Po kliknięciu **Stop** uruchamiamy `analyzePitchPlease(pcm, sampleRate, { model })` (`pitchPleaseBridge.js:12`, `pitchplease.js:137`), analizujące ścieżkę na dokładnych znacznikach `frame.time` (bez opóźnień bufora i narzutu UI w trakcie grania).
   - Po nagraniu można przestawić suwak czułości (`thresholdOn`), próg czasu trwania (`minNoteDurationMs`) lub siatkę kwantyzacji (`1/8`, `1/16`, triole) i przeliczyć TAB ponownie bez ponownego grania.
2. **Tryb Live — rozpoznawanie w locie:**
   - Zbieranie uderzeń bezpośrednio podczas gry z kompensacją opóźnienia poprzez użycie dokładnego `frame.time` (zamiast `emitTime`).

### 8.3. Wymaganie B — Wykrywanie możliwych pozycji po nagraniu + podświetlenie obszarów + edycja po fakcie
1. **Wyliczanie kandydatów pod aktywną ścieżkę:**
   - Kandydatów `{ string, fret }` wyliczamy bezpośrednio ze stroju, kapodastra i liczby strun **aktywnej ścieżki** (działa zarówno dla gitary 6-strunowej i strojów alternatywnych, jak i dla 4-strunowego basu), bez polegania na kolejności `pool[0]` z `/api/fretboard` (która iteruje najpierw po strunie).
2. **Analiza i wybór pozycji po zakończeniu nagrania (obszary `Pozycja 1 / 2 / 3...`):**
   - Po zakończeniu nagrania (lub dla zaznaczonej frazy) analizator znajduje wszystkie spójne, wykonalne ergonomicznie obszary na gryfie (np. *Pozycja 1: pr. 0–4*, *Pozycja 2: pr. 5–8*, *Pozycja 3: pr. 9–12*), dopasowane do zakresu strun/progów ścieżki.
   - Wszystkie komórki należące do kandydatów / wykrytych obszarów podświetlamy na gryfie Strefy E kolorem **bursztynowym (`#f59e0b`)** (wyraźnie odróżnionym od zielonego `#22c55e` aktywnego wykrycia z `DesktopGuiHtml.h:7285`).
   - Nad/obok gryfu wyświetlamy klikalne pola wyboru **`[Pozycja 1]` `[Pozycja 2]` `[Pozycja 3]` ...** — jedno kliknięcie natychmiast przepisuje całą nagraną frazę na wybrany obszar i zapisuje `{ string, fret }` w modelu `TabNote`.
3. **Edycja pojedynczych nut po fakcie + ręczne przenoszenie:**
   - Zaraz po *auto-place* (i po kliknięciu dowolnej nuty) nuta pozostaje zaznaczona, alternatywne pozycje dla jej MIDI świecą się bursztynowo na gryfie, a popover/lista `(struna, próg)` pozwala jednym kliknięciem przenieść wybraną nutę (w tym powtarzające się wysokości w różnych miejscach utworu).
   - Równolegle działa ręczne przenoszenie struny bez zmiany wysokości MIDI (`Alt+↑ / Alt+↓`) oraz zwykła edycja progu/struny.
4. **Persistencja:**
   - Wybrane `{ string, fret }` zapisuje się bezpośrednio w `TabNote`, dzięki czemu przetrwa zarówno zapis/odczyt `.gitara.json`, jak i eksport `.gp5`.

### 8.4. Mapowanie plików i kolejność prac w Fazie 3
- `web/js/dsp/pitchPleaseBridge.js` — dodanie `getConfig()` oraz opcjonalne zachowanie wysokiego progu w `GuitarEventTracker.effectiveThresholds()` podczas pracy Studia.
- `web/js/studio/studioModel.js` — pola `studioPrefs` (`detectSensitivity`, `detectHighThresholdOn`, `minNoteDurationMs`, `liveRecMode`), serializacja i helpery przeliczania pozycji frazy.
- `web/js/studio/studioInputs.js` — podpięcie `PitchPlease` (nadpisanie/przywrócenie configu, kolejka *pending* dla `minNoteDurationMs`), tryb *Step-Input*, nagrywanie do metronomu *Offline* (`analyzePitchPlease`) i *Live*, analizator pozycji frazy + lista kandydatów i edycja pozycji po fakcie.
- `web/js/studio/studioRenderer.js` — bursztynowe (`#f59e0b`) podświetlenie kandydatów i obszarów na gryfie Strefy E + popover wyboru pozycji + kafelki `Pozycja 1 / 2 / 3...`.
- `web/css/studio.css` — style suwaka czułości, przełącznika trybu nagrywania, bursztynowego podświetlenia kandydatów i pigułek wyboru pozycji (spójne przez zmienne CSS ze wszystkimi 4 motywami).
- `tests/studio_web_test.mjs` — testy jednostkowe: mapowanie `detectSensitivity -> thresholdOn`, przywrócenie globalnego progu po wyjściu, bramka `minNoteDurationMs` na kolejce *pending*, generowanie spójnych obszarów `Pozycja 1..N` oraz zmiana pozycji frazy i pojedynczej nuty.
