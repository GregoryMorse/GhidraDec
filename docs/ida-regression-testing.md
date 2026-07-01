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

`ghidradec.corpus.json` pins the initial angr corpus to commit
`c113ca72dc54c2164abb1c724f0a4a075536519a`. The first order is:

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

`tools/ida_batch.py` drives this flow from the host:

```bash
python tools/ida_batch.py \
  --ida-dir "/path/to/IDA Professional 9.3" \
  --ghidra-dir /path/to/ghidra \
  --save-database \
  --timeout 900 \
  --input-list build/corpus/angr-x86.txt
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

Current baseline status from the pinned angr set:

* A broad enabled run passes cleanly across `47/47` decompile-all targets
  with no `graceful_fail` or `dangerous_fail` results before enabling HPPA.
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

Additional processor status from angr:

* `m68k` is enabled as a sampled `individual-functions` target. Current probes
  cover early, mid, and late slices with `30/30` successes and no graceful or
  dangerous failures. Monolithic decompile-all remains too large for routine
  runs.
* `sh4` reaches Ghidra's SuperH4 language through local IDA `sh3`/`sh4` aliases
  and is enabled as a sampled `individual-functions` target. Current probes
  cover early and mid slices with `20/20` successes and no graceful or dangerous
  failures. Monolithic decompile-all remains too large for routine runs.
* `hppa` is enabled. The large/static-style PA-RISC target now passes
  decompile-all with `935/935` functions after packed p-code normalization for
  relative branches that skip terminal branches into trailing delay-slot ops.
  Current report:
  `build/corpus-reports/hppa-relative-normalize/ida-batch-summary.json`.

Current JVM/Dalvik status from the pinned angr set:

* Java `.class` targets are staged from selected `.jar` archive members because
  IDA's ZIP loader otherwise opens archive metadata rather than the class
  loader. The enabled Java smoke/extended lane currently passes `4/4` with no
  `graceful_fail` or `dangerous_fail` results. Report:
  `build/corpus-reports/angr-java-classes-switchsemantic/ida-batch-summary.json`.
* The initial Dalvik target is staged as `classes.dex` extracted from
  `tests/java/android1.apk`. It currently passes `1/1`, decompiling `12,285`
  functions with no `graceful_fail` or `dangerous_fail` results. Report:
  `build/corpus-reports/angr-dalvik/ida-batch-summary.json`.
* JVM constant-pool responses now use packed `<cpoolrec>` encoding for Ghidra
  12.x protocol queries. Empty JVM/Dalvik dynamic p-code injections are handled
  with a side-effect-free no-op fallback so missing semantic implementations do
  not crash the decompiler.
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
