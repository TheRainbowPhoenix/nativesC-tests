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
    if(label) nrender::draw_text(x + 5, y + 5, label, is_accent ? m_theme.txt_acc : m_theme.txt, nrender::pSystemFont1);
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
}

const char* Keyboard::update() {
    struct Input_Event ev;
    Mem_Memset(&ev, 0, sizeof(ev));
    if (GetInput(&ev, 0, 0x10) != 0 || ev.type != EVENT_TOUCH) return nullptr;
    if (ev.data.touch_single.direction == TOUCH_DOWN) {
        int tx = ev.data.touch_single.p1_x;
        int ty = ev.data.touch_single.p1_y;
        if (ty < m_y) return nullptr;
        int grid_y = m_y + 30; int row_h = 45;
        int row = (ty - grid_y) / row_h; int col = tx / (320 / 10);
        if (row >= 0 && row < 4 && col >= 0 && col < 10) {
            static char ret[2] = {0, 0}; ret[0] = layouts_data[row][col]; return ret;
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

    Keyboard kbd(theme);
    kbd.set_visible(true);

    while (true) {
        nrender::fill_rect(20, dlg.GetTopY(), 300, 528, 0xEF7D);
        tb.render();
        kbd.draw();
        LCD_Refresh();

        struct Input_Event ev;
        if (GetInput(&ev, 0xFFFFFFFF, 0x10) == 0) {
            if (ev.type == EVENT_TOUCH) {
                if (ev.data.touch_single.p1_y >= kbd.get_y()) {
                    const char* res = kbd.update();
                    if (res) tb.AppendChar(res[0]);
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
    (void)options; (void)count; (void)theme_name;
    nui::NDialog dlg(nui::NDialog::Height95, prompt);
    if (dlg.ShowDialog() == nui::NDialog::DialogResultOK) return 0;
    return -1;
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
