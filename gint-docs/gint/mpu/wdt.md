# wdt

gint:mpu:wdt - Watchdog Timer

## Data Structures

### `WTCNT`

sh7705_wdt_t - Watch Dog Timer */
typedef volatile struct
{
	/* WDT registers are unique in access size; reads are performed with
	   8-bit accesses, but writes are performed with 16-bit accesses.

**Fields**:

- `uint8_t READ`

- `uint16_t WRITE`

```c
union WTCNT {
uint8_t READ;
		uint16_t WRITE;
};
```

---

## Macros

### `SH7705_WDT`

```c
#define SH7705_WDT (*((sh7705_wdt_t *)0xffffff84))
```

---

## Implementation

Source files:

- [src/cpg/overclock.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/overclock.c)
