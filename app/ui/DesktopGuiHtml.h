#pragma once

namespace gitara::app::ui {

inline constexpr const char* kDesktopGuiHtml = R"HTML(<!DOCTYPE html>
<html lang="pl">
<head>
<meta charset="UTF-8">
<title>Apka Gitarowa – Studio Treningowe (A–G)</title>
<style>
:root {
  --bg-main: #110e0c;
  --bg-panel: #181411;
  --bg-card: #211c18;
  --bg-hover: #2c2520;
  --border: #332b24;
  --accent: #f39237;
  --accent-glow: rgba(243, 146, 55, 0.22);
  --green: #4ade80;
  --yellow: #facc15;
  --red: #f87171;
  --text: #f5f2eb;
  --text-muted: #9ca3af;
}
* { box-sizing: border-box; margin: 0; padding: 0; user-select: none; }
body {
  background: var(--bg-main);
  color: var(--text);
  font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
  height: 100vh;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}
.app-shell {
  display: flex;
  flex: 1;
  min-height: 0;
}
/* Boczna nawigacja */
.sidebar {
  width: 205px;
  flex-shrink: 0;
  background: var(--bg-panel);
  border-right: 1px solid var(--border);
  display: flex;
  flex-direction: column;
  padding: 12px 10px;
  gap: 5px;
  overflow-y: auto;
}
.brand {
  font-size: 16px;
  font-weight: 800;
  color: var(--accent);
  padding: 2px 10px 10px;
  border-bottom: 1px solid var(--border);
  margin-bottom: 4px;
  white-space: nowrap;
}
.nav-btn {
  background: transparent;
  color: var(--text-muted);
  border: none;
  text-align: left;
  padding: 9px 12px;
  border-radius: 8px;
  font-size: 13.5px;
  font-weight: 500;
  cursor: pointer;
  transition: 0.15s;
  white-space: nowrap;
}
.nav-btn:hover { background: var(--bg-hover); color: var(--text); }
.nav-btn.active {
  background: var(--accent-glow);
  color: var(--accent);
  border-left: 3px solid var(--accent);
  font-weight: 700;
}
.audio-status-box {
  margin-top: auto;
  background: var(--bg-card);
  border: 1px solid var(--border);
  border-radius: 10px;
  padding: 10px;
  font-size: 12px;
}
.live-dot {
  display: inline-block;
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--text-muted);
  margin-right: 6px;
}
.live-dot.on { background: var(--green); box-shadow: 0 0 8px var(--green); }

/* Główna przestrzeń robocza */
.workspace {
  flex: 1;
  display: flex;
  flex-direction: column;
  min-width: 0;
  min-height: 0;
  overflow: hidden;
}
.topbar {
  min-height: 52px;
  height: auto;
  background: var(--bg-panel);
  border-bottom: 1px solid var(--border);
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  padding: 6px 14px;
  gap: 8px 12px;
}
.subtabs {
  display: flex;
  flex-wrap: wrap;
  gap: 3px;
  background: var(--bg-main);
  padding: 3px;
  border-radius: 9px;
  border: 1px solid var(--border);
}
.subtab-btn {
  background: transparent;
  color: var(--text-muted);
  border: none;
  padding: 6px 12px;
  border-radius: 6px;
  font-size: 12.5px;
  font-weight: 600;
  cursor: pointer;
  white-space: nowrap;
}
.subtab-btn:hover { color: var(--text); }
.subtab-btn.active { background: var(--accent); color: #120e0a; }

.live-tuner-pill {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 8px;
  background: var(--bg-card);
  border: 1px solid var(--border);
  padding: 4px 10px;
  border-radius: 8px;
  font-size: 12px;
  white-space: nowrap;
}
.live-note-badge {
  font-size: 16px;
  font-weight: 800;
  color: var(--accent);
  min-width: 38px;
  text-align: center;
  white-space: nowrap;
}

.content-split {
  flex: 1;
  display: flex;
  min-width: 0;
  min-height: 0;
  overflow: hidden;
}
.main-stage {
  flex: 1;
  min-width: 0;
  padding: 14px 16px;
  overflow-y: auto;
  overflow-x: hidden;
  display: flex;
  flex-direction: column;
  gap: 12px;
}
.right-inspector {
  width: 320px;
  flex-shrink: 0;
  background: var(--bg-panel);
  border-left: 1px solid var(--border);
  padding: 14px;
  overflow-y: auto;
  display: flex;
  flex-direction: column;
  gap: 12px;
}

@media (max-width: 1360px) {
  .sidebar { width: 180px; padding: 10px 8px; }
  .right-inspector { width: 275px; padding: 12px; }
  .main-stage { padding: 10px 12px; }
  .subtab-btn { padding: 5px 9px; font-size: 12px; }
}
@media (max-width: 1060px) {
  .sidebar { width: 162px; }
  .right-inspector { width: 245px; }
}
.section-label {
  font-size: 11px;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.08em;
  color: var(--text-muted);
  margin-bottom: 6px;
}
.btn-grid-6 { display: grid; grid-template-columns: repeat(6, 1fr); gap: 5px; }
.btn-grid-3 { display: grid; grid-template-columns: repeat(3, 1fr); gap: 5px; }
.btn-grid-2 { display: grid; grid-template-columns: repeat(2, 1fr); gap: 6px; }

.chip-btn {
  background: var(--bg-card);
  color: var(--text);
  border: 1px solid var(--border);
  border-radius: 7px;
  padding: 8px 6px;
  font-size: 13px;
  font-weight: 600;
  cursor: pointer;
  text-align: center;
  transition: 0.12s;
}
.chip-btn:hover { border-color: var(--accent); }
.chip-btn.active {
  background: var(--accent);
  color: #120e0a;
  border-color: var(--accent);
}
.round-pill {
  width: 34px;
  height: 34px;
  border-radius: 50%;
  background: var(--bg-card);
  color: var(--text);
  border: 1px solid var(--border);
  font-size: 12px;
  font-weight: 700;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}
.round-pill.active {
  background: var(--accent);
  color: #120e0a;
  border-color: var(--accent);
}
.action-btn {
  background: var(--accent);
  color: #120e0a;
  border: none;
  border-radius: 8px;
  padding: 10px 14px;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
}
.action-btn.secondary {
  background: var(--bg-card);
  color: var(--text);
  border: 1px solid var(--border);
}
.action-btn:hover { filter: brightness(1.08); }
select, input[type="number"] {
  width: 100%;
  background: var(--bg-card);
  color: var(--text);
  border: 1px solid var(--border);
  border-radius: 7px;
  padding: 8px 10px;
  font-size: 13px;
}

/* Karta gryfu – rozciągalna w pionie z zachowaniem idealnie okrągłych znaczników 1:1 */
.fretboard-card {
  background: var(--bg-panel);
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 14px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.fretboard-toolbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 12px;
  color: var(--text-muted);
  flex-wrap: wrap;
  gap: 8px;
}
.fretboard-resizable-wrap {
  width: 100%;
  overflow-x: auto;
  overflow-y: hidden;
  border-radius: 10px;
  border: 1px solid #2b231d;
  background: linear-gradient(180deg, #15110e 0%, #100d0a 100%);
  box-shadow: inset 0 1px 0 rgba(255,255,255,0.04), 0 8px 24px rgba(0,0,0,0.45);
  position: relative;
  cursor: grab;
  padding-bottom: 4px;
}
.fretboard-resizable-wrap:active {
  cursor: grabbing;
}
#fretboardSvgHost {
  display: inline-block;
  min-width: max-content;
}
.fb-node {
  cursor: pointer;
  transition: filter 0.12s ease;
}
.fb-node:hover {
  filter: brightness(1.22) drop-shadow(0 0 6px rgba(243, 146, 55, 0.55));
}
.fb-hit-rect {
  fill: transparent;
  cursor: pointer;
}
.fb-hit-rect:hover {
  fill: rgba(243, 146, 55, 0.06);
}

/* Siatka kart akordów w stylu ze zrzutu ekranu (5 kolumn, czyste białe linie 6 strun) */
.chord-cards-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(175px, 1fr));
  gap: 14px;
}
.chord-box-card {
  background: #191512;
  border: 1px solid #2d2620;
  border-radius: 10px;
  padding: 14px 12px 12px;
  cursor: pointer;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
  transition: 0.15s;
}
.chord-box-card:hover, .chord-box-card.selected {
  border-color: var(--accent);
  box-shadow: 0 0 0 1px var(--accent);
}
.scale-func-card {
  transition: transform 0.12s ease, border-color 0.12s ease, box-shadow 0.12s ease, background 0.12s ease, opacity 0.15s ease;
}
.scale-func-card.playing-now {
  border-color: #4ade80 !important;
  box-shadow: 0 0 0 2px #4ade80, 0 0 14px rgba(74, 222, 128, 0.45) !important;
  background: rgba(74, 222, 128, 0.12) !important;
  transform: translateY(-2px);
}
.chord-box-sub {
  font-size: 11px;
  color: #8c8277;
  text-align: center;
  min-height: 15px;
}
.chord-box-title {
  font-size: 12px;
  font-weight: 700;
  color: #d6cec2;
  text-align: center;
}

/* Wiersze list (Flażolety, Szukaj) */
.harmonics-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 8px;
}
.list-row {
  background: var(--bg-card);
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 9px 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  cursor: pointer;
  font-size: 12px;
  transition: 0.12s;
}
.list-row:hover { border-color: var(--accent); }
.list-row.selected {
  border-color: var(--accent);
  background: rgba(243, 146, 55, 0.12);
}

/* Kształcenie słuchu – wyśrodkowany układ 2-kolumnowy z czytelnymi kafelkami */
.ear-config-layout {
  display: flex;
  gap: 28px;
  align-items: flex-start;
  width: 100%;
  max-width: 1360px;
  margin: 0 auto;
}
.ear-config-main {
  flex: 1;
  min-width: 0;
  display: flex;
  flex-direction: column;
  gap: 24px;
}
.ear-summary-box {
  width: 340px;
  flex-shrink: 0;
  background: #171310;
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 12px;
}
/* Praktyka skal – pełny, wyśrodkowany układ studyjny z kartami i sticky podsumowaniem */
.sp-shell {
  width: 100%;
  max-width: 1480px;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 16px;
}
.sp-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 14px;
  flex-wrap: wrap;
}
.sp-grid {
  display: grid;
  grid-template-columns: minmax(0, 1fr) 360px;
  gap: 20px;
  align-items: start;
}
.sp-cards-col {
  display: flex;
  flex-direction: column;
  gap: 16px;
  min-width: 0;
}
.sp-card {
  background: var(--bg-panel);
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 18px 20px;
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.sp-card-head {
  display: flex;
  justify-content: space-between;
  align-items: center;
  gap: 12px;
  padding-bottom: 10px;
  border-bottom: 1px solid #26201b;
}
.sp-card-head h3 {
  font-size: 16px;
  font-weight: 800;
  margin: 0;
  color: var(--text);
}
.sp-two-col {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 16px 20px;
}
.sp-dir-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 6px;
}
.sp-dir-btn {
  background: #16120f;
  color: #a8a096;
  border: 1px solid #2b241e;
  border-radius: 8px;
  padding: 9px 10px;
  font-size: 12px;
  font-weight: 600;
  cursor: pointer;
  white-space: nowrap;
  transition: 0.12s;
  text-align: center;
}
.sp-dir-btn:hover { color: var(--text); border-color: #5c4b3c; }
.sp-dir-btn.active {
  background: var(--accent);
  border-color: var(--accent);
  color: #140e08;
  font-weight: 700;
}
.sp-summary-card {
  position: sticky;
  top: 0;
  background: var(--bg-panel);
  border: 1px solid var(--border);
  border-radius: 12px;
  padding: 20px;
  display: flex;
  flex-direction: column;
  gap: 12px;
}
.sp-summary-pills {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 8px;
  background: #130f0c;
  border: 1px solid #26201b;
  border-radius: 9px;
  padding: 10px 12px;
}
.sp-summary-pill {
  display: flex;
  flex-direction: column;
  gap: 2px;
}
.sp-summary-pill span {
  font-size: 10px;
  text-transform: uppercase;
  letter-spacing: 0.6px;
  color: var(--text-muted);
}
.sp-summary-pill strong {
  font-size: 12.5px;
  color: var(--text);
  font-weight: 700;
}
@media (max-width: 1120px) {
  .sp-grid { grid-template-columns: 1fr; }
  .sp-summary-card { position: static; }
  .sp-two-col { grid-template-columns: 1fr; }
}
.ear-intervals-grid {
  display: grid;
  grid-template-columns: repeat(7, 92px);
  gap: 8px;
}
.ear-interval-tile {
  width: 92px;
  height: 64px;
  background: #181411;
  border: 1px solid #2e2721;
  border-radius: 8px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 6px;
  cursor: pointer;
  text-align: center;
  gap: 3px;
  transition: 0.12s;
}
.ear-interval-tile:hover {
  border-color: #6b5744;
}
.ear-interval-tile.selected {
  background: rgba(243, 146, 55, 0.10);
  border: 1.5px solid var(--accent);
}
.ear-interval-tile .iv-name {
  font-size: 11.5px;
  font-weight: 700;
  color: #a8a096;
  line-height: 1.15;
}
.ear-interval-tile .iv-code {
  font-size: 11px;
  font-weight: 700;
  color: #787067;
}
.ear-interval-tile.selected .iv-name,
.ear-interval-tile.selected .iv-code {
  color: var(--accent);
}
.ear-chord-pill {
  min-width: 96px;
  height: 44px;
  padding: 0 16px;
  background: #181411;
  border: 1px solid #2e2721;
  border-radius: 8px;
  color: #a8a096;
  font-size: 13px;
  font-weight: 700;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
}
.ear-chord-pill.selected {
  background: rgba(243, 146, 55, 0.10);
  border: 1.5px solid var(--accent);
  color: var(--accent);
}
.ear-seg-row {
  display: flex;
  background: #16120f;
  border: 1px solid #2b241e;
  border-radius: 8px;
  overflow: hidden;
}
.ear-seg-btn {
  flex: 1;
  background: transparent;
  color: #a8a096;
  border: none;
  padding: 10px 14px;
  font-size: 12.5px;
  font-weight: 600;
  cursor: pointer;
  transition: 0.12s;
}
.ear-seg-btn:hover { color: var(--text); }
.ear-seg-btn.active {
  background: var(--accent);
  color: #140e08;
  font-weight: 700;
}
.ear-toggle-switch {
  width: 44px;
  height: 24px;
  border-radius: 12px;
  background: #2e2721;
  position: relative;
  cursor: pointer;
  transition: 0.15s;
  border: none;
}
.ear-toggle-switch.on { background: var(--accent); }
.ear-toggle-switch::after {
  content: '';
  position: absolute;
  top: 3px;
  left: 3px;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: #9ca3af;
  transition: 0.15s;
}
.ear-toggle-switch.on::after {
  left: 23px;
  background: #140e08;
}
/* Aktywny ekran Treningu Słuchu (Quiz / Nauka / Zagraj / Akordy) */
.ear-trainer-screen {
  max-width: 760px;
  margin: 0 auto;
  width: 100%;
  display: flex;
  flex-direction: column;
  gap: 22px;
  padding: 12px 0;
}
.ear-answer-grid {
  display: grid;
  grid-template-columns: repeat(3, 220px);
  justify-content: center;
  gap: 12px;
}
.ear-answer-btn {
  width: 220px;
  height: 68px;
  background: #191512;
  border: 1px solid #2e2721;
  border-radius: 10px;
  color: var(--text);
  cursor: pointer;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 4px;
  transition: 0.12s;
}
.ear-answer-btn:hover { border-color: var(--accent); }
.ear-answer-btn.selected {
  border: 2px solid var(--accent);
  background: rgba(243, 146, 55, 0.14);
}
.ear-answer-btn.correct {
  border: 2px solid var(--green);
  background: rgba(74, 222, 128, 0.16);
}
.ear-answer-btn.wrong {
  border: 2px solid var(--red);
  background: rgba(248, 113, 113, 0.16);
}
.changes-chart-card.active-playing {
  border: 2px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.22) !important;
  box-shadow: 0 0 14px rgba(243, 146, 55, 0.35) !important;
}
.changes-arr-bar.active-playing {
  border: 1.5px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.08) !important;
}
.changes-beat-slot.active-beat {
  border: 2px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.28) !important;
}
.tuner-stage-card {
  background: #16120f;
  border: 1px solid var(--border);
  border-radius: 14px;
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 24px;
  gap: 18px;
}
.tuner-strings-row {
  display: flex;
  gap: 10px;
  margin-top: 6px;
}
.tuner-string-btn {
  width: 60px;
  height: 58px;
  background: #1e1915;
  border: 1px solid #332b24;
  border-radius: 8px;
  color: var(--text);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  gap: 2px;
}
.tuner-string-btn:hover { border-color: var(--accent); }
.tuner-string-btn.active {
  border-color: var(--accent);
  background: var(--accent-glow);
  color: var(--accent);
}

/* Dolny pasek wirtualnej klawiatury MIDI */
.midi-footer {
  min-height: 118px;
  height: auto;
  flex-shrink: 0;
  background: var(--bg-panel);
  border-top: 1px solid var(--border);
  display: flex;
  flex-direction: column;
  padding: 6px 14px 8px;
  gap: 5px;
}
.midi-bar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 12px;
}
.midi-keys {
  height: 76px;
  min-height: 76px;
  position: relative;
  display: flex;
  background: #0d0b09;
  border-radius: 6px;
  padding: 2px;
  overflow: hidden;
}
.white-key-wrap {
  flex: 1;
  position: relative;
  height: 100%;
  display: flex;
}
.piano-key.white {
  flex: 1;
  height: 100%;
  margin: 0 1px;
  background: linear-gradient(180deg, #f8fafc 0%, #e2e8f0 100%);
  color: #0f172a;
  border: 1px solid #94a3b8;
  border-radius: 0 0 6px 6px;
  display: flex;
  flex-direction: column;
  justify-content: flex-end;
  align-items: center;
  padding-bottom: 5px;
  font-size: 11.5px;
  font-weight: 800;
  letter-spacing: -0.2px;
  cursor: pointer;
  z-index: 1;
}
.piano-key.sharp {
  position: absolute;
  top: 0;
  right: -32%;
  width: 64%;
  height: 62%;
  background: linear-gradient(180deg, #27272a 0%, #09090b 100%);
  color: #f8fafc;
  border: 1px solid #3f3f46;
  border-top: none;
  border-radius: 0 0 5px 5px;
  box-shadow: 0 3px 6px rgba(0,0,0,0.65);
  display: flex;
  flex-direction: column;
  justify-content: flex-end;
  align-items: center;
  padding-bottom: 4px;
  font-size: 10.5px;
  font-weight: 800;
  letter-spacing: -0.2px;
  cursor: pointer;
  z-index: 3;
}
.piano-key.scale-member-key.white {
  background: linear-gradient(180deg, #fff7ed 0%, #fed7aa 100%);
  border-color: #f39237;
}
.piano-key.scale-member-key.sharp {
  background: linear-gradient(180deg, #543b26 0%, #3d2a1b 100%);
  border-color: #b47842;
}
.piano-key.scale-root-key.white,
.piano-key.scale-root-key.sharp {
  background: linear-gradient(180deg, #ff9f43 0%, #e57c1f 100%);
  color: #160f08;
  border-color: #fde68a;
}
.piano-key.guitar-octave-sync {
  background: linear-gradient(180deg, #52e58b 0%, #22c55e 100%) !important;
  color: #08170e !important;
  border-color: #86efac !important;
  box-shadow: 0 0 8px rgba(74, 222, 128, 0.55) !important;
}
.piano-key.active,
.piano-key.guitar-sync {
  background: #4ade80 !important;
  color: #05140b !important;
  border-color: #bbf7d0 !important;
  box-shadow: 0 0 12px rgba(74, 222, 128, 0.85) !important;
}
.piano-key.guitar-chord-sync {
  background: linear-gradient(180deg, #fdba74 0%, #f39237 100%) !important;
  color: #120e0a !important;
  border-color: #fed7aa !important;
}
.kbd-tag {
  font-size: 8.5px;
  font-weight: 500;
  color: #64748b;
  opacity: 0.52;
  margin-bottom: 2px;
}
.piano-key.sharp .kbd-tag {
  color: #a1a1aa;
  opacity: 0.50;
}
.piano-key.active .kbd-tag,
.piano-key.guitar-sync .kbd-tag,
.piano-key.guitar-octave-sync .kbd-tag,
.piano-key.scale-root-key .kbd-tag,
.piano-key.guitar-chord-sync .kbd-tag {
  color: #1e293b;
  opacity: 0.58;
}
.ex-notes-table {
  width: 100%;
  border-collapse: collapse;
  font-size: 12.5px;
}
.ex-notes-table th {
  text-align: left;
  color: var(--text-muted);
  font-size: 11px;
  font-weight: 600;
  padding: 6px 8px;
  border-bottom: 1px solid #2b231d;
}
.ex-notes-table td {
  padding: 6px 8px;
  border-bottom: 1px solid #211a15;
  vertical-align: middle;
}
.ex-notes-table tr.ex-active-row {
  background: rgba(243, 146, 55, 0.10);
}
.ex-notes-table select,
.ex-notes-table input {
  padding: 5px 7px;
  font-size: 12px;
  background: #171310;
  border: 1px solid #2e2620;
  color: var(--text);
  border-radius: 6px;
}
</style>
</head>
<body>
<div class="app-shell">
  <!-- Lewy pasek nawigacji -->
  <aside class="sidebar">
    <div class="brand">🎸 Apka Gitarowa</div>
    <button class="nav-btn active" onclick="switchMainView('practice', this)">🎵 Praktyka</button>
    <button class="nav-btn" onclick="switchMainView('ear', this)">👂 Kształcenie słuchu</button>
    <button class="nav-btn" onclick="switchMainView('changes', this)">🔄 Zmiany</button>
    <button class="nav-btn" onclick="switchMainView('workshop', this)">🛠️ Warsztat</button>

    <!-- Dolna sekcja lewego paska: Audio + Stroik i Ustawienia na samym dole po lewej -->
    <div class="audio-status-box">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;">
        <span style="font-weight:700;">Urządzenia Audio</span>
        <button class="chip-btn" style="padding:2px 6px;font-size:10px;" onclick="refreshAudioDevices(true)">🔄 Wykryj</button>
      </div>
      <div style="margin-bottom:6px;">
        <span id="micDot" class="live-dot"></span>
        <span id="micStatusText">Wejście wył.</span>
      </div>
      <div style="font-size:10px;color:var(--text-muted);margin-bottom:2px;">Wejście (Mikrofon / Interfejs):</div>
      <select id="audioInputSelect" style="margin-bottom:6px;font-size:11px;padding:4px 6px;" onchange="onAudioInputChanged(this.value)">
        <option value="default">Domyślne wejście systemowe</option>
      </select>
      <div style="font-size:10px;color:var(--text-muted);margin-bottom:2px;">Odtwarzanie (Głośniki / Słuchawki):</div>
      <select id="audioOutputSelect" style="margin-bottom:8px;font-size:11px;padding:4px 6px;" onchange="onAudioOutputChanged(this.value)">
        <option value="default">Domyślne urządzenie odtwarzania</option>
      </select>
      <button id="toggleMicBtn" class="action-btn" style="width:100%;padding:7px;font-size:12px;" onclick="toggleLiveAudio()">
        🎤 Włącz mikrofon
      </button>
    </div>

    <div style="border-top:1px solid var(--border);padding-top:8px;margin-top:6px;display:flex;flex-direction:column;gap:4px;">
      <button class="nav-btn" onclick="switchMainView('tuner', this)">🎛️ Stroik (Tuner)</button>
      <button class="nav-btn" onclick="switchMainView('settings', this)">⚙️ Postępy i Ustawienia</button>
    </div>
  </aside>

  <!-- Główna część -->
  <main class="workspace">
    <!-- Górny pasek zakładek -->
    <header class="topbar">
      <div id="practiceSubtabs" class="subtabs">
        <button class="subtab-btn active" data-subtab="scale" onclick="switchPracticeSubtab('scale', this)">Skala</button>
        <button class="subtab-btn" data-subtab="scale_practice" onclick="switchPracticeSubtab('scale_practice', this)">🎯 Praktyka skal</button>
        <button class="subtab-btn" data-subtab="chord" onclick="switchPracticeSubtab('chord', this)">Akord · Odkrywanie</button>
        <button class="subtab-btn" data-subtab="harmonics" onclick="switchPracticeSubtab('harmonics', this)">Flażolety</button>
        <button class="subtab-btn" data-subtab="search" onclick="switchPracticeSubtab('search', this)">Szukaj</button>
        <button class="subtab-btn" data-subtab="arpeggio" onclick="switchPracticeSubtab('arpeggio', this)">Arpeggio</button>
        <button class="subtab-btn" data-subtab="exercise" onclick="switchPracticeSubtab('exercise', this)">✏️ Ćwiczenie (TAB)</button>
      </div>
      <div id="viewHeaderTitle" style="display:none;font-size:20px;font-weight:800;">Tuner</div>

      <div class="live-tuner-pill">
        <button class="chip-btn" style="padding:4px 9px;font-size:12px;white-space:nowrap;border-color:var(--accent);color:var(--accent);" onclick="toggleFretboardSettingsModal()">
          🎸 Strój i Gryf: <span id="topTuningLabel">Standard (EADGBE)</span> ⚙
        </button>
        <select id="guitarTonePresetSelect" style="width:auto;padding:4px 8px;font-size:12px;font-weight:600;"
                onchange="state.guitarTonePreset=this.value;saveSettings()">
          <option value="acoustic">🎸 Brzmienie: Akustyczna (Steel)</option>
          <option value="nylon">🎶 Brzmienie: Klasyczna (Nylon)</option>
          <option value="clean">⚡ Brzmienie: Elektryczna Clean</option>
          <option value="crunch">🔥 Brzmienie: Blues Overdrive</option>
          <option value="lead">🤘 Brzmienie: Rock / Metal Lead</option>
          <option value="chorus">🌊 Brzmienie: Ambient Chorus</option>
        </select>
        <span>Wykryty:</span>
        <span id="liveNoteBadge" class="live-note-badge">—</span>
        <span id="liveCentsBadge" style="color:var(--text-muted);min-width:95px;white-space:nowrap;">0.0 ct · 0.0 Hz</span>
        <button class="chip-btn" style="padding:4px 9px;font-size:12px;white-space:nowrap;" onclick="playCurrentScalePreview()">▶ Posłuchaj</button>
      </div>
    </header>

    <!-- Szybki panel ustawień stroju gitary (w tym pojedynczych strun!) i parametrów gryfu -->
    <div id="fretboardSettingsModal" style="display:none;background:var(--bg-card);border-bottom:2px solid var(--accent);padding:14px 22px;gap:22px;align-items:flex-start;flex-wrap:wrap;">
      <div style="min-width:220px;">
        <div class="section-label">GOTOWY STRÓJ GITARY</div>
        <select id="globalTuningSelect" onchange="selectPresetTuning(this.value)"></select>
      </div>
      <div>
        <div class="section-label">STROJENIE POJEDYNCZYCH STRUN (6 → 1)</div>
        <div id="customStringsRow" style="display:flex;gap:8px;flex-wrap:wrap;"></div>
      </div>
      <div>
        <div class="section-label">LICZBA PROGÓW</div>
        <div style="display:flex;gap:5px;">
          <button class="chip-btn" style="padding:6px 10px;" onclick="setMaxFrets(12)">12</button>
          <button class="chip-btn" style="padding:6px 10px;" onclick="setMaxFrets(15)">15</button>
          <button class="chip-btn" style="padding:6px 10px;" onclick="setMaxFrets(22)">22</button>
          <button class="chip-btn" style="padding:6px 10px;" onclick="setMaxFrets(24)">24</button>
        </div>
      </div>
      <div>
        <div class="section-label">ZNAKI CHROMATYCZNE (BEMOLE / KRZYŻYKI)</div>
        <div style="display:flex;gap:5px;margin-top:2px;">
          <button id="accModeFlatBtn" class="chip-btn" style="padding:6px 12px;" onclick="setAccMode(1)">Bemole (♭)</button>
          <button id="accModeSharpBtn" class="chip-btn active" style="padding:6px 12px;" onclick="setAccMode(2)">Krzyżyki (♯)</button>
        </div>
      </div>
      <div>
        <div class="section-label">SKALA ROZMIARU GRYFU (PUNKT 0 = IDEALNY 1:1)</div>
        <div style="display:flex;align-items:center;gap:8px;margin-top:4px;">
          <span style="font-size:11px;color:var(--text-muted);">-5</span>
          <input type="range" id="fretboardZoomSlider" min="-5" max="10" step="1" value="0" style="width:130px;" oninput="setFretboardZoom(parseInt(this.value,10))">
          <span style="font-size:11px;color:var(--text-muted);">+10</span>
          <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="setFretboardZoom(0)">0 (Idealnie)</button>
          <span id="fretboardZoomLabel" style="font-size:12px;font-weight:700;color:var(--accent);">0</span>
        </div>
      </div>
      <div>
        <div class="section-label">BLOKUJ NIEZAZNACZONE DŹWIĘKI</div>
        <div style="display:flex;gap:5px;margin-top:2px;">
          <button id="lockUnmarkedModalOnBtn" class="chip-btn" style="padding:6px 12px;" onclick="setLockUnmarkedNotes(true)">Wł.</button>
          <button id="lockUnmarkedModalOffBtn" class="chip-btn active" style="padding:6px 12px;" onclick="setLockUnmarkedNotes(false)">Wył.</button>
        </div>
      </div>
      <button class="chip-btn" style="margin-left:auto;padding:6px 12px;" onclick="toggleFretboardSettingsModal()">✕ Zamknij</button>
    </div>

    <div class="content-split">
      <!-- Środkowa scena -->
      <section class="main-stage">
        <!-- Dedykowana karta gryfu (pokazywana TYLKO w zakładkach, które używają gryfu, z paskiem dopasowanym do danej zakładki!) -->
        <div id="fretboardCardContainer" class="fretboard-card">
          <div id="fretboardToolbarContent" class="fretboard-toolbar"></div>
          <div id="fretboardWrap" class="fretboard-resizable-wrap" oncontextmenu="return false;">
            <div id="fretboardSvgHost"></div>
          </div>
        </div>

        <!-- Dynamiczny panel dolny / główny zależny od wybranej zakładki -->
        <div id="dynamicStagePanel" style="flex:1;display:flex;flex-direction:column;gap:12px;"></div>
      </section>

      <!-- Prawy inspektor parametrów -->
      <aside class="right-inspector" id="rightInspector"></aside>
    </div>
  </main>
</div>

<!-- Dolny moduł wirtualnej klawiatury MIDI i klawiszy do komponowania -->
<footer class="midi-footer">
  <div class="midi-bar">
    <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
      <button id="midiFocusBtn" class="chip-btn active" style="padding:4px 12px;" onclick="toggleMidiFocus()">
        🎹 Klawisze
      </button>
      <button id="midiSyncGuitarBtn" class="chip-btn active" style="padding:4px 12px;" onclick="toggleSyncGuitarToKeyboard()"
              title="Po włączeniu podświetla na klawiaturze dźwięki grane i wybierane z gitary">
        🔄 Synchronizuj z gitarą: Wł.
      </button>
      <div style="display:flex;align-items:center;gap:5px;">
        <span style="font-size:11px;color:var(--text-muted);font-weight:600;">Brzmienie klawiszy:</span>
        <select id="keyboardPresetSelect" style="width:auto;padding:3px 8px;font-size:11.5px;font-weight:600;"
                onchange="state.keyboardPreset=this.value;saveSettings()">
          <option value="piano">🎹 Fortepian (Grand Piano)</option>
          <option value="hammond">🎛️ Organy Hammonda (B3 + Leslie)</option>
          <option value="organ">⛪ Organy klasyczne (Pipe Organ)</option>
          <option value="rhodes">✨ Pianino elektr. (Fender Rhodes)</option>
          <option value="guitar">🎸 Gitara (wg brzmienia gitary)</option>
          <option value="synth">🚀 Syntezator Lead (Analog Saw)</option>
        </select>
      </div>
      <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="shiftMidiOctave(-1)">◄ Oktawa [Z]</button>
      <span id="midiOctaveLabel" style="font-size:12px;font-weight:700;color:var(--accent);">Baza: C 4</span>
      <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="shiftMidiOctave(1)">Oktawa [X] ►</button>
      <span style="color:var(--text-muted);font-size:11px;">
        Białe: <b>Q W E R T Y U I O P [ ]</b> · Czarne (#): <b>2 3 &nbsp; 5 6 7 &nbsp; 9 0 &nbsp; =</b> · Pauza: <b>[Spacja]</b> · Cofnij: <b>[Backspace]</b>
      </span>
    </div>
  </div>
  <div id="midiKeysContainer" class="midi-keys"></div>
</footer>

<script>
// Stan globalny aplikacji: Krzyżyki (accMode = 2) jako domyślne, bez opcji Auto!
const state = {
  mainView: 'practice',
  subtab: 'scale',
  scaleRecognitionOn: true, // Przełącznik: pokazuje co gramy bezpośrednio na gryfie danej skali!
  tuningId: 'standard',
  customStringMidis: [40, 45, 50, 55, 59, 64], // E2, A2, D3, G3, B3, E4
  lastHandPos: { s: 2, f: 5 }, // Śledzenie pozycji dłoni w trybie rozpoznawania na gryfie
  dragMaskStart: null,
  rootPc: 0,
  accMode: 2, // 1 = Bemole (♭ / Łagodne), 2 = Krzyżyki (♯ / Ostre) – domyślnie Krzyżyki!
  scaleId: 'major',
  chordId: 'major',
  allChordTypes: false,
  chordGroupFilter: 'basic', // 'basic' | 'triad' | 'seventh' | 'extended' | 'altered' | 'all'
  chordSharpness: 'mild', // 'mild' (Łagodne) | 'sharp' (Ostre: szerokie rozciągnięcia lub tapping T)
  chordFilterByScale: false, // Domyślnie bez skali; po włączeniu pokazuje wszystkie pasujące akordy ze skali!
  chordScaleExtraTypeFilter: false, // Opcjonalny szczegółowy wybór konkretnego akordu w trybie „Wg skali” (domyślnie WYŁ.)
  chordLastMatchingIds: [],
  chordScaleRootPc: 0,
  chordScaleId: 'major',
  chordShowScaleOnFretboard: false, // Domyślnie na gryfie w Odkrywaniu Akordów widać sam kliknięty akord (bez skali w tle)
  chordVoicingsList: [],
  selectedChordVoicingIdx: 0,
  posSystem: 0, // 0=Brak, 1=3NPS, 2=4NPS, 3=CAGED
  posIdx: 0,
  labelType: 'notes', // 'notes' | 'intervals' | 'tensions' | 'none'
  // Stan zakładki Flażolety
  harmonicString: -1, // -1 = cały gryf, 0..5 = wybrana struna
  harmonicLabelType: 'none', // 'none' | 'intervals' | 'notes'
  harmonicSort: 'pitch', // 'pitch' | 'string'
  harmonicMinFret: 0,
  harmonicMaxFret: 12,
  harmonicDiffEnabled: {0: true, 1: false, 2: false, 3: false}, // Domyślnie jak na 1. zrzucie podświetlone Łatwe, reszta jako szare kontury!
  harmonicIntervalsEnabled: new Set(['1','2','3','#4','5','b6','b7','7']),
  harmonicsData: [],
  // Skalowanie gryfu wokół punktu 0 (oraz automatyczne wypełnianie pełnego ekranu bez ściskania przy małym oknie!)
  fretboardZoom: 0, // -5 .. 0 .. +10
  fretboardFitWidth: true,
  lockUnmarkedNotes: false, // Blokada klikania niezaznaczonych (spoza skali/akordu lub zamaskowanych) dźwięków na gryfie
  markedFretCells: new Set(),
  // Urządzenia audio
  selectedAudioInputId: 'default',
  selectedAudioOutputId: 'default',
  audioInputDevices: [{ id: 'default', label: 'Domyślne wejście (Mikrofon / Interfejs USB)' }],
  audioOutputDevices: [{ id: 'default', label: 'Domyślne wyjście (Głośniki / Słuchawki)' }],
  // Warsztat (Sesje, Nowa sesja, Dodaj ćwiczenie – zrzuty 5 i 6)
  workshopSubView: 'sessions', // 'sessions' | 'new_session' | 'add_exercise' | 'exercises_list'
  newSessionName: '',
  newSessionItems: [],
  workshopFilterCategory: 'all',
  workshopFilterLevel: 'all',
  wsCreator: {
    rootPc: 0,
    scaleId: 'major',
    posSystem: 1, // 0=Brak, 1=3NPS, 2=4NPS, 3=CAGED
    startString: -1, // -1=Dowolna, 0..5=struny E A D G B E
    direction: 0, // 0=Rosnąco, 1=Malejąco, 2=Rosnąco->Malejąco, 3=Malejąco->Rosnąco
    octaves: 1, // 1 lub 2
    pattern: 0 // 0=Cała skala, 3=Grupy po 3, 4=Grupy po 4, 5=Tercje, 6=Kwarty, 7=Kwinty, 8=Seksty, 9=Oktawy
  },
  customWorkshopExercises: [
    { id: 'ex_1', title: 'C Durowa · 3NPS · Grupy po 4', category: 'Technique Drill', level: 'Średniozaawansowany', rootPc: 0, scaleId: 'major', posSystem: 1, pattern: 4, octaves: 2 },
    { id: 'ex_2', title: 'A Pentatonika molowa · Blues Lick', category: 'Lick', level: 'Początkujący', rootPc: 9, scaleId: 'minor_pentatonic', posSystem: 0, pattern: 3, octaves: 1 },
    { id: 'ex_3', title: 'E Molowa naturalna · Riff kostkowany', category: 'Riff', level: 'Początkujący', rootPc: 4, scaleId: 'natural_minor', posSystem: 1, pattern: 0, octaves: 1 },
    { id: 'ex_4', title: 'A Molowa harmoniczna · Skoki tercjowe', category: 'Arpeggio', level: 'Zaawansowany', rootPc: 9, scaleId: 'harmonic_minor', posSystem: 2, pattern: 5, octaves: 2 }
  ],
  selectedProgressionIdx: 0,
  // Kompletny stan zakładki Zmiany (Zrzuty 1–5: Blues 12-taktowy, Kadencja andaluzyjska, Ćwiczenie na zmianach, Sesja na żywo, Podsumowanie)
  changesScreen: 'editor', // 'editor' | 'library' | 'practice_config' | 'practice_active' | 'practice_summary'
  changesKeyRootPc: 9, // A
  changesIsMinor: false,
  changesBpm: 108,
  changesTimeSig: '4/4',
  changesSectionName: 'A',
  changesRepeat: 'Wył.',
  changesFeel: 'Blues', // 'Blues' | 'Prosto' | 'Swing Jazz' | 'Funk' | 'Rock'
  changesCompInst: 'hammond', // 'hammond' (Organy) | 'clean' (Gitara czysta) | 'piano' (Fortepian) | 'rhodes' (Rhodes) | 'crunch' (Gitara przesterowana)
  changesBass: 'Bas elektryczny',
  changesSwing: 3, // 0=Równo, 1=Lekki, 2=Średni, 3=Mocny
  changesGroove: 'shuffle', // 'shuffle' | 'slow12_8'
  changesRhythmGuitar: true,
  changesLoop: true,
  changesViewMode: 'follow', // 'follow' (Śledzenie) | 'full' (Pełny układ)
  changesChordDiagrams: true,
  changesBackingBand: true,
  changesMetronomeClick: true,
  changesCycleKeys: false,
  changesMeasures: null, // Inicjalizowane z aktywnego standardu
  changesIsPlaying: false,
  changesIsPaused: false,
  changesActiveBarIdx: -1,
  changesActiveBeatIdx: -1,
  changesCountIn: 0,
  changesPracticeGoal: 'chord_tones', // 'chord_tones' (Dźwięki akordu) | 'chord_scale' (Skala akordu)
  changesPracticeMode: 'tempo', // 'tempo' (W tempie) | 'study' (Ćwiczebny)
  changesPracticeDegrees: 'all', // 'all' | '1' | '3' | '5'
  changesPracticeOrder: 'any', // 'any' (Dowolna) | 'seq' (Po kolei)
  changesPracticeShowFretboard: true,
  changesHits: 0,
  changesMisses: 0,
  changesCleanCount: 0,
  changesAlmostCount: 0,
  changesCloseCount: 0,
  changesEvaluatedBar: -1,
  // Kompletny stan Kształcenia Słuchu (Zrzuty 1–7)
  earScreen: 'config', // 'config' | 'active' | 'summary'
  earMode: 'identify', // 'learn' (Nauka) | 'identify' (Rozpoznaj) | 'play' (Zagraj) | 'chords' (Akordy)
  earSelectedSemis: new Set([0, 3, 4, 5, 7, 12]), // Domyślnie 6 interwałów: P1, m3, M3, P4, P5, P8
  earSelectedChords: new Set(['major', 'minor', 'dim', 'maj7', '7', 'm7', 'm7b5']), // 7 rodzajów akordów
  earSelectedInversions: new Set([0, 1, 2, 3]), // Zasadn., 1., 2., 3.
  earInstrument: 'guitar', // 'guitar' | 'piano'
  earPlaybackStyle: 'melodic', // 'melodic' | 'harmonic'
  earDirection: 'asc', // 'asc' | 'desc' | 'rand'
  earRootMode: 'random', // 'random' | 'fixed_note' | 'fixed_pitch'
  earFixedRootPc: 0,
  earFixedOctave: 3, // Oktawa bazowa dla trybu „Nuta + oktawa”
  earSessionType: 'fixed', // 'fixed' (Ustalony) | 'endless' (Bez końca)
  earTargetRounds: 10, // Dowolna wybrana liczba rund (np. 5, 10, 15, 20, 30, 50 lub własna)
  earMeasureReaction: false,
  earShowSongHints: false,
  // Stan aktywnego quizu i podsumowania Treningu Słuchu
  earRound: 1,
  earScore: 0,
  earAttempts: 0,
  earStreak: 0,
  earBestStreak: 0,
  earSessionStartMs: 0,
  earSessionDurationSec: 0,
  earQuestionAskedMs: 0,
  earReactionTimesMs: [],
  earHistory: [], // [{ key, name, code, correct, total }]
  earCurrentQuestion: null, // { rootMidi, targetMidi, semi, asc, chordId, inversion, askedAtMs, answered }
  earPickedChordId: null,
  earPickedInversion: 0,
  earCompareMode: false,
  earCompareSelection: [],
  // Stan zakładki Szukaj
  searchDir: 'chord_to_scale', // 'chord_to_scale' | 'scale_to_chord'
  searchData: null,
  selectedSearchScaleIdx: -1, // -1 = pokazuj tylko dźwięki akordu; >=0 = koloruj wg roli (zielony/żółty/czerwony)
  selectedSearchChordIdx: -1,
  searchLabelType: 'notes', // 'notes' | 'intervals' | 'tensions'
  // Stan Stroika
  tunerMode: 'guitar', // 'guitar' | 'chromatic'
  tunerDisplay: 'needle', // 'needle' | 'strobe'
  tunerLockedString: -1, // -1 = auto, 0..5 = zablokowana struna
  // Stan Postępy i Ustawienia (Screenshot 6)
  settingsTab: 'ustawienia', // 'postepy' | 'ustawienia'
  skillLevel: 'intermediate', // 'beginner' | 'intermediate' | 'advanced'
  dailyGoalMinutes: 10, // 5, 10, 15, 20, 30, 45, 60 (wszystkie odblokowane!)
  gamificationLevel: 'full', // 'full' | 'moderate' | 'minimal'
  gamificationSounds: true,
  resolvingDominants: 'inside', // 'inside' | 'outside' | 'custom'
  alteredDominantScale: 'auto', // 'auto' | 'mixolydian_b6' | 'whole_tone' | 'altered' | 'messiaen_3'
  alteredVoicingIntensity: {
    'G7alt': 'strong',
    'G7b9b13': 'strong',
    'G7#9#5': 'strong',
    'G7b9#5': 'strong',
    'G7b9': 'light',
    'G7#9': 'strong',
    'G7#11': 'light',
    'G7b5': 'light',
    'G13b9': 'strong'
  },
  showMoreAlteredTypes: false,
  movingVoices: true,
  leftHandedFretboard: false,
  todayPracticedMinutes: 6,
  streakDays: 3,
  // Zakładka Arpeggio i Sweep Picking
  arpViewMode: 'single', // 'single' | 'transition'
  arpPattern: 6, // 0=Cały gryf, 1=CAGED, 2=2NPS, 3=3NPS, 4=Sweep Picking, 5=Inwersje, 6=Od prymy (1-2 okt.)
  arpPosIdx: 0,
  arpSweepStrings: 3, // 3 | 5 | 6
  arpDirection: 0, // 0=W górę i w dół, 1=W górę, 2=W dół
  arpSeqStyle: 0, // 0=Prosto (1-3-5), 1=Grupy po 3, 2=Skoki (1-5-3)
  arpStringSet: 0, // 0=G-B-e/D-G-B-e, 1=D-G-B/A-D-G-B, 2=A-D-G/E-A-D-G
  arpOctaves: 2, // 1 | 2 dla trybu Od prymy
  arpTransitionId: 'neoclassical_minor',
  arpActiveSegIdx: 0,
  arpDiatonicSevenths: true,
  arpDiatonicScaleId: 'major',
  arpDiatonicScaleRootPc: 0,
  arpBpm: 90,
  arpLoop: true,
  arpPlaying: false,
  arpPlayStepIdx: -1,
  arpData: null,
  tabCreationEnabled: true,
  exerciseShowStaff: true, // Pięciolinia (nuty): po wyłączeniu widać samą tabulaturę (większą i w mniejszej wysokości)
  exerciseTabWrapRows: false, // false = 1 linia przewijana z zapasem | true = zawijane wiersze (2+ rzędy wypełniające okno)
  tempoBpm: 120,
  timeSigNum: 4,
  timeSigDen: 4,
  exerciseClef: 'treble', // 'treble' (Wiolinowy) | 'bass' (Basowy) | 'auto' (Auto)
  defaultDuration: 0, // -2=Cała, -1=Półnuta, 0=Ćwierćnuta, 1=Ósemka, 2=Szesnastka
  tupletActive: false,
  showExerciseNoteNames: true,
  exerciseMinFret: 0,
  exerciseMaxFret: 24,
  exerciseTitle: 'Twoje ćwiczenie',
  exercisePlayerActive: false,
  exercisePlayerMode: 'listen', // 'listen' | 'play'
  exercisePlayerPaused: false,
  exercisePlayerStepIdx: 0,
  // Stan zakładki Praktyka skal (Zrzut 3)
  scalePracticeMode: 'scale', // 'scale' | 'arpeggio' | 'arpeggio_pattern'
  spPosSystem: 0, // 0=Brak, 1=3NPS, 2=4NPS, 3=CAGED
  spStartString: -1, // -1=Dowolna, 0..5=struny
  spDirection: 0, // 0=Rosnąco, 1=Malejąco, 2=Rosnąco->Malejąco, 3=Malejąco->Rosnąco
  spOctaves: 1, // 1 | 2
  spPattern: 0, // 0, 3, 4, 5, 6, 7, 8, 9
  spUseHeadphones: true,
  spMetronome: true,
  spRefNote: false,
  spBgChord: false,
  spAddToSessionOpen: true,
  midiBaseOctave: 4,
  recordedEvents: [],
  maskedCells: new Set(),
  // Maskownica ze schodkami (domyślnie NIC nie jest zamaskowane: 0..24 w pełni odkryte!):
  maskLeft: [0, 0, 0, 0, 0, 0],
  maskRight: [24, 24, 24, 24, 24, 24],
  activeMaskDrag: null, // { side: 'left'|'right', mode: 'all'|'string' }
  maxDisplayedFret: 24,
  detectedMidi: null,
  detectedMidis: [], // Wykryte wysokości MIDI z gitary (1 dla pojedynczego dźwięku, 2-4 dla dwudźwięków i akordów)
  selectedScaleNotePc: null, // Ostatnio kliknięty / grany dźwięk (0..11) do podświetlenia wszystkich jego pozycji w skali
  detectedCents: 0.0,
  detectedHz: 0.0,
  midiFocused: true, // Domyślnie klawisze do komponowania QWERTY są od razu aktywne!
  syncGuitarToKeyboard: true, // Synchronizacja dźwięków z gitary na klawiaturę na dole
  highlightPlayedOnGuitar: true, // Podświetlanie granego dźwięku na gryfie gitary
  highlightScaleCardsOnPlay: true, // Podświetlanie kafelków stopni i funkcji skali podczas grania
  disabledScaleDegrees: [], // Lista wyłączonych numerów stopni skali (1..N) – działa dla każdej skali i tonacji!
  disabledScaleDegreePcs: [],
  guitarTonePreset: 'acoustic', // 'acoustic' | 'nylon' | 'clean' | 'crunch' | 'lead' | 'chorus'
  keyboardPreset: 'piano', // 'piano' | 'hammond' | 'organ' | 'rhodes' | 'guitar' | 'synth'
  isHoldingFretNote: false,
  isHoldingMouseMidi: false,
  heldMouseMidi: null,
  activeMidiNotes: new Set(),
  a4ReferenceHz: 440.0,
  initData: null,
  fretboardData: null
};

let audioCtx = null;
let masterAudioBus = null;
let masterCompressor = null;
let micStream = null;
let analyserNode = null;
let audioLoopId = null;
let micPollTimer = null;
let micPollBusy = false;

// Mapa aktywnie przytrzymywanych głosów (na gryfie gitary, na klawiaturze myszką oraz na klawiszach QWERTY)
const activeSustainedVoices = new Map();
const guitarSyncKeyTimers = new Map();

function noteStringToPc(noteStr, fallbackPc = 0) {
  if (!noteStr || typeof noteStr !== 'string') return fallbackPc;
  const baseMap = { C: 0, D: 2, E: 4, F: 5, G: 7, A: 9, B: 11 };
  const head = noteStr.charAt(0).toUpperCase();
  if (baseMap[head] === undefined) return fallbackPc;
  let pc = baseMap[head];
  const rest = noteStr.slice(1);
  for (const ch of rest) {
    if (ch === '#' || ch === '♯') pc += 1;
    else if (ch === 'b' || ch === '♭') pc -= 1;
    else if (ch === 'x' || ch === '𝄪') pc += 2;
    else if (ch === '𝄫') pc -= 2;
  }
  return ((pc % 12) + 12) % 12;
}

function getScaleDegreeObjForPc(pc) {
  if (!state.fretboardData || !Array.isArray(state.fretboardData.scaleDegrees)) return null;
  const normPc = ((Math.round(pc) % 12) + 12) % 12;
  for (const d of state.fretboardData.scaleDegrees) {
    const dPc = (d.pc !== undefined) ? (((d.pc % 12) + 12) % 12) : noteStringToPc(d.note, state.rootPc);
    if (dPc === normPc) return d;
  }
  return null;
}

function isScaleDegreeHighlightEnabled(pc) {
  if (pc === null || pc === undefined || pc < 0) return true;
  const degObj = getScaleDegreeObjForPc(pc);
  if (!degObj) return true;
  return !Array.isArray(state.disabledScaleDegrees) || !state.disabledScaleDegrees.includes(degObj.deg);
}

function toggleScaleDegreeHighlight(degNum) {
  if (!Array.isArray(state.disabledScaleDegrees)) state.disabledScaleDegrees = [];
  const idx = state.disabledScaleDegrees.indexOf(degNum);
  if (idx >= 0) {
    state.disabledScaleDegrees.splice(idx, 1);
  } else {
    state.disabledScaleDegrees.push(degNum);
  }
  saveSettings();
  drawFretboard();
  renderStageAndInspector();
}

function setAllScaleDegreesHighlight(enableAll) {
  if (enableAll) {
    state.disabledScaleDegrees = [];
  } else if (state.fretboardData && Array.isArray(state.fretboardData.scaleDegrees)) {
    state.disabledScaleDegrees = state.fretboardData.scaleDegrees.map(d => d.deg);
  }
  saveSettings();
  drawFretboard();
  renderStageAndInspector();
}

function toggleHighlightPlayedOnGuitar() {
  state.highlightPlayedOnGuitar = !state.highlightPlayedOnGuitar;
  saveSettings();
  drawFretboard();
  if (state.mainView === 'practice' && state.subtab === 'scale') {
    renderStageAndInspector();
  }
}

function toggleHighlightScaleCardsOnPlay() {
  state.highlightScaleCardsOnPlay = !state.highlightScaleCardsOnPlay;
  saveSettings();
  updateScaleFunctionCardsHighlight();
  if (state.mainView === 'practice' && state.subtab === 'scale') {
    renderStageAndInspector();
  }
}

function getChromaticFunctionName(semitones) {
  const s = ((Math.round(semitones) % 12) + 12) % 12;
  const names = [
    'Tonika (I · Pryma czysta 1)',
    'Stopień frygijski (♭II · Sekunda mała b2)',
    'Supertonika (II · Sekunda wielka 2)',
    'Medianta molowa (♭III · Tercja mała b3)',
    'Medianta durowa (III · Tercja wielka 3)',
    'Subdominanta (IV · Kwarta czysta 4)',
    'Tryton / Kwarta lidyjska (♯IV/♭V · #4/b5)',
    'Dominanta (V · Kwinta czysta 5)',
    'Submedianta molowa (♭VI · Seksta mała b6)',
    'Submedianta durowa (VI · Seksta wielka 6)',
    'Subtonika (♭VII · Septyma mała b7)',
    'Dźwięk prowadzący (VII · Septyma wielka 7)'
  ];
  return names[s] || 'Stopień skali';
}

function updateScaleFunctionCardsHighlight() {
  const cards = document.querySelectorAll('.scale-func-card');
  const activeMidi = state.detectedMidi;
  const focusPc = (activeMidi !== null && activeMidi !== undefined)
    ? (((Math.round(activeMidi) % 12) + 12) % 12)
    : ((state.selectedScaleNotePc !== null && state.selectedScaleNotePc !== undefined) ? state.selectedScaleNotePc : state.rootPc);

  if (cards.length > 0) {
    cards.forEach(card => {
      const pc = parseInt(card.getAttribute('data-pc') || '-1', 10);
      const isDegEnabled = isScaleDegreeHighlightEnabled(pc);
      const matchesFocus = state.highlightScaleCardsOnPlay && isDegEnabled && (pc === focusPc);
      card.classList.toggle('selected', matchesFocus);
      card.classList.toggle('playing-now', matchesFocus && activeMidi !== null);
    });
  }

  const badge = document.getElementById('activePlayedFunctionBadge');
  if (badge && state.fretboardData) {
    const labels = getRootLabels();
    const noteNm = labels[((focusPc % 12) + 12) % 12];
    const degObj = getScaleDegreeObjForPc(focusPc);
    if (degObj) {
      badge.innerHTML = `🎯 Aktywny dźwięk w skali <b>${state.fretboardData.scaleFullName}</b>: <b style="color:#4ade80;">${degObj.note}</b> — <b>Stopień ${degObj.deg} (${degObj.interval}) · ${degObj.functionName}</b>`;
    } else {
      const semi = ((focusPc - state.rootPc) % 12 + 12) % 12;
      badge.innerHTML = `🎯 Kliknięty dźwięk: <b style="color:#facc15;">${noteNm}</b> — <b>${getChromaticFunctionName(semi)}</b> <span style="color:var(--text-muted);">(dźwięk chromatyczny spoza skali ${state.fretboardData.scaleFullName})</span>`;
    }
  }
}

function resolvePianoKeyElement(midi) {
  if (midi === null || midi === undefined || isNaN(midi)) return null;
  let m = Math.round(midi);
  let el = document.getElementById(`midiKey_${m}`);
  if (el) return el;
  while (m < 40) m += 12;
  while (m > 88) m -= 12;
  return document.getElementById(`midiKey_${m}`);
}

function flashGuitarNoteOnKeyboard(midi, durationMs = 420) {
  if (!state.syncGuitarToKeyboard) return;
  const pc = ((Math.round(midi) % 12) + 12) % 12;
  if (state.mainView === 'practice' && state.subtab === 'scale' && !isScaleDegreeHighlightEnabled(pc)) return;
  const el = resolvePianoKeyElement(midi);
  if (!el) return;
  el.classList.add('guitar-sync');
  if (guitarSyncKeyTimers.has(el.id)) {
    clearTimeout(guitarSyncKeyTimers.get(el.id));
  }
  const tid = setTimeout(() => {
    guitarSyncKeyTimers.delete(el.id);
    const activeEl = (state.detectedMidi !== null) ? resolvePianoKeyElement(state.detectedMidi) : null;
    if (!activeEl || activeEl.id !== el.id) {
      el.classList.remove('guitar-sync');
    }
  }, durationMs);
  guitarSyncKeyTimers.set(el.id, tid);
}

function updateGuitarToKeyboardSync() {
  updateScaleFunctionCardsHighlight();
  document.querySelectorAll('.piano-key.guitar-sync, .piano-key.guitar-octave-sync, .piano-key.guitar-chord-sync, .piano-key.scale-root-key, .piano-key.scale-member-key').forEach(el => {
    if (!guitarSyncKeyTimers.has(el.id)) {
      el.classList.remove('guitar-sync');
    }
    el.classList.remove('guitar-octave-sync', 'guitar-chord-sync', 'scale-root-key', 'scale-member-key');
  });
  if (!state.syncGuitarToKeyboard) {
    guitarSyncKeyTimers.forEach(tid => clearTimeout(tid));
    guitarSyncKeyTimers.clear();
    document.querySelectorAll('.piano-key.guitar-sync').forEach(el => el.classList.remove('guitar-sync'));
    return;
  }

  // 1. W zakładce Skala podświetlamy na klawiaturze wszystkie aktywne stopnie aktualnie wybranej skali
  if (state.mainView === 'practice' && state.subtab === 'scale' && state.fretboardData && Array.isArray(state.fretboardData.scaleDegrees)) {
    const activeScalePcs = new Set();
    state.fretboardData.scaleDegrees.forEach(d => {
      const dPc = (d.pc !== undefined) ? (((d.pc % 12) + 12) % 12) : noteStringToPc(d.note, state.rootPc);
      if (isScaleDegreeHighlightEnabled(dPc)) {
        activeScalePcs.add(dPc);
      }
    });
    for (let m = 40; m <= 88; m++) {
      const pc = m % 12;
      if (!activeScalePcs.has(pc)) continue;
      const el = document.getElementById(`midiKey_${m}`);
      if (!el) continue;
      if (pc === state.rootPc) {
        el.classList.add('scale-root-key');
      } else {
        el.classList.add('scale-member-key');
      }
    }
  }

  // 2. W zakładce Akord · Odkrywanie podświetlamy na klawiszach wszystkie składniki wybranego chwytu akordu
  if (state.mainView === 'practice' && state.subtab === 'chord' && state.chordVoicingsList && state.chordVoicingsList.length > 0) {
    const v = state.chordVoicingsList[state.selectedChordVoicingIdx] || state.chordVoicingsList[0];
    if (v && Array.isArray(v.frets) && state.customStringMidis) {
      v.frets.forEach((f, sIdx) => {
        if (f >= 0 && state.customStringMidis[sIdx] !== undefined) {
          const el = resolvePianoKeyElement(state.customStringMidis[sIdx] + f);
          if (el) el.classList.add('guitar-chord-sync');
        }
      });
    }
  }

  // 2b. W zakładce Arpeggio podświetlamy na klawiszach wyłącznie dokładne dźwięki aktualnego kształtu / kroku ćwiczenia
  if (state.mainView === 'practice' && state.subtab === 'arpeggio' && state.arpData) {
    let arpNodes = state.arpData.nodes || [];
    if (state.arpViewMode === 'transition' && state.arpData.transition && state.arpData.transition.segments) {
      const seg = state.arpData.transition.segments[state.arpActiveSegIdx || 0];
      arpNodes = seg ? (seg.nodes || []) : [];
    } else if (state.arpPattern === 0 && (state.arpPlaying || state.arpPlayStepIdx >= 0)) {
      arpNodes = getActiveArpeggioSteps();
    }
    arpNodes.forEach(n => {
      if (n && n.midi !== undefined) {
        const el = resolvePianoKeyElement(n.midi);
        if (el) el.classList.add('guitar-chord-sync');
      }
    });
    if (state.arpPlayStepIdx >= 0) {
      const st = getActiveArpeggioSteps()[state.arpPlayStepIdx];
      if (st && st.midi !== undefined) {
        const el = resolvePianoKeyElement(st.midi);
        if (el) el.classList.add('guitar-play-sync');
      }
      return;
    }
  }

  // 3. Aktualnie kliknięty / grany dźwięk (lub dwudźwięk / akord z mikrofonu): podświetl na klawiaturze!
  const activeMidis = (micStream && state.detectedMidis && state.detectedMidis.length > 0)
    ? state.detectedMidis
    : ((state.detectedMidi !== null && state.detectedMidi !== undefined) ? [state.detectedMidi] : []);

  const focusPc = (activeMidis.length > 0)
    ? (((Math.round(activeMidis[0]) % 12) + 12) % 12)
    : ((state.mainView === 'practice' && state.subtab === 'scale') ? state.selectedScaleNotePc : null);

  if (focusPc !== null && focusPc !== undefined) {
    if (state.mainView === 'practice' && state.subtab === 'scale' && !isScaleDegreeHighlightEnabled(focusPc)) {
      return;
    }
    for (let m = 40; m <= 88; m++) {
      // Przy wykrywaniu z gitary (mikrofon) podświetlamy tylko dokładnie zagraną oktawę, nie wszystkie oktawy.
      if (!isExactPitchHighlightMode() && (m % 12) === focusPc) {
        const el = document.getElementById(`midiKey_${m}`);
        if (el) el.classList.add('guitar-octave-sync');
      }
    }
  }

  activeMidis.forEach(am => {
    const pc = ((Math.round(am) % 12) + 12) % 12;
    if (state.mainView === 'practice' && state.subtab === 'scale' && !isScaleDegreeHighlightEnabled(pc)) {
      return;
    }
    const el = resolvePianoKeyElement(am);
    if (el) el.classList.add('guitar-sync');
  });
}

function toggleSyncGuitarToKeyboard() {
  state.syncGuitarToKeyboard = !state.syncGuitarToKeyboard;
  const btn = document.getElementById('midiSyncGuitarBtn');
  if (btn) {
    btn.classList.toggle('active', !!state.syncGuitarToKeyboard);
    btn.textContent = `🔄 Synchronizuj z gitarą: ${state.syncGuitarToKeyboard ? 'Wł.' : 'Wył.'}`;
  }
  saveSettings();
  updateGuitarToKeyboardSync();
  if (state.mainView === 'practice' && state.subtab === 'scale') {
    renderStageAndInspector();
  }
}

function ensureAudioCtx() {
  if (!audioCtx) {
    audioCtx = new (window.AudioContext || window.webkitAudioContext)({ sampleRate: 48000 });
  }
  if (audioCtx.state === 'suspended') audioCtx.resume();
  if (!masterAudioBus) {
    masterAudioBus = audioCtx.createGain();
    masterAudioBus.gain.value = 0.52;
    masterCompressor = audioCtx.createDynamicsCompressor();
    masterCompressor.threshold.setValueAtTime(-3.5, audioCtx.currentTime);
    masterCompressor.knee.setValueAtTime(6.0, audioCtx.currentTime);
    masterCompressor.ratio.setValueAtTime(14.0, audioCtx.currentTime);
    masterCompressor.attack.setValueAtTime(0.002, audioCtx.currentTime);
    masterCompressor.release.setValueAtTime(0.10, audioCtx.currentTime);
    masterAudioBus.connect(masterCompressor);
    masterCompressor.connect(audioCtx.destination);
  }
  return audioCtx;
}

function getMasterAudioOut() {
  ensureAudioCtx();
  return masterAudioBus;
}

// Łagodna, asymetryczna charakterystyka lampowego przesteru (bez twardego obcinania i bez szumu!)
function makeTubeOverdriveCurve(driveAmount) {
  const n = 8192;
  const curve = new Float32Array(n);
  const k = Math.max(0.5, driveAmount);
  const norm = Math.tanh(k);
  for (let i = 0; i < n; i++) {
    const x = (i * 2) / n - 1;
    // Asymetria lampowa (2. harmoniczna dająca ciepło wzmacniacza lampowego bez bzyczenia)
    const asym = x >= 0 ? Math.tanh(k * x) : Math.tanh(k * 0.86 * x) * 0.94;
    curve[i] = (asym / norm) * 0.88;
  }
  return curve;
}

// Bezszumowy generator drgającej struny gitarowej (rozwiązanie równania falowego sztywnej struny
// ze składowymi harmonicznymi 1..K – ZERO losowego białego szumu Math.random() i 100% czysta intonacja!)
function buildCleanGuitarStringBuffer(ctx, freqHz, durationSec, tone) {
  const sr = ctx.sampleRate;
  const totalSamples = Math.max(64, Math.floor(sr * durationSec));
  const buffer = ctx.createBuffer(1, totalSamples, sr);
  const data = buffer.getChannelData(0);

  // Punkt pobudzenia struny (wzdłuż menzury) oraz charakterystyka tłumienia wyższych harmonicznych
  const pluckPos = (tone === 'nylon') ? 0.29
                 : (tone === 'acoustic') ? 0.19
                 : (tone === 'clean' || tone === 'chorus') ? 0.23
                 : (tone === 'crunch') ? 0.25
                 : 0.22; // lead
  const rollOffPower = (tone === 'nylon') ? 1.48
                     : (tone === 'acoustic') ? 1.08
                     : (tone === 'clean' || tone === 'chorus') ? 1.18
                     : (tone === 'crunch') ? 1.28
                     : 1.22;
  const baseDecayTau = (tone === 'nylon') ? 1.25
                     : (tone === 'acoustic') ? 1.75
                     : (tone === 'clean' || tone === 'chorus') ? 1.95
                     : (tone === 'crunch') ? 2.15
                     : 2.45;
  const inharmB = (tone === 'acoustic') ? 0.000038 : 0.000010;
  const maxHarm = Math.min(10, Math.max(3, Math.floor(9500 / Math.max(40, freqHz))));

  // Oblicz parametry poszczególnych składowych harmonicznych struny
  const partials = [];
  let ampSum = 0;
  for (let k = 1; k <= maxHarm; k++) {
    const fn = freqHz * k * Math.sqrt(1 + inharmB * k * k);
    if (fn >= sr * 0.44) break;
    let amp = Math.abs(Math.sin(k * Math.PI * pluckPos)) / Math.pow(k, rollOffPower);
    if (k === 1) amp = Math.max(amp, 0.95); // Wyraźny, mocny ton podstawowy w każdym brzmieniu
    if (k === 2) amp = Math.max(amp, 0.42);
    const tau = baseDecayTau / (1.0 + 0.42 * Math.pow(k - 1, 1.45));
    const decayPerSample = Math.exp(-1.0 / (sr * tau));
    partials.push({
      dPhase: (2.0 * Math.PI * fn) / sr,
      phase: (k % 2 === 0) ? 0.15 : 0.0,
      env: amp,
      decay: decayPerSample
    });
    ampSum += amp;
  }

  const normScale = ampSum > 0 ? (0.34 / ampSum) : 0.25;
  partials.forEach(p => { p.env *= normScale; });

  // Krótki, bezszumowy impuls uderzenia kostki / paznokcia (3.8 ms gładkiego kosinusa bez białego szumu!)
  const pickSamples = Math.floor(sr * 0.0038);
  const pickOmega = (2.0 * Math.PI * Math.min(2800, freqHz * 3.5)) / sr;
  const pickAmp = (tone === 'nylon') ? 0.06 : (tone === 'acoustic' ? 0.13 : 0.10);

  for (let i = 0; i < totalSamples; i++) {
    // Łagodny narost 1.8 ms eliminujący jakiekolwiek trzaski na starcie
    const attackEnv = (i < 85) ? (i / 85.0) : 1.0;
    let sample = 0.0;
    for (let j = 0; j < partials.length; j++) {
      const p = partials[j];
      sample += Math.sin(p.phase) * p.env;
      p.phase += p.dPhase;
      p.env *= p.decay;
    }
    if (i < pickSamples) {
      const w = 0.5 * (1.0 + Math.cos((Math.PI * i) / pickSamples));
      sample += Math.sin(pickOmega * i) * w * pickAmp;
    }
    data[i] = sample * attackEnv;
  }
  return buffer;
}

// Wspólny tor kształtowania barwy gitary (Akustyczna, Klasyczna, Clean, Blues Overdrive, Rock/Metal Lead, Chorus)
function connectGuitarToneChain(ctx, sourceNode, destNode, tone, now, stopTime, nodesToStop) {
  if (tone === 'nylon') {
    // 🎶 Klasyczna Nylon: ciepłe pudło cedrowe (205 Hz) + aksamitna góra
    const body = ctx.createBiquadFilter();
    body.type = 'peaking';
    body.frequency.value = 205;
    body.Q.value = 1.6;
    body.gain.value = 2.2;

    const warmLp = ctx.createBiquadFilter();
    warmLp.type = 'lowpass';
    warmLp.frequency.value = 3800;
    warmLp.Q.value = 0.75;

    sourceNode.connect(body);
    body.connect(warmLp);
    warmLp.connect(destNode);
    return;
  }

  if (tone === 'acoustic') {
    // 🎸 Akustyczna Steel: naturalne pudło dreadnought (155 Hz) + krystaliczny blask strun brązowych (3.1 kHz)
    const bodyLow = ctx.createBiquadFilter();
    bodyLow.type = 'peaking';
    bodyLow.frequency.value = 155;
    bodyLow.Q.value = 1.5;
    bodyLow.gain.value = 2.2;

    const midScoop = ctx.createBiquadFilter();
    midScoop.type = 'peaking';
    midScoop.frequency.value = 540;
    midScoop.Q.value = 1.3;
    midScoop.gain.value = -1.8;

    const sparkle = ctx.createBiquadFilter();
    sparkle.type = 'peaking';
    sparkle.frequency.value = 3100;
    sparkle.Q.value = 1.3;
    sparkle.gain.value = 2.8;

    sourceNode.connect(bodyLow);
    bodyLow.connect(midScoop);
    midScoop.connect(sparkle);
    sparkle.connect(destNode);
    return;
  }

  if (tone === 'clean') {
    // ⚡ Elektryczna Clean (Fender Twin Reverb: ciepły dół 240 Hz + dzwonkowy środek 2.4 kHz + kolumna 2x12)
    const warmBody = ctx.createBiquadFilter();
    warmBody.type = 'peaking';
    warmBody.frequency.value = 240;
    warmBody.Q.value = 1.3;
    warmBody.gain.value = 2.2;

    const chime = ctx.createBiquadFilter();
    chime.type = 'peaking';
    chime.frequency.value = 2350;
    chime.Q.value = 1.3;
    chime.gain.value = 2.6;

    const cab = ctx.createBiquadFilter();
    cab.type = 'lowpass';
    cab.frequency.value = 4500;
    cab.Q.value = 0.85;

    sourceNode.connect(warmBody);
    warmBody.connect(chime);
    chime.connect(cab);
    cab.connect(destNode);
    return;
  }

  if (tone === 'chorus') {
    // 🌊 Ambient Chorus (czysta gitara elektryczna + przestrzenny dwugłosowy chorus LFO)
    const cab = ctx.createBiquadFilter();
    cab.type = 'lowpass';
    cab.frequency.value = 4400;
    cab.Q.value = 0.85;
    sourceNode.connect(cab);

    const dryGain = ctx.createGain();
    dryGain.gain.value = 0.80;
    cab.connect(dryGain);
    dryGain.connect(destNode);

    const delayNode = ctx.createDelay();
    delayNode.delayTime.value = 0.015;
    const lfo = ctx.createOscillator();
    const lfoGain = ctx.createGain();
    lfo.frequency.value = 1.65;
    lfoGain.gain.value = 0.0032;
    lfo.connect(lfoGain);
    lfoGain.connect(delayNode.delayTime);

    const wetGain = ctx.createGain();
    wetGain.gain.value = 0.52;
    cab.connect(delayNode);
    delayNode.connect(wetGain);
    wetGain.connect(destNode);

    lfo.start(now);
    lfo.stop(stopTime);
    if (nodesToStop) nodesToStop.push(lfo);
    return;
  }

  if (tone === 'crunch') {
    // 🔥 Blues Overdrive (Tube Screamer + wzmacniacz lampowy Fender/Marshall: ciepły śpiewny środek, ZERO szumu!)
    const hp = ctx.createBiquadFilter();
    hp.type = 'highpass';
    hp.frequency.value = 115;

    const tsMid = ctx.createBiquadFilter();
    tsMid.type = 'peaking';
    tsMid.frequency.value = 760;
    tsMid.Q.value = 1.1;
    tsMid.gain.value = 4.2;

    const preLp = ctx.createBiquadFilter();
    preLp.type = 'lowpass';
    preLp.frequency.value = 3200;

    const shaper = ctx.createWaveShaper();
    shaper.curve = makeTubeOverdriveCurve(2.25);
    shaper.oversample = '4x';

    // Dwustopniowy filtr kolumny głośnikowej 1x12 Celestion (całkowicie wycina piaszczyste częstotliwości)
    const cabLp1 = ctx.createBiquadFilter();
    cabLp1.type = 'lowpass';
    cabLp1.frequency.value = 2650;
    cabLp1.Q.value = 0.85;

    const cabLp2 = ctx.createBiquadFilter();
    cabLp2.type = 'lowpass';
    cabLp2.frequency.value = 3300;
    cabLp2.Q.value = 0.70;

    const outGain = ctx.createGain();
    outGain.gain.value = 0.62;

    sourceNode.connect(hp);
    hp.connect(tsMid);
    tsMid.connect(preLp);
    preLp.connect(shaper);
    shaper.connect(cabLp1);
    cabLp1.connect(cabLp2);
    cabLp2.connect(outGain);
    outGain.connect(destNode);
    return;
  }

  // 🤘 Rock / Metal Lead (śpiewny, gęsty lead studyjny z podwójnym filtrem kolumny 4x12 i subtelnym echem slapback)
  const hp = ctx.createBiquadFilter();
  hp.type = 'highpass';
  hp.frequency.value = 140;

  const leadFocus = ctx.createBiquadFilter();
  leadFocus.type = 'peaking';
  leadFocus.frequency.value = 880;
  leadFocus.Q.value = 1.15;
  leadFocus.gain.value = 4.8;

  const preLp = ctx.createBiquadFilter();
  preLp.type = 'lowpass';
  preLp.frequency.value = 3000;

  const shaper = ctx.createWaveShaper();
  shaper.curve = makeTubeOverdriveCurve(3.85);
  shaper.oversample = '4x';

  const cabPresence = ctx.createBiquadFilter();
  cabPresence.type = 'peaking';
  cabPresence.frequency.value = 1650;
  cabPresence.Q.value = 1.2;
  cabPresence.gain.value = 2.6;

  // Kaskada dwóch filtrów dolnoprzepustowych 4x12 Vintage 30 (nachylenie 24 dB/okt – czysty, śpiewny ton bez bzyczenia!)
  const cabLp1 = ctx.createBiquadFilter();
  cabLp1.type = 'lowpass';
  cabLp1.frequency.value = 2450;
  cabLp1.Q.value = 0.85;

  const cabLp2 = ctx.createBiquadFilter();
  cabLp2.type = 'lowpass';
  cabLp2.frequency.value = 2950;
  cabLp2.Q.value = 0.70;

  const outGain = ctx.createGain();
  outGain.gain.value = 0.56;

  sourceNode.connect(hp);
  hp.connect(leadFocus);
  leadFocus.connect(preLp);
  preLp.connect(shaper);
  shaper.connect(cabPresence);
  cabPresence.connect(cabLp1);
  cabLp1.connect(cabLp2);
  cabLp2.connect(outGain);
  outGain.connect(destNode);

  // Delikatne studyjne echo leadowe (95 ms, ciepło przefiltrowane) nadające głębię solówkom
  const echoDelay = ctx.createDelay();
  echoDelay.delayTime.value = 0.095;
  const echoFilter = ctx.createBiquadFilter();
  echoFilter.type = 'lowpass';
  echoFilter.frequency.value = 1900;
  const echoGain = ctx.createGain();
  echoGain.gain.value = 0.16;
  outGain.connect(echoDelay);
  echoDelay.connect(echoFilter);
  echoFilter.connect(echoGain);
  echoGain.connect(destNode);
}

function pluckGuitarFrequency(freqHz, durationSec = 1.45, gainScale = 1.0) {
  if (state.syncGuitarToKeyboard && freqHz > 20) {
    const m = Math.round(69 + 12 * Math.log2(freqHz / (state.a4ReferenceHz || 440.0)));
    flashGuitarNoteOnKeyboard(m, Math.min(850, Math.max(240, Math.round(durationSec * 460))));
  }
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const tone = state.guitarTonePreset || 'acoustic';
  const effDur = (tone === 'lead') ? Math.max(durationSec, 1.85) : Math.max(durationSec, 1.40);

  const buffer = buildCleanGuitarStringBuffer(ctx, freqHz, effDur, tone);
  const src = ctx.createBufferSource();
  src.buffer = buffer;

  const noteOutGain = ctx.createGain();
  noteOutGain.gain.setValueAtTime(Math.max(0.05, Math.min(1.5, gainScale)), now);
  noteOutGain.connect(getMasterAudioOut());

  connectGuitarToneChain(ctx, src, noteOutGain, tone, now, now + effDur, null);
  src.start(now);
}

// Synteza brzmień klawiatury wirtualnej (Fortepian, Organy Hammonda B3, Organy klasyczne, Rhodes, Gitara, Syntezator)
function playKeyboardPresetNote(freqHz, durationSec = 1.15, gainScale = 1.0) {
  const preset = state.keyboardPreset || 'piano';
  if (preset === 'guitar') {
    pluckGuitarFrequency(freqHz, durationSec, gainScale);
    return;
  }
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const voiceScale = ctx.createGain();
  voiceScale.gain.setValueAtTime(Math.max(0.05, Math.min(1.5, gainScale)), now);
  voiceScale.connect(getMasterAudioOut());
  const master = ctx.createGain();
  master.connect(voiceScale);

  if (preset === 'piano') {
    // 🎹 Fortepian (addytywne harmoniczne z młoteczkowym atakiem i naturalnym wybrzmiewaniem)
    const harmonics = [
      { mult: 1.0, amp: 0.42, decay: durationSec },
      { mult: 2.0, amp: 0.20, decay: durationSec * 0.72 },
      { mult: 3.0, amp: 0.09, decay: durationSec * 0.50 },
      { mult: 4.0, amp: 0.04, decay: durationSec * 0.35 }
    ];
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.55, now + 0.007);
    master.gain.exponentialRampToValueAtTime(0.0008, now + durationSec);

    harmonics.forEach((h, idx) => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = idx === 0 ? 'triangle' : 'sine';
      osc.frequency.setValueAtTime(freqHz * h.mult, now);
      osc.detune.setValueAtTime((idx % 2 === 0 ? 1 : -1) * 1.6, now);
      g.gain.setValueAtTime(h.amp, now);
      g.gain.exponentialRampToValueAtTime(0.001, now + h.decay);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + durationSec);
    });
  } else if (preset === 'hammond') {
    // 🎛️ Organy Hammonda B3 (drawbary 16', 5 1/3', 8', 4', 2 2/3', 2' + klik perkusyjny + wirnik Leslie LFO)
    const drawbars = [
      { mult: 0.5, amp: 0.16 },
      { mult: 1.0, amp: 0.28 },
      { mult: 1.498, amp: 0.14 },
      { mult: 2.0, amp: 0.20 },
      { mult: 2.996, amp: 0.10 },
      { mult: 4.0, amp: 0.12 }
    ];
    const lfo = ctx.createOscillator();
    const lfoGain = ctx.createGain();
    lfo.frequency.value = 6.2; // Wirnik Leslie (Tremolo)
    lfoGain.gain.value = 7.5;
    lfo.start(now);
    lfo.stop(now + durationSec);

    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.42, now + 0.012);
    master.gain.setValueAtTime(0.38, now + Math.max(0.05, durationSec - 0.12));
    master.gain.exponentialRampToValueAtTime(0.001, now + durationSec);

    drawbars.forEach(d => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = 'sine';
      osc.frequency.setValueAtTime(freqHz * d.mult, now);
      lfo.connect(lfoGain);
      lfoGain.connect(osc.detune);
      g.gain.setValueAtTime(d.amp, now);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + durationSec);
    });

    // Hammond Percussion (3rd harmonic click)
    const perc = ctx.createOscillator();
    const pg = ctx.createGain();
    perc.type = 'sine';
    perc.frequency.setValueAtTime(freqHz * 3.0, now);
    pg.gain.setValueAtTime(0.18, now);
    pg.gain.exponentialRampToValueAtTime(0.001, now + 0.085);
    perc.connect(pg);
    pg.connect(master);
    perc.start(now);
    perc.stop(now + 0.09);
  } else if (preset === 'organ') {
    // ⛪ Organy klasyczne (piszczałkowe – bogate rejestry oktawowe i kwintowe z łagodnym zadęciem)
    const pipes = [
      { mult: 0.5, type: 'triangle', amp: 0.16 },
      { mult: 1.0, type: 'triangle', amp: 0.24 },
      { mult: 2.0, type: 'sine', amp: 0.18 },
      { mult: 3.0, type: 'sine', amp: 0.09 },
      { mult: 4.0, type: 'sine', amp: 0.11 }
    ];
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.40, now + 0.045);
    master.gain.setValueAtTime(0.38, now + Math.max(0.08, durationSec - 0.18));
    master.gain.exponentialRampToValueAtTime(0.001, now + durationSec);

    pipes.forEach((p, idx) => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = p.type;
      osc.frequency.setValueAtTime(freqHz * p.mult, now);
      osc.detune.setValueAtTime((idx - 2) * 2.2, now);
      g.gain.setValueAtTime(p.amp, now);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + durationSec);
    });
  } else if (preset === 'rhodes') {
    // ✨ Pianino elektryczne Fender Rhodes (ciepły dzwonek FM + tremolo)
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.48, now + 0.008);
    master.gain.exponentialRampToValueAtTime(0.001, now + durationSec);

    const carrier = ctx.createOscillator();
    const mod = ctx.createOscillator();
    const modGain = ctx.createGain();
    carrier.type = 'sine';
    carrier.frequency.setValueAtTime(freqHz, now);
    mod.type = 'sine';
    mod.frequency.setValueAtTime(freqHz * 7.0, now); // Dzwonkowy przydźwięk tine
    modGain.gain.setValueAtTime(freqHz * 1.4, now);
    modGain.gain.exponentialRampToValueAtTime(0.5, now + 0.22);

    mod.connect(modGain);
    modGain.connect(carrier.frequency);
    carrier.connect(master);
    mod.start(now);
    carrier.start(now);
    mod.stop(now + durationSec);
    carrier.stop(now + durationSec);
  } else if (preset === 'synth') {
    // 🚀 Syntezator Lead (2 rozstrojone przebiegi piłokształtne + rezonansowy filtr dolnoprzepustowy)
    const filter = ctx.createBiquadFilter();
    filter.type = 'lowpass';
    filter.Q.value = 4.5;
    filter.frequency.setValueAtTime(freqHz * 6.5, now);
    filter.frequency.exponentialRampToValueAtTime(freqHz * 1.6, now + 0.35);
    filter.connect(master);

    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.28, now + 0.015);
    master.gain.exponentialRampToValueAtTime(0.001, now + durationSec);

    [-7, 7].forEach(det => {
      const osc = ctx.createOscillator();
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(freqHz, now);
      osc.detune.setValueAtTime(det, now);
      osc.connect(filter);
      osc.start(now);
      osc.stop(now + durationSec);
    });
  }
}

// =========================================================================
// SILNIK PRZYTRZYMYWANIA DŹWIĘKÓW (SUSTAIN & RELEASE) DLA GITARY I KLAWISZY
// =========================================================================

function stopSustainedVoice(voiceKey, immediate = false) {
  const voice = activeSustainedVoices.get(voiceKey);
  if (!voice) return;
  activeSustainedVoices.delete(voiceKey);

  try {
    const ctx = ensureAudioCtx();
    const now = ctx.currentTime;
    const heldSec = Math.max(0, now - (voice.startedAt || now));
    // Jeśli było to bardzo krótkie kliknięcie (< 160 ms), dajemy minimalny czas wybrzmienia ataku
    const minHoldDelay = (!immediate && heldSec < 0.16) ? (0.16 - heldSec) : 0.0;
    const relStart = now + minHoldDelay;
    const relDur = immediate ? 0.035 : 0.11;

    voice.gainNode.gain.cancelScheduledValues(relStart);
    voice.gainNode.gain.setValueAtTime(Math.max(0.001, voice.gainNode.gain.value || 1.0), relStart);
    voice.gainNode.gain.exponentialRampToValueAtTime(0.0001, relStart + relDur);

    const stopAt = relStart + relDur + 0.02;
    (voice.nodesToStop || []).forEach(node => {
      try { node.stop(stopAt); } catch (e) {}
    });
  } catch (e) {}
}

function stopAllSustainedVoices() {
  Array.from(activeSustainedVoices.keys()).forEach(k => stopSustainedVoice(k, true));
}

// Uruchamia przytrzymywany dźwięk gitary (bezszumowy atak szarpniętej struny + ciągły harmoniczny sustain z lekkim wibrato,
// brzmiący tak długo, jak trzymasz przycisk myszy na gryfie lub klawisz na klawiaturze!)
function startGuitarSustainedVoice(voiceKey, freqHz, maxHoldSec = 18.0) {
  stopSustainedVoice(voiceKey, true);
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const tone = state.guitarTonePreset || 'acoustic';

  const voiceGain = ctx.createGain();
  voiceGain.gain.setValueAtTime(1.0, now);
  voiceGain.connect(getMasterAudioOut());

  const nodesToStop = [];

  // Wspólny tor kształtowania brzmienia gitary (bez szumu!)
  const chainInput = ctx.createGain();
  chainInput.gain.value = 1.0;
  connectGuitarToneChain(ctx, chainInput, voiceGain, tone, now, now + maxHoldSec, nodesToStop);

  // Warstwa 1: Bezszumowy fizyczny atak szarpniętej struny (idealnie nastrojony do freqHz!)
  const pluckDur = Math.min(3.8, maxHoldSec);
  const buffer = buildCleanGuitarStringBuffer(ctx, freqHz, pluckDur, tone);
  const pluckSrc = ctx.createBufferSource();
  pluckSrc.buffer = buffer;
  pluckSrc.connect(chainInput);
  pluckSrc.start(now);
  nodesToStop.push(pluckSrc);

  // Warstwa 2: Ciągłe podtrzymanie drgającej struny (Sustain + subtelne wibrato palca podczas przytrzymania)
  const susFilter = ctx.createBiquadFilter();
  susFilter.type = 'lowpass';
  susFilter.frequency.value = Math.min(3600, freqHz * (tone === 'nylon' ? 2.5 : 3.6));
  susFilter.Q.value = 0.9;

  const susGain = ctx.createGain();
  const targetSus = (tone === 'lead') ? 0.30
                  : (tone === 'crunch') ? 0.25
                  : (tone === 'clean' || tone === 'chorus') ? 0.20
                  : (tone === 'nylon') ? 0.16
                  : 0.19;
  susGain.gain.setValueAtTime(0.0001, now);
  susGain.gain.setValueAtTime(0.0001, now + 0.12);
  susGain.gain.linearRampToValueAtTime(targetSus, now + 0.55);

  const vibLfo = ctx.createOscillator();
  const vibGain = ctx.createGain();
  vibLfo.frequency.value = 5.2; // Naturalne wibrato palca na progu
  vibGain.gain.setValueAtTime(0.0, now);
  vibGain.gain.linearRampToValueAtTime(3.8, now + 0.50);
  vibLfo.connect(vibGain);
  vibLfo.start(now);
  vibLfo.stop(now + maxHoldSec);
  nodesToStop.push(vibLfo);

  // Czyste składowe harmoniczne struny (1x, 2x, 3x) bez szumu
  const harmWeights = (tone === 'lead' || tone === 'crunch')
    ? [{ mult: 1.0, type: 'triangle', amp: 0.62 }, { mult: 2.0, type: 'sine', amp: 0.28 }, { mult: 3.0, type: 'sine', amp: 0.12 }]
    : [{ mult: 1.0, type: 'triangle', amp: 0.68 }, { mult: 2.0, type: 'sine', amp: 0.24 }, { mult: 3.0, type: 'sine', amp: 0.08 }];

  harmWeights.forEach(hw => {
    const osc = ctx.createOscillator();
    const og = ctx.createGain();
    osc.type = hw.type;
    osc.frequency.setValueAtTime(freqHz * hw.mult, now);
    og.gain.value = hw.amp;
    vibGain.connect(osc.detune);
    osc.connect(og);
    og.connect(susFilter);
    osc.start(now);
    osc.stop(now + maxHoldSec);
    nodesToStop.push(osc);
  });

  susFilter.connect(susGain);
  susGain.connect(chainInput);

  activeSustainedVoices.set(voiceKey, {
    gainNode: voiceGain,
    nodesToStop,
    startedAt: now
  });
}

// Uruchamia przytrzymywany dźwięk klawiszy (działa dla QWERTY oraz trzymania myszką na wirtualnym pianinie!)
function startKeyboardSustainedVoice(voiceKey, midi, maxHoldSec = 20.0) {
  const freqHz = midiToFreq(midi);
  const preset = state.keyboardPreset || 'piano';
  if (preset === 'guitar') {
    startGuitarSustainedVoice(voiceKey, freqHz, maxHoldSec);
    return;
  }

  stopSustainedVoice(voiceKey, true);
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const voiceGain = ctx.createGain();
  voiceGain.gain.setValueAtTime(1.0, now);
  voiceGain.connect(getMasterAudioOut());

  const master = ctx.createGain();
  master.connect(voiceGain);
  const nodesToStop = [];

  if (preset === 'piano') {
    // 🎹 Fortepian z podniesionym tłumikiem podczas trzymania klawisza (długi śpiewny sustain 9s, wygaszany po puszczeniu!)
    const harmonics = [
      { mult: 1.0, amp: 0.42, decay: 9.5 },
      { mult: 2.0, amp: 0.20, decay: 6.5 },
      { mult: 3.0, amp: 0.09, decay: 4.2 },
      { mult: 4.0, amp: 0.04, decay: 2.8 }
    ];
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.55, now + 0.007);
    master.gain.exponentialRampToValueAtTime(0.06, now + 9.5);

    harmonics.forEach((h, idx) => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = idx === 0 ? 'triangle' : 'sine';
      osc.frequency.setValueAtTime(freqHz * h.mult, now);
      osc.detune.setValueAtTime((idx % 2 === 0 ? 1 : -1) * 1.6, now);
      g.gain.setValueAtTime(h.amp, now);
      g.gain.exponentialRampToValueAtTime(0.02, now + h.decay);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + maxHoldSec);
      nodesToStop.push(osc);
    });
  } else if (preset === 'hammond') {
    // 🎛️ Organy Hammonda B3 (pełny ciągły sustain dopóki trzymasz klawisz!)
    const drawbars = [
      { mult: 0.5, amp: 0.16 },
      { mult: 1.0, amp: 0.28 },
      { mult: 1.498, amp: 0.14 },
      { mult: 2.0, amp: 0.20 },
      { mult: 2.996, amp: 0.10 },
      { mult: 4.0, amp: 0.12 }
    ];
    const lfo = ctx.createOscillator();
    const lfoGain = ctx.createGain();
    lfo.frequency.value = 6.2;
    lfoGain.gain.value = 7.5;
    lfo.start(now);
    lfo.stop(now + maxHoldSec);
    nodesToStop.push(lfo);

    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.42, now + 0.012);

    drawbars.forEach(d => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = 'sine';
      osc.frequency.setValueAtTime(freqHz * d.mult, now);
      lfo.connect(lfoGain);
      lfoGain.connect(osc.detune);
      g.gain.setValueAtTime(d.amp, now);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + maxHoldSec);
      nodesToStop.push(osc);
    });

    const perc = ctx.createOscillator();
    const pg = ctx.createGain();
    perc.type = 'sine';
    perc.frequency.setValueAtTime(freqHz * 3.0, now);
    pg.gain.setValueAtTime(0.18, now);
    pg.gain.exponentialRampToValueAtTime(0.001, now + 0.085);
    perc.connect(pg);
    pg.connect(master);
    perc.start(now);
    perc.stop(now + 0.09);
    nodesToStop.push(perc);
  } else if (preset === 'organ') {
    // ⛪ Organy klasyczne (pełny ciągły sustain piszczałek przez cały czas trzymania klawisza!)
    const pipes = [
      { mult: 0.5, type: 'triangle', amp: 0.16 },
      { mult: 1.0, type: 'triangle', amp: 0.24 },
      { mult: 2.0, type: 'sine', amp: 0.18 },
      { mult: 3.0, type: 'sine', amp: 0.09 },
      { mult: 4.0, type: 'sine', amp: 0.11 }
    ];
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.40, now + 0.045);

    pipes.forEach((p, idx) => {
      const osc = ctx.createOscillator();
      const g = ctx.createGain();
      osc.type = p.type;
      osc.frequency.setValueAtTime(freqHz * p.mult, now);
      osc.detune.setValueAtTime((idx - 2) * 2.2, now);
      g.gain.setValueAtTime(p.amp, now);
      osc.connect(g);
      g.connect(master);
      osc.start(now);
      osc.stop(now + maxHoldSec);
      nodesToStop.push(osc);
    });
  } else if (preset === 'rhodes') {
    // ✨ Fender Rhodes z podtrzymaniem (ciepły dzwonek + długi śpiewny sustain z tremolo podczas trzymania)
    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.48, now + 0.008);
    master.gain.exponentialRampToValueAtTime(0.12, now + 8.0);

    const carrier = ctx.createOscillator();
    const mod = ctx.createOscillator();
    const modGain = ctx.createGain();
    carrier.type = 'sine';
    carrier.frequency.setValueAtTime(freqHz, now);
    mod.type = 'sine';
    mod.frequency.setValueAtTime(freqHz * 7.0, now);
    modGain.gain.setValueAtTime(freqHz * 1.4, now);
    modGain.gain.exponentialRampToValueAtTime(0.8, now + 0.25);

    mod.connect(modGain);
    modGain.connect(carrier.frequency);
    carrier.connect(master);
    mod.start(now);
    carrier.start(now);
    mod.stop(now + maxHoldSec);
    carrier.stop(now + maxHoldSec);
    nodesToStop.push(mod, carrier);
  } else if (preset === 'synth') {
    // 🚀 Syntezator Lead (ciągły sustain z filtrem rezonansowym przez cały czas trzymania klawisza!)
    const filter = ctx.createBiquadFilter();
    filter.type = 'lowpass';
    filter.Q.value = 4.5;
    filter.frequency.setValueAtTime(freqHz * 6.5, now);
    filter.frequency.exponentialRampToValueAtTime(freqHz * 2.1, now + 0.35);
    filter.connect(master);

    master.gain.setValueAtTime(0.001, now);
    master.gain.linearRampToValueAtTime(0.28, now + 0.015);

    [-7, 7].forEach(det => {
      const osc = ctx.createOscillator();
      osc.type = 'sawtooth';
      osc.frequency.setValueAtTime(freqHz, now);
      osc.detune.setValueAtTime(det, now);
      osc.connect(filter);
      osc.start(now);
      osc.stop(now + maxHoldSec);
      nodesToStop.push(osc);
    });
  }

  activeSustainedVoices.set(voiceKey, {
    gainNode: voiceGain,
    nodesToStop,
    startedAt: now
  });
}

function midiToFreq(midi) {
  return state.a4ReferenceHz * Math.pow(2, (midi - 69) / 12);
}

// ===== Rozpoznawanie wysokości dźwięku gitary: dwuzakresowy McLeod MPM + Klapuri FFT dla dwudźwięków i akordów =====
// Szybkie okno (W_fast ~22 ms) dla cienkich strun G3/B3/E4 i szybkiego grania naprzemiennego,
// pełne okno dla grubych strun E2..D3, dwukierunkowa weryfikacja widmowa przeciw błędom oktaw na przesterze
// oraz iteracyjne odejmowanie harmonicznych Klapuriego dla dwudźwięków i akordów.
let pitchNsdfBuf = new Float32Array(4100);

const POLY_FFT_N = 4096;
const fftCos = new Float32Array(POLY_FFT_N / 2);
const fftSin = new Float32Array(POLY_FFT_N / 2);
const fftRev = new Uint16Array(POLY_FFT_N);
(function initFftTables() {
  for (let i = 0; i < POLY_FFT_N / 2; i++) {
    const a = (-2 * Math.PI * i) / POLY_FFT_N;
    fftCos[i] = Math.cos(a);
    fftSin[i] = Math.sin(a);
  }
  const bits = 12;
  for (let i = 0; i < POLY_FFT_N; i++) {
    let x = i, r = 0;
    for (let b = 0; b < bits; b++) { r = (r << 1) | (x & 1); x >>= 1; }
    fftRev[i] = r;
  }
})();
const fftRe = new Float32Array(POLY_FFT_N);
const fftIm = new Float32Array(POLY_FFT_N);
const polySpec = new Float32Array(POLY_FFT_N / 2);
const polyRes = new Float32Array(POLY_FFT_N / 2);
const surgeSpec = new Float32Array(POLY_FFT_N / 2);
const bgSurgeSpec = new Float32Array(POLY_FFT_N / 2);
const deltaSpec = new Float32Array(POLY_FFT_N / 2);

function runFftDecimated4(buf, decLen, sampleRate, asymRight = false) {
  const N = buf.length;
  const useDec = Math.min(decLen, Math.floor(N / 4));
  const off = N - useDec * 4;
  fftRe.fill(0);
  fftIm.fill(0);
  let mean = 0;
  for (let i = 0; i < useDec; i++) {
    const s = 0.25 * (buf[off + i * 4] + buf[off + i * 4 + 1] + buf[off + i * 4 + 2] + buf[off + i * 4 + 3]);
    mean += s;
    fftRe[fftRev[i]] = s;
  }
  mean /= Math.max(1, useDec);
  const invDm1 = 1 / Math.max(1, useDec - 1);
  for (let i = 0; i < useDec; i++) {
    const u = i * invDm1;
    // Okno asymetryczne (szczyt na 72% ku „teraz”) dla szybkiego widma nowego ataku albo Hann dla pełnego akordu
    const w = asymRight
      ? (u < 0.72 ? 0.5 - 0.5 * Math.cos(Math.PI * (u / 0.72)) : 0.5 + 0.5 * Math.cos(Math.PI * ((u - 0.72) / 0.28)))
      : (0.5 - 0.5 * Math.cos(2 * Math.PI * u));
    fftRe[fftRev[i]] = (fftRe[fftRev[i]] - mean) * w;
  }

  for (let size = 2; size <= POLY_FFT_N; size <<= 1) {
    const half = size >> 1;
    const step = POLY_FFT_N / size;
    for (let i = 0; i < POLY_FFT_N; i += size) {
      for (let j = 0, tw = 0; j < half; j++, tw += step) {
        const c = fftCos[tw], s = fftSin[tw];
        const re = fftRe[i + j + half], im = fftIm[i + j + half];
        const tre = re * c - im * s;
        const tim = re * s + im * c;
        fftRe[i + j + half] = fftRe[i + j] - tre;
        fftIm[i + j + half] = fftIm[i + j] - tim;
        fftRe[i + j] += tre;
        fftIm[i + j] += tim;
      }
    }
  }

  const effSr = sampleRate * 0.25;
  const binHz = effSr / POLY_FFT_N;
  const maxBin = Math.min(POLY_FFT_N / 2 - 1, Math.ceil(2800 / binHz));
  return { binHz, maxBin };
}

function extractNotesFromSpectrum(specBuf, binHz, maxBin, minMidi, maxMidi, a4Hz, maxNotes = 4) {
  for (let k = 0; k <= maxBin; k++) polyRes[k] = specBuf[k];
  const peakAround = (hz) => {
    const cBin = hz / binHz;
    if (cBin < 2 || cBin > maxBin - 2) return 0;
    const rad = Math.max(1, Math.round(cBin * 0.025));
    const lo = Math.max(1, Math.floor(cBin - rad));
    const hi = Math.min(maxBin - 1, Math.ceil(cBin + rad));
    let mx = 0;
    for (let k = lo; k <= hi; k++) if (polyRes[k] > mx) mx = polyRes[k];
    return mx;
  };

  let specPeak = 0;
  for (let k = 5; k <= maxBin; k++) if (polyRes[k] > specPeak) specPeak = polyRes[k];
  if (specPeak <= 1e-6) return { notes: [], topScore: 0, specPeak: 0 };

  const harmWeights = [1.0, 0.68, 0.45, 0.28, 0.16];
  const found = [];
  const foundFunds = [];
  const hannHalfBins = Math.max(3, Math.ceil(12.5 / binHz));
  let topScore = 0;

  for (let iter = 0; iter < maxNotes; iter++) {
    let bestMidi = -1, bestScore = 0, bestFund = 0;
    for (let m = minMidi; m <= maxMidi; m++) {
      if (found.includes(m)) continue;
      let tooClose = false;
      for (let fi = 0; fi < found.length; fi++) {
        if (Math.abs(found[fi] - m) === 1) {
          const df = Math.abs(a4Hz * Math.pow(2, (found[fi] - 69) / 12) - a4Hz * Math.pow(2, (m - 69) / 12));
          if (df < 15 || peakAround(a4Hz * Math.pow(2, (m - 69) / 12)) < foundFunds[fi] * 0.65) {
            tooClose = true;
            break;
          }
        }
      }
      if (tooClose) continue;

      const f = a4Hz * Math.pow(2, (m - 69) / 12);
      const fund = peakAround(f);
      if (fund < specPeak * 0.16) continue;
      let sc = 0;
      for (let h = 0; h < harmWeights.length; h++) sc += harmWeights[h] * peakAround(f * (h + 1));
      const e2 = peakAround(f * 2), e3 = peakAround(f * 3);
      if (fund < e2 * 0.45 && e3 < e2 * 0.30) sc *= 0.45;
      if (sc > bestScore) { bestScore = sc; bestMidi = m; bestFund = fund; }
    }
    if (bestMidi < 0) break;
    if (iter === 0) topScore = bestScore;
    if (iter > 0 && (bestScore < topScore * 0.40 || bestFund < specPeak * 0.26 || bestFund < foundFunds[0] * 0.30)) break;
    found.push(bestMidi);
    foundFunds.push(bestFund);

    const fFound = a4Hz * Math.pow(2, (bestMidi - 69) / 12);
    const subFracs = [1.0, 0.85, 0.72, 0.55, 0.40, 0.25];
    for (let h = 0; h < subFracs.length; h++) {
      const fh = fFound * (h + 1);
      const cBin = Math.round(fh / binHz);
      if (cBin > maxBin) break;
      const lobe = Math.max(hannHalfBins, Math.min(8, Math.round(cBin * 0.035) + 2));
      const cut = peakAround(fh) * subFracs[h];
      for (let k = Math.max(0, cBin - lobe); k <= Math.min(maxBin, cBin + lobe); k++) {
        const d = Math.abs(k - cBin) / (lobe + 0.5);
        polyRes[k] = Math.max(0, polyRes[k] - cut * Math.max(0, 1.0 - d * d));
      }
    }
  }
  return { notes: found, topScore, specPeak };
}

function detectPolyphonicNotes(buf, sampleRate, minMidi, maxMidi, a4Hz) {
  const { binHz, maxBin } = runFftDecimated4(buf, 1024, sampleRate, false);
  for (let k = 0; k <= maxBin; k++) {
    polySpec[k] = Math.pow(Math.hypot(fftRe[k], fftIm[k]), 0.72);
  }
  const res = extractNotesFromSpectrum(polySpec, binHz, maxBin, minMidi, maxMidi, a4Hz, 4);
  res.notes.sort((a, b) => a - b);
  return res.notes;
}

function analyzeSpectralSurge(buf, sampleRate, minMidi, maxMidi, a4Hz, activeMidis = []) {
  const { binHz, maxBin } = runFftDecimated4(buf, 768, sampleRate, true);
  let totalNow = 0, totalBg = 0, totalDelta = 0;
  for (let k = 0; k <= maxBin; k++) {
    const s = Math.pow(Math.hypot(fftRe[k], fftIm[k]), 0.72);
    surgeSpec[k] = s;
    const d = Math.max(0, s - bgSurgeSpec[k] * 1.12);
    deltaSpec[k] = d;
    if (k >= 5) {
      totalNow += s;
      totalBg += bgSurgeSpec[k];
      totalDelta += d;
    }
  }
  const hasPrev = totalBg > 0.5;
  const surgeRatio = hasPrev ? (totalDelta / Math.max(1e-6, totalNow)) : 0;
  let surgeMidi = null;
  if (hasPrev && surgeRatio >= 0.24 && totalDelta > 1.5) {
    const ext = extractNotesFromSpectrum(deltaSpec, binHz, maxBin, minMidi, maxMidi, a4Hz, 2);
    if (ext.notes.length > 0 && ext.specPeak > 0.30) {
      // Jeśli pierwsza nuta z przyrostu już brzmi, sprawdź wyższą nową nutę / interwał zagrany na jej tle
      let pick = ext.notes[0];
      if (activeMidis.includes(pick)) {
        const peakAt = (m) => {
          const c = (a4Hz * Math.pow(2, (m - 69) / 12)) / binHz;
          const r = Math.max(1, Math.round(c * 0.025));
          let mx = 0;
          for (let k = Math.max(1, Math.floor(c - r)); k <= Math.min(maxBin - 1, Math.ceil(c + r)); k++) {
            if (deltaSpec[k] > mx) mx = deltaSpec[k];
          }
          return mx;
        };
        const pPick = peakAt(pick);
        if (ext.notes.length > 1 && !activeMidis.includes(ext.notes[1]) && peakAt(ext.notes[1]) > pPick * 0.75) {
          pick = ext.notes[1];
        } else {
          let bestOver = pick, bestOverVal = pPick * 1.35;
          for (const stepUp of [12, 19, 24]) {
            const candM = pick + stepUp;
            if (candM <= maxMidi && !activeMidis.includes(candM)) {
              const v = peakAt(candM);
              if (v > bestOverVal) { bestOverVal = v; bestOver = candM; }
            }
          }
          pick = bestOver;
        }
      }
      surgeMidi = pick;
    }
  }
  const alpha = (surgeRatio >= 0.22) ? 0.10 : 0.22;
  for (let k = 0; k <= maxBin; k++) {
    bgSurgeSpec[k] = bgSurgeSpec[k] * (1 - alpha) + surgeSpec[k] * alpha;
  }
  return { surgeMidi, surgeRatio, totalDelta };
}

function goertzelMagWindowed(buf, off, W, sr, hz) {
  if (hz <= 0 || hz >= sr * 0.46) return 0;
  const w = (2 * Math.PI * hz) / sr;
  const step = W > 2200 ? 2 : 1;
  const cosW = Math.cos(w * step), sinW = Math.sin(w * step);
  let cosPhase = 1, sinPhase = 0, re = 0, im = 0;
  const invNm1 = (2 * Math.PI) / Math.max(1, W - 1);
  for (let i = 0; i < W; i += step) {
    const win = 0.5 - 0.5 * Math.cos(i * invNm1);
    const v = buf[off + i] * win;
    re += v * cosPhase;
    im -= v * sinPhase;
    const nc = cosPhase * cosW - sinPhase * sinW;
    sinPhase = sinPhase * cosW + cosPhase * sinW;
    cosPhase = nc;
  }
  return Math.hypot(re, im);
}

function runMpmWindow(buf, W, sampleRate, minHz, maxHz) {
  const N = buf.length;
  const useW = Math.min(N, W);
  const off = N - useW;
  const maxTau = Math.min(Math.ceil(sampleRate / minHz) + 2, Math.floor(useW * 0.48));
  const minTau = Math.max(2, Math.floor(sampleRate / maxHz) - 1);
  if (maxTau <= minTau + 2) return null;

  let mean = 0;
  for (let j = 0; j < useW; j++) mean += buf[off + j];
  mean /= useW;
  let sumSq = 0;
  for (let j = 0; j < useW; j++) {
    const v = buf[off + j] - mean;
    sumSq += v * v;
  }
  const rms = Math.sqrt(sumSq / useW);
  if (rms < 1e-6) return null;

  if (pitchNsdfBuf.length < maxTau + 3) pitchNsdfBuf = new Float32Array(maxTau + 3);
  const nsdf = pitchNsdfBuf;

  let m = sumSq * 2;
  for (let tau = 0; tau <= maxTau + 1; tau++) {
    if (tau > 0) {
      const a = buf[off + useW - tau] - mean;
      const b = buf[off + tau - 1] - mean;
      m -= a * a + b * b;
    }
    let r = 0;
    const lim = useW - tau;
    for (let j = 0; j < lim; j++) r += (buf[off + j] - mean) * (buf[off + j + tau] - mean);
    nsdf[tau] = m > 1e-9 ? (2 * r) / m : 0;
  }

  let pos = 0;
  while (pos < maxTau && nsdf[pos] > 0) pos++;
  const peaks = [];
  let curMax = -1;
  for (; pos <= maxTau; pos++) {
    const v = nsdf[pos];
    if (v > 0) {
      if (v > nsdf[pos - 1] && v >= nsdf[pos + 1] && (curMax < 0 || v > nsdf[curMax])) curMax = pos;
    } else if (curMax >= 0) {
      peaks.push(curMax);
      curMax = -1;
    }
  }
  if (curMax >= 0) peaks.push(curMax);

  let highest = 0;
  for (const p of peaks) if (p >= minTau && nsdf[p] > highest) highest = nsdf[p];
  if (highest < 0.45) return { hz: 0, clarity: highest, rms, ambiguous: false };

  const validPeaks = peaks.filter(p => p >= minTau && (sampleRate / p) >= minHz * 0.97 && (sampleRate / p) <= maxHz);
  if (validPeaks.length === 0) return { hz: 0, clarity: 0, rms, ambiguous: false };

  const mpmThresh = highest * 0.89;
  let chosen = -1;
  for (const p of validPeaks) {
    if (nsdf[p] >= mpmThresh) { chosen = p; break; }
  }
  if (chosen < 0) chosen = validPeaks[0];

  const gOff = off + Math.max(0, useW - Math.min(useW, 2048));
  const gW = Math.min(useW, 2048);
  let ambiguous = false;
  let didDemote = false;

  const fChosen = sampleRate / chosen;
  const subCands = validPeaks.filter(p => p > chosen * 1.4 && nsdf[p] >= nsdf[chosen] - 0.025);
  if (subCands.length > 0) {
    const e1 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen);
    const e2 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen * 2.0);
    const e05 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen * 0.5);
    const e15 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen * 1.5);
    const refE = Math.max(e1, e2 * 0.65);
    const octDown = subCands.find(p => Math.abs(p / chosen - 2.0) < 0.08);
    if (octDown && (e15 > e1 * 0.22 || e05 > e1 * 0.35) && (e05 + 1.2 * e15) > refE * 0.30) {
      chosen = octDown;
      didDemote = true;
      ambiguous = nsdf[chosen] < highest * 0.88;
    } else {
      const twDown = subCands.find(p => Math.abs(p / chosen - 3.0) < 0.08);
      if (twDown) {
        const e13 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen / 3);
        const e23 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen * (2 / 3));
        const e43 = goertzelMagWindowed(buf, gOff, gW, sampleRate, fChosen * (4 / 3));
        if ((e13 + e23 + e43) > refE * 0.38) {
          chosen = twDown;
          didDemote = true;
          ambiguous = nsdf[chosen] < highest * 0.88;
        }
      }
    }
  }

  if (!didDemote && chosen > minTau * 2) {
    const halfPeak = validPeaks.find(p => p < chosen && Math.abs(chosen / p - 2.0) < 0.08 && nsdf[p] >= highest * 0.72);
    if (halfPeak) {
      const fFast = sampleRate / halfPeak;
      const eFast = goertzelMagWindowed(buf, gOff, gW, sampleRate, fFast);
      const eSlow = goertzelMagWindowed(buf, gOff, gW, sampleRate, fFast * 0.5);
      const e15Fast = goertzelMagWindowed(buf, gOff, gW, sampleRate, fFast * 1.5);
      if (eFast > eSlow * 2.1 && e15Fast < eFast * 0.19) {
        chosen = halfPeak;
        if (nsdf[chosen] < highest * 0.88) ambiguous = true;
      }
    }
  }

  const s0 = nsdf[chosen - 1], s1 = nsdf[chosen], s2 = nsdf[chosen + 1];
  const denom = s0 - 2 * s1 + s2;
  let shift = Math.abs(denom) > 1e-12 ? 0.5 * (s0 - s2) / denom : 0;
  if (shift > 1 || shift < -1) shift = 0;
  const tau = chosen + shift;
  const clarity = Math.min(1, Math.max(nsdf[chosen], s1 - 0.25 * (s0 - s2) * shift));
  const hz = sampleRate / tau;
  if (hz < minHz * 0.97 || hz > maxHz) return { hz: 0, clarity: 0, rms, ambiguous: false };
  return { hz, clarity, rms, ambiguous };
}

function detectPitchMpm(buf, sampleRate, minHz, maxHz, minRmsGate = 0.0012, minMidiPoly = 40, maxMidiPoly = 80, a4Hz = 440, activeMidis = micPitchTracker.stablePoly) {
  const N = buf.length;
  const attackLen = Math.min(N, Math.max(256, Math.round(sampleRate * 0.012)));
  const atkOff = N - attackLen;
  let diffSq = 0, atkSq = 0, fullSq = 0;
  for (let j = 1; j < attackLen; j++) {
    const d = buf[atkOff + j] - buf[atkOff + j - 1];
    diffSq += d * d;
    atkSq += buf[atkOff + j] * buf[atkOff + j];
  }
  for (let j = 0; j < N; j++) fullSq += buf[j] * buf[j];
  const atkRms = Math.sqrt(atkSq / attackLen);
  const fullRms = Math.sqrt(fullSq / N);
  const hfRatio = atkSq > 1e-10 ? Math.sqrt(diffSq / atkSq) : 0;
  const quickMaxRms = Math.max(atkRms, fullRms);
  if (quickMaxRms < minRmsGate * 0.8) {
    bgSurgeSpec.fill(0);
    return { hz: 0, clarity: 0, rms: quickMaxRms, hfRatio, ambiguous: false, polyMidis: [], surgeMidi: null, surgeRatio: 0 };
  }

  const surge = analyzeSpectralSurge(buf, sampleRate, minMidiPoly, maxMidiPoly, a4Hz, activeMidis);

  const fastMinHz = Math.max(minHz, 155);
  const wFast = Math.min(N, Math.max(960, Math.round(sampleRate * 0.022)));
  const rFast = runMpmWindow(buf, wFast, sampleRate, fastMinHz, maxHz);

  const wFull = Math.min(N, Math.max(2048, (Math.ceil(sampleRate / minHz) + 2) * 3));
  const rFull = runMpmWindow(buf, wFull, sampleRate, minHz, maxHz);

  const effRms = Math.max(atkRms, rFull ? rFull.rms : 0, rFast ? rFast.rms : 0);
  if (effRms < minRmsGate) {
    return { hz: 0, clarity: 0, rms: effRms, hfRatio, ambiguous: false, polyMidis: [], surgeMidi: null, surgeRatio: 0 };
  }

  let best = rFull;
  if (rFast && rFast.hz > 0 && rFast.clarity >= 0.78) {
    const fFast = rFast.hz;
    const gOff = N - wFast;
    const e1 = goertzelMagWindowed(buf, gOff, wFast, sampleRate, fFast);
    const e05 = (fFast * 0.5 >= minHz * 0.92) ? goertzelMagWindowed(buf, gOff, wFast, sampleRate, fFast * 0.5) : 0;
    const e15 = (fFast * 0.5 >= minHz * 0.92) ? goertzelMagWindowed(buf, gOff, wFast, sampleRate, fFast * 1.5) : 0;
    const hasLowSub = (fFast * 0.5 >= minHz * 0.92) && ((e05 > e1 * 0.42) || (e15 > e1 * 0.28 && (!rFull || rFull.hz < fFast * 0.75)));
    if (!hasLowSub) {
      if (!rFull || rFull.hz <= 0 || rFull.clarity < 0.78 ||
          Math.abs(Math.log2(rFull.hz / fFast)) < 0.04 ||
          (rFast.clarity >= 0.86 && e1 > e05 * 2.0)) {
        best = rFast;
      }
    }
  }

  let polyMidis = [];
  if (!best || best.hz <= 0 || best.clarity < 0.83) {
    const poly = detectPolyphonicNotes(buf, sampleRate, minMidiPoly, maxMidiPoly, a4Hz);
    if (poly.length >= 2) polyMidis = poly;
  }

  return {
    hz: best && best.hz > 0 ? best.hz : 0,
    clarity: best ? best.clarity : 0,
    rms: effRms,
    hfRatio,
    ambiguous: best ? best.ambiguous : false,
    polyMidis,
    surgeMidi: surge.surgeMidi,
    surgeRatio: surge.surgeRatio
  };
}

const micPitchTracker = {
  stableMidi: null, stablePoly: [], lastExact: 0,
  ringingSuppressMap: new Map(),
  candMidi: null, candCount: 0, candStartMs: 0, candMinCents: 99, candMaxCents: -99,
  surgeCandMidi: null, surgeCandCount: 0, surgeCandStartMs: 0,
  polyCandKey: '', polyCandCount: 0, polyCandStartMs: 0,
  lastConfidentMs: 0, lastOnsetMs: -1e9, onsetPending: false,
  rmsHist: [], hfHist: [], steadyClarity: 0, clarityDipSeen: false,
  noiseFloor: 0.0007, clarity: 0
};

function resetMicPitchTracker() {
  const T = micPitchTracker;
  T.stableMidi = null; T.stablePoly = []; T.lastExact = 0;
  T.ringingSuppressMap.clear();
  T.candMidi = null; T.candCount = 0; T.candStartMs = 0;
  T.candMinCents = 99; T.candMaxCents = -99;
  T.surgeCandMidi = null; T.surgeCandCount = 0; T.surgeCandStartMs = 0;
  T.polyCandKey = ''; T.polyCandCount = 0; T.polyCandStartMs = 0;
  T.lastConfidentMs = 0; T.lastOnsetMs = -1e9; T.onsetPending = false;
  T.rmsHist = []; T.hfHist = []; T.steadyClarity = 0; T.clarityDipSeen = false;
  T.noiseFloor = 0.0007; T.clarity = 0;
  bgSurgeSpec.fill(0);
}

function updateMicPitchTracker(det, nowMs, a4Hz) {
  const T = micPitchTracker;

  if (det.clarity < 0.65 && (!det.polyMidis || det.polyMidis.length < 2)) {
    T.noiseFloor = T.noiseFloor * 0.92 + det.rms * 0.08;
  } else if (det.rms < T.noiseFloor) {
    T.noiseFloor = T.noiseFloor * 0.85 + det.rms * 0.15;
  }
  T.noiseFloor = Math.max(0.0004, Math.min(0.0045, T.noiseFloor));

  let histMin = Infinity;
  for (const v of T.rmsHist) if (v < histMin) histMin = v;
  T.rmsHist.push(det.rms);
  if (T.rmsHist.length > 5) T.rmsHist.shift();

  let hfMed = Infinity;
  for (const v of T.hfHist) if (v < hfMed) hfMed = v;
  T.hfHist.push(det.hfRatio);
  if (T.hfHist.length > 5) T.hfHist.shift();

  if (T.stableMidi !== null) {
    if ((T.steadyClarity >= 0.96 && det.clarity < 0.935) || det.clarity < 0.76) {
      T.clarityDipSeen = true;
    }
  }

  // Wykrywanie nowego dźwięku zagranego NA TLE wybranej, nadal wybrzmiewającej struny ("dźwięk na dźwięku")
  const surgeCooldownMs = (det.surgeMidi !== null && det.surgeMidi !== T.stableMidi) ? 70 : 180;
  const mpmExactCur = (det.hz > 0) ? (69 + 12 * Math.log2(det.hz / a4Hz)) : null;
  const isSteadySingleFalseSurge = (det.clarity >= 0.88 && mpmExactCur !== null && T.stableMidi !== null &&
    Math.abs(mpmExactCur - T.stableMidi) <= 0.45 && det.rms <= histMin * 1.15 &&
    det.surgeMidi !== null && Math.abs(det.surgeMidi - T.stableMidi) <= 2);
  const isBendUpperHarmonic = (det.clarity >= 0.92 && mpmExactCur !== null && T.stableMidi !== null &&
    Math.abs(mpmExactCur - T.stableMidi) >= 0.14 && Math.abs(mpmExactCur - T.stableMidi) <= 0.65 &&
    det.surgeMidi !== null && det.surgeMidi >= T.stableMidi + 18 && det.surgeRatio < 0.48 && !T.onsetPending);
  if (!isSteadySingleFalseSurge && !isBendUpperHarmonic && T.stableMidi !== null && det.surgeMidi !== null && det.surgeMidi !== undefined &&
      det.rms >= Math.max(0.002, T.noiseFloor * 2.0) && (nowMs - T.lastOnsetMs > surgeCooldownMs)) {
    if (det.surgeMidi === T.surgeCandMidi) T.surgeCandCount++;
    else { T.surgeCandMidi = det.surgeMidi; T.surgeCandCount = 1; T.surgeCandStartMs = nowMs; }

    if (T.surgeCandCount >= 2 && (nowMs - T.surgeCandStartMs) >= 10 &&
        (det.surgeMidi !== T.stableMidi || (det.surgeRatio >= 0.34 && (nowMs - T.lastOnsetMs) > 180))) {
      if (det.surgeMidi !== T.stableMidi) {
        T.ringingSuppressMap.set(T.stableMidi, nowMs + 950);
        for (const pm of T.stablePoly) {
          if (pm !== det.surgeMidi) T.ringingSuppressMap.set(pm, nowMs + 950);
        }
      }
      T.ringingSuppressMap.delete(det.surgeMidi);
      T.lastOnsetMs = nowMs;
      T.stableMidi = det.surgeMidi;
      T.stablePoly = (det.polyMidis && det.polyMidis.length >= 2 && det.polyMidis.includes(det.surgeMidi))
        ? det.polyMidis.slice()
        : [det.surgeMidi];
      T.lastExact = det.surgeMidi;
      T.lastConfidentMs = nowMs;
      T.candMidi = det.surgeMidi;
      T.candCount = 0;
      T.surgeCandMidi = null;
      T.surgeCandCount = 0;
      T.onsetPending = false;
      T.clarityDipSeen = false;
      T.clarity = Math.max(0.82, det.clarity);
      return { event: 'on', midi: det.surgeMidi, polyMidis: T.stablePoly, exactMidi: det.surgeMidi };
    }
  } else {
    T.surgeCandMidi = null;
    T.surgeCandCount = 0;
  }

  const holdingForNewSurge = (T.surgeCandCount > 0 && T.surgeCandMidi !== null && T.surgeCandMidi !== T.stableMidi);
  const rmsJump = !holdingForNewSurge && det.rms > Math.max(0.005, T.noiseFloor * 2.2) && histMin < Infinity && det.rms > histMin * 1.5;
  const hfSpike = !holdingForNewSurge && T.stableMidi !== null && hfMed < Infinity && hfMed > 0.05 && det.hfRatio > hfMed * 1.42 && det.rms > 0.015;
  const pickDipRecovery = !holdingForNewSurge && T.stableMidi !== null && T.clarityDipSeen && det.clarity >= 0.90;
  if ((rmsJump || hfSpike || pickDipRecovery) && nowMs - T.lastOnsetMs > 120) {
    T.lastOnsetMs = nowMs;
    T.onsetPending = true;
    T.candCount = 0;
    T.candMidi = null;
    T.candStartMs = nowMs;
    T.clarityDipSeen = false;
    T.steadyClarity = 0;
  }

  const minGate = det.clarity >= 0.84 ? Math.max(0.0008, T.noiseFloor * 1.2) : Math.max(0.0012, T.noiseFloor * 1.9);
  const monoValid = det.hz > 0 && det.rms >= minGate && det.clarity >= 0.72;
  const polyValid = !monoValid && det.rms >= Math.max(0.003, T.noiseFloor * 2.4) && det.polyMidis && det.polyMidis.length >= 2;

  if (polyValid) {
    T.candMidi = null; T.candCount = 0;
    const key = det.polyMidis.join(',');
    if (key === T.polyCandKey) T.polyCandCount++;
    else { T.polyCandKey = key; T.polyCandCount = 1; T.polyCandStartMs = nowMs; }

    if (key === T.stablePoly.join(',')) {
      T.lastConfidentMs = nowMs;
      return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: null };
    }
    if (T.polyCandCount >= 3 && (nowMs - T.polyCandStartMs) >= 35) {
      const prevSet = new Set(T.stablePoly);
      const newlyAdded = det.polyMidis.find(m => !prevSet.has(m));
      if (newlyAdded === undefined && det.rms <= histMin * 1.15) {
        T.stablePoly = det.polyMidis.slice();
        T.lastConfidentMs = nowMs;
        return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: null };
      }
      T.stableMidi = newlyAdded !== undefined ? newlyAdded : det.polyMidis[0];
      T.stablePoly = det.polyMidis.slice();
      T.lastConfidentMs = nowMs;
      T.clarity = Math.max(0.7, det.clarity);
      return { event: 'on', midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: T.stableMidi };
    }
    return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: null };
  }

  if (!monoValid) {
    T.candMidi = null; T.candCount = 0;
    T.polyCandKey = ''; T.polyCandCount = 0;
    if ((T.stableMidi !== null || T.stablePoly.length > 0) && nowMs - T.lastConfidentMs > 155) {
      T.stableMidi = null; T.stablePoly = []; T.clarity = 0; T.clarityDipSeen = false; T.steadyClarity = 0;
      T.ringingSuppressMap.clear();
      return { event: 'off', midi: null, polyMidis: [], exactMidi: null };
    }
    return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: null };
  }

  T.polyCandKey = ''; T.polyCandCount = 0;
  const exact = 69 + 12 * Math.log2(det.hz / a4Hz);
  // Histereza kwantyzacji (±10 ct zapasu przy mocniejszym dociśnięciu struny, żeby nuta nie przeskakiwała o półton)
  let midi = Math.round(exact);
  if (T.stableMidi !== null && !T.onsetPending && Math.abs(exact - T.stableMidi) <= 0.60) {
    midi = T.stableMidi;
  }
  const centsFromMidi = (exact - midi) * 100;

  if (midi === T.candMidi) {
    T.candCount++;
    T.candMinCents = Math.min(T.candMinCents, centsFromMidi);
    T.candMaxCents = Math.max(T.candMaxCents, centsFromMidi);
  } else {
    T.candMidi = midi;
    T.candCount = 1;
    T.candStartMs = nowMs;
    T.candMinCents = centsFromMidi;
    T.candMaxCents = centsFromMidi;
  }

  if (midi === T.stableMidi && T.stablePoly.length <= 1) {
    T.lastConfidentMs = nowMs;
    T.lastExact = exact;
    T.clarity = det.clarity;
    T.steadyClarity = T.steadyClarity * 0.7 + det.clarity * 0.3;
  }

  const supUntil = T.ringingSuppressMap.get(midi) || 0;
  const isSuppressedRingingOld = (nowMs < supUntil && !T.onsetPending);
  const isNew = !isSuppressedRingingOld && ((midi !== T.stableMidi) || (T.stablePoly.length > 1) || T.onsetPending);
  if (!isNew) {
    if (isSuppressedRingingOld) T.lastConfidentMs = nowMs;
    return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly.length ? T.stablePoly : [T.stableMidi], exactMidi: (midi === T.stableMidi) ? exact : null };
  }

  const heldMs = nowMs - T.candStartMs;
  const spread = T.candMaxCents - T.candMinCents;
  const nearBorder = Math.abs(centsFromMidi) > 43;

  let needCount = 2;
  let needMs = 15;
  if (det.ambiguous || det.clarity < 0.85 || nearBorder || spread > 36) {
    needCount = 3;
    needMs = 42;
  } else if (det.clarity < 0.91) {
    needCount = 2;
    needMs = 22;
  }
  if (T.stableMidi !== null && !T.onsetPending && Math.abs(midi - T.stableMidi) % 12 === 0 && det.clarity < 0.90) {
    needCount = Math.max(needCount, 3);
    needMs = Math.max(needMs, 34);
  }

  if (T.candCount >= needCount && heldMs >= needMs) {
    T.stableMidi = midi;
    T.stablePoly = [midi];
    T.lastExact = exact;
    T.lastConfidentMs = nowMs;
    T.lastOnsetMs = nowMs;
    T.onsetPending = false;
    T.clarityDipSeen = false;
    T.steadyClarity = det.clarity;
    T.clarity = det.clarity;
    return { event: 'on', midi, polyMidis: [midi], exactMidi: exact };
  }
  return { event: null, midi: T.stableMidi, polyMidis: T.stablePoly, exactMidi: (midi === T.stableMidi) ? exact : null };
}

function identifyPolyChordLabel(polyMidis, noteNames) {
  if (!polyMidis || polyMidis.length < 2) return '';
  if (polyMidis.length === 2) {
    const semi = ((polyMidis[1] - polyMidis[0]) % 12 + 12) % 12;
    const root = noteNames[((polyMidis[0] % 12) + 12) % 12];
    if (semi === 7) return `Dwudźwięk: ${root}5 (Kwinta)`;
    if (semi === 0) return `Dwudźwięk: ${root} (Oktawa)`;
    if (semi === 4) return `Dwudźwięk: ${root} (Tercja wielka)`;
    if (semi === 3) return `Dwudźwięk: ${root} (Tercja mała)`;
    if (semi === 5) return `Dwudźwięk: ${root} (Kwarta)`;
    return `Dwudźwięk (${polyMidis.length} nuty)`;
  }
  const pcs = [...new Set(polyMidis.map(m => ((m % 12) + 12) % 12))];
  const tpl = [
    ['dur', [0, 4, 7]], ['mol', [0, 3, 7]], ['5', [0, 7]],
    ['7', [0, 4, 7, 10]], ['maj7', [0, 4, 7, 11]], ['m7', [0, 3, 7, 10]],
    ['sus4', [0, 5, 7]], ['sus2', [0, 2, 7]], ['dim', [0, 3, 6]], ['aug', [0, 4, 8]]
  ];
  for (const r of pcs) {
    const rel = new Set(pcs.map(p => (p - r + 12) % 12));
    for (const [nm, ivs] of tpl) {
      if (ivs.every(x => rel.has(x)) && pcs.length <= ivs.length + 1) {
        return `Akord: ${noteNames[r]} ${nm}`;
      }
    }
  }
  return `Wielodźwięk (${polyMidis.length} nuty)`;
}

async function refreshAudioDevices(requestPermissionFirst = false) {
  if (!navigator.mediaDevices || !navigator.mediaDevices.enumerateDevices) return;
  try {
    if (requestPermissionFirst) {
      const tempStream = await navigator.mediaDevices.getUserMedia({ audio: true });
      tempStream.getTracks().forEach(t => t.stop());
    }
    const devices = await navigator.mediaDevices.enumerateDevices();
    const ins = devices.filter(d => d.kind === 'audioinput').map((d, idx) => ({
      id: d.deviceId || 'default',
      label: d.label || `Wejście audio #${idx + 1} (Mikrofon / Interfejs)`
    }));
    const outs = devices.filter(d => d.kind === 'audiooutput').map((d, idx) => ({
      id: d.deviceId || 'default',
      label: d.label || `Urządzenie odtwarzania #${idx + 1} (Głośniki / Słuchawki)`
    }));
    if (ins.length > 0) state.audioInputDevices = ins;
    if (outs.length > 0) state.audioOutputDevices = outs;
    populateAudioSelectElements();
    renderStageAndInspector();
  } catch (err) {
    console.warn('Nie udało się pobrać listy urządzeń audio:', err);
  }
}

function populateAudioSelectElements() {
  const inSel = document.getElementById('audioInputSelect');
  if (inSel) {
    inSel.innerHTML = state.audioInputDevices.map(d =>
      `<option value="${d.id}" ${d.id === state.selectedAudioInputId ? 'selected' : ''}>${d.label}</option>`
    ).join('');
  }
  const outSel = document.getElementById('audioOutputSelect');
  if (outSel) {
    outSel.innerHTML = state.audioOutputDevices.map(d =>
      `<option value="${d.id}" ${d.id === state.selectedAudioOutputId ? 'selected' : ''}>${d.label}</option>`
    ).join('');
  }
}

async function onAudioInputChanged(deviceId) {
  state.selectedAudioInputId = deviceId;
  populateAudioSelectElements();
  if (micStream) {
    await toggleLiveAudio();
    await toggleLiveAudio();
  }
}

async function onAudioOutputChanged(deviceId) {
  state.selectedAudioOutputId = deviceId;
  populateAudioSelectElements();
  const ctx = ensureAudioCtx();
  if (typeof ctx.setSinkId === 'function') {
    try {
      await ctx.setSinkId(deviceId === 'default' ? '' : deviceId);
    } catch (e) {
      console.warn('setSinkId nieobsługiwane dla tego wyjścia:', e);
    }
  }
}

async function toggleLiveAudio() {
  const dot = document.getElementById('micDot');
  const txt = document.getElementById('micStatusText');
  const btn = document.getElementById('toggleMicBtn');
  if (micStream) {
    if (micPollTimer) { clearInterval(micPollTimer); micPollTimer = null; }
    if (audioLoopId) { cancelAnimationFrame(audioLoopId); audioLoopId = null; }
    micStream.getTracks().forEach(t => t.stop());
    micStream = null;
    dot.classList.remove('on');
    txt.textContent = 'Mikrofon wył.';
    if (btn) {
      btn.textContent = '🎤 Włącz mikrofon';
      btn.style.background = 'var(--accent)';
      btn.style.color = '#120e0a';
    }
    state.detectedMidi = null;
    state.detectedMidis = [];
    document.getElementById('liveNoteBadge').textContent = '—';
    document.getElementById('liveCentsBadge').textContent = '0.0 ct · 0.0 Hz';
    drawFretboard();
    updateGuitarToKeyboardSync();
    if (state.mainView === 'tuner') renderTunerCanvas();
    return;
  }
  try {
    const ctx = ensureAudioCtx();
    const audioConstraints = {
      echoCancellation: false,
      noiseSuppression: false,
      autoGainControl: false
    };
    if (state.selectedAudioInputId && state.selectedAudioInputId !== 'default') {
      audioConstraints.deviceId = { exact: state.selectedAudioInputId };
    }
    micStream = await navigator.mediaDevices.getUserMedia({ audio: audioConstraints });
    await refreshAudioDevices(false);
    const src = ctx.createMediaStreamSource(micStream);
    // Dwustopniowy filtr górnoprzepustowy (-24 dB/okt): tłumi brum sieciowy 50/60 Hz bez straty najniższej struny;
    // filtr dolnoprzepustowy 4200 Hz chroni wysokie harmoniczne cienkich strun (B3..E6).
    const hpCutoff = Math.max(30, midiToFreq(Math.min(...state.customStringMidis)) * 0.80);
    const micHp1 = ctx.createBiquadFilter();
    micHp1.type = 'highpass';
    micHp1.frequency.value = hpCutoff;
    micHp1.Q.value = 0.707;
    const micHp2 = ctx.createBiquadFilter();
    micHp2.type = 'highpass';
    micHp2.frequency.value = hpCutoff;
    micHp2.Q.value = 0.707;
    const micLowpass = ctx.createBiquadFilter();
    micLowpass.type = 'lowpass';
    micLowpass.frequency.value = 4200;
    micLowpass.Q.value = 0.707;
    const decimate2 = ctx.sampleRate > 54000;
    analyserNode = ctx.createAnalyser();
    analyserNode.fftSize = decimate2 ? 8192 : 4096;
    analyserNode.smoothingTimeConstant = 0;
    src.connect(micHp1);
    micHp1.connect(micHp2);
    micHp2.connect(micLowpass);
    micLowpass.connect(analyserNode);
    dot.classList.add('on');
    txt.textContent = 'Mikrofon wł. (nasłuch)';
    if (btn) {
      btn.textContent = '🔇 Wyłącz mikrofon';
      btn.style.background = 'var(--red)';
      btn.style.color = '#ffffff';
    }

    const rawBuf = new Float32Array(analyserNode.fftSize);
    const analysisBuf = decimate2 ? new Float32Array(4096) : rawBuf;
    const effSampleRate = decimate2 ? (ctx.sampleRate * 0.5) : ctx.sampleRate;
    let lastAppliedLowest = -1;
    micPollBusy = false;
    resetMicPitchTracker();

    const pollAudio = () => {
      if (!micStream || micPollBusy) return;
      micPollBusy = true;
      try {
        const lowest = Math.min(...state.customStringMidis);
        const highest = Math.max(...state.customStringMidis) + 24;
        if (lowest !== lastAppliedLowest) {
          lastAppliedLowest = lowest;
          const cut = Math.max(30, midiToFreq(lowest) * 0.80);
          micHp1.frequency.value = cut;
          micHp2.frequency.value = cut;
        }
        analyserNode.getFloatTimeDomainData(rawBuf);
        if (decimate2) {
          for (let i = 0; i < 4096; i++) analysisBuf[i] = 0.5 * (rawBuf[i * 2] + rawBuf[i * 2 + 1]);
        }
        const minHz = Math.max(30, midiToFreq(lowest - 2));
        const a4 = state.a4ReferenceHz || 440;
        const det = detectPitchMpm(analysisBuf, effSampleRate, minHz, 1500, 0.0012, lowest, Math.min(84, highest), a4, micPitchTracker.stablePoly);
        const tr = updateMicPitchTracker(det, performance.now(), a4);
        const T = micPitchTracker;
        const noteNames = getRootLabels();
        const activePoly = (T.stablePoly && T.stablePoly.length > 0) ? T.stablePoly : (T.stableMidi !== null ? [T.stableMidi] : []);

        if (activePoly.length > 0) {
          const isPoly = activePoly.length >= 2;
          const playedMidi = T.stableMidi !== null ? T.stableMidi : activePoly[0];
          const exactMidi = (!isPoly && tr.exactMidi !== null && tr.exactMidi !== undefined) ? tr.exactMidi : (isPoly ? playedMidi : T.lastExact);
          let targetMidi = playedMidi;
          if (!isPoly && state.mainView === 'tuner') {
            if (state.tunerLockedString >= 0 && state.customStringMidis[state.tunerLockedString] !== undefined) {
              targetMidi = state.customStringMidis[state.tunerLockedString];
            } else if (state.tunerMode === 'guitar' && state.customStringMidis.length === 6) {
              targetMidi = state.customStringMidis.reduce((best, m) =>
                Math.abs(m - exactMidi) < Math.abs(best - exactMidi) ? m : best,
                state.customStringMidis[0]
              );
            }
          }
          const cents = isPoly ? 0 : Math.max(-50, Math.min(50, (exactMidi - targetMidi) * 100));
          const hz = a4 * Math.pow(2, (exactMidi - 69) / 12);
          state.detectedMidi = targetMidi;
          state.detectedMidis = isPoly ? activePoly.slice() : [targetMidi];
          state.detectedCents = cents;
          state.detectedHz = hz;

          const noteLabel = isPoly
            ? activePoly.map(m => noteNames[((m % 12) + 12) % 12] + ' ' + (Math.floor(m / 12) - 1)).join(' + ')
            : (noteNames[((playedMidi % 12) + 12) % 12] + ' ' + (Math.floor(playedMidi / 12) - 1));
          document.getElementById('liveNoteBadge').textContent = noteLabel;
          document.getElementById('liveCentsBadge').textContent = isPoly
            ? identifyPolyChordLabel(activePoly, noteNames)
            : `${cents >= 0 ? '+' : ''}${cents.toFixed(1)} ct · ${hz.toFixed(1)} Hz · ${Math.round(T.clarity * 100)}%`;

          if (tr.event === 'on') {
            const heardBadge = document.getElementById('exerciseHeardBadge');
            if (heardBadge) heardBadge.textContent = `(słyszę: ${noteLabel})`;
            const earBox = document.getElementById('earPlayDetectedBox');
            if (earBox && !earBox.dataset.answered) {
              earBox.textContent = noteNames[((playedMidi % 12) + 12) % 12] + (Math.floor(playedMidi / 12) - 1);
            }
            if (state.mainView === 'practice' && state.subtab === 'exercise') {
              if (state.exercisePlayerActive && state.exercisePlayerMode === 'play') {
                for (const pm of activePoly) checkExercisePlayerPlayedMidi(pm);
              } else if (!state.exercisePlayerActive && state.tabCreationEnabled) {
                recordMidiNoteToTab(playedMidi);
              }
            }
            if (state.mainView === 'ear' && state.earScreen === 'active' && state.earMode === 'play') {
              for (const pm of activePoly) checkEarPlayGuitarMidi(pm);
            }
            if (state.mainView === 'changes' && state.changesScreen === 'practice_active') {
              checkChangesPracticePlayedMidi(playedMidi);
            }
            drawFretboard();
          }
        } else if (tr.event === 'off') {
          state.detectedMidi = null;
          state.detectedMidis = [];
          document.getElementById('liveNoteBadge').textContent = '—';
          document.getElementById('liveCentsBadge').textContent = '0.0 ct · 0.0 Hz';
          const heardBadge = document.getElementById('exerciseHeardBadge');
          if (heardBadge) heardBadge.textContent = '';
          const earBox = document.getElementById('earPlayDetectedBox');
          if (earBox && !earBox.dataset.answered) earBox.textContent = '—';
          drawFretboard();
        }
      } finally {
        micPollBusy = false;
      }
    };

    micPollTimer = setInterval(pollAudio, 10);
    const renderLoop = () => {
      if (!micStream) return;
      if (state.mainView === 'tuner') renderTunerCanvas();
      audioLoopId = requestAnimationFrame(renderLoop);
    };
    renderLoop();
  } catch (err) {
    alert('Nie udało się otworzyć wejścia audio: ' + err.message);
  }
}

function getRootLabels() {
  if (state.accMode === 1) return ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B'];
  return ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
}

async function fetchJson(url) {
  const res = await fetch(url);
  return await res.json();
}

function setFretboardZoom(zoomVal) {
  state.fretboardZoom = Math.max(-5, Math.min(10, zoomVal));
  const slider = document.getElementById('fretboardZoomSlider');
  if (slider) slider.value = state.fretboardZoom;
  const barSlider = document.getElementById('toolbarFretboardZoomSlider');
  if (barSlider) barSlider.value = state.fretboardZoom;
  const lbl = document.getElementById('fretboardZoomLabel');
  if (lbl) lbl.textContent = (state.fretboardZoom > 0 ? '+' : '') + state.fretboardZoom;
  const barLbl = document.getElementById('toolbarFretboardZoomLabel');
  if (barLbl) barLbl.textContent = (state.fretboardZoom > 0 ? '+' : '') + state.fretboardZoom;
  saveSettings();
  drawFretboard();
}

function toggleFretboardFitWidth() {
  state.fretboardFitWidth = !state.fretboardFitWidth;
  saveSettings();
  renderFretboardToolbarForActiveTab();
  drawFretboard();
}

async function initApp() {
  await loadSettings();
  state.initData = await fetchJson('/api/init');
  if (state.tuningId !== 'custom' && state.initData && state.initData.tunings) {
    const found = state.initData.tunings.find(t => t.id === state.tuningId);
    if (found && found.midis && found.midis.length === 6) {
      state.customStringMidis = found.midis.slice();
    }
  }
  // Domyślnie przy starcie nic nie jest zamaskowane (wszystkie progi do maxDisplayedFret są odkryte!)
  state.maskLeft = [0, 0, 0, 0, 0, 0];
  state.maskRight = [
    state.maxDisplayedFret, state.maxDisplayedFret, state.maxDisplayedFret,
    state.maxDisplayedFret, state.maxDisplayedFret, state.maxDisplayedFret
  ];
  buildMidiKeyboardUI();
  refreshAudioDevices(false);
  setupFretboardDragScroll();
  window.addEventListener('resize', () => {
    drawFretboard();
    if (state.mainView === 'tuner') renderTunerCanvas();
  });
  await renderAll();
}

function setupFretboardDragScroll() {
  const wrap = document.getElementById('fretboardWrap');
  if (!wrap) return;
  let isPanning = false;
  let startX = 0;
  let scrollLeftStart = 0;
  wrap.addEventListener('mousedown', e => {
    // Przeciąganie gryfu prawo-lewo przy wciśniętym środkowym przycisku lub z Alt
    if (e.button === 1 || (e.button === 0 && e.altKey)) {
      isPanning = true;
      startX = e.pageX - wrap.offsetLeft;
      scrollLeftStart = wrap.scrollLeft;
    }
  });
  wrap.addEventListener('wheel', e => {
    if (e.ctrlKey || e.metaKey) {
      e.preventDefault();
      const delta = e.deltaY < 0 ? 1 : -1;
      setFretboardZoom((state.fretboardZoom || 0) + delta);
    }
  }, { passive: false });
  window.addEventListener('mousemove', e => {
    if (!isPanning) return;
    const x = e.pageX - wrap.offsetLeft;
    wrap.scrollLeft = scrollLeftStart - (x - startX);
  });
  window.addEventListener('mouseup', () => { isPanning = false; });
}

async function refreshFretboardFromCpp() {
  const customParam = state.customStringMidis.join(',');
  const effRootPc = (state.subtab === 'chord' && state.chordFilterByScale) ? state.chordScaleRootPc : state.rootPc;
  const effScaleId = (state.subtab === 'chord' && state.chordFilterByScale) ? state.chordScaleId : state.scaleId;
  const url = `/api/fretboard?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${effRootPc}&scaleId=${effScaleId}&accMode=${state.accMode}&posSystem=${state.posSystem}&posIdx=${state.posIdx}`;
  state.fretboardData = await fetchJson(url);
  updateTopTuningLabel();
  renderFretboardToolbarForActiveTab();
}

function toggleFretboardSettingsModal() {
  const modal = document.getElementById('fretboardSettingsModal');
  if (!modal) return;
  modal.style.display = (modal.style.display === 'none' || !modal.style.display) ? 'flex' : 'none';
  renderTuningModalControls();
}

function updateTopTuningLabel() {
  const lbl = document.getElementById('topTuningLabel');
  if (!lbl || !state.initData) return;
  const noteNames = getRootLabels();
  const strNotes = state.customStringMidis.map(m => noteNames[((m % 12) + 12) % 12]).join(' ');
  const found = state.initData.tunings.find(t => t.id === state.tuningId);
  lbl.textContent = found ? `${found.name.split('(')[0].trim()} (${strNotes})` : `Własny (${strNotes})`;
}

function setLockUnmarkedNotes(on) {
  state.lockUnmarkedNotes = !!on;
  const onBtn = document.getElementById('lockUnmarkedModalOnBtn');
  const offBtn = document.getElementById('lockUnmarkedModalOffBtn');
  if (onBtn) onBtn.classList.toggle('active', state.lockUnmarkedNotes);
  if (offBtn) offBtn.classList.toggle('active', !state.lockUnmarkedNotes);
  saveSettings();
  renderFretboardToolbarForActiveTab();
  drawFretboard();
}

function renderTuningModalControls() {
  if (!state.initData) return;
  const onBtn = document.getElementById('lockUnmarkedModalOnBtn');
  const offBtn = document.getElementById('lockUnmarkedModalOffBtn');
  if (onBtn) onBtn.classList.toggle('active', !!state.lockUnmarkedNotes);
  if (offBtn) offBtn.classList.toggle('active', !state.lockUnmarkedNotes);
  const flatBtn = document.getElementById('accModeFlatBtn');
  const sharpBtn = document.getElementById('accModeSharpBtn');
  if (flatBtn) flatBtn.classList.toggle('active', state.accMode === 1);
  if (sharpBtn) sharpBtn.classList.toggle('active', state.accMode !== 1);
  const sel = document.getElementById('globalTuningSelect');
  if (sel) {
    sel.innerHTML = state.initData.tunings.map(t =>
      `<option value="${t.id}" ${t.id===state.tuningId?'selected':''}>${t.name}</option>`
    ).join('') + `<option value="custom" ${state.tuningId==='custom'?'selected':''}>Własny strój (niestandardowy)</option>`;
  }
  const row = document.getElementById('customStringsRow');
  if (row) {
    const noteNames = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
    row.innerHTML = state.customStringMidis.map((midi, sIdx) => {
      // Bez numerków oktaw – sama nazwa dźwięku (z użyciem # zamiast b)!
      const nm = noteNames[((midi % 12) + 12) % 12];
      return `
        <div style="background:var(--bg-panel);border:1px solid var(--border);border-radius:7px;padding:4px 8px;display:flex;align-items:center;gap:5px;font-size:12px;">
          <span style="color:var(--text-muted);">Str.${6 - sIdx}:</span>
          <button class="chip-btn" style="padding:2px 6px;" onclick="tuneSingleString(${sIdx}, -1)">−</button>
          <strong style="color:var(--accent);min-width:24px;text-align:center;">${nm}</strong>
          <button class="chip-btn" style="padding:2px 6px;" onclick="tuneSingleString(${sIdx}, 1)">+</button>
        </div>`;
    }).join('');
  }
}

function selectPresetTuning(tId) {
  state.tuningId = tId;
  if (state.initData) {
    const found = state.initData.tunings.find(t => t.id === tId);
    if (found && found.midis && found.midis.length === 6) {
      state.customStringMidis = found.midis.slice();
    }
  }
  saveSettings();
  renderTuningModalControls();
  renderAll();
}

function tuneSingleString(sIdx, semiDelta) {
  state.tuningId = 'custom';
  state.customStringMidis[sIdx] = Math.max(28, Math.min(76, state.customStringMidis[sIdx] + semiDelta));
  pluckGuitarFrequency(midiToFreq(state.customStringMidis[sIdx]), 1.2);
  saveSettings();
  renderTuningModalControls();
  renderAll();
}

function setMaxFrets(n) {
  const oldMax = state.maxDisplayedFret || 22;
  state.maxDisplayedFret = n;
  state.maskRight = state.maskRight.map(v => (v >= oldMax) ? n : Math.min(n, v));
  saveSettings();
  drawFretboard();
  if (state.mainView === 'settings') renderStageAndInspector();
}

function buildFretboardZoomControlsHtml() {
  const z = state.fretboardZoom || 0;
  const zStr = (z > 0 ? '+' : '') + z;
  return `
    <div style="display:flex;align-items:center;gap:5px;background:var(--bg-main);padding:3px 8px;border-radius:7px;border:1px solid var(--border);">
      <span style="font-size:11px;color:var(--text-muted);">Skaluj gryf:</span>
      <button class="chip-btn" style="padding:2px 7px;font-size:12px;" onclick="setFretboardZoom((state.fretboardZoom||0)-1)">−</button>
      <input type="range" id="toolbarFretboardZoomSlider" min="-5" max="10" step="1" value="${z}" style="width:85px;"
             oninput="setFretboardZoom(parseInt(this.value,10))">
      <button class="chip-btn" style="padding:2px 7px;font-size:12px;" onclick="setFretboardZoom((state.fretboardZoom||0)+1)">+</button>
      <span id="toolbarFretboardZoomLabel" style="font-size:11.5px;font-weight:700;color:var(--accent);min-width:22px;text-align:center;">${zStr}</span>
      <button class="chip-btn" style="padding:2px 6px;font-size:11px;" onclick="setFretboardZoom(0)">0</button>
      <button class="chip-btn ${state.fretboardFitWidth?'active':''}" style="padding:2px 7px;font-size:11px;"
              onclick="toggleFretboardFitWidth()" title="Rozciągnij gryf na pełną szerokość dużego ekranu (Full Screen)">↔ Pełna szer.</button>
      <button class="chip-btn ${state.lockUnmarkedNotes?'active':''}" style="padding:2px 8px;font-size:11px;"
              onclick="setLockUnmarkedNotes(!state.lockUnmarkedNotes)" title="Zablokuj klikanie niezaznaczonych dźwięków na gryfie">
        🔒 Blokuj niezaznaczone: ${state.lockUnmarkedNotes ? 'Wł.' : 'Wył.'}
      </button>
    </div>`;
}

function renderFretboardToolbarForActiveTab() {
  const bar = document.getElementById('fretboardToolbarContent');
  if (!bar || !state.fretboardData) return;

  const zoomCtrl = buildFretboardZoomControlsHtml();

  if (state.subtab === 'scale') {
    const isRec = !!state.scaleRecognitionOn;
    bar.innerHTML = `
      <div style="display:flex;align-items:center;gap:10px;flex-wrap:wrap;">
        <strong style="color:var(--text);font-size:14px;">🎵 Skala: ${state.fretboardData.scaleFullName}</strong>
        <button class="chip-btn ${isRec?'active':''}" style="padding:5px 12px;font-size:12px;"
                onclick="state.scaleRecognitionOn=!state.scaleRecognitionOn;renderFretboardToolbarForActiveTab();drawFretboard()">
          🎯 Tryb rozpoznawania na gryfie
        </button>
        ${isRec ? `
          <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="simulateDetectedNote(64)">Symuluj E 4</button>
          <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="simulateDetectedNote(57)">Symuluj A 3</button>
        ` : ''}
      </div>
      <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
        ${zoomCtrl}
        <span>Aktywne progi:</span>
        <input type="number" id="maskMinFret" value="0" min="0" max="24" style="width:48px;padding:3px 5px;" onchange="applyFretRangeMask()">
        <span>–</span>
        <input type="number" id="maskMaxFret" value="${state.maxDisplayedFret}" min="0" max="24" style="width:48px;padding:3px 5px;" onchange="applyFretRangeMask()">
        <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="resetFretMask()">Odmaskuj wszystko</button>
      </div>`;
  } else if (state.subtab === 'chord') {
    const activeV = (state.chordVoicingsList && state.chordVoicingsList[state.selectedChordVoicingIdx]) || null;
    const vTitle = activeV ? `${activeV.symbol} (${activeV.info})` : state.chordId;
    bar.innerHTML = `
      <div style="display:flex;align-items:center;gap:10px;flex-wrap:wrap;">
        <strong style="color:var(--text);font-size:14px;">🎸 Wybrany chwyt na gryfie: <span style="color:var(--accent);">${vTitle}</span></strong>
        <button class="chip-btn ${state.chordShowScaleOnFretboard?'active':''}" style="padding:5px 12px;font-size:12px;"
                onclick="state.chordShowScaleOnFretboard=!state.chordShowScaleOnFretboard;renderFretboardToolbarForActiveTab();drawFretboard()">
          🎵 Pokaż skalę w tle (${state.fretboardData.scaleFullName})
        </button>
      </div>
      <div style="display:flex;gap:10px;align-items:center;font-size:12px;flex-wrap:wrap;">
        ${zoomCtrl}
        <button class="chip-btn ${state.labelType==='notes'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('notes')">Nuty</button>
        <button class="chip-btn ${state.labelType==='intervals'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('intervals')">Interwały</button>
        <span><b style="color:#ff9f43">●</b> Pryma akordu</span>
        <span><b style="color:#4ade80">●</b> Składnik chwytu</span>
        <span><b style="color:#38bdf8">●</b> Tapping (T)</span>
      </div>`;
  } else if (state.subtab === 'harmonics') {
    bar.innerHTML = `
      <div>
        <strong style="color:var(--text);font-size:14px;">Gryf Flażoletów Naturalnych (Progi ${state.harmonicMinFret}–${state.harmonicMaxFret})</strong>
        <span style="margin-left:10px;">Wszystkie flażolety naturalne na Twoich strunach. Dotknij rombu lub wiersza poniżej, aby usłyszeć.</span>
      </div>
      ${zoomCtrl}`;
  } else if (state.subtab === 'search') {
    bar.innerHTML = `
      <div>
        <strong style="color:var(--text);font-size:14px;">Gryf Ról Harmonicznych (Szukaj: Akord ↔ Skale)</strong>
        <span style="margin-left:10px;">Wybierz skalę w panelu po prawej, aby zobaczyć jej dźwięki pokolorowane wg roli.</span>
      </div>
      <div style="display:flex;gap:14px;align-items:center;font-size:12px;flex-wrap:wrap;">
        ${zoomCtrl}
        <span><b style="color:#4ade80">●</b> Składnik akordu</span>
        <span><b style="color:#eab308">●</b> Napięcie</span>
        <span><b style="color:#ef4444">●</b> Do unikania</span>
      </div>`;
  } else if (state.subtab === 'arpeggio') {
    const arpTitle = (state.arpViewMode === 'transition' && state.arpData && state.arpData.transition)
      ? `⚡ Przejście Sweep: ${state.arpData.transition.name}`
      : `Gryf Arpeggio: ${(state.arpData && state.arpData.chordSymbol) ? state.arpData.chordSymbol : state.chordId}`;
    const patNames = [
      'Mapa całego gryfu',
      'CAGED (5 kszt.)',
      '2NPS (2/strunę)',
      '3NPS (legato)',
      `Sweep Picking (${state.arpSweepStrings} str.)`,
      'Inwersje (zestaw strun)',
      `Od prymy (${state.arpOctaves} okt.)`
    ];
    const subDesc = (state.arpViewMode === 'transition' && state.arpData && state.arpData.transition && state.arpData.transition.segments && state.arpData.transition.segments[state.arpActiveSegIdx])
      ? `Aktywny akord: ${state.arpData.transition.segments[state.arpActiveSegIdx].symbol} (${state.arpData.transition.segments[state.arpActiveSegIdx].roman})`
      : `Wzorzec: ${patNames[state.arpPattern] || 'Arpeggio'}`;
    bar.innerHTML = `
      <div>
        <strong style="color:var(--text);font-size:14px;">${arpTitle}</strong>
        <span style="margin-left:10px;color:var(--text-muted);">${subDesc}</span>
      </div>
      <div style="display:flex;gap:10px;align-items:center;font-size:12px;flex-wrap:wrap;">
        ${zoomCtrl}
        <button class="chip-btn ${state.labelType==='notes'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('notes')">Nuty</button>
        <button class="chip-btn ${state.labelType==='intervals'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('intervals')">Interwały</button>
        <span><b style="color:#f39237">●</b> 1</span>
        <span><b style="color:#4ade80">●</b> 3</span>
        <span><b style="color:#38bdf8">●</b> 5</span>
        <span><b style="color:#facc15">●</b> 7/rozsz.</span>
      </div>`;
  } else if (state.subtab === 'exercise') {
    bar.innerHTML = `
      <div>
        <strong style="color:var(--text);font-size:14px;">Gryf Kompozytora Ćwiczeń</strong>
        <span style="margin-left:10px;">Dotknij podstrunnicy poniżej lub użyj klawiszy (Q W E R...), by dodać nuty na pięciolinię i do tabulatury.</span>
      </div>
      ${zoomCtrl}`;
  }
}

// Tryb „dokładnego dźwięku”: przy wykrywaniu z gitary (mikrofon) oraz w trybie rozpoznawania na gryfie (także Symuluj)
// podświetlamy tylko dźwięk o tej samej wysokości (jedna oktawa), a nie wszystkie oktawy danej nazwy nuty.
function isExactPitchHighlightMode() {
  return !!micStream || (state.mainView === 'practice' && state.subtab === 'scale' && !!state.scaleRecognitionOn);
}

function simulateDetectedNote(midi) {
  state.detectedMidi = midi;
  pluckGuitarFrequency(midiToFreq(midi), 1.1);
  // Znajdź niezamaskowaną pozycję najbliższą poprzedniej pozycji dłoni
  if (state.fretboardData) {
    const cands = state.fretboardData.cells.filter(c => c.midi === midi && !isCellMasked(c.s, c.f));
    if (cands.length > 0) {
      cands.sort((a, b) => {
        const da = Math.abs(a.f - state.lastHandPos.f) + Math.abs(a.s - state.lastHandPos.s) * 1.25;
        const db = Math.abs(b.f - state.lastHandPos.f) + Math.abs(b.s - state.lastHandPos.s) * 1.25;
        return da - db;
      });
      state.lastHandPos = { s: cands[0].s, f: cands[0].f };
    }
  }
  drawFretboard();
  updateGuitarToKeyboardSync();
}

function isCellMasked(s, f) {
  const minF = (state.maskLeft && state.maskLeft[s] !== undefined) ? state.maskLeft[s] : 0;
  const maxF = (state.maskRight && state.maskRight[s] !== undefined) ? state.maskRight[s] : state.maxDisplayedFret;
  return f < minF || f > maxF || state.maskedCells.has(`${s}:${f}`);
}

function applyFretRangeMask() {
  const minF = Math.max(0, Math.min(24, parseInt(document.getElementById('maskMinFret').value || '0', 10)));
  const maxF = Math.max(minF, Math.min(24, parseInt(document.getElementById('maskMaxFret').value || '22', 10)));
  state.maskLeft = [minF, minF, minF, minF, minF, minF];
  state.maskRight = [maxF, maxF, maxF, maxF, maxF, maxF];
  state.maskedCells.clear();
  drawFretboard();
}

function resetFretMask() {
  const maxF = state.maxDisplayedFret;
  const minIn = document.getElementById('maskMinFret');
  const maxIn = document.getElementById('maskMaxFret');
  if (minIn) minIn.value = 0;
  if (maxIn) maxIn.value = maxF;
  state.maskLeft = [0, 0, 0, 0, 0, 0];
  state.maskRight = [maxF, maxF, maxF, maxF, maxF, maxF];
  state.maskedCells.clear();
  drawFretboard();
}

function computeIntervalLabel(pc, rootPc, useTensions = false) {
  const semi = (((pc - rootPc) % 12) + 12) % 12;
  if (useTensions) {
    const tens = ['1', '♭9', '9', '♯9', '3', '11', '♯11', '5', '♭13', '13', '♭7', '7'];
    return tens[semi] || '1';
  }
  const simple = ['1', '♭2', '2', '♭3', '3', '4', '♭5', '5', '♯5', '6', '♭7', '7'];
  return simple[semi] || '1';
}

function setFretboardLabelType(lblType) {
  state.labelType = lblType;
  saveSettings();
  renderFretboardToolbarForActiveTab();
  drawFretboard();
  if (state.subtab === 'arpeggio') {
    refreshArpTabSvgOnly();
  } else if (state.subtab === 'scale') {
    renderStageAndInspector();
  }
}

// Wektorowy renderer gryfu SVG (ostrość żylety na każdym ekranie, duchy nut chromatycznych w tle,
// gradienty, cienie i proporcje 1:1 dokładnie w stylu referencyjnym!)
function drawFretboard() {
  updateGuitarToKeyboardSync();
  const card = document.getElementById('fretboardCardContainer');
  if (!card || card.style.display === 'none') return;
  const host = document.getElementById('fretboardSvgHost');
  if (!host || !state.fretboardData) return;

  const isHarmonicsTab = (state.mainView === 'practice' && state.subtab === 'harmonics');
  const isSearchTab = (state.mainView === 'practice' && state.subtab === 'search');
  const isChordTab = (state.mainView === 'practice' && state.subtab === 'chord');
  const isArpeggioTab = (state.mainView === 'practice' && state.subtab === 'arpeggio');
  const activeChordVoicing = isChordTab && state.chordVoicingsList && state.chordVoicingsList.length > 0
    ? (state.chordVoicingsList[state.selectedChordVoicingIdx] || state.chordVoicingsList[0])
    : null;
  const showDetectedOnFretboard = state.highlightPlayedOnGuitar && (state.subtab !== 'scale' || state.scaleRecognitionOn);

  // Skalowanie gryfu:
  // - Gdy włączone „↔ Pełna szer.”, na dużym ekranie (Full Screen) gryf rozszerza się na całą szerokość karty,
  //   natomiast przy zmniejszaniu okna NIGDY nie ściska się poniżej 1260px (działa przewijanie prawo-lewo!).
  // - Dodatkowo suwakiem „Skaluj gryf” (-5 .. +10) lub Ctrl+kółkiem myszy można dowolnie powiększać/pomniejszać gryf!
  const wrap = document.getElementById('fretboardWrap');
  const availW = (wrap && wrap.clientWidth > 100) ? (wrap.clientWidth - 6) : 1280;
  const zoomFactor = 1.0 + (state.fretboardZoom || 0) * 0.09;
  const minBaseW = isHarmonicsTab ? 1080 : 1260;
  const minBaseH = isHarmonicsTab ? 316 : 356;
  const baseW = state.fretboardFitWidth ? Math.max(minBaseW, availW) : minBaseW;
  const widthRatio = baseW / minBaseW;
  const baseH = Math.round(minBaseH * Math.min(1.38, Math.max(1.0, Math.pow(widthRatio, 0.65))));
  const W = Math.round(baseW * zoomFactor);
  const H = Math.round(baseH * zoomFactor);

  const nStrings = 6;
  const maxFret = isHarmonicsTab ? state.harmonicMaxFret : state.maxDisplayedFret;
  const leftNutX = 62;
  const rightPad = 22;
  const topPad = 28;
  const bottomPad = isHarmonicsTab ? 44 : 68;
  const boardW = W - leftNutX - rightPad;
  const boardH = H - topPad - bottomPad;
  const fretW = boardW / maxFret;
  const stringH = boardH / (nStrings - 1);

  // Zapisz geometrię gryfu dla płynnego przeciągania suwaków maskownicy (ponad gryfem, pod gryfem i na strunach)
  state.lastFretboardGeom = { W, H, leftNutX, boardW, boardH, topPad, bottomPad, fretW, stringH, maxFret };

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg" text-rendering="geometricPrecision" shape-rendering="geometricPrecision">`;

  // Definicje gradientów i filtrów cieni dla studyjnej jakości wizualnej
  svg += `
    <defs>
      <linearGradient id="rootGrad" x1="0" y1="0" x2="0" y2="1">
        <stop offset="0%" stop-color="#ff9f43"/>
        <stop offset="100%" stop-color="#e57c1f"/>
      </linearGradient>
      <linearGradient id="scaleNoteGrad" x1="0" y1="0" x2="0" y2="1">
        <stop offset="0%" stop-color="#543b26"/>
        <stop offset="100%" stop-color="#3d2a1b"/>
      </linearGradient>
      <linearGradient id="chordRoleGrad" x1="0" y1="0" x2="0" y2="1">
        <stop offset="0%" stop-color="#52e58b"/>
        <stop offset="100%" stop-color="#2bb665"/>
      </linearGradient>
      <linearGradient id="tensionRoleGrad" x1="0" y1="0" x2="0" y2="1">
        <stop offset="0%" stop-color="#facc15"/>
        <stop offset="100%" stop-color="#d49e08"/>
      </linearGradient>
    </defs>`;

  // Tło podstrunnicy
  svg += `<rect x="${leftNutX}" y="${topPad - 10}" width="${boardW}" height="${boardH + 20}" fill="#14100d" rx="4"/>`;

  // Subtelne podświetlenie progów markerowych (3, 5, 7, 9, 12, 15, 17, 19, 21, 24)
  for (let f = 1; f <= maxFret; f++) {
    if ([3,5,7,9,12,15,17,19,21,24].includes(f)) {
      const fx = leftNutX + (f - 1) * fretW;
      svg += `<rect x="${fx}" y="${topPad - 10}" width="${fretW}" height="${boardH + 20}" fill="rgba(255,255,255,0.012)"/>`;
    }
  }

  // Druty progowe (1..maxFret) i numery progów wyraźnie poniżej kropek na 6. strunie
  for (let f = 1; f <= maxFret; f++) {
    const x = leftNutX + f * fretW;
    svg += `<line x1="${x}" y1="${topPad - 10}" x2="${x}" y2="${topPad + boardH + 10}" stroke="#2b231d" stroke-width="1.5"/>`;

    const showNum = isHarmonicsTab || [3,5,7,9,12,15,17,19,21,24].includes(f);
    if (showNum) {
      const numColor = (f === 12 && isHarmonicsTab) ? '#f39237' : '#9c9288';
      const numWeight = (f === 12) ? '700' : '600';
      const numY = isHarmonicsTab ? (topPad + boardH + 30) : (topPad + boardH + 34);
      svg += `<text x="${leftNutX + (f - 0.5) * fretW}" y="${numY}" fill="${numColor}" font-family="Segoe UI, system-ui, sans-serif" font-size="12" font-weight="${numWeight}" text-anchor="middle">${f}</text>`;
    }
  }

  // Siodełko (próg 0) – wyraźna pionowa belka oddzielająca puste struny
  svg += `<line x1="${leftNutX}" y1="${topPad - 10}" x2="${leftNutX}" y2="${topPad + boardH + 10}" stroke="#8c8175" stroke-width="2.5"/>`;

  // Struny (od najcieńszej E4 u góry do najgrubszej E2 na dole)
  for (let s = 0; s < nStrings; s++) {
    const row = nStrings - 1 - s;
    const y = topPad + row * stringH;
    const sw = (1.1 + (nStrings - 1 - s) * 0.24).toFixed(2);
    svg += `<line x1="${leftNutX}" y1="${y}" x2="${leftNutX + boardW}" y2="${y}" stroke="#574b40" stroke-width="${sw}"/>`;
  }

  // TRYB 1: FLAŻOLETY NATURALNE (wektorowe romby ◇ w dokładnych węzłach struny – Zrzuty 1–4!)
  if (isHarmonicsTab) {
    const noteNames = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
    for (let s = 0; s < nStrings; s++) {
      const row = nStrings - 1 - s;
      const y = topPad + row * stringH;
      const openNm = noteNames[((state.customStringMidis[s] % 12) + 12) % 12];
      svg += `<text x="${leftNutX * 0.45}" y="${y + 4}" fill="#9ca3af" font-family="Segoe UI, sans-serif" font-size="11.5" font-weight="700" text-anchor="middle">${openNm}</text>`;
    }

    const d = Math.min(12.5, Math.max(8.5, Math.min(fretW * 0.16, stringH * 0.34)));

    // Przebieg 1: Narysuj nieaktywne poziomy trudności jako ciemnoszare kontury rombów w tle (jak na zrzutach 1–4!)
    for (const h of state.harmonicsData) {
      if (state.harmonicDiffEnabled[h.diff]) continue;
      if (!state.harmonicIntervalsEnabled.has(h.interval)) continue;
      if (h.exactFret < state.harmonicMinFret || h.exactFret > maxFret + 0.15) continue;

      const s = h.stringIdx;
      const row = nStrings - 1 - s;
      const cy = topPad + row * stringH;
      const cx = leftNutX + h.exactFret * fretW;
      const pts = `${cx},${cy - d} ${cx + d},${cy} ${cx},${cy + d} ${cx - d},${cy}`;
      svg += `<polygon points="${pts}" fill="none" stroke="#3b332c" stroke-width="1.2" opacity="0.7"
                       style="cursor:${state.lockUnmarkedNotes ? 'default' : 'pointer'}"
                       ${state.lockUnmarkedNotes ? '' : `onmousedown="startFretHoldHarmonic(event, ${h.freq})"`}/>`;
    }

    // Przebieg 2: Narysuj aktywne poziomy trudności wyraźnym kolorem na wierzchu!
    for (const h of state.harmonicsData) {
      if (!state.harmonicDiffEnabled[h.diff]) continue;
      if (!state.harmonicIntervalsEnabled.has(h.interval)) continue;
      if (h.exactFret < state.harmonicMinFret || h.exactFret > maxFret + 0.15) continue;

      const s = h.stringIdx;
      const row = nStrings - 1 - s;
      const cy = topPad + row * stringH;
      const cx = leftNutX + h.exactFret * fretW;
      const pts = `${cx},${cy - d} ${cx + d},${cy} ${cx},${cy + d} ${cx - d},${cy}`;
      svg += `<g class="fb-node" onmousedown="startFretHoldHarmonic(event, ${h.freq})">`;
      svg += `<polygon points="${pts}" fill="rgba(25, 45, 32, 0.38)" stroke="${h.color}" stroke-width="2.1"/>`;
      if (state.harmonicLabelType !== 'none') {
        const lbl = (state.harmonicLabelType === 'intervals') ? h.interval : h.noteOnly;
        svg += `<text x="${cx}" y="${cy + 3.5}" fill="#f5f2eb" font-family="Segoe UI, sans-serif" font-size="9.5" font-weight="700" text-anchor="middle">${lbl}</text>`;
      }
      svg += `</g>`;
    }
    svg += `</svg>`;
    host.innerHTML = svg;
    return;
  }

  // TRYB 2: SKALA / ROZPOZNAWANIE / SZUKAJ / ARPEGGIO / ĆWICZENIE
  const radius = Math.min(26.0, Math.max(11.5, Math.min(fretW * 0.35, stringH * 0.40)));
  const noteFontSize = Math.max(10.5, Math.min(17.5, +(radius * 0.74).toFixed(1)));
  state.markedFretCells = new Set();

  let searchChordSet = null, searchTensionSet = null, searchAvoidSet = null;
  let arpNodeMap = null, arpNextSegMap = null, arpTechMap = null, arpActiveStepObj = null;
  let arpActiveRootPc = state.rootPc, arpNextRootPc = state.rootPc;
  if (isArpeggioTab && state.arpData) {
    arpNodeMap = new Map();
    arpNextSegMap = new Map();
    arpTechMap = new Map();

    let activeNodes = state.arpData.nodes || [];
    let activeSteps = state.arpData.steps || [];
    if (state.arpData.rootPc !== undefined) arpActiveRootPc = state.arpData.rootPc;
    if (state.arpViewMode === 'transition' && state.arpData.transition && state.arpData.transition.segments && state.arpData.transition.segments.length > 0) {
      const segs = state.arpData.transition.segments;
      const segIdx = Math.max(0, Math.min(state.arpActiveSegIdx || 0, segs.length - 1));
      const curSeg = segs[segIdx];
      activeNodes = curSeg.nodes || [];
      activeSteps = curSeg.steps || [];
      if (curSeg.rootPc !== undefined) arpActiveRootPc = curSeg.rootPc;
      const nextSeg = segs[(segIdx + 1) % segs.length];
      if (nextSeg && nextSeg !== curSeg) {
        arpNextRootPc = (nextSeg.rootPc !== undefined) ? nextSeg.rootPc : arpActiveRootPc;
        for (const nn of (nextSeg.nodes || [])) arpNextSegMap.set(`${nn.s}:${nn.f}`, nn);
      }
      const allSteps = getActiveArpeggioSteps();
      if (state.arpPlayStepIdx >= 0 && allSteps[state.arpPlayStepIdx]) {
        arpActiveStepObj = allSteps[state.arpPlayStepIdx];
      }
    } else {
      // Jeśli wybrano Cały gryf, w trakcie odtwarzania/ćwiczenia pokazuj WYŁĄCZNIE dźwięki granej sekwencji!
      if (state.arpPattern === 0 && (state.arpPlaying || state.arpPlayStepIdx >= 0) && activeSteps.length > 0) {
        const playedKeys = new Set(activeSteps.map(st => `${st.s}:${st.f}`));
        activeNodes = activeNodes.filter(n => playedKeys.has(`${n.s}:${n.f}`));
      }
      if (state.arpPlayStepIdx >= 0 && activeSteps[state.arpPlayStepIdx]) {
        arpActiveStepObj = activeSteps[state.arpPlayStepIdx];
      }
    }

    for (const n of activeNodes) arpNodeMap.set(`${n.s}:${n.f}`, n);
    for (const st of activeSteps) {
      const key = `${st.s}:${st.f}`;
      if (!arpTechMap.has(key)) arpTechMap.set(key, st);
    }

    // Subtelna trajektoria kostkowania w trybie Sweep, Od prymy oraz Inwersje (łączy kolejne nuty w kształcie)
    if ((state.arpPattern === 4 || state.arpPattern === 5 || state.arpPattern === 6 || state.arpViewMode === 'transition') && activeNodes.length >= 2) {
      const ordered = activeNodes.slice().sort((a, b) => (a.midi - b.midi) || (a.s - b.s));
      const pts = ordered.map(n => {
        const px = (n.f === 0) ? (leftNutX * 0.48) : (leftNutX + (n.f - 0.5) * fretW);
        const py = topPad + (nStrings - 1 - n.s) * stringH;
        return `${px.toFixed(1)},${py.toFixed(1)}`;
      }).join(' ');
      svg += `<polyline points="${pts}" fill="none" stroke="rgba(243,146,55,0.42)" stroke-width="2.8" stroke-linecap="round" stroke-linejoin="round" stroke-dasharray="6,4" pointer-events="none"/>`;
    }
  }
  if (isSearchTab && state.searchData) {
    if (state.searchDir === 'chord_to_scale') {
      if (state.selectedSearchScaleIdx >= 0 && state.searchData.matches[state.selectedSearchScaleIdx]) {
        const m = state.searchData.matches[state.selectedSearchScaleIdx];
        searchChordSet = new Set(m.chordPcs);
        searchTensionSet = new Set(m.tensionPcs);
        searchAvoidSet = new Set(m.avoidPcs);
      } else {
        searchChordSet = new Set(state.searchData.chordPcs || []);
        searchTensionSet = new Set();
        searchAvoidSet = new Set();
      }
    } else if (state.searchDir === 'scale_to_chord') {
      if (state.selectedSearchChordIdx >= 0 && state.searchData.chords && state.searchData.chords[state.selectedSearchChordIdx]) {
        searchChordSet = new Set(state.searchData.chords[state.selectedSearchChordIdx].chordPcs);
        searchTensionSet = new Set();
        searchAvoidSet = new Set();
      }
    }
  }

  const activeMidisSet = (micStream && state.detectedMidis && state.detectedMidis.length > 0)
    ? new Set(state.detectedMidis.map(m => Math.round(m)))
    : ((state.detectedMidi !== null && state.detectedMidi !== undefined) ? new Set([Math.round(state.detectedMidi)]) : null);

  for (const cell of state.fretboardData.cells) {
    const s = cell.s, f = cell.f;
    if (f > maxFret) continue;
    const row = nStrings - 1 - s;
    const cy = topPad + row * stringH;
    const cx = (f === 0) ? (leftNutX * 0.48) : (leftNutX + (f - 0.5) * fretW);
    const cellW = (f === 0) ? (leftNutX * 0.85) : fretW;

    const masked = isCellMasked(s, f);
    const nodeOpacity = masked ? '0.20' : '1.0';

    // Obszar kliknięcia / przeciągania
    svg += `<rect class="fb-hit-rect" x="${cx - cellW/2}" y="${cy - stringH/2}" width="${cellW}" height="${stringH}"
                  onmousedown="onFretCellMouseDown(event, ${s}, ${f})"
                  onmouseenter="onFretCellMouseEnter(event, ${s}, ${f})"/>`;

    const pc = ((cell.midi % 12) + 12) % 12;
    const degHighlightAllowed = (state.subtab !== 'scale') || isScaleDegreeHighlightEnabled(pc);
    const activePlayPc = (state.detectedMidi !== null && state.detectedMidi !== undefined)
      ? (((Math.round(state.detectedMidi) % 12) + 12) % 12)
      : ((state.mainView === 'practice' && state.subtab === 'scale') ? state.selectedScaleNotePc : null);
    const isDetected = (showDetectedOnFretboard && degHighlightAllowed && !masked && activeMidisSet && activeMidisSet.has(cell.midi));
    // Wykrywanie dźwięku z gitary (mikrofon): podświetlamy tylko dokładnie ten sam dźwięk (ta sama wysokość / oktawa),
    // a nie wszystkie oktawy danej nazwy nuty. Przy kliknięciach myszą nadal podświetlamy wszystkie oktawy.
    const micExactOnly = isExactPitchHighlightMode();
    const isSameNoteDegree = (showDetectedOnFretboard && degHighlightAllowed && !masked && (micExactOnly
      ? (activeMidisSet && activeMidisSet.has(cell.midi))
      : (activePlayPc !== null && activePlayPc !== undefined && pc === activePlayPc)));
    const isPrimaryDetected = isDetected && state.lastHandPos && state.lastHandPos.s === s && state.lastHandPos.f === f;

    let fillAttr = 'url(#scaleNoteGrad)';
    let strokeAttr = '#6e4e32';
    let textColor = '#f8f5f0';
    let shouldDrawCircle = false;
    let customNodeOpacity = nodeOpacity;
    let isTappedVoicingNode = false;

    if (isChordTab) {
      const vFret = (activeChordVoicing && activeChordVoicing.frets) ? activeChordVoicing.frets[s] : -1;
      const isVoicingNote = (vFret === f);
      const isVoicingTapped = isVoicingNote && activeChordVoicing.tapped && activeChordVoicing.tapped[s];
      const vRootPc = (activeChordVoicing && activeChordVoicing.rootPc !== undefined) ? activeChordVoicing.rootPc : state.rootPc;

      if (f === 0 && vFret === -1) {
        // Wyciszona struna (✕) w wybranym chwycie akordu
        svg += `<text x="${cx * 0.55}" y="${cy + 4.5}" fill="#ef4444" font-family="Segoe UI, sans-serif" font-size="13" font-weight="800" text-anchor="middle">✕</text>`;
      }

      if (isVoicingNote) {
        shouldDrawCircle = true;
        if (isVoicingTapped) {
          isTappedVoicingNode = true;
          fillAttr = '#0284c7';
          strokeAttr = '#7dd3fc';
          textColor = '#ffffff';
        } else if (pc === vRootPc) {
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#fde68a';
          textColor = '#160f08';
        } else {
          fillAttr = 'url(#chordRoleGrad)';
          strokeAttr = '#86efac';
          textColor = '#08170e';
        }
      } else if (isDetected) {
        shouldDrawCircle = true;
        fillAttr = 'rgba(74, 222, 128, 0.35)';
        strokeAttr = '#4ade80';
        textColor = '#f8f5f0';
      } else if (state.chordShowScaleOnFretboard && cell.inPosition) {
        shouldDrawCircle = true;
        customNodeOpacity = '0.42';
        if (cell.isRoot) {
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#ffb86c';
          textColor = '#160f08';
        }
      }
    } else if (isArpeggioTab && arpNodeMap) {
      const key = `${s}:${f}`;
      const aNode = arpNodeMap.get(key);
      const nextNode = arpNextSegMap ? arpNextSegMap.get(key) : null;
      const isCurStep = !!(arpActiveStepObj && arpActiveStepObj.s === s && arpActiveStepObj.f === f);

      if (aNode) {
        shouldDrawCircle = true;
        const ivStr = aNode.interval || computeIntervalLabel(pc, arpActiveRootPc, false);
        if (isCurStep) {
          fillAttr = '#22c55e';
          strokeAttr = '#ffffff';
          textColor = '#05140b';
        } else if (aNode.isRoot || ivStr === '1') {
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#fde68a';
          textColor = '#160f08';
        } else if (ivStr === '3' || ivStr === '♭3' || ivStr === 'b3') {
          fillAttr = 'url(#chordRoleGrad)';
          strokeAttr = '#86efac';
          textColor = '#08170e';
        } else if (ivStr === '5' || ivStr === '♭5' || ivStr === '♯5' || ivStr === 'b5' || ivStr === '#5') {
          fillAttr = '#0284c7';
          strokeAttr = '#7dd3fc';
          textColor = '#ffffff';
        } else {
          fillAttr = 'url(#tensionRoleGrad)';
          strokeAttr = '#fef08a';
          textColor = '#171206';
        }
      } else if (nextNode) {
        // Podgląd pozycji kolejnego akordu w przejściu sweep (przerywany kontur) — continue zapobiega zakryciu tekstu tłem!
        const nextLbl = (state.labelType === 'intervals')
          ? (nextNode.interval || computeIntervalLabel(pc, arpNextRootPc, false))
          : nextNode.note;
        svg += `<g opacity="0.72" pointer-events="none">
          <circle cx="${cx}" cy="${cy}" r="${(radius * 0.84).toFixed(1)}" fill="rgba(20,16,13,0.88)" stroke="#38bdf8" stroke-width="1.5" stroke-dasharray="3,2"/>
          <text x="${cx}" y="${cy + 3.8}" fill="#7dd3fc" font-family="Segoe UI, sans-serif" font-size="10.5" font-weight="700" text-anchor="middle">${nextLbl}</text>
        </g>`;
        continue;
      } else if (micStream && !state.arpPlaying && state.arpPlayStepIdx < 0 && isDetected) {
        // Podczas odtwarzania ćwiczenia arpeggio NIE podświetlamy innych strun/oktaw spoza ćwiczenia;
        // podświetlenie wykrywania działa tylko dla żywego mikrofonu poza syntetycznym odtwarzaniem.
        shouldDrawCircle = true;
        fillAttr = 'rgba(74, 222, 128, 0.35)';
        strokeAttr = '#4ade80';
        textColor = '#f8f5f0';
      }
    } else if (isSearchTab && searchChordSet) {
      const inChord = searchChordSet.has(pc);
      const inTension = searchTensionSet && searchTensionSet.has(pc);
      const inAvoid = searchAvoidSet && searchAvoidSet.has(pc);

      if (state.selectedSearchScaleIdx >= 0) {
        if (inChord) {
          shouldDrawCircle = true;
          fillAttr = 'url(#chordRoleGrad)';
          strokeAttr = '#4ade80';
          textColor = '#08170e';
        } else if (inTension) {
          shouldDrawCircle = true;
          fillAttr = 'url(#tensionRoleGrad)';
          strokeAttr = '#eab308';
          textColor = '#171206';
        } else if (inAvoid) {
          shouldDrawCircle = true;
          fillAttr = '#ef4444';
          strokeAttr = '#fca5a5';
          textColor = '#ffffff';
        }
      } else if (inChord) {
        shouldDrawCircle = true;
        if (pc === state.rootPc) {
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#ffb86c';
          textColor = '#160f08';
        }
      }
    } else {
      shouldDrawCircle = (cell.inPosition && degHighlightAllowed) || isDetected || (isSameNoteDegree && (cell.inPosition || state.detectedMidi !== null));
      if (isPrimaryDetected || (isDetected && cell.inPosition && degHighlightAllowed)) {
        fillAttr = 'url(#chordRoleGrad)';
        strokeAttr = '#bbf7d0';
        textColor = '#05140b';
      } else if (isDetected) {
        fillAttr = '#22c55e';
        strokeAttr = '#bbf7d0';
        textColor = '#05140b';
      } else if (isSameNoteDegree && (cell.inPosition || state.detectedMidi !== null)) {
        fillAttr = 'url(#chordRoleGrad)';
        strokeAttr = '#86efac';
        textColor = '#08170e';
      } else if (cell.isRoot) {
        fillAttr = 'url(#rootGrad)';
        strokeAttr = '#ffb86c';
        textColor = '#160f08';
      }
    }

    if (!shouldDrawCircle) {
      if (f === 0) {
        // Pusta struna poza aktywną skalą: elegancki ciemny okrąg z obwódką po lewej stronie siodełka
        svg += `<g class="fb-node" opacity="${nodeOpacity}" style="cursor:${state.lockUnmarkedNotes ? 'default' : 'pointer'}"
                   ${state.lockUnmarkedNotes ? '' : `onmousedown="onFretCellMouseDown(event, ${s}, ${f})"`}>`;
        svg += `<circle cx="${cx}" cy="${cy}" r="${radius * 0.92}" fill="#14100d" stroke="#52463b" stroke-width="1.4"/>`;
        svg += `<text x="${cx}" y="${cy + 4}" fill="#d6cec2" font-family="Segoe UI, system-ui, sans-serif" font-size="11.5" font-weight="700" text-anchor="middle">${cell.note}</text>`;
        svg += `</g>`;
      } else {
        // Subtelny „duch” nazwy dźwięku chromatycznego na strunie w tle
        svg += `<g opacity="${masked ? '0.18' : '1.0'}" pointer-events="none">`;
        svg += `<rect x="${cx - 10}" y="${cy - 6}" width="20" height="12" fill="#14100d"/>`;
        svg += `<text x="${cx}" y="${cy + 3.5}" fill="#3d342c" font-family="Segoe UI, system-ui, sans-serif" font-size="9.5" font-weight="600" text-anchor="middle">${cell.note}</text>`;
        svg += `</g>`;
      }
      continue;
    }

    if (!masked) {
      state.markedFretCells.add(`${s}:${f}`);
    }

    let label = cell.note;
    const activeLblType = isSearchTab ? state.searchLabelType : state.labelType;
    if (isArpeggioTab && arpNodeMap) {
      const aNode = arpNodeMap.get(`${s}:${f}`);
      if (aNode) {
        const ivLbl = (aNode.interval && (aNode.isRoot || aNode.interval !== '1'))
          ? aNode.interval
          : computeIntervalLabel(pc, arpActiveRootPc, false);
        label = (activeLblType === 'intervals') ? ivLbl : aNode.note;
      }
    } else if (isChordTab) {
      const vFret = (activeChordVoicing && activeChordVoicing.frets) ? activeChordVoicing.frets[s] : -1;
      const vRootPc = (activeChordVoicing && activeChordVoicing.rootPc !== undefined) ? activeChordVoicing.rootPc : state.rootPc;
      if (activeLblType === 'intervals') {
        label = (vFret === f)
          ? computeIntervalLabel(pc, vRootPc, false)
          : (cell.interval || computeIntervalLabel(pc, state.chordFilterByScale ? state.chordScaleRootPc : state.rootPc, false));
      }
    } else if (isSearchTab) {
      if (activeLblType === 'intervals') {
        label = computeIntervalLabel(pc, state.rootPc, false);
      } else if (activeLblType === 'tensions') {
        const isTens = !!(searchTensionSet && searchTensionSet.has(pc));
        label = computeIntervalLabel(pc, state.rootPc, isTens);
      }
    } else if (activeLblType === 'intervals') {
      label = cell.interval || computeIntervalLabel(pc, state.rootPc, false);
    }
    if (isTappedVoicingNode) {
      label = 'T:' + label;
    }

    const arpIsCurPlay = isArpeggioTab && arpActiveStepObj && arpActiveStepObj.s === s && arpActiveStepObj.f === f;
    svg += `<g class="fb-node" opacity="${customNodeOpacity}" onmousedown="onFretCellMouseDown(event, ${s}, ${f})" onmouseenter="onFretCellMouseEnter(event, ${s}, ${f})">`;
    if (arpIsCurPlay) {
      svg += `<circle cx="${cx}" cy="${cy}" r="${(radius + 4.5).toFixed(1)}" fill="none" stroke="#4ade80" stroke-width="2.5" opacity="0.9"/>`;
    }
    svg += `<circle cx="${cx}" cy="${cy}" r="${radius}" fill="${fillAttr}" stroke="${strokeAttr}" stroke-width="${arpIsCurPlay ? '2.5' : '1.5'}"/>`;
    svg += `<text x="${cx}" y="${cy + noteFontSize * 0.34}" fill="${textColor}" font-family="Segoe UI, system-ui, sans-serif" font-size="${isTappedVoicingNode ? Math.max(9.5, noteFontSize - 2) : noteFontSize}" font-weight="700" text-anchor="middle">${label}</text>`;
    if (isArpeggioTab && arpTechMap && (state.arpPattern === 4 || state.arpViewMode === 'transition')) {
      const stInfo = arpTechMap.get(`${s}:${f}`);
      if (stInfo && arpNodeMap && arpNodeMap.has(`${s}:${f}`)) {
        const symMap = { D: '↓', U: '↑', H: 'H', P: 'P', S: 'S' };
        const badge = (symMap[stInfo.tech] || stInfo.tech) + (stInfo.roll ? '·R' : '');
        svg += `<rect x="${cx - 11}" y="${cy - radius - 12}" width="22" height="11" rx="3" fill="#17120e" stroke="${stInfo.roll ? '#facc15' : '#574230'}" stroke-width="0.9"/>`;
        svg += `<text x="${cx}" y="${cy - radius - 3.5}" fill="${stInfo.roll ? '#fde047' : '#fb923c'}" font-family="Segoe UI, sans-serif" font-size="8.5" font-weight="800" text-anchor="middle">${badge}</text>`;
      }
    }
    svg += `</g>`;
  }

  // =========================================================================
  // MASKOWNICA ZE SCHODKAMI (6 strun w obie strony + uchwyt ponad gryfem!)
  // =========================================================================
  if (state.subtab === 'scale' || state.subtab === 'exercise') {
    const leftEdgeX = (fretVal) => (fretVal <= 0) ? 6 : (leftNutX + (fretVal - 1) * fretW);
    const rightEdgeX = (fretVal) => (fretVal >= maxFret) ? (leftNutX + boardW) : (leftNutX + fretVal * fretW);

    const bandTop = (row) => (row === 0) ? (topPad - 12) : (topPad + (row - 0.5) * stringH);
    const bandBot = (row) => (row === 5) ? (topPad + boardH + 12) : (topPad + (row + 0.5) * stringH);

    // 1. Lewa kurtyna przyciemniająca + schodkowa linia podziału (6 segmentów strun)
    let leftPoly = `6,${bandTop(0)} `;
    let leftLinePts = '';
    for (let row = 0; row < 6; row++) {
      const s = 5 - row;
      const lx = leftEdgeX(state.maskLeft[s]);
      const y1 = bandTop(row);
      const y2 = bandBot(row);
      leftPoly += `${lx},${y1} ${lx},${y2} `;
      leftLinePts += `${lx},${y1} ${lx},${y2} `;
    }
    leftPoly += `6,${bandBot(5)}`;
    svg += `<polygon points="${leftPoly}" fill="rgba(7, 5, 4, 0.58)" pointer-events="none"/>`;
    svg += `<polyline points="${leftLinePts}" fill="none" stroke="#f39237" stroke-width="2.6" stroke-dasharray="none" pointer-events="none"/>`;

    // 2. Prawa kurtyna przyciemniająca + schodkowa linia podziału (6 segmentów strun)
    const farRightX = leftNutX + boardW + 10;
    let rightPoly = `${farRightX},${bandTop(0)} `;
    let rightLinePts = '';
    for (let row = 0; row < 6; row++) {
      const s = 5 - row;
      const rx = rightEdgeX(state.maskRight[s]);
      const y1 = bandTop(row);
      const y2 = bandBot(row);
      rightPoly += `${rx},${y1} ${rx},${y2} `;
      rightLinePts += `${rx},${y1} ${rx},${y2} `;
    }
    rightPoly += `${farRightX},${bandBot(5)}`;
    svg += `<polygon points="${rightPoly}" fill="rgba(7, 5, 4, 0.58)" pointer-events="none"/>`;
    svg += `<polyline points="${rightLinePts}" fill="none" stroke="#f39237" stroke-width="2.6" pointer-events="none"/>`;

    // 3. SZYNA SUWAKA POD GRYFEM (z podpisem „Maskowanie”) do płynnego przesuwania linii!
    const botLeftX = leftEdgeX(state.maskLeft[0]);
    const botRightX = rightEdgeX(state.maskRight[0]);
    const botLeftThumbX = Math.max(42, Math.min(W - 42, botLeftX));
    const botRightThumbX = Math.max(42, Math.min(W - 42, botRightX));
    const railLeft = 6;
    const railRight = leftNutX + boardW;
    const botRailY = topPad + boardH + 54;

    // Dolna szyna suwaka (poniżej gryfu – przesuwa linię maskowania w lewo/prawo!)
    svg += `
      <rect x="${railLeft}" y="${botRailY - 4}" width="${railRight - railLeft}" height="8" rx="4"
            fill="#1c1612" stroke="#3b3028" stroke-width="1" style="cursor:pointer"
            onmousedown="onMaskRailTrackMouseDown(event, 'shift')"/>
      <rect x="${Math.min(botLeftX, botRightX)}" y="${botRailY - 3}" width="${Math.max(2, Math.abs(botRightX - botLeftX))}" height="6" rx="3"
            fill="rgba(243, 146, 55, 0.35)" pointer-events="none"/>
      <line x1="${botLeftX}" y1="${bandBot(5)}" x2="${botLeftX}" y2="${botRailY}" stroke="#f39237" stroke-width="2" stroke-dasharray="2,2" pointer-events="none"/>
      <line x1="${botRightX}" y1="${bandBot(5)}" x2="${botRightX}" y2="${botRailY}" stroke="#f39237" stroke-width="2" stroke-dasharray="2,2" pointer-events="none"/>
      <g style="cursor:ew-resize" onmousedown="startMaskCurtainDrag(event, 'left', 'shift', -1)">
        <rect x="${botLeftThumbX - 40}" y="${botRailY - 9}" width="80" height="18" rx="5" fill="#f39237" stroke="#14100d" stroke-width="1.5"/>
        <text x="${botLeftThumbX}" y="${botRailY + 3.5}" fill="#14100d" font-size="9.5" font-weight="800" text-anchor="middle">◄ Maskowanie ►</text>
      </g>
      <g style="cursor:ew-resize" onmousedown="startMaskCurtainDrag(event, 'right', 'shift', -1)">
        <rect x="${botRightThumbX - 40}" y="${botRailY - 9}" width="80" height="18" rx="5" fill="#f39237" stroke="#14100d" stroke-width="1.5"/>
        <text x="${botRightThumbX}" y="${botRailY + 3.5}" fill="#14100d" font-size="9.5" font-weight="800" text-anchor="middle">◄ Maskowanie ►</text>
      </g>`;

    // 4. Uchwyty schodkowe NA KAŻDEJ STRUNIE (przeciąganie między strunami tworzy schodki w obie strony!)
    for (let row = 0; row < 6; row++) {
      const s = 5 - row;
      const cy = topPad + row * stringH;
      const lx = leftEdgeX(state.maskLeft[s]);
      const rx = rightEdgeX(state.maskRight[s]);
      svg += `
        <g style="cursor:ew-resize" onmousedown="startMaskCurtainDrag(event, 'left', 'string', ${s})">
          <rect x="${lx - 9}" y="${cy - stringH*0.48}" width="18" height="${stringH*0.96}" fill="transparent"/>
          <circle cx="${lx}" cy="${cy}" r="5" fill="#f39237" stroke="#14100d" stroke-width="1.4"/>
        </g>
        <g style="cursor:ew-resize" onmousedown="startMaskCurtainDrag(event, 'right', 'string', ${s})">
          <rect x="${rx - 9}" y="${cy - stringH*0.48}" width="18" height="${stringH*0.96}" fill="transparent"/>
          <circle cx="${rx}" cy="${cy}" r="5" fill="#f39237" stroke="#14100d" stroke-width="1.4"/>
        </g>`;
    }
  }

  svg += `</svg>`;
  host.innerHTML = svg;
}

function computeMaskFretFromEvent(e) {
  const svgEl = document.querySelector('#fretboardSvgHost svg');
  const g = state.lastFretboardGeom;
  if (!svgEl || !g) return 0;
  const rect = svgEl.getBoundingClientRect();
  const svgX = (e.clientX - rect.left) * (g.W / Math.max(1, rect.width));
  if (svgX < g.leftNutX * 0.55) return 0;
  const f = Math.round((svgX - g.leftNutX) / g.fretW + 0.5);
  return Math.max(0, Math.min(g.maxFret, f));
}

function startMaskCurtainDrag(e, side, mode, sIdx) {
  e.preventDefault();
  e.stopPropagation();
  const startFret = computeMaskFretFromEvent(e);
  state.activeMaskDrag = {
    side,
    mode,
    sIdx,
    startFret,
    initialLeft: state.maskLeft.slice(),
    initialRight: state.maskRight.slice()
  };
}

function onMaskRailTrackMouseDown(e, mode) {
  e.preventDefault();
  e.stopPropagation();
  const clickedFret = computeMaskFretFromEvent(e);
  const avgLeft = state.maskLeft.reduce((a, b) => a + b, 0) / 6;
  const avgRight = state.maskRight.reduce((a, b) => a + b, 0) / 6;
  const side = (Math.abs(clickedFret - avgLeft) <= Math.abs(clickedFret - avgRight)) ? 'left' : 'right';
  state.activeMaskDrag = {
    side,
    mode,
    sIdx: -1,
    startFret: clickedFret,
    initialLeft: state.maskLeft.slice(),
    initialRight: state.maskRight.slice()
  };
  applyMaskDragAtCell(0, clickedFret);
}

function onGlobalMaskDragMove(e) {
  if (!state.activeMaskDrag) return;
  const f = computeMaskFretFromEvent(e);
  // Jeśli ciągniemy za kółko na strunie, sprawdźmy również na której wysokości struny jest kursor
  let s = state.activeMaskDrag.sIdx;
  if (state.activeMaskDrag.mode === 'string') {
    const svgEl = document.querySelector('#fretboardSvgHost svg');
    const g = state.lastFretboardGeom;
    if (svgEl && g) {
      const rect = svgEl.getBoundingClientRect();
      const svgY = (e.clientY - rect.top) * (g.H / Math.max(1, rect.height));
      const row = Math.max(0, Math.min(5, Math.round((svgY - g.topPad) / g.stringH)));
      s = 5 - row;
    }
  }
  applyMaskDragAtCell(s >= 0 ? s : 0, f);
}

function startFretHoldHarmonic(e, freqHz) {
  if (e) {
    e.preventDefault();
    e.stopPropagation();
  }
  state.isHoldingFretNote = true;
  if (freqHz > 20) {
    state.detectedMidi = Math.round(69 + 12 * Math.log2(freqHz / (state.a4ReferenceHz || 440.0)));
  }
  startGuitarSustainedVoice('fret_mouse', freqHz, 18.0);
  updateGuitarToKeyboardSync();
}

function findAscendingScaleCell(degPcOrNote, isOctave = false) {
  if (!state.fretboardData || !state.fretboardData.cells) return null;
  const targetPc = (typeof degPcOrNote === 'number')
    ? (((Math.round(degPcOrNote) % 12) + 12) % 12)
    : noteStringToPc(String(degPcOrNote), state.rootPc || 0);
  const baseRootMidi = 48 + (state.rootPc || 0); // Oktawa 3 od prymy aktywnej skali
  const semiFromRoot = isOctave ? 12 : (((targetPc - (state.rootPc || 0)) % 12 + 12) % 12);
  const exactTargetMidi = baseRootMidi + semiFromRoot;

  const matchingNotes = state.fretboardData.cells.filter(c => (((c.midi % 12) + 12) % 12) === targetPc);
  const exactOctaveCells = matchingNotes.filter(c => c.midi === exactTargetMidi);
  const pool = exactOctaveCells.length > 0 ? exactOctaveCells : matchingNotes;

  return pool.find(c => c.inPosition && !isCellMasked(c.s, c.f)) ||
         pool.find(c => !isCellMasked(c.s, c.f)) ||
         pool[0] || null;
}

function startHoldScaleNoteChip(e, degPcOrNote, isOctave = false) {
  if (e) {
    e.preventDefault();
    e.stopPropagation();
  }
  const cand = findAscendingScaleCell(degPcOrNote, isOctave);
  if (cand) {
    state.isHoldingFretNote = true;
    state.lastHandPos = { s: cand.s, f: cand.f };
    state.detectedMidi = cand.midi;
    state.selectedScaleNotePc = ((cand.midi % 12) + 12) % 12;
    startGuitarSustainedVoice('fret_mouse', cand.freq, 18.0);
    drawFretboard();
  }
}

function onScaleNoteChipMouseEnter(e, degPcOrNote, isOctave = false) {
  if (!state.isHoldingFretNote) return;
  startHoldScaleNoteChip(e, degPcOrNote, isOctave);
}

function onFretCellMouseDown(e, s, f) {
  if (!state.fretboardData) return;
  if (e.button === 2 || e.shiftKey) {
    e.preventDefault();
    // Prawy przycisk myszy ustawia schodek na danej strunie (z bliższej strony: lewej lub prawej)
    const distLeft = Math.abs(f - state.maskLeft[s]);
    const distRight = Math.abs(f - state.maskRight[s]);
    const side = (distLeft <= distRight) ? 'left' : 'right';
    state.activeMaskDrag = { side, mode: 'string', sIdx: s };
    applyMaskDragAtCell(s, f);
    return;
  }
  if (e.button !== 0 || e.altKey) return;
  if (state.lockUnmarkedNotes && (!state.markedFretCells || !state.markedFretCells.has(`${s}:${f}`))) {
    return;
  }

  const found = state.fretboardData.cells.find(c => c.s === s && c.f === f);
  if (found) {
    state.isHoldingFretNote = true;
    state.lastHandPos = { s, f };
    state.detectedMidi = found.midi;
    state.selectedScaleNotePc = ((found.midi % 12) + 12) % 12;
    startGuitarSustainedVoice('fret_mouse', found.freq, 18.0);
    drawFretboard();
    if (state.subtab === 'exercise') {
      state.recordedEvents.push({
        s,
        f,
        midi: found.midi,
        note: found.note,
        freq: found.freq,
        dur: state.defaultDuration,
        tuplet: state.tupletActive ? 3 : 0,
        rest: 0
      });
      renderStageAndInspector();
    }
  }
}

function applyMaskDragAtCell(s, f) {
  if (!state.activeMaskDrag) return;
  const { side, mode, startFret, initialLeft, initialRight } = state.activeMaskDrag;
  const maxF = state.maxDisplayedFret;
  if (mode === 'all') {
    // Górny suwak PONAD gryfem -> ustawia i przesuwa wszystkie 6 strun RÓWNO!
    for (let i = 0; i < 6; i++) {
      if (side === 'left') {
        state.maskLeft[i] = Math.max(0, Math.min(state.maskRight[i], f));
      } else {
        state.maskRight[i] = Math.max(state.maskLeft[i], Math.min(maxF, f));
      }
    }
  } else if (mode === 'shift' && initialLeft && initialRight) {
    // Dolny suwak POD gryfem -> przesuwa CAŁĄ linię razem z jej schodkami w lewo / prawo!
    const delta = f - (startFret !== undefined ? startFret : f);
    for (let i = 0; i < 6; i++) {
      if (side === 'left') {
        state.maskLeft[i] = Math.max(0, Math.min(state.maskRight[i], initialLeft[i] + delta));
      } else {
        state.maskRight[i] = Math.max(state.maskLeft[i], Math.min(maxF, initialRight[i] + delta));
      }
    }
  } else {
    // Przeciąganie NA STRUNIE / między strunami -> robi indywidualny schodek na strunie s!
    if (side === 'left') {
      state.maskLeft[s] = Math.max(0, Math.min(state.maskRight[s], f));
    } else {
      state.maskRight[s] = Math.max(state.maskLeft[s], Math.min(maxF, f));
    }
  }
  const minIn = document.getElementById('maskMinFret');
  const maxIn = document.getElementById('maskMaxFret');
  if (minIn) minIn.value = Math.min(...state.maskLeft);
  if (maxIn) maxIn.value = Math.max(...state.maskRight);
  drawFretboard();
}

function onFretCellMouseEnter(e, s, f) {
  if (state.activeMaskDrag) {
    applyMaskDragAtCell(s, f);
    return;
  }
  // Jeśli przytrzymujemy lewy przycisk myszy na gryfie, przesunięcie na inny próg daje płynne legato / slide!
  if (state.isHoldingFretNote && state.fretboardData) {
    if (state.lockUnmarkedNotes && (!state.markedFretCells || !state.markedFretCells.has(`${s}:${f}`))) {
      return;
    }
    if (state.lastHandPos && state.lastHandPos.s === s && state.lastHandPos.f === f) return;
    const found = state.fretboardData.cells.find(c => c.s === s && c.f === f);
    if (found) {
      state.lastHandPos = { s, f };
      state.detectedMidi = found.midi;
      state.selectedScaleNotePc = ((found.midi % 12) + 12) % 12;
      startGuitarSustainedVoice('fret_mouse', found.freq, 18.0);
      drawFretboard();
    }
  }
}

document.addEventListener('DOMContentLoaded', () => {
  window.addEventListener('mousemove', onGlobalMaskDragMove);
  window.addEventListener('mouseup', () => {
    state.activeMaskDrag = null;
    state.isDraggingMask = false;
    if (state.isHoldingFretNote) {
      state.isHoldingFretNote = false;
      stopSustainedVoice('fret_mouse');
      if (!micStream && state.activeMidiNotes.size === 0) {
        state.detectedMidi = null;
        drawFretboard();
      }
    }
    if (state.isHoldingMouseMidi) {
      stopMouseMidiKeyHold();
    }
  });
  window.addEventListener('blur', () => {
    stopAllSustainedVoices();
    state.isHoldingFretNote = false;
    state.isHoldingMouseMidi = false;
    state.activeMidiNotes.clear();
    document.querySelectorAll('.piano-key.active').forEach(el => el.classList.remove('active'));
  });
  initApp();
});

function recordMidiNoteToTab(midi) {
  if (!state.fretboardData) return;
  const minF = Math.max(0, state.exerciseMinFret || 0);
  const maxF = Math.min(24, state.exerciseMaxFret || 24);
  const candidates = state.fretboardData.cells.filter(
    c => c.midi === midi && !isCellMasked(c.s, c.f) && c.f >= minF && c.f <= maxF
  );
  const pool = candidates.length > 0
    ? candidates
    : state.fretboardData.cells.filter(c => c.midi === midi && !isCellMasked(c.s, c.f));
  if (pool.length === 0) return;
  const chosen = pool[0];
  state.recordedEvents.push({
    s: chosen.s,
    f: chosen.f,
    midi: chosen.midi,
    note: chosen.note,
    freq: chosen.freq,
    dur: state.defaultDuration,
    tuplet: state.tupletActive ? 3 : 0,
    rest: 0,
    tech: '-'
  });
  renderStageAndInspector();
}

function playCurrentScalePreview() {
  if (!state.fretboardData) return;
  const active = state.fretboardData.cells
    .filter(c => c.inPosition && !isCellMasked(c.s, c.f))
    .slice(0, 12);
  active.forEach((c, idx) => {
    setTimeout(() => {
      state.detectedMidi = c.midi;
      drawFretboard();
      pluckGuitarFrequency(c.freq, 0.7);
    }, idx * 220);
  });
}

function switchMainView(view, btn) {
  if (typeof stopArpeggioPlayback === 'function') {
    stopArpeggioPlayback(false);
  }
  state.mainView = view;
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
  if (btn) btn.classList.add('active');

  const subtabs = document.getElementById('practiceSubtabs');
  const title = document.getElementById('viewHeaderTitle');
  if (view === 'practice') {
    subtabs.style.display = 'flex';
    title.style.display = 'none';
  } else {
    subtabs.style.display = 'none';
    title.style.display = 'block';
    const titles = {
      tuner: '← Tuner',
      ear: 'Kształcenie słuchu',
      changes: 'Zmiany i Progresje',
      workshop: 'Warsztat – Sesje',
      settings: 'Ustawienia i Postępy'
    };
    title.textContent = titles[view] || '';
  }
  renderAll();
}

function switchPracticeSubtab(sub, btn) {
  if (typeof stopExercisePlayer === 'function') {
    stopExercisePlayer(false);
  }
  if (typeof stopArpeggioPlayback === 'function') {
    stopArpeggioPlayback(false);
  }
  state.subtab = sub;
  document.querySelectorAll('.subtab-btn').forEach(b => {
    const isMatch = btn ? (b === btn) : (b.getAttribute('data-subtab') === sub);
    b.classList.toggle('active', isMatch);
  });
  renderAll();
}

async function renderAll() {
  await refreshFretboardFromCpp();
  // Ukryj prawy panel inspektora w Kształceniu słuchu, na pełnoekranowych ekranach Zmian, w Praktyce skal oraz w Odtwarzaczu ćwiczeń
  const inspPanel = document.getElementById('rightInspector');
  if (inspPanel) {
    const hideInsp = (state.mainView === 'ear') ||
      (state.mainView === 'changes' && state.changesScreen !== 'editor') ||
      (state.mainView === 'practice' && state.subtab === 'scale_practice') ||
      (state.mainView === 'practice' && state.subtab === 'exercise' && state.exercisePlayerActive);
    inspPanel.style.display = hideInsp ? 'none' : 'flex';
  }
  // Gryf główny pokazywany w zakładkach Skala, Akord, Flażolety, Szukaj, Arpeggio;
  // W Ćwiczeniu (TAB) i Praktyce skal układ jest dopasowany indywidualnie (pięciolinia nad gryfem lub kreator).
  const fbCard = document.getElementById('fretboardCardContainer');
  const hideFretboard = (state.mainView !== 'practice') ||
    (state.subtab === 'scale_practice') ||
    (state.subtab === 'exercise');
  fbCard.style.display = hideFretboard ? 'none' : 'flex';

  await renderStageAndInspector();
  drawFretboard();
}

function buildRootButtonsHtml(onClickFnName) {
  const labels = getRootLabels();
  return `<div class="btn-grid-6">` +
    labels.map((lbl, pc) =>
      `<button class="chip-btn ${state.rootPc === pc ? 'active' : ''}" onclick="${onClickFnName}(${pc})">${lbl}</button>`
    ).join('') + `</div>`;
}

function setRootPc(pc) { state.rootPc = pc; state.selectedScaleNotePc = pc; saveSettings(); renderAll(); }
// Znaki chromatyczne (bemole / krzyżyki) przełączamy w panelu „Strój i Gryf” (rozwijane ustawienia gryfu u góry).
function setAccMode(m) { state.accMode = m; saveSettings(); renderTuningModalControls(); updateTopTuningLabel(); renderAll(); }

// Generuje czysty graficzny diagram 6-liniowy SVG dla karty akordu (dokładnie jak na 1. screenshocie!)
function buildChordSvgDiagram(frets, tapped) {
  const W = 136, H = 76;
  const leftX = 8, rightX = 128;
  const topY = 10, rowGap = 11;
  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg">`;

  // 6 poziomych linii strun (góra = struna 1 E4, dół = struna 6 E2)
  for (let r = 0; r < 6; r++) {
    const y = topY + r * rowGap;
    svg += `<line x1="${leftX}" y1="${y}" x2="${rightX}" y2="${y}" stroke="#8c857b" stroke-width="1.2"/>`;
  }
  // Pionowe zamknięcia po lewej i prawej stronie
  svg += `<line x1="${leftX}" y1="${topY}" x2="${leftX}" y2="${topY + 5 * rowGap}" stroke="#8c857b" stroke-width="1.3"/>`;
  svg += `<line x1="${rightX}" y1="${topY}" x2="${rightX}" y2="${topY + 5 * rowGap}" stroke="#8c857b" stroke-width="1.3"/>`;

  // Numery progów (np. T12, T13, 17, 0) na odpowiednich liniach strun z ciemnym tłem pod tekstem
  for (let s = 0; s < 6; s++) {
    const f = frets[s];
    if (f === undefined || f < 0) continue;
    const r = 5 - s;
    const y = topY + r * rowGap;
    const label = (tapped && tapped[s] ? 'T' : '') + String(f);
    const boxW = label.length * 7 + 6;
    const cx = 26;
    svg += `<rect x="${cx - boxW/2}" y="${y - 6}" width="${boxW}" height="12" fill="#191512"/>`;
    svg += `<text x="${cx}" y="${y + 3.5}" fill="#f5f2eb" font-family="Segoe UI, sans-serif" font-size="11" font-weight="700" text-anchor="middle">${label}</text>`;
  }

  svg += `</svg>`;
  return svg;
}

async function renderStageAndInspector() {
  const stage = document.getElementById('dynamicStagePanel');
  const insp = document.getElementById('rightInspector');
  if (!state.initData) return;
  if (!state.changesIsPlaying) saveSettings();

  // =========================================================================
  // WIDOK: STROIK (TUNER) — BEZ GRYFU, PÓŁKOLISTY ZEGAR + 6 STRUN (Screenshot 5)
  // =========================================================================
  if (state.mainView === 'tuner') {
    const noteNames = getRootLabels();
    const openStrings = state.customStringMidis.map((midi, idx) => ({
      idx,
      num: 6 - idx,
      name: noteNames[((midi % 12) + 12) % 12],
      midi
    }));
    stage.innerHTML = `
      <div class="tuner-stage-card">
        <canvas id="tunerArcCanvas" width="560" height="310" style="max-width:100%;"></canvas>
        <div id="tunerNoteReadout" style="font-size:15px;color:var(--text-muted);">Zagraj strunę...</div>
        <div class="tuner-strings-row">
          ${openStrings.map(st => `
            <div class="tuner-string-btn ${state.tunerLockedString === st.idx ? 'active' : ''}"
                 onmousedown="toggleTunerStringLock(event, ${st.idx}, ${st.midi})">
              <span style="font-size:10px;color:var(--text-muted);">${st.num}</span>
              <span style="font-size:16px;font-weight:800;">${st.name}</span>
            </div>
          `).join('')}
        </div>
        <div style="font-size:11px;color:var(--text-muted);">Dotknij i przytrzymaj strunę, żeby zablokować na niej tuner i odsłuchać wzorzec.</div>
      </div>`;

    const currentTuningObj = state.initData.tunings.find(t => t.id === state.tuningId);
    const currentTuningTitle = currentTuningObj
      ? currentTuningObj.name
      : `Własny (${openStrings.map(s => s.name).join(' ')})`;
    insp.innerHTML = `
      <div>
        <div class="section-label">STRÓJ</div>
        <div style="font-size:18px;font-weight:800;margin-bottom:8px;">${currentTuningTitle}</div>
        <select onchange="selectPresetTuning(this.value)">
          ${state.initData.tunings.map(t => `<option value="${t.id}" ${t.id===state.tuningId?'selected':''}>${t.name}</option>`).join('')}
          <option value="custom" ${state.tuningId==='custom'?'selected':''}>Własny strój (niestandardowy)</option>
        </select>
        <div class="section-label" style="margin-top:10px;">POJEDYNCZE STRUNY (6 → 1)</div>
        <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:6px;">
          ${openStrings.map(st => `
            <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:7px;padding:4px 6px;display:flex;align-items:center;justify-content:space-between;font-size:12px;">
              <span style="color:var(--text-muted);">${st.num}:</span>
              <button class="chip-btn" style="padding:1px 6px;" onclick="tuneSingleString(${st.idx}, -1)">−</button>
              <strong style="color:var(--accent);min-width:20px;text-align:center;">${st.name}</strong>
              <button class="chip-btn" style="padding:1px 6px;" onclick="tuneSingleString(${st.idx}, 1)">+</button>
            </div>
          `).join('')}
        </div>
      </div>
      <div>
        <div class="section-label">TRYB</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.tunerMode==='guitar'?'active':''}" onclick="state.tunerMode='guitar';renderStageAndInspector()">Gitara</button>
          <button class="chip-btn ${state.tunerMode==='chromatic'?'active':''}" onclick="state.tunerMode='chromatic';state.tunerLockedString=-1;renderStageAndInspector()">Chromatyczny</button>
        </div>
      </div>
      <div>
        <div class="section-label">WYŚWIETLANIE</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.tunerDisplay==='needle'?'active':''}" onclick="state.tunerDisplay='needle';renderTunerCanvas();renderStageAndInspector()">Zwykły</button>
          <button class="chip-btn ${state.tunerDisplay==='strobe'?'active':''}" onclick="state.tunerDisplay='strobe';renderTunerCanvas();renderStageAndInspector()">Strobo</button>
        </div>
      </div>
      <div>
        <div class="section-label">WEJŚCIE AUDIO (MIKROFON / INTERFEJS)</div>
        <select onchange="onAudioInputChanged(this.value)">
          ${state.audioInputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioInputId?'selected':''}>${d.label}</option>`).join('')}
        </select>
      </div>
      <div>
        <div class="section-label">ODTWARZANIE (GŁOŚNIKI / SŁUCHAWKI)</div>
        <select onchange="onAudioOutputChanged(this.value)">
          ${state.audioOutputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioOutputId?'selected':''}>${d.label}</option>`).join('')}
        </select>
      </div>
      <div>
        <div class="section-label">KALIBRACJA A4 (Hz)</div>
        <input type="number" value="${state.a4ReferenceHz}" min="430" max="450" step="0.5" onchange="state.a4ReferenceHz=parseFloat(this.value)">
      </div>`;
    renderTunerCanvas();
    return;
  }

  // =========================================================================
  // WIDOK: KSZTAŁCENIE SŁUCHU (TRENING SŁUCHU – ZRZUTY 1–7)
  // =========================================================================
  if (state.mainView === 'ear') {
    const allIntervals = [
      { semi: 0,  name: 'Pryma',          code: 'P1', hint: 'Powtórzony ten sam dźwięk' },
      { semi: 1,  name: 'Sekunda mała',   code: 'm2', hint: 'Szczęki (Jaws)' },
      { semi: 2,  name: 'Sekunda wielka', code: 'M2', hint: 'Sto lat / Happy Birthday' },
      { semi: 3,  name: 'Tercja mała',    code: 'm3', hint: 'Smoke on the Water' },
      { semi: 4,  name: 'Tercja wielka',  code: 'M3', hint: 'Oh When the Saints' },
      { semi: 5,  name: 'Kwarta czysta',  code: 'P4', hint: 'Marsz weselny' },
      { semi: 6,  name: 'Tryton',         code: 'TT', hint: 'The Simpsons' },
      { semi: 7,  name: 'Kwinta czysta',  code: 'P5', hint: 'Gwiezdne Wojny (Star Wars)' },
      { semi: 8,  name: 'Seksta mała',    code: 'm6', hint: 'The Entertainer' },
      { semi: 9,  name: 'Seksta wielka',  code: 'M6', hint: 'My Bonnie Lies Over the Ocean' },
      { semi: 10, name: 'Septyma mała',   code: 'm7', hint: 'The Winner Takes It All' },
      { semi: 11, name: 'Septyma wielka', code: 'M7', hint: 'Take On Me (refren)' },
      { semi: 12, name: 'Oktawa',         code: 'P8', hint: 'Somewhere Over the Rainbow' }
    ];

    const earChordCatalog = [
      { id: 'major', label: 'Durowy',      group: 'triad', intervals: [0, 4, 7] },
      { id: 'minor', label: 'Molowy',      group: 'triad', intervals: [0, 3, 7] },
      { id: 'dim',   label: 'Zmniejszony', group: 'triad', intervals: [0, 3, 6] },
      { id: 'maj7',  label: 'maj7',        group: 'seventh', intervals: [0, 4, 7, 11] },
      { id: '7',     label: '7',           group: 'seventh', intervals: [0, 4, 7, 10] },
      { id: 'm7',    label: 'm7',          group: 'seventh', intervals: [0, 3, 7, 10] },
      { id: 'm7b5',  label: 'm7b5',        group: 'seventh', intervals: [0, 3, 6, 10] }
    ];

    // EKRAN AKTYWNEGO TRENINGU SŁUCHU (po kliknięciu „Graj” – Zrzuty 2, 3, 4, 6, 7)
    if (state.earScreen === 'active') {
      const selectedIvObjs = allIntervals.filter(iv => state.earSelectedSemis.has(iv.semi));
      const selectedChObjs = earChordCatalog.filter(c => state.earSelectedChords.has(c.id));
      const roundLabel = state.earSessionType === 'endless'
        ? `${state.earRound} / ∞`
        : `${state.earRound} / ${state.earTargetRounds}`;

      const modeTitleMap = {
        learn: `Nauka · ${selectedIvObjs.length} interwałów`,
        identify: `Rozpoznaj · ${selectedIvObjs.length} interwałów`,
        play: `Zagraj · ${selectedIvObjs.length} interwałów`,
        chords: `Akordy · ${selectedChObjs.length} rodzajów akordów`
      };

      const topHeaderHtml = `
        <div style="display:flex;justify-content:space-between;align-items:flex-start;margin-bottom:8px;">
          <div style="display:flex;align-items:center;gap:14px;">
            <button class="chip-btn" style="width:36px;height:36px;font-size:16px;" onclick="exitEarActiveSession()">✕</button>
            <div>
              <h2 style="font-size:22px;font-weight:800;">Trening słuchu</h2>
              <div style="font-size:12.5px;color:var(--text-muted);margin-top:2px;">${modeTitleMap[state.earMode]}</div>
            </div>
          </div>
          ${state.earMode !== 'learn' ? `<div style="font-family:monospace;font-size:15px;color:var(--text-muted);">${roundLabel}</div>` : ''}
        </div>`;

      // WARIANT 1: NAUKA (Zrzut 3)
      if (state.earMode === 'learn') {
        stage.innerHTML = `
          <div class="ear-trainer-screen">
            ${topHeaderHtml}
            <div style="text-align:center;color:var(--text-muted);font-size:14px;margin-top:10px;">
              ${state.earCompareMode ? 'Wybierz dwa interwały, aby odsłuchać je jeden po drugim' : 'Dotknij interwału, aby go usłyszeć'}
            </div>
            <div class="ear-answer-grid">
              ${selectedIvObjs.map(iv => {
                const isComp = state.earCompareSelection.includes(iv.semi);
                return `
                  <button class="ear-answer-btn ${isComp?'selected':''}" onclick="onEarLearnTileClick(${iv.semi})">
                    <span style="font-size:15px;font-weight:700;">${iv.name}</span>
                    <span style="font-size:12px;color:var(--text-muted);">${iv.code}</span>
                    ${state.earShowSongHints ? `<span style="font-size:10px;color:var(--accent);">🎵 ${iv.hint}</span>` : ''}
                  </button>`;
              }).join('')}
            </div>
            <div style="display:grid;grid-template-columns:1fr 1fr;gap:14px;max-width:684px;margin:12px auto 0;width:100%;">
              <button class="chip-btn ${state.earCompareMode?'active':''}" style="padding:14px;font-size:14px;"
                      onclick="toggleEarCompareMode()">Wybierz dwa, aby porównać</button>
              <button class="action-btn" style="padding:14px;font-size:14px;"
                      onclick="state.earMode='identify';startEarTrainingSession()">Sprawdź się</button>
            </div>
          </div>`;
        return;
      }

      const q = state.earCurrentQuestion || {};

      // WARIANT 2: ROZPOZNAJ INTERWAŁ (Zrzut 2)
      if (state.earMode === 'identify') {
        const badgeText = q.answered ? q.correctCode : '?';
        const badgeBorder = q.answered ? (q.wasCorrect ? 'var(--green)' : 'var(--red)') : 'var(--accent)';
        stage.innerHTML = `
          <div class="ear-trainer-screen" style="align-items:center;margin-top:24px;">
            <div style="width:100%;">${topHeaderHtml}</div>
            <div style="width:92px;height:92px;border-radius:50%;border:2px solid ${badgeBorder};display:flex;align-items:center;justify-content:center;font-size:28px;font-weight:800;color:var(--accent);margin-top:16px;cursor:pointer;"
                 onclick="replayCurrentEarQuestion()">${badgeText}</div>
            <div style="font-size:17px;font-weight:700;color:var(--text-muted);">
              ${q.answered ? (q.wasCorrect ? '✅ Poprawnie! To ' + q.correctName : '❌ To była ' + q.correctName) : 'Jaki interwał?'}
            </div>
            ${state.earShowSongHints && q.answered ? `<div style="font-size:12px;color:var(--accent);">🎵 Skojarzenie: ${q.hint}</div>` : ''}
            <div style="background:#171310;border:1px solid #2b241e;border-radius:8px;padding:8px 22px;display:flex;gap:28px;align-items:center;font-size:13px;">
              <div><b style="font-size:19px;">${state.earScore}</b> <span style="color:var(--text-muted);">/ ${state.earAttempts}</span></div>
              <div style="color:var(--text-muted);">Seria: <b>${state.earStreak}</b></div>
            </div>
            <div class="ear-answer-grid">
              ${selectedIvObjs.map(iv => {
                let cls = '';
                if (q.answered) {
                  if (iv.semi === q.semi) cls = 'correct';
                  else if (iv.semi === q.guessedSemi) cls = 'wrong';
                }
                return `
                  <button class="ear-answer-btn ${cls}" onclick="submitEarIdentifyAnswer(${iv.semi})">
                    <span style="font-size:14.5px;font-weight:700;">${iv.name}</span>
                    <span style="font-size:12px;color:var(--text-muted);">${iv.code}</span>
                  </button>`;
              }).join('')}
            </div>
            <div style="display:flex;gap:12px;margin-top:6px;">
              <button class="chip-btn" style="padding:9px 22px;border-radius:20px;" onclick="replayCurrentEarQuestion()">▷ Odtwórz ponownie</button>
              ${q.answered ? `<button class="action-btn" style="padding:9px 22px;border-radius:20px;" onclick="nextEarActiveRound()">${(state.earSessionType === 'fixed' && state.earRound >= state.earTargetRounds) ? 'Podsumowanie →' : 'Następna runda →'}</button>` : ''}
            </div>
          </div>`;
        return;
      }

      // WARIANT 3: ZAGRAJ NA GITARZE (Zrzut 4)
      if (state.earMode === 'play') {
        const sharpNames = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
        const rootName = sharpNames[((q.rootMidi || 59) % 12 + 12) % 12];
        const dirText = q.asc ? `w górę od ${rootName}` : `w dół od ${rootName}`;
        const detectedText = state.detectedMidi !== null
          ? sharpNames[((state.detectedMidi % 12) + 12) % 12] + (Math.floor(state.detectedMidi / 12) - 1)
          : '—';

        stage.innerHTML = `
          <div class="ear-trainer-screen" style="align-items:center;margin-top:28px;">
            <div style="width:100%;">${topHeaderHtml}</div>
            <div style="font-size:13px;color:var(--text-muted);margin-top:16px;">Zagraj lub zaśpiewaj (albo użyj klawiatury na dole)</div>
            <div style="font-size:38px;font-weight:800;line-height:1.1;">${q.correctName || 'Tercja mała'}</div>
            <div style="font-size:21px;font-weight:700;color:var(--accent);">${dirText}</div>
            <button class="chip-btn" style="padding:9px 22px;border-radius:20px;margin-top:4px;"
                    onclick="pluckGuitarFrequency(midiToFreq(${q.rootMidi || 59}), 1.5)">▷ Odtwórz ${rootName}</button>
            <div id="earPlayDetectedBox" ${q.answered ? 'data-answered="1"' : ''} style="width:96px;height:78px;background:#171310;border:1px solid ${q.answered?'var(--green)':'#2b241e'};border-radius:10px;display:flex;align-items:center;justify-content:center;font-size:24px;font-weight:800;color:${q.answered?'var(--green)':'var(--text-muted)'};margin:12px 0;">
              ${q.answered ? '✔ ' + q.targetNoteName : detectedText}
            </div>
            <div style="background:#171310;border:1px solid #2b241e;border-radius:8px;padding:10px 28px;display:flex;gap:80px;align-items:center;font-size:13px;">
              <div><b style="font-size:20px;">${state.earScore}</b> <span style="color:var(--text-muted);">/ ${state.earAttempts}</span></div>
              <div style="color:var(--text-muted);">Seria: <b>${state.earStreak}</b></div>
            </div>
            ${q.answered ? `<button class="action-btn" style="padding:9px 22px;border-radius:20px;" onclick="nextEarActiveRound()">${(state.earSessionType === 'fixed' && state.earRound >= state.earTargetRounds) ? 'Podsumowanie →' : 'Następna runda →'}</button>` : ''}
          </div>`;
        return;
      }

      // WARIANT 4: AKORDY (Zrzuty 6 i 7)
      if (state.earMode === 'chords') {
        const invLabels = ['Zasadn.', '1.', '2.', '3.'];
        const activeInvs = [0,1,2,3].filter(inv => state.earSelectedInversions.has(inv));
        stage.innerHTML = `
          <div class="ear-trainer-screen" style="align-items:center;margin-top:16px;">
            <div style="width:100%;">${topHeaderHtml}</div>
            <div style="font-size:20px;font-weight:700;color:var(--text-muted);">
              ${q.answered ? (q.wasCorrect ? '✅ Brawo! To akord ' + q.correctName : '❌ To był akord ' + q.correctName) : 'Jaki to akord?'}
            </div>
            <div style="background:#171310;border:1px solid #2b241e;border-radius:8px;padding:10px 28px;display:flex;gap:90px;align-items:center;font-size:13px;">
              <div><b style="font-size:20px;">${state.earScore}</b> <span style="color:var(--text-muted);">/ ${state.earAttempts}</span></div>
              <div style="color:var(--text-muted);">Seria: <b>${state.earStreak}</b></div>
            </div>
            <button class="chip-btn" style="padding:9px 22px;border-radius:20px;" onclick="replayCurrentEarQuestion()">▷ Odtwórz ponownie</button>

            <div style="width:100%;max-width:660px;">
              <div class="section-label">AKORDY</div>
              <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:10px;">
                ${selectedChObjs.map(ch => {
                  let cls = (state.earPickedChordId === ch.id) ? 'selected' : '';
                  if (q.answered) {
                    if (ch.id === q.chordId) cls = 'correct';
                    else if (ch.id === state.earPickedChordId) cls = 'wrong';
                  }
                  return `
                    <button class="ear-answer-btn ${cls}" style="width:100%;height:54px;"
                            onclick="state.earPickedChordId='${ch.id}';renderStageAndInspector()">
                      <span style="font-size:14px;font-weight:700;">${ch.label}</span>
                    </button>`;
                }).join('')}
              </div>
            </div>

            ${activeInvs.length > 1 ? `
            <div style="width:100%;max-width:660px;">
              <div class="section-label">PRZEWROTY</div>
              <div class="ear-seg-row">
                ${activeInvs.map(inv => `
                  <button class="ear-seg-btn ${state.earPickedInversion===inv?'active':''}"
                          onclick="state.earPickedInversion=${inv};renderStageAndInspector()">${invLabels[inv]}</button>
                `).join('')}
              </div>
            </div>` : ''}

            <div style="width:100%;max-width:660px;">
              ${!q.answered ? `
                <button class="action-btn" style="width:100%;padding:14px;font-size:15px;"
                        onclick="submitEarChordAnswer()">Sprawdź</button>
              ` : `
                <button class="action-btn" style="width:100%;padding:14px;font-size:15px;"
                        onclick="nextEarActiveRound()">${(state.earSessionType === 'fixed' && state.earRound >= state.earTargetRounds) ? 'Podsumowanie →' : 'Następna runda →'}</button>
              `}
            </div>
          </div>`;
        return;
      }
    }

    const isChordsMode = (state.earMode === 'chords');
    const selectedCount = isChordsMode ? state.earSelectedChords.size : state.earSelectedSemis.size;
    const modeLabelMap = { learn: 'Nauka', identify: 'Rozpoznaj', play: 'Zagraj', chords: 'Akordy' };
    const summaryTitle = isChordsMode
      ? `Akordy · ${selectedCount} rodzajów akordów`
      : `${modeLabelMap[state.earMode]} · ${selectedCount} interwałów`;
    const pbText = (state.earPlaybackStyle === 'harmonic') ? 'Harmoniczne' : 'Melodyczne';
    const dirText = (state.earDirection === 'asc') ? 'Rosnąco' : (state.earDirection === 'desc' ? 'Malejąco' : 'Losowo');
    const roundsText = (state.earSessionType === 'endless') ? 'Bez końca' : `${state.earTargetRounds} rund`;
    const showDirectionOption = (!isChordsMode && state.earPlaybackStyle !== 'harmonic');
    const summarySub = showDirectionOption ? `${pbText} · ${dirText} · ${roundsText}` : `${pbText} · ${roundsText}`;

    // EKRAN PODSUMOWANIA PO UKOŃCZENIU WYBRANEJ LICZBY RUND (Zrzut ekranu podsumowania!)
    if (state.earScreen === 'summary') {
      const totalRounds = Math.max(1, state.earAttempts || state.earTargetRounds);
      const accuracyPct = Math.round((state.earScore / totalRounds) * 100);
      const headline = (accuracyPct >= 85)
        ? 'Świetny słuch! Tak trzymaj'
        : (accuracyPct >= 55 ? 'Dobra robota, robisz postępy!' : 'Słuchaj dalej, będzie łatwiej');
      const fullSub = `${summaryTitle} · ${summarySub}`;

      const baseXp = 10;
      const bonusXp = Math.round(state.earScore * 2);
      const totalXp = baseXp + bonusXp;
      const earnedNotes = Math.max(5, Math.round(totalXp / 2));

      const reactArr = Array.isArray(state.earReactionTimesMs) && state.earReactionTimesMs.length > 0
        ? state.earReactionTimesMs
        : [900];
      const avgReactSec = (reactArr.reduce((a, b) => a + b, 0) / reactArr.length / 1000).toFixed(1).replace('.', ',');

      const histList = Array.isArray(state.earHistory) ? state.earHistory : [];
      const weakItems = histList.filter(h => h.correct < h.total);
      const weakCount = weakItems.length;

      stage.innerHTML = `
        <div class="ear-config-layout" style="margin-top:12px;">
          <div class="ear-config-main" style="align-items:center;">
            <!-- Koło dokładności -->
            <div style="width:94px;height:94px;border-radius:50%;background:#171310;border:2px solid ${accuracyPct >= 70 ? '#4ade80' : '#2e2721'};display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;">
              <div style="font-size:23px;font-weight:800;color:#f5f2eb;">${accuracyPct}%</div>
              <div style="font-size:10.5px;color:var(--text-muted);">Dokładność</div>
            </div>

            <div style="text-align:center;">
              <h2 style="font-size:28px;font-weight:800;margin-bottom:6px;">${headline}</h2>
              <div style="font-size:13px;color:var(--text-muted);">${fullSub}</div>
            </div>

            <!-- 4 kafelki statystyk: Wynik, Czas, Najlepsza seria, Zdobyte XP -->
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:10px;width:100%;max-width:520px;">
              <div style="background:#171310;border:1px solid #26201b;border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earScore}/${totalRounds}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">Wynik</div>
              </div>
              <div style="background:#171310;border:1px solid #26201b;border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earSessionDurationSec || 30}s</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">Czas</div>
              </div>
              <div style="background:#171310;border:1px solid #26201b;border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earBestStreak || 0}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">Najlepsza seria</div>
              </div>
              <div style="background:rgba(243,146,55,0.08);border:1px solid #6e441f;border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;color:var(--accent);">+${totalXp} XP</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">Zdobyte XP</div>
              </div>
            </div>

            <div style="font-size:12.5px;font-weight:600;color:var(--accent);">
              ${state.streakDays} dni z rzędu. Poćwicz jutro, a będzie ${state.streakDays + 1}.
            </div>

            <!-- Rozliczenie XP i Nut -->
            <div style="background:#171310;border:1px solid #26201b;border-radius:10px;padding:14px 18px;width:100%;max-width:520px;display:flex;flex-direction:column;gap:8px;font-size:13px;">
              <div style="display:flex;justify-content:space-between;color:var(--text-muted);">
                <span>Baza</span>
                <strong style="color:var(--text);">+${baseXp} XP</strong>
              </div>
              ${bonusXp > 0 ? `
              <div style="display:flex;justify-content:space-between;color:var(--text-muted);">
                <span>Trafne odpowiedzi</span>
                <strong style="color:var(--text);">+${bonusXp} XP</strong>
              </div>` : ''}
              <div style="display:flex;justify-content:space-between;border-top:1px solid #26201b;padding-top:8px;">
                <span style="color:var(--text-muted);">Razem</span>
                <strong style="color:var(--accent);">+${totalXp} XP</strong>
              </div>
              <div style="text-align:right;font-size:12px;font-weight:700;color:var(--accent);">
                +${earnedNotes} Nut
              </div>
            </div>

            <!-- Według interwałów / Według akordów -->
            <div style="width:100%;max-width:520px;margin-top:4px;">
              <div style="display:flex;justify-content:space-between;align-items:baseline;margin-bottom:12px;">
                <h3 style="font-size:18px;font-weight:800;margin:0;">${isChordsMode ? 'Według akordów' : 'Według interwałów'}</h3>
                <span style="font-size:12px;color:var(--text-muted);">Średnia odpowiedź ${avgReactSec} s</span>
              </div>
              <div style="display:flex;flex-direction:column;gap:10px;">
                ${histList.map(item => {
                  const pct = item.total > 0 ? Math.round((item.correct / item.total) * 100) : 0;
                  const barCol = pct >= 75 ? '#4ade80' : (pct > 0 ? 'var(--accent)' : '#3b322b');
                  return `
                    <div style="display:grid;grid-template-columns:165px 1fr 44px;align-items:center;gap:12px;font-size:13px;">
                      <div>
                        <strong>${item.name}</strong>
                        ${item.code ? `<span style="color:var(--text-muted);font-size:11.5px;margin-left:4px;">${item.code}</span>` : ''}
                      </div>
                      <div style="height:6px;background:#1e1915;border-radius:4px;overflow:hidden;">
                        <div style="width:${pct}%;height:100%;background:${barCol};border-radius:4px;"></div>
                      </div>
                      <div style="font-family:monospace;color:var(--text-muted);text-align:right;">${item.correct}/${item.total}</div>
                    </div>`;
                }).join('')}
              </div>
            </div>
          </div>

          <!-- Prawa kolumna: Co dalej -->
          <div class="ear-summary-box">
            <div style="font-size:18px;font-weight:800;margin-bottom:4px;">Co dalej</div>
            <button class="action-btn" style="width:100%;padding:13px;font-size:14.5px;"
                    onclick="startEarTrainingSession()">↻ Jeszcze raz</button>
            ${weakCount > 0 ? `
              <button class="chip-btn" style="width:100%;padding:12px;font-size:13px;font-weight:700;"
                      onclick="startEarWeakItemsSession()">
                Ćwicz ${weakCount} ${isChordsMode ? 'słabych akordów' : 'słabych interwałów'}
              </button>
              <div style="font-size:11.5px;color:var(--text-muted);text-align:center;">
                Ćwiczenie to ${state.earTargetRounds} rund z najsłabszymi ${isChordsMode ? 'akordami' : 'interwałami'}.
              </div>
            ` : ''}
            <div style="text-align:center;margin-top:8px;">
              <span style="font-size:13px;font-weight:700;color:var(--text-muted);cursor:pointer;"
                    onclick="exitEarActiveSession()">Zmień ustawienia</span>
            </div>
          </div>
        </div>`;
      return;
    }

    const baseNoteLabels = getRootLabels();

    stage.innerHTML = `
      <div class="ear-config-layout">
        <!-- Lewa kolumna konfiguracji (Co ćwiczyć / Jak to brzmi / Sesja) -->
        <div class="ear-config-main">
          <div>
            <h3 style="font-size:18px;font-weight:800;margin-bottom:14px;">Co ćwiczyć</h3>
            <div class="section-label">TRYB</div>
            <div class="ear-seg-row" style="margin-bottom:18px;">
              <button class="ear-seg-btn ${state.earMode==='learn'?'active':''}" onclick="state.earMode='learn';renderStageAndInspector()">Nauka</button>
              <button class="ear-seg-btn ${state.earMode==='identify'?'active':''}" onclick="state.earMode='identify';renderStageAndInspector()">Rozpoznaj</button>
              <button class="ear-seg-btn ${state.earMode==='play'?'active':''}" onclick="state.earMode='play';renderStageAndInspector()">Zagraj</button>
              <button class="ear-seg-btn ${state.earMode==='chords'?'active':''}" onclick="state.earMode='chords';state.earPlaybackStyle='harmonic';renderStageAndInspector()">Akordy</button>
            </div>

            ${!isChordsMode ? `
              <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:8px;">
                <span class="section-label" style="margin:0;">INTERWAŁY</span>
                <div style="display:flex;gap:14px;font-size:12px;font-weight:700;color:var(--accent);">
                  <span style="cursor:pointer;" onclick="setAllEarIntervals(true)">Zaznacz wszystko</span>
                  <span style="cursor:pointer;" onclick="setAllEarIntervals(false)">Odznacz wszystko</span>
                </div>
              </div>
              <div class="ear-intervals-grid">
                ${allIntervals.map(iv => `
                  <div class="ear-interval-tile ${state.earSelectedSemis.has(iv.semi)?'selected':''}"
                       onclick="toggleEarIntervalTile(${iv.semi})">
                    <span class="iv-name">${iv.name}</span>
                    <span class="iv-code">${iv.code}</span>
                  </div>
                `).join('')}
              </div>
            ` : `
              <div class="section-label">AKORDY</div>
              <div style="font-size:12px;color:var(--text-muted);margin:6px 0;">Trójdźwięki</div>
              <div style="display:flex;gap:8px;flex-wrap:wrap;margin-bottom:12px;">
                ${earChordCatalog.filter(c => c.group === 'triad').map(c => `
                  <button class="ear-chord-pill ${state.earSelectedChords.has(c.id)?'selected':''}"
                          onclick="toggleEarChordTile('${c.id}')">${c.label}</button>
                `).join('')}
              </div>
              <div style="font-size:12px;color:var(--text-muted);margin:6px 0;">Akordy septymowe</div>
              <div style="display:flex;gap:8px;flex-wrap:wrap;margin-bottom:16px;">
                ${earChordCatalog.filter(c => c.group === 'seventh').map(c => `
                  <button class="ear-chord-pill ${state.earSelectedChords.has(c.id)?'selected':''}"
                          onclick="toggleEarChordTile('${c.id}')">${c.label}</button>
                `).join('')}
              </div>

              <div class="section-label">PRZEWROTY</div>
              <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;margin-bottom:16px;">
                ${['Zasadn.','1.','2.','3.'].map((lbl, invIdx) => `
                  <button class="chip-btn ${state.earSelectedInversions.has(invIdx)?'active':''}" style="padding:10px;"
                          onclick="toggleEarInversionTile(${invIdx})">${lbl}</button>
                `).join('')}
              </div>

              <div class="section-label">INSTRUMENT</div>
              <div class="btn-grid-2">
                <button class="chip-btn ${state.earInstrument==='guitar'?'active':''}" style="padding:10px;"
                        onclick="state.earInstrument='guitar';renderStageAndInspector()">Gitara</button>
                <button class="chip-btn ${state.earInstrument==='piano'?'active':''}" style="padding:10px;"
                        onclick="state.earInstrument='piano';renderStageAndInspector()">Fortepian</button>
              </div>
            `}
          </div>

          <!-- Jak to brzmi -->
          <div>
            <h3 style="font-size:18px;font-weight:800;margin-bottom:14px;">Jak to brzmi</h3>
            <div style="display:grid;grid-template-columns:${showDirectionOption ? '1fr 1.2fr' : '1fr'};gap:16px;margin-bottom:14px;">
              <div>
                <div class="section-label">ODTWARZANIE</div>
                <div class="ear-seg-row">
                  <button class="ear-seg-btn ${state.earPlaybackStyle==='melodic'?'active':''}"
                          onclick="state.earPlaybackStyle='melodic';renderStageAndInspector()">Melodyczne</button>
                  <button class="ear-seg-btn ${state.earPlaybackStyle==='harmonic'?'active':''}"
                          onclick="state.earPlaybackStyle='harmonic';renderStageAndInspector()">Harmoniczne</button>
                </div>
              </div>
              ${showDirectionOption ? `
              <div>
                <div class="section-label">KIERUNEK</div>
                <div class="ear-seg-row">
                  <button class="ear-seg-btn ${state.earDirection==='asc'?'active':''}"
                          onclick="state.earDirection='asc';renderStageAndInspector()">Rosnąco</button>
                  <button class="ear-seg-btn ${state.earDirection==='desc'?'active':''}"
                          onclick="state.earDirection='desc';renderStageAndInspector()">Malejąco</button>
                  <button class="ear-seg-btn ${state.earDirection==='rand'?'active':''}"
                          onclick="state.earDirection='rand';renderStageAndInspector()">Losowo</button>
                </div>
              </div>` : ''}
            </div>

            <div class="section-label">TRYB NUTY BAZOWEJ</div>
            <div class="ear-seg-row">
              <button class="ear-seg-btn ${state.earRootMode==='random'?'active':''}"
                      onclick="state.earRootMode='random';renderStageAndInspector()">Losowy</button>
              <button class="ear-seg-btn ${state.earRootMode==='fixed_note'?'active':''}"
                      onclick="state.earRootMode='fixed_note';renderStageAndInspector()">Stała nuta</button>
              <button class="ear-seg-btn ${state.earRootMode==='fixed_pitch'?'active':''}"
                      onclick="state.earRootMode='fixed_pitch';renderStageAndInspector()">Nuta + oktawa</button>
            </div>

            ${state.earRootMode !== 'random' ? `
              <div style="margin-top:12px;">
                <div style="font-size:12px;color:var(--text-muted);margin-bottom:8px;">Wybierz nutę bazową</div>
                <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:8px;">
                  ${baseNoteLabels.map((lbl, pc) => `
                    <button class="chip-btn ${state.earFixedRootPc===pc?'active':''}"
                            style="padding:10px;font-size:13px;font-weight:700;"
                            onclick="selectEarBaseNote(${pc})">${lbl}</button>
                  `).join('')}
                </div>
                ${state.earRootMode === 'fixed_pitch' ? `
                  <div style="font-size:12px;color:var(--text-muted);margin:12px 0 8px;">Wybierz oktawę bazową (${baseNoteLabels[state.earFixedRootPc]} ${state.earFixedOctave})</div>
                  <div class="ear-seg-row">
                    ${[2, 3, 4, 5].map(oct => `
                      <button class="ear-seg-btn ${state.earFixedOctave===oct?'active':''}"
                              onclick="selectEarBaseOctave(${oct})">Oktawa ${oct} (${baseNoteLabels[state.earFixedRootPc]} ${oct})</button>
                    `).join('')}
                  </div>
                ` : ''}
              </div>
            ` : ''}
          </div>

          <!-- Sesja -->
          <div>
            <h3 style="font-size:18px;font-weight:800;margin-bottom:14px;">Sesja</h3>
            <div style="display:grid;grid-template-columns:${state.earSessionType === 'fixed' ? '1fr 1.5fr' : '1fr'};gap:16px;margin-bottom:16px;">
              <div>
                <div class="section-label">TYP SESJI</div>
                <div class="ear-seg-row">
                  <button class="ear-seg-btn ${state.earSessionType==='fixed'?'active':''}"
                          onclick="state.earSessionType='fixed';renderStageAndInspector()">Ustalony</button>
                  <button class="ear-seg-btn ${state.earSessionType==='endless'?'active':''}"
                          onclick="state.earSessionType='endless';renderStageAndInspector()">Bez końca</button>
                </div>
              </div>
              ${state.earSessionType === 'fixed' ? `
              <div>
                <div class="section-label">LICZBA RUND (WYBIERZ LUB WPISZ WŁASNĄ)</div>
                <div style="display:flex;gap:8px;align-items:center;">
                  <div class="ear-seg-row" style="flex:1;">
                    ${[5, 10, 15, 20, 30, 50].map(r => `
                      <button class="ear-seg-btn ${state.earTargetRounds===r?'active':''}"
                              onclick="state.earTargetRounds=${r};renderStageAndInspector()">${r}</button>
                    `).join('')}
                  </div>
                  <input type="number" min="1" max="200" value="${state.earTargetRounds}"
                         style="width:68px;padding:8px;text-align:center;font-weight:700;"
                         title="Wpisz dowolną liczbę rund do zagrania"
                         onchange="state.earTargetRounds=Math.max(1,Math.min(200,parseInt(this.value,10)||10));renderStageAndInspector()">
                </div>
              </div>` : ''}
            </div>

            <div style="display:grid;grid-template-columns:1fr 1fr;gap:16px;align-items:center;">
              <div style="display:flex;justify-content:space-between;align-items:center;">
                <span class="section-label" style="margin:0;">MIERZ CZAS REAKCJI</span>
                <button class="ear-toggle-switch ${state.earMeasureReaction?'on':''}"
                        onclick="state.earMeasureReaction=!state.earMeasureReaction;renderStageAndInspector()"></button>
              </div>
              ${!isChordsMode ? `
              <div style="display:flex;justify-content:space-between;align-items:center;">
                <span class="section-label" style="margin:0;">PODPOWIEDZI PIOSENEK</span>
                <button class="ear-toggle-switch ${state.earShowSongHints?'on':''}"
                        onclick="state.earShowSongHints=!state.earShowSongHints;renderStageAndInspector()"></button>
              </div>` : ''}
            </div>
          </div>
        </div>

        <!-- Prawa karta podsumowania i startu (Graj / Zaskocz mnie) -->
        <div class="ear-summary-box">
          <div style="font-size:21px;font-weight:800;line-height:1.25;">${summaryTitle}</div>
          <div style="font-size:12.5px;color:var(--text-muted);margin-bottom:6px;">${summarySub}</div>
          <button class="action-btn" style="width:100%;padding:13px;font-size:15px;"
                  onclick="startEarTrainingSession()">Graj</button>
          ${!isChordsMode ? `
          <button class="chip-btn" style="width:100%;padding:12px;border-color:#573d26;background:rgba(243,146,55,0.08);"
                  onclick="startEarSurpriseSession()">
            <div style="font-weight:700;color:var(--text);">Zaskocz mnie</div>
            <div style="font-size:11px;color:var(--text-muted);margin-top:2px;">Losowe ćwiczenie dla Ciebie</div>
          </button>` : ''}
        </div>
      </div>`;
    return;
  }

  if (state.mainView === 'changes') {
    ensureChangesProgressionLoaded();
    renderChangesView(stage, insp);
    return;
  }

  // =========================================================================
  // WIDOK: WARSZTAT – SESJE, NOWA SESJA, DODAJ ĆWICZENIE (Zrzuty 5 i 6!)
  // =========================================================================
  if (state.mainView === 'workshop') {
    const sharpLabels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];

    // EKRAN A: < Dodaj ćwiczenie (Zrzut 5)
    if (state.workshopSubView === 'add_exercise') {
      const c = state.wsCreator;
      const patternOptions = [
        { code: 0, name: 'Cała skala' },
        { code: 3, name: 'Grupy po 3 (1-2-3, 2-3-4...)' },
        { code: 4, name: 'Grupy po 4 (1-2-3-4, 2-3-4-5...)' },
        { code: 5, name: 'Skoki tercjowe (1-3, 2-4...)' },
        { code: 6, name: 'Skoki kwartowe (1-4, 2-5...)' },
        { code: 7, name: 'Skoki kwintowe (1-5, 2-6...)' },
        { code: 8, name: 'Skoki sekstowe (1-6, 2-7...)' },
        { code: 9, name: 'Skoki oktawowe (1-8, 2-9...)' }
      ];

      stage.innerHTML = `
        <div class="fretboard-card" style="padding:22px;gap:18px;">
          <div style="display:flex;align-items:center;gap:10px;">
            <button class="chip-btn" style="padding:6px 12px;" onclick="state.workshopSubView='new_session';renderStageAndInspector()">‹ Wróć</button>
            <h2 style="font-size:22px;font-weight:800;">Dodaj ćwiczenie</h2>
          </div>

          <div>
            <div class="section-label">TONACJA</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;">
              ${sharpLabels.map((lbl, pc) => `
                <button class="chip-btn ${c.rootPc===pc?'active':''}" style="padding:10px;"
                        onclick="state.wsCreator.rootPc=${pc};renderStageAndInspector()">${lbl}</button>
              `).join('')}
            </div>
          </div>

          <div>
            <div class="section-label">SKALA</div>
            <select onchange="state.wsCreator.scaleId=this.value;renderStageAndInspector()">
              ${state.initData.scales.map(s => `<option value="${s.id}" ${s.id===c.scaleId?'selected':''}>${s.name} (${s.category})</option>`).join('')}
            </select>
          </div>

          <div>
            <div class="section-label">POZYCJA</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;">
              <button class="chip-btn ${c.posSystem===0?'active':''}" onclick="state.wsCreator.posSystem=0;renderStageAndInspector()">Brak</button>
              <button class="chip-btn ${c.posSystem===1?'active':''}" onclick="state.wsCreator.posSystem=1;renderStageAndInspector()">3NPS</button>
              <button class="chip-btn ${c.posSystem===2?'active':''}" onclick="state.wsCreator.posSystem=2;renderStageAndInspector()">4NPS</button>
              <button class="chip-btn ${c.posSystem===3?'active':''}" onclick="state.wsCreator.posSystem=3;renderStageAndInspector()">CAGED</button>
            </div>
          </div>

          <div>
            <div class="section-label">STRUNA STARTOWA</div>
            <div style="display:grid;grid-template-columns:repeat(7,1fr);gap:6px;">
              <button class="chip-btn ${c.startString===-1?'active':''}" onclick="state.wsCreator.startString=-1;renderStageAndInspector()">Dowolna</button>
              ${['E','A','D','G','B','E'].map((nm, sIdx) => `
                <button class="chip-btn ${c.startString===sIdx?'active':''}" onclick="state.wsCreator.startString=${sIdx};renderStageAndInspector()">${nm}</button>
              `).join('')}
            </div>
          </div>

          <div>
            <div class="section-label">KIERUNEK</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;">
              <button class="chip-btn ${c.direction===0?'active':''}" onclick="state.wsCreator.direction=0;renderStageAndInspector()">Rosnąco</button>
              <button class="chip-btn ${c.direction===1?'active':''}" onclick="state.wsCreator.direction=1;renderStageAndInspector()">Malejąco</button>
              <button class="chip-btn ${c.direction===2?'active':''}" onclick="state.wsCreator.direction=2;renderStageAndInspector()">Rosnąco → Malejąco</button>
              <button class="chip-btn ${c.direction===3?'active':''}" onclick="state.wsCreator.direction=3;renderStageAndInspector()">Malejąco → Rosnąco</button>
            </div>
          </div>

          <div style="display:grid;grid-template-columns:1fr 1fr;gap:18px;">
            <div>
              <div class="section-label">OKTAWY</div>
              <div class="btn-grid-2">
                <button class="chip-btn ${c.octaves===1?'active':''}" onclick="state.wsCreator.octaves=1;renderStageAndInspector()">1 oktawa</button>
                <button class="chip-btn ${c.octaves===2?'active':''}" onclick="state.wsCreator.octaves=2;renderStageAndInspector()">2 oktawy</button>
              </div>
            </div>
            <div>
              <div class="section-label">WZORZEC</div>
              <select onchange="state.wsCreator.pattern=parseInt(this.value,10);renderStageAndInspector()">
                ${patternOptions.map(p => `<option value="${p.code}" ${p.code===c.pattern?'selected':''}>${p.name}</option>`).join('')}
              </select>
            </div>
          </div>

          <button class="action-btn" style="width:100%;padding:14px;font-size:15px;margin-top:6px;"
                  onclick="confirmAddWorkshopExercise()">Dodaj ćwiczenie</button>
        </div>`;

      insp.innerHTML = `
        <div class="section-label">PODGLĄD GENERATORA</div>
        <div style="font-size:13px;color:var(--text-muted);line-height:1.5;">
          Ćwiczenie zostanie wygenerowane przez silnik <b>C++20 (WorkshopGenerator)</b>, dodane do bieżącej sesji oraz listy <b>Twoje ćwiczenia</b>, skąd możesz je od razu uruchomić na gryfie i w tabulaturze.
        </div>`;
      return;
    }

    // EKRAN B: < Nowa sesja (Zrzut 6)
    if (state.workshopSubView === 'new_session') {
      const filteredEx = state.customWorkshopExercises.filter(ex =>
        (state.workshopFilterCategory === 'all' || ex.category === state.workshopFilterCategory) &&
        (state.workshopFilterLevel === 'all' || ex.level === state.workshopFilterLevel)
      );

      stage.innerHTML = `
        <div class="fretboard-card" style="padding:22px;gap:18px;">
          <div style="display:flex;align-items:center;gap:10px;">
            <button class="chip-btn" style="padding:6px 12px;" onclick="state.workshopSubView='sessions';renderStageAndInspector()">‹ Wróć</button>
            <h2 style="font-size:22px;font-weight:800;">Nowa sesja</h2>
          </div>

          <div>
            <div class="section-label">NAZWA SESJI</div>
            <input type="text" value="${state.newSessionName}" placeholder="np. Poranna rozgrzewka"
                   style="width:100%;background:var(--bg-card);color:var(--text);border:1px solid var(--border);border-radius:8px;padding:10px 12px;font-size:14px;"
                   oninput="state.newSessionName=this.value">
          </div>

          <div>
            <h3 style="font-size:16px;margin-bottom:8px;">Ćwiczenia w tej sesji</h3>
            ${state.newSessionItems.length === 0 ? `
              <div style="padding:16px;background:#16120f;border:1px dashed #3b3129;border-radius:8px;color:var(--text-muted);font-size:13px;margin-bottom:10px;">
                Brak ćwiczeń. Dodaj skalę albo jedno ze swoich ćwiczeń poniżej.
              </div>
            ` : `
              <div style="display:flex;flex-direction:column;gap:6px;margin-bottom:10px;">
                ${state.newSessionItems.map((it, idx) => `
                  <div class="list-row">
                    <strong>${idx+1}. ${it.title}</strong>
                    <button class="chip-btn" style="padding:3px 8px;font-size:11px;" onclick="state.newSessionItems.splice(${idx},1);renderStageAndInspector()">✕ Usuń</button>
                  </div>
                `).join('')}
              </div>
            `}
            <div class="btn-grid-2">
              <button class="chip-btn" style="padding:11px;" onclick="state.workshopSubView='add_exercise';renderStageAndInspector()">+ Dodaj skalę (Kreator)</button>
              <button class="chip-btn" style="padding:11px;" onclick="switchMainView('practice');switchPracticeSubtab('exercise')">+ Stwórz własne w Edytorze TAB</button>
            </div>
          </div>

          <div style="margin-top:8px;">
            <h3 style="font-size:16px;margin-bottom:10px;">Twoje ćwiczenia</h3>
            <div class="btn-grid-2" style="margin-bottom:12px;">
              <select onchange="state.workshopFilterCategory=this.value;renderStageAndInspector()">
                <option value="all" ${state.workshopFilterCategory==='all'?'selected':''}>Wszystkie kategorie</option>
                <option value="Technique Drill" ${state.workshopFilterCategory==='Technique Drill'?'selected':''}>Technique Drill</option>
                <option value="Lick" ${state.workshopFilterCategory==='Lick'?'selected':''}>Lick</option>
                <option value="Riff" ${state.workshopFilterCategory==='Riff'?'selected':''}>Riff</option>
                <option value="Arpeggio" ${state.workshopFilterCategory==='Arpeggio'?'selected':''}>Arpeggio</option>
              </select>
              <select onchange="state.workshopFilterLevel=this.value;renderStageAndInspector()">
                <option value="all" ${state.workshopFilterLevel==='all'?'selected':''}>Wszystkie poziomy</option>
                <option value="Początkujący" ${state.workshopFilterLevel==='Początkujący'?'selected':''}>Początkujący</option>
                <option value="Średniozaawansowany" ${state.workshopFilterLevel==='Średniozaawansowany'?'selected':''}>Średniozaawansowany</option>
                <option value="Zaawansowany" ${state.workshopFilterLevel==='Zaawansowany'?'selected':''}>Zaawansowany</option>
              </select>
            </div>
            <div style="display:flex;flex-direction:column;gap:6px;">
              ${filteredEx.map(ex => `
                <div class="list-row">
                  <div>
                    <div style="font-weight:700;">${ex.title}</div>
                    <div style="font-size:11px;color:var(--text-muted);">${ex.category} · ${ex.level}</div>
                  </div>
                  <div style="display:flex;gap:6px;">
                    <button class="chip-btn" style="padding:5px 10px;font-size:12px;"
                            onclick='state.newSessionItems.push(${JSON.stringify(ex)});renderStageAndInspector()'>+ Do sesji</button>
                    <button class="action-btn" style="padding:5px 10px;font-size:12px;"
                            onclick='launchWorkshopExercise(${JSON.stringify(ex)})'>▶ Graj</button>
                  </div>
                </div>
              `).join('')}
            </div>
          </div>
        </div>`;

      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:16px;border-radius:10px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="font-size:20px;font-weight:800;">${state.newSessionName || 'Nowa sesja'}</div>
          <div style="font-size:12px;color:var(--text-muted);">
            ${state.newSessionItems.length === 0 ? 'Brak ćwiczeń' : `Ćwiczeń: ${state.newSessionItems.length}`}
          </div>
          <button class="action-btn" style="width:100%;" onclick="saveNewWorkshopSession()">Zapisz sesję</button>
          <button class="chip-btn" style="width:100%;" onclick="state.workshopSubView='sessions';renderStageAndInspector()">Anuluj</button>
        </div>`;
      return;
    }

    // EKRAN C: Główna lista Sesji i Ćwiczeń w Warsztacie
    stage.innerHTML = `
      <div class="fretboard-card" style="padding:20px;gap:14px;">
        <div style="display:flex;justify-content:space-between;align-items:center;">
          <div style="display:flex;gap:8px;align-items:center;">
            <h2 style="font-size:22px;font-weight:800;">Warsztat</h2>
            <div class="subtabs" style="margin-left:10px;">
              <button class="subtab-btn active">Sesje</button>
              <button class="subtab-btn" onclick="state.workshopSubView='new_session';renderStageAndInspector()">Ćwiczenia (${state.customWorkshopExercises.length})</button>
            </div>
          </div>
          <div style="display:flex;gap:8px;">
            <button class="chip-btn" style="padding:8px 14px;" onclick="state.workshopSubView='add_exercise';renderStageAndInspector()">+ Dodaj ćwiczenie (Skala)</button>
            <button class="action-btn" onclick="state.newSessionName='';state.newSessionItems=[];state.workshopSubView='new_session';renderStageAndInspector()">+ Nowa sesja</button>
          </div>
        </div>

        <h3 style="font-size:15px;color:var(--text-muted);">Twoje sesje</h3>
        <div style="display:flex;flex-direction:column;gap:8px;">
          ${state.initData.sessions.map((s, idx) => `
            <div class="list-row" style="padding:14px;">
              <div>
                <div style="font-weight:700;font-size:15px;">${s.name}</div>
                <div style="font-size:12px;color:var(--text-muted);margin-top:3px;">Przerwa: ${s.breakBars} takt · Grane dzisiaj</div>
              </div>
              <div style="display:flex;gap:8px;">
                <button class="action-btn" onclick="launchWorkshopExercise(state.customWorkshopExercises[0])">▶ Graj</button>
                <button class="chip-btn" style="padding:6px 10px;" onclick="state.initData.sessions.splice(${idx},1);renderStageAndInspector()">🗑️</button>
              </div>
            </div>
          `).join('')}
        </div>
      </div>`;

    insp.innerHTML = `
      <div style="font-size:15px;font-weight:800;">Przebieg sesji</div>
      <div>
        <div class="section-label">MIĘDZY ĆWICZENIAMI</div>
        <div class="btn-grid-2">
          <button class="chip-btn active">Przerwa i odliczanie</button>
          <button class="chip-btn">Od razu dalej</button>
        </div>
      </div>
      <div>
        <div class="section-label">WEJŚCIE AUDIO</div>
        <select onchange="onAudioInputChanged(this.value)">
          ${state.audioInputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioInputId?'selected':''}>${d.label}</option>`).join('')}
        </select>
      </div>
      <div>
        <div class="section-label">ODTWARZANIE (WYJŚCIE AUDIO)</div>
        <select onchange="onAudioOutputChanged(this.value)">
          ${state.audioOutputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioOutputId?'selected':''}>${d.label}</option>`).join('')}
        </select>
      </div>`;
    return;
  }

  // =========================================================================
  // WIDOK: POSTĘPY I USTAWIENIA (Screenshot 6 – zapisują się od razu!)
  // =========================================================================
  if (state.mainView === 'settings') {
    const goalOptions = [5, 10, 15, 20, 30, 45, 60];
    const baseAlteredList = [
      { id: 'G7alt',    label: 'G7alt',    formula: '3 ♭7 ♭9 ♭13 + ♯9' },
      { id: 'G7b9b13',  label: 'G7♭9♭13',  formula: '3 ♭7 ♭9 ♭13' },
      { id: 'G7#9#5',   label: 'G7♯9♯5',   formula: '3 ♯5 ♭7 ♯9' },
      { id: 'G7b9#5',   label: 'G7♭9♯5',   formula: '3 ♯5 ♭7 ♭9' }
    ];
    const extraAlteredList = [
      { id: 'G7b9',     label: 'G7♭9',     formula: '3 5 ♭7 ♭9' },
      { id: 'G7#9',     label: 'G7♯9',     formula: '3 5 ♭7 ♯9' },
      { id: 'G7#11',    label: 'G7♯11',    formula: '3 5 ♭7 ♯11' },
      { id: 'G7b5',     label: 'G7♭5',     formula: '3 ♭5 ♭7' },
      { id: 'G13b9',    label: 'G13♭9',    formula: '3 ♭7 ♭9 13' }
    ];
    const displayedAltered = state.showMoreAlteredTypes
      ? baseAlteredList.concat(extraAlteredList)
      : baseAlteredList;

    const headerTabs = `
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;">
        <div>
          <h2 style="font-size:22px;font-weight:800;">${state.settingsTab === 'ustawienia' ? 'Ustawienia' : 'Postępy'}</h2>
          <div style="font-size:12px;color:var(--text-muted);margin-top:2px;">Zmiany zapisują się od razu.</div>
        </div>
        <div class="subtabs">
          <button class="subtab-btn ${state.settingsTab==='postepy'?'active':''}"
                  onclick="state.settingsTab='postepy';saveSettings();renderStageAndInspector()">Postępy</button>
          <button class="subtab-btn ${state.settingsTab==='ustawienia'?'active':''}"
                  onclick="state.settingsTab='ustawienia';saveSettings();renderStageAndInspector()">Ustawienia</button>
        </div>
      </div>`;

    if (state.settingsTab === 'postepy') {
      const pct = Math.min(100, Math.round((state.todayPracticedMinutes / state.dailyGoalMinutes) * 100));
      stage.innerHTML = `
        <div class="fretboard-card" style="padding:20px;gap:18px;">
          ${headerTabs}
          <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:12px;">
            <div class="chord-box-card" style="align-items:flex-start;padding:16px;">
              <div class="section-label">CEL DZIENNY</div>
              <div style="font-size:26px;font-weight:800;color:var(--accent);">${state.todayPracticedMinutes} / ${state.dailyGoalMinutes} min</div>
              <div style="width:100%;height:8px;background:#2a221c;border-radius:4px;overflow:hidden;margin-top:6px;">
                <div style="width:${pct}%;height:100%;background:var(--green);"></div>
              </div>
            </div>
            <div class="chord-box-card" style="align-items:flex-start;padding:16px;">
              <div class="section-label">PASSA DNI Z RZĘDU</div>
              <div style="font-size:26px;font-weight:800;color:var(--green);">🔥 ${state.streakDays} dni</div>
              <div style="font-size:12px;color:var(--text-muted);">Plan „Pierwszy tydzień”: dzień 3 z 7</div>
            </div>
            <div class="chord-box-card" style="align-items:flex-start;padding:16px;">
              <div class="section-label">ŚREDNIA DOKŁADNOŚĆ GRY</div>
              <div style="font-size:26px;font-weight:800;color:var(--text);">94.5%</div>
              <div style="font-size:12px;color:var(--text-muted);">Intonacja: ±4.2 ct · Rytm: ±18 ms</div>
            </div>
          </div>
        </div>`;
      insp.innerHTML = `
        <div class="section-label">SZYBKIE AKCJE</div>
        <button class="action-btn" onclick="state.todayPracticedMinutes+=5;saveSettings();renderStageAndInspector()">+ Dodaj 5 min sesji testowej</button>`;
      return;
    }

    const skillDesc = state.skillLevel === 'beginner'
      ? 'Uproszczone widoki, szersza tolerancja detekcji (±45 ct)'
      : (state.skillLevel === 'intermediate'
          ? 'Pełne opcje, standardowa detekcja (±30 ct)'
          : 'Tryb studyjny, rygorystyczna ocena intonacji i rytmu (±15 ct)');

    stage.innerHTML = `
      <div class="fretboard-card" style="padding:20px;gap:18px;">
        ${headerTabs}
        <h3 style="font-size:17px;font-weight:800;border-bottom:1px solid var(--border);padding-bottom:8px;">Praktyka</h3>
        <div style="display:grid;grid-template-columns:1fr 1fr;gap:28px;">
          <!-- Lewa kolumna Ustawień -->
          <div style="display:flex;flex-direction:column;gap:18px;">
            <div>
              <div class="section-label">Poziom zaawansowania</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.skillLevel==='beginner'?'active':''}"
                        onclick="state.skillLevel='beginner';saveSettings();renderStageAndInspector()">Początkujący</button>
                <button class="chip-btn ${state.skillLevel==='intermediate'?'active':''}"
                        onclick="state.skillLevel='intermediate';saveSettings();renderStageAndInspector()">Średniozaawansowany</button>
                <button class="chip-btn ${state.skillLevel==='advanced'?'active':''}"
                        onclick="state.skillLevel='advanced';saveSettings();renderStageAndInspector()">Zaawansowany</button>
              </div>
              <div style="font-size:12px;color:var(--text-muted);margin-top:6px;">${skillDesc}</div>
            </div>

            <div>
              <div class="section-label">Grywalizacja</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.gamificationLevel==='full'?'active':''}"
                        onclick="state.gamificationLevel='full';saveSettings();renderStageAndInspector()">Pełna</button>
                <button class="chip-btn ${state.gamificationLevel==='moderate'?'active':''}"
                        onclick="state.gamificationLevel='moderate';saveSettings();renderStageAndInspector()">Umiarkowana</button>
                <button class="chip-btn ${state.gamificationLevel==='minimal'?'active':''}"
                        onclick="state.gamificationLevel='minimal';saveSettings();renderStageAndInspector()">Minimalna</button>
              </div>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:center;">
              <span style="font-size:13px;font-weight:600;">Dźwięki grywalizacji</span>
              <button class="chip-btn ${state.gamificationSounds?'active':''}" style="min-width:80px;"
                      onclick="state.gamificationSounds=!state.gamificationSounds;saveSettings();renderStageAndInspector()">
                ${state.gamificationSounds ? 'Wł.' : 'Wył.'}
              </button>
            </div>

            <div>
              <div class="section-label">Liczba progów na gryfie</div>
              <div class="btn-grid-2">
                <button class="chip-btn ${state.maxDisplayedFret===22?'active':''}"
                        onclick="setMaxFrets(22)">22 progi</button>
                <button class="chip-btn ${state.maxDisplayedFret===24?'active':''}"
                        onclick="setMaxFrets(24)">24 progi</button>
              </div>
            </div>
          </div>

          <!-- Prawa kolumna Ustawień -->
          <div style="display:flex;flex-direction:column;gap:18px;">
            <div>
              <div class="section-label">Cel dzienny</div>
              <div style="display:flex;gap:6px;flex-wrap:wrap;">
                ${goalOptions.map(m => `
                  <button class="chip-btn ${state.dailyGoalMinutes===m?'active':''}" style="min-width:68px;"
                          onclick="state.dailyGoalMinutes=${m};saveSettings();renderStageAndInspector()">${m} min</button>
                `).join('')}
              </div>
            </div>

            <div>
              <div class="section-label">Dominanty rozwiązujące się</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.resolvingDominants==='inside'?'active':''}"
                        onclick="state.resolvingDominants='inside';saveSettings();renderStageAndInspector()">Wewnątrz</button>
                <button class="chip-btn ${state.resolvingDominants==='outside'?'active':''}"
                        onclick="state.resolvingDominants='outside';saveSettings();renderStageAndInspector()">Na zewnątrz</button>
                <button class="chip-btn ${state.resolvingDominants==='custom'?'active':''}"
                        onclick="state.resolvingDominants='custom';saveSettings();renderStageAndInspector()">Twoje wybory</button>
              </div>
              <div style="font-size:11px;color:var(--text-muted);margin-top:6px;line-height:1.4;">
                Miksolidyjska do akordu durowego, frygijska dominantowa do molowego; dominanty alterowane według ustawienia poniżej. Zapisane rutyny zachowują własne ustawienie.
              </div>
            </div>

            <div>
              <div class="section-label">Dominanty alterowane (♯5, ♭13)</div>
              <div style="display:flex;gap:6px;flex-wrap:wrap;">
                ${[
                  {id:'auto', name:'Auto'},
                  {id:'mixolydian_b6', name:'Miksolidyjska ♭6'},
                  {id:'whole_tone', name:'Całotonowa'},
                  {id:'altered', name:'Alterowana'},
                  {id:'messiaen_3', name:'Messiaen 3'}
                ].map(opt => `
                  <button class="chip-btn ${state.alteredDominantScale===opt.id?'active':''}" style="padding:7px 11px;"
                          onclick="state.alteredDominantScale='${opt.id}';saveSettings();renderStageAndInspector()">${opt.name}</button>
                `).join('')}
              </div>
              <div style="font-size:11px;color:var(--text-muted);margin-top:6px;line-height:1.4;">
                Jak brzmią 7♯5, 9♯5 i 7♭13 w ćwiczeniu skal akordu i w Aranżacji. Auto wybiera dla każdego akordu; skala, która nie pasuje do akordu, jest pomijana.
              </div>
            </div>

            <div>
              <div class="section-label">Brzmienie akordów alterowanych</div>
              <div style="display:flex;align-items:center;gap:8px;margin-bottom:10px;font-size:12px;">
                <span style="color:var(--text-muted);">Wszystkie:</span>
                <button class="chip-btn" style="padding:5px 12px;" onclick="setAllAlteredIntensity('light')">Lekko</button>
                <button class="chip-btn" style="padding:5px 12px;" onclick="setAllAlteredIntensity('strong')">Mocno</button>
              </div>
              <div style="display:flex;flex-direction:column;gap:6px;">
                ${displayedAltered.map(item => {
                  const cur = state.alteredVoicingIntensity[item.id] || 'strong';
                  return `
                    <div class="list-row" style="cursor:default;">
                      <div>
                        <div style="font-weight:700;font-size:13px;">${item.label}</div>
                        <div style="font-size:11px;color:var(--text-muted);">${item.formula}</div>
                      </div>
                      <div style="display:flex;gap:4px;">
                        <button class="chip-btn ${cur==='light'?'active':''}" style="padding:5px 10px;font-size:12px;"
                                onclick="setAlteredIntensity('${item.id}','light')">Lekko</button>
                        <button class="chip-btn ${cur==='strong'?'active':''}" style="padding:5px 10px;font-size:12px;"
                                onclick="setAlteredIntensity('${item.id}','strong')">Mocno</button>
                      </div>
                    </div>`;
                }).join('')}
              </div>
              <button class="chip-btn" style="width:100%;margin-top:8px;"
                      onclick="state.showMoreAlteredTypes=!state.showMoreAlteredTypes;renderStageAndInspector()">
                ${state.showMoreAlteredTypes ? '− Ukryj dodatkowe typy' : '+ Więcej typów (5)'}
              </button>
              <div style="font-size:11px;color:var(--text-muted);margin-top:6px;">
                Najwięcej koloru, jakiego zespół używa dla każdego typu akordu. W środku utworu gra lżej.
              </div>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:flex-start;gap:12px;border-top:1px solid var(--border);padding-top:12px;">
              <div>
                <div style="font-size:13px;font-weight:700;">Ruchome głosy</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;line-height:1.35;">
                  Na długich akordach zespół prowadzi głos wewnętrzny, a w długich akordach alterowanych przesuwa nonę. Wyłączone: jedno brzmienie na akord.
                </div>
              </div>
              <button class="chip-btn ${state.movingVoices?'active':''}" style="min-width:74px;"
                      onclick="state.movingVoices=!state.movingVoices;saveSettings();renderStageAndInspector()">
                ${state.movingVoices ? 'Wł.' : 'Wył.'}
              </button>
            </div>
          </div>
        </div>
      </div>`;

    insp.innerHTML = `
      <div>
        <div class="section-label">DOMYŚLNY STRÓJ GITARY</div>
        <select onchange="selectPresetTuning(this.value)">
          ${state.initData.tunings.map(t => `<option value="${t.id}" ${t.id===state.tuningId?'selected':''}>${t.name}</option>`).join('')}
          <option value="custom" ${state.tuningId==='custom'?'selected':''}>Własny strój (niestandardowy)</option>
        </select>
      </div>
      <div>
        <div class="section-label">DOMYŚLNE ZNAKI CHROMATYCZNE (BEMOLE / KRZYŻYKI)</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.accMode===1?'active':''}" onclick="setAccMode(1);saveSettings()">Bemole (♭)</button>
          <button class="chip-btn ${state.accMode===2?'active':''}" onclick="setAccMode(2);saveSettings()">Krzyżyki (♯)</button>
        </div>
      </div>
      <div>
        <div class="section-label">KALIBRACJA STROIKA A4 (Hz)</div>
        <input type="number" value="${state.a4ReferenceHz}" min="430" max="450" step="0.5"
               onchange="state.a4ReferenceHz=parseFloat(this.value);saveSettings()">
      </div>`;
    return;
  }

  // =========================================================================
  // ZAKŁADKI WIDOKU „PRAKTYKA”
  // =========================================================================
  if (state.subtab === 'scale') {
    const rootNoteName = state.fretboardData.scaleNotes[0] || 'C';
    const degList = state.fretboardData.scaleDegrees || state.fretboardData.scaleNotes.map((n, idx) => ({
      note: n,
      pc: noteStringToPc(n, state.rootPc),
      deg: idx + 1,
      interval: String(idx + 1),
      intervalName: '',
      functionName: `Stopień ${idx + 1}.`,
      triad: '',
      roman: ''
    }));
    const rootDeg = degList[0] || { note: rootNoteName, pc: state.rootPc, deg: 1, triad: rootNoteName };
    const rootDegPc = (rootDeg.pc !== undefined) ? (((rootDeg.pc % 12) + 12) % 12) : noteStringToPc(rootNoteName, state.rootPc);
    const rootDegOn = isScaleDegreeHighlightEnabled(rootDegPc);

    stage.innerHTML = `
      <div class="fretboard-card">
        <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;margin-bottom:4px;">
          <div>
            <div class="section-label" style="margin:0;">
              Dźwięki i funkcje stopni skali (${state.fretboardData.scaleFullName}) — kliknij i przytrzymaj (lub przeciągnij), aby usłyszeć
            </div>
            <div id="activePlayedFunctionBadge" style="font-size:12px;color:var(--text);margin-top:4px;"></div>
          </div>
          <div style="display:flex;gap:6px;align-items:center;flex-wrap:wrap;">
            <span style="font-size:11px;color:var(--text-muted);font-weight:700;">Podświetlaj podczas gry:</span>
            <button class="chip-btn ${state.highlightPlayedOnGuitar?'active':''}" style="padding:3px 9px;font-size:11px;"
                    onclick="toggleHighlightPlayedOnGuitar()"
                    title="Włącz lub wyłącz podświetlanie wszystkich pozycji granego dźwięku na gryfie gitary">
              🎸 Na gitarze: ${state.highlightPlayedOnGuitar ? 'Wł.' : 'Wył.'}
            </button>
            <button class="chip-btn ${state.syncGuitarToKeyboard?'active':''}" style="padding:3px 9px;font-size:11px;"
                    onclick="toggleSyncGuitarToKeyboard()"
                    title="Włącz lub wyłącz podświetlanie skali i granego dźwięku na klawiszach pianina">
              🎹 Na klawiszach: ${state.syncGuitarToKeyboard ? 'Wł.' : 'Wył.'}
            </button>
            <button class="chip-btn ${state.highlightScaleCardsOnPlay?'active':''}" style="padding:3px 9px;font-size:11px;"
                    onclick="toggleHighlightScaleCardsOnPlay()"
                    title="Włącz lub wyłącz podświetlanie kafelków stopni i funkcji skali podczas gry">
              🎯 Kafelki funkcji: ${state.highlightScaleCardsOnPlay ? 'Wł.' : 'Wył.'}
            </button>
            <button class="chip-btn" style="padding:3px 8px;font-size:11px;"
                    onclick="setAllScaleDegreesHighlight(true)" title="Włącz podświetlanie wszystkich stopni skali">Wszystkie Wł.</button>
            <button class="chip-btn" style="padding:3px 8px;font-size:11px;"
                    onclick="setAllScaleDegreesHighlight(false)" title="Wyłącz podświetlanie wszystkich stopni skali">Wszystkie Wył.</button>
          </div>
        </div>
        <div style="display:grid;grid-template-columns:repeat(auto-fit, minmax(162px, 1fr));gap:10px;">
          ${degList.map((d, idx) => {
            const degPc = (d.pc !== undefined) ? (((d.pc % 12) + 12) % 12) : noteStringToPc(d.note, state.rootPc);
            const degOn = isScaleDegreeHighlightEnabled(degPc);
            return `
            <div class="chord-box-card scale-func-card"
                 data-pc="${degPc}" data-deg="${d.deg}" data-octave="0"
                 style="align-items:flex-start;padding:10px 12px;gap:4px;cursor:pointer;user-select:none;opacity:${degOn ? '1' : '0.45'};"
                 onmousedown="startHoldScaleNoteChip(event, ${degPc}, false)"
                 onmouseenter="onScaleNoteChipMouseEnter(event, ${degPc}, false)">
              <div style="display:flex;justify-content:space-between;align-items:baseline;width:100%;">
                <span style="font-size:20px;font-weight:800;color:${idx === 0 ? 'var(--accent)' : 'var(--text)'};">${d.note}</span>
                <span style="font-size:11.5px;font-weight:700;color:var(--accent);background:rgba(243,146,55,0.14);padding:2px 7px;border-radius:5px;">
                  Stopień ${d.deg} · ${d.interval}
                </span>
              </div>
              <div style="font-size:12.5px;font-weight:700;color:#f5f2eb;margin-top:2px;">${d.functionName}</div>
              <div style="display:flex;justify-content:space-between;align-items:center;width:100%;gap:6px;margin-top:2px;">
                <span style="font-size:11px;color:var(--text-muted);">${d.intervalName}${d.triad ? ` · Akord: <b style="color:#cbd5e1">${d.triad}</b>` : ''}</span>
                <button class="chip-btn ${degOn?'active':''}" style="padding:2px 7px;font-size:10px;flex-shrink:0;"
                        onmousedown="event.stopPropagation()"
                        onclick="event.stopPropagation();toggleScaleDegreeHighlight(${d.deg})"
                        title="Włącz lub wyłącz podświetlanie tego stopnia i funkcji na gitarze i klawiszach">
                  ${degOn ? '💡 Wł.' : '🚫 Wył.'}
                </button>
              </div>
            </div>`;
          }).join('')}
          <div class="chord-box-card scale-func-card"
               data-pc="${rootDegPc}" data-deg="1" data-octave="1"
               style="align-items:flex-start;padding:10px 12px;gap:4px;cursor:pointer;user-select:none;border-style:dashed;opacity:${rootDegOn ? '1' : '0.45'};"
               onmousedown="startHoldScaleNoteChip(event, ${rootDegPc}, true)"
               onmouseenter="onScaleNoteChipMouseEnter(event, ${rootDegPc}, true)">
            <div style="display:flex;justify-content:space-between;align-items:baseline;width:100%;">
              <span style="font-size:20px;font-weight:800;color:var(--accent);">${rootNoteName}</span>
              <span style="font-size:11.5px;font-weight:700;color:var(--accent);background:rgba(243,146,55,0.14);padding:2px 7px;border-radius:5px;">
                Oktawa · 8va
              </span>
            </div>
            <div style="font-size:12.5px;font-weight:700;color:#f5f2eb;margin-top:2px;">Tonika (I · wyższa oktawa)</div>
            <div style="display:flex;justify-content:space-between;align-items:center;width:100%;gap:6px;margin-top:2px;">
              <span style="font-size:11px;color:var(--text-muted);">Oktawa czysta (P8)${rootDeg.triad ? ` · Akord: <b style="color:#cbd5e1">${rootDeg.triad}</b>` : ''}</span>
              <button class="chip-btn ${rootDegOn?'active':''}" style="padding:2px 7px;font-size:10px;flex-shrink:0;"
                      onmousedown="event.stopPropagation()"
                      onclick="event.stopPropagation();toggleScaleDegreeHighlight(1)"
                      title="Włącz lub wyłącz podświetlanie toniki na gitarze i klawiszach">
                ${rootDegOn ? '💡 Wł.' : '🚫 Wył.'}
              </button>
            </div>
          </div>
        </div>
      </div>`;
    updateScaleFunctionCardsHighlight();
    insp.innerHTML = `
      <div>
        <div class="section-label">TONACJA</div>
        ${buildRootButtonsHtml('setRootPc')}
      </div>
      <div>
        <div class="section-label">SKALA</div>
        <select onchange="state.scaleId=this.value;state.disabledScaleDegrees=[];state.selectedScaleNotePc=state.rootPc;renderAll();">
          ${state.initData.scales.map(s => `<option value="${s.id}" ${s.id===state.scaleId?'selected':''}>${s.name}</option>`).join('')}
        </select>
      </div>
      <div>
        <div class="section-label">POZYCJA</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.posSystem===0?'active':''}" onclick="state.posSystem=0;renderAll()">Brak (Cały gryf)</button>
          <button class="chip-btn ${state.posSystem===1?'active':''}" onclick="state.posSystem=1;renderAll()">3NPS</button>
          <button class="chip-btn ${state.posSystem===2?'active':''}" onclick="state.posSystem=2;renderAll()">4NPS</button>
          <button class="chip-btn ${state.posSystem===3?'active':''}" onclick="state.posSystem=3;renderAll()">CAGED</button>
        </div>
      </div>
      ${state.posSystem > 0 ? `
      <div>
        <div class="section-label">NUMER POZYCJI</div>
        <div class="btn-grid-6">
          ${[0,1,2,3,4,5].map(idx => `<button class="chip-btn ${state.posIdx===idx?'active':''}" onclick="state.posIdx=${idx};renderAll()">#${idx+1}</button>`).join('')}
        </div>
      </div>` : ''}
      <div>
        <div class="section-label">ETYKIETY</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.labelType==='notes'?'active':''}" onclick="setFretboardLabelType('notes')">Nuty</button>
          <button class="chip-btn ${state.labelType==='intervals'?'active':''}" onclick="setFretboardLabelType('intervals')">Interwały</button>
        </div>
      </div>
      <div>
        <div class="section-label">STRÓJ GITARY</div>
        <select onchange="selectPresetTuning(this.value)">
          ${state.initData.tunings.map(t => `<option value="${t.id}" ${t.id===state.tuningId?'selected':''}>${t.name}</option>`).join('')}
          <option value="custom" ${state.tuningId==='custom'?'selected':''}>Własny strój (niestandardowy)</option>
        </select>
        <button class="chip-btn" style="width:100%;margin-top:6px;border-color:var(--accent);color:var(--accent);"
                onclick="toggleFretboardSettingsModal()">
          ⚙ Przestrój pojedyncze struny i ustawienia gryfu
        </button>
      </div>`;

  } else if (state.subtab === 'chord') {
    // =======================================================================
    // ZAKŁADKA: AKORD · ODKRYWANIE (Ostrość Łagodne/Ostre, filtry grup, trójdźwięki 3-strunowe, filtr skali!)
    // =======================================================================
    const customParam = state.customStringMidis.join(',');
    const reqGroup = state.chordGroupFilter || (state.chordFilterByScale ? 'all' : 'basic');
    const effAllTypes = state.chordFilterByScale
      ? !state.chordScaleExtraTypeFilter
      : !!state.allChordTypes;
    const effSharpness = state.chordSharpness || 'mild';
    const url = `/api/voicings?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${state.chordId}&group=${encodeURIComponent(reqGroup)}&sharpness=${encodeURIComponent(effSharpness)}&accMode=${state.accMode}&allTypes=${effAllTypes?1:0}&filterByScale=${state.chordFilterByScale?1:0}&scaleRootPc=${state.chordScaleRootPc}&scaleId=${state.chordScaleId}`;
    const data = await fetchJson(url);

    if (data.resolvedRootPc !== undefined) state.rootPc = data.resolvedRootPc;
    if (data.resolvedChordId) state.chordId = data.resolvedChordId;
    state.chordLastMatchingIds = data.matchingChordIds || [];
    state.chordVoicingsList = data.voicings || [];
    if (state.selectedChordVoicingIdx >= state.chordVoicingsList.length) {
      state.selectedChordVoicingIdx = 0;
    }

    const matchingRootsSet = new Set(data.matchingRootPcs || [0,1,2,3,4,5,6,7,8,9,10,11]);
    const matchingChordsSet = new Set(state.chordLastMatchingIds);
    const rootLabels = getRootLabels();

    stage.innerHTML = `
      <div class="chord-cards-grid">
        ${state.chordVoicingsList.map((v, idx) => {
          const svg = buildChordSvgDiagram(v.frets, v.tapped);
          const subLine = v.usesTapping
            ? 'T = tapping ręką prawą'
            : (v.isSharp ? 'Ostre · szerokie rozciągnięcie' : 'Łagodne · chwyt klasyczny');
          const titleLine = v.openNote ? `${v.symbol} · ${v.openNote}` : v.symbol;
          return `
            <div class="chord-box-card ${idx===state.selectedChordVoicingIdx?'selected':''}"
                 onclick='selectAndPlayChordCard(this, ${idx}, ${JSON.stringify(v.freqs)})'>
              ${svg}
              <div class="chord-box-sub">${subLine}</div>
              <div class="chord-box-title">${titleLine}</div>
            </div>`;
        }).join('')}
      </div>
      ${state.chordVoicingsList.length === 0 ? `<div style="padding:20px;text-align:center;color:var(--text-muted);border:1px dashed var(--border);border-radius:10px;">Brak pasujących chwytów dla wybranych filtrów.</div>` : ''}`;

    const availableChordsAll = state.chordFilterByScale
      ? state.initData.chords.filter(c => matchingChordsSet.has(c.id))
      : state.initData.chords;
    // Podział listy rodzajów akordów na spójne grupy
    const chordGroupTabs = [
      { id: 'basic', label: 'Podstawowe' },
      { id: 'triad', label: 'Trójdźwięki' },
      { id: 'seventh', label: 'Septymowe' },
      { id: 'extended', label: 'Rozszerzone' },
      { id: 'altered', label: 'Alterowane' },
      { id: 'all', label: 'Wszystkie' }
    ];
    const chordsInGroup = (g) => availableChordsAll.filter(c => chordInGroup(c, g));
    let activeChordGroup = reqGroup;
    let availableChords = chordsInGroup(activeChordGroup);
    if (availableChords.length === 0) {
      activeChordGroup = 'all';
      availableChords = availableChordsAll;
    }
    const showSingleChordSelect = state.chordFilterByScale
      ? !!state.chordScaleExtraTypeFilter
      : !state.allChordTypes;
    const chordOptHtml = (c) => `<option value="${c.id}" ${c.id===state.chordId?'selected':''}>${c.id} (${c.name})</option>`;
    const chordSelectHtml = (activeChordGroup === 'all')
      ? ['Trójdźwięki', 'Dwudźwięki', 'Septymowe', 'Rozszerzone', 'Alterowane'].map(cat => {
          const items = availableChords.filter(c => c.category === cat);
          return items.length ? `<optgroup label="${cat}">${items.map(chordOptHtml).join('')}</optgroup>` : '';
        }).join('')
      : availableChords.map(chordOptHtml).join('');

    insp.innerHTML = `
      <div>
        <div class="section-label">OSTROŚĆ</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${effSharpness==='mild'?'active':''}"
                  onclick="state.chordSharpness='mild';state.selectedChordVoicingIdx=0;renderStageAndInspector()">Łagodne</button>
          <button class="chip-btn ${effSharpness==='sharp'?'active':''}"
                  onclick="state.chordSharpness='sharp';state.selectedChordVoicingIdx=0;renderStageAndInspector()">Ostre</button>
        </div>
      </div>
      <div>
        <div class="section-label">SKALA (FILTRUJ AKORDY PASUJĄCE DO SKALI)</div>
        <div class="btn-grid-2" style="margin-bottom:8px;">
          <button class="chip-btn ${!state.chordFilterByScale?'active':''}"
                  onclick="state.chordFilterByScale=false;state.chordShowScaleOnFretboard=false;state.selectedChordVoicingIdx=0;renderAll()">Brak (wszystkie)</button>
          <button class="chip-btn ${state.chordFilterByScale?'active':''}"
                  onclick="state.chordFilterByScale=true;state.chordScaleRootPc=state.rootPc;state.chordGroupFilter='all';state.chordScaleExtraTypeFilter=false;state.selectedChordVoicingIdx=0;renderAll()">Wg skali</button>
        </div>
        ${state.chordFilterByScale ? `
          <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:10px;display:flex;flex-direction:column;gap:8px;">
            <div class="section-label" style="margin:0;">TONIKA SKALI</div>
            <div class="btn-grid-6">
              ${rootLabels.map((lbl, pc) => `
                <button class="chip-btn ${state.chordScaleRootPc===pc?'active':''}" style="padding:5px 2px;font-size:11.5px;"
                        onclick="state.chordScaleRootPc=${pc};state.selectedChordVoicingIdx=0;renderAll()">${lbl}</button>
              `).join('')}
            </div>
            <div class="section-label" style="margin:0;">WYBIERZ SKALĘ</div>
            <select onchange="state.chordScaleId=this.value;state.selectedChordVoicingIdx=0;renderAll()">
              ${state.initData.scales.map(s => `<option value="${s.id}" ${s.id===state.chordScaleId?'selected':''}>${s.name}</option>`).join('')}
            </select>
            <button class="chip-btn ${state.chordShowScaleOnFretboard?'active':''}" style="width:100%;font-size:12px;"
                    onclick="state.chordShowScaleOnFretboard=!state.chordShowScaleOnFretboard;renderFretboardToolbarForActiveTab();drawFretboard()">
              🎵 Pokaż dźwięki skali w tle na gryfie: ${state.chordShowScaleOnFretboard ? 'WŁ.' : 'WYŁ.'}
            </button>
          </div>
        ` : ''}
      </div>
      ${!state.chordFilterByScale ? `
      <div>
        <div class="section-label">WSZYSTKIE TYPY</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${!state.allChordTypes?'active':''}" onclick="state.allChordTypes=false;state.selectedChordVoicingIdx=0;renderStageAndInspector()">Wybrany rodzaj</button>
          <button class="chip-btn ${state.allChordTypes?'active':''}" onclick="state.allChordTypes=true;state.selectedChordVoicingIdx=0;renderStageAndInspector()">Wszystkie pasujące</button>
        </div>
      </div>` : ''}
      <div>
        <div class="section-label">${state.chordFilterByScale ? 'PRYMA AKORDU (TYLKO ZE SKALI)' : 'TONACJA (PRYMA AKORDU)'}</div>
        <div class="btn-grid-6">
          ${rootLabels.map((lbl, pc) => {
            if (state.chordFilterByScale && !matchingRootsSet.has(pc)) return '';
            return `<button class="chip-btn ${state.rootPc===pc?'active':''}" onclick="state.rootPc=${pc};state.selectedChordVoicingIdx=0;renderAll()">${lbl}</button>`;
          }).join('')}
        </div>
      </div>
      <div>
        <div class="section-label">${state.chordFilterByScale ? `KATEGORIA AKORDU (PASUJĄCE: ${availableChordsAll.length})` : 'RODZAJ AKORDU'}</div>
        <div style="display:flex;flex-wrap:wrap;gap:5px;margin-bottom:8px;">
          ${chordGroupTabs.map(g => {
            const n = chordsInGroup(g.id).length;
            return `<button class="chip-btn ${activeChordGroup===g.id?'active':''}" style="padding:5px 9px;font-size:11.5px;${n===0?'opacity:0.35;':''}"
                            ${n===0?'disabled':''} onclick="setChordGroupFilter('${g.id}')">${g.label} <span style="opacity:0.6;">${n}</span></button>`;
          }).join('')}
        </div>
        ${state.chordFilterByScale ? `
          <button class="chip-btn ${state.chordScaleExtraTypeFilter?'active':''}" style="width:100%;font-size:11.5px;margin-bottom:${showSingleChordSelect?'8px':'0'};"
                  onclick="state.chordScaleExtraTypeFilter=!state.chordScaleExtraTypeFilter;state.selectedChordVoicingIdx=0;renderStageAndInspector()">
            🎚 Dodatkowy filtr konkretnego akordu: ${state.chordScaleExtraTypeFilter ? 'WŁ.' : 'WYŁ.'}
          </button>
        ` : ''}
        ${showSingleChordSelect ? `
          <select onchange="state.chordId=this.value;state.selectedChordVoicingIdx=0;renderStageAndInspector();">
            ${chordSelectHtml}
          </select>
        ` : ''}
      </div>`;

    renderFretboardToolbarForActiveTab();
    drawFretboard();

  } else if (state.subtab === 'harmonics') {
    // =======================================================================
    // ZAKŁADKA: FLAŻOLETY (Screenshot 2 – romby na gryfie + filtry trudności i interwałów!)
    // =======================================================================
    const customParam = state.customStringMidis.join(',');
    const data = await fetchJson(`/api/harmonics?tuning=${state.tuningId}&customMidis=${customParam}&stringIdx=${state.harmonicString}&accMode=${state.accMode}`);
    state.harmonicsData = data.harmonics || [];

    let filtered = state.harmonicsData.filter(h =>
      state.harmonicDiffEnabled[h.diff] &&
      state.harmonicIntervalsEnabled.has(h.interval) &&
      h.exactFret >= state.harmonicMinFret &&
      h.exactFret <= state.harmonicMaxFret + 0.15
    );
    if (state.harmonicSort === 'string') {
      filtered.sort((a, b) => (a.stringIdx - b.stringIdx) || (a.exactFret - b.exactFret));
    } else {
      filtered.sort((a, b) => (a.midi - b.midi) || (a.exactFret - b.exactFret));
    }

    stage.innerHTML = `
      <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;font-size:12px;">
        <div style="display:flex;gap:14px;align-items:center;">
          <span style="color:var(--text-muted);">Trudność (obwód rombu):</span>
          <span style="cursor:pointer;opacity:${state.harmonicDiffEnabled[0]?1:0.35}" onclick="toggleHarmDiff(0)"><b style="color:#10b981">◆</b> Łatwy</span>
          <span style="cursor:pointer;opacity:${state.harmonicDiffEnabled[1]?1:0.35}" onclick="toggleHarmDiff(1)"><b style="color:#eab308">◆</b> Średni</span>
          <span style="cursor:pointer;opacity:${state.harmonicDiffEnabled[2]?1:0.35}" onclick="toggleHarmDiff(2)"><b style="color:#f97316">◆</b> Trudny</span>
          <span style="cursor:pointer;opacity:${state.harmonicDiffEnabled[3]?1:0.35}" onclick="toggleHarmDiff(3)"><b style="color:#ef4444">◆</b> Bardzo trudny</span>
        </div>
        <div style="display:flex;gap:6px;align-items:center;">
          <span style="color:var(--text-muted);">Sortuj:</span>
          <button class="chip-btn ${state.harmonicSort==='pitch'?'active':''}" style="padding:4px 10px;font-size:12px;" onclick="state.harmonicSort='pitch';renderStageAndInspector()">Wg dźwięku</button>
          <button class="chip-btn ${state.harmonicSort==='string'?'active':''}" style="padding:4px 10px;font-size:12px;" onclick="state.harmonicSort='string';renderStageAndInspector()">Wg struny</button>
        </div>
      </div>
      <div class="harmonics-grid">
        ${filtered.map(h => `
          <div class="list-row" onmousedown="startFretHoldHarmonic(event, ${h.freq})">
            <div>
              <span style="color:${h.color};margin-right:6px;">◆</span>
              <strong>${h.pitch}</strong>
              <span style="color:var(--text-muted);margin-left:6px;">${h.location} (struna ${h.stringName})</span>
            </div>
            <span style="color:var(--text-muted);">▶</span>
          </div>
        `).join('')}
      </div>`;

    const allHarmIvs = ['1','2','3','#4','5','b6','b7','7'];
    insp.innerHTML = `
      <div>
        <div class="section-label">ZAKRES</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.harmonicString===-1?'active':''}" onclick="state.harmonicString=-1;renderAll()">Cały gryf</button>
          <button class="chip-btn ${state.harmonicString>=0?'active':''}" onclick="state.harmonicString=0;renderAll()">Na strunę</button>
        </div>
      </div>
      ${state.harmonicString >= 0 ? `
      <div>
        <div class="section-label">STRUNA</div>
        <div class="btn-grid-6">
          ${state.customStringMidis.map((midi, s) => {
            const nm = getRootLabels()[((midi % 12) + 12) % 12];
            return `<button class="chip-btn ${state.harmonicString===s?'active':''}" onclick="state.harmonicString=${s};renderAll()">${nm}</button>`;
          }).join('')}
        </div>
      </div>` : ''}
      <div>
        <div class="section-label">ETYKIETY NA GRYFIE</div>
        <div class="btn-grid-3">
          <button class="chip-btn ${state.harmonicLabelType==='none'?'active':''}" onclick="state.harmonicLabelType='none';drawFretboard();renderStageAndInspector()">Brak</button>
          <button class="chip-btn ${state.harmonicLabelType==='intervals'?'active':''}" onclick="state.harmonicLabelType='intervals';drawFretboard();renderStageAndInspector()">Interwały</button>
          <button class="chip-btn ${state.harmonicLabelType==='notes'?'active':''}" onclick="state.harmonicLabelType='notes';drawFretboard();renderStageAndInspector()">Nuty</button>
        </div>
      </div>
      <div>
        <div class="section-label">INTERWAŁY (nad pustą struną)</div>
        <div style="display:flex;gap:5px;flex-wrap:wrap;margin-bottom:8px;">
          ${allHarmIvs.map(iv => `
            <button class="round-pill ${state.harmonicIntervalsEnabled.has(iv)?'active':''}"
                    onclick="toggleHarmInterval('${iv}')">${iv}</button>
          `).join('')}
        </div>
        <div class="btn-grid-2">
          <button class="chip-btn" onclick="setAllHarmIntervals(true)">Pokaż wszystkie</button>
          <button class="chip-btn" onclick="setAllHarmIntervals(false)">Wyczyść</button>
        </div>
      </div>
      <div>
        <div class="section-label">ZAKRES PROGÓW</div>
        <div class="btn-grid-2">
          <input type="number" value="${state.harmonicMinFret}" min="0" max="12" onchange="state.harmonicMinFret=parseInt(this.value,10);renderAll()">
          <input type="number" value="${state.harmonicMaxFret}" min="1" max="24" onchange="state.harmonicMaxFret=parseInt(this.value,10);renderAll()">
        </div>
      </div>`;
    drawFretboard();

  } else if (state.subtab === 'search') {
    // =======================================================================
    // ZAKŁADKA: SZUKAJ (Screenshots 3 & 4 – lista w prawym panelu, kliknięcie koloruje gryf wg roli!)
    // =======================================================================
    const data = await fetchJson(`/api/search?dir=${state.searchDir}&rootPc=${state.rootPc}&chordId=${state.chordId}&scaleId=${state.scaleId}&accMode=${state.accMode}`);
    state.searchData = data;

    stage.innerHTML = `
      <div style="display:flex;justify-content:space-between;align-items:center;padding:8px 4px;font-size:13px;">
        <div style="color:var(--text-muted);">
          ${state.searchDir === 'chord_to_scale'
            ? (state.selectedSearchScaleIdx >= 0 && data.matches[state.selectedSearchScaleIdx]
                ? `Wybrana skala: <b style="color:var(--text)">${data.matches[state.selectedSearchScaleIdx].scaleName}</b> na tle akordu <b style="color:var(--accent)">${data.chordSymbol}</b>`
                : `Akord <b style="color:var(--accent)">${data.chordSymbol}</b> — wybierz skalę z listy po prawej, aby pokolorować gryf wg roli!`)
            : `Skala <b style="color:var(--accent)">${data.scaleFullName}</b> — wybierz akord z listy po prawej`}
        </div>
        <div style="display:flex;gap:16px;align-items:center;font-size:12px;">
          <span><b style="color:#4ade80">●</b> Składnik akordu</span>
          <span><b style="color:#eab308">●</b> Napięcie</span>
          <span><b style="color:#ef4444">●</b> Do unikania</span>
        </div>
      </div>`;

    if (state.searchDir === 'chord_to_scale') {
      insp.innerHTML = `
        <div>
          <div class="section-label">SZUKAJ WG</div>
          <div class="btn-grid-2">
            <button class="chip-btn active" onclick="state.searchDir='chord_to_scale';state.selectedSearchScaleIdx=-1;renderAll()">Akord → skale</button>
            <button class="chip-btn" onclick="state.searchDir='scale_to_chord';state.selectedSearchChordIdx=-1;renderAll()">Skala → akordy</button>
          </div>
        </div>
        <div>
          <div class="section-label">PRYMA AKORDU</div>
          ${buildRootButtonsHtml('setSearchRootPc')}
        </div>
        <div>
          <div class="section-label">RODZAJ</div>
          <select onchange="state.chordId=this.value;state.selectedSearchScaleIdx=-1;renderAll();">
            ${state.initData.chords.map(c => `<option value="${c.id}" ${c.id===state.chordId?'selected':''}>${c.id}</option>`).join('')}
          </select>
        </div>
        <div style="flex:1;min-height:180px;overflow-y:auto;">
          <div class="section-label">PASUJĄCE SKALE (${data.matches.length})</div>
          ${data.matches.map((m, idx) => `
            <div class="list-row ${state.selectedSearchScaleIdx===idx?'selected':''}"
                 style="margin-bottom:5px;flex-direction:column;align-items:flex-start;gap:3px;"
                 onclick="selectSearchScale(${idx})">
              <div>
                <strong>${m.scaleName}</strong>
                <span style="color:var(--text-muted);margin-left:6px;">Napięcia: ${m.tensions || 'brak'}</span>
              </div>
              ${m.avoid ? `<div style="font-size:11px;color:var(--red);">Do unikania: ${m.avoid}</div>` : ''}
            </div>
          `).join('')}
        </div>
        <div>
          <div class="section-label">ETYKIETY</div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.searchLabelType==='notes'?'active':''}" onclick="state.searchLabelType='notes';drawFretboard();renderStageAndInspector()">Nuty</button>
            <button class="chip-btn ${state.searchLabelType==='intervals'?'active':''}" onclick="state.searchLabelType='intervals';drawFretboard();renderStageAndInspector()">Interwały</button>
            <button class="chip-btn ${state.searchLabelType==='tensions'?'active':''}" onclick="state.searchLabelType='tensions';drawFretboard();renderStageAndInspector()">Napięcia</button>
          </div>
        </div>`;
    } else {
      insp.innerHTML = `
        <div>
          <div class="section-label">SZUKAJ WG</div>
          <div class="btn-grid-2">
            <button class="chip-btn" onclick="state.searchDir='chord_to_scale';state.selectedSearchScaleIdx=-1;renderAll()">Akord → skale</button>
            <button class="chip-btn active" onclick="state.searchDir='scale_to_chord';state.selectedSearchChordIdx=-1;renderAll()">Skala → akordy</button>
          </div>
        </div>
        <div>
          <div class="section-label">TONIKA SKALI</div>
          ${buildRootButtonsHtml('setSearchRootPc')}
        </div>
        <div>
          <div class="section-label">SKALA</div>
          <select onchange="state.scaleId=this.value;state.selectedSearchChordIdx=-1;renderAll();">
            ${state.initData.scales.map(s => `<option value="${s.id}" ${s.id===state.scaleId?'selected':''}>${s.name}</option>`).join('')}
          </select>
        </div>
        <div style="flex:1;min-height:180px;overflow-y:auto;">
          <div class="section-label">PASUJĄCE AKORDY (${(data.chords||[]).length})</div>
          ${(data.chords||[]).map((c, idx) => `
            <div class="list-row ${state.selectedSearchChordIdx===idx?'selected':''}"
                 style="margin-bottom:5px;"
                 onclick="state.selectedSearchChordIdx=${idx};drawFretboard();renderStageAndInspector()">
              <strong>${c.symbol}</strong>
              <span style="color:var(--text-muted);">${c.notes}</span>
            </div>
          `).join('')}
        </div>`;
    }
    drawFretboard();

  } else if (state.subtab === 'arpeggio') {
    // =======================================================================
    // ZAKŁADKA: ARPEGGIO I SWEEP PICKING (Od prymy, CAGED, Inwersje, 2NPS, Sweep + 8 przejść Sweep!)
    // =======================================================================
    const customParam = state.customStringMidis.join(',');
    const effDiatRootPc = (state.arpViewMode === 'transition' && state.arpTransitionId === 'diatonic_climb')
      ? state.rootPc
      : state.arpDiatonicScaleRootPc;
    const arpUrl = `/api/arpeggio?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(state.chordId)}&pattern=${state.arpPattern}&posIdx=${state.arpPosIdx}&sweepStrings=${state.arpSweepStrings}&direction=${state.arpDirection}&seqStyle=${state.arpSeqStyle||0}&stringSet=${state.arpStringSet||0}&octaves=${state.arpOctaves||2}&viewMode=${encodeURIComponent(state.arpViewMode)}&transitionId=${encodeURIComponent(state.arpTransitionId)}&scaleRootPc=${effDiatRootPc}&scaleId=${encodeURIComponent(state.arpDiatonicScaleId)}&sevenths=${state.arpDiatonicSevenths?1:0}&accMode=${state.accMode}`;
    const data = await fetchJson(arpUrl);
    state.arpData = data;
    if (data.posIdx !== undefined) state.arpPosIdx = data.posIdx;
    const isTrans = (state.arpViewMode === 'transition');
    const tr = data.transition || { segments: [] };
    if (isTrans && tr.segments && state.arpActiveSegIdx >= tr.segments.length) {
      state.arpActiveSegIdx = 0;
    }

    const activeSteps = getActiveArpeggioSteps();
    const tabSvg = buildArpeggioTabSvg(activeSteps, isTrans ? (tr.segments || []) : null, state.arpPlayStepIdx, isTrans);
    const hasRollAny = activeSteps.some(st => st.roll);
    const rootLabels = getRootLabels();
    const quickArpTypes = [
      { id: 'major', lbl: 'maj' }, { id: 'minor', lbl: 'm' }, { id: 'dim', lbl: 'dim' },
      { id: 'aug', lbl: 'aug' }, { id: 'maj7', lbl: 'maj7' }, { id: '7', lbl: '7' },
      { id: 'm7', lbl: 'm7' }, { id: 'm7b5', lbl: 'm7♭5' }, { id: 'dim7', lbl: 'dim7' }
    ];
    const chordOptHtml = (c) => `<option value="${c.id}" ${c.id===state.chordId?'selected':''}>${c.id} (${c.name})</option>`;
    const groupedChordsHtml = ['Trójdźwięki', 'Dwudźwięki', 'Septymowe', 'Rozszerzone', 'Alterowane'].map(cat => {
      const items = (state.initData.chords || []).filter(c => c.category === cat);
      return items.length ? `<optgroup label="${cat}">${items.map(chordOptHtml).join('')}</optgroup>` : '';
    }).join('');
    const formulaHtml = (!isTrans && data.formula && data.formula.length)
      ? `<div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;margin-top:4px;">
           <span style="font-size:11.5px;color:var(--text-muted);">Składniki:</span>
           ${data.formula.map(f => {
             const iv = f.interval || '';
             const col = f.isRoot ? '#f39237' : ((iv === '3' || iv === '♭3') ? '#4ade80' : ((iv === '5' || iv === '♭5' || iv === '♯5') ? '#38bdf8' : '#facc15'));
             return `<button class="chip-btn" style="padding:2px 8px;font-size:11.5px;border-color:${col};color:${col};"
                             onclick="pluckGuitarFrequency(${f.freq.toFixed(2)}, 0.78)" title="Kliknij, aby posłuchać składnika">
                       <strong>${f.note}</strong> <span style="opacity:0.8;">(${iv})</span>
                     </button>`;
           }).join('')}
         </div>`
      : '';
    const is4Note = (data.formula && data.formula.length >= 4);
    const stringSetLabels = is4Note
      ? ['Struny D–G–B–e', 'Struny A–D–G–B', 'Struny E–A–D–G']
      : ['Struny G–B–e', 'Struny D–G–B', 'Struny A–D–G'];

    stage.innerHTML = `
      <div class="fretboard-card" style="gap:14px;">
        <div style="display:flex;justify-content:space-between;align-items:flex-start;flex-wrap:wrap;gap:10px;">
          <div>
            <div style="font-size:17px;font-weight:800;color:var(--text);">
              ${isTrans ? `⚡ ${tr.name}` : `Arpeggio: ${data.chordSymbol} <span style="color:var(--text-muted);font-weight:600;font-size:13px;">(${data.chordName})</span>`}
            </div>
            ${formulaHtml || `<div style="font-size:12px;color:var(--text-muted);margin-top:2px;">${tr.description}</div>`}
          </div>
          <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
            ${!isTrans ? `<button class="chip-btn" style="padding:7px 11px;font-size:12px;" onclick="playArpeggioChordPreview()" title="Posłuchaj akordu bazowego dla tego arpeggio">🎸 Akord</button>` : ''}
            <button id="arpPlayBtn" class="chip-btn ${state.arpPlaying?'active':''}" style="padding:7px 14px;font-weight:700;"
                    onclick="toggleArpeggioPlayback()">
              ${state.arpPlaying ? '⏹ Stop' : '▶ Odsłuchaj'}
            </button>
            <button class="chip-btn ${state.arpLoop?'active':''}" style="padding:7px 10px;font-size:12px;"
                    onclick="state.arpLoop=!state.arpLoop;renderStageAndInspector()">🔁 Pętla</button>
            <div style="display:flex;align-items:center;gap:6px;background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:4px 8px;font-size:12px;">
              <button class="chip-btn" style="padding:2px 6px;" onclick="setArpeggioBpm(state.arpBpm - 10)">−10</button>
              <strong id="arpBpmVal" style="min-width:54px;text-align:center;color:var(--accent);">${state.arpBpm} BPM</strong>
              <button class="chip-btn" style="padding:2px 6px;" onclick="setArpeggioBpm(state.arpBpm + 10)">+10</button>
            </div>
            <button class="chip-btn" style="padding:7px 12px;font-size:12px;border-color:var(--accent);color:var(--accent);"
                    onclick="sendArpeggioToExerciseTab()" title="Przenieś sekwencję do Edytora Ćwiczeń (ćwiczenie z mikrofonem i eksport GP5/MIDI)">
              ✏️ Ćwicz w TAB / GP5
            </button>
          </div>
        </div>

        <div id="arpTabSvgWrap" style="background:#14100d;border:1px solid #2b231d;border-radius:10px;padding:12px 14px;overflow-x:auto;">
          ${tabSvg}
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;font-size:12px;color:var(--text-muted);background:rgba(243,146,55,0.06);border:1px solid #3c2c1f;border-radius:8px;padding:9px 12px;">
          <div>
            <strong style="color:var(--accent);">💡 Wskazówka:</strong>
            ${isTrans ? tr.practiceTip
              : (state.arpPattern === 6 ? 'Czyste arpeggio od prymy do oktawy — idealny start! Najpierw kliknij „🎸 Akord”, żeby usłyszeć harmonię, potem zagraj składniki po kolei.'
                : (state.arpPattern === 5 ? 'Poziome przewroty (inwersje) na jednym zestawie strun: kliknij „Wszystkie po kolei”, aby przejść płynnie przez wszystkie przewroty w górę gryfu.'
                  : (state.arpPattern === 1 ? 'System CAGED łączy kształt akordu z arpeggio w jednej pozycji lewej dłoni (bez zdublowanych dźwięków unisono).'
                    : (hasRollAny
                      ? 'W tym kształcie występuje przetoczenie palca (Roll · R) — lekko przewiń staw palca między sąsiednimi strunami na tym samym progu.'
                      : 'Tabulatura automatycznie przewija się z zapasem do przodu — kliknij dowolny próg na TAB-ie, aby odsłuchać pojedynczy dźwięk.'))))}
          </div>
          <div style="display:flex;gap:10px;white-space:nowrap;">
            <span><b style="color:#fb923c;">↓</b> w dół</span>
            <span><b style="color:#38bdf8;">↑</b> w górę</span>
            <span><b style="color:#4ade80;">H/P</b> legato</span>
            <span><b style="color:#facc15;">S</b> slide</span>
            <span><b style="color:#fde047;">R</b> roll</span>
          </div>
        </div>

        ${isTrans ? `
          <div>
            <div class="section-label">KOLEJNE OGNIWA PRZEJŚCIA (KLIKNIJ, ABY ZOBACZYĆ KSZTAŁT NA GRYFIE; PRZERYWANY KONTUR = NASTĘPNY AKORD)</div>
            <div style="display:grid;grid-template-columns:repeat(auto-fill,minmax(195px,1fr));gap:8px;">
              ${(tr.segments || []).map((seg, idx) => `
                <div class="chord-box-card ${idx===state.arpActiveSegIdx?'selected':''}" style="align-items:flex-start;text-align:left;padding:10px 12px;gap:4px;"
                     onclick="selectArpTransitionSegment(${idx})">
                  <div style="display:flex;justify-content:space-between;width:100%;font-size:11px;color:var(--text-muted);">
                    <span>${idx + 1}. ${seg.roman}</span>
                    <span>${seg.stringSpan} str. · pr. ${seg.minFret}–${seg.maxFret}</span>
                  </div>
                  <div style="font-size:19px;font-weight:800;color:var(--accent);">${seg.symbol}</div>
                  <div style="font-size:11px;color:#d6cec2;line-height:1.3;">${seg.hint}</div>
                </div>
              `).join('')}
            </div>
          </div>
        ` : `
          <div>
            <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;margin-bottom:8px;">
              <div class="section-label" style="margin:0;">ARPEGGIO DIATONICZNE W SKALI (${data.scaleFullName}) — KLIKNIJ, ABY ZAŁADOWAĆ</div>
              <div style="display:flex;gap:6px;align-items:center;">
                <button class="chip-btn ${!state.arpDiatonicSevenths?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="state.arpDiatonicSevenths=false;renderStageAndInspector()">Trójdźwięki</button>
                <button class="chip-btn ${state.arpDiatonicSevenths?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="state.arpDiatonicSevenths=true;renderStageAndInspector()">Septymowe</button>
              </div>
            </div>
            <div class="chord-cards-grid">
              ${(data.diatonic || []).map(d => {
                const isCur = (d.rootPc === state.rootPc && d.chordId === state.chordId);
                return `
                  <div class="chord-box-card ${isCur?'selected':''}"
                       onclick="selectDiatonicArpeggio(${d.rootPc}, '${d.chordId}')">
                    <div style="font-size:12px;color:var(--text-muted);">Stopień ${d.roman}</div>
                    <div style="font-size:18px;font-weight:800;color:var(--accent);">${d.symbol}</div>
                  </div>`;
              }).join('')}
            </div>
          </div>
        `}
      </div>`;

    insp.innerHTML = `
      <div>
        <div class="section-label">TRYB PRACY</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${!isTrans?'active':''}"
                  onclick="stopArpeggioPlayback(false);state.arpViewMode='single';renderStageAndInspector()">Pojedyncze arpeggio</button>
          <button class="chip-btn ${isTrans?'active':''}"
                  onclick="stopArpeggioPlayback(false);state.arpViewMode='transition';state.arpActiveSegIdx=0;renderStageAndInspector()">⚡ Przejścia Sweep</button>
        </div>
      </div>

      <div>
        <div class="section-label">${isTrans ? 'TONACJA PRZEJŚCIA' : 'PRYMA ARPEGGIO'}</div>
        <div class="btn-grid-6">
          ${rootLabels.map((lbl, pc) => `
            <button class="chip-btn ${state.rootPc===pc?'active':''}"
                    onclick="stopArpeggioPlayback(false);state.rootPc=${pc};state.arpDiatonicScaleRootPc=${pc};state.arpPosIdx=0;renderAll()">${lbl}</button>
          `).join('')}
        </div>
      </div>

      ${isTrans ? `
        <div>
          <div class="section-label">LICZBA STRUN SWEEP</div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.arpSweepStrings===3?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=3;renderStageAndInspector()">3 struny</button>
            <button class="chip-btn ${state.arpSweepStrings===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=5;renderStageAndInspector()">5 strun</button>
            <button class="chip-btn ${state.arpSweepStrings===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=6;renderStageAndInspector()">6 strun</button>
          </div>
        </div>
        ${state.arpTransitionId === 'same_chord_inversions' ? `
          <div>
            <div class="section-label">AKORD DLA INWERSJI</div>
            <select onchange="stopArpeggioPlayback(false);state.chordId=this.value;renderStageAndInspector();">
              ${groupedChordsHtml}
            </select>
          </div>
        ` : ''}
        ${state.arpTransitionId === 'diatonic_climb' ? `
          <div>
            <div class="section-label">SKALA DO WSPINACZKI</div>
            <select onchange="stopArpeggioPlayback(false);state.arpDiatonicScaleId=this.value;renderStageAndInspector();">
              ${(state.initData.scales || []).map(s => `<option value="${s.id}" ${s.id===state.arpDiatonicScaleId?'selected':''}>${s.name}</option>`).join('')}
            </select>
          </div>
        ` : ''}
        <div>
          <div class="section-label">WYBIERZ PRZEJŚCIE SWEEP PICKING (${(data.transitionPresets || []).length})</div>
          <div style="display:flex;flex-direction:column;gap:6px;">
            ${(data.transitionPresets || []).map(p => `
              <button class="chip-btn ${state.arpTransitionId===p.id?'active':''}" style="text-align:left;padding:8px 10px;display:flex;flex-direction:column;gap:2px;"
                      onclick="stopArpeggioPlayback(false);state.arpTransitionId='${p.id}';state.arpActiveSegIdx=0;renderStageAndInspector()">
                <div style="display:flex;justify-content:space-between;width:100%;align-items:center;gap:6px;">
                  <strong style="font-size:12px;">${p.name}</strong>
                  <span style="font-size:10px;opacity:0.75;white-space:nowrap;">${p.category}</span>
                </div>
                <span style="font-size:11px;opacity:0.8;font-weight:400;line-height:1.25;">${p.description}</span>
              </button>
            `).join('')}
          </div>
        </div>
      ` : `
        <div>
          <div class="section-label">TYP ARPEGGIO (SZYBKI WYBÓR)</div>
          <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:5px;margin-bottom:7px;">
            ${quickArpTypes.map(qt => `
              <button class="chip-btn ${state.chordId===qt.id?'active':''}" style="padding:5px 4px;font-size:11.5px;"
                      onclick="stopArpeggioPlayback(false);state.chordId='${qt.id}';state.arpPosIdx=0;renderStageAndInspector()">${qt.lbl}</button>
            `).join('')}
          </div>
          <select onchange="stopArpeggioPlayback(false);state.chordId=this.value;state.arpPosIdx=0;renderStageAndInspector();">
            ${groupedChordsHtml}
          </select>
        </div>

        <div>
          <div class="section-label">UKŁAD NA GRYFIE</div>
          <div style="display:grid;grid-template-columns:1fr 1fr;gap:6px;">
            <button class="chip-btn ${state.arpPattern===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=6;state.arpPosIdx=0;renderStageAndInspector()">🎯 Od prymy</button>
            <button class="chip-btn ${state.arpPattern===1?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=1;state.arpPosIdx=0;renderStageAndInspector()">📦 CAGED (5 kszt.)</button>
            <button class="chip-btn ${state.arpPattern===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=5;state.arpPosIdx=0;renderStageAndInspector()">↔ Inwersje</button>
            <button class="chip-btn ${state.arpPattern===4?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=4;state.arpPosIdx=0;renderStageAndInspector()">⚡ Sweep Picking</button>
            <button class="chip-btn ${state.arpPattern===2?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=2;state.arpPosIdx=0;renderStageAndInspector()">2NPS (2/str.)</button>
            <button class="chip-btn ${state.arpPattern===0?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=0;state.arpPosIdx=0;renderStageAndInspector()">Mapa gryfu</button>
          </div>
        </div>

        ${state.arpPattern === 6 ? `
          <div>
            <div class="section-label">ZAKRES OKTAW</div>
            <div class="btn-grid-2">
              <button class="chip-btn ${state.arpOctaves===1?'active':''}" onclick="stopArpeggioPlayback(false);state.arpOctaves=1;renderStageAndInspector()">1 oktawa</button>
              <button class="chip-btn ${state.arpOctaves===2?'active':''}" onclick="stopArpeggioPlayback(false);state.arpOctaves=2;renderStageAndInspector()">2 oktawy</button>
            </div>
          </div>
        ` : ''}

        ${state.arpPattern === 5 ? `
          <div>
            <div class="section-label">ZESTAW STRUN (INWERSJE)</div>
            <div style="display:flex;flex-direction:column;gap:5px;">
              ${stringSetLabels.map((lbl, idx) => `
                <button class="chip-btn ${state.arpStringSet===idx?'active':''}" style="padding:5px 8px;font-size:11.5px;"
                        onclick="stopArpeggioPlayback(false);state.arpStringSet=${idx};state.arpPosIdx=0;renderStageAndInspector()">${lbl}</button>
              `).join('')}
            </div>
          </div>
        ` : ''}

        ${state.arpPattern === 4 ? `
          <div>
            <div class="section-label">LICZBA STRUN SWEEP</div>
            <div class="btn-grid-3">
              <button class="chip-btn ${state.arpSweepStrings===3?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=3;state.arpPosIdx=0;renderStageAndInspector()">3 struny</button>
              <button class="chip-btn ${state.arpSweepStrings===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=5;state.arpPosIdx=0;renderStageAndInspector()">5 strun</button>
              <button class="chip-btn ${state.arpSweepStrings===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=6;state.arpPosIdx=0;renderStageAndInspector()">6 strun</button>
            </div>
          </div>
        ` : ''}

        ${(data.positions && data.positions.length > 0) ? `
          <div>
            <div class="section-label">POZYCJA / PRZEWRÓT (${data.positions.length})</div>
            <div style="display:flex;flex-wrap:wrap;gap:5px;">
              ${data.positions.map(pos => `
                <button class="chip-btn ${state.arpPosIdx===pos.idx?'active':''}" style="padding:5px 8px;font-size:11px;"
                        onclick="stopArpeggioPlayback(false);state.arpPosIdx=${pos.idx};renderStageAndInspector()">
                  ${pos.label}${pos.hasRoll ? ' · R' : ''}
                </button>
              `).join('')}
            </div>
          </div>
        ` : ''}

        <div>
          <div class="section-label">SEKWENCJA I KIERUNEK</div>
          <div class="btn-grid-3" style="margin-bottom:6px;">
            <button class="chip-btn ${state.arpSeqStyle===0?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=0;renderStageAndInspector()">Prosto</button>
            <button class="chip-btn ${state.arpSeqStyle===1?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=1;renderStageAndInspector()">Grupy po 3</button>
            <button class="chip-btn ${state.arpSeqStyle===2?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=2;renderStageAndInspector()">Skoki 1–5</button>
          </div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.arpDirection===0?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=0;renderStageAndInspector()">↕ Góra-dół</button>
            <button class="chip-btn ${state.arpDirection===1?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=1;renderStageAndInspector()">↓ W górę</button>
            <button class="chip-btn ${state.arpDirection===2?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=2;renderStageAndInspector()">↑ W dół</button>
          </div>
        </div>

        <div>
          <div class="section-label">SKALA DLA KART DIATONICZNYCH</div>
          <select onchange="state.arpDiatonicScaleId=this.value;renderStageAndInspector();">
            ${(state.initData.scales || []).map(s => `<option value="${s.id}" ${s.id===state.arpDiatonicScaleId?'selected':''}>${s.name}</option>`).join('')}
          </select>
        </div>
      `}`;

    renderFretboardToolbarForActiveTab();
    drawFretboard();

  } else if (state.subtab === 'scale_practice') {
    // =======================================================================
    // ZAKŁADKA: PRAKTYKA SKAL I ARPEGGIÓW (Pełny, wyśrodkowany układ studyjny)
    // =======================================================================
    const baseNoteLabels = getRootLabels();
    const scaleObj = (state.initData.scales || []).find(s => s.id === state.scaleId) || { id: 'major', name: 'Durowa' };
    const chordObj = (state.initData.chords || []).find(c => c.id === state.chordId) || { id: 'minor', name: 'Molowy' };
    const degList = (state.fretboardData && state.fretboardData.scaleDegrees) ? state.fretboardData.scaleDegrees : [];
    const degNumsStr = degList.map(d => d.interval || String(d.deg)).join(' ');
    const patternOptions = [
      { code: 0, name: 'Cała skala', desc: 'Graj kolejno każdy dźwięk w skali: 1, 2, 3, 4, 5...' },
      { code: 3, name: 'Grupy po 3', desc: 'Trójki dźwiękowe w górę i w dół skali: 1-2-3, 2-3-4...' },
      { code: 4, name: 'Grupy po 4', desc: 'Cztery kolejne dźwięki w grupie: 1-2-3-4, 2-3-4-5...' },
      { code: 5, name: 'Skoki tercjowe', desc: 'Melodia skokami co tercję: 1-3, 2-4, 3-5...' },
      { code: 6, name: 'Skoki kwartowe', desc: 'Nowoczesne skoki co kwartę: 1-4, 2-5, 3-6...' },
      { code: 7, name: 'Skoki kwintowe', desc: 'Otwarte brzmienie kwintowe: 1-5, 2-6, 3-7...' },
      { code: 8, name: 'Skoki sekstowe', desc: 'Szerokie interwały sekstowe: 1-6, 2-7, 3-8...' },
      { code: 9, name: 'Skoki oktawowe', desc: 'Oktawy wzdłuż skali: 1-8, 2-9...' }
    ];
    const activePat = patternOptions.find(p => p.code === state.spPattern) || patternOptions[0];
    const dirNames = ['Rosnąco ↑', 'Malejąco ↓', 'Rosnąco → Malejąco', 'Malejąco → Rosnąco'];
    const posNames = ['Cały gryf', '3NPS', '4NPS', 'CAGED'];
    const stringLabels = (state.customStringMidis && state.customStringMidis.length === 6)
      ? state.customStringMidis.map(m => baseNoteLabels[((m % 12) + 12) % 12])
      : ['E', 'A', 'D', 'G', 'B', 'E'];
    const scaleDescMap = {
      major: 'Jasna i rozwiązana: punkt odniesienia dla reszty.',
      natural_minor: 'Naturalnie melancholijna i śpiewna barwa molowa.',
      harmonic_minor: 'Egzotyczne napięcie z septymą wielką (sekunda zwiększona).',
      melodic_minor: 'Nowoczesna jazzowa barwa molowa z podniesioną VI i VII.',
      dorian: 'Stylowe brzmienie rocka, funku i fusion z wielką sekstą.',
      phrygian: 'Hiszpański, dramatyczny charakter dzięki sekundzie małej (♭II).',
      lydian: 'Kosmiczne, eteryczne światło dzięki kwarcie zwiększonej (♯IV).',
      mixolydian: 'Bluesowo-rockowa dominanta z małą septymą (♭VII).',
      minor_pentatonic: 'Absolutna klasyka solówek rockowych i bluesowych.',
      blues: 'Pentatonika molowa wzbogacona o blue note (Tryton ♭5).'
    };
    const isScaleMode = (state.scalePracticeMode === 'scale');
    const scaleCharDesc = scaleDescMap[state.scaleId] || 'Pełna skala gotowa do ćwiczeń i analizy na gryfie.';
    const rootNoteLbl = baseNoteLabels[state.rootPc] || 'C';
    const summaryHeadline = isScaleMode
      ? `${rootNoteLbl} ${scaleObj.name.toLowerCase()}`
      : `${state.scalePracticeMode === 'arpeggio' ? 'Arpeggio' : 'Wzór arpeggia'}: ${rootNoteLbl}${chordObj.id === 'major' ? 'maj' : (chordObj.id === 'minor' ? 'm' : chordObj.id)}`;
    const activeStartStrLbl = state.spStartString < 0 ? 'Dowolna' : `Struna ${6 - state.spStartString} (${stringLabels[state.spStartString]})`;

    stage.innerHTML = `
      <div class="sp-shell">
        <div class="sp-header">
          <div>
            <h2 style="font-size:21px;font-weight:800;margin:0;color:var(--text);">Praktyka skal i arpeggiów</h2>
            <div style="font-size:12.5px;color:var(--text-muted);margin-top:2px;">
              Skonfiguruj ćwiczenie i uruchom interaktywną grę z mikrofonem lub odsłuch na żywo.
            </div>
          </div>
          <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
            <button class="chip-btn" style="padding:7px 13px;font-size:12px;" onclick="randomizeScalePractice()">🎲 Losowa tonacja i skala</button>
            <button class="chip-btn" style="padding:7px 13px;font-size:12px;" onclick="switchPracticeSubtab('exercise')">✏️ Otwórz kompozytor TAB</button>
          </div>
        </div>

        <div class="sp-grid">
          <div class="sp-cards-col">
            <!-- Karta 1: Co grać -->
            <div class="sp-card">
              <div class="sp-card-head">
                <h3>1. Co grać</h3>
                <span style="font-size:11.5px;color:var(--text-muted);">${isScaleMode ? 'Wybierz tonację i skalę' : 'Wybierz prymę i rodzaj arpeggio'}</span>
              </div>
              <div class="sp-two-col">
                <div>
                  <div class="section-label">TRYB</div>
                  <div class="ear-seg-row" style="margin-bottom:14px;">
                    <button class="ear-seg-btn ${state.scalePracticeMode==='scale'?'active':''}"
                            onclick="state.scalePracticeMode='scale';renderStageAndInspector()">Skala</button>
                    <button class="ear-seg-btn ${state.scalePracticeMode==='arpeggio'?'active':''}"
                            onclick="state.scalePracticeMode='arpeggio';renderStageAndInspector()">Arpeggio</button>
                    <button class="ear-seg-btn ${state.scalePracticeMode==='arpeggio_pattern'?'active':''}"
                            onclick="state.scalePracticeMode='arpeggio_pattern';renderStageAndInspector()">Wzór arpeggia</button>
                  </div>

                  <div class="section-label">${isScaleMode ? 'TONACJA' : 'PRYMA AKORDU'}</div>
                  <div style="display:grid;grid-template-columns:repeat(6,1fr);gap:6px;">
                    ${baseNoteLabels.map((lbl, pc) => `
                      <button class="chip-btn ${state.rootPc===pc?'active':''}" style="padding:9px 4px;font-weight:700;"
                              onclick="setRootPc(${pc})">${lbl}</button>
                    `).join('')}
                  </div>
                </div>

                <div>
                  <div class="section-label">${isScaleMode ? 'SKALA' : 'RODZAJ AKORDU'}</div>
                  ${isScaleMode ? `
                    <select onchange="state.scaleId=this.value;renderAll()" style="margin-bottom:10px;">
                      ${state.initData.scales.map(s => `<option value="${s.id}" ${s.id===state.scaleId?'selected':''}>${s.name} (${s.category})</option>`).join('')}
                    </select>
                    <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;margin-bottom:8px;flex-wrap:wrap;">
                      <span style="font-size:12.5px;color:var(--accent);font-weight:700;letter-spacing:2px;">${degNumsStr || '1 2 3 4 5 6 7'}</span>
                      <button class="chip-btn" style="padding:4px 10px;font-size:11px;color:#4ade80;border-color:#244b33;background:rgba(74,222,128,0.08);white-space:nowrap;"
                              onclick="playScalePracticeScalePreview()">▶ Posłuchaj skali</button>
                    </div>
                    <div style="font-size:12px;color:var(--text-muted);line-height:1.45;background:#130f0c;border:1px solid #26201b;border-radius:8px;padding:10px 12px;">
                      <b style="color:var(--text);">${scaleObj.name}</b><br>
                      <span>${scaleCharDesc}</span>
                    </div>
                  ` : `
                    <select onchange="state.chordId=this.value;renderAll()" style="margin-bottom:10px;">
                      ${state.initData.chords.map(c => `<option value="${c.id}" ${c.id===state.chordId?'selected':''}>${c.id} (${c.name})</option>`).join('')}
                    </select>
                    <div style="font-size:12px;color:var(--text-muted);line-height:1.45;background:#130f0c;border:1px solid #26201b;border-radius:8px;padding:10px 12px;">
                      <b style="color:var(--text);">${rootNoteLbl} · ${chordObj.name} (${chordObj.id})</b><br>
                      <span>${state.scalePracticeMode === 'arpeggio' ? 'Rozłożony akord grany dźwięk po dźwięku w wybranym kierunku i oktawach.' : 'Wzór melodyczny zbudowany na składnikach akordu.'}</span>
                    </div>
                  `}
                </div>
              </div>
            </div>

            <!-- Karta 2: Jak grać -->
            <div class="sp-card">
              <div class="sp-card-head">
                <h3>2. Jak grać</h3>
                <span style="font-size:11.5px;color:var(--text-muted);">Układ na gryfie, kierunek i wzorzec</span>
              </div>
              <div class="sp-two-col">
                <div>
                  <div class="section-label">POZYCJA NA GRYFIE</div>
                  <div class="ear-seg-row">
                    ${posNames.map((nm, pIdx) => `
                      <button class="ear-seg-btn ${state.spPosSystem===pIdx?'active':''}" style="padding:9px 8px;white-space:nowrap;"
                              onclick="state.spPosSystem=${pIdx};state.posSystem=${pIdx};renderAll()">${pIdx === 0 ? 'Brak' : nm}</button>
                    `).join('')}
                  </div>
                </div>
                <div>
                  <div class="section-label">STRUNA STARTOWA</div>
                  <div class="ear-seg-row">
                    <button class="ear-seg-btn ${state.spStartString===-1?'active':''}" style="padding:9px 8px;white-space:nowrap;"
                            onclick="state.spStartString=-1;renderStageAndInspector()">Dowolna</button>
                    ${stringLabels.map((sLbl, sIdx) => `
                      <button class="ear-seg-btn ${state.spStartString===sIdx?'active':''}" style="padding:9px 6px;white-space:nowrap;"
                              onclick="state.spStartString=${sIdx};renderStageAndInspector()">${sLbl}</button>
                    `).join('')}
                  </div>
                </div>
              </div>

              <div class="sp-two-col">
                <div>
                  <div class="section-label">KIERUNEK</div>
                  <div class="sp-dir-grid">
                    ${dirNames.map((dNm, dIdx) => `
                      <button class="sp-dir-btn ${state.spDirection===dIdx?'active':''}"
                              onclick="state.spDirection=${dIdx};renderStageAndInspector()">${dNm}</button>
                    `).join('')}
                  </div>
                </div>
                <div style="display:flex;flex-direction:column;gap:12px;">
                  <div>
                    <div class="section-label">OKTAWY</div>
                    <div class="ear-seg-row">
                      <button class="ear-seg-btn ${state.spOctaves===1?'active':''}"
                              onclick="state.spOctaves=1;renderStageAndInspector()">1 oktawa</button>
                      <button class="ear-seg-btn ${state.spOctaves===2?'active':''}"
                              onclick="state.spOctaves=2;renderStageAndInspector()">2 oktawy</button>
                    </div>
                  </div>
                  <div>
                    <div class="section-label">ETYKIETY NA GRYFIE</div>
                    <div class="ear-seg-row">
                      <button class="ear-seg-btn ${state.labelType==='notes'?'active':''}"
                              onclick="state.labelType='notes';saveSettings();renderStageAndInspector()">Nuty</button>
                      <button class="ear-seg-btn ${state.labelType==='intervals'?'active':''}"
                              onclick="state.labelType='intervals';saveSettings();renderStageAndInspector()">Interwały</button>
                    </div>
                  </div>
                </div>
              </div>

              <div>
                <div class="section-label">WZORZEC MELODYCZNY</div>
                <div style="display:flex;gap:14px;align-items:center;flex-wrap:wrap;">
                  <select onchange="state.spPattern=parseInt(this.value,10);renderStageAndInspector()" style="max-width:320px;">
                    ${patternOptions.map(p => `<option value="${p.code}" ${p.code===state.spPattern?'selected':''}>${p.name}</option>`).join('')}
                  </select>
                  <span style="font-size:12px;color:var(--text-muted);">${activePat.desc}</span>
                </div>
              </div>
            </div>

            <!-- Karta 3: Dźwięk i wejście -->
            <div class="sp-card">
              <div class="sp-card-head">
                <h3>3. Dźwięk i wejście</h3>
                <button class="chip-btn" style="padding:5px 11px;font-size:11.5px;" onclick="toggleFretboardSettingsModal()">⚙ Konfiguracja gitary i stroju</button>
              </div>
              <div class="sp-two-col">
                <div>
                  <div class="section-label">WEJŚCIE AUDIO</div>
                  <select onchange="onAudioInputChanged(this.value)">
                    ${state.audioInputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioInputId?'selected':''}>${d.label}</option>`).join('')}
                  </select>
                </div>
                <div style="display:grid;grid-template-columns:1fr 1fr;gap:10px 18px;align-content:center;">
                  <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;">
                    <span class="section-label" style="margin:0;">SŁUCHAWKI</span>
                    <button class="ear-toggle-switch ${state.spUseHeadphones?'on':''}"
                            onclick="state.spUseHeadphones=!state.spUseHeadphones;saveSettings();renderStageAndInspector()"></button>
                  </div>
                  <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;">
                    <span class="section-label" style="margin:0;">METRONOM</span>
                    <button class="ear-toggle-switch ${state.spMetronome?'on':''}"
                            onclick="state.spMetronome=!state.spMetronome;saveSettings();renderStageAndInspector()"></button>
                  </div>
                  <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;">
                    <span class="section-label" style="margin:0;">NUTA REF.</span>
                    <button class="ear-toggle-switch ${state.spRefNote?'on':''}"
                            onclick="state.spRefNote=!state.spRefNote;saveSettings();renderStageAndInspector()"></button>
                  </div>
                  <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;">
                    <span class="section-label" style="margin:0;">AKORD W TLE</span>
                    <button class="ear-toggle-switch ${state.spBgChord?'on':''}"
                            onclick="state.spBgChord=!state.spBgChord;saveSettings();renderStageAndInspector()"></button>
                  </div>
                </div>
              </div>
            </div>
          </div>

          <!-- Prawa karta podsumowania i startu (sticky) -->
          <div class="sp-summary-card">
            <div>
              <div style="font-size:11px;text-transform:uppercase;letter-spacing:0.8px;color:var(--accent);font-weight:700;margin-bottom:4px;">Wybrane ćwiczenie</div>
              <div style="font-size:21px;font-weight:800;line-height:1.25;color:var(--text);">${summaryHeadline}</div>
            </div>

            <div class="sp-summary-pills">
              <div class="sp-summary-pill">
                <span>Pozycja</span>
                <strong>${posNames[state.spPosSystem] || 'Cały gryf'}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Struna startowa</span>
                <strong>${activeStartStrLbl}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Kierunek</span>
                <strong>${dirNames[state.spDirection] || 'Rosnąco ↑'}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Zakres</span>
                <strong>${state.spOctaves} ${state.spOctaves === 1 ? 'oktawa' : 'oktawy'}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Wzorzec</span>
                <strong>${activePat.name}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Etykiety</span>
                <strong>${state.labelType === 'intervals' ? 'Interwały' : 'Nuty'}</strong>
              </div>
            </div>

            <button class="action-btn" style="width:100%;padding:13px;font-size:15px;"
                    onclick="launchScalePractice('play')">▶ Graj (z mikrofonem)</button>
            <button class="action-btn secondary" style="width:100%;padding:11px;font-size:14px;"
                    onclick="launchScalePractice('listen')">🔊 Posłuchaj</button>

            <button class="chip-btn" style="width:100%;padding:10px 12px;display:flex;justify-content:space-between;align-items:center;"
                    onclick="state.spAddToSessionOpen=!state.spAddToSessionOpen;renderStageAndInspector()">
              <span>Dodaj do sesji treningowej</span>
              <span>${state.spAddToSessionOpen ? '▴' : '▾'}</span>
            </button>
            ${state.spAddToSessionOpen ? `
              <div style="background:#120f0c;border:1px solid #26201b;border-radius:8px;padding:10px 12px;display:flex;flex-direction:column;gap:8px;">
                <div style="font-size:11.5px;color:var(--text-muted);line-height:1.35;"><b>${summaryHeadline}</b> trafi na koniec wybranej sesji.</div>
                ${(state.initData.sessions || []).map((ses, sIdx) => `
                  <div style="display:flex;justify-content:space-between;align-items:center;gap:10px;padding:4px 0;border-top:1px solid #1e1915;">
                    <div style="min-width:0;">
                      <div style="font-size:12.5px;font-weight:700;color:var(--text);line-height:1.25;">${ses.name}</div>
                      <div style="font-size:10.5px;color:var(--text-muted);">Sesja treningowa</div>
                    </div>
                    <button class="chip-btn" style="padding:5px 10px;font-size:11.5px;color:var(--accent);white-space:nowrap;flex-shrink:0;"
                            onclick="addScalePracticeToSession(${sIdx})">+ Dodaj</button>
                  </div>
                `).join('')}
                <button class="chip-btn" style="width:100%;padding:8px;font-size:12px;margin-top:2px;"
                        onclick="state.mainView='workshop';state.workshopSubView='new_session';renderAll()">+ Nowa sesja</button>
              </div>
            ` : ''}

            <button class="chip-btn" style="width:100%;padding:11px 12px;border-color:#573d26;background:rgba(243,146,55,0.08);"
                    onclick="surpriseScalePractice()">
              <div style="font-weight:700;color:var(--text);">🎲 Zaskocz mnie</div>
              <div style="font-size:11px;color:var(--text-muted);margin-top:2px;">Wylosuj gotowe ćwiczenie i uruchom</div>
            </button>
          </div>
        </div>
      </div>`;

  } else if (state.subtab === 'exercise') {
    // =======================================================================
    // ZAKŁADKA: ĆWICZENIE (ZRZUT 1: KOMPONOWANIE + TABELA NUT | ZRZUT 2: TRYB EXERCISE / SŁUCHANIA)
    // =======================================================================
    const hasNotes = state.recordedEvents.length > 0;

    // WIDOK ZRZUTU 2: AKTYWNY TRYB ĆWICZENIA / SŁUCHANIA („Exercise”)
    if (state.exercisePlayerActive && hasNotes) {
      const totalSteps = state.recordedEvents.length;
      const stepIdx = Math.max(0, Math.min(totalSteps - 1, state.exercisePlayerStepIdx || 0));
      const curEv = state.recordedEvents[stepIdx];
      const labels = getRootLabels();
      const targetLabel = (curEv && !curEv.rest && curEv.midi > 0)
        ? `${curEv.note || labels[((curEv.midi % 12) + 12) % 12]} ${Math.floor(curEv.midi / 12) - 1}`
        : 'Pauza (𝄽)';
      const prevScoreScroll = document.getElementById('exerciseScoreWrap')?.scrollLeft || 0;
      const prevScoreScrollTop = document.getElementById('exerciseScoreWrap')?.scrollTop || 0;
      const scoreAvailW = Math.max(720, Math.min(1140, (stage.clientWidth || 1100) - 56));
      const scoreSvg = buildExerciseStaffAndTabSvg(stepIdx, scoreAvailW);
      const fbSvg = buildExerciseFretboardSvg(stepIdx, false);

      stage.innerHTML = `
        <div style="max-width:1180px;margin:0 auto;width:100%;display:flex;flex-direction:column;gap:18px;padding:6px 0;">
          <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;">
            <div style="display:flex;align-items:center;gap:14px;">
              <button class="chip-btn" style="width:36px;height:36px;font-size:15px;"
                      onclick="toggleExercisePlayerPause()" title="${state.exercisePlayerPaused ? 'Wznów' : 'Pauza'}">
                ${state.exercisePlayerPaused ? '▶' : '⏸'}
              </button>
              <button class="chip-btn" style="width:36px;height:36px;font-size:15px;"
                      onclick="stopExercisePlayer(true)" title="Wróć do edytora ćwiczenia">✕</button>
              <h2 style="font-size:22px;font-weight:800;margin:0;">
                ${state.exercisePlayerMode === 'listen' ? 'Exercise · Odsłuch ćwiczenia' : 'Exercise · Zagraj na gitarze'}
              </h2>
            </div>
            <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
              <button class="chip-btn ${state.exerciseShowStaff!==false?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="state.exerciseShowStaff=!(state.exerciseShowStaff!==false);if(!state.exerciseShowStaff)state.tabCreationEnabled=true;renderStageAndInspector()"
                      title="Włącz lub ukryj pięciolinię (nuty), żeby mieć więcej miejsca na tabulaturę">
                🎼 Nuty: ${state.exerciseShowStaff!==false ? 'Wł.' : 'Wył.'}
              </button>
              <button class="chip-btn ${state.exerciseTabWrapRows?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="state.exerciseTabWrapRows=!state.exerciseTabWrapRows;renderStageAndInspector()"
                      title="Przełącz między 1 linią przewijaną a wieloma wierszami wypełniającymi okno">
                📄 Układ: ${state.exerciseTabWrapRows ? 'Wiersze' : '1 linia'}
              </button>
              <button class="chip-btn ${state.spMetronome?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="toggleExerciseMetronome()" title="Włącz / wyłącz metronom w tempie ćwiczenia">
                🥁 Metronom: ${state.spMetronome ? 'Wł.' : 'Wył.'}
              </button>
              <span style="font-size:12px;color:var(--text-muted);">${state.tempoBpm} BPM</span>
              <span style="font-family:monospace;font-size:15px;font-weight:700;color:var(--text);background:#171310;padding:6px 12px;border-radius:8px;border:1px solid #2b231d;">
                🎵 ${stepIdx + 1} / ${totalSteps}
              </span>
            </div>
          </div>

          ${(() => {
            if (state.exercisePlayerMode !== 'play') return '';
            const heard = (state.detectedMidi !== null && state.detectedMidi !== undefined)
              ? `${labels[((Math.round(state.detectedMidi) % 12) + 12) % 12]} ${Math.floor(Math.round(state.detectedMidi) / 12) - 1}`
              : null;
            if (!micStream) {
              return `<div style="display:flex;align-items:center;justify-content:center;gap:12px;padding:10px 14px;border-radius:10px;background:#2a1713;border:1px solid #7f1d1d;color:#fca5a5;font-weight:700;">
                🎤 Mikrofon wyłączony — nie wykryję dźwięku gitary
                <button class="chip-btn" style="padding:5px 12px;" onclick="toggleLiveAudio().then(()=>renderStageAndInspector())">Włącz mikrofon</button>
              </div>`;
            }
            if (state.exercisePlayerPaused) {
              return `<div style="text-align:center;padding:10px 14px;border-radius:10px;background:#171310;border:1px solid #2b231d;color:var(--text-muted);font-weight:700;">⏸ Wstrzymano</div>`;
            }
            return `<div style="display:flex;align-items:center;justify-content:center;gap:10px;padding:10px 14px;border-radius:10px;background:rgba(243,146,55,0.10);border:1px solid #573d26;color:var(--accent);font-weight:800;">
              🎤 Czekam na dźwięk z gitary… zagraj <span style="font-size:18px;">${targetLabel}</span>
              ${`<span id="exerciseHeardBadge" style="color:var(--text-muted);font-weight:600;font-size:12.5px;">${heard ? `(słyszę: ${heard})` : ''}</span>`}
            </div>`;
          })()}

          <div style="display:flex;justify-content:center;align-items:center;gap:12px;">
            <button class="chip-btn" style="padding:8px 14px;" onclick="stepExercisePlayer(-1)">◀ Poprzednia</button>
            <div style="min-width:94px;height:44px;background:#171310;border:1.5px solid var(--accent);border-radius:10px;display:flex;align-items:center;justify-content:center;font-size:18px;font-weight:800;color:var(--accent);padding:0 14px;">
              ${targetLabel}
            </div>
            <button class="chip-btn" style="padding:8px 14px;" onclick="stepExercisePlayer(1)">Następna ▶</button>
            <button class="chip-btn" style="padding:8px 14px;" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Od początku</button>
          </div>

          <div id="exerciseScoreWrap" style="background:#15110e;border:1px solid #2c241e;border-radius:12px;padding:16px 20px;${state.exerciseTabWrapRows ? 'max-height:360px;overflow-y:auto;overflow-x:hidden;' : 'overflow-x:auto;'}">
            ${scoreSvg}
          </div>

          <div style="background:#14100d;border:1px solid #2c241e;border-radius:12px;padding:14px 16px;overflow-x:auto;">
            ${fbSvg}
          </div>
        </div>`;
      const exScoreWrap = document.getElementById('exerciseScoreWrap');
      if (exScoreWrap) {
        if (state.exerciseTabWrapRows) {
          exScoreWrap.scrollTop = prevScoreScrollTop;
          const actRect = exScoreWrap.querySelector('[data-ex-active="1"]');
          if (actRect && typeof actRect.scrollIntoView === 'function') {
            actRect.scrollIntoView({ block: 'nearest', behavior: 'smooth' });
          }
        } else {
          exScoreWrap.scrollLeft = prevScoreScroll;
          const xStart = (state.exerciseShowStaff !== false) ? 96 : 58;
          scrollTabContainerToX(exScoreWrap, xStart + stepIdx * 46, true);
        }
      }
      return;
    }

    // WIDOK ZRZUTU 1: KOMPOZYTOR ĆWICZENIA (PIĘCIOLINIA + TAB -> GRYF -> TABELA NUT)
    const tabPreview = await fetchJson(`/api/tab_preview?events=${encodeURIComponent(JSON.stringify(state.recordedEvents))}&bpm=${state.tempoBpm}`);
    const durOptions = [
      { code: -2, label: 'Cała nuta (1/1)' },
      { code: -1, label: 'Półnuta (1/2)' },
      { code:  0, label: 'Ćwierćnuta (1/4)' },
      { code:  1, label: 'Ósemka (1/8)' },
      { code:  2, label: 'Szesnastka (1/16)' }
    ];
    const techOptions = [
      { code: '-', label: '—' },
      { code: 'H', label: 'H (Hammer-on)' },
      { code: 'P', label: 'P (Pull-off)' },
      { code: '/', label: '/ (Slide)' },
      { code: 'B', label: 'B (Podciągnięcie)' },
      { code: '~', label: '~ (Wibrato)' },
      { code: 'PM', label: 'PM (Tłumienie)' },
      { code: '◊', label: '◊ (Flażolet)' },
      { code: 'T', label: 'T (Tapping)' }
    ];
    const composerAvailW = Math.max(700, (stage.clientWidth || 960) - 48);
    const scoreSvg = buildExerciseStaffAndTabSvg(-1, composerAvailW);
    const exFretSvg = buildExerciseFretboardSvg(hasNotes ? (state.recordedEvents.length - 1) : -1, true);
    const rootLabels = getRootLabels();

    stage.innerHTML = `
      <div style="display:flex;flex-direction:column;gap:14px;">
        <!-- Górny przełącznik [ Praktyka skal | Ćwiczenie ] -->
        <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:12px;">
          <div style="display:flex;align-items:center;gap:16px;flex-wrap:wrap;">
            <h2 style="font-size:24px;font-weight:800;margin:0;">Praktyka</h2>
            <div class="ear-seg-row" style="width:320px;">
              <button class="ear-seg-btn" onclick="switchPracticeSubtab('scale_practice')">Praktyka skal</button>
              <button class="ear-seg-btn active" onclick="switchPracticeSubtab('exercise')">Ćwiczenie</button>
            </div>
          </div>
          <div style="display:flex;gap:6px;flex-wrap:wrap;">
            <button class="chip-btn" style="padding:6px 12px;" onclick="undoLastExerciseEvent()">⌫ Cofnij [Backspace]</button>
            <button class="chip-btn" style="padding:6px 12px;" onclick="state.recordedEvents=[];renderStageAndInspector()">🗑️ Wyczyść</button>
            <button class="action-btn" style="padding:6px 12px;" onclick="downloadTabExport('gp5')">💾 .GP5 (TuxGuitar)</button>
            <button class="action-btn secondary" style="padding:6px 12px;" onclick="downloadTabExport('mid')">💾 .MID</button>
          </div>
        </div>

        <h3 style="font-size:20px;font-weight:800;margin:2px 0 0;">Twoje ćwiczenie</h3>

        <!-- Karta pięciolinii i tabulatury nad gryfem -->
        <div style="background:#15110e;border:1px solid var(--border);border-radius:12px;padding:14px 16px;overflow-x:auto;">
          ${!hasNotes ? `
            <div style="padding:14px 6px;">
              <div style="font-weight:700;font-size:15px;margin-bottom:4px;">Brak nut</div>
              <div style="font-size:13px;color:var(--text-muted);">
                Dotknij progu na gryfie poniżej, użyj klawiatury (<b>Q W E R T Y U...</b>) lub kliknij <b>„Dodaj nutę”</b>.
              </div>
            </div>
          ` : `
            ${scoreSvg}
            ${state.tabCreationEnabled ? `
              <pre style="background:#0d0b09;padding:8px 12px;border-radius:8px;border:1px solid #26201b;font-family:monospace;font-size:11.5px;line-height:1.3;margin-top:8px;overflow-x:auto;">${tabPreview.asciiTab}</pre>
            ` : ''}
          `}
        </div>

        <!-- Interaktywny gryf bezpośrednio pod pięciolinią (Zrzut 1) -->
        <div>
          <div class="section-label" style="margin-bottom:6px;">DOTKNIJ PODSTRUNNICY, BY DODAĆ NUTY (PROGI ${state.exerciseMinFret || 0}–${state.exerciseMaxFret || 24})</div>
          <div style="background:#14100d;border:1px solid var(--border);border-radius:10px;padding:10px 12px;overflow-x:auto;">
            ${exFretSvg}
          </div>
        </div>

        <!-- Pasek szybkich akcji pod gryfem -->
        <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
          <button class="chip-btn" style="padding:9px 16px;font-weight:700;" onclick="addStepNoteFromScale()">Dodaj nutę</button>
          <button class="chip-btn" style="padding:9px 16px;font-weight:700;" onclick="addRestToExercise()">Dodaj pauzę</button>
          <button class="chip-btn ${state.tupletActive?'active':''}" style="padding:9px 16px;font-weight:700;"
                  onclick="state.tupletActive=!state.tupletActive;renderStageAndInspector()">Tuplet</button>
          <div style="display:flex;gap:4px;align-items:center;margin-left:auto;flex-wrap:wrap;">
            <span style="font-size:11px;color:var(--text-muted);margin-right:4px;">Skala (${state.fretboardData.scaleFullName}):</span>
            ${state.fretboardData.scaleNotes.map((n, idx) => `
              <button class="chip-btn" style="min-width:34px;padding:5px 7px;font-size:11.5px;"
                      onmousedown="addScaleDegreeNoteToExercise(${idx})">${n}</button>
            `).join('')}
          </div>
        </div>

        <!-- Pełna interaktywna tabela nut (Struna / Próg / Nuta / Czas trwania / Typ / Technika / ↑ ↓ ×) -->
        ${hasNotes ? `
          <div style="background:#15110e;border:1px solid var(--border);border-radius:10px;padding:8px 10px;overflow-x:auto;">
            <table class="ex-notes-table">
              <thead>
                <tr>
                  <th>#</th>
                  <th>Struna</th>
                  <th>Próg</th>
                  <th>Nuta</th>
                  <th>Czas trwania</th>
                  <th>Typ</th>
                  <th>Technika</th>
                  <th style="text-align:right;">Akcje</th>
                </tr>
              </thead>
              <tbody>
                ${state.recordedEvents.map((ev, idx) => ({ ev, idx })).reverse().map(({ ev, idx }) => {
                  const isRest = !!ev.rest;
                  const strNum = isRest ? 6 : (6 - (ev.s !== undefined ? ev.s : 0));
                  const fretNum = isRest ? 0 : (ev.f !== undefined ? ev.f : 0);
                  const noteNm = isRest ? '𝄽' : (ev.note || (ev.midi > 0 ? rootLabels[((ev.midi % 12) + 12) % 12] : '—'));
                  const curTech = ev.tech || '-';
                  return `
                    <tr class="${idx === state.recordedEvents.length - 1 ? 'ex-active-row' : ''}">
                      <td style="color:var(--text-muted);font-family:monospace;">${idx + 1}</td>
                      <td>
                        <select style="width:56px;" ${isRest?'disabled':''}
                                onchange="updateExerciseRow(${idx}, 'strNum', parseInt(this.value,10))">
                          ${[1,2,3,4,5,6].map(sn => `<option value="${sn}" ${sn===strNum?'selected':''}>${sn}</option>`).join('')}
                        </select>
                      </td>
                      <td>
                        <input type="number" min="0" max="24" value="${fretNum}" style="width:56px;" ${isRest?'disabled':''}
                               onchange="updateExerciseRow(${idx}, 'fret', parseInt(this.value,10))">
                      </td>
                      <td style="font-weight:800;color:${isRest?'var(--text-muted)':'var(--accent)'};min-width:34px;">${noteNm}</td>
                      <td>
                        <select onchange="updateExerciseRow(${idx}, 'dur', parseInt(this.value,10))">
                          ${durOptions.map(d => `<option value="${d.code}" ${ev.dur===d.code?'selected':''}>${d.label.replace(/\s*\(.*\)/,'')}</option>`).join('')}
                        </select>
                      </td>
                      <td>
                        <select onchange="updateExerciseRow(${idx}, 'rest', parseInt(this.value,10))">
                          <option value="0" ${!isRest?'selected':''}>Nuta</option>
                          <option value="1" ${isRest?'selected':''}>Pauza</option>
                        </select>
                      </td>
                      <td>
                        <select ${isRest?'disabled':''} onchange="updateExerciseRow(${idx}, 'tech', this.value)">
                          ${techOptions.map(t => `<option value="${t.code}" ${curTech===t.code?'selected':''}>${t.label}</option>`).join('')}
                        </select>
                      </td>
                      <td style="text-align:right;white-space:nowrap;">
                        <button class="chip-btn" style="padding:3px 7px;" ${idx >= state.recordedEvents.length - 1 ? 'disabled' : ''}
                                onclick="moveExerciseEvent(${idx}, 1)" title="Przesuń wyżej">↑</button>
                        <button class="chip-btn" style="padding:3px 7px;" ${idx <= 0 ? 'disabled' : ''}
                                onclick="moveExerciseEvent(${idx}, -1)" title="Przesuń niżej">↓</button>
                        <button class="chip-btn" style="padding:3px 7px;color:#ef4444;"
                                onclick="deleteExerciseEvent(${idx})" title="Usuń dźwięk">×</button>
                      </td>
                    </tr>`;
                }).join('')}
              </tbody>
            </table>
          </div>
          <div>
            <button class="chip-btn" style="padding:10px 18px;font-weight:700;" onclick="saveExerciseToWorkshop()">
              Zapisz w Warsztacie
            </button>
          </div>
        ` : ''}
      </div>`;

    insp.innerHTML = `
      <div style="background:var(--bg-card);padding:16px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:8px;">
        <div style="font-size:22px;font-weight:800;">${hasNotes ? `${state.recordedEvents.length} nut` : 'Brak nut'}</div>
        <div style="font-size:12px;color:var(--text-muted);">${tabPreview.detectedKey} · ${state.tempoBpm} BPM · ${state.timeSigNum}/${state.timeSigDen}</div>
        <button class="action-btn" style="width:100%;padding:12px;font-size:14.5px;opacity:${hasNotes?1:0.45}" ${hasNotes?'':'disabled'}
                onclick="startExercisePlayer('play')">Graj</button>
        <button class="action-btn secondary" style="width:100%;padding:11px;font-size:13.5px;opacity:${hasNotes?1:0.45}" ${hasNotes?'':'disabled'}
                onclick="startExercisePlayer('listen')">Posłuchaj</button>
      </div>

      <div style="font-size:16px;font-weight:800;margin-top:4px;">Ustawienia ćwiczenia</div>

      <div>
        <div class="section-label">TONACJA</div>
        <div class="btn-grid-2">
          <button class="chip-btn active">${tabPreview.detectedKey}</button>
          <button class="chip-btn" onclick="detectAndApplyKey('${tabPreview.detectedKey}')">Wykryj z nut</button>
        </div>
      </div>

      <div style="display:grid;grid-template-columns:1fr 1fr;gap:10px;">
        <div>
          <div class="section-label">PIĘCIOLINIA (NUTY)</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exerciseShowStaff!==false?'active':''}"
                    onclick="state.exerciseShowStaff=true;renderStageAndInspector()">Wł.</button>
            <button class="chip-btn ${state.exerciseShowStaff===false?'active':''}"
                    onclick="state.exerciseShowStaff=false;state.tabCreationEnabled=true;renderStageAndInspector()">Wył.</button>
          </div>
        </div>
        <div>
          <div class="section-label">TAB</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.tabCreationEnabled?'active':''}"
                    onclick="state.tabCreationEnabled=true;renderStageAndInspector()">Wł.</button>
            <button class="chip-btn ${!state.tabCreationEnabled?'active':''}"
                    onclick="state.tabCreationEnabled=false;state.exerciseShowStaff=true;renderStageAndInspector()">Wył.</button>
          </div>
        </div>
      </div>

      <div style="display:grid;grid-template-columns:1.25fr 1fr;gap:10px;">
        <div>
          <div class="section-label">UKŁAD TABULATURY</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${!state.exerciseTabWrapRows?'active':''}" style="padding:7px 4px;font-size:11.5px;"
                    onclick="state.exerciseTabWrapRows=false;renderStageAndInspector()">1 linia</button>
            <button class="chip-btn ${state.exerciseTabWrapRows?'active':''}" style="padding:7px 4px;font-size:11.5px;"
                    onclick="state.exerciseTabWrapRows=true;renderStageAndInspector()">Wiersze (2+)</button>
          </div>
        </div>
        <div>
          <div class="section-label">KLUCZ</div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.exerciseClef==='treble'?'active':''}" style="padding:7px 2px;font-size:11px;"
                    onclick="state.exerciseClef='treble';renderStageAndInspector()">Wiol.</button>
            <button class="chip-btn ${state.exerciseClef==='bass'?'active':''}" style="padding:7px 2px;font-size:11px;"
                    onclick="state.exerciseClef='bass';renderStageAndInspector()">Bas.</button>
            <button class="chip-btn ${state.exerciseClef==='auto'?'active':''}" style="padding:7px 2px;font-size:11px;"
                    onclick="state.exerciseClef='auto';renderStageAndInspector()">Auto</button>
          </div>
        </div>
      </div>

      <div style="display:grid;grid-template-columns:1.2fr 1fr;gap:10px;">
        <div>
          <div class="section-label">BPM</div>
          <div style="display:flex;gap:4px;align-items:center;">
            <button class="chip-btn" style="width:34px;" onclick="state.tempoBpm=Math.max(40,state.tempoBpm-5);renderStageAndInspector()">−</button>
            <input type="number" value="${state.tempoBpm}" min="40" max="260" style="text-align:center;"
                   onchange="state.tempoBpm=Math.max(40,Math.min(260,parseInt(this.value,10)||120));renderStageAndInspector()">
            <button class="chip-btn" style="width:34px;" onclick="state.tempoBpm=Math.min(260,state.tempoBpm+5);renderStageAndInspector()">+</button>
          </div>
        </div>
        <div>
          <div class="section-label">METRUM</div>
          <div style="display:flex;gap:4px;align-items:center;">
            <input type="number" value="${state.timeSigNum}" min="1" max="12" style="text-align:center;"
                   onchange="state.timeSigNum=Math.max(1,Math.min(12,parseInt(this.value,10)||4));renderStageAndInspector()">
            <span>/</span>
            <select onchange="state.timeSigDen=parseInt(this.value,10);renderStageAndInspector()">
              <option value="4" ${state.timeSigDen===4?'selected':''}>4</option>
              <option value="8" ${state.timeSigDen===8?'selected':''}>8</option>
              <option value="2" ${state.timeSigDen===2?'selected':''}>2</option>
            </select>
          </div>
        </div>
      </div>

      <div>
        <div class="section-label">DOMYŚLNY CZAS TRWANIA</div>
        <div style="display:grid;grid-template-columns:1.4fr 1fr;gap:6px;">
          <select onchange="state.defaultDuration=parseInt(this.value,10);renderStageAndInspector()">
            ${durOptions.map(d => `<option value="${d.code}" ${state.defaultDuration===d.code?'selected':''}>${d.label}</option>`).join('')}
          </select>
          <button class="chip-btn ${state.showExerciseNoteNames?'active':''}"
                  onclick="state.showExerciseNoteNames=!state.showExerciseNoteNames;renderStageAndInspector()">Nazwy nut</button>
        </div>
      </div>

      <div>
        <div class="section-label">ZAKRES PROGÓW</div>
        <div style="display:flex;align-items:center;gap:8px;">
          <input type="number" min="0" max="24" value="${state.exerciseMinFret || 0}" style="width:74px;text-align:center;"
                 onchange="state.exerciseMinFret=Math.max(0,Math.min(state.exerciseMaxFret||24,parseInt(this.value,10)||0));renderStageAndInspector()">
          <span>–</span>
          <input type="number" min="0" max="24" value="${state.exerciseMaxFret || 24}" style="width:74px;text-align:center;"
                 onchange="state.exerciseMaxFret=Math.max(state.exerciseMinFret||0,Math.min(24,parseInt(this.value,10)||24));renderStageAndInspector()">
        </div>
      </div>

      <div>
        <div class="section-label">WEJŚCIE AUDIO</div>
        <select onchange="onAudioInputChanged(this.value)">
          ${state.audioInputDevices.map(d => `<option value="${d.id}" ${d.id===state.selectedAudioInputId?'selected':''}>${d.label}</option>`).join('')}
        </select>
      </div>

      <div>
        <div class="section-label">SZYBKIE WPRAWKI Z WARSZTATU</div>
        <div class="btn-grid-2">
          <button class="chip-btn" onclick="loadWorkshopPatternIntoTab(3)">⚡ 3NPS (Grupy po 3)</button>
          <button class="chip-btn" onclick="loadWorkshopPatternIntoTab(4)">⚡ 3NPS (Grupy po 4)</button>
        </div>
      </div>`;
  }
}

function selectAndPlayChordCard(el, idx, freqs) {
  state.selectedChordVoicingIdx = idx;
  document.querySelectorAll('.chord-box-card').forEach(c => c.classList.remove('selected'));
  if (el) el.classList.add('selected');
  renderFretboardToolbarForActiveTab();
  drawFretboard();
  const gainScale = 1.0 / Math.sqrt(Math.max(1, (freqs || []).length));
  (freqs || []).forEach((f, i) => {
    setTimeout(() => pluckGuitarFrequency(f, 1.5, gainScale), i * 45);
  });
}

function toggleHarmDiff(d) {
  state.harmonicDiffEnabled[d] = !state.harmonicDiffEnabled[d];
  drawFretboard();
  renderStageAndInspector();
}

function toggleHarmInterval(iv) {
  if (state.harmonicIntervalsEnabled.has(iv)) state.harmonicIntervalsEnabled.delete(iv);
  else state.harmonicIntervalsEnabled.add(iv);
  drawFretboard();
  renderStageAndInspector();
}

function setAllHarmIntervals(on) {
  const all = ['1','2','3','#4','5','b6','b7','7'];
  state.harmonicIntervalsEnabled = on ? new Set(all) : new Set();
  drawFretboard();
  renderStageAndInspector();
}

async function confirmAddWorkshopExercise() {
  const c = state.wsCreator;
  const sharpLabels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
  const scObj = state.initData.scales.find(s => s.id === c.scaleId) || { name: 'Durowa' };
  const posNames = ['Cały gryf', '3NPS', '4NPS', 'CAGED'];
  const patNames = {
    0: 'Cała skala', 3: 'Grupy po 3', 4: 'Grupy po 4',
    5: 'Skoki tercjowe', 6: 'Skoki kwartowe', 7: 'Skoki kwintowe',
    8: 'Skoki sekstowe', 9: 'Skoki oktawowe'
  };
  const newEx = {
    id: 'ex_' + Date.now(),
    title: `${sharpLabels[c.rootPc]} ${scObj.name} · ${posNames[c.posSystem]} · ${patNames[c.pattern] || 'Cała skala'}`,
    category: 'Technique Drill',
    level: c.octaves === 2 ? 'Średniozaawansowany' : 'Początkujący',
    rootPc: c.rootPc,
    scaleId: c.scaleId,
    posSystem: c.posSystem,
    startString: c.startString,
    direction: c.direction,
    octaves: c.octaves,
    pattern: c.pattern
  };
  state.customWorkshopExercises.unshift(newEx);
  state.newSessionItems.push(newEx);
  state.workshopSubView = 'new_session';
  renderStageAndInspector();
}

function saveNewWorkshopSession() {
  const name = (state.newSessionName || '').trim() || `Sesja ${state.initData.sessions.length + 1}`;
  state.initData.sessions.push({
    id: 'ses_' + Date.now(),
    name: name,
    breakBars: 1
  });
  state.workshopSubView = 'sessions';
  renderStageAndInspector();
}

async function launchWorkshopExercise(ex) {
  if (!ex) return;
  if (typeof stopExercisePlayer === 'function') stopExercisePlayer(false);
  state.rootPc = ex.rootPc !== undefined ? ex.rootPc : 0;
  state.scaleId = ex.scaleId || 'major';
  state.posSystem = ex.posSystem !== undefined ? ex.posSystem : 1;
  state.exerciseTitle = ex.title || '';
  if (Array.isArray(ex.recordedEvents) && ex.recordedEvents.length > 0) {
    state.recordedEvents = ex.recordedEvents.map(ev => Object.assign({ dur: 1, tuplet: 0, rest: 0, tech: '-' }, ev));
  } else {
    const url = `/api/workshop_events?rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.posSystem}&startString=${ex.startString !== undefined ? ex.startString : -1}&direction=${ex.direction || 0}&octaves=${ex.octaves || 2}&pattern=${ex.pattern || 0}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${state.customStringMidis.join(',')}`;
    const res = await fetchJson(url);
    state.recordedEvents = (res.events || []).map(ev => ({
      s: ev.s,
      f: ev.f,
      midi: ev.midi,
      note: ev.note,
      freq: ev.freq,
      dur: 1,
      tuplet: 0,
      rest: 0,
      tech: '-'
    }));
  }
  state.mainView = 'practice';
  state.subtab = 'exercise';
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-subtab') === 'exercise');
  });
  await renderAll();
  startExercisePlayer(ex.playerMode || 'listen');
}

// Najpopularniejsze akordy do grupy „Podstawowe” (reszta jest w grupach wg kategorii)
const BASIC_CHORD_IDS = new Set(['major', 'minor', '7', 'maj7', 'm7', 'sus2', 'sus4', '5', 'dim', 'add9']);

function chordInGroup(c, group) {
  if (group === 'all') return true;
  if (group === 'basic') return BASIC_CHORD_IDS.has(c.id);
  if (group === 'triad') return c.category === 'Trójdźwięki';
  if (group === 'seventh') return c.category === 'Septymowe';
  if (group === 'extended') return c.category === 'Rozszerzone';
  if (group === 'altered') return c.category === 'Alterowane';
  return true;
}

function setChordGroupFilter(group) {
  state.chordGroupFilter = group;
  const pool = (state.chordFilterByScale && state.chordLastMatchingIds && state.chordLastMatchingIds.length > 0)
    ? (state.initData.chords || []).filter(c => state.chordLastMatchingIds.includes(c.id))
    : (state.initData.chords || []);
  const cur = pool.find(c => c.id === state.chordId);
  if (!cur || !chordInGroup(cur, group)) {
    const first = pool.find(c => chordInGroup(c, group));
    if (first) state.chordId = first.id;
  }
  state.selectedChordVoicingIdx = 0;
  saveSettings();
  renderAll();
}

function setSearchRootPc(pc) {
  state.rootPc = pc;
  state.selectedSearchScaleIdx = -1;
  state.selectedSearchChordIdx = -1;
  renderAll();
}

let arpPlaybackTimer = null;

// Płynne śledzenie tabulatury z zapasem ok. 74% widoku do przodu (aktywna nuta trzymana w ~26% od lewej)
function scrollTabContainerToX(wrap, noteX, smooth = true) {
  if (!wrap) return;
  const vw = wrap.clientWidth || 700;
  const maxScroll = Math.max(0, wrap.scrollWidth - vw);
  if (maxScroll <= 0) return;
  const target = Math.max(0, Math.min(maxScroll, Math.round(noteX - vw * 0.26)));
  if (Math.abs(wrap.scrollLeft - target) > 6) {
    wrap.scrollTo({ left: target, behavior: smooth ? 'smooth' : 'auto' });
  }
}

function getActiveArpeggioSteps() {
  if (!state.arpData) return [];
  if (state.arpViewMode === 'transition' && state.arpData.transition && state.arpData.transition.segments) {
    const out = [];
    state.arpData.transition.segments.forEach((seg, sIdx) => {
      (seg.steps || []).forEach(st => {
        out.push(Object.assign({}, st, { seg: sIdx, chordSymbol: seg.symbol, roman: seg.roman }));
      });
    });
    return out;
  }
  return state.arpData.steps || [];
}

function refreshArpTabSvgOnly() {
  const wrap = document.getElementById('arpTabSvgWrap');
  if (!wrap || !state.arpData) return;
  const prevScroll = wrap.scrollLeft || 0;
  const isTrans = (state.arpViewMode === 'transition');
  const tr = state.arpData.transition || { segments: [] };
  const activeSteps = getActiveArpeggioSteps();
  wrap.innerHTML = buildArpeggioTabSvg(activeSteps, isTrans ? (tr.segments || []) : null, state.arpPlayStepIdx, isTrans);
  wrap.scrollLeft = prevScroll;
}

function buildArpeggioTabSvg(steps, segments, activeIdx, isTransition) {
  if (!steps || steps.length === 0) {
    return `<div style="padding:18px;text-align:center;color:var(--text-muted);">Brak wygenerowanych nut dla tej konfiguracji.</div>`;
  }
  const nStrings = 6;
  const leftPad = 36;
  const colW = 34;
  const rightPad = 24;
  const showIntervalsAbove = (state.labelType === 'intervals');
  const topY = isTransition ? 56 : 40;
  const rowGap = 15;
  const W = Math.max(520, leftPad + steps.length * colW + rightPad);
  const H = topY + (nStrings - 1) * rowGap + 24;
  const strNames = ['e', 'B', 'G', 'D', 'A', 'E'];
  const symMap = { D: '↓', U: '↑', H: 'H', P: 'P', S: 'S' };
  const symColor = { D: '#fb923c', U: '#38bdf8', H: '#4ade80', P: '#4ade80', S: '#facc15' };

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg">`;
  // Pionowy kursor podążający za aktualną nutą (aktualizowany bez przeładowywania SVG -> idealne płynne przewijanie!)
  const playX = (activeIdx >= 0 && activeIdx < steps.length) ? (leftPad + activeIdx * colW + colW * 0.5 - 14) : -100;
  svg += `<rect id="arpPlayheadBg" x="${playX}" y="${topY - 22}" width="28" height="${(nStrings - 1) * rowGap + 34}" rx="6" fill="rgba(34,197,94,0.14)" stroke="rgba(74,222,128,0.45)" stroke-width="1" style="display:${activeIdx >= 0 ? 'block' : 'none'}"/>`;

  for (let r = 0; r < nStrings; r++) {
    const y = topY + r * rowGap;
    svg += `<text x="12" y="${y + 4}" fill="#8c8277" font-family="monospace" font-size="11" font-weight="700">${strNames[r]}</text>`;
    svg += `<line x1="${leftPad - 8}" y1="${y}" x2="${W - 10}" y2="${y}" stroke="#3e332a" stroke-width="1.2"/>`;
  }
  svg += `<line x1="${leftPad - 8}" y1="${topY}" x2="${leftPad - 8}" y2="${topY + (nStrings - 1) * rowGap}" stroke="#6e5c4d" stroke-width="1.5"/>`;

  let lastSeg = -1;
  for (let i = 0; i < steps.length; i++) {
    const st = steps[i];
    const x = leftPad + i * colW + colW * 0.5;
    const isAct = (i === activeIdx);

    if (isTransition && st.seg !== lastSeg) {
      lastSeg = st.seg;
      if (i > 0) {
        svg += `<line x1="${x - colW * 0.5}" y1="${topY - 34}" x2="${x - colW * 0.5}" y2="${topY + (nStrings - 1) * rowGap + 6}" stroke="#57412e" stroke-width="1.3" stroke-dasharray="4,3"/>`;
      }
      const segObj = segments && segments[st.seg];
      const hdr = segObj ? `${segObj.symbol} (${segObj.roman})` : '';
      const hdrColor = (st.seg === state.arpActiveSegIdx) ? '#f39237' : '#d6cec2';
      svg += `<text data-seg-hdr="${st.seg}" x="${x - 6}" y="14" fill="${hdrColor}" font-family="Segoe UI, sans-serif" font-size="11.5" font-weight="800">${hdr}</text>`;
    }

    // Subtelny znacznik toniki (lub interwał) WYŻEJ nad tabulaturą — bez zaciemniania/podświetlania cyfr progów!
    if (showIntervalsAbove && st.interval) {
      svg += `<text x="${x}" y="${topY - 24}" fill="${st.isRoot ? '#f39237' : '#9c9288'}" font-family="Segoe UI, sans-serif" font-size="9.5" font-weight="${st.isRoot ? '800' : '600'}" text-anchor="middle">${st.interval}</text>`;
    } else if (st.isRoot) {
      svg += `<circle cx="${x}" cy="${topY - 26}" r="2.8" fill="#f39237"><title>Tonika (1)</title></circle>`;
    }

    // Symbol kostkowania / techniki nad tabulaturą
    const techSym = symMap[st.tech] || st.tech || '↓';
    const tCol = symColor[st.tech] || '#fb923c';
    svg += `<text x="${x}" y="${topY - 10}" fill="${tCol}" font-family="Segoe UI, sans-serif" font-size="11.5" font-weight="800" text-anchor="middle">${techSym}${st.roll ? '·R' : ''}</text>`;

    // Numer progu na odpowiedniej strunie (czysty biały na ciemnym tle; podświetlenie wyłącznie dla aktywnego kroku!)
    const r = (nStrings - 1) - st.s;
    const y = topY + r * rowGap;
    const fretVal = (st.f !== undefined) ? st.f : st.fret;
    const boxW = String(fretVal).length > 1 ? 22 : 16;
    const bgFill = isAct ? '#22c55e' : '#14100d';
    const txtFill = isAct ? '#05140b' : '#f5f2eb';
    const strokeCol = isAct ? '#bbf7d0' : 'none';

    svg += `<g style="cursor:pointer" data-step-g="${i}" data-step-x="${x}" data-root="${st.isRoot?1:0}" onclick="playSingleArpeggioStep(${i})">`;
    svg += `<rect data-step-rect="${i}" x="${x - boxW / 2}" y="${y - 7.5}" width="${boxW}" height="15" rx="3.5" fill="${bgFill}" stroke="${strokeCol}" stroke-width="${isAct ? '1.2' : '0'}"/>`;
    svg += `<text data-step-txt="${i}" x="${x}" y="${y + 4}" fill="${txtFill}" font-family="Segoe UI, sans-serif" font-size="11.5" font-weight="800" text-anchor="middle">${fretVal}</text>`;
    svg += `</g>`;
  }

  svg += `</svg>`;
  return svg;
}

// Aktualizuje podświetlenie aktywnej nuty TAB bez niszczenia DOM i płynnie przesuwa pasek przewijania z zapasem do przodu!
function updateArpTabHighlightAndScroll(activeIdx, smooth = true) {
  const wrap = document.getElementById('arpTabSvgWrap');
  if (!wrap) return;
  const svg = wrap.querySelector('svg');
  if (!svg) return;

  svg.querySelectorAll('g[data-step-g]').forEach(g => {
    const idx = parseInt(g.getAttribute('data-step-g'), 10);
    const isAct = (idx === activeIdx);
    const rect = g.querySelector('rect');
    const txt = g.querySelector('text');
    if (rect) {
      rect.setAttribute('fill', isAct ? '#22c55e' : '#14100d');
      rect.setAttribute('stroke', isAct ? '#bbf7d0' : 'none');
      rect.setAttribute('stroke-width', isAct ? '1.2' : '0');
    }
    if (txt) {
      txt.setAttribute('fill', isAct ? '#05140b' : '#f5f2eb');
    }
  });

  if (state.arpViewMode === 'transition') {
    svg.querySelectorAll('text[data-seg-hdr]').forEach(t => {
      const seg = parseInt(t.getAttribute('data-seg-hdr'), 10);
      t.setAttribute('fill', seg === state.arpActiveSegIdx ? '#f39237' : '#d6cec2');
    });
  }

  const ph = svg.querySelector('#arpPlayheadBg');
  const actG = (activeIdx >= 0) ? svg.querySelector(`g[data-step-g="${activeIdx}"]`) : null;
  if (ph) {
    if (actG) {
      const nx = parseFloat(actG.getAttribute('data-step-x')) || 0;
      ph.setAttribute('x', String(nx - 14));
      ph.style.display = 'block';
      scrollTabContainerToX(wrap, nx, smooth);
    } else {
      ph.style.display = 'none';
    }
  }
}

function playArpeggioChordPreview() {
  if (!state.arpData || !state.arpData.formula || !state.arpData.formula.length) return;
  const freqs = state.arpData.formula.map(f => f.freq);
  const scaleGain = Math.min(0.65, 1.35 / Math.sqrt(Math.max(1, freqs.length)));
  freqs.forEach((hz, idx) => {
    setTimeout(() => pluckGuitarFrequency(hz, scaleGain), idx * 35);
  });
}

function selectDiatonicArpeggio(rootPc, chordId) {
  stopArpeggioPlayback(false);
  state.rootPc = rootPc;
  state.chordId = chordId;
  state.arpPosIdx = 0;
  renderAll();
}

function selectArpTransitionSegment(segIdx) {
  stopArpeggioPlayback(false);
  state.arpActiveSegIdx = segIdx;
  state.arpPlayStepIdx = -1;
  renderStageAndInspector();
}

function setArpeggioBpm(bpm) {
  state.arpBpm = Math.max(40, Math.min(200, bpm));
  const el = document.getElementById('arpBpmVal');
  if (el) el.textContent = `${state.arpBpm} BPM`;
  saveSettings();
  if (state.arpPlaying) {
    stopArpeggioPlayback(false);
    toggleArpeggioPlayback();
  }
}

function playSingleArpeggioStep(idx) {
  const steps = getActiveArpeggioSteps();
  const st = steps[idx];
  if (!st) return;
  state.arpPlayStepIdx = idx;
  if (state.arpViewMode === 'transition' && st.seg !== undefined) {
    state.arpActiveSegIdx = st.seg;
  }
  pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
  updateArpTabHighlightAndScroll(idx, true);
  renderFretboardToolbarForActiveTab();
  drawFretboard();
}

function stopArpeggioPlayback(redraw = true) {
  if (arpPlaybackTimer) {
    clearTimeout(arpPlaybackTimer);
    arpPlaybackTimer = null;
  }
  state.arpPlaying = false;
  state.arpPlayStepIdx = -1;
  const btn = document.getElementById('arpPlayBtn');
  if (btn) {
    btn.textContent = '▶ Odsłuchaj';
    btn.classList.remove('active');
  }
  if (redraw && state.subtab === 'arpeggio') {
    updateArpTabHighlightAndScroll(-1, false);
    drawFretboard();
  }
}

function toggleArpeggioPlayback() {
  if (state.arpPlaying) {
    stopArpeggioPlayback(true);
    return;
  }
  const steps = getActiveArpeggioSteps();
  if (!steps.length) return;
  state.arpPlaying = true;
  state.arpPlayStepIdx = 0;
  const btn = document.getElementById('arpPlayBtn');
  if (btn) {
    btn.textContent = '⏹ Stop';
    btn.classList.add('active');
  }

  const tick = () => {
    if (!state.arpPlaying || state.subtab !== 'arpeggio') {
      stopArpeggioPlayback(false);
      return;
    }
    const curSteps = getActiveArpeggioSteps();
    if (!curSteps.length) {
      stopArpeggioPlayback(true);
      return;
    }
    if (state.arpPlayStepIdx >= curSteps.length) {
      if (state.arpLoop) {
        state.arpPlayStepIdx = 0;
      } else {
        stopArpeggioPlayback(true);
        return;
      }
    }
    const st = curSteps[state.arpPlayStepIdx];
    const segChanged = (state.arpViewMode === 'transition' && st.seg !== undefined && st.seg !== state.arpActiveSegIdx);
    if (segChanged) {
      state.arpActiveSegIdx = st.seg;
      document.querySelectorAll('#dynamicStagePanel .chord-box-card').forEach((card, ci) => {
        card.classList.toggle('selected', ci === state.arpActiveSegIdx);
      });
      renderFretboardToolbarForActiveTab();
    }
    pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
    updateArpTabHighlightAndScroll(state.arpPlayStepIdx, state.arpPlayStepIdx > 0);
    drawFretboard();

    state.arpPlayStepIdx++;
    // Szesnastki / triole wg ustawionego BPM (np. 90 BPM -> 166 ms na nutę)
    const stepMs = Math.max(70, Math.round(60000 / (state.arpBpm * 4)));
    arpPlaybackTimer = setTimeout(tick, stepMs);
  };

  tick();
}

function sendArpeggioToExerciseTab() {
  stopArpeggioPlayback(false);
  const steps = getActiveArpeggioSteps();
  if (!steps.length) return;
  const techMap = { H: 'Hammer-on', P: 'Pull-off', S: 'Slide' };
  state.recordedEvents = steps.map(st => ({
    s: st.s,
    f: st.f,
    midi: st.midi,
    note: st.note,
    dur: 2, // Szesnastka
    rest: false,
    tech: techMap[st.tech] || '—'
  }));
  state.tempoBpm = state.arpBpm;
  state.exerciseTitle = (state.arpViewMode === 'transition' && state.arpData && state.arpData.transition)
    ? state.arpData.transition.name
    : `Arpeggio ${state.arpData ? state.arpData.chordSymbol : state.chordId}`;
  state.exercisePlayerActive = false;
  switchPracticeSubtab('exercise');
}

function saveSettings() {
  try {
    const payload = {
      rootPc: state.rootPc,
      accMode: state.accMode,
      scaleId: state.scaleId,
      chordId: state.chordId,
      posSystem: state.posSystem,
      posIdx: state.posIdx,
      labelType: state.labelType,
      scaleRecognitionOn: state.scaleRecognitionOn,
      allChordTypes: state.allChordTypes,
      chordGroupFilter: state.chordGroupFilter,
      chordSharpness: state.chordSharpness,
      chordFilterByScale: state.chordFilterByScale,
      chordScaleExtraTypeFilter: state.chordScaleExtraTypeFilter,
      chordScaleRootPc: state.chordScaleRootPc,
      chordScaleId: state.chordScaleId,
      chordShowScaleOnFretboard: state.chordShowScaleOnFretboard,
      arpViewMode: state.arpViewMode,
      arpPattern: state.arpPattern,
      arpSweepStrings: state.arpSweepStrings,
      arpDirection: state.arpDirection,
      arpSeqStyle: state.arpSeqStyle,
      arpStringSet: state.arpStringSet,
      arpOctaves: state.arpOctaves,
      arpTransitionId: state.arpTransitionId,
      arpDiatonicScaleId: state.arpDiatonicScaleId,
      arpDiatonicScaleRootPc: state.arpDiatonicScaleRootPc,
      arpBpm: state.arpBpm,
      tuningId: state.tuningId,
      customStringMidis: state.customStringMidis,
      a4ReferenceHz: state.a4ReferenceHz,
      selectedAudioInputId: state.selectedAudioInputId,
      selectedAudioOutputId: state.selectedAudioOutputId,
      tunerMode: state.tunerMode,
      tunerDisplay: state.tunerDisplay,
      skillLevel: state.skillLevel,
      dailyGoalMinutes: state.dailyGoalMinutes,
      gamificationLevel: state.gamificationLevel,
      gamificationSounds: state.gamificationSounds,
      resolvingDominants: state.resolvingDominants,
      alteredDominantScale: state.alteredDominantScale,
      alteredVoicingIntensity: state.alteredVoicingIntensity,
      movingVoices: state.movingVoices,
      maxDisplayedFret: state.maxDisplayedFret,
      fretboardZoom: state.fretboardZoom,
      fretboardFitWidth: state.fretboardFitWidth,
      lockUnmarkedNotes: state.lockUnmarkedNotes,
      guitarTonePreset: state.guitarTonePreset,
      keyboardPreset: state.keyboardPreset,
      syncGuitarToKeyboard: state.syncGuitarToKeyboard,
      highlightPlayedOnGuitar: state.highlightPlayedOnGuitar,
      highlightScaleCardsOnPlay: state.highlightScaleCardsOnPlay,
      disabledScaleDegreePcs: state.disabledScaleDegreePcs,
      midiBaseOctave: state.midiBaseOctave,
      todayPracticedMinutes: state.todayPracticedMinutes,
      streakDays: state.streakDays,
      selectedProgressionIdx: state.selectedProgressionIdx,
      changesBpm: state.changesBpm,
      changesFeel: state.changesFeel,
      changesCompInst: state.changesCompInst,
      changesBass: state.changesBass,
      changesSwing: state.changesSwing,
      changesGroove: state.changesGroove,
      changesRhythmGuitar: state.changesRhythmGuitar,
      changesLoop: state.changesLoop,
      changesBackingBand: state.changesBackingBand,
      changesMetronomeClick: state.changesMetronomeClick,
      changesCycleKeys: state.changesCycleKeys,
      tempoBpm: state.tempoBpm,
      timeSigNum: state.timeSigNum,
      timeSigDen: state.timeSigDen,
      exerciseClef: state.exerciseClef,
      tabCreationEnabled: state.tabCreationEnabled,
      exerciseShowStaff: state.exerciseShowStaff,
      exerciseTabWrapRows: state.exerciseTabWrapRows,
      defaultDuration: state.defaultDuration,
      showExerciseNoteNames: state.showExerciseNoteNames,
      exerciseMinFret: state.exerciseMinFret,
      exerciseMaxFret: state.exerciseMaxFret,
      scalePracticeMode: state.scalePracticeMode,
      spPosSystem: state.spPosSystem,
      spStartString: state.spStartString,
      spDirection: state.spDirection,
      spOctaves: state.spOctaves,
      spPattern: state.spPattern,
      spUseHeadphones: state.spUseHeadphones,
      spMetronome: state.spMetronome,
      spRefNote: state.spRefNote,
      spBgChord: state.spBgChord
    };
    const jsonStr = JSON.stringify(payload);
    localStorage.setItem('apka_gitara_settings_v1', jsonStr);
    fetch('/api/save_settings?data=' + encodeURIComponent(jsonStr)).catch(() => {});
  } catch (e) {}
}

async function loadSettings() {
  try {
    const rawLocal = localStorage.getItem('apka_gitara_settings_v1');
    if (rawLocal) {
      Object.assign(state, JSON.parse(rawLocal));
    }
  } catch (e) {}
  try {
    const diskData = await fetchJson('/api/settings');
    if (diskData && Object.keys(diskData).length > 0) {
      Object.assign(state, diskData);
    }
  } catch (e) {}
}

function setAlteredIntensity(id, val) {
  state.alteredVoicingIntensity[id] = val;
  saveSettings();
  renderStageAndInspector();
}

function setAllAlteredIntensity(val) {
  Object.keys(state.alteredVoicingIntensity).forEach(k => {
    state.alteredVoicingIntensity[k] = val;
  });
  saveSettings();
  renderStageAndInspector();
}

function selectSearchScale(idx) {
  state.selectedSearchScaleIdx = (state.selectedSearchScaleIdx === idx) ? -1 : idx;
  drawFretboard();
  renderStageAndInspector();
}

function toggleTunerStringLock(e, strIdx, midi) {
  if (e) {
    e.preventDefault();
    e.stopPropagation();
  }
  state.isHoldingFretNote = true;
  state.detectedMidi = midi;
  startGuitarSustainedVoice('fret_mouse', midiToFreq(midi), 18.0);
  updateGuitarToKeyboardSync();
  if (state.tunerLockedString === strIdx) {
    state.tunerLockedString = -1;
  } else {
    state.tunerLockedString = strIdx;
  }
  renderStageAndInspector();
  renderTunerCanvas();
}

let strobePhase = 0;
function renderTunerCanvas() {
  const c = document.getElementById('tunerArcCanvas');
  if (!c) return;
  const ctx = c.getContext('2d');
  const W = c.width, H = c.height;
  ctx.clearRect(0, 0, W, H);

  const cx = W / 2;
  const cy = H - 55;
  const R = 195;

  // Łuk skali (-50 ct .. +50 ct) jak w klasycznym stroiku gitarowym / efektach
  ctx.strokeStyle = '#26201b';
  ctx.lineWidth = 10;
  ctx.lineCap = 'round';
  ctx.beginPath();
  ctx.arc(cx, cy, R, Math.PI, 2 * Math.PI);
  ctx.stroke();

  // Zielona strefa idealnego stroju u góry (-7 ct .. +7 ct)
  ctx.strokeStyle = 'rgba(74, 222, 128, 0.28)';
  ctx.lineWidth = 18;
  ctx.beginPath();
  ctx.arc(cx, cy, R, 1.5 * Math.PI - 0.22, 1.5 * Math.PI + 0.22);
  ctx.stroke();

  // Podziałka kresek (-50 .. +50 co 5 centów)
  for (let ct = -50; ct <= 50; ct += 5) {
    const angle = Math.PI + ((ct + 50) / 100) * Math.PI;
    const isMajor = (ct % 25 === 0);
    const rIn = R - (isMajor ? 18 : 10);
    const rOut = R - 2;
    ctx.strokeStyle = (ct === 0) ? '#4ade80' : (isMajor ? '#d6cec2' : '#574c43');
    ctx.lineWidth = (ct === 0) ? 3 : (isMajor ? 2 : 1.2);
    ctx.beginPath();
    ctx.moveTo(cx + Math.cos(angle) * rIn, cy + Math.sin(angle) * rIn);
    ctx.lineTo(cx + Math.cos(angle) * rOut, cy + Math.sin(angle) * rOut);
    ctx.stroke();
  }

  ctx.fillStyle = '#8c8277';
  ctx.font = '12px Segoe UI';
  ctx.textAlign = 'center';
  ctx.fillText('-50¢', cx - R - 8, cy + 20);
  ctx.fillText('+50¢', cx + R + 8, cy + 20);

  // Jeśli wybrano tryb Strobo, rysujemy dodatkowo obracający się pierścień stroboskopowy wzdłuż łuku
  if (state.tunerDisplay === 'strobe' && state.detectedMidi !== null) {
    strobePhase = (strobePhase + state.detectedCents * 0.008) % (Math.PI / 6);
    ctx.strokeStyle = Math.abs(state.detectedCents) <= 7.0 ? '#4ade80' : '#f39237';
    ctx.lineWidth = 6;
    for (let a = Math.PI + 0.08; a < 2 * Math.PI - 0.08; a += Math.PI / 10) {
      const a1 = a + strobePhase;
      ctx.beginPath();
      ctx.arc(cx, cy, R + 14, a1, a1 + 0.12);
      ctx.stroke();
    }
  }

  // Wskazówka (igła)
  const clampedCents = (state.detectedMidi !== null)
    ? Math.max(-50, Math.min(50, state.detectedCents))
    : 0;
  const needleAngle = Math.PI + ((clampedCents + 50) / 100) * Math.PI;
  const inTune = (state.detectedMidi !== null && Math.abs(state.detectedCents) <= 7.0);

  ctx.strokeStyle = inTune ? '#4ade80' : '#9ca3af';
  ctx.lineWidth = 3;
  ctx.beginPath();
  ctx.moveTo(cx, cy);
  ctx.lineTo(cx + Math.cos(needleAngle) * (R - 18), cy + Math.sin(needleAngle) * (R - 18));
  ctx.stroke();

  // zielony grot na szczycie przy trafieniu w 0¢
  if (inTune) {
    ctx.strokeStyle = '#4ade80';
    ctx.lineWidth = 4;
    ctx.beginPath();
    ctx.moveTo(cx + Math.cos(needleAngle) * (R - 26), cy + Math.sin(needleAngle) * (R - 26));
    ctx.lineTo(cx + Math.cos(needleAngle) * R, cy + Math.sin(needleAngle) * R);
    ctx.stroke();
  }

  // Oś obrotu igły
  ctx.fillStyle = '#9ca3af';
  ctx.beginPath();
  ctx.arc(cx, cy, 7, 0, Math.PI * 2);
  ctx.fill();

  // Wyświetlacz nuty pod osią
  const readout = document.getElementById('tunerNoteReadout');
  if (state.detectedMidi !== null) {
    const labels = getRootLabels();
    const noteStr = labels[((state.detectedMidi % 12) + 12) % 12] + (Math.floor(state.detectedMidi / 12) - 1);
    ctx.fillStyle = inTune ? '#4ade80' : '#f5f2eb';
    ctx.font = 'bold 28px Segoe UI';
    ctx.fillText(noteStr, cx, cy + 42);
    if (readout) {
      readout.textContent = `${state.detectedCents >= 0 ? '+' : ''}${state.detectedCents.toFixed(1)} ¢ · ${state.detectedHz.toFixed(1)} Hz`;
    }
  } else {
    ctx.fillStyle = '#78716c';
    ctx.fillRect(cx - 22, cy + 28, 44, 10);
    if (readout) readout.textContent = 'Zagraj strunę...';
  }
}

let changesPlaybackTimer = null;

const changesChordIntervalsMap = {
  'major': [0, 4, 7],
  'minor': [0, 3, 7],
  '7':     [0, 4, 7, 10],
  'maj7':  [0, 4, 7, 11],
  'm7':    [0, 3, 7, 10],
  'm7b5':  [0, 3, 6, 10],
  'dim7':  [0, 3, 6, 9],
  '7b9':   [0, 4, 7, 10, 1]
};

function ensureChangesProgressionLoaded(forceReload = false) {
  if (!state.initData || !state.initData.progressions) return;
  if (state.changesMeasures && !forceReload) return;
  const p = state.initData.progressions[state.selectedProgressionIdx] || state.initData.progressions[0];
  if (!p) return;
  state.changesKeyRootPc = (p.keyRootPc !== undefined) ? p.keyRootPc : 9;
  state.changesIsMinor = !!p.isMinor;
  state.changesBpm = p.bpm || 108;
  state.changesFeel = (p.style === 'Blues') ? 'Blues' : (p.style === 'Jazz' ? 'Swing Jazz' : 'Prosto');
  state.changesCompInst = (p.style === 'Blues') ? 'hammond' : 'clean';
  state.changesSwing = (p.swing !== undefined) ? p.swing : 0;
  state.changesGroove = (p.groove === 1) ? 'shuffle' : 'slow12_8';
  state.changesCycleKeys = !!p.cycleKeys;
  state.changesMeasures = (p.bars || []).map(b => ({
    volta: 0,
    beats: [
      {
        offsetSemi: (b.offsetSemi !== undefined) ? b.offsetSemi : 0,
        chordId: b.chordId || '7',
        roman: b.roman || 'I'
      },
      null,
      null,
      null
    ]
  }));
}

function selectChangesProgression(idx) {
  stopChangesPlayback();
  state.selectedProgressionIdx = idx;
  state.changesScreen = 'editor';
  ensureChangesProgressionLoaded(true);
  renderAll();
}

function resolveChangesSlot(slot) {
  if (!slot) return null;
  const labels = getRootLabels();
  const rootPc = ((state.changesKeyRootPc + (slot.offsetSemi || 0)) % 12 + 12) % 12;
  const rootName = labels[rootPc];
  const suffixMap = {
    'major': '',
    'minor': 'm',
    '7': '7',
    'maj7': 'maj7',
    'm7': 'm7',
    'm7b5': 'm7♭5',
    'dim7': 'dim7',
    '7b9': '7♭9'
  };
  const suffix = (suffixMap[slot.chordId] !== undefined) ? suffixMap[slot.chordId] : slot.chordId;
  const symbol = rootName + suffix;
  const rootMidi = 48 + rootPc; // C3..B3
  const ivs = changesChordIntervalsMap[slot.chordId] || [0, 4, 7];
  const pcs = ivs.map(iv => (rootPc + iv) % 12);
  return {
    symbol,
    roman: slot.roman || 'I',
    rootPc,
    rootMidi,
    chordId: slot.chordId,
    intervals: ivs,
    pcs
  };
}

function playMetronomeTickAudio(isDownbeat) {
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const osc = ctx.createOscillator();
  const g = ctx.createGain();
  osc.type = 'triangle';
  osc.frequency.setValueAtTime(isDownbeat ? 1150 : 780, now);
  g.gain.setValueAtTime(0.18, now);
  g.gain.exponentialRampToValueAtTime(0.001, now + 0.045);
  osc.connect(g);
  g.connect(getMasterAudioOut());
  osc.start(now);
  osc.stop(now + 0.05);
}

function playChangesBackingStepAudio(resolvedChord, beatIdx) {
  if (!resolvedChord) return;
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;

  if (state.changesMetronomeClick) {
    playMetronomeTickAudio(beatIdx === 0);
  }

  // Na 1. mierze taktu (lub przy uderzeniu akompaniamentu) zagraj akord wybranym instrumentem z zapasem headroomu
  if (beatIdx === 0 || (state.changesRhythmGuitar && beatIdx === 2)) {
    const dur = (beatIdx === 0) ? 0.95 : 0.45;
    const gainScale = 1.0 / Math.sqrt(Math.max(1, resolvedChord.intervals.length));
    resolvedChord.intervals.forEach((iv, i) => {
      const m = resolvedChord.rootMidi + iv;
      const freq = midiToFreq(m);
      setTimeout(() => {
        if (state.changesCompInst === 'clean' || state.changesCompInst === 'crunch') {
          const prevTone = state.guitarTonePreset;
          state.guitarTonePreset = state.changesCompInst;
          pluckGuitarFrequency(freq, dur, gainScale);
          state.guitarTonePreset = prevTone;
        } else {
          const prevKbd = state.keyboardPreset;
          state.keyboardPreset = state.changesCompInst;
          playKeyboardPresetNote(freq, dur, gainScale);
          state.keyboardPreset = prevKbd;
        }
      }, i * 22);
    });
  }

  // Linia basu (Zespół akompaniujący / Bas elektryczny)
  if (state.changesBackingBand && state.changesBass !== 'Wył.') {
    const walkOffsets = [0, 4, 7, 10];
    const semi = walkOffsets[beatIdx % walkOffsets.length] || 0;
    const bassMidi = (resolvedChord.rootMidi - 12) + semi;
    const osc = ctx.createOscillator();
    const filter = ctx.createBiquadFilter();
    const g = ctx.createGain();
    osc.type = (state.changesBass === 'Syntezator basowy') ? 'sawtooth' : 'triangle';
    osc.frequency.setValueAtTime(midiToFreq(bassMidi), now);
    filter.type = 'lowpass';
    filter.frequency.setValueAtTime(420, now);
    g.gain.setValueAtTime(0.32, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.42);
    osc.connect(filter);
    filter.connect(g);
    g.connect(getMasterAudioOut());
    osc.start(now);
    osc.stop(now + 0.44);
  }
}

function stopChangesPlayback() {
  if (changesPlaybackTimer) {
    clearInterval(changesPlaybackTimer);
    changesPlaybackTimer = null;
  }
  state.changesIsPlaying = false;
  state.changesIsPaused = false;
  state.changesActiveBarIdx = -1;
  state.changesActiveBeatIdx = -1;
  state.changesCountIn = 0;
}

function updateChangesHighlightDom() {
  // Bez pełnego przerenderowania DOM podświetl aktywny takt w Zapisie Akordowym i aktywną miarę w Aranżacji!
  document.querySelectorAll('.changes-chart-card').forEach((el, idx) => {
    if (idx === state.changesActiveBarIdx && state.changesIsPlaying) {
      el.classList.add('active-playing');
    } else {
      el.classList.remove('active-playing');
    }
  });
  document.querySelectorAll('.changes-arr-bar').forEach((el, idx) => {
    if (idx === state.changesActiveBarIdx && state.changesIsPlaying) {
      el.classList.add('active-playing');
    } else {
      el.classList.remove('active-playing');
    }
  });
  document.querySelectorAll('.changes-beat-slot').forEach(el => {
    const b = parseInt(el.getAttribute('data-bar'), 10);
    const bt = parseInt(el.getAttribute('data-beat'), 10);
    if (state.changesIsPlaying && b === state.changesActiveBarIdx && bt === state.changesActiveBeatIdx) {
      el.classList.add('active-beat');
    } else {
      el.classList.remove('active-beat');
    }
  });
}

function toggleChangesEditorPlayback() {
  if (state.changesIsPlaying) {
    stopChangesPlayback();
    renderStageAndInspector();
    return;
  }
  ensureChangesProgressionLoaded();
  if (!state.changesMeasures || state.changesMeasures.length === 0) return;

  state.changesIsPlaying = true;
  state.changesIsPaused = false;
  state.changesActiveBarIdx = 0;
  state.changesActiveBeatIdx = 0;
  renderStageAndInspector();

  const fireStep = () => {
    if (!state.changesIsPlaying || state.changesIsPaused) return;
    const bar = state.changesMeasures[state.changesActiveBarIdx];
    if (!bar) return;
    const activeSlot = bar.beats[state.changesActiveBeatIdx] || bar.beats[0];
    const resolved = resolveChangesSlot(activeSlot);
    playChangesBackingStepAudio(resolved, state.changesActiveBeatIdx);
    updateChangesHighlightDom();

    state.changesActiveBeatIdx++;
    if (state.changesActiveBeatIdx >= 4) {
      state.changesActiveBeatIdx = 0;
      state.changesActiveBarIdx++;
      if (state.changesActiveBarIdx >= state.changesMeasures.length) {
        if (state.changesCycleKeys) {
          state.changesKeyRootPc = (state.changesKeyRootPc + 5) % 12; // Koło kwartowe
        }
        if (state.changesLoop) {
          state.changesActiveBarIdx = 0;
          if (state.changesCycleKeys) renderStageAndInspector();
        } else {
          stopChangesPlayback();
          renderStageAndInspector();
        }
      }
    }
  };

  fireStep();
  const beatMs = Math.max(180, Math.round(60000 / Math.max(40, state.changesBpm)));
  changesPlaybackTimer = setInterval(fireStep, beatMs);
}

function addChordToChangesMeasure(barIdx, targetBeatIdx = -1) {
  ensureChangesProgressionLoaded();
  const bar = state.changesMeasures[barIdx];
  if (!bar) return;
  const palette = [
    { offsetSemi: 0, chordId: state.changesIsMinor ? 'minor' : '7', roman: state.changesIsMinor ? 'i' : 'I7' },
    { offsetSemi: 5, chordId: state.changesIsMinor ? 'minor' : '7', roman: state.changesIsMinor ? 'iv' : 'IV7' },
    { offsetSemi: 7, chordId: '7', roman: 'V7' },
    { offsetSemi: 9, chordId: 'm7', roman: 'vim7' },
    { offsetSemi: 2, chordId: 'm7', roman: 'iim7' }
  ];
  let beatIdx = targetBeatIdx;
  if (beatIdx < 0) {
    beatIdx = bar.beats.findIndex((b, idx) => idx > 0 && !b);
    if (beatIdx < 0) beatIdx = 2;
  }
  const cur = bar.beats[beatIdx];
  if (!cur) {
    bar.beats[beatIdx] = Object.assign({}, palette[1]);
  } else {
    const nextIdx = (palette.findIndex(p => p.offsetSemi === cur.offsetSemi && p.chordId === cur.chordId) + 1) % palette.length;
    bar.beats[beatIdx] = Object.assign({}, palette[nextIdx]);
  }
  const ch = resolveChangesSlot(bar.beats[beatIdx]);
  if (ch) playChangesBackingStepAudio(ch, 0);
  renderStageAndInspector();
}

function reharmonizeChangesProgression() {
  ensureChangesProgressionLoaded();
  state.changesMeasures.forEach((bar, idx) => {
    const first = bar.beats[0];
    if (!first) return;
    if (first.offsetSemi === 7 && first.chordId === '7') {
      // Wstaw ii–V lub substytucję trytonową ♭II7 w miejsce V7
      bar.beats[0] = { offsetSemi: 2, chordId: state.changesIsMinor ? 'm7b5' : 'm7', roman: state.changesIsMinor ? 'iim7♭5' : 'iim7' };
      bar.beats[2] = { offsetSemi: 1, chordId: '7', roman: '♭II7 (SubV)' };
    } else if (first.chordId === 'major') {
      first.chordId = 'maj7';
      first.roman = first.roman + 'maj7';
    } else if (first.chordId === 'minor') {
      first.chordId = 'm7';
      first.roman = first.roman + '7';
    } else if (idx % 4 === 3 && first.offsetSemi === 0) {
      bar.beats[2] = { offsetSemi: 7, chordId: '7b9', roman: 'V7♭9' };
    }
  });
  renderStageAndInspector();
}

function saveChangesToWorkshop() {
  const progs = state.initData.progressions;
  const activeProg = progs[state.selectedProgressionIdx] || progs[0];
  const labels = getRootLabels();
  const title = `${activeProg.title} (${labels[state.changesKeyRootPc]} ${state.changesIsMinor ? 'moll' : 'dur'} · ${state.changesBpm} BPM)`;
  state.initData.sessions.push({
    id: 'ses_chg_' + Date.now(),
    name: title,
    breakBars: 1
  });
  alert(`Zapisano „${title}” w Warsztacie!`);
}

function startChangesLivePractice() {
  stopChangesPlayback();
  ensureChangesProgressionLoaded();
  state.changesScreen = 'practice_active';
  state.changesIsPlaying = true;
  state.changesIsPaused = false;
  state.changesCountIn = 3;
  state.changesActiveBarIdx = 0;
  state.changesActiveBeatIdx = 0;
  state.changesHits = 0;
  state.changesMisses = 0;
  state.changesCleanCount = 0;
  state.changesAlmostCount = 0;
  state.changesCloseCount = 0;
  state.changesEvaluatedBar = -1;
  renderAll();

  const beatMs = Math.max(220, Math.round(60000 / Math.max(40, state.changesBpm)));
  changesPlaybackTimer = setInterval(() => {
    if (!state.changesIsPlaying || state.changesIsPaused) return;
    if (state.changesCountIn > 0) {
      playMetronomeTickAudio(state.changesCountIn === 3);
      state.changesCountIn--;
      renderStageAndInspector();
      return;
    }

    const bar = state.changesMeasures[state.changesActiveBarIdx];
    const slot = bar ? (bar.beats[state.changesActiveBeatIdx] || bar.beats[0]) : null;
    const resolved = resolveChangesSlot(slot);
    if (state.changesBackingBand) {
      playChangesBackingStepAudio(resolved, state.changesActiveBeatIdx);
    } else if (state.changesMetronomeClick) {
      playMetronomeTickAudio(state.changesActiveBeatIdx === 0);
    }
    renderStageAndInspector();

    state.changesActiveBeatIdx++;
    if (state.changesActiveBeatIdx >= 4) {
      state.changesActiveBeatIdx = 0;
      state.changesActiveBarIdx++;
      if (state.changesActiveBarIdx >= state.changesMeasures.length) {
        if (state.changesCycleKeys) {
          state.changesKeyRootPc = (state.changesKeyRootPc + 5) % 12;
          state.changesActiveBarIdx = 0;
          renderStageAndInspector();
        } else {
          finishChangesLivePractice();
        }
      }
    }
  }, beatMs);
}

function finishChangesLivePractice() {
  stopChangesPlayback();
  state.changesScreen = 'practice_summary';
  renderAll();
}

function getActiveChangesPracticeTargetPcs() {
  if (!state.changesMeasures || state.changesActiveBarIdx < 0) return new Set();
  const bar = state.changesMeasures[state.changesActiveBarIdx] || state.changesMeasures[0];
  if (!bar) return new Set();
  const slot = bar.beats[state.changesActiveBeatIdx] || bar.beats[0];
  const resolved = resolveChangesSlot(slot);
  if (!resolved) return new Set();

  if (state.changesPracticeDegrees === '1') return new Set([resolved.rootPc]);
  if (state.changesPracticeDegrees === '3' && resolved.pcs.length > 1) return new Set([resolved.pcs[1]]);
  if (state.changesPracticeDegrees === '5' && resolved.pcs.length > 2) return new Set([resolved.pcs[2]]);

  if (state.changesPracticeGoal === 'chord_scale') {
    // Skala akordu (np. miksolidyjska dla 7, molowa dla m / m7, durowa dla maj)
    const scaleOffsets = (resolved.chordId === '7') ? [0, 2, 4, 5, 7, 9, 10]
                       : (resolved.chordId.startsWith('m') && resolved.chordId !== 'maj7') ? [0, 2, 3, 5, 7, 8, 10]
                       : [0, 2, 4, 5, 7, 9, 11];
    return new Set(scaleOffsets.map(o => (resolved.rootPc + o) % 12));
  }
  return new Set(resolved.pcs);
}

function checkChangesPracticePlayedMidi(playedMidi) {
  if (state.mainView !== 'changes' || state.changesScreen !== 'practice_active') return;
  if (state.changesCountIn > 0) return;
  const pc = ((playedMidi % 12) + 12) % 12;
  const targets = getActiveChangesPracticeTargetPcs();
  if (targets.has(pc)) {
    state.changesHits++;
    state.changesCleanCount++;
  } else {
    state.changesMisses++;
    state.changesCloseCount++;
  }
  renderStageAndInspector();
}

function buildChangesLiveFretboardSvg(resolvedChord) {
  const W = 1280, H = 225;
  const leftNutX = 52, rightPad = 18, topPad = 22, bottomPad = 32;
  const maxFret = 21;
  const boardW = W - leftNutX - rightPad;
  const boardH = H - topPad - bottomPad;
  const fretW = boardW / maxFret;
  const stringH = boardH / 5;
  const targets = getActiveChangesPracticeTargetPcs();
  const rootPc = resolvedChord ? resolvedChord.rootPc : state.changesKeyRootPc;
  const labels = getRootLabels();

  let svg = `<svg width="100%" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg">`;
  svg += `<rect x="${leftNutX}" y="${topPad - 8}" width="${boardW}" height="${boardH + 16}" fill="#14100d" rx="4"/>`;

  for (let f = 1; f <= maxFret; f++) {
    const x = leftNutX + f * fretW;
    svg += `<line x1="${x}" y1="${topPad - 8}" x2="${x}" y2="${topPad + boardH + 8}" stroke="#2b231d" stroke-width="1.4"/>`;
    if ([3,5,7,9,12,15,17,19,21].includes(f)) {
      svg += `<text x="${leftNutX + (f - 0.5) * fretW}" y="${H - 6}" fill="#8a8178" font-size="11" text-anchor="middle">${f}</text>`;
    }
  }
  svg += `<line x1="${leftNutX}" y1="${topPad - 8}" x2="${leftNutX}" y2="${topPad + boardH + 8}" stroke="#8c8175" stroke-width="2.4"/>`;

  for (let s = 0; s < 6; s++) {
    const row = 5 - s;
    const y = topPad + row * stringH;
    const openNm = labels[((state.customStringMidis[s] % 12) + 12) % 12];
    svg += `<text x="20" y="${y + 4}" fill="#9ca3af" font-size="11" font-weight="700" text-anchor="middle">${openNm}</text>`;
    svg += `<line x1="${leftNutX}" y1="${y}" x2="${leftNutX + boardW}" y2="${y}" stroke="#574b40" stroke-width="${(1.1 + row * 0.22).toFixed(2)}"/>`;

    for (let f = 0; f <= maxFret; f++) {
      const midi = state.customStringMidis[s] + f;
      const pc = ((midi % 12) + 12) % 12;
      const cx = (f === 0) ? (leftNutX * 0.62) : (leftNutX + (f - 0.5) * fretW);
      if (!targets.has(pc)) continue;
      const isRoot = (pc === rootPc);
      const fill = isRoot ? '#f39237' : '#3d2a1b';
      const stroke = isRoot ? '#fde68a' : '#8a6240';
      const txtCol = isRoot ? '#14100d' : '#f5f2eb';
      const nm = labels[pc];
      svg += `
        <g style="cursor:pointer" onmousedown="startGuitarSustainedVoice('fret_mouse', ${midiToFreq(midi)}, 10);state.isHoldingFretNote=true;checkChangesPracticePlayedMidi(${midi})">
          <circle cx="${cx}" cy="${y}" r="12" fill="${fill}" stroke="${stroke}" stroke-width="1.5"/>
          <text x="${cx}" y="${y + 4}" fill="${txtCol}" font-size="10.5" font-weight="800" text-anchor="middle">${nm}</text>
        </g>`;
    }
  }
  svg += `</svg>`;
  return svg;
}

function renderChangesView(stage, insp) {
  const progs = state.initData.progressions;
  const activeProg = progs[state.selectedProgressionIdx] || progs[0];
  const labels = getRootLabels();
  const keyName = `${labels[state.changesKeyRootPc]} ${state.changesIsMinor ? 'moll' : 'dur'}`;

  // =========================================================================
  // EKRAN 5: SESJA ZAKOŃCZONA (Zrzut 5)
  // =========================================================================
  if (state.changesScreen === 'practice_summary') {
    const total = state.changesHits + state.changesMisses;
    const accStr = total > 0 ? `${Math.round((state.changesHits / total) * 100)}%` : '—';
    const subMsg = total > 0
      ? `Oceniono dźwięków: ${total}. Postęp zapisany!`
      : 'Brak ocenionych dźwięków. Nic nie zapisano.';
    stage.innerHTML = `
      <div style="flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:14px;padding:40px 20px;">
        <div style="font-size:12px;letter-spacing:0.12em;color:var(--text-muted);font-weight:700;">SESJA ZAKOŃCZONA</div>
        <div style="font-size:36px;font-weight:800;color:var(--accent);">${accStr}</div>
        <div style="font-size:11px;letter-spacing:0.1em;color:var(--text-muted);font-weight:700;">CELNOŚĆ</div>
        <div style="font-size:13px;color:var(--text-muted);">${state.changesCleanCount} czyste · ${state.changesAlmostCount} prawie · ${state.changesCloseCount} blisko</div>
        <div style="font-size:12px;color:var(--text-muted);margin-bottom:24px;">${subMsg}</div>
        <button class="action-btn" style="width:100%;max-width:980px;padding:15px;font-size:15px;"
                onclick="startChangesLivePractice()">Ćwicz ponownie</button>
        <button class="chip-btn" style="width:100%;max-width:980px;padding:15px;font-size:15px;background:#16120f;"
                onclick="state.changesScreen='editor';renderAll()">Wstecz</button>
      </div>`;
    return;
  }

  // =========================================================================
  // EKRAN 4: AKTYWNE GRANIE NA ZMIANACH Z PODŚWIETLANIEM AKORDÓW I GRYFEM (Zrzut 4)
  // =========================================================================
  if (state.changesScreen === 'practice_active') {
    const activeBar = state.changesMeasures[state.changesActiveBarIdx] || state.changesMeasures[0];
    const activeSlot = activeBar ? (activeBar.beats[state.changesActiveBeatIdx] || activeBar.beats[0]) : null;
    const currentResolved = resolveChangesSlot(activeSlot);
    const statusBadge = state.changesCountIn > 0
      ? `Odliczanie ${state.changesCountIn}`
      : `Takt ${state.changesActiveBarIdx + 1} · Miara ${state.changesActiveBeatIdx + 1}`;
    const centerHeadline = state.changesCountIn > 0
      ? 'Przygotuj się...'
      : (currentResolved ? `Teraz graj: ${currentResolved.symbol} (${currentResolved.roman})` : 'Graj!');
    const targetNames = Array.from(getActiveChangesPracticeTargetPcs()).map(pc => labels[pc]).join(' · ');

    stage.innerHTML = `
      <div style="display:flex;flex-direction:column;gap:14px;flex:1;">
        <!-- Górny pasek: Pauza / Stop / Pulpit + Odliczanie i licznik trafień -->
        <div style="display:flex;justify-content:space-between;align-items:center;">
          <div style="display:flex;gap:6px;">
            <button class="chip-btn" style="padding:7px 14px;"
                    onclick="state.changesIsPaused=!state.changesIsPaused;renderStageAndInspector()">
              ${state.changesIsPaused ? '▶ Wznów' : 'Pauza'}
            </button>
            <button class="action-btn" style="padding:7px 16px;" onclick="finishChangesLivePractice()">Stop</button>
            <button class="chip-btn" style="padding:7px 14px;" onclick="stopChangesPlayback();state.changesScreen='editor';renderAll()">Pulpit</button>
          </div>
          <div style="text-align:right;">
            <span style="background:rgba(243,146,55,0.18);color:var(--accent);border:1px solid var(--accent);padding:3px 10px;border-radius:6px;font-size:12px;font-weight:700;">
              ${statusBadge}
            </span>
            <div style="font-size:12px;color:var(--text-muted);margin-top:4px;font-family:monospace;">
              — · ${state.changesHits} traf. / ${state.changesMisses} błęd.
            </div>
          </div>
        </div>

        <!-- Siatka akordów progresji (podświetla na pomarańczowo aktualnie grany akord!) -->
        <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:8px;">
          ${state.changesMeasures.map((m, idx) => {
            const ch = resolveChangesSlot(m.beats[0]);
            const isNow = (state.changesCountIn === 0 && idx === state.changesActiveBarIdx);
            const borderStyle = isNow
              ? 'border:2px solid var(--accent);background:rgba(243,146,55,0.20);box-shadow:0 0 14px rgba(243,146,55,0.35);'
              : 'border:1px solid #26201b;background:#0b0908;';
            return `
              <div style="${borderStyle}border-radius:7px;padding:12px 14px;display:flex;justify-content:space-between;align-items:center;">
                <span style="font-size:15px;font-weight:800;color:${isNow?'var(--accent)':'#f5f2eb'};">${ch ? ch.symbol : '—'}</span>
                <span style="font-size:11px;color:var(--text-muted);">${ch ? ch.roman : ''}</span>
              </div>`;
          }).join('')}
        </div>

        <!-- Środkowy komunikat i podpowiedź dźwięków docelowych -->
        <div style="flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;min-height:110px;">
          <div style="font-size:22px;font-weight:800;">${centerHeadline}</div>
          <div style="font-size:13px;color:var(--accent);">Dźwięki docelowe: <b>${targetNames}</b></div>
          <div style="font-size:12px;color:var(--text-muted);">Zagraj na gitarze (mikrofon), kliknij w gryf poniżej lub użyj klawiatury na dole</div>
        </div>

        <!-- Przycisk Gryf i dolny interaktywny gryf podświetlający dźwięki aktualnego akordu -->
        <div>
          <button class="chip-btn ${state.changesPracticeShowFretboard?'active':''}" style="padding:5px 12px;margin-bottom:6px;"
                  onclick="state.changesPracticeShowFretboard=!state.changesPracticeShowFretboard;renderStageAndInspector()">Gryf</button>
          ${state.changesPracticeShowFretboard ? `
            <div style="background:#120e0b;border:1px solid var(--border);border-radius:10px;padding:8px;overflow-x:auto;">
              ${buildChangesLiveFretboardSvg(currentResolved)}
            </div>
          ` : ''}
        </div>
      </div>`;
    return;
  }

  // =========================================================================
  // EKRAN 3: KONFIGURACJA „< ĆWICZENIE NA ZMIANACH” (Zrzut 3)
  // =========================================================================
  if (state.changesScreen === 'practice_config') {
    stage.innerHTML = `
      <div style="max-width:680px;margin:10px auto;width:100%;display:flex;flex-direction:column;gap:20px;">
        <div style="display:flex;align-items:center;gap:12px;">
          <button class="chip-btn" style="padding:6px 12px;" onclick="state.changesScreen='editor';renderAll()">‹</button>
          <span class="section-label" style="margin:0;font-size:13px;">ĆWICZENIE NA ZMIANACH</span>
        </div>
        <div style="font-size:15px;color:var(--text-muted);line-height:1.5;">
          Graj linię na swoim zapisie. Cele podświetlają się przy każdym akordzie, a każdy dźwięk oceniany jest na żywo.
        </div>

        <div>
          <div class="section-label">CEL</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeGoal==='chord_tones'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeGoal='chord_tones';renderStageAndInspector()">Dźwięki akordu</button>
            <button class="chip-btn ${state.changesPracticeGoal==='chord_scale'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeGoal='chord_scale';renderStageAndInspector()">Skala akordu</button>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB PRACY</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeMode==='tempo'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeMode='tempo';renderStageAndInspector()">W tempie</button>
            <button class="chip-btn ${state.changesPracticeMode==='study'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeMode='study';renderStageAndInspector()">Ćwiczebny</button>
          </div>
        </div>

        <div>
          <div class="section-label">DŹWIĘKI DO ZAGRANIA</div>
          <div style="display:inline-flex;gap:6px;">
            ${[
              {id:'all', lbl:'Wszystkie'},
              {id:'1',   lbl:'1'},
              {id:'3',   lbl:'3'},
              {id:'5',   lbl:'5'}
            ].map(d => `
              <button class="chip-btn ${state.changesPracticeDegrees===d.id?'active':''}" style="min-width:64px;padding:9px 16px;"
                      onclick="state.changesPracticeDegrees='${d.id}';renderStageAndInspector()">${d.lbl}</button>
            `).join('')}
          </div>
        </div>

        <div>
          <div class="section-label">KOLEJNOŚĆ</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeOrder==='any'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeOrder='any';renderStageAndInspector()">Dowolna</button>
            <button class="chip-btn ${state.changesPracticeOrder==='seq'?'active':''}" style="padding:9px 18px;"
                    onclick="state.changesPracticeOrder='seq';renderStageAndInspector()">Po kolei</button>
          </div>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;padding-top:6px;border-top:1px solid var(--border);">
          <div>
            <div style="font-size:15px;font-weight:700;">Przechodź przez tonacje</div>
            <div style="font-size:12px;color:var(--text-muted);margin-top:3px;">
              Ćwicz w każdej tonacji cyklu z wynikiem dla każdej z nich. Bez tego ćwiczysz w tonacji zapisu.
            </div>
          </div>
          <button class="ear-toggle-switch ${state.changesCycleKeys?'on':''}"
                  onclick="state.changesCycleKeys=!state.changesCycleKeys;renderStageAndInspector()"></button>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;">
          <div style="font-size:15px;font-weight:700;">Zespół podkładowy</div>
          <button class="ear-toggle-switch ${state.changesBackingBand?'on':''}"
                  onclick="state.changesBackingBand=!state.changesBackingBand;renderStageAndInspector()"></button>
        </div>

        <div style="padding:14px 16px;background:#16120f;border:1px solid var(--border);border-radius:10px;font-size:13px;color:var(--text-muted);">
          ${state.changesBackingBand
            ? 'Włączony: gra pełny zespół akompaniujący (bas, akordy i puls metronomu).'
            : 'Wyłączony: ćwiczenie działa tylko z mikrofonem i licznikiem na ekranie.'}
        </div>

        <button class="action-btn" style="width:100%;padding:15px;font-size:16px;"
                onclick="startChangesLivePractice()">Graj</button>
      </div>`;
    return;
  }

  // =========================================================================
  // EKRAN BIBLIOTEKI STANDARDÓW („< Biblioteka”)
  // =========================================================================
  if (state.changesScreen === 'library') {
    stage.innerHTML = `
      <div class="fretboard-card" style="padding:20px;gap:14px;">
        <div style="display:flex;justify-content:space-between;align-items:center;">
          <h2 style="font-size:22px;font-weight:800;">Biblioteka zmian i progresji</h2>
          <button class="chip-btn" style="padding:6px 14px;" onclick="state.changesScreen='editor';renderAll()">✕ Powrót do edytora</button>
        </div>
        <div style="display:grid;grid-template-columns:repeat(2,1fr);gap:10px;">
          ${progs.map((p, idx) => `
            <div class="list-row ${state.selectedProgressionIdx===idx?'selected':''}" style="padding:14px;"
                 onclick="selectChangesProgression(${idx})">
              <div>
                <div style="font-size:16px;font-weight:800;">${p.title}</div>
                <div style="font-size:12px;color:var(--text-muted);margin-top:3px;">Styl: ${p.style} · ${p.bpm} BPM · Taktów: ${(p.bars||[]).length}</div>
              </div>
              <span style="color:var(--accent);font-weight:700;">Wybierz →</span>
            </div>
          `).join('')}
        </div>
      </div>`;
    return;
  }

  // =========================================================================
  // EKRAN 1 & 2: GŁÓWNY EDYTOR I ODTWARZACZ PROGRESJI (Blues 12-taktowy / Kadencja andaluzyjska)
  // =========================================================================
  const compInstLabels = {
    'hammond': 'Organy',
    'clean': 'Gitara czysta',
    'piano': 'Fortepian',
    'rhodes': 'Pianino elektr.',
    'crunch': 'Gitara przesterowana'
  };
  const grooveLabel = (state.changesFeel === 'Blues')
    ? (state.changesGroove === 'shuffle' ? 'Shuffle' : 'Wolny 12/8')
    : state.changesFeel;
  const subBanner = `${state.changesFeel} · ${compInstLabels[state.changesCompInst] || 'Organy'} · ${state.changesBass}${state.changesLoop ? ' · W pętli' : ''}`;

  stage.innerHTML = `
    <div style="display:flex;flex-direction:column;gap:16px;">
      <!-- Górny pasek: < Biblioteka + szybki wybór standardu + tytuł -->
      <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;">
        <div style="display:flex;align-items:center;gap:12px;">
          <button class="chip-btn" style="padding:5px 12px;font-size:12px;" onclick="state.changesScreen='library';renderAll()">‹ Biblioteka</button>
          <select style="width:auto;padding:5px 10px;font-size:13px;font-weight:700;"
                  onchange="selectChangesProgression(parseInt(this.value,10))">
            ${progs.map((p, idx) => `<option value="${idx}" ${idx===state.selectedProgressionIdx?'selected':''}>${p.title}</option>`).join('')}
          </select>
        </div>
        <button class="chip-btn" style="padding:5px 12px;font-size:12px;"
                onclick="window.location.href='/api/export/progression_gp5?id=${activeProg.id}'">💾 Eksport .GP5 (TuxGuitar)</button>
      </div>

      <h1 style="font-size:28px;font-weight:800;margin-top:-4px;">${activeProg.title}</h1>

      <!-- Karta odtwarzacza (▶ Graj / Stop + opis podkładu + BPM) -->
      <div style="background:#171310;border:1px solid var(--border);border-radius:12px;padding:14px 18px;display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:12px;">
        <div style="display:flex;align-items:center;gap:16px;">
          <button class="action-btn" style="padding:12px 26px;font-size:16px;min-width:118px;"
                  onclick="toggleChangesEditorPlayback()">
            ${state.changesIsPlaying ? '⏹ Stop' : '▶ Graj'}
          </button>
          <div>
            <div style="font-size:16px;font-weight:800;">${grooveLabel} · ${keyName}</div>
            <div style="font-size:12px;color:var(--text-muted);margin-top:2px;">${subBanner}</div>
          </div>
        </div>
        <div style="display:flex;align-items:center;gap:6px;background:#110e0c;padding:6px 10px;border-radius:8px;border:1px solid var(--border);">
          <button class="chip-btn" style="padding:4px 10px;" onclick="state.changesBpm=Math.max(40,state.changesBpm-4);renderStageAndInspector()">−</button>
          <div style="text-align:center;min-width:48px;">
            <div style="font-size:15px;font-weight:800;">${state.changesBpm}</div>
            <div style="font-size:9px;color:var(--text-muted);">BPM</div>
          </div>
          <button class="chip-btn" style="padding:4px 10px;" onclick="state.changesBpm=Math.min(240,state.changesBpm+4);renderStageAndInspector()">+</button>
        </div>
      </div>

      <!-- ZAPIS AKORDOWY (podświetla aktywny akord podczas grania!) -->
      <div>
        <div class="section-label">ZAPIS AKORDOWY</div>
        <div style="font-size:12px;font-weight:700;color:var(--text-muted);margin-bottom:8px;">${state.changesSectionName}</div>
        <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:8px;">
          ${state.changesMeasures.map((m, barIdx) => {
            const ch = resolveChangesSlot(m.beats[0]);
            const isPlayingThis = (state.changesIsPlaying && state.changesActiveBarIdx === barIdx);
            const activeStyle = isPlayingThis
              ? 'border:2px solid var(--accent);background:rgba(243,146,55,0.22);box-shadow:0 0 14px rgba(243,146,55,0.35);'
              : 'border:1px solid #26201b;background:#0b0908;';
            return `
              <div class="changes-chart-card" style="${activeStyle}border-radius:8px;padding:11px 14px;cursor:pointer;transition:0.12s;"
                   onclick="state.changesActiveBarIdx=${barIdx};playChangesBackingStepAudio(resolveChangesSlot(state.changesMeasures[${barIdx}].beats[0]), 0);renderStageAndInspector()">
                <div style="font-size:15px;font-weight:800;color:${isPlayingThis?'var(--accent)':'#f5f2eb'};">${ch ? ch.symbol : '—'}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:2px;">${ch ? ch.roman : ''}</div>
              </div>`;
          }).join('')}
        </div>
      </div>

      <!-- ARANŻACJA (Edytor taktów i 4 miar w każdym takcie) -->
      <div>
        <div class="section-label">ARANŻACJA</div>
        <div style="background:#120f0c;border:1px solid var(--border);border-radius:12px;padding:16px;display:flex;flex-direction:column;gap:12px;">
          <!-- Nagłówek sekcji A + Powtórz -->
          <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;">
            <input type="text" value="${state.changesSectionName}"
                   style="flex:1;background:#0b0908;color:var(--text);border:1px solid var(--border);border-radius:7px;padding:8px 12px;font-weight:700;"
                   oninput="state.changesSectionName=this.value">
            <div style="display:flex;align-items:center;gap:8px;">
              <span class="section-label" style="margin:0;">POWTÓRZ</span>
              <select style="width:90px;padding:6px 8px;" onchange="state.changesRepeat=this.value">
                <option value="Wył." ${state.changesRepeat==='Wył.'?'selected':''}>Wył.</option>
                <option value="2x" ${state.changesRepeat==='2x'?'selected':''}>2x</option>
                <option value="4x" ${state.changesRepeat==='4x'?'selected':''}>4x</option>
              </select>
            </div>
          </div>

          <!-- Siatka 2-kolumnowa taktów (TAKT 1 .. TAKT N) -->
          <div style="display:grid;grid-template-columns:repeat(2,1fr);gap:10px;">
            ${state.changesMeasures.map((m, barIdx) => {
              const isPlayingBar = (state.changesIsPlaying && state.changesActiveBarIdx === barIdx);
              const barBorder = isPlayingBar ? 'border:1.5px solid var(--accent);background:rgba(243,146,55,0.08);' : 'border:1px solid #241e19;background:#0b0908;';
              return `
                <div class="changes-arr-bar" style="${barBorder}border-radius:9px;padding:10px 12px;display:flex;flex-direction:column;gap:8px;">
                  <div style="display:flex;justify-content:space-between;align-items:center;">
                    <span style="font-size:11px;font-weight:700;color:var(--text-muted);">
                      TAKT ${barIdx + 1} ${m.volta ? `· WOLTA ${m.volta}.` : ''}
                    </span>
                    <div style="display:flex;gap:6px;align-items:center;">
                      <button class="chip-btn" style="padding:3px 9px;font-size:11px;"
                              onclick="addChordToChangesMeasure(${barIdx}, -1)">+ Dodaj akord</button>
                      <button class="chip-btn" style="padding:3px 7px;font-size:11px;color:var(--text-muted);"
                              onclick="if(state.changesMeasures.length>1){state.changesMeasures.splice(${barIdx},1);renderStageAndInspector();}">✕</button>
                    </div>
                  </div>
                  <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:6px;">
                    ${[0,1,2,3].map(btIdx => {
                      const slot = m.beats[btIdx];
                      const ch = resolveChangesSlot(slot);
                      const isBeatNow = (isPlayingBar && state.changesActiveBeatIdx === btIdx);
                      const btStyle = isBeatNow
                        ? 'border:2px solid var(--accent);background:rgba(243,146,55,0.28);'
                        : (ch ? 'border:1px solid #3a3028;background:#1c1713;' : 'border:1px dashed #2e2620;background:transparent;');
                      return `
                        <div class="changes-beat-slot" data-bar="${barIdx}" data-beat="${btIdx}"
                             style="${btStyle}border-radius:6px;height:40px;display:flex;flex-direction:column;align-items:center;justify-content:center;cursor:pointer;"
                             onclick="addChordToChangesMeasure(${barIdx}, ${btIdx})">
                          ${ch ? `
                            <span style="font-size:12px;font-weight:800;color:${isBeatNow?'var(--accent)':'#f5f2eb'};">${ch.symbol}</span>
                            <span style="font-size:9.5px;color:var(--text-muted);">${ch.roman}</span>
                          ` : `
                            <span style="font-size:11px;color:#574c43;">${btIdx + 1}</span>
                          `}
                        </div>`;
                    }).join('')}
                  </div>
                </div>`;
            }).join('')}
          </div>

          <div style="display:flex;gap:8px;margin-top:4px;">
            <button class="chip-btn" style="padding:6px 12px;font-size:12px;"
                    onclick="state.changesMeasures.push({volta:0,beats:[{offsetSemi:0,chordId:state.changesIsMinor?'minor':'7',roman:state.changesIsMinor?'i':'I7'},null,null,null]});renderStageAndInspector()">+ Dodaj takt</button>
            <button class="chip-btn" style="padding:6px 12px;font-size:12px;"
                    onclick="if(state.changesMeasures.length>=2){state.changesMeasures[state.changesMeasures.length-2].volta=1;state.changesMeasures[state.changesMeasures.length-1].volta=2;renderStageAndInspector();}">+ Dodaj woltę</button>
          </div>
        </div>

        <div style="display:flex;gap:8px;margin-top:10px;">
          <button class="chip-btn" style="padding:7px 14px;font-size:12px;border-color:#573d26;color:var(--accent);"
                  onclick="state.changesMeasures.push(...state.changesMeasures.slice(0,4).map(m=>JSON.parse(JSON.stringify(m))));renderStageAndInspector()">+ Dodaj sekcję</button>
          <button class="chip-btn" style="padding:7px 14px;font-size:12px;"
                  onclick="reharmonizeChangesProgression()">✨ Reharmonizuj</button>
        </div>
      </div>

      <!-- Dolna karta: Ćwicz ten zapis / Albo zrób z niego ćwiczenie -->
      <div style="background:#14100d;border:1px solid var(--border);border-radius:12px;padding:20px;display:grid;grid-template-columns:1.1fr 1fr;gap:24px;align-items:center;">
        <div style="display:flex;flex-direction:column;gap:10px;">
          <h3 style="font-size:20px;font-weight:800;">Ćwicz ten zapis</h3>
          <div style="font-size:12.5px;color:var(--text-muted);line-height:1.45;">
            Graj z zespołem. Każdy akord podświetla swoje dźwięki docelowe, a każdy zagrany dźwięk jest oceniany.
          </div>
          <button class="action-btn" style="width:100%;padding:13px;font-size:15px;margin-top:4px;"
                  onclick="stopChangesPlayback();state.changesScreen='practice_config';renderAll()">Graj</button>
        </div>
        <div style="display:flex;flex-direction:column;gap:8px;">
          <div style="font-size:14px;font-weight:800;">Albo zrób z niego ćwiczenie</div>
          <div style="font-size:12px;color:var(--text-muted);line-height:1.4;">
            Jeden dźwięk docelowy na akord, na ekranie praktyki. Zapisz je, aby wrócić do niego z Warsztatu.
          </div>
          <div class="section-label" style="margin-top:4px;">CEL</div>
          <div style="display:flex;gap:8px;">
            <select style="flex:1;" onchange="state.changesPracticeGoal=this.value">
              <option value="chord_tones" ${state.changesPracticeGoal==='chord_tones'?'selected':''}>Dźwięki akordu</option>
              <option value="chord_scale" ${state.changesPracticeGoal==='chord_scale'?'selected':''}>Skala akordu</option>
            </select>
            <button class="chip-btn" style="padding:8px 18px;border-color:#573d26;color:var(--accent);"
                    onclick="stopChangesPlayback();state.changesScreen='practice_config';renderAll()">Ćwicz</button>
          </div>
          <button class="chip-btn" style="width:fit-content;padding:7px 14px;font-size:12px;margin-top:2px;"
                  onclick="saveChangesToWorkshop()">Zapisz do Warsztatu</button>
        </div>
      </div>
    </div>`;

  // Prawy panel inspektora: TONACJA GŁÓWNA, METRUM, USTAWIENIA ZESPOŁU
  insp.innerHTML = `
    <div>
      <div class="section-label">TONACJA GŁÓWNA</div>
      <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;margin-bottom:6px;">
        ${labels.map((lbl, pc) => `
          <button class="chip-btn ${state.changesKeyRootPc===pc?'active':''}" style="padding:8px 4px;"
                  onclick="state.changesKeyRootPc=${pc};renderStageAndInspector()">${lbl}</button>
        `).join('')}
      </div>
      <div class="btn-grid-2">
        <button class="chip-btn ${!state.changesIsMinor?'active':''}"
                onclick="state.changesIsMinor=false;renderStageAndInspector()">dur</button>
        <button class="chip-btn ${state.changesIsMinor?'active':''}"
                onclick="state.changesIsMinor=true;renderStageAndInspector()">moll</button>
      </div>
    </div>

    <div>
      <div class="section-label">METRUM</div>
      <div style="display:flex;gap:5px;flex-wrap:wrap;margin-bottom:6px;">
        ${['4/4','3/4','6/8','12/8','2/2'].map(ts => `
          <button class="chip-btn ${state.changesTimeSig===ts?'active':''}" style="padding:6px 10px;font-size:12px;"
                  onclick="state.changesTimeSig='${ts}';renderStageAndInspector()">${ts}</button>
        `).join('')}
      </div>
      <div style="font-size:11px;color:var(--text-muted);">Sekcje 1 · Akordy ${state.changesMeasures.length}</div>
    </div>

    <div style="border-top:1px solid var(--border);padding-top:10px;display:flex;flex-direction:column;gap:10px;">
      <div class="section-label">USTAWIENIA ZESPOŁU</div>

      <div>
        <div class="section-label">FEEL</div>
        <select onchange="state.changesFeel=this.value;renderStageAndInspector()">
          ${['Blues','Prosto','Swing Jazz','Funk','Rock'].map(f => `<option value="${f}" ${state.changesFeel===f?'selected':''}>${f}</option>`).join('')}
        </select>
      </div>

      <div>
        <div class="section-label">INSTRUMENT AKOMPANIAMENTU</div>
        <select onchange="state.changesCompInst=this.value;renderStageAndInspector()">
          <option value="hammond" ${state.changesCompInst==='hammond'?'selected':''}>Organy</option>
          <option value="clean" ${state.changesCompInst==='clean'?'selected':''}>Gitara czysta</option>
          <option value="piano" ${state.changesCompInst==='piano'?'selected':''}>Fortepian</option>
          <option value="rhodes" ${state.changesCompInst==='rhodes'?'selected':''}>Pianino elektr. (Rhodes)</option>
          <option value="crunch" ${state.changesCompInst==='crunch'?'selected':''}>Gitara przesterowana</option>
        </select>
      </div>

      <div>
        <div class="section-label">BAS</div>
        <select onchange="state.changesBass=this.value;renderStageAndInspector()">
          ${['Bas elektryczny','Kontrabas','Syntezator basowy','Wył.'].map(b => `<option value="${b}" ${state.changesBass===b?'selected':''}>${b}</option>`).join('')}
        </select>
      </div>

      <div>
        <div class="section-label">NATĘŻENIE SWINGU</div>
        <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:4px;">
          ${['Równo','Lekki','Średni','Mocny'].map((sLbl, sIdx) => `
            <button class="chip-btn ${state.changesSwing===sIdx?'active':''}" style="padding:6px 4px;font-size:11.5px;"
                    onclick="state.changesSwing=${sIdx};renderStageAndInspector()">${sLbl}</button>
          `).join('')}
        </div>
      </div>

      ${state.changesFeel === 'Blues' ? `
      <div>
        <div class="section-label">GROOVE</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.changesGroove==='shuffle'?'active':''}"
                  onclick="state.changesGroove='shuffle';renderStageAndInspector()">Shuffle</button>
          <button class="chip-btn ${state.changesGroove==='slow12_8'?'active':''}"
                  onclick="state.changesGroove='slow12_8';renderStageAndInspector()">Wolny 12/8</button>
        </div>
      </div>` : ''}

      <div>
        <div class="section-label">GITARA RYTMICZNA</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${!state.changesRhythmGuitar?'active':''}"
                  onclick="state.changesRhythmGuitar=false;renderStageAndInspector()">Wył.</button>
          <button class="chip-btn ${state.changesRhythmGuitar?'active':''}"
                  onclick="state.changesRhythmGuitar=true;renderStageAndInspector()">Wł.</button>
        </div>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <span style="font-size:12.5px;font-weight:700;">Pętla</span>
        <button class="ear-toggle-switch ${state.changesLoop?'on':''}"
                onclick="state.changesLoop=!state.changesLoop;renderStageAndInspector()"></button>
      </div>

      <div>
        <div class="section-label">WIDOK</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.changesViewMode==='follow'?'active':''}"
                  onclick="state.changesViewMode='follow';renderStageAndInspector()">Śledzenie</button>
          <button class="chip-btn ${state.changesViewMode==='full'?'active':''}"
                  onclick="state.changesViewMode='full';renderStageAndInspector()">Pełny układ</button>
        </div>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <span style="font-size:12.5px;font-weight:700;">Diagramy akordów</span>
        <button class="ear-toggle-switch ${state.changesChordDiagrams?'on':''}"
                onclick="state.changesChordDiagrams=!state.changesChordDiagrams;renderStageAndInspector()"></button>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <div>
          <div style="font-size:12.5px;font-weight:700;">Zespół akompaniujący</div>
          <div style="font-size:10.5px;color:var(--text-muted);">Kroczący bas i akompaniament</div>
        </div>
        <button class="ear-toggle-switch ${state.changesBackingBand?'on':''}"
                onclick="state.changesBackingBand=!state.changesBackingBand;renderStageAndInspector()"></button>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <div>
          <div style="font-size:12.5px;font-weight:700;">Klik metronomu</div>
          <div style="font-size:10.5px;color:var(--text-muted);">Puls rytmiczny w każdym takcie</div>
        </div>
        <button class="ear-toggle-switch ${state.changesMetronomeClick?'on':''}"
                onclick="state.changesMetronomeClick=!state.changesMetronomeClick;renderStageAndInspector()"></button>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <span style="font-size:12.5px;font-weight:700;">Przechodź przez tonacje</span>
        <button class="ear-toggle-switch ${state.changesCycleKeys?'on':''}"
                onclick="state.changesCycleKeys=!state.changesCycleKeys;renderStageAndInspector()"></button>
      </div>

      <button class="chip-btn" style="width:100%;padding:11px;margin-top:4px;font-weight:700;"
              onclick="saveChangesToWorkshop()">Zapisz jako rutynę</button>
    </div>`;
}

const earIntervalsCatalog = [
  { semi: 0,  name: 'Pryma',          code: 'P1', hint: 'Powtórzony ten sam dźwięk' },
  { semi: 1,  name: 'Sekunda mała',   code: 'm2', hint: 'Szczęki (Jaws)' },
  { semi: 2,  name: 'Sekunda wielka', code: 'M2', hint: 'Sto lat / Happy Birthday' },
  { semi: 3,  name: 'Tercja mała',    code: 'm3', hint: 'Smoke on the Water' },
  { semi: 4,  name: 'Tercja wielka',  code: 'M3', hint: 'Oh When the Saints' },
  { semi: 5,  name: 'Kwarta czysta',  code: 'P4', hint: 'Marsz weselny' },
  { semi: 6,  name: 'Tryton',         code: 'TT', hint: 'The Simpsons' },
  { semi: 7,  name: 'Kwinta czysta',  code: 'P5', hint: 'Gwiezdne Wojny (Star Wars)' },
  { semi: 8,  name: 'Seksta mała',    code: 'm6', hint: 'The Entertainer' },
  { semi: 9,  name: 'Seksta wielka',  code: 'M6', hint: 'My Bonnie Lies Over the Ocean' },
  { semi: 10, name: 'Septyma mała',   code: 'm7', hint: 'The Winner Takes It All' },
  { semi: 11, name: 'Septyma wielka', code: 'M7', hint: 'Take On Me (refren)' },
  { semi: 12, name: 'Oktawa',         code: 'P8', hint: 'Somewhere Over the Rainbow' }
];

const earChordsCatalog = [
  { id: 'major', label: 'Durowy',      intervals: [0, 4, 7] },
  { id: 'minor', label: 'Molowy',      intervals: [0, 3, 7] },
  { id: 'dim',   label: 'Zmniejszony', intervals: [0, 3, 6] },
  { id: 'maj7',  label: 'maj7',        intervals: [0, 4, 7, 11] },
  { id: '7',     label: '7',           intervals: [0, 4, 7, 10] },
  { id: 'm7',    label: 'm7',          intervals: [0, 3, 7, 10] },
  { id: 'm7b5',  label: 'm7b5',        intervals: [0, 3, 6, 10] }
];

function toggleEarIntervalTile(semi) {
  if (state.earSelectedSemis.has(semi)) {
    if (state.earSelectedSemis.size > 1) state.earSelectedSemis.delete(semi);
  } else {
    state.earSelectedSemis.add(semi);
  }
  renderStageAndInspector();
}

function setAllEarIntervals(on) {
  if (on) {
    state.earSelectedSemis = new Set([0,1,2,3,4,5,6,7,8,9,10,11,12]);
  } else {
    state.earSelectedSemis = new Set([0]);
  }
  renderStageAndInspector();
}

function toggleEarChordTile(id) {
  if (state.earSelectedChords.has(id)) {
    if (state.earSelectedChords.size > 1) state.earSelectedChords.delete(id);
  } else {
    state.earSelectedChords.add(id);
  }
  renderStageAndInspector();
}

function toggleEarInversionTile(invIdx) {
  if (state.earSelectedInversions.has(invIdx)) {
    if (state.earSelectedInversions.size > 1) state.earSelectedInversions.delete(invIdx);
  } else {
    state.earSelectedInversions.add(invIdx);
  }
  renderStageAndInspector();
}

function selectEarBaseNote(pc) {
  state.earFixedRootPc = ((pc % 12) + 12) % 12;
  const oct = state.earRootMode === 'fixed_pitch' ? (state.earFixedOctave || 3) : 3;
  const midi = (oct + 1) * 12 + state.earFixedRootPc;
  pluckGuitarFrequency(midiToFreq(midi), 1.1);
  renderStageAndInspector();
}

function selectEarBaseOctave(oct) {
  state.earFixedOctave = oct;
  const midi = (oct + 1) * 12 + state.earFixedRootPc;
  pluckGuitarFrequency(midiToFreq(midi), 1.1);
  renderStageAndInspector();
}

function recordEarRoundStat(key, name, code, wasCorrect) {
  const elapsedMs = Math.max(150, Math.round(performance.now() - (state.earQuestionAskedMs || performance.now())));
  state.earReactionTimesMs.push(elapsedMs);
  if (wasCorrect && state.earStreak > state.earBestStreak) {
    state.earBestStreak = state.earStreak;
  }
  let entry = state.earHistory.find(h => h.key === key);
  if (!entry) {
    entry = { key, name, code: code || '', correct: 0, total: 0 };
    state.earHistory.push(entry);
  }
  entry.total++;
  if (wasCorrect) entry.correct++;
}

function startEarWeakItemsSession() {
  const weak = (state.earHistory || []).filter(h => h.correct < h.total);
  if (state.earMode === 'chords') {
    if (weak.length >= 2) {
      state.earSelectedChords = new Set(weak.map(w => w.key));
    } else if (weak.length === 1) {
      const extra = earChordsCatalog.find(c => c.id !== weak[0].key) || earChordsCatalog[0];
      state.earSelectedChords = new Set([weak[0].key, extra.id]);
    }
  } else {
    if (weak.length >= 2) {
      state.earSelectedSemis = new Set(weak.map(w => Number(w.key)));
    } else if (weak.length === 1) {
      const wSemi = Number(weak[0].key);
      const extraSemi = wSemi === 7 ? 5 : 7;
      state.earSelectedSemis = new Set([wSemi, extraSemi]);
    }
  }
  startEarTrainingSession();
}

function startEarTrainingSession() {
  state.earScreen = 'active';
  state.earRound = 1;
  state.earScore = 0;
  state.earAttempts = 0;
  state.earStreak = 0;
  state.earBestStreak = 0;
  state.earSessionStartMs = performance.now();
  state.earSessionDurationSec = 0;
  state.earReactionTimesMs = [];
  state.earHistory = [];
  state.earCompareMode = false;
  state.earCompareSelection = [];
  if (state.earMode !== 'learn') {
    generateNextEarQuestion();
  }
  renderStageAndInspector();
}

function startEarSurpriseSession() {
  const all = [0,1,2,3,4,5,6,7,8,9,10,11,12];
  all.sort(() => Math.random() - 0.5);
  state.earSelectedSemis = new Set(all.slice(0, 6));
  state.earMode = 'identify';
  startEarTrainingSession();
}

function exitEarActiveSession() {
  state.earScreen = 'config';
  renderStageAndInspector();
}

function playEarIntervalAudio(rootMidi, secondMidi) {
  if (state.earPlaybackStyle === 'harmonic') {
    pluckGuitarFrequency(midiToFreq(rootMidi), 1.5);
    pluckGuitarFrequency(midiToFreq(secondMidi), 1.5);
  } else {
    pluckGuitarFrequency(midiToFreq(rootMidi), 1.3);
    setTimeout(() => pluckGuitarFrequency(midiToFreq(secondMidi), 1.4), 520);
  }
}

function playEarChordAudio(rootMidi, intervals, inversion) {
  const notes = intervals.map(s => rootMidi + s);
  for (let i = 0; i < inversion && i < notes.length; i++) {
    notes[i] += 12;
  }
  notes.sort((a, b) => a - b);
  const gainScale = 1.0 / Math.sqrt(Math.max(1, notes.length));
  if (state.earPlaybackStyle === 'melodic') {
    notes.forEach((m, idx) => {
      setTimeout(() => pluckGuitarFrequency(midiToFreq(m), 1.4, gainScale), idx * 320);
    });
  } else {
    notes.forEach((m, idx) => {
      setTimeout(() => pluckGuitarFrequency(midiToFreq(m), 1.6, gainScale), idx * 35);
    });
  }
}

function generateNextEarQuestion() {
  let rootMidi = 52 + Math.floor(Math.random() * 12); // E3 .. D#4
  if (state.earRootMode === 'fixed_note') {
    rootMidi = 48 + state.earFixedRootPc + (Math.random() < 0.5 ? 0 : 12);
  } else if (state.earRootMode === 'fixed_pitch') {
    const oct = (state.earFixedOctave !== undefined) ? state.earFixedOctave : 3;
    rootMidi = (oct + 1) * 12 + state.earFixedRootPc;
  }
  state.earQuestionAskedMs = performance.now();

  if (state.earMode === 'chords') {
    const pool = earChordsCatalog.filter(c => state.earSelectedChords.has(c.id));
    const chosenChord = pool[Math.floor(Math.random() * pool.length)] || earChordsCatalog[0];
    const invPool = Array.from(state.earSelectedInversions);
    const chosenInv = invPool[Math.floor(Math.random() * invPool.length)] || 0;
    state.earPickedChordId = pool[0] ? pool[0].id : 'major';
    state.earPickedInversion = invPool[0] !== undefined ? invPool[0] : 0;
    state.earCurrentQuestion = {
      rootMidi,
      chordId: chosenChord.id,
      correctName: chosenChord.label,
      intervals: chosenChord.intervals,
      inversion: chosenInv,
      answered: false,
      wasCorrect: false
    };
    setTimeout(() => replayCurrentEarQuestion(), 120);
    return;
  }

  const pool = earIntervalsCatalog.filter(iv => state.earSelectedSemis.has(iv.semi));
  const chosenIv = pool[Math.floor(Math.random() * pool.length)] || earIntervalsCatalog[0];
  let asc = true;
  if (state.earPlaybackStyle !== 'harmonic') {
    if (state.earDirection === 'desc') asc = false;
    else if (state.earDirection === 'rand') asc = (Math.random() < 0.5);
  }

  const targetMidi = asc ? (rootMidi + chosenIv.semi) : (rootMidi - chosenIv.semi);
  const sharpNames = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
  const targetNoteName = sharpNames[((targetMidi % 12) + 12) % 12] + (Math.floor(targetMidi / 12) - 1);

  state.earCurrentQuestion = {
    rootMidi,
    targetMidi,
    targetNoteName,
    semi: chosenIv.semi,
    asc,
    correctName: chosenIv.name,
    correctCode: chosenIv.code,
    hint: chosenIv.hint,
    answered: false,
    wasCorrect: false,
    guessedSemi: null
  };

  if (state.earMode === 'play') {
    setTimeout(() => pluckGuitarFrequency(midiToFreq(rootMidi), 1.5), 120);
  } else {
    setTimeout(() => replayCurrentEarQuestion(), 120);
  }
}

function replayCurrentEarQuestion() {
  const q = state.earCurrentQuestion;
  if (!q) return;
  if (state.earMode === 'chords') {
    playEarChordAudio(q.rootMidi, q.intervals, q.inversion);
  } else {
    playEarIntervalAudio(q.rootMidi, q.targetMidi);
  }
}

function onEarLearnTileClick(semi) {
  const asc = (state.earPlaybackStyle === 'harmonic' || state.earDirection !== 'desc');
  let rootMidi = 52;
  if (state.earRootMode === 'fixed_note') {
    rootMidi = 48 + state.earFixedRootPc;
  } else if (state.earRootMode === 'fixed_pitch') {
    rootMidi = ((state.earFixedOctave || 3) + 1) * 12 + state.earFixedRootPc;
  }
  const secondMidi = asc ? (rootMidi + semi) : (rootMidi - semi);

  if (state.earCompareMode) {
    state.earCompareSelection.push(semi);
    if (state.earCompareSelection.length > 2) {
      state.earCompareSelection.shift();
    }
    renderStageAndInspector();
    if (state.earCompareSelection.length === 2) {
      const s1 = state.earCompareSelection[0];
      const s2 = state.earCompareSelection[1];
      playEarIntervalAudio(rootMidi, asc ? (rootMidi + s1) : (rootMidi - s1));
      setTimeout(() => {
        playEarIntervalAudio(rootMidi, asc ? (rootMidi + s2) : (rootMidi - s2));
      }, 1250);
    } else {
      playEarIntervalAudio(rootMidi, secondMidi);
    }
    return;
  }

  playEarIntervalAudio(rootMidi, secondMidi);
}

function toggleEarCompareMode() {
  state.earCompareMode = !state.earCompareMode;
  state.earCompareSelection = [];
  renderStageAndInspector();
}

function submitEarIdentifyAnswer(guessedSemi) {
  const q = state.earCurrentQuestion;
  if (!q || q.answered) return;
  q.answered = true;
  q.guessedSemi = guessedSemi;
  q.wasCorrect = (guessedSemi === q.semi);
  state.earAttempts++;
  if (q.wasCorrect) {
    state.earScore++;
    state.earStreak++;
  } else {
    state.earStreak = 0;
  }
  recordEarRoundStat(q.semi, q.correctName, q.correctCode, q.wasCorrect);
  renderStageAndInspector();
}

function checkEarPlayGuitarMidi(playedMidi) {
  const q = state.earCurrentQuestion;
  if (!q || q.answered) return;
  if ((playedMidi % 12) === ((q.targetMidi % 12 + 12) % 12)) {
    q.answered = true;
    q.wasCorrect = true;
    state.earAttempts++;
    state.earScore++;
    state.earStreak++;
    recordEarRoundStat(q.semi, q.correctName, q.correctCode, true);
    renderStageAndInspector();
  }
}

function submitEarChordAnswer() {
  const q = state.earCurrentQuestion;
  if (!q || q.answered) return;
  q.answered = true;
  const invMatch = (state.earSelectedInversions.size <= 1) || (state.earPickedInversion === q.inversion);
  q.wasCorrect = (state.earPickedChordId === q.chordId) && invMatch;
  state.earAttempts++;
  if (q.wasCorrect) {
    state.earScore++;
    state.earStreak++;
  } else {
    state.earStreak = 0;
  }
  recordEarRoundStat(q.chordId, q.correctName, '', q.wasCorrect);
  renderStageAndInspector();
}

function nextEarActiveRound() {
  if (state.earSessionType === 'fixed' && state.earRound >= state.earTargetRounds) {
    state.earSessionDurationSec = Math.max(1, Math.round((performance.now() - (state.earSessionStartMs || performance.now())) / 1000));
    state.earScreen = 'summary';
    renderStageAndInspector();
    return;
  }
  state.earRound++;
  generateNextEarQuestion();
  renderStageAndInspector();
}

async function loadWorkshopPatternIntoTab(groupSize) {
  const customParam = state.customStringMidis.join(',');
  const res = await fetchJson(`/api/workshop_events?rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.posSystem}&pattern=${groupSize}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}`);
  state.recordedEvents = (res.events || []).map(ev => ({
    s: ev.s,
    f: ev.f,
    midi: ev.midi,
    note: ev.note,
    freq: ev.freq,
    dur: state.defaultDuration,
    tuplet: 0,
    rest: 0,
    tech: '-'
  }));
  renderStageAndInspector();
}

function downloadTabExport(fmt) {
  const evParam = encodeURIComponent(JSON.stringify(state.recordedEvents));
  window.location.href = `/api/export/${fmt}?events=${evParam}&bpm=${state.tempoBpm}`;
}

// Mapowanie klawiszy do komponowania dokładnie jak na dwuwarstwowym pianinie:
// Białe klawisze (dolny rząd): Q W E R T Y U I O P [ ]
// Czarne klawisze # (górny rząd nad przerwami między białymi): 2 3 (przerwa 4) 5 6 7 (przerwa 8) 9 0 (przerwa -) =
const qwertyOffsets = {
  'q': 0,  // C
  '2': 1,  // C# (między Q i W)
  'w': 2,  // D
  '3': 3,  // D# (między W i E)
  'e': 4,  // E
  'r': 5,  // F
  '5': 6,  // F# (między R i T)
  't': 7,  // G
  '6': 8,  // G# (między T i Y)
  'y': 9,  // A
  '7': 10, // A# (między Y i U)
  'u': 11, // B
  'i': 12, // C (wyższa oktawa)
  '9': 13, // C# (między I i O)
  'o': 14, // D
  '0': 15, // D# (między O i P)
  'p': 16, // E
  '[': 17, // F
  '=': 18, // F# (między [ i ])
  ']': 19  // G
};

function buildMidiKeyboardUI() {
  const gSel = document.getElementById('guitarTonePresetSelect');
  if (gSel && state.guitarTonePreset) gSel.value = state.guitarTonePreset;
  const kSel = document.getElementById('keyboardPresetSelect');
  if (kSel && state.keyboardPreset) kSel.value = state.keyboardPreset;
  const syncBtn = document.getElementById('midiSyncGuitarBtn');
  if (syncBtn) {
    syncBtn.classList.toggle('active', !!state.syncGuitarToKeyboard);
    syncBtn.textContent = `🔄 Synchronizuj z gitarą: ${state.syncGuitarToKeyboard ? 'Wł.' : 'Wył.'}`;
  }
  const container = document.getElementById('midiKeysContainer');
  const labels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
  const baseMidi = (state.midiBaseOctave + 1) * 12; // np. C4 = 60
  const kbdByOffset = {
    0:'Q', 1:'2', 2:'W', 3:'3', 4:'E', 5:'R', 6:'5',
    7:'T', 8:'6', 9:'Y', 10:'7', 11:'U', 12:'I', 13:'9',
    14:'O', 15:'0', 16:'P', 17:'[', 18:'=', 19:']'
  };
  let html = '';
  // Rysujemy dwuwarstwową klawiaturę fortepianową w pełnym 4-oktawowym zakresie gitary (E 2 = 40 .. E 6 = 88):
  // białe klawisze obok siebie, a czarne klawisze (#) zawieszone u góry dokładnie między odpowiednimi białymi klawiszami!
  for (let midi = 40; midi <= 88; midi++) {
    const pc = midi % 12;
    const isSharp = [1,3,6,8,10].includes(pc);
    if (isSharp) continue; // Czarny klawisz jest renderowany wewnątrz poprzedzającego białego klawisza

    const whiteName = labels[pc] + ' ' + (Math.floor(midi / 12) - 1);
    const whiteOffset = midi - baseMidi;
    const whiteHint = kbdByOffset[whiteOffset] !== undefined
      ? `<span class="kbd-tag">[${kbdByOffset[whiteOffset]}]</span>`
      : '';

    // Sprawdź, czy po tym białym klawiszu występuje czarny klawisz (po C, D, F, G, A)
    let blackKeyHtml = '';
    const nextMidi = midi + 1;
    const nextPc = nextMidi % 12;
    if ([1,3,6,8,10].includes(nextPc) && nextMidi <= 88) {
      const blackName = labels[nextPc] + ' ' + (Math.floor(nextMidi / 12) - 1);
      const blackOffset = nextMidi - baseMidi;
      const blackHint = kbdByOffset[blackOffset] !== undefined
        ? `<span class="kbd-tag">[${kbdByOffset[blackOffset]}]</span>`
        : '';
      blackKeyHtml = `
        <div id="midiKey_${nextMidi}" class="piano-key sharp"
             onmousedown="event.stopPropagation();startMouseMidiKeyHold(${nextMidi})"
             onmouseenter="onMouseMidiKeyEnter(${nextMidi})">
          ${blackHint}<span>${blackName}</span>
        </div>`;
    }

    html += `
      <div class="white-key-wrap">
        <div id="midiKey_${midi}" class="piano-key white"
             onmousedown="startMouseMidiKeyHold(${midi})"
             onmouseenter="onMouseMidiKeyEnter(${midi})">
          ${whiteHint}<span>${whiteName}</span>
        </div>
        ${blackKeyHtml}
      </div>`;
  }
  container.innerHTML = html;
  updateGuitarToKeyboardSync();
}

function getDurationBeats(durCode, isTuplet) {
  const base = Math.pow(2, -(durCode !== undefined ? durCode : 0));
  return isTuplet ? (base * 2 / 3) : base;
}

function buildExerciseStaffAndTabSvg(highlightIdx = -1, availWidth = 0) {
  const evs = state.recordedEvents || [];
  const isPlayer = highlightIdx >= 0;
  const showStaff = state.exerciseShowStaff !== false;
  const showTab = state.tabCreationEnabled || !showStaff;
  const wrapRows = !!state.exerciseTabWrapRows;

  const baseColW = 46;
  const x0 = showStaff ? 96 : 58;
  const staffGap = 10;
  const tabGap = showStaff ? 12 : 14; // Większa, czytelniejsza tabulatura gdy nuty są wyłączone!
  const staffTopRel = 32;
  const tabTopRel = showStaff ? 110 : 28;
  const rowH = showStaff ? (showTab ? 204 : 114) : 116;
  const rowGapY = 18;

  let useBass = state.exerciseClef === 'bass';
  if (state.exerciseClef === 'auto') {
    const pitched = evs.filter(e => !e.rest && e.midi > 0);
    if (pitched.length > 0) {
      const avgMidi = pitched.reduce((a, b) => a + b.midi, 0) / pitched.length;
      useBass = avgMidi < 54;
    }
  }
  const clefSymbol = useBass ? '𝄢' : '𝄞';
  const labels = getRootLabels();
  const diaPc = [0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6];
  const refDia = useBass ? 18 : 30;
  const measureBeats = state.timeSigNum * (4 / Math.max(1, state.timeSigDen));

  // Wylicz takty dla wszystkich zdarzeń, żeby w trybie wieloliniowym ładnie zawijać na granicach taktów
  const barMeta = [];
  {
    let acc = 0, bNum = 1;
    evs.forEach((ev, idx) => {
      const dur = ev.dur !== undefined ? ev.dur : 0;
      acc += getDurationBeats(dur, ev.tuplet === 3);
      const isBarEnd = (acc >= measureBeats - 0.001 && idx < evs.length - 1);
      barMeta.push({ barNum: bNum, isBarEnd });
      if (isBarEnd) { acc = 0; bNum++; }
    });
  }

  // Podział na wiersze (1 linia przewijana albo wiele wierszy wypełniających okno!)
  const rows = [];
  const targetRowW = Math.max(700, Math.round(availWidth || 1040));
  if (!wrapRows || evs.length === 0) {
    const W = Math.max(targetRowW, x0 + evs.length * baseColW + 24);
    rows.push({ start: 0, end: evs.length, W, colW: baseColW });
  } else {
    const maxPerRow = Math.max(8, Math.floor((targetRowW - x0 - 20) / 40));
    let cur = 0;
    while (cur < evs.length) {
      let end = Math.min(evs.length, cur + maxPerRow);
      if (end < evs.length) {
        // Jeśli w pobliżu końca wiersza wypada kreska taktowa, zawijaj dokładnie na takcie
        let bestBarBreak = -1;
        for (let k = end - 1; k >= Math.max(cur + Math.floor(maxPerRow * 0.55), cur + 4); k--) {
          if (barMeta[k] && barMeta[k].isBarEnd) { bestBarBreak = k + 1; break; }
        }
        if (bestBarBreak > cur) end = bestBarBreak;
      }
      const nInRow = Math.max(1, end - cur);
      // Rozciągnij odstępy pełnych wierszy równo do prawej krawędzi okna!
      const isLastShort = (end === evs.length && nInRow < Math.max(6, Math.floor(maxPerRow * 0.65)));
      const rowColW = isLastShort ? baseColW : ((targetRowW - x0 - 24) / nInRow);
      rows.push({ start: cur, end, W: targetRowW, colW: Math.min(64, Math.max(32, rowColW)) });
      cur = end;
    }
  }

  const totalW = wrapRows ? targetRowW : rows[0].W;
  const totalH = rows.length * rowH + Math.max(0, rows.length - 1) * rowGapY;
  const svgStyle = wrapRows ? 'width:100%;height:auto;display:block;' : 'display:block;';
  let svg = `<svg width="${totalW}" height="${totalH}" viewBox="0 0 ${totalW} ${totalH}" style="${svgStyle}" xmlns="http://www.w3.org/2000/svg">`;

  rows.forEach((rowInfo, rowIdx) => {
    const yOff = rowIdx * (rowH + rowGapY);
    const W = rowInfo.W;
    const colW = rowInfo.colW;
    const staffTop = yOff + staffTopRel;
    const tabTop = yOff + tabTopRel;
    const topVisual = showStaff ? staffTop : tabTop;
    const endY = showTab ? (tabTop + 5 * tabGap) : (staffTop + 4 * staffGap);
    const startBarNum = (rowInfo.start < barMeta.length) ? barMeta[rowInfo.start].barNum : 1;

    // Numer taktu na początku wiersza + tempo w pierwszym wierszu
    svg += `<text x="14" y="${yOff + 18}" fill="#9ca3af" font-size="10.5" font-weight="700">${startBarNum}</text>`;
    if (rowIdx === 0) {
      svg += `<text x="${showStaff ? 84 : 46}" y="${yOff + 16}" fill="#e7e1d8" font-size="12" font-weight="700">♩ = ${state.tempoBpm}</text>`;
    }

    // 1. Pięciolinia nutowa (opcjonalna — można wyłączyć, żeby widzieć więcej tabulatury!)
    if (showStaff) {
      for (let i = 0; i < 5; i++) {
        const y = staffTop + i * staffGap;
        svg += `<line x1="12" y1="${y}" x2="${W - 12}" y2="${y}" stroke="#6b6257" stroke-width="1.2"/>`;
      }
      svg += `<text x="20" y="${staffTop + (useBass ? 30 : 35)}" fill="${isPlayer ? '#e7e1d8' : '#f39237'}" font-size="34">${clefSymbol}</text>`;
      if (rowIdx === 0) {
        svg += `<text x="54" y="${staffTop + 18}" fill="#d6cec2" font-size="14" font-weight="800">${state.timeSigNum}</text>`;
        svg += `<text x="54" y="${staffTop + 37}" fill="#d6cec2" font-size="14" font-weight="800">${state.timeSigDen}</text>`;
      }
      svg += `<line x1="${W - 12}" y1="${staffTop}" x2="${W - 12}" y2="${staffTop + 40}" stroke="#6b6257" stroke-width="${rowIdx === rows.length - 1 ? '1.8' : '1.2'}"/>`;
    }

    // 2. 6-liniowa Tabulatura (TAB)
    if (showTab) {
      for (let s = 0; s < 6; s++) {
        const y = tabTop + s * tabGap;
        svg += `<line x1="12" y1="${y}" x2="${W - 12}" y2="${y}" stroke="#524940" stroke-width="1.15"/>`;
      }
      const tabMidH = 5 * tabGap;
      svg += `<text x="20" y="${tabTop + tabMidH * 0.30}" fill="#9ca3af" font-size="${showStaff ? 13 : 14}" font-weight="800">T</text>`;
      svg += `<text x="20" y="${tabTop + tabMidH * 0.56}" fill="#9ca3af" font-size="${showStaff ? 13 : 14}" font-weight="800">A</text>`;
      svg += `<text x="20" y="${tabTop + tabMidH * 0.82}" fill="#9ca3af" font-size="${showStaff ? 13 : 14}" font-weight="800">B</text>`;
      svg += `<line x1="${W - 12}" y1="${tabTop}" x2="${W - 12}" y2="${tabTop + 5 * tabGap}" stroke="#6b6257" stroke-width="${rowIdx === rows.length - 1 ? '1.8' : '1.2'}"/>`;
    }

    for (let idx = rowInfo.start; idx < rowInfo.end; idx++) {
      const ev = evs[idx];
      const localIdx = idx - rowInfo.start;
      const x = x0 + localIdx * colW;
      const isActive = (idx === highlightIdx);
      const colColor = isActive ? '#4ade80' : '#e7e1d8';
      const dur = ev.dur !== undefined ? ev.dur : 0;

      svg += `<g style="cursor:pointer" ${isActive ? 'data-ex-active="1"' : ''} onclick="onExerciseScoreNoteClick(${idx})">`;
      if (isActive) {
        svg += `<rect x="${x - 16}" y="${topVisual - 14}" width="32" height="${(endY - topVisual) + 24}" rx="6" fill="rgba(34,197,94,0.14)" stroke="rgba(74,222,128,0.45)" stroke-width="1"/>`;
      }

      if (ev.rest) {
        const restY = showStaff ? (staffTop + 26) : (tabTop + 2.7 * tabGap);
        svg += `<text x="${x}" y="${restY}" fill="${isActive ? '#f39237' : '#9ca3af'}" font-size="18" font-weight="700" text-anchor="middle">𝄽</text>`;
      } else {
        const midi = ev.midi || 60;
        const oct = Math.floor(midi / 12) - 1;
        const pc = ((midi % 12) + 12) % 12;
        const nm = ev.note || labels[pc];

        if (showStaff) {
          const dia = oct * 7 + diaPc[pc];
          const rawNoteY = (staffTop + 40) - (dia - refDia) * 5;
          const noteY = Math.max(yOff + 12, Math.min(staffTop + 62, rawNoteY));

          for (let ly = staffTop + 50; ly <= noteY + 1 && ly <= staffTop + 62; ly += 10) {
            svg += `<line x1="${x - 11}" y1="${ly}" x2="${x + 11}" y2="${ly}" stroke="#8c7f72" stroke-width="1.1"/>`;
          }
          for (let ly = staffTop - 10; ly >= noteY - 1 && ly >= yOff + 12; ly -= 10) {
            svg += `<line x1="${x - 11}" y1="${ly}" x2="${x + 11}" y2="${ly}" stroke="#8c7f72" stroke-width="1.1"/>`;
          }
          if (nm.includes('#')) {
            svg += `<text x="${x - 12}" y="${noteY + 4}" fill="${colColor}" font-size="12" font-weight="800" text-anchor="middle">#</text>`;
          } else if (nm.includes('b')) {
            svg += `<text x="${x - 12}" y="${noteY + 4}" fill="${colColor}" font-size="12" font-weight="800" text-anchor="middle">♭</text>`;
          }
          if (dur <= -1) {
            svg += `<ellipse cx="${x}" cy="${noteY}" rx="6.5" ry="4.5" fill="none" stroke="${colColor}" stroke-width="2"/>`;
          } else {
            svg += `<ellipse cx="${x}" cy="${noteY}" rx="6.2" ry="4.6" fill="${colColor}"/>`;
          }
          if (dur > -2) {
            const stemDown = noteY <= staffTop + 20;
            const sx = stemDown ? (x - 5.5) : (x + 5.5);
            const sy2 = stemDown ? (noteY + 24) : (noteY - 24);
            svg += `<line x1="${sx}" y1="${noteY}" x2="${sx}" y2="${sy2}" stroke="${colColor}" stroke-width="1.6"/>`;
            if (dur >= 1) {
              const dir = stemDown ? -1 : 1;
              svg += `<path d="M${sx},${sy2} Q${sx + 9},${sy2 + 6 * dir} ${sx + 4},${sy2 + 12 * dir}" fill="none" stroke="${colColor}" stroke-width="1.6"/>`;
            }
            if (dur >= 2) {
              const dir = stemDown ? -1 : 1;
              const sy3 = sy2 + 5 * dir;
              svg += `<path d="M${sx},${sy3} Q${sx + 9},${sy3 + 6 * dir} ${sx + 4},${sy3 + 12 * dir}" fill="none" stroke="${colColor}" stroke-width="1.6"/>`;
            }
          }
          if (ev.tuplet === 3) {
            svg += `<text x="${x}" y="${noteY - 26}" fill="#4ade80" font-size="10" font-weight="800" text-anchor="middle">3</text>`;
          }
        }

        const isRootPc = (pc === state.rootPc);
        const topNoteLbl = (state.labelType === 'intervals')
          ? computeIntervalLabel(pc, state.rootPc, false)
          : nm;
        if (state.showExerciseNoteNames) {
          svg += `<text x="${x}" y="${topVisual - 6}" fill="${isRootPc ? '#f39237' : '#f5f2eb'}" font-size="10" font-weight="${isRootPc ? '800' : '700'}" text-anchor="middle">${topNoteLbl}</text>`;
        } else if (isRootPc && showTab && !showStaff) {
          svg += `<circle cx="${x}" cy="${topVisual - 10}" r="2.6" fill="#f39237"><title>Tonika (1)</title></circle>`;
        }

        if (showTab && ev.s !== undefined && ev.f !== undefined && ev.s >= 0 && ev.f >= 0) {
          const row = 5 - ev.s;
          const ty = tabTop + row * tabGap;
          const boxW = String(ev.f).length > 1 ? 22 : 18;
          const tabBg = isActive ? '#22c55e' : '#15110e';
          const tabTxt = isActive ? '#05140b' : '#f5f2eb';
          svg += `<rect x="${x - boxW / 2}" y="${ty - 7.5}" width="${boxW}" height="15" rx="3.5" fill="${tabBg}" stroke="${isActive ? '#bbf7d0' : 'none'}" stroke-width="${isActive ? '1.1' : '0'}"/>`;
          svg += `<text x="${x}" y="${ty + 4.2}" fill="${tabTxt}" font-size="${showStaff ? 12 : 13}" font-weight="800" text-anchor="middle">${ev.f}</text>`;
          if (ev.tech && ev.tech !== '-' && ev.tech !== '—') {
            svg += `<text x="${x + boxW / 2 + 4}" y="${ty - 2}" fill="#4ade80" font-size="9" font-weight="800">${ev.tech}</text>`;
          }
        }
      }
      svg += `</g>`;

      if (barMeta[idx] && barMeta[idx].isBarEnd && idx < rowInfo.end - 1) {
        const barX = x + colW * 0.5;
        if (showStaff) {
          svg += `<line x1="${barX}" y1="${staffTop}" x2="${barX}" y2="${staffTop + 40}" stroke="#6b6257" stroke-width="1.2"/>`;
        }
        if (showTab) {
          svg += `<line x1="${barX}" y1="${tabTop}" x2="${barX}" y2="${tabTop + 5 * tabGap}" stroke="#524940" stroke-width="1.2"/>`;
        }
        svg += `<text x="${barX + 5}" y="${yOff + 18}" fill="#9ca3af" font-size="10.5" font-weight="700">${barMeta[idx].barNum + 1}</text>`;
      }
    }
  });

  svg += `</svg>`;
  return svg;
}

function buildExerciseFretboardSvg(highlightIdx = -1, isComposer = true) {
  const evs = state.recordedEvents || [];
  const W = 1040, H = 205;
  const openX = 24, nutX = 56, padR = 16;
  const fretCount = 24;
  const fretW = (W - nutX - padR) / fretCount;
  const topY = 24, strGap = 28;
  const labels = getRootLabels();
  const minF = isComposer ? (state.exerciseMinFret || 0) : 0;
  const maxF = isComposer ? (state.exerciseMaxFret || 24) : 24;
  const fretInlays = new Set([3, 5, 7, 9, 12, 15, 17, 19, 21]);
  const numLabels = new Set([3, 5, 7, 9, 12, 15, 17, 19, 21]);

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" style="width:100%;height:auto;display:block;user-select:none;" xmlns="http://www.w3.org/2000/svg">`;
  svg += `<rect x="${nutX}" y="${topY - 12}" width="${W - nutX - padR}" height="${5 * strGap + 24}" rx="4" fill="#18120e" stroke="#30271f" stroke-width="1"/>`;

  // Znaczniki progów
  for (let f = 1; f <= fretCount; f++) {
    const cx = nutX + (f - 0.5) * fretW;
    if (fretInlays.has(f)) {
      if (f === 12) {
        svg += `<circle cx="${cx}" cy="${topY + strGap * 1.5}" r="4.5" fill="rgba(255,255,255,0.06)"/>`;
        svg += `<circle cx="${cx}" cy="${topY + strGap * 3.5}" r="4.5" fill="rgba(255,255,255,0.06)"/>`;
      } else {
        svg += `<circle cx="${cx}" cy="${topY + strGap * 2.5}" r="4.5" fill="rgba(255,255,255,0.06)"/>`;
      }
    }
    if (f < minF || f > maxF) {
      svg += `<rect x="${nutX + (f - 1) * fretW}" y="${topY - 12}" width="${fretW}" height="${5 * strGap + 24}" fill="rgba(0,0,0,0.45)"/>`;
    }
  }

  // Progi (linie pionowe) + siodełko
  svg += `<line x1="${nutX}" y1="${topY - 12}" x2="${nutX}" y2="${topY + 5 * strGap + 12}" stroke="#b8aea0" stroke-width="3"/>`;
  for (let f = 1; f <= fretCount; f++) {
    const fx = nutX + f * fretW;
    svg += `<line x1="${fx}" y1="${topY - 12}" x2="${fx}" y2="${topY + 5 * strGap + 12}" stroke="#443930" stroke-width="1.2"/>`;
    if (numLabels.has(f)) {
      svg += `<text x="${fx - fretW * 0.5}" y="${topY + 5 * strGap + 28}" fill="#78716c" font-size="10.5" font-weight="700" text-anchor="middle">${f}</text>`;
    }
  }

  // Struny i nazwy pustych strun
  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const sy = topY + row * strGap;
    const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    const openLbl = labels[((openMidi % 12) + 12) % 12];
    const thickness = 1.0 + row * 0.35;
    svg += `<text x="${openX - 10}" y="${sy + 4}" fill="#9ca3af" font-size="11.5" font-weight="800" text-anchor="middle">${openLbl}</text>`;
    svg += `<line x1="${nutX}" y1="${sy}" x2="${W - padR}" y2="${sy}" stroke="#786b5d" stroke-width="${thickness}"/>`;

    // Subtelne chromatyczne nazwy w tle
    for (let f = 1; f <= fretCount; f++) {
      const m = openMidi + f;
      const lbl = labels[((m % 12) + 12) % 12];
      const cx = nutX + (f - 0.5) * fretW;
      svg += `<text x="${cx}" y="${sy + 3.5}" fill="rgba(214,206,194,0.16)" font-size="9" font-weight="600" text-anchor="middle">${lbl}</text>`;
    }
  }

  // Kropki dźwięków obecnych w ćwiczeniu
  const usedPos = new Map();
  evs.forEach((ev, idx) => {
    if (!ev.rest && ev.s >= 0 && ev.f >= 0) {
      usedPos.set(`${ev.s}_${ev.f}`, ev);
    }
  });

  usedPos.forEach((ev) => {
    const row = 5 - ev.s;
    const cy = topY + row * strGap;
    const cx = ev.f === 0 ? openX + 10 : (nutX + (ev.f - 0.5) * fretW);
    const openMidi = state.customStringMidis[ev.s] !== undefined ? state.customStringMidis[ev.s] : [40, 45, 50, 55, 59, 64][ev.s];
    const m = ev.midi || (openMidi + ev.f);
    const pc = ((m % 12) + 12) % 12;
    const nm = (state.labelType === 'intervals')
      ? computeIntervalLabel(pc, state.rootPc, false)
      : (ev.note || labels[pc]);
    svg += `<circle cx="${cx}" cy="${cy}" r="10.5" fill="#4f290f" stroke="#a65b26" stroke-width="1.4"/>`;
    svg += `<text x="${cx}" y="${cy + 3.8}" fill="#f5d0a9" font-size="10" font-weight="800" text-anchor="middle">${nm}</text>`;
  });

  // Podświetlona aktywna nuta (podwójny świecący pierścień jak na zrzucie Exercise)
  if (highlightIdx >= 0 && highlightIdx < evs.length) {
    const aEv = evs[highlightIdx];
    if (aEv && !aEv.rest && aEv.s >= 0 && aEv.f >= 0) {
      const row = 5 - aEv.s;
      const cy = topY + row * strGap;
      const cx = aEv.f === 0 ? openX + 10 : (nutX + (aEv.f - 0.5) * fretW);
      const openMidi = state.customStringMidis[aEv.s] !== undefined ? state.customStringMidis[aEv.s] : [40, 45, 50, 55, 59, 64][aEv.s];
      const m = aEv.midi || (openMidi + aEv.f);
      const pc = ((m % 12) + 12) % 12;
      const nm = (state.labelType === 'intervals')
        ? computeIntervalLabel(pc, state.rootPc, false)
        : (aEv.note || labels[pc]);
      svg += `<circle cx="${cx}" cy="${cy}" r="16" fill="rgba(243,146,55,0.22)" stroke="#f39237" stroke-width="2"/>`;
      svg += `<circle cx="${cx}" cy="${cy}" r="11.5" fill="#f39237" stroke="#fff2e2" stroke-width="1.2"/>`;
      svg += `<text x="${cx}" y="${cy + 4}" fill="#120f0c" font-size="10.5" font-weight="900" text-anchor="middle">${nm}</text>`;
    }
  }

  // Przezroczyste pola klikalne dla wszystkich 6 strun x 25 progów (0..24)
  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const ry = topY + row * strGap - strGap * 0.5;
    svg += `<rect x="4" y="${ry}" width="${nutX - 4}" height="${strGap}" fill="transparent" style="cursor:pointer" onclick="onExerciseFretCellClick(${s},0,${isComposer})"/>`;
    for (let f = 1; f <= fretCount; f++) {
      const rx = nutX + (f - 1) * fretW;
      svg += `<rect x="${rx}" y="${ry}" width="${fretW}" height="${strGap}" fill="transparent" style="cursor:pointer" onclick="onExerciseFretCellClick(${s},${f},${isComposer})"/>`;
    }
  }

  svg += `</svg>`;
  return svg;
}

function onExerciseFretCellClick(s, f, isComposer) {
  const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
  const midi = openMidi + f;
  const freq = midiToFreq(midi);
  const labels = getRootLabels();
  const note = labels[((midi % 12) + 12) % 12];
  state.detectedMidi = midi;
  pluckGuitarFrequency(freq, 0.95);
  updateGuitarToKeyboardSync();

  if (isComposer) {
    state.recordedEvents.push({
      s: s,
      f: f,
      midi: midi,
      note: note,
      freq: freq,
      dur: state.defaultDuration,
      tuplet: state.tupletActive ? 3 : 0,
      rest: 0,
      tech: '-'
    });
    renderStageAndInspector();
  } else if (state.exercisePlayerActive && state.exercisePlayerMode === 'play') {
    checkExercisePlayerPlayedMidi(midi);
  }
}

function onExerciseScoreNoteClick(idx) {
  const ev = state.recordedEvents[idx];
  if (!ev) return;
  if (!ev.rest && ev.midi > 0) {
    state.detectedMidi = ev.midi;
    pluckGuitarFrequency(ev.freq || midiToFreq(ev.midi), 0.95);
    updateGuitarToKeyboardSync();
  }
  if (state.exercisePlayerActive) {
    state.exercisePlayerStepIdx = idx;
    renderStageAndInspector();
  }
}

function updateExerciseRow(idx, field, val) {
  const ev = state.recordedEvents[idx];
  if (!ev) return;
  const labels = getRootLabels();
  const recalcFromSf = () => {
    const openMidi = state.customStringMidis[ev.s] !== undefined ? state.customStringMidis[ev.s] : [40, 45, 50, 55, 59, 64][ev.s];
    ev.midi = openMidi + ev.f;
    ev.freq = midiToFreq(ev.midi);
    ev.note = labels[((ev.midi % 12) + 12) % 12];
  };
  if (field === 'strNum') {
    const strNum = Math.max(1, Math.min(6, isNaN(val) ? 1 : val));
    ev.s = 6 - strNum;
    if (!ev.rest) recalcFromSf();
  } else if (field === 'fret') {
    ev.f = Math.max(0, Math.min(24, isNaN(val) ? 0 : val));
    if (!ev.rest) recalcFromSf();
  } else if (field === 'dur') {
    ev.dur = val;
  } else if (field === 'rest') {
    ev.rest = val ? 1 : 0;
    if (!ev.rest && (ev.s === undefined || ev.s < 0)) {
      ev.s = 3;
      ev.f = 5;
      recalcFromSf();
    }
  } else if (field === 'tech') {
    ev.tech = val || '-';
  }
  renderStageAndInspector();
}

function moveExerciseEvent(idx, delta) {
  const target = idx + delta;
  if (idx < 0 || idx >= state.recordedEvents.length || target < 0 || target >= state.recordedEvents.length) return;
  const tmp = state.recordedEvents[idx];
  state.recordedEvents[idx] = state.recordedEvents[target];
  state.recordedEvents[target] = tmp;
  renderStageAndInspector();
}

function deleteExerciseEvent(idx) {
  if (idx < 0 || idx >= state.recordedEvents.length) return;
  state.recordedEvents.splice(idx, 1);
  renderStageAndInspector();
}

function saveExerciseToWorkshop() {
  if (!state.recordedEvents || state.recordedEvents.length === 0) return;
  const rootLabels = getRootLabels();
  const newEx = {
    id: 'ex_tab_' + Date.now(),
    title: `Własne ćwiczenie TAB (${rootLabels[state.rootPc]} · ${state.recordedEvents.length} nut · ${state.tempoBpm} BPM)`,
    category: 'Tab Composer',
    level: 'Własne',
    rootPc: state.rootPc,
    scaleId: state.scaleId,
    posSystem: state.posSystem,
    startString: -1,
    direction: 0,
    octaves: 2,
    pattern: 0,
    recordedEvents: state.recordedEvents.map(e => Object.assign({}, e))
  };
  state.customWorkshopExercises.unshift(newEx);
  const btns = document.querySelectorAll('button[onclick="saveExerciseToWorkshop()"]');
  btns.forEach(b => { b.textContent = '✓ Zapisano w Warsztacie'; });
}

let exercisePlayerTimer = null;
let exerciseMetroTimer = null;
let exerciseMetroBeat = 0;
let lastExerciseAdvanceMs = 0;

function stopExerciseMetronome() {
  if (exerciseMetroTimer) {
    clearInterval(exerciseMetroTimer);
    exerciseMetroTimer = null;
  }
}

function startExerciseMetronome() {
  stopExerciseMetronome();
  if (!state.spMetronome || !state.exercisePlayerActive || state.exercisePlayerPaused) return;
  exerciseMetroBeat = 0;
  const beatMs = 60000 / Math.max(40, state.tempoBpm);
  const tick = () => {
    if (!state.exercisePlayerActive || state.exercisePlayerPaused || !state.spMetronome) {
      stopExerciseMetronome();
      return;
    }
    playMetronomeTickAudio(exerciseMetroBeat % Math.max(1, state.timeSigNum) === 0);
    exerciseMetroBeat++;
  };
  tick();
  exerciseMetroTimer = setInterval(tick, beatMs);
}

function toggleExerciseMetronome() {
  state.spMetronome = !state.spMetronome;
  saveSettings();
  if (state.spMetronome) startExerciseMetronome(); else stopExerciseMetronome();
  renderStageAndInspector();
}

function stopExercisePlayer(shouldRender = true) {
  if (exercisePlayerTimer) {
    clearTimeout(exercisePlayerTimer);
    exercisePlayerTimer = null;
  }
  stopExerciseMetronome();
  state.exercisePlayerActive = false;
  state.exercisePlayerPaused = false;
  state.detectedMidi = null;
  if (shouldRender) renderAll();
}

function playExerciseStepAudio(ev, idx) {
  if (!ev) return;
  if (state.spBgChord && idx % Math.max(1, state.timeSigNum) === 0) {
    const rootM = 48 + (state.rootPc || 0);
    const sc = (state.fretboardData && state.fretboardData.scaleDegrees) || [0, 4, 7];
    const triad = [rootM, rootM + (sc[2] !== undefined ? sc[2] : 4), rootM + (sc[4] !== undefined ? sc[4] : 7)];
    triad.forEach(m => pluckGuitarFrequency(midiToFreq(m), 0.22));
  }
  if (!ev.rest && ev.midi > 0) {
    state.detectedMidi = ev.midi;
    pluckGuitarFrequency(ev.freq || midiToFreq(ev.midi), 0.92);
    updateGuitarToKeyboardSync();
  }
}

function scheduleNextExercisePlayerTick() {
  if (exercisePlayerTimer) {
    clearTimeout(exercisePlayerTimer);
    exercisePlayerTimer = null;
  }
  if (!state.exercisePlayerActive || state.exercisePlayerPaused || state.exercisePlayerMode !== 'listen') return;
  const evs = state.recordedEvents || [];
  const idx = state.exercisePlayerStepIdx || 0;
  if (idx >= evs.length) {
    state.exercisePlayerStepIdx = 0;
    state.exercisePlayerPaused = true;
    renderStageAndInspector();
    return;
  }
  const curEv = evs[idx];
  playExerciseStepAudio(curEv, idx);

  const qMs = 60000 / Math.max(40, state.tempoBpm);
  const beats = getDurationBeats(curEv.dur !== undefined ? curEv.dur : state.defaultDuration, curEv.tuplet === 3);
  const stepMs = Math.max(150, qMs * beats);

  exercisePlayerTimer = setTimeout(() => {
    if (!state.exercisePlayerActive || state.exercisePlayerPaused) return;
    state.exercisePlayerStepIdx++;
    if (state.exercisePlayerStepIdx >= state.recordedEvents.length) {
      state.exercisePlayerStepIdx = 0;
      state.exercisePlayerPaused = true;
      state.detectedMidi = null;
      renderStageAndInspector();
      return;
    }
    renderStageAndInspector();
    scheduleNextExercisePlayerTick();
  }, stepMs);
}

async function startExercisePlayer(mode) {
  if (!state.recordedEvents || state.recordedEvents.length === 0) return;
  if (exercisePlayerTimer) {
    clearTimeout(exercisePlayerTimer);
    exercisePlayerTimer = null;
  }
  state.exercisePlayerActive = true;
  state.exercisePlayerMode = mode || 'listen';
  state.exercisePlayerPaused = false;
  state.exercisePlayerStepIdx = 0;

  if (state.exercisePlayerMode === 'play') {
    while (state.exercisePlayerStepIdx < state.recordedEvents.length - 1 &&
           state.recordedEvents[state.exercisePlayerStepIdx].rest) {
      state.exercisePlayerStepIdx++;
    }
    if (state.spRefNote) {
      pluckGuitarFrequency(midiToFreq(48 + (state.rootPc || 0)), 0.55);
    }
  }

  await renderAll();
  startExerciseMetronome();
  if (state.exercisePlayerMode === 'listen') {
    scheduleNextExercisePlayerTick();
  } else if (!micStream) {
    // Tryb „Graj”: do wykrywania dźwięków gitary potrzebny jest mikrofon — włącz go automatycznie.
    try { await toggleLiveAudio(); } catch (e) {}
    if (state.exercisePlayerActive) renderStageAndInspector();
  }
}

function toggleExercisePlayerPause() {
  state.exercisePlayerPaused = !state.exercisePlayerPaused;
  if (state.exercisePlayerPaused) {
    if (exercisePlayerTimer) {
      clearTimeout(exercisePlayerTimer);
      exercisePlayerTimer = null;
    }
    stopExerciseMetronome();
  } else {
    startExerciseMetronome();
    if (state.exercisePlayerMode === 'listen') {
      scheduleNextExercisePlayerTick();
    }
  }
  renderStageAndInspector();
}

async function stepExercisePlayer(delta) {
  const n = state.recordedEvents ? state.recordedEvents.length : 0;
  if (n === 0) return;
  state.exercisePlayerStepIdx = (state.exercisePlayerStepIdx + delta + n) % n;
  const ev = state.recordedEvents[state.exercisePlayerStepIdx];
  if (ev && !ev.rest && ev.midi > 0) {
    state.detectedMidi = ev.midi;
    pluckGuitarFrequency(ev.freq || midiToFreq(ev.midi), 0.9);
    updateGuitarToKeyboardSync();
  }
  await renderStageAndInspector();
  if (state.exercisePlayerMode === 'listen' && !state.exercisePlayerPaused) {
    scheduleNextExercisePlayerTick();
  }
}

function checkExercisePlayerPlayedMidi(playedMidi) {
  if (!state.exercisePlayerActive || state.exercisePlayerMode !== 'play' || state.exercisePlayerPaused) return;
  const evs = state.recordedEvents || [];
  if (evs.length === 0) return;
  const now = performance.now();
  if (now - lastExerciseAdvanceMs < 100) return;

  const idx = state.exercisePlayerStepIdx || 0;
  const cur = evs[idx];
  if (!cur || cur.rest || !cur.midi) return;

  // Liczy się konkretna wysokość z oktawą (np. G#3), a nie sama nazwa dźwięku
  if (Math.round(playedMidi) !== Math.round(cur.midi)) return;

  lastExerciseAdvanceMs = now;
  let nextIdx = idx + 1;
  while (nextIdx < evs.length && evs[nextIdx].rest) nextIdx++;
  if (nextIdx >= evs.length) {
    state.todayPracticedMinutes = (state.todayPracticedMinutes || 0) + 1;
    saveSettings();
    nextIdx = 0;
    while (nextIdx < evs.length - 1 && evs[nextIdx].rest) nextIdx++;
  }
  state.exercisePlayerStepIdx = nextIdx;
  renderStageAndInspector();
}

function playScalePracticeScalePreview() {
  if (state.scalePracticeMode === 'scale' && state.fretboardData && state.fretboardData.scaleNotes) {
    const notes = state.fretboardData.scaleNotes;
    const sharpLabels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
    const flatLabels = ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B'];
    let lastMidi = 48 + (state.rootPc || 0) - 1;
    const seq = [];
    for (let i = 0; i <= notes.length; i++) {
      const nm = notes[i % notes.length];
      let pc = sharpLabels.indexOf(nm);
      if (pc < 0) pc = flatLabels.indexOf(nm);
      if (pc < 0) pc = 0;
      let m = 48 + pc;
      while (m <= lastMidi) m += 12;
      lastMidi = m;
      seq.push(m);
    }
    seq.forEach((m, i) => {
      setTimeout(() => {
        state.detectedMidi = m;
        pluckGuitarFrequency(midiToFreq(m), 0.78);
        updateGuitarToKeyboardSync();
      }, i * 240);
    });
    return;
  }

  // Podgląd arpeggia
  const ivs = (typeof earChordsCatalog !== 'undefined')
    ? ((earChordsCatalog.find(c => c.id === state.chordId) || { intervals: [0, 4, 7] }).intervals)
    : [0, 4, 7];
  const baseMidi = 48 + (state.rootPc || 0);
  const seq = ivs.map(s => baseMidi + s).concat([baseMidi + 12]);
  seq.forEach((m, i) => {
    setTimeout(() => {
      state.detectedMidi = m;
      pluckGuitarFrequency(midiToFreq(m), 0.78);
      updateGuitarToKeyboardSync();
    }, i * 260);
  });
}

function randomizeScalePractice() {
  const scales = (state.initData && state.initData.scales) || [];
  state.rootPc = Math.floor(Math.random() * 12);
  if (scales.length > 0) {
    state.scaleId = scales[Math.floor(Math.random() * scales.length)].id;
  }
  saveSettings();
  renderAll();
}

async function buildArpeggioEventsForScalePractice() {
  const customParam = state.customStringMidis.join(',');
  let pcs = [];
  try {
    const vData = await fetchJson(`/api/voicings?tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(state.chordId)}&allTypes=0&accMode=${state.accMode}`);
    if (vData && vData.voicings && vData.voicings.length > 0) {
      const v0 = vData.voicings[0];
      (v0.frets || []).forEach((f, s) => {
        if (f >= 0) {
          const om = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
          const pc = ((om + f) % 12 + 12) % 12;
          if (!pcs.includes(pc)) pcs.push(pc);
        }
      });
    }
  } catch (e) {}
  if (pcs.length === 0) {
    const ivs = (typeof earChordsCatalog !== 'undefined')
      ? ((earChordsCatalog.find(c => c.id === state.chordId) || { intervals: [0, 4, 7] }).intervals)
      : [0, 4, 7];
    pcs = ivs.map(i => (state.rootPc + i) % 12);
  }
  const pcSet = new Set(pcs);
  const labels = getRootLabels();
  const minS = state.spStartString >= 0 ? state.spStartString : 0;
  const cells = [];
  for (let s = minS; s <= 5; s++) {
    const om = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    for (let f = 0; f <= 15; f++) {
      const m = om + f;
      const pc = ((m % 12) + 12) % 12;
      if (pcSet.has(pc)) {
        cells.push({ s, f, midi: m, note: labels[pc], freq: midiToFreq(m), dur: 1, tuplet: 0, rest: 0, tech: '-' });
      }
    }
  }
  // Wybierz unikalne wysokości rosnąco w kompaktowym zakresie gryfu
  cells.sort((a, b) => a.midi - b.midi || a.f - b.f);
  const uniq = [];
  let prevM = -1;
  cells.forEach(c => {
    if (c.midi !== prevM) {
      uniq.push(c);
      prevM = c.midi;
    }
  });
  const limit = state.spOctaves === 2 ? 9 : 5;
  let asc = uniq.slice(0, Math.min(limit, uniq.length));
  if (state.scalePracticeMode === 'arpeggio_pattern' && asc.length >= 3) {
    const pat = [];
    for (let i = 0; i < asc.length - 1; i++) {
      pat.push(asc[i], asc[i + 1]);
      if (i + 2 < asc.length) pat.push(asc[i + 2]);
    }
    asc = pat;
  }
  const desc = asc.slice().reverse();
  if (state.spDirection === 1) return desc;
  if (state.spDirection === 2) return asc.concat(desc.slice(1));
  if (state.spDirection === 3) return desc.concat(asc.slice(1));
  return asc;
}

async function launchScalePractice(mode) {
  state.posSystem = state.spPosSystem;
  saveSettings();
  let events = [];

  if (state.scalePracticeMode === 'scale') {
    const customParam = state.customStringMidis.join(',');
    const url = `/api/workshop_events?rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.spPosSystem}&startString=${state.spStartString}&direction=${state.spDirection}&octaves=${state.spOctaves}&pattern=${state.spPattern}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}`;
    const res = await fetchJson(url);
    events = (res && res.events ? res.events : []).map(ev => ({
      s: ev.s,
      f: ev.f,
      midi: ev.midi,
      note: ev.note,
      freq: ev.freq,
      dur: 1,
      tuplet: 0,
      rest: 0,
      tech: '-'
    }));
    if (events.length === 0 && state.spStartString >= 0) {
      const fallbackUrl = `/api/workshop_events?rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.spPosSystem}&startString=-1&direction=${state.spDirection}&octaves=${state.spOctaves}&pattern=${state.spPattern}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}`;
      const resFb = await fetchJson(fallbackUrl);
      events = (resFb && resFb.events ? resFb.events : []).map(ev => ({
        s: ev.s, f: ev.f, midi: ev.midi, note: ev.note, freq: ev.freq, dur: 1, tuplet: 0, rest: 0, tech: '-'
      }));
    }
  } else {
    events = await buildArpeggioEventsForScalePractice();
  }

  if (events.length === 0 && state.fretboardData && state.fretboardData.cells) {
    events = state.fretboardData.cells
      .filter(c => c.inScale && !c.masked)
      .slice(0, 12)
      .map(c => ({ s: c.s, f: c.f, midi: c.midi, note: c.note, freq: c.freq, dur: 1, tuplet: 0, rest: 0, tech: '-' }));
  }
  if (events.length === 0) return;

  state.recordedEvents = events;
  state.mainView = 'practice';
  state.subtab = 'exercise';
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-subtab') === 'exercise');
  });
  await startExercisePlayer(mode || 'play');
}

function addScalePracticeToSession(sessionIdx) {
  const scObj = (state.initData && state.initData.scales || []).find(s => s.id === state.scaleId) || { name: 'Durowa' };
  const rootLabels = getRootLabels();
  const posNames = ['Cały gryf', '3NPS', '4NPS', 'CAGED'];
  const patNames = {
    0: 'Cała skala', 3: 'Grupy po 3', 4: 'Grupy po 4',
    5: 'Tercje', 6: 'Kwarty', 7: 'Kwinty', 8: 'Seksty', 9: 'Oktawy'
  };
  const newEx = {
    id: 'ex_sp_' + Date.now(),
    title: `${rootLabels[state.rootPc]} ${scObj.name} · ${posNames[state.spPosSystem] || 'CAGED'} · ${patNames[state.spPattern] || 'Cała skala'}`,
    category: 'Scale Practice',
    level: state.spOctaves === 2 ? 'Średniozaawansowany' : 'Początkujący',
    rootPc: state.rootPc,
    scaleId: state.scaleId,
    posSystem: state.spPosSystem,
    startString: state.spStartString,
    direction: state.spDirection,
    octaves: state.spOctaves,
    pattern: state.spPattern,
    sessionIdx: sessionIdx
  };
  state.customWorkshopExercises.unshift(newEx);
  state.spAddToSessionOpen = false;
  renderStageAndInspector();
}

function surpriseScalePractice() {
  const scales = (state.initData && state.initData.scales) || [];
  const pats = [0, 3, 4, 5, 6, 7];
  state.scalePracticeMode = 'scale';
  state.rootPc = Math.floor(Math.random() * 12);
  if (scales.length > 0) {
    state.scaleId = scales[Math.floor(Math.random() * scales.length)].id;
  }
  state.spPosSystem = Math.floor(Math.random() * 4);
  state.spDirection = Math.floor(Math.random() * 4);
  state.spOctaves = Math.random() < 0.5 ? 1 : 2;
  state.spPattern = pats[Math.floor(Math.random() * pats.length)];
  launchScalePractice('play');
}

let stepNoteCursor = 0;
function addStepNoteFromScale() {
  if (!state.fretboardData) return;
  const minF = state.exerciseMinFret || 0;
  const maxF = state.exerciseMaxFret || 24;
  let active = state.fretboardData.cells.filter(c => c.inPosition && !isCellMasked(c.s, c.f) && c.f >= minF && c.f <= maxF);
  if (active.length === 0) {
    active = state.fretboardData.cells.filter(c => c.inPosition && !isCellMasked(c.s, c.f));
  }
  if (active.length === 0) return;
  const chosen = active[stepNoteCursor % active.length];
  stepNoteCursor++;
  pluckGuitarFrequency(chosen.freq, 0.9);
  state.recordedEvents.push({
    s: chosen.s,
    f: chosen.f,
    midi: chosen.midi,
    note: chosen.note,
    freq: chosen.freq,
    dur: state.defaultDuration,
    tuplet: state.tupletActive ? 3 : 0,
    rest: 0,
    tech: '-'
  });
  renderStageAndInspector();
}

function addScaleDegreeNoteToExercise(degIdx) {
  if (!state.fretboardData) return;
  const noteName = state.fretboardData.scaleNotes[degIdx];
  const chosen = findAscendingScaleCell(noteName, false);
  if (!chosen) return;
  state.isHoldingFretNote = true;
  state.lastHandPos = { s: chosen.s, f: chosen.f };
  state.detectedMidi = chosen.midi;
  startGuitarSustainedVoice('fret_mouse', chosen.freq, 18.0);
  drawFretboard();
  state.recordedEvents.push({
    s: chosen.s,
    f: chosen.f,
    midi: chosen.midi,
    note: chosen.note,
    freq: chosen.freq,
    dur: state.defaultDuration,
    tuplet: state.tupletActive ? 3 : 0,
    rest: 0,
    tech: '-'
  });
  renderStageAndInspector();
}

function addRestToExercise() {
  state.recordedEvents.push({
    s: -1,
    f: -1,
    midi: 0,
    dur: state.defaultDuration,
    tuplet: state.tupletActive ? 3 : 0,
    rest: 1,
    tech: '-'
  });
  renderStageAndInspector();
}

function undoLastExerciseEvent() {
  if (state.recordedEvents.length > 0) {
    state.recordedEvents.pop();
    renderStageAndInspector();
  }
}

function playComposedExercise() {
  startExercisePlayer('listen');
}

function detectAndApplyKey(detectedKeyStr) {
  if (!detectedKeyStr) return;
  const parts = detectedKeyStr.split(' ');
  const sharpLabels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
  const flatLabels = ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B'];
  let pc = sharpLabels.indexOf(parts[0]);
  if (pc < 0) pc = flatLabels.indexOf(parts[0]);
  if (pc >= 0) state.rootPc = pc;
  state.scaleId = (detectedKeyStr.toLowerCase().includes('mol')) ? 'natural_minor' : 'major';
  renderAll();
}

function shiftMidiOctave(delta) {
  stopAllSustainedVoices();
  state.activeMidiNotes.clear();
  state.midiBaseOctave = Math.max(2, Math.min(5, state.midiBaseOctave + delta));
  const lbl = document.getElementById('midiOctaveLabel');
  if (lbl) lbl.textContent = 'Baza: C ' + state.midiBaseOctave;
  buildMidiKeyboardUI();
}

function handleMidiNoteSideEffects(midi) {
  state.detectedMidi = midi;
  state.selectedScaleNotePc = ((Math.round(midi) % 12) + 12) % 12;
  drawFretboard();
  if (state.mainView === 'ear' && state.earScreen === 'active' && state.earMode === 'play') {
    checkEarPlayGuitarMidi(midi);
  }
  if (state.mainView === 'changes' && state.changesScreen === 'practice_active') {
    checkChangesPracticePlayedMidi(midi);
  }
  if (state.subtab === 'exercise') {
    if (state.exercisePlayerActive && state.exercisePlayerMode === 'play') {
      checkExercisePlayerPlayedMidi(midi);
    } else if (!state.exercisePlayerActive) {
      recordMidiNoteToTab(midi);
    }
  }
}

function startMouseMidiKeyHold(midi) {
  if (state.heldMouseMidi !== null && state.heldMouseMidi !== midi) {
    const prevEl = document.getElementById(`midiKey_${state.heldMouseMidi}`);
    if (prevEl) prevEl.classList.remove('active');
  }
  state.isHoldingMouseMidi = true;
  state.heldMouseMidi = midi;
  startKeyboardSustainedVoice('mouse_kbd', midi);
  handleMidiNoteSideEffects(midi);
  const el = document.getElementById(`midiKey_${midi}`);
  if (el) el.classList.add('active');
}

function onMouseMidiKeyEnter(midi) {
  if (!state.isHoldingMouseMidi || state.heldMouseMidi === midi) return;
  startMouseMidiKeyHold(midi);
}

function stopMouseMidiKeyHold() {
  stopSustainedVoice('mouse_kbd');
  if (state.heldMouseMidi !== null) {
    const el = document.getElementById(`midiKey_${state.heldMouseMidi}`);
    if (el) el.classList.remove('active');
  }
  state.isHoldingMouseMidi = false;
  state.heldMouseMidi = null;
  if (!micStream && state.activeMidiNotes.size === 0) {
    state.detectedMidi = null;
    drawFretboard();
  }
}

function triggerMidiNote(midi) {
  playKeyboardPresetNote(midiToFreq(midi), 1.15);
  handleMidiNoteSideEffects(midi);
  const el = document.getElementById(`midiKey_${midi}`);
  if (el) {
    el.classList.add('active');
    setTimeout(() => el.classList.remove('active'), 220);
  }
}

function toggleMidiFocus() {
  state.midiFocused = !state.midiFocused;
  const btn = document.getElementById('midiFocusBtn');
  if (state.midiFocused) {
    btn.classList.add('active');
    btn.textContent = '🎹 Klawisze';
  } else {
    stopAllSustainedVoices();
    state.activeMidiNotes.clear();
    document.querySelectorAll('.piano-key.active').forEach(el => el.classList.remove('active'));
    btn.classList.remove('active');
    btn.textContent = '🎹 Klawisze (wył.)';
  }
}

window.addEventListener('keydown', e => {
  if (e.target && (e.target.tagName === 'INPUT' || e.target.tagName === 'SELECT')) return;
  if (!state.midiFocused || e.repeat) return;
  const key = e.key.toLowerCase();
  if (key === 'z') { e.preventDefault(); shiftMidiOctave(-1); return; }
  if (key === 'x') { e.preventDefault(); shiftMidiOctave(1); return; }
  if (key === ' ' && state.subtab === 'exercise') { e.preventDefault(); addRestToExercise(); return; }
  if (key === 'backspace' && state.subtab === 'exercise') { e.preventDefault(); undoLastExerciseEvent(); return; }
  if (qwertyOffsets[key] !== undefined) {
    e.preventDefault();
    e.stopPropagation();
    const midi = (state.midiBaseOctave + 1) * 12 + qwertyOffsets[key];
    state.activeMidiNotes.add(midi);
    startKeyboardSustainedVoice('qwerty_' + key, midi);
    handleMidiNoteSideEffects(midi);
    const el = document.getElementById(`midiKey_${midi}`);
    if (el) el.classList.add('active');
  }
});

window.addEventListener('keyup', e => {
  if (!state.midiFocused) return;
  const key = e.key.toLowerCase();
  if (qwertyOffsets[key] !== undefined) {
    stopSustainedVoice('qwerty_' + key);
    const midi = (state.midiBaseOctave + 1) * 12 + qwertyOffsets[key];
    state.activeMidiNotes.delete(midi);
    const el = document.getElementById(`midiKey_${midi}`);
    if (el && !Array.from(state.activeMidiNotes).includes(midi)) {
      el.classList.remove('active');
    }
    if (!micStream && state.activeMidiNotes.size === 0 && !state.isHoldingFretNote && !state.isHoldingMouseMidi) {
      state.detectedMidi = null;
      drawFretboard();
    }
  }
});
</script>
</body>
</html>
)HTML";

} // namespace gitara::app::ui
