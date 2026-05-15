//---
// JustUI.jscrolledlist: A jlist inside a jframe
//---

#ifndef _J_JSCROLLEDLIST
#define _J_JSCROLLEDLIST

#include <justui/defs.h>
#include <justui/jlist.h>
#include <justui/jframe.h>

/* jscrolledlist: A jlist inside a jframe

   jlist as a variabled-size widget which is intended to be used inside a
   scrolling view like a jframe. However this still requires the jframe to
   scroll when the list cursor moves and when the model is refreshed.

   This utility widget does this wrapping. It does not have any specific
   functions, and instead returns the list and frame as its `->list` and
   `->frame` members. */
typedef struct {
    jwidget widget;
    jframe *frame;
    jlist *list;

} jscrolledlist;

/* Create a scrolled list; arguments are forwarded to the jlist. */
jscrolledlist *jscrolledlist_create(
    jlist_item_info_function info_function,
    jlist_item_paint_function paint_function,
    void *parent);

#endif /* _J_JSCROLLEDLIST */
