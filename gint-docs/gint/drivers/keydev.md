# keydev

gint:keydev - Kernel's keyboard input devices


## Functions


### `*keydev_std`

State of keys that have changes since the last flip monitoring reset.


```c
keydev_t *keydev_std(void);
```


---


### `*keydev_std`

Standard keyboard input device Returns the keyboard device structure for the calculator's keyboard.


```c
keydev_t *keydev_std(void);
```


---


### `keydev_init`

Initialize a keyboard device to its idle state


```c
void keydev_init(keydev_t *d);
```


---


### `keydev_process_state`

Process the new keyboard states for events This function compares the provided scanned state with the stored state and generates events to catch up. This should be used by scan-based keyboard devices after a scan.


```c
void keydev_process_state(keydev_t *d, uint8_t state[12]);
```


---


### `keydev_process_key`

Process a new key state for events This function compares the provided key state with the stored state and generates events to catch up. This should be used by event-based keyboard devices (such as demo replays) to feed in new events.


```c
void keydev_process_key(keydev_t *d, int keycode, bool state);
```


---


### `keydev_repeat_event`

Generate a repeat event if applicable At the end of every scan tick, this source will generate a repeat event if the repeat transform is enabled and the conditions for a repeat are satisfied.


```c
key_event_t keydev_repeat_event(keydev_t *d);
```


---


### `keydev_tick`

Prepare the next tick This function maintains time trackers in the device and should be called in each frame after the scanning is finished and the keydev_process_*() functions have been called. The timer elapsed since the last tick should be specified as well.


```c
void keydev_tick(keydev_t *d, uint us);
```


---


### `keydev_unqueue_event`

Retrieve the next keyboard event in queue This source provides the queued KEYEV_UP, KEYEV_DOWN and KEYEV_HOLD events generated from the regular scans in chronological order. It does not apply transforms; to do this, use keydev_read().


```c
key_event_t keydev_unqueue_event(keydev_t *d);
```


---


### `keydev_idle`

Check if all keys are released A list of keys to ignore can be specified as variable arguments. The list must be terminated by a 0 keycode.


```c
bool keydev_idle(keydev_t *d, ...);
```


---


### `keydev_async_filter`

Obtain current async filter


```c
keydev_async_filter_t keydev_async_filter(keydev_t const *d);
```


---


### `keydev_set_async_filter`

Set low-level async filter


```c
void keydev_set_async_filter(keydev_t *d, keydev_async_filter_t filter);
```


---


### `keydev_keydown`

Check if a key is down according to generated events


```c
bool keydev_keydown(keydev_t *d, int key);
```


---


### `keydev_keypressed`

Check if a key was pressed This compares to the state at the time of the last keydev_clear_flips().


```c
bool keydev_keypressed(keydev_t *d, int key);
```


---


### `keydev_keyreleased`

Check if a key was released This compares to the state at the time of the last keydev_clear_flips().


```c
bool keydev_keyreleased(keydev_t *d, int key);
```


---


### `keydev_clear_flips`

Reset flip info used by keypressed()/keyreleased()


```c
void keydev_clear_flips(keydev_t *d);
```


---


### `keydev_transform`

Obtain current transform parameters


```c
keydev_transform_t keydev_transform(keydev_t *d);
```


---


### `keydev_set_transform`

Set transform parameters


```c
void keydev_set_transform(keydev_t *d, keydev_transform_t tr);
```


---


### `keydev_set_standard_repeats`

Enable a simple repeater This function changes the [repeater] member of the devices's transform. It loads the default repeat profile which applies one delay (in us) before the first repeat, and then a second, usually shorter delay, between subsequent repeats. The unit of the argument is in microseconds, but the granularity of the delay is dependent on the keyboard scan frequency. In the default setting (128 Hz scans), the possible repeat delays are approximately 8 ms, 16 ms, 23 ms, 31 ms. The system default is (500 ms, 125 ms). With the 128 Hz setting, this default is reached exactly without approximation. gint's default is (400 ms, 40 ms) for more reactivity. Note: Due to a current API limitation, every input device uses the delays for the physical keyboard. @first_us  Delay between key press and first repeat (microseconds) @next_us   Delay between subsequent repeats (microseconds)


```c
void keydev_set_standard_repeats(keydev_t *d, int first_us, int next_us);
```


---


### `keydev_read`

Retrieve the next transformed event If there is no event, returns an event with type KEYEV_NONE, unless [wait=true], in which case waits for an event to occur or *timeout to become non-zero (if timeout is not NULL), whichever comes first.


```c
key_event_t keydev_read(keydev_t *d, bool wait, volatile int *timeout);
```


---


## Data Structures


### `keydev_t`

keydev_repeat_profile_t: Function deciding when and how to repeat keys. */
typedef int (*keydev_repeat_profile_t)(int key, int duration, int count);

/* keydev_transform_t: Full specification for transforms on the event stream */
typedef struct {
	/* List of enabled transforms. The order is cannot be changed because
	   any other order than the default produces useless results. This is
	   an OR-combination of KEYDEV_TR_* flags. */
	int enabled;

	/* Repeater function. This function is called whenever (key) is pressed
	   (KEYEV_DOWN) or repeated (KEYEV_HOLD), and should indicate how long
	   to wait until the next repeat. This setting is meaningful only if
	   <Repeats> is enabled.

	   -> (key) is the key currently being repeated.
	   -> (duration) is the time since it was first pressed (us).
	   -> (count) is the number of repeats produced so far.

	   The function should either return -1 to disable all further repeats,
	   or a positive number of microseconds until the next repeat. Note
	   that the precision of the delay is limited by the speed at which the
	   keyboard is scanned, which is nowhere near microsecond-level. By
	   default (128 Hz) the precision is about 7.8 ms. */
	keydev_repeat_profile_t repeater;

} GPACKEDENUM keydev_transform_t;

/* keydev_async_filter_t: Low-level asynchronous event filter. */
typedef bool (*keydev_async_filter_t)(key_event_t event);

/* keydev_t: Keyboard device

   This structure represents the state and settings of a keyboard device that
   provides input to the kernel and applications. The notion of keyboard device
   is useful for demo/replays that input events without the physical keyboard,
   and a couple of corner uses like control over USB.

   The keyboard device has built-in event transformations, which modify the
   stream of events by adding information, combining modifiers, and removing
   undesired events. Because the event transformation rely on the current state
   of the keyboard, they must be run by the driver whenever events are read, so
   they are tied to the device.

   The default keyboard functions pollevent(), waitevent(), getkey() and
   keydown() are shortcuts for keydev functions using the physical keyboard as
   their input.


**Fields**:

- `/* Current device time in scanning-ticks */
	uint time`

- `/* Last time when repeats were considered */
	uint time_repeats`

- `/* Next event in queue, position after last event in queue */
	int8_t queue_next`

- `int8_t queue_end`

- `/* Number of events lost because of missing queue space */
	uint16_t events_lost`

- `/* Event transforms */
	keydev_transform_t tr`

- `/* Asynchronous event filter. This is a low-level filter which is
	   called when events are generated to process and filter them. It
	   provides the unique ability to run keyboard-triggered code even if
	   the program's main thread is busy, for instance running some sort of
	   infinite loop. */
	keydev_async_filter_t async_filter`

- `uint pressed_alpha :1`

- `uint delayed_shift :1`

- `uint delayed_alpha :1`

- `uint :4`

- `/* Number of repeats already sent */
	int16_t rep_count`

- `/* Time since key was first pressed (us) */
	int rep_time`

- `/* Delay until next repeat, set by the repeat planner (us) */
	int rep_delay`

- `/* Latest state of keys we are aware of. At every processing step, the
	   difference between this and the fresh information is queued and this
	   is updated. state_now is identical to the real state obtained from
	   the device unless earlier events failed to be queued, in which case
	   a difference is maintained so they will be reconsidered later. */
	GALIGNED(4) uint8_t state_now[12]`

- `/* State of keys based on produced events. (state_queue + queue) is
	   always identical to (state_now). When the queue is empty both states
	   are the same. This is the user's view of the keyboard. */
	GALIGNED(4) uint8_t state_queue[12]`

- `/* Event queue (circular buffer) */
	key_event_t queue[KEYBOARD_QUEUE_SIZE]`

- `/* Parameters for the standard repeat function */
	int rep_standard_first, rep_standard_next`

- `/* State of keys that have changes since the last flip monitoring reset. */
   GALIGNED(4) uint8_t state_flips[12]`


```c
struct keydev_t {
/* Current device time in scanning-ticks */
	uint time;
	/* Last time when repeats were considered */
	uint time_repeats;

	/* Next event in queue, position after last event in queue */
	int8_t queue_next;
	int8_t queue_end;
	/* Number of events lost because of missing queue space */
	uint16_t events_lost;

	/* Event transforms */
	keydev_transform_t tr;

	/* Asynchronous event filter. This is a low-level filter which is
	   called when events are generated to process and filter them. It
	   provides the unique ability to run keyboard-triggered code even if
	   the program's main thread is busy, for instance running some sort of
	   infinite loop. */
	keydev_async_filter_t async_filter;

	// <Delayed Modifiers>

	/* delayed_* is set when the delayed modifier is active (after a press/
	   release). pressed_* is set between the press and the release. */
	uint pressed_shift :1;
	uint pressed_alpha :1;
	uint delayed_shift :1;
	uint delayed_alpha :1;
	uint :4;

	// <Repeats>

	/* Candidate key for repeats (or 0 if no key is candidate yet) */
	int16_t rep_key;
	/* Number of repeats already sent */
	int16_t rep_count;
	/* Time since key was first pressed (us) */
	int rep_time;
	/* Delay until next repeat, set by the repeat planner (us) */
	int rep_delay;

	/* Latest state of keys we are aware of. At every processing step, the
	   difference between this and the fresh information is queued and this
	   is updated. state_now is identical to the real state obtained from
	   the device unless earlier events failed to be queued, in which case
	   a difference is maintained so they will be reconsidered later. */
	GALIGNED(4) uint8_t state_now[12];
	/* State of keys based on produced events. (state_queue + queue) is
	   always identical to (state_now). When the queue is empty both states
	   are the same. This is the user's view of the keyboard. */
	GALIGNED(4) uint8_t state_queue[12];
	/* Event queue (circular buffer) */
	key_event_t queue[KEYBOARD_QUEUE_SIZE];

	/* Parameters for the standard repeat function */
	int rep_standard_first, rep_standard_next;

   /* State of keys that have changes since the last flip monitoring reset. */
   GALIGNED(4) uint8_t state_flips[12];
};
```


---


## Macros


### `KEYBOARD_QUEUE_SIZE`

Size of the buffer event queue


```c
#define KEYBOARD_QUEUE_SIZE 32
```


---
