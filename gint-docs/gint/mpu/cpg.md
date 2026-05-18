# cpg

gint:mpu:cpg - Clock Pulse Generator

## Macros

### `SH7705_CPG`

```c
#define SH7705_CPG (*((sh7705_cpg_t *)0xffffff80))
```

---

### `SH7305_CPG`

```c
#define SH7305_CPG (*((sh7305_cpg_t *)0xa4150000))
```

---

## Implementation

Source files:

- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/cpg/overclock.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/overclock.c)
- [src/cpg/cpg.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/cpg.c)
- [src/spu/spu.c](https://github.com/ClasspadDev/gint/blob/dev/src/spu/spu.c)
