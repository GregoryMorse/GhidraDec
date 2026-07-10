from __future__ import print_function

import os
import sys
import time
import traceback

import ida_auto
import ida_funcs
import ida_kernwin
import ida_loader
import ida_nalt
import ida_pro
import idc


MARKER = "<ghidradec_select>"
SMOKE_STATE = None


def log(message):
    print("[ghidradec-smoke] " + message)


def fail(message, code=1):
    log("FAIL: " + message)
    ida_pro.qexit(code)


def env_int(name, default):
    value = os.environ.get(name)
    if not value:
        return default
    try:
        return int(value, 0)
    except ValueError:
        fail("Environment variable {} is not an integer: {}".format(name, value), 2)


def normalize_path(path):
    if not path:
        return path
    return os.path.abspath(os.path.expandvars(os.path.expanduser(path)))


def configure_input_path():
    input_override = normalize_path(os.environ.get("GHIDRADEC_TEST_INPUT_PATH", ""))
    if input_override:
        if not os.path.isfile(input_override):
            fail("GHIDRADEC_TEST_INPUT_PATH does not exist: {}".format(input_override), 2)
        ida_nalt.set_root_filename(input_override)
        log("Using test input path {}".format(input_override))

    input_path = ida_nalt.get_input_file_path()
    if not input_path:
        fail("IDA did not report an input file path", 2)
    log("IDA input path is {}".format(input_path))
    return input_path


def function_size(fnc):
    return int(fnc.end_ea - fnc.start_ea)


def function_index_by_start(start_ea):
    qty = ida_funcs.get_func_qty()
    for i in range(qty):
        fnc = ida_funcs.getn_func(i)
        if fnc is not None and fnc.start_ea == start_ea:
            return i
    return -1


def target_function(target_ea):
    fnc = ida_funcs.get_func(target_ea)
    if fnc is None:
        fail("No function contains target address 0x{:x}".format(target_ea), 3)
    name = idc.get_func_name(fnc.start_ea) or ""
    index = function_index_by_start(fnc.start_ea)
    log("Target address 0x{:x} resolved to {} at 0x{:x} size {} bytes".format(
        target_ea, name, fnc.start_ea, function_size(fnc)))
    return [(index, fnc, name)]


def candidate_functions(start_index, max_functions, min_bytes, max_bytes):
    unnamed = []
    fallback = []
    qty = ida_funcs.get_func_qty()
    for i in range(qty):
        fnc = ida_funcs.getn_func(i)
        if fnc is None:
            continue
        if fnc.flags & (ida_funcs.FUNC_LIB | ida_funcs.FUNC_THUNK):
            continue

        name = idc.get_func_name(fnc.start_ea) or ""
        size = function_size(fnc)
        if size < min_bytes:
            continue
        if max_bytes > 0 and size > max_bytes:
            continue

        item = (i, fnc, name)
        if name.startswith(("sub_", "j_sub_")):
            unnamed.append(item)
        else:
            fallback.append(item)

    merged = []
    seen = set()
    for item in unnamed + fallback:
        ea = item[1].start_ea
        if ea not in seen:
            merged.append(item)
            seen.add(ea)

    selected = merged[start_index:start_index + max_functions]
    if not selected:
        fail("No suitable functions found; total function count is {}".format(qty), 3)
    return selected


def clean_output(output_path):
    if os.environ.get("GHIDRADEC_TEST_CLEAN_OUTPUT", "0") != "1":
        return
    if os.path.exists(output_path):
        os.remove(output_path)


def env_bool(name, default=False):
    value = os.environ.get(name)
    if value is None:
        return default
    return value.strip().lower() not in ("", "0", "false", "no", "off")


def run_plugin_once(plugin_names):
    for plugin_name in plugin_names:
        log("Running plugin {} with regression argument 4".format(plugin_name))
        if ida_loader.load_and_run_plugin(plugin_name, 4):
            return plugin_name
    fail("Could not load/run plugin names: {}".format(", ".join(plugin_names)), 4)


def check_output(output_path, start_time):
    if not os.path.exists(output_path):
        return None
    if os.path.getmtime(output_path) + 1.0 < start_time:
        return None

    size = os.path.getsize(output_path)
    if size < 64:
        fail("Output file is unexpectedly small: {} bytes".format(size), 6)
    return size


def smoke_one(index, fnc, name, output_path, plugin_names):
    old_cmt = ida_funcs.get_func_cmt(fnc, False) or ""
    marker_cmt = old_cmt
    if marker_cmt and not marker_cmt.endswith("\n"):
        marker_cmt += "\n"
    marker_cmt += MARKER

    clean_output(output_path)
    start_time = time.time()
    if not ida_funcs.set_func_cmt(fnc, marker_cmt, False):
        fail("Could not mark function {} at 0x{:x}".format(name, fnc.start_ea), 5)

    try:
        run_plugin_once(plugin_names)
    finally:
        ida_funcs.set_func_cmt(fnc, old_cmt, False)

    size = check_output(output_path, start_time)
    if size is None:
        fail("Plugin did not create output file {}".format(output_path), 6)

    log("OK function #{} {} at 0x{:x}; wrote {} bytes".format(index, name, fnc.start_ea, size))


def start_async_smoke(functions, output_path, plugin_names, timeout_seconds):
    global SMOKE_STATE
    SMOKE_STATE = {
        "functions": functions,
        "output_path": output_path,
        "plugin_names": plugin_names,
        "timeout_seconds": timeout_seconds,
        "current": -1,
        "old_cmt": "",
        "start_time": 0,
        "timer": None,
    }

    def start_next():
        state = SMOKE_STATE
        state["current"] += 1
        if state["current"] >= len(state["functions"]):
            log("PASS: {} function(s) decompiled".format(len(state["functions"])))
            ida_pro.qexit(0)
            return

        index, fnc, name = state["functions"][state["current"]]
        old_cmt = ida_funcs.get_func_cmt(fnc, False) or ""
        marker_cmt = old_cmt
        if marker_cmt and not marker_cmt.endswith("\n"):
            marker_cmt += "\n"
        marker_cmt += MARKER

        clean_output(output_path)
        if not ida_funcs.set_func_cmt(fnc, marker_cmt, False):
            fail("Could not mark function {} at 0x{:x}".format(name, fnc.start_ea), 5)

        state["old_cmt"] = old_cmt
        state["start_time"] = time.time()
        log("Selected {} at 0x{:x} size {} bytes; starting async plugin".format(
            name, fnc.start_ea, function_size(fnc)))
        run_plugin_once(plugin_names)
        ida_funcs.set_func_cmt(fnc, old_cmt, False)

    def poll():
        state = SMOKE_STATE
        index, fnc, name = state["functions"][state["current"]]
        size = check_output(output_path, state["start_time"])
        if size is not None:
            log("OK function #{} {} at 0x{:x}; wrote {} bytes".format(index, name, fnc.start_ea, size))
            start_next()
            return 500

        elapsed = time.time() - state["start_time"]
        if elapsed > state["timeout_seconds"]:
            fail("Plugin did not create output file {} within {} seconds".format(
                output_path, state["timeout_seconds"]), 6)
        return 500

    start_next()
    SMOKE_STATE["timer"] = ida_kernwin.register_timer(500, poll)


def main():
    try:
        log("Waiting for autoanalysis")
        ida_auto.auto_wait()

        ghidra_dir = normalize_path(
            os.environ.get("GHIDRADEC_TEST_GHIDRA", "") or
            os.environ.get("GHIDRADEC_GHIDRA_DIR", "") or
            os.environ.get("GHIDRA_INSTALL_DIR", ""))
        if ghidra_dir:
            os.environ["GHIDRADEC_GHIDRA_DIR"] = ghidra_dir
            os.environ["GHIDRA_INSTALL_DIR"] = ghidra_dir
            log("Using Ghidra path {}".format(ghidra_dir))

        input_path = configure_input_path()
        output_path = input_path + ".c"
        log("Expecting plugin output at {}".format(output_path))

        max_functions = max(1, env_int("GHIDRADEC_TEST_MAX_FUNCS", 1))
        start_index = max(0, env_int("GHIDRADEC_TEST_FUNC_START", 0))
        min_function_bytes = max(0, env_int("GHIDRADEC_TEST_MIN_FUNC_BYTES", 0))
        max_function_bytes = max(0, env_int("GHIDRADEC_TEST_MAX_FUNC_BYTES", 0))
        target_ea_text = os.environ.get("GHIDRADEC_TEST_TARGET_EA", "").strip()
        async_test = env_bool("GHIDRADEC_TEST_ASYNC", False)
        timeout_seconds = max(5, env_int("GHIDRADEC_TEST_OUTPUT_TIMEOUT", 60))
        plugin_names = []
        requested_plugin = os.environ.get("GHIDRADEC_TEST_PLUGIN", "ghidradec")
        for name in (requested_plugin, "ghidradec", "ghidradec64"):
            if name and name not in plugin_names:
                plugin_names.append(name)

        if target_ea_text:
            try:
                target_ea = int(target_ea_text, 0)
            except ValueError:
                fail("GHIDRADEC_TEST_TARGET_EA is not an integer: {}".format(target_ea_text), 2)
            functions = target_function(target_ea)
        else:
            functions = candidate_functions(
                start_index, max_functions, min_function_bytes, max_function_bytes)
        if async_test:
            start_async_smoke(functions, output_path, plugin_names, timeout_seconds)
            return

        for index, fnc, name in functions:
            smoke_one(index, fnc, name, output_path, plugin_names)

        log("PASS: {} function(s) decompiled".format(len(functions)))
        ida_pro.qexit(0)
    except SystemExit:
        raise
    except Exception as exc:
        log("Unhandled exception: {}".format(exc))
        traceback.print_exc()
        ida_pro.qexit(10)


if __name__ == "__main__":
    main()
