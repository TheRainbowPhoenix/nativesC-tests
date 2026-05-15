#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jbutton.h>
#include <justui/config.h>

#include <stdlib.h>

J_DEFINE_WIDGET(jbutton, csize, render, event)
J_DEFINE_EVENTS(JBUTTON_TRIGGERED)

static void jbutton_set_state(jbutton *b, int state)
{
    if((uint)state >= JBUTTON_STATE_NUM || b->state == state)
        return;
    b->state = state;
    jwidget_set_background(b, b->bg_colors[state]);
    b->widget.update = 1;
}

jbutton *jbutton_create(char const *text, void *parent)
{
    if(jbutton_type_id < 0)
        return NULL;

    jbutton *b = malloc(sizeof *b);
    if(!b)
        return NULL;

    jwidget_init(&b->widget, jbutton_type_id, parent);
    jwidget_set_focus_policy(b, J_FOCUS_POLICY_ACCEPT);

    b->text = text ? text : "";
    b->font = dfont_default();

#if GINT_RENDER_MONO
    b->fg_colors[JBUTTON_IDLE] = C_WHITE;
    b->fg_colors[JBUTTON_ACTIVE] = C_WHITE;
    b->fg_colors[JBUTTON_DISABLED] = C_BLACK;
    b->bg_colors[JBUTTON_IDLE] = C_BLACK;
    b->bg_colors[JBUTTON_ACTIVE] = C_BLACK;
    b->bg_colors[JBUTTON_DISABLED] = C_WHITE;
    jwidget_set_padding(b, 1, 2, 1, 2);
#endif
#if GINT_RENDER_RGB
    b->fg_colors[JBUTTON_IDLE] = C_WHITE;
    b->fg_colors[JBUTTON_ACTIVE] = C_WHITE;
    b->fg_colors[JBUTTON_DISABLED] = C_RGB(8, 8, 8);
    b->bg_colors[JBUTTON_IDLE] = C_BLACK;
    b->bg_colors[JBUTTON_ACTIVE] = C_RGB(8, 8, 8);
    b->bg_colors[JBUTTON_DISABLED] = C_RGB(24, 24, 24);
    jwidget_set_padding(b, 2, 4, 2, 4);
#endif

    jbutton_set_state(b, JBUTTON_IDLE);
    return b;
}

void jbutton_set_text(jbutton *b, char const *text)
{
    b->text = text ? text : "";
    b->widget.dirty = 1;
}

void jbutton_set_font(jbutton *b, font_t const *font)
{
    b->font = font ? font : dfont_default();
    b->widget.dirty = 1;
}

void jbutton_set_disabled(jbutton *b, bool disabled)
{
    if(disabled)
        jbutton_set_state(b, JBUTTON_DISABLED);
    else if(b->state == JBUTTON_DISABLED)
        jbutton_set_state(b, JBUTTON_IDLE);
}

//---
// Polymorphic widget operations
//---

void jbutton_poly_csize(void *b0)
{
    jbutton *b = b0;
    int w, h;
    dsize(b->text, b->font ? b->font : dfont_default(), &w, &h);
    b->widget.w = w;
    b->widget.h = h;
}

void jbutton_poly_render(void *b0, int x, int y)
{
    jbutton *b = b0;
    int cw = jwidget_content_width(b);
    int ch = jwidget_content_height(b);

    int fg = b->fg_colors[b->state];
    font_t const *old_font = dfont(b->font);
    dtext_opt(x + cw / 2, y + ch / 2, fg, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE,
        b->text, -1);
    dfont(old_font);
}

bool jbutton_poly_event(void *b0, jevent e)
{
    jbutton *b = b0;
    key_event_t ev = { .type = KEYEV_NONE };
    if(e.type == JWIDGET_KEY)
        ev = e.key;

#if J_CONFIG_TOUCH
    bool accepts_touch = b->state != JBUTTON_DISABLED;
    if((ev.type == KEYEV_TOUCH_DOWN || ev.type == KEYEV_TOUCH_DRAG ||
        ev.type == KEYEV_TOUCH_UP) && accepts_touch) {
        int lx = ev.x - jwidget_absolute_padding_x(b);
        int ly = ev.y - jwidget_absolute_padding_y(b);
        uint cw = jwidget_padding_width(b);
        uint ch = jwidget_padding_height(b);
        bool inside = (uint)lx < cw && (uint)ly < ch;

        if(ev.type == KEYEV_TOUCH_DOWN)
            jbutton_set_state(b, JBUTTON_ACTIVE);
        if(ev.type == KEYEV_TOUCH_DRAG)
            jbutton_set_state(b, inside ? JBUTTON_ACTIVE : JBUTTON_IDLE);
        if(ev.type == KEYEV_TOUCH_UP && b->state == JBUTTON_ACTIVE) {
            jbutton_set_state(b, JBUTTON_IDLE);
            jwidget_emit(b, (jevent){ .type = JBUTTON_TRIGGERED });
        }
        return true;
    }
#endif

    if(ev.type == KEYEV_DOWN && (ev.key == KEY_EXE || ev.key == KEY_OK)) {
        jwidget_emit(b, (jevent){ .type = JBUTTON_TRIGGERED });
        return true;
    }

    return jwidget_poly_event(b, e);
}
