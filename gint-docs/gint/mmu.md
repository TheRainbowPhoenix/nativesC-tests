# mmu

gint:mmu - Memory Management Unit

## Functions

### `mmu_translate`

Get the physical address for a virtual page Looks for a translation with the specified virtual address as start, and returns the corresponding physical address. Only works if the argument is page-aligned. @virtual  Virtual page address @size     If provided, set to the size of the page Returns the page's physical address, or -1 if not mapped.

```c
uint32_t mmu_translate(uint32_t page, uint32_t *size);
```

---

### `*mmu_uram`

Get pointer to physical start of user RAM Returns a pointer to the physical location behind 0x08100000. The physical location can be used to access without the TLB, which is useful when interrupts are processed with SR.BL=1. However, the location is highly platform-dependent.

```c
void *mmu_uram(void);
```

---

### `mmu_uram_size`

Get size of user RAM area Returns the size of the static memory at 0x08100000, whose address is returned by mmu_uram(). This is typically 8k on SH3 fx-9860G, 32k on SH4 fx-9860G, and 512k on fx-CG 50.

```c
uint32_t mmu_uram_size(void);
```

---

### `mmu_is_rom`

Determine if an address points to ROM Checks whether the supplied pointer points to ROM or to a virtualized portion of ROM. For the sake of efficiency, this function uses heuristics about the structure of P0 rather than actually checking the TLB. This is useful during filesystem accesses because only data outside of ROM can be written to files. Pointers for which this function returns true cannot be used as a source for BFile_Write().

```c
bool mmu_is_rom(void const *ptr);
```

---

### `tlb_mapped_memory`

tlb_mapped_memory() - count amount of mapped memory This function returns the amount of mapped text and data segment memory, in bytes. The ranges are defined as follows: ROM  00300000:512k RAM  08100000:512k Other mappings are ignored. Both pointers may be NULL. @rom  Pointer to amount of mapped ROM @ram  Pointer to amount of mapped RAM

```c
void tlb_mapped_memory(uint32_t *p_rom, uint32_t *p_ram);
```

---

### `tlb_translate`

Get the physical address for a virtual page

```c
uint32_t tlb_translate(uint32_t page, uint32_t *size);
```

---

### `utlb_mapped_memory`

utlb_mapped_memory() - count amount of mapped memory This function returns the amount of mapped text and data segment memory, in bytes. The ranges are defined as follows: ROM  00300000:4M RAM  08100000:512k Other mappings are ignored. Both pointers may be NULL. @rom  Pointer to amount of mapped ROM @ram  Pointer to amount of mapped RAM

```c
void utlb_mapped_memory(uint32_t *rom, uint32_t *ram);
```

---

### `utlb_translate`

Get the physical address for a virtual page

```c
uint32_t utlb_translate(uint32_t page, uint32_t *size);
```

---

## Macros

## Implementation

Source files:

- [src/kernel/hardware.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/hardware.c)
- [src/kernel/kernel.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/kernel.c)
- [src/kernel/start.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/start.c)
- [src/image/image_free.c](https://github.com/ClasspadDev/gint/blob/dev/src/image/image_free.c)
- [src/mmu/mmu.c](https://github.com/ClasspadDev/gint/blob/dev/src/mmu/mmu.c)
- [src/fs/fugue/fugue.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/fugue/fugue.c)
