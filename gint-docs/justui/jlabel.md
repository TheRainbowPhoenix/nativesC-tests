# jlabel

JustUI.jlabel: Simple one-line or multi-line text label without formatting

## Functions

### `*jlabel_create`

Create a label with a fixed text Initially the label has the supplied string as text (which must outlive the widget). If you want dynamic text, you can provide an empty string and use jlabel_printf() later.

```c
jlabel *jlabel_create(char const *text, void *parent);
```

---

### `jlabel_set_text`

Set a fixed string This function sets the label text to a fixed string. This is the same as in jlabel_create(). The string is not copied, it must outlive the label. This is the most useful when the provided string is a literal.

```c
void jlabel_set_text(jlabel *l, char const *text);
```

---

### `jlabel_asprintf`

Generate and set a formatted string This function generates the label string with printf-formatting. The resulting string is created with malloc() and owned by the label; it is destroyed when the text is replaced of the label is destroyed. Because of how asprintf() works, the string is generated twice. Returns the number of characters printed.

```c
int jlabel_asprintf(jlabel *l, char const *format, ...);
```

---

### `jlabel_snprintf`

Generate and set a formatted string Similar to jlabel_asprintf(), but an upper bound on the length of the result string has to be provided. This avoids generating the string twice. Return the number of characters printed.

```c
int jlabel_snprintf(jlabel *l, size_t size, char const *format, ...);
```

---

### `jlabel_set_block_alignment`

Get the current string

```c
void jlabel_set_block_alignment(jlabel *l, jalign horz, jalign vert);
```

---

### `jlabel_set_block_alignment`

Set block and text alignment, individually

```c
void jlabel_set_block_alignment(jlabel *l, jalign horz, jalign vert);
```

---

### `jlabel_set_alignment`

Set both horizontal alignments at the same time (most natural)

```c
void jlabel_set_alignment(jlabel *l, jalign horizontal_align);
```

---

### `jlabel_set_line_spacing`

Trivial properties

```c
void jlabel_set_line_spacing(jlabel *l, int line_spacing);
```

---

## Data Structures

### `jlabel`

jlabel_wrapmode: Text wrapping options */
typedef enum jwrapmode {
	/* Wrap only at \n characters */
	J_WRAP_NONE,
	/* Break only at spaces; if a word is longer than a full line, breaking in
	   that word is allowed */
	J_WRAP_WORD,
	/* Break only at spaces and tabs, even if a word is longer than a line */
	J_WRAP_WORD_ONLY,
	/* Break at any letter */
	J_WRAP_LETTER,

} __attribute__((packed)) jwrapmode;

/* jlabel: One-line or multi-line piece of text, without formatting.

   This widget is used for mundane text printing. All the text in the label is
   printed with a single font. The text can be aligned horizontally and
   vertically within the widget ("block alignment"), and the lines can also be
   aligned horizontally within the text block ("text alignment").

     +-------------+
     |    label    |    Text is centered within the widget (block centered).
     |      1      |    Lines are aligned by their center (text centered).
     +-------------+

     +-------------+
     |    label    |    Text is centered within the widget (block centered).
     |    2        |    Lines are aligned by their left side (text left).
     +-------------+

     +-------------+
     |label        |    Text is aligned left within the widget (block left).
     |  3          |    Lines are aligned by their center (text centered).
     +-------------+

   Lines are broken at '\n' characters. Depending on the word wrap settings,
   lines can also be broken whenever the edge of the widget is reached. Extra
   line spacing (even negative) can be specified.

   The natural size of the widget is always computed based on newline
   characters, since there is no way to wrap until a width has been assigned.
   If you want predictable results, use size constraints.

   The text color and font can also be set, using the same color values and
   fonts as <gint/display.h>. If the font is set to NULL, gint's default font
   is used. The background color for the whole widget is handled by jwidget.

**Fields**:

- `jwidget widget`

- `/* Horizontal block alignment */
	jalign block_halign`

- `/* Vertical block alignment */
	jalign block_valign`

- `/* Text alignment */
	jalign text_align`

- `/* Pixels of spacing between lines, in addition to font->line_distance */
	int8_t line_spacing`

- `/* Text to display */
	char const *text`

- `/* List of line break offsets`

- `indexes 2n and 2n+1 give the start and end
	   of line number n */
  DECLARE_VEC(uint16_t, breaks)`

- `/* Block width (maximum length of a rendered line) */
	uint16_t block_width`

- `/* Text wrapping mode */
	enum jwrapmode wrap_mode`

- `/* Whether the text has been allocated by the label or supplied by user */
	bool owns_text                :1`

- `/* Whether to preserve spaces around line-wrapping newlines */
	bool wrapped_newline_spacing  :1`

- `uint                          :5`

- `/* Color and font of text`

- `if NULL, gint's default font is used */
	int color`

- `font_t const *font`

```c
struct jlabel {
jwidget widget;

	/* Horizontal block alignment */
	jalign block_halign;
	/* Vertical block alignment */
	jalign block_valign;
	/* Text alignment */
	jalign text_align;
	/* Pixels of spacing between lines, in addition to font->line_distance */
	int8_t line_spacing;

	/* Text to display */
	char const *text;
	/* List of line break offsets; indexes 2n and 2n+1 give the start and end
	   of line number n */
  DECLARE_VEC(uint16_t, breaks);

	/* Block width (maximum length of a rendered line) */
	uint16_t block_width;
	/* Text wrapping mode */
	enum jwrapmode wrap_mode;
	/* Whether the text has been allocated by the label or supplied by user */
	bool owns_text                :1;
	/* Whether to preserve spaces around line-wrapping newlines */
	bool wrapped_newline_spacing  :1;

	uint                          :5;

	/* Color and font of text; if NULL, gint's default font is used */
	int color;
	font_t const *font;
};
```

---

## Implementation

Implementation is in the gint source tree.
