//---
// JustUI.jpainted: Widget defined by just a size and painting function
//---

#ifndef _J_JPAINTED
#define _J_JPAINTED

#include <justui/defs.h>
#include <justui/jwidget.h>

/* jpainted: Simple widget designed to integrate low-effort rendering

   This widget is simply a rectangle on which a custom function does the
   rendering. It allows the GUI to have custom rendering with very little code,
   as the widget only requires a natural size and a renderer.

   The natural size doesn't need to be exactly the size of the widget; ideally
   it should be larger than what the rendered needs. In any case the natural
   size is not the final one, as stretch can be used in layouts.

   This widget will typically be used like this:

     void draw_something(int x, int y) {
       dimage(x, y, ...);
       dtext(x+2, y+10, ...);
     }
     jpainted *widget = jpainted_create(draw_something, NULL, 10, 10, parent);

   The rendering function can optionally take an argument; this argument can be
   an integer or a pointer (essentially).

     void draw_value(int x, int y, uint32_t *value) {
       dprint(x, y, "%08X", *value);
     }
     uint32_t my_value;
     jpainted *widget = jpainted_create(draw_value, &my_value, 30, 8, parent);

   The type of the argument is defined as j_arg_t but you don't need to create
   a j_arg_t object, just pass the argument directly. If you don't have an
   argument, pass NULL.

   Note that in this example you will need to set (widget->update = 1) whenever
   you change (my_value), in order for the scene to produce a PAINT event.
   Otherwise, a change in (my_value) will not be seen on-screen until the next
   time the scene is painted.

   The layout and rendering will be performed automatically, allowing the GUI
   to benefit from automated positioning with virtually no overhead. */
typedef struct {
	jwidget widget;
	/* Renderer function and its argument */
	void (*paint)(int x, int y, j_arg_t arg);
	j_arg_t arg;
	/* Natural size */
	int16_t natural_w;
	int16_t natural_h;

} jpainted;

/* jpainted_create(): Create a simple painted widget */
jpainted *jpainted_create(void *function, j_arg_t arg, int natural_w,
  int natural_h, void *parent);

#endif /* _J_JPAINTED */
