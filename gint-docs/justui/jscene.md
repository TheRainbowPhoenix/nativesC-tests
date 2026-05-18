# jscene

JustUI.jscene: Root object that provides keyboard focus and event handling

## Functions

### `*jscene_create`

Events

```c
jscene *jscene_create(int x, int y, int w, int h, void *parent);
```

---

### `*jscene_create`

Create a new scene at that specified screen position

```c
jscene *jscene_create(int x, int y, int w, int h, void *parent);
```

---

### `*jscene_create_fullscreen`

Create a fullscreen scene The position is (0,0) and the size is (DWIDTH,DHEIGHT).

```c
jscene *jscene_create_fullscreen(void *parent);
```

---

### `*jscene_owning`

Find the scene that manages a widget (if any) This functions walks up the widget tree and locates the scene owning the specified widget, if there is one; otherwise returns NULL.

```c
jscene *jscene_owning(void *widget);
```

---

### `jscene_render`

Layout a scene This is automatically called by jscene_render(), but may be useful if you need to know the size of your widgets before rendering. The layout is recomputed only if something in the scene has changed.

```c
void jscene_render(jscene *scene);
```

---

### `jscene_render`

Layout and render a scene Layout is lazy and performed only if needed. The scene is rendered at its (x,y) point.

```c
void jscene_render(jscene *scene);
```

---

### `*jscene_widget_at`

Find top-most widget at specified location Returns the top-most widget at the given scene-local location. * If there is no widget in the scene covering this location, returns NULL. * If there is exactly one, returns it. * If there are multiple, this function returns the first found by depth- first search where children are enumerated floating first, non-floating next, and increasing within child index order within each group. * For widgets with a stacked layout, only the active child is considered. If exclude_mp is set, this function only intersects with the content and padding box, excluding margins and borders. Otherwise, an intersection with the full box (padding, border and margin included) is performed.

```c
jwidget *jscene_widget_at(jscene *scene, int x, int y, bool exclude_mp);
```

---

### `jscene_read_event`

Get the next upwards event from the queue If there is no event, returns an event of type JSCENE_NONE.

```c
jevent jscene_read_event(jscene *scene);
```

---

### `jscene_queue_event`

Queue an upwards event to be later read by the user This function records an event in the scene's queue, which will later be returned by jscene_pollevent(). This is mostly used by widget code to signal stuff that warrants attention.

```c
void jscene_queue_event(jscene *scene, jevent event);
```

---

### `*jscene_focused_widget`

Query the widget that currently has focus

```c
void *jscene_focused_widget(jscene *scene);
```

---

### `jscene_set_focused_widget`

Move the focus to a widget The selected widget, obviously, must be a descendant of the scene.

```c
void jscene_set_focused_widget(jscene *scene, void *widget);
```

---

### `jscene_show_and_focus`

Make a widget visible and focus it This function does three things: 1. Set the visibility of the target widget to [true] 2. Configure any parent with a stacked layout to show the child that contains the target widget 3. Focus the target widget

```c
void jscene_show_and_focus(jscene *scene, void *widget);
```

---

### `jscene_process_key_event`

Send a key event to the focused widget Returns true if the event was accepted, false if it was ignored.

```c
bool jscene_process_key_event(jscene *scene, key_event_t event);
```

---

### `jscene_process_event`

Bubble an event up from its source Returns true if the event was accepted along the way, false if ignored.

```c
bool jscene_process_event(jscene *scene, jevent event);
```

---

### `jscene_set_mainmenu`

Set whether jscene_run() will return to main menu Disabling this is useful if you want to catch the MENU key or clean up resources before invoking the main menu.

```c
void jscene_set_mainmenu(jscene *scene, bool mainmenu);
```

---

### `jscene_set_poweroff`

Set whether jscene_run() will poweroff This type of poweroff (SHIFT+AC/ON) doesn't allow return to menu, so the add-in "must" resume after powering on again, however sensitive programs will probably want to save important data before leaving anyway.

```c
void jscene_set_poweroff(jscene *scene, bool poweroff);
```

---

### `jscene_set_autopaint`

Set whether jscene_run() handles its own painting This will automatically handle JSCENE_PAINT events by drawing the scene widget and updating the screen. You should use this only if the scene is the only thing to draw; don't overdraw after this. If you have things to draw not handled by jscene, handle JSCENE_PAINT yourself. When enabling autopaint, you should also set a background color for the scene, otherwise frames will draw transparently on top of each other.

```c
void jscene_set_autopaint(jscene *scene, bool autopaint);
```

---

### `jscene_run`

Run a scene's main loop This function implements a main control loop that sleeps when there is nothing to do, forwards all key events to the scene, and returns only to notify GUI events or hand over key events that have been ignored by the scene. If a scene event occurs, returns it. If a key event occurs, an event of type JWIDGET_KEY is return and its .key attribute contains the details of the forwarded keyboard event.

```c
jevent jscene_run(jscene *scene);
```

---

## Data Structures

### `jscene`

jscene: A widget scene with keyboard focus and event handling

   This widget is designed to be the root of a widget tree. It keeps track of
   widgets with keyboard focus, feeds them keyboard events, and catches other
   useful events to store them in an event queue.

**Fields**:

- `jwidget widget`

- `/* Location on screen */
	int16_t x, y`

- `/* Circular event queue */
   jevent queue[JSCENE_QUEUE_SIZE]`

- `uint8_t queue_first`

- `uint8_t queue_next`

- `/* Number of events lots */
   uint16_t lost_events`

- `/* Whether jscene_run() returns to the main menu */
   bool mainmenu`

- `/* Whether jscene_run() powers off */
   bool poweroff`

- `/* Whether jscene_run() will autopaint */
   bool autopaint`

- `/* Last coordinates a touch cursor was seen at */
   int16_t touch_last_x, touch_last_y`

```c
struct jscene {
jwidget widget;

	/* Location on screen */
	int16_t x, y;

   /* Circular event queue */
   jevent queue[JSCENE_QUEUE_SIZE];
   uint8_t queue_first;
   uint8_t queue_next;

   /* Number of events lots */
   uint16_t lost_events;
   /* Whether jscene_run() returns to the main menu */
   bool mainmenu;
   /* Whether jscene_run() powers off */
   bool poweroff;
   /* Whether jscene_run() will autopaint */
   bool autopaint;

   /* Last coordinates a touch cursor was seen at */
   int16_t touch_last_x, touch_last_y;
};
```

---

## Macros

### `JSCENE_QUEUE_SIZE`

```c
#define JSCENE_QUEUE_SIZE 32
```

---

### `jscene_layout`

recomputed only if something in the scene has changed.

```c
#define jscene_layout jwidget_layout
```

---

## Implementation

Implementation is in the gint source tree.
