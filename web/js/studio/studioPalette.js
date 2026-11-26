/**
 * Paleta narzędzi kompozytora a'la Songsterr (Strefa B) oraz szybka wyszukiwarka komend Ctrl+K.
 * Bez zewnętrznych zależności DOM — generuje HTML i filtruje komendy po polsku i angielsku.
 */
import { BEND_PRESETS, DYNAMIC_LEVELS, WHAMMY_PRESETS } from './studioModel.js';
import { escapeHtml } from './studioRenderer.js';

export const STUDIO_COMMANDS = Object.freeze([
  // Rytm & Łuki
  { id: 'dur:-2', label: 'Cała nuta (1/1)', shortLabel: '1/1', category: 'Rytm', shortcut: 'Alt+1', keywords: 'whole cala rytm dlugosc' },
  { id: 'dur:-1', label: 'Półnuta (1/2)', shortLabel: '1/2', category: 'Rytm', shortcut: 'Alt+2', keywords: 'half polnuta rytm' },
  { id: 'dur:0',  label: 'Ćwierćnuta (1/4)', shortLabel: '1/4', category: 'Rytm', shortcut: 'Alt+3', keywords: 'quarter cwiercnuta rytm' },
  { id: 'dur:1',  label: 'Ósemka (1/8)', shortLabel: '1/8', category: 'Rytm', shortcut: 'Alt+4', keywords: 'eighth osemka rytm' },
  { id: 'dur:2',  label: 'Szesnastka (1/16)', shortLabel: '1/16', category: 'Rytm', shortcut: 'Alt+5', keywords: 'sixteenth szesnastka rytm' },
  { id: 'dur:3',  label: 'Trzydziestodwójka (1/32)', shortLabel: '1/32', category: 'Rytm', shortcut: 'Alt+6', keywords: '32nd rytm' },
  { id: 'dur:4',  label: 'Sześćdziesięcioczwórka (1/64)', shortLabel: '1/64', category: 'Rytm', shortcut: 'Alt+7', keywords: '64th rytm' },
  { id: 'dot:1',  label: 'Kropka rytmiczna (×1.5)', shortLabel: '|.', category: 'Rytm', shortcut: '.', keywords: 'dotted kropka' },
  { id: 'dot:2',  label: 'Podwójna kropka (×1.75)', shortLabel: '|..', category: 'Rytm', shortcut: '', keywords: 'double dot podwojna kropka' },
  { id: 'rest',   label: 'Wstaw pauzę w uderzeniu', shortLabel: 'Pauza', category: 'Rytm', shortcut: 'R / Ins', keywords: 'rest pauza cisza' },
  { id: 'tuplet:3:2', label: 'Triola (3:2)', shortLabel: '└3┘', category: 'Rytm', shortcut: 'Alt+T', keywords: 'triplet triola tuplet' },
  { id: 'tuplet:5:4', label: 'Kwintola (5:4)', shortLabel: '5:4', category: 'Rytm', shortcut: '', keywords: 'quintuplet kwintola tuplet' },
  { id: 'tuplet:6:4', label: 'Sekstola (6:4)', shortLabel: '6:4', category: 'Rytm', shortcut: '', keywords: 'sextuplet sekstola sweep tuplet' },
  { id: 'effect:tied', label: 'Ligatura / Tie (przetrzymaj dźwięk)', shortLabel: '⌣ Tie', category: 'Rytm', shortcut: 'L', keywords: 'tie ligatura luk' },
  { id: 'beat:fermata', label: 'Fermata (𝄐)', shortLabel: '𝄐', category: 'Rytm', shortcut: '', keywords: 'fermata hold' },

  // Artykulacja & Flażolety
  { id: 'effect:ghostNote', label: 'Ghost Note (nuta w nawiasie)', shortLabel: '(0)', category: 'Artykulacja', shortcut: 'G', keywords: 'ghost duch nawias' },
  { id: 'effect:accented', label: 'Akcent (>)', shortLabel: '>', category: 'Artykulacja', shortcut: '', keywords: 'accent akcent' },
  { id: 'effect:heavyAccented', label: 'Mocny akcent (Marcato ^)', shortLabel: '^', category: 'Artykulacja', shortcut: '', keywords: 'marcato heavy accent' },
  { id: 'effect:staccato', label: 'Staccato (krótka nuta)', shortLabel: 'Stacc.', category: 'Artykulacja', shortcut: '', keywords: 'staccato krotko' },
  { id: 'effect:letRing', label: 'Let Ring (niech wybrzmiewa)', shortLabel: 'let ring', category: 'Artykulacja', shortcut: '', keywords: 'let ring sustain' },
  { id: 'effect:palmMute', label: 'Palm Mute (P.M.)', shortLabel: 'P.M.', category: 'Artykulacja', shortcut: 'P', keywords: 'palm mute pm tlumienie' },
  { id: 'effect:deadNote', label: 'Dead Note (tłumione -X-)', shortLabel: '-X-', category: 'Artykulacja', shortcut: 'X', keywords: 'dead note mute x' },
  { id: 'harmonic:1', label: 'Flażolet naturalny (◇)', shortLabel: '◇ Nat.', category: 'Artykulacja', shortcut: 'O', keywords: 'harmonic flazolet natural' },
  { id: 'harmonic:2', label: 'Flażolet sztuczny / Pinch (◆)', shortLabel: '◆ Pinch', category: 'Artykulacja', shortcut: '', keywords: 'pinch artificial harmonic' },

  // Legato, Slide, Bend, Kostkowanie
  { id: 'effect:hammerOn', label: 'Hammer-On / Pull-Off (0⌒1)', shortLabel: '0⌒1 H/P', category: 'Legato & Bend', shortcut: 'H', keywords: 'hammer pull legato hp' },
  { id: 'slide:shift', label: 'Shift Slide (0/1)', shortLabel: '0/1', category: 'Legato & Bend', shortcut: 'S', keywords: 'shift slide' },
  { id: 'slide:legato', label: 'Legato Slide (0⌒/1)', shortLabel: '0⌒/1', category: 'Legato & Bend', shortcut: '', keywords: 'legato slide' },
  { id: 'slide:in_below', label: 'Slide-in od dołu (/0)', shortLabel: '/0', category: 'Legato & Bend', shortcut: '', keywords: 'slide in' },
  { id: 'slide:out_down', label: 'Slide-out w dół (0\\)', shortLabel: '0\\', category: 'Legato & Bend', shortcut: '', keywords: 'slide out' },
  { id: 'bend:full', label: 'Bend +1 ton (↗)', shortLabel: '↗ +1', category: 'Legato & Bend', shortcut: 'B', keywords: 'bend podciagniecie' },
  { id: 'bend:half', label: 'Bend +1/2 tonu', shortLabel: '↗ +½', category: 'Legato & Bend', shortcut: '', keywords: 'half bend pol tonu' },
  { id: 'bend:one_half', label: 'Bend +1½ tonu', shortLabel: '↗ +1½', category: 'Legato & Bend', shortcut: '', keywords: 'bend 1.5' },
  { id: 'bend:bend_release', label: 'Bend + Release', shortLabel: '↗ B+R', category: 'Legato & Bend', shortcut: '', keywords: 'bend release' },
  { id: 'bend:pre_bend', label: 'Pre-bend', shortLabel: '↗ Pre', category: 'Legato & Bend', shortcut: '', keywords: 'pre bend' },
  { id: 'whammy:dive_full', label: 'Wajcha Dive -1 ton (↘W)', shortLabel: '↘W -1', category: 'Legato & Bend', shortcut: 'W', keywords: 'whammy wajcha tremolo bar dive' },
  { id: 'whammy:dive_bomb', label: 'Wajcha Dive Bomb -2½', shortLabel: '↘W -2½', category: 'Legato & Bend', shortcut: '', keywords: 'whammy wajcha dive bomb' },
  { id: 'whammy:dip_full', label: 'Wajcha Dip v -1', shortLabel: 'v-1', category: 'Legato & Bend', shortcut: '', keywords: 'whammy wajcha dip' },
  { id: 'whammy:scoop', label: 'Wajcha Scoop /-1→0', shortLabel: '/-1', category: 'Legato & Bend', shortcut: '', keywords: 'whammy wajcha scoop' },
  { id: 'whammy:raise_half', label: 'Wajcha Pull Up +½', shortLabel: '↗W +½', category: 'Legato & Bend', shortcut: '', keywords: 'whammy wajcha pull up raise' },
  { id: 'effect:vibrato', label: 'Vibrato (~)', shortLabel: '~ Vib', category: 'Legato & Bend', shortcut: 'V', keywords: 'vibrato wibrato' },
  { id: 'effect:wideVibrato', label: 'Szerokie Vibrato (w)', shortLabel: 'w~', category: 'Legato & Bend', shortcut: '', keywords: 'wide vibrato szerokie' },
  { id: 'pick:1', label: 'Kostkowanie w dół (⊓)', shortLabel: '⊓ Dół', category: 'Legato & Bend', shortcut: '', keywords: 'downstroke kostka dol' },
  { id: 'pick:2', label: 'Kostkowanie w górę (∨)', shortLabel: '∨ Góra', category: 'Legato & Bend', shortcut: '', keywords: 'upstroke kostka gora' },
  { id: 'brush:down', label: 'Rozbicie akordu w dół/górę (↓↑)', shortLabel: '↓↑', category: 'Legato & Bend', shortcut: '', keywords: 'brush arpeggiate akord' },
  { id: 'effect:tap', label: 'Tapping (TAP)', shortLabel: 'TAP', category: 'Legato & Bend', shortcut: '', keywords: 'tap tapping' },
  { id: 'effect:graceNote', label: 'Przednutka (Grace Note)', shortLabel: '♪ Przedn.', category: 'Legato & Bend', shortcut: '', keywords: 'grace note przednutka' },
  { id: 'effect:tremoloPicking', label: 'Tremolo Picking (///)', shortLabel: '/// Trem', category: 'Legato & Bend', shortcut: '', keywords: 'tremolo picking' },
  { id: 'effect:trill', label: 'Trill (tr)', shortLabel: 'tr', category: 'Legato & Bend', shortcut: '', keywords: 'trill tryl' },

  // Struktura
  { id: 'struct:repeatOpen', label: 'Początek repetycji (||:)', shortLabel: '||:', category: 'Struktura', shortcut: '', keywords: 'repeat open repetycja' },
  { id: 'prompt:repeatClose', label: 'Koniec repetycji (:|| ×N)', shortLabel: ':|| ×N', category: 'Struktura', shortcut: '', keywords: 'repeat close repetycja' },
  { id: 'struct:volta:1', label: 'Volta 1. zakończenie (┌1.)', shortLabel: '┌1', category: 'Struktura', shortcut: '', keywords: 'volta 1 zakonczenie' },
  { id: 'struct:volta:2', label: 'Volta 2. zakończenie (┌2.)', shortLabel: '┌2', category: 'Struktura', shortcut: '', keywords: 'volta 2 zakonczenie' },
  { id: 'struct:addBefore', label: 'Dodaj takt przed bieżącym (+▬)', shortLabel: '+▬', category: 'Struktura', shortcut: '', keywords: 'add measure before dodaj takt' },
  { id: 'struct:addAfter', label: 'Dodaj takt po bieżącym (▬+)', shortLabel: '▬+', category: 'Struktura', shortcut: '', keywords: 'add measure after dodaj takt' },
  { id: 'prompt:section', label: 'Znacznik sekcji (Intro / Riff / Refren)', shortLabel: 'sect.', category: 'Struktura', shortcut: 'M', keywords: 'section sekcja marker intro refren solo' },
  { id: 'prompt:text', label: 'Tekst nad uderzeniem (TXT)', shortLabel: 'TXT', category: 'Struktura', shortcut: 'T', keywords: 'text tekst adnotacja' },
  { id: 'prompt:chord', label: 'Symbol akordu nad uderzeniem ([A])', shortLabel: '[A]', category: 'Struktura', shortcut: '', keywords: 'chord symbol akord' },

  // Przekształcenia, schowek i kasowanie
  { id: 'transform:strUp', label: 'Przenieś na wyższą strunę (ta sama wysokość)', shortLabel: 'Str ↑', category: 'Przekształcenia', shortcut: 'Alt+↑', keywords: 'string up struna wysokosc' },
  { id: 'transform:strDown', label: 'Przenieś na niższą strunę (ta sama wysokość)', shortLabel: 'Str ↓', category: 'Przekształcenia', shortcut: 'Alt+↓', keywords: 'string down struna wysokosc' },
  { id: 'transform:semiUp', label: 'Pół tonu w górę (+1 próg)', shortLabel: '+1 półt.', category: 'Przekształcenia', shortcut: 'Shift+→', keywords: 'semitone up pol tonu' },
  { id: 'transform:semiDown', label: 'Pół tonu w dół (-1 próg)', shortLabel: '-1 półt.', category: 'Przekształcenia', shortcut: 'Shift+←', keywords: 'semitone down pol tonu' },
  { id: 'transform:octUp', label: 'Oktawa w górę (+12)', shortLabel: '+Okt', category: 'Przekształcenia', shortcut: 'Shift+↑', keywords: 'octave up oktawa' },
  { id: 'transform:octDown', label: 'Oktawa w dół (-12)', shortLabel: '-Okt', category: 'Przekształcenia', shortcut: 'Shift+↓', keywords: 'octave down oktawa' },
  { id: 'transform:moveBeatLeft', label: 'Przesuń uderzenie w lewo', shortLabel: '← Beat', category: 'Przekształcenia', shortcut: '', keywords: 'move beat left' },
  { id: 'transform:moveBeatRight', label: 'Przesuń uderzenie w prawo', shortLabel: 'Beat →', category: 'Przekształcenia', shortcut: '', keywords: 'move beat right' },
  { id: 'prompt:transpose', label: 'Transpozycja zakresu taktów (A⇅)', shortLabel: 'A⇅', category: 'Przekształcenia', shortcut: '', keywords: 'transpose transpozycja' },

  { id: 'clip:cut', label: 'Wytnij takty do schowka', shortLabel: '✂ Wytnij', category: 'Schowek', shortcut: '', keywords: 'cut wytnij' },
  { id: 'clip:copy', label: 'Kopiuj takty do schowka', shortLabel: '📋 Kopiuj', category: 'Schowek', shortcut: 'Ctrl+C', keywords: 'copy kopiuj' },
  { id: 'clip:paste', label: 'Wklej takty ze schowka', shortLabel: '📥 Wklej', category: 'Schowek', shortcut: 'Ctrl+V', keywords: 'paste wklej' },
  { id: 'clip:dup1', label: 'Duplikuj bieżący takt / blok (×1)', shortLabel: '⚡ ×1', category: 'Schowek', shortcut: 'D', keywords: 'duplicate duplikuj' },
  { id: 'clip:dup2', label: 'Duplikuj ×2', shortLabel: '⚡ ×2', category: 'Schowek', shortcut: '', keywords: 'duplicate x2' },
  { id: 'clip:dup4', label: 'Duplikuj ×4', shortLabel: '⚡ ×4', category: 'Schowek', shortcut: '', keywords: 'duplicate x4' },

  { id: 'del:note', label: 'Usuń nutę pod kursorem (NOTE 🗑)', shortLabel: 'NOTE 🗑', category: 'Kasowanie', shortcut: 'Del', keywords: 'delete note usun nute' },
  { id: 'del:beat', label: 'Usuń uderzenie (BEAT 🗑)', shortLabel: 'BEAT 🗑', category: 'Kasowanie', shortcut: 'Ctrl+Del', keywords: 'delete beat usun uderzenie' },
  { id: 'del:barClear', label: 'Wyczyść zawartość taktu (BAR 🗑)', shortLabel: 'BAR 🗑', category: 'Kasowanie', shortcut: 'Shift+Del', keywords: 'clear bar wyczysc takt' },
  { id: 'del:barRemove', label: 'Usuń takt z utworu', shortLabel: '− Takt', category: 'Kasowanie', shortcut: 'Ctrl+Shift+Del', keywords: 'remove measure usun takt' },
  { id: 'edit:insertBeat', label: 'Wstaw nowe uderzenie (pauzę) po kursorze', shortLabel: '+ Uderzenie', category: 'Rytm', shortcut: '', keywords: 'insert beat dodaj uderzenie' }
]);

function normalizeSearch(s) {
  return String(s || '')
    .toLowerCase()
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '')
    .replace(/ł/g, 'l');
}

export function filterStudioCommands(query) {
  const q = normalizeSearch(query).trim();
  if (!q) return STUDIO_COMMANDS.slice();
  const terms = q.split(/\s+/).filter(Boolean);
  return STUDIO_COMMANDS.filter(cmd => {
    const hay = normalizeSearch(`${cmd.label} ${cmd.shortLabel} ${cmd.category} ${cmd.shortcut} ${cmd.keywords}`);
    return terms.every(t => hay.includes(t));
  });
}

function palBtn(cmd, symbol, active = false, tipName = '', tipDesc = '', shortcut = '', extraCls = '') {
  const cls = ['studio-pal-btn', active ? 'active' : '', extraCls].filter(Boolean).join(' ');
  const nm = tipName || symbol;
  const kbdAttr = shortcut ? ` data-tip-kbd="${escapeHtml(shortcut)}"` : '';
  const descAttr = tipDesc ? ` data-tip-desc="${escapeHtml(tipDesc)}"` : '';
  return `<button type="button" class="${cls}" data-action="palette-cmd" data-cmd="${escapeHtml(cmd)}" data-tip-name="${escapeHtml(nm)}"${descAttr}${kbdAttr} aria-label="${escapeHtml(nm)}">${escapeHtml(symbol)}</button>`;
}

export function renderInlinePromptBar(inlinePrompt) {
  if (!inlinePrompt) return '';
  const type = inlinePrompt.type;
  const val = inlinePrompt.value ?? '';
  if (type === 'section') {
    const presets = ['Intro', 'Riff', 'Zwrotka', 'Refren', 'Bridge', 'Solo', 'Outro'];
    return `<div class="studio-inline-bar">
      <span style="font-weight:700;">Znacznik sekcji (takt):</span>
      <input id="studioInlineInput" type="text" class="studio-insp-input" style="width:160px;" value="${escapeHtml(val)}" placeholder="np. Refren" data-action="inline-prompt-input">
      ${presets.map(p => `<button type="button" class="studio-btn sm" data-action="inline-prompt-preset" data-value="${escapeHtml(p)}">${escapeHtml(p)}</button>`).join('')}
      <button type="button" class="studio-btn sm primary" data-action="inline-prompt-submit">Zastosuj</button>
      <button type="button" class="studio-btn sm" data-action="inline-prompt-cancel">Anuluj</button>
    </div>`;
  }
  if (type === 'repeatClose') {
    return `<div class="studio-inline-bar">
      <span style="font-weight:700;">Liczba powtórzeń (:||):</span>
      ${[0, 2, 3, 4, 6, 8].map(n =>
        `<button type="button" class="studio-btn sm" data-action="inline-prompt-preset" data-value="${n}">${n === 0 ? 'Wyłącz' : `×${n}`}</button>`
      ).join('')}
      <button type="button" class="studio-btn sm" data-action="inline-prompt-cancel">Zamknij</button>
    </div>`;
  }
  if (type === 'transpose') {
    return `<div class="studio-inline-bar">
      <span style="font-weight:700;">Transpozycja zaznaczenia (półtony):</span>
      <input id="studioInlineInput" type="number" class="studio-insp-input" style="width:78px;" min="-24" max="24" value="${escapeHtml(val || '0')}" data-action="inline-prompt-input">
      ${[-12, -2, -1, 1, 2, 12].map(n =>
        `<button type="button" class="studio-btn sm" data-action="inline-prompt-preset" data-value="${n}">${n > 0 ? `+${n}` : n}</button>`
      ).join('')}
      <button type="button" class="studio-btn sm primary" data-action="inline-prompt-submit">Transponuj</button>
      <button type="button" class="studio-btn sm" data-action="inline-prompt-cancel">Anuluj</button>
    </div>`;
  }
  const titleMap = { text: 'Tekst nad uderzeniem (TXT):', chord: 'Symbol akordu ([A]):' };
  const placeholder = type === 'chord' ? 'np. Em7, G/B' : 'np. Palm mute, Wejście';
  return `<div class="studio-inline-bar">
    <span style="font-weight:700;">${escapeHtml(titleMap[type] || 'Wartość:')}</span>
    <input id="studioInlineInput" type="text" class="studio-insp-input" style="width:200px;" value="${escapeHtml(val)}" placeholder="${escapeHtml(placeholder)}" data-action="inline-prompt-input">
    <button type="button" class="studio-btn sm primary" data-action="inline-prompt-submit">Zastosuj</button>
    <button type="button" class="studio-btn sm" data-action="inline-prompt-preset" data-value="">Wyczyść</button>
    <button type="button" class="studio-btn sm" data-action="inline-prompt-cancel">Anuluj</button>
  </div>`;
}

/**
 * Renderuje zwartą paletę kompozytora w prawym panelu (kafelki symboli + bogate dymki hover).
 */
export function renderStudioPaletteHtml(ctx = {}) {
  const isPro = ctx.uiMode === 'pro';
  const track = ctx.track;
  const beat = ctx.beat;
  const note = ctx.note || (!beat?.isRest && beat?.notes?.[0]) || null;
  const measure = ctx.measure;
  const isDrum = track?.instrumentType === 'drums' || Boolean(track?.isPercussion);
  const clipCount = ctx.clipboard?.barCount || 0;
  const beatClipCount = ctx.clipboard?.kind === 'beats' ? (ctx.clipboard.beatCount || 0) : 0;
  const pasteLabel = beatClipCount > 0 ? `📥${beatClipCount}B` : (clipCount > 0 ? `📥${clipCount}` : '📥');
  const pasteActive = beatClipCount > 0 || clipCount > 0;
  const clipScope = ctx.clipboardScope === 'all' ? 'all' : 'track';
  const pasteMode = ctx.clipboardPasteMode === 'insert' ? 'insert' : 'overwrite';
  let selLabel = `T.${(ctx.cursor?.measureIdx ?? 0) + 1}`;
  if (ctx.beatSelection && ctx.beatSelection.start && ctx.beatSelection.end) {
    const bSel = ctx.beatSelection;
    if (bSel.kind === 'beats') {
      selLabel = (bSel.startMeasure === bSel.endMeasure)
        ? `T.${bSel.startMeasure + 1} B.${bSel.start.beatIdx + 1}–${bSel.end.beatIdx + 1}`
        : `T.${bSel.startMeasure + 1}:${bSel.start.beatIdx + 1}–T.${bSel.endMeasure + 1}:${bSel.end.beatIdx + 1}`;
    } else {
      selLabel = `T.${bSel.startMeasure + 1}–${bSel.endMeasure + 1}`;
    }
  } else if (ctx.selectionStart !== null && ctx.selectionStart !== undefined) {
    selLabel = `T.${Math.min(ctx.selectionStart, ctx.selectionEnd ?? ctx.selectionStart) + 1}–${Math.max(ctx.selectionStart, ctx.selectionEnd ?? ctx.selectionStart) + 1}`;
  }

  const allDurs = [
    { d: -2, lbl: '1/1',  name: 'Cała nuta (1/1)',            desc: 'Ustawia długość uderzenia na całą nutę (4 ćwierćnuty).',    k: 'Alt+1' },
    { d: -1, lbl: '1/2',  name: 'Półnuta (1/2)',              desc: 'Ustawia długość uderzenia na półnutę (2 ćwierćnuty).',      k: 'Alt+2' },
    { d: 0,  lbl: '1/4',  name: 'Ćwierćnuta (1/4)',           desc: 'Ustawia długość uderzenia na ćwierćnutę.',                  k: 'Alt+3' },
    { d: 1,  lbl: '1/8',  name: 'Ósemka (1/8)',               desc: 'Ustawia długość uderzenia na ósemkę.',                      k: 'Alt+4' },
    { d: 2,  lbl: '1/16', name: 'Szesnastka (1/16)',          desc: 'Ustawia długość uderzenia na szesnastkę.',                  k: 'Alt+5' },
    { d: 3,  lbl: '1/32', name: 'Trzydziestodwójka (1/32)',   desc: 'Ustawia długość uderzenia na trzydziestodwójkę.',           k: 'Alt+6' },
    { d: 4,  lbl: '1/64', name: 'Sześćdziesięcioczwórka',     desc: 'Ustawia długość uderzenia na 1/64.',                        k: 'Alt+7' }
  ];
  const durs = isPro ? allDurs : allDurs.slice(0, 5);

  const bendOptions = Object.values(BEND_PRESETS).map(bp =>
    `<option value="${escapeHtml(bp.id)}" ${note?.bendPreset === bp.id ? 'selected' : ''}>↗ ${escapeHtml(bp.label)}</option>`
  ).join('');

  const whammyOptions = Object.values(WHAMMY_PRESETS).map(wp =>
    `<option value="${escapeHtml(wp.id)}" ${note?.whammyPreset === wp.id ? 'selected' : ''}>↘W ${escapeHtml(wp.label)}</option>`
  ).join('');

  const dynOptions = DYNAMIC_LEVELS.map(dl =>
    `<option value="${dl.value}" ${(beat?.dynamic ?? 6) === dl.value ? 'selected' : ''}>${escapeHtml(dl.label)}</option>`
  ).join('');

  const bendSelectHtml = `<div class="studio-rail-select-row">
    <span style="font-size:10.5px;color:var(--text-muted);font-weight:700;">Krzywa</span>
    <select class="studio-pal-select" data-action="palette-bend" data-tip-name="Krzywa podciągnięcia (Bend)" data-tip-desc="Wybierz wysokość (+½, +1, +1½) lub bend + release / pre-bend.">
      <option value="clear" ${!note?.bendPreset ? 'selected' : ''}>↗ Krzywa…</option>
      ${bendOptions}
    </select>
  </div>`;

  const whammySelectHtml = `<div class="studio-rail-select-row">
    <span style="font-size:10.5px;color:var(--text-muted);font-weight:700;">Wajcha</span>
    <select class="studio-pal-select" data-action="palette-whammy" data-tip-name="Wajcha (Tremolo Bar / Whammy)" data-tip-desc="Obniżanie (Dive/Dip/Bomb) lub podnoszenie dźwięku mostkiem tremolo. Rysuje krzywą w/bar jak na Songsterr i gra w WebAudio.">
      <option value="clear" ${!note?.whammyPreset ? 'selected' : ''}>↘ Wajcha…</option>
      ${whammyOptions}
    </select>
  </div>`;

  const techGroupsHtml = isPro
    ? `<!-- 2. Artykulacja, tłumienia i flażolety -->
      <div class="studio-rail-group">
        <div class="studio-rail-head"><span>Artykulacja</span></div>
        <div class="studio-rail-grid">
          ${palBtn('effect:ghostNote', '(0)', Boolean(note?.ghostNote), 'Ghost Note', 'Cichsza nuta widmowa zapisana w nawiasie.', 'G')}
          ${palBtn('effect:accented', '>', Boolean(note?.accented), 'Akcent (>)', 'Wyraźnie podkreślone uderzenie kostką lub palcem.', '')}
          ${palBtn('effect:heavyAccented', '^', Boolean(note?.heavyAccented), 'Mocny akcent (^)', 'Silny akcent Marcato o większej dynamice.', '')}
          ${palBtn('effect:staccato', '•', Boolean(note?.staccato), 'Staccato', 'Szybko skrócona, urywana artykulacja dźwięku.', '')}
          ${palBtn('effect:letRing', 'LR', Boolean(note?.letRing), 'Let Ring', 'Pozwala strunom swobodnie wybrzmiewać (arpeggio / akordy).', '')}
          ${palBtn('effect:palmMute', 'PM', Boolean(note?.palmMute), 'Palm Mute (P.M.)', 'Tłumienie strun nasadą dłoni przy mostku.', 'P')}
          ${palBtn('effect:deadNote', 'X', Boolean(note?.deadNote), 'Dead Note (-X-)', 'Tłumiona nuta perkusyjna bez określonej wysokości.', 'X')}
          ${palBtn('harmonic:1', '◇', note?.harmonicType === 1, 'Flażolet naturalny', 'Krystaliczny flażolet nad progami 5, 7, 12.', 'O')}
          ${palBtn('harmonic:2', '◆', note?.harmonicType === 2, 'Flażolet sztuczny / Pinch', 'Wyciskany flażolet (pinch / artificial harmonic).', '')}
        </div>
      </div>

      <!-- 3. Legato, Slide, Bend, Kostkowanie -->
      <div class="studio-rail-group">
        <div class="studio-rail-head"><span>Legato &amp; Bend</span></div>
        <div class="studio-rail-grid">
          ${palBtn('effect:hammerOn', 'H/P', Boolean(note?.hammerOn), 'Hammer-On / Pull-Off', 'Płynne przejście legato bez ponownego szarpania.', 'H')}
          ${palBtn('slide:shift', '/', note?.slideType === 'shift', 'Shift Slide', 'Przesunięcie palcem do następnego dźwięku.', 'S')}
          ${palBtn('slide:legato', '⌒/', note?.slideType === 'legato', 'Legato Slide', 'Połączony ślizg legato ze wspólnym atakiem.', '')}
          ${palBtn('slide:in_below', '/0', note?.slideType === 'in_below', 'Slide-in od dołu', 'Wjazd ślizgiem z niższych progów do docelowej nuty.', '')}
          ${palBtn('slide:out_down', '0\\', note?.slideType === 'out_down', 'Slide-out w dół', 'Zjazd ślizgiem z nuty w stronę gryfu/główki.', '')}
          ${palBtn('bend:full', '↗', Boolean(note?.bendPreset), 'Bend +1 ton', 'Podciągnięcie struny w górę o cały ton.', 'B')}
          ${palBtn('whammy:dive_full', '↘W', Boolean(note?.whammyPreset), 'Wajcha / Tremolo Bar', 'Obniżenie lub podniesienie dźwięku mostkiem tremolo (Dive / Dip / Scoop).', 'W')}
          ${palBtn('effect:vibrato', '~', Boolean(note?.vibrato), 'Vibrato', 'Delikatna modulacja wysokości dźwięku na progu.', 'V')}
          ${palBtn('effect:wideVibrato', 'w~', Boolean(note?.wideVibrato), 'Szerokie Vibrato', 'Mocne rockowe vibrato o większej amplitudzie.', '')}
          ${palBtn('pick:1', '⊓', beat?.pickStroke === 1, 'Kostkowanie w dół', 'Oznacza ruch kostki z góry na dół.', '')}
          ${palBtn('pick:2', '∨', beat?.pickStroke === 2, 'Kostkowanie w górę', 'Oznacza ruch kostki z dołu do góry.', '')}
          ${palBtn('brush:down', '↓↑', Boolean(beat?.brushStroke), 'Rozbicie akordu', 'Płynne przejechanie po strunach akordu (strum / arpeggiate).', '')}
          ${palBtn('effect:tap', 'T', Boolean(note?.tap), 'Tapping', 'Uderzenie progu palcem prawej ręki.', '')}
          ${palBtn('effect:graceNote', '♪', Boolean(note?.graceNote), 'Przednutka (Grace Note)', 'Krótka ozdobna nuta poprzedzająca właściwy dźwięk.', '')}
          ${palBtn('effect:tremoloPicking', '///', Boolean(note?.tremoloPicking), 'Tremolo Picking', 'Szybkie repetycyjne kostkowanie jednej nuty.', '')}
          ${palBtn('effect:trill', 'tr', Boolean(note?.trill), 'Trill', 'Szybka naprzemienna oscylacja między dwoma progami.', '')}
        </div>
        ${bendSelectHtml}
        ${whammySelectHtml}
      </div>`
    : `<!-- 2. Techniki mające wpływ na brzmienie (tryb uproszczony — bez wajchy) -->
      <div class="studio-rail-group">
        <div class="studio-rail-head"><span>Techniki (brzmienie)</span></div>
        <div class="studio-rail-grid">
          ${palBtn('effect:tied', '⌣+', Boolean(note?.tied), 'Przedłuż ostatni dźwięk (Tie)', 'Kliknij puste pole po nucie i wciśnij ten przycisk (lub L), aby przedłużyć wybrzmiewanie poprzedniego dźwięku.', 'L')}
          ${palBtn('effect:hammerOn', 'H/P', Boolean(note?.hammerOn), 'Legato (Hammer-On / Pull-Off)', 'Płynne przejście legato bez ponownego szarpania.', 'H')}
          ${palBtn('bend:full', '↗', Boolean(note?.bendPreset), 'Bend +1 ton', 'Podciągnięcie struny w górę o cały ton.', 'B')}
          ${palBtn('effect:vibrato', '~', Boolean(note?.vibrato), 'Vibrato', 'Delikatna modulacja wysokości dźwięku na progu.', 'V')}
          ${palBtn('effect:palmMute', 'PM', Boolean(note?.palmMute), 'Palm Mute (P.M.)', 'Tłumienie strun nasadą dłoni przy mostku.', 'P')}
          ${palBtn('slide:shift', '/', note?.slideType === 'shift', 'Shift Slide', 'Przesunięcie palcem do następnego dźwięku.', 'S')}
          ${palBtn('effect:letRing', 'LR', Boolean(note?.letRing), 'Let Ring', 'Pozwala strunom swobodnie wybrzmiewać.', '')}
          ${palBtn('effect:deadNote', 'X', Boolean(note?.deadNote), 'Dead Note (-X-)', 'Tłumiona nuta perkusyjna.', 'X')}
          ${palBtn('effect:ghostNote', '(0)', Boolean(note?.ghostNote), 'Ghost Note', 'Cichsza nuta widmowa.', 'G')}
          ${palBtn('effect:accented', '>', Boolean(note?.accented), 'Akcent (>)', 'Wyraźnie podkreślone uderzenie.', '')}
          ${palBtn('effect:staccato', '•', Boolean(note?.staccato), 'Staccato', 'Krótka, urywana artykulacja dźwięku.', '')}
          ${palBtn('harmonic:1', '◇', note?.harmonicType === 1, 'Flażolet naturalny', 'Krystaliczny flażolet nad progami 5, 7, 12.', 'O')}
          ${palBtn('harmonic:2', '◆', note?.harmonicType === 2, 'Flażolet sztuczny / Pinch', 'Wyciskany flażolet (pinch).', '')}
          ${palBtn('effect:tap', 'T', Boolean(note?.tap), 'Tapping', 'Uderzenie progu palcem prawej ręki.', '')}
        </div>
        ${bendSelectHtml}
      </div>`;

  const effectiveDur = (!isDrum && beat?.isRest && Number.isInteger(ctx.activeDuration))
    ? ctx.activeDuration
    : beat?.duration;

  return `<div class="studio-rail-palette">
    ${ctx.includeInlinePrompt !== false ? renderInlinePromptBar(ctx.inlinePrompt) : ''}

    <!-- 1. Rytm, pauzy, łuki, dynamika -->
    <div class="studio-rail-group">
      <div class="studio-rail-head">
        <span>Rytm</span>
        <span style="color:var(--accent);">${escapeHtml(selLabel)}</span>
      </div>
      <div class="studio-rail-grid">
        ${durs.map(x => palBtn(`dur:${x.d}`, x.lbl, !isDrum && effectiveDur === x.d, x.name, x.desc, x.k)).join('')}
        ${palBtn('dot:1', '|.', !isDrum && beat?.dots === 1, 'Kropka rytmiczna', 'Wydłuża wartość uderzenia o połowę (×1.5).', '.')}
        ${isPro ? palBtn('dot:2', '|..', !isDrum && beat?.dots === 2, 'Podwójna kropka', 'Wydłuża wartość uderzenia o 75% (×1.75).', '') : ''}
        ${palBtn('rest', '𝄽', !isDrum && Boolean(beat?.isRest), 'Pauza', 'Zamienia zaznaczone uderzenie na pauzę (lub z powrotem).', 'R / Ins')}
        ${palBtn('edit:insertBeat', '+B', false, 'Dodaj uderzenie', 'Wstawia nową pauzę bezpośrednio po kursorze.', '')}
        ${palBtn('tuplet:3:2', '3:2', !isDrum && beat?.tupletNumerator === 3, 'Triola (3:2)', 'Dzieli grupę rytmiczną na 3 równe uderzenia zamiast 2.', 'Alt+T')}
        ${isPro ? palBtn('tuplet:5:4', '5:4', !isDrum && beat?.tupletNumerator === 5, 'Kwintola (5:4)', 'Grupuje 5 uderzeń w czasie 4.', '') : ''}
        ${palBtn('tuplet:6:4', '6:4', !isDrum && beat?.tupletNumerator === 6, 'Sekstola (6:4)', 'Grupuje 6 uderzeń w czasie 4 (np. sweep picking).', '')}
        ${palBtn('effect:tied', '⌣+', Boolean(note?.tied), 'Przedłuż dźwięk (Tie)', 'Kliknij puste pole po nucie i wciśnij ten przycisk (lub L), aby przedłużyć wybrzmiewanie poprzedniego dźwięku.', 'L')}
        ${isPro ? palBtn('beat:fermata', '𝄐', Boolean(beat?.fermata), 'Fermata', 'Chwilowe zatrzymanie / wydłużenie wybrzmienia taktu.', '') : ''}
      </div>
      <div class="studio-rail-select-row">
        <span style="font-size:10.5px;color:var(--text-muted);font-weight:700;">Dynamika</span>
        <select class="studio-pal-select" data-action="palette-dynamic" data-tip-name="Dynamika uderzenia" data-tip-desc="Głośność / akcentacja MIDI bieżącego uderzenia (pp – fff)." ${isDrum ? 'disabled' : ''}>
          ${dynOptions}
        </select>
      </div>
    </div>

    ${techGroupsHtml}

    <!-- Struktura utworu -->
    <div class="studio-rail-group">
      <div class="studio-rail-head"><span>Struktura</span></div>
      <div class="studio-rail-grid">
        ${isPro ? palBtn('struct:repeatOpen', '🔁|:', Boolean(measure?.repeatOpen), 'Początek pętli / repetycji', 'Oznacza początek powtarzanego fragmentu partytury (||:).', '') : ''}
        ${isPro ? palBtn('prompt:repeatClose', measure?.repeatClose >= 2 ? `🔁×${measure.repeatClose}` : ':||🔁', (measure?.repeatClose || 0) >= 2, 'Koniec pętli / liczba powtórzeń', 'Zamyka repetycję (:||) i ustawia liczbę powtórzeń (np. ×2, ×4).', '') : ''}
        ${isPro ? palBtn('struct:volta:1', '1.', Boolean((measure?.voltaMask || 0) & 1), 'Volta 1.', 'Pierwsze zakończenie przed powtórzeniem repetycji.', '') : ''}
        ${isPro ? palBtn('struct:volta:2', '2.', Boolean((measure?.voltaMask || 0) & 2), 'Volta 2.', 'Drugie zakończenie grane przy kolejnym przejściu.', '') : ''}
        ${palBtn('struct:addBefore', '+▬', false, 'Dodaj takt przed', 'Wstawia nowy pusty takt przed bieżącym taktem.', '')}
        ${palBtn('struct:addAfter', '▬+', false, 'Dodaj takt po', 'Dopisuje nowy pusty takt za bieżącym taktem.', '')}
        ${palBtn('prompt:section', '§', Boolean(measure?.sectionMarker), measure?.sectionMarker ? `Sekcja: ${measure.sectionMarker}` : 'Znacznik sekcji', 'Nadaje taktom etykietę sekcji (Intro, Riff, Zwrotka, Refren).', 'M')}
        ${isPro ? palBtn('prompt:text', 'Aa', Boolean(beat?.text), beat?.text ? `Tekst: ${beat.text}` : 'Tekst / Adnotacja', 'Wpisuje dowolną wskazówkę wykonawczą nad uderzeniem.', 'T') : ''}
        ${palBtn('prompt:chord', '[C]', Boolean(beat?.chordSymbol), beat?.chordSymbol ? `Akord: ${beat.chordSymbol}` : 'Symbol akordu', 'Umieszcza nazwę akordu (np. Em, G5, Cmaj7) nad uderzeniem.', '')}
      </div>
    </div>

    <!-- Przekształcenia, Schowek i Kasowanie -->
    <div class="studio-rail-group">
      <div class="studio-rail-head"><span>${isPro ? 'Przekształcenia &amp; Schowek' : 'Edycja &amp; Schowek'}</span></div>
      <div class="studio-rail-grid">
        ${palBtn('transform:strUp', 'Str↑', false, 'Wyższa struna', 'Przenosi nutę lub cały zaznaczony fragment na sąsiednią cieńszą strunę bez zmiany wysokości MIDI.', 'Alt+↑')}
        ${palBtn('transform:strDown', 'Str↓', false, 'Niższa struna', 'Przenosi nutę lub cały zaznaczony fragment na sąsiednią grubszą strunę bez zmiany wysokości MIDI.', 'Alt+↓')}
        ${palBtn('transform:semiDown', '-½', false, 'Pół tonu w dół', 'Obniża pojedynczą nutę lub całe zaznaczenie na TAB o 1 półton (-1 próg).', 'Shift+←')}
        ${palBtn('transform:semiUp', '+½', false, 'Pół tonu w górę', 'Podnosi pojedynczą nutę lub całe zaznaczenie na TAB o 1 półton (+1 próg).', 'Shift+→')}
        ${palBtn('transform:octDown', '-8', false, 'Oktawa w dół', 'Obniża nutę lub cały zaznaczony fragment o 12 półtonów.', 'Shift+↓')}
        ${palBtn('transform:octUp', '+8', false, 'Oktawa w górę', 'Podnosi nutę lub cały zaznaczony fragment o 12 półtonów.', 'Shift+↑')}
        ${isPro ? palBtn('transform:moveBeatLeft', '←B', false, 'Przesuń Beat w lewo', 'Zamienia bieżące uderzenie miejscami z poprzednim.', '') : ''}
        ${isPro ? palBtn('transform:moveBeatRight', 'B→', false, 'Przesuń Beat w prawo', 'Zamienia bieżące uderzenie miejscami z następnym.', '') : ''}
        ${palBtn('prompt:transpose', 'A⇅', false, 'Transpozycja zaznaczenia', 'Przesuwa wszystkie dźwięki w zaznaczonym fragmencie lub taktach o zadaną liczbę półtonów.', '')}
        ${palBtn('clip:cut', '✂', false, 'Wytnij fragment / takty', 'Kopiuje zaznaczony fragment uderzeń (lub takty) do schowka i czyści zawartość.', 'Ctrl+X')}
        ${palBtn('clip:copy', '📋', false, 'Kopiuj fragment / takty', 'Kopiuje zaznaczone uderzenia na TAB lub wybrany zakres taktów.', 'Ctrl+C')}
        ${palBtn('clip:paste', pasteLabel, pasteActive, 'Wklej w kliknięte miejsce', ctx.clipboard?.label ? `Wkleja ze schowka: ${ctx.clipboard.label} (${beatClipCount || clipCount}) od klikniętego taktu/uderzenia.` : 'Wkleja skopiowany fragment uderzeń od klikniętego taktu/uderzenia (lub pełne takty).', 'Ctrl+V')}
        ${palBtn('clip:dup1', '×1', false, 'Duplikuj (×1)', 'Powiela bieżący takt bezpośrednio po nim.', 'D')}
        ${isPro ? palBtn('clip:dup2', '×2', false, 'Duplikuj ×2', 'Powiela wybrany takt lub zakres dwukrotnie.', '') : ''}
        ${isPro ? palBtn('clip:dup4', '×4', false, 'Duplikuj ×4', 'Powiela wybrany takt lub zakres czterokrotnie.', '') : ''}
        ${palBtn('del:note', 'N🗑', false, 'Usuń nutę', 'Kasuje pojedynczą nutę pod pomarańczowym kursorem.', 'Del', 'danger')}
        ${palBtn('del:beat', 'B🗑', false, 'Usuń uderzenie', 'Usuwa całe uderzenie (Beat) z taktu.', 'Ctrl+Del', 'danger')}
        ${palBtn('del:barClear', 'T🗑', false, 'Wyczyść takt', 'Zamienia wszystkie uderzenia bieżącego taktu na pauzy.', 'Shift+Del', 'danger')}
        ${palBtn('del:barRemove', '−T', false, 'Usuń takt z utworu', 'Trwale usuwa bieżący takt ze wszystkich ścieżek.', 'Ctrl+Shift+Del', 'danger')}
      </div>
      ${isPro ? `<div class="studio-rail-grid-4" style="margin-top:2px;">
        <button type="button" class="studio-pal-btn ${clipScope === 'track' ? 'active' : ''}" data-action="set-clip-scope" data-scope="track" data-tip-name="Zakres: 1 ścieżka" data-tip-desc="Kopiuj i wklejaj tylko aktywną ścieżkę.">1 Śc.</button>
        <button type="button" class="studio-pal-btn ${clipScope === 'all' ? 'active' : ''}" data-action="set-clip-scope" data-scope="all" data-tip-name="Zakres: Wszystkie ścieżki" data-tip-desc="Kopiuj i wklejaj pełny układ całego zespołu.">Wsz.</button>
        <button type="button" class="studio-pal-btn ${pasteMode === 'overwrite' ? 'active' : ''}" data-action="set-paste-mode" data-mode="overwrite" data-tip-name="Tryb wklejania: Nadpisz" data-tip-desc="Zastępuje istniejące takty zawartością ze schowka.">Nadp.</button>
        <button type="button" class="studio-pal-btn ${pasteMode === 'insert' ? 'active' : ''}" data-action="set-paste-mode" data-mode="insert" data-tip-name="Tryb wklejania: Wstaw nowe" data-tip-desc="Wstawia nowe takty po bieżącym takcie.">Wstaw</button>
      </div>` : ''}
    </div>
  </div>`;
}

export function renderCommandPaletteHtml({ open = false, query = '', selectedIdx = 0, results = null } = {}) {
  if (!open) return '';
  const items = Array.isArray(results) ? results : filterStudioCommands(query);
  const clampedIdx = Math.max(0, Math.min(Math.max(0, items.length - 1), selectedIdx));
  const listHtml = items.length === 0
    ? `<div style="padding:18px;text-align:center;color:var(--text-muted);font-size:12.5px;">Brak pasujących akcji dla „${escapeHtml(query)}”.</div>`
    : items.slice(0, 14).map((cmd, i) => `
      <button type="button" class="studio-cmdk-item ${i === clampedIdx ? 'selected' : ''}" data-action="cmdk-run" data-cmd="${escapeHtml(cmd.id)}">
        <div style="display:flex;flex-direction:column;align-items:flex-start;gap:2px;">
          <span style="font-weight:700;font-size:12.5px;">${escapeHtml(cmd.label)}</span>
          <span style="font-size:10.5px;color:var(--text-muted);">${escapeHtml(cmd.category)}</span>
        </div>
        ${cmd.shortcut ? `<kbd class="studio-cmdk-kbd">${escapeHtml(cmd.shortcut)}</kbd>` : ''}
      </button>
    `).join('');

  return `<div class="studio-cmdk-backdrop" data-action="cmdk-backdrop">
    <div class="studio-cmdk-dialog">
      <div class="studio-cmdk-head">
        <span>🔍</span>
        <input id="studioCmdkInput" type="text" class="studio-cmdk-input" value="${escapeHtml(query)}" placeholder="Szukaj akcji Studia TAB (np. Palm Mute, Bend, Triola, Duplikuj)..." data-action="cmdk-input">
        <button type="button" class="studio-btn sm" data-action="cmdk-close">Esc</button>
      </div>
      <div class="studio-cmdk-list">${listHtml}</div>
    </div>
  </div>`;
}
