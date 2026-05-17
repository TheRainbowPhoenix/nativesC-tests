#include "navigation.hpp"
#include <os/file.h>
#include <os/mem.h>
#include <os/string.h>
#include <stdlib.h>

namespace ced {

static char* local_strdup(const char* s) {
    if (!s) return nullptr;
    unsigned int len = String_Strlen(s);
    char* d = (char*)Mem_Malloc(len + 1);
    if (d) String_Strcpy(d, s);
    return d;
}

char* FileBrowser::browse(const char* start_path, const char* theme) {
    (void)start_path;
    int findHandle; char_const16_t name[256]; struct File_FindInfo findInfo;
    char_const16_t path16[16] = {'\\', '\\', 'f', 'l', 's', '0', '\\', '*', '\0'};
    const char* options[100]; int count = 0;
    if (File_FindFirst(path16, &findHandle, name, &findInfo) == FILE_OK) {
        do {
            char* n = (char*)Mem_Malloc(256);
            for(int i=0; i<256; i++) { n[i] = (char)name[i]; if(name[i]==0) break; }
            options[count++] = n;
        } while (File_FindNext(findHandle, name, &findInfo) == FILE_OK && count < 100);
        (void)File_FindClose(findHandle);
    }
    int selected = ncinput::pick(options, count, "Select File", theme);
    char* result = (selected >= 0) ? local_strdup(options[selected]) : nullptr;
    for(int i=0; i<count; i++) Mem_Free((void*)options[i]);
    return result;
}

int Goto::show(int max_lines, const char* theme) {
    (void)max_lines;
    char* input = ncinput::input("Go to Line:", ncinput::InputType::NumericInt, theme);
    if (input) {
        int line = 0; // Simple atoi replacement
        for(int i=0; input[i]; i++) if(input[i]>='0' && input[i]<='9') line = line*10 + (input[i]-'0');
        Mem_Free(input); return line;
    }
    return -1;
}

} // namespace ced
