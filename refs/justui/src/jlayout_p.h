//---
// JustUI.jlayout (private): Internal layout functions
//---

#ifndef _J_JLAYOUT_P
#define _J_JLAYOUT_P

/* jlayout_*_csize(): Natural size of content box for widgets with layouts

   This function finds the size of the children of (w) with jwidget_msize(),
   and then deduces the natural content size of (w). The results are stored in
   (w->w) and (w->h), as usual for the first phase of the layout process.

   This function is called only by jwidget_msize() during layout. */
void jlayout_box_csize(void *w);
void jlayout_stack_csize(void *w);
void jlayout_grid_csize(void *w);

/* jlayout_*_apply(): Layout widgets with layouts

   This function is essentially the second phase of the layout process for
   widgets with layouts. Given that the margin-box size is set in (w->w) and
   (w->h), the layout splits available content space between children and
   positions them, before calling jwidget_layout() on the children.

   This function is called only by jwidget_layout() during layout. */
void jlayout_box_apply(void *w);
void jlayout_stack_apply(void *w);
void jlayout_grid_apply(void *w);

#endif /* _J_JLAYOUT_P */
