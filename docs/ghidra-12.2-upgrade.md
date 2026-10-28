# Ghidra 12.2 Boundary Review

Reviewed the exact upstream `Ghidra_12.1.2_build` and `Ghidra_12.2_build`
sources, not just release notes or a successful compilation. The engine remains
an external process; the shared static support target is our existing bridge,
packed codec and SLEIGH translator, not a statically linked decompiler engine.

## Changes Requiring Bridge Work

| Boundary | Upstream Change | GhidraDec 1.1 Handling |
| --- | --- | --- |
| Native process messages | Warnings are now immediate burst pairs **18/19**. **16/17** carry command errors, not a mixed warning/error string. | Read bounded warning frames without disturbing an active main response; display warnings separately. Reject native errors instead of manufacturing successful-looking C/XML output. |
| Function mapped symbols | Java `encodeFunction` now maps up to **8 bytes from the entry**, clipped to the contiguous entry range. Offcut queries use that same entry mapping. | Clip the mapping to the entry chunk, retrieve entry metadata for offcuts, and encode the entry address rather than the queried address. |
| Function-body holes | Java now starts a hole at the queried address, not at the beginning of the function/chunk. A query before the entry ends its hole before the entry. | Do not cache a body hole over a function's entry bytes. Tail chunks remain holes, not additional entries. Global symbol use-limits remain empty. |
| Packed codec | Native attribute-skip/advance lengths widened from 32 to 64 bits; unknown-element sentinel moved from 290 to 291. | Compile the upstream 12.2 codec rather than retaining a copied old implementation. |

The framing regression test uses the real bridge parser, with warnings before,
during and after main output, empty success messages, native errors containing
XML-sensitive text, malformed terminators and over-limit warnings. Mapping
tests cover entry/offcut windows, small bodies, pre-entry holes, tail chunks
and end-of-address-space arithmetic.

Inspecting actual C output also exposed a pre-existing renderer omission:
`<value>` tokens used by case/default labels were dropped. The native emitter
is unchanged between these releases. These tokens are now emitted in both
plain and colored output, with a rendering regression and an output-quality
gate rejecting empty `case:` labels.

## Reviewed, No New Wire Adapter Required

* All 19 packed query IDs (239-257) are unchanged. The established option IDs
  used by the bridge are unchanged; tests compare them to provisioned sources.
* Java `HighFunction`, `HighFunctionSymbol`, `PcodeDataTypeManager` and
  `AttributeId` are unchanged in this comparison. `HighSymbol` gains dynamic
  storage resolution, not a different mapsym encoding. `FunctionPrototype`
  changes a non-short-circuit boolean operation, not its noreturn encoding.
* Java packed encoding adds an integer-opcode overload using the existing signed
  opcode representation. Packed decoding gains `Closeable`, not a wire change.
* Register/program, action/options, decompile, flush and deregistration commands
  retain their request layout. Their native error/warning delivery changed as
  described above.
* Native `SymbolEntry` is split into mapped/dynamic implementations. Existing
  `addr` versus `hash` mapsym encodings remain; the bridge does not depend on
  those internal C++ classes.
* Override ID 221 is renamed from `indirectoverride` to `calldest`.
  `DecompileAt` now explicitly calls `Funcdata::encode(..., saveOverrides=false)`;
  this is not an override element our current response path must consume or emit.
* Constant-pool, injection, comment and string query implementations show no
  boundary change in the inspected native files.

## Engine Behavior To Watch

12.2 adds option `baddatacount` (ID 290), with a default maximum of **4** caught
bad-data exceptions. We retain the engine's default, as the Java client does;
there is no new UI override for this setting in 1.1. Malformed-code corpora may
therefore fail differently without indicating a transport incompatibility.

Prototype defaults allow much larger local/parameter stack ranges. Parameter
trial/type selection, array/structure pointer recovery, dynamic symbol storage,
wrapped stack ranges and C if/else emission also changed. These are reasons to
run representative live-callback and parameter-identification cases, not to
expect byte-for-byte equality with 12.1 output. A source-boundary review and
smoke tests are not proof of full semantic equivalence across all processors.

## Sources

* [Java decompiler client at 12.2](https://github.com/NationalSecurityAgency/ghidra/tree/Ghidra_12.2_build/Ghidra/Features/Decompiler/src/main/java/ghidra/app/decompiler):
  `DecompileProcess`, `DecompileCallback`, `DecompInterface`, `DecompileOptions`.
* [Java p-code model/serialization at 12.2](https://github.com/NationalSecurityAgency/ghidra/tree/Ghidra_12.2_build/Ghidra/Framework/SoftwareModeling/src/main/java/ghidra/program/model/pcode).
* [Native decompiler boundary at 12.2](https://github.com/NationalSecurityAgency/ghidra/tree/Ghidra_12.2_build/Ghidra/Features/Decompiler/src/decompile/cpp):
  `ghidra_arch`, `ghidra_process`, `marshal`, `database_ghidra`, `database`,
  `typegrp_ghidra`, `inject_ghidra`, `comment_ghidra`, `string_ghidra`,
  `options`, `translate`, `sleigh`, `slaformat`, `opcodes`, `fspec`,
  `type`, `funcdata` and `printc`.
* [12.1.2 baseline tag](https://github.com/NationalSecurityAgency/ghidra/tree/Ghidra_12.1.2_build).
