# video

gint:video - Generic video interface
//
// This header defines the interface for video (display) drivers. It allows
// high-level code to manipulate the display independently of the underlying
// hardware.

## Functions

### `video_get_current_mode_index`

Get the video interface currently in use. This can be NULL if the program is being linked without a display driver.

```c
int video_get_current_mode_index(void);
```

---

### `video_get_current_mode_index`

Get the index of the current video mode. This can be -1 if there is no interface; however, if there is an interface, this is always >= 0.

```c
int video_get_current_mode_index(void);
```

---

### `video_update`

Update the contents of the display from a framebuffer image. A combination of `VIDEO_UPDATE_*` flags can be specified to select the update method: - `ENABLE_DMA` allows the driver to copy using DMA, when applicable; - `ATOMIC` requires the driver to use an interrupt-less method. Returns true on success. Update flags will be ignored if not applicable (e.g. `ENABLE_DMA` for a video interface that doesn't support DMA) but will result in an error if applicable and the specified method fails (e.g. DMA transfer error). This function is usually called with (x,y) = (0,0) and a contiguous framebuffer whose size is the video mode size. Specifying images with other sizes, positions and strides is allowed only when they result in data transfers that are byte-aligned. DMA support is only guaranteed for contiguous input and output. The implied rectangle must be in-bounds.

```c
bool video_update(int x, int y, image_t const *fb, int flags);
```

---

## Data Structures

### `video_mode_t`

Video mode offered by a driver for rendering.

**Fields**:

- `/* Mode size */
    uint16_t width`

- `uint16_t height`

- `/* Pixel format */
    int16_t format`

- `/* Refresh frequency, -1 if unknown */
    int16_t freq`

```c
struct video_mode_t {
/* Mode size */
    uint16_t width;
    uint16_t height;
    /* Pixel format */
    int16_t format;
    /* Refresh frequency, -1 if unknown */
    int16_t freq;
};
```

---

### `video_interface_t`

Flags for the update function of the interface. */
#define VIDEO_UPDATE_NONE           0x00
#define VIDEO_UPDATE_ENABLE_DMA     0x01
#define VIDEO_UPDATE_ATOMIC         0x02
#define VIDEO_UPDATE_FOREIGN_WORLD  0x04

/* Video driver interface.

**Fields**:

- `/* Associated driver (NULL if there's none). */
    gint_driver_t const *driver`

- `/* List of modes (terminated by an all-0 element). The first mode is the
       default mode and it should always be available. */
    video_mode_t const *modes`

- `/* Get current video mode. */
    uint (*mode_get)(void)`

- `/* Set a video mode. */
    bool (*mode_set)(uint id)`

- `/* Minimum and maximum brightness settings. */
    int brightness_min`

- `int brightness_max`

- `/* Set a brightness setting. */
    bool (*brightness_set)(int setting)`

- `/* Implements video_update()`

- `bounds are checked befoer calling. */
    bool (*update)(int x, int y, image_t const *framebuffer, int flags)`

```c
struct video_interface_t {
/* Associated driver (NULL if there's none). */
    gint_driver_t const *driver;

    /* List of modes (terminated by an all-0 element). The first mode is the
       default mode and it should always be available. */
    video_mode_t const *modes;
    /* Get current video mode. */
    uint (*mode_get)(void);
    /* Set a video mode. */
    bool (*mode_set)(uint id);

    /* Minimum and maximum brightness settings. */
    int brightness_min;
    int brightness_max;
    /* Set a brightness setting. */
    bool (*brightness_set)(int setting);

    /* Implements video_update(); bounds are checked befoer calling. */
    bool (*update)(int x, int y, image_t const *framebuffer, int flags);
};
```

---

## Macros

### `VIDEO_UPDATE_NONE`

Flags for the update function of the interface.

```c
#define VIDEO_UPDATE_NONE 0x00
```

---

### `VIDEO_UPDATE_ENABLE_DMA`

```c
#define VIDEO_UPDATE_ENABLE_DMA 0x01
```

---

### `VIDEO_UPDATE_ATOMIC`

```c
#define VIDEO_UPDATE_ATOMIC 0x02
```

---

### `VIDEO_UPDATE_FOREIGN_WORLD`

```c
#define VIDEO_UPDATE_FOREIGN_WORLD 0x04
```

---

## Implementation

Source files:

- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/r61524/r61524.c](https://github.com/ClasspadDev/gint/blob/dev/src/r61524/r61524.c)
- [src/render-cg/dvram.c](https://github.com/ClasspadDev/gint/blob/dev/src/render-cg/dvram.c)
- [src/r61523/r61523.c](https://github.com/ClasspadDev/gint/blob/dev/src/r61523/r61523.c)
- [src/video/video.c](https://github.com/ClasspadDev/gint/blob/dev/src/video/video.c)
