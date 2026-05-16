#include "search.hpp"
#include "ced.hpp"
#include <cstring>
#include <cstdio>

namespace search {

void show(ced::Editor* editor, ncinput::ThemeName theme) {
    char pattern[64] = {0};
    if (ncinput::input(pattern, 64, "Search:", "alpha_numeric", theme) == 0) {
        if (strlen(pattern) == 0) return;

        bool replace = ncinput::ask("Search", "Do you want to Search and Replace?", "Yes", "No", theme);
        char replacement[64] = {0};
        if (replace) {
            if (ncinput::input(replacement, 64, "Replace with:", "alpha_numeric", theme) != 0) return;
        }

        // Search logic
        int found_line = -1;
        int found_pos = -1;

        // This is a slow operation, would ideally be done in a separate scene or with feedback
        for (int i = 0; i < 1000; i++) { // Limited for responsiveness
            char* line = editor->get_line(i);
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
            ncinput::ask("Search", msg, "Go to", "Cancel", theme);
            // Editor navigation would go here
        } else {
            ncinput::ask("Search", "Not found in first 1000 lines.", "OK", "Cancel", theme);
        }
    }
}

}
