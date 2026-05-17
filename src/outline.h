#ifndef OUTLINE_H
#define OUTLINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ncinput.h"
#include <stdint.h>

int outline_show(const char* filename, nc_theme_name_t theme);
void nc_to_os_path(const char* src, uint16_t* dest, int max_len);

#ifdef __cplusplus
}
#endif

#endif
