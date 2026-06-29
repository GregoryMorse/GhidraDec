#!/usr/bin/env python3
"""Run IDA batch-analysis regression jobs for GhidraDec."""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
IDA_SCRIPT = ROOT / "tools" / "ida_batch_decompile_all.py"


def copy_input(input_path: Path, work_dir: Path, refresh: bool) -> Path:
    work_dir.mkdir(parents=True, exist_ok=True)
    target = work_dir / input_path.name
    if refresh or not target.exists():
        shutil.copy2(input_path, target)
    return target


def default_idb_path(work_input: Path) -> Path:
    if work_input.suffix.lower() in {".idb", ".i64"}:
        return work_input
    return work_input.with_suffix(work_input.suffix + ".i64")


def run_one(args: argparse.Namespace, input_path: Path) -> int:
    work_root = Path(args.work_dir).resolve()
    case_dir = work_root / input_path.stem
    work_input = copy_input(input_path.resolve(), case_dir, args.refresh)
    output_path = case_dir / (work_input.name + ".c")
    idb_path = Path(args.save_idb).resolve() if args.save_idb else default_idb_path(work_input)
    log_path = case_dir / "ida-batch.log"

    env = os.environ.copy()
    env["GHIDRADEC_BATCH_INPUT"] = str(work_input)
    env["GHIDRADEC_BATCH_OUTPUT"] = str(output_path)
    env["GHIDRADEC_BATCH_SAVE_IDB"] = str(idb_path) if args.save_database else ""
    env["GHIDRADEC_BATCH_PLUGIN"] = args.plugin
    env["GHIDRADEC_BATCH_PLUGIN_ARG"] = str(args.plugin_arg)
    env["GHIDRADEC_BATCH_TIMEOUT"] = str(args.timeout)
    env["GHIDRADEC_BATCH_STABLE_POLLS"] = str(args.stable_polls)
    env["GHIDRADEC_BATCH_MIN_OUTPUT_BYTES"] = str(args.min_output_bytes)
    env["GHIDRADEC_BATCH_CLEAN_OUTPUT"] = "1"
    if args.ghidra_dir:
        env["GHIDRADEC_BATCH_GHIDRA"] = str(Path(args.ghidra_dir).resolve())
        env["GHIDRADEC_GHIDRA_DIR"] = env["GHIDRADEC_BATCH_GHIDRA"]
        env["GHIDRA_INSTALL_DIR"] = env["GHIDRADEC_BATCH_GHIDRA"]

    ida_args = [str(Path(args.ida).resolve())]
    if args.batch:
        ida_args.append("-A")
    ida_args.extend([f"-L{log_path}", f"-S{IDA_SCRIPT}", str(work_input)])

    print(f"[ghidradec-batch] {input_path} -> {case_dir}")
    try:
        completed = subprocess.run(
            ida_args,
            env=env,
            cwd=str(case_dir),
            timeout=args.timeout + args.launch_grace_seconds,
            check=False,
        )
        return completed.returncode
    except subprocess.TimeoutExpired:
        print(f"[ghidradec-batch] FAIL: IDA timed out for {input_path}", file=sys.stderr)
        return 124
    finally:
        if log_path.exists() and args.print_log_tail > 0:
            lines = log_path.read_text(encoding="utf-8", errors="replace").splitlines()
            for line in lines[-args.print_log_tail:]:
                print(line)


def expand_inputs(values: list[str]) -> list[Path]:
    result: list[Path] = []
    for value in values:
        path = Path(value)
        if path.is_dir():
            for child in sorted(path.rglob("*")):
                if child.is_file():
                    result.append(child)
        else:
            result.append(path)
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ida", required=True, help="Path to idat/idat64/ida/ida64")
    parser.add_argument("--ghidra-dir", help="Ghidra installation used by the plugin")
    parser.add_argument("--work-dir", default=str(ROOT / "build" / "ida-regression"))
    parser.add_argument("--plugin", default="ghidradec64,ghidradec")
    parser.add_argument("--plugin-arg", type=int, default=1, help="Plugin argument; 1 is decompile-all")
    parser.add_argument("--timeout", type=int, default=600)
    parser.add_argument("--launch-grace-seconds", type=int, default=30)
    parser.add_argument("--stable-polls", type=int, default=3)
    parser.add_argument("--min-output-bytes", type=int, default=64)
    parser.add_argument("--save-database", action="store_true", help="Save the analyzed IDB/I64 before decompiling")
    parser.add_argument("--save-idb", help="Explicit save path for a single input database")
    parser.add_argument("--refresh", action="store_true", help="Refresh copied inputs in the work directory")
    parser.add_argument("--no-batch", dest="batch", action="store_false")
    parser.add_argument("--print-log-tail", type=int, default=80)
    parser.add_argument("inputs", nargs="+")
    args = parser.parse_args()

    ida = Path(args.ida)
    if not ida.exists():
        raise SystemExit(f"IDA executable was not found: {ida}")
    if not IDA_SCRIPT.exists():
        raise SystemExit(f"IDA-side script was not found: {IDA_SCRIPT}")
    if args.save_idb and len(args.inputs) != 1:
        raise SystemExit("--save-idb can only be used with a single input")

    failures = 0
    for input_path in expand_inputs(args.inputs):
        if not input_path.exists() or not input_path.is_file():
            print(f"[ghidradec-batch] FAIL: input not found: {input_path}", file=sys.stderr)
            failures += 1
            continue
        code = run_one(args, input_path)
        if code != 0:
            failures += 1
            print(f"[ghidradec-batch] FAIL: {input_path} exited with {code}", file=sys.stderr)
        else:
            print(f"[ghidradec-batch] PASS: {input_path}")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
