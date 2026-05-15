//---
// JustUI.jinput: One-line input field
//---

#ifndef _J_JINPUT
#define _J_JINPUT

#include <justui/defs.h>
#include <justui/jwidget.h>
#include <justui/p/vec.h>

#include <gint/display.h>

/* Keymap function for jinput. See jinput_set_keymap_function(). */
typedef uint32_t jinput_keymap_function_t(int key, bool shift, bool alpha);

/* jinput: One-line input field

   This widget is used to read input from the user. It has a single line of
   text which can be edited when focused, and an optional prompt. On the right,
   an indicator displays the status of modifier keys.

   The edition rules support both the OS' native one-key-at-time input system,
   and the usual computer modifier-keys-held method.

   * The normal insertion mode is used by default.
   * When pressing SHIFT or ALPHA in combination with a key (without releasing
     SHIFT or ALPHA, as on a computer), the secondary or alphabetic function of
     the key is used.
   * Pressing then releasing SHIFT or ALPHA activates the secondary or
     alphabetic function for the next key press.
   * Double-tapping SHIFT or ALPHA locks the corresponding mode on until the
     locked mode is disabled by another press-release of the same modifier key.

   TODO: jinput: Selection with SHIFT
   TODO: jscene: Clipboard support

   A timer is used to make the cursor blink, sending JSCENE_REPAINT events
   every second or so. The timer is held only during editing and freed when
   input stops. If no timer is available the cursor is simply not animated for
   the corresponding input sequence.

   Events:
   * JINPUT_VALIDATED when EXE is pressed during edition
   * JINPUT_CANCELED when EXIT is pressed during edition */
typedef struct {
	jwidget widget;

	/* Color and font of text */
	int color;
	font_t const *font;

	/* Optional prompt */
	char const *prompt;

	/* Text being input */
	char *text;
	/* Current size (including NUL) and maximum size */
	uint16_t size;
	uint16_t max;

	/* Current cursor position */
	int16_t cursor;
	/* Current input mode */
	uint8_t mode;
	/* Timer ID for the cursor state */
	int8_t timer;

	/* Custom keymap function (may be NULL) */
	jinput_keymap_function_t *keymap_fun;

} jinput;

/* Type IDs */
extern uint16_t JINPUT_VALIDATED;
extern uint16_t JINPUT_CANCELED;

/* jinput_create(): Create an input field

   The input field is disabled until it receives focus from its scene. The
   edited text is initially empty and is allocated when needed. The length
   specifies the maximum amount of bytes in the input. */
jinput *jinput_create(char const *prompt, size_t length, void *parent);

/* Trivial properties */
void jinput_set_text_color(jinput *input, int color);
void jinput_set_font(jinput *input, font_t const *font);
void jinput_set_prompt(jinput *input, char const *prompt);

/* Set a custom keymap function. The keymap function is called when a key is
   pressed that should produce input in the field. The following parameters are
   provided:
   * key is the code for the pressed key (<gint/keycodes.h>)
   * shift and alpha indicate the state of modifiers
   The function should return a Unicode code point. Note that jinput can deal
   with any Unicode code point but the font used for the jinput might not! */
void jinput_set_keymap_function(jinput *input, jinput_keymap_function_t *kf);

/* Current value visible in the widget, normally useful upon receiving the
   JINPUT_VALIDATED event, not guaranteed otherwise */
char const *jinput_value(jinput *input);

/* jinput_clear(): Clear current text */
void jinput_clear(jinput *input);

#endif /* _J_JINPUT */
