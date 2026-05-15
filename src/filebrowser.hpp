#pragma once
#include "ncinput.hpp"
#include <string>

namespace filebrowser {

std::string browse(std::string const& title = "Open File", ncinput::ThemeName theme = ncinput::ThemeName::Light);

}
