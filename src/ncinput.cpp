#include "ncinput.hpp"
extern "C" {
#include <justui/jwidget-api.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <justui/jlayout.h>
#include <justui/jscene.h>
#include <justui/jscrolledlist.h>
}
#include <gint/display.h>
#include <gint/keyboard.h>
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

static const char* LAYOUT_QWERTY[4][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", ":"},
    {"z", "x", "c", "v", "b", "n", "m", ",", ".", "_"}
};

static const char* LAYOUT_SYM[4][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"@", "#", "$", "_", "&", "-", "+", "(", ")", "/"},
    {"=", "\\", "<", "*", "\"", "'", ":", ";", "!", "?"},
    {"{", "}", "[", "]", "^", "~", "`", "|", "<", ">"}
};

struct KeyRect {
    int x, y, w, h;
    const char* label;
    const char* val;
    bool is_spec;
    bool is_acc;
};

struct nkeyboard {
    jwidget widget;
    ThemeName theme_name;
    int current_tab;
    bool shift;
    const char* last_key_ptr;
    bool visible;
    bool enable_tabs;
    char* target_buffer;
    int max_len;
    bool* finished;
};

static int keyboard_type_id = -1;
uint16_t NKEYBOARD_KEY_PRESSED;

static int get_math_rects(int k_y, KeyRect* out) {
    int start_y = k_y + 30;
    int row_h = (260 - 30) / 4;
    int side_w = 50;
    int center_w = 320 - (side_w * 2);
    int numpad_w = center_w / 3;
    int n = 0;
    const char* l_chars[] = {"+", "-", "*", "/"};
    for (int i = 0; i < 4; i++) out[n++] = {0, start_y + i*row_h, side_w, row_h, l_chars[i], l_chars[i], true, false};
    struct { const char* disp; const char* val; bool spec; bool acc; } r_chars[] = {
        {"%", "%", true, false}, {" ", " ", true, false}, {"<-", "BACKSPACE", true, false}, {"EXE", "ENTER", false, true}
    };
    for (int i = 0; i < 4; i++) out[n++] = {320 - side_w, start_y + i*row_h, side_w, row_h, r_chars[i].disp, r_chars[i].val, r_chars[i].spec, r_chars[i].acc};
    const char* nums[3][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) out[n++] = {side_w + c*numpad_w, start_y + r*row_h, numpad_w, row_h, nums[r][c], nums[r][c], false, false};
    const char* bot_row[] = {",", "#", "0", "=", "."};
    int widths[] = {1, 1, 2, 1, 1};
    int unit_w = center_w / 6;
    int cur_x = side_w;
    for (int i = 0; i < 5; i++) {
        int w = widths[i] * unit_w;
        out[n++] = {cur_x, start_y + 3*row_h, w, row_h, bot_row[i], bot_row[i], false, false};
        cur_x += w;
    }
    return n;
}

static int get_numpad_rects(int k_y, KeyRect* out) {
    int row_h = 260 / 4;
    int action_w = 80;
    int digit_w = (320 - action_w) / 3;
    int n = 0;
    out[n++] = {320 - action_w, k_y, action_w, row_h, "<-", "BACKSPACE", true, false};
    out[n++] = {320 - action_w, k_y + row_h, action_w, row_h * 3, "EXE", "ENTER", false, true};
    const char* nums[3][3] = {{"1","2","3"}, {"4","5","6"}, {"7","8","9"}};
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++) out[n++] = {c * digit_w, k_y + r * row_h, digit_w, row_h, nums[r][c], nums[r][c], false, false};
    out[n++] = {0, k_y + 3 * row_h, digit_w, row_h, "-", "-", false, false};
    out[n++] = {digit_w, k_y + 3 * row_h, digit_w, row_h, "0", "0", false, false};
    out[n++] = {digit_w * 2, k_y + 3 * row_h, digit_w, row_h, ".", ".", false, false};
    return n;
}

static void draw_key_impl(int x, int y, int w, int h, const char* label, bool is_special, bool is_pressed, bool is_accent, Theme const& t) {
    int bg = is_pressed ? t.hl : (is_accent ? t.accent : (is_special ? t.key_spec : t.key_bg));
    int txt_col = is_accent ? t.txt_acc : t.txt;
    drect(x + 1, y + 1, x + w - 1, y + h - 1, bg);
    drect_border(x, y, x + w, y + h, C_NONE, 1, t.key_spec);
    dtext_opt(x + w/2, y + h/2, txt_col, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, label, -1);
}

static void nkeyboard_poly_render(void* w0, int x, int y) {
    nkeyboard* k = (nkeyboard*)w0;
    if (!k->visible) return;
    Theme const& t = get_theme(k->theme_name);
    drect(x, y, x + 320, y + 260, t.kbd_bg);
    dhline(y, t.key_spec);
    if (k->enable_tabs) {
        int tab_w = 320 / 3;
        static const char* tabs[] = {"ABC", "Sym", "Math"};
        for (int i = 0; i < 3; i++) {
            int tx = x + i * tab_w;
            bool is_active = (i == k->current_tab);
            drect(tx, y, tx + tab_w, y + 30, is_active ? t.kbd_bg : t.key_spec);
            drect_border(tx, y, tx + tab_w, y + 30, C_NONE, 1, t.key_spec);
            if (is_active) drect(tx + 1, y + 29, tx + tab_w - 1, y + 31, t.kbd_bg);
            dtext_opt(tx + tab_w/2, y + 15, t.txt, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, tabs[i], -1);
        }
        if (k->current_tab == 2) {
            KeyRect math_keys[32]; int n = get_math_rects(0, math_keys);
            for (int i = 0; i < n; i++) draw_key_impl(x + math_keys[i].x, y + math_keys[i].y, math_keys[i].w, math_keys[i].h, math_keys[i].label, math_keys[i].is_spec, k->last_key_ptr && strcmp(k->last_key_ptr, math_keys[i].val) == 0, math_keys[i].is_acc, t);
        } else {
            const char* (*layout)[10] = (k->current_tab == 1) ? LAYOUT_SYM : LAYOUT_QWERTY;
            int grid_y = y + 30; int row_h = (260 - 30) / 5;
            for (int r = 0; r < 4; r++) for (int c = 0; c < 10; c++) {
                char label_buf[2] = { (char)layout[r][c][0], 0 };
                if (k->current_tab == 0 && k->shift) label_buf[0] = toupper(label_buf[0]);
                draw_key_impl(x + c * 32, grid_y + r * row_h, 32, row_h, label_buf, false, k->last_key_ptr && (k->last_key_ptr[0] == label_buf[0]), false, t);
            }
            int bot_y = grid_y + 4 * row_h;
            draw_key_impl(x + 0, bot_y, 50, row_h, "CAPS", true, k->shift, false, t);
            draw_key_impl(x + 50, bot_y, 50, row_h, "<-", true, k->last_key_ptr && strcmp(k->last_key_ptr, "BACKSPACE") == 0, false, t);
            draw_key_impl(x + 100, bot_y, 160, row_h, "Space", false, k->last_key_ptr && strcmp(k->last_key_ptr, " ") == 0, false, t);
            draw_key_impl(x + 260, bot_y, 60, row_h, "EXE", false, k->last_key_ptr && strcmp(k->last_key_ptr, "ENTER") == 0, true, t);
        }
    } else {
        KeyRect numpad_keys[16]; int n = get_numpad_rects(0, numpad_keys);
        for (int i = 0; i < n; i++) draw_key_impl(x + numpad_keys[i].x, y + numpad_keys[i].y, numpad_keys[i].w, numpad_keys[i].h, numpad_keys[i].label, numpad_keys[i].is_spec, k->last_key_ptr && strcmp(k->last_key_ptr, numpad_keys[i].val) == 0, numpad_keys[i].is_acc, t);
    }
}

static bool nkeyboard_poly_event(void* w0, jevent e) {
    nkeyboard* k = (nkeyboard*)w0;
    if (e.type == JWIDGET_KEY) {
        key_event_t ev = e.key;
        if (ev.type == KEYEV_TOUCH_DOWN || ev.type == KEYEV_TOUCH_UP) {
            int tx = ev.x - k->widget.x, ty = ev.y - k->widget.y;
            if (ty < 0 || ty > 260) return false;
            if (ev.type == KEYEV_TOUCH_DOWN) {
                if (k->enable_tabs && ty < 30) { k->current_tab = tx / (320/3); if (k->current_tab > 2) k->current_tab = 2; k->widget.update = 1; return true; }
                const char* pressed = nullptr; static char single_char_val[2] = {0, 0};
                if (k->enable_tabs) {
                    if (k->current_tab == 2) {
                        KeyRect math_keys[32]; int n = get_math_rects(0, math_keys);
                        for (int i = 0; i < n; i++) if (tx >= math_keys[i].x && tx < math_keys[i].x + math_keys[i].w && ty >= math_keys[i].y && ty < math_keys[i].y + math_keys[i].h) { pressed = math_keys[i].val; break; }
                    } else {
                        int grid_y = 30, row_h = (260 - grid_y) / 5, row_idx = (ty - grid_y) / row_h;
                        if (row_idx >= 0 && row_idx < 4) {
                            const char* (*layout)[10] = (k->current_tab == 1) ? LAYOUT_SYM : LAYOUT_QWERTY;
                            int kw = 32, col_idx = tx / kw;
                            if (col_idx < 10) { single_char_val[0] = layout[row_idx][col_idx][0]; if (k->current_tab == 0 && k->shift) single_char_val[0] = toupper(single_char_val[0]); pressed = single_char_val; }
                        } else if (row_idx == 4) {
                            if (tx < 50) k->shift = !k->shift; else if (tx < 100) pressed = "BACKSPACE"; else if (tx < 260) pressed = " "; else pressed = "ENTER";
                        }
                    }
                } else {
                    KeyRect numpad_keys[16]; int n = get_numpad_rects(0, numpad_keys);
                    for (int i = 0; i < n; i++) if (tx >= numpad_keys[i].x && tx < numpad_keys[i].x + numpad_keys[i].w && ty >= numpad_keys[i].y && ty < numpad_keys[i].y + numpad_keys[i].h) { pressed = numpad_keys[i].val; break; }
                }
                if (pressed) {
                    k->last_key_ptr = pressed; k->widget.update = 1;
                    if (k->target_buffer) {
                        int len = strlen(k->target_buffer);
                        if (strcmp(pressed, "BACKSPACE") == 0) { if (len > 0) k->target_buffer[len - 1] = '\0'; }
                        else if (strcmp(pressed, "ENTER") == 0) { if (k->finished) *k->finished = true; }
                        else if (strcmp(pressed, "CAPS") == 0) {}
                        else if (len < k->max_len - 1) { k->target_buffer[len] = pressed[0]; k->target_buffer[len + 1] = '\0'; }
                    }
                    jevent ne; ne.source = k; ne.type = NKEYBOARD_KEY_PRESSED; jwidget_emit(k, ne); return true;
                }
            } else if (ev.type == KEYEV_TOUCH_UP) { k->last_key_ptr = nullptr; k->widget.update = 1; return true; }
        }
    }
    return false;
}

static jwidget_poly type_nkeyboard = {
    .name = "nkeyboard",
    .csize = [](void* w) { ((jwidget*)w)->w = 320; ((jwidget*)w)->h = 260; },
    .render = nkeyboard_poly_render,
    .event = nkeyboard_poly_event,
    .destroy = NULL, // No custom destruction needed for malloc'd widget itself as jwidget_destroy handles it
};

nkeyboard* nkeyboard_create(void* parent, ThemeName theme, bool enable_tabs) {
    if (keyboard_type_id < 0) return nullptr;
    nkeyboard* k = (nkeyboard*)malloc(sizeof(nkeyboard));
    if (!k) return nullptr;
    memset(k, 0, sizeof(nkeyboard));
    jwidget_init(&k->widget, keyboard_type_id, parent);
    k->theme_name = theme; k->enable_tabs = enable_tabs;
    jwidget_set_focus_policy(k, J_FOCUS_POLICY_ACCEPT);
    return k;
}

int input(char* buffer, int max_len, const char* prompt, const char* type, ThemeName theme) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0;
    jwidget_set_background((jwidget*)scene, t.modal_bg);
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header); jwidget_set_fixed_height(header, 40); jwidget_set_background(header, t.accent);
    jlabel_set_text_color(jlabel_create(prompt, header), t.txt_acc);
    jwidget* body = jwidget_create((jwidget*)scene);
    jlayout_set_vbox(body); jwidget_set_stretch(body, 1, 1, false);
    jlabel* lbl_text = jlabel_create("_", body); jlabel_set_text_color(lbl_text, t.txt);
    bool enable_tabs = !strstr(type, "numeric"), finished = false;
    nkeyboard* kbd = nkeyboard_create((jwidget*)scene, theme, enable_tabs);
    if (kbd && strcmp(type, "math") == 0) kbd->current_tab = 2;
    if (kbd) { kbd->target_buffer = buffer; kbd->max_len = max_len; kbd->finished = &finished; }
    int result = 0; char display_buf[max_len + 2];
    while (!finished) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) {
            dclear(t.modal_bg); strcpy(display_buf, buffer); strcat(display_buf, "_");
            jlabel_set_text(lbl_text, display_buf); jscene_render(scene); dupdate();
        } else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN) {
            if (e.key.key == KEY_EXIT) { result = -1; break; }
            else if (e.key.key == KEY_EXE) break;
        }
    }
    jwidget_destroy((jwidget*)scene);
    return result;
}

static const char** pick_options_arr = nullptr;
static void pick_info_fn(::jlist*, int, ::jlist_item_info* info) { info->selectable = info->triggerable = true; info->natural_height = 20; }
static void pick_paint_fn(int x, int y, int, int, ::jlist*, int i, bool sel) { if (pick_options_arr && i >= 0) dtext(x + 5, y + 2, sel ? C_WHITE : C_BLACK, pick_options_arr[i]); }

int pick(const char** options, int num_options, const char* prompt, ThemeName theme) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0; jwidget_set_background((jwidget*)scene, t.modal_bg);
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header); jwidget_set_fixed_height(header, 40); jwidget_set_background(header, t.accent);
    jlabel_set_text_color(jlabel_create(prompt, header), t.txt_acc);
    pick_options_arr = options;
    jscrolledlist* sl = (jscrolledlist*)jscrolledlist_create((jlist_item_info_function)pick_info_fn, (jlist_item_paint_function)pick_paint_fn, (jwidget*)scene);
    jlist_update_model((::jlist*)sl->list, num_options, nullptr);
    jwidget_set_stretch((jwidget*)sl, 1, 1, false);
    jwidget* footer = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(footer); jwidget_set_fixed_height(footer, 45); jwidget_set_background(footer, t.key_spec);
    jbutton_create("Select", footer);
    int result = -1; bool running = true;
    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) { dclear(t.modal_bg); jscene_render(scene); dupdate(); }
        else if (e.type == JBUTTON_TRIGGERED) { result = jlist_selected_item((::jlist*)sl->list); running = false; }
        else if (e.type == JLIST_ITEM_TRIGGERED) { result = (int)e.data; running = false; }
        else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN && e.key.key == KEY_EXIT) { result = -1; running = false; }
    }
    jwidget_destroy((jwidget*)scene); return result;
}

bool ask(const char* title, const char* body, const char* ok_text, const char* cancel_text, ThemeName theme) {
    Theme const& t = get_theme(theme);
    jscene* scene = (jscene*)jscene_create_fullscreen(nullptr);
    jlayout_set_vbox((jwidget*)scene)->spacing = 0; jwidget_set_background((jwidget*)scene, t.modal_bg);
    jwidget* header = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(header); jwidget_set_fixed_height(header, 40); jwidget_set_background(header, t.accent);
    jlabel_set_text_color(jlabel_create(title, header), t.txt_acc);
    jwidget* body_cont = jwidget_create((jwidget*)scene);
    jlayout_set_vbox(body_cont); jwidget_set_stretch(body_cont, 1, 1, false);
    jlabel_set_text_color(jlabel_create(body, body_cont), t.txt);
    jwidget* footer = jwidget_create((jwidget*)scene);
    jlayout_set_hbox(footer); jwidget_set_fixed_height(footer, 45); jwidget_set_background(footer, t.key_spec);
    jbutton* btn_cancel = jbutton_create(cancel_text, footer), *btn_ok = jbutton_create(ok_text, footer);
    bool running = true, result = false;
    while (running) {
        jevent e = jscene_run(scene);
        if (e.type == JSCENE_PAINT) { dclear(t.modal_bg); jscene_render(scene); dupdate(); }
        else if (e.type == JBUTTON_TRIGGERED) { if (e.source == btn_ok) result = true; running = false; }
        else if (e.type == JWIDGET_KEY && e.key.type == KEYEV_DOWN && e.key.key == KEY_EXIT) running = false;
    }
    jwidget_destroy((jwidget*)scene); return result;
}

void init() { if (keyboard_type_id < 0) { keyboard_type_id = j_register_widget(&type_nkeyboard); NKEYBOARD_KEY_PRESSED = j_register_event(); } }

}
