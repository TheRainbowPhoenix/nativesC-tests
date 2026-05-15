//---
// JustUI.util.vector: Dynamic arrays
//---

#ifndef _J_UTIL_VECTOR
#define _J_UTIL_VECTOR

#include <justui/defs.h>

/* Vector metadata, on four bytes; the pointer is included, but the user will
   have aliased union access from the macro definition */
typedef struct {
	/* Pointer to elements of the vector */
	void *data;
	/* Number of elements used in the vector */
	uint16_t size;
	/* Number of free elements (never more than 255) */
	uint8_t free;
	/* Element size, in bytes */
	uint8_t elsize;

} vec_t;

/* Macro to declare a vector and have typed access to its data */
#define DECLARE_VEC(type, name)	\
	union {						\
		type *name;				\
		vec_t name ## _vec;		\
	};

/* Initialize a vector; the element size can be specified with the typed
   pointer, like this: vec_init(&name_vector, sizeof *name); */
void vec_init(vec_t *v, size_t elsize);

/* Free a vector's data, can also be used to clear all elements */
void vec_clear(vec_t *v);

//---
// Size management
//---

/* Add elements to the vector. The elements should be assigned by the owner
   through the typed pointer, this only allocates and maintains the size. */
bool vec_add(vec_t *v, size_t elements);

/* Remove elements from the vector */
bool vec_remove(vec_t *v, size_t elements);

#endif /* _J_UTIL_VECTOR */
