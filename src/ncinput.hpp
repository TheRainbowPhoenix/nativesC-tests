#pragma once

#ifdef swap
#undef swap
#endif

extern "C" {
#include <gint/display.h>
#include <gint/keyboard.h>
#include <justui/jwidget.h>
#include <justui/jscrolledlist.h>
#include <justui/jlabel.h>
}

namespace ncinput {

struct Theme {
    int modal_bg;
    int kbd_bg;
    int key_bg;
    int key_spec;
    int key_out;
    int txt;
    int txt_dim;
    int accent;
    int txt_acc;
    int hl;
    int check;
};

enum class ThemeName {
    Light,
    Dark,
    Grey
};

Theme const& get_theme(ThemeName name);

void init();

// Public API avoiding STL
// Returns 0 on success, -1 on cancel
int input(char* buffer, int max_len, const char* prompt = "Input:", const char* type = "alpha_numeric", ThemeName theme = ThemeName::Light);

// Returns index of selection, -1 on cancel
int pick(const char** options, int num_options, const char* prompt = "Select:", ThemeName theme = ThemeName::Light);

bool ask(const char* title, const char* body, const char* ok_text = "OK", const char* cancel_text = "Cancel", ThemeName theme = ThemeName::Light);

} // namespace ncinput
