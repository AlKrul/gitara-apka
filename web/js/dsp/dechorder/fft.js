/**
 * Iteracyjna radix-2 FFT zespolona o ustalonym rozmiarze (in-place).
 * Port z composeApp/.../dsp/Fft.kt.
 */
export class Fft {
  constructor(size) {
    if (size < 2 || (size & (size - 1)) !== 0) {
      throw new Error(`FFT size must be a power of two: ${size}`);
    }
    this.size = size;
    this.cosTable = new Float64Array(size / 2);
    this.sinTable = new Float64Array(size / 2);
    for (let i = 0; i < size / 2; i++) {
      this.cosTable[i] = Math.cos((2 * Math.PI * i) / size);
      this.sinTable[i] = Math.sin((2 * Math.PI * i) / size);
    }
    this.reversed = new Int32Array(size);
    const bits = 31 - Math.clz32(size);
    for (let i = 0; i < size; i++) {
      let x = i;
      let r = 0;
      for (let b = 0; b < bits; b++) {
        r = (r << 1) | (x & 1);
        x >>= 1;
      }
      this.reversed[i] = r;
    }
  }

  transform(re, im) {
    const size = this.size;
    const { cosTable, sinTable, reversed } = this;
    for (let i = 0; i < size; i++) {
      const j = reversed[i];
      if (j > i) {
        let t = re[i]; re[i] = re[j]; re[j] = t;
        t = im[i]; im[i] = im[j]; im[j] = t;
      }
    }
    let len = 2;
    while (len <= size) {
      const half = len / 2;
      const step = size / len;
      for (let start = 0; start < size; start += len) {
        let k = 0;
        for (let j = start; j < start + half; j++) {
          const wr = cosTable[k];
          const wi = -sinTable[k];
          const l = j + half;
          const tr = re[l] * wr - im[l] * wi;
          const ti = re[l] * wi + im[l] * wr;
          re[l] = re[j] - tr;
          im[l] = im[j] - ti;
          re[j] += tr;
          im[j] += ti;
          k += step;
        }
      }
      len *= 2;
    }
  }
}
