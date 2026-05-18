# mmu

gint:mpu:mmu - Memory Management Unit
//
//	The MMU mainly exposes the contents of the TLB for us to inspect.
//	Functions to manipulate these are exposed by <gint/mmu.h>.

## Macros

### `SH7305_MMU`

```c
#define SH7305_MMU (*(sh7305_mmu_t *)0xff000000)
```

---

## Implementation

Implementation is in the gint source tree.
