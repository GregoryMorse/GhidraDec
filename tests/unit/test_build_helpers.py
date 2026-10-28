import argparse
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
import zipfile
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
import deps
import ida_sdk
import ida_batch


class SdkSelectors(unittest.TestCase):
    def setUp(self):
        self.manifest = ida_sdk.load_manifest()

    def test_aliases_and_exact_tags_are_identical(self):
        for selectors, tag in [
            (("9.3sp1", "9.3.1-sdk.1", "v9.3.1-sdk.1"), "v9.3.1-sdk.1"),
            (("9.4sp1", "9.4SP1", "9.4.0-sdk.1", "v9.4.0-sdk.1"), "v9.4.0-sdk.1"),
            (("9.4", "9.4.0", "v9.4.0-release"), "v9.4.0-release"),
            (("9.5", "v9.5.0-release"), "v9.5.0-release"),
            (("9.2", "v9.2"), "v9.2"),
        ]:
            expected = ida_sdk.find_sdk(self.manifest, selectors[0])
            for selector in selectors:
                with self.subTest(selector=selector):
                    actual = ida_sdk.find_sdk(self.manifest, selector)
                    self.assertEqual(expected, actual)
                    self.assertTrue(actual["url"].endswith(f"/{tag}.zip"))

    def test_future_revision_and_major(self):
        self.assertEqual(
            ida_sdk.find_sdk(self.manifest, "9.6sp2")["version"], "9.6.0-sdk.2")
        self.assertEqual(
            ida_sdk.find_sdk(self.manifest, "10.0")["version"], "10.0")

    def test_invalid_selectors(self):
        for selector in ("../9.4", "9.4sp0", "9.4sp-1", "9.4;echo", "unknown", ""):
            with self.subTest(selector=selector), self.assertRaises(SystemExit):
                ida_sdk.find_sdk(self.manifest, selector)

    def test_revision_directories_do_not_alias_base(self):
        directories = [sdk["directory"] for sdk in self.manifest["idaSdks"]]
        self.assertEqual(len(directories), len(set(directories)))

    def test_matrix_deduplicates_aliases_and_tags(self):
        entries = ida_sdk.selected_matrix_entries(
            self.manifest, versions={"9.4sp1", "v9.4.0-sdk.1", "9.4.0-sdk.1"},
            all_versions=False, include_private=False, current_host=False)
        self.assertEqual(len(entries), 4)
        self.assertEqual({row["ida"] for row in entries}, {"9.4.0-sdk.1"})
        self.assertEqual(len({row["asset_name"] for row in entries}), len(entries))

    def test_major_group_and_all_include_public_revisions(self):
        for versions, all_versions in (({"9"}, False), (set(), True)):
            entries = ida_sdk.selected_matrix_entries(
                self.manifest, versions=versions, all_versions=all_versions,
                include_private=False, current_host=False)
            ids = {row["ida"] for row in entries}
            self.assertTrue({"9.2", "9.3", "9.3.1-sdk.1", "9.4", "9.4.0-sdk.1", "9.5"} <= ids)
            self.assertNotIn("9.0", ids)

    def test_prepared_sdk_does_not_need_archive_or_network(self):
        with tempfile.TemporaryDirectory() as directory:
            marker = Path(directory) / "idasdk95/include/ida.hpp"
            marker.parent.mkdir(parents=True)
            marker.touch()
            args = argparse.Namespace(version="9.5", destination=directory)
            with patch.object(ida_sdk, "download", side_effect=AssertionError("network")):
                ida_sdk.ensure_sdk(args)

    def test_new_source_sdk_layout_is_flattened_with_libraries(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            archive = root / "sdk.zip"
            with zipfile.ZipFile(archive, "w") as sdk_zip:
                sdk_zip.writestr("ida-sdk-tag/src/include/ida.hpp", "header")
                sdk_zip.writestr("ida-sdk-tag/src/lib/x64_win_64/ida.lib", "library")
            sdk = ida_sdk.extract_archive(archive, root / "cache", "idasdk95")
            self.assertTrue((sdk / "include/ida.hpp").exists())
            self.assertTrue((sdk / "lib/x64_win_64/ida.lib").exists())

    @unittest.skipUnless(shutil.which("cmake"), "CMake not installed")
    def test_cmake_resolves_same_metadata(self):
        with tempfile.TemporaryDirectory() as directory:
            script = Path(directory) / "resolve.cmake"
            for selector in ("9.0sp1", "9.3sp1", "v9.4.0-sdk.1", "9.4sp1", "9.6sp2", "10.0"):
                expected = ida_sdk.find_sdk(self.manifest, selector)
                script.write_text(
                    f'set(CMAKE_SOURCE_DIR "{ROOT.as_posix()}")\n'
                    f'include("{ROOT.as_posix()}/cmake/GhidraDecManifest.cmake")\n'
                    f'ghidradec_manifest_find_ida_sdk("{selector}")\n'
                    'message(STATUS "SDK=${GHIDRADEC_IDA_SDK_VERSION}|${GHIDRADEC_IDA_SDK_DIRECTORY}|${GHIDRADEC_IDA_SDK_ARCHIVE}")\n',
                    encoding="utf-8")
                result = subprocess.run(
                    ["cmake", f"-DPython3_EXECUTABLE={sys.executable}", "-P", str(script)],
                    text=True, capture_output=True, timeout=90)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertIn(
                    f'SDK={expected["version"]}|{expected["directory"]}|{expected["archive"]}',
                    result.stdout)


class GhidraProvisioning(unittest.TestCase):
    def test_version_change_replaces_generated_subset(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source/cpp"
            source.mkdir(parents=True)
            (source / "address.cc").write_text("address", encoding="utf-8")
            (source / "sleigh.cc").write_text("12.1", encoding="utf-8")
            output = root / "generated"
            args = argparse.Namespace(
                version="12.1.2", source_dir=str(source), output_dir=str(output), refresh=False, url=None)
            deps.ensure_ghidra_sources(args)
            (source / "sleigh.cc").write_text("12.2", encoding="utf-8")
            (output / "decompile/obsolete.cc").touch()
            args.version = "12.2"
            deps.ensure_ghidra_sources(args)
            self.assertEqual((output / "decompile/sleigh.cc").read_text(), "12.2")
            self.assertFalse((output / "decompile/obsolete.cc").exists())
            stamp = json.loads((output / "source-version.json").read_text())
            self.assertEqual(stamp["version"], "12.2")
            with patch.object(deps.shutil, "copy2", side_effect=AssertionError("cache not reused")):
                deps.ensure_ghidra_sources(args)

    def test_unversioned_sources_are_not_reused(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source"
            source.mkdir()
            for name in ("address.cc", "sleigh.cc"):
                (source / name).write_text("new", encoding="utf-8")
            output = root / "generated"
            (output / "decompile").mkdir(parents=True)
            (output / "decompile/sleigh.cc").write_text("old", encoding="utf-8")
            deps.ensure_ghidra_sources(argparse.Namespace(
                version="12.2", source_dir=str(source), output_dir=str(output), refresh=False, url=None))
            self.assertEqual((output / "decompile/sleigh.cc").read_text(), "new")


class OutputQuality(unittest.TestCase):
    def test_missing_case_value_is_not_a_pass(self):
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory) / "output.c"
            log = Path(directory) / "ida.log"
            output.write_text("switch (x) {\n case : break;\n}", encoding="utf-8")
            self.assertEqual(ida_batch.classify_run_result(0, log, output)[0], "graceful_fail")
            output.write_text("switch (x) {\n case 7: break;\n default: break;\n}", encoding="utf-8")
            self.assertEqual(ida_batch.classify_run_result(0, log, output)[0], "success")


if __name__ == "__main__":
    unittest.main()
