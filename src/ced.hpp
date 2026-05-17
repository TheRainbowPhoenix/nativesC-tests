#pragma once

#include "ncinput.hpp"
#include <os/file.h>
#include <stddef.h>

namespace ced {

struct Config {
    char theme[16];
    bool word_wrap;
    int tab_size;
};

class Editor {
public:
    Editor();
    ~Editor();

    bool init();
    void run();

    bool load_config();
    bool load_file(const char* path);
    bool save_file(const char* path = nullptr);

    // Editing
    void insert_char(char c);
    void delete_char();
    void new_line();

private:
    Config m_config;
    char m_filename[256];
    bool m_modified;

    int m_cx, m_cy;
    int m_vx, m_vy;

    struct Line {
        uint32_t file_offset;
        uint16_t length;
        char* buffer; // Null if not modified
    };
    Line* m_lines;
    size_t m_line_count;
    size_t m_line_capacity;

    int m_fd;
    ncinput::Keyboard* m_keyboard;

    void clear_lines();
    bool add_line_info(uint32_t offset, uint16_t len);
    void handle_input();
    void render();

    char* get_line_text(int index);
};

} // namespace ced
