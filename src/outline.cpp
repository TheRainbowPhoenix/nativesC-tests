#include "outline.hpp"
#include "ced.hpp"
extern "C" {
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jscrolledlist.h>
#include <gint/display.h>
#include <cstring>
}

namespace outline {

struct Item {
    char name[64];
    int line;
};

static Item items[100];
static int num_items = 0;

static void info_fn(::jlist*, int, ::jlist_item_info* info) { info->selectable = info->triggerable = true; info->natural_height = 20; }
static void paint_fn(int x, int y, int, int, ::jlist*, int i, bool sel) { if (i >= 0 && i < num_items) dtext(x + 5, y + 2, sel ? C_WHITE : C_BLACK, items[i].name); }

int show(const char* filename, ncinput::ThemeName theme_name) {
    ncinput::Theme const& t = ncinput::get_theme(theme_name);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header); jwidget_set_fixed_height(header, 40); jwidget_set_background(header, t.accent);
    jlabel_set_text_color(jlabel_create("Outline", header), t.txt_acc);

    jscrolledlist* sl = (jscrolledlist*)jscrolledlist_create((jlist_item_info_function)info_fn, (jlist_item_paint_function)paint_fn, (jwidget*)scene);
    jwidget_set_stretch((jwidget*)sl, 1, 1, false);

    bool running = true, analyzing = true;
    int result = -1, line_idx = 0, current_pos = 0;
    ced::FileHandle f; f.open(filename, BFile_ReadOnly);
    num_items = 0;

    char read_buf[512];
    int buf_pos = 0;
    int buf_len = 0;

    while (running) {
        if (analyzing) {
            // Process chunks
            for (int k = 0; k < 20 && num_items < 100; k++) {
                char line[128];
                int line_pos = 0;
                bool found_newline = false;

                while (line_pos < 127) {
                    if (buf_pos >= buf_len) {
                        buf_len = f.read(read_buf, sizeof(read_buf), current_pos);
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
                        jlist_update_model((::jlist*)sl->list, num_items, nullptr);
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
            dclear(t.modal_bg);
            jscene_render(scene);
            if (analyzing) dtext(160, 220, C_RED, "Analyzing...");
            dupdate();
        } else if (e.type == JLIST_ITEM_TRIGGERED) {
            int idx = jlist_selected_item((::jlist*)sl->list);
            if (idx >= 0 && idx < num_items) { result = items[idx].line; running = false; }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN && e.key.key == KEY_EXIT) {
            running = false;
        }
    }
    jwidget_destroy((jwidget*)scene); return result;
}

}
