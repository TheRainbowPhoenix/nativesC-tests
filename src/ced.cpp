#include "ced.hpp"
#include <os/file.h>
#include <os/mem.h>
#include <os/lcd.h>
#include <os/input.h>
#include <os/string.h>

namespace ced {

static void fill_rect(int x1, int y1, int x2, int y2, uint16_t color) {
    uint16_t* v_addr = LCD_GetVRAMAddress();
    unsigned int sw, sh; LCD_GetSize(&sw, &sh);
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 > (int)sw) x2 = sw;
    if (y2 > (int)sh) y2 = sh;
    for (int y = y1; y < y2; y++) {
        uint16_t* row = v_addr + y * sw;
        for (int x = x1; x < x2; x++) row[x] = color;
    }
}

Editor::Editor() : m_modified(false), m_cx(0), m_cy(0), m_vx(0), m_vy(0), m_lines(nullptr), m_line_count(0), m_line_capacity(0), m_fd(-1), m_keyboard(nullptr) {
    String_Strcpy(m_filename, "untitled.py");
    m_config.word_wrap = false; m_config.tab_size = 4;
    String_Strcpy(m_config.theme, "light");
}

Editor::~Editor() { clear_lines(); if (m_fd >= 0) File_Close(m_fd); if (m_keyboard) delete m_keyboard; }

bool Editor::init() {
    load_config();
    const ncinput::Theme& theme = ncinput::get_theme(m_config.theme);
    m_keyboard = new ncinput::Keyboard(theme);
    if (m_line_count == 0) add_line_info(0, 0);
    return true;
}

bool Editor::load_config() {
    int fd = File_Open("\\\\fls0\\.ced", FILE_OPEN_READ);
    if (fd < 0) return false;
    char buffer[256]; int bytes = File_Read(fd, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        // (Simplified parser as strtok is missing)
    }
    File_Close(fd); return true;
}

void Editor::clear_lines() { Mem_Free(m_lines); m_lines = nullptr; m_line_count = 0; m_line_capacity = 0; }

bool Editor::add_line_info(uint32_t offset, uint16_t len) {
    if (m_line_count >= m_line_capacity) {
        size_t new_cap = m_line_capacity == 0 ? 128 : m_line_capacity * 2;
        Line* new_lines = (Line*)Mem_Malloc(new_cap * sizeof(Line));
        if (!new_lines) return false;
        if (m_lines) {
            Mem_Memcpy(new_lines, m_lines, m_line_count * sizeof(Line));
            Mem_Free(m_lines);
        }
        m_lines = new_lines; m_line_capacity = new_cap;
    }
    m_lines[m_line_count].file_offset = offset;
    m_lines[m_line_count].length = len;
    m_line_count++; return true;
}

bool Editor::load_file(const char* path) {
    if (m_fd >= 0) File_Close(m_fd);
    m_fd = File_Open(path, FILE_OPEN_READ);
    if (m_fd < 0) return false;
    String_Strcpy(m_filename, path);
    clear_lines();
    char buffer[1024]; int bytes; uint32_t current_offset = 0; uint32_t line_start = 0;
    while ((bytes = File_Read(m_fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes; i++) {
            if (buffer[i] == '\n') {
                add_line_info(line_start, (uint16_t)(current_offset + i - line_start));
                line_start = current_offset + i + 1;
            }
        }
        current_offset += bytes;
    }
    if (current_offset > line_start) add_line_info(line_start, (uint16_t)(current_offset - line_start));
    if (m_line_count == 0) add_line_info(0, 0);
    m_modified = false; m_cx = m_cy = 0; m_vx = m_vy = 0;
    return true;
}

char* Editor::get_line_text(int index) {
    if (index < 0 || (size_t)index >= m_line_count || m_fd < 0) return nullptr;
    static char line_cache[1024];
    uint16_t len = m_lines[index].length;
    if (len > 1023) len = 1023;
    (void)File_Lseek(m_fd, m_lines[index].file_offset, FILE_SEEK_SET);
    (void)File_Read(m_fd, line_cache, len);
    line_cache[len] = '\0';
    return line_cache;
}

bool Editor::save_file(const char* path) { (void)path; return false; }

void Editor::render() {
    const ncinput::Theme& theme = ncinput::get_theme(m_config.theme);
    fill_rect(0, 40, 320, 528, theme.modal_bg);
    fill_rect(0, 0, 320, 40, theme.accent);
    int start_y = 45; int line_h = 20;
    for (size_t i = m_vy; i < m_line_count && (start_y + line_h < 528); i++) {
        if (i == (size_t)m_cy) {
            int cursor_x = 5 + m_cx * 8;
            fill_rect(cursor_x, start_y, cursor_x + 2, start_y + line_h - 2, theme.txt);
        }
        start_y += line_h;
    }
    if (m_keyboard->is_visible()) m_keyboard->draw();
    LCD_Refresh();
}

void Editor::handle_input() {
    struct Input_Event ev;
    Mem_Memset(&ev, 0, sizeof(ev));
    if (GetInput(&ev, 0, 0x10) != 0 || ev.type == EVENT_NONE) return;
    if (ev.type == EVENT_KEY && ev.data.key.direction == KEY_PRESSED) {
        switch (ev.data.key.keyCode) {
            case KEYCODE_UP: if (m_cy > 0) m_cy--; break;
            case KEYCODE_DOWN: if (m_cy < (int)m_line_count - 1) m_cy++; break;
            case KEYCODE_LEFT: if (m_cx > 0) m_cx--; break;
            case KEYCODE_RIGHT: if (m_cx < (int)m_lines[m_cy].length) m_cx++; break;
            case KEYCODE_KEYBOARD: m_keyboard->set_visible(!m_keyboard->is_visible()); break;
            default: break;
        }
    }
}

void Editor::run() {
    while (true) {
        render(); handle_input();
        struct Input_Event ev;
        Mem_Memset(&ev, 0, sizeof(ev));
        if (GetInput(&ev, 0, 0x10) == 0 && ev.type == EVENT_KEY && ev.data.key.keyCode == KEYCODE_POWER_CLEAR) break;
    }
}

} // namespace ced
