#include "goto.h"
#include <stdlib.h>
#include <stdio.h>

int goto_line_show(int max_lines, nc_theme_name_t theme) {
    char buf[16] = {0};
    char prompt[32];
    sprintf(prompt, "Go to (1-%d):", max_lines);
    if (nc_input(buf, 16, prompt, "numeric_int", theme) == 0) {
        int line = atoi(buf);
        if (line < 1) line = 1;
        if (line > max_lines) line = max_lines;
        return line - 1;
    }
    return -1;
}
