#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include <justui/jfileselect.h>
#include <justui/config.h>

#include <gint/display.h>
#include <gint/gint.h>
#include <gint/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* Type identifier for jfileselect */
static int jfileselect_type_id = -1;

#if GINT_RENDER_MONO
# define JFILESELECT_LINE_SPACING 1
# define JFILESELECT_SCROLLBAR_WIDTH 1
# define JFILESELECT_INFO_SHORT 1
#elif GINT_RENDER_RGB
# define JFILESELECT_LINE_SPACING (J_CONFIG_TOUCH ? 6 : 4)
# define JFILESELECT_SCROLLBAR_WIDTH 2
# define JFILESELECT_INFO_SHORT 0
#endif

/* Events */
uint16_t JFILESELECT_LOADED;
uint16_t JFILESELECT_VALIDATED;
uint16_t JFILESELECT_CANCELED;

/* We can try pretty hard to not duplicate the information held by the
   directory descriptor, which is already a full array of all entries on gint.
   However, the standard API behind readdir(3) does not allow us any complex
   operations; no filtering, no sorting, no accessing auxiliary data such as
   the file size. So we do this manually. */
struct fileinfo {
	/* Entry name (owned by the structure) */
	char *name;
	/* File size in bytes if file, number of entries if folder */
	int size :24;
	/* Type from [struct dirent], -1 for the "Save As" entry */
	int8_t type;
};

jfileselect *jfileselect_create(void *parent)
{
	if(jfileselect_type_id < 0) return NULL;

	jfileselect *fs = malloc(sizeof *fs);
	if(!fs) return NULL;

	jwidget_init(&fs->widget, jfileselect_type_id, parent);
	jwidget_set_focus_policy(fs, J_FOCUS_POLICY_SCOPE);

	jinput *input = jinput_create("Filename: ", 32, fs);
	if(!input) {
		free(fs);
		return NULL;
	}
	jwidget_set_floating(input, true);
	jwidget_set_visible(input, false);

	fs->path = NULL;
	fs->entries = NULL;
	fs->entry_count = 0;
	fs->folder_error = 0;

	fs->selected_file = NULL;
	fs->saveas_input = input;
	fs->saveas = false;
	fs->input_mode = false;
	fs->scrollbar_width = JFILESELECT_SCROLLBAR_WIDTH;
	fs->filter_function = jfileselect_default_filter;

	fs->cursor = -1;
	fs->touch_cursor = -1;
	fs->scroll = 0;
	fs->visible_lines = 0;

	fs->line_spacing = JFILESELECT_LINE_SPACING;
	fs->font = dfont_default();
	fs->show_file_size = false;

	return fs;
}

static void count_visible_lines(jfileselect *fs)
{
	int ch = jwidget_content_height(fs);
	int line_height = fs->font->line_height + fs->line_spacing;
	fs->visible_lines = ch / line_height;
}

static void set_finfo(jfileselect *fs, struct fileinfo *finfo, int n)
{
	struct fileinfo *old = fs->entries;
	if(old) {
		for(int i = 0; i < fs->entry_count; i++)
			free(old[i].name);
		free(old);
	}
	fs->entries = finfo;
	fs->entry_count = n;
}

void jfileselect_set_saveas(jfileselect *fs, bool save_as)
{
	fs->saveas = save_as;
}

//---
// Input utilities
//---

static void start_input(jfileselect *fs)
{
	fs->input_mode = true;
	jinput_clear(fs->saveas_input);
	jwidget_scope_set_target(fs, fs->saveas_input);
	fs->widget.update = true;
}

static void stop_input(jfileselect *fs)
{
	fs->input_mode = false;
	jwidget_scope_set_target(fs, NULL);
	fs->widget.update = true;
}

//---
// Getters and setters
//---

void jfileselect_set_font(jfileselect *fs, font_t const *font)
{
	fs->font = font ? font : dfont_default();
	count_visible_lines(fs);
}

void jfileselect_set_line_spacing(jfileselect *fs, int line_spacing)
{
	fs->line_spacing = line_spacing;
	count_visible_lines(fs);
}

void jfileselect_set_scrollbar_width(jfileselect *fs, int scrollbar_width)
{
	fs->scrollbar_width = scrollbar_width;
}

void jfileselect_set_show_file_size(jfileselect *fs, bool show_file_size)
{
	fs->show_file_size = show_file_size;
	fs->widget.update = true;
}

//---
// Path and folder manipulation
//---

static char *path_down(char const *path, char const *name)
{
	char *child = malloc(strlen(path) + strlen(name) + 2);
	if(!child)
		return NULL;

	strcpy(child, path);
	if(strcmp(path, "/") != 0)
		strcat(child, "/");
	strcat(child, name);

	return child;
}

static char *path_up(char const *path)
{
	char *parent = strdup(path);
	if(!parent)
		return NULL;

	char *p = strrchr(parent, '/');
	if(p == parent)
		*(p+1) = 0;
	else if(p)
		*p = 0;

	return parent;
}

static int count_accepted_entries(jfileselect *fs, DIR *dp)
{
	int n = 0;
	struct dirent *ent;

	rewinddir(dp);
	while((ent = readdir(dp)))
		n += (fs->filter_function ? fs->filter_function(ent) : 1);

	return n;
}

static int compare_entries(void const *i1_0, void const *i2_0)
{
	struct fileinfo const *i1 = i1_0, *i2 = i2_0;

	/* Group directories first */
	int d1 = (i1->type == DT_DIR);
	int d2 = (i2->type == DT_DIR);
	if(d1 != d2)
		return d2 - d1;

	/* Then the "Save As" entry */
	int sa1 = (i1->type == -1);
	int sa2 = (i2->type == -1);
	if(sa1 != sa2)
		return sa2 - sa1;

	/* Then group by name */
	return strcmp(i1->name, i2->name);
}

static bool load_folder_switch(jfileselect *fs, char *path)
{
	set_finfo(fs, NULL, 0);
	free(fs->path);
	fs->path = path;

	struct dirent *ent;
	DIR *dp = opendir(path);
	if(!dp) {
		fs->folder_error = errno;
		return false;
	}

	/* Count entries */
	int n = count_accepted_entries(fs, dp) + fs->saveas;

	/* Allocate memory for the fileinfo structures */
	struct fileinfo *finfo = malloc(n * sizeof *finfo);
	if(n && !finfo) {
		closedir(dp);
		fs->folder_error = errno;
		return false;
	}

	/* Read the fileinfo structures */
	rewinddir(dp);
	for(int i = 0; i < n && (ent = readdir(dp));) {
		if(fs->filter_function && !fs->filter_function(ent))
			continue;

		finfo[i].name = strdup(ent->d_name);
		finfo[i].type = ent->d_type;
		finfo[i].size = -1;

		if(!finfo[i].name) {
			/* Profesionnal unwinding isn't it? */
			for(int j = 0; j < i; j++) free(finfo[j].name);
			free(finfo);
			closedir(dp);
			fs->folder_error = errno;
			return false;
		}

		char *full_path = path_down(path, ent->d_name);
		if(full_path) {
			if(ent->d_type == DT_DIR) {
				DIR *sub = opendir(full_path);
				if(sub) {
					finfo[i].size = count_accepted_entries(fs, sub);
					closedir(sub);
				}
				else {
					finfo[i].size = -errno;
				}
			}
			else {
				struct stat st;
				if(stat(full_path, &st) >= 0)
					finfo[i].size = st.st_size;
			}
		}

		i++;
	}

	/* Add the saveas entry */
	if(fs->saveas) {
		finfo[n-1].name = strdup("<Create a new file here>");
		finfo[n-1].type = -1;
		finfo[n-1].size = -1;
	}

	qsort(finfo, n, sizeof *finfo, compare_entries);

	closedir(dp);
	set_finfo(fs, finfo, n);
	fs->folder_error = 0;
	return true;
}

static bool load_folder(jfileselect *fs, char *path)
{
	bool ok =
		gint_world_switch(GINT_CALL(load_folder_switch, (void *)fs, path));

	fs->widget.update = true;
	jwidget_emit(fs, (jevent){ .type = JFILESELECT_LOADED });

	return ok;
}

bool jfileselect_browse(jfileselect *fs, char const *path)
{
	char *path_copy = strdup(path);
	if(!path_copy)
		return false;

	bool ok = load_folder(fs, path_copy);

	free(fs->selected_file);
	fs->selected_file = NULL;

	fs->cursor = 0;
	fs->touch_cursor = -1;
	fs->scroll = 0;
	stop_input(fs);
	return ok;
}

char const *jfileselect_selected_file(jfileselect *fs)
{
	return fs->selected_file;
}

char const *jfileselect_current_folder(jfileselect *fs)
{
	return fs->path;
}

void jfileselect_set_filter(jfileselect *fs,
   bool (*filter)(struct dirent const *entry))
{
	fs->filter_function = filter;
}

bool jfileselect_default_filter(struct dirent const *ent)
{
	if(!strcmp(ent->d_name, "@MainMem"))
		return false;
	if(!strcmp(ent->d_name, "SAVE-F"))
		return false;
	if(!strcmp(ent->d_name, "AutoImport"))
		return false;
	if(!strcmp(ent->d_name, "."))
		return false;
	if(!strcmp(ent->d_name, ".."))
		return false;
	return true;
}

GUNUSED static void jfileselect_select(jfileselect *fs, int index)
{
	/* Normalize out-of-bounds to -1 */
	if(index < 0 || index >= fs->entry_count)
		index = -1;
	if(fs->cursor == index)
		return;

	fs->cursor = index;
	fs->widget.update = 1;
}

//---
// Polymorphic widget operations
//---

static void jfileselect_poly_csize(void *fs0)
{
	jfileselect *fs = fs0;
	jwidget *w = &fs->widget;

	w->w = 128;
	w->h = 3 * max(fs->font->line_height + fs->line_spacing, 0);
}

static void jfileselect_poly_layout(void *fs0)
{
	jfileselect *fs = fs0;
	count_visible_lines(fs);
	fs->saveas_input->widget.w = jwidget_content_width(fs) - 4;
}

static void generate_info_string(char *str, bool isfolder, int size)
{
#if JFILESELECT_INFO_SHORT
	if(size < 0)
		sprintf(str, "E%d", -size);
	else if(isfolder)
		sprintf(str, "%d/", size);
	else if(size < 10000) /* 10 kB */
		sprintf(str, "%d", size);
	else
		sprintf(str, "%dk", size / 1000);
#else
	if(size < 0)
		sprintf(str, "E%d", -size);
	else if(isfolder)
		sprintf(str, "%d entries", size);
	else
		sprintf(str, "%d B", size);
#endif
}

static void jfileselect_poly_render(void *fs0, int x, int y)
{
	jfileselect *fs = fs0;

	font_t const *old_font = dfont(fs->font);
	int line_height = fs->font->line_height + fs->line_spacing;
	int cw = jwidget_content_width(fs) - 2 * fs->scrollbar_width;
	int ch = jwidget_content_height(fs);
	struct fileinfo *finfo = fs->entries;

	bool scrollbar =
		fs->entry_count > fs->visible_lines
		&& fs->scrollbar_width > 0;
	int entry_width = cw - (scrollbar ? 2 * fs->scrollbar_width : 0);

	if(fs->folder_error) {
		int text_y = y + (fs->line_spacing + 0) / 2;
		dprint(x, text_y, C_BLACK, "(E%d)", fs->folder_error);
		return;
	}
	if(!fs->entries || fs->entry_count == 0) {
		int text_y = y + (fs->line_spacing + 0) / 2;
		dprint(x, text_y, C_BLACK, "(No entries)");
		return;
	}

	for(int i = 0; i < fs->visible_lines && i < fs->entry_count; i++) {
		bool selected = (fs->cursor == fs->scroll + i);
		struct fileinfo *info = &finfo[fs->scroll + i];
		bool isfolder = (info->type == DT_DIR);

		/* Round `line_spacing / 2` down so there is more spacing below */
		int line_y = y + line_height * i;
		int text_y = line_y + (fs->line_spacing + 0) / 2;
		int fg = selected ? C_WHITE : C_BLACK;

		if(selected && fs->input_mode) {
			/* Little bit of a hack */
			fs->saveas_input->widget.visible = true;
			jwidget_render(fs->saveas_input, x+2, text_y);
			fs->saveas_input->widget.visible = false;
			continue;
		}

		if(selected) {
			drect(x, line_y, x + entry_width - 1, line_y + line_height - 1,
				C_BLACK);
		}

		dprint(x+2, text_y, fg, "%s%s", info->name, isfolder ? "/" : "");
		if(fs->show_file_size) {
			char str[32];
			generate_info_string(str, isfolder, info->size);
			dtext_opt(x + entry_width - 3, text_y, fg, C_NONE, DTEXT_RIGHT,
				DTEXT_TOP, str);
		}
	}

	if(scrollbar) {
		int sb_y = ch * fs->scroll / fs->entry_count;
		int sb_h = ch * fs->visible_lines / fs->entry_count;

		drect(x + cw - fs->scrollbar_width, y + sb_y,
			  x + cw - 1, y + sb_y + sb_h - 1,
			  C_BLACK);
	}

	dfont(old_font);
}

static bool trigger_entry(jfileselect *fs)
{
	struct fileinfo *finfo = fs->entries;
	struct fileinfo *i = &finfo[fs->cursor];

	if(i->type == DT_DIR) {
		char *child = path_down(fs->path, i->name);
		if(child) {
			load_folder(fs, child);
			fs->cursor = 0;
			fs->touch_cursor = -1;
			fs->scroll = 0;
			return true;
		}
	}
	else if(fs->saveas && i->type == -1) {
		start_input(fs);
		return true;
	}
	else {
		fs->selected_file = path_down(fs->path, i->name);
		if(fs->selected_file) {
			jwidget_emit(fs, (jevent){ .type = JFILESELECT_VALIDATED });
			return true;
		}
	}
	return false;
}

static bool jfileselect_poly_event(void *fs0, jevent e)
{
	jfileselect *fs = fs0;
	if(!fs->path)
		return false;

	if(e.type == JINPUT_CANCELED && e.source == fs->saveas_input) {
		stop_input(fs);
		return true;
	}
	else if(e.type == JINPUT_VALIDATED && e.source == fs->saveas_input) {
		stop_input(fs);
		fs->selected_file = path_down(fs->path,jinput_value(fs->saveas_input));
		if(fs->selected_file) {
			jwidget_emit(fs,(jevent){ .type = JFILESELECT_VALIDATED });
			return true;
		}
		else return false;
	}
	/* Send all events to the input when in input mode (without requiring
	   access to the jscene to actually move the focus */
	else if(fs->input_mode) {
		bool b = jwidget_event(fs->saveas_input, e);
		if(b)
			fs->widget.update = true;
		/* We do capture all key events if not used by the input, so F-keys are
		   disabled/etc */
		return b || e.type == JWIDGET_KEY;
	}

	if(e.type == JWIDGET_KEY) {
		key_event_t ev = e.key;

#if J_CONFIG_TOUCH
		bool accept_touch = !fs->folder_error && fs->entries && fs->entry_count;
		if((ev.type == KEYEV_TOUCH_DOWN || ev.type == KEYEV_TOUCH_DRAG ||
		    ev.type == KEYEV_TOUCH_UP) && accept_touch) {
			int lx = ev.x - jwidget_absolute_content_x(fs);
			int ly = ev.y - jwidget_absolute_content_y(fs);
			uint w = jwidget_content_width(fs);
			uint h = jwidget_content_height(fs);

			int index = -1;
			if((uint)lx < w && (uint)ly < h) {
				int line_height = fs->font->line_height + fs->line_spacing;
				index = (ly / line_height) + fs->scroll;
				if((uint)index >= (uint)fs->entry_count)
					index = -1;
			}

			if(ev.type == KEYEV_TOUCH_DOWN && index >= 0) {
				jfileselect_select(fs, index);
				fs->touch_cursor = index;
			}
			if(ev.type == KEYEV_TOUCH_DRAG && index >= 0)
				jfileselect_select(fs, index);
			if(ev.type == KEYEV_TOUCH_UP && index >= 0 &&
			   index == fs->touch_cursor)
				trigger_entry(fs);
			return true;
		}
#endif

		if(ev.type != KEYEV_DOWN && ev.type != KEYEV_HOLD)
			return false;
		int key = ev.key;

		bool moved = false;

		if(key == KEY_UP && fs->cursor > 0) {
			fs->cursor = ev.shift ? 0 : fs->cursor - 1;
			moved = true;
		}
		if(key == KEY_DOWN && fs->cursor < fs->entry_count - 1) {
			fs->cursor = ev.shift ? fs->entry_count - 1 : fs->cursor + 1;
			moved = true;
		}

		if(fs->scroll > 0 && fs->cursor <= fs->scroll)
			fs->scroll = max(fs->cursor - 1, 0);
		if(fs->scroll + fs->visible_lines < fs->entry_count
				&& fs->cursor >= fs->scroll + fs->visible_lines - 2) {
			fs->scroll = min(fs->cursor - fs->visible_lines + 2,
							 fs->entry_count - fs->visible_lines);
		}

		if(moved) {
			fs->widget.update = true;
			return true;
		}

		if(key == KEY_EXIT) {
			if(!strcmp(fs->path, "/")) {
				jwidget_emit(fs, (jevent){ .type = JFILESELECT_CANCELED });
				return true;
			}
			char *parent = path_up(fs->path);
			if(parent) {
				load_folder(fs, parent);
				fs->cursor = 0;
				fs->touch_cursor = -1;
				fs->scroll = 0;
				return true;
			}
		}
		else if((key == KEY_EXE || key == KEY_OK) && fs->entries) {
			if(trigger_entry(fs))
				return true;
		}
	}

	return jwidget_poly_event(fs, e);
}

static void jfileselect_poly_destroy(void *fs0)
{
	jfileselect *fs = fs0;

	free(fs->path);
	set_finfo(fs, NULL, 0);
	free(fs->selected_file);
}

/* jfileselect type definition */
static jwidget_poly type_jfileselect = {
	.name    = "jfileselect",
	.csize   = jfileselect_poly_csize,
	.layout  = jfileselect_poly_layout,
	.render  = jfileselect_poly_render,
	.event   = jfileselect_poly_event,
	.destroy = jfileselect_poly_destroy,
};

__attribute__((constructor))
static void j_register_jfileselect(void)
{
	jfileselect_type_id = j_register_widget(&type_jfileselect);
	JFILESELECT_LOADED = j_register_event();
	JFILESELECT_VALIDATED = j_register_event();
	JFILESELECT_CANCELED = j_register_event();
}
