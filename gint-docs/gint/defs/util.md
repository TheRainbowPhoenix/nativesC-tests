# util

gint:defs:util - Various utility macros

## Macros

### `synco`

synco instruction (in a form compatible with sh3eb-elf)

```c
#define synco() __asm__ volatile (".word 0x00ab":::"memory")
```

---

### `GAUTOTYPE`

```c
#define GAUTOTYPE auto
```

---

### `GAUTOTYPE`

```c
#define GAUTOTYPE __auto_type
```

---

### `min`

min(), max() (without double evaluation)

```c
#define min(a, b) ({			\
```

---

### `max`

```c
#define max(a, b) ({			\
```

---

### `sgn`

sgn() (without double evaluation)

```c
#define sgn(s) ({			\
```

---

### `swap`

swap() - exchange two variables of the same type

```c
#define swap(a, b) ({			\
```

---

## Implementation

Source files:

- [src/kmalloc/arena_gint.c](https://github.com/ClasspadDev/gint/blob/dev/src/kmalloc/arena_gint.c)
- [src/kmalloc/kmalloc.c](https://github.com/ClasspadDev/gint/blob/dev/src/kmalloc/kmalloc.c)
- [src/gray/grect.c](https://github.com/ClasspadDev/gint/blob/dev/src/gray/grect.c)
- [src/gray/gint_gline.c](https://github.com/ClasspadDev/gint/blob/dev/src/gray/gint_gline.c)
- [src/r61524/r61524.c](https://github.com/ClasspadDev/gint/blob/dev/src/r61524/r61524.c)
- [src/kernel/hardware.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/hardware.c)
- [src/usb/pipes.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/pipes.c)
- [src/render/dline.c](https://github.com/ClasspadDev/gint/blob/dev/src/render/dline.c)
