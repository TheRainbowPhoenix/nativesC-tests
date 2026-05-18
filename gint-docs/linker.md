# Linker Script Guide

This document explains the gint linker script for fxCP add-ins that link with HollyHock3.

## Overview

The linker script defines how code and data are organized in memory for gint applications. It specifies memory regions, sections, and the layout of the final binary.

## Memory Regions

The linker script defines five memory regions:

### ram (Main RAM)

- **Origin**: `0x8c052800`
- **Size**: 337920 bytes (330 KB)
- **Purpose**: VRAM backup area, main fixed area to load code
- **Attributes**: Read, Write, Execute (rwx)

### eram (End-of-RAM)

- **Origin**: `0x8cff0000`
- **Size**: 61184 bytes (64 KB - 0x1100)
- **Purpose**: Space after HollyHock loader
- **Attributes**: Read, Write, Execute (rwx)

HollyHock itself loads at `0x8cfd0000` (128 KB before the application). This region fits after it.

### vbr (Vector Base Register)

- **Origin**: `0x8cffef00`
- **Size**: 4352 bytes (0x1100)
- **Purpose**: Space for interrupt vectors
- **Attributes**: Read, Write, Execute (rwx)

### ilram (On-chip IL Memory)

- **Origin**: `0xe5200000`
- **Size**: 4 KB
- **Purpose**: Fast on-chip instruction local memory
- **Attributes**: Read, Write, Execute (rwx)

### xyram (On-chip X and Y Memory)

- **Origin**: `0xe500e000`
- **Size**: 16 KB
- **Purpose**: Fast on-chip data memory for DSP operations
- **Attributes**: Read, Write, Execute (rwx)

## Sections

### ROM Sections

These sections contain code and read-only data:

#### .hh3 Section

Contains machine code for entry functions and compiler constructors/destructors.

```ld
.hh3 : {
    *(.hh3*)
    *(.eram*)
} > eram
```

#### .text Section

Contains executable code including:
- Entry function (`.text.entry`)
- Constructors (`.ctors`)
- Destructors (`.dtors`)
- Exchange handlers (`.gint.exch`)
- TLB handlers (`.gint.tlbh`)
- User code (`.text`, `.text.*`)

```ld
.text : {
    *(.text.entry)
    _bctors = . ;
    KEEP(*(.ctors .ctors.*))
    _ectors = . ;
    
    _bdtors = . ;
    KEEP(*(.dtors .dtors.*))
    _edtors = . ;
    
    *(.gint.exch)
    *(.gint.tlbh)
    *(.text .text.*)
} > ram
```

#### .gint.blocks Section

Contains gint's interrupt handler blocks. These are relocated at startup by the library/drivers.

```ld
.gint.blocks ALIGN(4) : ALIGN(4) {
    KEEP(*(.gint.blocks));
} > ram
```

#### .gint.drivers Section

Contains exposed driver interfaces. Required to start and configure drivers even if symbols are not referenced.

```ld
.gint.drivers ALIGN(4) : ALIGN(4) {
    _gint_drivers = . ;
    KEEP(*(SORT_BY_NAME(.gint.drivers.*)));
    _gint_drivers_end = . ;
} > ram
```

#### .rodata Section

Contains read-only data:
- Resources from fxconv converters
- Compiler-marked read-only data

```ld
.rodata ALIGN(4) : ALIGN(4) SUBALIGN(4) {
    *(.rodata.4)
    *(.rodata .rodata.*)
} > ram
```

### RAM Sections

These sections contain read-write data:

#### .data Section

Contains initialized read-write data and mapped code.

```ld
.data ALIGN(4) : ALIGN(4) {
    _ldata = LOADADDR(.data);
    _rdata = . ;
    
    *(.data .data.*)
    *(.gint.mapped)
    *(.gint.mappedrel)
} > ram
```

#### .ilram Section

Contains code for on-chip IL memory.

```ld
.ilram ALIGN(4) : ALIGN(4) {
    _lilram = ABSOLUTE(LOADADDR(.ilram) + ORIGIN(ram));
    _rilram = . ;
    *(.ilram)
} > ilram
```

#### .xyram Section

Contains code for on-chip X and Y memory.

```ld
.xyram ALIGN(4) : ALIGN(4) {
    _lxyram = ABSOLUTE(LOADADDR(.xyram) + ORIGIN(ram));
    _rxyram = . ;
    *(.xram .yram .xyram)
} > xyram
```

#### .bss Section

Contains uninitialized data. Wiped by the loader at startup.

```ld
.bss : {
    _rbss = . ;
    *(.bss .bss.* COMMON)
} > ram
```

#### .gint.bss Section

Contains gint's uninitialized BSS section for large data arrays.

```ld
.gint.bss : {
    *(.gint.bss)
    _euram = . ;  /* End of user RAM */
} > ram
```

### Discarded Sections

These sections are removed from the final binary:

```ld
/DISCARD/ : {
    *(.gint.rodata.sh3 .gint.data.sh3 .gint.bss.sh3)  /* SH3-only data */
    *(.jcr)  /* Java class registration */
    *(.eh_frame_hdr) *(.eh_frame)  /* Exception handling tables */
    *(.comment)  /* Compiler comments */
}
```

## Key Symbols

The linker script defines several important symbols:

| Symbol | Description |
|--------|-------------|
| `_brom` | Start of ROM mappings |
| `_srom` | Size of ROM mappings |
| `_bctors`, `_ectors` | Constructor table bounds |
| `_bdtors`, `_edtors` | Destructor table bounds |
| `_gint_exch_start`, `_gint_exch_end` | Exchange handler bounds |
| `_gint_tlbh_start`, `_gint_tlbh_end` | TLB handler bounds |
| `_gint_drivers`, `_gint_drivers_end` | Driver info bounds |
| `_ldata`, `_rdata` | Data load/runtime addresses |
| `_sdata` | Size of data sections |
| `_lilram`, `_rilram` | ILRAM addresses |
| `_silram` | Size of ILRAM |
| `_lxyram`, `_rxyram` | XYRAM addresses |
| `_sxyram` | Size of XYRAM |
| `_rbss` | BSS start |
| `_sbss` | BSS size |
| `_sgbss` | gint BSS size |
| `_euram` | End of user RAM |
| `_gint_region_vbr` | VBR region origin |

## Implementation

Source file: [fxcp_hh3.ld.c](https://github.com/ClasspadDev/gint/blob/dev/fxcp_hh3.ld.c)
