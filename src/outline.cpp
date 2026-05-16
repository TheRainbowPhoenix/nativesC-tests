#include "outline.hpp"
#include <fstream>
#include <sstream>
extern "C" {
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jscrolledlist.h>
}
#include <gint/display.h>

namespace ced {

static std::vector<std::string> outline_display_names;

int show_outline(std::string const& filename, ncinput::ThemeName theme_name) {
    auto const& t = ncinput::get_theme(theme_name);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox((jwidget*)header);
    jwidget_set_fixed_height((jwidget*)header, 40);
    jwidget_set_background((jwidget*)header, t.accent);
    jlabel* lbl_title = jlabel_create("Outline", (jwidget*)header);
    jlabel_set_text_color(lbl_title, t.txt_acc);
    jwidget_set_stretch((jwidget*)header, 1, 0, false);

    // Body (Initially show a button to run)
    jwidget* body = jwidget_create((jwidget*)scene);
    jlayout_set_stack((jwidget*)body);
    jwidget_set_stretch((jwidget*)body, 1, 1, false);

    jwidget* prompt_view = jwidget_create((jwidget*)body);
    jlayout_set_vbox((jwidget*)prompt_view)->spacing = 10;
    jlabel_create("This may take time for large files.", (jwidget*)prompt_view);
    jbutton* btn_run = jbutton_create("Run Analysis", (jwidget*)prompt_view);

    jwidget* result_view = jwidget_create((jwidget*)body);
    jlayout_set_vbox((jwidget*)result_view);

    auto info_fn = [](::jlist*, int, ::jlist_item_info* info) {
        info->selectable = true;
        info->triggerable = true;
        info->natural_height = 20;
    };
    auto paint_fn = [](int x, int y, int, int, ::jlist*, int i, bool sel) {
        if (i >= 0 && i < (int)outline_display_names.size()) {
            dtext(x + 5, y + 2, sel ? C_WHITE : C_BLACK, outline_display_names[i].c_str());
        }
    };

    jscrolledlist* sl = (jscrolledlist*)jscrolledlist_create((jlist_item_info_function)+info_fn, (jlist_item_paint_function)+paint_fn, (jwidget*)result_view);
    jwidget_set_stretch((jwidget*)sl, 1, 1, false);

    jwidget_set_visible(result_view, false);

    bool running = true;
    int result_line = -1;
    std::vector<OutlineItem> items;

    bool analyzing = false;
    std::ifstream analyzer_f;
    int analyzer_line_idx = 0;

    while (running) {
        jevent e = jscene_run(scene);

        // Ensure analysis continues even without blocking events
        // Note: in a real single-threaded JustUI app, we'd need jscene_run
        // to be non-blocking or use a timer.
        // For this port, we'll process chunks whenever jscene_run returns.

        if (analyzing) {
            // Cooperative multitasking to simulate background threading
            std::string line;
            bool model_updated = false;
            for (int i = 0; i < 10 && std::getline(analyzer_f, line); ++i) {
                size_t first = line.find_first_not_of(" \t");
                if (first != std::string::npos) {
                    std::string trimmed = line.substr(first);
                    if (trimmed.compare(0, 4, "def ") == 0 || trimmed.compare(0, 6, "class ") == 0) {
                        std::string indent_str(first, ' ');
                        std::string display_name = indent_str + (trimmed.compare(0, 6, "class ") == 0 ? "◈ " : "○ ") + trimmed;
                        items.push_back({trimmed, analyzer_line_idx, (int)first});
                        outline_display_names.push_back(display_name);
                        model_updated = true;
                    }
                }
                analyzer_line_idx++;
            }
            if (model_updated) {
                jlist_update_model((::jlist*)sl->list, items.size(), nullptr);
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
            jscene_render((jscene*)scene);
            if (analyzing) {
                dtext(160, 500, C_RED, "Analyzing... Press EXIT to stop");
            }
            dupdate();
        } else if (e.type == JBUTTON_TRIGGERED && e.source == btn_run) {
            analyzer_f.open(filename);
            analyzer_line_idx = 0;
            analyzing = true;
            items.clear();
            outline_display_names.clear();
            jlist_clear((::jlist*)sl->list);
        } else if (e.type == JLIST_ITEM_TRIGGERED) {
            int idx = jlist_selected_item((::jlist*)sl->list);
            if (idx >= 0 && idx < (int)items.size()) {
                result_line = items[idx].line;
                running = false;
            }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) running = false;
        }
    }

    jwidget_destroy((jwidget*)scene);
    outline_display_names.clear();
    return result_line;
}

}
