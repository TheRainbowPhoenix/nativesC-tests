//---
// JustUI.jbutton: Touchable multi-state click button
//---

#ifndef _J_JBUTTON
#define _J_JBUTTON

#include <justui/defs.h>
#include <justui/jwidget.h>

#include <gint/display.h>

/* jbutton: Button with multiple states

   This widget is your everyday interactible button. It responds to touch by
   changing its background color for idle, focused and active states. It also
   has a disabled states which can be used to make a "selected" visual.

   TODO: Allow jbutton it to hold any widget inside.
   TODO: jbutton's focus state is untested since I only tested touch.

   Events:
   * JBUTTON_TRIGGERED when activated. */

enum {
    JBUTTON_IDLE,
    JBUTTON_ACTIVE,
    JBUTTON_DISABLED,

    JBUTTON_STATE_NUM,
};

typedef struct {
    jwidget widget;

    /* Text shown on the button; not owned by the button */
    char const *text;
    /* Rendering font */
    font_t const *font;
    /* Colors for all states */
    color_t fg_colors[JBUTTON_STATE_NUM];
    color_t bg_colors[JBUTTON_STATE_NUM];
    /* Current state */
    int state;

} jbutton;

/* Event IDs */
extern uint16_t JBUTTON_TRIGGERED;

/* jbutton_create(): Create a button */
jbutton *jbutton_create(char const *text, void *parent);

/* Trivial properties */
void jbutton_set_text(jbutton *b, char const *text);
void jbutton_set_font(jbutton *b, font_t const *font);
void jbutton_set_disabled(jbutton *b, bool disabled);

#endif /* _J_JBUTTON */
