#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jscrolledlist.h>
#include "util.h"

#include <stdlib.h>

/* Type identifier for jscrolledlist */
static int jscrolledlist_type_id = -1;

jscrolledlist *jscrolledlist_create(
    jlist_item_info_function info_function,
    jlist_item_paint_function paint_function,
    void *parent)
{
    if(jscrolledlist_type_id < 0)
        return NULL;

    jscrolledlist *l = malloc(sizeof *l);
    if(!l)
        return NULL;

    jwidget_init(&l->widget, jscrolledlist_type_id, parent);
    jwidget_set_stretch(l, 1, 1, false);

    l->frame = jframe_create(l);
    if(!l->frame) {
        free(l);
        return NULL;
    }

    jwidget_set_stretch(l->frame, 1, 1, false);
    jframe_set_align(l->frame, J_ALIGN_LEFT, J_ALIGN_TOP);

    l->list = jlist_create(info_function, paint_function, l->frame);
    if(!l->list) {
        jwidget_destroy(l->frame);
        free(l);
        return NULL;
    }
    jwidget_set_stretch(l->list, 1, 1, false);

    return l;
}

static void shake_scroll(jscrolledlist *l, bool clamp)
{
    int cursor = jlist_selected_item(l->list);
    if(cursor >= 0) {
        jrect r = jlist_selected_region(l->list);
        jframe_scroll_to_region(l->frame, r, clamp);
    }
}

//---
// Polymorphic widget operations
//---

static void jscrolledlist_poly_layout(void *l0)
{
    jscrolledlist *l = l0;

    l->frame->widget.x = 0;
    l->frame->widget.y = 0;
    l->frame->widget.w = jwidget_content_width(l);
    l->frame->widget.h = jwidget_content_height(l);

    shake_scroll(l, false);
}

static bool jscrolledlist_poly_event(void *l0, jevent e)
{
    jscrolledlist *l = l0;

    if(e.type == JLIST_SELECTION_MOVED && e.source == l->list)
        shake_scroll(l, true);

    if(e.type == JLIST_MODEL_UPDATED && e.source == l->list)
        shake_scroll(l, true);

    return jwidget_poly_event(l, e);
}

/* jscrolledlist type definition */
static jwidget_poly type_jscrolledlist = {
    .name    = "jscrolledlist",
    .layout  = jscrolledlist_poly_layout,
    .event   = jscrolledlist_poly_event,
};

__attribute__((constructor))
static void j_register_jscrolledlist(void)
{
    jscrolledlist_type_id = j_register_widget(&type_jscrolledlist);
}
