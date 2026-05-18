# timer

gint:timer - Timer operation


## Functions


### `timer_configure`

Reserve and configure a timer This function finds and configures a timer (without starting it). On success, it returns the ID of the configured timer, which is used in all other timer functions. If no timer matching the requested settings is available, this function returns -1. When started, the configured timer will run for the requested delay and call the supplied callback function at the end of this delay. The callback function can then decide whether to leave the timer running (and be called again after the same delay) or stop the timer. The first argument specifies what kind of timer you want. * TIMER_ANY will let timer_configure() choose any available timer. timer_configure() will only use an ETMU if the delay is more than 0.1 ms to avoid resolution issues. Most of the time this is what you need. * TIMER_TMU or TIMER_ETMU will let timer_configure() choose an available TMU or ETMU, respectively. * Specifying an ID (0..8) will request exactly that timer. In this case, and if the ID is a TMU (0,1,2), you may add (with + or |) a prescaler value to specify the clock input. Otherwise the clock is set to Pphi/4. If no timer matching the supplied settings is available, timer_configure() returns -1. The second argument is the delay. With TIMER_ANY, TIMER_TMU and TIMER_ETMU, the delay is interpreted as a number of µs. With an explicit ID, the delay is interpreted as a value of TCOR; see timer_delay() in this case. Note that TCOR values are sensitive to the overclock level! The third argument is an indirect function call to be made when the timer expires. You can create one with GINT_CALL(); the function must return an int equal to either TIMER_CONTINUE or TIMER_STOP to control the subsequent operation of the timer. Default calls are available in <gint/defs/call.h> with common operations that are useful in timers. If GINT_CALL_NULL is passed, a default function that stops the timer will be used. On success, the configured timer becomes reserved; it can no longer be returned by timer_configure() until: * Either timer_stop() is called, * Or the callback returns TIMER_STOP (which also stops the timer). Remember than the returned timer is not started yet; see timer_start(). @timer     Requested timer; TIMER_{ANY,TMU,ETMU} or an ID with prescaler @delay     Delay between each event, in µs unless first argument is an ID @callback  Function to be called when the timer fires


```c
int timer_configure(int timer, uint64_t delay_us, gint_call_t callback);
```


---


### `timer_start`

Start a configured timer The specified timer will start counting down and call its callback function at regular intervals until it is paused or stopped.


```c
void timer_start(int timer);
```


---


### `timer_pause`

Pause a timer without freeing it Pauses the specified timer will be paused. The timer stops counting but is not freed and can be resumed by calling timer_start().


```c
void timer_pause(int timer);
```


---


### `timer_stop`

Stop and free a timer Stops and frees a timer, making it available to timer_setup(). This timer should no longer be used unless it is returned again by timer_setup().


```c
void timer_stop(int timer);
```


---


### `timer_wait`

Wait for a timer to stop Waits until the timer pauses or stops. If the timer is not running, returns immediately. Even after timer_wait(), the timer may not be available since it may have only paused. If the timer never stops, you're in trouble.


```c
void timer_wait(int timer);
```


---


### `timer_spinwait`

Start a timer and actively wait for UNF Waits until the timer raises UNF, without sleeping. This is useful for delays in driver code that is run when interrupts are disabled. UNIE is disabled before starting the timer and waiting, so the callback is never called.


```c
void timer_spinwait(int timer);
```


---


### `timer_delay`

Compute a timer constant from a duration in seconds This function calculates the timer constant for a given delay. timer_setup() does this computation when TIMER_ANY, TIMER_TMU or TIMER_ETMU is provided, but expects you to provide the exact constant when an explicit timer ID is requested. timer_reload() also expects the constant. For TMU the clock can be Pphi/4, Pphi/16, Pphi/64 and Pphi/256, which can respectively count up to about 350 seconds, 23 minutes, 95 minutes and 381 minutes. For ETMU the clock is TCLK at 32768 Hz, which can count up to 36 hours. Any longer delay should probably be managed by the RTC (which counts even when the calculator is off). @timer     The timer you are planning to use @delay_us  Requested delay in microseconds @clock     The clock value, irrelevant if timer >= 3


```c
uint32_t timer_delay(int timer, uint64_t delay_us, int clock);
```


---


### `timer_reload`

Change a timer's delay constant for next interrupts Changes the delay constant of the given timer. Nothing will happen until the next callback; then the timer will update its delay to reflect the new constant. The new delay can be calculated by the timer_delay() function. @timer  Timer id, as returned by timer_setup() @delay  New delay (unit depends on the clock source)


```c
void timer_reload(int timer, uint32_t delay);
```


---


### `timer_setup`

Old variant of timer_configure()


```c
int timer_setup(int timer, uint64_t delay_us, timer_callback_t callback, ...);
```


---


### `timer_timeout`

Makes sure an argument is always provided, for va_arg()


```c
int timer_timeout(volatile void *arg);
```


---


### `timer_timeout`

Callback that sets a flag and halts the timer Use GINT_CALL_SET_STOP() with timer_configure() instead.


```c
int timer_timeout(volatile void *arg);
```


---


## Data Structures


## Macros


### `timer_count`

timer_count(): Number of timers available on the platform


```c
#define timer_count() (isSH3() ? 4 : 9)
```


---


### `timer_setup`

Makes sure an argument is always provided, for va_arg()


```c
#define timer_setup(...) timer_setup(__VA_ARGS__, 0)
```


---
