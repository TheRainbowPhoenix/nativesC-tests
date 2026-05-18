# display-cg

gint:display-cg - fx-CG 50 rendering functions
//
// This module covers rendering functions specific to the fx-CG 50. In addition
// to triple-buffering management, this mainly includes image manipulation
// tools as well as the very versatile dimage_effect() and dsubimage_effect()
// functions that support high-performance image rendering with a number of
// geometric and color effects.
//
// The fx-CG OS restricts the display to a 384x216 rectangle rougly around the
// center, leaving margins on three sides. However, gint configures the display
// to use the full 396x224 surface!

## Functions

### `dsetvram`

Control video RAM address and triple buffering Normal rendering under gint uses double-buffering: there is one image displayed on the screen and one in memory, in a region called the video RAM (VRAM). The application draws frames in the VRAM then sends them to the screen only when they are finished, using dupdate(). On fx-CG, sending frames with dupdate() is a common bottleneck because it takes about 11 ms. Fortunately, while the DMA is sending the frame to the display, the CPU is available to do work in parallel. This function sets up triple buffering (ie. a second VRAM) so that the CPU can start working on the next frame while the DMA is sending the current one. However, experience shows minimal performance improvements, because writing to main RAM does not parallelize with DMA transfers. Since gint 2.8, this is no longer the default, and the memory for the extra VRAM is instead available via malloc(). VRAMs must be contiguous, 32-aligned, (2*396*224)-byte buffers with 32 bytes of extra data on each side (ie. 32 bytes into a 32-aligned buffer of size 177472). @main       Main VRAM area, used alone if [secondary] is NULL @secondary  Additional VRAM area, enables triple buffering if non-NULL

```c
void dsetvram(uint16_t *main, uint16_t *secondary);
```

---

### `dgetvram`

dgetvram() - Get VRAM addresses Returns the VRAM buffer addresses used to render on fx-CG 50.

```c
void dgetvram(uint16_t **main, uint16_t **secondary);
```

---

### `*gint_vrambackup_get`

Predefine palette based on the GUI at the Hollyhock loading screen. Contains 109 entries plus a 110th dummy entry used internally as bounds check.

```c
void *gint_vrambackup_get(int *size);
```

---

### `*gint_vrambackup_get`

Get the pointer to the encoded VRAM backup created at load time. If [size] is not NULL, sets the size in [*size]. The pointer is heap allocated and remains owned by gint.

```c
void *gint_vrambackup_get(int *size);
```

---

### `gint_vrambackup_show`

Decode the load-time VRAM backup back to VRAM.

```c
void gint_vrambackup_show(void);
```

---

## Macros

### `DWIDTH`

```c
#define DWIDTH 396
```

---

### `DHEIGHT`

```c
#define DHEIGHT 224
```

---

### `DWIDTH`

```c
#define DWIDTH 320
```

---

### `DHEIGHT`

```c
#define DHEIGHT 528
```

---

### `C_RGB`

RGB color maker. Takes three channels of value 0..31 each (the extra bit of green is not used).

```c
#define C_RGB(r,g,b) (((r) << 11) | ((g) << 6) | (b))
```

---

## Implementation

Implementation is in the gint source tree.
