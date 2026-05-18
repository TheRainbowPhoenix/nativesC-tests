# types

gint:defs:types - Type definitions

## Macros

### `pad_nam2`

Giving a type to padding bytes is misguiding, let's hide it in a macro

```c
#define pad_nam2(c) _ ## c
```

---

### `pad_name`

```c
#define pad_name(c) pad_nam2(c)
```

---

### `pad`

```c
#define pad(bytes) uint8_t pad_name(__COUNTER__)[bytes]
```

---

### `byte_union`

byte_union() - union between an uint8_t 'byte' element and a bit field

```c
#define byte_union(name, fields) \
```

---

### `word_union`

word_union() - union between an uint16_t 'word' element and a bit field

```c
#define word_union(name, fields) \
```

---

### `lword_union`

lword_union() - union between an uint32_t 'lword' element and a bit field

```c
#define lword_union(name, fields) \
```

---

## Implementation

Source files:

- [src/rtc/rtc.c](https://github.com/ClasspadDev/gint/blob/dev/src/rtc/rtc.c)
- [src/gray/engine.c](https://github.com/ClasspadDev/gint/blob/dev/src/gray/engine.c)
- [src/gray/gpixel.c](https://github.com/ClasspadDev/gint/blob/dev/src/gray/gpixel.c)
- [src/gray/ggetpixel.c](https://github.com/ClasspadDev/gint/blob/dev/src/gray/ggetpixel.c)
- [src/r61524/r61524.c](https://github.com/ClasspadDev/gint/blob/dev/src/r61524/r61524.c)
- [src/kernel/hardware.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/hardware.c)
- [src/kernel/hhk3_start.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/hhk3_start.c)
- [src/kernel/start.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/start.c)
