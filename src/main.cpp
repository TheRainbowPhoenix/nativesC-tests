#include "ced.hpp"
#include "ncinput.hpp"
#include <gint/gint.h>

int main() {
    ncinput::init();
    ced::Editor app;
    app.run();
    return 0;
}
