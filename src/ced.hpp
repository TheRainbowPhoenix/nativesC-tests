#pragma once

#ifdef swap
#undef swap
#endif

extern "C" {
#include <gint/display.h>
#include <gint/gint.h>
#include <gint/bfile.h>
#include "ncinput.h"
#include "search.h"
#include "outline.h"
#include "problems.h"
#include "goto.h"
#include "filebrowser.h"
}

namespace ced {

// RAII File Descriptor
class FileHandle {
public:
    FileHandle();
    ~FileHandle();

    bool open(const char* path, int mode);
    void close();
    int size();
    int read(void* data, int sz, int whence = -1);
    int write(const void* data, int sz);
    int seek(int offset);
    int get_fd() const { return fd; }

private:
    int fd;
};

class Editor {
public:
    Editor();
    ~Editor();

    void run();
    void load_config();
    void load_file(const char* path);
    void save_file(const char* path = nullptr);

    char* get_line(int index);
    int get_total_lines() const { return total_lines; }

private:
    void render();
    void do_menu();

    char filename[128];
    int cx, cy; // Cursor pos
    int vy; // Viewport Y (line index)
    int total_lines;

    // Offsets of each line start in file
    static const int MAX_LINES = 5000;
    int line_offsets[MAX_LINES];

    // MRU Cache for lines
    static const int CACHE_SIZE = 64;
    static const int MAX_LINE_LEN = 256;
    struct CachedLine {
        int index;
        char text[MAX_LINE_LEN];
        int last_used;
    } cache[CACHE_SIZE];

    int timer_ticks;

    nc_theme_name_t current_theme;
    bool word_wrap;

    struct Token {
        int start;
        int len;
        int color;
    };
    static const int TOKEN_CACHE_LINES = 25;
    static const int MAX_TOKENS_PER_LINE = 20;
    struct LineTokens {
        int index;
        Token tokens[MAX_TOKENS_PER_LINE];
        int count;
    } tok_cache[TOKEN_CACHE_LINES];

    void update_tokens(int line_idx, const char* line);
    void draw_line(int x, int y, int line_idx, const char* line);

    void* scene; // Opaque pointer to avoid JustUI header in C++
    bool running;
    FileHandle file;
};

} // namespace ced
