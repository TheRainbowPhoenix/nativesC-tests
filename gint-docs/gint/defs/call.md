# call

gint:defs:call - Indirect calls
//
// The following types and functions can be used to create "indirect function
// calls": function calls that are executed several times or a long time after
// they are created. This is useful for callbacks, where you set up a function
// to be called when an event occurs. The function and its argument are kept in
// the call object until the event occurs, at which point the call is realized.

## Functions

### `GINT_CALL`

Callback that sets an integer to 1 This is defined as a function to make sure the pointer is to an int.

```c
return GINT_CALL(GINT_CALL_SET_function, pointer);
```

---

### `GINT_CALL`

Callback that increments an integer

```c
return GINT_CALL(GINT_CALL_INC_function, pointer);
```

---

### `GINT_CALL`

Same as GINT_CALL_SET(), but returns TIMER_STOP

```c
return GINT_CALL(GINT_CALL_SET_STOP_function, pointer);
```

---

### `GINT_CALL`

Callback that increments an integer

```c
return GINT_CALL(GINT_CALL_INC_STOP_function, pointer);
```

---

## Data Structures

### `gint_call_arg_t`

gint_call_arg_t: All types of arguments allowed in an indirect call

   Because a function call cannot be easily pieced together, there are
   restrictions on what arguments can be passed. The following union lists all
   of the available types. Other types can be used if casted, mainly pointers;
   see the description of GINT_CALL() for details.

**Fields**:

- `/* 32-bit integers */
	int i`

- `unsigned int u`

- `int32_t i32`

- `uint32_t u32`

- `/* 32-bit floating-point */
	float f`

- `/* Pointers to most common types, in all possible cv-qualifications */
	#define POINTER(type, name) \
	type *name`

- `\
	type const *name ## _c`

- `\
	type volatile *name ## _v`

- `\
	type volatile const *name ## _cv`

- `POINTER(void, pv)
	POINTER(char, pc)
	POINTER(unsigned char, puc)
	POINTER(short, ps)
	POINTER(unsigned short, pus)
	POINTER(int, pi)
	POINTER(unsigned int, pui)
	POINTER(int8_t, pi8)
	POINTER(uint8_t, pu8)
	POINTER(int16_t, pi16)
	POINTER(uint16_t, pu16)
	POINTER(int32_t, pi32)
	POINTER(uint32_t, pu32)
	POINTER(int64_t, pi64)
	POINTER(uint64_t, pu64)
	POINTER(long long int, pll)
	POINTER(unsigned long long int, pull)
	POINTER(float, pf)
	POINTER(double, pd)
	#undef POINTER`

```c
union gint_call_arg_t {
/* 32-bit integers */
	int i;
	unsigned int u;
	int32_t i32;
	uint32_t u32;
	/* 32-bit floating-point */
	float f;

	/* Pointers to most common types, in all possible cv-qualifications */
	#define POINTER(type, name) \
	type *name; \
	type const *name ## _c; \
	type volatile *name ## _v; \
	type volatile const *name ## _cv;

	POINTER(void, pv)
	POINTER(char, pc)
	POINTER(unsigned char, puc)
	POINTER(short, ps)
	POINTER(unsigned short, pus)
	POINTER(int, pi)
	POINTER(unsigned int, pui)
	POINTER(int8_t, pi8)
	POINTER(uint8_t, pu8)
	POINTER(int16_t, pi16)
	POINTER(uint16_t, pu16)
	POINTER(int32_t, pi32)
	POINTER(uint32_t, pu32)
	POINTER(int64_t, pi64)
	POINTER(uint64_t, pu64)
	POINTER(long long int, pll)
	POINTER(unsigned long long int, pull)
	POINTER(float, pf)
	POINTER(double, pd)
	#undef POINTER
};
```

---

### `gint_call_t`

gint_call_t: Indirect call with up to 4 register arguments

**Fields**:

- `void *function`

- `gint_call_arg_t args[4]`

```c
struct gint_call_t {
void *function;
	gint_call_arg_t args[4];
};
```

---

## Macros

### `GINT_CALL`

error: cast to union type from type not present in union -> This is emitted if you pass a parameter of an invalid type.

```c
#define GINT_CALL(func, ...) \
```

---

### `GINT_CALL_ARGS1`

```c
#define GINT_CALL_ARGS1(a1, ...) \
```

---

### `GINT_CALL_ARGS2`

```c
#define GINT_CALL_ARGS2(a2, ...) \
```

---

### `GINT_CALL_ARGS3`

```c
#define GINT_CALL_ARGS3(a3, ...) \
```

---

### `GINT_CALL_ARGS4`

```c
#define GINT_CALL_ARGS4(a4, ...) \
```

---

### `GINT_CALL_NULL`

GINT_CALL_NULL: Empty function call

```c
#define GINT_CALL_NULL ((gint_call_t){ .function = NULL, .args = {} })
```

---

### `GINT_CALL_FLAG`

called function will take the interrupt context as its first argument.

```c
#define GINT_CALL_FLAG(func, ...) \
```

---

## Implementation

Source files:

- [src/kernel/world.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/world.c)
- [src/touch/i2c.c](https://github.com/ClasspadDev/gint/blob/dev/src/touch/i2c.c)
