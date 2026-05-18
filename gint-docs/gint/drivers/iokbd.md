# iokbd

gint:drivers:iokbd - I/O ports-driven keyboard scanner
//
//	This is for SH3 only. It reads key presses from ports A/B/M.

## Functions

### `iokbd_scan`

iokbd_scan() - scan ports A/B/M to generate 12 rows of key data Numbering of rows is consistent with the keyboard. @scan  12-byte buffer filled with row data

```c
void iokbd_scan(uint8_t *scan);
```

---

## Macros

## Implementation

Source files:

- [src/keysc/keysc.c](https://github.com/ClasspadDev/gint/blob/dev/src/keysc/keysc.c)
