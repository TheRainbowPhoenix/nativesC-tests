# tmu

gint:intc:tmu - Timer Unit
//
//	The definitions in this file cover both the Timer Unit and the Extra
//	Timers. The structures are related but not identical; the behaviour is
//	subtly different.


## Macros


### `SH7705_TMU`


```c
#define SH7705_TMU (*(sh7705_tmu_t *)0xfffffe90)
```


---


### `SH7705_ETMU`


```c
#define SH7705_ETMU (*(sh7705_etmu_t *)0xa44c0030)
```


---


### `SH7305_TMU`


```c
#define SH7305_TMU (*((sh7305_tmu_t *)0xa4490004))
```


---


### `SH7305_ETMU`


```c
#define SH7305_ETMU (*(sh7305_etmu_t *)0xa44d0030)
```


---
