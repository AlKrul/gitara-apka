#!/usr/bin/env python3
"""Generate manifest.json, SHA256SUMS.txt, and release_notes.md for a built executable."""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import pathlib
import sys


def compute_sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        while True:
            chunk = f.read(1024 * 1024)
            if not chunk:
                break
            h.update(chunk)
    return h.hexdigest().lower()


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate release manifest and checksums")
    parser.add_argument("--exe", type=pathlib.Path, required=True)
    parser.add_argument("--repo", required=True)
    parser.add_argument("--tag", default="latest")
    parser.add_argument("--version", default="0.1.0")
    parser.add_argument("--commit", required=True)
    parser.add_argument("--out-dir", type=pathlib.Path, required=True)
    parser.add_argument("--built-at", default="")
    args = parser.parse_args()

    if not args.exe.is_file():
        print(f"Error: executable not found: {args.exe}", file=sys.stderr)
        return 1

    args.out_dir.mkdir(parents=True, exist_ok=True)
    built_at = args.built_at.strip() or dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    sha256_hex = compute_sha256(args.exe)
    size_bytes = args.exe.stat().st_size
    asset_name = args.exe.name
    short_commit = args.commit[:7]
    download_url = f"https://github.com/{args.repo}/releases/download/{args.tag}/{asset_name}"
    permalink_url = f"https://github.com/{args.repo}/releases/latest/download/{asset_name}"

    manifest = {
        "schema": 1,
        "version": args.version,
        "commit": args.commit,
        "shortCommit": short_commit,
        "builtAt": built_at,
        "assetName": asset_name,
        "downloadUrl": download_url,
        "sha256": sha256_hex,
        "size": size_bytes,
    }

    (args.out_dir / "manifest.json").write_text(
        json.dumps(manifest, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    (args.out_dir / "SHA256SUMS.txt").write_text(
        f"{sha256_hex}  {asset_name}\n",
        encoding="utf-8",
    )

    notes = (
        "## Apka Gitarowa — samodzielna wersja Windows (x64)\n\n"
        f"- **Stały link do pobrania:** {permalink_url}\n"
        f"- **Commit (`main`):** `{short_commit}` (`{args.commit}`)\n"
        f"- **Zbudowano (UTC):** `{built_at}`\n"
        f"- **Rozmiar:** {size_bytes:,} B ({size_bytes / (1024 * 1024):.2f} MB)\n"
        f"- **SHA-256 (`{asset_name}`):** `{sha256_hex}`\n\n"
        "### Jak używać\n"
        "1. Pobierz plik `ApkaGitarowa.exe` — nie wymaga instalacji WSL, Visual Studio ani Node.js.\n"
        "2. Przed każdym uruchomieniem aplikacja automatycznie sprawdza nowe wydanie w tym repozytorium "
        "i aktualizuje się w locie (w razie braku internetu uruchamia się od razu offline).\n"
        "3. Ustawienia i biblioteka użytkownika są zapisywane w `%APPDATA%\\ApkaGitarowa`, więc aktualizacje ich nie nadpisują.\n\n"
        "> **Uwaga dot. Windows SmartScreen:** Ponieważ plik nie jest podpisany komercyjnym certyfikatem, "
        "przy pierwszym uruchomieniu Windows może wyświetlić ostrzeżenie *„System Windows ochronił ten komputer”*. "
        "Kliknij **Więcej informacji → Uruchom mimo to**.\n"
    )
    (args.out_dir / "release_notes.md").write_text(notes, encoding="utf-8")

    print(f"Wrote manifest.json, SHA256SUMS.txt, release_notes.md to {args.out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
