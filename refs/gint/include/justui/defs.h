//---
// JustUI.defs: Type and utility definitions
//---

#ifndef _J_DEFS
#define _J_DEFS

#include <gint/defs/types.h>
#include <gint/defs/util.h>
#include <gint/defs/attributes.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* jdirs: Quadruplet with four directions */
typedef struct {
	uint8_t top;
	uint8_t right;
	uint8_t bottom;
	uint8_t left;
} jdirs;

/* jrect: Small rectangle */
typedef struct {
	int16_t x, y;
	int16_t w, h;
} jrect;

/* jalign: Alignment options with both horizontal and vertical names */
typedef enum {
	/* Horizontal names */
	J_ALIGN_LEFT   = 0,
	J_ALIGN_CENTER = 1,
	J_ALIGN_RIGHT  = 2,
	/* Vertical names */
	J_ALIGN_TOP    = 0,
	J_ALIGN_MIDDLE = 1,
	J_ALIGN_BOTTOM = 2,

} __attribute__((packed)) jalign;

/* j_arg_t: Standard 4-byte argument types for callbacks */
typedef union {
	/* Pointers of different qualifiers */
	void *p;
	void const *cp;
	volatile void *vp;
	volatile void const *vcp;
	/* Integers */
	int32_t i32;
	uint32_t u32;

} GTRANSPARENT j_arg_t;

/* Vararg macro to cast (void *) parameters to (jwidget *), useful in generic
   widget functions. J_CAST(x, y, ...) will expand to
     jwidget *x = x0;
     jwidget *y = y0;
     ...
   and accepts from 1 to 4 parameters. */
#define J_CAST0(x) _Pragma("GCC error \"J_CAST takes only up to 4 arguments\"")
#define J_CAST1(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST0(__VA_ARGS__))
#define J_CAST2(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST1(__VA_ARGS__))
#define J_CAST3(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST2(__VA_ARGS__))
#define J_CAST(x, ...)  jwidget *x = x ## 0; __VA_OPT__(J_CAST3(__VA_ARGS__))

#endif /* _J_DEFS */
