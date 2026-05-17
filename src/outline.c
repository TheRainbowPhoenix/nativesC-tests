#include "outline.h"
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jscrolledlist.h>
#include <gint/display.h>
#include <gint/bfile.h>
#include <gint/gint.h>
#include <string.h>

typedef struct {
    char name[64];
    int line;
} outline_item_t;

static outline_item_t items[100];
static int num_items = 0;

static void info_fn(jlist* l, int i, jlist_item_info* info) { (void)l; (void)i; info->selectable = info->triggerable = true; info->natural_height = 20; }
static void paint_fn(int x, int y, int w, int h, jlist* l, int i, bool sel) { (void)w; (void)h; (void)l; if (i >= 0 && i < num_items) dtext(x + 5, y + 2, sel ? C_WHITE : C_BLACK, items[i].name); }

void nc_to_os_path(const char* src, uint16_t* dest, int max_len) {
    int i = 0;
    while (src[i] && i < max_len - 1) {
        dest[i] = (uint8_t)src[i];
        i++;
    }
    dest[i] = 0;
}

int outline_show(const char* filename, nc_theme_name_t theme_name) {
    nc_theme_t const* t = nc_get_theme(theme_name);
    jscene* scene = (jscene*)jscene_create_fullscreen(NULL);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t->modal_bg);

    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header); jwidget_set_fixed_height(header, 40); jwidget_set_background(header, t->accent);
    jlabel_set_text_color(jlabel_create("Outline", header), t->txt_acc);

    jscrolledlist* sl = (jscrolledlist*)jscrolledlist_create(info_fn, paint_fn, (jwidget*)scene);
    jwidget_set_stretch((jwidget*)sl, 1, 1, false);

    bool running = true, analyzing = true;
    int result = -1, line_idx = 0, current_pos = 0;

    uint16_t os_path[128];
    nc_to_os_path(filename, os_path, 128);
    gint_world_switch();
    int fd = BFile_Open(os_path, BFile_ReadOnly);
    gint_world_switch();

    num_items = 0;

    char read_buf[512];
    int buf_pos = 0;
    int buf_len = 0;

    while (running) {
        if (analyzing && fd >= 0) {
            for (int k = 0; k < 20 && num_items < 100; k++) {
                char line[128];
                int line_pos = 0;
                bool found_newline = false;

                while (line_pos < 127) {
                    if (buf_pos >= buf_len) {
                        gint_world_switch();
                        buf_len = BFile_Read(fd, read_buf, sizeof(read_buf), current_pos);
                        gint_world_switch();
                        if (buf_len <= 0) break;
                        buf_pos = 0;
                    }
                    char c = read_buf[buf_pos++];
                    current_pos++;
                    if (c == '\n') { found_newline = true; break; }
                    if (c != '\r') line[line_pos++] = c;
                }
                line[line_pos] = '\0';

                if (line_pos > 0 || found_newline) {
                    char* p = line; while (*p == ' ' || *p == '\t') p++;
                    if (strncmp(p, "def ", 4) == 0 || strncmp(p, "class ", 6) == 0) {
                        strncpy(items[num_items].name, p, 63); items[num_items].name[63] = '\0';
                        items[num_items].line = line_idx; num_items++;
                        jlist_update_model(sl->list, num_items, NULL);
                    }
                    line_idx++;
                } else if (buf_len <= 0) {
                    analyzing = false;
                    break;
                }
            }
            if (num_items >= 100) analyzing = false;
        }

        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t->modal_bg);
            jscene_render(scene);
            if (analyzing) dtext(160, 220, C_RED, "Analyzing...");
            dupdate();
        } else if (e.type == JLIST_ITEM_TRIGGERED) {
            int idx = jlist_selected_item(sl->list);
            if (idx >= 0 && idx < num_items) { result = items[idx].line; running = false; }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN && e.key.key == KEY_EXIT) {
            running = false;
        }
    }

    if (fd >= 0) {
        gint_world_switch();
        BFile_Close(fd);
        gint_world_switch();
    }
    jwidget_destroy((jwidget*)scene); return result;
}
