//---
// JustUI.jlist: List widget with arbitrary, selectable children
//---

#ifndef _J_JLIST
#define _J_JLIST

#include <justui/defs.h>
#include <justui/jwidget.h>

typedef enum {
    /* Selected item is styled by the paint function or delegate */
    JLIST_SELECTION_MANUAL = 0,
    /* Selected item is indicated by inverting its rendered area */
    JLIST_SELECTION_INVERT = 1,
    /* Selected item is indicated by applying a background color */
    JLIST_SELECTION_BACKGROUND = 2,

} jlist_selection_style;

typedef struct {
    /* Delegate widget */
    jwidget *delegate;
    /* Whether item can be selected */
    bool selectable;
    /* Whether item can be triggered */
    bool triggerable;
    /* Selection style for jlist to draw */
    int8_t selection_style;
    /* Selection background color for JLIST_SELECTION_BACKGROUND */
    uint16_t selection_bg_color;

    /* The following fields are only applicable if there is no delegate. */

    /* Item's natural with and height, in pixels */
    int16_t natural_width, natural_height;

} jlist_item_info;

struct jlist;

/* Info function: should fill `info` with the data related to list element
   #index (starts at 0). `info` is guaranteed to be pre-initialized to 0. */
typedef void (*jlist_item_info_function)(struct jlist *list, int index,
    jlist_item_info *info);

/* Paint function: should draw element #index on the rectangle of size `w×h`
   at position `x,y`. If the item has a selection style that is not
   JLIST_SELECTION_MANUAL, the selection effect is handled by jlist. Otherwise,
   the paint function should check the `selected` parameter to apply any
   relevant styling. */
typedef void (*jlist_item_paint_function)(int x, int y, int w, int h,
    struct jlist *list, int index, bool selected);

/* jlist: List widget with arbitrary, selectable children

   This widget is used to make lists of selectable elements. The elements are
   backed by a model which is essentially associating an index in the list to
   some piece of user data.

   Elements can either be manually-rendered like jpainted, or be delegated to
   full widgets (eg. for editing in a list).

   In terms of layout, jlist is a raw vertical list of all of its items, with
   no spacing. Generally it is desirable to put it in a jframe to make it
   scroll; otherwise, it has rather unpredictable dimensions. */
typedef struct jlist {
    jwidget widget;

    /* Number of items */
    int item_count;
    /* Per-widget information */
    jlist_item_info *items;
    /* Item information and paint functions */
    jlist_item_info_function info_function;
    jlist_item_paint_function paint_function;

    /* Currently selected item, -1 if none */
    int cursor;
    /* Index of the currently touch-clicked item, -1 none */
    int touch_cursor;
    /* User data pointer */
    void *user;

} jlist;

/* Events */
extern uint16_t JLIST_ITEM_TRIGGERED;
extern uint16_t JLIST_SELECTION_MOVED;
extern uint16_t JLIST_MODEL_UPDATED;

/* jlist_create(): Create a new (empty) jlist. */
jlist *jlist_create(jlist_item_info_function info_function,
    jlist_item_paint_function paint_function, void *parent);

/* jlist_update_model(): Update jlists's information about the model
   The new model size is passed as parameter. The model is refreshed by
   repeatedly calling the info function. The user pointer is also updated. To
   keep it unchanged, pass `l->user` as third parameter. */
void jlist_update_model(jlist *l, int item_count, void *user);

/* jlist_clear(): Remove all items */
void jlist_clear(jlist *l);

/* jlist_select(): Move selection to a selectable item */
void jlist_select(jlist *l, int item);

/* jlist_selected_item(): Get currently selected item (-1 if none) */
int jlist_selected_item(jlist *l);

/* jlist_selected_region(): Get the currently selected region of the widget

   The region is returned as a jrect within the widget's coordinates. This is
   useful when the list is inside a frame, to scroll the frame to a suitable
   position after the list's selection moved. See jscrolledlist.

   The returned region is undefined if there is no selected item. */
jrect jlist_selected_region(jlist *l);

#endif /* _J_JLIST */
