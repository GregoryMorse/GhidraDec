from __future__ import print_function

import os
import time
import traceback

import ida_auto
import ida_kernwin
import ida_loader
import ida_nalt
import ida_pro


STATE = None


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


def output_ready(path, start_time, min_bytes):
    if not os.path.exists(path):
        return False, 0
    if os.path.getmtime(path) + 1.0 < start_time:
        return False, 0
    size = os.path.getsize(path)
    return size >= min_bytes, size


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


def start_output_poll(output_path, start_time, timeout_seconds, stable_polls, min_bytes):
    global STATE
    STATE = {
        "output_path": output_path,
        "start_time": start_time,
        "timeout_seconds": timeout_seconds,
        "stable_polls": stable_polls,
        "min_bytes": min_bytes,
        "last_size": -1,
        "stable_count": 0,
    }

    def poll():
        state = STATE
        ready, size = output_ready(state["output_path"], state["start_time"], state["min_bytes"])
        if ready:
            if size == state["last_size"]:
                state["stable_count"] += 1
            else:
                state["stable_count"] = 1
                state["last_size"] = size
            if state["stable_count"] >= state["stable_polls"]:
                log("PASS: decompile-all output is {} bytes".format(size))
                ida_pro.qexit(0)
                return -1

        elapsed = time.time() - state["start_time"]
        if elapsed > state["timeout_seconds"]:
            fail(
                "Plugin did not create stable output {} within {} seconds".format(
                    state["output_path"], state["timeout_seconds"]),
                7,
            )
        return 1000

    ida_kernwin.register_timer(1000, poll)


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

        output_path = normalize_path(os.environ.get("GHIDRADEC_BATCH_OUTPUT", "")) or (input_path + ".c")
        if os.environ.get("GHIDRADEC_BATCH_CLEAN_OUTPUT", "1") not in ("0", "false", "False"):
            if os.path.exists(output_path):
                os.remove(output_path)

        timeout_seconds = max(10, env_int("GHIDRADEC_BATCH_TIMEOUT", 600))
        stable_polls = max(1, env_int("GHIDRADEC_BATCH_STABLE_POLLS", 3))
        min_bytes = max(1, env_int("GHIDRADEC_BATCH_MIN_OUTPUT_BYTES", 64))
        plugin_argument = env_int("GHIDRADEC_BATCH_PLUGIN_ARG", 1)

        start_time = time.time()
        run_plugin(plugin_argument)
        ready, size = output_ready(output_path, start_time, min_bytes)
        if ready and stable_polls <= 1:
            log("PASS: decompile-all output is {} bytes".format(size))
            ida_pro.qexit(0)
            return

        start_output_poll(output_path, start_time, timeout_seconds, stable_polls, min_bytes)
    except SystemExit:
        raise
    except Exception as exc:
        log("Unhandled exception: {}".format(exc))
        traceback.print_exc()
        ida_pro.qexit(10)


if __name__ == "__main__":
    main()
