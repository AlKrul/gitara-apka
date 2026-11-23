#pragma once

namespace gitara::app::ui {

inline constexpr const char* kDesktopGuiHtml = R"HTML(<!DOCTYPE html>
<html lang="pl">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Apka Gitarowa – Studio Treningowe (A–G)</title>
<script type="module" src="/web/js/dsp/guitarDspEngine.js"></script>
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
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  user-select: none;
  scrollbar-width: thin;
  scrollbar-color: #3a3028 #14100d;
}
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}
::-webkit-scrollbar-track {
  background: #130f0c;
  border-radius: 6px;
}
::-webkit-scrollbar-thumb {
  background: #362d26;
  border-radius: 6px;
  border: 1.5px solid #130f0c;
}
::-webkit-scrollbar-thumb:hover {
  background: #574638;
}
::-webkit-scrollbar-thumb:active {
  background: rgba(243, 146, 55, 0.72);
}
::-webkit-scrollbar-corner {
  background: #130f0c;
}
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
  max-width: 180px;
  display: inline-block;
  overflow: hidden;
  text-overflow: ellipsis;
  text-align: center;
  white-space: nowrap;
  vertical-align: middle;
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
  filter: brightness(1.18) drop-shadow(0 0 6px rgba(243, 146, 55, 0.55));
}
.fb-node:hover .fb-note-dot {
  stroke: #f39237;
  stroke-width: 2.6;
}
.fb-hit-rect {
  fill: transparent;
  cursor: pointer;
}
.fb-hit-rect:hover {
  fill: rgba(243, 146, 55, 0.16);
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
.changes-workspace {
  max-width: 1240px;
  width: 100%;
  margin: 0 auto;
  display: flex;
  flex-direction: column;
  gap: 16px;
  padding: 4px 2px 18px;
}
.changes-player-card {
  background: linear-gradient(180deg, #1d1713 0%, #15110e 100%);
  border: 1px solid #30271f;
  border-radius: 14px;
  padding: 14px 18px;
  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.28);
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 14px;
}
.changes-section-box {
  background: #130f0c;
  border: 1px solid #26201a;
  border-radius: 13px;
  padding: 14px 16px;
  display: flex;
  flex-direction: column;
  gap: 10px;
}
.changes-chart-grid {
  display: grid;
  grid-template-columns: repeat(var(--chart-cols, 6), minmax(0, 1fr));
  gap: 8px;
}
@media (max-width: 960px) {
  .changes-chart-grid {
    grid-template-columns: repeat(4, minmax(0, 1fr));
  }
}
@media (max-width: 600px) {
  .changes-chart-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}
.changes-chart-card {
  position: relative;
  overflow: hidden;
  border-radius: 10px;
  padding: 11px 13px 13px;
  cursor: pointer;
  transition: border-color 0.14s, background 0.14s, box-shadow 0.14s, transform 0.12s;
  border: 1px solid #2b241e;
  border-left: 3px solid #3f3328;
  background: linear-gradient(180deg, #181411 0%, #110e0c 100%);
  min-width: 0;
  display: flex;
  flex-direction: column;
  gap: 4px;
}
.changes-chart-card:hover {
  border-color: #6b533d;
  border-left-color: var(--accent);
  transform: translateY(-1px);
}
.changes-chart-card.active-playing {
  border: 1.5px solid var(--accent) !important;
  border-left: 3px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.18) !important;
  box-shadow: 0 0 16px rgba(243, 146, 55, 0.28) !important;
}
.changes-chart-card .changes-card-prog {
  position: absolute;
  left: 0;
  bottom: 0;
  height: 3px;
  width: 0%;
  background: linear-gradient(90deg, #f39237, #fbbf24);
  transition: width 0.08s linear;
}
.changes-roman-pill {
  font-size: 10px;
  font-weight: 700;
  color: #cbd5e1;
  background: rgba(255, 255, 255, 0.06);
  border: 1px solid #302822;
  border-radius: 5px;
  padding: 1px 6px;
  white-space: nowrap;
}
.changes-arr-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 9px;
}
@media (max-width: 900px) {
  .changes-arr-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}
.changes-arr-bar {
  border: 1px solid #28211b;
  background: #0e0b09;
  border-radius: 9px;
  padding: 8px 9px;
  display: flex;
  flex-direction: column;
  gap: 6px;
  min-width: 0;
  transition: border-color 0.12s, background 0.12s;
}
.changes-arr-bar.active-playing {
  border: 1.5px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.09) !important;
}
.changes-beat-slot {
  border-radius: 6px;
  height: 38px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  min-width: 0;
  padding: 0 3px;
  transition: border-color 0.1s, background 0.1s;
}
.changes-beat-slot:hover {
  border-color: var(--accent) !important;
}
.changes-beat-slot.editing-slot {
  border: 1.5px solid #38bdf8 !important;
  background: rgba(56, 189, 248, 0.16) !important;
}
.changes-beat-slot.active-beat {
  border: 1.5px solid var(--accent) !important;
  background: rgba(243, 146, 55, 0.28) !important;
}
.changes-chord-editor-bar {
  background: #171310;
  border: 1px solid #4a3a2c;
  border-radius: 9px;
  padding: 8px 10px;
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 7px;
}
.changes-cta-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 14px;
  align-items: stretch;
}
@media (max-width: 820px) {
  .changes-cta-grid {
    grid-template-columns: 1fr;
  }
}
.changes-cta-card {
  background: linear-gradient(180deg, #16120f 0%, #110e0c 100%);
  border: 1px solid #2b241e;
  border-radius: 13px;
  padding: 16px 18px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  gap: 12px;
}
.changes-cta-card.changes-cta-primary {
  border-color: rgba(243, 146, 55, 0.38);
  background: linear-gradient(180deg, rgba(243, 146, 55, 0.08) 0%, #120e0b 100%);
}
:root[data-theme="light"] .changes-player-card,
:root[data-theme="light"] .changes-section-box,
:root[data-theme="light"] .changes-cta-card,
:root[data-theme="light"] .changes-chord-editor-bar {
  background: #f5f0e8;
  border-color: #d6cec2;
  box-shadow: none;
}
:root[data-theme="light"] .changes-cta-card.changes-cta-primary {
  background: linear-gradient(180deg, rgba(243, 146, 55, 0.14) 0%, #f5f0e8 100%);
  border-color: rgba(217, 119, 36, 0.45);
}
:root[data-theme="light"] .changes-chart-card,
:root[data-theme="light"] .changes-arr-bar {
  background: #ffffff;
  border-color: #d6cec2;
}
:root[data-theme="light"] .changes-roman-pill {
  background: rgba(0, 0, 0, 0.05);
  border-color: #d6cec2;
  color: #44403c;
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
  gap: 14px;
  margin-top: 10px;
  flex-wrap: wrap;
  justify-content: center;
}
.tuner-string-btn {
  width: 86px;
  height: 80px;
  background: var(--bg-card);
  border: 1.5px solid var(--border);
  border-radius: 11px;
  color: var(--text);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  gap: 3px;
  transition: border-color 0.14s, background 0.14s, transform 0.12s;
}
.tuner-string-btn:hover { border-color: var(--accent); transform: translateY(-1px); }
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
  background: linear-gradient(180deg, #9e420e 0%, #682807 100%);
  color: #fff5eb;
  border-color: #d97724;
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
/* 2-kolumnowa siatka kart nut w edytorze Ćwiczenie (TAB) — 2x więcej widocznych nut na ekranie! */
.ex-notes-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 6px;
  max-height: 295px;
  overflow-y: auto;
  padding-right: 4px;
}
@media (max-width: 1180px) {
  .ex-notes-grid { grid-template-columns: 1fr; }
}
.ex-note-card {
  display: flex;
  align-items: center;
  gap: 5px;
  flex-wrap: nowrap;
  background: #1a1511;
  border: 1px solid #2b231d;
  border-radius: 8px;
  padding: 5px 7px;
  font-size: 11.5px;
  min-width: 0;
}
.ex-note-card.ex-active-row {
  border-color: var(--accent);
  background: rgba(243, 146, 55, 0.11);
}
.ex-note-card select,
.ex-note-card input {
  padding: 3px 4px;
  font-size: 11px;
  background: #130f0c;
  border: 1px solid #2e2620;
  color: var(--text);
  border-radius: 5px;
  width: auto;
  min-width: 0;
}
.mic-gear-btn {
  width: 24px;
  height: 24px;
  border-radius: 6px;
  border: 1px solid #382f27;
  background: #1a1511;
  color: #b8aea3;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  font-size: 13px;
  line-height: 1;
  transition: 0.12s;
}
.mic-gear-btn:hover {
  border-color: var(--accent);
  color: var(--accent);
  background: rgba(243, 146, 55, 0.12);
}
.settings-back-btn {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 6px 12px;
  border-radius: 8px;
  border: 1px solid #4a392a;
  background: rgba(243, 146, 55, 0.10);
  color: var(--accent);
  font-size: 12.5px;
  font-weight: 700;
  cursor: pointer;
  transition: 0.12s;
}
.settings-back-btn:hover {
  background: rgba(243, 146, 55, 0.20);
  border-color: var(--accent);
}

/* Przełącznik skalowania interfejsu (Auto / Full HD / 2K) */
.ui-scale-switcher {
  background: var(--bg-card);
  border: 1px solid var(--border);
  border-radius: 9px;
  padding: 7px 8px;
  margin-bottom: 6px;
  display: flex;
  flex-direction: column;
  gap: 5px;
}
.ui-scale-head {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 10.5px;
  font-weight: 700;
  color: var(--text-muted);
  letter-spacing: 0.03em;
}
.ui-scale-badge {
  color: var(--accent);
  font-size: 10px;
  font-weight: 800;
  white-space: nowrap;
}
.ui-scale-seg {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 3px;
  background: var(--bg-main);
  padding: 2px;
  border-radius: 7px;
  border: 1px solid var(--border);
}
.ui-scale-btn {
  background: transparent;
  color: var(--text-muted);
  border: none;
  border-radius: 5px;
  padding: 4px 2px;
  font-size: 11px;
  font-weight: 700;
  cursor: pointer;
  transition: 0.12s;
  text-align: center;
  white-space: nowrap;
}
.ui-scale-btn:hover {
  color: var(--text);
  background: var(--bg-hover);
}
.ui-scale-btn.active {
  background: var(--accent);
  color: #120e0a;
}

/* Optymalizacje zagęszczenia i proporcji dla trybu Full HD (1920x1080 oraz laptopy 125%) */
body.ui-mode-fhd .sidebar {
  width: 194px;
  padding: 10px 8px;
  gap: 4px;
}
body.ui-mode-fhd .right-inspector {
  width: 300px;
  padding: 12px;
  gap: 10px;
}
body.ui-mode-fhd .topbar {
  min-height: 46px;
  padding: 5px 12px;
  gap: 6px 10px;
}
body.ui-mode-fhd .subtab-btn {
  padding: 5px 10px;
  font-size: 12px;
}
body.ui-mode-fhd .live-tuner-pill {
  padding: 3px 8px;
  gap: 6px;
  font-size: 11.5px;
}
body.ui-mode-fhd .main-stage {
  padding: 10px 14px;
  gap: 10px;
}
body.ui-mode-fhd .fretboard-card {
  padding: 11px 12px;
  gap: 6px;
}
body.ui-mode-fhd .midi-footer {
  min-height: 104px;
  padding: 5px 12px 6px;
  gap: 4px;
}
body.ui-mode-fhd .midi-keys {
  height: 68px;
  min-height: 68px;
}
body.ui-mode-fhd .ear-config-layout {
  gap: 20px;
}
body.ui-mode-fhd .ear-summary-box {
  width: 310px;
  padding: 16px;
}
body.ui-mode-fhd .ear-intervals-grid {
  grid-template-columns: repeat(auto-fill, minmax(84px, 1fr));
}
body.ui-mode-fhd .ear-interval-tile {
  width: auto;
  height: 58px;
}
body.ui-mode-fhd .ear-answer-grid {
  grid-template-columns: repeat(3, minmax(170px, 210px));
}
body.ui-mode-fhd .ear-answer-btn {
  width: auto;
  height: 62px;
}
body.ui-mode-fhd .sp-grid {
  grid-template-columns: minmax(0, 1fr) 330px;
  gap: 16px;
}
body.ui-mode-fhd .sp-card {
  padding: 14px 16px;
  gap: 12px;
}

/* Rozwijany pasek pięciolinii (nut) w sekcji Ćwiczenie (domyślnie zwinięty, TAB na wierzchu) */
.ex-staff-toggle-bar {
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  background: #1b1612;
  border: 1px solid #302821;
  border-radius: 9px;
  padding: 8px 12px;
  margin-bottom: 10px;
  color: var(--text);
  cursor: pointer;
  font-size: 12.5px;
  font-weight: 700;
  transition: border-color 0.14s, background 0.14s;
  text-align: left;
}
.ex-staff-toggle-bar:hover {
  border-color: var(--accent);
  background: #221c17;
}
.ex-staff-toggle-bar.open {
  border-color: rgba(243, 146, 55, 0.55);
  background: rgba(243, 146, 55, 0.08);
}
.ex-staff-toggle-bar .ex-staff-hint {
  font-size: 11.5px;
  font-weight: 500;
  color: var(--text-muted);
}
.ex-staff-toggle-bar .ex-staff-pill {
  display: inline-flex;
  align-items: center;
  gap: 5px;
  padding: 3px 10px;
  border-radius: 999px;
  font-size: 11px;
  font-weight: 700;
  background: var(--bg-main);
  border: 1px solid var(--border);
  color: var(--text-muted);
  white-space: nowrap;
}
.ex-staff-toggle-bar.open .ex-staff-pill {
  background: var(--accent);
  border-color: var(--accent);
  color: #120e0a;
}

/* Podłużny przycisk na prawej krawędzi do chowania i wysuwania prawego panelu */
.right-edge-toggle {
  width: 18px;
  flex-shrink: 0;
  background: #16120f;
  border: none;
  border-left: 1px solid var(--border);
  color: var(--text-muted);
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 0;
  transition: background 0.15s, color 0.15s, border-color 0.15s;
}
.right-edge-toggle:hover {
  background: #231d18;
  color: var(--accent);
  border-left-color: rgba(243, 146, 55, 0.55);
}
.right-edge-toggle .edge-pill {
  width: 14px;
  height: 76px;
  border-radius: 7px;
  background: #221c17;
  border: 1px solid #382f27;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 13px;
  font-weight: 800;
  line-height: 1;
  transition: 0.15s;
}
.right-edge-toggle:hover .edge-pill {
  border-color: var(--accent);
  background: rgba(243, 146, 55, 0.14);
  color: var(--accent);
}
.right-inspector.collapsed {
  display: none !important;
}

/* Przycisk chowania / wysuwania dolnej klawiatury (klawiszy) z prawej strony */
.midi-collapse-btn {
  margin-left: auto;
  background: var(--bg-card);
  color: var(--text-muted);
  border: 1px solid var(--border);
  border-radius: 7px;
  padding: 4px 12px;
  font-size: 11.5px;
  font-weight: 700;
  cursor: pointer;
  white-space: nowrap;
  transition: 0.14s;
}
.midi-collapse-btn:hover {
  color: var(--accent);
  border-color: var(--accent);
  background: #261f19;
}
.midi-footer.collapsed {
  display: none !important;
}
.midi-reveal-bar {
  position: fixed;
  right: 22px;
  bottom: 0;
  z-index: 40;
  background: #1d1814;
  color: var(--accent);
  border: 1px solid rgba(243, 146, 55, 0.5);
  border-bottom: none;
  border-radius: 9px 9px 0 0;
  padding: 5px 18px;
  font-size: 11.5px;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 -4px 16px rgba(0, 0, 0, 0.45);
  transition: background 0.14s, transform 0.14s;
}
.midi-reveal-bar:hover {
  background: #29211a;
  transform: translateY(-1px);
}

/* =========================================================
   JASNY MOTYW (Light Theme — wybierany w Ustawieniach)
   ========================================================= */
:root[data-theme="light"] {
  --bg-main: #f4f1ec;
  --bg-panel: #ece7df;
  --bg-card: #ffffff;
  --bg-hover: #e5dfd5;
  --border: #d6cec2;
  --accent: #d97316;
  --accent-glow: rgba(217, 115, 22, 0.14);
  --green: #16a34a;
  --yellow: #ca8a04;
  --red: #dc2626;
  --text: #1c1917;
  --text-muted: #57534e;
}
:root[data-theme="light"] * {
  scrollbar-color: #c7bfb3 #ece7df;
}
:root[data-theme="light"] ::-webkit-scrollbar-track { background: #ece7df; }
:root[data-theme="light"] ::-webkit-scrollbar-thumb { background: #c7bfb3; border-color: #ece7df; }
:root[data-theme="light"] .chip-btn,
:root[data-theme="light"] select,
:root[data-theme="light"] input[type="number"],
:root[data-theme="light"] input[type="text"],
:root[data-theme="light"] .list-row,
:root[data-theme="light"] .chord-box-card,
:root[data-theme="light"] .sp-card,
:root[data-theme="light"] .sp-summary-card,
:root[data-theme="light"] .sp-summary-pill,
:root[data-theme="light"] .ex-note-card,
:root[data-theme="light"] .ear-seg-row,
:root[data-theme="light"] .sp-dir-btn,
:root[data-theme="light"] .midi-footer,
:root[data-theme="light"] .right-edge-toggle .edge-pill,
:root[data-theme="light"] .midi-reveal-bar,
:root[data-theme="light"] .settings-back-btn,
:root[data-theme="light"] .mic-gear-btn {
  background: #ffffff;
  border-color: #d6cec2;
  color: #1c1917;
}
:root[data-theme="light"] .ear-seg-row {
  background: #eae4db;
}
:root[data-theme="light"] .chip-btn:hover,
:root[data-theme="light"] .sp-dir-btn:hover,
:root[data-theme="light"] .list-row:hover {
  background: #f0ebe3;
}
:root[data-theme="light"] .chip-btn.active,
:root[data-theme="light"] .ear-seg-btn.active,
:root[data-theme="light"] .sp-dir-btn.active,
:root[data-theme="light"] .round-pill.active,
:root[data-theme="light"] .subtab-btn.active {
  background: var(--accent);
  color: #ffffff;
  border-color: var(--accent);
}
:root[data-theme="light"] .action-btn {
  color: #ffffff;
}
:root[data-theme="light"] .action-btn.secondary {
  background: #e7e1d8;
  color: #1c1917;
  border-color: #cfc6b9;
}
:root[data-theme="light"] .live-tuner-pill {
  background: #ffffff;
  border-color: #d6cec2;
  color: #1c1917;
}
:root[data-theme="light"] .ear-toggle-switch {
  background: #cfc6b9;
  border-color: #bfb5a6;
}
:root[data-theme="light"] .ear-toggle-switch.on {
  background: var(--accent);
  border-color: var(--accent);
}
:root[data-theme="light"] .ex-staff-toggle-bar {
  background: #f3eee7;
  border-color: #d6cec2;
  color: #1c1917;
}

/* =========================================================
   ZWIĘZŁE PRZYCISKI POZYCJI + STYLE WIZUALNE CAŁEJ APLIKACJI
   (Klasyczny Studio / Neo-Glass Studio / Hardware Amp Rack)
   ========================================================= */
.pos-pill-grid {
  display: flex;
  flex-wrap: wrap;
  gap: 5px;
  align-items: center;
}
.pos-pill-grid-2col {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 5px;
}
.pos-pill-btn {
  height: 28px;
  padding: 0 9px;
  font-size: 11px;
  line-height: 1;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  max-width: 100%;
}

/* --- STYL 1 (ALTERNATYWA): NEO-GLASS STUDIO --- */
:root[data-ui-style="modern"] {
  --bg-main: #090d16;
  --bg-panel: rgba(16, 22, 36, 0.82);
  --bg-card: rgba(24, 34, 54, 0.78);
  --bg-hover: rgba(39, 54, 84, 0.88);
  --border: rgba(148, 163, 184, 0.18);
  --accent: #38bdf8;
  --accent-glow: rgba(56, 189, 248, 0.22);
  --green: #34d399;
  --yellow: #fbbf24;
  --red: #fb7185;
  --text: #f1f5f9;
  --text-muted: #94a3b8;
}
:root[data-ui-style="modern"] body {
  font-family: 'Inter', 'Plus Jakarta Sans', 'Segoe UI', system-ui, sans-serif;
  background:
    radial-gradient(900px 480px at 12% -8%, rgba(56, 189, 248, 0.14), transparent 60%),
    radial-gradient(820px 520px at 88% 8%, rgba(99, 102, 241, 0.13), transparent 60%),
    linear-gradient(180deg, #090d16 0%, #0d1322 100%);
}
:root[data-ui-style="modern"] .sidebar,
:root[data-ui-style="modern"] .topbar,
:root[data-ui-style="modern"] .right-inspector {
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
  background: rgba(14, 20, 34, 0.78);
}
:root[data-ui-style="modern"] .fretboard-card,
:root[data-ui-style="modern"] .sp-card,
:root[data-ui-style="modern"] .sp-summary-card,
:root[data-ui-style="modern"] .tuner-stage-card,
:root[data-ui-style="modern"] .changes-player-card,
:root[data-ui-style="modern"] .changes-section-box,
:root[data-ui-style="modern"] .changes-cta-card {
  backdrop-filter: blur(14px);
  -webkit-backdrop-filter: blur(14px);
  border-radius: 16px;
  border-color: rgba(148, 163, 184, 0.18);
  box-shadow: 0 12px 32px rgba(2, 6, 23, 0.38), inset 0 1px 0 rgba(255, 255, 255, 0.06);
}
:root[data-ui-style="modern"] .chip-btn,
:root[data-ui-style="modern"] .subtab-btn,
:root[data-ui-style="modern"] .ear-seg-btn,
:root[data-ui-style="modern"] .action-btn,
:root[data-ui-style="modern"] .settings-back-btn {
  border-radius: 999px;
}
:root[data-ui-style="modern"] .subtabs {
  border-radius: 999px;
  padding: 4px;
  background: rgba(10, 15, 28, 0.72);
}
:root[data-ui-style="modern"] .nav-btn {
  border-radius: 12px;
}
:root[data-ui-style="modern"] .nav-btn.active {
  background: linear-gradient(90deg, rgba(56, 189, 248, 0.22), rgba(99, 102, 241, 0.10));
  border-left: 3px solid #38bdf8;
}
:root[data-ui-style="modern"] .chip-btn.active,
:root[data-ui-style="modern"] .subtab-btn.active,
:root[data-ui-style="modern"] .ear-seg-btn.active,
:root[data-ui-style="modern"] .round-pill.active,
:root[data-ui-style="modern"] .action-btn {
  background: linear-gradient(135deg, #38bdf8 0%, #6366f1 100%);
  color: #ffffff;
  border-color: transparent;
  box-shadow: 0 4px 14px rgba(56, 189, 248, 0.28);
}
:root[data-ui-style="modern"] .chord-box-card,
:root[data-ui-style="modern"] .list-row,
:root[data-ui-style="modern"] select,
:root[data-ui-style="modern"] input[type="number"] {
  border-radius: 12px;
  background: rgba(20, 29, 48, 0.75);
  border-color: rgba(148, 163, 184, 0.20);
}
:root[data-ui-style="modern"] .fretboard-resizable-wrap,
:root[data-ui-style="modern"] #arpTabSvgWrap,
:root[data-ui-style="modern"] #sweepCompTabSvgWrap {
  background: linear-gradient(180deg, rgba(13, 19, 34, 0.96) 0%, rgba(9, 13, 24, 0.98) 100%) !important;
  border-color: rgba(56, 189, 248, 0.22) !important;
  border-radius: 14px !important;
  box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.05), 0 8px 24px rgba(2, 6, 23, 0.45);
}
:root[data-ui-style="modern"] * { scrollbar-color: #334155 #090d16; }
:root[data-ui-style="modern"] ::-webkit-scrollbar-track { background: #090d16; }
:root[data-ui-style="modern"] ::-webkit-scrollbar-thumb { background: #334155; border-color: #090d16; }

/* Neo-Glass Studio w trybie Jasnym */
:root[data-theme="light"][data-ui-style="modern"] {
  --bg-main: #edf2f8;
  --bg-panel: rgba(255, 255, 255, 0.80);
  --bg-card: rgba(244, 247, 252, 0.90);
  --bg-hover: rgba(226, 236, 248, 0.96);
  --border: rgba(15, 23, 42, 0.11);
  --accent: #0284c7;
  --accent-glow: rgba(2, 132, 199, 0.16);
  --text: #0f172a;
  --text-muted: #475569;
}
:root[data-theme="light"][data-ui-style="modern"] body {
  background:
    radial-gradient(900px 460px at 10% -6%, rgba(56, 189, 248, 0.18), transparent 60%),
    radial-gradient(820px 520px at 90% 10%, rgba(99, 102, 241, 0.14), transparent 60%),
    linear-gradient(180deg, #edf2f8 0%, #e4ebf5 100%);
}
:root[data-theme="light"][data-ui-style="modern"] .sidebar,
:root[data-theme="light"][data-ui-style="modern"] .topbar,
:root[data-theme="light"][data-ui-style="modern"] .right-inspector {
  background: rgba(255, 255, 255, 0.76);
}
:root[data-theme="light"][data-ui-style="modern"] .fretboard-card,
:root[data-theme="light"][data-ui-style="modern"] .sp-card,
:root[data-theme="light"][data-ui-style="modern"] .tuner-stage-card {
  box-shadow: 0 10px 28px rgba(15, 23, 42, 0.08), inset 0 1px 0 rgba(255, 255, 255, 0.9);
}
:root[data-theme="light"][data-ui-style="modern"] .subtabs {
  background: rgba(226, 234, 245, 0.85);
}
:root[data-theme="light"][data-ui-style="modern"] .chip-btn,
:root[data-theme="light"][data-ui-style="modern"] .chord-box-card,
:root[data-theme="light"][data-ui-style="modern"] .list-row,
:root[data-theme="light"][data-ui-style="modern"] select,
:root[data-theme="light"][data-ui-style="modern"] input[type="number"] {
  background: rgba(255, 255, 255, 0.92);
  border-color: rgba(15, 23, 42, 0.12);
}
:root[data-theme="light"][data-ui-style="modern"] .chip-btn.active,
:root[data-theme="light"][data-ui-style="modern"] .subtab-btn.active,
:root[data-theme="light"][data-ui-style="modern"] .ear-seg-btn.active,
:root[data-theme="light"][data-ui-style="modern"] .round-pill.active,
:root[data-theme="light"][data-ui-style="modern"] .action-btn {
  background: linear-gradient(135deg, #0284c7 0%, #4f46e5 100%);
  color: #ffffff;
}
:root[data-theme="light"][data-ui-style="modern"] .fretboard-resizable-wrap,
:root[data-theme="light"][data-ui-style="modern"] #arpTabSvgWrap,
:root[data-theme="light"][data-ui-style="modern"] #sweepCompTabSvgWrap {
  background: linear-gradient(180deg, #f8fafc 0%, #eef4fb 100%) !important;
  border-color: rgba(2, 132, 199, 0.24) !important;
  box-shadow: inset 0 1px 0 #ffffff, 0 6px 20px rgba(15, 23, 42, 0.08);
}

/* --- STYL 2 (ALTERNATYWA): HARDWARE AMP RACK --- */
:root[data-ui-style="rack"] {
  --bg-main: #0c0d0f;
  --bg-panel: #14161a;
  --bg-card: #1d2026;
  --bg-hover: #272b33;
  --border: #3a404c;
  --accent: #fbbf24;
  --accent-glow: rgba(251, 191, 36, 0.20);
  --green: #4ade80;
  --yellow: #fde047;
  --red: #ef4444;
  --text: #f3f4f6;
  --text-muted: #9ca3af;
}
:root[data-ui-style="rack"] body {
  font-family: 'JetBrains Mono', 'Fira Code', 'Cascadia Code', 'Consolas', monospace;
  background:
    repeating-linear-gradient(90deg, rgba(255,255,255,0.012) 0px, rgba(255,255,255,0.012) 1px, transparent 1px, transparent 6px),
    linear-gradient(180deg, #0c0d0f 0%, #121418 100%);
  letter-spacing: -0.01em;
}
:root[data-ui-style="rack"] .fretboard-card,
:root[data-ui-style="rack"] .sp-card,
:root[data-ui-style="rack"] .tuner-stage-card,
:root[data-ui-style="rack"] .changes-player-card,
:root[data-ui-style="rack"] .changes-section-box,
:root[data-ui-style="rack"] .changes-cta-card,
:root[data-ui-style="rack"] .chord-box-card,
:root[data-ui-style="rack"] .list-row,
:root[data-ui-style="rack"] .audio-status-box {
  border-radius: 3px;
  border: 1px solid #3f4654;
  box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.06), inset 0 -1px 0 rgba(0, 0, 0, 0.65), 0 4px 12px rgba(0, 0, 0, 0.45);
}
:root[data-ui-style="rack"] .section-label {
  letter-spacing: 0.12em;
  color: #eab308;
}
:root[data-ui-style="rack"] .chip-btn,
:root[data-ui-style="rack"] .subtab-btn,
:root[data-ui-style="rack"] .ear-seg-btn,
:root[data-ui-style="rack"] .nav-btn,
:root[data-ui-style="rack"] .action-btn,
:root[data-ui-style="rack"] .settings-back-btn,
:root[data-ui-style="rack"] select,
:root[data-ui-style="rack"] input[type="number"] {
  border-radius: 2px;
}
:root[data-ui-style="rack"] .chip-btn,
:root[data-ui-style="rack"] .subtab-btn {
  background: linear-gradient(180deg, #252932 0%, #1a1d24 100%);
  border: 1px solid #3b414e;
  box-shadow: inset 0 1px 0 rgba(255,255,255,0.06);
}
:root[data-ui-style="rack"] .chip-btn.active,
:root[data-ui-style="rack"] .subtab-btn.active,
:root[data-ui-style="rack"] .ear-seg-btn.active,
:root[data-ui-style="rack"] .round-pill.active,
:root[data-ui-style="rack"] .action-btn {
  background: linear-gradient(180deg, #322c1b 0%, #1f1b10 100%);
  color: #fde047;
  border-color: #fbbf24;
  box-shadow: inset 0 3px 0 #fbbf24, 0 0 10px rgba(251, 191, 36, 0.24);
}
:root[data-ui-style="rack"] .nav-btn.active {
  background: rgba(251, 191, 36, 0.14);
  color: #fde047;
  border-left: 4px solid #fbbf24;
}
:root[data-ui-style="rack"] .fretboard-resizable-wrap,
:root[data-ui-style="rack"] #arpTabSvgWrap,
:root[data-ui-style="rack"] #sweepCompTabSvgWrap {
  background: #0a0b0d !important;
  border: 2px solid #353b47 !important;
  border-radius: 3px !important;
  box-shadow: inset 0 2px 10px rgba(0, 0, 0, 0.75);
}
:root[data-ui-style="rack"] * { scrollbar-color: #4b5563 #0c0d0f; }
:root[data-ui-style="rack"] ::-webkit-scrollbar-track { background: #0c0d0f; }
:root[data-ui-style="rack"] ::-webkit-scrollbar-thumb { background: #4b5563; border-color: #0c0d0f; }

/* Hardware Amp Rack w trybie Jasnym (Silverface Aluminum Panel) */
:root[data-theme="light"][data-ui-style="rack"] {
  --bg-main: #d8dce2;
  --bg-panel: #e4e7ec;
  --bg-card: #f1f3f6;
  --bg-hover: #d1d6df;
  --border: #9ca3af;
  --accent: #b45309;
  --accent-glow: rgba(180, 83, 9, 0.16);
  --text: #111827;
  --text-muted: #4b5563;
}
:root[data-theme="light"][data-ui-style="rack"] body {
  background:
    repeating-linear-gradient(90deg, rgba(0,0,0,0.018) 0px, rgba(0,0,0,0.018) 1px, transparent 1px, transparent 5px),
    linear-gradient(180deg, #d8dce2 0%, #cdd2da 100%);
}
:root[data-theme="light"][data-ui-style="rack"] .fretboard-card,
:root[data-theme="light"][data-ui-style="rack"] .sp-card,
:root[data-theme="light"][data-ui-style="rack"] .tuner-stage-card,
:root[data-theme="light"][data-ui-style="rack"] .chord-box-card,
:root[data-theme="light"][data-ui-style="rack"] .list-row {
  border-color: #8f98a6;
  box-shadow: inset 0 1px 0 rgba(255, 255, 255, 0.85), 0 3px 8px rgba(0, 0, 0, 0.12);
}
:root[data-theme="light"][data-ui-style="rack"] .section-label {
  color: #92400e;
}
:root[data-theme="light"][data-ui-style="rack"] .chip-btn,
:root[data-theme="light"][data-ui-style="rack"] .subtab-btn {
  background: linear-gradient(180deg, #f9fafb 0%, #e5e7eb 100%);
  border-color: #9ca3af;
  color: #111827;
}
:root[data-theme="light"][data-ui-style="rack"] .chip-btn.active,
:root[data-theme="light"][data-ui-style="rack"] .subtab-btn.active,
:root[data-theme="light"][data-ui-style="rack"] .ear-seg-btn.active,
:root[data-theme="light"][data-ui-style="rack"] .round-pill.active,
:root[data-theme="light"][data-ui-style="rack"] .action-btn {
  background: linear-gradient(180deg, #fef3c7 0%, #fde68a 100%);
  color: #78350f;
  border-color: #b45309;
  box-shadow: inset 0 3px 0 #d97706, 0 2px 6px rgba(180, 83, 9, 0.20);
}
:root[data-theme="light"][data-ui-style="rack"] .fretboard-resizable-wrap,
:root[data-theme="light"][data-ui-style="rack"] #arpTabSvgWrap,
:root[data-theme="light"][data-ui-style="rack"] #sweepCompTabSvgWrap {
  background: #eceff4 !important;
  border: 2px solid #8f98a6 !important;
  box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.14);
}

/* =========================================================
   TRYB ŚCIEŻKI SKALI (ZDJĘCIE 3 — POZIOMA WSTĘGA NUT + WOLNY GRYF)
   ========================================================= */
.scale-ribbon-stage {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 14px;
  padding: 20px 18px;
  background: var(--bg-panel);
  border: 1px solid var(--border);
  border-radius: 14px;
  position: relative;
  overflow: hidden;
  min-height: 112px;
}
.scale-ribbon-heard {
  min-width: 68px;
  height: 62px;
  border-radius: 12px;
  background: var(--bg-card);
  border: 1.5px solid var(--border);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  font-weight: 800;
  font-size: 16px;
  color: var(--text-muted);
  flex-shrink: 0;
}
.scale-ribbon-heard.hit {
  border-color: var(--green);
  color: var(--green);
  background: rgba(74, 222, 128, 0.12);
}
.scale-ribbon-track {
  display: flex;
  align-items: center;
  gap: 10px;
  flex: 1;
  justify-content: center;
  overflow: hidden;
}
.ribbon-card-cur {
  min-width: 72px;
  height: 70px;
  border-radius: 14px;
  background: var(--accent);
  color: #14100d;
  font-size: 24px;
  font-weight: 900;
  display: flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 0 26px rgba(243, 146, 55, 0.36);
  padding: 0 12px;
  flex-shrink: 0;
}
.ribbon-card-cur sub, .ribbon-card-next sub, .ribbon-card-future sub {
  font-size: 12px;
  font-weight: 700;
  opacity: 0.8;
  margin-left: 1px;
}
.ribbon-card-next {
  min-width: 64px;
  height: 66px;
  border-radius: 13px;
  background: var(--bg-card);
  border: 1.5px solid rgba(243, 146, 55, 0.55);
  color: var(--text);
  font-size: 20px;
  font-weight: 800;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 0 10px;
  flex-shrink: 0;
}
.ribbon-card-future {
  min-width: 48px;
  text-align: center;
  font-size: 17px;
  font-weight: 700;
  color: var(--text-muted);
  flex-shrink: 0;
}
.ribbon-dir-pill {
  position: absolute;
  top: 10px;
  left: 14px;
  font-size: 10.5px;
  font-weight: 800;
  letter-spacing: 0.7px;
  text-transform: uppercase;
  padding: 3px 9px;
  border-radius: 6px;
  background: rgba(243, 146, 55, 0.14);
  color: var(--accent);
  border: 1px solid rgba(243, 146, 55, 0.35);
}

/* Karty ćwiczeń flażoletów */
.harm-ex-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(290px, 1fr));
  gap: 10px;
}
.harm-ex-card {
  background: var(--bg-card);
  border: 1px solid var(--border);
  border-radius: 11px;
  padding: 12px 14px;
  display: flex;
  flex-direction: column;
  gap: 8px;
  transition: border-color 0.15s, background 0.15s;
}
.harm-ex-card.active {
  border-color: var(--accent);
  box-shadow: 0 0 0 1px rgba(243, 146, 55, 0.25);
}
</style>
</head>
<body>
<div class="app-shell">
  <!-- Lewy pasek nawigacji -->
  <aside class="sidebar">
    <div id="brandTitle" class="brand">🎸 Apka Gitarowa</div>
    <button id="navPracticeBtn" class="nav-btn active" onclick="switchMainView('practice', this)">🎵 Praktyka</button>
    <button id="navEarBtn" class="nav-btn" onclick="switchMainView('ear', this)">👂 Kształcenie słuchu</button>
    <button id="navChangesBtn" class="nav-btn" onclick="switchMainView('changes', this)">🔄 Zmiany</button>
    <button id="navWorkshopBtn" class="nav-btn" onclick="switchMainView('workshop', this)">🛠️ Warsztat</button>

    <!-- Dolna sekcja lewego paska: Audio + Stroik i Ustawienia na samym dole po lewej -->
    <div class="audio-status-box">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:5px;gap:6px;">
        <div style="display:flex;align-items:center;gap:6px;">
          <span id="micDot" class="live-dot"></span>
          <span id="micTitleLabel" style="font-weight:700;font-size:12px;">Mikrofon</span>
        </div>
        <button class="mic-gear-btn" onclick="goToAudioSettings()" title="Ustawienia audio i mikrofonu">⚙</button>
      </div>
      <div id="micStatusText" style="font-size:10.5px;color:var(--text-muted);margin-bottom:7px;">Wejście wył.</div>
      <button id="toggleMicBtn" class="action-btn" style="width:100%;padding:5px 8px;font-size:11.5px;" onclick="toggleLiveAudio()">
        🎤 Włącz
      </button>
    </div>

    <div style="border-top:1px solid var(--border);padding-top:8px;margin-top:6px;display:flex;flex-direction:column;gap:4px;">
      <button id="navTunerBtn" class="nav-btn" onclick="switchMainView('tuner', this)">🎛️ Stroik (Tuner)</button>
      <button id="navSettingsBtn" class="nav-btn" onclick="switchMainView('settings', this)">⚙️ Ustawienia</button>
    </div>
  </aside>

  <!-- Główna część -->
  <main class="workspace">
    <!-- Górny pasek zakładek -->
    <header class="topbar">
      <div id="practiceSubtabs" class="subtabs">
        <button class="subtab-btn active" data-subtab="scale" onclick="switchPracticeSubtab('scale', this)">Skala</button>
        <button class="subtab-btn" data-subtab="chord" onclick="switchPracticeSubtab('chord', this)">Akord · Odkrywanie</button>
        <button class="subtab-btn" data-subtab="harmonics" onclick="switchPracticeSubtab('harmonics', this)">Flażolety</button>
        <button class="subtab-btn" data-subtab="search" onclick="switchPracticeSubtab('search', this)">Szukaj</button>
        <button class="subtab-btn" data-subtab="arpeggio" onclick="switchPracticeSubtab('arpeggio', this)">Arpeggio</button>
        <button class="subtab-btn" data-subtab="sweep" onclick="switchPracticeSubtab('sweep', this)">⚡ Sweep Picking</button>
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
        <span id="liveCentsBadge" style="color:var(--text-muted);"></span>
        <button id="topPreviewBtn" class="chip-btn" style="padding:4px 9px;font-size:12px;white-space:nowrap;" onclick="playCurrentScalePreview()">▶ Posłuchaj</button>
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

      <!-- Podłużny przycisk na prawej krawędzi do chowania / wysuwania prawego panelu -->
      <button id="rightInspectorEdgeToggle" class="right-edge-toggle" onclick="toggleRightInspectorPanel()"
              title="Schowaj lub wysuń prawy panel parametrów">
        <span id="rightEdgeArrow" class="edge-pill">›</span>
      </button>

      <!-- Prawy inspektor parametrów -->
      <aside class="right-inspector" id="rightInspector"></aside>
    </div>
  </main>
</div>

<!-- Dolny moduł wirtualnej klawiatury MIDI i klawiszy do komponowania -->
<footer id="midiFooter" class="midi-footer">
  <div class="midi-bar">
    <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;flex:1;">
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
      <button id="midiHideBtn" class="midi-collapse-btn" onclick="toggleMidiKeyboardPanel()"
              title="Schowaj dolną klawiaturę MIDI (klawisze)">
        ▾ Schowaj klawisze
      </button>
    </div>
  </div>
  <div id="midiKeysContainer" class="midi-keys"></div>
</footer>
<button id="midiRevealBtn" class="midi-reveal-bar" style="display:none;" onclick="toggleMidiKeyboardPanel()"
        title="Wysuń dolną klawiaturę MIDI (klawisze)">
  🎹 Pokaż klawisze ▴
</button>

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
  chordCapoFret: 0, // Kapodaster w Odkrywaniu Akordów (0 = brak, 1..12 = próg kapodastra)
  selectedChordVoicingIdx: 0,
  chordDiatonicSevenths: false, // Koło kwintowe: trójdźwięki (false) / akordy septymowe (true)
  chordShowCircle: true, // Pokaż panel koła kwintowego i akordów w skali
  cofTemplateMode: false, // Tryb szablonu 6 dźwięków (IV-I-V / ii-vi-iii) na Kole Kwintowym
  cofHighlightPc: null, // Chwilowo podświetlony dźwięk z paska „Dźwięki koła”
  diatonicData: null,
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
  // Skalowanie interfejsu (Auto / Full HD / 2K) oraz chowanie paneli
  uiScaleMode: 'auto', // 'auto' | 'fhd' | '2k'
  resolvedUiProfile: '2k',
  appliedUiZoom: 1.0,
  midiKeyboardHidden: false,
  rightInspectorHidden: false,
  exLastRenderedCount: -1,
  // Skalowanie gryfu wokół punktu 0 (oraz automatyczne wypełnianie pełnego ekranu bez ściskania przy małym oknie!)
  fretboardZoom: 0, // -5 .. 0 .. +10
  fretboardFitWidth: true,
  lockUnmarkedNotes: false, // Blokada klikania niezaznaczonych (spoza skali/akordu lub zamaskowanych) dźwięków na gryfie
  markedFretCells: new Set(),
  // Urządzenia audio i detektor polifoniczny (PitchPlease WASM SIMD + kalibracja z gitary)
  selectedAudioInputId: 'default',
  selectedAudioOutputId: 'default',
  audioInputDevices: [{ id: 'default', label: 'Domyślne wejście (Mikrofon / Interfejs USB)' }],
  audioOutputDevices: [{ id: 'default', label: 'Domyślne wyjście (Głośniki / Słuchawki)' }],
  dspModel: 'fast', // 'fast' (uniwersalny pod wzmacniacz/mikrofon, 1x CPU) | 'voice' (GuitarSet) | 'piano' (czysty akustyk)
  dspThresholdOn: 0.58,
  dspHighThresholdOn: 0.42, // Osobny, niższy próg dla cienkich strun (G3, B2, E1)
  dspAmpPreEq: true, // Kondycjonowanie pod wzmacniacz/mikrofon (HPF 68 Hz + tilt EQ +6 dB + lekki leveler + Goertzel f0)
  dspNoiseGateRms: 0.0020,
  dspCalibrated: false,
  dspDefaultsV2: true,
  dspCalibrating: false,
  dspCalibStatus: '',
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
  changesCompInst: 'hammond', // 'hammond' (Organy) | 'clean' (Gitara czysta) | 'piano' (Fortepian) | 'rhodes' (Rhodes) | 'crunch' (Gitara przesterowana) | 'off'
  changesBass: 'Wył.',
  changesAttackStrength: 0.75, // Siła ataku zespołu akompaniującego (0.20..1.00)
  changesGroove: 'shuffle', // 'shuffle' | 'slow12_8'
  changesRhythmGuitar: true,
  changesLoop: true,
  changesChordDiagrams: true,
  changesBackingBand: true,
  changesMetronomeClick: true,
  changesDrums: true, // Perkusja w zespole podkładowym
  changesDrumStyle: 'auto', // 'auto' | 'rock' | 'pop' | 'blues' | 'swing' | 'funk' | 'half_time'
  changesCycleKeys: false,
  changesMeasures: null, // Inicjalizowane z aktywnego standardu
  changesIsPlaying: false,
  changesIsPaused: false,
  changesActiveBarIdx: -1,
  changesActiveBeatIdx: -1,
  changesPlaybackSeq: [],
  changesPlaybackSeqIdx: 0,
  changesRepeatPass: 0,
  changesEditingSlot: null, // { barIdx, beatIdx } dla szybkiego edytora akordu
  changesCountIn: 0,
  changesPracticeGoal: 'chord_tones', // 'chord_tones' (Dźwięki akordu) | 'chord_scale' (Skala akordu)
  changesPracticeMode: 'tempo', // 'tempo' (W tempie) | 'study' (Ćwiczebny)
  changesPracticeDegrees: 'all', // 'all' | '1' | '3' | '5'
  changesPracticeOrder: 'any', // 'any' (Dowolna) | 'seq' (Po kolei)
  changesPracticeShowFretboard: true,
  changesPracticeHitPcs: [],
  changesPracticeSeqIdx: 0,
  changesPracticeLastMidi: -1,
  changesPracticeLastMidiTime: 0,
  changesToastMsg: '',
  customSavedSessions: [],
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
  searchDir: 'chord_to_scale', // 'chord_to_scale' | 'scale_to_chord' | 'detect'
  searchData: null,
  selectedSearchScaleIdx: -1, // -1 = pokazuj tylko dźwięki akordu; >=0 = koloruj wg roli (zielony/żółty/czerwony)
  selectedSearchChordIdx: -1,
  searchLabelType: 'notes', // 'notes' | 'intervals' | 'tensions'
  detectScaleMode: 'all', // 'all' | 'major' | 'minor'
  detectScalePcs: [], // wykryte klasy wysokości (tryb Wykryj skalę)
  detectScaleData: null,
  // Stan Stroika
  tunerMode: 'guitar', // 'guitar' | 'chromatic'
  tunerDisplay: 'needle', // 'needle' | 'strobe'
  tunerLockedString: -1, // -1 = auto, 0..5 = zablokowana struna
  // Stan Ustawień i Wyglądu (bez grywalizacji / celu dziennego)
  themeMode: 'dark', // 'dark' | 'light' | 'system'
  uiStyle: 'classic', // 'classic' | 'modern' | 'rack'
  uiLang: 'pl', // 'pl' | 'en'
  fretMarkersMode: 'numbers', // 'dots' | 'numbers'
  numberAllFrets: false, // Pokaż numery wszystkich progów (1..24)
  fretboardMargin: true, // Pokaż ±2 dodatkowe progi wokół zakresu ćwiczenia
  synthVolume: 0.85, // Głośność syntezatora gitary/klawiszy (0.1..1.0)
  metronomeVolume: 0.75, // Głośność metronomu (0.1..1.0)
  skillLevel: 'intermediate', // 'beginner' | 'intermediate' | 'advanced'
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
  // Zakładka Arpeggio (czyste arpeggia) i osobna zakładka ⚡ Sweep Picking
  arpViewMode: 'single', // 'single' | 'transition' (używane wewnętrznie w Sweep)
  sweepViewMode: 'transition', // 'transition' (Przejścia Sweep) | 'single' (Pojedyncze kształty Sweep 3/5/6 str.)
  arpPattern: 6, // 0=Cały gryf, 1=CAGED, 2=2NPS, 3=3NPS, 4=Sweep Picking, 5=Inwersje, 6=Od prymy (1-2 okt.)
  arpPosIdx: 0,
  sweepPosIdx: 0,
  sweepTransPosIdx: 0,
  arpSweepStrings: 3, // 3 | 5 | 6
  sweepMetricFit: 'sextuplets', // 'sixteenths' | 'sextuplets'
  sweepComposition: [], // Ułożone własne ogniwa Sweep w trybie Pojedynczy kształt
  sweepCompActiveIdx: 0,
  sweepCompactTabs: false, // Zmniejsza rozmiar tabulatur, by obie mieściły się jednocześnie
  sweepPlaybackTarget: 'single', // 'single' | 'comp'
  arpDirection: 0, // 0=W górę i w dół, 1=W górę, 2=W dół
  arpSeqStyle: 0, // 0=Prosto (1-3-5), 1=Grupy po 3, 2=Skoki (1-5-3)
  arpStringSet: 0, // 0=G-B-e/D-G-B-e, 1=D-G-B/A-D-G-B, 2=A-D-G/E-A-D-G
  arpOctaves: 2, // 1 | 2 dla trybu Od prymy
  arpStartStr: -1, // -1=auto (od najniższej struny), 0..5=struna od której zaczynamy arpeggio (0=E6 .. 5=e1)
  arpReturnStr: -1, // -1=pełny powrót (do najniższej), 0..5=struna do której wracamy w dół
  arpTransitionId: 'neoclassical_minor',
  arpActiveSegIdx: 0,
  arpDiatonicSevenths: true,
  arpDiatonicCategory: 'all', // 'all' | 'triad' | 'seventh' | 'sus_add_6' | 'extended'
  arpFilterByScale: false,
  arpAutoTempo: false,
  savedSweepCompositions: [],
  arpDiatonicScaleId: 'major',
  arpDiatonicScaleRootPc: 0,
  arpBpm: 90,
  arpLoop: false,
  sweepSequential: false, // W Sweep Picking domyślnie odtwarza wybrany sweep osobno; Wł. = wszystkie ogniwa po kolei
  arpShowScaleOnFretboard: false,
  arpPlaying: false,
  arpPlayStepIdx: -1,
  arpData: null,
  settingsReturnView: null, // { mainView, subtab } — do przycisku ← Wróć w Ustawieniach
  tabCreationEnabled: true,
  exerciseShowStaff: false, // Pięciolinia (nuty): domyślnie wyłączona, domyślnie sama tabulatura (TAB)
  exerciseStaffPrefV2: false,
  exerciseTabWrapRows: true, // Domyślnie zawijane wiersze jak tekst (bez poziomego paska przewijania)
  exerciseBarsPerRow: 0, // 0 = Auto (zawijaj gdy takt się nie mieści), lub 2 / 4 / 6 / 8 taktów w wierszu
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
  exerciseSubView: 'composer', // 'composer' | 'scale_practice' | 'harmonics'
  exercisePlayerActive: false,
  exercisePlayerMode: 'listen', // 'listen' | 'play'
  exercisePlayerView: 'tab', // 'tab' (Partytura TAB + Gryf) | 'ribbon' (Ścieżka skali / wolny gryf — Zdjęcie 3)
  exercisePlayerPaused: false,
  exercisePlayerStepIdx: 0,
  exerciseBaseBpm: 120,
  exerciseCompletedSet: new Set(),
  exerciseLastHitIdx: -1,
  exerciseLastHitTime: 0,
  exerciseLastHitLabel: '',
  selectedHarmonicExerciseId: 'harm_ballad_intro_em',
  // Stan zakładki Praktyka skal
  scalePracticeMode: 'scale', // 'scale' | 'arpeggio' | 'arpeggio_pattern'
  spPosSystem: 0, // 0=Brak, 1=3NPS, 2=4NPS, 3=CAGED
  spPosIdx: 0, // Numer pozycji (#1..#7 dla 3NPS, #1..#5 dla CAGED/4NPS)
  spTraversal: 0, // 0=Standard, 1=DiagonalShift (3 oktawy), 2=SingleString (1 struna poziomo)
  spStartString: -1, // -1=Dowolna, 0..5=struny
  spDirection: 0, // 0=Rosnąco, 1=Malejąco, 2=Rosnąco->Malejąco, 3=Malejąco->Rosnąco
  spOctaves: 2, // 1 | 2 | 3 oktawy
  spPattern: 0, // 0..18 (wszystkie sekwencje, interwały, trójdźwięki, septymowe, pedal point, string skipping)
  spStartOnRoot: true, // Rozpocznij bieg od toniki (prymy) skali
  spFretZone: 'all', // 'all' (0-24) | 'low' (0-7) | 'mid' (5-14) | 'high' (12-24)
  spNoteDur: 1, // 0=Ćwierćnuta, 1=Ósemka, 2=Szesnastka
  spTuplet: false, // true = Triole
  spBpm: 100, // Tempo ćwiczenia skalowego
  spUseHeadphones: true,
  spMetronome: true,
  spRefNote: false,
  spBgChord: false,
  spAddToSessionOpen: false,
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
let micSourceNode = null;
let micCaptureNode = null;
let micMuteSinkNode = null;
let liveGuitarDspEngine = null;
let livePitchPleaseDetector = null;

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

  // 2b. W zakładkach Arpeggio oraz Sweep Picking podświetlamy na klawiszach wyłącznie dokładne dźwięki aktualnego kształtu / kroku
  if (state.mainView === 'practice' && (state.subtab === 'arpeggio' || state.subtab === 'sweep') && state.arpData) {
    let arpNodes = state.arpData.nodes || [];
    const isSweepTrans = (state.subtab === 'sweep' && state.sweepViewMode === 'transition');
    if (isSweepTrans && state.arpData.transition && state.arpData.transition.segments) {
      const seg = state.arpData.transition.segments[state.arpActiveSegIdx || 0];
      arpNodes = seg ? (seg.nodes || []) : [];
    } else if (state.subtab === 'arpeggio' && state.arpPattern === 0 && (state.arpPlaying || state.arpPlayStepIdx >= 0)) {
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
      return;
    }
  }

  // 3. Aktualnie kliknięty / grany dźwięk (lub dwudźwięk / akord z mikrofonu): podświetl na klawiaturze!
  const activeMidis = getRecognitionMidis();

  const focusPc = (activeMidis.length > 0)
    ? (((Math.round(activeMidis[0]) % 12) + 12) % 12)
    : ((state.mainView === 'practice' && state.subtab === 'scale') ? state.selectedScaleNotePc : null);

  if (focusPc !== null && focusPc !== undefined) {
    if (state.mainView === 'practice' && state.subtab === 'scale' && !isScaleDegreeHighlightEnabled(focusPc)) {
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
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

const activePreviewVoices = new Set();
let previewTimers = [];
let previewSessionToken = 0;
let previewIsPlaying = false;

function stopPreviewVoices(fadeSec = 0.03) {
  if (activePreviewVoices.size === 0) return;
  const voices = Array.from(activePreviewVoices);
  activePreviewVoices.clear();
  voices.forEach(v => {
    try {
      const now = v.ctx.currentTime;
      v.gain.gain.cancelScheduledValues(now);
      v.gain.gain.setValueAtTime(Math.max(0.0001, v.gain.gain.value || 0.001), now);
      v.gain.gain.exponentialRampToValueAtTime(0.0001, now + fadeSec);
      v.src.stop(now + fadeSec + 0.01);
    } catch (e) {}
  });
}

function updateTopPreviewButtonUI() {
  const btn = document.getElementById('topPreviewBtn');
  if (btn) {
    btn.classList.toggle('active', !!previewIsPlaying);
    btn.textContent = previewIsPlaying ? '⏹ Zatrzymaj' : '▶ Posłuchaj';
  }
  const spBtn = document.getElementById('scalePracticePreviewBtn');
  if (spBtn) {
    spBtn.classList.toggle('active', !!previewIsPlaying);
    spBtn.textContent = previewIsPlaying ? '⏹ Zatrzymaj' : '▶ Posłuchaj skali';
  }
}

function stopCurrentScalePreview(clearHighlight = false) {
  previewSessionToken++;
  if (previewTimers.length > 0) {
    previewTimers.forEach(id => clearTimeout(id));
    previewTimers = [];
  }
  stopPreviewVoices(0.03);
  const wasPlaying = previewIsPlaying;
  previewIsPlaying = false;
  updateTopPreviewButtonUI();
  if (clearHighlight && wasPlaying && !micStream) {
    state.detectedMidi = null;
    drawFretboard();
  }
}

function pluckGuitarFrequency(freqHz, durationSec = 1.45, gainScale = 1.0, trackAsPreview = false) {
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
  const volMul = (state.synthVolume !== undefined) ? state.synthVolume : 0.85;
  noteOutGain.gain.setValueAtTime(Math.max(0.03, Math.min(1.5, gainScale * volMul)), now);
  noteOutGain.connect(getMasterAudioOut());

  if (trackAsPreview) {
    const voiceRec = { src, gain: noteOutGain, ctx };
    activePreviewVoices.add(voiceRec);
    src.onended = () => activePreviewVoices.delete(voiceRec);
  }

  connectGuitarToneChain(ctx, src, noteOutGain, tone, now, now + effDur, null);
  src.start(now);
}

// Krystaliczny dzwonkowy dźwięk flażoletu naturalnego (czyste alikwoty + szybki błysk ataku)
function playHarmonicBellFrequency(freqHz, durationSec = 1.65) {
  if (!freqHz || freqHz < 40) return;
  if (state.syncGuitarToKeyboard) {
    const m = Math.round(69 + 12 * Math.log2(freqHz / (state.a4ReferenceHz || 440.0)));
    flashGuitarNoteOnKeyboard(m, Math.min(900, Math.max(260, Math.round(durationSec * 460))));
  }
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const volMul = (state.synthVolume !== undefined) ? state.synthVolume : 0.85;
  const master = ctx.createGain();
  master.gain.setValueAtTime(0.0001, now);
  master.gain.linearRampToValueAtTime(0.42 * volMul, now + 0.008);
  master.gain.exponentialRampToValueAtTime(0.0008, now + durationSec);
  master.connect(getMasterAudioOut());

  const partials = [
    { mult: 1.0, amp: 0.78, dur: durationSec },
    { mult: 2.0, amp: 0.24, dur: durationSec * 0.58 },
    { mult: 3.0, amp: 0.08, dur: durationSec * 0.34 }
  ];
  partials.forEach(p => {
    const osc = ctx.createOscillator();
    const g = ctx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(freqHz * p.mult, now);
    g.gain.setValueAtTime(p.amp, now);
    g.gain.exponentialRampToValueAtTime(0.0005, now + p.dur);
    osc.connect(g);
    g.connect(master);
    osc.start(now);
    osc.stop(now + p.dur + 0.04);
  });
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

function t(pl, en) {
  return (state.uiLang === 'en') ? en : pl;
}

function resolveEffectiveTheme() {
  const m = state.themeMode || 'dark';
  if (m === 'system') {
    const isLightSys = (typeof window !== 'undefined' && window.matchMedia && window.matchMedia('(prefers-color-scheme: light)').matches);
    return isLightSys ? 'light' : 'dark';
  }
  return (m === 'light') ? 'light' : 'dark';
}

function isLightTheme() {
  return resolveEffectiveTheme() === 'light';
}

function applyTheme() {
  const effStyle = ['classic', 'modern', 'rack'].includes(state.uiStyle) ? state.uiStyle : 'classic';
  document.documentElement.setAttribute('data-theme', resolveEffectiveTheme());
  document.documentElement.setAttribute('data-ui-style', effStyle);
}

function setThemeMode(mode) {
  if (!['dark', 'light', 'system'].includes(mode)) mode = 'dark';
  state.themeMode = mode;
  applyTheme();
  saveSettings();
  drawFretboard();
  renderStageAndInspector();
}

function setUiStyle(style) {
  if (!['classic', 'modern', 'rack'].includes(style)) style = 'classic';
  state.uiStyle = style;
  applyTheme();
  saveSettings();
  drawFretboard();
  renderStageAndInspector();
}

function applyUiLanguage() {
  const en = (state.uiLang === 'en');
  document.documentElement.lang = en ? 'en' : 'pl';
  const setTxt = (id, plTxt, enTxt) => {
    const el = document.getElementById(id);
    if (el) el.textContent = en ? enTxt : plTxt;
  };
  setTxt('brandTitle', '🎸 Apka Gitarowa', '🎸 Guitar Studio');
  setTxt('navPracticeBtn', '🎵 Praktyka', '🎵 Practice');
  setTxt('navEarBtn', '👂 Kształcenie słuchu', '👂 Ear Training');
  setTxt('navChangesBtn', '🔄 Zmiany', '🔄 Changes');
  setTxt('navWorkshopBtn', '🛠️ Warsztat', '🛠️ Workshop');
  setTxt('navTunerBtn', '🎛️ Stroik (Tuner)', '🎛️ Tuner');
  setTxt('navSettingsBtn', '⚙️ Ustawienia', '⚙️ Settings');
  setTxt('micTitleLabel', 'Mikrofon', 'Microphone');

  const subtabMap = {
    scale:     ['Skala', 'Scale'],
    chord:     ['Akord · Odkrywanie', 'Chords · Explore'],
    harmonics: ['Flażolety', 'Harmonics'],
    search:    ['Szukaj', 'Search'],
    arpeggio:  ['Arpeggio', 'Arpeggio'],
    sweep:     ['⚡ Sweep Picking', '⚡ Sweep Picking'],
    exercise:  ['✏️ Ćwiczenie (TAB)', '✏️ Practice (TAB)']
  };
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(btn => {
    const key = btn.getAttribute('data-subtab');
    if (subtabMap[key]) btn.textContent = en ? subtabMap[key][1] : subtabMap[key][0];
  });
  syncMicButtonsUi(!!micStream);
}

function setUiLanguage(lang) {
  state.uiLang = (lang === 'en') ? 'en' : 'pl';
  applyUiLanguage();
  saveSettings();
  renderAll();
}

function resetAppSettings() {
  state.themeMode = 'dark';
  state.uiStyle = 'classic';
  state.uiLang = 'pl';
  state.fretMarkersMode = 'numbers';
  state.numberAllFrets = false;
  state.fretboardMargin = true;
  state.synthVolume = 0.85;
  state.metronomeVolume = 0.75;
  state.skillLevel = 'intermediate';
  state.uiScaleMode = 'auto';
  state.maxDisplayedFret = 24;
  state.tabCreationEnabled = true;
  state.exerciseClef = 'treble';
  state.a4ReferenceHz = 440.0;
  state.accMode = 2;
  applyTheme();
  applyUiLanguage();
  applyUiScale(true);
  saveSettings();
  renderAll();
}

function getViewDisplayName(mainView, subtab) {
  if (mainView === 'practice') {
    const map = {
      scale: t('Skala', 'Scale'),
      chord: t('Akord · Odkrywanie', 'Chords'),
      harmonics: t('Flażolety', 'Harmonics'),
      search: t('Szukaj', 'Search'),
      arpeggio: 'Arpeggio',
      sweep: 'Sweep Picking',
      exercise: t('Ćwiczenie (TAB)', 'Exercise (TAB)')
    };
    return map[subtab] || t('Praktyka', 'Practice');
  }
  const mainMap = {
    ear: t('Kształcenie słuchu', 'Ear Training'),
    changes: t('Zmiany', 'Changes'),
    workshop: t('Warsztat', 'Workshop'),
    tuner: t('Stroik (Tuner)', 'Tuner')
  };
  return mainMap[mainView] || t('Poprzedni widok', 'Previous view');
}

function goToAudioSettings() {
  if (state.mainView !== 'settings') {
    state.settingsReturnView = { mainView: state.mainView, subtab: state.subtab };
  }
  const btn = document.getElementById('navSettingsBtn');
  switchMainView('settings', btn);
}

function returnFromSettings() {
  const ret = state.settingsReturnView || { mainView: 'practice', subtab: state.subtab || 'scale' };
  state.settingsReturnView = null;
  const viewKey = ret.mainView || 'practice';
  const navBtn = Array.from(document.querySelectorAll('.nav-btn')).find(b => {
    const oc = b.getAttribute('onclick') || '';
    return oc.includes(`'${viewKey}'`);
  });
  if (viewKey === 'practice' && ret.subtab) {
    const resolvedSub = (ret.subtab === 'scale_practice') ? 'exercise' : ret.subtab;
    if (ret.subtab === 'scale_practice') state.exerciseSubView = 'scale_practice';
    state.subtab = resolvedSub;
    document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
      b.classList.toggle('active', b.getAttribute('data-subtab') === resolvedSub);
    });
  }
  switchMainView(viewKey, navBtn);
}

function syncMicButtonsUi(isOn) {
  const dot = document.getElementById('micDot');
  const txt = document.getElementById('micStatusText');
  const sideBtn = document.getElementById('toggleMicBtn');
  const setBtn = document.getElementById('settingsToggleMicBtn');
  if (dot) dot.classList.toggle('on', !!isOn);
  if (txt) txt.textContent = isOn ? t('Nasłuch aktywny', 'Listening') : t('Wejście wył.', 'Input off');
  if (sideBtn) {
    sideBtn.textContent = isOn ? t('🔇 Wyłącz', '🔇 Turn off') : t('🎤 Włącz', '🎤 Turn on');
    sideBtn.style.background = isOn ? 'var(--red)' : 'var(--accent)';
    sideBtn.style.color = isOn ? '#ffffff' : '#120e0a';
  }
  if (setBtn) {
    setBtn.textContent = isOn ? '🔇 Wyłącz mikrofon' : '🎤 Włącz mikrofon';
    setBtn.style.background = isOn ? 'var(--red)' : 'var(--accent)';
    setBtn.style.color = isOn ? '#ffffff' : '#120e0a';
  }
}

async function setDspModel(modelId) {
  const valid = ['piano', 'fast', 'voice'];
  const next = valid.includes(modelId) ? modelId : 'fast';
  state.dspModel = next;
  state.dspDefaultsV2 = true;
  if (!state.dspCalibrated && window.PitchPleaseBridge && typeof window.PitchPleaseBridge.defaultThresholdForModel === 'function') {
    const defT = window.PitchPleaseBridge.defaultThresholdForModel(next);
    state.dspThresholdOn = defT.on;
    state.dspHighThresholdOn = defT.highOn;
  }
  saveSettings();
  if (livePitchPleaseDetector && livePitchPleaseDetector.running) {
    try {
      await livePitchPleaseDetector.setModel(next);
      livePitchPleaseDetector.updateConfig({
        thresholdOn: state.dspThresholdOn,
        highThresholdOn: state.dspHighThresholdOn
      });
      const status = document.getElementById('micStatusText');
      if (status) status.textContent = `Aktywne · WASM (${next})`;
    } catch (e) {
      console.warn('Nie udało się przełączyć modelu PitchPlease:', e);
    }
  }
  if (state.mainView === 'settings') renderStageAndInspector();
}

function setDspThresholdOn(val) {
  const clamped = Math.max(0.38, Math.min(0.84, Number(val) || 0.58));
  state.dspThresholdOn = Math.round(clamped * 100) / 100;
  if (state.dspHighThresholdOn > state.dspThresholdOn) {
    state.dspHighThresholdOn = Math.max(0.32, Math.round((state.dspThresholdOn - 0.10) * 100) / 100);
  }
  saveSettings();
  if (livePitchPleaseDetector) {
    livePitchPleaseDetector.updateConfig({
      thresholdOn: state.dspThresholdOn,
      highThresholdOn: state.dspHighThresholdOn
    });
  }
  if (state.mainView === 'settings') renderStageAndInspector();
}

function setDspHighThresholdOn(val) {
  const clamped = Math.max(0.30, Math.min(0.78, Number(val) || 0.42));
  state.dspHighThresholdOn = Math.round(clamped * 100) / 100;
  saveSettings();
  if (livePitchPleaseDetector) {
    livePitchPleaseDetector.updateConfig({ highThresholdOn: state.dspHighThresholdOn });
  }
  if (state.mainView === 'settings') renderStageAndInspector();
}

function toggleDspAmpPreEq() {
  state.dspAmpPreEq = !(state.dspAmpPreEq !== false);
  saveSettings();
  if (livePitchPleaseDetector && typeof livePitchPleaseDetector.setAmpPreEq === 'function') {
    livePitchPleaseDetector.setAmpPreEq(state.dspAmpPreEq);
  }
  if (state.mainView === 'settings') renderStageAndInspector();
}

async function startGuitarMicCalibration() {
  if (state.dspCalibrating) return;
  if (!micStream) {
    await toggleLiveAudio();
  }
  if (!micStream || !livePitchPleaseDetector || !window.PitchPleaseBridge) {
    state.dspCalibStatus = t('Włącz mikrofon, aby skalibrować gitarę.', 'Enable the microphone to calibrate your guitar.');
    if (state.mainView === 'settings') renderStageAndInspector();
    return;
  }
  state.dspCalibrating = true;
  state.dspCalibStatus = t('1/3 Cisza (1.2 s) — wycisz struny...', '1/3 Silence (1.2 s) — mute strings...');
  if (state.mainView === 'settings') renderStageAndInspector();

  const updateStatusText = (txt) => {
    state.dspCalibStatus = txt;
    const el = document.getElementById('dspCalibStatusLabel');
    if (el) el.textContent = txt;
  };

  try {
    const res = await window.PitchPleaseBridge.calibrateGuitarFromLiveInput({
      getRms: () => livePitchPleaseDetector ? livePitchPleaseDetector.getCurrentRms() : 0,
      getRegisterProbs: () => livePitchPleaseDetector ? livePitchPleaseDetector.getLastRegisterProbs() : { low: 0, high: 0, all: 0 },
      onProgress: (p) => {
        const pct = Math.round((p.progress || 0) * 100);
        if (p.stage === 'silence') {
          updateStatusText(t(`1/3 Cisza (szum wzmacniacza/tła)... ${pct}%`, `1/3 Silence (amp/room noise)... ${pct}%`));
        } else if (p.stage === 'low') {
          updateStatusText(t(`2/3 Zagraj GRUBE struny (E6–D4)... ${pct}%`, `2/3 Play THICK strings (E6–D4)... ${pct}%`));
        } else {
          updateStatusText(t(`3/3 Zagraj CIENKIE struny (dwudźwięk G–B–E)... ${pct}%`, `3/3 Play THIN strings (G–B–E dyad)... ${pct}%`));
        }
      }
    });
    state.dspNoiseGateRms = res.noiseGateRms;
    if (res.ok) {
      state.dspThresholdOn = res.thresholdOn;
      state.dspHighThresholdOn = res.highThresholdOn;
      state.dspCalibrated = true;
      state.dspDefaultsV2 = true;
      const gateDb = Math.round(20 * Math.log10(Math.max(1e-5, res.noiseGateRms)));
      state.dspCalibStatus = t(
        `✓ Skalibrowano! Grube ${Math.round(res.thresholdOn * 100)}% · Cienkie ${Math.round(res.highThresholdOn * 100)}% · Bramka ${gateDb} dB`,
        `✓ Calibrated! Thick ${Math.round(res.thresholdOn * 100)}% · Thin ${Math.round(res.highThresholdOn * 100)}% · Gate ${gateDb} dB`
      );
    } else {
      state.dspCalibStatus = t(
        'Za cichy sygnał gitary — zapisano sam próg ciszy. Spróbuj zagrać głośniej.',
        'Guitar signal too quiet — saved noise gate only. Try playing louder.'
      );
    }
    if (livePitchPleaseDetector) {
      livePitchPleaseDetector.updateConfig({
        thresholdOn: state.dspThresholdOn,
        highThresholdOn: state.dspHighThresholdOn,
        noiseGateRms: state.dspNoiseGateRms
      });
    }
    saveSettings();
  } catch (e) {
    state.dspCalibStatus = t('Błąd kalibracji: ', 'Calibration error: ') + (e && e.message ? e.message : e);
  } finally {
    state.dspCalibrating = false;
    if (state.mainView === 'settings') renderStageAndInspector();
  }
}

async function toggleLiveAudio() {
  if (micStream) {
    if (micPollTimer) { clearInterval(micPollTimer); micPollTimer = null; }
    if (audioLoopId) { cancelAnimationFrame(audioLoopId); audioLoopId = null; }
    try {
      if (livePitchPleaseDetector) {
        livePitchPleaseDetector.stop();
      }
    } catch (e) {}
    livePitchPleaseDetector = null;
    try {
      if (micCaptureNode) {
        if (micCaptureNode.port) micCaptureNode.port.onmessage = null;
        micCaptureNode.onaudioprocess = null;
        micCaptureNode.disconnect();
      }
    } catch (e) {}
    try { if (micSourceNode) micSourceNode.disconnect(); } catch (e) {}
    try { if (micMuteSinkNode) micMuteSinkNode.disconnect(); } catch (e) {}
    micCaptureNode = null;
    micSourceNode = null;
    micMuteSinkNode = null;
    liveGuitarDspEngine = null;
    micStream.getTracks().forEach(t => t.stop());
    micStream = null;
    syncMicButtonsUi(false);
    state.detectedMidi = null;
    state.detectedMidis = [];
    document.getElementById('liveNoteBadge').textContent = '—';
    document.getElementById('liveCentsBadge').textContent = '';
    drawFretboard();
    updateGuitarToKeyboardSync();
    if (state.mainView === 'tuner') renderTunerCanvas();
    return;
  }
  try {
    const ctx = ensureAudioCtx();
    if (!window.PitchPleaseBridge) {
      try {
        window.PitchPleaseBridge = await import('/web/js/dsp/pitchPleaseBridge.js');
      } catch (ppModErr) {
        console.warn('Moduł /web/js/dsp/pitchPleaseBridge.js niedostępny, przełączam na GuitarDsp:', ppModErr);
      }
    }

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

    // GŁÓWNA ŚCIEŻKA (V3): Streamingowy silnik polifoniczny PitchPlease (CNN w AudioWorklet + WASM SIMD, ~33 ms)
    // + precyzyjny monofoniczny MPM (±0.2 ct) wyłącznie dla widoku Stroika (Tuner).
    if (window.PitchPleaseBridge && typeof window.PitchPleaseBridge.createGuitarPitchDetector === 'function') {
      const statusEl = document.getElementById('micStatusText');
      if (statusEl) statusEl.textContent = 'Ładowanie WASM...';

      const recordedTabIdxByMidi = new Map();
      let fretboardDirty = false;
      let prevActiveKey = '';
      let lastFineMpmMs = 0;
      let lastFineMidiNote = null;
      let lastFineExactMidi = null;
      let lastAppliedLowest = Math.min(...state.customStringMidis);
      let lastAppliedHighest = Math.min(88, Math.max(...state.customStringMidis) + 24);
      let lastAppliedA4 = state.a4ReferenceHz || 440;
      let tunerQuietFrames = 0;
      const chosenModel = ['piano', 'fast', 'voice'].includes(state.dspModel) ? state.dspModel : 'fast';

      const ppDetector = window.PitchPleaseBridge.createGuitarPitchDetector({
        model: chosenModel,
        minMidi: Math.max(28, lastAppliedLowest - 1),
        maxMidi: lastAppliedHighest,
        a4Hz: lastAppliedA4,
        thresholdOn: state.dspThresholdOn || 0.58,
        highThresholdOn: state.dspHighThresholdOn || 0.42,
        ampPreEq: state.dspAmpPreEq !== false,
        noiseGateRms: state.dspNoiseGateRms ?? 0.0020,
        onError: (err) => {
          console.warn('[PitchPlease]', err);
        },
        onFrame: ({ events, activeNotes, chord }) => {
          if (!micStream || livePitchPleaseDetector !== ppDetector) return;
          if (state.mainView === 'tuner') return; // Stroik ma własną, sub-centową pętlę MPM

          const noteNames = getRootLabels();
          const a4 = state.a4ReferenceHz || 440;
          let anyEvent = false;

          for (let eIdx = 0; eIdx < events.length; eIdx++) {
            const ev = events[eIdx];
            anyEvent = true;
            if (ev.type === 'noteOn') {
              const playedMidi = ev.midiNote;
              const evLabel = noteNames[((playedMidi % 12) + 12) % 12] + ' ' + (Math.floor(playedMidi / 12) - 1);
              const heardBadge = document.getElementById('exerciseHeardBadge');
              if (heardBadge) heardBadge.textContent = `(słyszę: ${evLabel})`;
              const earBox = document.getElementById('earPlayDetectedBox');
              if (earBox && !earBox.dataset.answered) {
                earBox.textContent = evLabel;
              }
              if (state.mainView === 'practice' && state.subtab === 'exercise') {
                if (state.exercisePlayerActive && state.exercisePlayerMode === 'play') {
                  checkExercisePlayerPlayedMidi(playedMidi);
                } else if (!state.exercisePlayerActive && state.tabCreationEnabled) {
                  recordMidiNoteToTab(playedMidi);
                  if (state.recordedEvents.length > 0) {
                    recordedTabIdxByMidi.set(playedMidi, state.recordedEvents.length - 1);
                  }
                }
              }
              if (state.mainView === 'ear' && state.earScreen === 'active' && state.earMode === 'play') {
                checkEarPlayGuitarMidi(playedMidi);
              }
              if (state.mainView === 'changes' && state.changesScreen === 'practice_active') {
                checkChangesPracticePlayedMidi(playedMidi);
              }
            } else if (ev.type === 'noteOff') {
              const recIdx = recordedTabIdxByMidi.get(ev.midiNote);
              if (recIdx !== undefined && state.recordedEvents[recIdx] && state.recordedEvents[recIdx].midi === ev.midiNote) {
                state.recordedEvents[recIdx].heldMs = Math.round(ev.durationMs || 0);
                recordedTabIdxByMidi.delete(ev.midiNote);
              }
            }
          }

          const activeMidis = activeNotes.map(n => n.midiNote);
          const activeKey = activeMidis.join(',');
          const setChanged = anyEvent || (activeKey !== prevActiveKey);
          prevActiveKey = activeKey;

          if (activeNotes.length > 0) {
            const isPoly = activeNotes.length >= 2;
            let primary = activeNotes[0];
            if (isPoly) {
              for (let i = 1; i < activeNotes.length; i++) {
                if (activeNotes[i].lastTriggerTimeMs >= primary.lastTriggerTimeMs) primary = activeNotes[i];
              }
            }

            let exactMidi = (!isPoly && lastFineMidiNote === primary.midiNote && lastFineExactMidi !== null)
              ? lastFineExactMidi
              : primary.midiNote;
            const nowMs = performance.now();
            if (!isPoly && (lastFineMidiNote !== primary.midiNote || (nowMs - lastFineMpmMs) >= 42)) {
              lastFineMpmMs = nowMs;
              lastFineMidiNote = primary.midiNote;
              lastFineExactMidi = primary.midiNote;
              const pcm = ppDetector.getLatestPcm();
              if (pcm) {
                const sr = ppDetector.getAnalyserSampleRate();
                const baseHz = a4 * Math.pow(2, (primary.midiNote - 69) / 12);
                const mpmFine = runMpmWindow(pcm, pcm.length, sr, Math.max(40, baseHz * 0.88), Math.min(2200, baseHz * 1.14));
                if (mpmFine && mpmFine.hz > 0 && mpmFine.clarity >= 0.72) {
                  const mpmMidi = 69 + 12 * Math.log2(mpmFine.hz / a4);
                  if (Math.abs(mpmMidi - primary.midiNote) <= 0.65) {
                    exactMidi = mpmMidi;
                    lastFineExactMidi = mpmMidi;
                  }
                }
              }
            }

            const cents = isPoly ? 0 : Math.max(-50, Math.min(50, (exactMidi - primary.midiNote) * 100));
            const hz = a4 * Math.pow(2, (exactMidi - 69) / 12);
            state.detectedMidi = primary.midiNote;
            state.detectedMidis = isPoly ? activeMidis : [primary.midiNote];
            state.detectedCents = cents;
            state.detectedHz = hz;

            const noteLabel = isPoly
              ? (resolveChordLabel(activeMidis) || (chord && chord.full) || activeMidis.map(m => noteNames[((m % 12) + 12) % 12]).join(' '))
              : noteNames[((primary.midiNote % 12) + 12) % 12];
            document.getElementById('liveNoteBadge').textContent = noteLabel;
            document.getElementById('liveCentsBadge').textContent = '';

            if (setChanged) fretboardDirty = true;
          } else if (setChanged) {
            state.detectedMidi = null;
            state.detectedMidis = [];
            document.getElementById('liveNoteBadge').textContent = '—';
            document.getElementById('liveCentsBadge').textContent = '';
            const heardBadge = document.getElementById('exerciseHeardBadge');
            if (heardBadge) heardBadge.textContent = '';
            const earBox = document.getElementById('earPlayDetectedBox');
            if (earBox && !earBox.dataset.answered) earBox.textContent = '—';
            fretboardDirty = true;
          }
        }
      });

      const started = await ppDetector.start(micStream, ctx);
      if (started && micStream) {
        livePitchPleaseDetector = ppDetector;
        syncMicButtonsUi(true);
        const stEl = document.getElementById('micStatusText');
        if (stEl) stEl.textContent = `Aktywne · WASM (${chosenModel})`;

        const renderLoop = () => {
          if (!micStream || livePitchPleaseDetector !== ppDetector) return;

          const lowest = Math.min(...state.customStringMidis);
          const highest = Math.min(88, Math.max(...state.customStringMidis) + 24);
          const a4 = state.a4ReferenceHz || 440;
          if (lowest !== lastAppliedLowest || highest !== lastAppliedHighest || Math.abs(a4 - lastAppliedA4) > 0.05) {
            lastAppliedLowest = lowest;
            lastAppliedHighest = highest;
            lastAppliedA4 = a4;
            ppDetector.updateConfig({ minMidi: Math.max(28, lowest - 1), maxMidi: highest, a4Hz: a4 });
          }

          if (state.mainView === 'tuner') {
            // Dedykowany, ultra-precyzyjny monofoniczny MPM (±0.2 ct) dla Stroika z obsługą A4
            const pcm = ppDetector.getLatestPcm();
            const rms = ppDetector.getCurrentRms();
            const gate = Math.max(0.0018, (state.dspNoiseGateRms ?? 0.0020) * 0.85);
            if (pcm && rms >= gate) {
              const sr = ppDetector.getAnalyserSampleRate();
              let minHz = Math.max(45, a4 * Math.pow(2, (lowest - 2 - 69) / 12));
              let maxHz = Math.min(1800, a4 * Math.pow(2, (highest - 69) / 12));
              if (state.tunerLockedString >= 0 && state.customStringMidis[state.tunerLockedString] !== undefined) {
                const lockHz = a4 * Math.pow(2, (state.customStringMidis[state.tunerLockedString] - 69) / 12);
                minHz = Math.max(40, lockHz * 0.72);
                maxHz = Math.min(2000, lockHz * 1.38);
              }
              const mpm = runMpmWindow(pcm, pcm.length, sr, minHz, maxHz);
              if (mpm && mpm.hz > 0 && mpm.clarity >= 0.74) {
                tunerQuietFrames = 0;
                const exactMidi = 69 + 12 * Math.log2(mpm.hz / a4);
                let targetMidi = Math.round(exactMidi);
                if (state.tunerLockedString >= 0 && state.customStringMidis[state.tunerLockedString] !== undefined) {
                  targetMidi = state.customStringMidis[state.tunerLockedString];
                } else if (state.tunerMode === 'guitar' && state.customStringMidis.length === 6) {
                  targetMidi = state.customStringMidis.reduce((best, m) =>
                    Math.abs(m - exactMidi) < Math.abs(best - exactMidi) ? m : best,
                    state.customStringMidis[0]
                  );
                }
                const cents = Math.max(-50, Math.min(50, (exactMidi - targetMidi) * 100));
                state.detectedMidi = targetMidi;
                state.detectedMidis = [targetMidi];
                state.detectedCents = cents;
                state.detectedHz = mpm.hz;
                const noteNames = getRootLabels();
                document.getElementById('liveNoteBadge').textContent =
                  noteNames[((targetMidi % 12) + 12) % 12];
                document.getElementById('liveCentsBadge').textContent = '';
              } else {
                tunerQuietFrames++;
              }
            } else {
              tunerQuietFrames++;
            }
            if (tunerQuietFrames >= 10 && state.detectedMidi !== null) {
              state.detectedMidi = null;
              state.detectedMidis = [];
              document.getElementById('liveNoteBadge').textContent = '—';
              document.getElementById('liveCentsBadge').textContent = '';
            }
            renderTunerCanvas();
          } else if (fretboardDirty) {
            fretboardDirty = false;
            drawFretboard();
            updateGuitarToKeyboardSync();
          }
          audioLoopId = requestAnimationFrame(renderLoop);
        };
        renderLoop();
        return;
        const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
      return;
      } else {
        ppDetector.stop();
        console.warn('PitchPlease nie uruchomił się, przełączam na fallback GuitarDsp.');
      }
    }

    if (!window.GuitarDsp) {
      try {
        window.GuitarDsp = await import('/web/js/dsp/guitarDspEngine.js');
      } catch (modErr) {
        console.warn('Moduł /web/js/dsp/guitarDspEngine.js niedostępny, przełączam na fallback MPM:', modErr);
      }
    }

    if (!window.DechorderDsp) {
      try {
        window.DechorderDsp = await import('/web/js/dsp/dechorder/index.js');
      } catch (modErr) {
        console.warn('Moduł /web/js/dsp/dechorder/index.js niedostępny:', modErr);
      }
    }

    const src = ctx.createMediaStreamSource(micStream);
    micSourceNode = src;

    const decimate2 = ctx.sampleRate > 54000;
    const effSampleRate = decimate2 ? (ctx.sampleRate * 0.5) : ctx.sampleRate;

    // ŚCIEŻKA ZAPASOWA (V2): Modularny silnik DSP w JS (gdyby AudioWorklet WASM był niedostępny)
    if (window.GuitarDsp && typeof window.GuitarDsp.GuitarNoteEngine === 'function') {
      const buildDspOverrides = (lowestMidi, highestMidi, a4Hz) => {
        const lowestFreq = a4Hz * Math.pow(2, (lowestMidi - 69) / 12);
        return {
          sampleRate: effSampleRate,
          a4ReferenceHz: a4Hz,
          minMidiNote: Math.max(28, Math.min(36, lowestMidi - 2)),
          maxMidiNote: Math.min(114, Math.max(108, highestMidi + 24)),
          bandpassLowHz: Math.max(42, Math.min(62, lowestFreq * 0.85)),
          fundamentalMinHz: Math.max(45, Math.min(66, lowestFreq * 0.90))
        };
      };

      let lastAppliedLowest = Math.min(...state.customStringMidis);
      let lastAppliedHighest = Math.max(...state.customStringMidis) + 24;
      let lastAppliedA4 = state.a4ReferenceHz || 440;
      liveGuitarDspEngine = new window.GuitarDsp.GuitarNoteEngine(
        buildDspOverrides(lastAppliedLowest, lastAppliedHighest, lastAppliedA4)
      );

      let decimatedBlock = null;
      let lastUiRefreshMs = 0;
      const recordedTabIdxByMidi = new Map();

      const processStreamingPcm = (inputPcm) => {
        if (!micStream || !liveGuitarDspEngine || !inputPcm || inputPcm.length === 0) return;

        const lowest = Math.min(...state.customStringMidis);
        const highest = Math.max(...state.customStringMidis) + 24;
        const a4 = state.a4ReferenceHz || 440;
        if (lowest !== lastAppliedLowest || highest !== lastAppliedHighest || Math.abs(a4 - lastAppliedA4) > 0.05) {
          lastAppliedLowest = lowest;
          lastAppliedHighest = highest;
          lastAppliedA4 = a4;
          liveGuitarDspEngine.prepare(buildDspOverrides(lowest, highest, a4));
        }

        let samples = inputPcm;
        if (decimate2) {
          const halfLen = inputPcm.length >> 1;
          if (!decimatedBlock || decimatedBlock.length !== halfLen) {
            decimatedBlock = new Float32Array(halfLen);
          }
          for (let i = 0; i < halfLen; i++) {
            decimatedBlock[i] = 0.5 * (inputPcm[i * 2] + inputPcm[i * 2 + 1]);
          }
          samples = decimatedBlock;
        }

        const events = liveGuitarDspEngine.pushSamples(samples);
        const activeNotes = liveGuitarDspEngine.activeNotes;
        const noteNames = getRootLabels();
        const nowMs = performance.now();
        let anyVisualChange = false;

        for (let eIdx = 0; eIdx < events.length; eIdx++) {
          const ev = events[eIdx];
          anyVisualChange = true;
          if (ev.type === 'noteOn') {
            const playedMidi = ev.midiNote;
            const evLabel = noteNames[((playedMidi % 12) + 12) % 12] + ' ' + (Math.floor(playedMidi / 12) - 1);
            const heardBadge = document.getElementById('exerciseHeardBadge');
            if (heardBadge) heardBadge.textContent = `(słyszę: ${evLabel})`;
            const earBox = document.getElementById('earPlayDetectedBox');
            if (earBox && !earBox.dataset.answered) {
              earBox.textContent = noteNames[((playedMidi % 12) + 12) % 12] + (Math.floor(playedMidi / 12) - 1);
            }
            if (state.mainView === 'practice' && state.subtab === 'exercise') {
              if (state.exercisePlayerActive && state.exercisePlayerMode === 'play') {
                checkExercisePlayerPlayedMidi(playedMidi);
              } else if (!state.exercisePlayerActive && state.tabCreationEnabled) {
                recordMidiNoteToTab(playedMidi);
                if (state.recordedEvents.length > 0) {
                  recordedTabIdxByMidi.set(playedMidi, state.recordedEvents.length - 1);
                }
              }
            }
            if (state.mainView === 'ear' && state.earScreen === 'active' && state.earMode === 'play') {
              checkEarPlayGuitarMidi(playedMidi);
            }
            if (state.mainView === 'changes' && state.changesScreen === 'practice_active') {
              checkChangesPracticePlayedMidi(playedMidi);
            }
          } else if (ev.type === 'noteOff') {
            const recIdx = recordedTabIdxByMidi.get(ev.midiNote);
            if (recIdx !== undefined && state.recordedEvents[recIdx] && state.recordedEvents[recIdx].midi === ev.midiNote) {
              state.recordedEvents[recIdx].heldMs = Math.round(ev.durationMs || 0);
              recordedTabIdxByMidi.delete(ev.midiNote);
            }
          }
        }

        if (!anyVisualChange && (nowMs - lastUiRefreshMs) < 34) {
          return;
          const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
      return;
        }
        lastUiRefreshMs = nowMs;

        if (activeNotes.length > 0) {
          const activeMidis = activeNotes.map(n => n.midiNote);
          const isPoly = activeNotes.length >= 2;
          let primary = activeNotes[0];
          if (isPoly) {
            for (let i = 1; i < activeNotes.length; i++) {
              if (activeNotes[i].lastTriggerTimeMs >= primary.lastTriggerTimeMs) primary = activeNotes[i];
            }
          }

          let exactMidi = primary.exactMidi;
          if (!isPoly) {
            // Dla pojedynczej struny (szczególnie w Stroiku) dociążamy dokładność centową (±0.2 ct) przez MPM w wąskim oknie wokół wykrytego f0
            const fineLow = Math.max(40, primary.frequencyHz * 0.88);
            const fineHigh = Math.min(2200, primary.frequencyHz * 1.14);
            const mpmFine = runMpmWindow(liveGuitarDspEngine.latestWindow, 2048, effSampleRate, fineLow, fineHigh);
            if (mpmFine && mpmFine.hz > 0 && mpmFine.clarity >= 0.70) {
              const mpmMidi = 69 + 12 * Math.log2(mpmFine.hz / a4);
              if (Math.abs(mpmMidi - primary.midiNote) <= 0.65) {
                exactMidi = mpmMidi;
              }
            }
          }

          const playedMidi = primary.midiNote;
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
          state.detectedMidis = isPoly ? activeMidis : [targetMidi];
          state.detectedCents = cents;
          state.detectedHz = hz;

          const noteLabel = isPoly
            ? (resolveChordLabel(activeMidis) || activeMidis.map(m => noteNames[((m % 12) + 12) % 12]).join(' '))
            : noteNames[((playedMidi % 12) + 12) % 12];
          document.getElementById('liveNoteBadge').textContent = noteLabel;
          document.getElementById('liveCentsBadge').textContent = '';

          if (anyVisualChange) {
            drawFretboard();
            updateGuitarToKeyboardSync();
          }
        } else if (anyVisualChange) {
          state.detectedMidi = null;
          state.detectedMidis = [];
          document.getElementById('liveNoteBadge').textContent = '—';
          document.getElementById('liveCentsBadge').textContent = '';
          const heardBadge = document.getElementById('exerciseHeardBadge');
          if (heardBadge) heardBadge.textContent = '';
          const earBox = document.getElementById('earPlayDetectedBox');
          if (earBox && !earBox.dataset.answered) earBox.textContent = '—';
          drawFretboard();
          updateGuitarToKeyboardSync();
        }
      };

      // Cichy sink (gain = 0), żeby graf WebAudio pobierał ramki ciągle bez sprzężenia zwrotnego mikrofonu
      const muteSink = ctx.createGain();
      muteSink.gain.value = 0;
      muteSink.connect(ctx.destination);
      micMuteSinkNode = muteSink;

      let workletAttached = false;
      if (ctx.audioWorklet && typeof AudioWorkletNode !== 'undefined') {
        try {
          if (!ctx._gitaraPcmWorkletLoaded) {
            const workletSrc = [
              'class GitaraPcmForwarder extends AudioWorkletProcessor {',
              '  process(inputs) {',
              '    const ch = inputs && inputs[0] && inputs[0][0];',
              '    if (ch && ch.length > 0) {',
              '      const copy = new Float32Array(ch.length);',
              '      copy.set(ch);',
              '      this.port.postMessage(copy, [copy.buffer]);',
              '    }',
              '    return true;',
              '  }',
              '}',
              'registerProcessor("gitara-pcm-forwarder", GitaraPcmForwarder);'
            ].join('\n');
            const blobUrl = URL.createObjectURL(new Blob([workletSrc], { type: 'application/javascript' }));
            try {
              await ctx.audioWorklet.addModule(blobUrl);
              ctx._gitaraPcmWorkletLoaded = true;
            } finally {
              URL.revokeObjectURL(blobUrl);
            }
          }
          const workletNode = new AudioWorkletNode(ctx, 'gitara-pcm-forwarder', {
            numberOfInputs: 1,
            numberOfOutputs: 1,
            channelCount: 1
          });
          workletNode.port.onmessage = (msg) => {
            if (msg.data instanceof Float32Array) {
              processStreamingPcm(msg.data);
            }
          };
          src.connect(workletNode);
          workletNode.connect(muteSink);
          micCaptureNode = workletNode;
          workletAttached = true;
        } catch (workletErr) {
          console.warn('AudioWorklet niedostępny, przełączam na ScriptProcessorNode(256):', workletErr);
        }
      }

      if (!workletAttached) {
        const spNode = ctx.createScriptProcessor(256, 1, 1);
        spNode.onaudioprocess = (ev) => {
          const ch = ev.inputBuffer.getChannelData(0);
          processStreamingPcm(ch);
        };
        src.connect(spNode);
        spNode.connect(muteSink);
        micCaptureNode = spNode;
      }

      syncMicButtonsUi(true);
      const renderLoop = () => {
        if (!micStream) return;
        if (state.mainView === 'tuner') renderTunerCanvas();
        audioLoopId = requestAnimationFrame(renderLoop);
      };
      renderLoop();
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
      return;
    }

    // FALLBACK (tylko gdyby nie załadowano modułu ES6 /web/js/dsp/guitarDspEngine.js)
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
    analyserNode = ctx.createAnalyser();
    analyserNode.fftSize = decimate2 ? 8192 : 4096;
    analyserNode.smoothingTimeConstant = 0;
    src.connect(micHp1);
    micHp1.connect(micHp2);
    micHp2.connect(micLowpass);
    micLowpass.connect(analyserNode);
    syncMicButtonsUi(true);

    const rawBuf = new Float32Array(analyserNode.fftSize);
    const analysisBuf = decimate2 ? new Float32Array(4096) : rawBuf;
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
            ? (resolveChordLabel(activePoly) || activePoly.map(m => noteNames[((m % 12) + 12) % 12]).join(' '))
            : noteNames[((playedMidi % 12) + 12) % 12];
          document.getElementById('liveNoteBadge').textContent = noteLabel;
          document.getElementById('liveCentsBadge').textContent = '';

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
          document.getElementById('liveCentsBadge').textContent = '';
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

async function fetchJsonPost(url, dataObj) {
  const res = await fetch(url, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(dataObj)
  });
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

let uiScaleResizeTimer = null;

function computeUiScaleProfile() {
  const vw = window.innerWidth || 1920;
  const vh = window.innerHeight || 1080;
  const mode = (state.uiScaleMode === 'fhd' || state.uiScaleMode === '2k') ? state.uiScaleMode : 'auto';
  const isAuto2k = (vw >= 2100 && vh >= 1120);
  const profile = (mode === 'auto') ? (isAuto2k ? '2k' : 'fhd') : mode;

  let zoom = 1.0;
  if (profile === 'fhd') {
    if (vw >= 1750 && vh >= 940) {
      zoom = 0.82; // Standardowy ekran Full HD (1920x1080 @ 100%) lub wymuszony tryb FHD na monitorze 2K
    } else if (vw >= 1400 && vh >= 760) {
      zoom = 0.75; // Laptop Full HD ze skalowaniem systemowym 125% (ok. 1536x864)
    } else {
      zoom = Math.max(0.66, Math.min(0.74, Math.min(vw / 1920, vh / 1080)));
    }
  }
  return { mode, profile, zoom: Math.round(zoom * 100) / 100, vw, vh };
}

function updateUiScaleControls() {
  const info = computeUiScaleProfile();
  const mode = info.mode;
  const pct = Math.round(info.zoom * 100);
  const profLbl = info.profile === '2k' ? '2K' : 'FHD';

  ['auto', 'fhd', '2k'].forEach(k => {
    const btn = document.getElementById(`uiScaleBtn_${k}`);
    if (btn) btn.classList.toggle('active', mode === k);
    const sBtn = document.getElementById(`settingsUiScaleBtn_${k}`);
    if (sBtn) sBtn.classList.toggle('active', mode === k);
  });

  const badge = document.getElementById('uiScaleBadge');
  if (badge) {
    badge.textContent = (mode === 'auto')
      ? `Auto→${profLbl} ${pct}%`
      : `${profLbl} · ${pct}%`;
  }

  const infoEl = document.getElementById('settingsUiScaleInfo');
  if (infoEl) {
    const dpr = Math.round((window.devicePixelRatio || 1) * 100);
    infoEl.textContent = `Aktywny profil: ${profLbl} (${pct}%) · Okno: ${info.vw}×${info.vh} px (DPI: ${dpr}%)`;
  }
}

function applyUiScale(redraw = true) {
  const info = computeUiScaleProfile();
  state.resolvedUiProfile = info.profile;
  state.appliedUiZoom = info.zoom;

  const body = document.body;
  const supportsZoom = (typeof CSS !== 'undefined' && CSS.supports) ? CSS.supports('zoom', '0.8') : true;

  body.classList.toggle('ui-mode-fhd', info.profile === 'fhd');
  body.classList.toggle('ui-mode-2k', info.profile === '2k');

  if (info.zoom < 0.999 && supportsZoom) {
    body.style.zoom = String(info.zoom);
    body.style.width = '100%';
    body.style.height = '100vh';
    // Samoweryfikacja rozmiaru viewportu dla różnic między standardowym CSS zoom (Edge/Chrome 128+, Firefox 126+) a starszym Blink
    const rect = body.getBoundingClientRect();
    if (Math.abs(rect.height - window.innerHeight) > 2) {
      body.style.height = `${Math.ceil(window.innerHeight / info.zoom)}px`;
    }
    if (Math.abs(rect.width - window.innerWidth) > 2) {
      body.style.width = `${Math.ceil(window.innerWidth / info.zoom)}px`;
    }
  } else {
    body.style.zoom = '';
    body.style.width = '';
    body.style.height = '';
  }

  updateUiScaleControls();

  if (redraw) {
    requestAnimationFrame(() => {
      drawFretboard();
      if (state.mainView === 'tuner') renderTunerCanvas();
      if (state.mainView === 'practice' && (state.subtab === 'arpeggio' || state.subtab === 'sweep' || state.subtab === 'exercise')) {
        renderStageAndInspector();
      }
    });
  }
}

function setUiScaleMode(mode) {
  if (mode !== 'auto' && mode !== 'fhd' && mode !== '2k') mode = 'auto';
  state.uiScaleMode = mode;
  saveSettings();
  applyUiScale(true);
}

function applyPanelCollapseStates() {
  const midiFooter = document.getElementById('midiFooter');
  const midiRevealBtn = document.getElementById('midiRevealBtn');
  const midiHidden = !!state.midiKeyboardHidden;
  if (midiFooter) midiFooter.classList.toggle('collapsed', midiHidden);
  if (midiRevealBtn) midiRevealBtn.style.display = midiHidden ? 'inline-flex' : 'none';

  const inspPanel = document.getElementById('rightInspector');
  const edgeBtn = document.getElementById('rightInspectorEdgeToggle');
  const edgeArrow = document.getElementById('rightEdgeArrow');
  const viewHidesInsp = (state.mainView === 'ear') ||
    (state.mainView === 'changes' && state.changesScreen !== 'editor') ||
    (state.mainView === 'practice' && state.subtab === 'scale_practice') ||
    (state.mainView === 'practice' && state.subtab === 'exercise' &&
      (state.exercisePlayerActive || state.exerciseSubView === 'scale_practice' || state.exerciseSubView === 'harmonics'));

  if (inspPanel) {
    inspPanel.style.display = viewHidesInsp ? 'none' : 'flex';
    inspPanel.classList.toggle('collapsed', !viewHidesInsp && !!state.rightInspectorHidden);
  }
  if (edgeBtn) {
    edgeBtn.style.display = viewHidesInsp ? 'none' : 'flex';
    edgeBtn.title = state.rightInspectorHidden
      ? 'Wysuń prawy panel parametrów'
      : 'Schowaj prawy panel parametrów';
  }
  if (edgeArrow) {
    edgeArrow.textContent = state.rightInspectorHidden ? '‹' : '›';
  }
}

function toggleMidiKeyboardPanel() {
  state.midiKeyboardHidden = !state.midiKeyboardHidden;
  saveSettings();
  applyPanelCollapseStates();
  requestAnimationFrame(() => {
    drawFretboard();
    if (state.mainView === 'tuner') renderTunerCanvas();
  });
}

function toggleRightInspectorPanel() {
  state.rightInspectorHidden = !state.rightInspectorHidden;
  saveSettings();
  applyPanelCollapseStates();
  requestAnimationFrame(() => {
    drawFretboard();
    if (state.mainView === 'tuner') renderTunerCanvas();
    if (state.mainView === 'practice' && (state.subtab === 'arpeggio' || state.subtab === 'sweep' || state.subtab === 'exercise')) {
      renderStageAndInspector();
    }
  });
}

function scrollExerciseTabTo(where) {
  const wrap = document.getElementById('exerciseComposerScoreWrap') || document.getElementById('exerciseScoreWrap');
  if (!wrap) return;
  const targetTop = (where === 'top') ? 0 : wrap.scrollHeight;
  wrap.scrollTo({ top: targetTop, behavior: 'smooth' });
}

async function initApp() {
  await loadSettings();
  applyTheme();
  applyUiLanguage();
  if (typeof window !== 'undefined' && window.matchMedia) {
    try {
      window.matchMedia('(prefers-color-scheme: light)').addEventListener('change', () => {
        if (state.themeMode === 'system') { applyTheme(); drawFretboard(); }
      });
    } catch (e) {}
  }
  applyUiScale(false);
  applyPanelCollapseStates();
  state.initData = await fetchJson('/api/init');
  if (state.initData && Array.isArray(state.initData.sessions) && Array.isArray(state.customSavedSessions)) {
    const existingIds = new Set(state.initData.sessions.map(s => s.id));
    state.customSavedSessions.forEach(cs => {
      if (cs && cs.id && !existingIds.has(cs.id)) {
        state.initData.sessions.push(cs);
      }
    });
  }
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
    clearTimeout(uiScaleResizeTimer);
    uiScaleResizeTimer = setTimeout(() => {
      applyUiScale(true);
    }, 60);
  });
  await renderAll();
  updateUiScaleControls();
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
      startX = e.clientX;
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
    const dx = (e.clientX - startX) / (state.appliedUiZoom || 1);
    wrap.scrollLeft = scrollLeftStart - dx;
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
    const capoFret = Math.max(0, Math.min(12, parseInt(state.chordCapoFret || 0, 10) || 0));
    bar.innerHTML = `
      <div style="display:flex;align-items:center;gap:10px;flex-wrap:wrap;">
        <strong style="color:var(--text);font-size:14px;">🎸 Wybrany chwyt na gryfie: <span style="color:var(--accent);">${vTitle}</span></strong>
        ${capoFret > 0 ? `
          <span style="display:inline-flex;align-items:center;gap:6px;background:rgba(243,146,55,0.16);border:1px solid var(--accent);color:var(--accent);border-radius:7px;padding:3px 9px;font-size:12px;font-weight:700;">
            🔒 Kapodaster: próg ${capoFret}
            <button type="button" style="background:none;border:none;color:var(--accent);cursor:pointer;font-weight:800;padding:0 2px;"
                    title="Zdejmij kapodaster" onclick="setChordCapoFret(0)">✕</button>
          </span>` : ''}
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
        <span><b style="color:#b85014">●</b> Pryma akordu</span>
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
  } else if (state.subtab === 'arpeggio' || state.subtab === 'sweep') {
    const isSweepTab = (state.subtab === 'sweep');
    const isSweepTrans = isSweepTab && (state.sweepViewMode === 'transition');
    const curSeg = (isSweepTrans && state.arpData && state.arpData.transition && state.arpData.transition.segments)
      ? state.arpData.transition.segments[state.arpActiveSegIdx]
      : null;
    const arpSym = (state.arpData && state.arpData.chordSymbol) ? state.arpData.chordSymbol : state.chordId;
    const patNames = [
      'Cały gryf',
      'CAGED',
      '2NPS',
      '3NPS',
      `Sweep (${state.arpSweepStrings} str.)`,
      'Inwersje',
      `Od prymy (${state.arpOctaves} okt.)`
    ];
    const arpTitle = isSweepTrans
      ? '⚡ Sweep Picking'
      : (isSweepTab ? `⚡ Sweep (${state.arpSweepStrings} str.): ${arpSym}` : `Arpeggio: ${arpSym}`);
    const subBadge = curSeg
      ? `<span style="background:rgba(243,146,55,0.14);border:1px solid rgba(243,146,55,0.42);color:var(--accent);border-radius:6px;padding:1px 7px;font-size:11.5px;font-weight:700;">${curSeg.symbol} (${curSeg.roman})</span>`
      : (isSweepTab ? '' : `<span style="color:var(--text-muted);font-size:12px;">${patNames[state.arpPattern] || 'Arpeggio'}</span>`);
    const arpScaleName = (state.arpData && state.arpData.scaleFullName)
      ? state.arpData.scaleFullName
      : ((state.fretboardData && state.fretboardData.scaleFullName) ? state.fretboardData.scaleFullName : 'Skala');
    bar.innerHTML = `
      <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
        <strong style="color:var(--text);font-size:14px;">${arpTitle}</strong>
        ${subBadge}
        <button class="chip-btn ${state.arpShowScaleOnFretboard?'active':''}" style="padding:4px 10px;font-size:11.5px;"
                onclick="state.arpShowScaleOnFretboard=!state.arpShowScaleOnFretboard;saveSettings();renderFretboardToolbarForActiveTab();drawFretboard()">
          🎵 Pokaż skalę w tle (${arpScaleName})
        </button>
      </div>
      <div style="display:flex;gap:8px;align-items:center;font-size:12px;flex-wrap:wrap;">
        ${zoomCtrl}
        <button class="chip-btn ${state.labelType==='notes'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('notes')">Nuty</button>
        <button class="chip-btn ${state.labelType==='intervals'?'active':''}" style="padding:3px 8px;font-size:11px;"
                onclick="setFretboardLabelType('intervals')">Interwały</button>
        <span><b style="color:#b85014">●</b> 1</span>
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
  if (state.subtab === 'arpeggio' || state.subtab === 'sweep') {
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
  const isSweepTab = (state.mainView === 'practice' && state.subtab === 'sweep');
  const isArpeggioTab = (state.mainView === 'practice' && (state.subtab === 'arpeggio' || state.subtab === 'sweep'));
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
  const isFhdMode = (state.resolvedUiProfile === 'fhd');
  const hasMaskRail = (state.subtab === 'scale' || state.subtab === 'exercise');
  const minBaseW = isHarmonicsTab ? 1080 : (isFhdMode ? 1160 : 1260);
  const minBaseH = isHarmonicsTab ? (isFhdMode ? 300 : 316) : (isArpeggioTab ? (isFhdMode ? 286 : 308) : (isFhdMode ? 332 : 356));
  const baseW = state.fretboardFitWidth ? Math.max(minBaseW, availW) : minBaseW;
  const widthRatio = baseW / minBaseW;
  const maxHRatio = isArpeggioTab ? (isFhdMode ? 1.14 : 1.22) : (isFhdMode ? 1.24 : 1.38);
  const baseH = Math.round(minBaseH * Math.min(maxHRatio, Math.max(1.0, Math.pow(widthRatio, 0.62))));
  const W = Math.round(baseW * zoomFactor);
  const H = Math.round(baseH * zoomFactor);

  const nStrings = 6;
  const maxFret = isHarmonicsTab ? state.harmonicMaxFret : state.maxDisplayedFret;
  const leftNutX = 62;
  const rightPad = 22;
  const topPad = 28;
  const bottomPad = hasMaskRail ? 68 : 40;
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
        <stop offset="0%" stop-color="#a3430e"/>
        <stop offset="100%" stop-color="#682707"/>
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

  // Tło podstrunnicy dopasowane do motywu (Ciemny / Jasny) oraz stylu aplikacji (Klasyczny / Neo-Glass / Rack)
  const effTheme = resolveEffectiveTheme();
  const effStyle = ['classic', 'modern', 'rack'].includes(state.uiStyle) ? state.uiStyle : 'classic';
  const lightMode = (effTheme === 'light');
  let boardBg = lightMode ? '#f6f1e9' : '#14100d';
  let fretWireCol = lightMode ? '#cfc6b8' : '#2b231d';
  let inlayCol = lightMode ? 'rgba(120,105,90,0.22)' : 'rgba(255,255,255,0.14)';
  let numBaseCol = lightMode ? '#57534e' : '#9c9288';
  let stringCol = lightMode ? '#8c7e70' : '#574b40';
  let nutCol = lightMode ? '#6b5e50' : '#8c8175';
  let boardStroke = lightMode ? '#d6cec2' : 'none';
  if (effStyle === 'modern') {
    boardBg = lightMode ? '#f5f8fd' : '#0b1120';
    fretWireCol = lightMode ? '#c8d6e8' : '#1e293b';
    inlayCol = lightMode ? 'rgba(2,132,199,0.20)' : 'rgba(56,189,248,0.16)';
    numBaseCol = lightMode ? '#475569' : '#7dd3fc';
    stringCol = lightMode ? '#64748b' : '#475569';
    nutCol = lightMode ? '#334155' : '#94a3b8';
    boardStroke = lightMode ? '#cbd5e1' : '#1e293b';
  } else if (effStyle === 'rack') {
    boardBg = lightMode ? '#eceff4' : '#0a0b0e';
    fretWireCol = lightMode ? '#b0b8c4' : '#323844';
    inlayCol = lightMode ? 'rgba(180,83,9,0.24)' : 'rgba(251,191,36,0.20)';
    numBaseCol = lightMode ? '#374151' : '#cbd5e1';
    stringCol = lightMode ? '#6b7280' : '#4b5563';
    nutCol = lightMode ? '#1f2937' : '#d1d5db';
    boardStroke = lightMode ? '#9ca3af' : '#353b47';
  }

  svg += `<rect x="${leftNutX}" y="${topPad - 10}" width="${boardW}" height="${boardH + 20}" fill="${boardBg}" rx="4" stroke="${boardStroke}" stroke-width="1"/>`;

  const markerFrets = [3,5,7,9,12,15,17,19,21,24];
  const dotsMode = (state.fretMarkersMode === 'dots');
  const numAll = !!state.numberAllFrets;

  // Subtelne podświetlenie progów markerowych oraz kropki inkrustacji (tryb „Kropki”)
  for (let f = 1; f <= maxFret; f++) {
    if (markerFrets.includes(f)) {
      const fx = leftNutX + (f - 1) * fretW;
      const cxM = leftNutX + (f - 0.5) * fretW;
      svg += `<rect x="${fx}" y="${topPad - 10}" width="${fretW}" height="${boardH + 20}" fill="${lightMode ? 'rgba(0,0,0,0.025)' : 'rgba(255,255,255,0.015)'}"/>`;
      if (dotsMode) {
        if (f === 12 || f === 24) {
          svg += `<circle cx="${cxM}" cy="${topPad + stringH * 1.5}" r="5.2" fill="${inlayCol}"/>`;
          svg += `<circle cx="${cxM}" cy="${topPad + stringH * 3.5}" r="5.2" fill="${inlayCol}"/>`;
        } else {
          svg += `<circle cx="${cxM}" cy="${topPad + stringH * 2.5}" r="5.2" fill="${inlayCol}"/>`;
        }
      }
    }
  }

  // Druty progowe (1..maxFret) i numery progów / znaczniki pod gryfem
  for (let f = 1; f <= maxFret; f++) {
    const x = leftNutX + f * fretW;
    svg += `<line x1="${x}" y1="${topPad - 10}" x2="${x}" y2="${topPad + boardH + 10}" stroke="${fretWireCol}" stroke-width="1.5"/>`;

    const isMarker = markerFrets.includes(f);
    const showNum = numAll || isHarmonicsTab || (!dotsMode && isMarker);
    const showDotBelow = dotsMode && !numAll && !isHarmonicsTab && isMarker;
    const cxF = leftNutX + (f - 0.5) * fretW;
    const numY = hasMaskRail ? (topPad + boardH + 34) : (topPad + boardH + 28);
    if (showNum) {
      const numColor = (f === 12 && (isHarmonicsTab || numAll)) ? '#f39237' : numBaseCol;
      const numWeight = (f === 12 || isMarker) ? '700' : '500';
      svg += `<text x="${cxF}" y="${numY}" fill="${numColor}" font-family="Segoe UI, system-ui, sans-serif" font-size="${numAll ? 11 : 12}" font-weight="${numWeight}" text-anchor="middle">${f}</text>`;
    } else if (showDotBelow) {
      if (f === 12 || f === 24) {
        svg += `<circle cx="${cxF - 4.5}" cy="${numY - 4}" r="3" fill="${numBaseCol}"/>`;
        svg += `<circle cx="${cxF + 4.5}" cy="${numY - 4}" r="3" fill="${numBaseCol}"/>`;
      } else {
        svg += `<circle cx="${cxF}" cy="${numY - 4}" r="3.2" fill="${numBaseCol}"/>`;
      }
    }
  }

  // Siodełko (próg 0) – wyraźna pionowa belka oddzielająca puste struny
  svg += `<line x1="${leftNutX}" y1="${topPad - 10}" x2="${leftNutX}" y2="${topPad + boardH + 10}" stroke="${nutCol}" stroke-width="2.5"/>`;

  // Struny (od najcieńszej E4 u góry do najgrubszej E2 na dole)
  for (let s = 0; s < nStrings; s++) {
    const row = nStrings - 1 - s;
    const y = topPad + row * stringH;
    const sw = (1.1 + (nStrings - 1 - s) * 0.24).toFixed(2);
    svg += `<line x1="${leftNutX}" y1="${y}" x2="${leftNutX + boardW}" y2="${y}" stroke="${stringCol}" stroke-width="${sw}"/>`;
  }

  // Kapodaster w zakładce Akord · Odkrywanie: przyciemnia progi przed kapodastrem i rysuje poprzeczkę kapo
  const activeChordCapo = isChordTab ? Math.max(0, Math.min(maxFret, parseInt(state.chordCapoFret || 0, 10) || 0)) : 0;
  if (activeChordCapo > 0) {
    const capoLeftX = leftNutX + (activeChordCapo - 1) * fretW;
    const capoRightX = leftNutX + activeChordCapo * fretW;
    const capoBarX = capoRightX - Math.min(10, fretW * 0.22);
    const dimW = Math.max(0, capoLeftX - leftNutX);
    if (dimW > 0) {
      svg += `<rect x="${leftNutX}" y="${topPad - 10}" width="${dimW}" height="${boardH + 20}" fill="${lightMode ? 'rgba(60,48,36,0.22)' : 'rgba(6,4,3,0.68)'}" pointer-events="none"/>`;
    }
    svg += `<rect x="${capoBarX - 4}" y="${topPad - 16}" width="8" height="${boardH + 32}" rx="4" fill="#d97724" stroke="#fde68a" stroke-width="1.3" pointer-events="none"/>`;
    svg += `<rect x="${capoBarX - 28}" y="${topPad - 26}" width="56" height="15" rx="4" fill="#9e420e" stroke="#f59e0b" stroke-width="1" pointer-events="none"/>`;
    svg += `<text x="${capoBarX}" y="${topPad - 15.5}" fill="#fff7ed" font-family="Segoe UI, system-ui, sans-serif" font-size="9.5" font-weight="800" text-anchor="middle" pointer-events="none">KAPO ${activeChordCapo}</text>`;
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
  let arpBgScaleSet = null, arpBgScaleRootPc = state.arpDiatonicScaleRootPc;
  if (isArpeggioTab && state.arpData) {
    if (state.arpShowScaleOnFretboard && Array.isArray(state.arpData.scalePcs)) {
      arpBgScaleSet = new Set(state.arpData.scalePcs);
      if (state.arpData.scaleRootPc !== undefined) arpBgScaleRootPc = state.arpData.scaleRootPc;
    }
    arpNodeMap = new Map();
    arpNextSegMap = new Map();
    arpTechMap = new Map();

    let activeNodes = state.arpData.nodes || [];
    let activeSteps = state.arpData.steps || [];
    const isSweepTrans = isSweepTab && (state.sweepViewMode === 'transition');
    const isSweepCompPlay = isSweepTab && !isSweepTrans &&
      state.sweepPlaybackTarget === 'comp' &&
      Array.isArray(state.sweepComposition) &&
      state.sweepComposition.length > 0 &&
      (state.arpPlaying || state.arpPlayStepIdx >= 0);

    if (state.arpData.rootPc !== undefined) arpActiveRootPc = state.arpData.rootPc;
    if (isSweepTrans && state.arpData.transition && state.arpData.transition.segments && state.arpData.transition.segments.length > 0) {
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
    } else if (isSweepCompPlay) {
      const segs = state.sweepComposition;
      const segIdx = Math.max(0, Math.min(state.sweepCompActiveIdx || 0, segs.length - 1));
      const curSeg = segs[segIdx];
      activeSteps = curSeg.steps || [];
      const playedKeys = new Set(activeSteps.map(st => `${st.s}:${st.f}`));
      activeNodes = (curSeg.nodes || []).filter(n => playedKeys.has(`${n.s}:${n.f}`));
      if (curSeg.rootPc !== undefined) arpActiveRootPc = curSeg.rootPc;
      const nextSeg = (segIdx + 1 < segs.length) ? segs[segIdx + 1] : null;
      if (nextSeg && nextSeg !== curSeg) {
        const nextPlayed = new Set((nextSeg.steps || []).map(st => `${st.s}:${st.f}`));
        arpNextRootPc = (nextSeg.rootPc !== undefined) ? nextSeg.rootPc : arpActiveRootPc;
        for (const nn of (nextSeg.nodes || [])) {
          if (nextPlayed.has(`${nn.s}:${nn.f}`)) arpNextSegMap.set(`${nn.s}:${nn.f}`, nn);
        }
      }
      const allSteps = getSweepCompositionSteps();
      if (state.arpPlayStepIdx >= 0 && allSteps[state.arpPlayStepIdx]) {
        arpActiveStepObj = allSteps[state.arpPlayStepIdx];
      }
    } else {
      if (isSweepTab) {
        activeSteps = getActiveArpeggioSteps();
        if (activeSteps.length > 0 && !isSweepTransitionActive()) {
          const existingMap = new Map(activeNodes.map(n => [`${n.s}:${n.f}`, n]));
          const seenN = new Set();
          const syncedNodes = [];
          for (const st of activeSteps) {
            const k = `${st.s}:${st.f}`;
            if (seenN.has(k)) continue;
            seenN.add(k);
            if (existingMap.has(k)) {
              syncedNodes.push(existingMap.get(k));
            } else {
              syncedNodes.push({
                s: st.s, f: st.f, midi: st.midi, freq: st.freq,
                note: st.note, interval: st.interval, isRoot: !!st.isRoot
              });
            }
          }
          activeNodes = syncedNodes;
        }
      }
      // Jeśli wybrano Cały gryf, w trakcie odtwarzania/ćwiczenia pokazuj WYŁĄCZNIE dźwięki granej sekwencji!
      if (!isSweepTab && state.arpPattern === 0 && (state.arpPlaying || state.arpPlayStepIdx >= 0) && activeSteps.length > 0) {
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

    // Subtelna trajektoria kostkowania w Sweep Picking, Od prymy oraz Inwersjach (łączy kolejne nuty w kształcie)
    if ((isSweepTab || state.arpPattern === 5 || state.arpPattern === 6) && activeNodes.length >= 2) {
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

  const recMidisForFret = getRecognitionMidis();
  const activeMidisSet = (recMidisForFret.length > 0)
    ? new Set(recMidisForFret.map(m => Math.round(m)))
    : null;

  // Zestaw 7 dźwięków wybranej (lub aktualnie przeciąganej szablonem!) tonacji z Koła Kwintowego dla zakładki Akordy
  let cofFretKeyRootPc = state.chordScaleRootPc;
  let cofFretKeySet = null;
  let cofFretTriadSet = null;
  if (isChordTab && (state.chordShowScaleOnFretboard || (typeof cofDrag !== 'undefined' && cofDrag.isTemplateDrag) || state.cofHighlightPc !== null)) {
    const dragging = (typeof cofDrag !== 'undefined' && cofDrag.isTemplateDrag);
    const isMinKey = dragging
      ? !!cofDrag.previewIsMinor
      : (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
    cofFretKeyRootPc = dragging ? cofDrag.previewRootPc : state.chordScaleRootPc;
    const scaleOffsets = isMinKey ? [0, 2, 3, 5, 7, 8, 10] : [0, 2, 4, 5, 7, 9, 11];
    const triadOffsets = isMinKey ? [0, 3, 7] : [0, 4, 7];
    cofFretKeySet = new Set(scaleOffsets.map(o => (cofFretKeyRootPc + o) % 12));
    cofFretTriadSet = new Set(triadOffsets.map(o => (cofFretKeyRootPc + o) % 12));
  }

  for (const cell of state.fretboardData.cells) {
    const s = cell.s, f = cell.f;
    if (f > maxFret) continue;
    const row = nStrings - 1 - s;
    const cy = topPad + row * stringH;
    const cx = (f === 0) ? (leftNutX * 0.48) : (leftNutX + (f - 0.5) * fretW);
    const cellW = (f === 0) ? (leftNutX * 0.85) : fretW;

    const masked = isCellMasked(s, f);
    const nodeOpacity = masked ? '0.20' : '1.0';

    // Okrągły obszar kliknięcia / przeciągania (łatwe celowanie po skosie, bez łapania sąsiednich nut)
    const hitR = Math.min(cellW, stringH) * 0.5;
    svg += `<circle class="fb-hit-rect" cx="${cx}" cy="${cy}" r="${hitR.toFixed(1)}"
                  onmousedown="onFretCellMouseDown(event, ${s}, ${f})"
                  onmouseenter="onFretCellMouseEnter(event, ${s}, ${f})"/>`;

    const pc = ((cell.midi % 12) + 12) % 12;
    const degHighlightAllowed = (state.subtab !== 'scale') || isScaleDegreeHighlightEnabled(pc);
    const recMidiForCell = getRecognitionMidi();
    const activePlayPc = (recMidiForCell !== null && recMidiForCell !== undefined)
      ? (((Math.round(recMidiForCell) % 12) + 12) % 12)
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
      const behindCapo = (activeChordCapo > 0 && f < activeChordCapo);
      const isVoicingNote = !behindCapo && (vFret === f);
      const isVoicingTapped = isVoicingNote && activeChordVoicing.tapped && activeChordVoicing.tapped[s];
      const vRootPc = (activeChordVoicing && activeChordVoicing.rootPc !== undefined) ? activeChordVoicing.rootPc : state.rootPc;

      if (f === activeChordCapo && vFret === -1) {
        // Wyciszona struna (✕) w wybranym chwycie akordu (przy siodełku lub tuż przed kapodastrem)
        const muteX = (activeChordCapo > 0)
          ? (leftNutX + (activeChordCapo - 0.84) * fretW)
          : (cx * 0.55);
        svg += `<text x="${muteX}" y="${cy + 4.5}" fill="#ef4444" font-family="Segoe UI, sans-serif" font-size="13" font-weight="800" text-anchor="middle">✕</text>`;
      }

      if (behindCapo) {
        // Progi za kapodastrem są nieaktywne — nic nie podświetlamy
      } else if (state.cofHighlightPc !== null && pc === state.cofHighlightPc) {
        shouldDrawCircle = true;
        fillAttr = '#0284c7';
        strokeAttr = '#7dd3fc';
        textColor = '#ffffff';
      } else if (isVoicingNote) {
        shouldDrawCircle = true;
        if (isVoicingTapped) {
          isTappedVoicingNode = true;
          fillAttr = '#0284c7';
          strokeAttr = '#7dd3fc';
          textColor = '#ffffff';
        } else if (pc === vRootPc) {
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#d97724';
          textColor = '#fff5eb';
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
      } else if (cofFretKeySet && cofFretKeySet.has(pc)) {
        shouldDrawCircle = true;
        if (pc === cofFretKeyRootPc) {
          customNodeOpacity = '0.84';
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#d97724';
          textColor = '#fff5eb';
        } else if (cofFretTriadSet && cofFretTriadSet.has(pc)) {
          customNodeOpacity = '0.68';
          fillAttr = 'url(#scaleNoteGrad)';
          strokeAttr = '#f39237';
          textColor = '#f8f5f0';
        } else {
          customNodeOpacity = '0.46';
          fillAttr = 'url(#scaleNoteGrad)';
          strokeAttr = '#6e4e32';
          textColor = '#f8f5f0';
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
          strokeAttr = '#d97724';
          textColor = '#fff5eb';
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
      } else if (arpBgScaleSet && arpBgScaleSet.has(pc)) {
        shouldDrawCircle = true;
        if (pc === arpBgScaleRootPc) {
          customNodeOpacity = '0.58';
          fillAttr = 'url(#rootGrad)';
          strokeAttr = '#d97724';
          textColor = '#fff5eb';
        } else {
          customNodeOpacity = '0.40';
          fillAttr = 'url(#scaleNoteGrad)';
          strokeAttr = '#6e4e32';
          textColor = '#e7dfd6';
        }
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
          strokeAttr = '#d97724';
          textColor = '#fff5eb';
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
        strokeAttr = '#d97724';
        textColor = '#fff5eb';
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
        svg += `<rect x="${cx - 10}" y="${cy - 6}" width="20" height="12" rx="6" fill="#14100d"/>`;
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
      } else if (activeLblType === 'intervals') {
        label = computeIntervalLabel(pc, arpBgScaleRootPc, false);
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
    const isCapoOpenNode = isChordTab && activeChordCapo > 0 && f === activeChordCapo && activeChordVoicing && activeChordVoicing.frets && activeChordVoicing.frets[s] === f;
    svg += `<g class="fb-node" opacity="${customNodeOpacity}" onmousedown="onFretCellMouseDown(event, ${s}, ${f})" onmouseenter="onFretCellMouseEnter(event, ${s}, ${f})">`;
    if (arpIsCurPlay) {
      svg += `<circle cx="${cx}" cy="${cy}" r="${(radius + 4.5).toFixed(1)}" fill="none" stroke="#4ade80" stroke-width="2.5" opacity="0.9"/>`;
    } else if (isCapoOpenNode) {
      svg += `<circle cx="${cx}" cy="${cy}" r="${(radius + 3.6).toFixed(1)}" fill="none" stroke="#fbbf24" stroke-width="2.1" stroke-dasharray="3,2" opacity="0.95"/>`;
    }
    svg += `<circle class="fb-note-dot" cx="${cx}" cy="${cy}" r="${radius}" fill="${fillAttr}" stroke="${strokeAttr}" stroke-width="${arpIsCurPlay ? '2.5' : '1.5'}"/>`;
    svg += `<text x="${cx}" y="${cy + noteFontSize * 0.34}" fill="${textColor}" font-family="Segoe UI, system-ui, sans-serif" font-size="${isTappedVoicingNode ? Math.max(9.5, noteFontSize - 2) : noteFontSize}" font-weight="700" text-anchor="middle">${label}</text>`;
    if (isSweepTab && arpTechMap) {
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
  if (state.mainView === 'practice' && state.subtab === 'chord' && (state.chordCapoFret || 0) > 0 && f < state.chordCapoFret) {
    return;
  }
  if (state.lockUnmarkedNotes && (!state.markedFretCells || !state.markedFretCells.has(`${s}:${f}`))) {
    return;
  }

  const found = state.fretboardData.cells.find(c => c.s === s && c.f === f);
  if (found) {
    if (previewIsPlaying) stopCurrentScalePreview(false);
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
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
  if (previewIsPlaying) {
    stopCurrentScalePreview(true);
    return;
  }
  stopCurrentScalePreview(false);

  // W zakładce Akord · Odkrywanie odtwórz aktualnie wybrany chwyt (bez nakładania z poprzednim)
  if (state.mainView === 'practice' && state.subtab === 'chord' && state.chordVoicingsList && state.chordVoicingsList.length > 0) {
    const v = state.chordVoicingsList[state.selectedChordVoicingIdx] || state.chordVoicingsList[0];
    const freqs = (v && Array.isArray(v.freqs)) ? v.freqs : [];
    if (freqs.length === 0) return;
    const myToken = previewSessionToken;
    previewIsPlaying = true;
    updateTopPreviewButtonUI();
    const gainScale = 1.0 / Math.sqrt(Math.max(1, freqs.length));
    freqs.forEach((f, i) => {
      const tid = setTimeout(() => {
        if (previewSessionToken !== myToken) return;
        pluckGuitarFrequency(f, 1.35, gainScale, true);
      }, i * 48);
      previewTimers.push(tid);
    });
    const endTid = setTimeout(() => {
      if (previewSessionToken !== myToken) return;
      previewIsPlaying = false;
      previewTimers = [];
      updateTopPreviewButtonUI();
    }, freqs.length * 48 + 900);
    previewTimers.push(endTid);
    return;
  }

  if (!state.fretboardData) return;
  const raw = state.fretboardData.cells.filter(c => c.inPosition && !isCellMasked(c.s, c.f));
  if (raw.length === 0) return;

  // Uporządkuj unikalne dźwięki rosnąco (od najniższej prymy, jeśli dostępna) i odtwarzaj bez nakładania
  const byMidi = new Map();
  raw.forEach(c => {
    if (!byMidi.has(c.midi) || c.f < byMidi.get(c.midi).f) byMidi.set(c.midi, c);
  });
  let sorted = Array.from(byMidi.values()).sort((a, b) => a.midi - b.midi);
  const firstRootIdx = sorted.findIndex(c => c.isRoot);
  if (firstRootIdx >= 0 && sorted.length - firstRootIdx >= 6) {
    sorted = sorted.slice(firstRootIdx);
  }
  const active = sorted.slice(0, 12);
  if (active.length === 0) return;

  const myToken = previewSessionToken;
  previewIsPlaying = true;
  updateTopPreviewButtonUI();

  const stepMs = 230;
  active.forEach((c, idx) => {
    const tid = setTimeout(() => {
      if (previewSessionToken !== myToken) return;
      stopPreviewVoices(0.025);
      state.detectedMidi = c.midi;
      drawFretboard();
      pluckGuitarFrequency(c.freq, 0.55, 1.0, true);
    }, idx * stepMs);
    previewTimers.push(tid);
  });
  const endTid = setTimeout(() => {
    if (previewSessionToken !== myToken) return;
    previewIsPlaying = false;
    previewTimers = [];
    updateTopPreviewButtonUI();
  }, active.length * stepMs + 350);
  previewTimers.push(endTid);
}

function switchMainView(view, btn) {
  stopCurrentScalePreview(false);
  stopDetectScalePolling();
  if (typeof stopArpeggioPlayback === 'function') {
    stopArpeggioPlayback(false);
  }
  if (view !== 'changes' && typeof stopChangesPlayback === 'function' && state.changesIsPlaying) {
    stopChangesPlayback();
  }
  if (view === 'settings' && state.mainView !== 'settings') {
    state.settingsReturnView = { mainView: state.mainView, subtab: state.subtab };
  } else if (view !== 'settings') {
    state.settingsReturnView = null;
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
      tuner: t('← Stroik (Tuner)', '← Tuner'),
      ear: t('Kształcenie słuchu', 'Ear Training'),
      changes: t('Zmiany i Progresje', 'Changes & Progressions'),
      workshop: t('Warsztat – Sesje', 'Workshop – Sessions'),
      settings: t('Ustawienia', 'Settings')
    };
    title.textContent = titles[view] || '';
  }
  renderAll();
}

function switchPracticeSubtab(sub, btn) {
  stopCurrentScalePreview(false);
  stopDetectScalePolling();
  if (typeof stopExercisePlayer === 'function') {
    stopExercisePlayer(false);
  }
  if (typeof stopArpeggioPlayback === 'function') {
    stopArpeggioPlayback(false);
  }
  if (sub === 'scale_practice') {
    state.subtab = 'exercise';
    state.exerciseSubView = 'scale_practice';
  } else {
    state.subtab = sub;
  }
  document.querySelectorAll('.subtab-btn').forEach(b => {
    const isMatch = btn ? (b === btn) : (b.getAttribute('data-subtab') === state.subtab);
    b.classList.toggle('active', isMatch);
  });
  renderAll();
}

function setExerciseSubView(viewName) {
  if (typeof stopExercisePlayer === 'function') stopExercisePlayer(false);
  state.exerciseSubView = viewName || 'composer';
  state.mainView = 'practice';
  state.subtab = 'exercise';
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-subtab') === 'exercise');
  });
  renderAll();
}

async function renderAll() {
  if (previewIsPlaying) stopCurrentScalePreview(false);
  await refreshFretboardFromCpp();
  // Ukryj prawy panel inspektora tam gdzie widok jest pełnoekranowy oraz zastosuj stan zwinięcia paneli
  applyPanelCollapseStates();
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

// Nazwa akordu na wyświetlaczu (hybryda: nuty z PitchPlease + namer dechordera z inwersjami).
function resolveChordLabel(activeMidis) {
  if (!activeMidis || activeMidis.length < 2) return '';
  if (window.DechorderDsp && window.DechorderDsp.matchChordFromMidis) {
    const r = window.DechorderDsp.matchChordFromMidis(activeMidis);
    return window.DechorderDsp.readingLabel(r) || '';
  }
  return identifyPolyChordLabel(activeMidis, getRootLabels());
}

// Aktywne nuty rozpoznawania na gryfie (hybryda: nuty z PitchPlease).
function getRecognitionMidis() {
  if (state.detectedMidis && state.detectedMidis.length > 0) return state.detectedMidis;
  if (state.detectedMidi !== null && state.detectedMidi !== undefined) return [state.detectedMidi];
  return [];
}

function getRecognitionMidi() {
  const ms = getRecognitionMidis();
  if (ms.length > 0) return ms[ms.length - 1];
  return (state.detectedMidi !== null && state.detectedMidi !== undefined) ? state.detectedMidi : null;
}

function buildRootButtonsHtml(onClickFnName) {
  const labels = getRootLabels();
  return `<div class="btn-grid-6">` +
    labels.map((lbl, pc) =>
      `<button class="chip-btn ${state.rootPc === pc ? 'active' : ''}" onclick="${onClickFnName}(${pc})">${lbl}</button>`
    ).join('') + `</div>`;
}

const COF_RING_ITEMS = [
  { pc: 0,  maj: 'C',  min: 'Am',  minPc: 9,  preferFlats: false },
  { pc: 7,  maj: 'G',  min: 'Em',  minPc: 4,  preferFlats: false },
  { pc: 2,  maj: 'D',  min: 'Bm',  minPc: 11, preferFlats: false },
  { pc: 9,  maj: 'A',  min: 'F♯m', minPc: 6,  preferFlats: false },
  { pc: 4,  maj: 'E',  min: 'C♯m', minPc: 1,  preferFlats: false },
  { pc: 11, maj: 'B',  min: 'G♯m', minPc: 8,  preferFlats: false },
  { pc: 6,  maj: 'F♯', min: 'D♯m', minPc: 3,  preferFlats: false },
  { pc: 1,  maj: 'D♭', min: 'B♭m', minPc: 10, preferFlats: true  },
  { pc: 8,  maj: 'A♭', min: 'Fm',  minPc: 5,  preferFlats: true  },
  { pc: 3,  maj: 'E♭', min: 'Cm',  minPc: 0,  preferFlats: true  },
  { pc: 10, maj: 'B♭', min: 'Gm',  minPc: 7,  preferFlats: true  },
  { pc: 5,  maj: 'F',  min: 'Dm',  minPc: 2,  preferFlats: true  }
];

const cofDrag = {
  active: false,
  isTemplateDrag: false,
  moved: false,
  holdTimer: null,
  startX: 0,
  startY: 0,
  startMajorPc: 0,
  startIsMinor: false,
  startRingIdx: 0,
  fromWedge: false,
  grabOffsetDeg: 0,
  hoverIdx: 0,
  currentAngleDeg: -90,
  previewRootPc: 0,
  previewIsMinor: false,
  svgEl: null
};

function getCofKeyNotesInfo(ringIdx, isMinor) {
  const safeIdx = ((ringIdx % 12) + 12) % 12;
  const item = COF_RING_ITEMS[safeIdx];
  const rootPc = isMinor ? item.minPc : item.pc;
  const notePool = item.preferFlats
    ? ['C', 'D♭', 'D', 'E♭', 'E', 'F', 'G♭', 'G', 'A♭', 'A', 'B♭', 'B']
    : ['C', 'C♯', 'D', 'D♯', 'E', 'F', 'F♯', 'G', 'G♯', 'A', 'A♯', 'B'];
  const intervals = isMinor ? [0, 2, 3, 5, 7, 8, 10] : [0, 2, 4, 5, 7, 9, 11];
  const degLabels = isMinor ? ['1', '2', '♭3', '4', '5', '♭6', '♭7'] : ['1', '2', '3', '4', '5', '6', '7'];
  const triadOffsets = isMinor ? new Set([0, 3, 7]) : new Set([0, 4, 7]);
  const notes = intervals.map((semi, idx) => {
    const pc = (rootPc + semi) % 12;
    return {
      pc,
      name: notePool[pc],
      deg: degLabels[idx],
      isRoot: idx === 0,
      isTriad: triadOffsets.has(semi)
    };
  });
  return {
    ringIdx: safeIdx,
    item,
    rootPc,
    isMinor,
    centerName: isMinor ? item.min : item.maj,
    keyTitle: isMinor ? `${item.min} (${t('Mol naturalny', 'Natural Minor')})` : `${item.maj} (${t('Dur', 'Major')})`,
    notes
  };
}

function playCircleKeyTriad(rootPc, isMinor) {
  try {
    ensureAudioCtx();
    // Bazowa wysokość w rejestrze gitarowym (E3..Eb4)
    const baseMidi = 48 + (((rootPc % 12) + 12) % 12) + (rootPc < 4 ? 12 : 0);
    const offsets = isMinor ? [0, 3, 7, 12] : [0, 4, 7, 12];
    offsets.forEach((off, idx) => {
      const freq = midiToFreq(baseMidi + off);
      setTimeout(() => pluckGuitarFrequency(freq, 0.58), idx * 34);
    });
  } catch (e) {}
}

function playCircleScaleNotes() {
  const isMinor = (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
  const activeMajorPc = isMinor ? ((state.chordScaleRootPc + 3) % 12) : state.chordScaleRootPc;
  const activeIdx = Math.max(0, COF_RING_ITEMS.findIndex(it => it.pc === activeMajorPc));
  const info = getCofKeyNotesInfo(activeIdx, isMinor);
  const baseMidi = 48 + info.rootPc + (info.rootPc < 4 ? 12 : 0);
  const intervals = isMinor ? [0, 2, 3, 5, 7, 8, 10, 12] : [0, 2, 4, 5, 7, 9, 11, 12];
  intervals.forEach((semi, idx) => {
    setTimeout(() => {
      const m = baseMidi + semi;
      state.cofHighlightPc = ((m % 12) + 12) % 12;
      drawFretboard();
      pluckGuitarFrequency(midiToFreq(m), 0.42);
      if (idx === intervals.length - 1) {
        setTimeout(() => {
          state.cofHighlightPc = null;
          drawFretboard();
        }, 380);
      }
    }, idx * 145);
  });
}

function onCofNoteChipDown(pc) {
  state.cofHighlightPc = pc;
  drawFretboard();
  try {
    const baseMidi = 52 + (((pc - 4) % 12 + 12) % 12);
    startGuitarSustainedVoice('cof_note_' + pc, midiToFreq(baseMidi), 4.0);
  } catch (e) {}
}

function onCofNoteChipUp(pc) {
  try { stopSustainedVoice('cof_note_' + pc); } catch (e) {}
  if (state.cofHighlightPc === pc) {
    state.cofHighlightPc = null;
    drawFretboard();
  }
}

function selectCircleKey(majorPc, isMinor, playAudio = true) {
  state.chordFilterByScale = true;
  state.chordScaleId = isMinor ? 'natural_minor' : 'major';
  state.chordScaleRootPc = isMinor ? ((majorPc + 9) % 12) : majorPc;
  state.rootPc = state.chordScaleRootPc;
  state.chordId = isMinor ? 'minor' : 'major';
  state.chordGroupFilter = 'all';
  state.chordScaleExtraTypeFilter = false;
  state.chordShowScaleOnFretboard = true;
  state.cofHighlightPc = null;
  state.selectedChordVoicingIdx = 0;
  if (playAudio) {
    playCircleKeyTriad(state.chordScaleRootPc, isMinor);
  }
  renderAll();
}

// Krótkie kliknięcie na Kole Kwintowym (w tym obok toniki i na wewnętrznym pierścieniu):
// NIE zmienia zakresu 6 dźwięków ani tonacji na wewnętrzny mol — ładuje i gra kliknięty akord oraz pokazuje dźwięki!
function selectCircleChordInCurrentKey(ringIdx, isMinorNode, playAudio = true) {
  const safeIdx = ((ringIdx % 12) + 12) % 12;
  const item = COF_RING_ITEMS[safeIdx];
  const chordRootPc = isMinorNode ? item.minPc : item.pc;

  // Dopasuj rodzaj akordu z bieżącej rodziny diatonicznej (np. G7 przy włączonych septymowych), albo czysty dur/mol
  let chordId = isMinorNode ? 'minor' : 'major';
  const diatonic = (state.diatonicData && state.diatonicData.chords) || [];
  const matchingDia = diatonic.find(c => {
    if (c.rootPc !== chordRootPc) return false;
    const isDiaMin = c.chordId.startsWith('m') && !c.chordId.startsWith('maj');
    return isMinorNode ? isDiaMin : !isDiaMin;
  });
  if (matchingDia) chordId = matchingDia.chordId;

  state.rootPc = chordRootPc;
  state.chordId = chordId;
  state.allChordTypes = false;
  state.chordFilterByScale = false;
  state.chordShowScaleOnFretboard = true;
  state.cofHighlightPc = null;
  state.selectedChordVoicingIdx = 0;
  if (playAudio) {
    playCircleKeyTriad(chordRootPc, isMinorNode);
  }
  renderAll();
}

function selectDiatonicChord(rootPc, chordId) {
  state.rootPc = rootPc;
  state.chordId = chordId;
  state.allChordTypes = false;
  state.chordFilterByScale = false;
  state.selectedChordVoicingIdx = 0;
  renderAll();
}

function sendProgressionToChanges(degrees) {
  const chords = (state.diatonicData && state.diatonicData.chords) || [];
  if (chords.length < 7) return;
  const keyRoot = state.chordScaleRootPc;
  const isMinorKey = (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
  // Przesunięcia w Zmianach liczone są zawsze bezpośrednio od toniki wybranej tonacji (zarówno w dur, jak i w moll)
  state.changesMeasures = degrees.map(dIdx => {
    const c = chords[dIdx % chords.length];
    return {
      volta: 0,
      beats: [{ offsetSemi: (((c.rootPc - keyRoot) % 12) + 12) % 12, chordId: c.chordId, roman: c.roman }, null, null, null]
    };
  });
  state.changesKeyRootPc = keyRoot;
  state.changesIsMinor = isMinorKey;
  state.changesBpm = 100;
  state.changesFeel = 'Prosto';
  state.changesEditingSlot = null;
  state.changesScreen = 'editor';
  const nav = document.getElementById('navChangesBtn');
  switchMainView('changes', nav);
}

function buildCofSvgMarkup(activeIdx, isMinorScale, templateMode, wedgeAngleOverrideDeg = null) {
  const light = isLightTheme();
  const cx = 160, cy = 160;
  const rMajRing = 126, rMajNode = 21;
  const rMinRing = 82,  rMinNode = 15.5;
  const safeActiveIdx = ((activeIdx % 12) + 12) % 12;
  const info = getCofKeyNotesInfo(safeActiveIdx, isMinorScale);

  const trackStroke = light ? '#d6cec2' : '#2f2720';
  const nodeBg = light ? '#ffffff' : '#171310';
  const nodeMinBg = light ? '#f5f0e8' : '#14100d';
  const nodeStroke = light ? '#cfc6b8' : '#352d25';
  const textCol = light ? '#1c1917' : '#f5f2eb';
  const mutedCol = light ? '#57534e' : '#a8a29b';

  // Wycinek szablonu 6 dźwięków (IV–I–V zewnętrznie + ii–vi–iii wewnętrznie)
  const centerDeg = (wedgeAngleOverrideDeg !== null) ? wedgeAngleOverrideDeg : (-90 + safeActiveIdx * 30);
  const aStart = (centerDeg - 45) * Math.PI / 180;
  const aEnd   = (centerDeg + 45) * Math.PI / 180;
  const rOut = rMajRing + rMajNode + 6;
  const rIn  = rMinRing - rMinNode - 5;
  const p1x = (cx + rOut * Math.cos(aStart)).toFixed(1), p1y = (cy + rOut * Math.sin(aStart)).toFixed(1);
  const p2x = (cx + rOut * Math.cos(aEnd)).toFixed(1),   p2y = (cy + rOut * Math.sin(aEnd)).toFixed(1);
  const p3x = (cx + rIn  * Math.cos(aEnd)).toFixed(1),   p3y = (cy + rIn  * Math.sin(aEnd)).toFixed(1);
  const p4x = (cx + rIn  * Math.cos(aStart)).toFixed(1), p4y = (cy + rIn  * Math.sin(aStart)).toFixed(1);

  const wedgeFill = templateMode
    ? (light ? 'rgba(234, 88, 12, 0.20)' : 'rgba(243, 146, 55, 0.22)')
    : (light ? 'rgba(217, 115, 22, 0.10)' : 'rgba(243, 146, 55, 0.09)');
  const wedgeStroke = templateMode
    ? '#f39237'
    : (light ? 'rgba(217, 115, 22, 0.35)' : 'rgba(243, 146, 55, 0.28)');

  // Małe uchwyty kierunkowe na krawędziach wycinka w trybie Szablonu (wizualny znak, że można obracać 6 dźwięków!)
  let handlesSvg = '';
  if (templateMode) {
    const rMid = (rOut + rIn) * 0.5;
    const h1x = (cx + rMid * Math.cos(aStart)).toFixed(1), h1y = (cy + rMid * Math.sin(aStart)).toFixed(1);
    const h2x = (cx + rMid * Math.cos(aEnd)).toFixed(1),   h2y = (cy + rMid * Math.sin(aEnd)).toFixed(1);
    handlesSvg = `
      <circle cx="${h1x}" cy="${h1y}" r="4.2" fill="#f39237" stroke="#14100d" stroke-width="1.3" pointer-events="none"/>
      <circle cx="${h2x}" cy="${h2y}" r="4.2" fill="#f39237" stroke="#14100d" stroke-width="1.3" pointer-events="none"/>`;
  }

  const wedgeSvg = `
    <g style="cursor:${templateMode ? 'grabbing' : 'grab'}" onpointerdown="onCofPointerDown(event, ${info.item.pc}, ${isMinorScale ? 'true' : 'false'}, ${safeActiveIdx}, true)">
      <path d="M ${p1x} ${p1y} A ${rOut} ${rOut} 0 0 1 ${p2x} ${p2y} L ${p3x} ${p3y} A ${rIn} ${rIn} 0 0 0 ${p4x} ${p4y} Z"
            fill="${wedgeFill}" stroke="${wedgeStroke}" stroke-width="${templateMode ? 2.4 : 1.2}"
            ${templateMode ? 'stroke-dasharray="6 3"' : ''}/>
      ${handlesSvg}
    </g>`;

  // Role szablonu 6 dźwięków dla wycinka (-1, 0, +1 względem toniki)
  const majTemplateRoles = isMinorScale
    ? { '-1': '♭VI', '0': '♭III', '1': '♭VII' }
    : { '-1': 'IV',  '0': 'I',    '1': 'V'    };
  const minTemplateRoles = isMinorScale
    ? { '-1': 'iv',  '0': 'i',    '1': 'v'    }
    : { '-1': 'ii',  '0': 'vi',   '1': 'iii'  };

  let nodes = '';
  COF_RING_ITEMS.forEach((it, i) => {
    const ang = (-90 + i * 30) * Math.PI / 180;
    const cosA = Math.cos(ang), sinA = Math.sin(ang);
    const mx = (cx + rMajRing * cosA).toFixed(1);
    const my = (cy + rMajRing * sinA).toFixed(1);
    const nx = (cx + rMinRing * cosA).toFixed(1);
    const ny = (cy + rMinRing * sinA).toFixed(1);

    const relOffset = ((i - safeActiveIdx + 18) % 12) - 6; // -6..+5
    const inSixFamily = (relOffset === -1 || relOffset === 0 || relOffset === 1);
    const majorActive = !isMinorScale && relOffset === 0;
    const minorActive = isMinorScale && relOffset === 0;
    const curChordIsMin = typeof state.chordId === 'string' && state.chordId.startsWith('m') && !state.chordId.startsWith('maj');
    const isClickedMajChord = !curChordIsMin && state.rootPc === it.pc && !majorActive;
    const isClickedMinChord = curChordIsMin && state.rootPc === it.minPc && !minorActive;

    const majStroke = isClickedMajChord ? '#38bdf8' : (majorActive ? '#e07b2e' : (inSixFamily ? '#c25918' : nodeStroke));
    const minStroke = isClickedMinChord ? '#38bdf8' : (minorActive ? '#bbf7d0' : (inSixFamily ? '#4ade80' : nodeStroke));

    const majFill = majorActive
      ? '#9e420e'
      : (isClickedMajChord ? (light ? '#e0f2fe' : '#102a3a') : (templateMode && inSixFamily ? (light ? '#fff3e4' : '#2c1e12') : nodeBg));
    const minFill = minorActive
      ? '#22c55e'
      : (isClickedMinChord ? (light ? '#e0f2fe' : '#102a3a') : (templateMode && inSixFamily ? (light ? '#e8faef' : '#13261b') : nodeMinBg));

    const majRole = inSixFamily ? majTemplateRoles[String(relOffset)] : '';
    const minRole = inSixFamily ? minTemplateRoles[String(relOffset)] : '';

    const majTextMarkup = (templateMode && inSixFamily)
      ? `<text x="${mx}" y="${(+my + 0.5).toFixed(1)}" fill="${majorActive ? '#fff5eb' : '#e07b2e'}" font-size="${majRole.length > 2 ? 10.5 : 12.5}" font-weight="900" text-anchor="middle">${majRole}</text>
         <text x="${mx}" y="${(+my + 11.5).toFixed(1)}" fill="${majorActive ? '#fed7aa' : mutedCol}" font-size="8.5" font-weight="700" text-anchor="middle">${it.maj}</text>`
      : `<text x="${mx}" y="${(+my + 5).toFixed(1)}" fill="${majorActive ? '#fff5eb' : (inSixFamily ? textCol : mutedCol)}" font-size="${it.maj.length > 1 ? 13 : 14.5}" font-weight="800" text-anchor="middle">${it.maj}</text>`;

    const minTextMarkup = (templateMode && inSixFamily)
      ? `<text x="${nx}" y="${(+ny + 0.5).toFixed(1)}" fill="${minorActive ? '#05140b' : '#4ade80'}" font-size="10.5" font-weight="900" text-anchor="middle">${minRole}</text>
         <text x="${nx}" y="${(+ny + 9.5).toFixed(1)}" fill="${minorActive ? '#052e16' : mutedCol}" font-size="7.5" font-weight="700" text-anchor="middle">${it.min}</text>`
      : `<text x="${nx}" y="${(+ny + 3.8).toFixed(1)}" fill="${minorActive ? '#05140b' : (inSixFamily ? textCol : mutedCol)}" font-size="${it.min.length > 2 ? 9.5 : 10.5}" font-weight="700" text-anchor="middle">${it.min}</text>`;

    nodes += `
      <line x1="${nx}" y1="${ny}" x2="${mx}" y2="${my}" stroke="${inSixFamily ? 'rgba(243,146,55,0.40)' : trackStroke}" stroke-width="${inSixFamily ? 1.4 : 1}"/>
      <g style="cursor:${templateMode ? 'grab' : 'pointer'}" onpointerdown="onCofPointerDown(event, ${it.pc}, false, ${i}, false)">
        <circle cx="${mx}" cy="${my}" r="${rMajNode}" fill="${majFill}" stroke="${majStroke}" stroke-width="${majorActive ? 2.4 : (inSixFamily ? 1.8 : 1.1)}"/>
        ${majTextMarkup}
      </g>
      <g style="cursor:${templateMode ? 'grab' : 'pointer'}" onpointerdown="onCofPointerDown(event, ${it.pc}, true, ${i}, false)">
        <circle cx="${nx}" cy="${ny}" r="${rMinNode}" fill="${minFill}" stroke="${minStroke}" stroke-width="${minorActive ? 2.2 : (inSixFamily ? 1.5 : 1.0)}"/>
        ${minTextMarkup}
      </g>`;
  });

  const scaleNotesCompact = info.notes.map(n => n.name).join(' · ');
  const centerSub1 = templateMode
    ? t('SZABLON (6 DŹW.)', 'TEMPLATE (6 NOTES)')
    : (isMinorScale ? t('Mol naturalny', 'Natural Minor') : t('Dur (skala)', 'Major (scale)'));

  return `
    <svg id="cofInteractiveSvg" width="316" height="316" viewBox="0 0 320 320" xmlns="http://www.w3.org/2000/svg"
         style="display:block;max-width:100%;touch-action:none;user-select:none;">
      <circle cx="${cx}" cy="${cy}" r="${rMajRing}" fill="none" stroke="${trackStroke}" stroke-width="1.2"/>
      <circle cx="${cx}" cy="${cy}" r="${rMinRing}" fill="none" stroke="${trackStroke}" stroke-width="1" stroke-dasharray="3 3"/>
      ${wedgeSvg}
      ${nodes}
      <g style="cursor:pointer" onclick="playCircleScaleNotes()" title="${t('Kliknij środek, aby usłyszeć dźwięki tej tonacji', 'Click center to hear the key notes')}">
        <circle cx="${cx}" cy="${cy}" r="51" fill="${nodeBg}" stroke="${templateMode ? '#f39237' : trackStroke}" stroke-width="${templateMode ? 1.8 : 1.2}"/>
        <text x="${cx}" y="${cy - 12}" fill="${templateMode ? '#f39237' : mutedCol}" font-size="8.2" font-weight="800" text-anchor="middle" letter-spacing="0.4">${centerSub1}</text>
        <text x="${cx}" y="${cy + 9}" fill="${textCol}" font-size="20" font-weight="900" text-anchor="middle">${info.centerName}</text>
        <text x="${cx}" y="${cy + 25}" fill="${mutedCol}" font-size="8.5" font-weight="700" text-anchor="middle">${scaleNotesCompact}</text>
      </g>
    </svg>`;
}

function buildCofNotesStripMarkup(ringIdx, isMinorScale, templateMode) {
  const info = getCofKeyNotesInfo(ringIdx, isMinorScale);
  const safeIdx = info.ringIdx;
  const prevIt = COF_RING_ITEMS[(safeIdx + 11) % 12];
  const curIt  = COF_RING_ITEMS[safeIdx];
  const nextIt = COF_RING_ITEMS[(safeIdx + 1) % 12];
  const sixSummary = isMinorScale
    ? `${prevIt.min} (iv) · ${curIt.min} (i) · ${nextIt.min} (v) · ${prevIt.maj} (♭VI) · ${curIt.maj} (♭III) · ${nextIt.maj} (♭VII)`
    : `${prevIt.maj} (IV) · ${curIt.maj} (I) · ${nextIt.maj} (V) · ${prevIt.min} (ii) · ${curIt.min} (vi) · ${nextIt.min} (iii)`;

  const noteBtns = info.notes.map(n => `
    <button class="chip-btn ${n.isRoot ? 'active' : ''}"
            style="display:flex;flex-direction:column;align-items:center;gap:1px;padding:6px 9px;min-width:42px;${!n.isRoot && n.isTriad ? 'border-color:rgba(243,146,55,0.65);' : ''}"
            onpointerdown="onCofNoteChipDown(${n.pc})"
            onpointerup="onCofNoteChipUp(${n.pc})"
            onpointerleave="onCofNoteChipUp(${n.pc})"
            title="${t('Przytrzymaj, aby zagrać i podświetlić na gryfie', 'Hold to play and highlight on fretboard')}">
      <span style="font-size:13px;font-weight:800;">${n.name}</span>
      <span style="font-size:9.5px;opacity:0.75;">${n.deg}${n.isTriad ? ' ★' : ''}</span>
    </button>`).join('');

  return `
    <div>
      <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:6px;margin-bottom:7px;">
        <div class="section-label" style="margin:0;">
          ${templateMode
            ? t('SZABLON 6 DŹWIĘKÓW — przeciągaj po kole, aby transponować', '6-NOTE TEMPLATE — drag around circle to transpose')
            : t('DŹWIĘKI KOŁA (SKALA I TRÓJDŹWIĘK ★) — kliknij, aby zagrać i zobaczyć na gryfie', 'CIRCLE NOTES (SCALE & TRIAD ★) — click to play & highlight')}
        </div>
        <div style="display:flex;gap:6px;flex-wrap:wrap;">
          <button class="chip-btn" style="padding:4px 9px;font-size:11px;" onclick="playCircleScaleNotes()">🔊 ${t('Zagraj dźwięki', 'Play notes')}</button>
          <button class="chip-btn ${state.chordShowScaleOnFretboard ? 'active' : ''}" style="padding:4px 9px;font-size:11px;"
                  onclick="state.chordShowScaleOnFretboard=!state.chordShowScaleOnFretboard;saveSettings();renderFretboardToolbarForActiveTab();drawFretboard();renderCofSvgOnly()">
            🎸 ${t('Gryf', 'Fretboard')}: ${state.chordShowScaleOnFretboard ? t('Wł.', 'On') : t('Wył.', 'Off')}
          </button>
        </div>
      </div>
      ${templateMode ? `<div style="font-size:11.5px;color:var(--accent);font-weight:700;margin-bottom:8px;">${sixSummary}</div>` : ''}
      <div style="display:flex;flex-wrap:wrap;gap:6px;">${noteBtns}</div>
    </div>`;
}

function renderCofSvgOnly() {
  const svgWrap = document.getElementById('cofSvgWrap');
  const notesWrap = document.getElementById('cofNotesStripWrap');
  if (!svgWrap) return;
  const dragging = cofDrag.isTemplateDrag;
  const isMinor = dragging
    ? cofDrag.previewIsMinor
    : (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
  let activeIdx = 0;
  if (dragging) {
    activeIdx = cofDrag.hoverIdx;
  } else {
    const activeMajorPc = isMinor ? ((state.chordScaleRootPc + 3) % 12) : state.chordScaleRootPc;
    activeIdx = Math.max(0, COF_RING_ITEMS.findIndex(it => it.pc === activeMajorPc));
  }
  const tplMode = dragging || !!state.cofTemplateMode;
  svgWrap.innerHTML = buildCofSvgMarkup(activeIdx, isMinor, tplMode, dragging ? cofDrag.currentAngleDeg : null);
  if (notesWrap) {
    notesWrap.innerHTML = buildCofNotesStripMarkup(activeIdx, isMinor, tplMode);
  }
  const bNotes = document.getElementById('cofModeNotesBtn');
  const bTpl = document.getElementById('cofModeTplBtn');
  if (bNotes) bNotes.classList.toggle('active', !tplMode);
  if (bTpl) bTpl.classList.toggle('active', tplMode);
}

function onCofPointerDown(ev, majorPc, isMinorNode, ringIdx, fromWedge = false) {
  if (ev.button !== undefined && ev.button !== 0) return;
  ev.preventDefault();
  if (cofDrag.holdTimer) { clearTimeout(cofDrag.holdTimer); cofDrag.holdTimer = null; }

  // Zawsze kotwiczymy początkową pozycję szablonu 6 dźwięków na AKTUALNIE WYBRANEJ tonacji
  // (kliknięcie obok toniki lub na wewnętrznym pierścieniu NIE przesuwa zakresu ani nie zmienia trybu Dur/Mol!)
  const curIsMinor = (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
  const curMajorPc = curIsMinor ? ((state.chordScaleRootPc + 3) % 12) : state.chordScaleRootPc;
  const curActiveIdx = Math.max(0, COF_RING_ITEMS.findIndex(it => it.pc === curMajorPc));

  const svgEl = document.getElementById('cofInteractiveSvg');
  let grabOffsetDeg = 0;
  if (svgEl) {
    const rect = svgEl.getBoundingClientRect();
    const cx = rect.left + rect.width * 0.5;
    const cy = rect.top + rect.height * 0.5;
    const ptrDeg = Math.atan2(ev.clientY - cy, ev.clientX - cx) * (180 / Math.PI);
    grabOffsetDeg = (-90 + curActiveIdx * 30) - ptrDeg;
  }

  cofDrag.active = true;
  cofDrag.isTemplateDrag = !!fromWedge;
  cofDrag.moved = false;
  cofDrag.startX = ev.clientX;
  cofDrag.startY = ev.clientY;
  cofDrag.startMajorPc = majorPc;
  cofDrag.startIsMinor = !!isMinorNode;
  cofDrag.startRingIdx = ringIdx;
  cofDrag.fromWedge = !!fromWedge;
  cofDrag.grabOffsetDeg = grabOffsetDeg;
  cofDrag.hoverIdx = curActiveIdx;
  cofDrag.currentAngleDeg = -90 + curActiveIdx * 30;
  cofDrag.previewIsMinor = curIsMinor;
  cofDrag.previewRootPc = curIsMinor ? COF_RING_ITEMS[curActiveIdx].minPc : COF_RING_ITEMS[curActiveIdx].pc;
  cofDrag.svgEl = svgEl;

  if (fromWedge) {
    renderCofSvgOnly();
    drawFretboard();
  } else {
    // Przytrzymanie >= 240 ms zamienia koło w szablon 6 dźwięków (do przeciągania zakresu lub podglądu)
    cofDrag.holdTimer = setTimeout(() => {
      cofDrag.holdTimer = null;
      if (!cofDrag.active) return;
      cofDrag.isTemplateDrag = true;
      renderCofSvgOnly();
      drawFretboard();
    }, 240);
  }
}

function onCofGlobalPointerMove(ev) {
  if (!cofDrag.active) return;
  const dx = ev.clientX - cofDrag.startX;
  const dy = ev.clientY - cofDrag.startY;
  if (!cofDrag.isTemplateDrag && Math.hypot(dx, dy) >= 8) {
    if (cofDrag.holdTimer) { clearTimeout(cofDrag.holdTimer); cofDrag.holdTimer = null; }
    cofDrag.isTemplateDrag = true;
  }
  if (!cofDrag.isTemplateDrag) return;

  const svgEl = document.getElementById('cofInteractiveSvg') || cofDrag.svgEl;
  if (!svgEl) return;
  const rect = svgEl.getBoundingClientRect();
  const cx = rect.left + rect.width * 0.5;
  const cy = rect.top + rect.height * 0.5;
  const angleDeg = Math.atan2(ev.clientY - cy, ev.clientX - cx) * (180 / Math.PI) + (cofDrag.grabOffsetDeg || 0);
  const normFromTop = ((angleDeg + 90) % 360 + 360) % 360;
  const newIdx = Math.round(normFromTop / 30) % 12;

  // Zmiana tonacji między zewnętrznym (Dur) a wewnętrznym (Mol) pierścieniem następuje WYŁĄCZNIE przez przeciągnięcie!
  const scaleToViewBox = rect.width > 0 ? (320 / rect.width) : 1;
  const radialDist = Math.hypot(ev.clientX - cx, ev.clientY - cy) * scaleToViewBox;
  let newIsMinor = cofDrag.previewIsMinor;
  if (radialDist >= 56 && radialDist <= 154) {
    if (radialDist < 100) newIsMinor = true;
    else if (radialDist > 108) newIsMinor = false;
  }

  cofDrag.currentAngleDeg = angleDeg;
  if (newIdx !== cofDrag.hoverIdx || newIsMinor !== cofDrag.previewIsMinor) {
    cofDrag.moved = true;
    cofDrag.hoverIdx = newIdx;
    cofDrag.previewIsMinor = newIsMinor;
    const item = COF_RING_ITEMS[newIdx];
    cofDrag.previewRootPc = newIsMinor ? item.minPc : item.pc;
    // Delikatny dźwięk prymy przy przeskoku szablonu na kolejną tonację koła
    try {
      const m = 48 + cofDrag.previewRootPc + (cofDrag.previewRootPc < 4 ? 12 : 0);
      pluckGuitarFrequency(midiToFreq(m), 0.28);
    } catch (e) {}
    drawFretboard();
  }
  renderCofSvgOnly();
}

function onCofGlobalPointerUp() {
  if (!cofDrag.active) return;
  if (cofDrag.holdTimer) { clearTimeout(cofDrag.holdTimer); cofDrag.holdTimer = null; }

  const wasTemplate = cofDrag.isTemplateDrag;
  const wasMoved = cofDrag.moved;
  const targetIdx = cofDrag.hoverIdx;
  const targetIsMinor = cofDrag.previewIsMinor;
  const clickedIdx = cofDrag.startRingIdx;
  const clickedIsMinor = cofDrag.startIsMinor;
  const fromWedge = cofDrag.fromWedge;

  cofDrag.active = false;
  cofDrag.isTemplateDrag = false;
  cofDrag.moved = false;
  cofDrag.svgEl = null;

  if (!wasTemplate) {
    // KRÓTKIE WCIŚNIĘCIE: NIE zmieniaj zakresu 6 dźwięków ani tonacji na wewnętrzny mol — pokaż i zagraj kliknięty akord/dźwięki!
    state.cofTemplateMode = false;
    selectCircleChordInCurrentKey(clickedIdx, clickedIsMinor, true);
  } else if (wasMoved) {
    // PRZECIĄGNIĘCIE SZABLONU 6 DŹWIĘKÓW: przesuń zakres i zatwierdź nową tonację (zewn. Dur / wewn. Mol)!
    const targetItem = COF_RING_ITEMS[targetIdx];
    selectCircleKey(targetItem.pc, targetIsMinor, true);
  } else if (!fromWedge) {
    // SAMO PRZYTRZYMANIE W MIEJSCU (>= 240 ms): przełącz stały widok Szablonu 6 dźwięków bez zmiany tonacji
    state.cofTemplateMode = !state.cofTemplateMode;
    renderCofSvgOnly();
    drawFretboard();
  } else {
    renderCofSvgOnly();
    drawFretboard();
  }
}

window.addEventListener('pointermove', onCofGlobalPointerMove);
window.addEventListener('pointerup', onCofGlobalPointerUp);
window.addEventListener('pointercancel', onCofGlobalPointerUp);

function buildCircleOfFifthsPanel() {
  if (!state.diatonicData || !state.diatonicData.chords) return '';
  const isMinorScale = (state.chordScaleId === 'natural_minor' || state.chordScaleId === 'minor');
  const activeMajorPc = isMinorScale ? ((state.chordScaleRootPc + 3) % 12) : state.chordScaleRootPc;
  const activeIdx = Math.max(0, COF_RING_ITEMS.findIndex(it => it.pc === activeMajorPc));
  const activeItem = COF_RING_ITEMS[activeIdx];
  const centerName = isMinorScale ? activeItem.min : activeItem.maj;
  const scaleFullLbl = state.diatonicData.scaleFullName || '';

  // Kompaktowy pasek rozwijania / zwijania z lewej u góry bezpośrednio pod gryfem
  const toggleHeader = `
    <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:8px;">
      <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
        <button class="chip-btn ${state.chordShowCircle ? 'active' : ''}"
                style="padding:6px 13px;font-size:12px;font-weight:700;display:inline-flex;align-items:center;gap:6px;"
                onclick="state.chordShowCircle=!state.chordShowCircle;saveSettings();renderStageAndInspector()">
          <span>${state.chordShowCircle ? '▾' : '▸'}</span>
          <span>${state.chordShowCircle ? t('Zwiń Koło Kwintowe', 'Hide Circle of Fifths') : t('Rozwiń Koło Kwintowe', 'Show Circle of Fifths')}</span>
        </button>
        <span style="font-size:12px;color:var(--text-muted);">
          ${t('Tonacja', 'Key')}: <strong style="color:var(--accent);">${centerName}</strong> (${scaleFullLbl})
        </span>
      </div>
      ${state.chordShowCircle ? `
        <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
          <div class="ear-seg-row" style="width:auto;">
            <button id="cofModeNotesBtn" class="ear-seg-btn ${!state.cofTemplateMode ? 'active' : ''}" style="padding:4px 10px;font-size:11.5px;"
                    onclick="state.cofTemplateMode=false;renderStageAndInspector()">${t('Dźwięki koła', 'Circle Notes')}</button>
            <button id="cofModeTplBtn" class="ear-seg-btn ${state.cofTemplateMode ? 'active' : ''}" style="padding:4px 10px;font-size:11.5px;"
                    onclick="state.cofTemplateMode=true;renderStageAndInspector()">${t('Szablon (6 dźw.)', 'Template (6)')}</button>
          </div>
          <div class="ear-seg-row" style="width:auto;">
            <button class="ear-seg-btn ${!state.chordDiatonicSevenths ? 'active' : ''}" style="padding:4px 11px;font-size:11.5px;"
                    onclick="state.chordDiatonicSevenths=false;renderAll()">${t('Trójdźwięki', 'Triads')}</button>
            <button class="ear-seg-btn ${state.chordDiatonicSevenths ? 'active' : ''}" style="padding:4px 11px;font-size:11.5px;"
                    onclick="state.chordDiatonicSevenths=true;renderAll()">${t('Septymowe (7)', 'Sevenths (7)')}</button>
          </div>
        </div>
      ` : ''}
    </div>`;

  if (!state.chordShowCircle) {
    return toggleHeader;
  }

  const diatonic = state.diatonicData.chords;

  const diatonicChips = diatonic.map(c => {
    const isCurDia = (c.rootPc === state.rootPc && c.chordId === state.chordId);
    return `
    <button class="chip-btn ${isCurDia ? 'active' : ''}" style="display:flex;flex-direction:column;align-items:center;gap:2px;padding:8px 8px;min-width:58px;"
            onclick="selectDiatonicChord(${c.rootPc}, '${c.chordId}')" title="Załaduj chwyty akordu ${c.symbol}">
      <span style="font-size:12.5px;font-weight:800;color:${isCurDia ? '#14100d' : 'var(--accent)'};">${c.roman}</span>
      <span style="font-size:11px;font-weight:700;">${c.symbol}</span>
    </button>`;
  }).join('');

  // Poprawne progresje zależne od trybu (Dur vs Mol — bez duplikowania I-IV-V jako i-iv-v!)
  const progressions = isMinorScale ? [
    { name: 'i–iv–v',          deg: [0, 3, 4] },
    { name: 'i–♭VI–♭III–♭VII', deg: [0, 5, 2, 6] },
    { name: 'i–♭VII–♭VI–♭VII', deg: [0, 6, 5, 6] },
    { name: 'ii°–v–i',         deg: [1, 4, 0] },
    { name: 'i–iv–♭VII–♭III',  deg: [0, 3, 6, 2] },
    { name: 'i–♭VI–iv–v',      deg: [0, 5, 3, 4] }
  ] : [
    { name: 'I–IV–V',          deg: [0, 3, 4] },
    { name: 'I–V–vi–IV',       deg: [0, 4, 5, 3] },
    { name: 'ii–V–I',          deg: [1, 4, 0] },
    { name: 'I–vi–IV–V',       deg: [0, 5, 3, 4] },
    { name: 'vi–IV–I–V',       deg: [5, 3, 0, 4] },
    { name: 'I–IV–vi–V',       deg: [0, 3, 5, 4] }
  ];

  const progChips = progressions.map(p => {
    const syms = p.deg.map(d => (diatonic[d] ? diatonic[d].symbol : '')).join(' · ');
    return `<button class="chip-btn" style="display:flex;flex-direction:column;align-items:center;gap:2px;padding:8px 11px;"
                    onclick="sendProgressionToChanges([${p.deg.join(',')}])" title="Wyślij „${p.name}” do zakładki Zmiany">
      <span style="font-size:12px;font-weight:800;">${p.name}</span>
      <span style="font-size:10.5px;color:var(--text-muted);">${syms}</span>
    </button>`;
  }).join('');

  return `
    ${toggleHeader}
    <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:12px;padding:14px 18px;display:flex;gap:22px;align-items:center;flex-wrap:wrap;">
      <div style="display:flex;flex-direction:column;align-items:center;gap:6px;flex-shrink:0;">
        <div id="cofSvgWrap">${buildCofSvgMarkup(activeIdx, isMinorScale, !!state.cofTemplateMode, null)}</div>
        <div style="font-size:10.5px;color:var(--text-muted);text-align:center;max-width:316px;line-height:1.4;">
          ${t('Kliknij: załaduj i zagraj akord (bez zmiany zakresu) · Przytrzymaj i przeciągnij: przesuń szablon 6 dźwięków (zewn. Dur / wewn. Mol)', 'Click: load & play chord (keeps 6-note range) · Hold & drag: move 6-note template (outer Maj / inner Min)')}
        </div>
      </div>
      <div style="flex:1;min-width:290px;display:flex;flex-direction:column;gap:14px;justify-content:center;">
        <div id="cofNotesStripWrap">${buildCofNotesStripMarkup(activeIdx, isMinorScale, !!state.cofTemplateMode)}</div>
        <div>
          <div class="section-label" style="margin:0 0 7px;">${t('AKORDY W SKALI — kliknij, aby załadować chwyty', 'CHORDS IN KEY — click to load voicings')}</div>
          <div style="display:flex;flex-wrap:wrap;gap:6px;">${diatonicChips}</div>
        </div>
        <div>
          <div class="section-label" style="margin:0 0 7px;">${t('SUGESTIE PROGRESJI — wyślij do „Zmiany”', 'PROGRESSION IDEAS — send to Changes')}</div>
          <div style="display:flex;flex-wrap:wrap;gap:6px;">${progChips}</div>
        </div>
      </div>
    </div>`;
}

function setRootPc(pc) { stopCurrentScalePreview(false); state.rootPc = pc; state.selectedScaleNotePc = pc; saveSettings(); renderAll(); }
// Znaki chromatyczne (bemole / krzyżyki) przełączamy w panelu „Strój i Gryf” (rozwijane ustawienia gryfu u góry).
function setAccMode(m) { state.accMode = m; saveSettings(); renderTuningModalControls(); updateTopTuningLabel(); renderAll(); }

// Generuje czysty graficzny diagram 6-liniowy SVG dla karty akordu (z obsługą kapodastra!)
function buildChordSvgDiagram(frets, tapped, capo = 0) {
  const W = 136, H = 76;
  const leftX = 8, rightX = 128;
  const topY = 10, rowGap = 11;
  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg">`;

  // 6 poziomych linii strun (góra = struna 1 E4, dół = struna 6 E2)
  for (let r = 0; r < 6; r++) {
    const y = topY + r * rowGap;
    svg += `<line x1="${leftX}" y1="${y}" x2="${rightX}" y2="${y}" stroke="#8c857b" stroke-width="1.2"/>`;
  }
  // Pionowe zamknięcia po lewej i prawej stronie (gdy jest kapodaster, lewa belka świeci jak poprzeczka kapo)
  if (capo > 0) {
    svg += `<line x1="${leftX}" y1="${topY - 3}" x2="${leftX}" y2="${topY + 5 * rowGap + 3}" stroke="#f39237" stroke-width="3.2" stroke-linecap="round"/>`;
  } else {
    svg += `<line x1="${leftX}" y1="${topY}" x2="${leftX}" y2="${topY + 5 * rowGap}" stroke="#8c857b" stroke-width="1.3"/>`;
  }
  svg += `<line x1="${rightX}" y1="${topY}" x2="${rightX}" y2="${topY + 5 * rowGap}" stroke="#8c857b" stroke-width="1.3"/>`;

  // Numery progów (np. T12, T13, 17, 0 lub próg kapodastra na złoto) na odpowiednich liniach strun
  for (let s = 0; s < 6; s++) {
    const f = frets[s];
    if (f === undefined || f < 0) continue;
    const r = 5 - s;
    const y = topY + r * rowGap;
    const isCapoStr = (capo > 0 && f === capo);
    const label = (tapped && tapped[s] ? 'T' : '') + String(f);
    const boxW = label.length * 7 + 6;
    const cx = 26;
    svg += `<rect x="${cx - boxW/2}" y="${y - 6}" width="${boxW}" height="12" rx="2" fill="${isCapoStr ? '#59290a' : '#191512'}"/>`;
    svg += `<text x="${cx}" y="${y + 3.5}" fill="${isCapoStr ? '#fbbf24' : '#f5f2eb'}" font-family="Segoe UI, sans-serif" font-size="11" font-weight="700" text-anchor="middle">${label}</text>`;
  }

  svg += `</svg>`;
  return svg;
}

async function renderStageAndInspector() {
  const stage = document.getElementById('dynamicStagePanel');
  const insp = document.getElementById('rightInspector');
  if (!state.initData) return;

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
      <div class="tuner-stage-card" style="gap:16px;padding:24px;">
        <canvas id="tunerArcCanvas" width="820" height="450" style="width:min(820px, 100%);height:auto;max-height:56vh;"></canvas>
        <div id="tunerNoteReadout" style="font-size:20px;font-weight:700;color:var(--text-muted);">${t('Zagraj strunę...', 'Play a string...')}</div>
        <div class="tuner-strings-row">
          ${openStrings.map(st => `
            <div class="tuner-string-btn ${state.tunerLockedString === st.idx ? 'active' : ''}"
                 onmousedown="toggleTunerStringLock(event, ${st.idx}, ${st.midi})">
              <span style="font-size:13px;color:var(--text-muted);font-weight:600;">${st.num}</span>
              <span style="font-size:24px;font-weight:800;">${st.name}</span>
            </div>
          `).join('')}
        </div>
        <div style="font-size:13px;color:var(--text-muted);">${t('Dotknij i przytrzymaj strunę, żeby zablokować na niej tuner i odsłuchać wzorzec.', 'Click and hold a string to lock the tuner and hear the reference tone.')}</div>
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
        const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
        const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
        const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
        const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
            <div style="width:94px;height:94px;border-radius:50%;background:var(--bg-card);border:2px solid ${accuracyPct >= 70 ? 'var(--green)' : 'var(--border)'};display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;">
              <div style="font-size:23px;font-weight:800;color:var(--text);">${accuracyPct}%</div>
              <div style="font-size:10.5px;color:var(--text-muted);">${t('Dokładność', 'Accuracy')}</div>
            </div>

            <div style="text-align:center;">
              <h2 style="font-size:28px;font-weight:800;margin-bottom:6px;">${headline}</h2>
              <div style="font-size:13px;color:var(--text-muted);">${fullSub}</div>
            </div>

            <!-- 3 czyste kafelki statystyk sesji (bez grywalizacji i XP) -->
            <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:10px;width:100%;max-width:480px;">
              <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earScore}/${totalRounds}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">${t('Wynik', 'Score')}</div>
              </div>
              <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earSessionDurationSec || 30}s</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">${t('Czas', 'Time')}</div>
              </div>
              <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:10px;padding:12px 10px;text-align:center;">
                <div style="font-size:20px;font-weight:800;">${state.earBestStreak || 0}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:3px;">${t('Najlepsza seria', 'Best streak')}</div>
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
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
                <button class="action-btn" onclick="launchWorkshopSession(${idx})">▶ Graj</button>
                <button class="chip-btn" style="padding:6px 10px;" onclick="deleteWorkshopSession(${idx})">🗑️</button>
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
  // WIDOK: USTAWIENIA (Wygląd, Język, Notacja, Interfejs, Audio, Harmonia — bez grywalizacji)
  // =========================================================================
  if (state.mainView === 'settings') {
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

    const retView = state.settingsReturnView;
    const retLabel = retView ? getViewDisplayName(retView.mainView, retView.subtab) : t('Praktyka', 'Practice');
    const skillDesc = state.skillLevel === 'beginner'
      ? t('Szersza tolerancja detekcji intonacji (±45 ct)', 'Wider pitch detection tolerance (±45 ct)')
      : (state.skillLevel === 'intermediate'
          ? t('Standardowa detekcja intonacji (±30 ct)', 'Standard pitch detection (±30 ct)')
          : t('Tryb studyjny, precyzyjna ocena intonacji (±15 ct)', 'Studio mode, strict pitch detection (±15 ct)'));
    const curTheme = state.themeMode || 'dark';
    const curLang = state.uiLang || 'pl';
    const curFretMarkers = state.fretMarkersMode || 'numbers';
    const synthVolPct = Math.round(((state.synthVolume !== undefined) ? state.synthVolume : 0.85) * 100);
    const metroVolPct = Math.round(((state.metronomeVolume !== undefined) ? state.metronomeVolume : 0.75) * 100);

    stage.innerHTML = `
      <div class="fretboard-card" style="padding:22px;gap:20px;">
        <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:12px;">
          <div style="display:flex;align-items:center;gap:12px;flex-wrap:wrap;">
            <button class="settings-back-btn" onclick="returnFromSettings()" title="${t('Wróć do poprzedniego widoku', 'Return to previous view')}">
              ← ${t('Wróć', 'Back')}${retView ? `: ${retLabel}` : ''}
            </button>
            <div>
              <h2 style="font-size:22px;font-weight:800;">${t('Ustawienia', 'Settings')}</h2>
              <div style="font-size:12px;color:var(--text-muted);margin-top:2px;">${t('Wszystkie zmiany zapisują się automatycznie.', 'All changes are saved automatically.')}</div>
            </div>
          </div>
          <button class="chip-btn" style="padding:6px 12px;font-size:12px;" onclick="resetAppSettings()">
            ↺ ${t('Przywróć domyślne', 'Reset defaults')}
          </button>
        </div>

        <!-- SEKCJA 1: WYGLĄD I NOTACJA (Zdjęcie 2) -->
        <h3 style="font-size:17px;font-weight:800;border-bottom:1px solid var(--border);padding-bottom:8px;">${t('Wygląd i notacja', 'Appearance & Notation')}</h3>
        <div style="display:grid;grid-template-columns:1fr 1fr;gap:28px;">
          <div style="display:flex;flex-direction:column;gap:18px;">
            <div>
              <div class="section-label">${t('Motyw', 'Theme')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${curTheme==='dark'?'active':''}" style="padding:7px 6px;font-size:12px;" onclick="setThemeMode('dark')">${t('Ciemny', 'Dark')}</button>
                <button class="chip-btn ${curTheme==='light'?'active':''}" style="padding:7px 6px;font-size:12px;" onclick="setThemeMode('light')">${t('Jasny', 'Light')}</button>
                <button class="chip-btn ${curTheme==='system'?'active':''}" style="padding:7px 6px;font-size:12px;" onclick="setThemeMode('system')">${t('Systemowy', 'System')}</button>
              </div>
            </div>

            <div>
              <div class="section-label">${t('Styl wizualny aplikacji', 'Application Visual Style')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${(state.uiStyle||'classic')==='classic'?'active':''}" style="padding:7px 6px;font-size:11.5px;" onclick="setUiStyle('classic')">${t('Klasyczny Studio', 'Classic Studio')}</button>
                <button class="chip-btn ${state.uiStyle==='modern'?'active':''}" style="padding:7px 6px;font-size:11.5px;" onclick="setUiStyle('modern')">Neo-Glass Studio</button>
                <button class="chip-btn ${state.uiStyle==='rack'?'active':''}" style="padding:7px 6px;font-size:11.5px;" onclick="setUiStyle('rack')">Hardware Amp Rack</button>
              </div>
              <div style="font-size:11px;color:var(--text-muted);margin-top:4px;">${t('Zmienia wygląd całej aplikacji (panele, przyciski, typografię i gryf) zarówno w trybie Ciemnym, jak i Jasnym.', 'Changes the entire app appearance (panels, controls, typography, fretboard) in both Dark and Light mode.')}</div>
            </div>

            <div>
              <div class="section-label">${t('Znaczniki progów', 'Fret Markers')}</div>
              <div class="ear-seg-row">
                <button class="ear-seg-btn ${curFretMarkers==='dots'?'active':''}"
                        onclick="state.fretMarkersMode='dots';saveSettings();drawFretboard();renderStageAndInspector()">${t('Kropki', 'Dots')}</button>
                <button class="ear-seg-btn ${curFretMarkers==='numbers'?'active':''}"
                        onclick="state.fretMarkersMode='numbers';saveSettings();drawFretboard();renderStageAndInspector()">${t('Numery', 'Numbers')}</button>
              </div>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;">
              <div>
                <div style="font-size:13.5px;font-weight:700;">${t('Numeruj wszystkie progi', 'Number all frets')}</div>
                <div style="font-size:11.5px;color:var(--text-muted);margin-top:2px;">${t('Wyświetl numer nad/pod każdym progiem od 1 do 24', 'Display numbers on every fret from 1 to 24')}</div>
              </div>
              <button class="ear-toggle-switch ${state.numberAllFrets?'on':''}"
                      onclick="state.numberAllFrets=!state.numberAllFrets;saveSettings();drawFretboard();renderStageAndInspector()"></button>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;">
              <div>
                <div style="font-size:13.5px;font-weight:700;">${t('Margines podstrunnicy', 'Fretboard margin')}</div>
                <div style="font-size:11.5px;color:var(--text-muted);margin-top:2px;">${t('Pokaż ±2 dodatkowe progi wokół zakresu ćwiczenia', 'Show ±2 extra frets around the exercise range')}</div>
              </div>
              <button class="ear-toggle-switch ${state.fretboardMargin!==false?'on':''}"
                      onclick="state.fretboardMargin=!(state.fretboardMargin!==false);saveSettings();renderStageAndInspector()"></button>
            </div>
          </div>

          <div style="display:flex;flex-direction:column;gap:18px;">
            <div>
              <div class="section-label">${t('Język', 'Language')}</div>
              <select onchange="setUiLanguage(this.value)">
                <option value="pl" ${curLang==='pl'?'selected':''}>Polski</option>
                <option value="en" ${curLang==='en'?'selected':''}>English</option>
              </select>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;">
              <div>
                <div class="section-label" style="margin:0 0 3px;">${t('Notacja', 'Notation')}</div>
                <div style="font-size:13.5px;font-weight:700;">${t('Pokaż tabulaturę (TAB)', 'Show tablature (TAB)')}</div>
              </div>
              <button class="ear-toggle-switch ${state.tabCreationEnabled?'on':''}"
                      onclick="state.tabCreationEnabled=!state.tabCreationEnabled;if(!state.tabCreationEnabled)state.exerciseShowStaff=true;saveSettings();renderStageAndInspector()"></button>
            </div>

            <div>
              <div class="section-label">${t('Domyślny klucz', 'Default clef')}</div>
              <div class="ear-seg-row">
                <button class="ear-seg-btn ${state.exerciseClef==='treble'?'active':''}"
                        onclick="state.exerciseClef='treble';saveSettings();renderStageAndInspector()">${t('Wiolinowy', 'Treble')}</button>
                <button class="ear-seg-btn ${state.exerciseClef==='bass'?'active':''}"
                        onclick="state.exerciseClef='bass';saveSettings();renderStageAndInspector()">${t('Basowy', 'Bass')}</button>
                <button class="ear-seg-btn ${state.exerciseClef==='auto'?'active':''}"
                        onclick="state.exerciseClef='auto';saveSettings();renderStageAndInspector()">Auto</button>
              </div>
            </div>

            <div>
              <div class="section-label">${t('Skalowanie interfejsu (Full HD / 2K)', 'Interface Scaling (Full HD / 2K)')}</div>
              <div class="ear-seg-row">
                <button id="settingsUiScaleBtn_auto" class="ear-seg-btn ${(state.uiScaleMode||'auto')==='auto'?'active':''}"
                        onclick="setUiScaleMode('auto')">Auto</button>
                <button id="settingsUiScaleBtn_fhd" class="ear-seg-btn ${state.uiScaleMode==='fhd'?'active':''}"
                        onclick="setUiScaleMode('fhd')">Full HD</button>
                <button id="settingsUiScaleBtn_2k" class="ear-seg-btn ${state.uiScaleMode==='2k'?'active':''}"
                        onclick="setUiScaleMode('2k')">2K (100%)</button>
              </div>
              <div id="settingsUiScaleInfo" style="font-size:11.5px;color:var(--accent);font-weight:600;margin-top:6px;">
                ${(() => {
                  const info = computeUiScaleProfile();
                  const dpr = Math.round((window.devicePixelRatio || 1) * 100);
                  return `${t('Aktywny profil', 'Active profile')}: ${info.profile === '2k' ? '2K' : 'FHD'} (${Math.round(info.zoom * 100)}%) · ${info.vw}×${info.vh} px (DPI: ${dpr}%)`;
                })()}
              </div>
            </div>
          </div>
        </div>

        <!-- SEKCJA 2: URZĄDZENIA AUDIO, MIKROFON I GŁOŚNOŚĆ -->
        <h3 style="font-size:17px;font-weight:800;border-bottom:1px solid var(--border);padding-bottom:8px;">${t('Audio, mikrofon i głośność', 'Audio, Microphone & Volume')}</h3>
        <div style="display:grid;grid-template-columns:1fr 1fr;gap:28px;">
          <div style="display:flex;flex-direction:column;gap:12px;">
            <div>
              <div class="section-label">${t('Wejście (mikrofon / interfejs)', 'Input (microphone / interface)')}</div>
              <select id="audioInputSelect" onchange="onAudioInputChanged(this.value)">
                <option value="default">${t('Domyślne wejście systemowe', 'System default input')}</option>
              </select>
            </div>
            <div>
              <div class="section-label">${t('Odtwarzanie (głośniki / słuchawki)', 'Output (speakers / headphones)')}</div>
              <select id="audioOutputSelect" onchange="onAudioOutputChanged(this.value)">
                <option value="default">${t('Domyślne urządzenie odtwarzania', 'System default output')}</option>
              </select>
            </div>
            <button class="chip-btn" style="width:fit-content;padding:6px 14px;" onclick="refreshAudioDevices(true)">🔄 ${t('Wykryj urządzenia', 'Refresh devices')}</button>
            <div style="margin-top:4px;padding:12px;border-radius:10px;background:var(--bg-card);border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
              <div class="section-label" style="margin:0;">${t('Detekcja polifoniczna (PitchPlease WASM)', 'Polyphonic detection (PitchPlease WASM)')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${(state.dspModel||'fast')==='fast'?'active':''}" onclick="setDspModel('fast')">${t('Wzmacniacz / Fast', 'Amp / Fast')}</button>
                <button class="chip-btn ${state.dspModel==='voice'?'active':''}" onclick="setDspModel('voice')">${t('Gitara / Głos', 'Guitar / Voice')}</button>
                <button class="chip-btn ${state.dspModel==='piano'?'active':''}" onclick="setDspModel('piano')">${t('Akustyk / Clean', 'Acoustic / Clean')}</button>
              </div>
              <button class="chip-btn ${(state.dspAmpPreEq !== false)?'active':''}" style="width:100%;padding:6px 10px;font-size:11.5px;text-align:left;" onclick="toggleDspAmpPreEq()">
                ${(state.dspAmpPreEq !== false)
                  ? t('🔊 Filtr wzmacniacza/mikrofonu (EQ + wzmocnienie cienkich strun): WŁ', '🔊 Amp/Mic EQ + thin-string boost: ON')
                  : t('🔇 Filtr wzmacniacza/mikrofonu (surowy sygnał): WYŁ', '🔇 Amp/Mic EQ (raw input): OFF')}
              </button>
              <div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;">
                <div>
                  <div style="display:flex;justify-content:space-between;font-size:11px;margin-bottom:3px;">
                    <span style="color:var(--text-muted);">${t('Próg grubych (E6–D4)', 'Thick strings (E6–D4)')}</span>
                    <strong id="dspThresholdOnLbl" style="color:var(--accent);">${Math.round((state.dspThresholdOn || 0.58) * 100)}%</strong>
                  </div>
                  <input type="range" min="40" max="78" step="2" value="${Math.round((state.dspThresholdOn || 0.58) * 100)}" style="width:100%;"
                         oninput="const v=Math.max(40,Math.min(78,parseInt(this.value,10)||58));state.dspThresholdOn=v/100;const lbl=document.getElementById('dspThresholdOnLbl');if(lbl)lbl.textContent=v+'%';if(livePitchPleaseDetector)livePitchPleaseDetector.updateConfig({thresholdOn:state.dspThresholdOn});"
                         onchange="setDspThresholdOn(parseInt(this.value,10)/100)">
                </div>
                <div>
                  <div style="display:flex;justify-content:space-between;font-size:11px;margin-bottom:3px;">
                    <span style="color:var(--text-muted);">${t('Próg cienkich (G3–E1)', 'Thin strings (G3–E1)')}</span>
                    <strong id="dspHighThresholdOnLbl" style="color:var(--accent);">${Math.round((state.dspHighThresholdOn || 0.42) * 100)}%</strong>
                  </div>
                  <input type="range" min="30" max="72" step="2" value="${Math.round((state.dspHighThresholdOn || 0.42) * 100)}" style="width:100%;"
                         oninput="const v=Math.max(30,Math.min(72,parseInt(this.value,10)||42));state.dspHighThresholdOn=v/100;const lbl=document.getElementById('dspHighThresholdOnLbl');if(lbl)lbl.textContent=v+'%';if(livePitchPleaseDetector)livePitchPleaseDetector.updateConfig({highThresholdOn:state.dspHighThresholdOn});"
                         onchange="setDspHighThresholdOn(parseInt(this.value,10)/100)">
                </div>
              </div>
              <div style="display:flex;align-items:center;gap:10px;flex-wrap:wrap;">
                <button class="chip-btn" style="padding:6px 12px;font-weight:700;" ${state.dspCalibrating ? 'disabled' : ''} onclick="startGuitarMicCalibration()">
                  🎸 ${state.dspCalibrating ? t('Kalibracja w toku...', 'Calibrating...') : t('Kalibruj z gitary (5s)', 'Calibrate from guitar (5s)')}
                </button>
                <span id="dspCalibStatusLabel" style="font-size:11.5px;color:${state.dspCalibrated ? '#4ade80' : 'var(--text-muted)'};">
                  ${state.dspCalibStatus || (state.dspCalibrated ? t('Skalibrowano pod Twoją gitarę', 'Calibrated for your guitar') : t('Samoadaptacja Zero-Calibration aktywna (opcjonalna kalibracja 5s)', 'Zero-Calibration auto-adapt active (optional 5s calibration)'))}
                </span>
              </div>
            </div>
          </div>
          <div style="display:flex;flex-direction:column;gap:12px;">
            <div>
              <div style="display:flex;justify-content:space-between;font-size:12px;margin-bottom:4px;">
                <span class="section-label" style="margin:0;">${t('Głośność syntezatora', 'Synth volume')}</span>
                <strong id="synthVolLbl" style="color:var(--accent);">${synthVolPct}%</strong>
              </div>
              <input type="range" min="10" max="100" step="5" value="${synthVolPct}" style="width:100%;"
                     oninput="const p=parseInt(this.value,10)||85;state.synthVolume=p/100;const l=document.getElementById('synthVolLbl');if(l)l.textContent=p+'%';"
                     onchange="state.synthVolume=parseInt(this.value,10)/100;saveSettings();">
            </div>
            <div>
              <div style="display:flex;justify-content:space-between;font-size:12px;margin-bottom:4px;">
                <span class="section-label" style="margin:0;">${t('Głośność metronomu', 'Metronome volume')}</span>
                <strong id="metroVolLbl" style="color:var(--accent);">${metroVolPct}%</strong>
              </div>
              <input type="range" min="10" max="100" step="5" value="${metroVolPct}" style="width:100%;"
                     oninput="const p=parseInt(this.value,10)||75;state.metronomeVolume=p/100;const l=document.getElementById('metroVolLbl');if(l)l.textContent=p+'%';"
                     onchange="state.metronomeVolume=parseInt(this.value,10)/100;saveSettings();">
            </div>
            <button id="settingsToggleMicBtn" class="action-btn" style="width:100%;padding:10px;font-size:13px;background:${micStream ? 'var(--red)' : 'var(--accent)'};color:${micStream ? '#ffffff' : '#120e0a'};" onclick="toggleLiveAudio()">
              ${micStream ? t('🔇 Wyłącz mikrofon', '🔇 Turn off microphone') : t('🎤 Włącz mikrofon', '🎤 Turn on microphone')}
            </button>
          </div>
        </div>

        <!-- SEKCJA 3: PRAKTYKA I HARMONIA -->
        <h3 style="font-size:17px;font-weight:800;border-bottom:1px solid var(--border);padding-bottom:8px;">${t('Praktyka i harmonia', 'Practice & Harmony')}</h3>
        <div style="display:grid;grid-template-columns:1fr 1fr;gap:28px;">
          <div style="display:flex;flex-direction:column;gap:16px;">
            <div>
              <div class="section-label">${t('Tolerancja detekcji / Poziom', 'Detection tolerance / Level')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.skillLevel==='beginner'?'active':''}"
                        onclick="state.skillLevel='beginner';saveSettings();renderStageAndInspector()">${t('Początkujący', 'Beginner')}</button>
                <button class="chip-btn ${state.skillLevel==='intermediate'?'active':''}"
                        onclick="state.skillLevel='intermediate';saveSettings();renderStageAndInspector()">${t('Średni', 'Intermediate')}</button>
                <button class="chip-btn ${state.skillLevel==='advanced'?'active':''}"
                        onclick="state.skillLevel='advanced';saveSettings();renderStageAndInspector()">${t('Zaawansowany', 'Advanced')}</button>
              </div>
              <div style="font-size:12px;color:var(--text-muted);margin-top:6px;">${skillDesc}</div>
            </div>

            <div>
              <div class="section-label">${t('Liczba progów na gryfie', 'Frets on fretboard')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.maxDisplayedFret===21?'active':''}" onclick="setMaxFrets(21)">21</button>
                <button class="chip-btn ${state.maxDisplayedFret===22?'active':''}" onclick="setMaxFrets(22)">22</button>
                <button class="chip-btn ${state.maxDisplayedFret===24?'active':''}" onclick="setMaxFrets(24)">24</button>
              </div>
            </div>

            <div>
              <div class="section-label">${t('Dominanty rozwiązujące się', 'Resolving dominants')}</div>
              <div class="btn-grid-3">
                <button class="chip-btn ${state.resolvingDominants==='inside'?'active':''}"
                        onclick="state.resolvingDominants='inside';saveSettings();renderStageAndInspector()">${t('Wewnątrz', 'Inside')}</button>
                <button class="chip-btn ${state.resolvingDominants==='outside'?'active':''}"
                        onclick="state.resolvingDominants='outside';saveSettings();renderStageAndInspector()">${t('Na zewnątrz', 'Outside')}</button>
                <button class="chip-btn ${state.resolvingDominants==='custom'?'active':''}"
                        onclick="state.resolvingDominants='custom';saveSettings();renderStageAndInspector()">${t('Własne', 'Custom')}</button>
              </div>
            </div>

            <div>
              <div class="section-label">${t('Dominanty alterowane (♯5, ♭13)', 'Altered dominants (♯5, ♭13)')}</div>
              <div style="display:flex;gap:6px;flex-wrap:wrap;">
                ${[
                  {id:'auto', name:'Auto'},
                  {id:'mixolydian_b6', name:t('Miksolidyjska ♭6', 'Mixolydian ♭6')},
                  {id:'whole_tone', name:t('Całotonowa', 'Whole Tone')},
                  {id:'altered', name:t('Alterowana', 'Altered')},
                  {id:'messiaen_3', name:'Messiaen 3'}
                ].map(opt => `
                  <button class="chip-btn ${state.alteredDominantScale===opt.id?'active':''}" style="padding:7px 11px;"
                          onclick="state.alteredDominantScale='${opt.id}';saveSettings();renderStageAndInspector()">${opt.name}</button>
                `).join('')}
              </div>
            </div>
          </div>

          <div style="display:flex;flex-direction:column;gap:14px;">
            <div>
              <div class="section-label">${t('Brzmienie akordów alterowanych', 'Altered chords voicing')}</div>
              <div style="display:flex;align-items:center;gap:8px;margin-bottom:8px;font-size:12px;">
                <span style="color:var(--text-muted);">${t('Wszystkie:', 'All:')}</span>
                <button class="chip-btn" style="padding:5px 12px;" onclick="setAllAlteredIntensity('light')">${t('Lekko', 'Light')}</button>
                <button class="chip-btn" style="padding:5px 12px;" onclick="setAllAlteredIntensity('strong')">${t('Mocno', 'Strong')}</button>
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
                                onclick="setAlteredIntensity('${item.id}','light')">${t('Lekko', 'Light')}</button>
                        <button class="chip-btn ${cur==='strong'?'active':''}" style="padding:5px 10px;font-size:12px;"
                                onclick="setAlteredIntensity('${item.id}','strong')">${t('Mocno', 'Strong')}</button>
                      </div>
                    </div>`;
                }).join('')}
              </div>
              <button class="chip-btn" style="width:100%;margin-top:8px;"
                      onclick="state.showMoreAlteredTypes=!state.showMoreAlteredTypes;renderStageAndInspector()">
                ${state.showMoreAlteredTypes ? t('− Ukryj dodatkowe typy', '− Hide extra types') : t('+ Więcej typów (5)', '+ More types (5)')}
              </button>
            </div>

            <div style="display:flex;justify-content:space-between;align-items:center;gap:12px;border-top:1px solid var(--border);padding-top:12px;">
              <div>
                <div style="font-size:13px;font-weight:700;">${t('Ruchome głosy', 'Moving inner voices')}</div>
                <div style="font-size:11px;color:var(--text-muted);margin-top:2px;">${t('Prowadzenie głosów wewnętrznych na długich akordach w podkładzie.', 'Voice leading on sustained chords in backing tracks.')}</div>
              </div>
              <button class="ear-toggle-switch ${state.movingVoices?'on':''}"
                      onclick="state.movingVoices=!state.movingVoices;saveSettings();renderStageAndInspector()"></button>
            </div>
          </div>
        </div>
      </div>`;

    insp.innerHTML = `
      <div>
        <div class="section-label">${t('DOMYŚLNY STRÓJ GITARY', 'DEFAULT TUNING')}</div>
        <select onchange="selectPresetTuning(this.value)">
          ${state.initData.tunings.map(tt => `<option value="${tt.id}" ${tt.id===state.tuningId?'selected':''}>${tt.name}</option>`).join('')}
          <option value="custom" ${state.tuningId==='custom'?'selected':''}>${t('Własny strój', 'Custom tuning')}</option>
        </select>
      </div>
      <div>
        <div class="section-label">${t('ZNAKI CHROMATYCZNE', 'ACCIDENTALS')}</div>
        <div class="btn-grid-2">
          <button class="chip-btn ${state.accMode===1?'active':''}" onclick="setAccMode(1);saveSettings()">${t('Bemole (♭)', 'Flats (♭)')}</button>
          <button class="chip-btn ${state.accMode===2?'active':''}" onclick="setAccMode(2);saveSettings()">${t('Krzyżyki (♯)', 'Sharps (♯)')}</button>
        </div>
      </div>
      <div>
        <div class="section-label">${t('KALIBRACJA STROIKA A4 (Hz)', 'TUNER REFERENCE A4 (Hz)')}</div>
        <div class="btn-grid-3" style="margin-bottom:6px;">
          <button class="chip-btn ${Math.abs(state.a4ReferenceHz-432)<0.1?'active':''}" onclick="state.a4ReferenceHz=432;saveSettings();renderStageAndInspector()">432 Hz</button>
          <button class="chip-btn ${Math.abs(state.a4ReferenceHz-440)<0.1?'active':''}" onclick="state.a4ReferenceHz=440;saveSettings();renderStageAndInspector()">440 Hz</button>
          <button class="chip-btn ${Math.abs(state.a4ReferenceHz-442)<0.1?'active':''}" onclick="state.a4ReferenceHz=442;saveSettings();renderStageAndInspector()">442 Hz</button>
        </div>
        <input type="number" value="${state.a4ReferenceHz}" min="430" max="450" step="0.5"
               onchange="state.a4ReferenceHz=parseFloat(this.value)||440;saveSettings();renderStageAndInspector()">
      </div>`;
    populateAudioSelectElements();
    return;
  }

  // =========================================================================
  // ZAKŁADKI WIDOKU „PRAKTYKA”
  // =========================================================================
  if (state.subtab === 'scale') {
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
      </div>`;

  } else if (state.subtab === 'chord') {
    // =======================================================================
    // ZAKŁADKA: AKORD · ODKRYWANIE (Ostrość Łagodne/Ostre, kapodaster, filtry grup, trójdźwięki 3-strunowe, filtr skali!)
    // =======================================================================
    const customParam = state.customStringMidis.join(',');
    const reqGroup = state.chordGroupFilter || (state.chordFilterByScale ? 'all' : 'basic');
    const effAllTypes = state.chordFilterByScale
      ? !state.chordScaleExtraTypeFilter
      : !!state.allChordTypes;
    const effSharpness = state.chordSharpness || 'mild';
    const capoFret = Math.max(0, Math.min(12, parseInt(state.chordCapoFret || 0, 10) || 0));
    const url = `/api/voicings?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${state.chordId}&group=${encodeURIComponent(reqGroup)}&sharpness=${encodeURIComponent(effSharpness)}&accMode=${state.accMode}&allTypes=${effAllTypes?1:0}&filterByScale=${state.chordFilterByScale?1:0}&scaleRootPc=${state.chordScaleRootPc}&scaleId=${state.chordScaleId}&capo=${capoFret}`;
    const data = await fetchJson(url);

    if (data.resolvedRootPc !== undefined) state.rootPc = data.resolvedRootPc;
    if (data.resolvedChordId) state.chordId = data.resolvedChordId;
    state.chordLastMatchingIds = data.matchingChordIds || [];
    state.chordVoicingsList = data.voicings || [];
    if (state.selectedChordVoicingIdx >= state.chordVoicingsList.length) {
      state.selectedChordVoicingIdx = 0;
    }

    state.diatonicData = await fetchJson(`/api/diatonic?rootPc=${state.chordScaleRootPc}&scaleId=${state.chordScaleId}&sevenths=${state.chordDiatonicSevenths ? 1 : 0}&accMode=${state.accMode}`);

    const matchingRootsSet = new Set(data.matchingRootPcs || [0,1,2,3,4,5,6,7,8,9,10,11]);
    const matchingChordsSet = new Set(state.chordLastMatchingIds);
    const rootLabels = getRootLabels();
    const capoRelRootLbl = rootLabels[((state.rootPc - capoFret) % 12 + 12) % 12];

    stage.innerHTML = `
      ${buildCircleOfFifthsPanel()}
      <div style="display:flex;justify-content:space-between;align-items:center;margin:${state.chordShowCircle ? '12px' : '0'} 0 4px;flex-wrap:wrap;gap:8px;">
        <span class="section-label" style="margin:0;">CHWYTY (${state.chordVoicingsList.length})${capoFret > 0 ? ` · KAPODASTER PRÓG ${capoFret}` : ''}</span>
        ${capoFret > 0 ? `<span style="font-size:11.5px;color:var(--accent);font-weight:700;">Brzmi: ${rootLabels[state.rootPc]} · Kształt przy kapodastrze: ${capoRelRootLbl} (progi fizyczne ${capoFret}–24)</span>` : ''}
      </div>
      <div class="chord-cards-grid">
        ${state.chordVoicingsList.map((v, idx) => {
          const svg = buildChordSvgDiagram(v.frets, v.tapped, capoFret);
          const subParts = [];
          if (capoFret > 0 && v.shapeSymbol) subParts.push(`Kapo ${capoFret} · kształt ${v.shapeSymbol}`);
          subParts.push(v.usesTapping
            ? 'T = tapping ręką prawą'
            : (v.isSharp ? 'Ostre · rozciągnięcie' : 'Łagodne · klasyczny'));
          const subLine = subParts.join(' · ');
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
        <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:5px;">
          <div class="section-label" style="margin:0;">KAPODASTER (CAPO)</div>
          ${capoFret > 0 ? `<button class="chip-btn" style="padding:2px 7px;font-size:10.5px;color:var(--accent);border-color:var(--accent);" onclick="setChordCapoFret(0)">✕ Zdejmij</button>` : ''}
        </div>
        <div style="display:flex;align-items:center;gap:5px;margin-bottom:6px;">
          <button class="chip-btn" style="padding:5px 10px;" onclick="setChordCapoFret(${Math.max(0, capoFret - 1)})">−</button>
          <div style="flex:1;text-align:center;font-size:12px;font-weight:700;background:var(--bg-card);border:1px solid ${capoFret>0?'var(--accent)':'var(--border)'};border-radius:7px;padding:5px 6px;color:${capoFret>0?'var(--accent)':'var(--text-muted)'};">
            ${capoFret > 0 ? `Próg ${capoFret} (kształt ${capoRelRootLbl})` : 'Brak (puste struny)'}
          </div>
          <button class="chip-btn" style="padding:5px 10px;" onclick="setChordCapoFret(${Math.min(12, capoFret + 1)})">+</button>
        </div>
        <div style="display:grid;grid-template-columns:repeat(7,1fr);gap:4px;">
          <button class="chip-btn ${capoFret===0?'active':''}" style="padding:4px 2px;font-size:10.5px;" onclick="setChordCapoFret(0)">0</button>
          ${[1,2,3,4,5,6,7,8,9,10,11,12].map(f => `
            <button class="chip-btn ${capoFret===f?'active':''}" style="padding:4px 2px;font-size:10.5px;" onclick="setChordCapoFret(${f})">${f}</button>
          `).join('')}
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
    if (state.searchDir === 'detect') {
      ensureDetectScalePolling();
      stage.innerHTML = `
        <div style="display:flex;justify-content:space-between;align-items:center;padding:8px 4px;font-size:13px;">
          <div style="color:var(--text-muted);">
            🎤 <b style="color:var(--accent)">Wykrywanie skali</b> — zagraj dźwięki / akord, a lista pokaże pasujące skale
          </div>
        </div>
        <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:10px;padding:14px;display:flex;flex-direction:column;gap:10px;">
          <div style="font-size:12px;">
            <span style="color:var(--text-muted);">Wykryte dźwięki:</span>
            <strong id="detectScalePcsLabel" style="color:var(--accent);font-size:15px;">—</strong>
          </div>
          <div id="detectScaleList" style="display:flex;flex-direction:column;gap:4px;max-height:52vh;overflow-y:auto;"></div>
        </div>`;
      insp.innerHTML = `
        <div>
          <div class="section-label">SZUKAJ WG</div>
          <div class="btn-grid-2">
            <button class="chip-btn" onclick="state.searchDir='chord_to_scale';state.selectedSearchScaleIdx=-1;renderAll()">Akord → skale</button>
            <button class="chip-btn" onclick="state.searchDir='scale_to_chord';state.selectedSearchChordIdx=-1;renderAll()">Skala → akordy</button>
            <button class="chip-btn active" style="grid-column:1/-1;" onclick="state.searchDir='detect';renderAll()">🎤 Wykryj skalę</button>
          </div>
        </div>
        <div>
          <div class="section-label">TYP SKALI</div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.detectScaleMode==='all'?'active':''}" onclick="state.detectScaleMode='all';refreshDetectScale()">Wszystkie</button>
            <button class="chip-btn ${state.detectScaleMode==='major'?'active':''}" onclick="state.detectScaleMode='major';refreshDetectScale()">Dur</button>
            <button class="chip-btn ${state.detectScaleMode==='minor'?'active':''}" onclick="state.detectScaleMode='minor';refreshDetectScale()">Moll</button>
          </div>
        </div>
        <div>
          <button class="chip-btn" style="width:100%;" onclick="clearDetectScalePcs()">🧹 Wyczyść wykryte dźwięki</button>
        </div>
        <div style="font-size:11.5px;color:var(--text-muted);line-height:1.5;">
          ${t('Włącz mikrofon (lewy pasek) i zagraj akord lub kilka dźwięków. Lista zbiera grane klasy wysokości i podaje pasujące skale (najpierw podstawowe i modalne, potem pentatoniki).', 'Turn the microphone on (left bar) and play a chord or a few notes. The list collects the played pitch classes and shows matching scales.')}
        </div>`;
      refreshDetectScale();
      drawFretboard();
    } else {
      stopDetectScalePolling();
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
              <button class="chip-btn" style="grid-column:1/-1;" onclick="state.searchDir='detect';renderAll()">🎤 Wykryj skalę</button>
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
              <button class="chip-btn" style="grid-column:1/-1;" onclick="state.searchDir='detect';renderAll()">🎤 Wykryj skalę</button>
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
    }

  } else if (state.subtab === 'arpeggio' || state.subtab === 'sweep') {
    // =======================================================================
    // ROZDZIELONE ZAKŁADKI:
    // 1) ARPEGGIO (czyste arpeggia: Od prymy, CAGED, Inwersje, 2NPS, 3NPS, Mapa gryfu)
    // 2) ⚡ SWEEP PICKING (8 przejść Sweep + pojedyncze kształty Sweep 3/5/6 str., pętla domyślnie WYŁ.)
    // =======================================================================
    const isSweepTab = (state.subtab === 'sweep');
    const isTrans = isSweepTab && (state.sweepViewMode === 'transition');
    if (!isSweepTab && state.arpPattern === 4) state.arpPattern = 6;
    const curChordId = normalizeChordId(state.chordId);
    state.chordId = curChordId;
    const reqPattern = isSweepTab ? 4 : state.arpPattern;
    const reqPosIdx = isSweepTab ? (isTrans ? (state.sweepTransPosIdx || 0) : (state.sweepPosIdx || 0)) : (state.arpPosIdx || 0);
    const reqViewMode = isTrans ? 'transition' : 'single';
    const customParam = state.customStringMidis.join(',');
    const effDiatRootPc = (isTrans && state.arpTransitionId !== 'same_chord_inversions')
      ? state.rootPc
      : state.arpDiatonicScaleRootPc;
    const arpUrl = `/api/arpeggio?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(curChordId)}&pattern=${reqPattern}&posIdx=${reqPosIdx}&sweepStrings=${state.arpSweepStrings}&direction=${state.arpDirection}&seqStyle=${state.arpSeqStyle||0}&stringSet=${state.arpStringSet||0}&octaves=${state.arpOctaves||2}&startStr=${state.arpStartStr ?? -1}&returnStr=${state.arpReturnStr ?? -1}&viewMode=${encodeURIComponent(reqViewMode)}&transitionId=${encodeURIComponent(state.arpTransitionId)}&scaleRootPc=${effDiatRootPc}&scaleId=${encodeURIComponent(state.arpDiatonicScaleId)}&sevenths=${state.arpDiatonicSevenths?1:0}&accMode=${state.accMode}`;
    let data = await fetchJson(arpUrl);
    if (state._matchSweepSegBounds) {
      const mb = state._matchSweepSegBounds;
      state._matchSweepSegBounds = null;
      if (isSweepTab && !isTrans && Array.isArray(data.positions) && data.positions.length > 0) {
        const targetMid = (mb.minFret + mb.maxFret) * 0.5;
        let match = data.positions[0];
        let bestDist = Infinity;
        for (const p of data.positions) {
          const dist = Math.abs((p.minFret + p.maxFret) * 0.5 - targetMid) * 2
                     + Math.abs(p.minFret - mb.minFret) * 0.25
                     + Math.abs(p.maxFret - mb.maxFret) * 0.25;
          if (dist < bestDist) {
            bestDist = dist;
            match = p;
          }
        }
        if (match && match.idx !== data.posIdx) {
          state.sweepPosIdx = match.idx;
          const retryUrl = `/api/arpeggio?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(state.chordId)}&pattern=${reqPattern}&posIdx=${state.sweepPosIdx}&sweepStrings=${state.arpSweepStrings}&direction=${state.arpDirection}&seqStyle=${state.arpSeqStyle||0}&stringSet=${state.arpStringSet||0}&octaves=${state.arpOctaves||2}&startStr=${state.arpStartStr ?? -1}&returnStr=${state.arpReturnStr ?? -1}&viewMode=${encodeURIComponent(reqViewMode)}&transitionId=${encodeURIComponent(state.arpTransitionId)}&scaleRootPc=${effDiatRootPc}&scaleId=${encodeURIComponent(state.arpDiatonicScaleId)}&sevenths=${state.arpDiatonicSevenths?1:0}&accMode=${state.accMode}`;
          data = await fetchJson(retryUrl);
        }
      }
    }
    state.arpData = data;
    const effSweepPosIdx = (data.posIdx !== undefined) ? data.posIdx : (state.sweepPosIdx || 0);
    const effArpPosIdx = (data.posIdx !== undefined) ? data.posIdx : (state.arpPosIdx || 0);
    const tr = data.transition || { segments: [] };
    if (isTrans && tr.segments && state.arpActiveSegIdx >= tr.segments.length) {
      state.arpActiveSegIdx = 0;
    }

    const isSweepSingle = isSweepTab && !isTrans;
    const compactOn = isSweepSingle && !!state.sweepCompactTabs;
    const activeSteps = getActiveArpeggioSteps();
    const mainPlayStepIdx = (isSweepSingle && state.sweepPlaybackTarget === 'comp') ? -1 : state.arpPlayStepIdx;
    const tabSvg = buildArpeggioTabSvg(activeSteps, isTrans ? (tr.segments || []) : null, mainPlayStepIdx, isTrans, {
      compact: compactOn,
      svgId: 'arpTabSvg',
      playheadId: 'arpPlayheadBg',
      onStepClickFn: 'playSingleArpeggioStep',
      activeSegIdx: state.arpActiveSegIdx
    });
    const compList = Array.isArray(state.sweepComposition) ? state.sweepComposition : [];
    if (state.sweepCompActiveIdx >= compList.length) state.sweepCompActiveIdx = Math.max(0, compList.length - 1);
    const compSteps = (isSweepSingle && compList.length > 0) ? getSweepCompositionSteps() : [];
    const compSegments = compList.map((c, idx) => ({
      symbol: `${idx + 1}. ${c.symbol}`,
      roman: c.rangeLabel || `Poz. ${(c.posIdx || 0) + 1}`
    }));
    const compPlayStepIdx = (isSweepSingle && state.sweepPlaybackTarget === 'comp') ? state.arpPlayStepIdx : -1;
    const compTabSvg = (isSweepSingle && compList.length > 0)
      ? buildArpeggioTabSvg(compSteps, compSegments, compPlayStepIdx, true, {
          compact: compactOn,
          svgId: 'sweepCompTabSvg',
          playheadId: 'sweepCompPlayheadBg',
          onStepClickFn: 'playSingleSweepCompStep',
          activeSegIdx: state.sweepCompActiveIdx
        })
      : '';
    const isMainPlayActive = state.arpPlaying && (!isSweepSingle || state.sweepPlaybackTarget !== 'comp' || (!!state.sweepSequential && compList.length > 0));
    const isCompPlayActive = state.arpPlaying && isSweepSingle && state.sweepPlaybackTarget === 'comp';
    const hasRollAny = activeSteps.some(st => st.roll);
    const rootLabels = getRootLabels();
    const curLoopOn = !!state.arpLoop;
    const curSeqOn = !!state.sweepSequential;
    const quickArpTypes = [
      { id: 'major', lbl: 'maj' }, { id: 'minor', lbl: 'm' }, { id: 'dim', lbl: 'dim' },
      { id: 'aug', lbl: 'aug' }, { id: 'maj7', lbl: 'maj7' }, { id: '7', lbl: '7' },
      { id: 'm7', lbl: 'm7' }, { id: 'm7b5', lbl: 'm7♭5' }, { id: 'dim7', lbl: 'dim7' }
    ];
    const scaleDegMap = new Map();
    (data.scaleDegrees || []).forEach(sd => {
      if (!scaleDegMap.has(sd.pc)) scaleDegMap.set(sd.pc, sd.roman);
    });
    const possibleForRootList = Array.isArray(data.possibleForRoot) ? data.possibleForRoot : [];
    const possibleIdSet = new Set(possibleForRootList.map(p => normalizeChordId(p.id)));
    const savedPresets = Array.isArray(state.savedSweepCompositions) ? state.savedSweepCompositions : [];
    const savedPresetsBarHtml = (isSweepSingle && savedPresets.length > 0)
      ? `<div style="display:flex;align-items:center;flex-wrap:wrap;gap:6px;padding-top:4px;border-top:1px dashed var(--border);">
           <span style="font-size:10.5px;font-weight:700;color:var(--text-muted);text-transform:uppercase;letter-spacing:0.4px;">💾 Zapisane układy (${savedPresets.length}):</span>
           ${savedPresets.map((pr, pIdx) => `
             <div style="display:inline-flex;align-items:center;gap:2px;background:var(--bg-card);border:1px solid var(--border);border-radius:7px;padding:2px 4px;">
               <button class="chip-btn" style="border:none;background:transparent;padding:2px 6px;font-size:11px;"
                       onclick="loadSweepCompositionPreset(${pIdx})" title="Wczytaj zapisaną kompozycję">${pr.name}</button>
               <button class="chip-btn" style="border:none;background:transparent;padding:2px 5px;font-size:10px;color:#f87171;"
                       onclick="deleteSweepCompositionPreset(${pIdx})" title="Usuń zapisany układ">✕</button>
             </div>
           `).join('')}
         </div>`
      : '';
    const chordOptHtml = (c) => {
      const inSc = possibleIdSet.has(normalizeChordId(c.id));
      return `<option value="${c.id}" ${normalizeChordId(c.id)===curChordId?'selected':''}>${inSc ? '✓ ' : ''}${c.id} (${c.name})${inSc ? ' · w skali' : ''}</option>`;
    };
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

    const stageHeading = isTrans
      ? `⚡ ${tr.name}`
      : (isSweepTab
          ? `⚡ Sweep Picking (${state.arpSweepStrings} str.): ${data.chordSymbol} <span style="color:var(--text-muted);font-weight:600;font-size:13px;">(${data.chordName})</span>`
          : `Arpeggio: ${data.chordSymbol} <span style="color:var(--text-muted);font-weight:600;font-size:13px;">(${data.chordName})</span>`);

    const curTransSeg = (isTrans && tr.segments && tr.segments[state.arpActiveSegIdx]) ? tr.segments[state.arpActiveSegIdx] : null;
    stage.innerHTML = `
      <div class="fretboard-card" style="gap:10px;padding:12px 14px;">
        <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;">
          <div style="min-width:0;">
            <div style="font-size:16px;font-weight:800;color:var(--text);">${stageHeading}</div>
            ${formulaHtml || `<div style="font-size:11.5px;color:var(--text-muted);margin-top:2px;">${tr.description}</div>`}
          </div>
          <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
            ${!isTrans ? `<button class="chip-btn" style="padding:6px 10px;font-size:12px;" onclick="playArpeggioChordPreview()" title="Posłuchaj akordu bazowego">🎸 Akord</button>` : ''}
            <button id="arpPlayBtn" class="chip-btn ${isMainPlayActive?'active':''}" style="padding:6px 13px;font-weight:700;"
                    onclick="toggleArpeggioPlayback()">
              ${isMainPlayActive ? '⏹ Stop' : '▶ Odsłuchaj'}
            </button>
            ${isSweepTab ? `
              <button class="chip-btn ${curSeqOn?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="toggleArpOrSweepLoop()"
                      title="Wł.: odtwarza wszystkie sweepy po kolei od początku. Wył.: odtwarza tylko aktualnie wybrany sweep.">⏭️ Po kolei: ${curSeqOn ? 'Wł.' : 'Wył.'}</button>
            ` : `
              <button class="chip-btn ${curLoopOn?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="toggleArpOrSweepLoop()">🔁 Pętla: ${curLoopOn ? 'Wł.' : 'Wył.'}</button>
            `}
            <div style="display:flex;align-items:center;gap:5px;background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:3px 7px;font-size:12px;">
              <button class="chip-btn" style="padding:2px 6px;" onclick="setArpeggioBpm(state.arpBpm - 10)">−10</button>
              <strong id="arpBpmVal" style="min-width:52px;text-align:center;color:var(--accent);">${state.arpBpm} BPM</strong>
              <button class="chip-btn" style="padding:2px 6px;" onclick="setArpeggioBpm(state.arpBpm + 10)">+10</button>
            </div>
            <button id="arpAutoTempoBtn" class="chip-btn ${state.arpAutoTempo?'active':''}" style="padding:6px 10px;font-size:12px;"
                    onclick="toggleArpAutoTempo()"
                    title="Po każdym pełnym odegraniu podnosi tempo o +4 BPM">🚀 Auto-Tempo: ${state.arpAutoTempo ? '+4 BPM' : 'Wył.'}</button>
            <button class="chip-btn" style="padding:6px 11px;font-size:12px;border-color:var(--accent);color:var(--accent);"
                    onclick="sendArpeggioToExerciseTab()" title="Przenieś sekwencję do Edytora Ćwiczeń (ćwiczenie z mikrofonem i eksport GP5/MIDI)">
              ✏️ Ćwicz w TAB / GP5
            </button>
          </div>
        </div>

        ${isSweepSingle ? `
          <div style="display:flex;align-items:stretch;gap:8px;">
            <div id="arpTabSvgWrap" style="flex:1;min-width:0;background:#14100d;border:1px solid #2b231d;border-radius:10px;padding:${compactOn ? '6px 10px' : '8px 12px'};overflow-x:auto;">
              ${tabSvg}
            </div>
            <div style="flex-shrink:0;display:flex;flex-direction:column;gap:5px;justify-content:center;">
              <button class="chip-btn" style="flex:1;min-width:52px;min-height:46px;padding:6px 10px;font-size:21px;font-weight:900;line-height:1;border-color:var(--accent);color:var(--accent);display:flex;flex-direction:column;align-items:center;justify-content:center;gap:2px;"
                      onclick="addCurrentSweepToComposition()"
                      title="Dodaj ten kształt Sweep do kompozycji poniżej">
                <span>+</span>
                <span style="font-size:9.5px;font-weight:700;">Dodaj</span>
              </button>
              <button class="chip-btn ${state.sweepCompactTabs?'active':''}" style="padding:4px 8px;font-size:10.5px;white-space:nowrap;"
                      onclick="toggleSweepCompactTabs()"
                      title="Zmniejsz lub przywróć rozmiar obu tabulatur (żeby zmieściły się jednocześnie)">
                🔍 ${state.sweepCompactTabs ? '100%' : 'Zmniejsz'}
              </button>
            </div>
          </div>

          <div style="background:rgba(255,255,255,0.02);border:1px solid #2b231d;border-radius:10px;padding:8px 12px;display:flex;flex-direction:column;gap:8px;">
            <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:8px;">
              <div style="display:flex;align-items:center;flex-wrap:wrap;gap:5px;">
                <span style="font-size:10.5px;font-weight:700;color:var(--text-muted);text-transform:uppercase;letter-spacing:0.4px;margin-right:2px;">Ruch:</span>
                <button class="chip-btn ${(state.arpDirection||0)===0?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="setSweepDirectionAndFit(0)">↕ Pełny (Dół-Góra)</button>
                <button class="chip-btn ${(state.arpDirection||0)===1?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="setSweepDirectionAndFit(1)">↓ Tylko w dół</button>
                <button class="chip-btn ${(state.arpDirection||0)===2?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="setSweepDirectionAndFit(2)">↑ Tylko w górę</button>
                <span style="width:1px;height:16px;background:#3a2f26;margin:0 4px;"></span>
                <span style="font-size:10.5px;font-weight:700;color:var(--text-muted);text-transform:uppercase;letter-spacing:0.4px;margin-right:2px;">Rytm:</span>
                <button class="chip-btn ${state.sweepMetricFit==='sixteenths'?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="setSweepMetricFit('sixteenths')" title="Podział szesnastkowy (4 nuty na ćwierćnutę)">🎵 Szesnastki</button>
                <button class="chip-btn ${state.sweepMetricFit!=='sixteenths'?'active':''}" style="padding:3px 9px;font-size:11px;"
                        onclick="setSweepMetricFit('sextuplets')" title="Podział sekstolowy / triolowy (6 nut na ćwierćnutę)">🎶 Sekstole</button>
              </div>
              <div style="font-size:11px;font-weight:700;color:#d6cec2;background:rgba(255,255,255,0.04);border:1px solid #352a20;border-radius:6px;padding:3px 8px;white-space:nowrap;">
                ${formatSweepMetricSummary(activeSteps, state.sweepMetricFit)}
              </div>
            </div>

            ${(data.positions && data.positions.length > 0) ? `
              <div style="display:flex;align-items:center;flex-wrap:wrap;gap:6px;">
                <span style="font-size:10.5px;font-weight:700;color:var(--text-muted);text-transform:uppercase;letter-spacing:0.4px;white-space:nowrap;">Pozycje kształtu (${data.positions.length}):</span>
                <div class="pos-pill-grid">
                  ${data.positions.map((pos, pIdx) => `
                    <button class="chip-btn pos-pill-btn ${effSweepPosIdx===pos.idx?'active':''}"
                            title="${String(pos.label || '').replace(/"/g, '&quot;')}"
                            onclick="stopArpeggioPlayback(false);state.sweepPosIdx=${pos.idx};saveSettings();renderStageAndInspector()">
                      ${formatCompactPositionLabel(pos, pIdx)}
                    </button>
                  `).join('')}
                </div>
              </div>
            ` : ''}

            ${buildArpDiatonicSectionHtml(data, true, true)}
            ${compList.length === 0 ? savedPresetsBarHtml : ''}
          </div>
        ` : `
          <div id="arpTabSvgWrap" style="background:#14100d;border:1px solid #2b231d;border-radius:10px;padding:8px 12px;overflow-x:auto;">
            ${tabSvg}
          </div>
        `}

        ${(isSweepSingle && compList.length > 0) ? `
          <div style="background:rgba(255,255,255,0.02);border:1px solid #352a20;border-radius:10px;padding:10px 12px;display:flex;flex-direction:column;gap:8px;">
            <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;">
              <div class="section-label" style="margin:0;">🧩 UKŁADANIE SWEEPÓW — TWOJA KOMPOZYCJA (${compList.length} ogniw · ${compSteps.length} nut${getSweepBarFillText(compList, compSteps.length)})</div>
              <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
                <button id="sweepCompPlayBtn" class="chip-btn ${isCompPlayActive?'active':''}" style="padding:5px 11px;font-size:11.5px;font-weight:700;"
                        onclick="toggleSweepCompositionPlayback()">
                  ${isCompPlayActive ? '⏹ Stop' : '▶ Odsłuchaj kompozycję'}
                </button>
                <button class="chip-btn" style="padding:5px 10px;font-size:11.5px;"
                        onclick="saveCurrentSweepCompositionPreset()" title="Zapisz bieżący układ sweepów do pamięci podręcznej">
                  💾 Zapisz układ
                </button>
                <button class="chip-btn" style="padding:5px 10px;font-size:11.5px;border-color:var(--accent);color:var(--accent);"
                        onclick="sendSweepCompositionToExerciseTab()">
                  ✏️ Ćwicz kompozycję w TAB / GP5
                </button>
                <button class="chip-btn" style="padding:5px 9px;font-size:11px;color:#f87171;border-color:#7f1d1d;"
                        onclick="clearSweepComposition()">
                  🗑️ Wyczyść
                </button>
              </div>
            </div>
            ${savedPresetsBarHtml}
            <div style="display:grid;grid-template-columns:repeat(auto-fill,minmax(160px,1fr));gap:7px;">
              ${compList.map((seg, idx) => `
                <div class="chord-box-card ${idx===state.sweepCompActiveIdx?'selected':''}" data-sweep-comp-seg="${idx}"
                     style="align-items:flex-start;text-align:left;padding:7px 9px;gap:2px;"
                     onclick="selectSweepCompItem(${idx})">
                  <div style="display:flex;justify-content:space-between;width:100%;font-size:10px;color:var(--text-muted);">
                    <span>${idx + 1}.</span>
                    <span>${seg.sweepStrings} str. · pr. ${seg.minFret}–${seg.maxFret}</span>
                  </div>
                  <div style="font-size:16px;font-weight:800;color:var(--accent);">${seg.symbol}</div>
                  <div style="font-size:10.5px;color:#d6cec2;line-height:1.25;">Poz. ${(seg.posIdx || 0) + 1} · ${seg.rangeLabel || ''}</div>
                  <div style="display:flex;gap:4px;margin-top:4px;width:100%;" onclick="event.stopPropagation()">
                    <button class="chip-btn" style="padding:2px 6px;font-size:10px;opacity:${idx===0?'0.4':'1'};" ${idx===0?'disabled':''}
                            onclick="moveSweepCompItem(${idx}, -1)" title="Przesuń w lewo">◀</button>
                    <button class="chip-btn" style="padding:2px 6px;font-size:10px;opacity:${idx===compList.length-1?'0.4':'1'};" ${idx===compList.length-1?'disabled':''}
                            onclick="moveSweepCompItem(${idx}, 1)" title="Przesuń w prawo">▶</button>
                    <button class="chip-btn" style="padding:2px 6px;font-size:10px;margin-left:auto;"
                            onclick="replaceSweepCompItemWithCurrent(${idx})" title="Podmień na aktualnie ustawiony Sweep z góry">↻</button>
                    <button class="chip-btn" style="padding:2px 6px;font-size:10px;color:#f87171;border-color:#7f1d1d;"
                            onclick="removeSweepCompItem(${idx})" title="Usuń z kompozycji">✕</button>
                  </div>
                </div>
              `).join('')}
              <div class="chord-box-card" style="border-style:dashed;border-color:#57412e;justify-content:center;align-items:center;min-height:86px;gap:4px;"
                   onclick="addCurrentSweepToComposition()" title="Dodaj aktualny kształt Sweep na koniec kompozycji">
                <div style="font-size:20px;font-weight:900;color:var(--accent);">+</div>
                <div style="font-size:11px;color:var(--text-muted);font-weight:700;">Dodaj aktualny</div>
              </div>
            </div>
            <div id="sweepCompTabSvgWrap" style="background:#14100d;border:1px solid #2b231d;border-radius:10px;padding:${state.sweepCompactTabs ? '6px 10px' : '8px 12px'};overflow-x:auto;">
              ${compTabSvg}
            </div>
          </div>
        ` : ''}

        ${isTrans ? `
          <div>
            <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;margin-bottom:6px;">
              <div class="section-label" style="margin:0;">KOLEJNE OGNIWA PRZEJŚCIA (KLIKNIJ AKORD · PRZERYWANY KONTUR = NASTĘPNY)</div>
              <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
                <span style="font-size:11px;color:var(--text-muted);">Strefa startu:</span>
                ${[0, 1, 2, 3].map(pv => `
                  <button class="chip-btn ${(state.sweepTransPosIdx||0)===pv?'active':''}" style="padding:3px 8px;font-size:11px;"
                          onclick="setSweepTransPosIdx(${pv})">${pv===0 ? 'Auto' : 'Poz. ' + pv}</button>
                `).join('')}
                ${curTransSeg ? `
                  <button class="chip-btn" style="padding:3px 10px;font-size:11px;border-color:var(--accent);color:var(--accent);"
                          onclick="openSweepSingleForSegment(${state.arpActiveSegIdx})"
                          title="Przejdź do widoku pojedynczego kształtu i zobacz wszystkie pozycje tego akordu na całym gryfie">
                    🔍 Wszystkie pozycje: ${curTransSeg.symbol}
                  </button>
                ` : ''}
              </div>
            </div>
            <div style="display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:8px;">
              ${(tr.segments || []).map((seg, idx) => `
                <div class="chord-box-card ${idx===state.arpActiveSegIdx?'selected':''}" data-sweep-seg="${idx}"
                     style="align-items:flex-start;text-align:left;padding:8px 10px;gap:3px;"
                     onclick="selectArpTransitionSegment(${idx})">
                  <div style="display:flex;justify-content:space-between;width:100%;font-size:10.5px;color:var(--text-muted);">
                    <span>${idx + 1}. ${seg.roman}</span>
                    <span>${seg.stringSpan} str. · pr. ${seg.minFret}–${seg.maxFret}</span>
                  </div>
                  <div style="font-size:18px;font-weight:800;color:var(--accent);">${seg.symbol}</div>
                  <div style="font-size:10.5px;color:#d6cec2;line-height:1.25;">${seg.hint}</div>
                </div>
              `).join('')}
            </div>
          </div>
        ` : (isSweepSingle ? '' : buildArpDiatonicSectionHtml(data, isSweepTab, false))}
      </div>`;

    if (isSweepTab) {
      insp.innerHTML = `
        <div>
          <div class="section-label">TRYB SWEEP PICKING</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${isTrans?'active':''}"
                    onclick="stopArpeggioPlayback(false);state.sweepViewMode='transition';state.arpActiveSegIdx=0;saveSettings();renderStageAndInspector()">⚡ Przejścia Sweep</button>
            <button class="chip-btn ${!isTrans?'active':''}"
                    onclick="stopArpeggioPlayback(false);state.sweepViewMode='single';saveSettings();renderStageAndInspector()">🎸 Pojedynczy kształt</button>
          </div>
        </div>

        ${isTrans ? `
          <div>
            <div class="section-label">TONACJA PRZEJŚCIA</div>
            <div class="btn-grid-6">
              ${rootLabels.map((lbl, pc) => `
                <button class="chip-btn ${state.rootPc===pc?'active':''}"
                        onclick="stopArpeggioPlayback(false);state.rootPc=${pc};state.arpDiatonicScaleRootPc=${pc};saveSettings();renderAll()">${lbl}</button>
              `).join('')}
            </div>
          </div>

          <div>
            <div class="section-label">LICZBA STRUN SWEEP</div>
            <div class="btn-grid-3">
              <button class="chip-btn ${state.arpSweepStrings===3?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=3;saveSettings();renderStageAndInspector()">3 struny</button>
              <button class="chip-btn ${state.arpSweepStrings===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=5;saveSettings();renderStageAndInspector()">5 strun</button>
              <button class="chip-btn ${state.arpSweepStrings===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=6;saveSettings();renderStageAndInspector()">6 strun</button>
            </div>
          </div>

          <div>
            <div class="section-label">SKALA DLA SWEEP PICKING</div>
            <select onchange="stopArpeggioPlayback(false);state.arpDiatonicScaleId=this.value;saveSettings();renderStageAndInspector();">
              ${(state.initData.scales || []).map(s => `<option value="${s.id}" ${s.id===state.arpDiatonicScaleId?'selected':''}>${s.name}</option>`).join('')}
            </select>
            <div class="btn-grid-2" style="margin-top:6px;">
              <button class="chip-btn ${!state.arpDiatonicSevenths?'active':''}" style="padding:5px 6px;font-size:11px;"
                      onclick="stopArpeggioPlayback(false);state.arpDiatonicSevenths=false;saveSettings();renderStageAndInspector()">Trójdźwięki</button>
              <button class="chip-btn ${state.arpDiatonicSevenths?'active':''}" style="padding:5px 6px;font-size:11px;"
                      onclick="stopArpeggioPlayback(false);state.arpDiatonicSevenths=true;saveSettings();renderStageAndInspector()">Septymowe</button>
            </div>
            <button class="chip-btn ${state.arpTransitionId==='diatonic_climb'?'active':''}" style="width:100%;margin-top:6px;padding:6px 8px;font-size:11.5px;"
                    onclick="launchDiatonicScaleSweep()">
              ⚡ Wspinaczka po wybranej skali
            </button>
          </div>
          ${state.arpTransitionId === 'same_chord_inversions' ? `
            <div>
              <div class="section-label">AKORD DLA INWERSJI</div>
              <select onchange="stopArpeggioPlayback(false);state.chordId=this.value;saveSettings();renderStageAndInspector();">
                ${groupedChordsHtml}
              </select>
            </div>
          ` : ''}
          <div>
            <div class="section-label">GOTOWE PROGRESJE SWEEP PICKING (${(data.transitionPresets || []).length})</div>
            <div style="display:flex;flex-direction:column;gap:6px;">
              ${(data.transitionPresets || []).map(p => `
                <button class="chip-btn ${state.arpTransitionId===p.id?'active':''}" style="text-align:left;padding:8px 10px;display:flex;flex-direction:column;gap:2px;"
                        onclick="stopArpeggioPlayback(false);state.arpTransitionId='${p.id}';state.arpActiveSegIdx=0;saveSettings();renderStageAndInspector()">
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
          ${buildArpScaleCohesionInspectorHtml({ data, rootLabels, quickArpTypes, groupedChordsHtml, possibleForRootList, possibleIdSet, scaleDegMap, isSweepMode: true })}
          <div>
            <div class="section-label">LICZBA STRUN SWEEP</div>
            <div class="btn-grid-3">
              <button class="chip-btn ${state.arpSweepStrings===3?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=3;saveSettings();renderStageAndInspector()">3 struny</button>
              <button class="chip-btn ${state.arpSweepStrings===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=5;saveSettings();renderStageAndInspector()">5 strun</button>
              <button class="chip-btn ${state.arpSweepStrings===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpSweepStrings=6;saveSettings();renderStageAndInspector()">6 strun</button>
            </div>
          </div>
          ${(data.positions && data.positions.length > 0) ? `
            <div>
              <div class="section-label">POZYCJA KSZTAŁTU SWEEP (${data.positions.length})</div>
              <div class="pos-pill-grid-2col">
                ${data.positions.map((pos, pIdx) => `
                  <button class="chip-btn pos-pill-btn ${effSweepPosIdx===pos.idx?'active':''}"
                          title="${String(pos.label || '').replace(/"/g, '&quot;')}"
                          onclick="stopArpeggioPlayback(false);state.sweepPosIdx=${pos.idx};saveSettings();renderStageAndInspector()">
                    ${formatCompactPositionLabel(pos, pIdx)}
                  </button>
                `).join('')}
              </div>
            </div>
          ` : ''}
          <div>
            <div class="section-label">SEKWENCJA</div>
            <div class="btn-grid-3">
              <button class="chip-btn ${state.arpSeqStyle===0?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=0;saveSettings();renderStageAndInspector()">Prosto</button>
              <button class="chip-btn ${state.arpSeqStyle===1?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=1;saveSettings();renderStageAndInspector()">Grupy po 3</button>
              <button class="chip-btn ${state.arpSeqStyle===2?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=2;saveSettings();renderStageAndInspector()">Skoki 1–5</button>
            </div>
          </div>
          ${buildArpReferenceScaleInspectorHtml(rootLabels)}
        `}`;
    } else {
      insp.innerHTML = `
        ${buildArpScaleCohesionInspectorHtml({ data, rootLabels, quickArpTypes, groupedChordsHtml, possibleForRootList, possibleIdSet, scaleDegMap, isSweepMode: false })}

        <div>
          <div class="section-label">UKŁAD NA GRYFIE</div>
          <div style="display:grid;grid-template-columns:1fr 1fr;gap:6px;">
            <button class="chip-btn ${state.arpPattern===6?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=6;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">🎯 Od prymy</button>
            <button class="chip-btn ${state.arpPattern===1?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=1;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">📦 CAGED (5 kszt.)</button>
            <button class="chip-btn ${state.arpPattern===5?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=5;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">↔ Inwersje</button>
            <button class="chip-btn ${state.arpPattern===2?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=2;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">2NPS (2/str.)</button>
            <button class="chip-btn ${state.arpPattern===3?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=3;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">3NPS (legato)</button>
            <button class="chip-btn ${state.arpPattern===0?'active':''}" onclick="stopArpeggioPlayback(false);state.arpPattern=0;state.arpPosIdx=0;saveSettings();renderStageAndInspector()">Mapa gryfu</button>
          </div>
        </div>

        ${state.arpPattern === 6 ? `
          <div>
            <div class="section-label">ZAKRES OKTAW</div>
            <div class="btn-grid-2">
              <button class="chip-btn ${state.arpOctaves===1?'active':''}" onclick="stopArpeggioPlayback(false);state.arpOctaves=1;saveSettings();renderStageAndInspector()">1 oktawa</button>
              <button class="chip-btn ${state.arpOctaves===2?'active':''}" onclick="stopArpeggioPlayback(false);state.arpOctaves=2;saveSettings();renderStageAndInspector()">2 oktawy</button>
            </div>
          </div>
        ` : ''}

        ${state.arpPattern === 5 ? `
          <div>
            <div class="section-label">ZESTAW STRUN (INWERSJE)</div>
            <div style="display:flex;flex-direction:column;gap:5px;">
              ${stringSetLabels.map((lbl, idx) => `
                <button class="chip-btn ${state.arpStringSet===idx?'active':''}" style="padding:5px 8px;font-size:11.5px;"
                        onclick="stopArpeggioPlayback(false);state.arpStringSet=${idx};saveSettings();renderStageAndInspector()">${lbl}</button>
              `).join('')}
            </div>
          </div>
        ` : ''}

        ${(data.positions && data.positions.length > 0) ? `
          <div>
            <div class="section-label">POZYCJA / PRZEWRÓT (${data.positions.length})</div>
            <div class="pos-pill-grid-2col">
              ${data.positions.map((pos, pIdx) => `
                <button class="chip-btn pos-pill-btn ${effArpPosIdx===pos.idx?'active':''}"
                        title="${String(pos.label || '').replace(/"/g, '&quot;')}"
                        onclick="stopArpeggioPlayback(false);state.arpPosIdx=${pos.idx};saveSettings();renderStageAndInspector()">
                  ${formatCompactPositionLabel(pos, pIdx)}
                </button>
              `).join('')}
            </div>
          </div>
        ` : ''}

        <div>
          <div class="section-label">SEKWENCJA I KIERUNEK</div>
          <div class="btn-grid-3" style="margin-bottom:6px;">
            <button class="chip-btn ${state.arpSeqStyle===0?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=0;saveSettings();renderStageAndInspector()">Prosto</button>
            <button class="chip-btn ${state.arpSeqStyle===1?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=1;saveSettings();renderStageAndInspector()">Grupy po 3</button>
            <button class="chip-btn ${state.arpSeqStyle===2?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpSeqStyle=2;saveSettings();renderStageAndInspector()">Skoki 1–5</button>
          </div>
          <div class="btn-grid-3">
            <button class="chip-btn ${state.arpDirection===0?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=0;saveSettings();renderStageAndInspector()">↕ Góra-dół</button>
            <button class="chip-btn ${state.arpDirection===1?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=1;saveSettings();renderStageAndInspector()">↓ W górę</button>
            <button class="chip-btn ${state.arpDirection===2?'active':''}" style="font-size:11px;" onclick="stopArpeggioPlayback(false);state.arpDirection=2;saveSettings();renderStageAndInspector()">↑ W dół</button>
          </div>
        </div>

        <div>
          <div class="section-label">ZAKRES STRUN (FRAZA POD METRONOM)</div>
          <div style="display:flex;flex-direction:column;gap:6px;">
            <div style="display:flex;align-items:center;gap:6px;">
              <span style="font-size:11px;color:var(--text-muted);min-width:62px;white-space:nowrap;">Od struny:</span>
              <select style="flex:1;min-width:0;" onchange="stopArpeggioPlayback(false);state.arpStartStr=parseInt(this.value,10);saveSettings();renderStageAndInspector()">
                <option value="-1" ${(state.arpStartStr ?? -1) === -1 ? 'selected' : ''}>Auto (najniższa)</option>
                ${[0,1,2,3,4,5].map(s => `<option value="${s}" ${(state.arpStartStr ?? -1) === s ? 'selected' : ''}>Str. ${6 - s} (${rootLabels[(state.customStringMidis[s] % 12 + 12) % 12]})</option>`).join('')}
              </select>
            </div>
            <div style="display:flex;align-items:center;gap:6px;">
              <span style="font-size:11px;color:var(--text-muted);min-width:62px;white-space:nowrap;">Powrót do:</span>
              <select style="flex:1;min-width:0;" onchange="stopArpeggioPlayback(false);state.arpReturnStr=parseInt(this.value,10);saveSettings();renderStageAndInspector()">
                <option value="-1" ${(state.arpReturnStr ?? -1) === -1 ? 'selected' : ''}>Pełny (do najniższej)</option>
                ${[0,1,2,3,4,5].map(s => `<option value="${s}" ${(state.arpReturnStr ?? -1) === s ? 'selected' : ''}>Str. ${6 - s} (${rootLabels[(state.customStringMidis[s] % 12 + 12) % 12]})</option>`).join('')}
              </select>
            </div>
            <div style="font-size:10.5px;color:var(--text-muted);line-height:1.4;">
              ${t('Wybierz, od której struny zaczynasz i do której wracasz — dobierz parzystą liczbę nut, żeby fraza pętliła się równo z metronomem (bez pauzy 3/16).', 'Pick the start string and the return string to get an even note count so the phrase loops in time (no 3/16 rest).')}
            </div>
          </div>
        </div>

        ${buildArpReferenceScaleInspectorHtml(rootLabels)}`;
    }

    if (isTrans) {
      if (state.arpPlayStepIdx >= 0) {
        updateArpTabHighlightAndScroll(state.arpPlayStepIdx, false);
      } else {
        scrollArpTabToSegment(state.arpActiveSegIdx || 0, false);
      }
    } else if (isSweepSingle && compList.length > 0) {
      if (isCompPlayActive && state.arpPlayStepIdx >= 0) {
        updateSweepCompTabHighlightAndScroll(state.arpPlayStepIdx, false);
      } else {
        scrollSweepCompTabToSegment(state.sweepCompActiveIdx || 0, false);
      }
    }

    renderFretboardToolbarForActiveTab();
    drawFretboard();

  } else if ((state.subtab === 'scale_practice') || (state.subtab === 'exercise' && !state.exercisePlayerActive && state.exerciseSubView === 'scale_practice')) {
    // =======================================================================
    // PODWIDOK ĆWICZENIA: PRAKTYKA SKAL I ARPEGGIÓW (1–3 oktawy, 16 przebiegów, Diagonal/1 str./3NPS/4NPS/CAGED)
    // =======================================================================
    const baseNoteLabels = getRootLabels();
    const scaleObj = (state.initData.scales || []).find(s => s.id === state.scaleId) || { id: 'major', name: 'Durowa' };
    const chordObj = (state.initData.chords || []).find(c => c.id === state.chordId) || { id: 'minor', name: 'Molowy' };
    const degList = (state.fretboardData && state.fretboardData.scaleDegrees) ? state.fretboardData.scaleDegrees : [];
    const degNumsStr = degList.map(d => d.interval || String(d.deg)).join(' ');
    const patternGroups = [
      {
        label: 'Liniowe i sekwencje melodyczne',
        items: [
          { code: 0,  name: 'Cała skala (liniowo)',         desc: 'Ciągły bieg po kolei: 1-2-3-4-5-6-7...' },
          { code: 3,  name: 'Sekwencja 3-nutowa (1-2-3)',   desc: 'Klasyczne trójki wzdłuż skali: 1-2-3, 2-3-4, 3-4-5...' },
          { code: 4,  name: 'Sekwencja 4-nutowa (1-2-3-4)', desc: 'Cztery kolejne dźwięki: 1-2-3-4, 2-3-4-5, 3-4-5-6...' },
          { code: 10, name: 'Sekstole / grupy po 6 (shred)',desc: 'Płynne biegi 6-nutowe (legato / alternate): 1-2-3-4-5-6, 2-3-4-5-6-7...' },
          { code: 11, name: 'Krok w tył (1-2-3-2)',         desc: 'Niezależność palców i precyzja: 1-2-3-2, 2-3-4-3...' },
          { code: 12, name: 'Zwrot shred (1-2-3-1)',        desc: 'Klasyczny wzór kostkowania naprzemiennego (Paul Gilbert): 1-2-3-1, 2-3-4-2...' }
        ]
      },
      {
        label: 'Interwały diatoniczne',
        items: [
          { code: 5,  name: 'Tercje diatoniczne (1-3, 2-4)',       desc: 'Melodia skokami co tercję: 1-3, 2-4, 3-5, 4-6...' },
          { code: 13, name: 'Tercje naprzemienne (zig-zag)',       desc: 'Złamane tercje (1-3, 4-2, 3-5, 6-4...) — świetne na frazowanie.' },
          { code: 6,  name: 'Kwarty diatoniczne (1-4, 2-5)',       desc: 'Nowoczesne, otwarte skoki co kwartę: 1-4, 2-5, 3-6...' },
          { code: 7,  name: 'Kwinty diatoniczne (1-5, 2-6)',       desc: 'Szerokie, rockowo-fusionowe skoki kwintowe: 1-5, 2-6, 3-7...' },
          { code: 8,  name: 'Seksty diatoniczne (1-6, 2-7)',       desc: 'Śpiewne szerokie interwały sekstowe: 1-6, 2-7...' },
          { code: 9,  name: 'Skoki oktawowe (1-8, 2-9)',           desc: 'Kontrola odległości oktawy wzdłuż skali: 1-8, 2-9...' }
        ]
      },
      {
        label: 'Harmonia w skali i technika gitarowa',
        items: [
          { code: 14, name: 'Trójdźwięki po skali (1-3-5, 2-4-6)',     desc: 'Harmonizowane trójdźwięki diatoniczne w ramach skali — podstawa improwizacji!' },
          { code: 15, name: 'Trójdźwięki naprzemienne (1-3-5, 6-4-2)', desc: 'Co drugi trójdźwięk grany w dół — klasyk jazz/fusion/rock.' },
          { code: 16, name: 'Czterodźwięki septymowe (1-3-5-7)',       desc: 'Arpeggia 7-kowe z każdego stopnia skali: 1-3-5-7, 2-4-6-8...' },
          { code: 17, name: 'Nuta pedałowa na prymie (1-2-1-3-1-4)',   desc: 'Pedal point zakotwiczony na tonice — styl neoklasyczny / rock / metal.' },
          { code: 18, name: 'Przeskok struny (String Skipping)',       desc: 'Przeplatanie niesąsiednich strun (0→2, 1→3, 2→4, 3→5) — precyzja prawej ręki.' }
        ]
      }
    ];
    const allPatterns = patternGroups.flatMap(g => g.items);
    const activePat = allPatterns.find(p => p.code === state.spPattern) || allPatterns[0];
    const dirNames = ['Rosnąco ↑', 'Malejąco ↓', 'Rosnąco → Malejąco', 'Malejąco → Rosnąco'];
    const posNames = ['Cały gryf', '3NPS', '4NPS', 'CAGED'];
    const travNames = ['Standard (pozycja)', '↗ Diagonalnie (3 okt.)', '↔ 1 struna (poziomo)'];
    const cagedLabels = ['C', 'A', 'G', 'E', 'D'];
    const stringLabels = (state.customStringMidis && state.customStringMidis.length === 6)
      ? state.customStringMidis.map(m => baseNoteLabels[((m % 12) + 12) % 12])
      : ['E', 'A', 'D', 'G', 'B', 'E'];
    const fretZones = [
      { id: 'all',  lbl: 'Cały (0–24)', min: 0,  max: 24 },
      { id: 'low',  lbl: 'Niska (0–7)', min: 0,  max: 7 },
      { id: 'mid',  lbl: 'Środek (5–14)', min: 5, max: 14 },
      { id: 'high', lbl: 'Wysoka (12–24)', min: 12, max: 24 }
    ];
    const activeZone = fretZones.find(z => z.id === state.spFretZone) || fretZones[0];
    const rhythmOpts = [
      { id: 'q',   dur: 0, tup: false, lbl: '♩ Ćwierć' },
      { id: 'e',   dur: 1, tup: false, lbl: '♪ Ósemki' },
      { id: 't',   dur: 1, tup: true,  lbl: '♪³ Triole' },
      { id: 's',   dur: 2, tup: false, lbl: '♬ Szesnastki' }
    ];
    const activeRhythm = rhythmOpts.find(r => r.dur === (state.spNoteDur ?? 1) && r.tup === !!state.spTuplet) || rhythmOpts[1];
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
    const activeStartStrLbl = state.spStartString < 0
      ? (state.spTraversal === 2 ? `Domyślna (1 · ${stringLabels[5]})` : 'Dowolna')
      : `Struna ${6 - state.spStartString} (${stringLabels[state.spStartString]})`;
    const activeSystemLbl = state.spTraversal === 1
      ? '↗ Diagonalnie po gryfie'
      : (state.spTraversal === 2
          ? '↔ 1 struna (poziomo)'
          : (state.spPosSystem > 0
              ? `${posNames[state.spPosSystem]} #${(state.spPosIdx || 0) + 1}${state.spPosSystem === 3 ? ` (${cagedLabels[(state.spPosIdx || 0) % 5]})` : ''}`
              : 'Cały gryf'));
    const posCountForSys = (state.spPosSystem === 1) ? 7 : 5;

    stage.innerHTML = `
      <div class="sp-shell">
        <div class="sp-header">
          <div style="display:flex;align-items:center;gap:14px;flex-wrap:wrap;">
            <h2 style="font-size:21px;font-weight:800;margin:0;color:var(--text);">${t('Ćwiczenie', 'Exercise')}</h2>
            <div class="ear-seg-row" style="width:auto;">
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('composer')">✏️ ${t('Edytor TAB', 'TAB Editor')}</button>
              <button class="ear-seg-btn active" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('scale_practice')">🎯 ${t('Praktyka skal', 'Scale Practice')}</button>
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('harmonics')">✨ ${t('Flażolety', 'Harmonics')}</button>
            </div>
          </div>
          <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
            <button class="chip-btn" style="padding:7px 13px;font-size:12px;" onclick="randomizeScalePractice()">🎲 ${t('Losowa tonacja i skala', 'Random key & scale')}</button>
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
                      <button id="scalePracticePreviewBtn" class="chip-btn" style="padding:4px 10px;font-size:11px;color:#4ade80;border-color:#244b33;background:rgba(74,222,128,0.08);white-space:nowrap;"
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

            <!-- Karta 2: Nawigacja po gryfie i przebieg -->
            <div class="sp-card">
              <div class="sp-card-head">
                <h3>2. Układ na gryfie i przebieg (sekwencja)</h3>
                <span style="font-size:11.5px;color:var(--text-muted);">System, 1–3 oktawy, kierunek i wzorzec</span>
              </div>

              <div class="sp-two-col">
                <div>
                  <div class="section-label">PROWADZENIE PO GRYFIE</div>
                  <div class="ear-seg-row" style="margin-bottom:10px;">
                    ${travNames.map((tNm, tIdx) => `
                      <button class="ear-seg-btn ${(state.spTraversal || 0)===tIdx?'active':''}" style="padding:8px 6px;font-size:11.5px;white-space:nowrap;"
                              onclick="state.spTraversal=${tIdx};${tIdx===1?'state.spOctaves=3;':''}saveSettings();renderStageAndInspector()">${tNm}</button>
                    `).join('')}
                  </div>

                  ${(state.spTraversal || 0) === 0 ? `
                    <div class="section-label">SYSTEM POZYCJI</div>
                    <div class="ear-seg-row">
                      ${posNames.map((nm, pIdx) => `
                        <button class="ear-seg-btn ${state.spPosSystem===pIdx?'active':''}" style="padding:8px 6px;white-space:nowrap;"
                                onclick="state.spPosSystem=${pIdx};state.posSystem=${pIdx};state.spPosIdx=0;saveSettings();renderAll()">${pIdx === 0 ? 'Cały gryf' : nm}</button>
                      `).join('')}
                    </div>
                    ${state.spPosSystem > 0 ? `
                      <div style="margin-top:8px;">
                        <div class="section-label">NUMER POZYCJI (${posNames[state.spPosSystem]})</div>
                        <div style="display:grid;grid-template-columns:repeat(${posCountForSys},1fr);gap:5px;">
                          ${Array.from({length: posCountForSys}, (_, idx) => `
                            <button class="chip-btn ${(state.spPosIdx || 0)===idx?'active':''}" style="padding:6px 2px;font-size:11.5px;"
                                    onclick="state.spPosIdx=${idx};state.posIdx=${idx};saveSettings();renderAll()">
                              ${state.spPosSystem === 3 ? cagedLabels[idx] : `#${idx+1}`}
                            </button>
                          `).join('')}
                        </div>
                      </div>
                    ` : ''}
                  ` : `
                    <div style="font-size:11.5px;color:var(--text-muted);background:#130f0c;border:1px solid #26201b;border-radius:8px;padding:9px 11px;line-height:1.4;">
                      ${state.spTraversal === 1
                        ? '↗ <b>Przebieg diagonalny:</b> płynne przechodzenie w górę gryfu (3–4 dźwięki na strunę z shiftem pozycji) aż do wysokich progów — idealne do 3-oktawowych biegów!'
                        : '↔ <b>Jedna struna (poziomo):</b> granie skali wzdłuż wybranej struny od progu 0 do 24 — najlepsze ćwiczenie do opanowania gryfu poziomo.'}
                    </div>
                  `}
                </div>

                <div style="display:flex;flex-direction:column;gap:12px;">
                  <div>
                    <div class="section-label">${state.spTraversal === 2 ? 'STRUNA DO BIEGU POZIOMEGO' : 'STRUNA STARTOWA'}</div>
                    <div class="ear-seg-row">
                      <button class="ear-seg-btn ${state.spStartString===-1?'active':''}" style="padding:8px 6px;font-size:11.5px;white-space:nowrap;"
                              onclick="state.spStartString=-1;saveSettings();renderStageAndInspector()">Dowolna</button>
                      ${stringLabels.map((sLbl, sIdx) => `
                        <button class="ear-seg-btn ${state.spStartString===sIdx?'active':''}" style="padding:8px 5px;font-size:11.5px;white-space:nowrap;"
                                onclick="state.spStartString=${sIdx};saveSettings();renderStageAndInspector()">${sLbl}</button>
                      `).join('')}
                    </div>
                  </div>
                  <div>
                    <div class="section-label">ZAKRES OKTAW</div>
                    <div class="ear-seg-row">
                      <button class="ear-seg-btn ${state.spOctaves===1?'active':''}"
                              onclick="state.spOctaves=1;saveSettings();renderStageAndInspector()">1 oktawa</button>
                      <button class="ear-seg-btn ${state.spOctaves===2?'active':''}"
                              onclick="state.spOctaves=2;saveSettings();renderStageAndInspector()">2 oktawy</button>
                      <button class="ear-seg-btn ${state.spOctaves===3?'active':''}"
                              onclick="state.spOctaves=3;saveSettings();renderStageAndInspector()">3 oktawy</button>
                    </div>
                  </div>
                  <div>
                    <div class="section-label">STREFA PROGÓW</div>
                    <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
                      ${fretZones.map(z => `
                        <button class="chip-btn ${(state.spFretZone || 'all')===z.id?'active':''}" style="padding:6px 4px;font-size:11px;white-space:nowrap;"
                                onclick="state.spFretZone='${z.id}';saveSettings();renderStageAndInspector()">${z.lbl}</button>
                      `).join('')}
                    </div>
                  </div>
                </div>
              </div>

              <div class="sp-two-col">
                <div>
                  <div class="section-label">KIERUNEK</div>
                  <div class="sp-dir-grid">
                    ${dirNames.map((dNm, dIdx) => `
                      <button class="sp-dir-btn ${state.spDirection===dIdx?'active':''}"
                              onclick="state.spDirection=${dIdx};saveSettings();renderStageAndInspector()">${dNm}</button>
                    `).join('')}
                  </div>
                </div>

                <div style="display:flex;flex-direction:column;gap:12px;">
                  <div>
                    <div class="section-label">PRZEBIEG / WZORZEC MELODYCZNY (${allPatterns.length} OPCJI)</div>
                    <select onchange="state.spPattern=parseInt(this.value,10);saveSettings();renderStageAndInspector()">
                      ${patternGroups.map(g => `
                        <optgroup label="${g.label}">
                          ${g.items.map(p => `<option value="${p.code}" ${p.code===state.spPattern?'selected':''}>${p.name}</option>`).join('')}
                        </optgroup>
                      `).join('')}
                    </select>
                    <div style="font-size:11.5px;color:var(--text-muted);margin-top:5px;line-height:1.35;">${activePat.desc}</div>
                  </div>
                </div>
              </div>

              <div class="sp-two-col">
                <div>
                  <div class="section-label">PODZIAŁ RYTMICZNY I TEMPO</div>
                  <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
                    <div class="ear-seg-row" style="flex:1;min-width:210px;">
                      ${rhythmOpts.map(r => `
                        <button class="ear-seg-btn ${activeRhythm.id===r.id?'active':''}" style="padding:7px 6px;font-size:11.5px;white-space:nowrap;"
                                onclick="state.spNoteDur=${r.dur};state.spTuplet=${r.tup};saveSettings();renderStageAndInspector()">${r.lbl}</button>
                      `).join('')}
                    </div>
                    <div style="display:flex;align-items:center;gap:4px;background:#130f0c;border:1px solid #2b241e;border-radius:8px;padding:3px 6px;">
                      <button class="chip-btn" style="padding:3px 7px;" onclick="state.spBpm=Math.max(40,(state.spBpm||100)-5);saveSettings();renderStageAndInspector()">−</button>
                      <strong style="min-width:56px;text-align:center;font-size:12px;color:var(--accent);">${state.spBpm || 100} BPM</strong>
                      <button class="chip-btn" style="padding:3px 7px;" onclick="state.spBpm=Math.min(240,(state.spBpm||100)+5);saveSettings();renderStageAndInspector()">+</button>
                    </div>
                  </div>
                </div>

                <div style="display:grid;grid-template-columns:1fr 1fr;gap:12px;align-items:end;">
                  <div>
                    <div class="section-label">ETYKIETY NA GRYFIE</div>
                    <div class="ear-seg-row">
                      <button class="ear-seg-btn ${state.labelType==='notes'?'active':''}" style="padding:7px 8px;"
                              onclick="state.labelType='notes';saveSettings();renderStageAndInspector()">Nuty</button>
                      <button class="ear-seg-btn ${state.labelType==='intervals'?'active':''}" style="padding:7px 8px;"
                              onclick="state.labelType='intervals';saveSettings();renderStageAndInspector()">Interwały</button>
                    </div>
                  </div>
                  <div style="display:flex;justify-content:space-between;align-items:center;background:#130f0c;border:1px solid #26201b;border-radius:8px;padding:8px 10px;">
                    <span class="section-label" style="margin:0;">START OD TONIKI</span>
                    <button class="ear-toggle-switch ${state.spStartOnRoot!==false?'on':''}"
                            onclick="state.spStartOnRoot=!(state.spStartOnRoot!==false);saveSettings();renderStageAndInspector()"></button>
                  </div>
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
                <span>Układ / Pozycja</span>
                <strong>${activeSystemLbl}</strong>
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
                <span>Zakres / Strefa</span>
                <strong>${state.spOctaves} ${state.spOctaves === 1 ? 'okt.' : 'okt.'} · ${activeZone.min}–${activeZone.max}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Przebieg</span>
                <strong>${activePat.name}</strong>
              </div>
              <div class="sp-summary-pill">
                <span>Rytm i Tempo</span>
                <strong>${activeRhythm.lbl} · ${state.spBpm || 100} BPM</strong>
              </div>
            </div>

            <button class="action-btn" style="width:100%;padding:13px;font-size:15px;"
                    onclick="launchScalePractice('play','ribbon')">🎯 ${t('Graj: Ścieżka skali (wolny gryf)', 'Play: Scale Ribbon (free neck)')}</button>
            <button class="action-btn secondary" style="width:100%;padding:11px;font-size:13.5px;"
                    onclick="launchScalePractice('play','tab')">▶ ${t('Graj z TAB (dokładne progi)', 'Play with TAB (exact frets)')}</button>
            <button class="action-btn secondary" style="width:100%;padding:10px;font-size:13.5px;"
                    onclick="launchScalePractice('listen','ribbon')">🔊 ${t('Posłuchaj przebiegu', 'Listen to run')}</button>
            <button class="chip-btn" style="width:100%;padding:9px 12px;font-size:12.5px;border-color:var(--accent);color:var(--accent);"
                    onclick="launchScalePractice('edit','tab')" title="Otwórz wygenerowaną sekwencję w edytorze TAB / GP5">
              ✏️ ${t('Otwórz jako TAB / GP5', 'Open as TAB / GP5')}
            </button>

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

  } else if (state.subtab === 'exercise' && !state.exercisePlayerActive && state.exerciseSubView === 'harmonics') {
    // =======================================================================
    // PODWIDOK ĆWICZENIA: FLAŻOLETY (GRYF Z FLAŻOLETAMI + PRZYKŁADY PRZEJŚĆ I MELODII)
    // =======================================================================
    const selHarm = HARMONICS_EXERCISES.find(e => e.id === state.selectedHarmonicExerciseId) || HARMONICS_EXERCISES[0];
    const harmFbSvg = buildHarmonicsExerciseFretboardSvg(selHarm);
    const selEvents = buildHarmonicExerciseEvents(selHarm);
    const previewTabsStr = selEvents.map(ev => ev.tech === '◊'
      ? `<span style="color:#f39237;font-weight:800;">${ev.note}&lt;${ev.f}◊&gt;</span>`
      : `<span style="color:var(--text);">${ev.note}(${ev.f})</span>`
    ).join(' <span style="opacity:0.45;">→</span> ');

    stage.innerHTML = `
      <div class="sp-shell" style="display:flex;flex-direction:column;gap:14px;">
        <div class="sp-header">
          <div style="display:flex;align-items:center;gap:14px;flex-wrap:wrap;">
            <h2 style="font-size:21px;font-weight:800;margin:0;color:var(--text);">${t('Ćwiczenie', 'Exercise')}</h2>
            <div class="ear-seg-row" style="width:auto;">
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('composer')">✏️ ${t('Edytor TAB', 'TAB Editor')}</button>
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('scale_practice')">🎯 ${t('Praktyka skal', 'Scale Practice')}</button>
              <button class="ear-seg-btn active" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('harmonics')">✨ ${t('Flażolety', 'Harmonics')}</button>
            </div>
          </div>
          <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
            <button class="chip-btn" style="padding:6px 12px;font-size:12px;" onclick="switchPracticeSubtab('harmonics')">🔍 ${t('Pełna mapa flażoletów', 'Full harmonics map')}</button>
          </div>
        </div>

        <!-- Gryf flażoletowy z podświetlonymi dźwiękami wybranego ćwiczenia -->
        <div class="sp-card" style="gap:10px;">
          <div class="sp-card-head">
            <div>
              <h3 style="margin:0;">✨ ${selHarm.title}</h3>
              <div style="font-size:12px;color:var(--text-muted);margin-top:3px;">${selHarm.desc}</div>
            </div>
            <div style="display:flex;gap:6px;flex-wrap:wrap;">
              <button class="action-btn" style="padding:7px 13px;font-size:12.5px;" onclick="launchHarmonicExercise('${selHarm.id}','play')">🎸 ${t('Graj (z mikrofonem)', 'Play (microphone)')}</button>
              <button class="action-btn secondary" style="padding:7px 13px;font-size:12.5px;" onclick="launchHarmonicExercise('${selHarm.id}','listen')">🔊 ${t('Posłuchaj (Player)', 'Listen (Player)')}</button>
              <button class="chip-btn" style="padding:7px 12px;font-size:12px;border-color:var(--accent);color:var(--accent);" onclick="launchHarmonicExercise('${selHarm.id}','edit')">✏️ ${t('Otwórz w TAB / GP5', 'Open in TAB / GP5')}</button>
            </div>
          </div>

          <div style="background:var(--bg-main);border:1px solid var(--border);border-radius:10px;padding:12px 14px;overflow-x:auto;">
            ${harmFbSvg}
          </div>

          <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;font-size:11.5px;color:var(--text-muted);">
            <span>${t('Kliknij kółko ● (zwykła nuta) lub romb ◇ (flażolet nad progiem), aby usłyszeć brzmienie. Melodie łączą normalną grę z akcentami flażoletowymi!', 'Click a circle ● (normal note) or diamond ◇ (natural harmonic over the fret wire) to hear it. Melodies mix normal notes with harmonic accents!')}</span>
            <span><b style="color:#60a5fa;">●</b> ${t('Zwykłe nuty melodii', 'Normal melody notes')} · <b style="color:#f39237;">◇</b> ${t('Flażolety w melodii', 'Harmonics in melody')} · <b style="color:#10b981;">◇</b> 12/7</span>
          </div>

          <div style="font-family:monospace;font-size:11.5px;color:var(--accent);background:var(--bg-main);border:1px solid var(--border);border-radius:8px;padding:8px 11px;overflow-x:auto;white-space:nowrap;">
            ${previewTabsStr}
          </div>
        </div>

        <!-- Katalog ćwiczeń i prostych melodii flażoletowych -->
        <div class="sp-card" style="gap:10px;">
          <div class="sp-card-head">
            <h3 style="margin:0;">${t('Ćwiczenia, przejścia i proste melodie na flażoletach', 'Harmonics exercises, transitions & melodies')}</h3>
            <span style="font-size:11.5px;color:var(--text-muted);">${HARMONICS_EXERCISES.length} ${t('gotowych przykładów', 'ready presets')}</span>
          </div>
          <div class="harm-ex-grid">
            ${HARMONICS_EXERCISES.map(ex => {
              const active = (ex.id === selHarm.id);
              return `
                <div class="harm-ex-card ${active ? 'active' : ''}" style="cursor:pointer;"
                     onclick="state.selectedHarmonicExerciseId='${ex.id}';renderStageAndInspector()">
                  <div style="display:flex;justify-content:space-between;align-items:flex-start;gap:8px;">
                    <strong style="font-size:13.5px;line-height:1.25;">${ex.title}</strong>
                    <span style="font-size:10.5px;color:#10b981;font-weight:700;white-space:nowrap;">${ex.level} · ${ex.bpm} BPM</span>
                  </div>
                  <div style="font-size:11px;color:var(--accent);font-weight:700;">${ex.fretsLabel} · ${ex.steps.length} ${t('nut', 'notes')}</div>
                  <div style="font-size:11.5px;color:var(--text-muted);line-height:1.38;">${ex.desc}</div>
                  <div style="display:flex;gap:6px;margin-top:auto;padding-top:4px;flex-wrap:wrap;" onclick="event.stopPropagation()">
                    <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;" onclick="previewHarmonicExercise('${ex.id}')">▶ ${t('Podgląd', 'Preview')}</button>
                    <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;color:var(--accent);border-color:var(--accent);" onclick="launchHarmonicExercise('${ex.id}','play')">🎸 ${t('Graj', 'Play')}</button>
                    <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;" onclick="launchHarmonicExercise('${ex.id}','listen')">🔊 ${t('Odsłuch', 'Listen')}</button>
                    <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;" onclick="launchHarmonicExercise('${ex.id}','edit')">✏️ TAB</button>
                  </div>
                </div>`;
            }).join('')}
          </div>
        </div>
      </div>`;

  } else if (state.subtab === 'exercise') {
    // =======================================================================
    // ZAKŁADKA: ĆWICZENIE (KOMPONOWANIE TAB | ODTWARZACZ TAB / ŚCIEŻKA SKALI)
    // =======================================================================
    const hasNotes = state.recordedEvents.length > 0;

    // WIDOK AKTYWNEGO ODTWARZACZA ĆWICZEŃ (TRYB TAB+GRYF LUB TRYB ŚCIEŻKI SKALI ZE ZDJĘCIA 3!)
    if (state.exercisePlayerActive && hasNotes) {
      const totalSteps = state.recordedEvents.length;
      const stepIdx = Math.max(0, Math.min(totalSteps - 1, state.exercisePlayerStepIdx || 0));
      const curEv = state.recordedEvents[stepIdx];
      const labels = getRootLabels();
      const targetLabel = (curEv && !curEv.rest && curEv.midi > 0)
        ? `${curEv.note || labels[((curEv.midi % 12) + 12) % 12]} ${Math.floor(curEv.midi / 12) - 1}`
        : 'Pauza (𝄽)';
      const isRibbonView = (state.exercisePlayerView === 'ribbon');
      const prevScoreScrollTop = document.getElementById('exerciseScoreWrap')?.scrollTop || 0;
      const scoreAvailW = Math.max(720, Math.min(1140, (stage.clientWidth || 1100) - 56));
      const scoreSvg = isRibbonView ? '' : buildExerciseStaffAndTabSvg(stepIdx, scoreAvailW);
      const fbSvg = isRibbonView ? '' : buildExerciseFretboardSvg(stepIdx, false);
      const ribbonHtml = isRibbonView ? buildScaleRibbonPlayerHtml(stepIdx) : '';

      stage.innerHTML = `
        <div style="max-width:1180px;margin:0 auto;width:100%;display:flex;flex-direction:column;gap:16px;padding:6px 0;">
          <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;">
            <div style="display:flex;align-items:center;gap:12px;">
              <button class="chip-btn" style="width:36px;height:36px;font-size:15px;"
                      onclick="toggleExercisePlayerPause()" title="${state.exercisePlayerPaused ? t('Wznów', 'Resume') : t('Pauza', 'Pause')}">
                ${state.exercisePlayerPaused ? '▶' : '⏸'}
              </button>
              <button class="chip-btn" style="width:36px;height:36px;font-size:15px;"
                      onclick="stopExercisePlayer(true)" title="${t('Wróć do ćwiczeń', 'Return to exercises')}">✕</button>
              <h2 style="font-size:20px;font-weight:800;margin:0;">
                ${state.exerciseTitle || (state.exercisePlayerMode === 'listen' ? t('Odsłuch ćwiczenia', 'Listen mode') : t('Zagraj na gitarze', 'Play on guitar'))}
              </h2>
            </div>
            <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
              <div class="ear-seg-row" style="width:auto;">
                <button type="button" class="ear-seg-btn ${!isRibbonView ? 'active' : ''}" style="padding:5px 10px;font-size:11.5px;white-space:nowrap;"
                        onclick="state.exercisePlayerView='tab';saveSettings();renderStageAndInspector()">📜 ${t('TAB + Gryf', 'TAB + Neck')}</button>
                <button type="button" class="ear-seg-btn ${isRibbonView ? 'active' : ''}" style="padding:5px 10px;font-size:11.5px;white-space:nowrap;"
                        onclick="state.exercisePlayerView='ribbon';saveSettings();renderStageAndInspector()">🎯 ${t('Ścieżka skali (wolny gryf)', 'Scale Ribbon (free neck)')}</button>
              </div>
              ${!isRibbonView ? `
                <div class="ear-seg-row" style="width:auto;">
                  <button type="button" class="ear-seg-btn ${(!state.exerciseShowStaff && state.tabCreationEnabled)?'active':''}" style="padding:5px 9px;font-size:11.5px;white-space:nowrap;"
                          onclick="setExerciseScoreViewMode('tab')">🎸 TAB</button>
                  <button type="button" class="ear-seg-btn ${(state.exerciseShowStaff && state.tabCreationEnabled)?'active':''}" style="padding:5px 9px;font-size:11.5px;white-space:nowrap;"
                          onclick="setExerciseScoreViewMode('both')">🎼 ${t('Nuty+TAB', 'Staff+TAB')}</button>
                  <button type="button" class="ear-seg-btn ${(state.exerciseShowStaff && !state.tabCreationEnabled)?'active':''}" style="padding:5px 9px;font-size:11.5px;white-space:nowrap;"
                          onclick="setExerciseScoreViewMode('staff')">🎼 ${t('Nuty', 'Staff')}</button>
                </div>
                ${buildExerciseBarsPerRowControlHtml(true)}
              ` : ''}
              <div class="ear-seg-row" style="width:auto;">
                <button type="button" class="ear-seg-btn ${state.exercisePlayerMode==='play'?'active':''}" style="padding:5px 9px;font-size:11.5px;white-space:nowrap;"
                        onclick="switchExercisePlayerMode('play')">🎤 ${t('Graj', 'Play')}</button>
                <button type="button" class="ear-seg-btn ${state.exercisePlayerMode==='listen'?'active':''}" style="padding:5px 9px;font-size:11.5px;white-space:nowrap;"
                        onclick="switchExercisePlayerMode('listen')">🔊 ${t('Odsłuch', 'Listen')}</button>
              </div>
              <button class="chip-btn ${state.spMetronome?'active':''}" style="padding:6px 10px;font-size:12px;"
                      onclick="toggleExerciseMetronome()" title="Metronom">
                🥁 ${state.spMetronome ? t('Wł.', 'On') : t('Wył.', 'Off')}
              </button>
              <div style="display:inline-flex;align-items:center;gap:4px;background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:3px 6px;font-size:12px;">
                <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)" title="-10 BPM">−10</button>
                <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)" title="-5 BPM">−5</button>
                <input id="exPlayerBpmInput" type="number" min="30" max="260" value="${state.tempoBpm}"
                       style="width:54px;padding:2px 4px;text-align:center;font-weight:800;color:var(--accent);font-size:12.5px;"
                       onchange="setExercisePlayerBpm(this.value)" title="Prędkość (BPM)">
                <span style="font-size:11px;color:var(--text-muted);font-weight:700;">BPM</span>
                <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)" title="+5 BPM">+5</button>
                <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)" title="+10 BPM">+10</button>
              </div>
              <span style="font-family:monospace;font-size:14.5px;font-weight:700;color:var(--text);background:var(--bg-card);padding:5px 11px;border-radius:8px;border:1px solid var(--border);">
                ${stepIdx + 1} / ${totalSteps}
              </span>
            </div>
          </div>

          ${(() => {
            if (state.exercisePlayerMode !== 'play') return '';
            const heard = (state.detectedMidi !== null && state.detectedMidi !== undefined)
              ? `${labels[((Math.round(state.detectedMidi) % 12) + 12) % 12]}${Math.floor(Math.round(state.detectedMidi) / 12) - 1}`
              : null;
            const justHit = (typeof performance !== 'undefined' && (performance.now() - (state.exerciseLastHitTime || 0)) < 1100 && state.exerciseLastHitLabel);
            const curPosTxt = (curEv && !curEv.rest && curEv.s >= 0 && curEv.f >= 0)
              ? `str. ${6 - curEv.s} · pr. ${curEv.f}`
              : '';
            const shapeName = (curEv && curEv.chordSymbol) ? ` · kształt ${curEv.chordSymbol}` : '';
            if (state.exercisePlayerPaused) {
              return `<div style="text-align:center;padding:9px 14px;border-radius:10px;background:var(--bg-card);border:1px solid var(--border);color:var(--text-muted);font-weight:700;">⏸ ${t('Wstrzymano', 'Paused')}</div>`;
            }
            const bannerBg = justHit ? 'rgba(34,197,94,0.16)' : (micStream ? 'var(--accent-glow)' : 'rgba(243,146,55,0.08)');
            const bannerBorder = justHit ? '#22c55e' : 'var(--accent)';
            const bannerColor = justHit ? '#4ade80' : 'var(--accent)';
            return `<div style="display:flex;align-items:center;justify-content:center;gap:12px;flex-wrap:wrap;padding:9px 14px;border-radius:10px;background:${bannerBg};border:1px solid ${bannerBorder};color:${bannerColor};font-weight:800;">
              ${justHit ? `<span style="background:#22c55e;color:#052e16;padding:2px 9px;border-radius:6px;font-size:12px;font-weight:900;">✓ Trafiono: ${state.exerciseLastHitLabel}</span>` : ''}
              <span>🎯 ${t('Oczekiwana nuta', 'Expected')}: <strong style="font-size:18px;">${targetLabel}</strong>${curPosTxt ? ` <span style="font-size:12px;opacity:0.85;font-weight:600;">(${curPosTxt}${shapeName})</span>` : ''}</span>
              ${micStream
                ? `<span id="exerciseHeardBadge" style="color:var(--text-muted);font-weight:600;font-size:12.5px;">${heard ? `(${t('słyszę', 'heard')}: ${heard})` : `(${t('mikrofon aktywny', 'mic active')})`}</span>`
                : `<button class="chip-btn" style="padding:4px 10px;font-size:11.5px;" onclick="toggleLiveAudio().then(()=>renderStageAndInspector())">🎤 ${t('Włącz mikrofon', 'Turn on mic')}</button>`}
            </div>`;
          })()}

          ${isRibbonView ? `
            ${ribbonHtml}
            <div style="display:flex;justify-content:center;align-items:center;gap:10px;">
              <button class="chip-btn" style="padding:7px 14px;" onclick="stepExercisePlayer(-1)">◀ ${t('Poprzednia', 'Previous')}</button>
              <button class="chip-btn" style="padding:7px 14px;" onclick="stepExercisePlayer(1)">${t('Następna', 'Next')} ▶</button>
              <button class="chip-btn" style="padding:7px 14px;" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ ${t('Od początku', 'Restart')}</button>
            </div>
          ` : `
            <div style="display:flex;justify-content:center;align-items:center;gap:12px;">
              <button class="chip-btn" style="padding:8px 14px;" onclick="stepExercisePlayer(-1)">◀ ${t('Poprzednia', 'Previous')}</button>
              <div style="min-width:94px;height:44px;background:var(--bg-card);border:1.5px solid var(--accent);border-radius:10px;display:flex;align-items:center;justify-content:center;font-size:18px;font-weight:800;color:var(--accent);padding:0 14px;">
                ${targetLabel}
              </div>
              <button class="chip-btn" style="padding:8px 14px;" onclick="stepExercisePlayer(1)">${t('Następna', 'Next')} ▶</button>
              <button class="chip-btn" style="padding:8px 14px;" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ ${t('Od początku', 'Restart')}</button>
            </div>

            <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:12px;padding:12px 16px;">
              ${buildExerciseStaffBarHtml()}
              <div id="exerciseScoreWrap" style="max-height:${state.exerciseShowStaff ? 442 : 276}px;overflow-y:auto;overflow-x:hidden;">
                ${scoreSvg}
              </div>
            </div>

            <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:12px;padding:14px 16px;overflow-x:auto;">
              ${fbSvg}
            </div>
          `}
        </div>`;
      const exScoreWrap = document.getElementById('exerciseScoreWrap');
      if (exScoreWrap) {
        exScoreWrap.scrollTop = prevScoreScrollTop;
        const actRect = exScoreWrap.querySelector('[data-ex-active="1"]');
        if (actRect && typeof actRect.scrollIntoView === 'function') {
          actRect.scrollIntoView({ block: 'nearest', behavior: 'smooth' });
        }
      }
      const baseBpm = state.exerciseBaseBpm || state.tempoBpm || 120;
      insp.innerHTML = `
        <div style="background:var(--bg-card);padding:14px;border-radius:12px;border:1px solid var(--border);display:flex;flex-direction:column;gap:10px;">
          <div style="display:flex;justify-content:space-between;align-items:center;">
            <span class="section-label" style="margin:0;">PRĘDKOŚĆ ĆWICZENIA</span>
            <strong id="exPlayerInspBpmLbl" style="font-size:17px;color:var(--accent);">${state.tempoBpm} BPM</strong>
          </div>
          <input type="range" min="30" max="260" value="${state.tempoBpm}" style="width:100%;"
                 oninput="const el=document.getElementById('exPlayerInspBpmLbl');if(el)el.textContent=this.value+' BPM';"
                 onchange="setExercisePlayerBpm(this.value)">
          <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-5)">−5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+5)">+5</button>
            <button class="chip-btn" style="padding:5px 2px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
          </div>
          <div>
            <div class="section-label" style="margin-bottom:4px;">SKRÓTY PRĘDKOŚCI (BAZA ${baseBpm} BPM)</div>
            <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;">
              ${[0.5, 0.75, 1.0, 1.25].map(r => {
                const tBpm = Math.max(30, Math.min(260, Math.round(baseBpm * r)));
                const isActR = Math.abs((state.tempoBpm || 120) - tBpm) <= 1;
                return `<button class="chip-btn ${isActR?'active':''}" style="padding:5px 2px;font-size:11px;"
                                onclick="setExercisePlayerSpeedRatio(${r})">${Math.round(r * 100)}%</button>`;
              }).join('')}
            </div>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB ĆWICZENIA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.exercisePlayerMode==='play'?'active':''}" onclick="switchExercisePlayerMode('play')">🎤 Graj (mikrofon)</button>
            <button class="chip-btn ${state.exercisePlayerMode==='listen'?'active':''}" onclick="switchExercisePlayerMode('listen')">🔊 Odsłuch</button>
          </div>
        </div>

        <div style="display:flex;flex-direction:column;gap:6px;">
          <button class="chip-btn ${state.spMetronome?'active':''}" onclick="toggleExerciseMetronome()">🥁 Metronom: ${state.spMetronome ? 'Włączony' : 'Wyłączony'}</button>
          <button class="chip-btn" onclick="startExercisePlayer('${state.exercisePlayerMode}')">↻ Zacznij od początku</button>
          <button class="chip-btn" onclick="stopExercisePlayer(true)">← Wróć do Edytora TAB</button>
        </div>

        <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:10px;padding:10px;font-size:11.5px;color:var(--text-muted);line-height:1.45;">
          <strong style="color:var(--text);">Legenda gryfu:</strong><br>
          <span style="color:#22c55e;">● Zielony</span> — zaliczone nuty<br>
          <span style="color:#f39237;">◉ Pomarańczowy</span> — oczekiwana teraz<br>
          <span style="color:#38bdf8;">◌ Przerywany niebieski</span> — kolejne arpeggio
        </div>`;
      return;
    }

    // WIDOK ZRZUTU 1: KOMPOZYTOR ĆWICZENIA (TAB -> GRYF -> TABELA NUT)
    const prevComposerScrollEl = document.getElementById('exerciseComposerScoreWrap');
    const prevComposerScrollTop = prevComposerScrollEl ? prevComposerScrollEl.scrollTop : 0;
    const prevCount = state.exLastRenderedCount;
    const shouldScrollToNewest = (prevCount !== state.recordedEvents.length);
    state.exLastRenderedCount = state.recordedEvents.length;

    let tabPreview = { detectedKey: (state.fretboardData && state.fretboardData.scaleFullName) ? state.fretboardData.scaleFullName : 'C Durowa' };
    if (hasNotes) {
      try {
        tabPreview = await fetchJsonPost(`/api/tab_preview?bpm=${state.tempoBpm}`, state.recordedEvents);
      } catch (e) {}
    }
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
      { code: 'S', label: 'S (Slide)' },
      { code: '/', label: '/ (Slide alt.)' },
      { code: 'B', label: 'B (Podciągnięcie)' },
      { code: '~', label: '~ (Wibrato)' },
      { code: 'PM', label: 'PM (Tłumienie)' },
      { code: '◊', label: '◊ (Flażolet)' },
      { code: 'T', label: 'T (Tapping)' }
    ];
    const composerAvailW = Math.max(660, (stage.clientWidth || 960) - 56);
    const scoreSvg = buildExerciseStaffAndTabSvg(-1, composerAvailW);
    const exFretSvg = buildExerciseFretboardSvg(hasNotes ? (state.recordedEvents.length - 1) : -1, true);
    const rootLabels = getRootLabels();
    const twoRowsMaxH = state.exerciseShowStaff ? 442 : 276;

    stage.innerHTML = `
      <div style="display:flex;flex-direction:column;gap:14px;">
        <!-- Górny przełącznik podwidoków w Ćwiczeniu [ Edytor TAB | Praktyka skal | Flażolety ] -->
        <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:12px;">
          <div style="display:flex;align-items:center;gap:14px;flex-wrap:wrap;">
            <h2 style="font-size:21px;font-weight:800;margin:0;">${t('Ćwiczenie', 'Exercise')}</h2>
            <div class="ear-seg-row" style="width:auto;">
              <button class="ear-seg-btn active" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('composer')">✏️ ${t('Edytor TAB', 'TAB Editor')}</button>
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('scale_practice')">🎯 ${t('Praktyka skal', 'Scale Practice')}</button>
              <button class="ear-seg-btn" style="padding:7px 14px;white-space:nowrap;" onclick="setExerciseSubView('harmonics')">✨ ${t('Flażolety', 'Harmonics')}</button>
            </div>
          </div>
          <div style="display:flex;gap:6px;flex-wrap:wrap;">
            <button class="chip-btn" style="padding:6px 12px;" onclick="undoLastExerciseEvent()">⌫ Cofnij [Backspace]</button>
            <button class="chip-btn" style="padding:6px 12px;" onclick="state.recordedEvents=[];renderStageAndInspector()">🗑️ Wyczyść</button>
            <button class="action-btn" style="padding:6px 12px;" onclick="downloadTabExport('gp5')">💾 .GP5 (TuxGuitar)</button>
            <button class="action-btn secondary" style="padding:6px 12px;" onclick="downloadTabExport('mid')">💾 .MID</button>
          </div>
        </div>

        <div style="display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:8px;margin-top:2px;">
          <h3 style="font-size:20px;font-weight:800;margin:0;">Twoje ćwiczenie</h3>
          <div style="display:flex;align-items:center;gap:6px;flex-wrap:wrap;">
            <div style="display:inline-flex;align-items:center;gap:4px;background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:3px 6px;font-size:12px;">
              <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)-10)">−10</button>
              <input id="exComposerBpmInput" type="number" min="30" max="260" value="${state.tempoBpm}"
                     style="width:52px;padding:2px 4px;text-align:center;font-weight:800;color:var(--accent);font-size:12px;"
                     onchange="setExercisePlayerBpm(this.value)" title="Prędkość (BPM)">
              <span style="font-size:11px;color:var(--text-muted);font-weight:700;">BPM</span>
              <button class="chip-btn" style="padding:3px 7px;font-size:11px;" onclick="setExercisePlayerBpm((state.tempoBpm||120)+10)">+10</button>
            </div>
            ${hasNotes ? `
              <button class="action-btn" style="padding:5px 11px;font-size:12px;" onclick="startExercisePlayer('play')">🎤 Graj</button>
              <button class="chip-btn" style="padding:5px 10px;font-size:12px;" onclick="startExercisePlayer('listen')">🔊 Posłuchaj</button>
              <button class="chip-btn" style="padding:4px 9px;font-size:11px;" onclick="scrollExerciseTabTo('top')" title="Przewiń tabulaturę do początku (takt 1)">⬆ Początek</button>
              <button class="chip-btn" style="padding:4px 9px;font-size:11px;" onclick="scrollExerciseTabTo('bottom')" title="Przewiń do najnowszych taktów">⬇ Najnowsze</button>
            ` : ''}
            ${buildExerciseBarsPerRowControlHtml(false)}
          </div>
        </div>

        <!-- Karta tabulatury graficznej (max 2 widoczne linie + auto-scroll do najnowszych) + rozwijany pasek nut -->
        <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:12px;padding:12px 16px;">
          ${buildExerciseStaffBarHtml()}
          <div id="exerciseComposerScoreWrap" style="overflow-x:hidden;overflow-y:auto;max-height:${twoRowsMaxH}px;">
            ${!hasNotes ? `
              <div style="padding:10px 6px;">
                <div style="font-weight:700;font-size:15px;margin-bottom:4px;">Brak dźwięków w tabulaturze</div>
                <div style="font-size:13px;color:var(--text-muted);">
                  Dotknij progu na gryfie poniżej, użyj klawiatury (<b>Q W E R T Y U...</b>) lub kliknij <b>„Dodaj nutę”</b>.
                </div>
              </div>
            ` : scoreSvg}
          </div>
        </div>

        <!-- Interaktywny gryf bezpośrednio pod pięciolinią (Zrzut 1) -->
        <div>
          <div class="section-label" style="margin-bottom:6px;">DOTKNIJ PODSTRUNNICY, BY DODAĆ NUTY (PROGI ${state.exerciseMinFret || 0}–${state.exerciseMaxFret || 24})</div>
          <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:10px;padding:10px 12px;overflow-x:auto;">
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

        <!-- Kompaktowa 2-kolumnowa lista nut (2 nuty w jednym rzędzie — wolniejsze przewijanie!) -->
        ${hasNotes ? `
          <div style="background:#15110e;border:1px solid var(--border);border-radius:10px;padding:8px 10px;">
            <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;font-size:11px;color:var(--text-muted);">
              <span><b>LISTA NUT W ĆWICZENIU (${state.recordedEvents.length})</b> · najnowsze na górze (2 kolumny)</span>
              <span>Str. / Próg / Czas / Typ / Technika</span>
            </div>
            <div class="ex-notes-grid">
              ${state.recordedEvents.map((ev, idx) => ({ ev, idx })).reverse().map(({ ev, idx }) => {
                const isRest = !!ev.rest;
                const strNum = isRest ? 6 : (6 - (ev.s !== undefined ? ev.s : 0));
                const fretNum = isRest ? 0 : (ev.f !== undefined ? ev.f : 0);
                const noteNm = isRest ? '𝄽' : (ev.note || (ev.midi > 0 ? rootLabels[((ev.midi % 12) + 12) % 12] : '—'));
                const curTech = ev.tech || '-';
                return `
                  <div class="ex-note-card ${idx === state.recordedEvents.length - 1 ? 'ex-active-row' : ''}">
                    <span style="color:var(--text-muted);font-family:monospace;min-width:22px;">#${idx + 1}</span>
                    <strong style="color:${isRest?'var(--text-muted)':'var(--accent)'};min-width:26px;text-align:center;">${noteNm}</strong>
                    <select title="Struna" style="width:42px;" ${isRest?'disabled':''}
                            onchange="updateExerciseRow(${idx}, 'strNum', parseInt(this.value,10))">
                      ${[1,2,3,4,5,6].map(sn => `<option value="${sn}" ${sn===strNum?'selected':''}>S${sn}</option>`).join('')}
                    </select>
                    <input type="number" title="Próg" min="0" max="24" value="${fretNum}" style="width:42px;" ${isRest?'disabled':''}
                           onchange="updateExerciseRow(${idx}, 'fret', parseInt(this.value,10))">
                    <select title="Czas trwania" style="flex:1;min-width:58px;" onchange="updateExerciseRow(${idx}, 'dur', parseInt(this.value,10))">
                      ${durOptions.map(d => `<option value="${d.code}" ${ev.dur===d.code?'selected':''}>${d.label.replace(/\s*\(.*\)/,'')}</option>`).join('')}
                    </select>
                    <select title="Typ" style="width:58px;" onchange="updateExerciseRow(${idx}, 'rest', parseInt(this.value,10))">
                      <option value="0" ${!isRest?'selected':''}>Nuta</option>
                      <option value="1" ${isRest?'selected':''}>Pauza</option>
                    </select>
                    <select title="Technika" style="width:60px;" ${isRest?'disabled':''} onchange="updateExerciseRow(${idx}, 'tech', this.value)">
                      ${techOptions.map(t => `<option value="${t.code}" ${curTech===t.code?'selected':''}>${t.label}</option>`).join('')}
                    </select>
                    <span style="display:flex;gap:2px;margin-left:auto;flex-shrink:0;">
                      <button class="chip-btn" style="padding:2px 5px;font-size:10.5px;" ${idx >= state.recordedEvents.length - 1 ? 'disabled' : ''}
                              onclick="moveExerciseEvent(${idx}, 1)" title="Przesuń wyżej">↑</button>
                      <button class="chip-btn" style="padding:2px 5px;font-size:10.5px;" ${idx <= 0 ? 'disabled' : ''}
                              onclick="moveExerciseEvent(${idx}, -1)" title="Przesuń niżej">↓</button>
                      <button class="chip-btn" style="padding:2px 5px;font-size:10.5px;color:#ef4444;"
                              onclick="deleteExerciseEvent(${idx})" title="Usuń dźwięk">×</button>
                    </span>
                  </div>`;
              }).join('')}
            </div>
          </div>
          <div>
            <button class="chip-btn" style="padding:10px 18px;font-weight:700;" onclick="saveExerciseToWorkshop()">
              Zapisz w Warsztacie
            </button>
          </div>
        ` : ''}
      </div>`;

    const compScoreWrap = document.getElementById('exerciseComposerScoreWrap');
    if (compScoreWrap) {
      if (shouldScrollToNewest) {
        compScoreWrap.scrollTop = compScoreWrap.scrollHeight;
      } else {
        compScoreWrap.scrollTop = prevComposerScrollTop;
      }
    }

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
        <div class="section-label">WIDOK ZAPISU (NUTY / TAB)</div>
        <div class="ear-seg-row">
          <button class="ear-seg-btn ${(!state.exerciseShowStaff && state.tabCreationEnabled)?'active':''}" style="padding:8px 6px;font-size:11.5px;white-space:nowrap;"
                  onclick="setExerciseScoreViewMode('tab')">🎸 Tylko TAB</button>
          <button class="ear-seg-btn ${(state.exerciseShowStaff && state.tabCreationEnabled)?'active':''}" style="padding:8px 6px;font-size:11.5px;white-space:nowrap;"
                  onclick="setExerciseScoreViewMode('both')">🎼 Nuty + TAB</button>
          <button class="ear-seg-btn ${(state.exerciseShowStaff && !state.tabCreationEnabled)?'active':''}" style="padding:8px 6px;font-size:11.5px;white-space:nowrap;"
                  onclick="setExerciseScoreViewMode('staff')">🎼 Tylko Nuty</button>
        </div>
      </div>

      <div>
        <div class="section-label">TAKTÓW W WIERSZU (ZAWIJANIE)</div>
        <div style="display:grid;grid-template-columns:1.3fr repeat(4, 1fr);gap:4px;">
          ${[0, 2, 4, 6, 8].map(n => `
            <button class="chip-btn ${(state.exerciseBarsPerRow || 0) === n ? 'active' : ''}" style="padding:7px 2px;font-size:11.5px;"
                    onclick="setExerciseBarsPerRow(${n})">${n === 0 ? 'Auto' : n}</button>
          `).join('')}
        </div>
      </div>

      <div>
        <div class="section-label">KLUCZ (GDY ROZWINIĘTO NUTY)</div>
        <div class="btn-grid-3">
          <button class="chip-btn ${state.exerciseClef==='treble'?'active':''}" style="padding:7px 2px;font-size:11px;"
                  onclick="state.exerciseClef='treble';saveSettings();renderStageAndInspector()">Wiolinowy</button>
          <button class="chip-btn ${state.exerciseClef==='bass'?'active':''}" style="padding:7px 2px;font-size:11px;"
                  onclick="state.exerciseClef='bass';saveSettings();renderStageAndInspector()">Basowy</button>
          <button class="chip-btn ${state.exerciseClef==='auto'?'active':''}" style="padding:7px 2px;font-size:11px;"
                  onclick="state.exerciseClef='auto';saveSettings();renderStageAndInspector()">Auto</button>
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

function setChordCapoFret(fret) {
  state.chordCapoFret = Math.max(0, Math.min(12, parseInt(fret, 10) || 0));
  state.selectedChordVoicingIdx = 0;
  saveSettings();
  renderStageAndInspector();
}

function launchWorkshopSession(idx) {
  const ses = state.initData && state.initData.sessions ? state.initData.sessions[idx] : null;
  if (!ses) return;
  if (ses.type === 'changes' && ses.changesSnapshot) {
    const snap = ses.changesSnapshot;
    stopChangesPlayback();
    if (snap.progressionIdx !== undefined) state.selectedProgressionIdx = snap.progressionIdx;
    if (snap.keyRootPc !== undefined) state.changesKeyRootPc = snap.keyRootPc;
    if (snap.isMinor !== undefined) state.changesIsMinor = !!snap.isMinor;
    if (snap.bpm !== undefined) state.changesBpm = snap.bpm;
    if (snap.timeSig) state.changesTimeSig = snap.timeSig;
    if (snap.feel) state.changesFeel = snap.feel;
    if (snap.compInst) state.changesCompInst = snap.compInst;
    if (typeof snap.attackStrength === 'number' && Number.isFinite(snap.attackStrength)) {
      state.changesAttackStrength = Math.max(0.20, Math.min(1.0, snap.attackStrength));
    }
    if (snap.groove) state.changesGroove = snap.groove;
    if (snap.repeat) state.changesRepeat = snap.repeat;
    if (snap.practiceGoal) state.changesPracticeGoal = snap.practiceGoal;
    if (Array.isArray(snap.measures) && snap.measures.length > 0) {
      state.changesMeasures = JSON.parse(JSON.stringify(snap.measures));
    }
    state.changesEditingSlot = null;
    state.changesScreen = 'editor';
    const nav = document.getElementById('navChangesBtn');
    switchMainView('changes', nav);
    return;
  }
  launchWorkshopExercise(state.customWorkshopExercises[0]);
}

function deleteWorkshopSession(idx) {
  if (!state.initData || !Array.isArray(state.initData.sessions)) return;
  const removed = state.initData.sessions.splice(idx, 1)[0];
  if (removed && removed.id && Array.isArray(state.customSavedSessions)) {
    state.customSavedSessions = state.customSavedSessions.filter(s => s.id !== removed.id);
    saveSettings();
  }
  renderStageAndInspector();
}

function selectAndPlayChordCard(el, idx, freqs) {
  stopCurrentScalePreview(false);
  state.selectedChordVoicingIdx = idx;
  document.querySelectorAll('.chord-box-card').forEach(c => c.classList.remove('selected'));
  if (el) el.classList.add('selected');
  renderFretboardToolbarForActiveTab();
  drawFretboard();
  const myToken = previewSessionToken;
  const gainScale = 1.0 / Math.sqrt(Math.max(1, (freqs || []).length));
  (freqs || []).forEach((f, i) => {
    const tid = setTimeout(() => {
      if (previewSessionToken !== myToken) return;
      pluckGuitarFrequency(f, 1.5, gainScale, true);
    }, i * 45);
    previewTimers.push(tid);
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

// =========================================================================
// TRYB „WYKRYJ SKALĘ" — zbiera grane klasy wysokości i znajduje pasujące skale
// =========================================================================
let detectScalePollTimer = null;

function stopDetectScalePolling() {
  if (detectScalePollTimer) { clearInterval(detectScalePollTimer); detectScalePollTimer = null; }
}

function ensureDetectScalePolling() {
  if (detectScalePollTimer) return;
  refreshDetectScale();
  detectScalePollTimer = setInterval(refreshDetectScale, 800);
}

function clearDetectScalePcs() {
  state.detectScalePcs = [];
  state.detectScaleData = null;
  refreshDetectScale();
}

async function refreshDetectScale() {
  if (state.mainView !== 'practice' || state.subtab !== 'search' || state.searchDir !== 'detect') return;
  const recMidis = getRecognitionMidis();
  const pcs = new Set();
  for (const m of recMidis) pcs.add(((Math.round(m) % 12) + 12) % 12);
  const pcArr = [...pcs].sort((a, b) => a - b);
  state.detectScalePcs = pcArr;
  const rootHint = recMidis.length ? (((Math.round(recMidis[0]) % 12) + 12) % 12) : -1;

  const pcsEl = document.getElementById('detectScalePcsLabel');
  if (pcsEl) pcsEl.textContent = pcArr.length ? pcArr.map(p => getRootLabels()[p]).join('  ') : '—';

  const listEl = document.getElementById('detectScaleList');
  if (pcArr.length === 0) {
    if (listEl) listEl.innerHTML = '<div style="color:var(--text-muted);font-size:12px;padding:4px 2px;">Zagraj dźwięki / akord, aby wykryć skalę…</div>';
    return;
  }
  const mode = state.detectScaleMode || 'all';
  const data = await fetchJson(`/api/detect-scale?pcs=${pcArr.join(',')}&mode=${mode}&rootHint=${rootHint}&accMode=${state.accMode}`);
  state.detectScaleData = data;
  if (listEl) {
    const scales = (data && data.scales) || [];
    listEl.innerHTML = scales.length === 0
      ? '<div style="color:var(--text-muted);font-size:12px;padding:4px 2px;">Brak pasującej skali.</div>'
      : scales.slice(0, 30).map(s =>
          `<div class="list-row" style="margin-bottom:5px;cursor:default;">
             <strong>${s.scaleName}</strong>
             <span style="color:var(--text-muted);margin-left:6px;">${s.category}</span>
           </div>`).join('');
  }
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

function isSweepTransitionActive() {
  return (state.subtab === 'sweep' && state.sweepViewMode === 'transition');
}

function syncSweepSegCardSelection() {
  document.querySelectorAll('#dynamicStagePanel [data-sweep-seg]').forEach(card => {
    const cardSeg = parseInt(card.getAttribute('data-sweep-seg'), 10);
    card.classList.toggle('selected', cardSeg === state.arpActiveSegIdx);
  });
}

function syncSweepCompCardSelection() {
  document.querySelectorAll('#dynamicStagePanel [data-sweep-comp-seg]').forEach(card => {
    const cardSeg = parseInt(card.getAttribute('data-sweep-comp-seg'), 10);
    card.classList.toggle('selected', cardSeg === state.sweepCompActiveIdx);
  });
}

function scrollSvgTabWrapToSegment(wrapId, steps, segIdx, smooth = false) {
  const wrap = document.getElementById(wrapId);
  if (!wrap || !Array.isArray(steps)) return;
  const firstIdx = steps.findIndex(st => st.seg === segIdx);
  if (firstIdx < 0) return;
  const svg = wrap.querySelector('svg');
  const g = svg ? svg.querySelector(`g[data-step-g="${firstIdx}"]`) : null;
  if (!g) return;
  const nx = parseFloat(g.getAttribute('data-step-x')) || 0;
  scrollTabContainerToX(wrap, nx, smooth);
}

function scrollArpTabToSegment(segIdx, smooth = false) {
  scrollSvgTabWrapToSegment('arpTabSvgWrap', getActiveArpeggioSteps(), segIdx, smooth);
}

function scrollSweepCompTabToSegment(segIdx, smooth = false) {
  scrollSvgTabWrapToSegment('sweepCompTabSvgWrap', getSweepCompositionSteps(), segIdx, smooth);
}

function toggleArpOrSweepLoop() {
  if (state.subtab === 'sweep') {
    state.sweepSequential = !state.sweepSequential;
  } else {
    state.arpLoop = !state.arpLoop;
  }
  saveSettings();
  renderStageAndInspector();
}

function stringLabelShort(s) {
  return ['6(E)', '5(A)', '4(D)', '3(G)', '2(B)', '1(e)'][s] || String(6 - s);
}

function normalizeChordId(id) {
  if (id === 'maj') return 'major';
  if (id === 'min' || id === 'm') return 'minor';
  return id || 'major';
}

const ARP_DIATONIC_CATEGORY_SETS = {
  triad: new Set(['major', 'minor', 'dim', 'aug']),
  seventh: new Set(['maj7', '7', 'm7', 'mMaj7', 'm7b5', 'dim7', 'aug7', 'augMaj7']),
  sus_add_6: new Set(['sus2', 'sus4', '5', '6', 'm6', 'add9', 'madd9', '7sus4']),
  extended: new Set(['9', 'maj9', 'm9', '11', 'm11', 'maj7#11', '13', 'maj13', 'm13'])
};

function filterPossibleByArpCategory(possibleList, cat = 'all') {
  const list = Array.isArray(possibleList) ? possibleList : [];
  if (!cat || cat === 'all') return list;
  const set = ARP_DIATONIC_CATEGORY_SETS[cat];
  if (!set) return list;
  return list.filter(p => set.has(normalizeChordId(p.id)));
}

function setArpDiatonicCategory(cat) {
  const allowed = ['all', 'triad', 'seventh', 'sus_add_6', 'extended'];
  state.arpDiatonicCategory = allowed.includes(cat) ? cat : 'all';
  if (state.arpDiatonicCategory === 'triad') state.arpDiatonicSevenths = false;
  else if (state.arpDiatonicCategory === 'seventh') state.arpDiatonicSevenths = true;
  saveSettings();
  renderStageAndInspector();
}

function syncArpScaleFromMainScaleTab() {
  state.arpDiatonicScaleRootPc = state.rootPc;
  state.arpDiatonicScaleId = state.scaleId || 'major';
  saveSettings();
  renderAll();
}

function toggleArpAutoTempo() {
  state.arpAutoTempo = !state.arpAutoTempo;
  const btn = document.getElementById('arpAutoTempoBtn');
  if (btn) {
    btn.classList.toggle('active', !!state.arpAutoTempo);
    btn.textContent = `🚀 Auto-Tempo: ${state.arpAutoTempo ? '+4 BPM' : 'Wył.'}`;
  }
  saveSettings();
}

function bumpArpeggioAutoTempo() {
  if (!state.arpAutoTempo) return false;
  state.arpBpm = Math.min(240, (state.arpBpm || 90) + 4);
  const el = document.getElementById('arpBpmVal');
  if (el) el.textContent = `${state.arpBpm} BPM`;
  return true;
}

function saveCurrentSweepCompositionPreset(customName) {
  const compList = Array.isArray(state.sweepComposition) ? state.sweepComposition : [];
  if (!compList.length) return null;
  if (!Array.isArray(state.savedSweepCompositions)) state.savedSweepCompositions = [];
  const defaultTitle = `${compList.map(c => c.symbol).join(' → ')} (${compList.length})`;
  const title = (typeof customName === 'string' && customName.trim()) ? customName.trim() : defaultTitle;
  const preset = {
    id: 'swp_' + Date.now() + '_' + Math.floor(Math.random() * 1000),
    name: title,
    createdAt: Date.now(),
    items: compList.map(seg => Object.assign({}, seg, {
      nodes: Array.isArray(seg.nodes) ? seg.nodes.map(n => Object.assign({}, n)) : [],
      steps: Array.isArray(seg.steps) ? seg.steps.map(st => Object.assign({}, st)) : []
    }))
  };
  state.savedSweepCompositions.unshift(preset);
  if (state.savedSweepCompositions.length > 16) state.savedSweepCompositions.length = 16;
  saveSettings();
  renderStageAndInspector();
  return preset;
}

function rebuildSweepSegmentSteps(seg) {
  if (!seg || !Array.isArray(seg.nodes) || !seg.nodes.length || (seg.seqStyle || 0) !== 0) return seg;
  const prevDir = state.arpDirection;
  const prevFit = state.sweepMetricFit;
  const prevSeq = state.arpSeqStyle;
  state.arpDirection = seg.direction || 0;
  state.sweepMetricFit = normalizeSweepMetricFit(seg.metricFit);
  state.arpSeqStyle = 0;
  try {
    const newSteps = buildCustomSingleSweepSteps(seg.nodes, seg.steps || []);
    if (Array.isArray(newSteps) && newSteps.length > 0) {
      seg.steps = newSteps;
      seg.metricFit = state.sweepMetricFit;
      seg.isSextuplet = isSweepSextupletCount(newSteps.length, state.sweepMetricFit);
      seg.rangeLabel = formatSweepRangeLabel(newSteps, state.sweepMetricFit, state.arpDirection);
    }
  } finally {
    state.arpDirection = prevDir;
    state.sweepMetricFit = prevFit;
    state.arpSeqStyle = prevSeq;
  }
  return seg;
}

function loadSweepCompositionPreset(idx) {
  const list = Array.isArray(state.savedSweepCompositions) ? state.savedSweepCompositions : [];
  const preset = list[idx];
  if (!preset || !Array.isArray(preset.items) || !preset.items.length) return;
  stopArpeggioPlayback(false);
  state.sweepComposition = preset.items.map(seg => rebuildSweepSegmentSteps(Object.assign({}, seg, {
    nodes: Array.isArray(seg.nodes) ? seg.nodes.map(n => Object.assign({}, n)) : [],
    steps: Array.isArray(seg.steps) ? seg.steps.map(st => Object.assign({}, st)) : []
  })));
  state.sweepCompActiveIdx = 0;
  state.sweepCompactTabs = true;
  saveSettings();
  renderStageAndInspector();
}

function deleteSweepCompositionPreset(idx) {
  if (!Array.isArray(state.savedSweepCompositions) || idx < 0 || idx >= state.savedSweepCompositions.length) return;
  state.savedSweepCompositions.splice(idx, 1);
  saveSettings();
  renderStageAndInspector();
}

function buildArpDiatonicSectionHtml(data, isSweepMode, isSweepSingleMode) {
  const cat = state.arpDiatonicCategory || 'all';
  const curChordNorm = normalizeChordId(state.chordId);
  const cats = [
    { id: 'all', lbl: 'Wszystkie w skali' },
    { id: 'triad', lbl: 'Trójdźwięki' },
    { id: 'seventh', lbl: 'Septymowe' },
    { id: 'sus_add_6', lbl: 'Sus / 6 / Add' },
    { id: 'extended', lbl: 'Rozszerzone (9–13)' }
  ];
  const headingTxt = isSweepMode
    ? `AKORDY DIATONICZNE DO SWEEP W SKALI (${data.scaleFullName}) — ${isSweepSingleMode ? 'KLIKNIJ LUB DODAJ +' : 'KLIKNIJ, ABY ZAŁADOWAĆ'}`
    : `ARPEGGIO DIATONICZNE W SKALI (${data.scaleFullName}) — WSZYSTKIE MOŻLIWE NA STOPNIACH`;

  return `
    <div>
      <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:6px;margin-bottom:6px;">
        <div class="section-label" style="margin:0;">${headingTxt}</div>
        <div style="display:flex;gap:5px;align-items:center;flex-wrap:wrap;">
          ${isSweepMode ? `<button class="chip-btn" style="padding:3px 10px;font-size:11px;border-color:var(--accent);color:var(--accent);" onclick="launchDiatonicScaleSweep()">⚡ Wspinaczka po skali</button>` : ''}
          ${cats.map(c => `
            <button class="chip-btn ${cat===c.id?'active':''}" style="padding:3px 8px;font-size:11px;"
                    onclick="setArpDiatonicCategory('${c.id}')">${c.lbl}</button>
          `).join('')}
        </div>
      </div>
      <div style="display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:7px;">
        ${(data.diatonic || []).map(d => {
          const fallbackObj = { id: normalizeChordId(d.chordId), symbol: d.symbol, name: '', category: '' };
          const allPossible = (Array.isArray(d.possible) && d.possible.length > 0) ? d.possible : [fallbackObj];
          const filtered = filterPossibleByArpCategory(allPossible, cat);
          let primary = filtered[0] || null;
          if (cat === 'all') {
            primary = filtered.find(p => normalizeChordId(p.id) === normalizeChordId(d.chordId)) || primary;
          }
          const isCurDeg = (d.rootPc === state.rootPc);
          const isCurPrimary = isCurDeg && primary && (normalizeChordId(primary.id) === curChordNorm);
          const isAnyOnDegSelected = isCurDeg && filtered.some(p => normalizeChordId(p.id) === curChordNorm);
          const clickPrimaryId = primary ? primary.id : d.chordId;
          const secondaryPills = primary ? filtered.filter(p => p !== primary) : [];

          return `
            <div class="chord-box-card ${isAnyOnDegSelected?'selected':''}" style="padding:8px 9px;gap:4px;align-items:flex-start;text-align:left;"
                 onclick="selectDiatonicArpeggio(${d.rootPc}, '${clickPrimaryId}')">
              <div style="display:flex;justify-content:space-between;align-items:center;width:100%;gap:4px;">
                <span style="font-size:10.5px;color:var(--text-muted);">Stopień ${d.roman}</span>
                ${(isSweepSingleMode && primary) ? `
                  <button class="chip-btn" style="padding:1px 7px;font-size:12px;font-weight:900;line-height:1.15;border-color:var(--accent);color:var(--accent);"
                          onclick="event.stopPropagation();quickAddDiatonicSweepToComposition(${d.rootPc}, '${isAnyOnDegSelected ? curChordNorm : primary.id}')"
                          title="Dodaj do kompozycji poniżej">+</button>
                ` : ''}
              </div>
              <div style="font-size:16px;font-weight:800;color:${isCurPrimary?'var(--accent)':'var(--text)'};">
                ${primary ? primary.symbol : '—'}
              </div>
              ${secondaryPills.length > 0 ? `
                <div style="display:flex;flex-wrap:wrap;gap:3px;width:100%;margin-top:2px;" onclick="event.stopPropagation()">
                  ${secondaryPills.map(p => {
                    const pAct = isCurDeg && (normalizeChordId(p.id) === curChordNorm);
                    return `<button class="chip-btn ${pAct?'active':''}" style="padding:1.5px 6px;font-size:10px;line-height:1.25;"
                                    onclick="selectDiatonicArpeggio(${d.rootPc}, '${p.id}')"
                                    title="${p.name || p.id}">${p.symbol}</button>`;
                  }).join('')}
                </div>
              ` : `<div style="font-size:10px;color:var(--text-muted);">${primary ? (primary.name || '') : 'Brak w tej kategorii'}</div>`}
            </div>`;
        }).join('')}
      </div>
    </div>`;
}

function buildArpScaleCohesionInspectorHtml({ data, rootLabels, quickArpTypes, groupedChordsHtml, possibleForRootList, possibleIdSet, scaleDegMap, isSweepMode }) {
  const curChordNorm = normalizeChordId(state.chordId);
  const rootBtnTitle = isSweepMode ? 'PRYMA KSZTAŁTU SWEEP' : 'PRYMA ARPEGGIO';
  const typeTitle = isSweepMode ? 'TYP AKORDU SWEEP' : 'TYP ARPEGGIO (SZYBKI WYBÓR)';
  const list = Array.isArray(possibleForRootList) ? possibleForRootList : [];

  return `
    <div>
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:5px;">
        <div class="section-label" style="margin:0;">${rootBtnTitle}</div>
        <span style="font-size:10px;color:var(--green);font-weight:600;">zielone = w skali</span>
      </div>
      <div class="btn-grid-6">
        ${rootLabels.map((lbl, pc) => {
          const degRoman = scaleDegMap.get(pc);
          const inScale = !!degRoman;
          const isAct = (state.rootPc === pc);
          const dimStyle = (state.arpFilterByScale && !inScale) ? 'opacity:0.32;' : '';
          const borderStyle = (inScale && !isAct) ? 'border-color:rgba(74,222,128,0.48);' : '';
          return `
            <button class="chip-btn ${isAct?'active':''}" style="padding:5px 2px;line-height:1.15;display:flex;flex-direction:column;align-items:center;gap:1px;${dimStyle}${borderStyle}"
                    onclick="stopArpeggioPlayback(false);state.rootPc=${pc};saveSettings();renderAll()"
                    title="${inScale ? `Stopień ${degRoman} w skali ${data.scaleFullName}` : `Spoza skali ${data.scaleFullName}`}">
              <span style="font-size:12px;font-weight:700;">${lbl}</span>
              ${inScale ? `<span style="font-size:9px;font-weight:700;color:${isAct?'inherit':'var(--green)'};">${degRoman}</span>` : `<span style="font-size:9px;opacity:0.35;">·</span>`}
            </button>`;
        }).join('')}
      </div>
    </div>

    <div>
      <div class="section-label">${typeTitle}</div>
      <div style="display:grid;grid-template-columns:repeat(3,1fr);gap:5px;margin-bottom:7px;">
        ${quickArpTypes.map(qt => {
          const isFit = possibleIdSet.has(qt.id);
          const isAct = (curChordNorm === normalizeChordId(qt.id));
          const dimStyle = (!isFit && state.arpFilterByScale) ? 'opacity:0.36;' : (!isFit ? 'opacity:0.65;' : '');
          return `
            <button class="chip-btn ${isAct?'active':''}" style="padding:5px 4px;font-size:11.5px;${dimStyle}"
                    onclick="stopArpeggioPlayback(false);state.chordId='${qt.id}';saveSettings();renderStageAndInspector()">
              ${qt.lbl}${isFit ? ' <span style="font-size:9.5px;color:' + (isAct ? 'inherit' : 'var(--green)') + ';">✓</span>' : ''}
            </button>`;
        }).join('')}
      </div>
      <select onchange="stopArpeggioPlayback(false);state.chordId=this.value;saveSettings();renderStageAndInspector();">
        ${groupedChordsHtml}
      </select>
    </div>

    <div style="background:rgba(255,255,255,0.02);border:1px solid var(--border);border-radius:9px;padding:8px 9px;display:flex;flex-direction:column;gap:6px;">
      <div class="section-label" style="margin:0;color:var(--green);">
        💡 MOŻLIWE ARPEGGIA W SKALI (${list.length}) — ${rootLabels[state.rootPc]}
      </div>
      ${list.length > 0 ? `
        <div style="display:flex;flex-wrap:wrap;gap:4px;">
          ${list.map(p => {
            const pAct = (normalizeChordId(p.id) === curChordNorm);
            return `
              <button class="chip-btn ${pAct?'active':''}" style="padding:3px 7px;font-size:11px;"
                      onclick="stopArpeggioPlayback(false);state.chordId='${p.id}';saveSettings();renderStageAndInspector()"
                      title="${p.name} (${p.category})">
                ${p.symbol}
              </button>`;
          }).join('')}
        </div>
      ` : `
        <div style="font-size:11px;color:var(--text-muted);line-height:1.35;">
          Pryma <strong>${rootLabels[state.rootPc]}</strong> nie należy do skali <strong>${data.scaleFullName}</strong> — wybierz stopień oznaczony na zielono powyżej.
        </div>
      `}
    </div>`;
}

function buildArpReferenceScaleInspectorHtml(rootLabels) {
  return `
    <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:9px;padding:9px 10px;display:flex;flex-direction:column;gap:7px;">
      <div style="display:flex;justify-content:space-between;align-items:center;gap:6px;">
        <div class="section-label" style="margin:0;">SKALA ODNIESIENIA (PODPOWIEDZI)</div>
        <button class="chip-btn" style="padding:2px 7px;font-size:10px;border-color:var(--accent);color:var(--accent);"
                onclick="syncArpScaleFromMainScaleTab()" title="Ustaw skalę i tonikę z aktualnego wyboru">
          🔗 Z zakładki Skala
        </button>
      </div>
      <div class="btn-grid-6">
        ${rootLabels.map((lbl, pc) => `
          <button class="chip-btn ${state.arpDiatonicScaleRootPc===pc?'active':''}" style="padding:4px 2px;font-size:11px;"
                  onclick="stopArpeggioPlayback(false);state.arpDiatonicScaleRootPc=${pc};saveSettings();renderAll()">${lbl}</button>
        `).join('')}
      </div>
      <select onchange="stopArpeggioPlayback(false);state.arpDiatonicScaleId=this.value;saveSettings();renderAll();">
        ${(state.initData.scales || []).map(s => `<option value="${s.id}" ${s.id===state.arpDiatonicScaleId?'selected':''}>${s.name}</option>`).join('')}
      </select>
      <button class="chip-btn ${state.arpFilterByScale?'active':''}" style="width:100%;font-size:11px;padding:5px 8px;"
              onclick="state.arpFilterByScale=!state.arpFilterByScale;saveSettings();renderStageAndInspector()">
        🔍 Przyciemnij akordy spoza skali: ${state.arpFilterByScale ? 'WŁ.' : 'WYŁ.'}
      </button>
    </div>`;
}

function normalizeSweepMetricFit(fit) {
  return (fit === 'sixteenths') ? 'sixteenths' : 'sextuplets';
}

function isSweepSextupletCount(count, metricFit = 'sextuplets') {
  const n = count | 0;
  if (n <= 0) return false;
  return normalizeSweepMetricFit(metricFit) === 'sextuplets' && (n % 3 === 0);
}

function formatCompactPositionLabel(pos, idx = 0) {
  if (!pos) return `Poz. ${idx + 1}`;
  const raw = String(pos.label || '');
  const rangeTxt = `pr. ${pos.minFret ?? 0}–${pos.maxFret ?? 0}`;
  const rollTxt = pos.hasRoll ? ' · R' : '';
  if (pos.idx < 0 || raw.startsWith('Wszystkie')) {
    return `Wszystkie · ${rangeTxt}`;
  }
  const cagedMatch = raw.match(/^(Kształt\s+[A-G])/i);
  if (cagedMatch) {
    return `${cagedMatch[1]} · ${rangeTxt}`;
  }
  if (raw.startsWith('Od str.')) {
    const strMatch = raw.match(/Od str\.\s*([EADGBe][0-9]?)/);
    const highTag = raw.includes('wysoka') ? '↑' : '';
    const tag = strMatch ? `${strMatch[1]}${highTag}` : `Poz. ${idx + 1}`;
    return `${tag} · ${rangeTxt}`;
  }
  const num = (typeof pos.idx === 'number' && pos.idx >= 0) ? (pos.idx + 1) : (idx + 1);
  return `Poz. ${num} · ${rangeTxt}${rollTxt}`;
}

function formatPlNoteWord(n) {
  const mod10 = n % 10;
  const mod100 = n % 100;
  if (n === 1) return 'nuta';
  if (mod10 >= 2 && mod10 <= 4 && (mod100 < 12 || mod100 > 14)) return 'nuty';
  return 'nut';
}

function formatSweepRangeLabel(steps, metricFit = 'sextuplets', dir = 0) {
  if (!Array.isArray(steps) || steps.length === 0) return '—';
  const n = steps.length;
  const isSext = isSweepSextupletCount(n, metricFit);
  const dirSym = (dir === 1) ? '↓' : (dir === 2 ? '↑' : '↕');
  const divLbl = isSext ? (n === 3 ? 'Triola' : 'Sekstole') : 'Szesnastki';
  return `${n} ${formatPlNoteWord(n)} · ${divLbl} ${dirSym}`;
}

function formatSweepMetricSummary(steps, metricFit = 'sextuplets') {
  if (!Array.isArray(steps) || !steps.length) return 'Brak nut';
  const n = steps.length;
  const isSext = isSweepSextupletCount(n, metricFit);
  const beats = isSext ? (n / 6) : (n / 4);
  const beatTxt = Math.abs(beats - 4) < 0.01 ? '1 pełny takt 4/4'
    : Math.abs(beats - 3) < 0.01 ? '3 ćwierćnuty'
    : Math.abs(beats - 2) < 0.01 ? '1/2 taktu 4/4'
    : Math.abs(beats - 1.5) < 0.01 ? '1.5 ćwierćnuty'
    : Math.abs(beats - 1) < 0.01 ? '1 ćwierćnuta'
    : Math.abs(beats - 0.5) < 0.01 ? '1/2 ćwierćnuty'
    : `${Math.round(beats * 100) / 100} ćwierćnuty`;
  const divTxt = isSext ? (n === 3 ? 'triola' : 'sekstole') : 'szesnastki';
  return `${n} ${formatPlNoteWord(n)} · ${divTxt} (${beatTxt})`;
}

function getSweepBarFillText(compList, totalNotes) {
  const list = Array.isArray(compList) ? compList : [];
  if (!list.length || !totalNotes) return '';
  let totalBeats = 0;
  for (const seg of list) {
    const segLen = Array.isArray(seg.steps) ? seg.steps.length : 0;
    const isSext = (typeof seg.isSextuplet === 'boolean')
      ? seg.isSextuplet
      : isSweepSextupletCount(segLen, seg.metricFit || 'sextuplets');
    totalBeats += isSext ? (segLen / 6) : (segLen / 4);
  }
  const rounded = Math.round(totalBeats * 100) / 100;
  const bars = rounded / 4;
  if (Math.abs(bars - Math.round(bars)) < 0.01 && Math.round(bars) >= 1) {
    const b = Math.round(bars);
    return ` · ${b} ${b === 1 ? 'pełny takt' : (b < 5 ? 'pełne takty' : 'pełnych taktów')} 4/4`;
  }
  if (Math.abs(rounded - 2) < 0.01) return ' · 1/2 taktu 4/4';
  if (Math.abs(rounded - 1) < 0.01) return ' · 1 ćwierćnuta';
  return ` · ${rounded} miar 4/4`;
}

function recalcSweepStepTechniques(orderedNodes, dirHint = 0, isSext = false) {
  const tup = isSext ? 3 : 0;
  const out = [];
  for (let i = 0; i < orderedNodes.length; i++) {
    const curr = orderedNodes[i];
    const currF = (curr.f !== undefined) ? curr.f : curr.fret;
    const prev = i > 0 ? orderedNodes[i - 1] : null;
    const prevF = prev ? ((prev.f !== undefined) ? prev.f : prev.fret) : 0;
    const next = (i + 1 < orderedNodes.length) ? orderedNodes[i + 1] : null;
    let tech = 'D';
    let roll = false;
    if (!prev) {
      if (next && next.s < curr.s) tech = 'U';
      else if (dirHint === 2 && (!next || next.s <= curr.s)) tech = 'U';
      else tech = 'D';
    } else if (curr.s === prev.s) {
      if (currF > prevF) tech = 'H';
      else if (currF < prevF) tech = 'P';
      else tech = (next && next.s < curr.s) ? 'U' : 'D';
    } else if (curr.s > prev.s) {
      tech = 'D';
      roll = (currF > 0 && currF === prevF && curr.s === prev.s + 1);
    } else {
      tech = 'U';
      roll = (currF > 0 && currF === prevF && prev.s === curr.s + 1);
    }
    out.push({
      s: curr.s,
      f: currF,
      midi: curr.midi,
      freq: (curr.freq !== undefined) ? curr.freq : curr.frequencyHz,
      note: (curr.note !== undefined) ? curr.note : curr.noteName,
      interval: curr.interval,
      isRoot: !!curr.isRoot,
      tech,
      roll,
      tuplet: tup,
      seg: 0
    });
  }
  return out;
}

function findExtraChordToneOnString(anchorNode, prevMidiLimit, nextMidiLimit, pcInfoMap, curMinF, curMaxF, preferHigher = false) {
  if (!anchorNode || !pcInfoMap || pcInfoMap.size === 0) return null;
  const fAnchor = (anchorNode.f !== undefined) ? anchorNode.f : anchorNode.fret;
  const mAnchor = anchorNode.midi;
  const baseFreq = (anchorNode.freq !== undefined) ? anchorNode.freq : (anchorNode.frequencyHz || midiToFreq(mAnchor));

  const hasSkippedChordTone = (mLo, mHi) => {
    for (let mm = mLo + 1; mm < mHi; mm++) {
      if (pcInfoMap.has(((mm % 12) + 12) % 12)) return true;
    }
    return false;
  };

  const makeCand = (fCand) => {
    const mCand = mAnchor + (fCand - fAnchor);
    const pc = ((mCand % 12) + 12) % 12;
    const info = pcInfoMap.get(pc);
    if (!info) return null;
    return {
      s: anchorNode.s,
      f: fCand,
      midi: mCand,
      freq: baseFreq * Math.pow(2, (fCand - fAnchor) / 12),
      note: info.note,
      interval: info.interval,
      isRoot: !!info.isRoot
    };
  };

  const searchBelow = () => {
    for (let fCand = fAnchor - 2; fCand >= Math.max(0, fAnchor - 5); fCand--) {
      const mCand = mAnchor + (fCand - fAnchor);
      if (prevMidiLimit !== null && mCand <= prevMidiLimit) continue;
      if (!pcInfoMap.has(((mCand % 12) + 12) % 12)) continue;
      if (hasSkippedChordTone(mCand, mAnchor)) continue;
      if (fCand > 0 && (Math.max(curMaxF, fAnchor) - Math.min(curMinF, fCand) > 8)) continue;
      return makeCand(fCand);
    }
    return null;
  };

  const searchAbove = () => {
    for (let fCand = fAnchor + 2; fCand <= Math.min(24, fAnchor + 5); fCand++) {
      const mCand = mAnchor + (fCand - fAnchor);
      if (nextMidiLimit !== null && mCand >= nextMidiLimit) continue;
      if (!pcInfoMap.has(((mCand % 12) + 12) % 12)) continue;
      if (hasSkippedChordTone(mAnchor, mCand)) continue;
      if (Math.max(curMaxF, fCand) - Math.min(curMinF, fAnchor) > 8) continue;
      return makeCand(fCand);
    }
    return null;
  };

  return preferHigher ? (searchAbove() || searchBelow()) : (searchBelow() || searchAbove());
}

function normalizeSweepShapeNodes(nodes) {
  const raw = Array.isArray(nodes) ? nodes : [];
  const seen = new Set();
  const sorted = raw.slice()
    .map(n => ({
      s: n.s,
      f: (n.f !== undefined) ? n.f : n.fret,
      midi: n.midi,
      freq: (n.freq !== undefined) ? n.freq : n.frequencyHz,
      note: (n.note !== undefined) ? n.note : n.noteName,
      interval: n.interval,
      isRoot: !!n.isRoot
    }))
    .sort((a, b) => (a.s - b.s) || (a.f - b.f))
    .filter(n => {
      const k = `${n.s}:${n.f}`;
      if (seen.has(k)) return false;
      seen.add(k);
      return true;
    });

  if (sorted.length < 2) {
    return { sorted, span: sorted.length, lowSingle: sorted[0] || null, lowPair: null, midNodes: [], topPair: sorted.slice() };
  }

  const pcInfoMap = new Map();
  for (const n of sorted) {
    const pc = ((n.midi % 12) + 12) % 12;
    if (!pcInfoMap.has(pc)) {
      pcInfoMap.set(pc, { note: n.note, interval: n.interval, isRoot: !!n.isRoot });
    }
  }

  const byStr = new Map();
  for (const n of sorted) {
    if (!byStr.has(n.s)) byStr.set(n.s, []);
    byStr.get(n.s).push(n);
  }
  const strs = Array.from(byStr.keys()).sort((a, b) => a - b);
  const minS = strs[0];
  const topS = strs[strs.length - 1];
  const curMinF = Math.min(...sorted.map(n => n.f));
  const curMaxF = Math.max(...sorted.map(n => n.f));

  // Najwyższa struna (1e): zawsze przygotuj parę [e1, e2] (f1 < f2), żeby przy powrocie było dokładnie 3 dźwięki (e1 -> e2(H) -> e1(P))
  let topArr = (byStr.get(topS) || []).slice();
  if (topArr.length === 1) {
    const prevTopStr = strs.length >= 2 ? byStr.get(strs[strs.length - 2]) : null;
    const prevTopMidi = prevTopStr ? prevTopStr[prevTopStr.length - 1].midi : null;
    const extraTop = findExtraChordToneOnString(topArr[0], prevTopMidi, null, pcInfoMap, curMinF, curMaxF, true);
    if (extraTop) {
      topArr = [topArr[0], extraTop].sort((a, b) => a.f - b.f);
    }
  } else if (topArr.length > 2) {
    topArr = [topArr[0], topArr[1]];
  }

  // Mediana progów środkowych strun + dolnego dźwięku na 1(e)
  const bodyFrets = [topArr[0].f];
  for (let i = 1; i < strs.length - 1; i++) {
    const arr = byStr.get(strs[i]);
    bodyFrets.push(arr[0].f);
  }
  bodyFrets.sort((a, b) => a - b);
  const bodyMedian = bodyFrets[Math.floor(bodyFrets.length / 2)] || 0;

  const midNodes = [];
  for (let i = 1; i < strs.length - 1; i++) {
    const arr = byStr.get(strs[i]);
    let best = arr[0];
    for (const c of arr) {
      if (Math.abs(c.f - bodyMedian) < Math.abs(best.f - bodyMedian)) best = c;
    }
    midNodes.push(best);
  }

  const lowArr = (byStr.get(minS) || []).slice();
  let lowSingle = lowArr[0];
  for (const c of lowArr) {
    if (Math.abs(c.f - bodyMedian) < Math.abs(lowSingle.f - bodyMedian)) lowSingle = c;
  }

  return {
    sorted,
    span: strs.length,
    minS,
    topS,
    lowSingle,
    midNodes,
    topPair: topArr,
    pcInfoMap,
    curMinF,
    curMaxF
  };
}

function buildCustomSingleSweepSteps(nodes, baseSteps) {
  if (!Array.isArray(nodes) || nodes.length === 0) return baseSteps || [];
  if ((state.arpSeqStyle || 0) !== 0) return baseSteps || [];

  const dir = state.arpDirection || 0;
  const fit = normalizeSweepMetricFit(state.sweepMetricFit);
  const info = normalizeSweepShapeNodes(nodes);
  if (info.sorted.length < 2) return recalcSweepStepTechniques(info.sorted, dir, false);

  const e1 = info.topPair[0];
  const e2 = info.topPair[info.topPair.length - 1];
  const hasTwoTop = (info.topPair.length >= 2);
  // Struna startowa (minS) zawsze ma dokładnie 1 dźwięk (lowSingle), bez dokładania 2. nuty startowej
  const ascSingle = hasTwoTop
    ? [info.lowSingle, ...info.midNodes, e1, e2]
    : [info.lowSingle, ...info.midNodes, e1];
  const ascOnePerStr = [info.lowSingle, ...info.midNodes, e1];

  const fullCycle = (ascArr) => ascArr.concat(ascArr.slice(1, -1).reverse());

  let chosenNodes;

  if (dir === 0) {
    // ↕ Pełny (Dół-Góra): zawsze 1 dźwięk na strunie startowej i dokładnie 3 dźwięki na najwyższej 1(e) (e1 -> e2(H) -> e1(P))
    chosenNodes = fullCycle(ascSingle); // 3 str.=6 nut, 5 str.=10 nut, 6 str.=12 nut
  } else if (dir === 1) {
    // ↓ Tylko w dół (bez powrotu — zawsze 1 dźwięk na strunie startowej; 1 nuta na e w sekstolach lub 2 w szesnastkach)
    if ((info.span <= 3 || info.span >= 6) && fit === 'sextuplets') {
      chosenNodes = ascOnePerStr; // 3 str.=3 nuty, 6 str.=6 nut
    } else {
      chosenNodes = ascSingle; // 3 str.=4 nuty, 5 str.=6 nut, 6 str.=7 nut
    }
  } else {
    // ↑ Tylko w górę (zawsze 2 dźwięki na 1(e) i 1 dźwięk na strunie startowej)
    chosenNodes = ascSingle.slice().reverse();
  }

  const isSext = isSweepSextupletCount(chosenNodes.length, fit);
  return recalcSweepStepTechniques(chosenNodes, dir, isSext);
}

function setSweepDirectionAndFit(dir) {
  stopArpeggioPlayback(false);
  state.arpDirection = dir;
  saveSettings();
  renderStageAndInspector();
}

function setSweepMetricFit(fit) {
  stopArpeggioPlayback(false);
  state.sweepMetricFit = normalizeSweepMetricFit(fit);
  saveSettings();
  renderStageAndInspector();
}

function getActiveArpeggioSteps() {
  if (!state.arpData) return [];
  if (isSweepTransitionActive() && state.arpData.transition && state.arpData.transition.segments) {
    const out = [];
    state.arpData.transition.segments.forEach((seg, sIdx) => {
      (seg.steps || []).forEach(st => {
        out.push(Object.assign({}, st, { seg: sIdx, chordSymbol: seg.symbol, roman: seg.roman }));
      });
    });
    return out;
  }
  if (state.subtab === 'sweep') {
    return buildCustomSingleSweepSteps(state.arpData.nodes || [], state.arpData.steps || []);
  }
  return state.arpData.steps || [];
}

function getSweepCompositionSteps() {
  const list = Array.isArray(state.sweepComposition) ? state.sweepComposition : [];
  const out = [];
  list.forEach((seg, sIdx) => {
    const segSteps = Array.isArray(seg.steps) ? seg.steps : [];
    const isSext = (typeof seg.isSextuplet === 'boolean')
      ? seg.isSextuplet
      : isSweepSextupletCount(segSteps.length, seg.metricFit || 'sextuplets');
    segSteps.forEach(st => {
      out.push(Object.assign({}, st, {
        seg: sIdx,
        chordSymbol: seg.symbol,
        roman: seg.rangeLabel || '',
        tuplet: (st.tuplet !== undefined) ? st.tuplet : (isSext ? 3 : 0)
      }));
    });
  });
  return out;
}

function toggleSweepCompactTabs() {
  state.sweepCompactTabs = !state.sweepCompactTabs;
  saveSettings();
  renderStageAndInspector();
}

function buildSweepCompSnapshotFromData(arpData, rootPc, chordId, fallbackPosIdx) {
  if (!arpData) return null;
  const nodes = arpData.nodes || [];
  const steps = buildCustomSingleSweepSteps(nodes, arpData.steps || []);
  if (!steps.length) return null;
  const effPosIdx = (arpData.posIdx !== undefined) ? arpData.posIdx : (fallbackPosIdx || 0);
  const posObj = Array.isArray(arpData.positions)
    ? arpData.positions.find(p => p.idx === effPosIdx)
    : null;
  let minF = 99, maxF = 0;
  for (const st of steps) {
    const fv = (st.f !== undefined) ? st.f : st.fret;
    if (fv < minF) minF = fv;
    if (fv > maxF) maxF = fv;
  }
  if (minF > maxF) minF = 0;
  const metricFit = normalizeSweepMetricFit(state.sweepMetricFit);
  const dir = state.arpDirection || 0;
  const isSext = isSweepSextupletCount(steps.length, metricFit);
  return {
    id: 'sw_' + Date.now() + '_' + Math.floor(Math.random() * 1000),
    symbol: arpData.chordSymbol || chordId,
    chordName: arpData.chordName || '',
    rootPc,
    chordId,
    posIdx: effPosIdx,
    posLabel: posObj ? posObj.label : `Poz. ${effPosIdx + 1}`,
    sweepStrings: state.arpSweepStrings,
    metricFit,
    isSextuplet: isSext,
    direction: dir,
    seqStyle: state.arpSeqStyle || 0,
    rangeLabel: formatSweepRangeLabel(steps, metricFit, dir),
    minFret: minF,
    maxFret: maxF,
    nodes: nodes.map(n => Object.assign({}, n)),
    steps: steps.map(st => Object.assign({}, st))
  };
}

function buildCurrentSweepCompSnapshot() {
  return buildSweepCompSnapshotFromData(state.arpData, state.rootPc, state.chordId, state.sweepPosIdx || 0);
}

function addCurrentSweepToComposition() {
  const snap = buildCurrentSweepCompSnapshot();
  if (!snap) return;
  if (!Array.isArray(state.sweepComposition)) state.sweepComposition = [];
  const wasEmpty = (state.sweepComposition.length === 0);
  state.sweepComposition.push(snap);
  state.sweepCompActiveIdx = state.sweepComposition.length - 1;
  if (wasEmpty) state.sweepCompactTabs = true;
  saveSettings();
  renderStageAndInspector();
}

async function quickAddDiatonicSweepToComposition(rootPc, chordId) {
  stopArpeggioPlayback(false);
  chordId = normalizeChordId(chordId);
  const customParam = (state.customStringMidis || []).join(',');
  const effPosIdx = state.sweepPosIdx || 0;
  const effDiatRootPc = (typeof state.arpDiatonicScaleRootPc === 'number' && state.arpDiatonicScaleRootPc >= 0)
    ? state.arpDiatonicScaleRootPc
    : rootPc;
  const url = `/api/arpeggio?tuning=${state.tuningId}&customMidis=${customParam}&rootPc=${rootPc}&chordId=${encodeURIComponent(chordId)}&pattern=4&posIdx=${effPosIdx}&sweepStrings=${state.arpSweepStrings}&direction=${state.arpDirection}&seqStyle=${state.arpSeqStyle||0}&stringSet=${state.arpStringSet||0}&octaves=${state.arpOctaves||2}&viewMode=single&transitionId=${encodeURIComponent(state.arpTransitionId)}&scaleRootPc=${effDiatRootPc}&scaleId=${encodeURIComponent(state.arpDiatonicScaleId)}&sevenths=${state.arpDiatonicSevenths?1:0}&accMode=${state.accMode}`;
  try {
    const data = await fetchJson(url);
    const snap = buildSweepCompSnapshotFromData(data, rootPc, chordId, effPosIdx);
    if (snap) {
      if (!Array.isArray(state.sweepComposition)) state.sweepComposition = [];
      const wasEmpty = (state.sweepComposition.length === 0);
      state.sweepComposition.push(snap);
      state.sweepCompActiveIdx = state.sweepComposition.length - 1;
      if (wasEmpty) state.sweepCompactTabs = true;
    }
    state.rootPc = rootPc;
    state.chordId = chordId;
    saveSettings();
    await renderAll();
  } catch (e) {}
}

async function selectSweepCompItem(idx) {
  if (!Array.isArray(state.sweepComposition) || idx < 0 || idx >= state.sweepComposition.length) return;
  stopArpeggioPlayback(false);
  state.sweepCompActiveIdx = idx;
  const item = state.sweepComposition[idx];
  if (item) {
    state.rootPc = item.rootPc;
    state.chordId = item.chordId;
    state.arpSweepStrings = item.sweepStrings || 3;
    state.sweepPosIdx = item.posIdx || 0;
    state.sweepMetricFit = normalizeSweepMetricFit(item.metricFit);
    state.arpDirection = item.direction || 0;
    state.arpSeqStyle = item.seqStyle || 0;
  }
  saveSettings();
  await renderAll();
  scrollSweepCompTabToSegment(idx, false);
}

function moveSweepCompItem(idx, delta) {
  if (!Array.isArray(state.sweepComposition)) return;
  const target = idx + delta;
  if (idx < 0 || idx >= state.sweepComposition.length || target < 0 || target >= state.sweepComposition.length) return;
  stopArpeggioPlayback(false);
  const tmp = state.sweepComposition[idx];
  state.sweepComposition[idx] = state.sweepComposition[target];
  state.sweepComposition[target] = tmp;
  state.sweepCompActiveIdx = target;
  saveSettings();
  renderStageAndInspector();
}

function replaceSweepCompItemWithCurrent(idx) {
  if (!Array.isArray(state.sweepComposition) || idx < 0 || idx >= state.sweepComposition.length) return;
  const snap = buildCurrentSweepCompSnapshot();
  if (!snap) return;
  stopArpeggioPlayback(false);
  state.sweepComposition[idx] = snap;
  state.sweepCompActiveIdx = idx;
  saveSettings();
  renderStageAndInspector();
}

function removeSweepCompItem(idx) {
  if (!Array.isArray(state.sweepComposition) || idx < 0 || idx >= state.sweepComposition.length) return;
  stopArpeggioPlayback(false);
  state.sweepComposition.splice(idx, 1);
  if (state.sweepCompActiveIdx >= state.sweepComposition.length) {
    state.sweepCompActiveIdx = Math.max(0, state.sweepComposition.length - 1);
  }
  if (state.sweepComposition.length === 0) {
    state.sweepPlaybackTarget = 'single';
  }
  saveSettings();
  renderStageAndInspector();
}

function clearSweepComposition() {
  stopArpeggioPlayback(false);
  state.sweepComposition = [];
  state.sweepCompActiveIdx = 0;
  state.sweepPlaybackTarget = 'single';
  saveSettings();
  renderStageAndInspector();
}

function refreshArpTabSvgOnly() {
  const wrap = document.getElementById('arpTabSvgWrap');
  if (!wrap || !state.arpData) return;
  const prevScroll = wrap.scrollLeft || 0;
  const isTrans = isSweepTransitionActive();
  const isSweepSingle = (state.subtab === 'sweep' && !isTrans);
  const tr = state.arpData.transition || { segments: [] };
  const activeSteps = getActiveArpeggioSteps();
  const playIdx = (isSweepSingle && state.sweepPlaybackTarget === 'comp') ? -1 : state.arpPlayStepIdx;
  wrap.innerHTML = buildArpeggioTabSvg(activeSteps, isTrans ? (tr.segments || []) : null, playIdx, isTrans, {
    compact: isSweepSingle && !!state.sweepCompactTabs,
    svgId: 'arpTabSvg',
    playheadId: 'arpPlayheadBg',
    onStepClickFn: 'playSingleArpeggioStep',
    activeSegIdx: state.arpActiveSegIdx
  });
  wrap.scrollLeft = prevScroll;
}

function buildArpeggioTabSvg(steps, segments, activeIdx, isTransition, options = {}) {
  if (!steps || steps.length === 0) {
    return `<div style="padding:18px;text-align:center;color:var(--text-muted);">Brak wygenerowanych nut dla tej konfiguracji.</div>`;
  }
  const compact = !!options.compact;
  const svgIdAttr = options.svgId ? `id="${options.svgId}"` : '';
  const playheadId = options.playheadId || 'arpPlayheadBg';
  const onStepClickFn = options.onStepClickFn || 'playSingleArpeggioStep';
  const activeSegIdx = (options.activeSegIdx !== undefined) ? options.activeSegIdx : state.arpActiveSegIdx;

  const nStrings = 6;
  const leftPad = compact ? 28 : 36;
  const colW = compact ? 26 : 34;
  const rightPad = compact ? 16 : 24;
  const showIntervalsAbove = (state.labelType === 'intervals');
  const topY = isTransition ? (compact ? 34 : 46) : (compact ? 26 : 34);
  const rowGap = compact ? 10 : 14;
  const W = Math.max(compact ? 420 : 520, leftPad + steps.length * colW + rightPad);
  const H = topY + (nStrings - 1) * rowGap + (compact ? 13 : 18);
  const strNames = ['e', 'B', 'G', 'D', 'A', 'E'];
  const symMap = { D: '↓', U: '↑', H: 'H', P: 'P', S: 'S' };
  const symColor = { D: '#fb923c', U: '#38bdf8', H: '#4ade80', P: '#4ade80', S: '#facc15' };
  const phW = compact ? 22 : 28;
  const phPadY = compact ? 15 : 20;
  const phExtraH = compact ? 22 : 30;

  let svg = `<svg ${svgIdAttr} width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" xmlns="http://www.w3.org/2000/svg">`;
  // Pionowy kursor podążający za aktualną nutą (aktualizowany bez przeładowywania SVG -> idealne płynne przewijanie!)
  const playX = (activeIdx >= 0 && activeIdx < steps.length) ? (leftPad + activeIdx * colW + colW * 0.5 - phW * 0.5) : -100;
  svg += `<rect id="${playheadId}" x="${playX}" y="${topY - phPadY}" width="${phW}" height="${(nStrings - 1) * rowGap + phExtraH}" rx="${compact ? 5 : 6}" fill="rgba(34,197,94,0.14)" stroke="rgba(74,222,128,0.45)" stroke-width="1" style="display:${activeIdx >= 0 ? 'block' : 'none'}"/>`;

  for (let r = 0; r < nStrings; r++) {
    const y = topY + r * rowGap;
    svg += `<text x="${compact ? 8 : 12}" y="${y + (compact ? 3.2 : 4)}" fill="#8c8277" font-family="monospace" font-size="${compact ? 9.5 : 11}" font-weight="700">${strNames[r]}</text>`;
    svg += `<line x1="${leftPad - 8}" y1="${y}" x2="${W - 10}" y2="${y}" stroke="#3e332a" stroke-width="${compact ? 1.0 : 1.2}"/>`;
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
        svg += `<line x1="${x - colW * 0.5}" y1="${topY - (compact ? 22 : 30)}" x2="${x - colW * 0.5}" y2="${topY + (nStrings - 1) * rowGap + 5}" stroke="#57412e" stroke-width="1.3" stroke-dasharray="4,3"/>`;
      }
      const segObj = segments && segments[st.seg];
      const hdr = segObj ? (segObj.roman ? `${segObj.symbol} (${segObj.roman})` : segObj.symbol) : '';
      const hdrColor = (st.seg === activeSegIdx) ? '#f39237' : '#d6cec2';
      svg += `<text data-seg-hdr="${st.seg}" x="${x - 6}" y="${compact ? 10 : 12}" fill="${hdrColor}" font-family="Segoe UI, sans-serif" font-size="${compact ? 10 : 11.5}" font-weight="800">${hdr}</text>`;
    }

    // Subtelny znacznik toniki (lub interwał) WYŻEJ nad tabulaturą — bez zaciemniania/podświetlania cyfr progów!
    if (showIntervalsAbove && st.interval) {
      svg += `<text x="${x}" y="${topY - (compact ? 15 : 21)}" fill="${st.isRoot ? '#f39237' : '#9c9288'}" font-family="Segoe UI, sans-serif" font-size="${compact ? 8.5 : 9.5}" font-weight="${st.isRoot ? '800' : '600'}" text-anchor="middle">${st.interval}</text>`;
    } else if (st.isRoot) {
      svg += `<circle cx="${x}" cy="${topY - (compact ? 16 : 22)}" r="${compact ? 2.1 : 2.6}" fill="#f39237"><title>Tonika (1)</title></circle>`;
    }

    // Symbol kostkowania / techniki nad tabulaturą
    const techSym = symMap[st.tech] || st.tech || '↓';
    const tCol = symColor[st.tech] || '#fb923c';
    svg += `<text x="${x}" y="${topY - (compact ? 6 : 9)}" fill="${tCol}" font-family="Segoe UI, sans-serif" font-size="${compact ? 9.5 : 11}" font-weight="800" text-anchor="middle">${techSym}${st.roll ? '·R' : ''}</text>`;

    // Numer progu na odpowiedniej strunie (czysty biały na ciemnym tle; podświetlenie wyłącznie dla aktywnego kroku!)
    const r = (nStrings - 1) - st.s;
    const y = topY + r * rowGap;
    const fretVal = (st.f !== undefined) ? st.f : st.fret;
    const boxW = compact ? (String(fretVal).length > 1 ? 18 : 13) : (String(fretVal).length > 1 ? 22 : 16);
    const boxH = compact ? 12 : 15;
    const bgFill = isAct ? '#22c55e' : '#14100d';
    const txtFill = isAct ? '#05140b' : '#f5f2eb';
    const strokeCol = isAct ? '#bbf7d0' : 'none';

    svg += `<g style="cursor:pointer" data-step-g="${i}" data-step-x="${x}" data-root="${st.isRoot?1:0}" onclick="${onStepClickFn}(${i})">`;
    svg += `<rect data-step-rect="${i}" x="${x - boxW / 2}" y="${y - boxH / 2}" width="${boxW}" height="${boxH}" rx="${compact ? 3 : 3.5}" fill="${bgFill}" stroke="${strokeCol}" stroke-width="${isAct ? '1.2' : '0'}"/>`;
    svg += `<text data-step-txt="${i}" x="${x}" y="${y + (compact ? 3.4 : 4)}" fill="${txtFill}" font-family="Segoe UI, sans-serif" font-size="${compact ? 10 : 11.5}" font-weight="800" text-anchor="middle">${fretVal}</text>`;
    svg += `</g>`;
  }

  svg += `</svg>`;
  return svg;
}

// Aktualizuje podświetlenie aktywnej nuty TAB bez niszczenia DOM i płynnie przesuwa pasek przewijania z zapasem do przodu!
function updateTabWrapHighlightAndScroll(wrapId, playheadId, activeSegIdx, activeIdx, smooth = true) {
  const wrap = document.getElementById(wrapId);
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

  svg.querySelectorAll('text[data-seg-hdr]').forEach(t => {
    const seg = parseInt(t.getAttribute('data-seg-hdr'), 10);
    t.setAttribute('fill', seg === activeSegIdx ? '#f39237' : '#d6cec2');
  });

  const ph = svg.querySelector(`#${playheadId}`);
  const actG = (activeIdx >= 0) ? svg.querySelector(`g[data-step-g="${activeIdx}"]`) : null;
  if (ph) {
    if (actG) {
      const nx = parseFloat(actG.getAttribute('data-step-x')) || 0;
      const phW = parseFloat(ph.getAttribute('width')) || 28;
      ph.setAttribute('x', String(nx - phW * 0.5));
      ph.style.display = 'block';
      scrollTabContainerToX(wrap, nx, smooth);
    } else {
      ph.style.display = 'none';
    }
  }
}

function updateArpTabHighlightAndScroll(activeIdx, smooth = true) {
  updateTabWrapHighlightAndScroll('arpTabSvgWrap', 'arpPlayheadBg', state.arpActiveSegIdx, activeIdx, smooth);
}

function updateSweepCompTabHighlightAndScroll(activeIdx, smooth = true) {
  updateTabWrapHighlightAndScroll('sweepCompTabSvgWrap', 'sweepCompPlayheadBg', state.sweepCompActiveIdx, activeIdx, smooth);
}

function playArpeggioChordPreview() {
  if (!state.arpData || !state.arpData.formula || !state.arpData.formula.length) return;
  stopCurrentScalePreview(false);
  const myToken = previewSessionToken;
  const freqs = state.arpData.formula.map(f => f.freq);
  const scaleGain = Math.min(0.65, 1.35 / Math.sqrt(Math.max(1, freqs.length)));
  freqs.forEach((hz, idx) => {
    const tid = setTimeout(() => {
      if (previewSessionToken !== myToken) return;
      pluckGuitarFrequency(hz, 1.35, scaleGain, true);
    }, idx * 35);
    previewTimers.push(tid);
  });
}

function selectDiatonicArpeggio(rootPc, chordId) {
  stopArpeggioPlayback(false);
  state.rootPc = rootPc;
  state.chordId = normalizeChordId(chordId);
  saveSettings();
  renderAll();
}

function selectDiatonicSweepChord(rootPc, chordId) {
  stopArpeggioPlayback(false);
  state.rootPc = rootPc;
  state.chordId = normalizeChordId(chordId);
  state.sweepViewMode = 'transition';
  state.arpTransitionId = 'same_chord_inversions';
  state.arpActiveSegIdx = 0;
  state.sweepTransPosIdx = 0;
  saveSettings();
  renderAll();
}

function setSweepTransPosIdx(idx) {
  stopArpeggioPlayback(false);
  state.sweepTransPosIdx = idx;
  state.arpActiveSegIdx = 0;
  state.arpPlayStepIdx = -1;
  saveSettings();
  renderStageAndInspector();
}

function openSweepSingleForSegment(segIdx) {
  stopArpeggioPlayback(false);
  const segs = (state.arpData && state.arpData.transition && state.arpData.transition.segments) || [];
  const seg = segs[segIdx] || segs[0];
  if (!seg) return;
  if (state.arpTransitionId !== 'same_chord_inversions') {
    state.arpDiatonicScaleRootPc = state.rootPc;
  }
  state.rootPc = seg.rootPc;
  state.chordId = seg.chordId;
  state.sweepViewMode = 'single';
  state.sweepPosIdx = 0;
  state._matchSweepSegBounds = { minFret: seg.minFret, maxFret: seg.maxFret };
  saveSettings();
  renderAll();
}

function launchDiatonicScaleSweep() {
  stopArpeggioPlayback(false);
  if (state.arpDiatonicScaleRootPc !== undefined && state.arpDiatonicScaleRootPc !== null) {
    state.rootPc = state.arpDiatonicScaleRootPc;
  }
  state.sweepViewMode = 'transition';
  state.arpTransitionId = 'diatonic_climb';
  state.arpActiveSegIdx = 0;
  state.sweepTransPosIdx = 0;
  saveSettings();
  renderAll();
}

async function selectArpTransitionSegment(segIdx) {
  stopArpeggioPlayback(false);
  state.arpActiveSegIdx = segIdx;
  state.arpPlayStepIdx = -1;
  await renderStageAndInspector();
  scrollArpTabToSegment(segIdx, false);
}

function setArpeggioBpm(bpm) {
  state.arpBpm = Math.max(40, Math.min(240, bpm));
  const el = document.getElementById('arpBpmVal');
  if (el) el.textContent = `${state.arpBpm} BPM`;
  saveSettings();
  if (state.arpPlaying) {
    const prevTarget = state.sweepPlaybackTarget || 'single';
    stopArpeggioPlayback(false);
    startSweepOrArpPlayback(prevTarget);
  }
}

function playSingleArpeggioStep(idx) {
  const steps = getActiveArpeggioSteps();
  const st = steps[idx];
  if (!st) return;
  state.sweepPlaybackTarget = 'single';
  state.arpPlayStepIdx = idx;
  if (isSweepTransitionActive() && st.seg !== undefined) {
    state.arpActiveSegIdx = st.seg;
    syncSweepSegCardSelection();
  }
  pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
  updateSweepCompTabHighlightAndScroll(-1, false);
  updateArpTabHighlightAndScroll(idx, true);
  renderFretboardToolbarForActiveTab();
  drawFretboard();
}

function playSingleSweepCompStep(idx) {
  const steps = getSweepCompositionSteps();
  const st = steps[idx];
  if (!st) return;
  state.sweepPlaybackTarget = 'comp';
  state.arpPlayStepIdx = idx;
  if (st.seg !== undefined) {
    state.sweepCompActiveIdx = st.seg;
    syncSweepCompCardSelection();
  }
  pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
  updateArpTabHighlightAndScroll(-1, false);
  updateSweepCompTabHighlightAndScroll(idx, true);
  drawFretboard();
}

function stopArpeggioPlayback(redraw = true) {
  if (arpPlaybackTimer) {
    clearTimeout(arpPlaybackTimer);
    arpPlaybackTimer = null;
  }
  state.arpPlaying = false;
  state.arpPlayStepIdx = -1;
  state.sweepPlaybackTarget = 'single';
  const btn = document.getElementById('arpPlayBtn');
  if (btn) {
    btn.textContent = '▶ Odsłuchaj';
    btn.classList.remove('active');
  }
  const compBtn = document.getElementById('sweepCompPlayBtn');
  if (compBtn) {
    compBtn.textContent = '▶ Odsłuchaj kompozycję';
    compBtn.classList.remove('active');
  }
  if (redraw && (state.subtab === 'arpeggio' || state.subtab === 'sweep')) {
    updateArpTabHighlightAndScroll(-1, false);
    updateSweepCompTabHighlightAndScroll(-1, false);
    drawFretboard();
  }
}

function startSweepOrArpPlayback(targetMode = 'single') {
  const isComp = (targetMode === 'comp' && state.subtab === 'sweep' && state.sweepViewMode === 'single');
  const steps = isComp ? getSweepCompositionSteps() : getActiveArpeggioSteps();
  if (!steps.length) return;

  state.sweepPlaybackTarget = isComp ? 'comp' : 'single';
  const isTransStart = !isComp && isSweepTransitionActive();
  let lockedSeg = (isTransStart && !state.sweepSequential) ? (state.arpActiveSegIdx || 0) : null;
  let startIdx = 0;

  if (isComp) {
    state.sweepCompActiveIdx = 0;
    startIdx = 0;
    syncSweepCompCardSelection();
    updateArpTabHighlightAndScroll(-1, false);
  } else if (isTransStart) {
    if (!state.sweepSequential) {
      startIdx = steps.findIndex(st => st.seg === lockedSeg);
      if (startIdx < 0) startIdx = 0;
    } else {
      state.arpActiveSegIdx = 0;
      startIdx = 0;
      syncSweepSegCardSelection();
      renderFretboardToolbarForActiveTab();
    }
  }

  state.arpPlaying = true;
  state.arpPlayStepIdx = startIdx;

  const isMainPlayBtnActive = !isComp || (!!state.sweepSequential && isComp);
  const btn = document.getElementById('arpPlayBtn');
  if (btn) {
    btn.textContent = isMainPlayBtnActive ? '⏹ Stop' : '▶ Odsłuchaj';
    btn.classList.toggle('active', isMainPlayBtnActive);
  }
  const compBtn = document.getElementById('sweepCompPlayBtn');
  if (compBtn) {
    compBtn.textContent = isComp ? '⏹ Stop' : '▶ Odsłuchaj kompozycję';
    compBtn.classList.toggle('active', isComp);
  }

  const tick = () => {
    if (!state.arpPlaying || (state.subtab !== 'arpeggio' && state.subtab !== 'sweep')) {
      stopArpeggioPlayback(false);
      return;
    }
    const curSteps = isComp ? getSweepCompositionSteps() : getActiveArpeggioSteps();
    if (!curSteps.length) {
      stopArpeggioPlayback(true);
      return;
    }

    let playedStep = null;
    if (isComp) {
      if (state.arpPlayStepIdx >= curSteps.length) {
        if (state.arpAutoTempo) {
          bumpArpeggioAutoTempo();
          state.arpPlayStepIdx = 0;
          state.sweepCompActiveIdx = 0;
          syncSweepCompCardSelection();
        } else {
          stopArpeggioPlayback(true);
          return;
        }
      }
      const st = curSteps[state.arpPlayStepIdx];
      playedStep = st;
      if (st.seg !== undefined && st.seg !== state.sweepCompActiveIdx) {
        state.sweepCompActiveIdx = st.seg;
        syncSweepCompCardSelection();
      }
      pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
      updateSweepCompTabHighlightAndScroll(state.arpPlayStepIdx, state.arpPlayStepIdx > startIdx);
      drawFretboard();
    } else {
      const isTransNow = isSweepTransitionActive();
      if (isTransNow) {
        if (state.sweepSequential) {
          lockedSeg = null;
        } else if (lockedSeg === null) {
          lockedSeg = state.arpActiveSegIdx || 0;
        }
        if (state.arpPlayStepIdx >= curSteps.length ||
            (lockedSeg !== null && curSteps[state.arpPlayStepIdx].seg !== lockedSeg)) {
          if (state.arpAutoTempo) {
            bumpArpeggioAutoTempo();
            state.arpPlayStepIdx = startIdx;
          } else {
            stopArpeggioPlayback(true);
            return;
          }
        }
      } else {
        const loopEnabled = (state.subtab === 'sweep') ? false : !!state.arpLoop;
        if (state.arpPlayStepIdx >= curSteps.length) {
          if (loopEnabled || state.arpAutoTempo) {
            if (state.arpAutoTempo) bumpArpeggioAutoTempo();
            state.arpPlayStepIdx = 0;
          } else {
            stopArpeggioPlayback(true);
            return;
          }
        }
      }
      const st = curSteps[state.arpPlayStepIdx];
      playedStep = st;
      const segChanged = (isTransNow && st.seg !== undefined && st.seg !== state.arpActiveSegIdx);
      if (segChanged) {
        state.arpActiveSegIdx = st.seg;
        syncSweepSegCardSelection();
        renderFretboardToolbarForActiveTab();
      }
      pluckGuitarFrequency(st.freq || midiToFreq(st.midi), 0.78);
      updateArpTabHighlightAndScroll(state.arpPlayStepIdx, state.arpPlayStepIdx > startIdx);
      drawFretboard();
    }

    state.arpPlayStepIdx++;
    // Szesnastki (4/ćwierćnutę) albo sekstole (6/ćwierćnutę, tuplet: 3) wg ustawionego BPM
    const perBeat = (playedStep && playedStep.tuplet === 3) ? 6 : 4;
    const stepMs = Math.max(55, Math.round(60000 / ((state.arpBpm || 120) * perBeat)));
    arpPlaybackTimer = setTimeout(tick, stepMs);
  };

  tick();
}

function toggleArpeggioPlayback() {
  if (state.arpPlaying) {
    if (state.subtab === 'sweep' && state.sweepViewMode === 'single' &&
        state.sweepPlaybackTarget === 'comp' && !state.sweepSequential) {
      stopArpeggioPlayback(false);
      startSweepOrArpPlayback('single');
      return;
    }
    stopArpeggioPlayback(true);
    return;
  }
  if (state.subtab === 'sweep' && state.sweepViewMode === 'single' &&
      state.sweepSequential && Array.isArray(state.sweepComposition) && state.sweepComposition.length > 0) {
    startSweepOrArpPlayback('comp');
    return;
  }
  startSweepOrArpPlayback('single');
}

function toggleSweepCompositionPlayback() {
  if (state.arpPlaying && state.sweepPlaybackTarget === 'comp') {
    stopArpeggioPlayback(true);
    return;
  }
  if (state.arpPlaying) {
    stopArpeggioPlayback(false);
  }
  startSweepOrArpPlayback('comp');
}

function exportStepsToExerciseTab(steps, title) {
  if (!steps || !steps.length) return;
  const techShortMap = { 'Hammer-on': 'H', 'Pull-off': 'P', Slide: 'S', H: 'H', P: 'P', S: 'S', D: 'D', U: 'U' };
  const fallbackTuplet = isSweepSextupletCount(steps.length, state.sweepMetricFit || 'sextuplets') ? 3 : 0;
  state.recordedEvents = steps.map(st => ({
    s: st.s,
    f: st.f,
    midi: st.midi,
    note: st.note,
    freq: st.freq || midiToFreq(st.midi),
    dur: 2, // Szesnastka (z tuplet: 3 -> sekstola / 6 na ćwierćnutę)
    tuplet: (st.tuplet !== undefined) ? (st.tuplet === 3 ? 3 : 0) : fallbackTuplet,
    rest: 0,
    tech: techShortMap[st.tech] || '-',
    seg: (st.seg !== undefined) ? st.seg : 0,
    chordSymbol: st.chordSymbol || '',
    roman: st.roman || '',
    isRoot: !!st.isRoot,
    interval: st.interval || '',
    roll: !!st.roll
  }));
  state.tempoBpm = state.arpBpm;
  state.exerciseBaseBpm = state.arpBpm;
  state.exerciseTitle = title;
  state.exercisePlayerView = 'tab';
  state.exercisePlayerActive = false;
  state.exerciseCompletedSet = new Set();
  state.exerciseLastHitIdx = -1;
  state.exerciseLastHitTime = 0;
  state.exerciseLastHitLabel = '';
  switchPracticeSubtab('exercise');
}

function sendArpeggioToExerciseTab() {
  stopArpeggioPlayback(false);
  const steps = getActiveArpeggioSteps();
  const title = (isSweepTransitionActive() && state.arpData && state.arpData.transition)
    ? state.arpData.transition.name
    : `${state.subtab === 'sweep' ? 'Sweep Picking' : 'Arpeggio'} ${state.arpData ? state.arpData.chordSymbol : state.chordId}`;
  exportStepsToExerciseTab(steps, title);
}

function sendSweepCompositionToExerciseTab() {
  stopArpeggioPlayback(false);
  const list = Array.isArray(state.sweepComposition) ? state.sweepComposition : [];
  if (!list.length) return;
  const steps = getSweepCompositionSteps();
  const title = `Kompozycja Sweep (${list.map(c => c.symbol).join(' → ')})`;
  exportStepsToExerciseTab(steps, title);
}

let saveSettingsDiskTimer = null;

function saveSettings() {
  try {
    const payload = {
      rootPc: state.rootPc,
      accMode: state.accMode,
      scaleId: state.scaleId,
      chordId: normalizeChordId(state.chordId),
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
      chordCapoFret: state.chordCapoFret || 0,
      arpViewMode: state.arpViewMode,
      sweepViewMode: state.sweepViewMode,
      arpPattern: state.arpPattern,
      arpPosIdx: state.arpPosIdx || 0,
      arpActiveSegIdx: state.arpActiveSegIdx || 0,
      sweepPosIdx: state.sweepPosIdx,
      sweepTransPosIdx: state.sweepTransPosIdx || 0,
      arpSweepStrings: state.arpSweepStrings,
      sweepMetricFit: normalizeSweepMetricFit(state.sweepMetricFit),
      sweepCompactTabs: !!state.sweepCompactTabs,
      sweepComposition: Array.isArray(state.sweepComposition) ? state.sweepComposition.slice(0, 32) : [],
      sweepCompActiveIdx: state.sweepCompActiveIdx || 0,
      arpDirection: state.arpDirection,
      arpSeqStyle: state.arpSeqStyle,
      arpStringSet: state.arpStringSet,
      arpOctaves: state.arpOctaves,
      arpStartStr: state.arpStartStr,
      arpReturnStr: state.arpReturnStr,
      arpTransitionId: state.arpTransitionId,
      arpDiatonicSevenths: !!state.arpDiatonicSevenths,
      arpDiatonicCategory: state.arpDiatonicCategory || 'all',
      arpFilterByScale: !!state.arpFilterByScale,
      arpAutoTempo: !!state.arpAutoTempo,
      savedSweepCompositions: Array.isArray(state.savedSweepCompositions) ? state.savedSweepCompositions.slice(0, 16) : [],
      arpDiatonicScaleId: state.arpDiatonicScaleId,
      arpDiatonicScaleRootPc: state.arpDiatonicScaleRootPc,
      arpShowScaleOnFretboard: !!state.arpShowScaleOnFretboard,
      arpBpm: state.arpBpm,
      arpLoop: !!state.arpLoop,
      sweepSequential: !!state.sweepSequential,
      tuningId: state.tuningId,
      customStringMidis: state.customStringMidis,
      a4ReferenceHz: state.a4ReferenceHz,
      selectedAudioInputId: state.selectedAudioInputId,
      selectedAudioOutputId: state.selectedAudioOutputId,
      tunerMode: state.tunerMode,
      tunerDisplay: state.tunerDisplay,
      themeMode: state.themeMode || 'dark',
      uiStyle: ['classic', 'modern', 'rack'].includes(state.uiStyle) ? state.uiStyle : 'classic',
      uiLang: state.uiLang || 'pl',
      fretMarkersMode: state.fretMarkersMode || 'numbers',
      numberAllFrets: !!state.numberAllFrets,
      fretboardMargin: state.fretboardMargin !== false,
      synthVolume: state.synthVolume !== undefined ? state.synthVolume : 0.85,
      metronomeVolume: state.metronomeVolume !== undefined ? state.metronomeVolume : 0.75,
      chordShowCircle: state.chordShowCircle !== false,
      skillLevel: state.skillLevel,
      resolvingDominants: state.resolvingDominants,
      alteredDominantScale: state.alteredDominantScale,
      alteredVoicingIntensity: state.alteredVoicingIntensity,
      movingVoices: state.movingVoices,
      maxDisplayedFret: state.maxDisplayedFret,
      uiScaleMode: state.uiScaleMode,
      midiKeyboardHidden: !!state.midiKeyboardHidden,
      rightInspectorHidden: !!state.rightInspectorHidden,
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
      exercisePlayerView: state.exercisePlayerView || 'tab',
      selectedProgressionIdx: state.selectedProgressionIdx,
      changesBpm: state.changesBpm,
      changesTimeSig: state.changesTimeSig,
      changesRepeat: state.changesRepeat,
      changesFeel: state.changesFeel,
      changesCompInst: state.changesCompInst,
      changesBass: 'Wył.',
      changesAttackStrength: getChangesAttackMul(),
      changesGroove: state.changesGroove,
      changesRhythmGuitar: state.changesRhythmGuitar,
      changesLoop: state.changesLoop,
      changesChordDiagrams: state.changesChordDiagrams,
      changesBackingBand: state.changesBackingBand,
      changesMetronomeClick: state.changesMetronomeClick,
      changesDrums: state.changesDrums,
      changesDrumStyle: state.changesDrumStyle,
      changesCycleKeys: state.changesCycleKeys,
      changesPracticeGoal: state.changesPracticeGoal,
      changesPracticeMode: state.changesPracticeMode,
      changesPracticeDegrees: state.changesPracticeDegrees,
      changesPracticeOrder: state.changesPracticeOrder,
      customSavedSessions: Array.isArray(state.customSavedSessions) ? state.customSavedSessions.slice(-20) : [],
      tempoBpm: state.tempoBpm,
      timeSigNum: state.timeSigNum,
      timeSigDen: state.timeSigDen,
      exerciseClef: state.exerciseClef,
      tabCreationEnabled: state.tabCreationEnabled,
      exerciseShowStaff: !!state.exerciseShowStaff,
      exerciseStaffPrefV2: !!state.exerciseStaffPrefV2,
      exerciseTabWrapRows: true,
      exerciseBarsPerRow: state.exerciseBarsPerRow || 0,
      defaultDuration: state.defaultDuration,
      showExerciseNoteNames: state.showExerciseNoteNames,
      exerciseMinFret: state.exerciseMinFret,
      exerciseMaxFret: state.exerciseMaxFret,
      scalePracticeMode: state.scalePracticeMode,
      spPosSystem: state.spPosSystem,
      spPosIdx: state.spPosIdx,
      spTraversal: state.spTraversal,
      spStartString: state.spStartString,
      spDirection: state.spDirection,
      spOctaves: state.spOctaves,
      spPattern: state.spPattern,
      spStartOnRoot: state.spStartOnRoot,
      spFretZone: state.spFretZone,
      spNoteDur: state.spNoteDur,
      spTuplet: !!state.spTuplet,
      spBpm: state.spBpm,
      spUseHeadphones: state.spUseHeadphones,
      spMetronome: state.spMetronome,
      spRefNote: state.spRefNote,
      spBgChord: state.spBgChord,
      dspModel: state.dspModel,
      dspThresholdOn: state.dspThresholdOn,
      dspHighThresholdOn: state.dspHighThresholdOn,
      dspAmpPreEq: state.dspAmpPreEq !== false,
      dspNoiseGateRms: state.dspNoiseGateRms,
      dspCalibrated: !!state.dspCalibrated,
      dspDefaultsV2: !!state.dspDefaultsV2
    };
    const jsonStr = JSON.stringify(payload);
    localStorage.setItem('apka_gitara_settings_v1', jsonStr);
    clearTimeout(saveSettingsDiskTimer);
    saveSettingsDiskTimer = setTimeout(() => {
      fetch('/api/save_settings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: jsonStr
      }).catch(() => {});
    }, 250);
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
  // Migracja z ustawień domyślnych V1 (piano/0.60) do uniwersalnego profilu wzmacniaczowo-mikrofonowego (fast + cienkie struny)
  if (!state.dspDefaultsV2) {
    state.dspDefaultsV2 = true;
    if (!state.dspCalibrated) {
      state.dspModel = 'fast';
      state.dspThresholdOn = 0.58;
      state.dspHighThresholdOn = 0.42;
      state.dspAmpPreEq = true;
    } else if (typeof state.dspHighThresholdOn !== 'number') {
      state.dspHighThresholdOn = Math.max(0.34, Math.round(((state.dspThresholdOn || 0.58) - 0.16) * 100) / 100);
      state.dspAmpPreEq = true;
    }
  }
  // Bas w panelu Zmian jest wstępnie wyłączony; siła ataku zawsze znormalizowana do [0.20..1.00]
  state.changesBass = 'Wył.';
  if (typeof state.changesAttackStrength !== 'number' || !Number.isFinite(state.changesAttackStrength) ||
      state.changesAttackStrength < 0.2 || state.changesAttackStrength > 1.0) {
    state.changesAttackStrength = 0.75;
  }
  // Domyślnie w sekcji Ćwiczenie nuty (pięciolinia) są zwinięte, a tabulatura (TAB) jest widoczna
  if (!state.exerciseStaffPrefV2) {
    state.exerciseShowStaff = false;
    state.tabCreationEnabled = true;
  }
  if (typeof state.sweepSequential !== 'boolean') {
    state.sweepSequential = false;
  }
  if (typeof state.sweepPosIdx !== 'number' || !Number.isFinite(state.sweepPosIdx)) {
    state.sweepPosIdx = 0;
  }
  if (typeof state.arpPosIdx !== 'number' || !Number.isFinite(state.arpPosIdx)) {
    state.arpPosIdx = 0;
  }
  if (typeof state.arpActiveSegIdx !== 'number' || !Number.isFinite(state.arpActiveSegIdx)) {
    state.arpActiveSegIdx = 0;
  }
  state.chordId = normalizeChordId(state.chordId);
  if (!['dark', 'light', 'system'].includes(state.themeMode)) {
    state.themeMode = 'dark';
  }
  if (!['classic', 'modern', 'rack'].includes(state.uiStyle)) {
    state.uiStyle = 'classic';
  }
  if (!['all', 'triad', 'seventh', 'sus_add_6', 'extended'].includes(state.arpDiatonicCategory)) {
    state.arpDiatonicCategory = 'all';
  }
  state.arpFilterByScale = !!state.arpFilterByScale;
  state.arpAutoTempo = !!state.arpAutoTempo;
  if (!Array.isArray(state.savedSweepCompositions)) {
    state.savedSweepCompositions = [];
  } else {
    state.savedSweepCompositions.forEach(pr => {
      if (pr && Array.isArray(pr.items)) pr.items.forEach(seg => rebuildSweepSegmentSteps(seg));
    });
  }
  state.sweepMetricFit = normalizeSweepMetricFit(state.sweepMetricFit);
  delete state.sweepStartStr;
  delete state.sweepReturnMode;
  if (!Array.isArray(state.sweepComposition)) {
    state.sweepComposition = [];
  } else {
    state.sweepComposition.forEach(seg => rebuildSweepSegmentSteps(seg));
  }
  if (typeof state.sweepCompActiveIdx !== 'number' || !Number.isFinite(state.sweepCompActiveIdx)) {
    state.sweepCompActiveIdx = 0;
  }
  state.sweepCompactTabs = !!state.sweepCompactTabs;
  state.sweepPlaybackTarget = 'single';
}

function setExerciseScoreViewMode(mode) {
  state.exerciseStaffPrefV2 = true;
  if (mode === 'tab') {
    state.exerciseShowStaff = false;
    state.tabCreationEnabled = true;
  } else if (mode === 'staff') {
    state.exerciseShowStaff = true;
    state.tabCreationEnabled = false;
  } else {
    state.exerciseShowStaff = true;
    state.tabCreationEnabled = true;
  }
  saveSettings();
  renderStageAndInspector();
}

function setExerciseShowStaff(show) {
  state.exerciseShowStaff = !!show;
  state.exerciseStaffPrefV2 = true;
  if (!state.exerciseShowStaff) {
    state.tabCreationEnabled = true;
  }
  saveSettings();
  renderStageAndInspector();
}

function toggleExerciseStaff() {
  setExerciseShowStaff(!state.exerciseShowStaff);
}

function buildExerciseStaffBarHtml() {
  const mode = (!state.exerciseShowStaff && state.tabCreationEnabled)
    ? 'tab'
    : ((state.exerciseShowStaff && !state.tabCreationEnabled) ? 'staff' : 'both');
  return `
    <div class="ex-staff-toggle-bar ${mode !== 'tab' ? 'open' : ''}" style="cursor:default;">
      <span style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
        <span>Widok zapisu:</span>
        <span class="ex-staff-hint">${mode === 'tab' ? 'Sama tabulatura (TAB)' : (mode === 'both' ? 'Pięciolinia (nuty) + Tabulatura (TAB)' : 'Sama pięciolinia (nuty)')}</span>
      </span>
      <div class="ear-seg-row" style="width:auto;flex-shrink:0;">
        <button type="button" class="ear-seg-btn ${mode==='tab'?'active':''}" style="padding:5px 11px;font-size:11.5px;white-space:nowrap;"
                onclick="setExerciseScoreViewMode('tab')">🎸 Tylko TAB</button>
        <button type="button" class="ear-seg-btn ${mode==='both'?'active':''}" style="padding:5px 11px;font-size:11.5px;white-space:nowrap;"
                onclick="setExerciseScoreViewMode('both')">🎼 Nuty + TAB</button>
        <button type="button" class="ear-seg-btn ${mode==='staff'?'active':''}" style="padding:5px 11px;font-size:11.5px;white-space:nowrap;"
                onclick="setExerciseScoreViewMode('staff')">🎼 Tylko Nuty</button>
      </div>
    </div>`;
}

function setExerciseBarsPerRow(n) {
  const allowed = [0, 2, 4, 6, 8];
  state.exerciseBarsPerRow = allowed.includes(n) ? n : 0;
  state.exerciseTabWrapRows = true;
  saveSettings();
  renderStageAndInspector();
}

function buildExerciseBarsPerRowControlHtml(compact = false) {
  const cur = [0, 2, 4, 6, 8].includes(state.exerciseBarsPerRow) ? state.exerciseBarsPerRow : 0;
  const opts = compact ? [0, 4, 6, 8] : [0, 2, 4, 6, 8];
  return `
    <div style="display:flex;align-items:center;gap:4px;background:var(--bg-card);border:1px solid var(--border);border-radius:8px;padding:3px 6px;">
      <span style="font-size:11px;color:var(--text-muted);font-weight:700;padding:0 3px;white-space:nowrap;">Taktów w linii:</span>
      ${opts.map(n => `
        <button class="chip-btn ${cur === n ? 'active' : ''}" style="padding:3px 8px;font-size:11px;"
                onclick="setExerciseBarsPerRow(${n})"
                title="${n === 0 ? 'Automatyczne zawijanie taktów do nowej linii bez poziomego paska' : `Dokładnie ${n} takty w każdym wierszu`}">
          ${n === 0 ? 'Auto' : n}
        </button>
      `).join('')}
    </div>`;
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

  const light = isLightTheme();
  const cx = W / 2;
  const cy = H - 84;
  const R = 290;

  // Łuk skali (-50 ct .. +50 ct)
  ctx.strokeStyle = light ? '#d6cec2' : '#26201b';
  ctx.lineWidth = 14;
  ctx.lineCap = 'round';
  ctx.beginPath();
  ctx.arc(cx, cy, R, Math.PI, 2 * Math.PI);
  ctx.stroke();

  // Zielona strefa idealnego stroju u góry (-7 ct .. +7 ct)
  ctx.strokeStyle = light ? 'rgba(22, 163, 74, 0.28)' : 'rgba(74, 222, 128, 0.30)';
  ctx.lineWidth = 24;
  ctx.beginPath();
  ctx.arc(cx, cy, R, 1.5 * Math.PI - 0.22, 1.5 * Math.PI + 0.22);
  ctx.stroke();

  // Podziałka kresek (-50 .. +50 co 5 centów)
  for (let ct = -50; ct <= 50; ct += 5) {
    const angle = Math.PI + ((ct + 50) / 100) * Math.PI;
    const isMajor = (ct % 25 === 0);
    const rIn = R - (isMajor ? 26 : 15);
    const rOut = R - 2;
    ctx.strokeStyle = (ct === 0)
      ? (light ? '#16a34a' : '#4ade80')
      : (isMajor ? (light ? '#44403c' : '#d6cec2') : (light ? '#a8a29e' : '#574c43'));
    ctx.lineWidth = (ct === 0) ? 4.5 : (isMajor ? 2.8 : 1.6);
    ctx.beginPath();
    ctx.moveTo(cx + Math.cos(angle) * rIn, cy + Math.sin(angle) * rIn);
    ctx.lineTo(cx + Math.cos(angle) * rOut, cy + Math.sin(angle) * rOut);
    ctx.stroke();
  }

  ctx.fillStyle = light ? '#57534e' : '#9c9288';
  ctx.font = 'bold 15px Segoe UI';
  ctx.textAlign = 'center';
  ctx.fillText('-50¢', cx - R - 14, cy + 28);
  ctx.fillText('+50¢', cx + R + 14, cy + 28);
  ctx.font = '600 13px Segoe UI';
  ctx.fillText('-25¢', cx - R * 0.78, cy - R * 0.78);
  ctx.fillText('+25¢', cx + R * 0.78, cy - R * 0.78);

  // Jeśli wybrano tryb Strobo, rysujemy dodatkowo obracający się pierścień stroboskopowy wzdłuż łuku
  if (state.tunerDisplay === 'strobe' && state.detectedMidi !== null) {
    strobePhase = (strobePhase + state.detectedCents * 0.008) % (Math.PI / 6);
    ctx.strokeStyle = Math.abs(state.detectedCents) <= 7.0 ? '#4ade80' : '#f39237';
    ctx.lineWidth = 8;
    for (let a = Math.PI + 0.08; a < 2 * Math.PI - 0.08; a += Math.PI / 10) {
      const a1 = a + strobePhase;
      ctx.beginPath();
      ctx.arc(cx, cy, R + 20, a1, a1 + 0.12);
      ctx.stroke();
    }
  }

  // Wskazówka (igła)
  const clampedCents = (state.detectedMidi !== null)
    ? Math.max(-50, Math.min(50, state.detectedCents))
    : 0;
  const needleAngle = Math.PI + ((clampedCents + 50) / 100) * Math.PI;
  const inTune = (state.detectedMidi !== null && Math.abs(state.detectedCents) <= 7.0);
  const greenCol = light ? '#16a34a' : '#4ade80';

  ctx.strokeStyle = inTune ? greenCol : (light ? '#57534e' : '#b5ada3');
  ctx.lineWidth = 4.5;
  ctx.beginPath();
  ctx.moveTo(cx, cy);
  ctx.lineTo(cx + Math.cos(needleAngle) * (R - 24), cy + Math.sin(needleAngle) * (R - 24));
  ctx.stroke();

  // Zielony grot na szczycie przy trafieniu w 0¢
  if (inTune) {
    ctx.strokeStyle = greenCol;
    ctx.lineWidth = 6;
    ctx.beginPath();
    ctx.moveTo(cx + Math.cos(needleAngle) * (R - 36), cy + Math.sin(needleAngle) * (R - 36));
    ctx.lineTo(cx + Math.cos(needleAngle) * R, cy + Math.sin(needleAngle) * R);
    ctx.stroke();
  }

  // Oś obrotu igły
  ctx.fillStyle = inTune ? greenCol : (light ? '#44403c' : '#a8a29b');
  ctx.beginPath();
  ctx.arc(cx, cy, 10.5, 0, Math.PI * 2);
  ctx.fill();

  // Wyświetlacz nuty pod osią
  const readout = document.getElementById('tunerNoteReadout');
  if (state.detectedMidi !== null) {
    const labels = getRootLabels();
    const noteStr = labels[((state.detectedMidi % 12) + 12) % 12] + (Math.floor(state.detectedMidi / 12) - 1);
    ctx.fillStyle = inTune ? greenCol : (light ? '#1c1917' : '#f5f2eb');
    ctx.font = 'bold 46px Segoe UI';
    ctx.fillText(noteStr, cx, cy + 62);
    if (readout) {
      readout.style.color = inTune ? 'var(--green)' : 'var(--text)';
      readout.textContent = `${state.detectedCents >= 0 ? '+' : ''}${state.detectedCents.toFixed(1)} ¢ · ${state.detectedHz.toFixed(1)} Hz`;
    }
  } else {
    ctx.fillStyle = light ? '#a8a29e' : '#78716c';
    ctx.fillRect(cx - 32, cy + 40, 64, 12);
    if (readout) {
      readout.style.color = 'var(--text-muted)';
      readout.textContent = t('Zagraj strunę...', 'Play a string...');
    }
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

function ensureChangesBarBeatsLength(bar, beatsPerBar) {
  if (!bar) return;
  if (!Array.isArray(bar.beats)) bar.beats = [];
  while (bar.beats.length < beatsPerBar) bar.beats.push(null);
}

function getChangesDefaultTonicSlot() {
  if (state.changesIsMinor) {
    return { offsetSemi: 0, chordId: state.changesFeel === 'Swing Jazz' ? 'm7' : 'minor', roman: state.changesFeel === 'Swing Jazz' ? 'im7' : 'i' };
  }
  if (state.changesFeel === 'Blues') {
    return { offsetSemi: 0, chordId: '7', roman: 'I7' };
  }
  if (state.changesFeel === 'Swing Jazz') {
    return { offsetSemi: 0, chordId: 'maj7', roman: 'Imaj7' };
  }
  return { offsetSemi: 0, chordId: 'major', roman: 'I' };
}

function ensureChangesProgressionLoaded(forceReload = false) {
  if (!state.initData || !state.initData.progressions) return;
  if (state.changesMeasures && state.changesMeasures.length > 0 && !forceReload) {
    const bpb = getChangesBeatsPerBar();
    state.changesMeasures.forEach(m => ensureChangesBarBeatsLength(m, bpb));
    return;
  }
  const p = state.initData.progressions[state.selectedProgressionIdx] || state.initData.progressions[0];
  if (!p) return;
  state.changesKeyRootPc = (p.keyRootPc !== undefined) ? p.keyRootPc : 9;
  state.changesIsMinor = !!p.isMinor;
  state.changesBpm = p.bpm || 108;
  state.changesFeel = (p.style === 'Blues') ? 'Blues' : (p.style === 'Jazz' ? 'Swing Jazz' : 'Prosto');
  state.changesCompInst = (p.style === 'Blues') ? 'hammond' : 'clean';
  state.changesGroove = (p.groove === 1) ? 'shuffle' : 'slow12_8';
  state.changesCycleKeys = !!p.cycleKeys;
  state.changesEditingSlot = null;
  const bpb = getChangesBeatsPerBar();
  state.changesMeasures = (p.bars || []).map(b => {
    const beats = new Array(Math.max(4, bpb)).fill(null);
    beats[0] = {
      offsetSemi: (b.offsetSemi !== undefined) ? b.offsetSemi : 0,
      chordId: b.chordId || '7',
      roman: b.roman || 'I'
    };
    return { volta: 0, beats };
  });
}

function selectChangesProgression(idx) {
  stopChangesPlayback();
  state.selectedProgressionIdx = idx;
  state.changesScreen = 'editor';
  state.changesEditingSlot = null;
  ensureChangesProgressionLoaded(true);
  saveSettings();
  renderAll();
}

// Tonika progresji: przesunięcia offsetSemi w C++ i edytorze liczone są ZAWSZE bezpośrednio od toniki (dur lub moll)!
function getChangesTransposeRootPc() {
  return ((state.changesKeyRootPc % 12) + 12) % 12;
}

function formatChangesRomanForSlot(offsetSemi, chordId) {
  const semi = ((offsetSemi % 12) + 12) % 12;
  const majorNames = { 0:'I', 1:'♭II', 2:'II', 3:'♭III', 4:'III', 5:'IV', 6:'♯IV', 7:'V', 8:'♭VI', 9:'VI', 10:'♭VII', 11:'VII' };
  const minorNames = { 0:'i', 1:'♭ii', 2:'ii', 3:'♭iii', 4:'iii', 5:'iv', 6:'♯iv', 7:'v', 8:'♭vi', 9:'vi', 10:'♭vii', 11:'vii' };
  const isMinType = (chordId === 'minor' || chordId === 'm7' || chordId === 'm7b5' || chordId === 'dim7');
  const base = isMinType ? (minorNames[semi] || 'i') : (majorNames[semi] || 'I');
  const extMap = { 'major':'', 'minor':'', '7':'7', 'maj7':'maj7', 'm7':'7', 'm7b5':'7♭5', 'dim7':'dim7', '7b9':'7♭9' };
  return base + (extMap[chordId] !== undefined ? extMap[chordId] : '');
}

// Przełącza tryb dur ↔ moll, inteligentnie dopasowując tryb akordów w progresji bez przesuwania toniki!
function setChangesMode(toMinor) {
  const nextMinor = !!toMinor;
  if (state.changesIsMinor === nextMinor) return;
  ensureChangesProgressionLoaded();
  state.changesIsMinor = nextMinor;
  if (Array.isArray(state.changesMeasures)) {
    state.changesMeasures.forEach(bar => {
      (bar.beats || []).forEach(slot => {
        if (!slot) return;
        const semi = ((slot.offsetSemi || 0) % 12 + 12) % 12;
        if (nextMinor) {
          if (semi === 0 && (slot.chordId === 'major' || slot.chordId === 'maj7' || slot.chordId === '7')) {
            slot.chordId = (slot.chordId === 'major') ? 'minor' : 'm7';
          } else if (semi === 5 && (slot.chordId === 'major' || slot.chordId === 'maj7' || slot.chordId === '7')) {
            slot.chordId = (slot.chordId === 'major') ? 'minor' : 'm7';
          } else if (semi === 2 && slot.chordId === 'm7') {
            slot.chordId = 'm7b5';
          } else if (semi === 4 && (slot.chordId === 'minor' || slot.chordId === 'm7')) {
            slot.offsetSemi = 3;
            slot.chordId = (slot.chordId === 'minor') ? 'major' : 'maj7';
          } else if (semi === 9 && (slot.chordId === 'minor' || slot.chordId === 'm7')) {
            slot.offsetSemi = 8;
            slot.chordId = (slot.chordId === 'minor') ? 'major' : 'maj7';
          }
        } else {
          if (semi === 0 && (slot.chordId === 'minor' || slot.chordId === 'm7')) {
            slot.chordId = (slot.chordId === 'minor') ? 'major' : (state.changesFeel === 'Blues' ? '7' : 'maj7');
          } else if (semi === 5 && (slot.chordId === 'minor' || slot.chordId === 'm7')) {
            slot.chordId = (slot.chordId === 'minor') ? 'major' : (state.changesFeel === 'Blues' ? '7' : 'maj7');
          } else if (semi === 2 && slot.chordId === 'm7b5') {
            slot.chordId = 'm7';
          } else if (semi === 3 && (slot.chordId === 'major' || slot.chordId === 'maj7')) {
            slot.offsetSemi = 4;
            slot.chordId = (slot.chordId === 'major') ? 'minor' : 'm7';
          } else if (semi === 8 && (slot.chordId === 'major' || slot.chordId === 'maj7')) {
            slot.offsetSemi = 9;
            slot.chordId = (slot.chordId === 'major') ? 'minor' : 'm7';
          }
        }
        slot.roman = formatChangesRomanForSlot(slot.offsetSemi, slot.chordId);
      });
    });
  }
  saveSettings();
  renderChangesPreservingScroll();
}

function getEffectiveChangesSlot(bar, beatIdx) {
  if (!bar || !Array.isArray(bar.beats)) return null;
  for (let b = Math.min(beatIdx, bar.beats.length - 1); b >= 0; b--) {
    if (bar.beats[b]) return bar.beats[b];
  }
  return bar.beats[0] || null;
}

function getChangesBarSummarySlots(bar, beatsPerBar) {
  if (!bar || !Array.isArray(bar.beats)) return [];
  const out = [];
  const lim = Math.min(beatsPerBar, bar.beats.length);
  for (let b = 0; b < lim; b++) {
    if (bar.beats[b]) {
      const res = resolveChangesSlot(bar.beats[b]);
      if (res) out.push({ beatIdx: b, resolved: res });
    }
  }
  return out;
}

function getChangesRepeatCount() {
  if (state.changesRepeat === '4x') return 4;
  if (state.changesRepeat === '2x') return 2;
  return 1;
}

function buildChangesPlaybackSequence() {
  const measures = state.changesMeasures || [];
  const totalPasses = getChangesRepeatCount();
  const seq = [];
  for (let passIdx = 0; passIdx < totalPasses; passIdx++) {
    for (let barIdx = 0; barIdx < measures.length; barIdx++) {
      const v = measures[barIdx].volta || 0;
      if (v === 1 && totalPasses > 1 && passIdx === totalPasses - 1) continue;
      if (v === 2 && totalPasses > 1 && passIdx < totalPasses - 1) continue;
      seq.push({ barIdx, passIdx, totalPasses });
    }
  }
  if (seq.length === 0 && measures.length > 0) {
    measures.forEach((_, barIdx) => seq.push({ barIdx, passIdx: 0, totalPasses: 1 }));
  }
  return seq;
}

function resolveChangesSlot(slot) {
  if (!slot) return null;
  const labels = getRootLabels();
  const rootPc = ((getChangesTransposeRootPc() + (slot.offsetSemi || 0)) % 12 + 12) % 12;
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
  const mVol = (state.metronomeVolume !== undefined) ? state.metronomeVolume : 0.75;
  osc.type = 'triangle';
  osc.frequency.setValueAtTime(isDownbeat ? 1150 : 780, now);
  g.gain.setValueAtTime(Math.max(0.01, (isDownbeat ? 0.24 : 0.16) * mVol), now);
  g.gain.exponentialRampToValueAtTime(0.001, now + 0.045);
  osc.connect(g);
  g.connect(getMasterAudioOut());
  osc.start(now);
  osc.stop(now + 0.05);
}

// ===== Synteza perkusji (kick, snare, hi-hat) dla zespołu podkładowego =====
function buildDrumNoiseBuffer(ctx, seconds) {
  const sr = ctx.sampleRate;
  const len = Math.max(1, Math.floor(sr * seconds));
  const buf = ctx.createBuffer(1, len, sr);
  const data = buf.getChannelData(0);
  for (let i = 0; i < len; i++) data[i] = Math.random() * 2 - 1;
  return buf;
}

function playDrumHit(kind, velocity = 1.0) {
  const ctx = ensureAudioCtx();
  const now = ctx.currentTime;
  const out = getMasterAudioOut();

  if (kind === 'kick') {
    const osc = ctx.createOscillator();
    const g = ctx.createGain();
    osc.type = 'sine';
    osc.frequency.setValueAtTime(120, now);
    osc.frequency.exponentialRampToValueAtTime(45, now + 0.12);
    g.gain.setValueAtTime(0.9 * velocity, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.32);
    osc.connect(g); g.connect(out);
    osc.start(now); osc.stop(now + 0.34);
  } else if (kind === 'snare') {
    const src = ctx.createBufferSource();
    src.buffer = buildDrumNoiseBuffer(ctx, 0.25);
    const bp = ctx.createBiquadFilter();
    bp.type = 'bandpass'; bp.frequency.value = 1900; bp.Q.value = 0.9;
    const g = ctx.createGain();
    g.gain.setValueAtTime(0.55 * velocity, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.22);
    const tone = ctx.createOscillator();
    const tg = ctx.createGain();
    tone.type = 'triangle'; tone.frequency.setValueAtTime(190, now);
    tg.gain.setValueAtTime(0.5 * velocity, now);
    tg.gain.exponentialRampToValueAtTime(0.001, now + 0.16);
    src.connect(bp); bp.connect(g); g.connect(out);
    tone.connect(tg); tg.connect(out);
    src.start(now); tone.start(now); tone.stop(now + 0.18);
  } else if (kind === 'hihat_closed') {
    const src = ctx.createBufferSource();
    src.buffer = buildDrumNoiseBuffer(ctx, 0.08);
    const hp = ctx.createBiquadFilter();
    hp.type = 'highpass'; hp.frequency.value = 7200;
    const g = ctx.createGain();
    g.gain.setValueAtTime(0.28 * velocity, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.06);
    src.connect(hp); hp.connect(g); g.connect(out);
    src.start(now);
  } else if (kind === 'hihat_open') {
    const src = ctx.createBufferSource();
    src.buffer = buildDrumNoiseBuffer(ctx, 0.3);
    const hp = ctx.createBiquadFilter();
    hp.type = 'highpass'; hp.frequency.value = 6200;
    const g = ctx.createGain();
    g.gain.setValueAtTime(0.3 * velocity, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.26);
    src.connect(hp); hp.connect(g); g.connect(out);
    src.start(now);
  } else if (kind === 'crash') {
    const src = ctx.createBufferSource();
    src.buffer = buildDrumNoiseBuffer(ctx, 0.6);
    const hp = ctx.createBiquadFilter();
    hp.type = 'highpass'; hp.frequency.value = 4200;
    const g = ctx.createGain();
    g.gain.setValueAtTime(0.35 * velocity, now);
    g.gain.exponentialRampToValueAtTime(0.001, now + 0.55);
    src.connect(hp); hp.connect(g); g.connect(out);
    src.start(now);
  }
}

function changesBeatMs() {
  const base = Math.max(180, Math.round(60000 / Math.max(40, state.changesBpm)));
  return (state.changesTimeSig === '6/8') ? Math.max(150, Math.round(base * 0.67)) : base;
}

function getChangesBeatsPerBar() {
  if (state.changesTimeSig === '3/4') return 3;
  if (state.changesTimeSig === '2/2') return 2;
  if (state.changesTimeSig === '6/8') return 6;
  return 4; // 4/4 oraz 12/8 (4 pulsy z podziałem triolowym)
}

function getChangesAttackMul() {
  const raw = (state.changesAttackStrength !== undefined) ? Number(state.changesAttackStrength) : 0.75;
  if (!Number.isFinite(raw)) return 0.75;
  return Math.max(0.20, Math.min(1.0, raw));
}

function getChangesSwingRatio() {
  return (state.changesFeel === 'Blues' || state.changesFeel === 'Swing Jazz') ? 0.63 : 0.50;
}

// Bezpieczna aktualizacja opcji w panelu Zmian: natychmiast zapisuje, zachowuje scroll panelu i aktualizuje tempo/sekwencję na żywo!
function updateChangesOption(patch) {
  const prevBpm = state.changesBpm;
  const prevTs = state.changesTimeSig;
  const prevRep = state.changesRepeat;
  Object.assign(state, patch);
  state.changesBass = 'Wył.';
  if (state.changesMeasures) {
    const bpb = getChangesBeatsPerBar();
    state.changesMeasures.forEach(m => ensureChangesBarBeatsLength(m, bpb));
  }
  if (state.changesRepeat !== prevRep && state.changesIsPlaying) {
    state.changesPlaybackSeq = buildChangesPlaybackSequence();
    if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
      state.changesPlaybackSeqIdx = 0;
    }
  }
  saveSettings();
  if (state.changesIsPlaying && (state.changesBpm !== prevBpm || state.changesTimeSig !== prevTs)) {
    restartChangesPlaybackTimer();
  }
  renderChangesPreservingScroll();
}

function renderChangesPreservingScroll() {
  const insp = document.getElementById('rightInspector');
  const stageWrap = document.querySelector('.main-stage');
  const inspTop = insp ? insp.scrollTop : 0;
  const stageTop = stageWrap ? stageWrap.scrollTop : 0;
  renderStageAndInspector();
  if (insp) insp.scrollTop = inspTop;
  if (stageWrap) stageWrap.scrollTop = stageTop;
}

// Czysty odsłuch samego akordu przy kliknięciu kafelka (bez perkusji i metronomu!)
function previewChangesChordAudio(resolvedChord) {
  if (!resolvedChord || !resolvedChord.intervals || resolvedChord.intervals.length === 0) return;
  ensureAudioCtx();
  const inst = (state.changesCompInst && state.changesCompInst !== 'off') ? state.changesCompInst : 'clean';
  const gainScale = 1.0 / Math.sqrt(Math.max(1, resolvedChord.intervals.length));
  resolvedChord.intervals.forEach((iv, i) => {
    const freq = midiToFreq(resolvedChord.rootMidi + iv);
    setTimeout(() => {
      if (inst === 'clean' || inst === 'crunch') {
        const prevTone = state.guitarTonePreset;
        state.guitarTonePreset = inst;
        pluckGuitarFrequency(freq, 0.85, gainScale);
        state.guitarTonePreset = prevTone;
      } else {
        const prevKbd = state.keyboardPreset;
        state.keyboardPreset = inst;
        playKeyboardPresetNote(freq, 0.85, gainScale);
        state.keyboardPreset = prevKbd;
      }
    }, i * 20);
  });
}

// Odtwarza perkusję dla pojedynczej miary taktu, zgodnie z metrum, stylem, siłą ataku i groovem.
function playChangesDrumStep(beatIdx) {
  if (!state.changesBackingBand || !state.changesDrums) return;
  const raw = (state.changesDrumStyle === 'auto') ? state.changesFeel : state.changesDrumStyle;
  const style = String(raw || 'rock').toLowerCase();
  const beatsPerBar = getChangesBeatsPerBar();
  const beatMs = changesBeatMs();
  const swingRatio = getChangesSwingRatio();
  const isTriplet12_8 = (state.changesTimeSig === '12/8') || (state.changesFeel === 'Blues' && state.changesGroove === 'slow12_8');
  const isSwingStyle = style.includes('swing');
  const atk = 0.35 + 0.90 * getChangesAttackMul();

  if (beatsPerBar === 3) {
    // Walc 3/4: kick na 1, delikatny werbel na 2 i 3
    if (beatIdx === 0) playDrumHit('kick', 0.95 * atk);
    else playDrumHit('snare', 0.55 * atk);
  } else if (beatsPerBar === 2) {
    if (beatIdx === 0) playDrumHit('kick', 0.95 * atk);
    else playDrumHit('snare', 0.9 * atk);
  } else if (beatsPerBar === 6) {
    // 6/8: kick na 1, werbel na 4
    if (beatIdx === 0) playDrumHit('kick', 0.95 * atk);
    if (beatIdx === 3) playDrumHit('snare', 0.9 * atk);
  } else if (style === 'half_time') {
    if (beatIdx === 0) playDrumHit('kick', 1.0 * atk);
    if (beatIdx === 2) playDrumHit('snare', 1.0 * atk);
  } else if (isSwingStyle) {
    // Jazz Swing: lekkie stopy (feathering) + akcent hi-hatu na 2 i 4
    if (beatIdx === 0) playDrumHit('kick', 0.45 * atk);
    if (beatIdx === 1 || beatIdx === 3) playDrumHit('hihat_open', 0.55 * atk);
  } else if (style === 'funk') {
    if (beatIdx === 0 || beatIdx === 2) playDrumHit('kick', 0.95 * atk);
    if (beatIdx === 1 || beatIdx === 3) playDrumHit('snare', 0.95 * atk);
    if (beatIdx === 3) playDrumHit('hihat_open', 0.65 * atk);
    if (beatIdx === 1) {
      setTimeout(() => {
        if (state.changesIsPlaying && !state.changesIsPaused && state.changesBackingBand && state.changesDrums) {
          playDrumHit('kick', 0.68 * atk);
        }
      }, beatMs * 0.5);
    }
  } else if (style === 'pop') {
    if (beatIdx === 0 || beatIdx === 2) playDrumHit('kick', 0.9 * atk);
    if (beatIdx === 1 || beatIdx === 3) playDrumHit('snare', 0.8 * atk);
  } else {
    // rock / blues / prosto — kick na 1 i 3, werbel na 2 i 4
    if (beatIdx === 0 || beatIdx === 2) playDrumHit('kick', 0.95 * atk);
    if (beatIdx === 1 || beatIdx === 3) playDrumHit('snare', 0.9 * atk);
  }

  playDrumHit('hihat_closed', ((beatIdx === 0) ? 0.85 : 0.6) * atk);
  if (isTriplet12_8) {
    setTimeout(() => {
      if (state.changesIsPlaying && !state.changesIsPaused && state.changesBackingBand && state.changesDrums) {
        playDrumHit('hihat_closed', 0.42 * atk);
      }
    }, beatMs * 0.33);
    setTimeout(() => {
      if (state.changesIsPlaying && !state.changesIsPaused && state.changesBackingBand && state.changesDrums) {
        playDrumHit('hihat_closed', 0.52 * atk);
      }
    }, beatMs * 0.67);
  } else if (isSwingStyle && beatsPerBar === 4) {
    if (beatIdx === 1 || beatIdx === 3) {
      const swDelay = beatMs * Math.max(0.60, swingRatio);
      setTimeout(() => {
        if (state.changesIsPlaying && !state.changesIsPaused && state.changesBackingBand && state.changesDrums) {
          playDrumHit('hihat_closed', 0.58 * atk);
        }
      }, swDelay);
    }
  } else if (beatsPerBar !== 6) {
    const offDelay = beatMs * swingRatio;
    setTimeout(() => {
      if (state.changesIsPlaying && !state.changesIsPaused && state.changesBackingBand && state.changesDrums) {
        playDrumHit('hihat_closed', 0.5 * atk);
      }
    }, offDelay);
  }
}

function triggerChangesCompChord(resolvedChord, dur, volScale = 1.0) {
  if (!resolvedChord || !resolvedChord.intervals || state.changesCompInst === 'off') return;
  const atkMul = getChangesAttackMul();
  const effVol = volScale * (0.35 + 0.85 * atkMul);
  const gainScale = (effVol / Math.sqrt(Math.max(1, resolvedChord.intervals.length)));
  const strumDelayMs = Math.round(32 - 20 * atkMul);
  resolvedChord.intervals.forEach((iv, i) => {
    const freq = midiToFreq(resolvedChord.rootMidi + iv);
    setTimeout(() => {
      if (!state.changesBackingBand || state.changesCompInst === 'off') return;
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
    }, i * strumDelayMs);
  });
}

function playChangesBackingStepAudio(resolvedChord, beatIdx, isNewChordOnBeat = false) {
  ensureAudioCtx();

  // 1. Klik metronomu działa niezależnie od zespołu podkładowego
  if (state.changesMetronomeClick) {
    playMetronomeTickAudio(beatIdx === 0);
  }

  // 2. Jeśli Zespół akompaniujący jest wyłączony, nic z zespołu nie gra
  if (!state.changesBackingBand) return;

  // 3. Perkusja (sprawdza state.changesDrums)
  playChangesDrumStep(beatIdx);

  if (!resolvedChord) return;

  // 4. Instrument akompaniamentu (zawsze na 1. mierze oraz przy każdej zmianie akordu w środku taktu,
  //    a przy włączonym Akompaniamencie rytmicznym również dodatkowe akcenty rytmiczne)
  const beatsPerBar = getChangesBeatsPerBar();
  const isRhythmBeat = state.changesRhythmGuitar && (
    (beatsPerBar === 4 && (beatIdx === 2 || beatIdx === 3)) ||
    (beatsPerBar === 3 && (beatIdx === 1 || beatIdx === 2)) ||
    (beatsPerBar === 6 && beatIdx === 3) ||
    (beatsPerBar === 2 && beatIdx === 1)
  );
  if (state.changesCompInst !== 'off' && (beatIdx === 0 || isNewChordOnBeat || isRhythmBeat)) {
    const isMainAttack = (beatIdx === 0 || isNewChordOnBeat);
    const dur = isMainAttack ? 0.95 : 0.36;
    const vol = isMainAttack ? 1.0 : 0.68;
    triggerChangesCompChord(resolvedChord, dur, vol);
  }
}

let changesActiveStepCallback = null;

function restartChangesPlaybackTimer() {
  if (changesPlaybackTimer) {
    clearInterval(changesPlaybackTimer);
    changesPlaybackTimer = null;
  }
  if (state.changesIsPlaying && typeof changesActiveStepCallback === 'function') {
    changesPlaybackTimer = setInterval(changesActiveStepCallback, changesBeatMs());
  }
}

function stopChangesPlayback() {
  if (changesPlaybackTimer) {
    clearInterval(changesPlaybackTimer);
    changesPlaybackTimer = null;
  }
  changesActiveStepCallback = null;
  state.changesIsPlaying = false;
  state.changesIsPaused = false;
  state.changesActiveBarIdx = -1;
  state.changesActiveBeatIdx = -1;
  state.changesPlaybackSeq = [];
  state.changesPlaybackSeqIdx = 0;
  state.changesRepeatPass = 0;
  state.changesCountIn = 0;
}

function buildChangesLiveHeaderStatusHtml() {
  const measures = state.changesMeasures || [];
  if (measures.length === 0) return '';
  if (!state.changesIsPlaying) {
    return `<span style="font-size:11px;color:var(--text-muted);">${measures.length} taktów · kliknij takt, aby odsłuchać</span>`;
  }
  const beatsPerBar = getChangesBeatsPerBar();
  const activeBar = Math.max(0, Math.min(measures.length - 1, state.changesActiveBarIdx || 0));
  const activeBeat = Math.max(0, state.changesActiveBeatIdx || 0);
  const seq = (state.changesPlaybackSeq && state.changesPlaybackSeq.length > 0)
    ? state.changesPlaybackSeq
    : buildChangesPlaybackSequence();
  const seqIdx = Math.max(0, Math.min(state.changesPlaybackSeqIdx || 0, Math.max(0, seq.length - 1)));
  const curEntry = seq[seqIdx] || { passIdx: 0, totalPasses: 1 };
  const passStr = curEntry.totalPasses > 1 ? ` · Przejście ${curEntry.passIdx + 1}/${curEntry.totalPasses}` : '';
  const beatDots = Array.from({ length: beatsPerBar }, (_, bi) => {
    const lit = bi === activeBeat;
    const passed = bi < activeBeat;
    const col = lit ? 'var(--accent)' : (passed ? '#7c5534' : '#2e2620');
    return `<span style="display:inline-block;width:${lit ? 13 : 6}px;height:5px;border-radius:99px;background:${col};transition:0.08s;"></span>`;
  }).join('');
  return `
    <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
      <span style="font-size:11px;font-weight:700;color:var(--accent);">● Takt ${activeBar + 1}/${measures.length} · Miara ${activeBeat + 1}/${beatsPerBar}${passStr}</span>
      <span style="display:inline-flex;align-items:center;gap:3px;">${beatDots}</span>
    </div>`;
}

function updateChangesHighlightDom() {
  const beatsPerBar = getChangesBeatsPerBar();
  const beatPct = state.changesIsPlaying
    ? Math.min(100, Math.round(((Math.max(0, state.changesActiveBeatIdx) + 1) / Math.max(1, beatsPerBar)) * 100))
    : 0;

  // Odśwież zwięzły status w nagłówku Zapisu Akordowego bez pełnego przerenderowania panelu
  const hdrEl = document.getElementById('changesLiveHeaderStatus');
  if (hdrEl) hdrEl.innerHTML = buildChangesLiveHeaderStatusHtml();

  // Podświetl aktywny takt i pasek postępu w Zapisie Akordowym
  document.querySelectorAll('.changes-chart-card').forEach((el, idx) => {
    const progEl = el.querySelector('.changes-card-prog');
    if (idx === state.changesActiveBarIdx && state.changesIsPlaying) {
      el.classList.add('active-playing');
      if (progEl) progEl.style.width = beatPct + '%';
      if (state.changesActiveBeatIdx === 0) {
        try { el.scrollIntoView({ behavior: 'smooth', block: 'nearest' }); } catch (e) {}
      }
    } else {
      el.classList.remove('active-playing');
      if (progEl) progEl.style.width = '0%';
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

function seekChangesBar(barIdx) {
  ensureChangesProgressionLoaded();
  if (!state.changesMeasures || !state.changesMeasures[barIdx]) return;
  if (state.changesIsPlaying) {
    const seq = state.changesPlaybackSeq && state.changesPlaybackSeq.length > 0
      ? state.changesPlaybackSeq
      : buildChangesPlaybackSequence();
    const foundIdx = seq.findIndex(item => item.barIdx === barIdx);
    state.changesPlaybackSeqIdx = foundIdx >= 0 ? foundIdx : 0;
    state.changesActiveBarIdx = barIdx;
    state.changesActiveBeatIdx = 0;
    updateChangesHighlightDom();
  } else {
    state.changesActiveBarIdx = barIdx;
    const ch = resolveChangesSlot(getEffectiveChangesSlot(state.changesMeasures[barIdx], 0));
    if (ch) previewChangesChordAudio(ch);
    updateChangesHighlightDom();
  }
}

function toggleChangesEditorPlayback() {
  if (state.changesIsPlaying) {
    stopChangesPlayback();
    renderChangesPreservingScroll();
    return;
  }
  ensureChangesProgressionLoaded();
  if (!state.changesMeasures || state.changesMeasures.length === 0) return;

  state.changesPlaybackSeq = buildChangesPlaybackSequence();
  state.changesPlaybackSeqIdx = 0;
  state.changesIsPlaying = true;
  state.changesIsPaused = false;
  state.changesActiveBarIdx = state.changesPlaybackSeq[0].barIdx;
  state.changesActiveBeatIdx = 0;
  state.changesRepeatPass = 0;
  renderChangesPreservingScroll();

  const fireStep = () => {
    if (!state.changesIsPlaying || state.changesIsPaused) return;
    const beatsPerBar = getChangesBeatsPerBar();
    if (!state.changesPlaybackSeq || state.changesPlaybackSeq.length === 0) {
      state.changesPlaybackSeq = buildChangesPlaybackSequence();
    }
    if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
      state.changesPlaybackSeqIdx = 0;
    }
    const seqEntry = state.changesPlaybackSeq[state.changesPlaybackSeqIdx];
    state.changesActiveBarIdx = seqEntry.barIdx;
    state.changesRepeatPass = seqEntry.passIdx;
    if (state.changesActiveBeatIdx >= beatsPerBar) state.changesActiveBeatIdx = 0;

    const bar = state.changesMeasures[state.changesActiveBarIdx];
    if (!bar) return;
    ensureChangesBarBeatsLength(bar, beatsPerBar);
    const beatIdxNow = state.changesActiveBeatIdx;
    const isNewChordOnBeat = !!(bar.beats[beatIdxNow] && beatIdxNow > 0);
    const activeSlot = getEffectiveChangesSlot(bar, beatIdxNow);
    const resolved = resolveChangesSlot(activeSlot);
    playChangesBackingStepAudio(resolved, beatIdxNow, isNewChordOnBeat);
    updateChangesHighlightDom();

    state.changesActiveBeatIdx++;
    if (state.changesActiveBeatIdx >= beatsPerBar) {
      state.changesActiveBeatIdx = 0;
      state.changesPlaybackSeqIdx++;
      if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
        if (state.changesCycleKeys) {
          state.changesKeyRootPc = (state.changesKeyRootPc + 5) % 12; // Koło kwartowe
        }
        if (state.changesLoop) {
          state.changesPlaybackSeq = buildChangesPlaybackSequence();
          state.changesPlaybackSeqIdx = 0;
          if (state.changesCycleKeys) renderChangesPreservingScroll();
        } else {
          stopChangesPlayback();
          renderChangesPreservingScroll();
        }
      }
    }
  };

  changesActiveStepCallback = fireStep;
  fireStep();
  changesPlaybackTimer = setInterval(fireStep, changesBeatMs());
}

function openChangesBeatEditor(barIdx, beatIdx) {
  ensureChangesProgressionLoaded();
  const bar = state.changesMeasures[barIdx];
  if (!bar) return;
  ensureChangesBarBeatsLength(bar, getChangesBeatsPerBar());
  if (state.changesEditingSlot && state.changesEditingSlot.barIdx === barIdx && state.changesEditingSlot.beatIdx === beatIdx) {
    state.changesEditingSlot = null;
  } else {
    state.changesEditingSlot = { barIdx, beatIdx };
    const slot = bar.beats[beatIdx] || getEffectiveChangesSlot(bar, beatIdx);
    const ch = resolveChangesSlot(slot);
    if (ch) previewChangesChordAudio(ch);
  }
  renderChangesPreservingScroll();
}

function setChangesSlotValue(barIdx, beatIdx, offsetSemi, chordId) {
  ensureChangesProgressionLoaded();
  const bar = state.changesMeasures[barIdx];
  if (!bar) return;
  ensureChangesBarBeatsLength(bar, getChangesBeatsPerBar());
  const cleanSemi = ((parseInt(offsetSemi, 10) || 0) % 12 + 12) % 12;
  const cleanId = chordId || 'major';
  bar.beats[beatIdx] = {
    offsetSemi: cleanSemi,
    chordId: cleanId,
    roman: formatChangesRomanForSlot(cleanSemi, cleanId)
  };
  const ch = resolveChangesSlot(bar.beats[beatIdx]);
  if (ch) previewChangesChordAudio(ch);
  renderChangesPreservingScroll();
}

function clearChangesSlot(barIdx, beatIdx) {
  ensureChangesProgressionLoaded();
  const bar = state.changesMeasures[barIdx];
  if (!bar || beatIdx <= 0) return;
  bar.beats[beatIdx] = null;
  state.changesEditingSlot = null;
  renderChangesPreservingScroll();
}

function addChordToChangesMeasure(barIdx, targetBeatIdx = -1) {
  ensureChangesProgressionLoaded();
  const bar = state.changesMeasures[barIdx];
  if (!bar) return;
  const beatsPerBar = getChangesBeatsPerBar();
  ensureChangesBarBeatsLength(bar, beatsPerBar);
  const palette = state.changesIsMinor
    ? [
        { offsetSemi: 0, chordId: 'minor', roman: 'i' },
        { offsetSemi: 5, chordId: 'minor', roman: 'iv' },
        { offsetSemi: 7, chordId: '7', roman: 'V7' },
        { offsetSemi: 8, chordId: 'major', roman: '♭VI' },
        { offsetSemi: 10, chordId: 'major', roman: '♭VII' }
      ]
    : (state.changesFeel === 'Blues'
        ? [
            { offsetSemi: 0, chordId: '7', roman: 'I7' },
            { offsetSemi: 5, chordId: '7', roman: 'IV7' },
            { offsetSemi: 7, chordId: '7', roman: 'V7' },
            { offsetSemi: 2, chordId: 'm7', roman: 'ii7' },
            { offsetSemi: 9, chordId: 'm7', roman: 'vi7' }
          ]
        : [
            { offsetSemi: 0, chordId: 'major', roman: 'I' },
            { offsetSemi: 5, chordId: 'major', roman: 'IV' },
            { offsetSemi: 7, chordId: 'major', roman: 'V' },
            { offsetSemi: 9, chordId: 'minor', roman: 'vi' },
            { offsetSemi: 2, chordId: 'm7', roman: 'ii7' }
          ]);
  let beatIdx = targetBeatIdx;
  if (beatIdx < 0) {
    beatIdx = bar.beats.findIndex((b, idx) => idx > 0 && idx < beatsPerBar && !b);
    if (beatIdx < 0) beatIdx = Math.min(2, beatsPerBar - 1);
  }
  const cur = bar.beats[beatIdx];
  if (!cur) {
    bar.beats[beatIdx] = Object.assign({}, palette[1] || palette[0]);
  } else {
    const nextIdx = (palette.findIndex(p => p.offsetSemi === cur.offsetSemi && p.chordId === cur.chordId) + 1) % palette.length;
    bar.beats[beatIdx] = Object.assign({}, palette[nextIdx]);
  }
  state.changesEditingSlot = { barIdx, beatIdx };
  const ch = resolveChangesSlot(bar.beats[beatIdx]);
  if (ch) previewChangesChordAudio(ch);
  renderChangesPreservingScroll();
}

function addChangesMeasureBar() {
  ensureChangesProgressionLoaded();
  const bpb = getChangesBeatsPerBar();
  const beats = new Array(Math.max(4, bpb)).fill(null);
  beats[0] = getChangesDefaultTonicSlot();
  state.changesMeasures.push({ volta: 0, beats });
  if (state.changesIsPlaying) state.changesPlaybackSeq = buildChangesPlaybackSequence();
  renderChangesPreservingScroll();
}

function removeChangesMeasureBar(barIdx) {
  if (!state.changesMeasures || state.changesMeasures.length <= 1) return;
  state.changesMeasures.splice(barIdx, 1);
  if (state.changesActiveBarIdx >= state.changesMeasures.length) {
    state.changesActiveBarIdx = state.changesMeasures.length - 1;
  }
  if (state.changesEditingSlot) {
    if (state.changesEditingSlot.barIdx === barIdx) state.changesEditingSlot = null;
    else if (state.changesEditingSlot.barIdx > barIdx) state.changesEditingSlot.barIdx--;
  }
  if (state.changesIsPlaying) {
    state.changesPlaybackSeq = buildChangesPlaybackSequence();
    if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) state.changesPlaybackSeqIdx = 0;
  }
  renderChangesPreservingScroll();
}

function toggleChangesVolta() {
  ensureChangesProgressionLoaded();
  if (!state.changesMeasures || state.changesMeasures.length < 2) return;
  const hasAny = state.changesMeasures.some(m => m.volta > 0);
  if (hasAny) {
    state.changesMeasures.forEach(m => { m.volta = 0; });
  } else {
    state.changesMeasures.forEach(m => { m.volta = 0; });
    state.changesMeasures[state.changesMeasures.length - 2].volta = 1;
    state.changesMeasures[state.changesMeasures.length - 1].volta = 2;
    if (state.changesRepeat === 'Wył.') state.changesRepeat = '2x';
  }
  if (state.changesIsPlaying) {
    state.changesPlaybackSeq = buildChangesPlaybackSequence();
    state.changesPlaybackSeqIdx = 0;
  }
  saveSettings();
  renderChangesPreservingScroll();
}

function reharmonizeChangesProgression() {
  ensureChangesProgressionLoaded();
  const bpb = getChangesBeatsPerBar();
  const splitBeat = Math.min(2, Math.max(1, Math.floor(bpb / 2)));
  state.changesMeasures.forEach((bar, idx) => {
    ensureChangesBarBeatsLength(bar, bpb);
    const first = bar.beats[0];
    if (!first) return;
    if (first.offsetSemi === 7 && first.chordId === '7') {
      // Wstaw ii–SubV (substytucja trytonowa ♭II7 w miejsce V7)
      bar.beats[0] = { offsetSemi: 2, chordId: state.changesIsMinor ? 'm7b5' : 'm7', roman: state.changesIsMinor ? 'ii7♭5' : 'ii7' };
      bar.beats[splitBeat] = { offsetSemi: 1, chordId: '7', roman: '♭II7' };
    } else if (first.chordId === 'major') {
      first.chordId = 'maj7';
      first.roman = formatChangesRomanForSlot(first.offsetSemi, 'maj7');
    } else if (first.chordId === 'minor') {
      first.chordId = 'm7';
      first.roman = formatChangesRomanForSlot(first.offsetSemi, 'm7');
    } else if (idx % 4 === 3 && first.offsetSemi === 0 && !bar.beats[splitBeat]) {
      bar.beats[splitBeat] = { offsetSemi: 7, chordId: '7b9', roman: 'V7♭9' };
    }
  });
  renderChangesPreservingScroll();
}

function resetChangesProgressionToOriginal() {
  stopChangesPlayback();
  state.changesEditingSlot = null;
  ensureChangesProgressionLoaded(true);
  renderChangesPreservingScroll();
}

function showChangesToast(msg) {
  state.changesToastMsg = msg;
  renderChangesPreservingScroll();
  setTimeout(() => {
    if (state.changesToastMsg === msg) {
      state.changesToastMsg = '';
      const el = document.getElementById('changesToastBanner');
      if (el) el.remove();
    }
  }, 3200);
}

function saveChangesToWorkshop() {
  ensureChangesProgressionLoaded();
  const progs = state.initData.progressions;
  const activeProg = progs[state.selectedProgressionIdx] || progs[0];
  const labels = getRootLabels();
  const title = `${activeProg.title} (${labels[state.changesKeyRootPc]} ${state.changesIsMinor ? 'moll' : 'dur'} · ${state.changesBpm} BPM)`;
  const sesObj = {
    id: 'ses_chg_' + Date.now(),
    name: title,
    breakBars: 1,
    type: 'changes',
    changesSnapshot: {
      progressionIdx: state.selectedProgressionIdx,
      keyRootPc: state.changesKeyRootPc,
      isMinor: !!state.changesIsMinor,
      bpm: state.changesBpm,
      timeSig: state.changesTimeSig,
      feel: state.changesFeel,
      compInst: state.changesCompInst,
      attackStrength: getChangesAttackMul(),
      groove: state.changesGroove,
      repeat: state.changesRepeat,
      practiceGoal: state.changesPracticeGoal,
      measures: JSON.parse(JSON.stringify(state.changesMeasures || []))
    }
  };
  state.initData.sessions.push(sesObj);
  if (!Array.isArray(state.customSavedSessions)) state.customSavedSessions = [];
  state.customSavedSessions.push(sesObj);
  saveSettings();
  showChangesToast(`✓ Zapisano „${title}” w Warsztacie (możesz uruchomić z zakładki Warsztat)`);
}

function getChangesPracticeOrderedTargets() {
  if (!state.changesMeasures || state.changesActiveBarIdx < 0) return [];
  const bar = state.changesMeasures[state.changesActiveBarIdx] || state.changesMeasures[0];
  if (!bar) return [];
  const slot = getEffectiveChangesSlot(bar, Math.max(0, state.changesActiveBeatIdx || 0));
  const resolved = resolveChangesSlot(slot);
  if (!resolved) return [];

  if (state.changesPracticeDegrees === '1') return [resolved.rootPc];
  if (state.changesPracticeDegrees === '3' && resolved.pcs.length > 1) return [resolved.pcs[1]];
  if (state.changesPracticeDegrees === '5' && resolved.pcs.length > 2) return [resolved.pcs[2]];

  if (state.changesPracticeGoal === 'chord_scale') {
    const scaleOffsets = (resolved.chordId === '7' || resolved.chordId === '7b9') ? [0, 2, 4, 5, 7, 9, 10]
                       : (resolved.chordId === 'm7b5') ? [0, 1, 3, 5, 6, 8, 10]
                       : (resolved.chordId.startsWith('m') && resolved.chordId !== 'maj7') ? [0, 2, 3, 5, 7, 8, 10]
                       : [0, 2, 4, 5, 7, 9, 11];
    return scaleOffsets.map(o => (resolved.rootPc + o) % 12);
  }
  return Array.from(new Set(resolved.pcs));
}

function getActiveChangesPracticeTargetPcs() {
  return new Set(getChangesPracticeOrderedTargets());
}

function advanceChangesStudyBar() {
  state.changesPracticeHitPcs = [];
  state.changesPracticeSeqIdx = 0;
  state.changesPlaybackSeqIdx++;
  if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
    if (state.changesCycleKeys) {
      state.changesKeyRootPc = (state.changesKeyRootPc + 5) % 12;
      state.changesPlaybackSeq = buildChangesPlaybackSequence();
      state.changesPlaybackSeqIdx = 0;
    } else {
      finishChangesLivePractice();
      return;
    }
  }
  const entry = state.changesPlaybackSeq[state.changesPlaybackSeqIdx];
  state.changesActiveBarIdx = entry.barIdx;
  state.changesActiveBeatIdx = 0;
  state.changesRepeatPass = entry.passIdx;
  const bar = state.changesMeasures[state.changesActiveBarIdx];
  const resolved = resolveChangesSlot(getEffectiveChangesSlot(bar, 0));
  if (state.changesBackingBand && resolved) previewChangesChordAudio(resolved);
  renderStageAndInspector();
}

function startChangesLivePractice() {
  stopChangesPlayback();
  ensureChangesProgressionLoaded();
  state.changesPlaybackSeq = buildChangesPlaybackSequence();
  state.changesPlaybackSeqIdx = 0;
  state.changesScreen = 'practice_active';
  state.changesIsPlaying = true;
  state.changesIsPaused = false;
  state.changesActiveBarIdx = state.changesPlaybackSeq.length > 0 ? state.changesPlaybackSeq[0].barIdx : 0;
  state.changesActiveBeatIdx = 0;
  state.changesRepeatPass = 0;
  state.changesHits = 0;
  state.changesMisses = 0;
  state.changesCleanCount = 0;
  state.changesAlmostCount = 0;
  state.changesCloseCount = 0;
  state.changesEvaluatedBar = -1;
  state.changesPracticeHitPcs = [];
  state.changesPracticeSeqIdx = 0;
  state.changesPracticeLastMidi = -1;
  state.changesPracticeLastMidiTime = 0;

  // Tryb ĆWICZEBNY (study): bez uciekającego timera — program czeka na zagranie wymaganych dźwięków każdego akordu!
  if (state.changesPracticeMode === 'study') {
    state.changesCountIn = 0;
    const bar0 = state.changesMeasures[state.changesActiveBarIdx];
    const res0 = resolveChangesSlot(getEffectiveChangesSlot(bar0, 0));
    if (state.changesBackingBand && res0) previewChangesChordAudio(res0);
    renderAll();
    return;
  }

  state.changesCountIn = 3;
  renderAll();

  const firePracticeStep = () => {
    if (!state.changesIsPlaying || state.changesIsPaused) return;
    if (state.changesCountIn > 0) {
      playMetronomeTickAudio(state.changesCountIn === 3);
      state.changesCountIn--;
      renderStageAndInspector();
      return;
    }

    const beatsPerBar = getChangesBeatsPerBar();
    if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
      state.changesPlaybackSeqIdx = 0;
    }
    const seqEntry = state.changesPlaybackSeq[state.changesPlaybackSeqIdx];
    const barChanged = (state.changesActiveBarIdx !== seqEntry.barIdx);
    state.changesActiveBarIdx = seqEntry.barIdx;
    state.changesRepeatPass = seqEntry.passIdx;
    if (state.changesActiveBeatIdx >= beatsPerBar) state.changesActiveBeatIdx = 0;
    if (barChanged || state.changesActiveBeatIdx === 0) {
      state.changesPracticeHitPcs = [];
      state.changesPracticeSeqIdx = 0;
    }

    const bar = state.changesMeasures[state.changesActiveBarIdx];
    const beatIdxNow = state.changesActiveBeatIdx;
    const isNewChordOnBeat = !!(bar && bar.beats[beatIdxNow] && beatIdxNow > 0);
    if (isNewChordOnBeat) {
      state.changesPracticeHitPcs = [];
      state.changesPracticeSeqIdx = 0;
    }
    const slot = bar ? getEffectiveChangesSlot(bar, beatIdxNow) : null;
    const resolved = resolveChangesSlot(slot);
    playChangesBackingStepAudio(resolved, beatIdxNow, isNewChordOnBeat);
    renderStageAndInspector();

    state.changesActiveBeatIdx++;
    if (state.changesActiveBeatIdx >= beatsPerBar) {
      state.changesActiveBeatIdx = 0;
      state.changesPlaybackSeqIdx++;
      if (state.changesPlaybackSeqIdx >= state.changesPlaybackSeq.length) {
        if (state.changesCycleKeys) {
          state.changesKeyRootPc = (state.changesKeyRootPc + 5) % 12;
          state.changesPlaybackSeq = buildChangesPlaybackSequence();
          state.changesPlaybackSeqIdx = 0;
          renderStageAndInspector();
        } else {
          finishChangesLivePractice();
        }
      }
    }
  };
  changesActiveStepCallback = firePracticeStep;
  changesPlaybackTimer = setInterval(firePracticeStep, changesBeatMs());
}

function finishChangesLivePractice() {
  stopChangesPlayback();
  state.changesScreen = 'practice_summary';
  renderAll();
}

function checkChangesPracticePlayedMidi(playedMidi) {
  if (state.mainView !== 'changes' || state.changesScreen !== 'practice_active') return;
  if (state.changesCountIn > 0 || state.changesIsPaused) return;
  const nowMs = Date.now();
  if (playedMidi === state.changesPracticeLastMidi && (nowMs - state.changesPracticeLastMidiTime) < 260) {
    return;
  }
  state.changesPracticeLastMidi = playedMidi;
  state.changesPracticeLastMidiTime = nowMs;

  const pc = ((playedMidi % 12) + 12) % 12;
  const orderedTargets = getChangesPracticeOrderedTargets();
  const targetSet = new Set(orderedTargets);

  if (state.changesPracticeOrder === 'seq' && orderedTargets.length > 0) {
    const expectedPc = orderedTargets[Math.min(state.changesPracticeSeqIdx, orderedTargets.length - 1)];
    if (pc === expectedPc) {
      state.changesHits++;
      state.changesCleanCount++;
      if (!state.changesPracticeHitPcs.includes(pc)) state.changesPracticeHitPcs.push(pc);
      state.changesPracticeSeqIdx++;
      if (state.changesPracticeMode === 'study' && state.changesPracticeSeqIdx >= orderedTargets.length) {
        advanceChangesStudyBar();
        return;
      }
      if (state.changesPracticeSeqIdx >= orderedTargets.length) {
        state.changesPracticeSeqIdx = 0;
      }
    } else if (targetSet.has(pc)) {
      // Dźwięk należy do akordu/skali, ale w innej kolejności -> „prawie”
      state.changesAlmostCount++;
    } else {
      state.changesMisses++;
      state.changesCloseCount++;
    }
  } else {
    if (targetSet.has(pc)) {
      state.changesHits++;
      state.changesCleanCount++;
      if (!state.changesPracticeHitPcs.includes(pc)) state.changesPracticeHitPcs.push(pc);
      if (state.changesPracticeMode === 'study' && state.changesPracticeHitPcs.length >= orderedTargets.length) {
        advanceChangesStudyBar();
        return;
      }
    } else {
      state.changesMisses++;
      state.changesCloseCount++;
    }
  }
  renderStageAndInspector();
}

function buildChangesLiveFretboardSvg(resolvedChord) {
  const W = 1280, H = 215;
  const leftNutX = 52, rightPad = 18, topPad = 20, bottomPad = 30;
  const maxFret = 21;
  const boardW = W - leftNutX - rightPad;
  const boardH = H - topPad - bottomPad;
  const fretW = boardW / maxFret;
  const stringH = boardH / 5;
  const orderedTargets = getChangesPracticeOrderedTargets();
  const targets = new Set(orderedTargets);
  const nextSeqPc = (state.changesPracticeOrder === 'seq' && orderedTargets.length > 0)
    ? orderedTargets[Math.min(state.changesPracticeSeqIdx, orderedTargets.length - 1)]
    : -1;
  const hitSet = new Set(state.changesPracticeHitPcs || []);
  const rootPc = resolvedChord ? resolvedChord.rootPc : getChangesTransposeRootPc();
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
      const isNextSeq = (nextSeqPc >= 0 && pc === nextSeqPc);
      const isHit = hitSet.has(pc);
      const isRoot = (pc === rootPc);
      const fill = isNextSeq ? '#16a34a' : (isHit ? '#15803d' : (isRoot ? '#9e420e' : '#3d2a1b'));
      const stroke = isNextSeq ? '#bbf7d0' : (isHit ? '#4ade80' : (isRoot ? '#d97724' : '#8a6240'));
      const txtCol = '#fff5eb';
      const nm = labels[pc];
      svg += `
        <g style="cursor:pointer" onmousedown="startGuitarSustainedVoice('fret_mouse', ${midiToFreq(midi)}, 10);state.isHoldingFretNote=true;checkChangesPracticePlayedMidi(${midi})">
          <circle cx="${cx}" cy="${y}" r="12" fill="${fill}" stroke="${stroke}" stroke-width="${isNextSeq ? '2.2' : '1.5'}"/>
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
  const rootPcClean = getChangesTransposeRootPc();
  const relMajPc = state.changesIsMinor ? (rootPcClean + 3) % 12 : rootPcClean;
  const relMinPc = state.changesIsMinor ? rootPcClean : (rootPcClean + 9) % 12;
  const keyLabels = [1, 3, 5, 8, 10].includes(relMajPc)
    ? ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B']
    : (relMajPc === 6 ? labels : ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B']);
  const keyName = state.changesIsMinor
    ? `${keyLabels[rootPcClean]} moll (wzgl. ${keyLabels[relMajPc]} dur)`
    : `${keyLabels[rootPcClean]} dur (wzgl. ${keyLabels[relMinPc]} moll)`;
  const beatsPerBar = getChangesBeatsPerBar();
  const barCount = (state.changesMeasures || []).length;
  const chartCols = (barCount <= 4) ? Math.max(1, barCount)
    : (barCount <= 6) ? barCount
    : (barCount <= 8) ? 4
    : (barCount === 16 ? 8 : 6);
  const primaryKeyBadge = state.changesIsMinor ? `${keyLabels[rootPcClean]} moll` : `${keyLabels[rootPcClean]} dur`;
  const relKeyNote = state.changesIsMinor ? `wzgl. ${keyLabels[relMajPc]} dur` : `wzgl. ${keyLabels[relMinPc]} moll`;

  // =========================================================================
  // EKRAN 5: SESJA ZAKOŃCZONA
  // =========================================================================
  if (state.changesScreen === 'practice_summary') {
    const total = state.changesHits + state.changesAlmostCount + state.changesMisses;
    const accStr = total > 0 ? `${Math.round((state.changesHits / total) * 100)}%` : '—';
    const subMsg = total > 0
      ? `Oceniono dźwięków: ${total}.`
      : 'Brak ocenionych dźwięków.';
    stage.innerHTML = `
      <div style="flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:14px;padding:40px 20px;">
        <div style="font-size:12px;letter-spacing:0.12em;color:var(--text-muted);font-weight:700;">SESJA ZAKOŃCZONA</div>
        <div style="font-size:36px;font-weight:800;color:var(--accent);">${accStr}</div>
        <div style="font-size:11px;letter-spacing:0.1em;color:var(--text-muted);font-weight:700;">CELNOŚĆ</div>
        <div style="font-size:13px;color:var(--text-muted);">${state.changesCleanCount} czyste · ${state.changesAlmostCount} prawie · ${state.changesCloseCount} błędne</div>
        <div style="font-size:12px;color:var(--text-muted);margin-bottom:24px;">${subMsg}</div>
        <button class="action-btn" style="width:100%;max-width:720px;padding:14px;font-size:15px;"
                onclick="startChangesLivePractice()">Ćwicz ponownie</button>
        <button class="chip-btn" style="width:100%;max-width:720px;padding:14px;font-size:15px;"
                onclick="state.changesScreen='editor';renderAll()">Wstecz do edytora</button>
      </div>`;
    return;
  }

  // =========================================================================
  // EKRAN 4: AKTYWNE GRANIE NA ZMIANACH Z PODŚWIETLANIEM AKORDÓW I GRYFEM
  // =========================================================================
  if (state.changesScreen === 'practice_active') {
    const activeBar = state.changesMeasures[state.changesActiveBarIdx] || state.changesMeasures[0];
    const activeSlot = activeBar ? getEffectiveChangesSlot(activeBar, Math.max(0, state.changesActiveBeatIdx || 0)) : null;
    const currentResolved = resolveChangesSlot(activeSlot);
    const isStudy = (state.changesPracticeMode === 'study');
    const orderedTargets = getChangesPracticeOrderedTargets();
    const hitSet = new Set(state.changesPracticeHitPcs || []);
    const totalAttempts = state.changesHits + state.changesAlmostCount + state.changesMisses;
    const accPct = totalAttempts > 0 ? `${Math.round((state.changesHits / totalAttempts) * 100)}%` : '—';
    const statusBadge = state.changesCountIn > 0
      ? `Odliczanie ${state.changesCountIn}`
      : (isStudy
          ? `Tryb ćwiczebny · Takt ${state.changesActiveBarIdx + 1}/${state.changesMeasures.length}`
          : `Takt ${state.changesActiveBarIdx + 1}/${state.changesMeasures.length} · Miara ${state.changesActiveBeatIdx + 1}/${beatsPerBar}`);
    const centerHeadline = state.changesCountIn > 0
      ? 'Przygotuj się...'
      : (currentResolved ? `Teraz graj: ${currentResolved.symbol} (${currentResolved.roman})` : 'Graj!');
    const targetPillsHtml = orderedTargets.map((pc, idx) => {
      const done = (state.changesPracticeOrder === 'seq') ? (idx < state.changesPracticeSeqIdx) : hitSet.has(pc);
      const isNext = (state.changesPracticeOrder === 'seq') && (idx === state.changesPracticeSeqIdx);
      const pillStyle = done
        ? 'background:rgba(74,222,128,0.20);border:1px solid #4ade80;color:#86efac;'
        : (isNext
            ? 'background:rgba(243,146,55,0.22);border:1.5px solid var(--accent);color:var(--accent);'
            : 'background:#16120f;border:1px solid #352c24;color:var(--text);');
      return `<span style="${pillStyle}padding:4px 10px;border-radius:7px;font-size:13px;font-weight:800;">${done ? '✓ ' : ''}${labels[pc]}</span>`;
    }).join('');

    stage.innerHTML = `
      <div style="display:flex;flex-direction:column;gap:12px;flex:1;">
        <!-- Górny pasek: Pauza/Pomiń / Stop / Pulpit + status i celność -->
        <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;">
          <div style="display:flex;gap:6px;flex-wrap:wrap;">
            ${isStudy ? `
              <button class="chip-btn" style="padding:7px 12px;" onclick="previewChangesChordAudio(resolveChangesSlot(getEffectiveChangesSlot(state.changesMeasures[state.changesActiveBarIdx], 0)))">🔊 Odsłuchaj akord</button>
              <button class="chip-btn" style="padding:7px 12px;" onclick="advanceChangesStudyBar()">Pomiń takt →</button>
            ` : `
              <button class="chip-btn" style="padding:7px 14px;"
                      onclick="state.changesIsPaused=!state.changesIsPaused;renderStageAndInspector()">
                ${state.changesIsPaused ? '▶ Wznów' : 'Pauza'}
              </button>
            `}
            <button class="action-btn" style="padding:7px 16px;" onclick="finishChangesLivePractice()">Stop</button>
            <button class="chip-btn" style="padding:7px 14px;" onclick="stopChangesPlayback();state.changesScreen='editor';renderAll()">Pulpit</button>
          </div>
          <div style="text-align:right;">
            <span style="background:rgba(243,146,55,0.18);color:var(--accent);border:1px solid var(--accent);padding:3px 10px;border-radius:6px;font-size:12px;font-weight:700;">
              ${statusBadge}
            </span>
            <div style="font-size:12px;color:var(--text-muted);margin-top:4px;font-family:monospace;">
              ${accPct} · ${state.changesHits} traf.${state.changesAlmostCount > 0 ? ` · ${state.changesAlmostCount} prawie` : ''} · ${state.changesMisses} błęd.
            </div>
          </div>
        </div>

        <!-- Symetryczna siatka wszystkich akordów progresji -->
        <div class="changes-chart-grid" style="--chart-cols:${chartCols};">
          ${state.changesMeasures.map((m, idx) => {
            const sum = getChangesBarSummarySlots(m, beatsPerBar);
            const sym = sum.length > 0 ? sum.map(s => s.resolved.symbol).join(' | ') : '—';
            const rom = sum.length > 0 ? sum.map(s => s.resolved.roman).join('·') : '';
            const isNow = (state.changesCountIn === 0 && idx === state.changesActiveBarIdx);
            return `
              <div class="changes-chart-card ${isNow ? 'active-playing' : ''}" style="cursor:default;">
                <div style="display:flex;justify-content:space-between;align-items:baseline;gap:4px;">
                  <span style="font-size:13.5px;font-weight:800;color:${isNow?'var(--accent)':'var(--text)'};white-space:nowrap;overflow:hidden;text-overflow:ellipsis;">${sym}</span>
                  <span style="font-size:10px;color:var(--text-muted);flex-shrink:0;">T${idx + 1} ${rom}</span>
                </div>
              </div>`;
          }).join('')}
        </div>

        <!-- Środkowy komunikat i lista dźwięków docelowych -->
        <div style="flex:1;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;min-height:95px;">
          <div style="font-size:21px;font-weight:800;">${centerHeadline}</div>
          <div style="display:flex;gap:6px;flex-wrap:wrap;justify-content:center;align-items:center;">
            <span style="font-size:12px;color:var(--text-muted);">${state.changesPracticeOrder === 'seq' ? 'Zagraj po kolei:' : 'Dźwięki docelowe:'}</span>
            ${targetPillsHtml}
          </div>
          <div style="font-size:11.5px;color:var(--text-muted);">
            ${isStudy
              ? 'Zagraj wszystkie podświetlone dźwięki na gitarze (mikrofon), gryfie lub klawiaturze, aby przejść dalej.'
              : 'Zagraj na gitarze (mikrofon), kliknij w gryf poniżej lub użyj klawiatury na dole.'}
          </div>
        </div>

        <!-- Przycisk Gryf i dolny interaktywny gryf -->
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
  // EKRAN 3: KONFIGURACJA „< ĆWICZENIE NA ZMIANACH”
  // =========================================================================
  if (state.changesScreen === 'practice_config') {
    stage.innerHTML = `
      <div style="max-width:680px;margin:10px auto;width:100%;display:flex;flex-direction:column;gap:18px;">
        <div style="display:flex;align-items:center;gap:12px;">
          <button class="chip-btn" style="padding:6px 12px;" onclick="state.changesScreen='editor';renderAll()">‹</button>
          <span class="section-label" style="margin:0;font-size:13px;">ĆWICZENIE NA ZMIANACH</span>
        </div>
        <div style="font-size:14px;color:var(--text-muted);line-height:1.5;">
          Graj linię na swoim zapisie. Cele podświetlają się przy każdym akordzie, a każdy dźwięk oceniany jest na żywo.
        </div>

        <div>
          <div class="section-label">CEL</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeGoal==='chord_tones'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeGoal:'chord_tones'})">Dźwięki akordu</button>
            <button class="chip-btn ${state.changesPracticeGoal==='chord_scale'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeGoal:'chord_scale'})">Skala akordu</button>
          </div>
        </div>

        <div>
          <div class="section-label">TRYB PRACY</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeMode==='tempo'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeMode:'tempo'})">W tempie</button>
            <button class="chip-btn ${state.changesPracticeMode==='study'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeMode:'study'})">Ćwiczebny (czeka na dźwięki)</button>
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
              <button class="chip-btn ${state.changesPracticeDegrees===d.id?'active':''}" style="min-width:64px;padding:8px 14px;"
                      onclick="updateChangesOption({changesPracticeDegrees:'${d.id}'})">${d.lbl}</button>
            `).join('')}
          </div>
        </div>

        <div>
          <div class="section-label">KOLEJNOŚĆ</div>
          <div style="display:inline-flex;gap:6px;background:#14100d;padding:4px;border-radius:10px;border:1px solid var(--border);">
            <button class="chip-btn ${state.changesPracticeOrder==='any'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeOrder:'any'})">Dowolna</button>
            <button class="chip-btn ${state.changesPracticeOrder==='seq'?'active':''}" style="padding:8px 16px;"
                    onclick="updateChangesOption({changesPracticeOrder:'seq'})">Po kolei</button>
          </div>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;padding-top:6px;border-top:1px solid var(--border);">
          <div>
            <div style="font-size:14.5px;font-weight:700;">Przechodź przez tonacje</div>
            <div style="font-size:12px;color:var(--text-muted);margin-top:2px;">
              Po każdym przejściu automatycznie przesuwa progresję po kole kwart (+5 półtonów).
            </div>
          </div>
          <button class="ear-toggle-switch ${state.changesCycleKeys?'on':''}"
                  onclick="updateChangesOption({changesCycleKeys:!state.changesCycleKeys})"></button>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;">
          <div style="font-size:14.5px;font-weight:700;">Zespół podkładowy</div>
          <button class="ear-toggle-switch ${state.changesBackingBand?'on':''}"
                  onclick="updateChangesOption({changesBackingBand:!state.changesBackingBand})"></button>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;">
          <div style="font-size:14.5px;font-weight:700;">Klik metronomu</div>
          <button class="ear-toggle-switch ${state.changesMetronomeClick?'on':''}"
                  onclick="updateChangesOption({changesMetronomeClick:!state.changesMetronomeClick})"></button>
        </div>

        <div style="padding:12px 14px;background:#16120f;border:1px solid var(--border);border-radius:10px;font-size:12.5px;color:var(--text-muted);">
          ${state.changesPracticeMode === 'study'
            ? 'Tryb ćwiczebny: program gra wzorcowy akord i czeka, aż zagrasz wszystkie dźwięki docelowe danego taktu.'
            : (state.changesBackingBand
                ? `Włączony: gra zespół akompaniujący (${state.changesCompInst === 'off' ? 'bez akordów' : 'akordy'}${state.changesDrums ? ' + perkusja' : ''}${state.changesMetronomeClick ? ' + metronom' : ''}).`
                : (state.changesMetronomeClick
                    ? 'Zespół wyłączony: gra wyłącznie klik metronomu.'
                    : 'Wyłączony: ćwiczenie działa w ciszy, tylko z mikrofonem i licznikiem na ekranie.'))}
        </div>

        <button class="action-btn" style="width:100%;padding:14px;font-size:16px;"
                onclick="startChangesLivePractice()">▶ Rozpocznij ćwiczenie</button>
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
  // EKRAN 1 & 2: GŁÓWNY EDYTOR I ODTWARZACZ PROGRESJI (kompaktowy układ 4 takty/wiersz + linia na żywo)
  // =========================================================================
  const compInstLabels = {
    'hammond': 'Organy',
    'clean': 'Gitara czysta',
    'piano': 'Fortepian',
    'rhodes': 'Pianino elektr.',
    'crunch': 'Gitara przesterowana',
    'off': 'Bez akordów'
  };
  const grooveLabel = (state.changesFeel === 'Blues')
    ? (state.changesGroove === 'shuffle' ? 'Shuffle' : 'Wolny 12/8')
    : state.changesFeel;
  const activeLayers = [];
  if (state.changesBackingBand) {
    if (state.changesCompInst !== 'off') activeLayers.push(compInstLabels[state.changesCompInst] || 'Organy');
    if (state.changesDrums) activeLayers.push('Perkusja');
  }
  if (state.changesMetronomeClick) activeLayers.push('Metronom');
  if (state.changesRepeat !== 'Wył.') activeLayers.push(`Powtórz ${state.changesRepeat}`);
  if (state.changesLoop) activeLayers.push('W pętli');
  const subBanner = activeLayers.length > 0
    ? `${state.changesFeel} · ${state.changesTimeSig} · ${activeLayers.join(' · ')}`
    : `${state.changesFeel} · ${state.changesTimeSig} · Podkład wyciszony`;
  const beatIndices = Array.from({ length: beatsPerBar }, (_, i) => i);
  const hasAnyVolta = (state.changesMeasures || []).some(m => m.volta > 0);

  // Szybki 1-liniowy edytor wybranej miary w Aranżacji
  let beatEditorHtml = '';
  if (state.changesEditingSlot && state.changesMeasures && state.changesMeasures[state.changesEditingSlot.barIdx]) {
    const eb = state.changesEditingSlot.barIdx;
    const ebt = state.changesEditingSlot.beatIdx;
    const eBar = state.changesMeasures[eb];
    ensureChangesBarBeatsLength(eBar, beatsPerBar);
    const curSlot = eBar.beats[ebt] || getEffectiveChangesSlot(eBar, ebt) || getChangesDefaultTonicSlot();
    const curSemi = ((curSlot.offsetSemi || 0) % 12 + 12) % 12;
    const curCid = curSlot.chordId || 'major';
    const chordQualityOpts = [
      ['major','dur'], ['minor','moll (m)'], ['7','7'], ['maj7','maj7'],
      ['m7','m7'], ['m7b5','m7♭5'], ['dim7','dim7'], ['7b9','7♭9']
    ];
    beatEditorHtml = `
      <div class="changes-chord-editor-bar">
        <span style="font-size:11.5px;font-weight:800;color:#38bdf8;">Edycja: Takt ${eb + 1} · Miara ${ebt + 1}</span>
        <select style="width:auto;padding:4px 8px;font-size:12px;"
                onchange="setChangesSlotValue(${eb}, ${ebt}, parseInt(this.value,10), '${curCid}')">
          ${Array.from({ length: 12 }, (_, semi) => {
            const pc = (rootPcClean + semi) % 12;
            const rom = formatChangesRomanForSlot(semi, curCid);
            return `<option value="${semi}" ${semi===curSemi?'selected':''}>${labels[pc]} (${rom})</option>`;
          }).join('')}
        </select>
        <select style="width:auto;padding:4px 8px;font-size:12px;"
                onchange="setChangesSlotValue(${eb}, ${ebt}, ${curSemi}, this.value)">
          ${chordQualityOpts.map(([id, lbl]) => `<option value="${id}" ${id===curCid?'selected':''}>${lbl}</option>`).join('')}
        </select>
        <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;"
                onclick="previewChangesChordAudio(resolveChangesSlot(state.changesMeasures[${eb}].beats[${ebt}] || getEffectiveChangesSlot(state.changesMeasures[${eb}], ${ebt})))">🔊 Odsłuchaj</button>
        ${ebt > 0 && eBar.beats[ebt] ? `
          <button class="chip-btn" style="padding:4px 9px;font-size:11.5px;color:#f87171;border-color:#7f1d1d;"
                  onclick="clearChangesSlot(${eb}, ${ebt})">✕ Wyczyść miarę</button>
        ` : ''}
        <button class="chip-btn" style="margin-left:auto;padding:4px 9px;font-size:11.5px;"
                onclick="state.changesEditingSlot=null;renderChangesPreservingScroll()">Gotowe</button>
      </div>`;
  }

  stage.innerHTML = `
    <div class="changes-workspace">
      ${state.changesToastMsg ? `<div id="changesToastBanner" style="background:rgba(34,197,94,0.14);border:1px solid #22c55e;color:#86efac;border-radius:9px;padding:8px 14px;font-size:12.5px;font-weight:700;">${state.changesToastMsg}</div>` : ''}

      <!-- Górny pasek: < Biblioteka + selektor progresji + metka stylu + Eksport .GP5 -->
      <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:10px;">
        <div style="display:flex;align-items:center;gap:9px;flex-wrap:wrap;">
          <button class="chip-btn" style="padding:6px 12px;font-size:12px;" onclick="state.changesScreen='library';renderAll()">‹ Biblioteka</button>
          <select style="width:auto;min-width:210px;padding:6px 12px;font-size:13.5px;font-weight:700;"
                  onchange="selectChangesProgression(parseInt(this.value,10))">
            ${progs.map((p, idx) => `<option value="${idx}" ${idx===state.selectedProgressionIdx?'selected':''}>${p.title}</option>`).join('')}
          </select>
          <span style="font-size:11.5px;font-weight:700;color:var(--text-muted);background:rgba(255,255,255,0.04);border:1px solid #2b241e;border-radius:7px;padding:4px 9px;">
            ${activeProg.style || state.changesFeel} · ${barCount} taktów
          </span>
        </div>
        <button class="chip-btn" style="padding:6px 12px;font-size:12px;"
                onclick="window.location.href='/api/export/progression_gp5?id=${activeProg.id}&keyRootPc=${state.changesKeyRootPc}&bpm=${state.changesBpm}'">💾 Eksport .GP5</button>
      </div>

      <!-- Karta odtwarzacza -->
      <div class="changes-player-card">
        <div style="display:flex;align-items:center;gap:15px;flex-wrap:wrap;">
          <button class="action-btn" style="padding:10px 24px;font-size:14.5px;min-width:116px;border-radius:10px;"
                  onclick="toggleChangesEditorPlayback()">
            ${state.changesIsPlaying ? '⏹ Stop' : '▶ Graj'}
          </button>
          <div style="display:flex;flex-direction:column;gap:4px;">
            <div style="display:flex;align-items:center;gap:8px;flex-wrap:wrap;">
              <span style="font-size:17px;font-weight:800;color:var(--text);">${activeProg.title}</span>
              <span style="font-size:12px;font-weight:800;color:var(--accent);background:rgba(243,146,55,0.14);border:1px solid rgba(243,146,55,0.35);padding:2px 8px;border-radius:6px;">${primaryKeyBadge}</span>
              <span style="font-size:11.5px;color:var(--text-muted);">${grooveLabel} · (${relKeyNote})</span>
            </div>
            <div style="font-size:11.5px;color:var(--text-muted);">${subBanner}</div>
          </div>
        </div>
        <div style="display:flex;align-items:center;gap:7px;background:#0e0b09;padding:5px 10px;border-radius:10px;border:1px solid #2b241e;">
          <button class="chip-btn" style="padding:4px 10px;font-size:13px;" onclick="updateChangesOption({changesBpm:Math.max(40,state.changesBpm-4)})">−</button>
          <div style="text-align:center;min-width:50px;">
            <div style="font-size:15px;font-weight:800;line-height:1.1;">${state.changesBpm}</div>
            <div style="font-size:9px;letter-spacing:0.06em;color:var(--text-muted);">BPM</div>
          </div>
          <button class="chip-btn" style="padding:4px 10px;font-size:13px;" onclick="updateChangesOption({changesBpm:Math.min(240,state.changesBpm+4)})">+</button>
        </div>
      </div>

      <!-- ZAPIS AKORDOWY: symetryczne frazy (6+6 dla 12 taktów, 4+4 dla 8 taktów) -->
      <div class="changes-section-box">
        <div style="display:flex;justify-content:space-between;align-items:center;gap:8px;flex-wrap:wrap;">
          <div class="section-label" style="margin:0;">ZAPIS AKORDOWY</div>
          <div id="changesLiveHeaderStatus">${buildChangesLiveHeaderStatusHtml()}</div>
        </div>

        <div class="changes-chart-grid" style="--chart-cols:${chartCols};">
          ${state.changesMeasures.map((m, barIdx) => {
            const sum = getChangesBarSummarySlots(m, beatsPerBar);
            const primaryCh = sum.length > 0 ? sum[0].resolved : null;
            const symTxt = sum.length > 0 ? sum.map(s => s.resolved.symbol).join(' | ') : '—';
            const romTxt = sum.length > 0 ? sum.map(s => s.resolved.roman).join('·') : '';
            const isPlayingThis = (state.changesIsPlaying && state.changesActiveBarIdx === barIdx);
            const chordTonesTxt = (state.changesChordDiagrams && primaryCh && primaryCh.pcs)
              ? primaryCh.pcs.map(pc => labels[pc]).join(' · ')
              : '';
            const beatPct = isPlayingThis
              ? Math.min(100, Math.round(((Math.max(0, state.changesActiveBeatIdx) + 1) / Math.max(1, beatsPerBar)) * 100))
              : 0;
            return `
              <div class="changes-chart-card ${isPlayingThis ? 'active-playing' : ''}"
                   onclick="seekChangesBar(${barIdx})">
                <div style="display:flex;justify-content:space-between;align-items:center;gap:4px;">
                  <span style="font-size:10px;font-weight:700;color:var(--text-muted);">T${barIdx + 1}${m.volta ? ` <b style="color:var(--accent);">W${m.volta}.</b>` : ''}</span>
                  ${romTxt ? `<span class="changes-roman-pill">${romTxt}</span>` : ''}
                </div>
                <div style="font-size:19px;font-weight:800;line-height:1.15;color:${isPlayingThis?'var(--accent)':'var(--text)'};white-space:nowrap;overflow:hidden;text-overflow:ellipsis;">${symTxt}</div>
                ${chordTonesTxt ? `<div style="font-size:10.5px;color:#9ca3af;font-family:monospace;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;">${chordTonesTxt}</div>` : ''}
                <div class="changes-card-prog" style="width:${beatPct}%;"></div>
              </div>`;
          }).join('')}
        </div>
      </div>

      <!-- ARANŻACJA: symetryczna siatka 4 takty w wierszu -->
      <div class="changes-section-box">
        <div style="display:flex;justify-content:space-between;align-items:center;gap:10px;flex-wrap:wrap;">
          <div style="display:flex;align-items:baseline;gap:8px;flex-wrap:wrap;">
            <div class="section-label" style="margin:0;">ARANŻACJA</div>
            <span style="font-size:11px;color:var(--text-muted);">Kliknij miarę, aby edytować akord</span>
          </div>
          <div style="display:flex;align-items:center;gap:7px;">
            <span class="section-label" style="margin:0;">POWTÓRZ</span>
            <select style="width:84px;padding:4px 8px;font-size:12px;" onchange="updateChangesOption({changesRepeat:this.value})">
              <option value="Wył." ${state.changesRepeat==='Wył.'?'selected':''}>Wył.</option>
              <option value="2x" ${state.changesRepeat==='2x'?'selected':''}>2x</option>
              <option value="4x" ${state.changesRepeat==='4x'?'selected':''}>4x</option>
            </select>
          </div>
        </div>

        ${beatEditorHtml}

        <div class="changes-arr-grid">
          ${state.changesMeasures.map((m, barIdx) => {
            ensureChangesBarBeatsLength(m, beatsPerBar);
            const isPlayingBar = (state.changesIsPlaying && state.changesActiveBarIdx === barIdx);
            return `
              <div class="changes-arr-bar ${isPlayingBar ? 'active-playing' : ''}">
                <div style="display:flex;justify-content:space-between;align-items:center;gap:4px;">
                  <span style="font-size:10.5px;font-weight:700;color:var(--text-muted);white-space:nowrap;">
                    Takt ${barIdx + 1}${m.volta ? ` · <b style="color:var(--accent);">W${m.volta}.</b>` : ''}
                  </span>
                  <div style="display:flex;gap:4px;align-items:center;">
                    <button class="chip-btn" style="padding:1px 7px;font-size:11px;" title="Dodaj kolejny akord w tym takcie"
                            onclick="addChordToChangesMeasure(${barIdx}, -1)">+</button>
                    <button class="chip-btn" style="padding:1px 6px;font-size:10px;color:var(--text-muted);" title="Usuń takt"
                            onclick="removeChangesMeasureBar(${barIdx})">✕</button>
                  </div>
                </div>
                <div style="display:grid;grid-template-columns:repeat(${beatsPerBar},minmax(0,1fr));gap:4px;">
                  ${beatIndices.map(btIdx => {
                    const slot = m.beats[btIdx];
                    const ch = resolveChangesSlot(slot);
                    const isBeatNow = (isPlayingBar && state.changesActiveBeatIdx === btIdx);
                    const isEditing = !!(state.changesEditingSlot && state.changesEditingSlot.barIdx === barIdx && state.changesEditingSlot.beatIdx === btIdx);
                    const btStyle = ch
                      ? 'border:1px solid #3e3329;background:#191410;'
                      : 'border:1px dashed #312922;background:transparent;';
                    return `
                      <div class="changes-beat-slot ${isBeatNow ? 'active-beat' : ''} ${isEditing ? 'editing-slot' : ''}"
                           data-bar="${barIdx}" data-beat="${btIdx}" style="${btStyle}"
                           title="Kliknij, aby edytować akord na mierze ${btIdx + 1}"
                           onclick="openChangesBeatEditor(${barIdx}, ${btIdx})">
                        ${ch ? `
                          <span style="font-size:12px;font-weight:800;color:${isBeatNow?'var(--accent)':'#f5f2eb'};white-space:nowrap;overflow:hidden;text-overflow:ellipsis;max-width:100%;">${ch.symbol}</span>
                          <span style="font-size:9px;color:var(--text-muted);line-height:1;">${ch.roman}</span>
                        ` : `
                          <span style="font-size:10.5px;color:#6f6257;">${btIdx + 1}</span>
                        `}
                      </div>`;
                  }).join('')}
                </div>
              </div>`;
          }).join('')}
        </div>

        <div style="display:flex;gap:7px;flex-wrap:wrap;padding-top:8px;border-top:1px solid #241e19;">
          <button class="chip-btn" style="padding:5px 11px;font-size:11.5px;" onclick="addChangesMeasureBar()">+ Dodaj takt</button>
          <button class="chip-btn ${hasAnyVolta ? 'active' : ''}" style="padding:5px 11px;font-size:11.5px;" onclick="toggleChangesVolta()">
            ${hasAnyVolta ? '✕ Usuń wolty' : '+ Dodaj woltę (1./2.)'}
          </button>
          <button class="chip-btn" style="padding:5px 11px;font-size:11.5px;border-color:#573d26;color:var(--accent);"
                  onclick="state.changesMeasures.push(...state.changesMeasures.slice(0,4).map(m=>JSON.parse(JSON.stringify(m))));renderChangesPreservingScroll()">+ Powiel 4 takty</button>
          <button class="chip-btn" style="padding:5px 11px;font-size:11.5px;" onclick="reharmonizeChangesProgression()">✨ Reharmonizuj</button>
          <button class="chip-btn" style="padding:5px 11px;font-size:11.5px;margin-left:auto;" onclick="resetChangesProgressionToOriginal()">↺ Przywróć oryginał</button>
        </div>
      </div>

      <!-- Dolna para proporcjonalnych kart CTA -->
      <div class="changes-cta-grid">
        <div class="changes-cta-card changes-cta-primary">
          <div style="display:flex;flex-direction:column;gap:5px;">
            <div style="font-size:17px;font-weight:800;color:var(--text);">Ćwicz ten zapis</div>
            <div style="font-size:12px;color:var(--text-muted);line-height:1.45;">
              Graj z zespołem lub w trybie ćwiczebnym. Każdy akord podświetla dźwięki docelowe i ocenia Twoją grę na żywo.
            </div>
          </div>
          <div>
            <button class="action-btn" style="padding:10px 22px;font-size:14px;border-radius:9px;"
                    onclick="stopChangesPlayback();state.changesScreen='practice_config';renderAll()">▶ Konfiguruj i graj</button>
          </div>
        </div>

        <div class="changes-cta-card">
          <div style="display:flex;flex-direction:column;gap:5px;">
            <div style="font-size:15px;font-weight:800;color:var(--text);">Szybkie ćwiczenie i zapis</div>
            <div style="font-size:12px;color:var(--text-muted);line-height:1.4;">
              Wybierz cel harmoniczny i zacznij od razu albo zapisz progresję w Warsztacie.
            </div>
          </div>
          <div style="display:flex;gap:8px;align-items:center;flex-wrap:wrap;">
            <select style="flex:1;min-width:150px;padding:7px 9px;font-size:12.5px;" onchange="updateChangesOption({changesPracticeGoal:this.value})">
              <option value="chord_tones" ${state.changesPracticeGoal==='chord_tones'?'selected':''}>Dźwięki akordu</option>
              <option value="chord_scale" ${state.changesPracticeGoal==='chord_scale'?'selected':''}>Skala akordu</option>
            </select>
            <button class="chip-btn" style="padding:7px 14px;border-color:#573d26;color:var(--accent);font-size:12.5px;font-weight:700;"
                    onclick="stopChangesPlayback();state.changesPracticeMode='study';saveSettings();startChangesLivePractice()">Ćwicz teraz</button>
            <button class="chip-btn" style="padding:7px 13px;font-size:12px;"
                    onclick="saveChangesToWorkshop()">💾 Zapisz do Warsztatu</button>
          </div>
        </div>
      </div>
    </div>`;

  // Prawy panel inspektora: TONACJA GŁÓWNA, METRUM, PODKŁAD I METRONOM (bez zakładki Bas!)
  const bandDimStyle = state.changesBackingBand ? '' : 'opacity:0.48;';
  insp.innerHTML = `
    <div>
      <div class="section-label">TONACJA GŁÓWNA</div>
      <div style="display:grid;grid-template-columns:repeat(4,1fr);gap:5px;margin-bottom:6px;">
        ${labels.map((lbl, pc) => `
          <button class="chip-btn ${state.changesKeyRootPc===pc?'active':''}" style="padding:7px 4px;"
                  onclick="updateChangesOption({changesKeyRootPc:${pc}})">${lbl}</button>
        `).join('')}
      </div>
      <div class="btn-grid-2" style="margin-bottom:10px;">
        <button class="chip-btn ${!state.changesIsMinor?'active':''}"
                onclick="setChangesMode(false)">dur</button>
        <button class="chip-btn ${state.changesIsMinor?'active':''}"
                onclick="setChangesMode(true)">moll</button>
      </div>
      <div class="section-label">METRUM</div>
      <div style="display:flex;gap:5px;flex-wrap:wrap;margin-bottom:6px;">
        ${['4/4','3/4','6/8','12/8','2/2'].map(ts => `
          <button class="chip-btn ${state.changesTimeSig===ts?'active':''}" style="padding:5px 9px;font-size:12px;"
                  onclick="updateChangesOption({changesTimeSig:'${ts}'})">${ts}</button>
        `).join('')}
      </div>
      <div style="font-size:11px;color:var(--text-muted);">Takty: ${state.changesMeasures.length} · ${beatsPerBar} miary/takt${state.changesRepeat !== 'Wył.' ? ` · Powtórz ${state.changesRepeat}` : ''}</div>
    </div>

    <div style="border-top:1px solid var(--border);padding-top:10px;display:flex;flex-direction:column;gap:10px;">
      <div class="section-label" style="margin-bottom:0;">PODKŁAD I METRONOM</div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <div>
          <div style="font-size:12.5px;font-weight:700;">Zespół akompaniujący</div>
          <div style="font-size:10.5px;color:var(--text-muted);">Akordy i perkusja</div>
        </div>
        <button class="ear-toggle-switch ${state.changesBackingBand?'on':''}"
                onclick="updateChangesOption({changesBackingBand:!state.changesBackingBand})"></button>
      </div>

      <div style="display:flex;justify-content:space-between;align-items:center;">
        <div>
          <div style="font-size:12.5px;font-weight:700;">Klik metronomu</div>
          <div style="font-size:10.5px;color:var(--text-muted);">Niezależny puls rytmiczny</div>
        </div>
        <button class="ear-toggle-switch ${state.changesMetronomeClick?'on':''}"
                onclick="updateChangesOption({changesMetronomeClick:!state.changesMetronomeClick})"></button>
      </div>

      <div style="display:flex;flex-direction:column;gap:10px;${bandDimStyle}">
        <div>
          <div class="section-label">PERKUSJA</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${!state.changesDrums?'active':''}"
                    onclick="updateChangesOption({changesDrums:false})">Wył.</button>
            <button class="chip-btn ${state.changesDrums?'active':''}"
                    onclick="updateChangesOption({changesDrums:true})">Wł.</button>
          </div>
        </div>

        ${state.changesDrums ? `
        <div>
          <div class="section-label">RYTM PERKUSJI</div>
          <select onchange="updateChangesOption({changesDrumStyle:this.value})">
            ${[['auto','Auto (z Feel)'],['rock','Rock'],['pop','Pop'],['blues','Blues / Shuffle'],['swing','Swing Jazz'],['funk','Funk'],['half_time','Half-time']].map(([v,l]) => `<option value="${v}" ${state.changesDrumStyle===v?'selected':''}>${l}</option>`).join('')}
          </select>
        </div>` : ''}

        <div>
          <div class="section-label">INSTRUMENT AKOMPANIAMENTU</div>
          <select onchange="updateChangesOption({changesCompInst:this.value})">
            <option value="hammond" ${state.changesCompInst==='hammond'?'selected':''}>Organy</option>
            <option value="clean" ${state.changesCompInst==='clean'?'selected':''}>Gitara czysta</option>
            <option value="piano" ${state.changesCompInst==='piano'?'selected':''}>Fortepian</option>
            <option value="rhodes" ${state.changesCompInst==='rhodes'?'selected':''}>Pianino elektr. (Rhodes)</option>
            <option value="crunch" ${state.changesCompInst==='crunch'?'selected':''}>Gitara przesterowana</option>
            <option value="off" ${state.changesCompInst==='off'?'selected':''}>Wył. (bez akordów)</option>
          </select>
        </div>

        <div>
          <div class="section-label">FEEL</div>
          <select onchange="updateChangesOption({changesFeel:this.value})">
            ${['Blues','Prosto','Swing Jazz','Funk','Rock'].map(f => `<option value="${f}" ${state.changesFeel===f?'selected':''}>${f}</option>`).join('')}
          </select>
        </div>

        <div>
          <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:4px;">
            <div class="section-label" style="margin:0;">SIŁA ATAKU</div>
            <span id="changesAttackValLabel" style="font-size:11.5px;font-weight:800;color:var(--accent);">${Math.round(getChangesAttackMul() * 100)}%</span>
          </div>
          <input type="range" min="20" max="100" step="5" value="${Math.round(getChangesAttackMul() * 100)}"
                 style="width:100%;accent-color:var(--accent);cursor:pointer;"
                 oninput="state.changesAttackStrength=parseInt(this.value,10)/100;const el=document.getElementById('changesAttackValLabel');if(el)el.textContent=this.value+'%';"
                 onchange="updateChangesOption({changesAttackStrength:parseInt(this.value,10)/100})">
          <div style="display:flex;justify-content:space-between;font-size:10px;color:var(--text-muted);margin-top:2px;">
            <span>Delikatny</span><span>Średni</span><span>Mocny</span>
          </div>
        </div>

        ${state.changesFeel === 'Blues' ? `
        <div>
          <div class="section-label">GROOVE</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${state.changesGroove==='shuffle'?'active':''}"
                    onclick="updateChangesOption({changesGroove:'shuffle'})">Shuffle</button>
            <button class="chip-btn ${state.changesGroove==='slow12_8'?'active':''}"
                    onclick="updateChangesOption({changesGroove:'slow12_8'})">Wolny 12/8</button>
          </div>
        </div>` : ''}

        <div>
          <div class="section-label">AKOMPANIAMENT RYTMICZNY</div>
          <div class="btn-grid-2">
            <button class="chip-btn ${!state.changesRhythmGuitar?'active':''}"
                    onclick="updateChangesOption({changesRhythmGuitar:false})">Wył.</button>
            <button class="chip-btn ${state.changesRhythmGuitar?'active':''}"
                    onclick="updateChangesOption({changesRhythmGuitar:true})">Wł.</button>
          </div>
        </div>
      </div>

      <div style="border-top:1px solid var(--border);padding-top:10px;display:flex;flex-direction:column;gap:10px;">
        <div style="display:flex;justify-content:space-between;align-items:center;">
          <span style="font-size:12.5px;font-weight:700;">Pętla</span>
          <button class="ear-toggle-switch ${state.changesLoop?'on':''}"
                  onclick="updateChangesOption({changesLoop:!state.changesLoop})"></button>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;">
          <span style="font-size:12.5px;font-weight:700;">Dźwięki akordów na kartach</span>
          <button class="ear-toggle-switch ${state.changesChordDiagrams?'on':''}"
                  onclick="updateChangesOption({changesChordDiagrams:!state.changesChordDiagrams})"></button>
        </div>

        <div style="display:flex;justify-content:space-between;align-items:center;">
          <span style="font-size:12.5px;font-weight:700;">Przechodź przez tonacje</span>
          <button class="ear-toggle-switch ${state.changesCycleKeys?'on':''}"
                  onclick="updateChangesOption({changesCycleKeys:!state.changesCycleKeys})"></button>
        </div>

        <button class="chip-btn" style="width:100%;padding:10px;margin-top:4px;font-weight:700;"
                onclick="saveChangesToWorkshop()">Zapisz</button>
      </div>
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

async function downloadTabExport(fmt) {
  try {
    const res = await fetch(`/api/export/${fmt}?bpm=${state.tempoBpm}`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(state.recordedEvents)
    });
    if (!res.ok) return;
    const blob = await res.blob();
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = (fmt === 'mid') ? 'cwiczenie_tuxguitar.mid' : 'cwiczenie_tuxguitar.gp5';
    document.body.appendChild(a);
    a.click();
    a.remove();
    setTimeout(() => URL.revokeObjectURL(url), 1500);
  } catch (e) {}
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

function normalizeExerciseTechShort(tech) {
  if (!tech || tech === '-' || tech === '—') return '';
  if (tech === 'Hammer-on') return 'H';
  if (tech === 'Pull-off') return 'P';
  if (tech === 'Slide') return 'S';
  if (tech === 'D') return '↓';
  if (tech === 'U') return '↑';
  return String(tech);
}

function getActiveExerciseShapeSlice(evs, highlightIdx = 0) {
  if (!Array.isArray(evs) || evs.length === 0) {
    return { startIdx: 0, endIdx: -1, events: [] };
  }
  const hi = Math.max(0, Math.min(evs.length - 1, highlightIdx >= 0 ? highlightIdx : 0));

  let hasDistinctSegs = false;
  let firstSeg = null;
  for (let i = 0; i < evs.length; i++) {
    if (evs[i].seg !== undefined) {
      if (firstSeg === null) firstSeg = evs[i].seg;
      else if (evs[i].seg !== firstSeg) { hasDistinctSegs = true; break; }
    }
  }

  if (hasDistinctSegs) {
    const curSeg = evs[hi].seg;
    let startIdx = hi;
    let endIdx = hi;
    while (startIdx > 0 && evs[startIdx - 1].seg === curSeg) startIdx--;
    while (endIdx + 1 < evs.length && evs[endIdx + 1].seg === curSeg) endIdx++;
    return {
      startIdx,
      endIdx,
      events: evs.slice(startIdx, endIdx + 1).map((ev, offset) => ({ ev, idx: startIdx + offset }))
    };
  }

  if (evs.length <= 16) {
    return {
      startIdx: 0,
      endIdx: evs.length - 1,
      events: evs.map((ev, idx) => ({ ev, idx }))
    };
  }

  // Dla dłuższych przebiegów bez jawnych segmentów dzielimy na naturalne kształty/frazy (cykl po strunach lub max 12 nut)
  const ranges = [];
  let blockStart = 0;
  for (let i = 4; i < evs.length; i++) {
    const curS = evs[i].s;
    const prevS = evs[i - 1].s;
    const prev2S = evs[i - 2].s;
    const len = i - blockStart;
    const isBottomTurnaround = (curS >= 0 && prevS >= 0 && prev2S >= 0 && prev2S > prevS && curS > prevS && len >= 6);
    if (len >= 12 || (isBottomTurnaround && len >= 5)) {
      ranges.push([blockStart, i - 1]);
      blockStart = i;
    }
  }
  ranges.push([blockStart, evs.length - 1]);

  let hitRange = ranges[0];
  for (const r of ranges) {
    if (hi >= r[0] && hi <= r[1]) {
      hitRange = r;
      break;
    }
  }
  return {
    startIdx: hitRange[0],
    endIdx: hitRange[1],
    events: evs.slice(hitRange[0], hitRange[1] + 1).map((ev, offset) => ({ ev, idx: hitRange[0] + offset }))
  };
}

function getNextExerciseShapeSlice(evs, curSlice) {
  if (!Array.isArray(evs) || !curSlice || (curSlice.startIdx === 0 && curSlice.endIdx >= evs.length - 1)) {
    return null;
  }
  const nextStart = (curSlice.endIdx + 1 < evs.length) ? (curSlice.endIdx + 1) : 0;
  const nextSlice = getActiveExerciseShapeSlice(evs, nextStart);
  if (!nextSlice || (nextSlice.startIdx === curSlice.startIdx && nextSlice.endIdx === curSlice.endIdx)) {
    return null;
  }
  return nextSlice;
}

function buildExerciseShapeNextOverlaySvg(nextEntries, occupiedPosKeys, topY, strGap, labels, xForFret, isFretVisible, light) {
  if (!Array.isArray(nextEntries) || nextEntries.length === 0) return '';
  let out = '';
  const nextPts = [];
  nextEntries.forEach(({ ev }) => {
    if (isFretVisible(ev.f)) {
      const cx = xForFret(ev.f);
      const cy = topY + (5 - ev.s) * strGap;
      if (!nextPts.length || nextPts[nextPts.length - 1].cx !== cx || nextPts[nextPts.length - 1].cy !== cy) {
        nextPts.push({ cx, cy });
      }
    }
  });
  if (nextPts.length >= 2) {
    out += `<polyline points="${nextPts.map(p => `${p.cx.toFixed(1)},${p.cy.toFixed(1)}`).join(' ')}" fill="none" stroke="rgba(56,189,248,0.38)" stroke-width="1.6" stroke-dasharray="4,3" pointer-events="none"/>`;
  }
  const seenNext = new Set();
  const nextCircleFill = light ? 'rgba(255,255,255,0.92)' : 'rgba(20,16,13,0.88)';
  const nextTextFill = light ? '#0284c7' : '#7dd3fc';
  nextEntries.forEach(({ ev }) => {
    const key = `${ev.s}_${ev.f}`;
    if (seenNext.has(key) || (occupiedPosKeys && occupiedPosKeys.has(key))) return;
    seenNext.add(key);
    if (!isFretVisible(ev.f)) return;
    const cy = topY + (5 - ev.s) * strGap;
    const cx = xForFret(ev.f);
    const openMidi = state.customStringMidis[ev.s] !== undefined ? state.customStringMidis[ev.s] : [40, 45, 50, 55, 59, 64][ev.s];
    const m = ev.midi || (openMidi + ev.f);
    const pc = ((m % 12) + 12) % 12;
    const nm = (state.labelType === 'intervals') ? computeIntervalLabel(pc, state.rootPc, false) : (ev.note || labels[pc]);
    out += `<g opacity="0.78" style="cursor:pointer" onclick="onExerciseFretCellClick(${ev.s},${ev.f},false)">
      <circle cx="${cx}" cy="${cy}" r="9.6" fill="${nextCircleFill}" stroke="#38bdf8" stroke-width="1.6" stroke-dasharray="3,2"/>
      <text x="${cx}" y="${cy + 3.4}" fill="${nextTextFill}" font-size="9.5" font-weight="700" text-anchor="middle">${nm}</text>
    </g>`;
  });
  return out;
}

function buildExerciseStaffAndTabSvg(highlightIdx = -1, availWidth = 0) {
  const evs = state.recordedEvents || [];
  const light = isLightTheme();
  const inkMain = light ? '#1c1917' : '#e7e1d8';
  const inkNote = light ? '#1c1917' : '#f5f2eb';
  const cardBg = light ? '#ffffff' : '#15110e';
  const staffLineCol = light ? '#9a9083' : '#6b6257';
  const tabLineCol = light ? '#b5aa9c' : '#524940';
  const isPlayer = highlightIdx >= 0;
  const isPlayMode = isPlayer && state.exercisePlayerMode === 'play';
  const completedSet = (state.exerciseCompletedSet instanceof Set) ? state.exerciseCompletedSet : null;
  const showStaff = !!state.exerciseShowStaff;
  const showTab = state.tabCreationEnabled || !showStaff;
  const barsPerRow = [0, 2, 4, 6, 8].includes(state.exerciseBarsPerRow) ? state.exerciseBarsPerRow : 0;

  const x0 = showStaff ? 86 : 48;
  const rightMargin = 14;
  const staffGap = 10;
  const tabGap = showStaff ? 12 : 14;
  const staffTopRel = 34;
  const tabTopRel = showStaff ? 114 : 42;
  const rowH = showStaff ? (showTab ? 210 : 118) : 130;
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

  // 1. Pogrupuj zdarzenia w pełne takty (measures)
  const measures = [];
  {
    let curM = { barNum: 1, events: [], totalBeats: 0 };
    evs.forEach((ev, idx) => {
      const dur = ev.dur !== undefined ? ev.dur : 0;
      const beats = getDurationBeats(dur, ev.tuplet === 3);
      if (curM.events.length > 0 && curM.totalBeats >= measureBeats - 0.001) {
        measures.push(curM);
        curM = { barNum: curM.barNum + 1, events: [], totalBeats: 0 };
      }
      curM.events.push({ ev, idx, beats });
      curM.totalBeats += beats;
    });
    if (curM.events.length > 0) {
      measures.push(curM);
    }
  }

  // 2. Zwarty układ wierszy dopasowany do gęstości szybkich nut (szesnastki / sekstole)
  const targetRowW = Math.max(620, Math.round(availWidth || 1000));
  const usableW = Math.max(420, targetRowW - x0 - rightMargin);
  const measurePrefW = (m) => {
    const perNotePrefW = (m.events.length >= 8) ? 21 : (m.events.length >= 5 ? 24 : 30);
    const slotEst = Math.max(Math.round(measureBeats), m.events.length);
    return Math.max(106, slotEst * perNotePrefW + 18);
  };
  const maxBarWFor = (m) => {
    const perNoteCap = (m.events.length >= 8) ? 26 : (m.events.length >= 5 ? 32 : 48);
    return Math.max(126, m.events.length * perNoteCap + 22);
  };
  const rows = [];

  if (measures.length === 0) {
    rows.push({ measures: [] });
  } else if (barsPerRow > 0) {
    for (let i = 0; i < measures.length; i += barsPerRow) {
      rows.push({ measures: measures.slice(i, i + barsPerRow) });
    }
  } else {
    let curRow = [];
    let curPrefW = 0;
    for (const m of measures) {
      const prefW = measurePrefW(m);
      if (curRow.length > 0 && curPrefW + prefW > usableW) {
        rows.push({ measures: curRow });
        curRow = [m];
        curPrefW = prefW;
      } else {
        curRow.push(m);
        curPrefW += prefW;
      }
    }
    if (curRow.length > 0) rows.push({ measures: curRow });
  }

  const totalW = targetRowW;
  const totalH = rows.length * rowH + Math.max(0, rows.length - 1) * rowGapY;
  let svg = `<svg width="${totalW}" height="${totalH}" viewBox="0 0 ${totalW} ${totalH}" style="width:100%;height:auto;display:block;" xmlns="http://www.w3.org/2000/svg">`;

  rows.forEach((rowInfo, rowIdx) => {
    const yOff = rowIdx * (rowH + rowGapY);
    const W = targetRowW;
    const staffTop = yOff + staffTopRel;
    const tabTop = yOff + tabTopRel;
    const topVisual = showStaff ? staffTop : tabTop;
    const endY = showTab ? (tabTop + 5 * tabGap) : (staffTop + 4 * staffGap);
    const rowMeasures = rowInfo.measures;
    const isLastRow = (rowIdx === rows.length - 1);

    if (rowIdx === 0) {
      svg += `<text x="${showStaff ? 84 : 48}" y="${yOff + 15}" fill="${inkMain}" font-size="11.5" font-weight="700">♩ = ${state.tempoBpm}</text>`;
    }

    if (showStaff) {
      for (let i = 0; i < 5; i++) {
        const y = staffTop + i * staffGap;
        svg += `<line x1="12" y1="${y}" x2="${W - rightMargin}" y2="${y}" stroke="${staffLineCol}" stroke-width="1.2"/>`;
      }
      svg += `<text x="18" y="${staffTop + (useBass ? 30 : 35)}" fill="${isPlayer ? inkMain : '#f39237'}" font-size="33">${clefSymbol}</text>`;
      if (rowIdx === 0) {
        svg += `<text x="50" y="${staffTop + 18}" fill="${inkMain}" font-size="14" font-weight="800">${state.timeSigNum}</text>`;
        svg += `<text x="50" y="${staffTop + 37}" fill="${inkMain}" font-size="14" font-weight="800">${state.timeSigDen}</text>`;
      }
      svg += `<line x1="${W - rightMargin}" y1="${staffTop}" x2="${W - rightMargin}" y2="${staffTop + 40}" stroke="${staffLineCol}" stroke-width="${isLastRow ? '1.8' : '1.2'}"/>`;
    }

    if (showTab) {
      for (let s = 0; s < 6; s++) {
        const y = tabTop + s * tabGap;
        svg += `<line x1="12" y1="${y}" x2="${W - rightMargin}" y2="${y}" stroke="${tabLineCol}" stroke-width="1.15"/>`;
      }
      const tabMidH = 5 * tabGap;
      svg += `<text x="18" y="${tabTop + tabMidH * 0.30}" fill="${light ? '#57534e' : '#9ca3af'}" font-size="${showStaff ? 13 : 14}" font-weight="800">T</text>`;
      svg += `<text x="18" y="${tabTop + tabMidH * 0.56}" fill="${light ? '#57534e' : '#9ca3af'}" font-size="${showStaff ? 13 : 14}" font-weight="800">A</text>`;
      svg += `<text x="18" y="${tabTop + tabMidH * 0.82}" fill="${light ? '#57534e' : '#9ca3af'}" font-size="${showStaff ? 13 : 14}" font-weight="800">B</text>`;
      svg += `<line x1="${W - rightMargin}" y1="${tabTop}" x2="${W - rightMargin}" y2="${tabTop + 5 * tabGap}" stroke="${staffLineCol}" stroke-width="${isLastRow ? '1.8' : '1.2'}"/>`;
    }

    if (rowMeasures.length === 0) return;

    const weights = rowMeasures.map(m => Math.max(2.5, m.events.length + 0.8));
    const sumWeights = weights.reduce((a, b) => a + b, 0);
    let barWidths;
    if (barsPerRow > 0) {
      const fillRatio = (isLastRow && rowMeasures.length < barsPerRow)
        ? (rowMeasures.length / barsPerRow)
        : 1.0;
      const allocW = Math.min(usableW, Math.max(usableW * fillRatio, rowMeasures.reduce((a, m) => a + Math.max(106, m.events.length * 22 + 16), 0)));
      barWidths = weights.map((w, idx) => Math.min((w / sumWeights) * allocW, maxBarWFor(rowMeasures[idx])));
    } else {
      const prefWidths = rowMeasures.map(measurePrefW);
      const totalPref = prefWidths.reduce((a, b) => a + b, 0);
      if (!isLastRow || totalPref >= usableW * 0.82) {
        barWidths = weights.map((w, idx) => Math.min((w / sumWeights) * usableW, maxBarWFor(rowMeasures[idx])));
      } else {
        barWidths = prefWidths;
      }
    }

    let cursorX = x0;
    rowMeasures.forEach((m, mIdx) => {
      const barStartX = cursorX;
      const barW = barWidths[mIdx];
      const barEndX = barStartX + barW;
      const isLastMeasureOfAll = (isLastRow && mIdx === rowMeasures.length - 1);
      const isIncompleteTail = isLastMeasureOfAll && (m.totalBeats < measureBeats - 0.001);

      const barNumX = (mIdx === 0) ? 14 : (barStartX + 4);
      svg += `<text x="${barNumX}" y="${yOff + 16}" fill="#9ca3af" font-size="10" font-weight="700">${m.barNum}</text>`;

      if (mIdx > 0) {
        if (showStaff) {
          svg += `<line x1="${barStartX}" y1="${staffTop}" x2="${barStartX}" y2="${staffTop + 40}" stroke="#6b6257" stroke-width="1.25"/>`;
        }
        if (showTab) {
          svg += `<line x1="${barStartX}" y1="${tabTop}" x2="${barStartX}" y2="${tabTop + 5 * tabGap}" stroke="#5c5248" stroke-width="1.25"/>`;
        }
      }

      const padL = Math.min(12, Math.max(6, barW * 0.04));
      const padR = Math.min(10, Math.max(5, barW * 0.03));
      const innerW = Math.max(20, barW - padL - padR);
      const slotCount = isIncompleteTail
        ? Math.max(m.events.length, Math.max(1, Math.round(measureBeats)))
        : Math.max(1, m.events.length);
      const slotW = innerW / slotCount;
      const noteScale = Math.min(1.0, Math.max(0.70, slotW / 23));
      const fretFontSize = Math.round(((showStaff ? 12 : 12.5) * noteScale) * 10) / 10;
      const boxH = Math.round(14.5 * noteScale * 10) / 10;
      const noteNameFontSize = Math.round(9.5 * noteScale * 10) / 10;

      m.events.forEach(({ ev, idx }, k) => {
        const x = barStartX + padL + (k + 0.5) * slotW;
        const isActive = (idx === highlightIdx);
        const isCompleted = isPlayMode && completedSet && completedSet.has(idx);
        const isGreenHighlight = isCompleted || (!isPlayMode && isActive);
        const isOrangeTarget = isPlayMode && isActive && !isCompleted;
        const colColor = isGreenHighlight ? '#22c55e' : (isOrangeTarget ? '#f39237' : '#e7e1d8');
        const dur = ev.dur !== undefined ? ev.dur : 0;
        const shortTech = normalizeExerciseTechShort(ev.tech);

        svg += `<g style="cursor:pointer" ${isActive ? 'data-ex-active="1"' : ''} onclick="onExerciseScoreNoteClick(${idx})">`;
        if (isActive) {
          const hlW = Math.max(16, Math.min(28, slotW * 0.92));
          const hlFill = isPlayMode ? 'rgba(243,146,55,0.16)' : 'rgba(34,197,94,0.14)';
          const hlStroke = isPlayMode ? 'rgba(243,146,55,0.55)' : 'rgba(74,222,128,0.45)';
          svg += `<rect x="${x - hlW / 2}" y="${topVisual - 14}" width="${hlW}" height="${(endY - topVisual) + 22}" rx="5" fill="${hlFill}" stroke="${hlStroke}" stroke-width="1"/>`;
        }

        if (ev.rest) {
          const restY = showStaff ? (staffTop + 26) : (tabTop + 2.7 * tabGap);
          svg += `<text x="${x}" y="${restY}" fill="${isActive ? '#f39237' : '#9ca3af'}" font-size="${Math.round(18 * noteScale)}" font-weight="700" text-anchor="middle">𝄽</text>`;
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
              svg += `<line x1="${x - 10 * noteScale}" y1="${ly}" x2="${x + 10 * noteScale}" y2="${ly}" stroke="#8c7f72" stroke-width="1.1"/>`;
            }
            for (let ly = staffTop - 10; ly >= noteY - 1 && ly >= yOff + 12; ly -= 10) {
              svg += `<line x1="${x - 10 * noteScale}" y1="${ly}" x2="${x + 10 * noteScale}" y2="${ly}" stroke="#8c7f72" stroke-width="1.1"/>`;
            }
            if (nm.includes('#')) {
              svg += `<text x="${x - 11 * noteScale}" y="${noteY + 4}" fill="${colColor}" font-size="${Math.round(12 * noteScale)}" font-weight="800" text-anchor="middle">#</text>`;
            } else if (nm.includes('b')) {
              svg += `<text x="${x - 11 * noteScale}" y="${noteY + 4}" fill="${colColor}" font-size="${Math.round(12 * noteScale)}" font-weight="800" text-anchor="middle">♭</text>`;
            }
            if (dur <= -1) {
              svg += `<ellipse cx="${x}" cy="${noteY}" rx="${6.2 * noteScale}" ry="${4.3 * noteScale}" fill="none" stroke="${colColor}" stroke-width="1.9"/>`;
            } else {
              svg += `<ellipse cx="${x}" cy="${noteY}" rx="${6.0 * noteScale}" ry="${4.4 * noteScale}" fill="${colColor}"/>`;
            }
            if (dur > -2) {
              const stemDown = noteY <= staffTop + 20;
              const sx = stemDown ? (x - 5.2 * noteScale) : (x + 5.2 * noteScale);
              const sy2 = stemDown ? (noteY + 23) : (noteY - 23);
              svg += `<line x1="${sx}" y1="${noteY}" x2="${sx}" y2="${sy2}" stroke="${colColor}" stroke-width="1.5"/>`;
              if (dur >= 1) {
                const dir = stemDown ? -1 : 1;
                svg += `<path d="M${sx},${sy2} Q${sx + 8 * noteScale},${sy2 + 6 * dir} ${sx + 3.5 * noteScale},${sy2 + 11 * dir}" fill="none" stroke="${colColor}" stroke-width="1.5"/>`;
              }
              if (dur >= 2) {
                const dir = stemDown ? -1 : 1;
                const sy3 = sy2 + 5 * dir;
                svg += `<path d="M${sx},${sy3} Q${sx + 8 * noteScale},${sy3 + 6 * dir} ${sx + 3.5 * noteScale},${sy3 + 11 * dir}" fill="none" stroke="${colColor}" stroke-width="1.5"/>`;
              }
            }
            if (ev.tuplet === 3) {
              svg += `<text x="${x}" y="${noteY - 25}" fill="#4ade80" font-size="${Math.round(10 * noteScale)}" font-weight="800" text-anchor="middle">3</text>`;
            }
          }

          const isRootPc = (pc === state.rootPc);
          const topNoteLbl = (state.labelType === 'intervals')
            ? computeIntervalLabel(pc, state.rootPc, false)
            : nm;
          const labelY = shortTech ? (topVisual - 18) : (topVisual - 7);
          if (state.showExerciseNoteNames) {
            svg += `<text x="${x}" y="${labelY}" fill="${isGreenHighlight ? '#4ade80' : (isRootPc ? '#d97316' : inkNote)}" font-size="${noteNameFontSize}" font-weight="${isRootPc ? '800' : '700'}" text-anchor="middle">${topNoteLbl}</text>`;
          } else if (isRootPc && showTab && !showStaff) {
            svg += `<circle cx="${x}" cy="${labelY - 3}" r="2.5" fill="#f39237"><title>Tonika (1)</title></circle>`;
          }

          if (showTab && ev.s !== undefined && ev.f !== undefined && ev.s >= 0 && ev.f >= 0) {
            if (shortTech) {
              const tCol = (shortTech === 'H' || shortTech === 'P') ? '#4ade80' : (shortTech === 'S' ? '#facc15' : (shortTech === '↑' ? '#38bdf8' : '#fb923c'));
              svg += `<text x="${x}" y="${tabTop - 6}" fill="${tCol}" font-size="${Math.round(9.5 * noteScale * 10) / 10}" font-weight="900" text-anchor="middle">${shortTech}</text>`;
            }
            const row = 5 - ev.s;
            const ty = tabTop + row * tabGap;
            // Łuk legato H/P między sąsiednimi nutami na tej samej strunie
            if ((shortTech === 'H' || shortTech === 'P') && k > 0 && m.events[k - 1] && m.events[k - 1].ev.s === ev.s) {
              const prevX = barStartX + padL + (k - 0.5) * slotW;
              const arcY = Math.max(tabTop - 4, ty - boxH * 0.65);
              svg += `<path d="M${prevX.toFixed(1)},${(ty - 4).toFixed(1)} Q${((prevX + x) * 0.5).toFixed(1)},${(arcY - 5).toFixed(1)} ${x.toFixed(1)},${(ty - 4).toFixed(1)}" fill="none" stroke="rgba(74,222,128,0.65)" stroke-width="1.2"/>`;
            }
            const boxW = Math.round(((String(ev.f).length > 1 ? 20 : 15) * noteScale) * 10) / 10;
            const tabBg = isGreenHighlight ? '#22c55e' : (isOrangeTarget ? '#f39237' : cardBg);
            const tabTxt = (isGreenHighlight || isOrangeTarget) ? '#05140b' : inkNote;
            const tabStroke = isGreenHighlight ? '#86efac' : (isOrangeTarget ? '#fef08a' : 'none');
            svg += `<rect x="${x - boxW / 2}" y="${ty - boxH / 2}" width="${boxW}" height="${boxH}" rx="3" fill="${tabBg}" stroke="${tabStroke}" stroke-width="${(isGreenHighlight || isOrangeTarget) ? '1.1' : '0'}"/>`;
            svg += `<text x="${x}" y="${ty + fretFontSize * 0.34}" fill="${tabTxt}" font-size="${fretFontSize}" font-weight="800" text-anchor="middle">${ev.f}</text>`;
          }
        }
        svg += `</g>`;
      });

      if (isLastMeasureOfAll && barEndX < W - rightMargin - 6) {
        if (showStaff) {
          svg += `<line x1="${barEndX}" y1="${staffTop}" x2="${barEndX}" y2="${staffTop + 40}" stroke="#6b6257" stroke-width="1.4"/>`;
        }
        if (showTab) {
          svg += `<line x1="${barEndX}" y1="${tabTop}" x2="${barEndX}" y2="${tabTop + 5 * tabGap}" stroke="#5c5248" stroke-width="1.4"/>`;
        }
      }

      cursorX = barEndX;
    });
  });

  svg += `</svg>`;
  return svg;
}

function computeStableExerciseFretWindow(evs, maxBoardFret = 24) {
  const usedFrets = [];
  if (Array.isArray(evs)) {
    for (let i = 0; i < evs.length; i++) {
      const e = evs[i];
      if (e && !e.rest && e.f >= 0) usedFrets.push(e.f);
    }
  }
  if (usedFrets.length === 0) {
    return { viewStartFret: 1, viewEndFret: maxBoardFret, showOpenCol: true };
  }
  const hasOpen = usedFrets.some(f => f === 0);
  const rawMin = Math.min(...usedFrets);
  const rawMax = Math.max(...usedFrets);
  let lo = hasOpen ? 0 : Math.max(1, rawMin - 2);
  let hi = Math.min(maxBoardFret, rawMax + 2);
  const minSpan = Math.min(maxBoardFret, 12);
  if ((hi - lo + 1) < minSpan) {
    const mid = (lo + hi) * 0.5;
    lo = Math.round(mid - (minSpan - 1) * 0.5);
    if (lo < (hasOpen ? 0 : 1)) lo = hasOpen ? 0 : 1;
    hi = lo + minSpan - 1;
    if (hi > maxBoardFret) {
      hi = maxBoardFret;
      lo = Math.max(hasOpen ? 0 : 1, hi - minSpan + 1);
    }
  }
  if (lo <= 2 || hasOpen) {
    return { viewStartFret: 1, viewEndFret: Math.min(maxBoardFret, Math.max(minSpan, hi)), showOpenCol: true };
  }
  return { viewStartFret: lo, viewEndFret: hi, showOpenCol: false };
}

function buildExerciseFretboardSvg(highlightIdx = -1, isComposer = true) {
  const evs = state.recordedEvents || [];
  const light = isLightTheme();
  const W = 1040, H = 205;
  const openX = 24, nutX = 56, padR = 16;
  const maxBoardFret = state.maxDisplayedFret || 24;
  const topY = 24, strGap = 28;
  const labels = getRootLabels();
  const isPlayMode = !isComposer && state.exercisePlayerMode === 'play';
  const completedSet = (state.exerciseCompletedSet instanceof Set) ? state.exerciseCompletedSet : null;

  const shapeSlice = (!isComposer && evs.length > 0) ? getActiveExerciseShapeSlice(evs, highlightIdx) : null;
  const displayEntries = shapeSlice
    ? shapeSlice.events.filter(({ ev }) => !ev.rest && ev.s >= 0 && ev.f >= 0)
    : evs.map((ev, idx) => ({ ev, idx })).filter(({ ev }) => !ev.rest && ev.s >= 0 && ev.f >= 0);
  const nextSlice = shapeSlice ? getNextExerciseShapeSlice(evs, shapeSlice) : null;
  const nextEntries = nextSlice
    ? nextSlice.events.filter(({ ev }) => !ev.rest && ev.s >= 0 && ev.f >= 0)
    : [];

  // Stabilne wyśrodkowanie całego ćwiczenia raz dla wszystkich jego nut (nigdy nie przeskakuje między kształtami)
  const win = (!isComposer && evs.length > 0)
    ? computeStableExerciseFretWindow(evs, maxBoardFret)
    : { viewStartFret: 1, viewEndFret: maxBoardFret, showOpenCol: true };
  const viewStartFret = win.viewStartFret;
  const viewEndFret = win.viewEndFret;
  const showOpenCol = win.showOpenCol;

  const visibleFrets = Math.max(1, viewEndFret - viewStartFret + 1);
  const fretW = (W - nutX - padR) / visibleFrets;
  const minF = isComposer ? (state.exerciseMinFret || 0) : 0;
  const maxF = isComposer ? (state.exerciseMaxFret || maxBoardFret) : maxBoardFret;
  const fretInlays = new Set([3, 5, 7, 9, 12, 15, 17, 19, 21, 24]);
  const dotsMode = (state.fretMarkersMode === 'dots');
  const numAll = !!state.numberAllFrets;

  const bgFill = light ? '#f6f1e9' : '#18120e';
  const bgStroke = light ? '#d6cec2' : '#30271f';
  const wireCol = light ? '#cfc6b8' : '#443930';
  const strCol = light ? '#8c7e70' : '#786b5d';
  const nutCol = light ? '#6b5e50' : '#b8aea0';
  const numCol = light ? '#57534e' : '#78716c';
  const ghostCol = light ? 'rgba(60,50,40,0.16)' : 'rgba(214,206,194,0.16)';

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" style="width:100%;height:auto;display:block;user-select:none;" xmlns="http://www.w3.org/2000/svg">`;
  svg += `<rect x="${nutX}" y="${topY - 12}" width="${W - nutX - padR}" height="${5 * strGap + 24}" rx="4" fill="${bgFill}" stroke="${bgStroke}" stroke-width="1"/>`;

  for (let f = viewStartFret; f <= viewEndFret; f++) {
    const colIdx = f - viewStartFret;
    const cx = nutX + (colIdx + 0.5) * fretW;
    if (fretInlays.has(f)) {
      const dotFill = dotsMode ? (light ? 'rgba(110,95,80,0.24)' : 'rgba(255,255,255,0.15)') : (light ? 'rgba(110,95,80,0.10)' : 'rgba(255,255,255,0.06)');
      if (f === 12 || f === 24) {
        svg += `<circle cx="${cx}" cy="${topY + strGap * 1.5}" r="4.8" fill="${dotFill}"/>`;
        svg += `<circle cx="${cx}" cy="${topY + strGap * 3.5}" r="4.8" fill="${dotFill}"/>`;
      } else {
        svg += `<circle cx="${cx}" cy="${topY + strGap * 2.5}" r="4.8" fill="${dotFill}"/>`;
      }
    }
    if (f < minF || f > maxF) {
      svg += `<rect x="${nutX + colIdx * fretW}" y="${topY - 12}" width="${fretW}" height="${5 * strGap + 24}" fill="rgba(0,0,0,0.42)"/>`;
    }
  }

  svg += `<line x1="${nutX}" y1="${topY - 12}" x2="${nutX}" y2="${topY + 5 * strGap + 12}" stroke="${showOpenCol ? nutCol : wireCol}" stroke-width="${showOpenCol ? 3 : 1.4}"/>`;
  for (let f = viewStartFret; f <= viewEndFret; f++) {
    const colIdx = f - viewStartFret;
    const fx = nutX + (colIdx + 1) * fretW;
    svg += `<line x1="${fx}" y1="${topY - 12}" x2="${fx}" y2="${topY + 5 * strGap + 12}" stroke="${wireCol}" stroke-width="1.2"/>`;
    const isMarker = fretInlays.has(f);
    if (numAll || (!dotsMode && isMarker)) {
      svg += `<text x="${fx - fretW * 0.5}" y="${topY + 5 * strGap + 28}" fill="${(f === 12 && numAll) ? '#f39237' : numCol}" font-size="10.5" font-weight="700" text-anchor="middle">${f}</text>`;
    } else if (dotsMode && isMarker) {
      if (f === 12 || f === 24) {
        svg += `<circle cx="${fx - fretW * 0.5 - 4}" cy="${topY + 5 * strGap + 24}" r="2.8" fill="${numCol}"/>`;
        svg += `<circle cx="${fx - fretW * 0.5 + 4}" cy="${topY + 5 * strGap + 24}" r="2.8" fill="${numCol}"/>`;
      } else {
        svg += `<circle cx="${fx - fretW * 0.5}" cy="${topY + 5 * strGap + 24}" r="3" fill="${numCol}"/>`;
      }
    }
  }

  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const sy = topY + row * strGap;
    const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    const openLbl = labels[((openMidi % 12) + 12) % 12];
    const thickness = 1.0 + row * 0.35;
    svg += `<text x="${openX - 10}" y="${sy + 4}" fill="${numCol}" font-size="11.5" font-weight="800" text-anchor="middle">${openLbl}</text>`;
    svg += `<line x1="${nutX}" y1="${sy}" x2="${W - padR}" y2="${sy}" stroke="${strCol}" stroke-width="${thickness}"/>`;

    for (let f = viewStartFret; f <= viewEndFret; f++) {
      const m = openMidi + f;
      const lbl = labels[((m % 12) + 12) % 12];
      const cx = nutX + (f - viewStartFret + 0.5) * fretW;
      svg += `<text x="${cx}" y="${sy + 3.5}" fill="${ghostCol}" font-size="9" font-weight="600" text-anchor="middle">${lbl}</text>`;
    }
  }

  const xForFret = (f) => (f === 0 ? (openX + 10) : (nutX + (f - viewStartFret + 0.5) * fretW));
  const isFretVisible = (f) => (f === 0 ? showOpenCol : (f >= viewStartFret && f <= viewEndFret));

  // Podgląd kolejnego arpeggio (przerywana linia + przerywane kontury) oraz ścieżka aktualnego kształtu
  const curOccupiedKeys = new Set(displayEntries.map(({ ev }) => `${ev.s}_${ev.f}`));
  if (!isComposer && nextEntries.length > 0) {
    svg += buildExerciseShapeNextOverlaySvg(nextEntries, curOccupiedKeys, topY, strGap, labels, xForFret, isFretVisible, light);
  }
  if (!isComposer && displayEntries.length >= 2) {
    const pts = [];
    displayEntries.forEach(({ ev }) => {
      if (isFretVisible(ev.f)) {
        const cx = xForFret(ev.f);
        const cy = topY + (5 - ev.s) * strGap;
        if (!pts.length || pts[pts.length - 1].cx !== cx || pts[pts.length - 1].cy !== cy) {
          pts.push({ cx, cy });
        }
      }
    });
    if (pts.length >= 2) {
      svg += `<polyline points="${pts.map(p => `${p.cx.toFixed(1)},${p.cy.toFixed(1)}`).join(' ')}" fill="none" stroke="rgba(243,146,55,0.45)" stroke-width="2" stroke-dasharray="5,3"/>`;
    }
  }

  // Kropki/romby zadeklarowanego kształtu + zielone potwierdzenie dla trafionych dźwięków
  const usedPos = new Map();
  displayEntries.forEach(({ ev, idx }) => {
    const key = `${ev.s}_${ev.f}`;
    const existing = usedPos.get(key) || { ev, completed: false };
    if (isPlayMode && completedSet && completedSet.has(idx)) existing.completed = true;
    existing.ev = ev;
    usedPos.set(key, existing);
  });

  usedPos.forEach(({ ev, completed }) => {
    if (!isFretVisible(ev.f)) return;
    const row = 5 - ev.s;
    const cy = topY + row * strGap;
    const cx = xForFret(ev.f);
    const openMidi = state.customStringMidis[ev.s] !== undefined ? state.customStringMidis[ev.s] : [40, 45, 50, 55, 59, 64][ev.s];
    const m = ev.midi || (openMidi + ev.f);
    const pc = ((m % 12) + 12) % 12;
    const nm = (state.labelType === 'intervals')
      ? computeIntervalLabel(pc, state.rootPc, false)
      : (ev.note || labels[pc]);
    if (completed) {
      svg += `<circle cx="${cx}" cy="${cy}" r="11" fill="#22c55e" stroke="#86efac" stroke-width="1.5"/>`;
      svg += `<text x="${cx}" y="${cy + 3.8}" fill="#052e16" font-size="10" font-weight="900" text-anchor="middle">${nm}</text>`;
    } else if (ev.tech === '◊') {
      const d = 12;
      svg += `<polygon points="${cx},${cy - d} ${cx + d},${cy} ${cx},${cy + d} ${cx - d},${cy}" fill="#143524" stroke="#10b981" stroke-width="1.8"/>`;
      svg += `<text x="${cx}" y="${cy + 3.5}" fill="#d1fae5" font-size="9.5" font-weight="800" text-anchor="middle">${nm}</text>`;
    } else {
      svg += `<circle cx="${cx}" cy="${cy}" r="10.5" fill="#4f290f" stroke="#a65b26" stroke-width="1.4"/>`;
      svg += `<text x="${cx}" y="${cy + 3.8}" fill="#f5d0a9" font-size="10" font-weight="800" text-anchor="middle">${nm}</text>`;
    }
  });

  // Podświetlona aktualnie oczekiwana / grana nuta
  if (highlightIdx >= 0 && highlightIdx < evs.length) {
    const aEv = evs[highlightIdx];
    if (aEv && !aEv.rest && aEv.s >= 0 && aEv.f >= 0 && isFretVisible(aEv.f)) {
      const row = 5 - aEv.s;
      const cy = topY + row * strGap;
      const cx = xForFret(aEv.f);
      const openMidi = state.customStringMidis[aEv.s] !== undefined ? state.customStringMidis[aEv.s] : [40, 45, 50, 55, 59, 64][aEv.s];
      const m = aEv.midi || (openMidi + aEv.f);
      const pc = ((m % 12) + 12) % 12;
      const nm = (state.labelType === 'intervals')
        ? computeIntervalLabel(pc, state.rootPc, false)
        : (aEv.note || labels[pc]);
      const isCurCompleted = isPlayMode && completedSet && completedSet.has(highlightIdx);
      const mainCol = isCurCompleted ? '#22c55e' : '#f39237';
      const haloFill = isCurCompleted ? 'rgba(34,197,94,0.24)' : 'rgba(243,146,55,0.24)';
      svg += `<circle cx="${cx}" cy="${cy}" r="16" fill="${haloFill}" stroke="${mainCol}" stroke-width="2.2"/>`;
      if (aEv.tech === '◊') {
        const d = 13;
        svg += `<polygon points="${cx},${cy - d} ${cx + d},${cy} ${cx},${cy + d} ${cx - d},${cy}" fill="${mainCol}" stroke="#fff2e2" stroke-width="1.4"/>`;
      } else {
        svg += `<circle cx="${cx}" cy="${cy}" r="11.5" fill="${mainCol}" stroke="#fff2e2" stroke-width="1.3"/>`;
      }
      svg += `<text x="${cx}" y="${cy + 4}" fill="#0b140e" font-size="10.5" font-weight="900" text-anchor="middle">${nm}</text>`;
    }
  }

  // Przezroczyste pola klikalne dla wszystkich widocznych progów
  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const ry = topY + row * strGap - strGap * 0.5;
    if (showOpenCol) {
      svg += `<rect x="4" y="${ry}" width="${nutX - 4}" height="${strGap}" fill="transparent" style="cursor:pointer" onclick="onExerciseFretCellClick(${s},0,${isComposer})"/>`;
    }
    for (let f = viewStartFret; f <= viewEndFret; f++) {
      const rx = nutX + (f - viewStartFret) * fretW;
      svg += `<rect x="${rx}" y="${ry}" width="${fretW}" height="${strGap}" fill="transparent" style="cursor:pointer" onclick="onExerciseFretCellClick(${s},${f},${isComposer})"/>`;
    }
  }

  svg += `</svg>`;
  return svg;
}

// =========================================================================
// TRYB KSZTAŁTU ZE WSTĘGĄ NUT: pokazuje zadeklarowany kształt + zielone trafienia
// =========================================================================
function buildScaleRibbonFretboardSvg(highlightIdx = 0) {
  const evs = state.recordedEvents || [];
  const light = isLightTheme();
  const W = 1060, H = 228;
  const openX = 24, nutX = 56, padR = 16;
  const maxBoardFret = state.maxDisplayedFret || 24;
  const topY = 26, strGap = 31;
  const labels = getRootLabels();
  const isPlayMode = (state.exercisePlayerMode === 'play');
  const completedSet = (state.exerciseCompletedSet instanceof Set) ? state.exerciseCompletedSet : null;

  const shapeSlice = getActiveExerciseShapeSlice(evs, highlightIdx);
  const shapeEntries = shapeSlice.events.filter(({ ev }) => !ev.rest && ev.s >= 0 && ev.f >= 0);
  const nextSlice = getNextExerciseShapeSlice(evs, shapeSlice);
  const nextEntries = nextSlice
    ? nextSlice.events.filter(({ ev }) => !ev.rest && ev.s >= 0 && ev.f >= 0)
    : [];

  // Stabilne wyśrodkowanie całego ćwiczenia raz dla wszystkich jego nut (nigdy nie przeskakuje między kształtami)
  const win = computeStableExerciseFretWindow(evs, maxBoardFret);
  const viewStartFret = win.viewStartFret;
  const viewEndFret = win.viewEndFret;
  const showOpenCol = win.showOpenCol;
  const visibleFrets = Math.max(1, viewEndFret - viewStartFret + 1);
  const fretW = (W - nutX - padR) / visibleFrets;
  const fretInlays = new Set([3, 5, 7, 9, 12, 15, 17, 19, 21, 24]);
  const dotsMode = (state.fretMarkersMode === 'dots');
  const numAll = !!state.numberAllFrets;

  const curEv = (highlightIdx >= 0 && highlightIdx < evs.length) ? evs[highlightIdx] : null;
  const bgFill = light ? '#f6f1e9' : '#14100d';
  const bgStroke = light ? '#d6cec2' : '#2e261f';
  const wireCol = light ? '#cfc6b8' : '#352c25';
  const strCol = light ? '#8c7e70' : '#574b40';
  const nutCol = light ? '#6b5e50' : '#a89f94';
  const numCol = light ? '#57534e' : '#857d75';

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" style="width:100%;height:auto;display:block;user-select:none;" xmlns="http://www.w3.org/2000/svg">`;
  svg += `<rect x="${nutX}" y="${topY - 12}" width="${W - nutX - padR}" height="${5 * strGap + 24}" rx="5" fill="${bgFill}" stroke="${bgStroke}" stroke-width="1"/>`;

  for (let f = viewStartFret; f <= viewEndFret; f++) {
    const colIdx = f - viewStartFret;
    const fx = nutX + (colIdx + 1) * fretW;
    svg += `<line x1="${fx}" y1="${topY - 12}" x2="${fx}" y2="${topY + 5 * strGap + 12}" stroke="${wireCol}" stroke-width="1.2"/>`;
    const isMarker = fretInlays.has(f);
    if (numAll || (!dotsMode && isMarker)) {
      svg += `<text x="${fx - fretW * 0.5}" y="${topY + 5 * strGap + 30}" fill="${numCol}" font-size="11" font-weight="700" text-anchor="middle">${f}</text>`;
    } else if (dotsMode && isMarker) {
      svg += `<circle cx="${fx - fretW * 0.5}" cy="${topY + 5 * strGap + 26}" r="3" fill="${numCol}"/>`;
    }
  }
  svg += `<line x1="${nutX}" y1="${topY - 12}" x2="${nutX}" y2="${topY + 5 * strGap + 12}" stroke="${showOpenCol ? nutCol : wireCol}" stroke-width="${showOpenCol ? 3 : 1.4}"/>`;

  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const sy = topY + row * strGap;
    const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    const openLbl = labels[((openMidi % 12) + 12) % 12];
    svg += `<text x="${openX - 10}" y="${sy + 4}" fill="${numCol}" font-size="11.5" font-weight="800" text-anchor="middle">${openLbl}</text>`;
    svg += `<line x1="${nutX}" y1="${sy}" x2="${W - padR}" y2="${sy}" stroke="${strCol}" stroke-width="${(1.05 + row * 0.32).toFixed(2)}"/>`;
  }

  const xForFret = (f) => (f === 0 ? (openX + 10) : (nutX + (f - viewStartFret + 0.5) * fretW));
  const isFretVisible = (f) => (f === 0 ? showOpenCol : (f >= viewStartFret && f <= viewEndFret));

  // Kolejne arpeggio (przerywana linia i przerywane niebieskie kontury) + ścieżka bieżącego kształtu
  const curOccupiedKeys = new Set(shapeEntries.map(({ ev }) => `${ev.s}_${ev.f}`));
  if (nextEntries.length > 0) {
    svg += buildExerciseShapeNextOverlaySvg(nextEntries, curOccupiedKeys, topY, strGap, labels, xForFret, isFretVisible, light);
  }
  if (shapeEntries.length >= 2) {
    const pts = [];
    shapeEntries.forEach(({ ev }) => {
      if (isFretVisible(ev.f)) {
        const cx = xForFret(ev.f);
        const cy = topY + (5 - ev.s) * strGap;
        if (!pts.length || pts[pts.length - 1].cx !== cx || pts[pts.length - 1].cy !== cy) {
          pts.push({ cx, cy });
        }
      }
    });
    if (pts.length >= 2) {
      svg += `<polyline points="${pts.map(p => `${p.cx.toFixed(1)},${p.cy.toFixed(1)}`).join(' ')}" fill="none" stroke="rgba(243,146,55,0.45)" stroke-width="2" stroke-dasharray="5,3"/>`;
    }
  }

  // Grupuj pozycje zadeklarowanego kształtu (bez zbędnych nut całej skali!)
  const posMap = new Map();
  shapeEntries.forEach(({ ev, idx }) => {
    const key = `${ev.s}_${ev.f}`;
    const rec = posMap.get(key) || { ev, completed: false, upcoming: false, isCurPos: false };
    if (idx === highlightIdx) rec.isCurPos = true;
    else if (idx > highlightIdx) rec.upcoming = true;
    if (isPlayMode && completedSet && completedSet.has(idx)) rec.completed = true;
    posMap.set(key, rec);
  });

  posMap.forEach(({ ev, completed, upcoming, isCurPos }) => {
    if (!isFretVisible(ev.f)) return;
    const row = 5 - ev.s;
    const sy = topY + row * strGap;
    const cx = xForFret(ev.f);
    const openMidi = state.customStringMidis[ev.s] !== undefined ? state.customStringMidis[ev.s] : [40, 45, 50, 55, 59, 64][ev.s];
    const m = ev.midi || (openMidi + ev.f);
    const pc = ((m % 12) + 12) % 12;
    const nm = (state.labelType === 'intervals') ? computeIntervalLabel(pc, state.rootPc, false) : (ev.note || labels[pc]);

    svg += `<g style="cursor:pointer" onclick="onExerciseFretCellClick(${ev.s},${ev.f},false)">`;
    if (isCurPos) {
      const targetCol = completed ? '#22c55e' : '#f39237';
      svg += `<circle cx="${cx}" cy="${sy}" r="16" fill="${completed ? 'rgba(34,197,94,0.24)' : 'rgba(243,146,55,0.24)'}" stroke="${targetCol}" stroke-width="2.4"/>`;
      svg += `<circle cx="${cx}" cy="${sy}" r="11.5" fill="${targetCol}" stroke="#fef08a" stroke-width="1.4"/>`;
      svg += `<text x="${cx}" y="${sy + 3.8}" fill="#0b140e" font-size="10.5" font-weight="900" text-anchor="middle">${nm}</text>`;
    } else if (completed) {
      svg += `<circle cx="${cx}" cy="${sy}" r="11" fill="#22c55e" stroke="#86efac" stroke-width="1.5"/>`;
      svg += `<text x="${cx}" y="${sy + 3.6}" fill="#052e16" font-size="10" font-weight="900" text-anchor="middle">${nm}</text>`;
    } else if (upcoming) {
      svg += `<circle cx="${cx}" cy="${sy}" r="10.5" fill="#60a5fa" stroke="#93c5fd" stroke-width="1.1"/>`;
      svg += `<text x="${cx}" y="${sy + 3.6}" fill="#0b1320" font-size="10" font-weight="800" text-anchor="middle">${nm}</text>`;
    } else {
      svg += `<circle cx="${cx}" cy="${sy}" r="10" fill="${light ? '#d6c7b4' : '#4f290f'}" stroke="${light ? '#b8a48c' : '#a65b26'}" stroke-width="1.2"/>`;
      svg += `<text x="${cx}" y="${sy + 3.5}" fill="${light ? '#3d2817' : '#f5d0a9'}" font-size="9.8" font-weight="700" text-anchor="middle">${nm}</text>`;
    }
    svg += `</g>`;
  });

  // Jeśli curEv nie miał (s, f), zachowaj bezpieczeństwo widoku
  if (curEv && !curEv.rest && curEv.s >= 0 && curEv.f >= 0 && !posMap.has(`${curEv.s}_${curEv.f}`) && isFretVisible(curEv.f)) {
    const cx = xForFret(curEv.f);
    const sy = topY + (5 - curEv.s) * strGap;
    const nm = curEv.note || labels[((curEv.midi % 12) + 12) % 12];
    svg += `<circle cx="${cx}" cy="${sy}" r="11.5" fill="#f39237" stroke="#fef08a" stroke-width="1.5"/>`;
    svg += `<text x="${cx}" y="${sy + 3.8}" fill="#0b140e" font-size="10.5" font-weight="900" text-anchor="middle">${nm}</text>`;
  }

  svg += `</svg>`;
  return svg;
}

function buildScaleRibbonPlayerHtml(stepIdx) {
  const evs = state.recordedEvents || [];
  const labels = getRootLabels();
  const fmtNote = (ev) => {
    if (!ev || ev.rest || !ev.midi) return { nm: '𝄽', oct: '' };
    const pc = ((Math.round(ev.midi) % 12) + 12) % 12;
    const oct = Math.floor(Math.round(ev.midi) / 12) - 1;
    return { nm: ev.note || labels[pc], oct: String(oct) };
  };
  const cur = evs[stepIdx];
  const next1 = evs[stepIdx + 1];
  const futureSlice = evs.slice(stepIdx + 2, stepIdx + 8);
  const curFmt = fmtNote(cur);
  const nextFmt = next1 ? fmtNote(next1) : null;

  let dirBadge = t('→ SEKWENCJA', '→ SEQUENCE');
  if (evs.length >= 2 && !evs[0].rest && !evs[1].rest) {
    if (evs[1].midi > evs[0].midi) dirBadge = t('↑ ROSNĄCO', '↑ ASCENDING');
    else if (evs[1].midi < evs[0].midi) dirBadge = t('↓ MALEJĄCO', '↓ DESCENDING');
  }

  const heardMidi = (state.detectedMidi !== null && state.detectedMidi !== undefined) ? Math.round(state.detectedMidi) : null;
  const heardMatch = (heardMidi !== null && cur && !cur.rest && Math.round(cur.midi) === heardMidi);
  const justHit = (typeof performance !== 'undefined' && (performance.now() - (state.exerciseLastHitTime || 0)) < 1100);
  const heardHtml = heardMidi !== null
    ? `${labels[((heardMidi % 12) + 12) % 12]}<sub>${Math.floor(heardMidi / 12) - 1}</sub>`
    : '—';

  return `
    <div class="scale-ribbon-stage">
      <div class="ribbon-dir-pill">${dirBadge}</div>
      <div class="scale-ribbon-heard ${(heardMatch || justHit) ? 'hit' : ''}" title="${t('Ostatnio wykryty / zagrany dźwięk', 'Last detected / played note')}">
        <span style="font-size:9px;text-transform:uppercase;opacity:0.7;">${t('Grasz', 'Played')}</span>
        <span>${heardHtml}</span>
      </div>
      <div class="scale-ribbon-track">
        <div class="ribbon-card-cur">${curFmt.nm}<sub>${curFmt.oct}</sub></div>
        ${nextFmt ? `<div class="ribbon-card-next">${nextFmt.nm}<sub>${nextFmt.oct}</sub></div>` : ''}
        ${futureSlice.map((ev, i) => {
          const f = fmtNote(ev);
          const op = Math.max(0.22, 0.80 - i * 0.12).toFixed(2);
          return `<div class="ribbon-card-future" style="opacity:${op};">${f.nm}<sub>${f.oct}</sub></div>`;
        }).join('')}
      </div>
    </div>
    <div style="display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;gap:8px;font-size:11.5px;color:var(--text-muted);padding:0 4px;">
      <span>${t('🎯 Na gryfie poniżej widzisz tylko aktualny zadeklarowany kształt — trafione przez mikrofon nuty zmieniają kolor na zielony.', '🎯 Only the active declared shape is shown below — detected notes turn green.')}</span>
      <span style="display:flex;gap:12px;align-items:center;">
        <span><b style="color:#22c55e;">●</b> ${t('Zaliczone', 'Hit')}</span>
        <span><b style="color:#f39237;">◉</b> ${t('Zagraj teraz', 'Play now')}</span>
        <span><b style="color:#60a5fa;">●</b> ${t('Nadchodzące', 'Upcoming')}</span>
      </span>
    </div>
    <div style="background:var(--bg-card);border:1px solid var(--border);border-radius:12px;padding:14px 16px;overflow-x:auto;">
      ${buildScaleRibbonFretboardSvg(stepIdx)}
    </div>`;
}

// =========================================================================
// KATALOG MELODII I RIFFÓW ŁĄCZĄCYCH NORMALNE NUTY Z FLAŻOLETAMI (XII, VII, V)
// =========================================================================
const HARMONICS_EXERCISES = [
  {
    id: 'harm_ballad_intro_em',
    title: 'Balladowe intro Em (Arpeggio + dzwonki XII)',
    level: 'Łatwy',
    bpm: 76,
    fretsLabel: 'Nuty + flażolety XII',
    desc: 'Klasyczne akustyczne intro w E-moll: spokojne rozłożenie otwartych strun zwieńczone tylko 3 flażoletami na XII progu, potem akord C/D i jeden kończący dzwonek.',
    steps: [
      { s: 0, f: 0, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false },
      { s: 4, f: 0, dur: 1, harm: false }, { s: 5, f: 0, dur: 1, harm: false },
      { s: 4, f: 0, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false },
      { s: 3, f: 12, dur: 1, harm: true }, { s: 4, f: 12, dur: 1, harm: true }, { s: 5, f: 12, dur: 0, harm: true },
      { s: 1, f: 3, dur: 1, harm: false }, { s: 2, f: 2, dur: 1, harm: false },
      { s: 3, f: 0, dur: 1, harm: false }, { s: 4, f: 0, dur: 1, harm: false },
      { s: 2, f: 0, dur: 1, harm: false }, { s: 3, f: 2, dur: 1, harm: false },
      { s: 5, f: 12, dur: -1, harm: true }
    ]
  },
  {
    id: 'harm_call_response',
    title: 'Pytanie i odpowiedź (Bas E + flażolety XII / VII / V)',
    level: 'Łatwy',
    bpm: 82,
    fretsLabel: 'Bas + akcenty XII · VII · V',
    desc: 'Dialog między normalną linią basu na strunach E i A a krótkimi, pojedynczymi odpowiedziami dzwonkowymi na progach XII, VII i V.',
    steps: [
      { s: 0, f: 0, dur: 1, harm: false }, { s: 0, f: 2, dur: 1, harm: false }, { s: 0, f: 3, dur: 0, harm: false },
      { s: 4, f: 12, dur: 1, harm: true }, { s: 5, f: 12, dur: 0, harm: true },
      { s: 1, f: 0, dur: 1, harm: false }, { s: 1, f: 2, dur: 0, harm: false },
      { s: 4, f: 7, dur: 1, harm: true },  { s: 5, f: 7, dur: 0, harm: true },
      { s: 0, f: 3, dur: 1, harm: false }, { s: 0, f: 2, dur: 1, harm: false }, { s: 0, f: 0, dur: 0, harm: false },
      { s: 5, f: 5, dur: -1, harm: true }
    ]
  },
  {
    id: 'harm_acoustic_em_c_g_d',
    title: 'Melodia Em – C – G – D (flażolet na końcu frazy)',
    level: 'Łatwy',
    bpm: 88,
    fretsLabel: '4 akordy + 1 flażolet / takt',
    desc: 'Naturalne, muzykalne granie po akordach Em–C–G–D: 3 zwykłe dźwięki akordu i jeden lśniący flażolet zamykający każdą frazę.',
    steps: [
      { s: 0, f: 0, dur: 1, harm: false }, { s: 2, f: 2, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false }, { s: 4, f: 12, dur: 0, harm: true },
      { s: 1, f: 3, dur: 1, harm: false }, { s: 2, f: 2, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false }, { s: 5, f: 12, dur: 0, harm: true },
      { s: 0, f: 3, dur: 1, harm: false }, { s: 2, f: 0, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false }, { s: 4, f: 7, dur: 0, harm: true },
      { s: 2, f: 0, dur: 1, harm: false }, { s: 3, f: 2, dur: 1, harm: false }, { s: 4, f: 3, dur: 1, harm: false }, { s: 5, f: 7, dur: 0, harm: true },
      { s: 0, f: 0, dur: -1, harm: false }
    ]
  },
  {
    id: 'harm_rock_riff_em',
    title: 'Rockowy riff w E-moll (z akcentami VII i V)',
    level: 'Średni',
    bpm: 98,
    fretsLabel: 'Riff rockowy + flażolety VII · V',
    desc: 'Klasyczny rockowy motyw na grubych strunach przełamywany dwoma ostrymi flażoletami na progu VII oraz finałem na progu V i XII.',
    steps: [
      { s: 0, f: 0, dur: 1, harm: false }, { s: 0, f: 0, dur: 1, harm: false },
      { s: 1, f: 2, dur: 1, harm: false }, { s: 2, f: 2, dur: 1, harm: false }, { s: 3, f: 0, dur: 1, harm: false },
      { s: 3, f: 7, dur: 1, harm: true },  { s: 4, f: 7, dur: 0, harm: true },
      { s: 0, f: 3, dur: 1, harm: false }, { s: 0, f: 2, dur: 1, harm: false },
      { s: 0, f: 0, dur: 1, harm: false }, { s: 1, f: 2, dur: 1, harm: false },
      { s: 3, f: 5, dur: 1, harm: true },  { s: 4, f: 5, dur: 1, harm: true }, { s: 5, f: 12, dur: -1, harm: true }
    ]
  },
  {
    id: 'harm_prog_melody',
    title: 'Progresywna fraza (Styl Yes / Rush)',
    level: 'Średni',
    bpm: 86,
    fretsLabel: 'Pochód melodyczny + XII · VII',
    desc: 'Śpiewny pochód na strunach D/G/B otwierający się w przestrzenne flażolety na progu XII i VII, z powrotem po normalnej melodii.',
    steps: [
      { s: 2, f: 2, dur: 1, harm: false }, { s: 2, f: 4, dur: 1, harm: false }, { s: 2, f: 5, dur: 1, harm: false },
      { s: 3, f: 4, dur: 1, harm: false }, { s: 4, f: 5, dur: 0, harm: false },
      { s: 3, f: 12, dur: 1, harm: true }, { s: 4, f: 7, dur: 1, harm: true }, { s: 5, f: 12, dur: 0, harm: true },
      { s: 4, f: 3, dur: 1, harm: false }, { s: 3, f: 2, dur: 1, harm: false },
      { s: 2, f: 4, dur: 1, harm: false }, { s: 2, f: 2, dur: 1, harm: false },
      { s: 0, f: 0, dur: 0, harm: false }, { s: 5, f: 12, dur: -1, harm: true }
    ]
  },
  {
    id: 'harm_warmup_12_7_5',
    title: 'Krótka rozgrzewka samych flażoletów (XII · VII · V)',
    level: 'Łatwy',
    bpm: 76,
    fretsLabel: 'Tylko flażolety: XII · VII · V',
    desc: 'Zwięzłe 9-nutowe ćwiczenie czystości ataku i wyczucia progów XII, VII i V przed zagraniem melodii powyżej.',
    steps: [
      { s: 2, f: 12, dur: 0, harm: true }, { s: 3, f: 12, dur: 0, harm: true }, { s: 4, f: 12, dur: 0, harm: true }, { s: 5, f: 12, dur: 0, harm: true },
      { s: 4, f: 7, dur: 0, harm: true },  { s: 3, f: 7, dur: 0, harm: true },  { s: 2, f: 7, dur: 0, harm: true },
      { s: 4, f: 5, dur: 0, harm: true },  { s: 5, f: 5, dur: -1, harm: true }
    ]
  }
];

function naturalHarmonicOffsetSemitones(fret) {
  if (fret === 12) return 12; // 2. składowa (+1 oktawa)
  if (fret === 7 || fret === 19) return 19; // 3. składowa (+oktawa i kwinta)
  if (fret === 5 || fret === 24) return 24; // 4. składowa (+2 oktawy)
  if (fret === 4 || fret === 9 || fret === 16) return 28; // 5. składowa (+2 oktawy i tercja wielka)
  return 12;
}

function buildHarmonicExerciseEvents(ex) {
  if (!ex || !Array.isArray(ex.steps)) return [];
  const labels = getRootLabels();
  return ex.steps.map(st => {
    const openMidi = state.customStringMidis[st.s] !== undefined ? state.customStringMidis[st.s] : [40, 45, 50, 55, 59, 64][st.s];
    const isHarm = (st.harm !== false);
    const midi = isHarm ? (openMidi + naturalHarmonicOffsetSemitones(st.f)) : (openMidi + st.f);
    const pc = ((midi % 12) + 12) % 12;
    return {
      s: st.s,
      f: st.f,
      midi: midi,
      note: labels[pc],
      freq: midiToFreq(midi),
      dur: st.dur !== undefined ? st.dur : 0,
      tuplet: 0,
      rest: 0,
      tech: isHarm ? '◊' : '-'
    };
  });
}

let harmonicPreviewTimers = [];
function stopHarmonicPreview() {
  harmonicPreviewTimers.forEach(id => clearTimeout(id));
  harmonicPreviewTimers = [];
}

function previewHarmonicExercise(exId) {
  stopHarmonicPreview();
  const ex = HARMONICS_EXERCISES.find(e => e.id === exId) || HARMONICS_EXERCISES[0];
  if (!ex) return;
  const evs = buildHarmonicExerciseEvents(ex);
  const qMs = 60000 / Math.max(50, ex.bpm || 84);
  let tAcc = 0;
  evs.forEach(ev => {
    const tid = setTimeout(() => {
      state.detectedMidi = ev.midi;
      if (ev.tech === '◊') {
        playHarmonicBellFrequency(ev.freq, 1.45);
      } else {
        pluckGuitarFrequency(ev.freq, 0.92);
      }
      updateGuitarToKeyboardSync();
    }, tAcc);
    harmonicPreviewTimers.push(tid);
    tAcc += Math.max(180, qMs * getDurationBeats(ev.dur, false));
  });
}

async function launchHarmonicExercise(exId, mode) {
  stopHarmonicPreview();
  const ex = HARMONICS_EXERCISES.find(e => e.id === exId) || HARMONICS_EXERCISES[0];
  if (!ex) return;
  state.selectedHarmonicExerciseId = ex.id;
  state.recordedEvents = buildHarmonicExerciseEvents(ex);
  state.tempoBpm = ex.bpm || 84;
  state.exerciseTitle = ex.title;
  state.exercisePlayerView = 'tab';
  state.mainView = 'practice';
  state.subtab = 'exercise';
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-subtab') === 'exercise');
  });
  if (mode === 'edit') {
    state.exerciseSubView = 'composer';
    state.exercisePlayerActive = false;
    await renderAll();
  } else {
    state.exerciseSubView = 'harmonics';
    await startExercisePlayer(mode || 'listen');
  }
}

function buildHarmonicsExerciseFretboardSvg(selectedEx) {
  const light = isLightTheme();
  const W = 1040, H = 216;
  const openX = 24, nutX = 56, padR = 16;
  const fretCount = 15;
  const fretW = (W - nutX - padR) / fretCount;
  const topY = 26, strGap = 29;
  const labels = getRootLabels();

  const usedHarmSet = new Set();
  const usedNormalMap = new Map();
  if (selectedEx && Array.isArray(selectedEx.steps)) {
    selectedEx.steps.forEach((st) => {
      const key = `${st.s}_${st.f}`;
      if (st.harm !== false) {
        usedHarmSet.add(key);
      } else {
        usedNormalMap.set(key, st);
      }
    });
  }

  const bgFill = light ? '#f6f1e9' : '#14100d';
  const bgStroke = light ? '#d6cec2' : '#2e261f';
  const wireCol = light ? '#cfc6b8' : '#352c25';
  const strCol = light ? '#8c7e70' : '#574b40';
  const numCol = light ? '#57534e' : '#8c8277';

  let svg = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}" style="width:100%;height:auto;display:block;user-select:none;" xmlns="http://www.w3.org/2000/svg">`;
  svg += `<rect x="${nutX}" y="${topY - 12}" width="${W - nutX - padR}" height="${5 * strGap + 24}" rx="5" fill="${bgFill}" stroke="${bgStroke}" stroke-width="1"/>`;

  // Podświetlenie głównych pasm flażoletów (5, 7, 12)
  [5, 7, 12].forEach(hf => {
    const xLine = nutX + hf * fretW;
    svg += `<line x1="${xLine}" y1="${topY - 12}" x2="${xLine}" y2="${topY + 5 * strGap + 12}" stroke="rgba(16,185,129,0.28)" stroke-width="5"/>`;
  });

  // Druty progowe + numery progów wyśrodkowane pomiędzy drutami (|1|, |2|, ..., |12|)
  svg += `<line x1="${nutX}" y1="${topY - 12}" x2="${nutX}" y2="${topY + 5 * strGap + 12}" stroke="${light ? '#6b5e50' : '#b8aea0'}" stroke-width="3"/>`;
  for (let f = 1; f <= fretCount; f++) {
    const fx = nutX + f * fretW;
    svg += `<line x1="${fx}" y1="${topY - 12}" x2="${fx}" y2="${topY + 5 * strGap + 12}" stroke="${wireCol}" stroke-width="1.2"/>`;
    const isMainH = (f === 5 || f === 7 || f === 12);
    svg += `<text x="${(fx - fretW * 0.5).toFixed(1)}" y="${topY + 5 * strGap + 28}" fill="${isMainH ? '#10b981' : numCol}" font-size="${isMainH ? 12 : 10.5}" font-weight="${isMainH ? 800 : 600}" text-anchor="middle">${f}</text>`;
  }

  // Struny
  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const sy = topY + row * strGap;
    const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    const openLbl = labels[((openMidi % 12) + 12) % 12];
    svg += `<text x="${openX - 10}" y="${sy + 4}" fill="${numCol}" font-size="11.5" font-weight="800" text-anchor="middle">${openLbl}</text>`;
    svg += `<line x1="${nutX}" y1="${sy}" x2="${W - padR}" y2="${sy}" stroke="${strCol}" stroke-width="${(1.05 + row * 0.32).toFixed(2)}"/>`;
  }

  // 1. Zwykłe nuty melodii (kółka ● pośrodku pól progów lub przy siodełku dla pustych strun)
  usedNormalMap.forEach((st) => {
    const row = 5 - st.s;
    const sy = topY + row * strGap;
    const cx = (st.f === 0) ? (openX + 12) : (nutX + (st.f - 0.5) * fretW);
    const openMidi = state.customStringMidis[st.s] !== undefined ? state.customStringMidis[st.s] : [40, 45, 50, 55, 59, 64][st.s];
    const m = openMidi + st.f;
    const pc = ((m % 12) + 12) % 12;
    const nm = labels[pc];
    const freq = midiToFreq(m);
    svg += `<g style="cursor:pointer" onclick="pluckGuitarFrequency(${freq.toFixed(2)}, 0.95)">`;
    svg += `<circle cx="${cx.toFixed(1)}" cy="${sy}" r="11" fill="#3b82f6" stroke="#bfdbfe" stroke-width="1.4"/>`;
    svg += `<text x="${cx.toFixed(1)}" y="${sy + 3.6}" fill="#ffffff" font-size="9.8" font-weight="800" text-anchor="middle">${nm}</text>`;
    svg += `</g>`;
  });

  // 2. Romby flażoletów naturalnych dokładnie nad drutami progowymi (4, 5, 7, 9, 12)
  const harmFrets = [
    { f: 12, diffCol: '#10b981' },
    { f: 7,  diffCol: '#10b981' },
    { f: 5,  diffCol: '#eab308' },
    { f: 9,  diffCol: '#f97316' },
    { f: 4,  diffCol: '#f97316' }
  ];
  for (let row = 0; row < 6; row++) {
    const s = 5 - row;
    const sy = topY + row * strGap;
    const openMidi = state.customStringMidis[s] !== undefined ? state.customStringMidis[s] : [40, 45, 50, 55, 59, 64][s];
    harmFrets.forEach(hf => {
      const cx = nutX + hf.f * fretW;
      const m = openMidi + naturalHarmonicOffsetSemitones(hf.f);
      const pc = ((m % 12) + 12) % 12;
      const nm = labels[pc];
      const freq = midiToFreq(m);
      const key = `${s}_${hf.f}`;
      const isInEx = usedHarmSet.has(key);
      const d = isInEx ? 13 : 9;

      svg += `<g style="cursor:pointer" onclick="playHarmonicBellFrequency(${freq.toFixed(2)}, 1.55)">`;
      if (isInEx) {
        svg += `<polygon points="${cx},${sy - (d + 3.5)} ${cx + (d + 3.5)},${sy} ${cx},${sy + (d + 3.5)} ${cx - (d + 3.5)},${sy}" fill="rgba(243,146,55,0.22)" stroke="#f39237" stroke-width="1.6"/>`;
        svg += `<polygon points="${cx},${sy - d} ${cx + d},${sy} ${cx},${sy + d} ${cx - d},${sy}" fill="#f39237" stroke="#fff7ed" stroke-width="1.3"/>`;
        svg += `<text x="${cx}" y="${sy + 3.6}" fill="#14100d" font-size="9.8" font-weight="900" text-anchor="middle">${nm}</text>`;
      } else {
        svg += `<polygon points="${cx},${sy - d} ${cx + d},${sy} ${cx},${sy + d} ${cx - d},${sy}" fill="${light ? 'rgba(16,185,129,0.10)' : 'rgba(16,185,129,0.14)'}" stroke="${hf.diffCol}" stroke-width="1.3" opacity="0.62"/>`;
        svg += `<text x="${cx}" y="${sy + 3.1}" fill="${light ? '#1c1917' : '#d6d3d1'}" font-size="8.2" font-weight="700" text-anchor="middle">${nm}</text>`;
      }
      svg += `</g>`;
    });
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

function setExercisePlayerBpm(newBpm) {
  const parsed = parseInt(newBpm, 10);
  const clamped = Math.max(30, Math.min(260, Number.isFinite(parsed) ? parsed : (state.tempoBpm || 120)));
  state.tempoBpm = clamped;
  if (typeof state.exerciseTitle === 'string' && /·\s*\d+\s*BPM\b/.test(state.exerciseTitle)) {
    state.exerciseTitle = state.exerciseTitle.replace(/·\s*\d+\s*BPM\b/, `· ${clamped} BPM`);
  }
  saveSettings();
  if (state.exercisePlayerActive && !state.exercisePlayerPaused) {
    if (state.spMetronome) startExerciseMetronome();
    if (state.exercisePlayerMode === 'listen') {
      scheduleNextExercisePlayerTick();
    }
  }
  renderStageAndInspector();
}

function setExercisePlayerSpeedRatio(ratio) {
  const base = state.exerciseBaseBpm || state.tempoBpm || 120;
  setExercisePlayerBpm(Math.round(base * ratio));
}

function switchExercisePlayerMode(mode) {
  const nextMode = (mode === 'listen') ? 'listen' : 'play';
  if (state.exercisePlayerMode === nextMode && state.exercisePlayerActive) return;
  if (exercisePlayerTimer) {
    clearTimeout(exercisePlayerTimer);
    exercisePlayerTimer = null;
  }
  state.exercisePlayerMode = nextMode;
  state.exercisePlayerPaused = false;
  if (state.exercisePlayerActive) {
    if (state.spMetronome) startExerciseMetronome();
    if (nextMode === 'listen') {
      scheduleNextExercisePlayerTick();
    } else if (!micStream) {
      try { toggleLiveAudio().catch(() => {}); } catch (e) {}
    }
  }
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
  state.exerciseCompletedSet = new Set();
  state.exerciseLastHitIdx = -1;
  state.exerciseLastHitTime = 0;
  state.exerciseLastHitLabel = '';
  if (shouldRender) renderAll();
}

function playExerciseStepAudio(ev, idx) {
  if (!ev) return;
  if (state.spBgChord && idx % Math.max(1, state.timeSigNum) === 0) {
    const rootM = 48 + (state.rootPc || 0);
    const rawDegs = (state.fretboardData && state.fretboardData.scaleDegrees) || [];
    const sc = rawDegs.map(d => (typeof d === 'number' ? d : (d && typeof d.semitones === 'number' ? d.semitones : null))).filter(x => x !== null);
    const triad = [rootM, rootM + (sc[2] !== undefined ? sc[2] : 4), rootM + (sc[4] !== undefined ? sc[4] : 7)];
    triad.forEach(m => pluckGuitarFrequency(midiToFreq(m), 0.22));
  }
  if (!ev.rest && ev.midi > 0) {
    state.detectedMidi = ev.midi;
    if (ev.tech === '◊') {
      playHarmonicBellFrequency(ev.freq || midiToFreq(ev.midi), 1.45);
    } else {
      pluckGuitarFrequency(ev.freq || midiToFreq(ev.midi), 0.92);
    }
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

  const qMs = 60000 / Math.max(30, state.tempoBpm);
  const beats = getDurationBeats(curEv.dur !== undefined ? curEv.dur : state.defaultDuration, curEv.tuplet === 3);
  const stepMs = Math.max(45, qMs * beats);

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
  if (!state.exercisePlayerActive) {
    state.exerciseBaseBpm = state.tempoBpm || 120;
  }
  state.exercisePlayerActive = true;
  state.exercisePlayerMode = mode || 'listen';
  state.exercisePlayerPaused = false;
  state.exercisePlayerStepIdx = 0;
  state.exerciseCompletedSet = new Set();
  state.exerciseLastHitIdx = -1;
  state.exerciseLastHitTime = 0;
  state.exerciseLastHitLabel = '';

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
  if (!(state.exerciseCompletedSet instanceof Set)) state.exerciseCompletedSet = new Set();
  state.exerciseCompletedSet.add(idx);
  state.exerciseLastHitIdx = idx;
  state.exerciseLastHitTime = now;
  const labels = getRootLabels();
  const hitNm = cur.note || labels[((Math.round(cur.midi) % 12) + 12) % 12];
  const hitOct = Math.floor(Math.round(cur.midi) / 12) - 1;
  state.exerciseLastHitLabel = `${hitNm}${hitOct}`;

  let nextIdx = idx + 1;
  while (nextIdx < evs.length && evs[nextIdx].rest) nextIdx++;
  if (nextIdx >= evs.length) {
    nextIdx = 0;
    while (nextIdx < evs.length - 1 && evs[nextIdx].rest) nextIdx++;
    state.exerciseCompletedSet = new Set();
  }
  state.exercisePlayerStepIdx = nextIdx;
  renderStageAndInspector();
}

function playScalePracticeScalePreview() {
  if (previewIsPlaying) {
    stopCurrentScalePreview(true);
    return;
  }
  stopCurrentScalePreview(false);
  const myToken = previewSessionToken;
  let seq = [];
  let stepMs = 240;

  if (state.scalePracticeMode === 'scale' && state.fretboardData && state.fretboardData.scaleNotes) {
    const notes = state.fretboardData.scaleNotes;
    const sharpLabels = ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'];
    const flatLabels = ['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B'];
    let lastMidi = 48 + (state.rootPc || 0) - 1;
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
  } else {
    const ivs = (typeof earChordsCatalog !== 'undefined')
      ? ((earChordsCatalog.find(c => c.id === state.chordId) || { intervals: [0, 4, 7] }).intervals)
      : [0, 4, 7];
    const baseMidi = 48 + (state.rootPc || 0);
    seq = ivs.map(s => baseMidi + s).concat([baseMidi + 12]);
    stepMs = 260;
  }

  if (seq.length === 0) return;
  previewIsPlaying = true;
  updateTopPreviewButtonUI();
  seq.forEach((m, i) => {
    const tid = setTimeout(() => {
      if (previewSessionToken !== myToken) return;
      stopPreviewVoices(0.025);
      state.detectedMidi = m;
      pluckGuitarFrequency(midiToFreq(m), 0.58, 1.0, true);
      updateGuitarToKeyboardSync();
    }, i * stepMs);
    previewTimers.push(tid);
  });
  const endTid = setTimeout(() => {
    if (previewSessionToken !== myToken) return;
    previewIsPlaying = false;
    previewTimers = [];
    updateTopPreviewButtonUI();
  }, seq.length * stepMs + 350);
  previewTimers.push(endTid);
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
  const chordIdNorm = normalizeChordId(state.chordId);
  const reqPat = (state.spPosSystem === 3) ? 1 : 6; // CAGED lub Od prymy (zadeklarowany kształt pozycji!)
  const spDirMap = { 0: 1, 1: 2, 2: 0, 3: 0 };
  const reqDir = spDirMap[state.spDirection || 0] ?? 0;
  const seqStyle = (state.scalePracticeMode === 'arpeggio_pattern') ? 1 : 0;
  try {
    const url = `/api/arpeggio?tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(chordIdNorm)}&pattern=${reqPat}&posIdx=${state.spPosIdx || 0}&direction=${reqDir}&seqStyle=${seqStyle}&octaves=${state.spOctaves >= 2 ? 2 : 1}&accMode=${state.accMode}`;
    const res = await fetchJson(url);
    if (res && Array.isArray(res.steps) && res.steps.length > 0) {
      const effDur = (state.spNoteDur !== undefined) ? state.spNoteDur : 1;
      const effTup = state.spTuplet ? 3 : 0;
      const techShort = { H: 'H', P: 'P', S: 'S', D: 'D', U: 'U' };
      return res.steps.map(st => ({
        s: st.s,
        f: st.f,
        midi: st.midi,
        note: st.note,
        freq: st.freq || midiToFreq(st.midi),
        dur: effDur,
        tuplet: effTup,
        rest: 0,
        tech: techShort[st.tech] || '-',
        seg: st.seg || 0,
        chordSymbol: res.chordSymbol || '',
        isRoot: !!st.isRoot,
        interval: st.interval || ''
      }));
    }
  } catch (e) {}

  let pcs = [];
  try {
    const vData = await fetchJson(`/api/voicings?tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}&rootPc=${state.rootPc}&chordId=${encodeURIComponent(chordIdNorm)}&allTypes=0&accMode=${state.accMode}`);
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
      ? ((earChordsCatalog.find(c => c.id === chordIdNorm) || { intervals: [0, 4, 7] }).intervals)
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

async function launchScalePractice(mode, viewMode) {
  state.posSystem = state.spPosSystem;
  state.posIdx = state.spPosIdx || 0;
  state.tempoBpm = state.spBpm || 100;
  if (viewMode === 'ribbon' || viewMode === 'tab') {
    state.exercisePlayerView = viewMode;
  }
  const scObjTitle = ((state.initData && state.initData.scales) || []).find(s => s.id === state.scaleId);
  const rootLblTitle = getRootLabels()[state.rootPc] || 'C';
  state.exerciseTitle = (state.scalePracticeMode === 'scale')
    ? `${rootLblTitle} ${scObjTitle ? scObjTitle.name : state.scaleId}`
    : `${rootLblTitle} ${state.chordId} (Arpeggio)`;
  saveSettings();
  let events = [];

  const fretZones = {
    all:  { min: 0,  max: 24 },
    low:  { min: 0,  max: 7  },
    mid:  { min: 5,  max: 14 },
    high: { min: 12, max: 24 }
  };
  const z = fretZones[state.spFretZone || 'all'] || fretZones.all;
  const effDur = (state.spNoteDur !== undefined) ? state.spNoteDur : 1;
  const effTup = state.spTuplet ? 3 : 0;

  if (state.scalePracticeMode === 'scale') {
    const customParam = state.customStringMidis.join(',');
    const baseQuery = `rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.spPosSystem}&posIdx=${state.spPosIdx || 0}&traversal=${state.spTraversal || 0}&direction=${state.spDirection}&octaves=${state.spOctaves}&pattern=${state.spPattern}&startOnRoot=${state.spStartOnRoot !== false ? 1 : 0}&minFret=${z.min}&maxFret=${z.max}&bpm=${state.spBpm || 100}&dur=${effDur}&tuplet=${state.spTuplet ? 1 : 0}&accMode=${state.accMode}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}`;
    const url = `/api/workshop_events?${baseQuery}&startString=${state.spStartString}`;
    const res = await fetchJson(url);
    events = (res && res.events ? res.events : []).map(ev => ({
      s: ev.s,
      f: ev.f,
      midi: ev.midi,
      note: ev.note,
      freq: ev.freq,
      dur: effDur,
      tuplet: effTup,
      rest: 0,
      tech: '-'
    }));
    if (events.length === 0 && (state.spStartString >= 0 || z.min > 0 || z.max < 24)) {
      const fallbackUrl = `/api/workshop_events?rootPc=${state.rootPc}&scaleId=${encodeURIComponent(state.scaleId)}&posSystem=${state.spPosSystem}&posIdx=${state.spPosIdx || 0}&traversal=${state.spTraversal || 0}&startString=-1&direction=${state.spDirection}&octaves=${state.spOctaves}&pattern=${state.spPattern}&startOnRoot=${state.spStartOnRoot !== false ? 1 : 0}&minFret=0&maxFret=24&bpm=${state.spBpm || 100}&dur=${effDur}&tuplet=${state.spTuplet ? 1 : 0}&accMode=${state.accMode}&tuning=${encodeURIComponent(state.tuningId)}&customMidis=${customParam}`;
      const resFb = await fetchJson(fallbackUrl);
      events = (resFb && resFb.events ? resFb.events : []).map(ev => ({
        s: ev.s, f: ev.f, midi: ev.midi, note: ev.note, freq: ev.freq, dur: effDur, tuplet: effTup, rest: 0, tech: '-'
      }));
    }
  } else {
    events = await buildArpeggioEventsForScalePractice();
  }

  if (events.length === 0 && state.fretboardData && state.fretboardData.cells) {
    events = state.fretboardData.cells
      .filter(c => c.inScale && !c.masked)
      .slice(0, 12)
      .map(c => ({ s: c.s, f: c.f, midi: c.midi, note: c.note, freq: c.freq, dur: effDur, tuplet: effTup, rest: 0, tech: '-' }));
  }
  if (events.length === 0) return;

  state.recordedEvents = events;
  state.mainView = 'practice';
  state.subtab = 'exercise';
  document.querySelectorAll('#practiceSubtabs .subtab-btn').forEach(b => {
    b.classList.toggle('active', b.getAttribute('data-subtab') === 'exercise');
  });
  if (mode === 'edit') {
    state.exerciseSubView = 'composer';
    state.exercisePlayerActive = false;
    await renderAll();
  } else {
    state.exerciseSubView = 'scale_practice';
    await startExercisePlayer(mode || 'play');
  }
}

function addScalePracticeToSession(sessionIdx) {
  const scObj = (state.initData && state.initData.scales || []).find(s => s.id === state.scaleId) || { name: 'Durowa' };
  const rootLabels = getRootLabels();
  const posNames = ['Cały gryf', '3NPS', '4NPS', 'CAGED'];
  const patNames = {
    0: 'Cała skala', 3: 'Grupy po 3', 4: 'Grupy po 4',
    5: 'Tercje', 6: 'Kwarty', 7: 'Kwinty', 8: 'Seksty', 9: 'Oktawy',
    10: 'Sekstole (6)', 11: 'Krok w tył', 12: 'Zwrot shred',
    13: 'Tercje zig-zag', 14: 'Trójdźwięki', 15: 'Trójdźwięki naprzem.',
    16: 'Septymowe', 17: 'Pedal point', 18: 'String Skipping'
  };
  const modeTag = state.spTraversal === 1 ? 'Diagonal' : (state.spTraversal === 2 ? '1 struna' : (posNames[state.spPosSystem] || 'Cały gryf'));
  const newEx = {
    id: 'ex_sp_' + Date.now(),
    title: `${rootLabels[state.rootPc]} ${scObj.name} · ${modeTag} (${state.spOctaves} okt.) · ${patNames[state.spPattern] || 'Cała skala'}`,
    category: 'Scale Practice',
    level: state.spOctaves >= 3 ? 'Zaawansowany' : (state.spOctaves === 2 ? 'Średniozaawansowany' : 'Początkujący'),
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
