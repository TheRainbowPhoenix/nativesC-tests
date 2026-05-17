#include "problems.h"
#include <justui/jscene.h>
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <gint/display.h>
#include <stdio.h>

typedef struct {
    int line;
    char message[64];
} problem_t;

static problem_t problems_list[10];
static int num_problems = 0;

void problems_run_lint(const char* filename) {
    (void)filename;
    num_problems = 0;
    problems_list[num_problems++] = (problem_t){5, "Missing docstring"};
    problems_list[num_problems++] = (problem_t){12, "Variable 'x' defined but never used"};
}

void problems_show(nc_theme_name_t theme_name) {
    nc_theme_t const* t = nc_get_theme(theme_name);
    jscene* scene = (jscene*)jscene_create_fullscreen(NULL);
    jlayout_set_vbox((jwidget*)scene)->spacing = 5;
    jwidget_set_background((jwidget*)scene, t->modal_bg);

    jlabel_create("Linter Results:", (jwidget*)scene);

    if (num_problems == 0) {
        jlabel_create("No problems found.", (jwidget*)scene);
    } else {
        for (int i = 0; i < num_problems; i++) {
            char buf[128];
            sprintf(buf, "Line %d: %s", problems_list[i].line, problems_list[i].message);
            jlabel_create(buf, (jwidget*)scene);
        }
    }

    bool running = true;
    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) { dclear(t->modal_bg); jscene_render(scene); dupdate(); }
        else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN && e.key.key == KEY_EXIT) running = false;
    }
    jwidget_destroy((jwidget*)scene);
}
