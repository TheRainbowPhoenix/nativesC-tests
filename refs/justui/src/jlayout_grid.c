#include <justui/jlayout.h>
#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include "jlayout_p.h"

/* TODO: Functions for grid layouts */

void jlayout_grid_csize(void *w0)
{
	J_CAST(w)
	w->w = 0;
	w->h = 0;
}

void jlayout_grid_apply(GUNUSED void *w0)
{
}
