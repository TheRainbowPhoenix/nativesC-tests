# keyboard

gint:keyboard - Keyboard input


## Functions


### `keysc_scan_frequency`

Keyboard frequency analysis is a runtime setting since gint 2.4. This macro is preserved for compatibility until gint 3.


```c
int keysc_scan_frequency(void);
```


---


### `keysc_scan_frequency`

Get the current keyboard scan frequency in Hertz


```c
int keysc_scan_frequency(void);
```


---


### `keysc_scan_frequency_us`

Get keyboard scan delay in microseconds


```c
uint32_t keysc_scan_frequency_us(void);
```


---


### `keysc_set_scan_frequency`

Set the keyboard scan frequency in Hertz The new frequency must be at least 64 for the keyboard to work reliably, and at most 32768 for the underlying ETMU to support it. Out-of-range values are forced to the closest valid value. @freq  New scan frequency, in Hertz


```c
void keysc_set_scan_frequency(int freq);
```


---


### `pollevent`

Poll the next keyboard event This function returns the next event from the event queue, chronologically. If no event is available, it returns a dummy event with type=KEYEV_NONE and time set to the current driver time. This function always returns events with mod=0.


```c
key_event_t pollevent(void);
```


---


### `waitevent`

Wait for the next keyboard event This function works as pollevent() but waits if no event is available. When timeout=NULL, it waits indefinitely. Otherwise, it waits until *timeout becomes non-zero. It is particularly suitable to set *timeout to 1 using a timer with [timer_timeout] as callback. See <gint/timer.h>.


```c
key_event_t waitevent(volatile int *timeout);
```


---


### `clearevents`

Read all events waiting in the queue


```c
void clearevents(void);
```


---


### `cleareventflips`

Set the time reference for keypressed()/keyreleased() The two functions keypressed() and keyreleased() will use the keyboard state at the time this function was called to determine whether any given key was just pressed or jut released.


```c
void cleareventflips(void);
```


---


### `keydown`

Current key state This function returns zero if the specified key is currently up (according to the last events that have been processed) and non-zero if it is down.


```c
int keydown(int key);
```


---


### `keydown_all`

Check a set of keys for simultaneous input Returns non-zero if all provided keys are down. The list should end with a 0 as terminator.


```c
int keydown_all(int key1, ...);
```


---


### `keydown_any`

Check a set of keys for any input Returns nonzero if any one of the specified keys is currently pressed. The sequence should be terminated by a 0 integer.


```c
int keydown_any(int key1, ...);
```


---


### `keypressed`

Check if a key was just pressed This function returns non-zero if the specified key is currently down, *and* it was up at the time of the last call to cleareventflips().


```c
int keypressed(int key);
```


---


### `keyreleased`

Check if a key was just released This function returns non-zero if the specified key is currently up, *and* it was down at the time of the last call to cleareventflips().


```c
int keyreleased(int key);
```


---


### `getkey`

Wait for a key press This function mimics the behavior of the fxlib GetKey(). It returns a key_event_t object where [mod=1], and where [shift] and [alpha] indicate whether SHIFT or ALPHA was pressed before the key was hit. [event] is KEYEV_DOWN when a new key is pressed and KEYEV_HOLD in case of repeats. Similarities with GetKey() include: - Wait for a key to be pressed *after* the call (held keys don't count) - Supports SHIFT and ALPHA modifiers - Repeats arrows keys - Allows return to main menu if the MENU key is pressed - Controls backlight on models that have a back-lit screen getkey() is equivalent to getkey_opt(GETKEY_DEFAULT, NULL).


```c
key_event_t getkey(void);
```


---


### `getkey_opt`

Enhanced getkey() This function enhances getkey() with more general features. An or-combination of option flags (see above) must be supplied as first argument; GETKEY_NONE stands for no option. getkey_opt() returns the same kind of events as getkey(). getkey_opt() supports a generic timeout function in the form of a volatile pointer [timeout]. If it's NULL, getkey_opt() waits indefinitely. Otherwise, it waits until *timeout becomes non-zero. It's up to you to change the value whenever you want to interrupt the call; using a timer with [timer_timeout] as callback is suitable. See <gint/timer.h>. Event transforms in getkey_opt() (SHIFT, ALPHA and repetitions) are handled by changing the transform settings on the keyboard device. These settings are restored when getkey_opt() returns, so if they are originally disabled (which they are unless set manually) then the status of the SHIFT and ALPHA keys is lost between calls (this has an effect it getkey_opt() is interrupted by timeout). Therefore, in order to use modifiers across several calls to getkey_opt(), make sure to enable the transforms on the keyboard device; see <gint/drivers/keydev.h> for details. @options  An or-combination of values from the GETKEY_* enumeration @timeout  Optional pointer to a timeout value Returns a key event of type KEYEV_DOWN or KEYEV_HOLD with [mod=1].


```c
key_event_t getkey_opt(int options, volatile int *timeout);
```


---


### `getkey_feature_function`

Get the current feature function


```c
getkey_feature_t getkey_feature_function(void);
```


---


### `getkey_set_feature_function`

Set the global feature function The feature function can be used to extend getkey() with application-wide shortcuts, in a way similar to the CATALOG, CAPTURE or OFF functions of the original getkey(). The feature function can be set globally at the application level, and thus does not require every call site that uses getkey() to support the shortcuts explicitly. The feature function receives events when they are generated; if can them process them, and return either true to accept the event (preventing getkey() from returning it) or false to refuse the event (in which case it is returned normally). The feature function is enabled by default and be disabled by removing the GETKEY_FEATURES flag in getkey_opt(). Setting function=NULL disables the functionality.


```c
void getkey_set_feature_function(getkey_feature_t function);
```


---


### `keycode_function`

Identify keys F1 .. F6 This function returns number of each F-key (eg. it returns 2 for KEY_F2), and -1 for other keys.


```c
int keycode_function(int keycode);
```


---


### `keycode_digit`

Identify keys 0 .. 9 This function returns the digit associated with digit keycodes (eg. it returns 7 for KEY_7) and -1 for other keys.


```c
int keycode_digit(int keycode);
```


---


## Data Structures


## Macros


### `KEYBOARD_SCAN_FREQUENCY`

Keyboard frequency analysis is a runtime setting since gint 2.4. This macro is preserved for compatibility until gint 3.


```c
#define KEYBOARD_SCAN_FREQUENCY keysc_scan_frequency()
```


---
