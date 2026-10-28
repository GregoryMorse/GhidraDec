from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]
NATIVE = ROOT / "build/deps/ghidra-decompiler/Ghidra"


@unittest.skipUnless((NATIVE / "ghidra_arch.cc").exists(), "Ghidra sources not provisioned")
class ProtocolIds(unittest.TestCase):
    def test_query_ids_match_upstream(self):
        pattern = r'ElementId\("(command_[a-z]+)",\s*(\d+)\)'
        native = dict(re.findall(pattern, (NATIVE / "ghidra_arch.cc").read_text(encoding="utf-8")))
        bridge = dict(re.findall(pattern, (ROOT / "src/sleighinterface.cpp").read_text(encoding="utf-8")))
        self.assertGreaterEqual(len(native), 19)
        for name, identifier in native.items():
            with self.subTest(element=name):
                self.assertEqual(bridge.get(name), identifier)

    def test_option_ids_match_upstream(self):
        pattern = r'ElementId\("([a-z0-9_]+)",\s*(\d+)\)'
        native = dict(re.findall(pattern, (NATIVE / "options.cc").read_text(encoding="utf-8")))
        bridge = dict(re.findall(pattern, (ROOT / "src/sleighinterface.cpp").read_text(encoding="utf-8")))
        overlap = native.keys() & bridge.keys()
        self.assertGreaterEqual(len(overlap), 20)
        for name in overlap:
            with self.subTest(element=name):
                self.assertEqual(bridge[name], native[name])
