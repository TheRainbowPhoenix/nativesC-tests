# util

gint:defs:util - Various utility macros


## Macros


### `synco`

synco instruction (in a form compatible with sh3eb-elf)


```c
#define synco() __asm__ volatile (".word 0x00ab":::"memory")
```


---


### `GAUTOTYPE`


```c
#define GAUTOTYPE auto
```


---


### `GAUTOTYPE`


```c
#define GAUTOTYPE __auto_type
```


---


### `min`

min(), max() (without double evaluation)


```c
#define min(a, b) ({			\
```


---


### `max`


```c
#define max(a, b) ({			\
```


---


### `sgn`

sgn() (without double evaluation)


```c
#define sgn(s) ({			\
```


---


### `swap`

swap() - exchange two variables of the same type


```c
#define swap(a, b) ({			\
```


---
