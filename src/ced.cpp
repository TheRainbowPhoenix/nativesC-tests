#include "ced.hpp"
#include "ncinput.hpp"
#include "filebrowser.hpp"
#include "goto.hpp"
#include "search.hpp"
#include "outline.hpp"
#include "problems.hpp"
#include <justui/jlayout.h>
#include <justui/jlabel.h>
#include <justui/jbutton.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdio>

#ifndef C_MAGENTA
#define C_MAGENTA 0xf81f
#endif

extern "C" {
    void __cxa_pure_virtual() { while (1); }
    void __cxa_guard_acquire(void*) { }
    void __cxa_guard_release(void*) { }
    void __cxa_guard_abort(void*) { }

    void _free(void* ptr) { free(ptr); }
    void* _malloc(size_t sz) { return malloc(sz); }
    void* _realloc(void* ptr, size_t sz) { return realloc(ptr, sz); }

    void __ZSt20__throw_length_errorPKc(char const*) { while(1); }
    void __ZSt19__throw_logic_errorPKc(char const*) { while(1); }
    void __ZSt20__throw_out_of_rangePKc(char const*) { while(1); }
    void __ZSt24__throw_out_of_range_fmtPKcz(char const*, ...) { while(1); }
    void __ZSt28__throw_bad_array_new_lengthv() { while(1); }
}

void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* ptr) noexcept { free(ptr); }
void operator delete[](void* ptr) noexcept { free(ptr); }
void operator delete(void* ptr, size_t) noexcept { free(ptr); }
void operator delete[](void* ptr, size_t) noexcept { free(ptr); }

namespace ced {

Editor::Editor() : filename("untitled.py"), cx(0), cy(0), vy(0), total_lines(1), current_theme(ncinput::ThemeName::Light), word_wrap(false), running(true) {
    scene = jscene_create_fullscreen(nullptr);
}

Editor::~Editor() {
    jwidget_destroy((jwidget*)scene);
}

void Editor::load_config() {
    std::ifstream f(".ced");
    if (f.is_open()) {
        std::string line;
        while (std::getline(f, line)) {
            if (line == "theme=dark") current_theme = ncinput::ThemeName::Dark;
            else if (line == "theme=grey") current_theme = ncinput::ThemeName::Grey;
            else if (line == "wrap=on") word_wrap = true;
        }
    }
}

void Editor::load_file(std::string const& path) {
    if (file_handle.is_open()) file_handle.close();
    file_handle.open(path, std::ios::binary);
    if (file_handle.is_open()) {
        filename = path;
        total_lines = 0;
        line_offsets.clear();
        lines.clear();
        line_loaded.clear();
        loaded_indices.clear();
        token_cache.clear();
        std::string line;
        while (true) {
            line_offsets.push_back(file_handle.tellg());
            if (!std::getline(file_handle, line)) break;
            lines.push_back("");
            line_loaded.push_back(false);
            total_lines++;
        }
        if (total_lines == 0) {
            total_lines = 1;
            line_offsets.push_back(0);
            lines.push_back("");
            line_loaded.push_back(true);
        }
        cx = cy = vy = 0;
    }
}

static std::string EMPTY_LINE = "";
std::string& Editor::get_line(int index) {
    if (index < 0 || index >= total_lines) return EMPTY_LINE;
    if (line_loaded[index]) return lines[index];

    file_handle.clear();
    file_handle.seekg(line_offsets[index]);
    std::getline(file_handle, lines[index]);
    line_loaded[index] = true;
    loaded_indices.push_back(index);

    // Evict old lines if there are too many loaded (O(1)ish eviction)
    if (loaded_indices.size() > 500) {
        size_t to_evict = loaded_indices.size() - 400;
        for (size_t k = 0; k < to_evict; ++k) {
            int idx = loaded_indices[k];
            // Don't evict what's near the viewport
            if (idx < vy - 50 || idx > vy + 50) {
                lines[idx].clear();
                lines[idx].shrink_to_fit();
                line_loaded[idx] = false;
                token_cache.erase(idx);
            }
        }
        loaded_indices.erase(loaded_indices.begin(), loaded_indices.begin() + to_evict);
    }

    return lines[index];
}

void Editor::save_file(std::string const& path) {
    std::string target = path.empty() ? filename : path;
    bool is_save_as = !path.empty() || target == "untitled.py";

    if (target == "untitled.py") {
        target = ncinput::input("Save as:", "alpha_numeric", current_theme);
        if (target.empty()) return;
    }

    if (is_save_as) {
        std::ifstream check(target);
        if (check.good()) {
            check.close();
            if (!ncinput::ask("Overwrite?", "File already exists. Overwrite?", "Yes", "No", current_theme)) {
                return;
            }
        }
    }

    std::string temp_target = target + ".tmp";
    std::ofstream f(temp_target, std::ios::binary);
    if (f.is_open()) {
        for (int i = 0; i < total_lines; ++i) {
            f << get_line(i) << "\n";
        }
        f.close();

        // Close source handle before renaming
        if (file_handle.is_open()) file_handle.close();

        std::remove(target.c_str());
        std::rename(temp_target.c_str(), target.c_str());

        filename = target;
        load_file(filename); // Re-initialize with new offsets
    }
}

void Editor::do_menu() {
    std::vector<std::string> opts = {
        "New", "Open", "Save", "Save As",
        "Search", "Replace", "Go To", "Outline", "Problems",
        "Theme", "Quit"
    };
    std::string choice = ncinput::pick(opts, "Menu", current_theme);
    if (choice == "New") {
        filename = "untitled.py"; total_lines = 1; cx = cy = vy = 0;
    } else if (choice == "Open") {
        std::string path = filebrowser::browse("Open File", current_theme);
        if (!path.empty()) load_file(path);
    } else if (choice == "Save") {
        save_file();
    } else if (choice == "Save As") {
        save_file("");
    } else if (choice == "Search") {
        SearchResult res = show_search(filename, current_theme);
        if (res.line != -1) {
            cy = res.line; cx = res.col;
            if (cy < vy || cy >= vy + 20) vy = cy - 5;
            if (vy < 0) vy = 0;
        }
    } else if (choice == "Replace") {
        show_replace(filename, current_theme);
        load_file(filename); // Reload to update line count etc
    } else if (choice == "Go To") {
        int line = show_goto(total_lines, current_theme);
        if (line != -1) {
            cy = line; cx = 0;
            if (cy < vy || cy >= vy + 20) vy = cy - 5;
            if (vy < 0) vy = 0;
        }
    } else if (choice == "Outline") {
        int line = show_outline(filename, current_theme);
        if (line != -1) {
            cy = line; cx = 0;
            if (cy < vy || cy >= vy + 20) vy = cy - 5;
            if (vy < 0) vy = 0;
        }
    } else if (choice == "Problems") {
        show_problems(filename, current_theme);
    } else if (choice == "Theme") {
        if (current_theme == ncinput::ThemeName::Light) current_theme = ncinput::ThemeName::Dark;
        else if (current_theme == ncinput::ThemeName::Dark) current_theme = ncinput::ThemeName::Grey;
        else current_theme = ncinput::ThemeName::Light;
    } else if (choice == "Quit") {
        running = false;
    }
}

static const std::vector<std::string> py_keywords = {
    "def", "class", "if", "else", "elif", "while", "for", "import", "from",
    "return", "True", "False", "None", "break", "continue", "pass", "try",
    "except", "with", "as", "global", "print", "len", "range", "in", "is",
    "not", "and", "or"
};

std::vector<Editor::Token> const& Editor::tokenize(int line_idx, std::string const& line) {
    if (token_cache.count(line_idx)) return token_cache[line_idx];

    std::vector<Token> tokens;
    auto const& t = ncinput::get_theme(current_theme);

    int col_kw = C_BLUE;
    int col_str = C_RGB(0, 128, 0);
    int col_com = C_RGB(128, 128, 128);
    int col_num = C_RED;
    int col_op = C_MAGENTA;

    if (current_theme == ncinput::ThemeName::Dark) {
        col_kw = C_RGB(76, 127, 255);
        col_str = C_RGB(134, 102, 255); // Actually light green in py, but let's follow
    }

    size_t i = 0;
    while (i < line.length()) {
        char c = line[i];
        if (c == '#') {
            tokens.push_back({line.substr(i), col_com});
            break;
        } else if (c == '"' || c == '\'') {
            size_t start = i++;
            while (i < line.length() && line[i] != c) i++;
            if (i < line.length()) i++;
            tokens.push_back({line.substr(start, i - start), col_str});
        } else if (isspace(c)) {
            size_t start = i++;
            while (i < line.length() && isspace(line[i])) i++;
            tokens.push_back({line.substr(start, i - start), t.txt});
        } else if (isdigit(c)) {
            size_t start = i++;
            while (i < line.length() && isdigit(line[i])) i++;
            tokens.push_back({line.substr(start, i - start), col_num});
        } else if (isalpha(c) || c == '_') {
            size_t start = i++;
            while (i < line.length() && (isalnum(line[i]) || line[i] == '_')) i++;
            std::string word = line.substr(start, i - start);
            int color = t.txt;
            for (auto const& kw : py_keywords) if (kw == word) { color = col_kw; break; }
            tokens.push_back({word, color});
        } else {
            tokens.push_back({std::string(1, c), col_op});
            i++;
        }
    }
    token_cache[line_idx] = tokens;
    return token_cache[line_idx];
}

void Editor::draw_line(int x, int y, int line_idx, std::string const& line) {
    auto const& tokens = tokenize(line_idx, line);
    int cur_x = x;
    for (auto const& tok : tokens) {
        dtext(cur_x, y, tok.color, tok.text.c_str());
        int w, h;
        dtext_size(tok.text.c_str(), dfont_default(), &w, &h);
        cur_x += w;
    }
}


void Editor::render() {
    auto const& t = ncinput::get_theme(current_theme);
    dclear(t.modal_bg);

    int y = 45;
    for (int i = vy; i < total_lines && y < 528; ++i) {
        std::string& line = get_line(i);
        draw_line(5, y, i, line);
        if (i == cy) {
            int cw, ch;
            dtext_nsize(line.c_str(), cx, dfont_default(), &cw, &ch);
            drect(5 + cw, y, 5 + cw + 1, y + 18, t.txt);
        }
        y += 20;
    }

    // Header
    drect(0, 0, 320, 40, t.accent);
    dtext_opt(160, 20, t.txt_acc, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, filename.c_str(), -1);
}

void Editor::run() {
    load_config();
    while (running) {
        render();
        dupdate();

        key_event_t e = getkey();
        if (e.key == KEY_MENU) {
            do_menu();
        } else if (e.key == KEY_UP) {
            if (cy > 0) {
                cy--;
                if (cy < vy) vy = cy;
            }
        } else if (e.key == KEY_DOWN) {
            if (cy < total_lines - 1) {
                cy++;
                if (cy >= vy + 20) vy = cy - 19;
            }
        } else if (e.key == KEY_LEFT) {
            if (cx > 0) cx--;
        } else if (e.key == KEY_RIGHT) {
            std::string line = get_line(cy);
            if (cx < (int)line.length()) cx++;
        } else if (e.key >= ' ' && e.key <= '~') {
            // Basic character input
            std::string& line = get_line(cy);
            line.insert(cx, 1, (char)e.key);
            token_cache.erase(cy);
            cx++;
        } else if (e.key == KEY_DEL) {
            std::string& line = get_line(cy);
            if (cx > 0) {
                line.erase(cx - 1, 1);
                token_cache.erase(cy);
                cx--;
            } else if (cy > 0) {
                // Join with previous line
                std::string current = get_line(cy);
                std::string& prev = get_line(cy - 1);
                cx = prev.length();
                prev += current;
                lines.erase(lines.begin() + cy);
                line_loaded.erase(line_loaded.begin() + cy);
                line_offsets.erase(line_offsets.begin() + cy);
                total_lines--;
                token_cache.clear();
                cy--;
            }
        } else if (e.key == KEY_EXE) {
            // New line
            std::string& line = get_line(cy);
            std::string rem = line.substr(cx);
            line = line.substr(0, cx);
            token_cache.erase(cy);

            cy++;
            total_lines++;
            lines.insert(lines.begin() + cy, rem);
            line_loaded.insert(line_loaded.begin() + cy, true);
            line_offsets.insert(line_offsets.begin() + cy, 0);
            token_cache.clear();
            cx = 0;
        } else if (e.key == KEY_EXIT) {
            running = false;
        }
    }
}

} // namespace ced
