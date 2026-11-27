#!/usr/bin/env python3
"""Pack index HTML and web/ assets into GITAPAK1 binary and generate embed stub."""

from __future__ import annotations

import argparse
import pathlib
import re
import struct
import sys

MAGIC = b"GITAPAK1"

MIME_BY_EXT = {
    ".js": "application/javascript; charset=utf-8",
    ".mjs": "application/javascript; charset=utf-8",
    ".css": "text/css; charset=utf-8",
    ".html": "text/html; charset=utf-8",
    ".svg": "image/svg+xml",
    ".json": "application/json; charset=utf-8",
    ".wasm": "application/wasm",
    ".bin": "application/octet-stream",
    ".sf2": "application/octet-stream",
}

ALLOWED_REL_RE = re.compile(r"^[A-Za-z0-9/._-]+$")


def write_if_changed(path: pathlib.Path, data: bytes) -> bool:
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists():
        try:
            if path.read_bytes() == data:
                return False
        except OSError:
            pass
    path.write_bytes(data)
    return True


def extract_index_html(header_path: pathlib.Path) -> bytes:
    text = header_path.read_text(encoding="utf-8")
    start_tag = 'R"HTML('
    end_tag = ')HTML"'
    p1 = text.find(start_tag)
    p2 = text.rfind(end_tag)
    if p1 == -1 or p2 == -1 or p2 <= p1 + len(start_tag):
        raise ValueError(f"Could not find R\"HTML(...)HTML\" delimiters in {header_path}")
    html = text[p1 + len(start_tag) : p2]
    return html.encode("utf-8")


def collect_web_entries(web_dir: pathlib.Path) -> list[tuple[str, str, bytes]]:
    if not web_dir.is_dir():
        raise FileNotFoundError(f"Web directory does not exist: {web_dir}")

    entries: list[tuple[str, str, bytes]] = []
    files = sorted(
        (p for p in web_dir.rglob("*") if p.is_file()),
        key=lambda p: p.relative_to(web_dir).as_posix(),
    )
    for file_path in files:
        rel = file_path.relative_to(web_dir).as_posix()
        if ".." in rel or rel.startswith("/") or not ALLOWED_REL_RE.match(rel):
            continue
        mime = MIME_BY_EXT.get(file_path.suffix.lower())
        if mime is None:
            continue
        entries.append((f"/web/{rel}", mime, file_path.read_bytes()))
    return entries


def build_gitapak(entries: list[tuple[str, str, bytes]]) -> bytes:
    header_size = 8 + 4
    toc_entries: list[tuple[bytes, bytes, bytes]] = []
    toc_size = 0
    for url_path, mime, payload in entries:
        p_bytes = url_path.encode("utf-8")
        m_bytes = mime.encode("utf-8")
        toc_entries.append((p_bytes, m_bytes, payload))
        toc_size += 4 + len(p_bytes) + 4 + len(m_bytes) + 8 + 8

    out = bytearray()
    out.extend(MAGIC)
    out.extend(struct.pack("<I", len(toc_entries)))

    data_offset = header_size + toc_size
    for p_bytes, m_bytes, payload in toc_entries:
        out.extend(struct.pack("<I", len(p_bytes)))
        out.extend(p_bytes)
        out.extend(struct.pack("<I", len(m_bytes)))
        out.extend(m_bytes)
        out.extend(struct.pack("<QQ", data_offset, len(payload)))
        data_offset += len(payload)

    for _, _, payload in toc_entries:
        out.extend(payload)

    return bytes(out)


def parse_version_quad(version: str) -> tuple[int, int, int, int]:
    parts = [int(x) if x.isdigit() else 0 for x in re.split(r"[^0-9]+", version) if x][:4]
    while len(parts) < 4:
        parts.append(0)
    return (parts[0], parts[1], parts[2], parts[3])


def render_rc_embed(
    pak_path: pathlib.Path,
    icon_path: pathlib.Path | None,
    version: str,
    commit: str,
) -> str:
    v0, v1, v2, v3 = parse_version_quad(version)
    ver_str = f"{v0}.{v1}.{v2}.{v3}"
    pak_posix = pak_path.resolve().as_posix().replace('"', '\\"')
    lines = [
        "#pragma code_page(65001)",
        "#include <winver.h>",
        "",
        f'101 RCDATA "{pak_posix}"',
    ]
    if icon_path is not None:
        ico_posix = icon_path.resolve().as_posix().replace('"', '\\"')
        lines.append(f'102 ICON "{ico_posix}"')

    lines.extend(
        [
            "",
            "1 VERSIONINFO",
            f"FILEVERSION {v0},{v1},{v2},{v3}",
            f"PRODUCTVERSION {v0},{v1},{v2},{v3}",
            "FILEFLAGSMASK 0x3fL",
            "FILEFLAGS 0x0L",
            "FILEOS 0x40004L",
            "FILETYPE 0x1L",
            "FILESUBTYPE 0x0L",
            "BEGIN",
            '    BLOCK "StringFileInfo"',
            "    BEGIN",
            '        BLOCK "040904b0"',
            "        BEGIN",
            '            VALUE "CompanyName", "ApkaGitarowa\\0"',
            '            VALUE "FileDescription", "Apka Gitarowa\\0"',
            f'            VALUE "FileVersion", "{ver_str} ({commit[:8]})\\0"',
            '            VALUE "InternalName", "ApkaGitarowa\\0"',
            '            VALUE "OriginalFilename", "ApkaGitarowa.exe\\0"',
            '            VALUE "ProductName", "Apka Gitarowa\\0"',
            f'            VALUE "ProductVersion", "{version}\\0"',
            "        END",
            "    END",
            '    BLOCK "VarFileInfo"',
            "    BEGIN",
            '        VALUE "Translation", 0x409, 1200',
            "    END",
            "END",
            "",
        ]
    )
    return "\n".join(lines)


def render_asm_embed(pak_path: pathlib.Path) -> str:
    pak_escaped = pak_path.resolve().as_posix().replace("\\", "\\\\").replace('"', '\\"')
    return (
        "    .section .rodata\n"
        "    .balign 8\n"
        "    .global gitara_assets_pak_start\n"
        "    .global gitara_assets_pak_end\n"
        "gitara_assets_pak_start:\n"
        f'    .incbin "{pak_escaped}"\n'
        "gitara_assets_pak_end:\n"
        '    .section .note.GNU-stack,"",@progbits\n'
    )


def render_cpp_embed(pak_bytes: bytes) -> str:
    chunks: list[str] = []
    for i in range(0, len(pak_bytes), 16):
        row = ", ".join(f"0x{b:02x}" for b in pak_bytes[i : i + 16])
        chunks.append(f"    {row},")
    body = "\n".join(chunks)
    return (
        "#include <cstddef>\n"
        "#include <cstdint>\n\n"
        "namespace {\n"
        "alignas(8) constexpr std::uint8_t kGitaraAssetsPak[] = {\n"
        f"{body}\n"
        "};\n"
        "} // namespace\n\n"
        "extern \"C\" const std::uint8_t* gitara_assets_pak_data() noexcept {\n"
        "    return kGitaraAssetsPak;\n"
        "}\n\n"
        "extern \"C\" std::size_t gitara_assets_pak_size() noexcept {\n"
        f"    return {len(pak_bytes)}u;\n"
        "}\n"
    )


def main() -> int:
    parser = argparse.ArgumentParser(description="Pack HTML and web assets for embedding")
    parser.add_argument("--html", type=pathlib.Path, required=True)
    parser.add_argument("--web-dir", type=pathlib.Path, required=True)
    parser.add_argument("--out-pak", type=pathlib.Path, required=True)
    parser.add_argument("--mode", choices=("rc", "asm", "cpp"), required=True)
    parser.add_argument("--out-embed", type=pathlib.Path, required=True)
    parser.add_argument("--icon", type=pathlib.Path, default=None)
    parser.add_argument("--version", default="0.1.0")
    parser.add_argument("--commit", default="dev")
    args = parser.parse_args()

    index_html = extract_index_html(args.html)
    entries: list[tuple[str, str, bytes]] = [("/index.html", "text/html; charset=utf-8", index_html)]
    entries.extend(collect_web_entries(args.web_dir))

    pak_bytes = build_gitapak(entries)
    write_if_changed(args.out_pak, pak_bytes)

    if args.mode == "rc":
        embed_text = render_rc_embed(args.out_pak, args.icon, args.version, args.commit)
    elif args.mode == "asm":
        embed_text = render_asm_embed(args.out_pak)
    else:
        embed_text = render_cpp_embed(pak_bytes)

    write_if_changed(args.out_embed, embed_text.encode("utf-8"))
    print(f"Packed {len(entries)} assets ({len(pak_bytes)} bytes) -> {args.out_pak} [{args.mode}]")
    return 0


if __name__ == "__main__":
    sys.exit(main())
