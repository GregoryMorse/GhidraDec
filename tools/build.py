#!/usr/bin/env python3
"""Manifest-driven local build helper for GhidraDec."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def default_ida_version() -> str:
    manifest = json.loads((ROOT / "ghidradec.targets.json").read_text(encoding="utf-8"))
    return str(manifest["project"]["defaultIdaSdk"])


def run(command: list[str]) -> None:
    print("+ " + " ".join(command))
    subprocess.run(command, cwd=ROOT, check=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ida-version", default="", help="IDA SDK version from ghidradec.targets.json")
    parser.add_argument("--build-dir", default="build")
    parser.add_argument("--config", default="Release")
    parser.add_argument("--generator", default="")
    parser.add_argument("--install", action="store_true")
    args = parser.parse_args()

    ida_version = args.ida_version or default_ida_version()
    run([sys.executable, "tools/ida_sdk.py", "ensure", "--version", ida_version])

    configure = [
        "cmake",
        "-S",
        ".",
        "-B",
        args.build_dir,
        f"-DGHIDRADEC_IDA_VERSION={ida_version}",
        f"-DCMAKE_BUILD_TYPE={args.config}",
    ]
    if args.generator:
        configure[1:1] = ["-G", args.generator]
    run(configure)

    run(["cmake", "--build", args.build_dir, "--config", args.config, "--parallel"])
    if args.install:
        run(["cmake", "--build", args.build_dir, "--config", args.config, "--target", "install"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
