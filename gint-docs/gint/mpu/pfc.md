# pfc

gint:mpu:pfc - Pin Function Controller
//
//	The Pin Function Controller has a simple register interface, the main
//	difficulty is still understanding the role of its pins.

## Macros

### `SH7705_PFC`

```c
#define SH7705_PFC (*((sh7705_pfc_t *)0xa4000100))
```

---

### `SH7305_PFC`

```c
#define SH7305_PFC (*((sh7305_pfc_t *)0xa4050100))
```

---

## Implementation

Source files:

- [src/kernel/hardware.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/hardware.c)
- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/touch/i2c.c](https://github.com/ClasspadDev/gint/blob/dev/src/touch/i2c.c)
- [src/keysc/iokbd.c](https://github.com/ClasspadDev/gint/blob/dev/src/keysc/iokbd.c)
