#!/usr/bin/env python3
"""IDA SDK manifest helper for local builds and GitHub Actions."""

from __future__ import annotations

import argparse
import json
import os
import platform
import shutil
import sys
import urllib.request
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "ghidradec.targets.json"
PRIVATE_SDK_DIR = ROOT / ".idasdks"
PUBLISHABLE_SDK_DIR = ROOT / "idasdks"


def load_manifest() -> dict:
    return json.loads(MANIFEST.read_text(encoding="utf-8"))


def host_platform() -> str:
    system = platform.system().lower()
    if system == "darwin":
        return "macos"
    if system.startswith("windows"):
        return "windows"
    if system == "linux":
        return "linux"
    return system


def find_sdk(manifest: dict, version: str) -> dict:
    for sdk in manifest["idaSdks"]:
        if sdk["version"] == version:
            return sdk
    raise SystemExit(f"IDA SDK {version} is not defined in {MANIFEST}")


def extract_archive(archive: Path, destination_root: Path, directory: str) -> Path:
    if not archive.exists():
        raise SystemExit(f"Archive not found: {archive}")

    destination_root.mkdir(parents=True, exist_ok=True)
    sdk_dir = destination_root / directory
    if sdk_dir.exists() and (sdk_dir / "include" / "ida.hpp").exists():
        return sdk_dir

    scratch = destination_root / f".extract-{directory}"
    if scratch.exists():
        shutil.rmtree(scratch)
    scratch.mkdir(parents=True)

    with zipfile.ZipFile(archive) as zf:
        zf.extractall(scratch)

    nested_archive = scratch / f"{directory}.zip"
    if not any(scratch.rglob("include/ida.hpp")) and nested_archive.exists():
        nested_scratch = scratch / ".nested"
        nested_scratch.mkdir(parents=True)
        with zipfile.ZipFile(nested_archive) as zf:
            zf.extractall(nested_scratch)
        for child in list(scratch.iterdir()):
            if child != nested_scratch:
                if child.is_dir():
                    shutil.rmtree(child)
                else:
                    child.unlink()
        for child in nested_scratch.iterdir():
            shutil.move(str(child), str(scratch / child.name))
        shutil.rmtree(nested_scratch, ignore_errors=True)

    source_dir = scratch
    for marker in scratch.rglob("include/ida.hpp"):
        source_dir = marker.parent.parent
        break
    if not (source_dir / "include" / "ida.hpp").exists():
        raise SystemExit(f"{archive} did not extract to a recognizable IDA SDK layout")

    if sdk_dir.exists():
        shutil.rmtree(sdk_dir)
    shutil.move(str(source_dir), str(sdk_dir))
    shutil.rmtree(scratch, ignore_errors=True)
    return sdk_dir


def download(url: str, output: Path) -> Path:
    output.parent.mkdir(parents=True, exist_ok=True)
    if output.exists():
        return output
    print(f"Downloading {url}")
    with urllib.request.urlopen(url) as response, output.open("wb") as handle:
        shutil.copyfileobj(response, handle)
    return output


def ensure_sdk(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    sdk = find_sdk(manifest, args.version or manifest["project"]["defaultIdaSdk"])
    destination_root = Path(args.destination).resolve() if args.destination else PRIVATE_SDK_DIR

    if sdk["source"] in {"public-release", "public-branch"}:
        archive = download(sdk["url"], destination_root / sdk["archive"])
    else:
        archive = destination_root / sdk["archive"]
        if not archive.exists():
            encrypted = PUBLISHABLE_SDK_DIR / f"{sdk['archive']}.enc"
            raise SystemExit(
                f"Private SDK archive missing: {archive}. "
                f"Decrypt {encrypted} first or place {sdk['archive']} in {destination_root}."
            )

    sdk_dir = extract_archive(archive, destination_root, sdk["directory"])
    print(sdk_dir)


def matrix(args: argparse.Namespace) -> None:
    manifest = load_manifest()
    include_private = args.include_private or os.environ.get("GHIDRADEC_INCLUDE_PRIVATE_SDKS") == "1"
    only_host = args.current_host
    current = host_platform()
    default_version = manifest["project"]["defaultIdaSdk"]
    requested_versions = {
        version.strip()
        for version in (args.versions or "").split(",")
        if version.strip()
    }
    if args.all and requested_versions:
        raise SystemExit("--all and --versions are mutually exclusive")
    known_versions = {sdk["version"] for sdk in manifest["idaSdks"]}
    unknown_versions = sorted(requested_versions - known_versions)
    if unknown_versions:
        raise SystemExit(
            "Unknown IDA SDK version(s): "
            + ", ".join(unknown_versions)
            + ". Check ghidradec.targets.json for valid version names."
        )
    include = []

    for sdk in manifest["idaSdks"]:
        if not sdk.get("enabled", False):
            continue
        if requested_versions:
            if sdk["version"] not in requested_versions:
                continue
        elif not args.all:
            if sdk["version"] != default_version:
                continue
        if sdk["source"] not in {"public-release", "public-branch"} and not include_private:
            continue
        for os_name in sdk.get("platforms", ["windows", "linux", "macos"]):
            if only_host and os_name != current:
                continue
            runner = {
                "windows": "windows-latest",
                "linux": "ubuntu-latest",
                "macos": "macos-latest",
            }[os_name]
            include.append(
                {
                    "ida": sdk["version"],
                    "os": os_name,
                    "runner": runner,
                    "sdk_dir": sdk["directory"],
                    "sdk_source": sdk["source"],
                    "cmake_var": sdk["cmakeVariable"],
                }
            )

    print(json.dumps({"include": include}, separators=(",", ":")))


def main() -> int:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="command", required=True)

    ensure = sub.add_parser("ensure", help="Download/extract a target IDA SDK")
    ensure.add_argument("--version")
    ensure.add_argument("--destination")
    ensure.set_defaults(func=ensure_sdk)

    mat = sub.add_parser("matrix", help="Emit GitHub Actions matrix JSON")
    mat.add_argument("--include-private", action="store_true")
    mat.add_argument("--current-host", action="store_true")
    mat.add_argument("--all", action="store_true", help="Include every enabled SDK target.")
    mat.add_argument("--versions", help="Comma-separated SDK versions to include.")
    mat.set_defaults(func=matrix)

    args = parser.parse_args()
    args.func(args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
