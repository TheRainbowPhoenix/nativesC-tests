# display-fx

gint:display-fx - fx9860g drawing functions
//
//	This module is in charge of all monochrome rendering. The gray engine
//	has its own functions, but often relies on this module (because the
//	gray effect is created through two monochrome buffers).


## Data Structures


### `color_t`

Dimensions of the VRAM */
#define DWIDTH 128
#define DHEIGHT 64

/* gint VRAM address. This value must always point to a 4-aligned buffer of
   size 1024. Any function can use it freely to:
   - Use another video ram area (triple buffering or more, gray engine);
   - Implement additional drawing functions;
   - Store data when not drawing. */
extern uint32_t *gint_vram;

/* color_t - colors available for drawing
   The following colors are defined by the library:

     OPAQUE COLORS (override existing pixels)
       white, black	- the usual thing
       light, dark	- intermediate colors used with the gray engine

     OPERATORS (combine with existing pixels)
       none		- leaves unchanged
       invert		- inverts white <-> black, light <-> dark
       lighten		- shifts black -> dark -> light -> white -> white
       darken		- shifts white -> light -> dark -> black -> black

   Not all colors can be used with all functions. To avoid ambiguities, all
   functions explicitly indicate compatible colors.


**Fields**:

- `/* Opaque colors */
	C_WHITE    = 0,
	C_LIGHT    = 1,
	C_DARK     = 2,
	C_BLACK    = 3,

	/* Monochrome operators */
	C_NONE     = 4,
	C_INVERT   = 5,

	/* Gray operators */
	C_LIGHTEN  = 6,
	C_DARKEN   = 7,`


```c
enum color_t {
/* Opaque colors */
	C_WHITE    = 0,
	C_LIGHT    = 1,
	C_DARK     = 2,
	C_BLACK    = 3,

	/* Monochrome operators */
	C_NONE     = 4,
	C_INVERT   = 5,

	/* Gray operators */
	C_LIGHTEN  = 6,
	C_DARKEN   = 7,
};
```


---


## Macros


### `DWIDTH`

Dimensions of the VRAM


```c
#define DWIDTH 128
```


---


### `DHEIGHT`


```c
#define DHEIGHT 64
```


---
