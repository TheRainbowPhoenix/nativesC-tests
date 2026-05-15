#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jlist.h>
#include <justui/config.h>
#include "util.h"

#include <gint/display.h>
#include <stdlib.h>
#include <string.h>

/* Type identifier for jlist */
static int jlist_type_id = -1;

/* Events */
uint16_t JLIST_ITEM_TRIGGERED;
uint16_t JLIST_SELECTION_MOVED;
uint16_t JLIST_MODEL_UPDATED;

struct jlist_item_info {
    /* Whether the item can be selected */
    bool selectable;
};

jlist *jlist_create(jlist_item_info_function info_function,
    jlist_item_paint_function paint_function, void *parent)
{
    if(jlist_type_id < 0)
        return NULL;

    jlist *l = malloc(sizeof *l);
    if(!l)
        return NULL;

    jwidget_init(&l->widget, jlist_type_id, parent);
    jwidget_set_focus_policy(l, J_FOCUS_POLICY_ACCEPT);

    l->item_count = 0;
    l->items = NULL;
    l->info_function = info_function;
    l->paint_function = paint_function;
    l->cursor = -1;
    l->touch_cursor = -1;
    l->user = NULL;
    return l;
}

//---
// Selection management
//---

static int prev_selectable(jlist *l, int cursor)
{
    for(int i = cursor - 1; i >= 0; i--) {
        if(l->items[i].selectable)
            return i;
    }
    return cursor;
}

static int next_selectable(jlist *l, int cursor)
{
    for(int i = cursor + 1; i < l->item_count; i++) {
        if(l->items[i].selectable)
            return i;
    }
    return cursor;
}

static int first_selectable(jlist *l)
{
    return next_selectable(l, -1);
}

static int last_selectable(jlist *l)
{
    int p = prev_selectable(l, l->item_count);
    return p == l->item_count ? -1 : p;
}

static int nearest_selectable(jlist *l, int cursor)
{
    if(cursor < 0)
        return first_selectable(l);
    if(cursor >= l->item_count)
        return last_selectable(l);
    if(l->items[cursor].selectable)
        return cursor;

    int i = prev_selectable(l, cursor);
    if(i != cursor)
        return i;
    i = next_selectable(l, cursor);
    if(i != cursor)
        return i;
    return -1;
}

void jlist_select(jlist *l, int cursor)
{
    /* Normalize out-of-bounds to -1 */
    if(cursor < 0 || cursor >= l->item_count)
        cursor = -1;
    if(l->cursor == cursor || (cursor > 0 && !l->items[cursor].selectable))
        return;

    l->cursor = cursor;
    jwidget_emit(l, (jevent){ .type = JLIST_SELECTION_MOVED });
    l->widget.update = 1;
}

int jlist_selected_item(jlist *l)
{
    return l->cursor;
}

//---
// Item management
//---

void jlist_update_model(jlist *l, int item_count, void *user)
{
    l->user = user;

    if(l->item_count != item_count) {
        l->items = realloc(l->items, item_count * sizeof *l->items);
        if(!l->items) {
            l->item_count = 0;
            l->cursor = -1;
            l->widget.dirty = 1;
            return;
        }
    }

    l->item_count = item_count;
    for(int i = 0; i < item_count; i++) {
        memset(&l->items[i], 0, sizeof l->items[i]);
        l->info_function(l, i, &l->items[i]);
    }

    jlist_select(l, nearest_selectable(l, l->cursor));
    jwidget_emit(l, (jevent){ .type = JLIST_MODEL_UPDATED });
    l->widget.dirty = 1;
}

void jlist_clear(jlist *l)
{
    jlist_update_model(l, 0, NULL);
}

jrect jlist_selected_region(jlist *l)
{
    int y=0, h=0;

    for(int i = 0; i <= l->cursor; i++) {
        jlist_item_info *info = &l->items[i];
        y += h;
        if(info->delegate)
            h = jwidget_full_height(info->delegate);
        else
            h = info->natural_height;
    }

    return (jrect){ .x = 0, .y = y, .w = jwidget_content_width(l), .h = h };
}

//---
// Polymorphic widget operations
//---

GUNUSED static int item_index_at(void *l0, int ly)
{
    jlist *l = l0;
    int y = 0;

    for(int i = 0; i < l->item_count; i++) {
        jlist_item_info *info = &l->items[i];
        int h = info->delegate
            ? jwidget_full_height(info->delegate)
            : info->natural_height;

        if(ly >= y && ly < y + h)
            return i;
        y += h;
    }

    return -1;
}

static void jlist_poly_csize(void *l0)
{
    jlist *l = l0;
    jwidget *w = &l->widget;

    w->w = 0;
    w->h = 0;

    for(int i = 0; i < l->item_count; i++) {
        jlist_item_info *info = &l->items[i];
        int item_w, item_h;

        if(info->delegate) {
            item_w = jwidget_full_width(info->delegate);
            item_h = jwidget_full_height(info->delegate);
        }
        else {
            item_w = info->natural_width;
            item_h = info->natural_height;
        }

        w->w = max(w->w, item_w);
        w->h += item_h;
    }
}

static void jlist_poly_render(void *l0, int x, int y)
{
    jlist *l = l0;
    int x1 = x;
    int x2 = x + jwidget_content_width(l) - 1;

    for(int i = 0; i < l->item_count; i++) {
        jlist_item_info *info = &l->items[i];
        bool selected = (l->cursor == i);

        int h = info->delegate
            ? jwidget_full_height(info->delegate)
            : info->natural_height;

        if(selected && info->selection_style == JLIST_SELECTION_BACKGROUND)
            drect(x1, y, x2, y + h - 1, info->selection_bg_color);

        if(info->delegate)
            jwidget_render(info->delegate, x1, y);
        else
            l->paint_function(x1, y, x2-x1+1, h, l, i, selected);

        if(selected && info->selection_style == JLIST_SELECTION_INVERT)
            drect(x1, y, x2, y + h - 1, C_INVERT);

        y += h;
    }
}

static bool jlist_poly_event(void *l0, jevent e)
{
    jlist *l = l0;

    if(e.type == JWIDGET_FOCUS_CHANGED && !jwidget_has_active_focus(l))
        l->touch_cursor = -1;

    if(e.type != JWIDGET_KEY || l->cursor < 0)
        return false;

    key_event_t ev = e.key;

#if J_CONFIG_TOUCH
    if(ev.type == KEYEV_TOUCH_DOWN || ev.type == KEYEV_TOUCH_DRAG ||
       ev.type == KEYEV_TOUCH_UP) {
        int lx = ev.x - jwidget_absolute_content_x(l);
        int ly = ev.y - jwidget_absolute_content_y(l);
        uint w = jwidget_content_width(l);
        uint h = jwidget_content_height(l);
        int index = ((uint)lx < w && (uint)ly < h) ? item_index_at(l, ly) : -1;

        if(ev.type == KEYEV_TOUCH_DOWN && index >= 0) {
            jlist_select(l, index);
            l->touch_cursor = jlist_selected_item(l);
        }
        if(ev.type == KEYEV_TOUCH_DRAG && index >= 0)
            jlist_select(l, index);
        if(ev.type == KEYEV_TOUCH_UP && index >= 0 && index == l->touch_cursor) {
            jevent e = { .type = JLIST_ITEM_TRIGGERED,
                         .data = l->touch_cursor };
            jwidget_emit(l, e);
        }
        return true;
    }
#endif

    if(ev.type != KEYEV_DOWN && ev.type != KEYEV_HOLD)
        return false;

    int key = ev.key;

    /* Cursor movement */

    if(key == KEY_UP && ev.shift && !ev.alpha) {
        jlist_select(l, first_selectable(l));
        return true;
    }
    if(key == KEY_DOWN && ev.shift && !ev.alpha) {
        jlist_select(l, last_selectable(l));
        return true;
    }
    if(key == KEY_UP && !ev.alpha) {
        jlist_select(l, prev_selectable(l, l->cursor));
        return true;
    }
    if(key == KEY_DOWN && !ev.alpha) {
        jlist_select(l, next_selectable(l, l->cursor));
        return true;
    }

    /* Triggering items */

    if((key == KEY_EXE || key == KEY_OK) && l->items[l->cursor].triggerable) {
        jevent e = { .type = JLIST_ITEM_TRIGGERED, .data = l->cursor };
        jwidget_emit(l, e);
        return true;
    }

    return jwidget_poly_event(l, e);
}

static void jlist_poly_destroy(void *l0)
{
    jlist *l = l0;
    free(l->items);
}

/* jlist type definition */
static jwidget_poly type_jlist = {
    .name    = "jlist",
    .csize   = jlist_poly_csize,
    .render  = jlist_poly_render,
    .event   = jlist_poly_event,
    .destroy = jlist_poly_destroy,
};

__attribute__((constructor))
static void j_register_jlist(void)
{
    jlist_type_id = j_register_widget(&type_jlist);
    JLIST_ITEM_TRIGGERED = j_register_event();
    JLIST_SELECTION_MOVED = j_register_event();
    JLIST_MODEL_UPDATED = j_register_event();
}
