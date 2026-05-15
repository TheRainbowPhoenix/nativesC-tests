#pragma once

#include <gint/display.h>
#include <gint/keyboard.h>
#include <justui/jwidget.h>

#ifdef swap
#undef swap
#endif

#include <string>
#include <vector>
#include <map>

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

// Public API matching cinput.py
std::string input(std::string const& prompt = "Input:", std::string const& type = "alpha_numeric", ThemeName theme = ThemeName::Light);
std::string pick(std::vector<std::string> const& options, std::string const& prompt = "Select:", ThemeName theme = ThemeName::Light, bool multi = false);
bool ask(std::string const& title, std::string const& body, std::string const& ok_text = "OK", std::string const& cancel_text = "Cancel", ThemeName theme = ThemeName::Light);

} // namespace ncinput
