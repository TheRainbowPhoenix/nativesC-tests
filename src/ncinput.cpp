#include "ncinput.hpp"
#include <justui/jwidget-api.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jlayout.h>
#include <justui/jscene.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <vector>
#include <string>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <new>

namespace ncinput {

static const Theme LightTheme = {
    .modal_bg = C_WHITE,
    .kbd_bg   = C_WHITE,
    .key_bg   = C_WHITE,
    .key_spec = C_RGB(28, 29, 28),
    .key_out  = C_DARK,
    .txt      = C_RGB(4, 4, 4),
    .txt_dim  = C_RGB(8, 8, 8),
    .accent   = C_RGB(1, 11, 26),
    .txt_acc  = C_WHITE,
    .hl       = C_RGB(28, 29, 28),
    .check    = C_WHITE
};

static const Theme DarkTheme = {
    .modal_bg = C_RGB(7, 7, 8),
    .kbd_bg   = C_RGB(7, 7, 8),
    .key_bg   = C_RGB(7, 7, 8),
    .key_spec = C_RGB(11, 11, 12),
    .key_out  = C_RGB(12, 19, 31),
    .txt      = C_WHITE,
    .txt_dim  = C_RGB(8, 8, 8),
    .accent   = C_RGB(12, 19, 31),
    .txt_acc  = C_WHITE,
    .hl       = C_RGB(11, 11, 12),
    .check    = C_WHITE
};

static const Theme GreyTheme = {
    .modal_bg = C_LIGHT,
    .kbd_bg   = C_LIGHT,
    .key_bg   = C_WHITE,
    .key_spec = 0xCE59,
    .key_out  = C_BLACK,
    .txt      = C_BLACK,
    .txt_dim  = C_RGB(8, 8, 8),
    .accent   = C_BLACK,
    .txt_acc  = C_WHITE,
    .hl       = 0xCE59,
    .check    = C_WHITE
};

Theme const& get_theme(ThemeName name) {
    switch (name) {
        case ThemeName::Dark: return DarkTheme;
        case ThemeName::Grey: return GreyTheme;
        case ThemeName::Light:
        default: return LightTheme;
    }
}

// --- Custom Keyboard Widget ---

static const std::vector<std::vector<std::string>> LAYOUT_QWERTY = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", ":"},
    {"z", "x", "c", "v", "b", "n", "m", ",", ".", "_"}
};

static const std::vector<std::vector<std::string>> LAYOUT_SYM = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"@", "#", "$", "_", "&", "-", "+", "(", ")", "/"},
    {"=", "\\", "<", "*", "\"", "'", ":", ";", "!", "?"},
    {"{", "}", "[", "]", "^", "~", "`", "|", "<", ">"}
};

struct KeyRect {
    int x, y, w, h;
    std::string label;
    std::string val;
    bool is_spec;
    bool is_acc;
};

struct nkeyboard {
    jwidget widget;
    ThemeName theme_name;
    int current_tab;
    bool shift;
    std::string last_key;
    bool visible;
    bool enable_tabs;
    // For communication
    std::string* target_text;
    bool* finished;
};

static int keyboard_type_id = -1;
uint16_t NKEYBOARD_KEY_PRESSED;

static std::vector<KeyRect> get_math_rects(int k_y) {
    std::vector<KeyRect> keys;
    int start_y = k_y + 30;
    int row_h = (260 - 30) / 4;
    int side_w = 50;
    int center_w = 320 - (side_w * 2);
    int numpad_w = center_w / 3;

    const char* l_chars[] = {"+", "-", "*", "/"};
    for (int i = 0; i < 4; i++) {
        keys.push_back({0, start_y + i*row_h, side_w, row_h, l_chars[i], l_chars[i], true, false});
    }

    struct { const char* disp; const char* val; bool spec; bool acc; } r_chars[] = {
        {"%", "%", true, false}, {" ", " ", true, false}, {"<-", "BACKSPACE", true, false}, {"EXE", "ENTER", false, true}
    };
    for (int i = 0; i < 4; i++) {
        keys.push_back({320 - side_w, start_y + i*row_h, side_w, row_h, r_chars[i].disp, r_chars[i].val, r_chars[i].spec, r_chars[i].acc});
    }

    const char* nums[3][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            keys.push_back({side_w + c*numpad_w, start_y + r*row_h, numpad_w, row_h, nums[r][c], nums[r][c], false, false});
        }
    }

    const char* bot_row[] = {",", "#", "0", "=", "."};
    int widths[] = {1, 1, 2, 1, 1};
    int unit_w = center_w / 6;
    int cur_x = side_w;
    for (int i = 0; i < 5; i++) {
        int w = widths[i] * unit_w;
        keys.push_back({cur_x, start_y + 3*row_h, w, row_h, bot_row[i], bot_row[i], false, false});
        cur_x += w;
    }
    return keys;
}

static std::vector<KeyRect> get_numpad_rects(int k_y) {
    std::vector<KeyRect> keys;
    int row_h = 260 / 4;
    int action_w = 80;
    int digit_w = (320 - action_w) / 3;

    keys.push_back({320 - action_w, k_y, action_w, row_h, "<-", "BACKSPACE", true, false});
    keys.push_back({320 - action_w, k_y + row_h, action_w, row_h * 3, "EXE", "ENTER", false, true});

    const char* nums[3][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            keys.push_back({c * digit_w, k_y + r * row_h, digit_w, row_h, nums[r][c], nums[r][c], false, false});
        }
    }

    // Bottom row for numpad (0, ., -)
    keys.push_back({0, k_y + 3 * row_h, digit_w, row_h, "-", "-", false, false});
    keys.push_back({digit_w, k_y + 3 * row_h, digit_w, row_h, "0", "0", false, false});
    keys.push_back({digit_w * 2, k_y + 3 * row_h, digit_w, row_h, ".", ".", false, false});

    return keys;
}

static void draw_key_impl(int x, int y, int w, int h, std::string label, bool is_special, bool is_pressed, bool is_accent, Theme const& t) {
    int bg;
    if (is_pressed) bg = t.hl;
    else if (is_accent) bg = t.accent;
    else if (is_special) bg = t.key_spec;
    else bg = t.key_bg;

    int txt_col = is_accent ? t.txt_acc : t.txt;
    int border_col = t.key_spec;

    drect(x + 1, y + 1, x + w - 1, y + h - 1, bg);
    drect_border(x, y, x + w, y + h, C_NONE, 1, border_col);
    dtext_opt(x + w/2, y + h/2, txt_col, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, label.c_str(), -1);
}

static void nkeyboard_poly_render(void* w0, int x, int y) {
    nkeyboard* k = (nkeyboard*)w0;
    if (!k->visible) return;
    Theme const& t = get_theme(k->theme_name);
    int k_y = y;

    drect(x, k_y, x + 320, k_y + 260, t.kbd_bg);
    dhline(k_y, t.key_spec);

    if (k->enable_tabs) {
        int tab_w = 320 / 3;
        static const char* tabs[] = {"ABC", "Sym", "Math"};
        for (int i = 0; i < 3; i++) {
            int tx = x + i * tab_w;
            bool is_active = (i == k->current_tab);
            int bg = is_active ? t.kbd_bg : t.key_spec;
            drect(tx, k_y, tx + tab_w, k_y + 30, bg);
            drect_border(tx, k_y, tx + tab_w, k_y + 30, C_NONE, 1, t.key_spec);
            if (is_active) {
                drect(tx + 1, k_y + 30 - 1, tx + tab_w - 1, k_y + 30 + 1, t.kbd_bg);
            }
            dtext_opt(tx + tab_w/2, k_y + 15, t.txt, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, tabs[i], -1);
        }

        if (k->current_tab == 2) {
            static std::vector<KeyRect> math_keys;
            if (math_keys.empty()) math_keys = get_math_rects(0);
            for (auto const& kr : math_keys) {
                bool is_pressed = (k->last_key == kr.val);
                draw_key_impl(x + kr.x, k_y + (kr.y - 0), kr.w, kr.h, kr.label, kr.is_spec, is_pressed, kr.is_acc, t);
            }
        } else {
            auto const& layout = (k->current_tab == 1) ? LAYOUT_SYM : LAYOUT_QWERTY;
            int grid_y = k_y + 30;
            int row_h = (260 - 30) / 5;
            for (size_t r = 0; r < layout.size(); r++) {
                int kw = 320 / layout[r].size();
                for (size_t c = 0; c < layout[r].size(); c++) {
                    char label_buf[2] = { (char)layout[r][c][0], 0 };
                    if (k->current_tab == 0 && k->shift) {
                        label_buf[0] = toupper(label_buf[0]);
                    }
                    draw_key_impl(x + c * kw, grid_y + r * row_h, kw, row_h, label_buf, false, k->last_key == label_buf, false, t);
                }
            }
            int bot_y = grid_y + 4 * row_h;
            draw_key_impl(x + 0, bot_y, 50, row_h, "CAPS", true, k->shift, false, t);
            draw_key_impl(x + 50, bot_y, 50, row_h, "<-", true, k->last_key == "BACKSPACE", false, t);
            draw_key_impl(x + 100, bot_y, 160, row_h, "Space", false, k->last_key == " ", false, t);
            draw_key_impl(x + 260, bot_y, 60, row_h, "EXE", false, k->last_key == "ENTER", true, t);
        }
    } else {
        static std::vector<KeyRect> numpad_keys;
        if (numpad_keys.empty()) numpad_keys = get_numpad_rects(0);
        for (auto const& kr : numpad_keys) {
            bool is_pressed = (k->last_key == kr.val);
            draw_key_impl(x + kr.x, k_y + (kr.y - 0), kr.w, kr.h, kr.label, kr.is_spec, is_pressed, kr.is_acc, t);
        }
    }
}

static bool nkeyboard_poly_event(void* w0, jevent e) {
    nkeyboard* k = (nkeyboard*)w0;
    if (e.type == JWIDGET_KEY) {
        key_event_t ev = e.key;
        if (ev.type == KEYEV_TOUCH_DOWN || ev.type == KEYEV_TOUCH_UP) {
            int tx = ev.x - k->widget.x;
            int ty = ev.y - k->widget.y;
            if (ty < 0 || ty > 260) return false;

            if (ev.type == KEYEV_TOUCH_DOWN) {
                if (k->enable_tabs && ty < 30) {
                    k->current_tab = tx / (320/3);
                    if (k->current_tab > 2) k->current_tab = 2;
                    k->widget.update = 1;
                    return true;
                }

                std::string pressed = "";
                if (k->enable_tabs) {
                    if (k->current_tab == 2) {
                        auto keys = get_math_rects(k->widget.y);
                        for (auto const& kr : keys) {
                            if (tx >= kr.x && tx < kr.x + kr.w && ev.y >= kr.y && ev.y < kr.y + kr.h) {
                                pressed = kr.val;
                                break;
                            }
                        }
                    } else {
                        int grid_y = 30;
                        int row_h = (260 - grid_y) / 5;
                        int row_idx = (ty - grid_y) / row_h;
                        if (row_idx >= 0 && row_idx < 4) {
                            auto const& layout = (k->current_tab == 1) ? LAYOUT_SYM : LAYOUT_QWERTY;
                            int kw = 320 / layout[row_idx].size();
                            int col_idx = tx / kw;
                            if (col_idx < (int)layout[row_idx].size()) {
                                pressed = layout[row_idx][col_idx];
                                if (k->current_tab == 0 && k->shift) pressed[0] = toupper(pressed[0]);
                            }
                        } else if (row_idx == 4) {
                            if (tx < 50) { k->shift = !k->shift; }
                            else if (tx < 100) { pressed = "BACKSPACE"; }
                            else if (tx < 260) { pressed = " "; }
                            else { pressed = "ENTER"; }
                        }
                    }
                } else {
                    auto keys = get_numpad_rects(k->widget.y);
                    for (auto const& kr : keys) {
                        if (tx >= kr.x && tx < kr.x + kr.w && ev.y >= kr.y && ev.y < kr.y + kr.h) {
                            pressed = kr.val;
                            break;
                        }
                    }
                }

                if (!pressed.empty()) {
                    k->last_key = pressed;
                    k->widget.update = 1;

                    if (k->target_text) {
                        if (pressed == "BACKSPACE") {
                            if (!k->target_text->empty()) k->target_text->pop_back();
                        } else if (pressed == "ENTER") {
                            if (k->finished) *k->finished = true;
                        } else if (pressed == "CAPS") {
                            // Already handled
                        } else {
                            *k->target_text += pressed;
                        }
                    }

                    jevent ne;
                    ne.source = k;
                    ne.type = NKEYBOARD_KEY_PRESSED;
                    jwidget_emit(k, ne);
                    return true;
                }
            } else if (ev.type == KEYEV_TOUCH_UP) {
                k->last_key = "";
                k->widget.update = 1;
                return true;
            }
        }
    }
    return false;
}

static void nkeyboard_poly_destroy(void* w0) {
    nkeyboard* k = (nkeyboard*)w0;
    k->~nkeyboard();
    // malloc was used in nkeyboard_create, JustUI will free() the jwidget pointer
    // since this is called from jwidget_destroy.
}

static jwidget_poly type_nkeyboard = {
    .name = "nkeyboard",
    .csize = [](void* w) { ((jwidget*)w)->w = 320; ((jwidget*)w)->h = 260; },
    .layout = NULL,
    .render = nkeyboard_poly_render,
    .event = nkeyboard_poly_event,
    .destroy = nkeyboard_poly_destroy,
};

nkeyboard* nkeyboard_create(void* parent, ThemeName theme, bool enable_tabs) {
    if (keyboard_type_id < 0) return nullptr;
    nkeyboard* k = (nkeyboard*)malloc(sizeof(nkeyboard));
    if (!k) return nullptr;
    new (k) nkeyboard();
    jwidget_init(&k->widget, keyboard_type_id, parent);
    k->theme_name = theme;
    k->current_tab = 0;
    k->shift = false;
    k->last_key = "";
    k->visible = true;
    k->enable_tabs = enable_tabs;
    k->target_text = nullptr;
    k->finished = nullptr;
    jwidget_set_focus_policy(k, J_FOCUS_POLICY_ACCEPT);
    return k;
}

// --- Public API ---

std::string input(std::string const& prompt, std::string const& type, ThemeName theme) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_prompt = jlabel_create(prompt.c_str(), (jwidget*)header);
    jlabel_set_text_color(lbl_prompt, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // Body
    jwidget* body = jwidget_create((jwidget*)scene);
    jlayout_set_vbox(body);
    jwidget_set_stretch(body, 1, 1, false);

    std::string text = "";
    jlabel* lbl_text = jlabel_create("_", (jwidget*)body);
    jlabel_set_text_color(lbl_text, t.txt);

    // Keyboard
    bool enable_tabs = true;
    int start_tab = 0;
    if (type.find("numeric") != std::string::npos) {
        enable_tabs = false;
    } else if (type == "math") {
        start_tab = 2;
    }

    nkeyboard* kbd = nkeyboard_create((jwidget*)scene, theme, enable_tabs);
    kbd->current_tab = start_tab;
    bool finished = false;
    kbd->target_text = &text;
    kbd->finished = &finished;

    bool running = true;
    std::string result = "";
    bool cancelled = false;

    while (running && !finished) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            std::string display_text = text + "_";
            jlabel_set_text(lbl_text, display_text.c_str());
            jscene_render((jscene*)scene);
            dupdate();
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) {
                cancelled = true;
                running = false;
            } else if (e.key.key == KEY_EXE) {
                running = false;
            }
        }
    }

    if (!cancelled) result = text;
    jwidget_destroy((jwidget*)scene);
    return cancelled ? "" : result;
}

#include <justui/jscrolledlist.h>
#include <cstdio>

std::string pick(std::vector<std::string> const& options, std::string const& prompt, ThemeName theme, bool multi) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_prompt = jlabel_create(prompt.c_str(), (jwidget*)header);
    jlabel_set_text_color(lbl_prompt, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // Implementation for pick using jscrolledlist
    static std::vector<std::string> const* current_options;
    current_options = &options;

    auto info_fn = [](struct jlist* /* l */, int /* i */, jlist_item_info* info) {
        info->selectable = true;
        info->triggerable = true;
        info->natural_height = 20;
    };
    auto paint_fn = [](int x, int y, int /* w */, int /* h */, struct jlist* /* l */, int i, bool sel) {
        if (current_options && i >= 0 && i < (int)current_options->size()) {
            dtext(x + 5, y + 2, sel ? C_WHITE : C_BLACK, (*current_options)[i].c_str());
        }
    };

    jscrolledlist* sl = jscrolledlist_create(info_fn, paint_fn, (jwidget*)scene);
    jlist_update_model(sl->list, options.size(), nullptr);
    jwidget_set_stretch((jwidget*)sl, 1, 1, false);

    // Footer
    jwidget* footer = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(footer);
    jwidget_set_fixed_height(footer, 45);
    jwidget_set_background(footer, t.key_spec);
    jbutton* btn_ok = jbutton_create(multi ? "OK" : "Select", footer);
    jwidget_set_stretch(btn_ok, 1, 0, false);
    jwidget_set_stretch(footer, 1, 0, false);

    bool running = true;
    std::string result = "";

    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            jscene_render((jscene*)scene);
            dupdate();
        } else if (e.type == JBUTTON_TRIGGERED && e.source == btn_ok) {
            int idx = jlist_selected_item(sl->list);
            if (idx >= 0 && idx < (int)options.size()) {
                result = options[idx];
            }
            running = false;
        } else if (e.type == JLIST_ITEM_TRIGGERED) {
            int idx = (int)e.data;
            if (idx >= 0 && idx < (int)options.size()) {
                result = options[idx];
            }
            running = false;
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) {
                result = "";
                running = false;
            }
        }
    }

    jwidget_destroy((jwidget*)scene);
    return result;
}

bool ask(std::string const& title, std::string const& body, std::string const& ok_text, std::string const& cancel_text, ThemeName theme) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);

    // Header
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header);
    jwidget_set_fixed_height(header, 40);
    jwidget_set_background(header, t.accent);
    jlabel* lbl_title = jlabel_create(title.c_str(), (jwidget*)header);
    jlabel_set_text_color(lbl_title, t.txt_acc);
    jwidget_set_stretch(header, 1, 0, false);

    // Body
    jwidget* body_cont = jwidget_create((jwidget*)scene);
    jlayout_set_vbox(body_cont);
    jwidget_set_stretch(body_cont, 1, 1, false);
    jlabel* lbl_body = jlabel_create(body.c_str(), (jwidget*)body_cont);
    jlabel_set_text_color(lbl_body, t.txt);

    // Footer
    jwidget* footer = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(footer);
    jwidget_set_fixed_height(footer, 45);
    jwidget_set_background(footer, t.key_spec);
    jbutton* btn_cancel = jbutton_create(cancel_text.c_str(), footer);
    jwidget_set_stretch(btn_cancel, 1, 0, false);
    jbutton* btn_ok = jbutton_create(ok_text.c_str(), footer);
    jwidget_set_stretch(btn_ok, 1, 0, false);
    jwidget_set_stretch(footer, 1, 0, false);

    bool running = true;
    bool result = false;

    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg);
            jscene_render((jscene*)scene);
            dupdate();
        } else if (e.type == JBUTTON_TRIGGERED) {
            if (e.source == btn_ok) {
                result = true;
                running = false;
            } else if (e.source == btn_cancel) {
                result = false;
                running = false;
            }
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) {
                result = false;
                running = false;
            }
        }
    }

    jwidget_destroy((jwidget*)scene);
    return result;
}

// --- Initialization ---
void init() {
    if (keyboard_type_id >= 0) return;
    keyboard_type_id = j_register_widget(&type_nkeyboard);
    NKEYBOARD_KEY_PRESSED = j_register_event();
}

} // namespace ncinput
