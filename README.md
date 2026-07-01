<p align="center">
  <img src="docs/assets/GhidraDec-light-logo.png" alt="GhidraDec" width="720">
</p>

# GhidraDec

GhidraDec is an IDA Pro plugin that uses Ghidra's native decompiler engine from
inside IDA. It is intended for analysts who want IDA's database, loaders,
processor modules, navigation, and scripting ecosystem while also having access
to Ghidra decompilation for processors where Hex-Rays is unavailable or where a
second decompiler view is useful.

The current mainline target is Ghidra 12.1.x and IDA SDK 9.3. The repository
also carries compatibility targets for older IDA SDKs through the manifest in
`ghidradec.targets.json`.

Author: Gregory Morse.

## Status

* Primary local development target: IDA Pro 9.3, EA64.
* Default CI/release target: IDA SDK 9.3 only.
* Older IDA SDK artifacts are generated on demand from tagged releases.
* Build system: CMake, with Visual Studio project files kept under `ide/vs`.
* Runtime decompiler source: generated under `build/deps/ghidra-decompiler`;
  generated third-party source trees are not maintained at the repository root.
* IDA-backed regression testing is intended for local or self-hosted licensed
  runners. Public GitHub-hosted CI is compile/package-only.

## Installation

1. Install or extract a supported Ghidra release.
2. Install the plugin binary from a release package, or build it locally.
3. Copy the plugin into IDA's plugin directory.

Typical per-user IDA plugin directory on Windows:

```text
%APPDATA%\Hex-Rays\IDA Pro\plugins
```

The local build helper can attempt this copy after a successful build:

```powershell
python tools\build.py --ida-version 9.3 --deploy
```

Use `--deploy-dir <plugins-dir>` for a custom destination. Deployment is
best-effort: if IDA has the plugin loaded or the destination is not writable, the
build completes and prints a warning.

## Runtime Configuration

Open IDA and run the GhidraDec configuration action. Configure the Ghidra folder
when `decompile`/`decompile.exe` is not on `PATH`.

Useful environment variables:

* `GHIDRA_INSTALL_DIR`: extracted Ghidra runtime.
* `GHIDRADEC_GHIDRA_DIR`: alternate Ghidra runtime override.
* `GHIDRADEC_TRACE=1`: enables verbose diagnostic trace messages.
* `GHIDRADEC_PROTOCOL_LOG=<path>`: records decompiler protocol traffic for
  debugging regression failures.

By default, normal builds back up the IDA database before decompilation. Debug
and trace-default sessions skip that default for faster iteration unless the
option is explicitly enabled in the plugin settings.

## Basic Usage

The plugin's preferred hotkey is `Ctrl+G`, which invokes selective
decompilation for the function at the current cursor position.

The plugin also supports additional invocation arguments, which can be bound in
IDA's `plugins.cfg` when you want explicit hotkeys or menu entries:

| Argument | Mode |
| --- | --- |
| `0` | Selective decompilation for the current function. |
| `1` | Full decompile-all to a selected output file. |
| `2` | Plugin configuration. |
| `3` | Generate the intermediate decompiler database only. |
| `4` | Regression selective decompilation using a marked function. |
| `5` | Unattended regression decompile-all. |

Example `plugins.cfg` entries:

```text
; Plugin_name                    File_name     Hotkey        Arg
; -----------------------------------------------------------------
Ghidra_Decompiler                ghidradec64    Ctrl-G        0
Ghidra_Decompiler_All            ghidradec64    Ctrl-Shift-G  1
Ghidra_Decompiler_Configuration  ghidradec64    Ctrl-Shift-C  2
```

Use `ghidradec` instead of `ghidradec64` when binding an EA32 plugin build.

## Build Requirements

* C++17 compiler.
* CMake 3.19 or newer.
* IDA SDK selected from `ghidradec.targets.json`.
* Ghidra source archive or local Ghidra source/install for generated decompiler
  support sources.
* Bison only when regenerating parser sources; normal builds use checked-in
  generated parser outputs.

Windows builds use MSVC. Linux and macOS builds are supported by the CMake path.

## Local Build

The manifest-driven helper is the usual path:

```powershell
python tools\build.py --ida-version 9.3
```

By default this writes to:

```text
build/matrix/ida-9.3
```

Build against the moving public Hex-Rays SDK branch with:

```powershell
python tools\build.py --ida-version latest
```

Manual CMake equivalent:

```powershell
python tools\ida_sdk.py ensure --version 9.3
python tools\deps.py ensure-ghidra-sources
cmake -S . -B build/matrix/ida-9.3 -DGHIDRADEC_IDA_VERSION=9.3
cmake --build build/matrix/ida-9.3 --config Release --parallel
```

Package the built plugins with:

```powershell
cmake --build build/matrix/ida-9.3 --config Release --target package_plugins
```

## CMake Options

* `GHIDRADEC_IDA_VERSION`: IDA SDK version from `ghidradec.targets.json`.
* `IDA_SDK_DIR`: explicit EA64 SDK path override.
* `IDA_SDK_DIR32`: explicit legacy EA32 SDK path override.
* `GHIDRADEC_AUTO_DEPLOY`: opt-in post-build plugin copy.
* `GHIDRADEC_DEPLOY_DIR`: deployment destination; on Windows defaults to the
  per-user IDA plugin directory.
* `GHIDRADEC_REFRESH_GHIDRA_SOURCES`: refresh generated Ghidra support sources
  from a local source/install tree.
* `GHIDRA_INSTALL_DIR`: local extracted Ghidra runtime/source root.
* `GHIDRA_DECOMPILER_CPP_DIR`: explicit Ghidra decompiler C++ source directory.
* `IDA_PATH`: optional install root used by the CMake `install` target.

Do not commit machine-specific SDK, IDA, or Ghidra paths.

## Visual Studio

The maintained Visual Studio files live under `ide/vs`. MSVC build products are
redirected under `build/msvc` by `Directory.Build.props`.

Example:

```powershell
msbuild ide\vs\GhidraDec.vcxproj /p:Configuration=Release /p:Platform=x64 /p:IDA_SDK_DIR=<path-to-idasdk93>
```

## Release Workflow

The normal release workflow builds IDA 9.3 artifacts by default. Other supported
IDA versions are intended to be generated on demand from official tagged
releases, then attached permanently to the release for users who need those
specific SDK targets.

This avoids building every historical SDK on every release while still allowing
the project to accumulate version-specific artifacts over time.

## Regression Testing

IDA GUI/batch regression testing requires a licensed IDA installation and is not
appropriate for public GitHub-hosted runners.

The current unattended corpus battery is Windows-only in practice. The scripts
can locate and launch IDA on other platforms, but the dialog/crash-window
automation that keeps batch runs human-free currently uses Win32 APIs. Linux and
macOS IDA regression runners should be considered future work until equivalent
automation is added and validated.

The current batch harness is documented in:

```text
docs/ida-regression-testing.md
```

The corpus tooling stages public samples such as `angr/binaries`, then launches
IDA in batch mode, waits for analysis, optionally saves the scanned database,
and runs GhidraDec decompile-all. Current focus is broad architecture coverage
starting with x86_64, x86_32, and x86_16, then ARM/AArch64/MIPS and other
processors where IDA and Ghidra overlap.

Example local battery:

```powershell
python tools\ida_corpus_battery.py --corpus angr-binaries --arch x86_64,x86_32,x86_16 --tier smoke,extended
```

## Third-Party Code

Dependency versions and source archives are pinned in `ghidradec.targets.json`.

Current notable dependencies:

* Ghidra 12.1.x decompiler support sources.
* RetDec v5.0 config/common/serdes/utils libraries.
* jsoncpp v1.9.8.

Generated dependency source is placed under `build/deps` and should not be
committed as root-level source.

## License

Copyright (c) 2019-2026 Gregory Morse.

GhidraDec is licensed under the MIT license. See `LICENSE` for details.
Portions of the IDA plugin code derive from the RetDec IDA plugin, copyright
(c) 2017 Avast Software, also under the MIT license.

Third-party notices are tracked in `LICENSE-THIRD-PARTY` where applicable.
