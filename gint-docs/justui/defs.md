# defs

JustUI.defs: Type and utility definitions


## Data Structures


### `jdirs`

jdirs: Quadruplet with four directions


**Fields**:

- `uint8_t top`

- `uint8_t right`

- `uint8_t bottom`

- `uint8_t left`


```c
struct jdirs {
uint8_t top;
	uint8_t right;
	uint8_t bottom;
	uint8_t left;
};
```


---


### `jrect`

jrect: Small rectangle


**Fields**:

- `int16_t x, y`

- `int16_t w, h`


```c
struct jrect {
int16_t x, y;
	int16_t w, h;
};
```


---


## Macros


### `J_CAST0`

and accepts from 1 to 4 parameters.


```c
#define J_CAST0(x) _Pragma("GCC error \"J_CAST takes only up to 4 arguments\"")
```


---


### `J_CAST1`


```c
#define J_CAST1(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST0(__VA_ARGS__))
```


---


### `J_CAST2`


```c
#define J_CAST2(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST1(__VA_ARGS__))
```


---


### `J_CAST3`


```c
#define J_CAST3(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST2(__VA_ARGS__))
```


---


### `J_CAST`


```c
#define J_CAST(x, ...) jwidget *x = x ## 0; __VA_OPT__(J_CAST3(__VA_ARGS__))
```


---
