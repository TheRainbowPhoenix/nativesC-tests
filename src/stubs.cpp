#include <cstdlib>
#include <new>

extern "C" {
    void __cxa_pure_virtual() { while (1); }

    // Minimal __cxa_guard for threadsafe statics (though we use -fno-threadsafe-statics)
    int __cxa_guard_acquire(int *g) { return !*g; }
    void __cxa_guard_release(int *g) { *g = 1; }
    void __cxa_guard_abort(int *g) { (void)g; }
}

// Basic operators for new/delete since we use C++
void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* p) noexcept { free(p); }
void operator delete[](void* p) noexcept { free(p); }
void operator delete(void* p, size_t) noexcept { free(p); }
void operator delete[](void* p, size_t) noexcept { free(p); }

namespace std {
    void __throw_bad_alloc() { while(1); }
    void __throw_length_error(char const*) { while(1); }
    void __throw_bad_array_new_length() { while(1); }
}
