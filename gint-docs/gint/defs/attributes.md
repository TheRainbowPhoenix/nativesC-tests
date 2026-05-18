# attributes

gint:defs:attributes - Macros for compiler-specific attributes


## Macros


### `GSECTION`

Objects from specific sections


```c
#define GSECTION(x) __attribute__((section(x)))
```


---


### `GBSS`

Objects from the gint's uninitialized BSS section


```c
#define GBSS __attribute__((section(".gint.bss")))
```


---


### `GRODATA3`

Additional sections that are only needed on SH3


```c
#define GRODATA3 __attribute__((section(".gint.rodata.sh3")))
```


---


### `GDATA3`


```c
#define GDATA3 __attribute__((section(".gint.data.sh3")))
```


---


### `GBSS3`


```c
#define GBSS3 __attribute__((section(".gint.bss.sh3")))
```


---


### `GILRAM`

Objects for the ILRAM, XRAM and YRAM regions


```c
#define GILRAM __attribute__((section(".ilram")))
```


---


### `GXRAM`


```c
#define GXRAM __attribute__((section(".xyram")))
```


---


### `GYRAM`


```c
#define GYRAM __attribute__((section(".xyram")))
```


---


### `GUNUSED`

Unused parameters or variables


```c
#define GUNUSED __attribute__((unused))
```


---


### `GINLINE`

Functions that *must* be inlined


```c
#define GINLINE __attribute__((always_inline)) inline
```


---


### `GALIGNED`

Aligned variables


```c
#define GALIGNED(x) __attribute__((aligned(x)))
```


---


### `GPACKED`

access sizes silently fail - honestly you don't want this to happen


```c
#define GPACKED(x) __attribute__((packed, aligned(x)))
```


---


### `GPACKEDENUM`

Packed enumerations


```c
#define GPACKEDENUM __attribute__((packed))
```


---


### `GTRANSPARENT`

Transparent unions


```c
#define GTRANSPARENT __attribute__((transparent_union))
```


---


### `GVISIBLE`

Functions and globals that are visible through whole-program optimization


```c
#define GVISIBLE __attribute__((externally_visible))
```


---


### `GWEAK`

Weak symbols


```c
#define GWEAK __attribute__((weak))
```


---


### `GCONSTRUCTOR`

Constructors


```c
#define GCONSTRUCTOR __attribute__((constructor))
```


---


### `GDESTRUCTOR`


```c
#define GDESTRUCTOR __attribute__((destructor))
```


---


### `GNORETURN`

Functions that do not return


```c
#define GNORETURN __attribute__((noreturn))
```


---
