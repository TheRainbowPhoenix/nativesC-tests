#include "goto.hpp"
#include <string>

namespace ced {

int show_goto(int max_lines, ncinput::ThemeName theme) {
    std::string res = ncinput::input("Go to line (1-" + std::to_string(max_lines) + "):", "numeric_int", theme);
    if (res.empty()) return -1;
    try {
        int line = std::stoi(res);
        if (line < 1) line = 1;
        if (line > max_lines) line = max_lines;
        return line - 1;
    } catch (...) {
        return -1;
    }
}

}
