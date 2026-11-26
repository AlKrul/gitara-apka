/**
 * Wspólny moduł lokalnej biblioteki użytkownika (Sweep, Progresje, TAB-y, Studio TAB, Sesje)
 * oraz wymiany plików (.gitara-*.json i paczki .gitara-pack.json).
 * Działa zarówno z backendem HTTP (/api/library/* -> folder user_library/) jak i z cache localStorage.
 */

export const LIBRARY_FORMAT = 'apka-gitara-library-v1';
export const LIBRARY_CATEGORIES = Object.freeze(['sweeps', 'progressions', 'tabs', 'studio', 'sessions']);

const CATEGORY_PREFIX = {
  sweeps: 'swp',
  progressions: 'prg',
  tabs: 'tab',
  studio: 'stu',
  sessions: 'ses'
};

const POLISH_CHAR_MAP = {
  'ą': 'a', 'Ą': 'A',
  'ć': 'c', 'Ć': 'C',
  'ę': 'e', 'Ę': 'E',
  'ł': 'l', 'Ł': 'L',
  'ń': 'n', 'Ń': 'N',
  'ó': 'o', 'Ó': 'O',
  'ś': 's', 'Ś': 'S',
  'ź': 'z', 'Ź': 'Z',
  'ż': 'z', 'Ż': 'Z'
};

function asciiTransliterate(str) {
  return String(str || '')
    .split('')
    .map(ch => (POLISH_CHAR_MAP[ch] !== undefined ? POLISH_CHAR_MAP[ch] : ch))
    .join('')
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '');
}

/**
 * Normalizuje ID elementu do bezpiecznego formatu [A-Za-z0-9_-] (dł. 1..80).
 */
export function sanitizeItemId(raw, prefix = 'item') {
  const cleaned = asciiTransliterate(raw)
    .replace(/[^A-Za-z0-9_-]+/g, '_')
    .replace(/^_+|_+$/g, '')
    .slice(0, 80);
  if (cleaned && /^[A-Za-z0-9_-]+$/.test(cleaned)) {
    return cleaned;
  }
  const safePrefix = asciiTransliterate(prefix).replace(/[^A-Za-z0-9_-]+/g, '') || 'item';
  const rand = Math.floor(Math.random() * 9000 + 1000);
  return `${safePrefix}_${Date.now()}_${rand}`.slice(0, 80);
}

/**
 * Buduje bezpieczną nazwę pliku do pobrania z przeglądarki.
 */
export function makeSafeFilename(name, ext = 'json') {
  const base = asciiTransliterate(name)
    .toLowerCase()
    .replace(/[^a-z0-9_-]+/g, '_')
    .replace(/^_+|_+$/g, '')
    .slice(0, 64) || 'projekt';
  const cleanExt = String(ext || 'json').replace(/^\.+/, '') || 'json';
  return `${base}.${cleanExt}`;
}

function isPlainObject(v) {
  return Boolean(v && typeof v === 'object' && !Array.isArray(v));
}

function emptyStoreBuckets() {
  return {
    sweeps: [],
    progressions: [],
    tabs: [],
    studio: [],
    sessions: []
  };
}

function validateAndNormalizeItem(category, raw, defaultSource = 'local') {
  if (!LIBRARY_CATEGORIES.includes(category)) {
    throw new Error(`Nieobsługiwana kategoria biblioteki: ${category}`);
  }
  if (!isPlainObject(raw)) {
    throw new Error('Element biblioteki musi być poprawnym obiektem.');
  }

  const now = Date.now();
  const rawTitle = raw.name || raw.title || (raw.song && raw.song.title) || 'Bez nazwy';
  const cleanId = raw.id
    ? sanitizeItemId(raw.id, CATEGORY_PREFIX[category])
    : sanitizeItemId(`${CATEGORY_PREFIX[category]}_${now}_${Math.floor(Math.random() * 1000)}`, CATEGORY_PREFIX[category]);
  const updatedAt = Number.isFinite(Number(raw.updatedAt)) && Number(raw.updatedAt) > 0
    ? Number(raw.updatedAt)
    : (Number.isFinite(Number(raw.createdAt)) && Number(raw.createdAt) > 0 ? Number(raw.createdAt) : now);
  const source = raw.source === 'imported' ? 'imported' : (raw.source || defaultSource);

  if (category === 'sweeps') {
    if (!Array.isArray(raw.items) || raw.items.length === 0) {
      throw new Error('Układ Sweep musi zawierać co najmniej 1 segment w polu items.');
    }
    const name = String(raw.name || raw.title || 'Własny układ Sweep').trim().slice(0, 120);
    return {
      ...raw,
      id: cleanId,
      name,
      createdAt: Number(raw.createdAt) || updatedAt,
      updatedAt,
      source
    };
  }

  if (category === 'progressions') {
    const hasMeasures = Array.isArray(raw.measures) && raw.measures.length > 0;
    const hasBars = Array.isArray(raw.bars) && raw.bars.length > 0;
    if (!hasMeasures && !hasBars) {
      throw new Error('Progresja musi zawierać takty (measures lub bars).');
    }
    const title = String(raw.title || raw.name || 'Własna progresja').trim().slice(0, 120);
    return {
      ...raw,
      id: cleanId,
      title,
      name: String(raw.name || title).trim().slice(0, 120),
      createdAt: Number(raw.createdAt) || updatedAt,
      updatedAt,
      source
    };
  }

  if (category === 'tabs') {
    if (!Array.isArray(raw.recordedEvents) || raw.recordedEvents.length === 0) {
      throw new Error('Ćwiczenie TAB musi zawierać listę dźwięków (recordedEvents).');
    }
    const title = String(raw.title || raw.name || 'Własne ćwiczenie TAB').trim().slice(0, 120);
    return {
      ...raw,
      id: cleanId,
      title,
      name: String(raw.name || title).trim().slice(0, 120),
      createdAt: Number(raw.createdAt) || updatedAt,
      updatedAt,
      source
    };
  }

  if (category === 'studio') {
    const songObj = isPlainObject(raw.song) && Array.isArray(raw.song.tracks)
      ? raw.song
      : (Array.isArray(raw.tracks) ? raw : null);
    if (!songObj || !Array.isArray(songObj.tracks) || songObj.tracks.length === 0) {
      throw new Error('Projekt Studia TAB musi zawierać obiekt song ze ścieżkami.');
    }
    const title = String(raw.name || raw.title || songObj.title || 'Projekt Studio TAB').trim().slice(0, 120);
    return {
      id: cleanId,
      name: title,
      title,
      createdAt: Number(raw.createdAt) || updatedAt,
      updatedAt,
      source,
      song: songObj,
      studioPrefs: isPlainObject(raw.studioPrefs) ? raw.studioPrefs : null
    };
  }

  // sessions
  const name = String(raw.name || raw.title || rawTitle).trim().slice(0, 120);
  return {
    ...raw,
    id: cleanId,
    name,
    createdAt: Number(raw.createdAt) || updatedAt,
    updatedAt,
    source
  };
}

/**
 * Pakuje pojedynczy element biblioteki do formatu eksportu (.gitara-*.json).
 */
export function exportItemPayload(category, item) {
  const normalized = validateAndNormalizeItem(category, item, item?.source || 'local');
  return {
    format: LIBRARY_FORMAT,
    version: 1,
    kind: 'item',
    category,
    exportedAt: new Date().toISOString(),
    item: normalized
  };
}

/**
 * Pakuje wszystkie elementy z wybranych kategorii do jednej paczki wymiany (.gitara-pack.json).
 */
export function exportPackPayload(libraryData, { name = 'Paczka Apka Gitarowa', categories = LIBRARY_CATEGORIES } = {}) {
  const items = [];
  for (const cat of categories) {
    if (!LIBRARY_CATEGORIES.includes(cat)) continue;
    const list = Array.isArray(libraryData?.[cat]) ? libraryData[cat] : [];
    for (const raw of list) {
      try {
        const normalized = validateAndNormalizeItem(cat, raw, raw?.source || 'local');
        items.push({ category: cat, item: normalized });
      } catch (_) {}
    }
  }
  return {
    format: LIBRARY_FORMAT,
    version: 1,
    kind: 'pack',
    name: String(name || 'Paczka Apka Gitarowa'),
    exportedAt: new Date().toISOString(),
    items
  };
}

/**
 * Parsuje dowolny importowany JSON (pojedynczy element, paczkę lub bezpośredni plik Studio .gitara.json).
 */
export function parseImportText(jsonText, fallbackCategory = null) {
  let parsed;
  try {
    parsed = JSON.parse(String(jsonText || ''));
  } catch (_) {
    throw new Error('Niepoprawny plik JSON — nie udało się odczytać zawartości.');
  }

  if (!isPlainObject(parsed)) {
    throw new Error('Nieobsługiwany format pliku biblioteki.');
  }

  if (parsed.format === LIBRARY_FORMAT) {
    if (parsed.kind === 'pack' && Array.isArray(parsed.items)) {
      const entries = [];
      for (const entry of parsed.items) {
        if (!isPlainObject(entry) || !LIBRARY_CATEGORIES.includes(entry.category)) continue;
        try {
          const item = validateAndNormalizeItem(entry.category, entry.item, 'imported');
          entries.push({ category: entry.category, item });
        } catch (_) {}
      }
      if (entries.length === 0) {
        throw new Error('Paczka nie zawiera żadnych poprawnych elementów.');
      }
      return { kind: 'pack', entries };
    }

    if (LIBRARY_CATEGORIES.includes(parsed.category) && isPlainObject(parsed.item)) {
      const item = validateAndNormalizeItem(parsed.category, parsed.item, 'imported');
      return { kind: 'item', entries: [{ category: parsed.category, item }] };
    }
    throw new Error('Niepoprawna struktura pliku apka-gitara-library-v1.');
  }

  // Bezpośredni plik Studio TAB (.gitara.json)
  if (parsed.format === 'apka-gitara-studio' || (isPlainObject(parsed.song) && Array.isArray(parsed.song.tracks)) || Array.isArray(parsed.tracks)) {
    const item = validateAndNormalizeItem('studio', parsed, 'imported');
    return { kind: 'item', entries: [{ category: 'studio', item }] };
  }

  // Bezpośredni Sweep
  if (Array.isArray(parsed.items) && parsed.items.length > 0) {
    const item = validateAndNormalizeItem('sweeps', parsed, 'imported');
    return { kind: 'item', entries: [{ category: 'sweeps', item }] };
  }

  // Bezpośrednia Progresja
  if ((Array.isArray(parsed.measures) && parsed.measures.length > 0) || (Array.isArray(parsed.bars) && parsed.bars.length > 0)) {
    const item = validateAndNormalizeItem('progressions', parsed, 'imported');
    return { kind: 'item', entries: [{ category: 'progressions', item }] };
  }

  // Bezpośredni TAB / Ćwiczenie
  if (Array.isArray(parsed.recordedEvents) && parsed.recordedEvents.length > 0) {
    const item = validateAndNormalizeItem('tabs', parsed, 'imported');
    return { kind: 'item', entries: [{ category: 'tabs', item }] };
  }

  if (fallbackCategory && LIBRARY_CATEGORIES.includes(fallbackCategory)) {
    const item = validateAndNormalizeItem(fallbackCategory, parsed, 'imported');
    return { kind: 'item', entries: [{ category: fallbackCategory, item }] };
  }

  throw new Error('Nie rozpoznano formatu pliku (obsługiwane: Sweep, Progresje, Edytor TAB, Studio TAB i paczki).');
}

/**
 * Pobiera plik JSON z przeglądarki (bezpieczne no-op poza DOM).
 */
export function downloadJsonPayload(payload, filename = 'eksport.json') {
  if (typeof document === 'undefined' || typeof Blob === 'undefined' || typeof URL === 'undefined') {
    return false;
  }
  const json = typeof payload === 'string' ? payload : JSON.stringify(payload, null, 2);
  const blob = new Blob([json], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1500);
  return true;
}

/**
 * Magazyn lokalnej biblioteki użytkownika zsynchronizowany z /api/library/* i localStorage.
 */
export function createUserLibraryStore({
  fetchFn = undefined,
  storage = undefined,
  storageKey = 'apka_gitara_user_library_v1'
} = {}) {
  const useFetch = fetchFn !== undefined ? fetchFn : (typeof fetch === 'function' ? fetch.bind(globalThis) : null);
  const useStorage = storage !== undefined ? storage : (typeof localStorage !== 'undefined' ? localStorage : null);

  let data = emptyStoreBuckets();

  function sortBucket(cat) {
    data[cat].sort((a, b) => (Number(b.updatedAt) || 0) - (Number(a.updatedAt) || 0));
  }

  function persistStorageCache() {
    if (!useStorage || typeof useStorage.setItem !== 'function') return;
    try {
      useStorage.setItem(storageKey, JSON.stringify(data));
    } catch (_) {}
  }

  function mergeBucket(cat, incomingList) {
    if (!Array.isArray(incomingList)) return;
    const byId = new Map();
    for (const cur of data[cat]) {
      if (cur && cur.id) byId.set(cur.id, cur);
    }
    for (const raw of incomingList) {
      try {
        const norm = validateAndNormalizeItem(cat, raw, raw?.source || 'local');
        const prev = byId.get(norm.id);
        if (!prev || (Number(norm.updatedAt) || 0) >= (Number(prev.updatedAt) || 0)) {
          byId.set(norm.id, norm);
        }
      } catch (_) {}
    }
    data[cat] = Array.from(byId.values());
    sortBucket(cat);
  }

  async function load() {
    data = emptyStoreBuckets();
    if (useStorage && typeof useStorage.getItem === 'function') {
      try {
        const cached = JSON.parse(useStorage.getItem(storageKey) || 'null');
        if (isPlainObject(cached)) {
          for (const cat of LIBRARY_CATEGORIES) {
            mergeBucket(cat, cached[cat]);
          }
        }
      } catch (_) {}
    }
    if (useFetch) {
      try {
        const res = await useFetch('/api/library');
        if (res && res.ok) {
          const remote = await res.json();
          if (isPlainObject(remote)) {
            for (const cat of LIBRARY_CATEGORIES) {
              mergeBucket(cat, remote[cat]);
            }
          }
        }
      } catch (_) {}
    }
    persistStorageCache();
    return getAll();
  }

  function getItems(category) {
    if (!LIBRARY_CATEGORIES.includes(category)) return [];
    return data[category].map(it => JSON.parse(JSON.stringify(it)));
  }

  function getById(category, id) {
    if (!LIBRARY_CATEGORIES.includes(category)) return null;
    const found = data[category].find(it => it.id === id);
    return found ? JSON.parse(JSON.stringify(found)) : null;
  }

  function getAll() {
    const out = {};
    for (const cat of LIBRARY_CATEGORIES) {
      out[cat] = getItems(cat);
    }
    return out;
  }

  async function saveItem(category, rawItem) {
    const item = validateAndNormalizeItem(category, { ...rawItem, updatedAt: Date.now() }, rawItem?.source || 'local');
    const idx = data[category].findIndex(it => it.id === item.id);
    if (idx >= 0) data[category][idx] = item;
    else data[category].unshift(item);
    sortBucket(category);
    persistStorageCache();

    if (useFetch) {
      try {
        await useFetch(`/api/library/save?category=${encodeURIComponent(category)}&id=${encodeURIComponent(item.id)}`, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(item)
        });
      } catch (_) {}
    }
    return JSON.parse(JSON.stringify(item));
  }

  async function deleteItem(category, rawId) {
    if (!LIBRARY_CATEGORIES.includes(category)) return false;
    const safeId = sanitizeItemId(rawId, CATEGORY_PREFIX[category]);
    const idx = data[category].findIndex(it => it.id === safeId || it.id === rawId);
    const idToDelete = idx >= 0 ? data[category][idx].id : safeId;
    const existed = idx >= 0;
    if (idx >= 0) {
      data[category].splice(idx, 1);
      persistStorageCache();
    }
    if (useFetch) {
      try {
        const res = await useFetch(`/api/library/delete?category=${encodeURIComponent(category)}&id=${encodeURIComponent(idToDelete)}`, {
          method: 'POST'
        });
        if (res && res.ok) return true;
      } catch (_) {}
    }
    return existed;
  }

  async function importTextPermanently(jsonText, fallbackCategory = null) {
    const parsed = parseImportText(jsonText, fallbackCategory);
    const savedEntries = [];
    for (const entry of parsed.entries) {
      const saved = await saveItem(entry.category, { ...entry.item, source: 'imported' });
      savedEntries.push({ category: entry.category, item: saved });
    }
    const categories = Array.from(new Set(savedEntries.map(e => e.category)));
    return {
      kind: parsed.kind,
      entries: savedEntries,
      count: savedEntries.length,
      categories
    };
  }

  return {
    load,
    getItems,
    getById,
    getAll,
    saveItem,
    deleteItem,
    importTextPermanently
  };
}
