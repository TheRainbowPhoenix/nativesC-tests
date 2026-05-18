# clock

gint:clock - Clock signals, overclock, and standby modes

## Functions

### `cpg_compute_freq`

clock_freq() - get the frequency of the main clocks This function returns the address of a static object which is used by the module; this address never changes.

```c
void cpg_compute_freq(void);
```

---

### `cpg_compute_freq`

Compute the current clock frequency This function updates the data structure returned by clock_freq() by determining the current clock frequencies from the CPG.

```c
void cpg_compute_freq(void);
```

---

### `clock_get_speed`

The default clock speed is always Ftune's F1

```c
int clock_get_speed(void);
```

---

### `clock_get_speed`

Determine the current clock speed This function compares the current hardware state with the settings for each speed level and returns the current one. If the hardware state does not correspond to any of Ftune's settings, CLOCK_SPEED_UNKNOWN is returned.

```c
int clock_get_speed(void);
```

---

### `clock_set_speed`

Set the current clock speed This function sets the clock speed to the desired level. This is "the overclock function", although depending on the model or settings it is also the downclocking function. The process of changing clock speeds is non-trivial, requires waiting for the DMA to finish its work and slightly affects running timers. You should avoid changing the clock speed constantly if not necessary. If this function detects that the desired clock speed is already in use, it returns without performing any change. Currently the clock speed is not reset during a world switch nor when leaving the add-in.

```c
void clock_set_speed(int speed);
```

---

### `cpg_get_overclock_setting`

Queries the clock setting from the hardware.

```c
void cpg_get_overclock_setting(struct cpg_overclock_setting *s);
```

---

### `cpg_set_overclock_setting`

Applies the specified overclock setting.

```c
void cpg_set_overclock_setting(struct cpg_overclock_setting const *s);
```

---

### `sleep_us`

Sleep for a fixed duration in microseconds Stops the processor until the specified delay in microseconds has elapsed. (The processor will still wake up occasionally to handle interrupts.) This function selects a timer with timer_setup() called with TIMER_ANY.

```c
void sleep_us(uint64_t delay_us);
```

---

### `sleep_us_spin`

Actively sleep for a fixed duration in microseconds Like sleep_us(), but uses timer_spinwait() and does not rely on interrupts being enabled. Useful in timer code running without interrupts.

```c
void sleep_us_spin(uint64_t delay_us);
```

---

## Data Structures

### `cpg_overclock_setting`

clock_get_speed(): Determine the current clock speed

   This function compares the current hardware state with the settings for each
   speed level and returns the current one. If the hardware state does not
   correspond to any of Ftune's settings, CLOCK_SPEED_UNKNOWN is returned. */
int clock_get_speed(void);

/* clock_set_speed(): Set the current clock speed

   This function sets the clock speed to the desired level. This is "the
   overclock function", although depending on the model or settings it is also
   the downclocking function.

   The process of changing clock speeds is non-trivial, requires waiting for
   the DMA to finish its work and slightly affects running timers. You should
   avoid changing the clock speed constantly if not necessary. If this function
   detects that the desired clock speed is already in use, it returns without
   performing any change.

   Currently the clock speed is not reset during a world switch nor when
   leaving the add-in. */
void clock_set_speed(int speed);

/* If you want to faithfully save and restore the clock state while properly
   handling clock speeds that are not Ftune/PTune's defaults, you can get a
   full copy of the settings.

   WARNING: Applying random settings with cpg_set_overclock_setting() might
   damage your calculator!

**Fields**:

- `uint32_t FLLFRQ, FRQCR`

- `uint32_t CS0BCR, CS2BCR, CS3BCR, CS5aBCR`

- `uint32_t CS0WCR, CS2WCR, CS3WCR, CS5aWCR`

```c
struct cpg_overclock_setting {
uint32_t FLLFRQ, FRQCR;
    uint32_t CS0BCR, CS2BCR, CS3BCR, CS5aBCR;
    uint32_t CS0WCR, CS2WCR, CS3WCR, CS5aWCR;
};
```

---

## Macros

### `sleep_ms`

sleep_ms(): Sleep for a fixed duration in milliseconds

```c
#define sleep_ms(delay_ms) sleep_us((delay_ms) * 1000ull)
```

---

## Implementation

Source files:

- [src/prof/prof.c](https://github.com/ClasspadDev/gint/blob/dev/src/prof/prof.c)
- [src/dma/dma.c](https://github.com/ClasspadDev/gint/blob/dev/src/dma/dma.c)
- [src/kernel/exch.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/exch.c)
- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/usb/pipes.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/pipes.c)
- [src/tmu/tmu.c](https://github.com/ClasspadDev/gint/blob/dev/src/tmu/tmu.c)
- [src/tmu/sleep.c](https://github.com/ClasspadDev/gint/blob/dev/src/tmu/sleep.c)
- [src/cpg/overclock.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/overclock.c)
