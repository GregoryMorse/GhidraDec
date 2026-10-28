# Minimal Windows IDA Runtime Grid

This is a risk-based installation recommendation, not a claim that these
versions have all been runtime-certified. Continue compiling **every supported
SDK** from `ghidradec.targets.json`; a successful build of one SDK is not proof
of binary compatibility with another IDA release. Use the matching plugin
package on each installed IDA version. Use Ghidra **12.2** everywhere.

## Recommended Grid

Eleven installations cover the meaningful compatibility branches currently in
`src/defs.h`, `src/defs.cpp`, `src/idaplugin.cpp`, and `src/decompiler.cpp`.
This is deliberately more selective than one install per point release, but
more cautious than one per major version:

| IDA | Why Retain This Runtime | Required Address Modes |
| --- | --- | --- |
| 6.8 | Oldest supported SDK; legacy names, buffers, Qt/viewer APIs and 32-bit host process. | EA32 and EA64 |
| 6.95 | Last legacy API generation; exercises the 6.95-specific adapter split independently of 6.8. | EA32 and EA64 |
| 7.0 x64 | First new 7.x API/64-bit host boundary; plugin entry signature, instruction, range and widget changes. | EA32 and EA64 |
| 7.2 | First native anonymous-UDT/stack-point adapter path; still before the 7.3 inf-accessor transition. | EA32 and EA64 |
| 7.4 | Represents the distinct 7.3-7.4 inf-accessor shim while retaining the old plugin lifecycle. Also covers the Python 3 transition for automation. | EA32 and EA64 |
| 7.5 | First plugmod/PLUGIN_MULTI and event-listener lifecycle; still uses pre-7.6 flag/bitness and pre-7.7 stack-argument handling. | EA32 and EA64 |
| 7.7 | Covers both the 7.6 bitness/flag changes and 7.7 stack-argument-area path; representative late 7.x runtime. | EA32 and EA64 |
| 8.4 Pro | Last pre-8.5 struct/enum generation, including the 8.4 UDT/member adapters; representative 8.0-8.4 runtime. | EA32 and EA64 |
| 8.5 | Dedicated EA64-only build/type-system and graph/execute-request adapter boundary; do not fold this into 8.4 or 9.x. | EA64; PE32 and PE32+ inputs |
| 9.1 | Represents 9.0/9.0sp1/9.1 before the 9.2 calling-convention and processor API changes. | EA64; PE32 and PE32+ inputs |
| 9.5 | Current public SDK/runtime; covers the 9.2+ calling-convention adapters and 9.3+ calls-view fallback. | EA64; PE32 and PE32+ inputs |

The 9.0-9.5 family is **not entirely one code path** in this plugin. Retaining
9.1 and 9.5 is the smallest sensible split while the 9.2 boundary exists.
The existing licensed 9.3 installation remains our immediate development
runtime; do not call 9.5 runtime-certified just because its SDK builds.

If eleven installations are initially impractical, start with **6.95, 7.4,
7.7, 8.4, 8.5, 9.1 and 9.5**, retaining the existing 9.3 test machine. That
seven-version budget grid leaves the 6.8, first-7.0 and first-7.5 runtime paths
compile-only; record that limitation in release results.

## What To Run

For every row, load a fresh redistributable PE32 and PE32+ input where that IDA
edition supports them. Include a small ELF x86-64 input and an ARM/AArch64 input
on the newest runtime; broader processor coverage belongs to the corpus suite,
not to a Cartesian product of every old IDA and every processor.

1. Plugin load, configuration, unload and reopen with no crash.
2. Ctrl+G inside a non-entry instruction of a nontrivial function; repeat it,
   navigate calls/xrefs, rename a symbol, and open/close the code and graph views.
3. Decompile-all and a selective run, with parameter identification both off
   and on; check diagnostics and output, not merely file existence.
4. A text/batch run with no test workaround, followed by a second invocation.
   Require a done marker, clean process exit and useful output within a timeout.
5. On the newest installed version, repeat with live callbacks to avoid cache
   preloads hiding callback bugs.

Use IDC for old releases that cannot run the modern Python harness. EA32/EA64
are database/plugin address widths, not the input's instruction-set bitness.
A modern EA64-only IDA must still decompile 32-bit machine code.

Before a major compatibility change, add the immediately adjacent releases
around the changed adapter. Add an 8.4 Teams runtime if certifying Teams-specific
behavior, and an actual 9.0/9.0sp1 runtime if changing idalib startup or licensing.
SDK-only revisions need compile/package coverage but normally do not need
another licensed product installation.

## When Headless Started

**IDA 9.0 was the first release with `idalib`**, the library API for embedding
IDA's analysis engine in a standalone C++/Python process. See the
[IDA 9.0 release notes](https://docs.hex-rays.com/release-notes/9_0).

That is different from text-mode/batch automation. `idat` (previously
`idaw`/`idal`) and `-A -S` scripting predate 9.0 and are available at our
oldest supported baseline, **6.8**; this does not mean batch mode was introduced
in 6.8. Hex-Rays documents terminal-free `TVHEADLESS=1 idat -A -S...` in its
[batch-mode article](https://hex-rays.com/blog/igor-tip-of-the-week-08-batch-mode-under-the-hood).
We do not claim an exact historical introduction version for `TVHEADLESS`.

GhidraDec 1.1 automatically disables decompiler workers in text mode and
autonomous/batch mode. `GHIDRADEC_NO_THREADS=1` is the supported explicit
override; `GHIDRADEC_TEST_LIVE_CALLBACKS` is no longer required for headless use.
This policy does not by itself certify an idalib integration.

## Upstream Evidence

The grid is our engineering judgement, informed by the adapter branches and:

* [IDA 7.0 API porting guide](https://docs.hex-rays.com/release-notes/7_0/api70_porting_guide):
  major API/host-process transition and renamed text executable.
* [IDA 7.5 release notes](https://docs.hex-rays.com/release-notes/7_5):
  class-based plugin/loader/processor lifecycle.
* [IDA 8.x to 8.5 porting guide](https://docs.hex-rays.com/8.5/developer-guide/idapython/idapython-porting-guide-ida-9):
  removal of legacy struct/enum APIs in favor of type information.
* [Public SDK releases](https://github.com/HexRaysSA/ida-sdk/releases):
  SDK-only updates are distinct from product versions/service packs.
