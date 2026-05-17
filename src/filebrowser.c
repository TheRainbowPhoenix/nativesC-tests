#include "filebrowser.h"
#include <justui/jfileselect.h>
#include <justui/jscene.h>
#include <gint/display.h>
#include <string.h>

bool filebrowser_show(char* result_path, nc_theme_name_t theme) {
    (void)theme;
    uint16_t path16[128];
    if (jfileselect(path16, NULL, "Select File") == 0) {
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
