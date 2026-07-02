from __future__ import print_function

import os
import time
import traceback

import ida_auto
import ida_funcs
import ida_kernwin
import ida_loader
import ida_name
import ida_nalt
import ida_pro

try:
    from PySide6 import QtCore
except Exception:
    QtCore = None


def log(message):
    print("[ghidradec-batch] " + message)


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
        fail("{} is not an integer: {}".format(name, value), 2)


def normalize_path(path):
    if not path:
        return path
    return os.path.abspath(os.path.expandvars(os.path.expanduser(path)))


def save_scanned_database(input_path):
    save_path = normalize_path(os.environ.get("GHIDRADEC_BATCH_SAVE_IDB", ""))
    if not save_path:
        return
    parent = os.path.dirname(save_path)
    if parent and not os.path.isdir(parent):
        os.makedirs(parent)
    log("Saving analyzed database to {}".format(save_path))
    if not ida_loader.save_database(save_path, 0):
        fail("IDA could not save analyzed database for {}".format(input_path), 5)


def function_name(function):
    name = ida_name.get_name(function.start_ea)
    return name or "sub_{:X}".format(function.start_ea)


def write_function_list(path):
    import json

    functions = []
    for index in range(ida_funcs.get_func_qty()):
        function = ida_funcs.getn_func(index)
        if function is None:
            continue
        functions.append({
            "ea": int(function.start_ea),
            "ea_hex": "0x{:x}".format(function.start_ea),
            "end_ea": int(function.end_ea),
            "name": function_name(function),
        })
    parent = os.path.dirname(path)
    if parent and not os.path.isdir(parent):
        os.makedirs(parent)
    with open(path, "w", encoding="utf-8") as handle:
        json.dump({"functions": functions}, handle, indent=2, sort_keys=True)
    log("Wrote {} functions to {}".format(len(functions), path))


def select_function_for_plugin():
    select_ea_text = os.environ.get("GHIDRADEC_BATCH_SELECT_EA", "").strip()
    select_name = os.environ.get("GHIDRADEC_BATCH_SELECT_NAME", "").strip()
    if not select_ea_text and not select_name:
        return

    select_ea = None
    if select_ea_text:
        try:
            select_ea = int(select_ea_text, 0)
        except ValueError:
            fail("GHIDRADEC_BATCH_SELECT_EA is not an integer: {}".format(select_ea_text), 2)

    selected = None
    for index in range(ida_funcs.get_func_qty()):
        function = ida_funcs.getn_func(index)
        if function is None:
            continue
        current_name = function_name(function)
        comment = ida_funcs.get_func_cmt(function, False) or ""
        cleaned = "\n".join(
            line for line in comment.splitlines()
            if "<ghidradec_select>" not in line
        ).strip()
        if cleaned != comment:
            ida_funcs.set_func_cmt(function, cleaned, False)
        if selected is None and (
            (select_ea is not None and int(function.start_ea) == select_ea)
            or (select_name and current_name == select_name)
        ):
            selected = function

    if selected is None:
        fail("Selected function was not found: ea={} name={}".format(select_ea_text, select_name), 8)

    current = ida_funcs.get_func_cmt(selected, False) or ""
    marker = "<ghidradec_select>"
    updated = (current + "\n" + marker).strip() if current else marker
    ida_funcs.set_func_cmt(selected, updated, False)
    log("Selected function {} @ 0x{:x}".format(function_name(selected), selected.start_ea))


def output_ready(path, start_time, min_bytes):
    if not os.path.exists(path):
        return False, 0
    if os.path.getmtime(path) + 1.0 < start_time:
        return False, 0
    size = os.path.getsize(path)
    return size >= min_bytes, size


def done_ready(path, start_time):
    if not path:
        return True
    if not os.path.exists(path):
        return False
    if os.path.getmtime(path) + 1.0 < start_time:
        return False
    return True


def plugin_names():
    names = []
    configured = os.environ.get("GHIDRADEC_BATCH_PLUGIN", "ghidradec64")
    for name in configured.split(",") + ["ghidradec64", "ghidradec"]:
        name = name.strip()
        if name and name not in names:
            names.append(name)
    return names


def run_plugin(argument):
    for name in plugin_names():
        log("Running plugin {} with argument {}".format(name, argument))
        if ida_loader.load_and_run_plugin(name, argument):
            return name
    fail("Could not load/run plugin candidates: {}".format(", ".join(plugin_names())), 6)


def pump_ui_events():
    if QtCore is not None:
        QtCore.QCoreApplication.processEvents(QtCore.QEventLoop.AllEvents, 100)
    time.sleep(0.05)


def wait_for_output(output_path, done_path, start_time, timeout_seconds, stable_polls, min_bytes):
    last_size = -1
    stable_count = 0
    last_log_second = -1
    while True:
        pump_ui_events()
        ready, size = output_ready(output_path, start_time, min_bytes)
        if ready:
            if size == last_size:
                stable_count += 1
            else:
                stable_count = 1
                last_size = size
            if stable_count >= stable_polls and done_ready(done_path, start_time):
                log("Output ready: decompile output is {} bytes".format(size))
                ida_pro.qexit(0)
                return

        elapsed = time.time() - start_time
        elapsed_second = int(elapsed)
        if elapsed_second != last_log_second and elapsed_second > 0 and elapsed_second % 30 == 0:
            last_log_second = elapsed_second
            log("Waiting for stable output {} ({}s elapsed)".format(output_path, elapsed_second))
        if elapsed > timeout_seconds:
            fail(
                "Plugin did not create stable output {} within {} seconds".format(
                    output_path, timeout_seconds),
                7,
            )


def wait_for_done(done_path, start_time, timeout_seconds):
    last_log_second = -1
    while True:
        pump_ui_events()
        if done_ready(done_path, start_time):
            log("Done marker ready: {}".format(done_path))
            ida_pro.qexit(0)
            return

        elapsed = time.time() - start_time
        elapsed_second = int(elapsed)
        if elapsed_second != last_log_second and elapsed_second > 0 and elapsed_second % 30 == 0:
            last_log_second = elapsed_second
            log("Waiting for done marker {} ({}s elapsed)".format(done_path, elapsed_second))
        if elapsed > timeout_seconds:
            fail(
                "Plugin did not create done marker {} within {} seconds".format(
                    done_path, timeout_seconds),
                7,
            )


def main():
    try:
        input_override = normalize_path(os.environ.get("GHIDRADEC_BATCH_INPUT", ""))
        if input_override:
            if not os.path.isfile(input_override):
                fail("GHIDRADEC_BATCH_INPUT does not exist: {}".format(input_override), 2)
            ida_nalt.set_root_filename(input_override)

        input_path = ida_nalt.get_input_file_path()
        if not input_path:
            fail("IDA did not report an input file path", 2)
        log("IDA input path is {}".format(input_path))

        ghidra_dir = normalize_path(
            os.environ.get("GHIDRADEC_BATCH_GHIDRA", "")
            or os.environ.get("GHIDRADEC_GHIDRA_DIR", "")
            or os.environ.get("GHIDRA_INSTALL_DIR", ""))
        if ghidra_dir:
            os.environ["GHIDRADEC_GHIDRA_DIR"] = ghidra_dir
            os.environ["GHIDRA_INSTALL_DIR"] = ghidra_dir
            log("Using Ghidra path {}".format(ghidra_dir))

        log("Waiting for autoanalysis")
        ida_auto.auto_wait()
        save_scanned_database(input_path)

        list_functions_path = normalize_path(os.environ.get("GHIDRADEC_BATCH_LIST_FUNCTIONS", ""))
        if list_functions_path:
            write_function_list(list_functions_path)
            ida_pro.qexit(0)
            return

        select_function_for_plugin()

        output_path = normalize_path(os.environ.get("GHIDRADEC_BATCH_OUTPUT", "")) or (input_path + ".c")
        done_path = normalize_path(os.environ.get("GHIDRADEC_BATCH_DONE", ""))
        if os.environ.get("GHIDRADEC_BATCH_CLEAN_OUTPUT", "1") not in ("0", "false", "False"):
            if os.path.exists(output_path):
                os.remove(output_path)
            if done_path and os.path.exists(done_path):
                os.remove(done_path)

        timeout_seconds = max(10, env_int("GHIDRADEC_BATCH_TIMEOUT", 600))
        stable_polls = max(1, env_int("GHIDRADEC_BATCH_STABLE_POLLS", 3))
        min_bytes = max(1, env_int("GHIDRADEC_BATCH_MIN_OUTPUT_BYTES", 64))
        plugin_argument = env_int("GHIDRADEC_BATCH_PLUGIN_ARG", 5)
        gui_like = os.environ.get("GHIDRADEC_TEST_GUI_LIKE", "") not in ("", "0", "false", "False")

        start_time = time.time()
        run_plugin(plugin_argument)
        if gui_like:
            if not done_path:
                fail("GHIDRADEC_TEST_GUI_LIKE requires GHIDRADEC_BATCH_DONE", 2)
            wait_for_done(done_path, start_time, timeout_seconds)
            return
        ready, size = output_ready(output_path, start_time, min_bytes)
        if ready and stable_polls <= 1 and done_ready(done_path, start_time):
            log("Output ready: decompile output is {} bytes".format(size))
            ida_pro.qexit(0)
            return

        wait_for_output(output_path, done_path, start_time, timeout_seconds, stable_polls, min_bytes)
    except SystemExit:
        raise
    except Exception as exc:
        log("Unhandled exception: {}".format(exc))
        traceback.print_exc()
        ida_pro.qexit(10)


if __name__ == "__main__":
    main()
