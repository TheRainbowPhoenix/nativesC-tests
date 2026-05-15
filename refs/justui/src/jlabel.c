#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jlabel.h>

#include <gint/display.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <gint/std/string.h>
#include <stdarg.h>

/* Type identifier for jlabel */
static int jlabel_type_id = -1;

jlabel *jlabel_create(char const *text, void *parent)
{
	if(jlabel_type_id < 0) return NULL;

	jlabel *l = malloc(sizeof *l);
	if(!l) return NULL;

	jwidget_init(&l->widget, jlabel_type_id, parent);

	l->block_halign = J_ALIGN_LEFT;
	l->block_valign = J_ALIGN_TOP;
	l->text_align   = J_ALIGN_LEFT;

	l->line_spacing = 0;
	l->color = C_BLACK;
	l->font = NULL;

	l->wrap_mode = J_WRAP_NONE;
	l->text = text;
	l->owns_text = false;
	l->wrapped_newline_spacing = false;

	vec_init(&l->breaks_vec, sizeof *l->breaks);

	return l;
}

/* Revert the label to an empty string. */
static void remove_text(jlabel *l)
{
	if(l->owns_text) free((char *)l->text);
	l->text = NULL;
	l->owns_text = false;
	vec_clear(&l->breaks_vec);
}

/* Add a breaking point at the specified string offset. */
static void add_break(jlabel *l, uint16_t br)
{
	if(!vec_add(&l->breaks_vec, 1)) return;
	l->breaks[l->breaks_vec.size - 1] = br;
}

static char const *word_boundary(char const *start, char const *cursor, bool
	look_ahead)
{
	char const *str = cursor;

	/* Look for a word boundary behind the cursor */
	while(1) {
		/* Current position is end-of-string: suitable */
		if(*str == 0) return str;
		/* Current position is start of string: bad */
		if(str <= start) break;

		/* Look for heteregoneous neighboring characters */
		int space_l = (str[-1] == ' ' || str[-1] == '\n');
		int space_r = (str[0]  == ' ' || str[0]  == '\n');

		if(!space_l && space_r) return str;
		str--;
	}

	/* If we can't look ahead, return the starting position to force a cut */
	if(!look_ahead) return cursor;
	str++;

	/* Otherwise, look ahead */
	while(*str) {
		int space_l = (str[-1] == ' ' || str[-1] == '\n');
		int space_r = (str[0]  == ' ' || str[0]  == '\n');

		if(!space_l && space_r) return str;
		str++;
	}

	/* If there's really nothing, return end-of-string */
	return str;
}

//---
// Text manipulation
//---

void jlabel_set_text(jlabel *l, char const *text)
{
	remove_text(l);
	l->text = text;
	l->owns_text = false;
	l->widget.dirty = 1;
}

int jlabel_asprintf(jlabel *l, char const *format, ...)
{
	remove_text(l);
	char *text = NULL;

	va_list args;
	va_start(args, format);
	int count = vasprintf(&text, format, args);
	va_end(args);

	l->text = text;
	l->owns_text = (text != NULL && count >= 0);
	l->widget.dirty = 1;
	return count;
}

int jlabel_snprintf(jlabel *l, size_t size, char const *format, ...)
{
	char *text = malloc(size + 1);
	if(!text) return -1;

	va_list args;
	va_start(args, format);
	int count = vsnprintf(text, size, format, args);
	va_end(args);

	l->text = text;
	l->owns_text = true;
	l->widget.dirty = 1;
	return count;
}

char const *jlabel_text(jlabel *l)
{
	return l->text;
}

//---
// Property setters
//---

void jlabel_set_block_alignment(jlabel *l, jalign horz, jalign vert)
{
	l->block_halign = horz;
	l->block_valign = vert;
	l->widget.dirty = 1;
}

void jlabel_set_text_alignment(jlabel *l, jalign align)
{
	l->text_align = align;
	l->widget.dirty = 1;
}

void jlabel_set_alignment(jlabel *l, jalign horz)
{
	l->block_halign = horz;
	l->text_align = horz;
	l->widget.dirty = 1;
}

void jlabel_set_line_spacing(jlabel *l, int line_spacing)
{
	l->line_spacing = line_spacing;
	l->widget.dirty = 1;
}

void jlabel_set_wrap_mode(jlabel *l, jwrapmode mode)
{
	l->wrap_mode = mode;
	l->widget.dirty = 1;
}

void jlabel_set_text_color(jlabel *l, int color)
{
	l->color = color;
	l->widget.update = 1;
}

void jlabel_set_font(jlabel *l, font_t const *font)
{
	/* A NULL font is acceptable here, dfont() will handle it later */
	l->font = font;
	l->widget.dirty = 1;
}

void jlabel_set_wrapped_newline_spacing(jlabel *l, bool preserve)
{
	l->wrapped_newline_spacing = preserve;
	l->widget.dirty = 1;
}

//---
// Polymorphic widget operations
//---

static void jlabel_poly_csize(void *l0)
{
	jlabel *l = l0;
	jwidget *w = &l->widget;
	w->w = 0;
	w->h = 0;

	font_t const *old_font = dfont(l->font);

	/* Cut at newline characters */
	char const *str = l->text;
	while(*str)
	{
		char const *end_of_line = strchrnul(str, '\n');
		int line_w, line_h;

		dnsize(str, end_of_line - str, NULL, &line_w, &line_h);
		w->w = max(w->w, line_w);
		w->h += (w->h > 0 ? l->line_spacing : 0) + line_h;

		str = end_of_line + (*end_of_line == '\n');
	}

	dfont(old_font);
}

static void jlabel_poly_layout(void *l0)
{
	jlabel *l = l0;
	vec_clear(&l->breaks_vec);

	int cw = jwidget_content_width(l);

	/* Configure the font for dnsize() below; we can't pass l->font directly as
	   NULL would default to the current font rather than gint's default */
	font_t const *old_font = dfont(l->font);

	/* Determine the end of each line, influenced by the wrap mode */
	char const *str = l->text;
	while(*str)
	{
		/* Start of line */
		add_break(l, str - l->text);

		/* A "\n" forces a newline in all wrap modes */
		char const *end_of_line = strchrnul(str, '\n');

		/* Also consider word or letters boundaries */
		if(l->wrap_mode != J_WRAP_NONE) {
			char const *end_of_widget = drsize(str, NULL, cw, NULL);

			if(end_of_widget < end_of_line) {
				/* In WRAP_LETTER mode, stop exactly at the limiting letter */
				if(l->wrap_mode == J_WRAP_LETTER)
					end_of_line = end_of_widget;
				/* In WRAP_WORD, try to find a word boundary behind; if this
				   fails, we fall back to end_of_line */
				else if(l->wrap_mode == J_WRAP_WORD)
					end_of_line = word_boundary(str, end_of_widget, false);
				/* In WRAP_WORD_ONLY, we want a word boundary, even if ahead */
				else if(l->wrap_mode == J_WRAP_WORD_ONLY)
					end_of_line = word_boundary(str, end_of_widget, true);
			}
		}

		bool natural_break = (*end_of_line == '\n');
		char const *next_start = end_of_line + (*end_of_line == '\n');

		if(!natural_break && !l->wrapped_newline_spacing) {
			/* Skip trailing spaces on this line */
			while(end_of_line > str && end_of_line[-1] == ' ')
				end_of_line--;
			/* Skip leading spaces on the next line */
			while(next_start[0] == ' ')
				next_start++;
		}

		add_break(l, end_of_line - l->text);

		/* Compute the length of this line (this is needed even if drsize() has
		   been used since spaces may have been removed) */
		int line_width;
		dnsize(str, end_of_line - str, NULL, &line_width, NULL);

		l->block_width = max(l->block_width, line_width);
		str = next_start;
	}

	dfont(old_font);
}

static void jlabel_poly_render(void *l0, int x, int y)
{
	jlabel *l = l0;
	font_t const *old_font = dfont(l->font);
	/* Set the font again; this returns l->font most of the time, except when
	   l->font is NULL, in which case it gives the default font's address*/
	font_t const *f = dfont(l->font);

	/* Available content width and height */
	int cw = jwidget_content_width(l);
	int ch = jwidget_content_height(l);

	/* Position the block vertically */

	int lines = l->breaks_vec.size / 2;
	int block_height = lines * (f->line_distance + l->line_spacing) -
		l->line_spacing;

	if(l->block_valign == J_ALIGN_MIDDLE)
		y += (ch - block_height) / 2;
	if(l->block_valign == J_ALIGN_BOTTOM)
		y += ch - block_height;

	/* Position the block horizontally */

	if(l->block_halign == J_ALIGN_CENTER)
		x += (cw - l->block_width) / 2;
	if(l->block_halign == J_ALIGN_RIGHT)
		x += cw - l->block_width;

	/* Render lines */

	for(int i = 0; i < lines; i++) {
		char const *str = l->text + l->breaks[2*i];
		int line_length = l->breaks[2*i+1] - l->breaks[2*i];

		/* Handle horizontal alignment */
		int dx = 0;
		if(l->text_align != J_ALIGN_LEFT) {
			int line_width;
			dnsize(str, line_length, NULL, &line_width, NULL);

			if(l->text_align == J_ALIGN_CENTER)
				dx = (l->block_width - line_width) / 2;
			if(l->text_align == J_ALIGN_RIGHT)
				dx = l->block_width - line_width;
		}

		dtext_opt(x + dx, y, l->color, C_NONE, DTEXT_LEFT, DTEXT_TOP,
			str, line_length);

		y += f->line_distance + l->line_spacing;
		str = l->text + l->breaks[i];
	}

	dfont(old_font);
}

static void jlabel_poly_destroy(void *l)
{
	remove_text(l);
}

/* jlabel type definition */
static jwidget_poly type_jlabel = {
	.name    = "jlabel",
	.csize   = jlabel_poly_csize,
	.layout  = jlabel_poly_layout,
	.render  = jlabel_poly_render,
	.event   = NULL,
	.destroy = jlabel_poly_destroy,
};

/* Type registration */
__attribute__((constructor))
static void j_register_jlabel(void)
{
	jlabel_type_id = j_register_widget(&type_jlabel);
}
