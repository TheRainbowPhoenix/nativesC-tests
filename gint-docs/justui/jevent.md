# jevent

JustUI.jevent: GUI union event


## Data Structures


## Macros


### `jevent_is_press`


```c
#define jevent_is_press(E, KEY) jevent_is_press_mods(E, KEY, false, false)
```


---


### `jevent_is_shift_press`


```c
#define jevent_is_shift_press(E, KEY) jevent_is_press_mods(E, KEY, true, false)
```


---


### `jevent_is_alpha_press`


```c
#define jevent_is_alpha_press(E, KEY) jevent_is_press_mods(E, KEY, false, true)
```


---


### `jevent_is_shift_alpha_press`


```c
#define jevent_is_shift_alpha_press(E, KEY) \
```


---
