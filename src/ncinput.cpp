#include "ncinput.hpp"
#include "nui.hpp"
#include "nrender.hpp"
#include <os/lcd.h>
#include <os/input.h>
#include <os/mem.h>
#include <os/string.h>

namespace ncinput {

const Theme light_theme = { .modal_bg = 0xFFFF, .kbd_bg = 0xFFFF, .key_bg = 0xFFFF, .key_spec = 0xAD55, .key_out = 0, .txt = 0, .txt_dim = 0x8410, .accent = 0x001F, .txt_acc = 0xFFFF, .hl = 0xAD55, .check = 0xFFFF };
const Theme dark_theme = { .modal_bg = 0x2104, .kbd_bg = 0x2104, .key_bg = 0x2104, .key_spec = 0x4208, .key_out = 0x001F, .txt = 0xFFFF, .txt_dim = 0x8410, .accent = 0x001F, .txt_acc = 0xFFFF, .hl = 0x4208, .check = 0xFFFF };
const Theme grey_theme = { .modal_bg = 0xD69A, .kbd_bg = 0xD69A, .key_bg = 0xFFFF, .key_spec = 0xCE59, .key_out = 0, .txt = 0, .txt_dim = 0x8410, .accent = 0, .txt_acc = 0xFFFF, .hl = 0xCE59, .check = 0xFFFF };

const Theme& get_theme(const char* name) {
    if (name && String_Strcmp(name, "dark") == 0) return dark_theme;
    if (name && String_Strcmp(name, "grey") == 0) return grey_theme;
    return light_theme;
}

Keyboard::Keyboard(const Theme& theme, const char* layout) : m_theme(theme), m_visible(false), m_y(528 - 260), m_current_tab(0), m_shift(false), m_last_key(nullptr) { (void)layout; }

void Keyboard::draw_key(int x, int y, int w, int h, const char* label, bool is_spec, bool is_pressed, bool is_accent) {
    uint16_t bg = is_pressed ? m_theme.hl : (is_accent ? m_theme.accent : (is_spec ? m_theme.key_spec : m_theme.key_bg));
    nrender::fill_rect(x + 1, y + 1, x + w - 1, y + h - 1, bg);
    uint16_t border = m_theme.key_spec;
    for(int i=x; i<x+w; i++) { LCD_SetPixel(i, y, border); LCD_SetPixel(i, y+h-1, border); }
    for(int i=y; i<y+h; i++) { LCD_SetPixel(x, i, border); LCD_SetPixel(x+w-1, i, border); }
    if(label) {
        int tw = nrender::get_text_width(label, nrender::pSystemFont1);
        nrender::draw_text(x + w/2 - tw/2, y + h/2 - 8, label, is_accent ? m_theme.txt_acc : m_theme.txt, nrender::pSystemFont1);
    }
}

static const char* layouts_data[4] = { "1234567890", "qwertyuiop", "asdfghjkl:", "zxcvbnm,._" };

void Keyboard::draw() {
    if (!m_visible) return;
    nrender::fill_rect(0, m_y, 320, 528, m_theme.kbd_bg);
    int grid_y = m_y + 30; int row_h = 45;
    for (int r = 0; r < 4; r++) {
        int kw = 320 / 10;
        for (int c = 0; c < 10; c++) {
            char buf[2] = {layouts_data[r][c], '\0'};
            draw_key(c * kw, grid_y + r * row_h, kw, row_h, buf);
        }
    }
    draw_key(0, grid_y + 4*row_h, 60, row_h, "CAPS", true, m_shift);
    draw_key(60, grid_y + 4*row_h, 60, row_h, "<-", true);
    draw_key(120, grid_y + 4*row_h, 140, row_h, "Space");
    draw_key(260, grid_y + 4*row_h, 60, row_h, "EXE", false, false, true);
}

const char* Keyboard::handle_event(const struct Input_Event& ev) {
    if (ev.type != EVENT_TOUCH) return nullptr;
    if (ev.data.touch_single.direction == TOUCH_DOWN) {
        int tx = ev.data.touch_single.p1_x; int ty = ev.data.touch_single.p1_y;
        if (ty < m_y) return nullptr;
        int grid_y = m_y + 30; int row_h = 45;
        int row = (ty - grid_y) / row_h;
        if (row >= 0 && row < 4) {
            int col = tx / (320 / 10);
            if (col >= 0 && col < 10) {
                static char ret[2] = {0, 0}; ret[0] = layouts_data[row][col]; return ret;
            }
        } else if (row == 4) {
            if (tx < 60) { m_shift = !m_shift; return nullptr; }
            if (tx < 120) return "BACKSPACE";
            if (tx < 260) return " ";
            return "ENTER";
        }
    }
    return nullptr;
}

static char* local_strdup(const char* s) {
    if (!s) return nullptr;
    unsigned int len = String_Strlen(s);
    char* d = (char*)Mem_Malloc(len + 1);
    if (d) String_Strcpy(d, s);
    return d;
}

char* input(const char* prompt, InputType type, const char* theme_name, const char* layout) {
    (void)type; const Theme& theme = get_theme(theme_name); (void)layout;
    nui::NDialog dlg(nui::NDialog::Height95, prompt);
    nui::NTextBox tb(dlg.GetLeftX() + 10, dlg.GetTopY() + 40, 200, 256);
    tb.set_focused(true);
    dlg.AddElement(tb);
    Keyboard kbd(theme); kbd.set_visible(true);
    while (true) {
        nrender::fill_rect(0, 0, 320, 528, theme.modal_bg);
        nrender::fill_rect(20, dlg.GetTopY(), 300, 528, 0xEF7D);
        tb.render(); kbd.draw();
        LCD_Refresh();
        struct Input_Event ev;
        Mem_Memset(&ev, 0, sizeof(ev));
        if (GetInput(&ev, 0xFFFFFFFF, 0x10) == 0) {
            if (ev.type == EVENT_TOUCH) {
                if (ev.data.touch_single.p1_y >= kbd.get_y()) {
                    const char* res = kbd.handle_event(ev);
                    if (res) {
                        if (String_Strcmp(res, "BACKSPACE") == 0) tb.Backspace();
                        else if (String_Strcmp(res, "ENTER") == 0) return local_strdup(tb.GetText());
                        else tb.AppendChar(res[0]);
                    }
                } else {
                    tb.handle_touch(ev.data.touch_single.p1_x, ev.data.touch_single.p1_y, (int)ev.data.touch_single.direction);
                }
            }
            if (ev.type == EVENT_KEY) {
                if (ev.data.key.keyCode == KEYCODE_EXE) return local_strdup(tb.GetText());
                if (ev.data.key.keyCode == KEYCODE_POWER_CLEAR || ev.data.key.keyCode == KEYCODE_POWER) return nullptr;
                if (ev.data.key.keyCode == KEYCODE_BACKSPACE) tb.Backspace();
            }
        }
    }
}

int pick(const char** options, size_t count, const char* prompt, const char* theme_name) {
    (void)theme_name;
    nui::NDialog dlg(nui::NDialog::Height95, prompt);
    nui::NButton close_btn(10, 10, 45, 45, "X", 99);
    dlg.AddElement(close_btn);

    nui::NButton* btns[10];
    for (int i = 0; i < 10; i++) btns[i] = nullptr;
    int n = (int)count; if (n > 10) n = 10;
    for (int i = 0; i < n; i++) {
        btns[i] = new nui::NButton(40, 60 + i * 45, 280, 100 + i * 45, options[i], i);
        dlg.AddElement(*btns[i]);
    }

    int result = -1;
    while (true) {
        nrender::fill_rect(0, 0, 320, 528, 0xEF7D);
        nrender::fill_rect(0, 0, 320, 50, 0x001F);
        close_btn.render();
        int tw = nrender::get_text_width(prompt, nrender::pSystemFont1);
        nrender::draw_text(160 - tw/2, 15, prompt, 0xFFFF, nrender::pSystemFont1);
        for (int i = 0; i < n; i++) if(btns[i]) btns[i]->render();
        LCD_Refresh();
        struct Input_Event ev;
        Mem_Memset(&ev, 0, sizeof(ev));
        if (GetInput(&ev, 0xFFFFFFFF, 0x10) == 0) {
            if (ev.type == EVENT_TOUCH) {
                int tx = ev.data.touch_single.p1_x; int ty = ev.data.touch_single.p1_y;
                int act = (int)ev.data.touch_single.direction;
                close_btn.handle_touch(tx, ty, act);
                for (int i = 0; i < n; i++) if(btns[i]) btns[i]->handle_touch(tx, ty, act);
            }
            if (ev.type == EVENT_KEY && (ev.data.key.keyCode == KEYCODE_POWER_CLEAR || ev.data.key.keyCode == KEYCODE_POWER)) break;
            if (close_btn.is_clicked()) break;
            for (int i = 0; i < n; i++) {
                if (btns[i] && btns[i]->is_clicked()) { result = i; goto done; }
            }
        }
    }
done:
    for (int i = 0; i < n; i++) if(btns[i]) delete btns[i];
    return result;
}

bool ask(const char* title, const char* body, const char* ok_text, const char* cancel_text, const char* theme_name) {
    (void)theme_name;
    nui::NDialog dlg(nui::NDialog::Height35, title);
    nui::NLabel label(dlg.GetLeftX() + 10, dlg.GetTopY() + 10, body);
    nui::NButton ok_btn(dlg.GetLeftX() + 100, dlg.GetTopY() + 40, dlg.GetLeftX() + 160, dlg.GetTopY() + 65, ok_text, 1);
    nui::NButton cancel_btn(dlg.GetLeftX() + 20, dlg.GetTopY() + 40, dlg.GetLeftX() + 80, dlg.GetTopY() + 65, cancel_text, 2);
    dlg.AddElement(label); dlg.AddElement(ok_btn); dlg.AddElement(cancel_btn);
    return (dlg.ShowDialog() == nui::NDialog::DialogResultOK);
}

} // namespace ncinput
