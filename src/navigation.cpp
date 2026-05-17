#include "navigation.hpp"
#include <os/file.h>
#include <os/mem.h>
#include <stdlib.h>
#include <string.h>

namespace ced {
char* FileBrowser::browse(const char* start_path, const char* theme) {
    int findHandle; char_const16_t name[256]; struct File_FindInfo findInfo;
    char_const16_t path16[16] = {'\\', '\\', 'f', 'l', 's', '0', '\\', '*', '\0'};
    const char* options[100]; int count = 0;
    if (File_FindFirst(path16, &findHandle, name, &findInfo) == FILE_OK) {
        do {
            char* n = (char*)malloc(256);
            for(int i=0; i<256; i++) { n[i] = (char)name[i]; if(name[i]==0) break; }
            options[count++] = n;
        } while (File_FindNext(findHandle, name, &findInfo) == FILE_OK && count < 100);
        File_FindClose(findHandle);
    }
    int selected = ncinput::pick(options, count, "Select File", theme);
    char* result = (selected >= 0) ? strdup(options[selected]) : nullptr;
    for(int i=0; i<count; i++) free((void*)options[i]);
    return result;
}
int Goto::show(int max_lines, const char* theme) {
    char* input = ncinput::input("Go to Line:", ncinput::InputType::NumericInt, theme);
    if (input) { int line = atoi(input); free(input); return line; }
    return -1;
}
} // namespace ced
