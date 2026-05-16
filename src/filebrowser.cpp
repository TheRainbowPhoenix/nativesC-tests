#include "filebrowser.hpp"
extern "C" {
#include <justui/jfileselect.h>
#include <justui/jscene.h>
#include <gint/display.h>
#include <cstring>
}

namespace filebrowser {

bool show(char* result_path, ncinput::ThemeName theme) {
    (void)theme;
    uint16_t path16[128];
    // jfileselect usually returns a FONTCHARACTER (uint16_t) path
    if (jfileselect(path16, nullptr, "Select File") == 0) {
        // Convert uint16_t to char (simplistic)
        int i = 0;
        while (path16[i] && i < 127) {
            result_path[i] = (char)path16[i];
            i++;
        }
        result_path[i] = '\0';
        return true;
    }
    return false;
}

}
