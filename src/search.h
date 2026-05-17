#ifndef SEARCH_H
#define SEARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ncinput.h"

// Forward declaration of Editor handled in C by void* or specific header if needed
// but since we want to avoid C++ headers in C, we'll use a callback or opaque pointer.
typedef char* (*get_line_cb)(void* editor, int index);

void search_show(void* editor, get_line_cb cb, nc_theme_name_t theme);

#ifdef __cplusplus
}
#endif

#endif
