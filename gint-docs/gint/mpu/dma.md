# dma

gint:mpu:dma - Direct Memory Access control
//
//	The DMA is a major module on fxcg50 because it is needed to send data
//	to the display at a reasonable speed. On fx9860g, it is very rarely
//	used, if ever.


## Macros


### `SH7305_DMA`


```c
#define SH7305_DMA (*((sh7305_dma_t *)0xfe008020))
```


---
