#include <justui/jfkeys.h>
#include <justui/jwidget-api.h>
#include <gint/defs/attributes.h>
#include <gint/std/stdlib.h>
#include <gint/std/string.h>

J_DEFINE_WIDGET(jfkeys, csize, render, event)
J_DEFINE_EVENTS(JFKEYS_TRIGGERED)

extern font_t j_font_fkeys_fx;

#if GINT_RENDER_MONO
# define JFKEYS_HEIGHT 8
#elif GINT_RENDER_RGB
# define JFKEYS_HEIGHT 17
#endif

jfkeys *jfkeys_create2(
	bopti_image_t const *img, char const *labels, void *parent)
{
	if(jfkeys_type_id < 0) return NULL;

	jfkeys *f = malloc(sizeof *f);
	jwidget_init(&f->widget, jfkeys_type_id, parent);
	jfkeys_set2(f, img, labels);
	for(int i = 0; i < 6; i++) f->overrides[i] = NULL;
	f->bg_color = C_BLACK;
	f->bg_special_color = C_WHITE;
	f->text_color = C_WHITE;
	f->text_special_color = C_BLACK;

#if GINT_RENDER_MONO
	f->font = &j_font_fkeys_fx;
#else
	f->font = dfont_default();
#endif
	return f;
}

void jfkeys_set2(jfkeys *f, bopti_image_t const *img, char const *labels)
{
	f->img = img;
	f->labels = labels;
	f->level = 0;
	f->widget.update = true;
}

/* get_level(): Find the level inside a function definition */
static char const *get_level(char const *labels, int level)
{
	/* Navigate to level */
	while(level > 0) {
		labels = strchrnul(labels, '|');
		labels += (*labels == '|');
		level--;
	}
	return (*labels == 0) ? NULL : labels;
}

/* get_label(): Find a key within a level */
static char const *get_label(char const *level, int key, size_t *len)
{
	int current_key = 0;

	while(current_key <= key) {
		/* We reached the end of the level without finding the key */
		if(*level == 0 || *level == '|') return NULL;
		/* Next entry */
		if(*level == ';') {
			current_key++;
			level++;
			continue;
		}
		/* Found contents */
		if(current_key == key) {
			*len = 0;
			while(level[*len] != ';' && level[*len] != '|' && level[*len])
				(*len)++;
			return level;
		}

		level++;
	}

	return NULL;
}

//---
// Polymorphic widget operations
//---

void jfkeys_poly_csize(void *f0)
{
	jfkeys *f = f0;
	f->widget.w = DWIDTH;
	f->widget.h = JFKEYS_HEIGHT;
}

void jfkeys_poly_render(void *f0, int base_x, int y)
{
	jfkeys *f = f0;

	if(f->img) {
		dsubimage(base_x, y, f->img, 0, (JFKEYS_HEIGHT+1) * f->level,
			f->img->width, JFKEYS_HEIGHT, DIMAGE_NONE);
		return;
	}

	font_t const *old_font = dfont(f->font);
	char const *level = get_level(f->labels, f->level);
	if(!level) return;

	for(int position = 0; position < 6; position++) {
		size_t length = 0;
		char const *text = f->overrides[position];
		if(!text) text = get_label(level, position, &length);
		if(!text || (*text != '.' && *text != '/' && *text != '@'
			&& *text != '#')) continue;

		int fw = jwidget_full_width(f);
		int margin = (fw >= 250 ? 4 : 2);
		int spacing = 2;
		int w = (fw - 2 * margin - 5 * spacing) / 6;
		int x = base_x + margin + (w + spacing) * position;
		int color = (text[0] == '#') ? f->text_special_color : f->text_color;

		if(text[0] == '.') {
			drect(x, y, x + w - 1, y + 14, f->bg_color);
		}
		if(text[0] == '@') {
			dline(x + 1, y, x + w - 2, y, f->bg_color);
			dline(x + 1, y + 14, x + w - 2, y + 14, f->bg_color);
			drect(x, y + 1, x + w - 1, y + 13, f->bg_color);
		}
		if(text[0] == '/') {
			dline(x + 1, y, x + w - 2, y, f->bg_color);
			drect(x, y + 1, x + w - 1, y + 9, f->bg_color);
			dline(x, y + 10, x + w - 2, y + 10, f->bg_color);
			dline(x, y + 11, x + w - 3, y + 11, f->bg_color);
			dline(x, y + 12, x + w - 4, y + 12, f->bg_color);
			dline(x, y + 13, x + w - 5, y + 13, f->bg_color);
			dline(x, y + 14, x + w - 6, y + 14, f->bg_color);
		}
		if(text[0] == '#') {
			dline(x + 1, y, x + w - 2, y, f->bg_color);
			dline(x + 1, y + 14, x + w - 2, y + 14, f->bg_color);
			drect(x, y + 1, x + 1, y + 13, f->bg_color);
			drect(x + w - 2, y + 1, x + w - 1, y + 13, f->bg_color);
			drect(x + 2, y + 1, x + w - 3, y + 13, f->bg_special_color);
		}

		dtext_opt(x + (w >> 1), y + 3, color, C_NONE, DTEXT_CENTER, DTEXT_TOP,
			text + 1, length - 1);
	}

	dfont(old_font);
}

bool jfkeys_poly_event(void *f0, jevent e)
{
	jfkeys *f = f0;
	jevent te;
	te.source = f;
	te.type = JFKEYS_TRIGGERED;

	if(e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
		int fun = keycode_function(e.key.key);
		if(fun >= 0) {
			te.data = fun - 1;
			jwidget_emit(f, te);
			return true;
		}

#if GINT_HW_CP
		static uint8_t const CP_Fk[6] = {
			KEY_EQUALS, KEY_X, KEY_Y, KEY_Z, KEY_CARET, KEY_DIV };

		for(int i = 0; i < 6; i++) {
			if(e.key.key == CP_Fk[i]) {
				te.data = i;
				jwidget_emit(f, te);
				return true;
			}
		}
#endif
	}

	return jwidget_poly_event(f, e);
}

int jfkeys_level(jfkeys *f)
{
	return f->level;
}

void jfkeys_set_level(jfkeys *f, int level)
{
	if(f->level == level) return;
	f->level = level;
	f->widget.update = 1;
}

char const *jfkeys_override(jfkeys *keys, int key)
{
	if(key < 1 || key > 6) return NULL;
	return keys->overrides[key - 1];
}

void jfkeys_set_override(jfkeys *keys, int key, char const *override)
{
	if(key < 1 || key > 6) return;
	keys->overrides[key - 1] = override;
}

void jfkeys_set_color(
	jfkeys *keys, int bg, int bg_special, int text, int text_special)
{
	keys->bg_color = bg;
	keys->bg_special_color = bg_special;
	keys->text_color = text;
	keys->text_special_color = text_special;
}

void jfkeys_set_font(jfkeys *keys, font_t const *font)
{
	keys->font = font;
}
