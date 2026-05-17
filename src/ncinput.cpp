#include "ncinput.hpp"
#include <os/lcd.h>
#include <os/input.h>
#include <os/mem.h>
#include <os/string.h>
#include <os/gui.hpp>

namespace ncinput {

static inline uint16_t safe_rgb(int r, int g, int b) {
    return RGB_TO_RGB565(r, g, b);
}

const Theme light_theme = {
    .modal_bg = safe_rgb(31, 63, 31),
    .kbd_bg = safe_rgb(31, 63, 31),
    .key_bg = safe_rgb(31, 63, 31),
    .key_spec = safe_rgb(28, 58, 28),
    .key_out = safe_rgb(0, 0, 0),
    .txt = safe_rgb(4, 8, 4),
    .txt_dim = safe_rgb(8, 16, 8),
    .accent = safe_rgb(1, 22, 26),
    .txt_acc = safe_rgb(31, 63, 31),
    .hl = safe_rgb(28, 58, 28),
    .check = safe_rgb(31, 63, 31)
};

const Theme dark_theme = {
    .modal_bg = safe_rgb(7, 14, 8),
    .kbd_bg = safe_rgb(7, 14, 8),
    .key_bg = safe_rgb(7, 14, 8),
    .key_spec = safe_rgb(11, 22, 12),
    .key_out = safe_rgb(12, 38, 31),
    .txt = safe_rgb(31, 63, 31),
    .txt_dim = safe_rgb(8, 16, 8),
    .accent = safe_rgb(12, 38, 31),
    .txt_acc = safe_rgb(31, 63, 31),
    .hl = safe_rgb(11, 22, 12),
    .check = safe_rgb(31, 63, 31)
};

const Theme grey_theme = {
    .modal_bg = safe_rgb(24, 48, 24),
    .kbd_bg = safe_rgb(24, 48, 24),
    .key_bg = safe_rgb(31, 63, 31),
    .key_spec = 0xCE59,
    .key_out = safe_rgb(0, 0, 0),
    .txt = safe_rgb(0, 0, 0),
    .txt_dim = safe_rgb(8, 16, 8),
    .accent = safe_rgb(0, 0, 0),
    .txt_acc = safe_rgb(31, 63, 31),
    .hl = 0xCE59,
    .check = safe_rgb(31, 63, 31)
};

const Theme& get_theme(const char* name) {
    if (name && String_Strcmp(name, "dark") == 0) return dark_theme;
    if (name && String_Strcmp(name, "grey") == 0) return grey_theme;
    return light_theme;
}

static void draw_rect(int x1, int y1, int x2, int y2, uint16_t color) {
    uint16_t* vram_addr = LCD_GetVRAMAddress();
    unsigned int sw, sh; LCD_GetSize(&sw, &sh);
    for (int y = y1; y < y2; y++) {
        if (y < 0 || (unsigned int)y >= sh) continue;
        for (int x = x1; x < x2; x++) {
            if (x < 0 || (unsigned int)x >= sw) continue;
            vram_addr[y * sw + x] = color;
        }
    }
}

Keyboard::Keyboard(const Theme& theme, const char* layout) : m_theme(theme), m_visible(false), m_y(528 - 260), m_current_tab(0), m_shift(false), m_last_key(nullptr) { (void)layout; }

void Keyboard::draw_key(int x, int y, int w, int h, const char* label, bool is_spec, bool is_pressed, bool is_accent) {
    (void)label;
    uint16_t bg = is_pressed ? m_theme.hl : (is_accent ? m_theme.accent : (is_spec ? m_theme.key_spec : m_theme.key_bg));
    draw_rect(x + 1, y + 1, x + w - 1, y + h - 1, bg);
}

static const char* layouts_data[4] = {
    "1234567890",
    "qwertyuiop",
    "asdfghjkl:",
    "zxcvbnm,._"
};

void Keyboard::draw() {
    if (!m_visible) return;
    draw_rect(0, m_y, 320, 528, m_theme.kbd_bg);
    int grid_y = m_y + 30;
    int row_h = 45;
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
        int grid_y = m_y + 30;
        int row_h = 45;
        int row = (ty - grid_y) / row_h;
        int col = tx / (320 / 10);
        if (row >= 0 && row < 4 && col >= 0 && col < 10) {
            static char ret[2] = {0, 0};
            ret[0] = layouts_data[row][col];
            return ret;
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
    (void)theme_name; (void)layout;
    GUIDialog::Height h = GUIDialog::Height25;
    GUIDialog::KeyboardState ks = GUIDialog::KeyboardStateABC;
    if (type == InputType::NumericInt || type == InputType::NumericFloat) ks = GUIDialog::KeyboardStateNumber;
    else if (type == InputType::Math) ks = GUIDialog::KeyboardStateMath1;
    GUIDialog dlg(h, GUIDialog::AlignCenter, prompt, ks);
    GUITextBox tb(dlg.GetLeftX() + 10, dlg.GetTopY() + 40, 200, 256, false);
    dlg.AddElement(tb);
    if (dlg.ShowDialog() == GUIDialog::DialogResultOK) {
        return local_strdup(tb.GetText());
    }
    return nullptr;
}

int pick(const char** options, size_t count, const char* prompt, const char* theme_name) {
    (void)options; (void)count; (void)theme_name;
    GUIDialog dlg(GUIDialog::Height95, GUIDialog::AlignCenter, prompt, GUIDialog::KeyboardStateNone);
    // Placeholder as GUIDropDownMenu requires adding items individually and event handling
    if (dlg.ShowDialog() == GUIDialog::DialogResultOK) return 0;
    return -1;
}

bool ask(const char* title, const char* body, const char* ok_text, const char* cancel_text, const char* theme_name) {
    (void)theme_name;
    GUIDialog dlg(GUIDialog::Height35, GUIDialog::AlignCenter, title, GUIDialog::KeyboardStateNone);
    GUILabel label(dlg.GetLeftX() + 10, dlg.GetTopY() + 10, body);
    GUIButton ok_btn(dlg.GetLeftX() + 100, dlg.GetTopY() + 40, 60, 25, ok_text, 1);
    GUIButton cancel_btn(dlg.GetLeftX() + 20, dlg.GetTopY() + 40, 60, 25, cancel_text, 2);
    dlg.AddElement(label);
    dlg.AddElement(ok_btn);
    dlg.AddElement(cancel_btn);
    return (dlg.ShowDialog() == GUIDialog::DialogResultOK);
}

} // namespace ncinput
