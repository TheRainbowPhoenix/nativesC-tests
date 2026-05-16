#include "ced.hpp"
#include "ncinput.hpp"

int main() {
    ncinput::init();

    // Allocate Editor on heap to avoid stack overflow
    ced::Editor* editor = new (std::nothrow) ced::Editor();
    if (!editor) return 1;

    editor->load_config();
    editor->run();

    delete editor;
    return 0;
}
