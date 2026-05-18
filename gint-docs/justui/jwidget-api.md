# jwidget-api

JustUI.jwidget-API: API for subclassed widget types

## Functions

### `j_register_widget`

Register a new widget type This function returns a new widget type ID to pass to jwidget_init() when creating widgets of the custom type. Returns -1 if registration fails. The polymorphic structure must outlive all widgets of the custom type.

```c
int j_register_widget(jwidget_poly *poly);
```

---

### `j_register_event`

Register a new event type This function returns a new ID to set in the (type) field of jevent objects. The ID is unique, and can be trusted to be always valid (unless you register more than 64k events in which case you asked for the trouble).

```c
int j_register_event(void);
```

---

### `jwidget_init`

Initialize a widget This function should be called in the constructor for the subclassed widget type, preferably as soon as possible. It initializes common widget attributes, sets the widget type, and declares the parent. A subclassed widget type must have a jwidget as a first member (so that the address of any instance of the subclassed widget is a valid pointer to jwidget), and that jwidget must be initialized with jwidget_init(). @w       Widget to initialize @type    Type ID, as returned by j_register_widget() @parent  Parent, same as in jwidget_create()

```c
void jwidget_init(jwidget *w, int type, void *parent);
```

---

### `jwidget_msize`

Compute and apply the natural size of a widget's margin-box This function computes the widget's natural margin-box size. It determines the natural content size with the csize() function of either the layout or the widget type, then adds the geometry. The margin-box size is stored in the (w) and (h) attributes of the widget. This function should only be called during the first phase of the layout, to implement subclassed csize() functions. Usually, the parent will implement a customn csize() function by combining the position and msize() of its children.

```c
void jwidget_msize(void *w);
```

---

### `jwidget_emit`

Emit an upwards event from this widget This function walks up the tree until it finds a jscene that can store the event. If there is no jscene in the tree, the event is ignored. The (source) field can be omitted and will be set to the widget's address by default.

```c
void jwidget_emit(void *w, jevent e);
```

---

### `jwidget_event`

Send a downwards event to a widget This function calls the polymorphic event function of the targeted widget to notify it of the specified event.

```c
bool jwidget_event(void *w, jevent e);
```

---

## Data Structures

### `jwidget_poly`

jwidget_poly_csize_t: Determine the natural content size of a widget

   This function is called during layout. It should set the natural size of the
   content box of the widget in (w->w) and (w->h). If the widget has a layout,
   this function will not be called; instead, the layout will determine the
   natural content size. Thus, this function is mostly useful for content
   widgets and not useful for containers.

   Implementations of this function should use jwidget_msize() on the children
   to position their margin box within the widget's content box. The size set
   by this function needs not be in the minimum/maximum range of the widget,
   which is handled later.

   If not overloaded (i.e. NULL in the poly structure), the default behavior is
   to compute the smallest size that fits all children (based on their own
   natural content size), which only makes sense if the children are at fixed
   positions. */
typedef void jwidget_poly_csize_t(void *w);

/* jwidget_poly_layout_t: Layout a widget after its size has been set

   This function is called during the second phase of the layout process, if
   the widget has no layout. The margin-box size allocated to the widget has
   been set in (w->w) and (w->h); the widget must now position its contents and
   children. If the widget has a layout, the layout's specialized function is
   called instead of this one.

   Custom positioning for children is only relevant for widgets that use custom
   layouts, which is fairly rare. Most often, this function is used to position
   internal elements of the widget after the size has been set (e.g. jlabel
   computes line breaks here).

   If not overloaded (i.e. NULL in the poly structure), the default behavior is
   to leave children's positions unchanged, assuming they are fixed. */
typedef void jwidget_poly_layout_t(void *w);

/* jwidget_poly_render_t: Render a widget

   This function is called during rendering after the widget's geometry is
   drawn. (x,y) are the coordinates of the content box. This function must
   render widget-specific visuals. If the widget is clipped (as specified by
   `jwidget_set_clipped()`), this function can specify any drawing coordinates
   and all drawing will automatically be restricted with the widget's box.
   However, if the widget is not clipped, drawing beyond the widget's width and
   height will overflow to other widgets.

   This function should render its children. In the simple case where all
   children can be rendered at the same time, you can simply call
   `jwidget_poly_render(w)` which will do that.

   If for any reason children need to be rendered separately, this function can
   also called `jwidget_render()` on individual children. `jwidget_render()`
   handles the geometry and takes as parameters the coordinates of the margin
   box, so it can be called as:

     jwidget_render(child, x + child->x, y + child->y).

   It will draw the geometry and call the polymorphic renderer of the child at
   its content-box coordinates. Normally you can ignore geometry altogether.

   If not overloaded, the behavior is `jwidget_poly_render(w)`, i.e. just
   render the children if there are any. */
typedef void jwidget_poly_render_t(void *w, int x, int y);
extern jwidget_poly_render_t jwidget_poly_render;

/* jwidget_poly_event_t: Handle an event

   This function is called when an event is targeted at a widget, including for
   key events. This function is somewhat of a catch-all function for dynamic
   occurrences. The widget should either accept the event, do something with
   it, and return true, or refuse the event, do nothing and return false. This
   influences events that propagate, such as key events.

   This function should always default to

       return jwidget_poly_event(w, e);

   to allow default/generic behaviors to propagate, unless the widget
   explicitly wants to interfere with them. */
typedef bool jwidget_poly_event_t(void *w, jevent e);
extern jwidget_poly_event_t jwidget_poly_event;

/* jwidget_poly_destroy_t: Destroy a widget's specific resources

   This function must destroy the widget-specific resources. It is called by
   jwidget_destroy(), which follows it by freeing the widget's standard data
   and destroying the children. This function can be NULL if there are no
   widget-specific resources to free. It should not call the "base" version of
   the function. */
typedef void jwidget_poly_destroy_t(void *w);

/* jwidget_poly: Polymorphic interface for a widget type

**Fields**:

- `/* Type name, used for display and inheritance */
	char const *name`

- `/* Polymorphic functions. If unused for custom widgets, should be set to
	   NULL and the default behavior (described above) will apply. */
	jwidget_poly_csize_t   *csize`

- `jwidget_poly_layout_t  *layout`

- `jwidget_poly_render_t  *render`

- `jwidget_poly_event_t   *event`

- `jwidget_poly_destroy_t *destroy`

```c
struct jwidget_poly {
/* Type name, used for display and inheritance */
	char const *name;
	/* Polymorphic functions. If unused for custom widgets, should be set to
	   NULL and the default behavior (described above) will apply. */
	jwidget_poly_csize_t   *csize;
	jwidget_poly_layout_t  *layout;
	jwidget_poly_render_t  *render;
	jwidget_poly_event_t   *event;
	jwidget_poly_destroy_t *destroy;
};
```

---

## Macros

### `J_DEFINE_WIDGET`

the widget creation function.

```c
#define J_DEFINE_WIDGET(NAME, ...) \
```

---

### `J_DEFINE_WIDGET_POLY`

```c
#define J_DEFINE_WIDGET_POLY(NAME, METHOD) \
```

---

### `J_DEFINE_WIDGET_POLY_PROTO`

```c
#define J_DEFINE_WIDGET_POLY_PROTO(NAME, METHOD) \
```

---

### `J_DEFINE_EVENTS`

```

```c
#define J_DEFINE_EVENTS(...) \
```

---

### `J_DEFINE_EVENTS_NAME2`

```c
#define J_DEFINE_EVENTS_NAME2(COUNTER) _j_init_##COUNTER
```

---

### `J_DEFINE_EVENTS_NAME`

```c
#define J_DEFINE_EVENTS_NAME(COUNTER) J_DEFINE_EVENTS_NAME2(COUNTER)
```

---

### `J_DEFINE_EVENTS_INIT`

```c
#define J_DEFINE_EVENTS_INIT(EVENT) EVENT = j_register_event();
```

---

## Implementation

Implementation is in the gint source tree.
