#include "goto.hpp"
#include <cstdlib>
#include <cstdio>

namespace goto_line {

int show(int max_lines, ncinput::ThemeName theme) {
    char buf[16] = {0};
    char prompt[32];
    sprintf(prompt, "Go to (1-%d):", max_lines);
    if (ncinput::input(buf, 16, prompt, "numeric_int", theme) == 0) {
        int line = atoi(buf);
        if (line < 1) line = 1;
        if (line > max_lines) line = max_lines;
        return line - 1;
    }
    return -1;
}

}
