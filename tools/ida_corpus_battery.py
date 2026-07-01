#!/usr/bin/env python3
"""Stage a public corpus and run the IDA-backed GhidraDec regression battery."""

from __future__ import annotations

import argparse
import json
import os
import subprocess
import sys
import time
from pathlib import Path

import corpus as corpus_tool


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_REPORT_DIR = ROOT / "build" / "corpus-reports"
DEFAULT_WORK_DIR = ROOT / "build" / "ida-corpus-regression"
DEFAULT_PLUGIN = ROOT / "build" / "matrix" / "ida-9.3" / "Release" / "ghidradec64.dll"


def default_ghidra_dir() -> str:
    return os.environ.get("GHIDRADEC_GHIDRA_DIR", "") or os.environ.get("GHIDRA_INSTALL_DIR", "")


def parse_filters(value: str) -> set[str]:
    return {item.strip() for item in value.split(",") if item.strip()}


def stage_targets(args: argparse.Namespace) -> list[dict[str, object]]:
    manifest = corpus_tool.load_manifest(Path(args.manifest).resolve())
    targets = corpus_tool.selected_targets(
        manifest,
        args.corpus,
        parse_filters(args.arch),
        parse_filters(args.tier),
    )
    if not targets:
        raise SystemExit("No corpus targets matched the requested filters.")

    destination = Path(args.destination).resolve()
    staged = []
    for corpus, target in targets:
        if corpus["kind"] != "github-raw":
            raise SystemExit(f"Unsupported corpus kind: {corpus['kind']}")
        if not args.no_stage:
            output = corpus_tool.stage_target(destination, corpus, target, args.refresh)
        else:
            output = corpus_tool.target_output_path(destination, corpus, target)
        if not output.exists():
            raise SystemExit(f"Corpus target is not staged: {output}")
        staged.append({
            "corpus": corpus["name"],
            "repository": corpus.get("repository", ""),
            "ref": corpus.get("ref", ""),
            "id": target["id"],
            "arch": target["arch"],
            "tier": target.get("tier", ""),
            "source_path": target["path"],
            "path": str(output),
            "test_mode": target.get("testMode", "decompile-all"),
            "individual_start_index": int(target.get("individualStartIndex", 0)),
            "individual_max": int(target.get("individualMax", 0)),
        })
    return staged


def build_ida_batch_command(
    args: argparse.Namespace,
    input_list: Path,
    summary_json: Path,
    *,
    individual_functions: bool = False,
    individual_start_index: int = 0,
    individual_max: int = 0,
) -> list[str]:
    cmd = [
        sys.executable,
        str(ROOT / "tools" / "ida_batch.py"),
        "--work-dir",
        str(Path(args.work_dir).resolve()),
        "--input-list",
        str(input_list),
        "--summary-json",
        str(summary_json),
        "--timeout",
        str(args.timeout),
        "--stable-polls",
        str(args.stable_polls),
        "--print-log-tail",
        str(args.print_log_tail),
        "--plugin",
        str(Path(args.plugin).resolve()),
    ]
    if args.ida:
        cmd.extend(["--ida", args.ida])
    if args.ida_dir:
        cmd.extend(["--ida-dir", args.ida_dir])
    if args.ghidra_dir:
        cmd.extend(["--ghidra-dir", args.ghidra_dir])
    if args.save_database:
        cmd.append("--save-database")
    if args.refresh_database:
        cmd.append("--refresh")
    if args.reuse_database:
        cmd.append("--reuse-database")
    if args.paramid:
        cmd.append("--paramid")
    if individual_functions:
        cmd.append("--individual-functions")
        if individual_start_index:
            cmd.extend(["--individual-start-index", str(individual_start_index)])
        if individual_max:
            cmd.extend(["--individual-max", str(individual_max)])
    if args.dialog_automation == "off":
        cmd.extend(["--dialog-automation", "off"])
    if not args.trace:
        cmd.append("--no-trace")
    return cmd


def write_input_list(report_dir: Path, mode: str, staged: list[dict[str, object]]) -> Path:
    name = "inputs.txt" if mode == "decompile-all" else f"inputs-{mode}.txt"
    input_list = report_dir / name
    input_list.parent.mkdir(parents=True, exist_ok=True)
    input_list.write_text("\n".join(str(item["path"]) for item in staged) + "\n", encoding="utf-8")
    return input_list


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest", default=str(corpus_tool.MANIFEST))
    parser.add_argument("--corpus", default="angr-binaries")
    parser.add_argument("--arch", default="x86_64,x86_32,x86_16")
    parser.add_argument("--tier", default="smoke")
    parser.add_argument("--destination", default=str(corpus_tool.DEFAULT_DESTINATION))
    parser.add_argument("--report-dir", default=str(DEFAULT_REPORT_DIR))
    parser.add_argument("--work-dir", default=str(DEFAULT_WORK_DIR))
    parser.add_argument("--ida")
    parser.add_argument("--ida-dir")
    parser.add_argument("--ghidra-dir", default=default_ghidra_dir())
    parser.add_argument("--plugin", default=str(DEFAULT_PLUGIN))
    parser.add_argument("--timeout", type=int, default=900)
    parser.add_argument("--stable-polls", type=int, default=1)
    parser.add_argument("--print-log-tail", type=int, default=60)
    parser.add_argument("--refresh", action="store_true", help="Refresh staged corpus downloads")
    parser.add_argument("--refresh-database", action="store_true", help="Refresh copied inputs in IDA work dirs")
    parser.add_argument("--reuse-database", action="store_true")
    parser.add_argument("--save-database", action="store_true")
    parser.add_argument("--paramid", action="store_true")
    parser.add_argument("--trace", dest="trace", action="store_true", default=True, help="Enable GhidraDec trace logging in IDA")
    parser.add_argument("--no-trace", dest="trace", action="store_false", help="Disable GhidraDec trace logging in IDA")
    parser.add_argument("--dialog-automation", choices=["auto", "off"], default="auto")
    parser.add_argument("--no-stage", action="store_true", help="Use already staged corpus files only")
    parser.add_argument("--dry-run", action="store_true", help="Stage/list targets but do not launch IDA")
    args = parser.parse_args()
    if not args.dry_run and not args.ghidra_dir:
        raise SystemExit(
            "A Ghidra installation is required. Pass --ghidra-dir or set GHIDRADEC_GHIDRA_DIR/GHIDRA_INSTALL_DIR."
        )

    started = time.time()
    report_dir = Path(args.report_dir).resolve()
    report_dir.mkdir(parents=True, exist_ok=True)
    battery_summary = report_dir / "battery-summary.json"

    staged = stage_targets(args)
    groups: dict[str, list[dict[str, object]]] = {}
    for item in staged:
        groups.setdefault(str(item.get("test_mode") or "decompile-all"), []).append(item)

    exit_code = 0
    batch_runs = []
    for mode, mode_targets in groups.items():
        input_list = write_input_list(report_dir, mode, mode_targets)
        summary_name = "ida-batch-summary.json" if len(groups) == 1 and mode == "decompile-all" else f"ida-batch-summary-{mode}.json"
        batch_summary = report_dir / summary_name
        individual = mode == "individual-functions"
        individual_start_index = 0
        individual_max = 0
        if individual:
            starts = {int(item.get("individual_start_index", 0)) for item in mode_targets}
            if len(starts) != 1:
                raise SystemExit("Mixed individualStartIndex values require separate battery runs.")
            individual_start_index = starts.pop()
            individual_max = max(int(item.get("individual_max", 0)) for item in mode_targets)
        command = build_ida_batch_command(
            args,
            input_list,
            batch_summary,
            individual_functions=individual,
            individual_start_index=individual_start_index,
            individual_max=individual_max,
        )
        run_exit = 0
        if args.dry_run:
            print("[ghidradec-battery] dry run; not launching IDA")
            print("[ghidradec-battery] " + " ".join(command))
        else:
            print(f"[ghidradec-battery] running {len(mode_targets)} {mode} target(s)")
            run_exit = subprocess.run(command, cwd=str(ROOT)).returncode
        if run_exit != 0 and exit_code == 0:
            exit_code = run_exit
        batch_data = {}
        if batch_summary.exists():
            batch_data = json.loads(batch_summary.read_text(encoding="utf-8"))
        batch_runs.append({
            "mode": mode,
            "exit_code": run_exit,
            "input_list": str(input_list),
            "batch_summary": str(batch_summary),
            "command": command,
            "targets": mode_targets,
            "batch": batch_data,
        })

    report = {
        "corpus": args.corpus,
        "arch": args.arch,
        "tier": args.tier,
        "started": started,
        "elapsed_seconds": round(time.time() - started, 3),
        "exit_code": exit_code,
        "input_list": batch_runs[0]["input_list"] if len(batch_runs) == 1 else "",
        "batch_summary": batch_runs[0]["batch_summary"] if len(batch_runs) == 1 else "",
        "command": batch_runs[0]["command"] if len(batch_runs) == 1 else [],
        "targets": staged,
        "batch": batch_runs[0]["batch"] if len(batch_runs) == 1 else {},
        "batch_runs": batch_runs,
    }
    battery_summary.write_text(json.dumps(report, indent=2, sort_keys=True), encoding="utf-8")
    print(f"[ghidradec-battery] wrote {battery_summary}")
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
