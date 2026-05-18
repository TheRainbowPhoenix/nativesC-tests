# vec

JustUI.util.vector: Dynamic arrays

## Functions

### `vec_init`

Macro to declare a vector and have typed access to its data

```c
void vec_init(vec_t *v, size_t elsize);
```

---

### `vec_init`

Initialize a vector; the element size can be specified with the typed pointer, like this: vec_init(&name_vector, sizeof *name);

```c
void vec_init(vec_t *v, size_t elsize);
```

---

### `vec_clear`

Free a vector's data, can also be used to clear all elements

```c
void vec_clear(vec_t *v);
```

---

### `vec_add`

Add elements to the vector. The elements should be assigned by the owner through the typed pointer, this only allocates and maintains the size.

```c
bool vec_add(vec_t *v, size_t elements);
```

---

### `vec_remove`

Remove elements from the vector

```c
bool vec_remove(vec_t *v, size_t elements);
```

---

## Data Structures

### `vec_t`

Vector metadata, on four bytes; the pointer is included, but the user will
   have aliased union access from the macro definition

**Fields**:

- `/* Pointer to elements of the vector */
	void *data`

- `/* Number of elements used in the vector */
	uint16_t size`

- `/* Number of free elements (never more than 255) */
	uint8_t free`

- `/* Element size, in bytes */
	uint8_t elsize`

```c
struct vec_t {
/* Pointer to elements of the vector */
	void *data;
	/* Number of elements used in the vector */
	uint16_t size;
	/* Number of free elements (never more than 255) */
	uint8_t free;
	/* Element size, in bytes */
	uint8_t elsize;
};
```

---

## Macros

### `DECLARE_VEC`

Macro to declare a vector and have typed access to its data

```c
#define DECLARE_VEC(type, name) \
```

---

## Implementation

Implementation is in the gint source tree.
