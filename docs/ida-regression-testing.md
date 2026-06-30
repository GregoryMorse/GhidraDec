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

Stage the smoke tier with:

```bash
python tools/corpus.py stage \
  --corpus angr-binaries \
  --arch x86_64,x86_32,x86_16 \
  --tier smoke \
  --output-list build/corpus/angr-x86.txt
```

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

By default the runner removes stale IDA sidecar databases for raw binary inputs
before each run. Pass `--reuse-database` only when deliberately debugging a
previously scanned database. Pass `--paramid` to include Ghidra parameter
identification in a deeper release-certification run.

On Windows, `tools/ida_batch.py` also watches IDA-owned dialogs and sends the
default confirmation action for common startup, warning, crash, and recovery
dialogs. This keeps IDA Pro 9.3 batch runs non-interactive while still logging
which dialogs were handled.

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
