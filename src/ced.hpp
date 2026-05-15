#pragma once
#include <string>
#include <vector>
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
    void handle_events();
    void do_menu();

    std::string filename;
    int cx, cy; // Cursor pos
    int vy; // Viewport Y (line index)
    int total_lines;

    std::string get_line(int index);

    ncinput::ThemeName current_theme;
    bool word_wrap;

    struct Token {
        std::string text;
        int color;
    };
    std::map<int, std::vector<Token>> token_cache;
    std::vector<Token> tokenize(int line_idx, std::string const& line);
    void draw_line(int x, int y, int line_idx, std::string const& line);

    jscene* scene;
    bool running;

    // Efficient file handling
    std::string current_chunk;
    size_t chunk_offset;
    void load_chunk(size_t offset);
};

} // namespace ced
