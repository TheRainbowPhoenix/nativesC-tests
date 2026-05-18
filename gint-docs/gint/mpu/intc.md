# intc

gint:mpu:intc - Interrupt Controller
//
//	The interrupt controller is unwieldy because SH7705 and SH7305 have a
//	completely different interface. Everything here is split up and you'll
//	have to explicitly handle both to be able to use the device.
//
//	gint's API provides higher-level and platform-agnostic interrupt
//	management. This is probably what you are looking for.

## Data Structures

## Macros

## Implementation

Source files:

- [src/dma/dma.c](https://github.com/ClasspadDev/gint/blob/dev/src/dma/dma.c)
- [src/kernel/kernel.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/kernel.c)
- [src/intc/intc.c](https://github.com/ClasspadDev/gint/blob/dev/src/intc/intc.c)
- [src/touch/i2c.c](https://github.com/ClasspadDev/gint/blob/dev/src/touch/i2c.c)
