# cpu

gint:cpu - CPU registers and built-in functions

## Functions

### `cpu_atomic_start`

Enter atomic mode This function enters "atomic mode", a mode where distractions to the CPU (mainly interrupts) are disabled. This is useful when doing critical operations on the hardware, because it ensures that no other code will see any intermediate state between the start and end of the atomic mode, thereby making the sequence atomic to other code. Atomic mode disables interrupts with IMASK=15, however it does not set BL=1 because exceptions never occur on their own (and it is desirable to have panic reports if the atomic code is buggy), and TLB misses are almost always desirable. If you want to set BL=1, you can do so with cpu_setSR(). This function uses a mutex so atomic mode can be started within atomic code; every cpu_atomic_start() must be paired with exactly one cpu_atomic_end(). Entering atomic mode several times does not affect the CPU state, however atomic mode will be exited only after all exits have been completed. Once atomic mod is exited the original value of IMASK at the first call to cpu_atomic_start() is restored.

```c
void cpu_atomic_start(void);
```

---

### `cpu_atomic_end`

Exit atomic mode There should be exactly one cpu_atomic_end() for each cpu_atomic_start().

```c
void cpu_atomic_end(void);
```

---

### `cpu_getVBR`

Read and write the VBR register

```c
uint32_t cpu_getVBR(void);
```

---

### `cpu_getCPUOPM`

Read and write the CPU Operation Mode register. After a write, the register is re-read and an (icbi) instruction is executed to apply the change. Non- writable bits should be left to their initial value during a write.

```c
uint32_t cpu_getCPUOPM(void);
```

---

### `cpu_getSR`

Read and write the SR register. When writing, only "permanent" bits are set: * MD, RB, BL, DSP, IMASK are set. * M, Q, S and T are not set to preserve the behavior of ongoing divisions and tests. You can change T with (sett) and (clrt). * RC, DMY, DMX and DF are not set: use (setrc), (setdmx), (setdmy), and (clrdmxy). DF is preserved for old-style (setrc) loops to work.

```c
cpu_sr_t cpu_getSR(void);
```

---

### `sleep`

Put the processor to sleep This function uses the [sleep] instruction to put the processor in sleep mode. This allows reduced energy consumption while waiting for interrupts. In some certain situations, sleeping would block the process that is being waited for (generally when on-chip memory is involved). When this occurs, this function will not sleep at all and instead return instantly. The indented use for sleep() is *always* in some sort of loop, as there is no guarantee about time or interrupts elapsed before this function returns.

```c
void sleep(void);
```

---

### `sleep_block`

Block processor sleep This function blocks processor sleep until sleep_unblock() is called. The driver maintains a counter of the number of blocking sources, therefore it is important that *every* sleep_block() is followed by a sleep_unblock().

```c
void sleep_block(void);
```

---

### `sleep_unblock`

Cancel a processor sleep block

```c
void sleep_unblock(void);
```

---

### `cpu_csleep_init`

cpu_csleep_t: A cancellable sleep function This object holds sleep code that can be disabled by an interrupt. The size and layout of this variable is dependent on some assembler code. This should be allocated on the stack, heap, or on-chip memory, because the data segment is not executable!

```c
void cpu_csleep_init(cpu_csleep_t *ics);
```

---

### `cpu_csleep_init`

Create an ICS function This function initializes the provided ICS routine.

```c
void cpu_csleep_init(cpu_csleep_t *ics);
```

---

### `cpu_csleep`

Run the sleep function until the sleep is cancelled

```c
void cpu_csleep(cpu_csleep_t *ics);
```

---

### `cpu_csleep_cancel`

Cancel the sleep function from within an interrupt

```c
void cpu_csleep_cancel(cpu_csleep_t *ics);
```

---

### `cpu_configure_VBR`

Select the VBR address to be loaded in the driver This function can be used before the driver is configure()'d. It sets the VBR address that will be used in the next world to be initialized. After a configure(), this is reset to 0.

```c
void cpu_configure_VBR(uint32_t VBR);
```

---

## Macros

## Implementation

Source files:

- [src/dma/dma.c](https://github.com/ClasspadDev/gint/blob/dev/src/dma/dma.c)
- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/kernel/kernel.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/kernel.c)
- [src/kernel/world.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/world.c)
- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/intc/intc.c](https://github.com/ClasspadDev/gint/blob/dev/src/intc/intc.c)
- [src/tmu/tmu.c](https://github.com/ClasspadDev/gint/blob/dev/src/tmu/tmu.c)
- [src/cpu/cpu.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpu/cpu.c)
