# Changelog

All notable user-facing changes are tracked here.

## v1.0 - 2026-07-03

This is the first major release after `v0.4`. The project has been substantially
modernized and is now targeting current Ghidra and IDA releases while retaining
wide historical IDA SDK compatibility.

### Highlights

* Updated the primary target to Ghidra 12.1.x and IDA SDK 9.3.
* Added manifest-driven support for IDA SDKs from 6.8 through 9.3, plus a
  moving `latest` target for the public Hex-Rays SDK branch.
* Added CI/release workflows that build IDA 9.3 by default and allow older SDK
  release artifacts to be generated on demand from official tags.
* Reworked dependency handling so Ghidra decompiler sources, RetDec, and
  jsoncpp are fetched/extracted into build-local dependency trees instead of
  being maintained as stale root-level source.
* Updated third-party dependency pins to RetDec v5.0 and jsoncpp v1.9.8.
* Added a Python build helper with optional post-build deployment to the IDA
  user plugin directory.
* Added project logo/icon assets and integrated the plugin icon with IDA.

### Decompiler Integration

* Updated the Ghidra decompiler bridge for the current 12.1 protocol.
* Added handling for newer packed decompiler query elements and additional
  decompiler protocol commands.
* Improved mapped-symbol lookup so IDA names are preserved instead of falling
  back to synthetic `func_0x...`/`ram0x...` names when live callbacks are
  available.
* Improved parameter-identification handling and database update behavior.
* Improved import handling so decompile-all runs survive imports gracefully.
* Added or improved processor/language coverage for x86, ARM, AArch64, MIPS,
  PowerPC, RISC-V, Java, and Dalvik where IDA and Ghidra support overlap.
* Improved switch and control-flow emission for newer Ghidra output.

### User Interface

* Modernized normal logging by separating user-facing messages from trace
  diagnostics. Enable `GHIDRADEC_TRACE=1` for verbose regression/debug output.
* Enabled IDB backup by default for normal builds before decompilation.
* Reworked the configuration dialogs into focused display and decompiler
  behavior option panels.
* Exposed Ghidra decompiler behavior settings such as jump-table limits,
  maximum instructions, alias blocking, datatype splitting, namespace display,
  brace formatting, NaN handling, and prototype evaluation defaults.
* Improved the decompiler viewer, token coloring, navigation behavior, and
  decompiler output prologue generation.
* Fixed several GUI crashes around decompiled-output navigation, symbol
  interaction, graph display, and main-thread callback failures.
* Continued work on the experimental structured C graph view. It is useful for
  inspection, but still considered a power-user feature while layout and edge
  polish continue.

### Build And Packaging

* Replaced the legacy root-folder build flow with CMake-first builds under
  `build/matrix/ida-<version>`.
* Moved maintained Visual Studio project assets under `ide/vs`.
* Added Linux and macOS compile/package support for the portable build path.
* Added packaging targets that produce versioned plugin ZIPs for release
  uploads.
* Added release publishing logic that skips assets already attached to the
  GitHub Release, allowing historical IDA SDK artifacts to be backfilled once
  and then reused by everyone.

### Testing

* Added an unattended IDA batch regression harness for selective decompilation,
  decompile-all, per-function decompile-all, saved database reuse, and
  dangerous-failure detection.
* Added corpus tooling around public test binaries, with `angr/binaries` as the
  primary broad-architecture corpus.
* Added quality gates for dangerous failures such as pipe breakage, crashes,
  fake symbol names, unsupported protocol queries, and malformed output.
* Added documentation for local licensed IDA regression testing in
  `docs/ida-regression-testing.md`.

### Known Limitations

* Public GitHub-hosted CI is compile/package-only because IDA execution requires
  a licensed local or self-hosted installation.
* The current unattended IDA regression automation is Windows-first. Linux and
  macOS builds are supported, but equivalent no-human-input IDA GUI automation
  still needs validation.
* A few public corpus entries remain quality-investigation targets rather than
  enabled release blockers, notably MIPS switch-heavy jump-table recovery and
  tiny RISC-V relocatable jump/branch object files.
* The structured C graph view remains experimental.

## v0.4 - 2019-era baseline

Previous public release. This release is kept as the comparison point for the
v1.0 modernization work.
