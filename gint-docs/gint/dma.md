# dma

gint:dma - Direct Memory Access for efficient data transfer


## Functions


### `dma_transfer_wait`

Perform an asynchronous DMA data transfer This function starts a DMA data transfer and returns immediately. The provided callback will be invoked once the transfer is finish. You can also call dma_transfer_wait() to wait until the transfer completes. You can create a callback with GINT_CALL() or pass GINT_CALL_NULL. @channel   DMA channel (0..5) @size      Transfer size @blocks    Number of blocks (transferred memory = size * blocks) @src       Source pointer, must be aligned with transfer size @src_mode  Source address mode @dst       Destination address, must be aligned with transfer size @dst_mode  Destination address mode @callback  Function to invoke when the transfer finishes


**Returns**: Returns true on success.


```c
void dma_transfer_wait(int channel);
```


---


### `dma_transfer_wait`

Wait for an asynchronous transfer to finish @channel   DMA channel (0..5)


```c
void dma_transfer_wait(int channel);
```


---


### `*dma_memset`

Fast 32-aligned memset This function is your typical memset(), except that the destination and size must be 32-aligned, and that the pattern is 4 bytes instead of one. It is replicated to 32 bytes then used to fill the destination area. This 4-byte fixed size may be lifted in future versions. This function cannot be used with virtualized (P0) addresses. @dst      Destination address (32-aligned) @pattern  4-byte pattern to fill @dst @size     Sie of destination area (32-aligned)


```c
void *dma_memset(void *dst, uint32_t pattern, size_t size);
```


---


## Data Structures


### `dma_size_t`

dma_size_t - Transfer block size


**Fields**:

- `/* Normal transfers of 1, 2, 4, 8, 16 or 32 bytes at a time */
	DMA_1B = 0,
	DMA_2B = 1,
	DMA_4B = 2,
	DMA_8B = 7,
	DMA_16B = 3,
	DMA_32B = 4,

	/* Transfers of 16 or 32 bytes divided in two operations */
	DMA_16B_DIV = 11,
	DMA_32B_DIV = 12,`


```c
enum dma_size_t {
/* Normal transfers of 1, 2, 4, 8, 16 or 32 bytes at a time */
	DMA_1B = 0,
	DMA_2B = 1,
	DMA_4B = 2,
	DMA_8B = 7,
	DMA_16B = 3,
	DMA_32B = 4,

	/* Transfers of 16 or 32 bytes divided in two operations */
	DMA_16B_DIV = 11,
	DMA_32B_DIV = 12,
};
```


---


### `dma_address_t`

dma_address_t - Addressing mode for source and destination regions


**Fields**:

- `/* Fixed address mode: the same address is read/written at each step */
	DMA_FIXED = 0,
	/* Increment: address is incremented by transfer size at each step */
	DMA_INC = 1,
	/* Decrement: only allowed for 1/2/4-byte transfers */
	DMA_DEC = 2,
	/* Fixed division mode: address is fixed even in 16/32-divide mode */
	DMA_FIXEDDIV = 3,`


```c
enum dma_address_t {
/* Fixed address mode: the same address is read/written at each step */
	DMA_FIXED = 0,
	/* Increment: address is incremented by transfer size at each step */
	DMA_INC = 1,
	/* Decrement: only allowed for 1/2/4-byte transfers */
	DMA_DEC = 2,
	/* Fixed division mode: address is fixed even in 16/32-divide mode */
	DMA_FIXEDDIV = 3,
};
```


---


## Macros


### `dma_transfer_noint`

Old name for dma_transfer_atomic()


```c
#define dma_transfer_noint dma_transfer_atomic
```


---
