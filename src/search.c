#include "search.h"
#include <string.h>
#include <stdio.h>

void search_show(void* editor, get_line_cb cb, nc_theme_name_t theme) {
    char pattern[64] = {0};
    if (nc_input(pattern, 64, "Search:", "alpha_numeric", theme) == 0) {
        if (strlen(pattern) == 0) return;

        bool replace = nc_ask("Search", "Do you want to Search and Replace?", "Yes", "No", theme);
        char replacement[64] = {0};
        if (replace) {
            if (nc_input(replacement, 64, "Replace with:", "alpha_numeric", theme) != 0) return;
        }

        int found_line = -1;
        int found_pos = -1;

        for (int i = 0; i < 1000; i++) {
            char* line = cb(editor, i);
            if (!line) break;
            char* p = strstr(line, pattern);
            if (p) {
                found_line = i;
                found_pos = p - line;
                break;
            }
        }

        if (found_line != -1) {
            char msg[128];
            sprintf(msg, "Found at line %d, pos %d.", found_line + 1, found_pos);
            nc_ask("Search", msg, "Go to", "Cancel", theme);
        } else {
            nc_ask("Search", "Not found in first 1000 lines.", "OK", "Cancel", theme);
        }
    }
}
