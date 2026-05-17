#pragma once
#include "ncinput.hpp"

namespace ced {
class Outline {
public:
    struct Entry { char name[64]; int line; bool is_class; };
    static void show(const char* filename, const char* theme = "light");
};
class Problems {
public:
    static void show(const char* theme = "light");
};
} // namespace ced
