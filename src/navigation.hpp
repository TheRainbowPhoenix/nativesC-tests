#pragma once
#include "ncinput.hpp"

namespace ced {
class FileBrowser {
public:
    static char* browse(const char* start_path = "\\\\fls0", const char* theme = "light");
};
class Goto {
public:
    static int show(int max_lines, const char* theme = "light");
};
} // namespace ced
