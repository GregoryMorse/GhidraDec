# IDA Regression Testing

The next regression layer should exercise IDA's loader and autoanalysis first,
then run GhidraDec's full decompilation path against the scanned database. This
is closer to user behavior than feeding synthetic function selections to the
plugin, and it gives us reusable IDB/I64 artifacts for debugging failures.

## Corpus Strategy

Use small, pinned, legally redistributable binaries first:

* `angr/binaries`: best first corpus because it is already organized around
  architecture and loader diversity.
* Triton samples: useful second layer for instruction semantics and symbolic
  execution flavored edge cases.
* QEMU test binaries: good generated coverage, but better as an opt-in or
  nightly job because the matrix can get large quickly.
* Exploit Education Phoenix: useful x86/x64 ELF coverage with real calling
  convention and stack-layout patterns.
* CTF archives: curate carefully. Prefer unstripped binaries only when license
  and redistribution are clear, and keep giant or obscure samples out of default
  CI.

Do not commit bulky databases or decompiler outputs. Keep corpus manifests in
the repo and fetch/extract binaries into the build tree.

`ghidradec.corpus.json` pins the angr corpus to commit
`ec5fd26d01d8db05d109945fb64502a67d013d5c`. The first order is:

1. `x86_64`
2. `x86_32`
3. `x86_16`

The baseline expansion order then adds:

1. `armel`
2. `armhf`
3. `aarch64`
4. `mips`
5. `mipsel`
6. `mips64`
7. `ppc`
8. `ppc64`
9. `ppc64el`
10. `riscv64`
11. `java`
12. `dalvik`
13. `m68k`
14. `hppa`
15. `sh4`

The latest upstream angr tree also contains `alpha` and `s390x` samples. These
are recorded in the manifest as disabled `upstream` inventory entries rather
than release blockers because the local IDA/Ghidra processor audit found no
stock Ghidra 12.1.2 decompilable processor binding for IDA 9.3's Alpha or S/390
modules. Upstream does not currently provide a separate SPARC test lane in
`angr/binaries`.

Stage the smoke tier with:

```bash
python tools/corpus.py stage \
  --corpus angr-binaries \
  --arch x86_64,x86_32,x86_16 \
  --tier smoke \
  --output-list build/corpus/angr-x86.txt
```

For normal local certification runs, prefer the battery wrapper. It stages the
manifest-selected files, writes the input list, launches the IDA batch runner,
and writes machine-readable summaries:

```bash
python tools/ida_corpus_battery.py \
  --corpus angr-binaries \
  --arch x86_64,x86_32,x86_16 \
  --tier smoke,extended \
  --ida-dir "/path/to/IDA Professional 9.3" \
  --ghidra-dir /path/to/ghidra \
  --plugin build/matrix/ida-9.3/Release/ghidradec64.dll \
  --work-dir build/ida-corpus-regression/angr-x86-all \
  --report-dir build/corpus-reports/angr-x86-all
```

For the first non-x86 smoke sweep:

```bash
python tools/ida_corpus_battery.py \
  --corpus angr-binaries \
  --arch armel,armhf,aarch64,mips,mipsel,mips64 \
  --tier smoke \
  --ida-dir "/path/to/IDA Professional 9.3" \
  --ghidra-dir /path/to/ghidra \
  --plugin build/matrix/ida-9.3/Release/ghidradec64.dll \
  --work-dir build/ida-corpus-regression/angr-arm-mips-smoke \
  --report-dir build/corpus-reports/angr-arm-mips-smoke
```

Use `--no-stage` when the selected corpus files are already present under
`build/corpus`. Use `--refresh` to redownload the pinned public inputs.

## Test Flow

1. Copy the target binary into an isolated work directory.
2. Launch IDA in batch mode with `tools/ida_batch_decompile_all.py`.
3. Wait for IDA autoanalysis to finish.
4. Optionally save the scanned database before plugin execution.
5. Run the plugin with argument `5`, the unattended full decompile-all
   regression command. This keeps the decompiler worker thread enabled for
   IDA callback dispatch and skips parameter identification by default.
6. Wait for a stable C output file and fail if it is missing, tiny, or late.

For cache honesty checks, add `--live-callbacks`. This disables broad batch
byte/symbol preloads and forces regression callback bodies to query IDA directly
instead of relying on preloaded cache state. It is slower and intentionally more
stressful than the default cached batch mode; a release target should pass both.

`tools/ida_batch.py` drives this flow from the host:

```bash
python tools/ida_batch.py \
  --ida-dir "/path/to/IDA Professional 9.3" \
  --ghidra-dir /path/to/ghidra \
  --save-database \
  --timeout 900 \
  --input-list build/corpus/angr-x86.txt
```

Example live-callback smoke run:

```bash
python tools/ida_corpus_battery.py \
  --corpus angr-binaries \
  --arch x86_64 \
  --tier smoke \
  --no-stage \
  --live-callbacks \
  --ghidra-dir /path/to/ghidra \
  --plugin build/matrix/ida-9.3/Release/ghidradec64.dll \
  --work-dir build/ida-corpus-regression/live-callback-x86_64-smoke \
  --report-dir build/corpus-reports/live-callback-x86_64-smoke
```

The default work directory is `build/ida-regression`. Each input gets its own
subdirectory containing the copied binary, IDA log, optional analyzed database,
and GhidraDec output.

`tools/ida_batch.py` and `tools/ida_corpus_battery.py` report three outcomes:

* `success`: IDA exited cleanly, the done marker was written, and decompiler
  output met the minimum size check.
* `graceful_fail`: the plugin reported a controlled diagnostic, such as a
  selected import function that intentionally cannot be decompiled, a native
  decompiler marshaling diagnostic, or a decompile-all run where not every
  selected function completed.
* `dangerous_fail`: IDA crashed, timed out, failed to create output, or hit an
  unhandled failure pattern. Windows crash exit codes such as stack overflow and
  access violation are named in the JSON summary.

The battery writes:

* `inputs.txt`: exact staged binaries used for the run.
* `inputs-individual-functions.txt`: exact staged binaries used for any
  manifest-selected sampled individual-function run.
* `ida-batch-summary.json`: per-input pass/fail details, logs, outputs, done
  markers, and output sizes for single-mode decompile-all runs.
* `ida-batch-summary-decompile-all.json`: decompile-all details when a
  manifest selection mixes execution modes.
* `ida-batch-summary-individual-functions.json`: per-input pass/fail details
  for sampled individual-function targets when the selection mixes test modes.
* `battery-summary.json`: selected corpus metadata plus the embedded batch
  summary and command line. Mixed-mode runs also include `batch_runs`, one entry
  per execution mode.

By default the runner removes stale IDA sidecar databases for raw binary inputs
before each run. Pass `--reuse-database` only when deliberately debugging a
previously scanned database. Pass `--paramid` to include Ghidra parameter
identification in a deeper release-certification run.

Batch runs enable `GHIDRADEC_TRACE` by default so the IDA log and
`ghidradec-protocol.log` contain setup, query, and decompile breadcrumbs. Pass
`--no-trace` to `tools/ida_batch.py` or `tools/ida_corpus_battery.py` for quieter
logs. GUI use defaults to user-facing `INFO_MSG` output only; set
`GHIDRADEC_TRACE=1` when debugging an interactive session.

The x86-64 live-callback smoke lane currently passes `2/2` pinned angr inputs:
`fauxware` and `test_arrays`. Report:
`build/corpus-reports/live-callback-x86_64-smoke/ida-batch-summary.json`.

Previous verified baseline status before the latest angr-manifest expansion:

* A broad enabled run passed cleanly across `47/47` decompile-all targets
  with no `graceful_fail` or `dangerous_fail` results before enabling HPPA and
  before adding the newer x86-64 decompiler-corpus, RISC-V relocation, Java,
  and ABI/ioctl targets.
  Report:
  `build/corpus-reports/enabled-full-arrayfix/ida-batch-summary-decompile-all.json`.
* The remaining HPPA target now passes directly with `935/935` functions
  decompiled, no `graceful_fail` or `dangerous_fail` results, and is enabled in
  `ghidradec.corpus.json`. Report:
  `build/corpus-reports/hppa-relative-normalize/ida-batch-summary.json`.
* The sampled static-target lane now passes `3/3` inputs, `10/10` functions per
  input, with no `graceful_fail` or `dangerous_fail` results. Report:
  `build/corpus-reports/individual-summary-arrayfix/ida-batch-summary-individual-functions.json`.
* The previous strict marshaling queue (`x86_64`, `x86_16`, `armel`,
  `aarch64`, and `ppc` samples) was fixed by emitting packed array type
  `arraysize` as a signed integer, matching Ghidra's 12.x packed decoder.

Current ARM/AArch64/MIPS/RISC-V status from the pinned angr set:

* `armel`, `armhf`, `aarch64`, `mips`, `mipsel`, and `mips64`
  smoke/extended targets pass cleanly in the latest strict run.
* `riscv64` smoke now passes and remains enabled. The plugin currently adds a
  local `riscv` alias while waiting for Ghidra's RISCV ldefs to advertise an
  `IDA-PRO` external name upstream.
* MIPS big-endian now passes after register-space mapped-symbol misses were
  changed to cache a full register-space hole instead of a one-byte partial
  register hole. Current combined report:
  `build/corpus-reports/mips-all-postfix/ida-batch-summary.json`.

Current PowerPC status from the pinned angr set:

* `ppc`, `ppc64`, and `ppc64el` smoke targets pass cleanly in the latest
  strict run.
* The small `ppc64` extended targets pass cleanly and remain enabled.
* PPC64 depends on compiler-spec pcode injection declared inside
  `<default_proto>`. This is covered by `angr-ppc64-fauxware`,
  `angr-ppc64-test_arrays`, `angr-ppc64-test_loops`,
  `angr-ppc64-test_division`, and `angr-ppc64el-fauxware`.
* `angr-ppc64el-fauxware-static` is enabled as a sampled
  `individual-functions` target. A 10-function probe currently passes `10/10`
  with no `graceful_fail` or `dangerous_fail` results. Monolithic decompile-all
  remains too large for routine runs.

Current strict smoke status from the latest expanded angr pin
`ec5fd26d01d8db05d109945fb64502a67d013d5c`:

* Full explicit smoke grid report:
  `build/corpus-reports/expanded-full-smoke-sampled/battery-summary.json`.
* Decompile-all lanes: `16/24` clean, `8/24` graceful quality/protocol
  failures, `0/24` dangerous failures.
* Sampled live-callback lanes: `54/60` selected functions clean across `m68k`,
  `hppa`, and `sh4`; grouped target result is `0/3` clean because each sampled
  target still has at least one graceful quality failure.
* Clean decompile-all smoke lanes include `x86_64`, `x86_32`, `armel`,
  `aarch64`, `mips`, `mipsel`, `mips64`, `ppc`, `ppc64` `test_arrays`, and
  `riscv64`.
* Graceful decompile-all smoke failures in that report are `x86_16`
  bad-instruction control flow, `armhf` unimplemented-instruction warnings,
  `ppc64` fauxware unimplemented-instruction warnings, `ppc64el` unresolved
  `func_0x` calls, and the JVM/Dalvik dynamic p-code injection failures that
  predate the focused fixes below.
* These stricter runs disable broad byte/symbol preloads where practical so
  callback coverage is not hidden by the batch cache.
* `ppc64` ELFv1 dotted function names must not be treated as runtime helpers.
  The helper classifier now strips a leading dot only for matching known
  runtime names and skips `.plt_call` shims, so the smoke run decompiles the
  real `.main`/`.authenticate` bodies rather than reporting success on PLT
  stubs.
* The focused `ppc64el` fauxware unresolved-call failure is fixed after named
  orphan code labels were promoted to unknown-prototype callable symbols. This
  handles IDA's descriptor/prologue split where `rejected` is a thunk-like
  entry and the real named body label is `rejected_0` at `0xbe4`. Cached and
  live-callback reports both pass cleanly:
  `build/corpus-reports/ppc64el-orphan-callable-symbol/ida-batch-summary.json`
  and
  `build/corpus-reports/ppc64el-orphan-callable-symbol-live/ida-batch-summary.json`.
* The 1.0 Angr expansion added ten strict decompile-all targets covering
  x86-64 switches, x86-64 return/prototype and structure cases, i386 switches,
  i386 calling-convention/object-file coverage, i386 arithmetic expression
  output, ARM little-endian branch/control-flow cases, and a PowerPC branch
  target. The focused report passes cleanly with `10/10` success and no
  graceful or dangerous failures:
  `build/corpus-reports/candidate-1.0-expansion-clean10-final/ida-batch-summary.json`.
* `mips` big-endian live-callback coverage now passes the `fauxware` code
  functions cleanly. The root cause of the previous `Read pipe is bad` failure
  was a duplicate zero-sized tracked `$gp` register entry being sent to
  Ghidra's native decompiler after the valid GOT seed. Zero-sized tracked
  register varnodes are now filtered before protocol encoding. The stricter
  quality gate also treats bad-instruction decompiler warnings as failures;
  `exit_ptr`-style known no-return import pointers are typed so Ghidra does not
  walk into terminal-call padding. Current focused report:
  `build/ida-corpus-regression/mips-be-quality-clean/.../individual/summary.json`.
* `angr-mipsel-jumptable-0` is retained as a blocked inventory target rather
  than enabled release coverage. It decompiles all `176/176` functions and the
  previous bad-instruction warning after the terminal indirect exit call is
  fixed by no-return boundary handling, but strict gating still catches a
  cluster of `Removing unreachable block` and `Could not recover jumptable`
  warnings in switch-heavy functions. Ghidra's global native decompiler
  options, including jump table sizing and flow/error controls, are now
  marshalled through to the native decompiler, but this target remains a real
  recovery-quality blocker until callback fidelity versus upstream jump-table
  recovery is isolated. Focused report:
  `build/corpus-reports/blocked-option-audit/ida-batch-summary.json`.
* RISC-V relocatable object coverage is split by quality. The tiny call
  relocation target `angr-riscv64-call-reloc` is enabled as extended coverage
  after the batch harness learned to seed `.text` when IDA only creates import
  stubs and to honor per-target `minOutputBytes`; focused report:
  `build/corpus-reports/riscv-call-promoted/ida-batch-summary.json`.
  `angr-riscv64-branch-reloc` and `angr-riscv64-jal-reloc` remain blocked:
  branch now decompiles a seeded `_start` but emits unreachable/infinite-loop
  warnings, while JAL emits an unimplemented-instruction warning. Current
  diagnostic report:
  `build/corpus-reports/blocked-option-audit/ida-batch-summary.json`.
* `angr-alpha-test-instr` and `angr-s390x-test_arrays` are kept as disabled
  `upstream` inventory targets, not blocked GhidraDec release targets. They need
  stock Ghidra decompilable processor/package coverage or an accepted upstream
  binding before the bridge can test them meaningfully.
* Import-table entries and matching dotted dynamic import stubs are excluded
  from individual-function selection by default because they are not owned code
  bodies. Use `tools/ida_batch.py --include-import-functions` only when
  deliberately testing import skip diagnostics.
* `m68k` is enabled as a sampled `individual-functions` target. Automatic
  sampling skips obvious runtime/helper functions before applying
  `individualMax`. The current strict smoke sample is `19/20` clean with one
  graceful naming-quality failure (`func_0x`) and no dangerous failures.
  Focused inspection shows the remaining `__run_exit_handlers` `func_0x0`
  output is a genuine unmapped/null indirect-call query, not an IDA symbol
  lookup miss. Current focused report:
  `build/ida-corpus-regression/m68k-null-call-sanity/.../individual/summary.json`.
* `sh4` reaches Ghidra's SuperH4 language through local IDA `sh3`/`sh4` aliases
  and is enabled as a sampled `individual-functions` target. The current
  strict smoke sample is close but not clean: `19/20` success, `1/20` graceful
  unreachable-block quality failure, no dangerous failures.
* `hppa` is enabled as a sampled `individual-functions` target. The
  large/static-style PA-RISC target is not suitable for routine monolithic
  decompile-all because it can spend too long in libc-style routines. The
  current strict smoke sample is `16/20` clean with four graceful control-flow
  quality failures and no dangerous failures.

JVM/Dalvik status:

* Java `.class` targets are staged from selected `.jar` archive members because
  IDA's ZIP loader otherwise opens archive metadata rather than the class
  loader.
* The initial Dalvik target is staged as `classes.dex` extracted from
  `tests/java/android1.apk`.
* JVM constant-pool responses now use packed `<cpoolrec>` encoding for Ghidra
  12.x protocol queries. Empty JVM/Dalvik dynamic p-code injections are handled
  with a side-effect-free non-empty fallback so missing semantic
  implementations do not crash the decompiler or trigger Ghidra's `Empty
  injection` low-level diagnostic.
* Current focused JVM/Dalvik smoke report:
  `build/corpus-reports/jvm-dalvik-injection-fallback/ida-batch-summary.json`.
  The two Java smoke class files pass cleanly. The Dalvik sample now decompiles
  `12,285/12,285` functions with no low-level dynamic injection errors, but it
  is still classified as graceful because Ghidra reports bad-instruction
  control-flow warnings in the generated output.
* Current focused Java extended report:
  `build/corpus-reports/java-extended-injection-fallback/ida-batch-summary.json`.
  Five of six class-file targets pass cleanly; the remaining target decompiles
  all selected functions but is classified as graceful because Ghidra reports a
  `Removing unreachable block` warning.
* JVM `lookupswitch` and `tableswitch` translation is handled locally before
  calling native SLEIGH because Ghidra's translator can access-violate on these
  variable-length bytecodes in this embedding. The local encoder emits packed
  p-code matching Ghidra's JVM semantics: `lookupswitch` uses `switchAssist`,
  and `tableswitch` emits signed bounds checks, table lookup, and an indirect
  branch.

On Windows, `tools/ida_batch.py` also watches IDA-owned dialogs and sends the
default confirmation action for common startup, warning, crash, and recovery
dialogs. This keeps IDA Pro 9.3 batch runs non-interactive while still logging
which dialogs were handled.

For deeper diagnosis, pass `--individual-functions` to `tools/ida_batch.py`.
This first asks IDA for the analyzed function list and then launches one
isolated plugin invocation per function, which is useful for separating
per-function coverage from combined decompile-all failures.

## CI Shape

Public GitHub-hosted CI is compile/package-only. It can stage public corpora and
eventually run standalone Ghidra interface tests, but it must not launch IDA Pro
because IDA requires a licensed install, user-specific configuration, and GUI
dialog handling on some platforms.

IDA-backed corpus runs should be manual or scheduled on a private licensed
machine outside GitHub-hosted CI:

* default public CI: build and package the latest official IDA SDK target,
  currently 9.3.
* manual corpus run: execute `tools/ida_batch.py` locally or from private CI
  with IDA Pro 9.3 installed and licensed.
* scheduled corpus run: broader public corpus sweep with artifacts retained for
  failing cases only.

When a failure occurs, preserve the copied input, IDA log, scanned database, and
GhidraDec output as artifacts. Successful databases should stay ephemeral.
