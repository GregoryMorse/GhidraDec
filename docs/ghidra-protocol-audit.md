# Ghidra Decompiler Protocol Audit

This note tracks how GhidraDec's IDA bridge lines up with the Ghidra native
decompiler protocol. It is meant to guide corpus expansion and targeted quality
fixes rather than to replace live regression testing.

## Scope

The local generated dependency tree contains the Ghidra 12.1.2 native
decompiler sources, but not the full Ghidra Java client source tree. This first
pass therefore audits against the native protocol contract used by the Java
client: `ghidra_arch.*`, `ghidra_process.*`, and related C++ decompiler code.
It also spot-checks upstream `DecompInterface.java` to compare command
sequencing, cache behavior, and Java-only helper commands.

When a full Ghidra source checkout is available, the next pass should compare
the Java-side packet construction and option defaults directly against
`DecompInterface`, `DecompileProcess`, and the decompiler task classes.

## Command Coverage

| Ghidra command | GhidraDec status | Notes |
| --- | --- | --- |
| `registerProgram` | Full | Sends pspec, cspec, tspec, core type archive, and initial options. |
| `deregisterProgram` | Full | Used for process cleanup. |
| `flushNative` | Full | Sent immediately after `decompileAt` returns, before local XML parsing/conversion. |
| `decompileAt` | Full | Main single-function decompile path. |
| `setAction` | Full | Covers action style and output toggles such as C, syntax tree, param measures, and jump-load behavior. |
| `setOptions` | Full | Sends packed option list after program registration and when options change. |
| `structureGraph` | Not implemented | Not used by the current plugin. Worth revisiting if the experimental C graph view should ask Ghidra to structure an explicit graph instead of deriving a graph from decompile output. |
| `getSignatureSettings` | Not implemented | Signature-generation helper used by Ghidra Java APIs, not required for normal C decompilation. |
| `setSignatureSettings` | Not implemented | Signature-generation helper used by Ghidra Java APIs, not required for normal C decompilation. |
| `generateSignatures` | Not implemented | Signature-generation helper used by Ghidra Java APIs, not required for normal C decompilation. |
| `debugSignatures` | Not implemented | Signature-generation helper used by Ghidra Java APIs, not required for normal C decompilation. |

Ghidra's Java client treats several settings as cached desired state: options,
action name, syntax-tree output, C output, parameter measures, and jump-load
output. On process restart it resends the cached state after `registerProgram`.
GhidraDec follows the same general model.

The packed `setOptions` path now covers Ghidra 12.1's global/default native
options that are safe to apply process-wide: action toggles, read-only and
constant-pointer analysis, loop analysis, context setting, unimplemented and
flow-error policy, jump-load recording, alias blocking, maximum instruction and
jump-table limits, namespace display, datatype splitting, NaN handling,
printing/comment controls, integer and brace formatting, language selection,
and prototype evaluation. Per-function/per-rule options such as `inline`,
`noreturn`, `warning`, `extrapop`, `togglerule`, and cspec-specific
`defaultprototype` are intentionally not emitted as defaults because they need
target-specific addresses, rule names, or model names. Ghidra 12.1 registers
`hideextensions` by name but does not assign it a packed option element id; the
legacy XML serialization includes it, while the packed transport leaves the
native C printer default in effect.

Ghidra's Java `decompileFunction()` flushes the native cache after a normal
decompile when the process remains alive. GhidraDec now follows that cadence by
flushing as soon as `decompileAt` returns, before local result parsing. This
keeps native cache state from surviving local XML or display conversion
failures.

## Packed Query Coverage

Ghidra 12.1.x uses packed query element IDs 239 through 257. GhidraDec declares
all of these IDs and handles them in the packed response path.

| Query element | ID | Status | Notes |
| --- | ---: | --- | --- |
| `command_isnameused` | 239 | Partial | Answers from the namespace/name registry populated by emitted symbols; unknown scopes still behave conservatively. |
| `command_getbytes` | 240 | Full | Reads bytes from IDA through the callback layer. |
| `command_getcallfixup` | 241 | Full | Routed through p-code injection handling. |
| `command_getcallmech` | 242 | Full | Routed through p-code injection handling. |
| `command_getcallotherfixup` | 243 | Full | Routed through p-code injection handling. |
| `command_getcodelabel` | 244 | Full | Resolves labels through IDA names. |
| `command_getcomments` | 245 | Full | Emits packed comment database records. |
| `command_getcpoolref` | 246 | Full | Encodes constant-pool records; important for JVM/Dalvik coverage. |
| `command_getdatatype` | 247 | Full | Builds type XML from callback metadata. High-risk for structs, classes, enums, and Java-like types. |
| `command_getexternalref` | 248 | Full | Uses IDA external reference data when available, otherwise returns a hole. |
| `command_getmappedsymbols` | 249 | Full | Handles functions, data, labels, external references, and holes. |
| `command_getnamespacepath` | 250 | Partial | Emits Ghidra's expected parent/val path shape for namespaces observed in symbol responses. |
| `command_getpcode` | 251 | Full | Uses SLEIGH translation, with special handling for JVM switch behavior and a guarded fallback for unimplemented p-code. |
| `command_getpcodeexecutable` | 252 | Full | Routed through p-code injection handling. Historically fragile; keep corpus coverage around this. |
| `command_getregister` | 253 | Full | Resolves register metadata through the translator. |
| `command_getregistername` | 254 | Full | Resolves register name by space and offset. |
| `command_getstringdata` | 255 | Full | Reads string data through IDA callback, with max-size handling. Needs wide/UTF string tests. |
| `command_gettrackedregisters` | 256 | Partial | Uses tracked context where available and otherwise emits an empty set. MIPS now seeds `$gp` from `_GLOBAL_OFFSET_TABLE_ + 0x7ff0`; more architecture-specific context tests are still needed. |
| `command_getuseropname` | 257 | Full | Resolves user-defined op names from the translator. |

## Main Risk Areas

The protocol surface is now mostly covered, but several answers are intentionally
conservative. These are the places most likely to produce plausible-looking but
lower-quality output:

- Namespace handling is no longer empty, but it is still based on names observed
  through IDA symbol queries. It should be expanded before expecting perfect C++
  namespace, Java package, or heavily overloaded symbol output.
- P-code injections are covered, but they remain a high-risk surface because
  missing or malformed dynamic injection XML can terminate a decompile process.
- Tracked register context can silently degrade when an architecture depends on
  context registers. This matters for segmented x86, ARM/Thumb-style mode
  changes, MIPS variants, and similar processor-specific state. MIPS `$gp`
  seeding is now covered, and zero-sized tracked register varnodes are filtered
  after that path exposed a native decompiler access violation on big-endian
  MIPS.
- Function-pointer datatype fidelity is still intentionally conservative.
  No-return import pointers such as `exit_ptr` are modeled because they affect
  control-flow correctness, but full packed function-pointer return storage
  needs a separate audit before broad signature emission is enabled.
- String data handling should be tested with ASCII, UTF-8, UTF-16, narrow and
  wide C strings, Pascal-style strings, and truncated reads.
- Batch-mode cache preloading can mask callback and reentrancy bugs. The batch
  runner now has a `--live-callbacks` mode that disables broad byte/symbol
  preloads and uses direct callback dispatch for regression runs.
- Java, Dalvik, and JVM switch support should be tested explicitly because they
  exercise constant-pool, datatype, p-code, and switch-injection paths together.

## Suggested Quality Gates

Use these as the next corpus milestones:

1. For every corpus binary, record success, graceful decompiler failure, timeout,
   crash, and output-quality warnings separately.
2. Run each target in cached batch mode and with `tools/ida_batch.py
   --live-callbacks`. A target is only truly clean when both paths agree.
3. Add golden snippets for representative calls, globals, strings, switch
   statements, imports, external references, and namespace-heavy symbols.
4. Add targeted corpus entries for p-code injection queries: call fixups,
   callother fixups, p-code executables, JVM switches, and MIPS/PowerPC branch
   behavior.
5. Track output quality separately from process stability. A run that produces
   compilable-looking but semantically bogus output should not be counted as a
   clean pass.

## Next Audit Pass

When the full Ghidra Java source tree is available, audit:

- Java option defaults versus `DecompInterface::setOptions()`.
- Java command sequencing versus GhidraDec's register, option, action, decompile,
  flush, and deregister flow.
- Java-side timeout and process-restart behavior versus GhidraDec's pipe and
  child-process recovery.
- Java token/color model versus GhidraDec's `EmitXml` color and navigation model.
- Java graph APIs and whether `structureGraph` should be implemented for the
  experimental C graph view.
