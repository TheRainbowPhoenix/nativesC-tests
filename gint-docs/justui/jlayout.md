# jlayout

JustUI.jlayout: Widget positioning mechanisms


## Functions


### `jlayout_set_manual`

Remove a layout from a widget This function removes the current layout of the widget.


```c
void jlayout_set_manual(void *w);
```


---


### `*jlayout_get_hbox`

Spacing between elements


```c
jlayout_box *jlayout_get_hbox(void *w);
```


---


### `*jlayout_get_hbox`

jlayout_get_hbox(), jlayout_get_vbox(): Get configuration for box layouts These functions return the jlayout_box parameters for widgets that have a box layout, and NULL otherwise.


```c
jlayout_box *jlayout_get_hbox(void *w);
```


---


### `*jlayout_set_hbox`

jlayout_set_hbox(), jlayout_set_vbox(): Create box layouts These functions configure the specified widget to have a box layout, and return the jlayout_box parameters to configure that layout. The parameters are initialized even if the widget previously had a box layout.


```c
jlayout_box *jlayout_set_hbox(void *w);
```


---


### `*jlayout_get_stack`

Index of currently-visible child


```c
jlayout_stack *jlayout_get_stack(void *w);
```


---


### `*jlayout_get_stack`

Get configuration for stack layouts For widgets that have a stack layout, returns a pointer to the parameters. Otherwise, returns NULL.


```c
jlayout_stack *jlayout_get_stack(void *w);
```


---


### `*jlayout_set_stack`

Create stack layouts Configure the specified widget to have a stack layout and return the new parameters. The new layout is cleared even if the widget previously had a stack layout.


```c
jlayout_stack *jlayout_set_stack(void *w);
```


---


## Data Structures


### `jlayout_type`

jlayout_type: Built-in layout mechanisms


**Fields**:

- `/* User or sub-class places children manually */
	J_LAYOUT_NONE = 0,
	/* Children are laid out in a vertical box`

- `spacing applies. Extra
	   space is redistributed proportionally to the stretch attribute. */
	J_LAYOUT_VBOX,
	/* Same as J_LAYOUT_VBOX, but horizontally. */
	J_LAYOUT_HBOX,
	/* Children are stacked, only one is visible at a time. */
	J_LAYOUT_STACK,
	/* Children are laid out in a grid`

- `spacing applies. */
	J_LAYOUT_GRID,`


```c
enum jlayout_type {
/* User or sub-class places children manually */
	J_LAYOUT_NONE = 0,
	/* Children are laid out in a vertical box; spacing applies. Extra
	   space is redistributed proportionally to the stretch attribute. */
	J_LAYOUT_VBOX,
	/* Same as J_LAYOUT_VBOX, but horizontally. */
	J_LAYOUT_HBOX,
	/* Children are stacked, only one is visible at a time. */
	J_LAYOUT_STACK,
	/* Children are laid out in a grid; spacing applies. */
	J_LAYOUT_GRID,
};
```


---


### `jlayout_box`

jlayout_set_manual(): Remove a layout from a widget
   This function removes the current layout of the widget. */
void jlayout_set_manual(void *w);

//---
// Box layout
//---

/* jlayout_box: Parameters for VBOX and HBOX layouts


**Fields**:

- `/* Spacing between elements */
	uint8_t spacing`

- `uint :24`


```c
struct jlayout_box {
/* Spacing between elements */
	uint8_t spacing;
	uint :24;
};
```


---


### `jlayout_stack`

jlayout_get_hbox(), jlayout_get_vbox(): Get configuration for box layouts
   These functions return the jlayout_box parameters for widgets that have a
   box layout, and NULL otherwise. */
jlayout_box *jlayout_get_hbox(void *w);
jlayout_box *jlayout_get_vbox(void *w);

/* jlayout_set_hbox(), jlayout_set_vbox(): Create box layouts
   These functions configure the specified widget to have a box layout, and
   return the jlayout_box parameters to configure that layout. The parameters
   are initialized even if the widget previously had a box layout. */
jlayout_box *jlayout_set_hbox(void *w);
jlayout_box *jlayout_set_vbox(void *w);

//---
// Stack layouts
//---

/* jlayout_stack: Parameters for STACK layouts


**Fields**:

- `/* Index of currently-visible child */
	int8_t active`

- `uint :24`


```c
struct jlayout_stack {
/* Index of currently-visible child */
	int8_t active;
	uint :24;
};
```


---
