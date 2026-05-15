//---
// JustUI.util: Header-level utilities that cannot be exposed to users
//---

#ifndef _J_UTIL
#define _J_UTIL

#include <justui/defs.h>
#include <gint/display.h>

/* Clamp a value between two ends. */
__attribute__((always_inline))
static inline int clamp(int value, int min, int max)
{
	/* Mark the branches as unlikely, that might help */
	if(__builtin_expect(value < min, 0)) return min;
	if(__builtin_expect(value > max, 0)) return max;
	return value;
}

/* Code point for a character input */
uint32_t keymap_translate(int key, bool shift, bool alpha);

/* Intersect two dwindow settings. */
static inline struct dwindow intersect_dwindow(
	struct dwindow d1, struct dwindow d2)
{
	struct dwindow win;
	win.left   = max(d1.left, d2.left);
	win.top    = max(d1.top, d2.top);
	win.right  = max(min(d1.right, d2.right), win.left);
	win.bottom = max(min(d1.bottom, d2.bottom), win.top);
	return win;
}

#endif /* _J_UTIL */
