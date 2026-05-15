#include "problems.hpp"
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <gint/display.h>

namespace ced {

void show_problems(std::string const& filename, ncinput::ThemeName theme_name) {
    auto const& t = ncinput::get_theme(theme_name);
    jscene* scene = jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_title = jlabel_create("Problems", (jwidget*)header);
    jlabel_set_color(lbl_title, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // Body
    jwidget* body = jwidget_create((jwidget*)scene);
    jlayout_set_vbox(body);
    jwidget_set_stretch(body, 1, 1, false);
    jlabel_create("No problems detected (Dummy Linter).", (jwidget*)body);

    bool running = true;
    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            jscene_render((jscene*)scene);
            dupdate();
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) running = false;
        }
    }

    jwidget_destroy((jwidget*)scene);
}

}
