#!/usr/bin/env python3
"""Dependency provisioning helpers for CI and local builds."""

from __future__ import annotations

import argparse
import fnmatch
import json
import os
import shutil
import urllib.error
import urllib.request
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "ghidradec.targets.json"
SCRATCH = ROOT / "build" / "deps"
DEFAULT_GHIDRA_OUTPUT = SCRATCH / "ghidra-decompiler"

GHIDRA_DEC_DEPS = (
    "address.*",
    "context.*",
    "emulate.*",
    "error.hh",
    "filemanage.*",
    "float.*",
    "globalcontext.*",
    "loadimage.*",
    "marshal.*",
    "memstate.*",
    "opbehavior.*",
    "opcodes.*",
    "partmap.hh",
    "pcodecompile.*",
    "pcodeparse.*",
    "pcoderaw.*",
    "rangemap.hh",
    "semantics.*",
    "slaformat.*",
    "sleigh.*",
    "sleighbase.*",
    "slghpatexpress.*",
    "slghpattern.*",
    "slghsymbol.*",
    "space.*",
    "translate.*",
    "types.h",
    "xml.*",
    "compression.*",
    "crc32.*",
)


def load_manifest() -> dict:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def ghidra_source_urls(version: str) -> list[str]:
    env_url = os.environ.get("GHIDRA_SOURCE_URL")
    if env_url:
        return [env_url]
    return [
        f"https://github.com/NationalSecurityAgency/ghidra/archive/refs/tags/Ghidra_{version}_build.zip",
        f"https://github.com/NationalSecurityAgency/ghidra/archive/refs/tags/Ghidra_{version}.zip",
    ]


def download(url: str, output: Path, refresh: bool) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists() and not refresh:
        return output
    print(f"[ghidradec-deps] downloading {url}")
    with urllib.request.urlopen(url) as response, output.open("wb") as handle:
        shutil.copyfileobj(response, handle)
    return output


def download_first(urls: list[str], output: Path, refresh: bool) -> Path:
    failures: list[str] = []
    for index, url in enumerate(urls):
        candidate = output if len(urls) == 1 else output.with_name(f"{output.stem}-{index}{output.suffix}")
        try:
            return download(url, candidate, refresh)
        except urllib.error.URLError as exc:
            if candidate.exists():
                candidate.unlink()
            failures.append(f"{url}: {exc}")
    raise SystemExit("Could not download Ghidra source archive:\n  " + "\n  ".join(failures))


def extract_zip(archive: Path, destination: Path, refresh: bool) -> Path:
    if destination.exists() and refresh:
        shutil.rmtree(destination)
    if destination.exists():
        return destination
    scratch = destination.with_name(destination.name + ".extract")
    if scratch.exists():
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True)
    with zipfile.ZipFile(archive) as zf:
        zf.extractall(scratch)
    children = [child for child in scratch.iterdir()]
    source = children[0] if len(children) == 1 and children[0].is_dir() else scratch
    if destination.exists():
        shutil.rmtree(destination)
    shutil.move(str(source), str(destination))
    shutil.rmtree(scratch, ignore_errors=True)
    return destination


def find_ghidra_cpp_dir(source_root: Path) -> Path:
    expected = source_root / "Ghidra" / "Features" / "Decompiler" / "src" / "decompile" / "cpp"
    if (expected / "address.cc").exists():
        return expected
    for marker in source_root.rglob("Ghidra/Features/Decompiler/src/decompile/cpp/address.cc"):
        return marker.parent
    raise SystemExit(f"Could not find Ghidra decompiler C++ sources under {source_root}")


def copy_matching(source_dir: Path, destination: Path, patterns: tuple[str, ...]) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    for child in source_dir.iterdir():
        if child.is_dir():
            continue
        if any(fnmatch.fnmatch(child.name, pattern) for pattern in patterns):
            shutil.copy2(child, destination / child.name)


def patch_filemanage(path: Path) -> None:
    if not path.exists():
        return
    text = path.read_text(encoding="utf-8")
    text = text.replace("GetFileAttributes(", "GetFileAttributesA(")
    path.write_text(text, encoding="utf-8")


def ensure_ghidra_sources(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    version = args.version or manifest["project"]["defaultGhidraVersion"]
    output_dir = Path(args.output_dir).resolve() if args.output_dir else DEFAULT_GHIDRA_OUTPUT
    ghidra_dir = output_dir / "Ghidra"
    decompile_dir = output_dir / "decompile"
    if (decompile_dir / "sleigh.cc").exists() and not args.refresh:
        print(f"[ghidradec-deps] Ghidra decompiler subset already exists in {output_dir}")
        return
    if output_dir.exists() and args.refresh:
        shutil.rmtree(output_dir)

    if args.source_dir:
        cpp_dir = Path(args.source_dir).resolve()
    else:
        urls = [args.url] if args.url else ghidra_source_urls(version)
        archive = download_first(urls, SCRATCH / f"ghidra-{version}.zip", args.refresh)
        source_root = extract_zip(archive, SCRATCH / f"ghidra-{version}", args.refresh)
        cpp_dir = find_ghidra_cpp_dir(source_root)

    if not (cpp_dir / "address.cc").exists():
        raise SystemExit(f"Ghidra decompiler C++ source directory is invalid: {cpp_dir}")

    ghidra_dir.mkdir(parents=True, exist_ok=True)
    decompile_dir.mkdir(parents=True, exist_ok=True)

    for child in cpp_dir.iterdir():
        if child.is_file():
            shutil.copy2(child, ghidra_dir / child.name)
    copy_matching(cpp_dir, decompile_dir, GHIDRA_DEC_DEPS)
    patch_filemanage(decompile_dir / "filemanage.cc")

    zlib_dir = cpp_dir.parent / "zlib"
    if zlib_dir.exists():
        target = decompile_dir / "zlib"
        if target.exists():
            shutil.rmtree(target)
        shutil.copytree(zlib_dir, target)

    print(f"[ghidradec-deps] refreshed Ghidra decompiler subset from {cpp_dir} into {output_dir}")


def main() -> int:
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)

    ghidra = subparsers.add_parser("ensure-ghidra-sources")
    ghidra.add_argument("--version", help="Ghidra version; defaults to manifest project.defaultGhidraVersion")
    ghidra.add_argument("--url", help="Source archive URL override")
    ghidra.add_argument("--source-dir", help="Existing Ghidra decompiler C++ source directory")
    ghidra.add_argument("--output-dir", help="Generated output directory; defaults to build/deps/ghidra-decompiler")
    ghidra.add_argument("--refresh", action="store_true")
    ghidra.set_defaults(func=ensure_ghidra_sources)

    args = parser.parse_args()
    args.func(args)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
