#include "goto.hpp"
#include <string>

namespace ced {

int show_goto(int max_lines, ncinput::ThemeName theme) {
    std::string res = ncinput::input("Go to line (1-" + std::to_string(max_lines) + "):", "numeric_int", theme);
    if (res.empty()) return -1;
    char *endptr;
    long line = std::strtol(res.c_str(), &endptr, 10);
    if (*endptr != '\0') return -1;
    if (line < 1) line = 1;
    if (line > max_lines) line = max_lines;
    return (int)line - 1;
}

}
