#include "analysis.hpp"
#include <os/file.h>
#include <os/input.h>
#include <os/string.h>
#include <os/mem.h>
#include <stdlib.h>

namespace ced {

static char* local_strdup(const char* s) {
    if (!s) return nullptr;
    unsigned int len = String_Strlen(s);
    char* d = (char*)Mem_Malloc(len + 1);
    if (d) String_Strcpy(d, s);
    return d;
}

static int local_strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) return (unsigned char)s1[i] - (unsigned char)s2[i];
        if (s1[i] == 0) return 0;
    }
    return 0;
}

void Outline::show(const char* filename, const char* theme_name) {
    if (!ncinput::ask("Outline View", "Scan Python code?", "Run", "Cancel", theme_name)) return;
    int fd = File_Open(filename, FILE_OPEN_READ);
    if (fd < 0) return;

    char buffer[1024]; int bytes;
    const char* results[100]; int count = 0;

    while ((bytes = File_Read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        char* line = buffer;
        while (*line && count < 100) {
            char* next_line = (char*)String_Strchr(line, '\n');
            if (next_line) *next_line = '\0';
            char* p = line; while(*p == ' ' || *p == '\t') p++;
            if (local_strncmp(p, "def ", 4) == 0 || local_strncmp(p, "class ", 6) == 0) {
                results[count++] = local_strdup(p);
            }
            if (!next_line) break;
            line = next_line + 1;
        }
        struct Input_Event ev;
        Mem_Memset(&ev, 0, sizeof(ev));
        if (GetInput(&ev, 0, 0x10) == 0 && ev.type == EVENT_KEY && (ev.data.key.keyCode == KEYCODE_POWER_CLEAR || ev.data.key.keyCode == KEYCODE_POWER)) break;
    }
    (void)File_Close(fd);
    int selected = ncinput::pick(results, count, "Outline", theme_name);
    (void)selected;
    for(int i=0; i<count; i++) Mem_Free((void*)results[i]);
}

void Problems::show(const char* theme) {
    const char* dummy[] = {"No issues found."};
    (void)ncinput::pick(dummy, 1, "Linter", theme);
}

} // namespace ced
