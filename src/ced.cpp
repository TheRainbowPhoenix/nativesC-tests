#include "ced.hpp"
#include <cstring>
#include <cstdio>
#include <cctype>

namespace ced {

static char* search_cb(void* editor, int index) {
    return ((Editor*)editor)->get_line(index);
}

Editor::Editor() : total_lines(0), vy(0), cx(0), cy(0), current_theme(NC_THEME_LIGHT), word_wrap(false), running(true) {
    filename[0] = '\0';
    for (int i = 0; i < CACHE_SIZE; i++) cache[i].index = -1;
    for (int i = 0; i < TOKEN_CACHE_LINES; i++) tok_cache[i].index = -1;
    timer_ticks = 0;
}

Editor::~Editor() {}

void Editor::load_config() {
    FileHandle cfg;
    if (cfg.open("\\\\fls0\\.ced", BFile_ReadOnly)) {
        char buf[256]; int r = cfg.read(buf, 255);
        if (r > 0) {
            buf[r] = '\0';
            if (strstr(buf, "theme=dark")) current_theme = NC_THEME_DARK;
            else if (strstr(buf, "theme=grey")) current_theme = NC_THEME_GREY;
            if (strstr(buf, "wrap=true")) word_wrap = true;
        }
    }
}

void Editor::load_file(const char* path) {
    if (!file.open(path, BFile_ReadOnly)) {
        nc_ask("Error", "Could not open file.", "OK", "Cancel", current_theme);
        return;
    }
    strncpy(filename, path, sizeof(filename)-1); filename[sizeof(filename)-1] = '\0';
    total_lines = 0; line_offsets[0] = 0; total_lines = 1;
    char buf[1024]; int bytes_read; int current_pos = 0;
    while ((bytes_read = file.read(buf, sizeof(buf), current_pos)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') { if (total_lines < MAX_LINES) line_offsets[total_lines++] = current_pos + i + 1; }
        }
        current_pos += bytes_read;
    }
    vy = 0; cx = 0; cy = 0;
    for (int i = 0; i < CACHE_SIZE; i++) cache[i].index = -1;
    for (int i = 0; i < TOKEN_CACHE_LINES; i++) tok_cache[i].index = -1;
}

char* Editor::get_line(int index) {
    if (index < 0 || index >= total_lines) return nullptr;
    for (int i = 0; i < CACHE_SIZE; i++) { if (cache[i].index == index) { cache[i].last_used = ++timer_ticks; return cache[i].text; } }
    int oldest = 0; for (int i = 0; i < CACHE_SIZE; i++) { if (cache[i].last_used < cache[oldest].last_used) oldest = i; }
    int offset = line_offsets[index];
    int next_offset = (index + 1 < total_lines) ? line_offsets[index + 1] : file.size();
    int len = next_offset - offset; if (len >= MAX_LINE_LEN) len = MAX_LINE_LEN - 1;
    cache[oldest].index = index; cache[oldest].last_used = ++timer_ticks;
    int r = file.read(cache[oldest].text, len, offset); if (r < 0) r = 0;
    cache[oldest].text[r] = '\0';
    for (int i = 0; i < r; i++) { if (cache[oldest].text[i] == '\r' || cache[oldest].text[i] == '\n') { cache[oldest].text[i] = '\0'; break; } }
    return cache[oldest].text;
}

static bool is_py_keyword(const char* s, int len) {
    static const char* kw[] = {"def", "class", "if", "else", "elif", "for", "while", "return", "import", "from", "as", "try", "except", "finally", "with", "pass", "break", "continue", "None", "True", "False"};
    for (size_t i = 0; i < sizeof(kw)/sizeof(kw[0]); i++) { if ((int)strlen(kw[i]) == len && strncmp(s, kw[i], len) == 0) return true; }
    return false;
}

void Editor::update_tokens(int line_idx, const char* line) {
    int cache_idx = line_idx % TOKEN_CACHE_LINES; if (tok_cache[cache_idx].index == line_idx) return;
    tok_cache[cache_idx].index = line_idx; tok_cache[cache_idx].count = 0;
    int i = 0;
    while (line[i] && tok_cache[cache_idx].count < MAX_TOKENS_PER_LINE) {
        while (line[i] && isspace(line[i])) i++; if (!line[i]) break;
        int start = i; int color = C_BLACK;
        if (line[i] == '#') { tok_cache[cache_idx].tokens[tok_cache[cache_idx].count++] = {start, (int)strlen(line + start), C_RGB(0, 15, 0)}; break; }
        else if (line[i] == '"' || line[i] == '\'') { char quote = line[i++]; while (line[i] && line[i] != quote) { if (line[i] == '\\' && line[i+1]) i++; i++; } if (line[i]) i++; color = C_RGB(15, 8, 0); }
        else if (isdigit(line[i])) { while (line[i] && (isdigit(line[i]) || line[i] == '.')) i++; color = C_RGB(0, 0, 15); }
        else if (isalpha(line[i]) || line[i] == '_') { while (line[i] && (isalnum(line[i]) || line[i] == '_')) i++; if (is_py_keyword(line + start, i - start)) color = C_RGB(15, 0, 15); else color = C_BLACK; }
        else { i++; color = C_BLACK; }
        tok_cache[cache_idx].tokens[tok_cache[cache_idx].count++] = {start, i - start, color};
    }
}

void Editor::draw_line(int x, int y, int line_idx, const char* line) {
    update_tokens(line_idx, line); int cache_idx = line_idx % TOKEN_CACHE_LINES;
    int cur_x = x, last_pos = 0;
    for (int i = 0; i < tok_cache[cache_idx].count; i++) {
        Token& t = tok_cache[cache_idx].tokens[i];
        if (t.start > last_pos) { char tmp[MAX_LINE_LEN]; int len = t.start - last_pos; strncpy(tmp, line + last_pos, len); tmp[len] = '\0'; dtext(cur_x, y, C_BLACK, tmp); int w; dsize(tmp, NULL, &w, NULL); cur_x += w; }
        char tmp[MAX_LINE_LEN]; strncpy(tmp, line + t.start, t.len); tmp[t.len] = '\0'; dtext(cur_x, y, t.color, tmp); int w; dsize(tmp, NULL, &w, NULL); cur_x += w;
        last_pos = t.start + t.len;
    }
    if (line[last_pos]) dtext(cur_x, y, C_BLACK, line + last_pos);
}

void Editor::render() {
    nc_theme_t const* t = nc_get_theme(current_theme); dclear(t->modal_bg); int y = 5;
    for (int i = vy; i < vy + 12 && i < total_lines; i++) {
        const char* line = get_line(i); if (!line) line = "";
        if (i == cy) drect(0, y, 320, y + 18, t->hl);
        draw_line(5, y, i, line);
        if (i == cy) {
            int cw = 0; if (cx > 0) { char tmp[MAX_LINE_LEN]; int len = (cx < (int)strlen(line)) ? cx : (int)strlen(line); strncpy(tmp, line, len); tmp[len] = '\0'; dnsize(tmp, len, NULL, &cw, NULL); }
            drect(5 + cw, y, 5 + cw + 1, y + 18, t->txt);
        }
        y += 20;
    }
}

void Editor::do_menu() {
    const char* options[] = {"Open File", "Save File", "Search", "Go To", "Outline", "Problems", "Exit"};
    int choice = nc_pick(options, 7, "Menu", current_theme);
    switch (choice) {
        case 0: { char path[128]; if (filebrowser_show(path, current_theme)) load_file(path); break; }
        case 1: save_file(); break;
        case 2: search_show(this, search_cb, current_theme); break;
        case 3: { int target = goto_line_show(total_lines, current_theme); if (target >= 0) { cy = target; if (cy < vy) vy = cy; if (cy >= vy + 12) vy = cy - 11; } break; }
        case 4: { int target = outline_show(filename, current_theme); if (target >= 0) { cy = target; if (cy < vy) vy = cy; if (cy >= vy + 12) vy = cy - 11; } break; }
        case 5: problems_show(current_theme); break;
        case 6: running = false; break;
    }
}

void Editor::run() {
    while (running) {
        render(); dupdate();
        key_event_t e = getkey();
        if (e.key == KEY_MENU) do_menu();
        else if (e.key == KEY_UP) { if (cy > 0) cy--; if (cy < vy) vy = cy; }
        else if (e.key == KEY_DOWN) { if (cy < total_lines - 1) cy++; if (cy >= vy + 12) vy++; }
        else if (e.key == KEY_LEFT) { if (cx > 0) cx--; }
        else if (e.key == KEY_RIGHT) { const char* l = get_line(cy); if (l && cx < (int)strlen(l)) cx++; }
        else if (e.key == KEY_EXIT) running = false;
    }
}

void Editor::save_file(const char* path) {
    if (strlen(filename) == 0 && path == nullptr) { nc_ask("Error", "No file to save.", "OK", "Cancel", current_theme); return; }
    nc_ask("Save", "Save is limited to rewriting existing files on Fugue FS.", "OK", "Cancel", current_theme);
}

FileHandle::FileHandle() : fd(-1) {}
FileHandle::~FileHandle() { close(); }
bool FileHandle::open(const char* path, int mode) { close(); uint16_t os_path[128]; nc_to_os_path(path, os_path, 128); gint_world_switch(); fd = BFile_Open(os_path, mode); gint_world_switch(); return fd >= 0; }
void FileHandle::close() { if (fd >= 0) { gint_world_switch(); BFile_Close(fd); gint_world_switch(); fd = -1; } }
int FileHandle::size() { if (fd < 0) return -1; gint_world_switch(); int s = BFile_Size(fd); gint_world_switch(); return s; }
int FileHandle::read(void* data, int sz, int whence) { if (fd < 0) return -1; gint_world_switch(); int r = BFile_Read(fd, data, sz, whence); gint_world_switch(); return r; }
int FileHandle::write(const void* data, int sz) { if (fd < 0) return -1; gint_world_switch(); int r = BFile_Write(fd, data, sz); gint_world_switch(); return r; }
int FileHandle::seek(int offset) { if (fd < 0) return -1; gint_world_switch(); int r = BFile_Seek(fd, offset); gint_world_switch(); return r; }

}
