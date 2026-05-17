#ifndef NCINPUT_H
#define NCINPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gint/display.h>

typedef struct {
    int modal_bg;
    int kbd_bg;
    int key_bg;
    int key_spec;
    int key_out;
    int txt;
    int txt_dim;
    int accent;
    int txt_acc;
    int hl;
    int check;
} nc_theme_t;

typedef enum {
    NC_THEME_LIGHT,
    NC_THEME_DARK,
    NC_THEME_GREY
} nc_theme_name_t;

nc_theme_t const* nc_get_theme(nc_theme_name_t name);

void nc_init(void);

// C API
int nc_input(char* buffer, int max_len, const char* prompt, const char* type, nc_theme_name_t theme);
int nc_pick(const char** options, int num_options, const char* prompt, nc_theme_name_t theme);
bool nc_ask(const char* title, const char* body, const char* ok_text, const char* cancel_text, nc_theme_name_t theme);

#ifdef __cplusplus
}
#endif

#endif
