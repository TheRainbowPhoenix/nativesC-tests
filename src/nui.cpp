#include "nui.hpp"
#include <os/lcd.h>
#include <os/input.h>
#include <os/mem.h>
#include <os/string.h>

namespace nui {

NLabel::NLabel(int x, int y, const char* text) : m_x(x), m_y(y), m_text(text) {}
void NLabel::render() {
    nrender::draw_text(m_x, m_y, m_text, 0, nrender::pSystemFont1);
}

NButton::NButton(int x1, int y1, int x2, int y2, const char* text, int id) : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2), m_text(text), m_id(id), m_pressed(false), m_clicked(false) { (void)m_id; }
void NButton::render() {
    uint16_t bg = m_pressed ? 0xAD55 : 0xFFFF;
    nrender::fill_rect(m_x1, m_y1, m_x2, m_y2, bg);
    nrender::draw_text(m_x1 + 5, m_y1 + 5, m_text, 0, nrender::pSystemFont1);
}
void NButton::handle_touch(int tx, int ty, int action) {
    if (tx >= m_x1 && tx < m_x2 && ty >= m_y1 && ty < m_y2) {
        if (action == TOUCH_DOWN) m_pressed = true;
        else if (action == TOUCH_UP && m_pressed) { m_pressed = false; m_clicked = true; }
    } else if (action == TOUCH_UP) m_pressed = false;
}

NTextBox::NTextBox(int x, int y, int w, int maxLen) : m_x(x), m_y(y), m_w(w), m_maxLen(maxLen) { m_buffer[0] = '\0'; (void)m_maxLen; }
void NTextBox::render() {
    nrender::fill_rect(m_x, m_y, m_x + m_w, m_y + 25, 0xFFFF);
    nrender::draw_text(m_x + 5, m_y + 5, m_buffer, 0, nrender::pSystemFont1);
}
void NTextBox::handle_touch(int tx, int ty, int action) { (void)tx; (void)ty; (void)action; }
void NTextBox::SetText(const char* text) { if(text) String_Strcpy(m_buffer, text); }

NDialog::NDialog(Height h, const char* title) : m_title(title), m_count(0) {
    (void)m_title;
    m_x1 = 20; m_x2 = 300;
    int h_px = 100;
    if (h == Height25) h_px = 130;
    else if (h == Height95) h_px = 480;
    m_y1 = (528 - h_px) / 2; m_y2 = m_y1 + h_px;
}
NDialog::~NDialog() {}
void NDialog::AddElement(NElement& el) { if (m_count < 10) m_elements[m_count++] = &el; }
NDialog::Result NDialog::ShowDialog() {
    while (true) {
        nrender::fill_rect(m_x1, m_y1, m_x2, m_y2, 0xEF7D);
        nrender::fill_rect(m_x1, m_y1, m_x2, m_y1 + 30, 0x001F);
        for(int i=0; i<m_count; i++) m_elements[i]->render();
        LCD_Refresh();
        struct Input_Event ev;
        if (GetInput(&ev, 0xFFFFFFFF, 0x10) == 0) {
            if (ev.type == EVENT_TOUCH) {
                int tx = ev.data.touch_single.p1_x;
                int ty = ev.data.touch_single.p1_y;
                int act = (int)ev.data.touch_single.direction;
                for(int i=0; i<m_count; i++) m_elements[i]->handle_touch(tx, ty, act);
            }
            if (ev.type == EVENT_KEY && ev.data.key.keyCode == KEYCODE_EXE) return DialogResultOK;
            if (ev.type == EVENT_KEY && (ev.data.key.keyCode == KEYCODE_POWER_CLEAR || ev.data.key.keyCode == KEYCODE_POWER)) return DialogResultCancel;
            for(int i=0; i<m_count; i++) {
                if (m_elements[i]->get_type() == TYPE_BUTTON) {
                    NButton* b = static_cast<NButton*>(m_elements[i]);
                    if (b->is_clicked()) { b->reset_clicked(); return DialogResultOK; }
                }
            }
        }
    }
}
} // namespace nui
