# rtc

gint:rtc - Real-Time Clock

## Functions

### `rtc_get_time`

Hour (0..23)

```c
void rtc_get_time(rtc_time_t *time);
```

---

### `rtc_get_time`

Minute (0..59)

```c
void rtc_get_time(rtc_time_t *time);
```

---

### `rtc_get_time`

Second (0..59)

```c
void rtc_get_time(rtc_time_t *time);
```

---

### `rtc_get_time`

128-Hz sub-second counter (0...127)

```c
void rtc_get_time(rtc_time_t *time);
```

---

### `rtc_get_time`

Read the current time from the RTC @time  Pointer to rtc_time_t structure (needs not be initialized)

```c
void rtc_get_time(rtc_time_t *time);
```

---

### `rtc_set_time`

Set current time in the RTC If [time->week_day] is not in the valid range, it is set to 0. Other fields are not checked. R64CNT cannot be set to [time->ticks] is ignored. @time  Pointer to new time

```c
void rtc_set_time(rtc_time_t const *time);
```

---

### `rtc_ticks`

Get number of 128-Hz ticks elapsed since Midnight. Returns R64CNT + 128*RSECCNT + 128*60*RMINCNT + 128*60*60*RHRCNT. This can be used as a 128-Hz counter, but it wraps around at midnight.

```c
uint32_t rtc_ticks(void);
```

---

### `rtc_periodic_enable`

Enable the periodic interrupt This function sets up the periodic interrupt to invoke the provided callback regularly. As with timers, the callback must return either TIMER_CONTINUE or TIMER_STOP. Do not confuse this interrupt with CASIO's extra timers that run at 32768 Hz (which are called "RTC timers" in CPU73050.dll). These timers are called Extra TMU or ETMU in gint and are handled by <gint/timer.h>. Note that the timing of the first callback is always uncertain. A 1 Hz timer set up when half of the current second is already elapsed will be called for the first time after only 500 ms, for instance. @frequency  Periodic interrupt frequency @callback   Function to call back at the specified frequency Returns true on success, false if the interrupt is already in use.

```c
bool rtc_periodic_enable(int frequency, gint_call_t callback);
```

---

### `rtc_periodic_disable`

Stop the periodic interrupt This has the same effect as returning TIMER_STOP from the callback, or setting RTC_NONE as the parameter for rtc_periodic_enable().

```c
void rtc_periodic_disable(void);
```

---

### `rtc_start_timer`

Deprecated versions with old-style callbacks and more confusing names

```c
int rtc_start_timer(int frequency, timer_callback_t callback, ...);
```

---

## Data Structures

### `rtc_time_t`

rtc_time_t: A point in time, representable in the RTC registers

   WARNING: A copy of this definition is used in fxlibc, make sure to keep it
   in sync. (We don't install kernel headers before compiling the fxlibc, it's
   kind of a nightmare for the modest build system.)

**Fields**:

- `uint16_t year`

- `/* Years (exact value, e.g. 2018) */
	uint8_t week_day`

- `/* Day of week, (0=Sunday, 6=Saturday) */
	uint8_t month`

- `/* Month (0..11) */
	uint8_t month_day`

- `/* Day of month (1..31) */
	uint8_t hours`

- `/* Hour (0..23) */
	uint8_t minutes`

- `/* Minute (0..59) */
	uint8_t seconds`

- `/* Second (0..59) */
	uint8_t ticks`

- `/* 128-Hz sub-second counter (0...127) */`

```c
struct rtc_time_t {
uint16_t year;		/* Years (exact value, e.g. 2018) */
	uint8_t week_day;	/* Day of week, (0=Sunday, 6=Saturday) */
	uint8_t month;		/* Month (0..11) */
	uint8_t month_day;	/* Day of month (1..31) */
	uint8_t hours;		/* Hour (0..23) */
	uint8_t minutes;	/* Minute (0..59) */
	uint8_t seconds;	/* Second (0..59) */
	uint8_t ticks;		/* 128-Hz sub-second counter (0...127) */
};
```

---

## Macros

### `rtc_start_timer`

```c
#define rtc_start_timer(...) rtc_start_timer(__VA_ARGS__, 0)
```

---

## Implementation

Source files:

- [src/fxlibc_hal.c](https://github.com/ClasspadDev/gint/blob/dev/src/fxlibc_hal.c)
- [src/rtc/rtc_ticks.c](https://github.com/ClasspadDev/gint/blob/dev/src/rtc/rtc_ticks.c)
- [src/rtc/rtc.c](https://github.com/ClasspadDev/gint/blob/dev/src/rtc/rtc.c)
