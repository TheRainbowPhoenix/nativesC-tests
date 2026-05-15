#include "outline.hpp"
#include <fstream>
#include <sstream>
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jscrolledlist.h>
#include <gint/display.h>

namespace ced {

int show_outline(std::string const& filename, ncinput::ThemeName theme_name) {
    auto const& t = ncinput::get_theme(theme_name);
    jscene* scene = jscene_create_fullscreen(nullptr);
    jlayout_set_vbox(scene)->spacing = 0;
    jwidget_set_background(scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create(scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_title = jlabel_create("Outline", header);
    jlabel_set_color(lbl_title, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // Body (Initially show a button to run)
    jwidget* body = jwidget_create(scene);
    jlayout_set_stack(body);
    jwidget_set_stretch(body, 1, 1, false);

    jwidget* prompt_view = jwidget_create(body);
    jlayout_set_vbox(prompt_view)->spacing = 10;
    jlabel_create("This may take time for large files.", prompt_view);
    jbutton* btn_run = jbutton_create("Run Analysis", prompt_view);

    jwidget* result_view = jwidget_create(body);
    jlayout_set_vbox(result_view);
    jscrolledlist* sl = jscrolledlist_create(result_view);
    jwidget_set_stretch(sl, 1, 1, false);

    jwidget_set_visible(result_view, false);

    bool running = true;
    int result_line = -1;
    std::vector<OutlineItem> items;

    bool analyzing = false;
    std::ifstream analyzer_f;
    int analyzer_line_idx = 0;

    while (running) {
        jevent e = jscene_run(scene);
        if (analyzing) {
            // Cooperative multitasking to simulate background threading
            std::string line;
            for (int i = 0; i < 10 && std::getline(analyzer_f, line); ++i) {
                size_t first = line.find_first_not_of(" \t");
                if (first != std::string::npos) {
                    std::string trimmed = line.substr(first);
                    if (trimmed.compare(0, 4, "def ") == 0 || trimmed.compare(0, 6, "class ") == 0) {
                        items.push_back({trimmed, analyzer_line_idx, (int)first});
                        jscrolledlist_add_item(sl, trimmed.c_str());
                    }
                }
                analyzer_line_idx++;
            }
            if (analyzer_f.eof()) {
                analyzing = false;
                jwidget_set_visible(prompt_view, false);
                jwidget_set_visible(result_view, true);
                jscene_show_and_focus(scene, result_view);
            }
            // Check for interruption
            if (keydown(KEY_EXIT)) {
                analyzing = false;
                analyzer_f.close();
            }
        }

        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            jscene_render(scene);
            if (analyzing) {
                dtext(160, 500, C_RED, "Analyzing... Press EXIT to stop");
            }
            dupdate();
        } else if (e.type == JBUTTON_TRIGGERED && e.source == btn_run) {
            analyzer_f.open(filename);
            analyzer_line_idx = 0;
            analyzing = true;
            items.clear();
            jscrolledlist_clear(sl);
        } else if (e.type == JSCROLLEDLIST_ITEM_TRIGGERED) {
            int idx = jscrolledlist_get_selected_index(sl);
            if (idx >= 0 && idx < (int)items.size()) {
                result_line = items[idx].line;
                running = false;
            }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) running = false;
        }
    }

    jwidget_destroy(scene);
    return result_line;
}

}
