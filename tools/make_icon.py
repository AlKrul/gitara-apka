#!/usr/bin/env python3
"""Generate multi-resolution app/resources/apka_gitara.ico using Python stdlib."""

from __future__ import annotations

import argparse
import math
import pathlib
import struct
import zlib


def make_png(width: int, height: int, rgba_bytes: bytes) -> bytes:
    def chunk(tag: bytes, data: bytes) -> bytes:
        body = tag + data
        return struct.pack(">I", len(data)) + body + struct.pack(">I", zlib.crc32(body) & 0xFFFFFFFF)

    raw = bytearray()
    stride = width * 4
    for y in range(height):
        raw.append(0)  # filter type 0 (None)
        raw.extend(rgba_bytes[y * stride : (y + 1) * stride])

    ihdr = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    idat = zlib.compress(bytes(raw), level=9)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", idat) + chunk(b"IEND", b"")


def blend(dst: tuple[float, float, float, float], src: tuple[float, float, float, float]) -> tuple[float, float, float, float]:
    sa = src[3]
    if sa <= 0.0:
        return dst
    if sa >= 1.0:
        return src
    da = dst[3]
    out_a = sa + da * (1.0 - sa)
    if out_a <= 1e-6:
        return (0.0, 0.0, 0.0, 0.0)
    r = (src[0] * sa + dst[0] * da * (1.0 - sa)) / out_a
    g = (src[1] * sa + dst[1] * da * (1.0 - sa)) / out_a
    b = (src[2] * sa + dst[2] * da * (1.0 - sa)) / out_a
    return (r, g, b, out_a)


def smoothstep(edge0: float, edge1: float, x: float) -> float:
    if edge1 <= edge0:
        return 1.0 if x >= edge1 else 0.0
    t = max(0.0, min(1.0, (x - edge0) / (edge1 - edge0)))
    return t * t * (3.0 - 2.0 * t)


def rounded_box_sdf(px: float, py: float, half_w: float, half_h: float, radius: float) -> float:
    qx = abs(px) - (half_w - radius)
    qy = abs(py) - (half_h - radius)
    dx = max(qx, 0.0)
    dy = max(qy, 0.0)
    return math.hypot(dx, dy) + min(max(qx, qy), 0.0) - radius


def pick_sdf(nx: float, ny: float) -> float:
    """Signed-like distance to a guitar pick shape in normalized [-1, 1] space."""
    # Shift slightly upward so bottom tip balances inside tile
    y = ny - 0.03
    x = nx
    # Top rounded shoulders + tapered bottom tip
    t = max(-1.0, min(1.0, (y + 0.62) / 1.32))
    half_width = 0.60 * math.sin(max(0.0, min(1.0, 0.18 + 0.82 * (1.0 - t))) * math.pi * 0.72)
    # Use smooth envelope: wide around y=-0.22, rounded top, tip at y=0.70
    # Blend ellipse top and rounded triangle bottom
    dy_top = (y + 0.18) / 0.48
    dx_top = x / 0.62
    d_top = (math.hypot(dx_top, dy_top) - 1.0) * 0.50

    # Tapered lower wedge
    if y > -0.18:
        prog = max(0.0, min(1.0, (y + 0.18) / 0.88))
        w = 0.62 * ((1.0 - prog) ** 0.78) + 0.07 * (1.0 - prog)
        d_bot = max(abs(x) - w, y - 0.70) * 0.78
        if y > 0.56:
            d_bot = math.hypot(x, (y - 0.56) * 1.15) - 0.14
        return min(d_top, d_bot) if y < 0.02 else d_bot
    return d_top


def render_icon_rgba(size: int) -> bytes:
    """Render dark #14110F tile with #332B24 border and #F39237 guitar pick emblem."""
    out = bytearray(size * size * 4)
    aa = 1.6 / size

    bg_col = (0x14 / 255.0, 0x11 / 255.0, 0x0F / 255.0)
    border_col = (0x33 / 255.0, 0x2B / 255.0, 0x24 / 255.0)
    card_col = (0x21 / 255.0, 0x1C / 255.0, 0x18 / 255.0)

    # 2x2 supersampling per pixel for crisp icons at 16x16..256x256
    samples = (-0.25, 0.25)
    for py in range(size):
        for px in range(size):
            acc_r = acc_g = acc_b = acc_a = 0.0
            for sy in samples:
                for sx in samples:
                    nx = ((px + 0.5 + sx) / size) * 2.0 - 1.0
                    ny = ((py + 0.5 + sy) / size) * 2.0 - 1.0

                    # Outer rounded tile
                    d_tile = rounded_box_sdf(nx, ny, 0.90, 0.90, 0.26)
                    tile_alpha = 1.0 - smoothstep(-aa, aa, d_tile)
                    if tile_alpha <= 0.0:
                        continue

                    # Subtle top-to-bottom depth inside tile
                    grad_t = max(0.0, min(1.0, (ny + 0.9) / 1.8))
                    col = (
                        bg_col[0] * (1.0 - 0.15 * grad_t) + card_col[0] * (0.15 * (1.0 - grad_t)),
                        bg_col[1] * (1.0 - 0.15 * grad_t) + card_col[1] * (0.15 * (1.0 - grad_t)),
                        bg_col[2] * (1.0 - 0.15 * grad_t) + card_col[2] * (0.15 * (1.0 - grad_t)),
                        1.0,
                    )

                    # 1-pixel scaled border ring
                    border_width = max(0.045, 2.2 / size)
                    if d_tile > -border_width:
                        b_mix = smoothstep(-border_width - aa, -border_width + aa, d_tile)
                        col = (
                            col[0] * (1.0 - b_mix) + border_col[0] * b_mix,
                            col[1] * (1.0 - b_mix) + border_col[1] * b_mix,
                            col[2] * (1.0 - b_mix) + border_col[2] * b_mix,
                            1.0,
                        )

                    # Warm radial glow behind pick
                    glow_r = math.hypot(nx, ny + 0.05)
                    glow = math.exp(-glow_r * glow_r * 3.4) * 0.20
                    col = blend(col, (0xF3 / 255.0, 0x92 / 255.0, 0x37 / 255.0, glow))

                    # Guitar pick silhouette
                    d_pick = pick_sdf(nx, ny)
                    pick_alpha = 1.0 - smoothstep(-aa, aa, d_pick)
                    if pick_alpha > 0.0:
                        # Subtle vertical amber gradient (#FBB36B -> #F39237 -> #D9721B)
                        pt = max(0.0, min(1.0, (ny + 0.65) / 1.35))
                        pr = (0xFB * (1.0 - pt) + 0xD9 * pt) / 255.0
                        pg = (0xB3 * (1.0 - pt) + 0x72 * pt) / 255.0
                        pb = (0x6B * (1.0 - pt) + 0x1B * pt) / 255.0
                        pick_col = (pr, pg, pb, pick_alpha)

                        # Three guitar string / wave cutouts inside pick
                        string_mask = 0.0
                        for sx_off, half_len in ((-0.16, 0.26), (0.0, 0.34), (0.16, 0.26)):
                            if abs(ny + 0.06) <= half_len:
                                dist_str = abs(nx - sx_off) - 0.036
                                cap = abs(ny + 0.06) - (half_len - 0.036)
                                sd = max(dist_str, cap) if cap <= 0 else math.hypot(max(dist_str, 0.0), cap)
                                string_mask = max(string_mask, 1.0 - smoothstep(-aa, aa, sd))
                        if string_mask > 0.0:
                            pick_col = (
                                pick_col[0] * (1.0 - string_mask * 0.88) + bg_col[0] * (string_mask * 0.88),
                                pick_col[1] * (1.0 - string_mask * 0.88) + bg_col[1] * (string_mask * 0.88),
                                pick_col[2] * (1.0 - string_mask * 0.88) + bg_col[2] * (string_mask * 0.88),
                                pick_alpha,
                            )
                        col = blend(col, pick_col)

                    acc_r += col[0] * tile_alpha
                    acc_g += col[1] * tile_alpha
                    acc_b += col[2] * tile_alpha
                    acc_a += tile_alpha

            inv = 0.25
            a = max(0.0, min(1.0, acc_a * inv))
            if a > 1e-6:
                r = max(0.0, min(1.0, (acc_r * inv) / a))
                g = max(0.0, min(1.0, (acc_g * inv) / a))
                b = max(0.0, min(1.0, (acc_b * inv) / a))
            else:
                r = g = b = 0.0

            idx = (py * size + px) * 4
            out[idx] = int(round(r * 255.0))
            out[idx + 1] = int(round(g * 255.0))
            out[idx + 2] = int(round(b * 255.0))
            out[idx + 3] = int(round(a * 255.0))

    return bytes(out)


def build_ico(sizes: tuple[int, ...] = (16, 32, 48, 256)) -> bytes:
    pngs = [make_png(sz, sz, render_icon_rgba(sz)) for sz in sizes]
    header = struct.pack("<HHH", 0, 1, len(sizes))
    offset = 6 + 16 * len(sizes)
    entries = bytearray()
    for sz, png in zip(sizes, pngs):
        dim = 0 if sz >= 256 else sz
        entries.extend(struct.pack("<BBBBHHII", dim, dim, 0, 0, 1, 32, len(png), offset))
        offset += len(png)
    return header + bytes(entries) + b"".join(pngs)


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate app/resources/apka_gitara.ico")
    default_out = pathlib.Path(__file__).resolve().parent.parent / "app" / "resources" / "apka_gitara.ico"
    parser.add_argument("--out", type=pathlib.Path, default=default_out)
    args = parser.parse_args()

    args.out.parent.mkdir(parents=True, exist_ok=True)
    ico_bytes = build_ico()
    args.out.write_bytes(ico_bytes)
    print(f"Generated {args.out} ({len(ico_bytes)} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
