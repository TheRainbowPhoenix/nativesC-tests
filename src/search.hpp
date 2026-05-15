#pragma once
#include <string>
#include "ncinput.hpp"

namespace ced {

struct SearchResult {
    int line;
    int col;
};

SearchResult show_search(std::string const& filename, ncinput::ThemeName theme = ncinput::ThemeName::Light);
void show_replace(std::string const& filename, ncinput::ThemeName theme = ncinput::ThemeName::Light);

}
