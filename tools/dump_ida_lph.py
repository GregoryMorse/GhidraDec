#!/usr/bin/env python3
"""Dump IDA processor short names from exported processor_t LPH records."""

from __future__ import annotations

import argparse
import ctypes
import json
import os
import sys
from pathlib import Path


CharPtrArray = ctypes.POINTER(ctypes.c_char_p)


class Processor(ctypes.Structure):
    """IDA 9.x 64-bit processor_t prefix through name/assembler pointers."""

    _fields_ = [
        ("version", ctypes.c_int32),
        ("id", ctypes.c_int32),
        ("flag", ctypes.c_uint32),
        ("flag2", ctypes.c_uint32),
        ("cnbits", ctypes.c_int32),
        ("dnbits", ctypes.c_int32),
        ("psnames", CharPtrArray),
        ("plnames", CharPtrArray),
        ("assemblers", ctypes.c_void_p),
    ]


def read_c_string_array(values: CharPtrArray | None, limit: int = 256) -> list[str]:
    result: list[str] = []
    if not values:
        return result
    for i in range(limit):
        value = values[i]
        if not value:
            break
        result.append(value.decode("utf-8", errors="replace"))
    return result


def dump_module(path: Path) -> dict[str, object]:
    lib = ctypes.CDLL(str(path))
    ph = Processor.in_dll(lib, "LPH")
    return {
        "module": path.stem,
        "path": str(path),
        "version": ph.version,
        "id": ph.id,
        "flag": ph.flag,
        "flag2": ph.flag2,
        "cnbits": ph.cnbits,
        "dnbits": ph.dnbits,
        "psnames": read_c_string_array(ph.psnames),
        "plnames": read_c_string_array(ph.plnames),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--ida-dir",
        default=r"C:\Program Files\IDA Professional 9.3",
        help="IDA installation directory.",
    )
    parser.add_argument(
        "modules",
        nargs="*",
        help="Processor module names to dump. Defaults to every DLL in procs.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="Write JSON here instead of stdout.",
    )
    args = parser.parse_args()

    ida_dir = Path(args.ida_dir)
    procs_dir = ida_dir / "procs"
    if hasattr(os, "add_dll_directory"):
        os.add_dll_directory(str(ida_dir))
        os.add_dll_directory(str(procs_dir))

    modules = args.modules or [path.stem for path in sorted(procs_dir.glob("*.dll"))]
    results: list[dict[str, object]] = []
    errors: list[dict[str, str]] = []
    for module in modules:
        path = procs_dir / f"{module}.dll"
        try:
            results.append(dump_module(path))
        except Exception as exc:  # noqa: BLE001 - audit tool should report all modules.
            errors.append({"module": module, "error": str(exc)})

    text = json.dumps({"processors": results, "errors": errors}, indent=2) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding="utf-8")
    else:
        sys.stdout.write(text)
        sys.stdout.flush()

    # IDA's runtime prints a sign-off banner during normal process teardown once
    # its DLLs are loaded, which corrupts stdout JSON. We have no resources that
    # need Python-level cleanup here.
    try:
        with open(os.devnull, "w", encoding="utf-8") as devnull:
            os.dup2(devnull.fileno(), sys.stdout.fileno())
    except Exception:
        pass
    os._exit(1 if errors else 0)


if __name__ == "__main__":
    raise SystemExit(main())
