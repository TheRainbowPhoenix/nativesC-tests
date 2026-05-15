#include <gint/display.h>
#include <gint/keyboard.h>
#include "ced.hpp"

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================

int main() {
    // Initialize display
    dclear(C_WHITE);
    dupdate();
    
    // Create and run CED application
    CED editor;
    editor.loadConfig("/.ced");
    editor.run();
    editor.saveConfig("/.ced");
    
    return 0;
}
