#pragma once
#include <justui/jscene.h>

#ifdef swap
#undef swap
#endif

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ios>
#include <cstdio>
#include <justui/jscene.h>
#include "ncinput.hpp"

namespace ced {

class Editor {
public:
    Editor();
    ~Editor();

    void run();
    void load_config();
    void load_file(std::string const& path);
    void save_file(std::string const& path = "");

private:
    void render();
    void do_menu();

    std::string filename;
    int cx, cy; // Cursor pos
    int vy; // Viewport Y (line index)
    int total_lines;
    std::vector<std::streampos> line_offsets;
    std::vector<std::string> lines;
    std::vector<bool> line_loaded;
    std::vector<int> loaded_indices;

    std::string& get_line(int index);

    ncinput::ThemeName current_theme;
    bool word_wrap;

    struct Token {
        std::string text;
        int color;
    };
    std::map<int, std::vector<Token>> token_cache;
    std::vector<Token> const& tokenize(int line_idx, std::string const& line);
    void draw_line(int x, int y, int line_idx, std::string const& line);

    jscene* scene;
    bool running;
    std::ifstream file_handle;
};

} // namespace ced
