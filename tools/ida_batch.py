#!/usr/bin/env python3
"""Run IDA batch-analysis regression jobs for GhidraDec."""

from __future__ import annotations

import argparse
import hashlib
import os
import platform
import shutil
import subprocess
import sys
import time
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
IDA_SCRIPT = ROOT / "tools" / "ida_batch_decompile_all.py"
DEFAULT_IDA_93_DIRS = [
    Path(r"C:\Program Files\IDA Professional 9.3"),
    Path(r"C:\Program Files\IDA Pro 9.3"),
    Path(r"C:\Program Files\IDA Free 9.3"),
]
WINDOW_DIALOG_PATTERNS = (
    "license",
    "not yet accepted",
    "python 3 is not configured",
    "unpacked version",
    "restore packed",
    "did not close properly",
    "warning",
    "hex-rays",
    "ida has encountered a problem",
    "encountered a problem",
    "minidump",
    "crash",
    "access violation",
    "application error",
)
DEFAULT_FAILURE_PATTERNS = (
    "[GhidraDec error]",
    "Low-level Error:",
    "Unhandled exception:",
    "[ghidradec-batch] FAIL:",
)


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


def remove_if_exists(path: Path) -> None:
    try:
        path.unlink()
    except FileNotFoundError:
        return


def clean_ida_database_sidecars(work_input: Path, idb_path: Path) -> None:
    if work_input.suffix.lower() in {".idb", ".i64"}:
        return
    remove_if_exists(idb_path)
    for suffix in (".id0", ".id1", ".id2", ".nam", ".til"):
        remove_if_exists(work_input.with_suffix(suffix))


def stage_plugin_plugins(plugin_spec: str, case_dir: Path) -> tuple[str, Path | None]:
    names = []
    ida_user_dir = None
    for item in plugin_spec.split(","):
        item = item.strip()
        if not item:
            continue
        plugin_path = Path(item)
        if plugin_path.exists() and plugin_path.is_file():
            if ida_user_dir is None:
                ida_user_dir = case_dir / "ida-user"
                if ida_user_dir.exists():
                    shutil.rmtree(ida_user_dir)
            plugin_dir = ida_user_dir / "plugins"
            plugin_dir.mkdir(parents=True, exist_ok=True)
            staged = plugin_dir / f"{plugin_path.stem}_batch{plugin_path.suffix}"
            shutil.copy2(plugin_path, staged)
            names.append(staged.stem)
        else:
            names.append(item)
    return ",".join(names), ida_user_dir


def case_name(input_path: Path) -> str:
    digest = hashlib.sha1(str(input_path.resolve()).encode("utf-8")).hexdigest()[:12]
    return f"{input_path.stem}-{digest}"


def find_ida_executable(args: argparse.Namespace) -> Path:
    if args.ida:
        return Path(args.ida).resolve()

    candidate_dirs = []
    if args.ida_dir:
        candidate_dirs.append(Path(args.ida_dir))
    if platform.system().lower() == "windows":
        candidate_dirs.extend(DEFAULT_IDA_93_DIRS)

    names = ["idat.exe", "ida.exe"] if platform.system().lower() == "windows" else ["idat", "ida"]
    for directory in candidate_dirs:
        for name in names:
            candidate = directory / name
            if candidate.exists():
                return candidate.resolve()
    raise SystemExit("IDA executable was not found. Pass --ida or --ida-dir.")


def windows_dialog_automation(process_id: int, allow_global_crash_windows: bool = True) -> None:
    if platform.system().lower() != "windows":
        return

    import ctypes
    from ctypes import wintypes

    user32 = ctypes.windll.user32
    enum_windows_proc = ctypes.WINFUNCTYPE(wintypes.BOOL, wintypes.HWND, wintypes.LPARAM)

    def get_text(hwnd, fn, size=512):
        buffer = ctypes.create_unicode_buffer(size)
        fn(hwnd, buffer, size)
        return buffer.value

    def callback(hwnd, _lparam):
        if not user32.IsWindowVisible(hwnd):
            return True

        hwnd_value = int(hwnd)
        pid = wintypes.DWORD()
        user32.GetWindowThreadProcessId(hwnd, ctypes.byref(pid))
        title = get_text(hwnd, user32.GetWindowTextW)
        class_name = get_text(hwnd, user32.GetClassNameW, 256)
        haystack = f"{title} {class_name}".lower()
        is_dialog = class_name == "#32770"
        matched = any(pattern in haystack for pattern in WINDOW_DIALOG_PATTERNS)
        owned_by_ida = pid.value == process_id
        crash_window = allow_global_crash_windows and "werfault" in haystack
        if not ((owned_by_ida and (is_dialog or matched)) or crash_window):
            return True

        print(
            f"[ghidradec-batch] accepting dialog hwnd=0x{hwnd_value:08x} pid={pid.value} "
            f"class='{class_name}' title='{title}'")
        user32.SetForegroundWindow(hwnd)
        user32.SendMessageW(hwnd, 0x0111, 1, 0)
        user32.PostMessageW(hwnd, 0x0100, 0x0D, 0)
        user32.PostMessageW(hwnd, 0x0101, 0x0D, 0)
        return True

    user32.EnumWindows(enum_windows_proc(callback), 0)


def run_ida(args: argparse.Namespace, ida_args: list[str], env: dict[str, str], case_dir: Path) -> int:
    deadline = time.monotonic() + args.timeout + args.launch_grace_seconds
    process = subprocess.Popen(ida_args, env=env, cwd=str(case_dir))
    try:
        while True:
            exit_code = process.poll()
            if exit_code is not None:
                return exit_code
            if time.monotonic() > deadline:
                print(f"[ghidradec-batch] FAIL: IDA timed out; stopping pid {process.pid}", file=sys.stderr)
                process.kill()
                return 124
            if args.dialog_automation != "off":
                windows_dialog_automation(process.pid)
            time.sleep(1)
    except KeyboardInterrupt:
        process.terminate()
        raise


def scan_failure_patterns(args: argparse.Namespace, log_path: Path) -> bool:
    if not log_path.exists():
        return False
    patterns = []
    if not args.no_default_fail_log_patterns:
        patterns.extend(DEFAULT_FAILURE_PATTERNS)
    patterns.extend(args.fail_log_pattern or [])
    if not patterns:
        return False
    text = log_path.read_text(encoding="utf-8", errors="replace")
    for pattern in patterns:
        if pattern in text:
            print(f"[ghidradec-batch] FAIL: log contains failure pattern: {pattern}", file=sys.stderr)
            return True
    return False


def validate_output(args: argparse.Namespace, output_path: Path) -> bool:
    if not output_path.exists():
        print(f"[ghidradec-batch] FAIL: output was not created: {output_path}", file=sys.stderr)
        return False
    size = output_path.stat().st_size
    if size < args.min_output_bytes:
        print(
            f"[ghidradec-batch] FAIL: output is too small: {output_path} ({size} bytes)",
            file=sys.stderr,
        )
        return False
    return True


def run_one(args: argparse.Namespace, input_path: Path) -> int:
    work_root = Path(args.work_dir).resolve()
    case_dir = work_root / case_name(input_path)
    work_input = copy_input(input_path.resolve(), case_dir, args.refresh)
    output_path = case_dir / (work_input.name + ".c")
    idb_path = Path(args.save_idb).resolve() if args.save_idb else default_idb_path(work_input)
    log_path = case_dir / "ida-batch.log"
    remove_if_exists(log_path)
    if not args.reuse_database:
        clean_ida_database_sidecars(work_input, idb_path)
    plugin_names, ida_user_dir = stage_plugin_plugins(args.plugin, case_dir)

    env = os.environ.copy()
    env["GHIDRADEC_BATCH_INPUT"] = str(work_input)
    env["GHIDRADEC_BATCH_OUTPUT"] = str(output_path)
    env["GHIDRADEC_BATCH_SAVE_IDB"] = str(idb_path) if args.save_database else ""
    env["GHIDRADEC_BATCH_PLUGIN"] = plugin_names
    env["GHIDRADEC_BATCH_PLUGIN_ARG"] = str(args.plugin_arg)
    env["GHIDRADEC_BATCH_TIMEOUT"] = str(args.timeout)
    env["GHIDRADEC_BATCH_STABLE_POLLS"] = str(args.stable_polls)
    env["GHIDRADEC_BATCH_MIN_OUTPUT_BYTES"] = str(args.min_output_bytes)
    env["GHIDRADEC_BATCH_CLEAN_OUTPUT"] = "1"
    env["GHIDRADEC_TEST_SKIP_PARAMID"] = "0" if args.paramid else "1"
    env["GHIDRADEC_TEST_TIMEOUT"] = str(args.timeout)
    if ida_user_dir is not None:
        env["IDAUSR"] = str(ida_user_dir)
        print(f"[ghidradec-batch] staged plugin(s) in IDAUSR={ida_user_dir}")
    if args.ghidra_dir:
        env["GHIDRADEC_BATCH_GHIDRA"] = str(Path(args.ghidra_dir).resolve())
        env["GHIDRADEC_GHIDRA_DIR"] = env["GHIDRADEC_BATCH_GHIDRA"]
        env["GHIDRA_INSTALL_DIR"] = env["GHIDRADEC_BATCH_GHIDRA"]

    ida_args = [str(Path(args.ida).resolve())]
    if args.batch:
        ida_args.append("-A")
    ida_args.extend([f"-L{log_path}", f"-S{IDA_SCRIPT}", str(work_input)])

    print(f"[ghidradec-batch] {input_path} -> {case_dir}")
    exit_code = 0
    try:
        exit_code = run_ida(args, ida_args, env, case_dir)
        if exit_code == 0 and scan_failure_patterns(args, log_path):
            exit_code = 8
        if exit_code == 0 and not validate_output(args, output_path):
            exit_code = 9
        return exit_code
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


def read_input_lists(paths: list[str]) -> list[str]:
    values = []
    for path_text in paths:
        path = Path(path_text)
        for line in path.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if line and not line.startswith("#"):
                values.append(line)
    return values


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--ida", help="Path to idat/idat64/ida/ida64")
    parser.add_argument("--ida-dir", help="IDA 9.3 installation directory; used when --ida is omitted")
    parser.add_argument("--ghidra-dir", help="Ghidra installation used by the plugin")
    parser.add_argument("--work-dir", default=str(ROOT / "build" / "ida-regression"))
    parser.add_argument("--plugin", default="ghidradec64,ghidradec")
    parser.add_argument("--plugin-arg", type=int, default=5, help="Plugin argument; 5 is unattended decompile-all")
    parser.add_argument(
        "--paramid",
        action="store_true",
        help="Enable Ghidra parameter identification during the run; smoke tests skip it by default",
    )
    parser.add_argument("--timeout", type=int, default=600)
    parser.add_argument("--launch-grace-seconds", type=int, default=30)
    parser.add_argument("--stable-polls", type=int, default=3)
    parser.add_argument("--min-output-bytes", type=int, default=64)
    parser.add_argument("--save-database", action="store_true", help="Save the analyzed IDB/I64 before decompiling")
    parser.add_argument("--save-idb", help="Explicit save path for a single input database")
    parser.add_argument("--refresh", action="store_true", help="Refresh copied inputs in the work directory")
    parser.add_argument("--reuse-database", action="store_true", help="Reuse an existing IDB/I64 next to the staged input")
    parser.add_argument("--no-batch", dest="batch", action="store_false")
    parser.add_argument("--dialog-automation", choices=["auto", "off"], default="auto")
    parser.add_argument("--fail-log-pattern", action="append", default=[], help="Additional log substring that fails a run")
    parser.add_argument("--no-default-fail-log-patterns", action="store_true")
    parser.add_argument("--print-log-tail", type=int, default=80)
    parser.add_argument("--input-list", action="append", default=[], help="Newline-delimited input path list")
    parser.add_argument("inputs", nargs="*")
    args = parser.parse_args()

    args.inputs = read_input_lists(args.input_list) + args.inputs
    if not args.inputs:
        raise SystemExit("No inputs were provided.")

    ida = find_ida_executable(args)
    args.ida = str(ida)
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
