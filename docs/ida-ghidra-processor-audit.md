# IDA/Ghidra Processor Binding Audit

Generated from the local IDA Pro 9.3 install at
`C:\Program Files\IDA Professional 9.3\procs` and Ghidra 12.1.2 at
`C:\Users\Gregory\Desktop\Apps\ghidra_12.1.2_PUBLIC\Ghidra\Processors`.

The important Ghidra metadata is `external_name tool="IDA-PRO"` in each
`.ldefs` file. GhidraDec uses these names to map IDA's processor name to a
Ghidra language. The mapping is not always the IDA DLL filename; for example,
IDA's ARM64 inputs still load `arm.dll` and report the ARM processor family.
IDA processor short names were confirmed from the exported `processor_t LPH`
records with `tools/dump_ida_lph.py`. GhidraDec lowercases both sides of the
mapping, matching the lowercase style already used by Ghidra's ARM/MIPS
bindings even when IDA's `LPH` table uses uppercase names.

## IDA Modules

IDA Pro 9.3 processor modules observed locally:

`78k0`, `78k0s`, `ad218x`, `alpha`, `arc`, `arm`, `avr`, `c166`, `cli`,
`dalvik`, `dsp56k`, `f2mc`, `fr`, `h8`, `h8500`, `hppa`, `i196`, `i51`,
`i860`, `i960`, `ia`, `java`, `kr1878`, `m16c`, `m32r`, `m65`, `m65816`,
`m740`, `m7700`, `m7900`, `mc6812`, `mc6816`, `mc68k`, `mc8`, `mips`,
`nds32`, `nec850`, `oakdsp`, `pc`, `pdp11`, `pic`, `pic16`, `ppc`, `riscv`,
`rl78`, `rx`, `s390`, `sam8`, `script`, `sh3`, `sparc`, `spc700`, `st20`,
`st7`, `st9`, `tms32028`, `tms32054`, `tms32055`, `tms320c1`, `tms320c3`,
`tms320c5`, `tms320c6`, `tricore`, `unsp`, `wasm`, `xa`, `xtensa`, `z8`,
`z80`.

## Ghidra Packages

Ghidra 12.1.2 has 187 language ids across these processor packages. Packages
with `IDA-PRO` names already have at least one binding in the installed
`.ldefs`.

| Ghidra package | Languages | Processor ids | Compiler ids | IDA-PRO names |
| --- | ---: | --- | --- | --- |
| `6502` | 2 | `6502`, `65C02` | `default` | `m6502`, `m65c02` |
| `68000` | 4 | `68000` | `default`, `register` | `68000`, `68010`, `68020`, `68020EX`, `68030`, `68040`, `68K`, `colfire` |
| `8048` | 1 | `8048` | `default` | - |
| `8051` | 5 | `80251`, `80390`, `8051` | `Archimedes`, `default` | `8051` |
| `8085` | 1 | `8085` | `default` | - |
| `AARCH64` | 5 | `AARCH64` | `default`, `golang`, `swift`, `windows` | - |
| `ARM` | 22 | `ARM` | `apcs`, `default`, `windows` | `arm`, `armb` |
| `Atmel` | 5 | `AVR32`, `AVR8` | `default`, `gcc`, `iarV1`, `imgCraftV8` | `avr` |
| `BPF` | 1 | `BPF` | `default` | - |
| `CP1600` | 1 | `CP1600` | `default` | - |
| `CR16` | 1 | `CR16C` | `default` | - |
| `Dalvik` | 13 | `Dalvik` | `default` | - |
| `DATA` | 2 | `DATA` | `pointer16`, `pointer32`, `pointer64` | - |
| `eBPF` | 2 | `eBPF` | `default` | - |
| `HCS08` | 6 | `HC05`, `HC08`, `HCS08` | `default` | - |
| `HCS12` | 4 | `HC-12`, `HCS-12`, `HCS-12X` | `default` | - |
| `Hexagon` | 1 | `Hexagon` | `default` | - |
| `JVM` | 1 | `JVM` | `default` | `java` |
| `Loongarch` | 4 | `Loongarch` | `default` | - |
| `M16C` | 2 | `M16C/60`, `M16C/80` | `default` | - |
| `M8C` | 1 | `M8C` | `default` | - |
| `MC6800` | 3 | `6805`, `6809`, `H6309` | `default` | `6309`, `6800`, `6801`, `6803`, `6805`, `6808`, `6809` |
| `MCS96` | 1 | `MCS96` | `default` | - |
| `MIPS` | 22 | `MIPS` | `default`, `eabi`, `n32`, `o32`, `o64`, `windows` | `mipsb`, `mipsl`, `r5900l`, `r5900r` |
| `NDS32` | 2 | `NDS32` | `default` | - |
| `PA-RISC` | 1 | `PA-RISC` | `default` | `hppa` |
| `PIC` | 13 | `PIC-12`, `PIC-16`, `PIC-17`, `PIC-18`, `PIC-24`, `dsPIC30F`, `dsPIC33C`, `dsPIC33E`, `dsPIC33F` | `default` | `pic12cxx`, `pic16cxx`, `pic16fxx`, `pic18cxx` |
| `PowerPC` | 23 | `PowerPC` | `default`, `macosx`, `windows` | `ppc`, `ppcl` |
| `RISCV` | 12 | `RISCV` | `gcc` | - |
| `Sparc` | 2 | `Sparc` | `default` | `sparcb` |
| `SuperH` | 3 | `SuperH` | `default` | - |
| `SuperH4` | 2 | `SuperH4` | `default`, `windows` | `sh4`, `sh4b` |
| `TI_MSP430` | 2 | `TI_MSP430`, `TI_MSP430X` | `default` | `msp430` |
| `tricore` | 4 | `tricore` | `default` | - |
| `V850` | 1 | `V850` | `default` | - |
| `x86` | 6 | `x86` | `borlandcpp`, `borlanddelphi`, `clangwindows`, `default`, `gcc`, `golang`, `swift`, `windows` | `80386r`, `80486p`, `80486r`, `80586p`, `80586r`, `80686p`, `8086`, `8086p`, `8086r`, `athlon`, `k62`, `metapc`, `p2`, `p3`, `p4` |
| `Xtensa` | 2 | `Xtensa` | `default` | - |
| `Z80` | 4 | `Z180`, `Z80` | `default` | `z180`, `z80` |

## PR-Ready Missing Bindings

These are high-confidence because the local IDA processor DLL exposes the
listed short processor names through its exported `LPH` record, or because
Ghidra already uses the same IDA family binding in adjacent languages.

### AARCH64

File:
`Ghidra/Processors/AARCH64/data/languages/AARCH64.ldefs`

IDA's AArch64 loader uses the ARM processor module. The generic Ghidra AArch64
languages can share the existing IDA names `arm` and `armb`; size and endianness
disambiguate them from 32-bit ARM. The IDA `arm.dll` `LPH` short names are
`ARM` and `ARMB`; Ghidra's existing ARM bindings use lowercase `arm` and
`armb`.

```xml
<!-- AARCH64:LE:64:v8A -->
<external_name tool="IDA-PRO" name="arm"/>

<!-- AARCH64:BE:64:v8A -->
<external_name tool="IDA-PRO" name="armb"/>

<!-- AARCH64:LE:32:ilp32 -->
<external_name tool="IDA-PRO" name="arm"/>

<!-- AARCH64:BE:32:ilp32 -->
<external_name tool="IDA-PRO" name="armb"/>
```

Do not add the Apple Silicon language blindly. It is the same size/endian as
generic AArch64 LE and would make automatic selection more ambiguous unless
there is a separate IDA processor name or importer hint for Apple Silicon.

### RISCV

File:
`Ghidra/Processors/RISCV/data/languages/riscv.ldefs`

IDA `riscv.dll` exposes `riscv`.

```xml
<!-- RISCV:LE:64:default -->
<external_name tool="IDA-PRO" name="riscv"/>

<!-- RISCV:LE:32:default -->
<external_name tool="IDA-PRO" name="riscv"/>
```

### NDS32

File:
`Ghidra/Processors/NDS32/data/languages/nds32.ldefs`

IDA `nds32.dll` exposes `nds32`.

```xml
<!-- NDS32:BE:32:default -->
<external_name tool="IDA-PRO" name="nds32"/>

<!-- NDS32:LE:32:default -->
<external_name tool="IDA-PRO" name="nds32"/>
```

### TriCore

File:
`Ghidra/Processors/tricore/data/languages/tricore.ldefs`

IDA `tricore.dll` exposes `TRICORE`. GhidraDec lowercases this to `tricore`.
For the upstream Ghidra change, adding the binding to the default TriCore
language is the conservative fix; adding it to all TC29x/TC172x/TC176x variants
may be correct only if Ghidra's importer has another discriminator.

```xml
<!-- tricore:LE:32:default -->
<external_name tool="IDA-PRO" name="tricore"/>
```

### Xtensa

File:
`Ghidra/Processors/Xtensa/data/languages/xtensa.ldefs`

IDA exposes little- and big-endian names in the processor module strings as
`XTENSA` and `XTENSAB`. Ghidra convention is lowercase in nearby bindings, and
GhidraDec lowercases before matching.

```xml
<!-- Xtensa:LE:32:default -->
<external_name tool="IDA-PRO" name="xtensa"/>

<!-- Xtensa:BE:32:default -->
<external_name tool="IDA-PRO" name="xtensab"/>
```

### M16C

Files:
`Ghidra/Processors/M16C/data/languages/M16C_60.ldefs` and
`Ghidra/Processors/M16C/data/languages/M16C_80.ldefs`

IDA `m16c.dll` exposes `m16c60`, `m16c20`, `m16ctiny`, `r8c`, `m16c80`,
`m32c80`, and `r32c`. Ghidra currently has M16C/60 and M16C/80 language IDs, so
only the matching aliases are listed here.

```xml
<!-- M16C/60:LE:16:default -->
<external_name tool="IDA-PRO" name="m16c60"/>

<!-- M16C/80:LE:16:default -->
<external_name tool="IDA-PRO" name="m16c80"/>
```

### HCS12

File:
`Ghidra/Processors/HCS12/data/languages/HCS12.ldefs`

IDA `mc6812.dll` exposes `6812`, `HCS12`, and `HCS12X`. Ghidra has two IDs for
the HCS12X language; bind both aliases to avoid depending on which one a tool
selects.

```xml
<!-- HC-12:BE:16:default -->
<external_name tool="IDA-PRO" name="6812"/>

<!-- HCS-12:BE:24:default -->
<external_name tool="IDA-PRO" name="hcs12"/>

<!-- HCS-12X:BE:24:default -->
<external_name tool="IDA-PRO" name="hcs12x"/>

<!-- HCS12:BE:24:default -->
<external_name tool="IDA-PRO" name="hcs12x"/>
```

### MCS96

File:
`Ghidra/Processors/MCS96/data/languages/MCS96.ldefs`

IDA `i196.dll` exposes `80196`, `80196NP`, `8096`, `8061`, and `8065`. Ghidra
has a single MCS96 language, so all five aliases can point to it.

```xml
<!-- MCS96:LE:16:default -->
<external_name tool="IDA-PRO" name="80196"/>
<external_name tool="IDA-PRO" name="80196np"/>
<external_name tool="IDA-PRO" name="8096"/>
<external_name tool="IDA-PRO" name="8061"/>
<external_name tool="IDA-PRO" name="8065"/>
```

### SuperH

File:
`Ghidra/Processors/SuperH/data/languages/superh.ldefs`

IDA `sh3.dll` exposes `SH3`, `SH3B`, `SH4`, `SH4B`, and `SH2A`. Ghidra already
binds `sh4`/`sh4b` in the `SuperH4` package. The only direct missing stock
binding in the `SuperH` package is SH-2A.

```xml
<!-- SuperH:BE:32:SH-2A -->
<external_name tool="IDA-PRO" name="sh2a"/>
```

### V850

File:
`Ghidra/Processors/V850/data/languages/V850.ldefs`

IDA `nec850.dll` exposes `V850`, `V850E`, `V850E1`, `V850E2M`, and `RH850`.
Ghidra has a single V850 language in this install. The conservative binding is
`v850`; the broader aliases should be added only if the V850 language is
intended to cover those later variants.

```xml
<!-- V850:LE:32:default -->
<external_name tool="IDA-PRO" name="v850"/>
```

### Dalvik

File:
`Ghidra/Processors/Dalvik/data/languages/Dalvik.ldefs`

IDA `dalvik.dll` exposes `dalvik`. Ghidra has many same-size/same-endian Dalvik
language variants, so binding the default language is the least ambiguous
initial upstream patch.

```xml
<!-- Dalvik:LE:32:default -->
<external_name tool="IDA-PRO" name="dalvik"/>
```

## Needs Sample Confirmation

These names are present in IDA's `LPH` tables, but the exact Ghidra language
coverage or variant selection should be checked before sending broad aliases
upstream.

| Ghidra package | IDA module | Confirmed IDA names | Notes |
| --- | --- | --- | --- |
| `RISCV` | `riscv` | `riscv` | `RISCV:LE:32:AndeStar_v5` has no separate IDA name in IDA 9.3. Keep `riscv` on the generic RV32/RV64 languages unless Ghidra wants duplicate aliases. |
| `tricore` | `tricore` | `TRICORE` | One IDA name covers four Ghidra variants. Binding only default is conservative; binding all variants needs importer-side selection confidence. |
| `V850` | `nec850` | `V850`, `V850E`, `V850E1`, `V850E2M`, `RH850` | Only `v850` is clearly conservative for the current single Ghidra V850 language. |
| `SuperH` | `sh3` | `SH3`, `SH3B`, `SH4`, `SH4B`, `SH2A` | `sh4`/`sh4b` already exist in `SuperH4`. No stock Ghidra SH-3 language was found locally. |

## Ghidra Without Local IDA Match

These Ghidra processor packages have no `IDA-PRO` binding and no obvious local
IDA processor module in this IDA 9.3 install:

`8048`, `8085`, `BPF`, `CP1600`, `CR16`, `DATA`, `eBPF`, `HCS08`, `Hexagon`,
`Loongarch`, `M8C`.

This does not mean IDA cannot support them through third-party processors or
newer builds, only that the local IDA 9.3 processor directory did not expose a
clear stock match.

## IDA Without Local Ghidra Match

These IDA modules have no obvious Ghidra processor package in the local Ghidra
12.1.2 install, or the package exists but lacks a binding and needs a separate
language implementation/mapping audit:

`78k0`, `78k0s`, `ad218x`, `alpha`, `arc`, `c166`, `cli`, `dsp56k`, `f2mc`,
`fr`, `h8`, `h8500`, `i51`, `i860`, `i960`, `ia`, `kr1878`, `m32r`, `m65`,
`m65816`, `m740`, `m7700`, `m7900`, `mc6816`, `mc8`, `oakdsp`, `pc`, `pdp11`,
`rl78`, `rx`, `s390`, `sam8`, `script`, `spc700`, `st20`, `st7`, `st9`,
`tms32028`, `tms32054`, `tms32055`, `tms320c1`, `tms320c3`, `tms320c5`,
`tms320c6`, `unsp`, `wasm`, `xa`, `z8`.
