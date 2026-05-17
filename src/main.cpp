#include "ced.hpp"
#include <os/lcd.h>

int main(void) {
    ced::Editor editor;
    if (editor.init()) {
        editor.run();
    }
    return 0;
}
