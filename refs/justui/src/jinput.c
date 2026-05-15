#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jinput.h>
#include "util.h"

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/config.h>
#include <stdlib.h>

/* Type identifier for jinput */
static int jinput_type_id = -1;

/* Events */
uint16_t JINPUT_VALIDATED;
uint16_t JINPUT_CANCELED;

/* Input modes. Valid combinations are:
   * (SHIFT | ALPHA)? | SIMUL
   * (SHIFT or SHIFT_LOCK)? | (ALPHA or ALPHA_LOCK)? */
enum {
	JINPUT_FLAT        = 0x00,
	JINPUT_SHIFT       = 0x01,
	JINPUT_ALPHA       = 0x02,
	JINPUT_SHIFT_LOCK  = 0x04,
	JINPUT_ALPHA_LOCK  = 0x08,
	JINPUT_SIMUL       = 0x10,
};

/* Mode indicators and their size */
extern bopti_image_t j_img_input_modes;
#if GINT_RENDER_MONO
# define JINPUT_INDICATOR 9
# define JINPUT_CURSOR_WIDTH 1
#elif GINT_RENDER_RGB
# define JINPUT_INDICATOR 12
# define JINPUT_CURSOR_WIDTH 2
#endif

jinput *jinput_create(char const *prompt, size_t length, void *parent)
{
	if(jinput_type_id < 0) return NULL;

	jinput *i = malloc(sizeof *i);
	if(!i) return NULL;

	jwidget_init(&i->widget, jinput_type_id, parent);
	jwidget_set_focus_policy(i, J_FOCUS_POLICY_ACCEPT);

	i->color = C_BLACK;
	jinput_set_font(i, NULL);
	i->prompt = (prompt ? prompt : "");

	i->text = malloc(length + 1);
	i->text[0] = 0;
	i->size = 1;
	i->max = length + 1;
	i->cursor = -1;

	i->mode = JINPUT_FLAT;
	i->timer = -1;
	i->keymap_fun = NULL;

	return i;
}

void jinput_set_text_color(jinput *i, int color)
{
	i->color = color;
	i->widget.update = 1;
}

void jinput_set_font(jinput *i, font_t const *font)
{
	if(!font) font = dfont_default();
	i->font = font;
	i->widget.dirty = 1;
}

void jinput_set_prompt(jinput *i, char const *prompt)
{
	i->prompt = prompt;
	i->widget.dirty = 1;
}

void jinput_set_keymap_function(jinput *i, jinput_keymap_function_t *kf)
{
	i->keymap_fun = kf;
}

//---
// Input helpers
//---

static void insert_str(jinput *i, char const *str, size_t n)
{
	if(i->size + n > i->max || i->cursor < 0) return;

	/* Insert at i->cursor, shift everything else right n places */
	for(int k = i->size - 1; k >= i->cursor; k--)
		i->text[k + n] = i->text[k];

	for(int k = 0; k < (int)n; k++)
		i->text[i->cursor + k] = str[k];

	i->size += n;
	i->cursor += n;
}

static void insert_code_point(jinput *i, uint32_t p)
{
	char str[4] = { 0x00, 0x80, 0x80, 0x80 };
	size_t size = 0;

	if(p <= 0x7f) {
		str[0] = p;
		size = 1;
	}
	else if(p <= 0x7ff) {
		str[0] = 0xc0 | (p >> 6);
		str[1] |= (p & 0x3f);
		size = 2;
	}
	else if(p <= 0xffff) {
		str[0] = 0xe0 | (p >> 12);
		str[1] |= ((p > 6) & 0x3f);
		str[2] |= (p & 0x3f);
		size = 3;
	}
	else {
		str[0] = 0xf0 | (p >> 18);
		str[1] |= ((p >> 12) & 0x3f);
		str[2] |= ((p >> 6) & 0x3f);
		str[3] |= (p & 0x3f);
		size = 4;
	}

	insert_str(i, str, size);
}

static int previous(char const *str, int position)
{
	if(position <= 0)
		return position;

	while((str[--position] & 0xc0) == 0x80) {}
	return position;
}

static int next(char const *str, int position)
{
	if(str[position] == 0) return position;

	while((str[++position] & 0xc0) == 0x80) {}
	return position;
}

static void delete(jinput *i)
{
	if(i->cursor < 0) return;
	int prev = previous(i->text, i->cursor);
	int diff = i->cursor - prev;
	if(!diff) return;

	/* Move everything from (i->cursor) to (prev) */
	for(int k = prev; k < i->size - diff; k++) {
		i->text[k] = i->text[k + diff];
	}

	i->size -= diff;
	i->cursor = prev;
}

static void clear(jinput *i)
{
	i->text[0] = 0;
	i->size = 1;
	i->cursor = 0;
}

char const *jinput_value(jinput *i)
{
	return i->text;
}

void jinput_clear(jinput *i)
{
	clear(i);
}

//---
// Polymorphic widget operations
//---

static void jinput_poly_csize(void *i0)
{
	jinput *i = i0;
	int w, h;

	dsize(i->prompt, i->font, &w, &h);

	i->widget.w = w + 16 + JINPUT_INDICATOR;
	i->widget.h = max(h, i->font->data_height);
	i->widget.h = max(i->widget.h, (int)j_img_input_modes.height);
}

static void jinput_poly_render(void *i0, int x, int y)
{
	jinput *i = i0;
	font_t const *old_font = dfont(i->font);
	int prompt_w, cursor_w, h;

	dsize(i->prompt, i->font, &prompt_w, &h);
	dtext(x, y, i->color, i->prompt);

	if(i->text) {
		dtext(x + prompt_w, y, i->color, i->text);
	}
	if(i->cursor >= 0) {
		if(i->text) dnsize(i->text, i->cursor, i->font, &cursor_w, NULL);
		else cursor_w = 0;

		int cursor_x = x + prompt_w + cursor_w;
		drect(cursor_x, y, cursor_x+JINPUT_CURSOR_WIDTH-1, y + h - 1, i->color);
	}

	dfont(old_font);

	/* Mode indicators. Only one indicator is needed most of the time, unless
	   one level is 1 and the other is 2. In this case, the temporary level (of
	   value 1) is displayed instead. This leaves only 8 cases to deal with. */
	if(i->mode == 0) return;
	int sl = (i->mode & JINPUT_SHIFT_LOCK) ? 2 : (i->mode & JINPUT_SHIFT) != 0;
	int al = (i->mode & JINPUT_ALPHA_LOCK) ? 2 : (i->mode & JINPUT_ALPHA) != 0;
	int mode = (sl + al == 3) ? 5 : ((sl | al) + (al ? (sl ? 4 : 2) : 0));

	x += jwidget_content_width(i) - JINPUT_INDICATOR;
	dsubimage(x, y, &j_img_input_modes, (JINPUT_INDICATOR + 1) * (mode - 1), 0,
		JINPUT_INDICATOR, j_img_input_modes.height, DIMAGE_NONE);
}

static bool jinput_poly_event(void *i0, jevent e)
{
	jinput *i = i0;

	if(e.type == JWIDGET_FOCUS_CHANGED) {
		i->cursor = jwidget_has_focus(i) ? (i->size - 1) : -1;
		i->mode = 0;
		i->widget.update = 1;
	}

	if(e.type == JWIDGET_KEY) {
		key_event_t ev = e.key;
		bool handled = true;

		/* Releasing modifiers */
		if(ev.type == KEYEV_UP && ev.key == KEY_SHIFT) {
			if(i->mode & JINPUT_SIMUL) i->mode &= ~JINPUT_SHIFT;
			i->widget.update = 1;
			return true;
		}
		if(ev.type == KEYEV_UP && ev.key == KEY_ALPHA) {
			if(i->mode & JINPUT_SIMUL) i->mode &= ~JINPUT_ALPHA;
			i->widget.update = 1;
			return true;
		}
		if(i->mode == JINPUT_SIMUL) i->mode = 0;

		if(ev.type != KEYEV_DOWN) return false;

		if(ev.key == KEY_EXE || ev.key == KEY_OK) {
			jevent e = { .source = i, .type = JINPUT_VALIDATED };
			jwidget_emit(i, e);
		}
		else if(ev.key == KEY_EXIT) {
			clear(i);
			jevent e = { .source = i, .type = JINPUT_CANCELED };
			jwidget_emit(i, e);
		}
		else if(ev.key == KEY_RIGHT) {
			i->cursor = next(i->text, i->cursor);
		}
		else if(ev.key == KEY_LEFT) {
			i->cursor = previous(i->text, i->cursor);
		}
		else if(ev.key == KEY_DEL) {
			delete(i);
		}
		else if(ev.key == KEY_ACON) {
			clear(i);
		}
		else if(ev.key == KEY_SHIFT) {
			if(i->mode & JINPUT_SHIFT_LOCK)
				i->mode ^= JINPUT_SHIFT_LOCK;
			else if(i->mode & JINPUT_SHIFT && !(i->mode & JINPUT_SIMUL))
				i->mode ^= JINPUT_SHIFT | JINPUT_SHIFT_LOCK;
			else
				i->mode |= JINPUT_SHIFT;
		}
		else if(ev.key == KEY_ALPHA) {
			if(i->mode & JINPUT_ALPHA_LOCK)
				i->mode ^= JINPUT_ALPHA_LOCK;
			else if(i->mode & JINPUT_ALPHA && !(i->mode & JINPUT_SIMUL))
				i->mode ^= JINPUT_ALPHA | JINPUT_ALPHA_LOCK;
			else
				i->mode |= JINPUT_ALPHA;
		}
		else {
			jinput_keymap_function_t *kf = i->keymap_fun;
			if(!kf)
				kf = &keymap_translate;
			uint32_t code_point = (*kf)(ev.key,
				(i->mode & JINPUT_SHIFT) || (i->mode & JINPUT_SHIFT_LOCK),
				(i->mode & JINPUT_ALPHA) || (i->mode & JINPUT_ALPHA_LOCK)
			);
			if(code_point) {
				insert_code_point(i, code_point);
				/* Mark modifiers as simultaneous if they're not released */
				if((keydown(KEY_SHIFT) && !(i->mode & JINPUT_SHIFT_LOCK)) ||
				   (keydown(KEY_ALPHA) && !(i->mode & JINPUT_ALPHA_LOCK)))
					i->mode |= JINPUT_SIMUL;
				/* Remove modifiers otherwise */
				else i->mode &= ~(JINPUT_SHIFT | JINPUT_ALPHA);
			}
			else handled = false;
		}

		i->widget.update = 1;
		if(handled)
			return true;
	}

	return jwidget_poly_event(i, e);
}

static void jinput_poly_destroy(void *i0)
{
	jinput *i = i0;
	free(i->text);
}

/* jinput type definition */
static jwidget_poly type_jinput = {
	.name   = "jinput",
	.csize   = jinput_poly_csize,
	.layout  = NULL,
	.render  = jinput_poly_render,
	.event   = jinput_poly_event,
	.destroy = jinput_poly_destroy,
};

__attribute__((constructor))
static void j_register_jinput(void)
{
	jinput_type_id = j_register_widget(&type_jinput);
	JINPUT_VALIDATED = j_register_event();
	JINPUT_CANCELED = j_register_event();
}
