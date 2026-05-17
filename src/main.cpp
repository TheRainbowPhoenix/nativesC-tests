#include "ced.hpp"
#include <sdk/os/lcd.h>

int main(void) {
    ced::Editor editor;
    if (editor.init()) {
        editor.run();
    }
    return 0;
}
