#include <justui/p/vec.h>
#include <gint/std/stdlib.h>

void vec_init(vec_t *v, size_t elsize)
{
	v->data = NULL;
	v->size = 0;
	v->free = 0;
	v->elsize = elsize;
}

void vec_clear(vec_t *v)
{
	free(v->data);
	v->data = NULL;
	v->size = 0;
	v->free = 0;
	/* Leave v->elsize for potential future reuse */
}

bool vec_add(vec_t *v, size_t n)
{
	if(n > v->free) {
		/* Allocate either size/2 new elements or 4 elements */
		size_t ext = max(v->size / 2, 4);
		/* Make sure that no more than 255 free elements will remain */
		if(v->free + ext > n + 255)
			ext = (n + 255) - v->free;

		size_t newsize = v->size + v->free + ext;
		void *newdata = realloc(v->data, newsize * v->elsize);
		if(!newdata) return false;

		v->data = newdata;
		v->free = newsize - v->size - n;
		v->size += n;
	}
	else {
		v->free -= n;
		v->size += n;
	}

	return true;
}

bool vec_remove(vec_t *v, size_t n)
{
	n = min(n, v->size);

	/* Make sure that the vector is at least half-full and that no more than
	   255 free elements remain */
	if(v->size - n <= (v->size + v->free) / 2 || v->free + n > 255) {
		size_t newsize = v->size - n;
		void *newdata = realloc(v->data, newsize * v->elsize);
		if(!newdata) return false;

		v->data = newdata;
		v->free = 0;
	}
	else {
		v->free += n;
	}

	v->size -= n;
	return true;
}
