#include "filebrowser.hpp"
#include <justui/jfileselect.h>
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <gint/display.h>

namespace filebrowser {

std::string browse(std::string const& title, ncinput::ThemeName theme_name) {
    auto const& t = ncinput::get_theme(theme_name);
    jscene* scene = jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_title = jlabel_create(title.c_str(), (jwidget*)header);
    jlabel_set_color(lbl_title, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // File Selector
    jfileselect* fs = jfileselect_create((jwidget*)scene);
    jwidget_set_stretch(fs, 1, 1, false);

    // Footer
    jwidget* footer = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(footer);
    jwidget_set_fixed_height(footer, 45);
    jwidget_set_background(footer, t.key_spec);
    jbutton* btn_cancel = jbutton_create("Cancel", footer);
    jwidget_set_stretch(btn_cancel, 1, 0, false);
    jbutton* btn_ok = jbutton_create("Open", footer);
    jwidget_set_stretch(btn_ok, 1, 0, false);
    jwidget_set_stretch(footer, 1, 0, false);

    std::string result = "";
    bool running = true;

    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            jscene_render((jwidget*)scene);
            dupdate();
        } else if (e.type == JBUTTON_TRIGGERED) {
            if (e.source == btn_ok) {
                char const* path = jfileselect_get_selected_path(fs);
                if (path) result = path;
                running = false;
            } else if (e.source == btn_cancel) {
                running = false;
            }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) {
                running = false;
            }
        }
    }

    jwidget_destroy((jwidget*)scene);
    return result;
}

}
