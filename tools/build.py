#!/usr/bin/env python3
"""Manifest-driven local build helper for GhidraDec."""

from __future__ import annotations

import argparse
import json
import os
import shutil
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


def default_build_dir(ida_version: str) -> str:
    return str(Path("build") / "matrix" / f"ida-{ida_version}")


def cmake_command() -> str:
    if found := shutil.which("cmake"):
        return found

    candidate_roots = [
        Path(os.environ[key])
        for key in ("ProgramFiles", "ProgramFiles(x86)")
        if os.environ.get(key)
    ]
    for root in candidate_roots:
        candidate = root / "CMake" / "bin" / "cmake.exe"
        if candidate.exists():
            return str(candidate)

        visual_studio_root = root / "Microsoft Visual Studio"
        if visual_studio_root.exists():
            for candidate in visual_studio_root.glob("*/**/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe"):
                if candidate.exists():
                    return str(candidate)

    return "cmake"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ida-version", default="", help="IDA SDK version from ghidradec.targets.json")
    parser.add_argument("--build-dir", default="", help="CMake build directory. Defaults to build/matrix/ida-<version>.")
    parser.add_argument("--config", default="Release")
    parser.add_argument("--generator", default="")
    parser.add_argument("--install", action="store_true")
    parser.add_argument("--deploy", action="store_true", help="Try to copy built plugin(s) to the configured IDA plugin directory after build.")
    parser.add_argument("--deploy-dir", default="", help="Plugin directory for --deploy. Defaults to the CMake platform default.")
    args = parser.parse_args()

    ida_version = args.ida_version or default_ida_version()
    build_dir = args.build_dir or default_build_dir(ida_version)
    cmake = cmake_command()
    run([sys.executable, "tools/ida_sdk.py", "ensure", "--version", ida_version])

    configure = [
        cmake,
        "-S",
        ".",
        "-B",
        build_dir,
        f"-DGHIDRADEC_IDA_VERSION={ida_version}",
        f"-DCMAKE_BUILD_TYPE={args.config}",
        f"-DGHIDRADEC_AUTO_DEPLOY={'ON' if args.deploy else 'OFF'}",
    ]
    if args.deploy_dir:
        configure.append(f"-DGHIDRADEC_DEPLOY_DIR={args.deploy_dir}")
    if args.generator:
        configure[1:1] = ["-G", args.generator]
    run(configure)

    run([cmake, "--build", build_dir, "--config", args.config, "--parallel"])
    if args.install:
        run([cmake, "--build", build_dir, "--config", args.config, "--target", "install"])
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
