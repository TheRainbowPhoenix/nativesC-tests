#include "ced.hpp"
#include <os/lcd.h>
#include <os/mem.h>

int main(void) {
    ced::Editor editor;
    if (editor.init()) {
        editor.run();
    }
    return 0;
}

// Bare-metal stubs
extern "C" {
    void __cxa_pure_virtual() { while (1); }
}

void* operator new(size_t size) { return Mem_Malloc(size); }
void operator delete(void* p) noexcept { if(p) Mem_Free(p); }
void operator delete(void* p, size_t) noexcept { if(p) Mem_Free(p); }
