import importlib.util
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import MagicMock, patch


ROOT = Path(__file__).resolve().parents[2]


class GuiHarness(unittest.TestCase):
    def setUp(self):
        names = ("ida_auto", "ida_funcs", "ida_kernwin", "ida_loader", "ida_name",
                 "ida_nalt", "ida_pro", "ida_segment")
        modules = {name: MagicMock() for name in names}
        modules["PySide6"] = None
        spec = importlib.util.spec_from_file_location(
            "ida_gui_harness_test", ROOT / "tools/ida_batch_decompile_all.py")
        self.harness = importlib.util.module_from_spec(spec)
        with patch.dict(sys.modules, modules):
            spec.loader.exec_module(self.harness)

    def test_wait_for_done_does_not_exit_before_next_invocation(self):
        harness = self.harness
        with patch.object(harness, "done_ready", return_value=True), \
                patch.object(harness, "pump_ui_events"), patch.object(harness, "log"):
            harness.wait_for_done("done", 0, 10)
        harness.ida_pro.qexit.assert_not_called()

    def test_repeat_clears_marker_and_exits_once_after_all_invocations(self):
        harness = self.harness
        harness.ida_nalt.get_input_file_path.return_value = "public-input"
        with tempfile.TemporaryDirectory() as directory:
            done = Path(directory) / "output.c.done"
            done.write_text("stale", encoding="utf-8")

            def complete(_argument):
                self.assertFalse(done.exists(), "a previous completion must not satisfy the next run")
                done.write_text("done", encoding="utf-8")

            with (
                patch.dict(os.environ, {
                    "GHIDRADEC_TEST_GUI_LIKE": "1", "GHIDRADEC_BATCH_GUI_REPEATS": "3",
                    "GHIDRADEC_BATCH_DONE": str(done), "GHIDRADEC_BATCH_PLUGIN_ARG": "4",
                    "GHIDRADEC_BATCH_OUTPUT": str(Path(directory) / "output.c"),
                }, clear=True),
                patch.object(harness, "seed_text_function_if_empty"),
                patch.object(harness, "save_scanned_database"),
                patch.object(harness, "select_function_for_plugin"),
                patch.object(harness, "pump_ui_events"),
                patch.object(harness, "log"),
                patch.object(harness, "run_plugin", side_effect=complete) as run,
            ):
                harness.main()
            self.assertEqual(run.call_count, 3)
            harness.ida_pro.qexit.assert_called_once_with(0)


if __name__ == "__main__":
    unittest.main()
