# jinput

JustUI.jinput: One-line input field

## Functions

### `*jinput_create`

Type IDs

```c
jinput *jinput_create(char const *prompt, size_t length, void *parent);
```

---

### `*jinput_create`

Create an input field The input field is disabled until it receives focus from its scene. The edited text is initially empty and is allocated when needed. The length specifies the maximum amount of bytes in the input.

```c
jinput *jinput_create(char const *prompt, size_t length, void *parent);
```

---

### `jinput_set_text_color`

Trivial properties

```c
void jinput_set_text_color(jinput *input, int color);
```

---

### `jinput_set_keymap_function`

Set a custom keymap function. The keymap function is called when a key is pressed that should produce input in the field. The following parameters are provided: * key is the code for the pressed key (<gint/keycodes.h>) * shift and alpha indicate the state of modifiers The function should return a Unicode code point. Note that jinput can deal with any Unicode code point but the font used for the jinput might not!

```c
void jinput_set_keymap_function(jinput *input, jinput_keymap_function_t *kf);
```

---

### `jinput_clear`

Current value visible in the widget, normally useful upon receiving the JINPUT_VALIDATED event, not guaranteed otherwise

```c
void jinput_clear(jinput *input);
```

---

### `jinput_clear`

Clear current text

```c
void jinput_clear(jinput *input);
```

---

## Data Structures

### `jinput`

Keymap function for jinput. See jinput_set_keymap_function(). */
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
   * JINPUT_CANCELED when EXIT is pressed during edition

**Fields**:

- `jwidget widget`

- `/* Color and font of text */
	int color`

- `font_t const *font`

- `/* Optional prompt */
	char const *prompt`

- `/* Text being input */
	char *text`

- `/* Current size (including NUL) and maximum size */
	uint16_t size`

- `uint16_t max`

- `/* Current cursor position */
	int16_t cursor`

- `/* Current input mode */
	uint8_t mode`

- `/* Timer ID for the cursor state */
	int8_t timer`

- `/* Custom keymap function (may be NULL) */
	jinput_keymap_function_t *keymap_fun`

```c
struct jinput {
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
};
```

---

## Implementation

Implementation is in the gint source tree.
