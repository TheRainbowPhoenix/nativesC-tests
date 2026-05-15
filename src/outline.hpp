#pragma once
#include <string>
#include <vector>
#include "ncinput.hpp"

namespace ced {

struct OutlineItem {
    std::string name;
    int line;
    int indent;
};

int show_outline(std::string const& filename, ncinput::ThemeName theme = ncinput::ThemeName::Light);

}
