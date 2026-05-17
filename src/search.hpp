#pragma once
#include "ncinput.hpp"

namespace ced {
class Search {
public:
    static void show(const char* theme = "light");
    static bool search_in_file(int fd, const char* query, int* found_line, int* found_col);
};
} // namespace ced
