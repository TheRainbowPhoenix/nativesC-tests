//---
// JustUI.jlayout: Widget positioning mechanisms
//---

#ifndef _J_JLAYOUT
#define _J_JLAYOUT

#include <justui/defs.h>

/* jlayout_type: Built-in layout mechanisms */
typedef enum {
	/* User or sub-class places children manually */
	J_LAYOUT_NONE = 0,
	/* Children are laid out in a vertical box; spacing applies. Extra
	   space is redistributed proportionally to the stretch attribute. */
	J_LAYOUT_VBOX,
	/* Same as J_LAYOUT_VBOX, but horizontally. */
	J_LAYOUT_HBOX,
	/* Children are stacked, only one is visible at a time. */
	J_LAYOUT_STACK,
	/* Children are laid out in a grid; spacing applies. */
	J_LAYOUT_GRID,

} jlayout_type;

//---
// Manual layout
//---

/* jlayout_set_manual(): Remove a layout from a widget
   This function removes the current layout of the widget. */
void jlayout_set_manual(void *w);

//---
// Box layout
//---

/* jlayout_box: Parameters for VBOX and HBOX layouts */
typedef struct {
	/* Spacing between elements */
	uint8_t spacing;
	uint :24;

} jlayout_box;

/* jlayout_get_hbox(), jlayout_get_vbox(): Get configuration for box layouts
   These functions return the jlayout_box parameters for widgets that have a
   box layout, and NULL otherwise. */
jlayout_box *jlayout_get_hbox(void *w);
jlayout_box *jlayout_get_vbox(void *w);

/* jlayout_set_hbox(), jlayout_set_vbox(): Create box layouts
   These functions configure the specified widget to have a box layout, and
   return the jlayout_box parameters to configure that layout. The parameters
   are initialized even if the widget previously had a box layout. */
jlayout_box *jlayout_set_hbox(void *w);
jlayout_box *jlayout_set_vbox(void *w);

//---
// Stack layouts
//---

/* jlayout_stack: Parameters for STACK layouts */
typedef struct {
	/* Index of currently-visible child */
	int8_t active;
	uint :24;

} jlayout_stack;

/* jlayout_get_stack(): Get configuration for stack layouts
   For widgets that have a stack layout, returns a pointer to the parameters.
   Otherwise, returns NULL. */
jlayout_stack *jlayout_get_stack(void *w);

/* jlayout_set_stack(): Create stack layouts
   Configure the specified widget to have a stack layout and return the new
   parameters. The new layout is cleared even if the widget previously had a
   stack layout. */
jlayout_stack *jlayout_set_stack(void *w);

//---
// Grid layouts
//---

typedef enum {
	/* Rows from top to bottom */
	J_LAYOUT_GRID_TOPDOWN,
	/* Rows from bottom to top */
	J_LAYOUT_GRID_BOTTOMUP,
	/* Columns from left to right */
	J_LAYOUT_GRID_LEFTRIGHT,
	/* Columns from right to left */
	J_LAYOUT_GRID_RIGHTLEFT,

} jlayout_grid_order;

typedef struct  {
	/* Spacing between rows and between columns */
	uint8_t row_spacing;
	uint8_t col_spacing;
	/* Child order. The major order specifies whether the grid is filled by
	   rows or by columns. The minor order specifies how these rows or
	   columns are filled themselves. There must be exactly one vertical
	   setting and one horizontal setting. */
	uint major_order :2;
	uint minor_order :2;
	/* Number of rows, and number of columns; default to -1, in which case
	   the minor size is determined based on the first minor group and the
	   major size is calculated greedily. */
	uint rows :6;
	uint cols :6;

} jlayout_grid;

/* TODO: Functions for grid layouts */

#endif /* _J_JLAYOUT */
