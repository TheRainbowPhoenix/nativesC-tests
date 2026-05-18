# display

gint:display - Drawing functions
//
//	This module covers the drawing functions that are common to fx9860g and
//	fxcg50. Platform-specific definitions are found in <gint/display-fx.h>
//	and <gint/display-cg.h>.


## Functions


### `dupdate`

Push the video RAM to the display driver This function makes the contents of the VRAM visible on the screen. It is the equivalent of Bdisp_PutDisp_DD() in most situations. On fx-9860G, this function also manages the gray engine settings. When the gray engine is stopped, it pushes the contents of the VRAM to screen, and when it is on, it swaps buffer and lets the engine's timer push the VRAMs to screen when suitable. To make the transition between the two modes smooth, dgray() does not enable the gray engine immediately; instead the first call to update() after dgray() switches the gray engine on and off. On fx-CG 50, because rendering is slow and sending data to screen is also slow, a special mechanism known as triple buffering is implemented. Two VRAMs are allocated, and frames are alternately rendered on each VRAM. When dupdate() is called to push one of the VRAMs to screen, it starts the DMA (which performs the push in the background) and immediately returns after switching to the other VRAM so that the user can draw during the push. The transfer from the DMA to the screen lasts about 10 ms if memory is available every cycle. Each access to the memory delays the transfer by a bit, so to fully use the available time, try to run AIs, physics or other calculation-intensive code rather than using the VRAM. Typical running time without overclock: fx-9860G: 1 ms (gray engine off) fx-9860G: ~30% of CPU time (gray engine on) fx-CG 50: 11 ms


```c
void dupdate(void);
```


---


### `dupdate_set_hook`

Define a function to be called after each dupdate() This functions configures the update hook, which is called after each dupdate() has sent VRAM to the display (but before VRAMs are switched when triple-buffering is used on fx-CG 50). The hook is mostly useful to send a copy of the frame to another medium, typically through a USB connection to a projector-style application. See usb_fxlink_videocapture() in <gint/usb-ff-bulk.h> for an example. The function is an indirect call; create one with the GINT_CALL() macro from <gint/defs/call.h>. Pass GINT_CALL_NULL to disable the feature. Indirect call to perform after each dupdate().


```c
void dupdate_set_hook(gint_call_t function);
```


---


### `dupdate_get_hook`

Get a copy of the dupdate() hook


```c
gint_call_t dupdate_get_hook(void);
```


---


### `dclear`

Fill the screen with a single color This function clears the screen by painting all the pixels in a single color. It is optimized for opaque colors; on fx-CG 50, it uses dma_memset() to fill in the VRAM. Typical running time without overclock: fx-9860G SH3:  70 µs fx-9860G SH4:  15 µs fx-CG 50:      2.5 ms (full-screen drect() is about 6 ms) On fx9860g, use drect() if you need operators that modify existing pixels instead of replacing them such as invert. @color  fx-9860G: white light* dark* black fx-CG 50: Any R5G6B5 color *: When the gray engine is on, see dgray().


```c
void dclear(color_t color);
```


---


### `drect`

Fill a rectangle of the screen This functions applies a color or an operator to a rectangle defined by two points (x1 y1) and (x2 y2). Both are included in the rectangle. @x1 @y1  Top-left rectangle corner (included) @x2 @y2  Bottom-right rectangle corner (included) @color  fx-9860G: white light* dark* black none invert lighten* darken* fx-CG 50: Any R5G6B5 color, C_NONE or C_INVERT *: When the gray engine is on, see dgray().


```c
void drect(int x1, int y1, int x2, int y2, int color);
```


---


### `dpixel`

Change a pixel's color Paints the selected pixel with an opaque color or applies an operator to a pixel. Setting pixels individually is a slow method for rendering. Other functions that draw lines, rectangles, images or text will take advantage of possible optimizations to make the rendering faster, so prefer using them when they apply. On fx-9860G, if an operator such as invert is used, the result will depend on the current color of the pixel. @x @y   Coordinates of the pixel to repaint @color  fx-9860G: white light* dark* black none invert lighten* darken* fx-CG 50: Any R5G6B5 color, C_NONE or C_INVERT *: When the gray engine is on, see dgray().


```c
void dpixel(int x, int y, int color);
```


---


### `dgetpixel`

Get a pixel's color Returns the current color of any pixel in the VRAM. This function ignores the rendering window. Returns -1 if (x,y) is out of bounds.


```c
int dgetpixel(int x, int y);
```


---


### `dline`

Render a straight line This function draws a line using a Bresenham-style algorithm. Please note that dline() may not render lines exactly like Bdisp_DrawLineVRAM(). dline() has optimizations for horizontal and vertical lines. The speedup for horizontal lines is about x2 on fx-CG 50 and probably about x30 on fx-9860G. Vertical lines have a smaller speedup. dline() is currently not able to clip arbitrary lines without calculating all the pixels, so drawing a line from (-1e6,0) to (1e6,395) will work but will be very slow. @x1 @y1 @x2 @y2  Endpoints of the line (both included). @color           Line color (same values as dpixel() are allowed)


```c
void dline(int x1, int y1, int x2, int y2, int color);
```


---


### `dhline`

Full-width horizontal line This function draws a horizontal line from the left end of the screen to the right end, much like the Basic command "Horizontal". @y      Line number @color  Line color (same values as dline() are allowed)


```c
void dhline(int y, int color);
```


---


### `dvline`

Full-height vertical line This function draws a vertical line from the top end of the screen to the bottom end, much like the Basic command "Vertical". @x      Column number @color  Line color (same values as dline() are allowed)


```c
void dvline(int x, int color);
```


---


### `dcircle`

Circle with border This function renders a circle defined by its middle point (xm, ym) and a radius r. The border and fill can be set separately. This function uses Bresenham's algorithm and only renders circles of odd diameter; if you want an even diameter, use dellipse() with a bounding box. @xm @ym        Coordinates of center @r             Radius (integer, >= 0) @fill_color    Color of the disc inside the circle, C_NONE to disable @border_color  Color of the circle itself, C_NONE to disable


```c
void dcircle(int xm, int ym, int r, int fill_color, int border_color);
```


---


### `dpoly`

Render an arbitrary polygon Renders the polygon defined by vertices (x[i], y[i]) for 0 < i < N. A fill color and border color can be specified separately. For filling, N must be at least 3. For border, N must be at least 2. Note: this is a fairly slow function, not designed for rendering triangles in 3D games. There are faster methods for that. @x @y          Arrays of vertex coordinates @N             Number of vertices (length of x and y) @fill_color    Color of the polygon's interior, C_NONE to disable @border_color  Color of the polygon's border, C_NONE to disable


```c
void dpoly(int const *x, int const *y, int N, int fill_color, int border_color);
```


---


### `dsize`

Get the width and height of rendered text This function computes the size that the given string would take up if rendered with a certain font. If you specify a NULL font, the currently configured font will be used; this is different from dfont(), which uses gint's default font when NULL is passed. Note that the height of each glyph is not stored in the font, only the maximum. Usually this is what you want because vertically-centered strings must have the same baseline regardless of their contents. So the height set by dsize() is the same for all strings and only depends on the font. The height is computed in constant time, and the width in linear time. If the third argument is NULL, this function returns in constant time. @str   String whose size must be evaluated @font  Font to use; if NULL, defaults to the current font @w @h  Set to the width and height of the rendered text, may be NULL


```c
void dsize(char const *str, font_t const *font, int *w, int *h);
```


---


### `dnsize`

Get the width and height of rendered text, with character limit This function is similar to dsize(), but stops after (size) bytes If (size < 0), there is no limit and this function is identical to dsize(). @str   String whose size must be evaluated @size  Maximum number of bytes to read from (str) @font  Font to use; if NULL, defaults to the current font @w @h  Set to the width and height of the rendered text, may be NULL


```c
void dnsize(char const *str, int size, font_t const *font, int *w, int *h);
```


---


### `dtext`

The last parameter @size was added in gint 2.4; this macro will add it automatically with a value of -1 if the call is made with only 7 parameters. This is for backwards compatibility.


```c
void dtext(int x, int y, int fg, char const *str);
```


---


### `dtext`

Simple version of dtext_opt() with defaults Calls dtext_opt() with bg=C_NONE, halign=DTEXT_LEFT and valign=DTEXT_TOP.


```c
void dtext(int x, int y, int fg, char const *str);
```


---


### `dprint`

Display a formatted string This function is exactly like dtext_opt(), but accepts printf-like formats with arguments. See <gint/std/stdio.h> for a detailed view of what this format supports. For example: dprint_opt(x, y, fg, bg, halign, valign, "A=%d B=%d", A, B);


```c
void dprint(int x, int y, int fg, char const *format, ...);
```


---


### `dprint`

Simple version of dprint_op() with defaults Calls dprint_opt() with bg=C_NONE, halign=DTEXT_LEFT and valign=DTEXT_TOP


```c
void dprint(int x, int y, int fg, char const *format, ...);
```


---


### `dfont_glyph_index`

Obtain the glyph index of a Unicode code point Returns the position of code_point in the character table of the given font, or -1 if code_point is not part of that set.


```c
int dfont_glyph_index(font_t const *font, uint32_t code_point);
```


---


### `dfont_glyph_offset`

Use a font index to find the location of a glyph The provided glyph value (usuall obtained by dfont_glyph_index()) must be nonnegative. Returns the offset the this glyph's data in the font's data array. When using a proportional font, the size array is ignored.


```c
int dfont_glyph_offset(font_t const *font, uint glyph_number);
```


---


### `dtext_utf8_next`

Read the next UTF-8 code point of a string Returns the next code point and advances the string. Returns 0 (NUL) at the end of the string.


```c
uint32_t dtext_utf8_next(uint8_t const **str_pointer);
```


---


### `dimage`

The bopti_image_t structure is platform-dependent, see <gint/display-fx.h> and <gint/display-cg.h> if you're curious.


```c
void dimage(int x, int y, bopti_image_t const *image);
```


---


### `dimage`

Render a full image This function blits an image on the VRAM using gint's special format. It is a special case of dsubimage() where the full image is drawn with clipping. @x @y   Coordinates of the top-left corner of the image @image  Pointer to image encoded with fxconv for bopti


```c
void dimage(int x, int y, bopti_image_t const *image);
```


---


## Data Structures


### `dwindow`

Platform-specific functions include VRAM management and the definition of
   the color_t type. */

#if GINT_RENDER_MONO
#include <gint/display-fx.h>
#endif

#if GINT_RENDER_RGB
#include <gint/display-cg.h>
#endif

//---
//	Video RAM management
//---

/* dupdate(): Push the video RAM to the display driver

   This function makes the contents of the VRAM visible on the screen. It is
   the equivalent of Bdisp_PutDisp_DD() in most situations.

   On fx-9860G, this function also manages the gray engine settings. When the
   gray engine is stopped, it pushes the contents of the VRAM to screen, and
   when it is on, it swaps buffer and lets the engine's timer push the VRAMs to
   screen when suitable. To make the transition between the two modes smooth,
   dgray() does not enable the gray engine immediately; instead the first call
   to update() after dgray() switches the gray engine on and off.

   On fx-CG 50, because rendering is slow and sending data to screen is also
   slow, a special mechanism known as triple buffering is implemented. Two
   VRAMs are allocated, and frames are alternately rendered on each VRAM. When
   dupdate() is called to push one of the VRAMs to screen, it starts the DMA
   (which performs the push in the background) and immediately returns after
   switching to the other VRAM so that the user can draw during the push.

   The transfer from the DMA to the screen lasts about 10 ms if memory is
   available every cycle. Each access to the memory delays the transfer by a
   bit, so to fully use the available time, try to run AIs, physics or other
   calculation-intensive code rather than using the VRAM.

   Typical running time without overclock:
   fx-9860G: 1 ms (gray engine off)
   fx-9860G: ~30% of CPU time (gray engine on)
   fx-CG 50: 11 ms */
void dupdate(void);

/* dupdate_set_hook(): Define a function to be called after each dupdate()

   This functions configures the update hook, which is called after each
   dupdate() has sent VRAM to the display (but before VRAMs are switched when
   triple-buffering is used on fx-CG 50).

   The hook is mostly useful to send a copy of the frame to another medium,
   typically through a USB connection to a projector-style application. See
   usb_fxlink_videocapture() in <gint/usb-ff-bulk.h> for an example.

   The function is an indirect call; create one with the GINT_CALL() macro from
   <gint/defs/call.h>. Pass GINT_CALL_NULL to disable the feature.

   @function  Indirect call to perform after each dupdate(). */
void dupdate_set_hook(gint_call_t function);

/* dupdate_get_hook(): Get a copy of the dupdate() hook */
gint_call_t dupdate_get_hook(void);

//---
// Rendering mode control
//---

/* dmode: Rendering mode settings

   This structure indicates the current window settings. Rendering is limited
   to the rectangle spanning from (left,top) included, to (right,bottom)
   excluded. The default is from (0,0) to (DWIDTH,DHEIGHT).


**Fields**:

- `int left, top`

- `int right, bottom`


```c
struct dwindow {
int left, top;
   int right, bottom;
};
```


---


## Macros


### `dtext_opt8`

This is for backwards compatibility.


```c
#define dtext_opt8(x,y,fg,bg,h,v,str,sz,...) dtext_opt(x,y,fg,bg,h,v,str,sz)
```


---


### `dtext_opt`


```c
#define dtext_opt(...) dtext_opt8(__VA_ARGS__, -1)
```


---
