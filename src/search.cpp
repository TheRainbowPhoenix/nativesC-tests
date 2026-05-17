#include "search.hpp"
#include <sdk/os/file.h>
#include <string.h>
#include <stdlib.h>

namespace ced {
void Search::show(const char* theme) {
    char* query = ncinput::input("Search:", ncinput::InputType::AlphaNumeric, theme);
    if (query) {
        // Implementation would call search_in_file and jump to result
        free(query);
    }
}
bool Search::search_in_file(int fd, const char* query, int* found_line, int* found_col) {
    if (fd < 0 || !query) return false;
    File_Lseek(fd, 0, FILE_SEEK_SET);
    char buffer[1024]; int bytes; int line = 0; uint32_t total_read = 0;
    while ((bytes = File_Read(fd, buffer, sizeof(buffer))) > 0) {
        char* match = strstr(buffer, query);
        if (match) {
            *found_col = (int)(match - buffer);
            // Count lines in this buffer before match
            for(char* p = buffer; p < match; p++) if(*p == '\n') line++;
            *found_line = line;
            return true;
        }
        for(int i=0; i<bytes; i++) if(buffer[i] == '\n') line++;
        total_read += bytes;
        // Optimization: search only part of file or handle boundaries (simplified for now)
    }
    return false;
}
} // namespace ced
