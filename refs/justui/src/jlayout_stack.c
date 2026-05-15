#include <justui/jlayout.h>
#include <justui/jwidget.h>
#include <justui/jwidget-api.h>
#include "jlayout_p.h"
#include "util.h"

jlayout_stack *jlayout_get_stack(void *w0)
{
	J_CAST(w)
	return (w->layout == J_LAYOUT_STACK) ? &w->layout_stack : NULL;
}

jlayout_stack *jlayout_set_stack(void *w0)
{
	J_CAST(w)
	w->layout = J_LAYOUT_STACK;
	jlayout_stack *l = &w->layout_stack;

	l->active = -1;
	return l;
}

void jlayout_stack_csize(void *w0)
{
	J_CAST(w)
	w->w = 0;
	w->h = 0;

	for(int k = 0; k < w->child_count; k++) {
		jwidget *child = w->children[k];
		if(!child->visible) continue;
		jwidget_msize(child);

		w->w = max(w->w, child->w);
		w->h = max(w->h, child->h);
	}
}

void jlayout_stack_apply(void *w0)
{
	J_CAST(w)

	int cw = jwidget_content_width(w);
	int ch = jwidget_content_height(w);

	for(int k = 0; k < w->child_count; k++) {
		jwidget *child = w->children[k];
		if(!child->visible || child->floating) continue;

		/* Maximum size to enforce: this is the acceptable size closest to our
		   content size (that space we have to distribute) */
		int max_w = clamp(cw, child->min_w, child->max_w);
		int max_h = clamp(ch, child->min_h, child->max_h);

		/* Set every child to an acceptable size */
		child->w = clamp(child->w, child->min_w, max_w);
		child->h = clamp(child->h, child->min_h, max_h);

		/* Expand each child if any level of stretch is specified */
		if(child->stretch_x > 0) child->w = max_w;
		if(child->stretch_y > 0) child->h = max_h;

		/* Center each child in the container */
		child->x = (cw - child->w) / 2;
		child->y = (ch - child->h) / 2;
	}
}
