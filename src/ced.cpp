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

namespace ced {

Editor::Editor() : filename("untitled.py"), cx(0), cy(0), vy(0), total_lines(1), current_theme(ncinput::ThemeName::Light), word_wrap(false), running(true) {
    scene = jscene_create_fullscreen(nullptr);
}

Editor::~Editor() {
    jwidget_destroy(scene);
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
    std::ifstream f(path);
    if (f.is_open()) {
        filename = path;
        total_lines = 0;
        std::string line;
        while (std::getline(f, line)) {
            total_lines++;
        }
        if (total_lines == 0) total_lines = 1;
        cx = cy = vy = 0;
    }
}

std::string Editor::get_line(int index) {
    if (index < 0 || index >= total_lines) return "";
    std::ifstream f(filename);
    std::string line;
    for (int i = 0; i <= index; ++i) {
        if (!std::getline(f, line)) return "";
    }
    return line;
}

void Editor::save_file(std::string const& path) {
    std::string target = path.empty() ? filename : path;
    if (target == "untitled.py") {
        target = ncinput::input("Save as:", "alpha_numeric", current_theme);
        if (target.empty()) return;
    }
    std::ofstream f(target);
    if (f.is_open()) {
        for (size_t i = 0; i < lines.size(); ++i) {
            f << lines[i] << (i == lines.size() - 1 ? "" : "\n");
        }
        filename = target;
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

std::vector<Editor::Token> Editor::tokenize(int line_idx, std::string const& line) {
    if (token_cache.count(line_idx)) return token_cache[line_idx];

    std::vector<Token> tokens;
    auto const& t = ncinput::get_theme(current_theme);

    // Simplistic Python lexer port
    static const std::vector<std::string> keywords = {
        "def", "class", "if", "else", "elif", "while", "for", "import", "from",
        "return", "True", "False", "None", "break", "continue", "pass", "try",
        "except", "with", "as", "global", "print", "len", "range", "in", "is",
        "not", "and", "or"
    };

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
            for (auto const& kw : keywords) if (kw == word) { color = col_kw; break; }
            tokens.push_back({word, color});
        } else {
            tokens.push_back({std::string(1, c), col_op});
            i++;
        }
    }
    token_cache[line_idx] = tokens;
    return tokens;
}

void Editor::draw_line(int x, int y, int line_idx, std::string const& line) {
    auto tokens = tokenize(line_idx, line);
    int cur_x = x;
    for (auto const& tok : tokens) {
        dtext(cur_x, y, tok.color, tok.text.c_str());
        int w, h;
        dsize(tok.text.c_str(), dfont_default(), &w, &h);
        cur_x += w;
    }
}

void Editor::load_chunk(size_t offset) {
    std::ifstream f(filename, std::ios::binary);
    if (f.is_open()) {
        f.seekg(offset);
        char buffer[1024];
        f.read(buffer, sizeof(buffer));
        current_chunk = std::string(buffer, f.gcount());
        chunk_offset = offset;
    }
}

void Editor::render() {
    auto const& t = ncinput::get_theme(current_theme);
    dclear(t.modal_bg);

    int y = 45;
    for (int i = vy; i < total_lines && y < 528; ++i) {
        std::string line = get_line(i);
        draw_line(5, y, i, line);
        if (i == cy) {
            int cw, ch;
            dnsize(line.c_str(), cx, dfont_default(), &cw, &ch);
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
        } else if (e.key == KEY_EXIT) {
            running = false;
        }
    }
}

} // namespace ced
