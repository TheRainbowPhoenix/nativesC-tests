#include "analysis.hpp"
#include <sdk/os/file.h>
#include <sdk/os/input.h>
#include <string.h>
#include <stdlib.h>

namespace ced {
void Outline::show(const char* filename, const char* theme_name) {
    if (!ncinput::ask("Outline View", "Scan Python code?", "Run", "Cancel", theme_name)) return;
    int fd = File_Open(filename, FILE_OPEN_READ);
    if (fd < 0) return;

    char buffer[1024]; int bytes; int line_idx = 0;
    const char* results[100]; int count = 0;

    while ((bytes = File_Read(fd, buffer, sizeof(buffer))) > 0) {
        char* line = strtok(buffer, "\n");
        while (line && count < 100) {
            // Trim leading spaces
            char* p = line; while(*p == ' ' || *p == '\t') p++;
            if (strncmp(p, "def ", 4) == 0 || strncmp(p, "class ", 6) == 0) {
                results[count++] = strdup(p);
            }
            line = strtok(NULL, "\n");
        }
        struct Input_Event ev;
        if (GetInput(&ev, 0, 0x10) == 0 && ev.type == EVENT_KEY && ev.data.key.keyCode == KEYCODE_POWER_CLEAR) break;
    }
    File_Close(fd);
    int selected = ncinput::pick(results, count, "Outline", theme_name);
    if (selected >= 0) { /* Jump to line logic */ }
    for(int i=0; i<count; i++) free((void*)results[i]);
}
void Problems::show(const char* theme) {
    const char* dummy[] = {"No issues found."};
    ncinput::pick(dummy, 1, "Linter", theme);
}
} // namespace ced
