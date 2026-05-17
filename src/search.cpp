#include "search.hpp"
#include <os/file.h>
#include <os/string.h>
#include <os/mem.h>
#include <stdlib.h>

namespace ced {

static bool local_strstr(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    unsigned int h_len = String_Strlen(haystack);
    unsigned int n_len = String_Strlen(needle);
    if (n_len == 0) return true;
    if (h_len < n_len) return false;
    for (unsigned int i = 0; i <= h_len - n_len; i++) {
        bool match = true;
        for (unsigned int j = 0; j < n_len; j++) {
            if (haystack[i + j] != needle[j]) { match = false; break; }
        }
        if (match) return true;
    }
    return false;
}

void Search::show(const char* theme) {
    char* query = ncinput::input("Search:", ncinput::InputType::AlphaNumeric, theme);
    if (query) { Mem_Free(query); }
}

bool Search::search_in_file(int fd, const char* query, int* found_line, int* found_col) {
    if (fd < 0 || !query) return false;
    int sres = File_Lseek(fd, 0, FILE_SEEK_SET); (void)sres;
    char buffer[1024]; int bytes; int line = 0;
    while ((bytes = File_Read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        if (local_strstr(buffer, query)) {
            if (found_line) *found_line = line;
            if (found_col) *found_col = 0;
            return true;
        }
        for(int i=0; i<bytes; i++) if(buffer[i] == '\n') line++;
    }
    return false;
}
} // namespace ced
