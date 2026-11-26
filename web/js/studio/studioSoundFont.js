/**
 * Lekki parser SoundFont2 (.sf2 / RIFF sfbk) i odtwarzacz PCM oparty o WebAudio.
 * Zero zależności zewnętrznych — działa w 100% offline dla dowolnego pliku .sf2
 * wczytanego przez użytkownika (bank melodyczny 0 oraz perkusja GM bank 128).
 */

const GEN_START_ADDR_OFFSET = 0;
const GEN_END_ADDR_OFFSET = 1;
const GEN_STARTLOOP_ADDR_OFFSET = 2;
const GEN_ENDLOOP_ADDR_OFFSET = 3;
const GEN_START_ADDR_COARSE_OFFSET = 4;
const GEN_END_ADDR_COARSE_OFFSET = 12;
const GEN_PAN = 17;
const GEN_DELAY_VOL_ENV = 33;
const GEN_ATTACK_VOL_ENV = 34;
const GEN_HOLD_VOL_ENV = 35;
const GEN_DECAY_VOL_ENV = 36;
const GEN_SUSTAIN_VOL_ENV = 37;
const GEN_RELEASE_VOL_ENV = 38;
const GEN_INSTRUMENT = 41;
const GEN_KEY_RANGE = 43;
const GEN_VEL_RANGE = 44;
const GEN_STARTLOOP_ADDR_COARSE_OFFSET = 45;
const GEN_INITIAL_ATTENUATION = 48;
const GEN_ENDLOOP_ADDR_COARSE_OFFSET = 50;
const GEN_COARSE_TUNE = 51;
const GEN_FINE_TUNE = 52;
const GEN_SAMPLE_ID = 53;
const GEN_SAMPLE_MODES = 54;
const GEN_SCALE_TUNING = 56;
const GEN_OVERRIDING_ROOT_KEY = 58;

function readFourCc(view, offset) {
  return String.fromCharCode(
    view.getUint8(offset),
    view.getUint8(offset + 1),
    view.getUint8(offset + 2),
    view.getUint8(offset + 3)
  );
}

function readFixedAscii(bytes, offset, length) {
  let end = offset;
  const max = Math.min(bytes.length, offset + length);
  while (end < max && bytes[end] !== 0) end++;
  let out = '';
  for (let i = offset; i < end; i++) {
    out += String.fromCharCode(bytes[i]);
  }
  return out.trim();
}

function timecentsToSec(tc) {
  if (!Number.isFinite(tc) || tc <= -12000) return 0.001;
  return Math.max(0.001, Math.min(30, Math.pow(2, tc / 1200)));
}

function parseGenList(view, start, count) {
  const out = [];
  for (let i = 0; i < count; i++) {
    const off = start + i * 4;
    if (off + 4 > view.byteLength) break;
    const oper = view.getUint16(off, true);
    const amountS16 = view.getInt16(off + 2, true);
    const amountU16 = view.getUint16(off + 2, true);
    const lo = view.getUint8(off + 2);
    const hi = view.getUint8(off + 3);
    out.push({ oper, amountS16, amountU16, lo, hi });
  }
  return out;
}

function applyZoneGenerators(gens, target, isPreset) {
  for (const g of gens) {
    if (g.oper === GEN_KEY_RANGE) {
      target.keyLo = Math.max(target.keyLo, g.lo);
      target.keyHi = Math.min(target.keyHi, g.hi);
      target.hasKeyRange = true;
    } else if (g.oper === GEN_VEL_RANGE) {
      target.velLo = Math.max(target.velLo, g.lo);
      target.velHi = Math.min(target.velHi, g.hi);
      target.hasVelRange = true;
    } else if (isPreset && g.oper === GEN_INSTRUMENT) {
      target.instrumentIdx = g.amountU16;
    } else if (!isPreset && g.oper === GEN_SAMPLE_ID) {
      target.sampleId = g.amountU16;
    } else {
      target.gens.set(g.oper, g.amountS16);
    }
  }
}

function buildZones(bagList, bagStart, bagEnd, genList, isPreset) {
  const linkOper = isPreset ? GEN_INSTRUMENT : GEN_SAMPLE_ID;
  const zones = [];
  let globalZone = null;

  for (let b = bagStart; b < bagEnd; b++) {
    const genStart = bagList[b].genNdx;
    const genEnd = (b + 1 < bagList.length) ? bagList[b + 1].genNdx : genList.length;
    const slice = genList.slice(genStart, Math.max(genStart, genEnd));
    const hasLink = slice.some(g => g.oper === linkOper);
    if (!hasLink) {
      if (b === bagStart && !globalZone) {
        globalZone = {
          keyLo: 0,
          keyHi: 127,
          velLo: 0,
          velHi: 127,
          gens: new Map()
        };
        applyZoneGenerators(slice, globalZone, isPreset);
      }
      continue;
    }

    const zone = {
      keyLo: globalZone ? globalZone.keyLo : 0,
      keyHi: globalZone ? globalZone.keyHi : 127,
      velLo: globalZone ? globalZone.velLo : 0,
      velHi: globalZone ? globalZone.velHi : 127,
      gens: new Map(globalZone ? globalZone.gens : undefined),
      instrumentIdx: null,
      sampleId: null
    };
    // Własny zakres key/vel nadpisuje zakres globalny strefy nadrzędnej
    const ownKey = slice.find(g => g.oper === GEN_KEY_RANGE);
    if (ownKey) { zone.keyLo = ownKey.lo; zone.keyHi = ownKey.hi; }
    const ownVel = slice.find(g => g.oper === GEN_VEL_RANGE);
    if (ownVel) { zone.velLo = ownVel.lo; zone.velHi = ownVel.hi; }

    for (const g of slice) {
      if (g.oper === GEN_KEY_RANGE || g.oper === GEN_VEL_RANGE) continue;
      if (isPreset && g.oper === GEN_INSTRUMENT) zone.instrumentIdx = g.amountU16;
      else if (!isPreset && g.oper === GEN_SAMPLE_ID) zone.sampleId = g.amountU16;
      else zone.gens.set(g.oper, g.amountS16);
    }
    zones.push(zone);
  }
  return zones;
}

/**
 * Parsuje bufor ArrayBuffer lub Uint8Array pliku SoundFont2 (.sf2 / RIFF sfbk).
 * @param {ArrayBuffer|Uint8Array} input
 */
export function parseSoundFont2(input) {
  const bytes = input instanceof Uint8Array ? input : new Uint8Array(input || 0);
  if (bytes.byteLength < 12) {
    throw new Error('Plik .sf2 jest zbyt mały lub pusty.');
  }
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const riff = readFourCc(view, 0);
  const form = readFourCc(view, 8);
  if (riff !== 'RIFF' || form !== 'sfbk') {
    throw new Error('Nieprawidłowy nagłówek RIFF/sfbk pliku .sf2.');
  }

  let name = 'SoundFont 2';
  let smplOffset = -1;
  let smplLength = 0;
  const pdtaChunks = new Map();

  function scanList(offset, endOffset) {
    let pos = offset;
    while (pos + 8 <= endOffset) {
      const id = readFourCc(view, pos);
      const size = view.getUint32(pos + 4, true);
      const dataStart = pos + 8;
      const dataEnd = Math.min(endOffset, dataStart + size);
      if (id === 'LIST' && dataStart + 4 <= dataEnd) {
        const listType = readFourCc(view, dataStart);
        if (listType === 'INFO' || listType === 'sdta' || listType === 'pdta') {
          scanList(dataStart + 4, dataEnd);
        }
      } else {
        if (id === 'INAM' && dataEnd > dataStart) {
          const decoded = readFixedAscii(bytes, dataStart, dataEnd - dataStart);
          if (decoded) name = decoded;
        } else if (id === 'smpl') {
          smplOffset = dataStart;
          smplLength = dataEnd - dataStart;
        } else if (['phdr', 'pbag', 'pgen', 'inst', 'ibag', 'igen', 'shdr'].includes(id)) {
          pdtaChunks.set(id, { start: dataStart, size: dataEnd - dataStart });
        }
      }
      pos = dataStart + size + (size & 1);
    }
  }

  scanList(12, bytes.byteLength);

  if (smplOffset < 0 || smplLength < 2) {
    throw new Error('Plik .sf2 nie zawiera bloku próbek PCM (sdta/smpl).');
  }
  for (const req of ['phdr', 'pbag', 'pgen', 'inst', 'ibag', 'igen', 'shdr']) {
    if (!pdtaChunks.has(req)) {
      throw new Error(`Brak wymaganego podbloku ${req} w pliku .sf2.`);
    }
  }

  // Konwersja smpl do Int16Array (niezależnie od wyrównania offsetu bajtowego)
  const sampleCountTotal = Math.floor(smplLength / 2);
  let samples = null;
  if (((bytes.byteOffset + smplOffset) & 1) === 0) {
    samples = new Int16Array(bytes.buffer, bytes.byteOffset + smplOffset, sampleCountTotal);
  } else {
    samples = new Int16Array(sampleCountTotal);
    for (let i = 0; i < sampleCountTotal; i++) {
      samples[i] = view.getInt16(smplOffset + i * 2, true);
    }
  }

  // Presety (phdr: 38B)
  const phdrChunk = pdtaChunks.get('phdr');
  const phdrRaw = [];
  for (let off = phdrChunk.start; off + 38 <= phdrChunk.start + phdrChunk.size; off += 38) {
    phdrRaw.push({
      name: readFixedAscii(bytes, off, 20),
      program: view.getUint16(off + 20, true),
      bank: view.getUint16(off + 22, true),
      bagNdx: view.getUint16(off + 24, true)
    });
  }

  // pbag (4B) + pgen (4B)
  const pbagChunk = pdtaChunks.get('pbag');
  const pbag = [];
  for (let off = pbagChunk.start; off + 4 <= pbagChunk.start + pbagChunk.size; off += 4) {
    pbag.push({ genNdx: view.getUint16(off, true), modNdx: view.getUint16(off + 2, true) });
  }
  const pgenChunk = pdtaChunks.get('pgen');
  const pgen = parseGenList(view, pgenChunk.start, Math.floor(pgenChunk.size / 4));

  // Instrumenty (inst: 22B) + ibag (4B) + igen (4B)
  const instChunk = pdtaChunks.get('inst');
  const instRaw = [];
  for (let off = instChunk.start; off + 22 <= instChunk.start + instChunk.size; off += 22) {
    instRaw.push({
      name: readFixedAscii(bytes, off, 20),
      bagNdx: view.getUint16(off + 20, true)
    });
  }
  const ibagChunk = pdtaChunks.get('ibag');
  const ibag = [];
  for (let off = ibagChunk.start; off + 4 <= ibagChunk.start + ibagChunk.size; off += 4) {
    ibag.push({ genNdx: view.getUint16(off, true), modNdx: view.getUint16(off + 2, true) });
  }
  const igenChunk = pdtaChunks.get('igen');
  const igen = parseGenList(view, igenChunk.start, Math.floor(igenChunk.size / 4));

  // Próbki (shdr: 46B)
  const shdrChunk = pdtaChunks.get('shdr');
  const shdrs = [];
  const shdrCount = Math.floor(shdrChunk.size / 46);
  for (let i = 0; i < Math.max(0, shdrCount - 1); i++) {
    const off = shdrChunk.start + i * 46;
    shdrs.push({
      name: readFixedAscii(bytes, off, 20),
      start: view.getUint32(off + 20, true),
      end: view.getUint32(off + 24, true),
      startLoop: view.getUint32(off + 28, true),
      endLoop: view.getUint32(off + 32, true),
      sampleRate: view.getUint32(off + 36, true) || 44100,
      originalPitch: view.getUint8(off + 40),
      pitchCorrection: view.getInt8(off + 41),
      sampleLink: view.getUint16(off + 42, true),
      sampleType: view.getUint16(off + 44, true)
    });
  }

  // Budowa stref instrumentów i presetów (z pominięciem rekordów końcowych EOI/EOP)
  const instruments = [];
  for (let i = 0; i < Math.max(0, instRaw.length - 1); i++) {
    const bagStart = instRaw[i].bagNdx;
    const bagEnd = instRaw[i + 1].bagNdx;
    instruments.push({
      name: instRaw[i].name,
      zones: buildZones(ibag, bagStart, bagEnd, igen, false)
    });
  }

  const presets = [];
  for (let i = 0; i < Math.max(0, phdrRaw.length - 1); i++) {
    const bagStart = phdrRaw[i].bagNdx;
    const bagEnd = phdrRaw[i + 1].bagNdx;
    presets.push({
      name: phdrRaw[i].name,
      bank: phdrRaw[i].bank,
      program: phdrRaw[i].program,
      zones: buildZones(pbag, bagStart, bagEnd, pgen, true)
    });
  }

  return {
    name,
    presets,
    instruments,
    samplesHeader: shdrs,
    samples
  };
}

/**
 * Znajduje pasującą strefę brzmienia dla (bank, program, midi, velocity).
 */
export function findSoundFontZone(sf2, bank = 0, program = 0, midi = 60, velocity = 100) {
  if (!sf2 || !Array.isArray(sf2.presets) || sf2.presets.length === 0) return null;
  const reqBank = Math.max(0, Math.round(bank));
  const reqProg = Math.max(0, Math.round(program));
  const m = Math.max(0, Math.min(127, Math.round(midi)));
  const vel = Math.max(0, Math.min(127, Math.round(velocity)));

  let preset = sf2.presets.find(p => p.bank === reqBank && p.program === reqProg);
  if (!preset && reqBank === 128) {
    preset = sf2.presets.find(p => p.bank === 128 && p.program === 0) || sf2.presets.find(p => p.bank === 128);
  }
  if (!preset) {
    preset = sf2.presets.find(p => p.bank === 0 && p.program === reqProg)
      || sf2.presets.find(p => p.bank === 0)
      || sf2.presets[0];
  }
  if (!preset) return null;

  for (const pZone of preset.zones) {
    if (m < pZone.keyLo || m > pZone.keyHi || vel < pZone.velLo || vel > pZone.velHi) continue;
    const inst = sf2.instruments[pZone.instrumentIdx];
    if (!inst) continue;

    for (const iZone of inst.zones) {
      if (m < iZone.keyLo || m > iZone.keyHi || vel < iZone.velLo || vel > iZone.velHi) continue;
      const shdr = sf2.samplesHeader[iZone.sampleId];
      if (!shdr || (shdr.sampleType & 0x8000)) continue;
      // W stereo bierzemy jeden kanał (lub mono), aby nie podwajać głosu bez potrzeby
      if (shdr.sampleType === 2) continue;

      const getGen = (oper, fallback = 0, addRelative = false) => {
        const iv = iZone.gens.has(oper) ? iZone.gens.get(oper) : fallback;
        if (!addRelative) return iv;
        return iv + (pZone.gens.has(oper) ? pZone.gens.get(oper) : 0);
      };

      const start = Math.max(0, shdr.start + getGen(GEN_START_ADDR_OFFSET, 0) + getGen(GEN_START_ADDR_COARSE_OFFSET, 0) * 32768);
      const end = Math.min(sf2.samples.length, shdr.end + getGen(GEN_END_ADDR_OFFSET, 0) + getGen(GEN_END_ADDR_COARSE_OFFSET, 0) * 32768);
      if (end <= start) continue;

      const loopStart = Math.max(start, shdr.startLoop + getGen(GEN_STARTLOOP_ADDR_OFFSET, 0) + getGen(GEN_STARTLOOP_ADDR_COARSE_OFFSET, 0) * 32768);
      const loopEnd = Math.min(end, shdr.endLoop + getGen(GEN_ENDLOOP_ADDR_OFFSET, 0) + getGen(GEN_ENDLOOP_ADDR_COARSE_OFFSET, 0) * 32768);

      const overrideRoot = iZone.gens.get(GEN_OVERRIDING_ROOT_KEY);
      let rootKey = (overrideRoot !== undefined && overrideRoot >= 0 && overrideRoot <= 127)
        ? overrideRoot
        : (shdr.originalPitch <= 127 ? shdr.originalPitch : 60);

      const coarseTune = getGen(GEN_COARSE_TUNE, 0, true);
      const fineTune = getGen(GEN_FINE_TUNE, 0, true) + (shdr.pitchCorrection || 0);
      const scaleTuning = iZone.gens.has(GEN_SCALE_TUNING) ? iZone.gens.get(GEN_SCALE_TUNING) : 100;
      const sampleMode = getGen(GEN_SAMPLE_MODES, 0);
      const attackTc = iZone.gens.has(GEN_ATTACK_VOL_ENV) ? iZone.gens.get(GEN_ATTACK_VOL_ENV) : -12000;
      const releaseTc = (iZone.gens.has(GEN_RELEASE_VOL_ENV) ? iZone.gens.get(GEN_RELEASE_VOL_ENV) : -3200)
        + (pZone.gens.get(GEN_RELEASE_VOL_ENV) || 0);
      const attenCb = Math.max(0, getGen(GEN_INITIAL_ATTENUATION, 0, true));
      const panRaw = Math.max(-500, Math.min(500, getGen(GEN_PAN, 0, true)));

      return {
        presetName: preset.name,
        bank: preset.bank,
        program: preset.program,
        sampleIndex: iZone.sampleId,
        sampleName: shdr.name,
        sampleRate: shdr.sampleRate || 44100,
        start,
        end,
        loopStart,
        loopEnd,
        sampleMode,
        rootKey,
        coarseTune,
        fineTuneCents: fineTune,
        scaleTuning,
        attackSec: timecentsToSec(attackTc),
        releaseSec: Math.min(1.8, Math.max(0.04, timecentsToSec(releaseTc))),
        attenuationGain: Math.pow(10, -(attenCb * 0.4) / 200),
        pan: panRaw / 500
      };
    }
  }
  return null;
}

/**
 * Tworzy odtwarzacz próbek .sf2 korzystający z WebAudio AudioContext hosta.
 */
export function createSoundFontSynth({ host = null } = {}) {
  let sf2 = null;
  let bufferCache = new Map();
  let fileName = '';

  function clear() {
    sf2 = null;
    fileName = '';
    bufferCache.clear();
  }

  function loadFromArrayBuffer(arrayBuffer, name = 'soundfont.sf2') {
    const parsed = parseSoundFont2(arrayBuffer);
    sf2 = parsed;
    fileName = name || parsed.name || 'soundfont.sf2';
    bufferCache.clear();
    return getStatus();
  }

  function getAudioBufferForZone(ctx, zone) {
    const cacheKey = `${zone.sampleIndex}:${zone.start}:${zone.end}`;
    if (bufferCache.has(cacheKey)) return bufferCache.get(cacheKey);
    const len = Math.max(1, zone.end - zone.start);
    const buf = ctx.createBuffer(1, len, zone.sampleRate);
    const ch = buf.getChannelData(0);
    const pcm = sf2.samples;
    for (let i = 0; i < len; i++) {
      ch[i] = pcm[zone.start + i] / 32768;
    }
    bufferCache.set(cacheKey, buf);
    return buf;
  }

  function playNote({
    bank = 0,
    program = 25,
    midi = 60,
    velocity = 95,
    durationSec = 0.5,
    gain = 0.85,
    whenSec = null
  } = {}) {
    if (!sf2) return false;
    const ctx = host && typeof host.ensureAudioCtx === 'function' ? host.ensureAudioCtx() : null;
    if (!ctx) return false;

    const zone = findSoundFontZone(sf2, bank, program, midi, velocity);
    if (!zone) return false;

    const t0 = Number.isFinite(whenSec) ? Math.max(ctx.currentTime, whenSec) : ctx.currentTime;
    const dur = Math.max(0.05, Number(durationSec) || 0.5);

    const src = ctx.createBufferSource();
    src.buffer = getAudioBufferForZone(ctx, zone);
    if ((zone.sampleMode === 1 || zone.sampleMode === 3) && zone.loopEnd > zone.loopStart + 8) {
      src.loop = true;
      src.loopStart = (zone.loopStart - zone.start) / zone.sampleRate;
      src.loopEnd = (zone.loopEnd - zone.start) / zone.sampleRate;
    }

    const cents = (midi - zone.rootKey) * zone.scaleTuning + zone.coarseTune * 100 + zone.fineTuneCents;
    const playbackRate = bank === 128 ? 1 : Math.pow(2, cents / 1200);
    src.playbackRate.setValueAtTime(Math.max(0.125, Math.min(8, playbackRate)), t0);

    const env = ctx.createGain();
    const peakGain = Math.max(0.01, Math.min(1.4, gain * (velocity / 110) * zone.attenuationGain));
    const atk = Math.min(dur * 0.4, Math.max(0.002, zone.attackSec));
    const rel = zone.releaseSec;

    env.gain.setValueAtTime(0.0001, t0);
    env.gain.linearRampToValueAtTime(peakGain, t0 + atk);
    env.gain.setValueAtTime(peakGain, t0 + dur);
    env.gain.exponentialRampToValueAtTime(0.0001, t0 + dur + rel);

    const outNode = (host && typeof host.getMasterAudioOut === 'function' ? host.getMasterAudioOut() : null) || ctx.destination;
    if (typeof ctx.createStereoPanner === 'function' && Math.abs(zone.pan) > 0.02) {
      const panner = ctx.createStereoPanner();
      panner.pan.setValueAtTime(zone.pan, t0);
      src.connect(env);
      env.connect(panner);
      panner.connect(outNode);
    } else {
      src.connect(env);
      env.connect(outNode);
    }

    src.start(t0);
    src.stop(t0 + dur + rel + 0.04);
    return true;
  }

  function getStatus() {
    return {
      loaded: Boolean(sf2),
      name: sf2 ? (sf2.name || fileName) : '',
      fileName,
      presetCount: sf2 ? sf2.presets.length : 0,
      sampleCount: sf2 ? sf2.samplesHeader.length : 0
    };
  }

  return {
    loadFromArrayBuffer,
    playNote,
    clear,
    isLoaded: () => Boolean(sf2),
    getStatus
  };
}
