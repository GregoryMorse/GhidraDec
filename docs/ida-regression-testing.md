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

## Test Flow

1. Copy the target binary into an isolated work directory.
2. Launch IDA in batch mode with `tools/ida_batch_decompile_all.py`.
3. Wait for IDA autoanalysis to finish.
4. Optionally save the scanned database before plugin execution.
5. Run the plugin with argument `1`, matching the full decompile-all command.
6. Wait for a stable C output file and fail if it is missing, tiny, or late.

`tools/ida_batch.py` drives this flow from the host:

```bash
python tools/ida_batch.py \
  --ida /path/to/idat64 \
  --ghidra-dir /path/to/ghidra \
  --save-database \
  --timeout 900 \
  /path/to/corpus/sample
```

The default work directory is `build/ida-regression`. Each input gets its own
subdirectory containing the copied binary, IDA log, optional analyzed database,
and GhidraDec output.

## CI Shape

Default CI should keep using a very small smoke set. Larger corpus runs should
be manual or scheduled:

* default: one or two tiny binaries for the latest official IDA SDK.
* manual corpus run: selected architectures or corpus families.
* scheduled corpus run: broader public corpus sweep with artifacts retained for
  failing cases only.

When a failure occurs, preserve the copied input, IDA log, scanned database, and
GhidraDec output as artifacts. Successful databases should stay ephemeral.
