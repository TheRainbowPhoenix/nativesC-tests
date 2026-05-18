# usb

gint:intc:usb - USB 2.0 Interface

## Macros

### `SH7305_USB`

```c
#define SH7305_USB (*(sh7305_usb_t *)0xa4d80000)
```

---

### `SH7305_USB_UPONCR`

```c
#define SH7305_USB_UPONCR (*(sh7305_usb_uponcr_t *)0xa40501d4)
```

---

## Implementation

Source files:

- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/usb/pipes.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/pipes.c)
- [src/usb/setup.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/setup.c)
