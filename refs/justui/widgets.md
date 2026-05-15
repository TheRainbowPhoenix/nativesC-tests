# JustUI: Widget types

## General widgets and void types

JustUI defines a base structure `jwidget` useful for containers, as well as a
number of derived structures for content widgets. Because C does not support
any type of polymorphism, JustUI uses `void *` pointers when referring to "any
widget", which accepts pointers to any base or derived structure.

This relaxed typing scheme mostly applies to the `jwidget_*()` functions that
operate on the widget level. Widget-specific functions such as `jinput_value()`
request exactly their type of widget as input (in this case, `jinput`).
Essentially, this means that `jwidget` can be inherited from transparently,
but the other types cannot (a structure attribute must be used). This is a
compromise to work with polymorphic types in the C type system.

## Widget collection

**`jwidget`** is the base type. It provides children/parent management (see
[Widget hierarchy](hierarchy.md)), built-in layouts (see [Space distribution
and layout](layout.md)) and size constraints, geometry (margin, border,
padding, background), and automatic repaint/relayout mechanisms.

**`jscene`** is a special widget designed to be at the root of the widget tree.
It provides event dispatching, automatic repaint and layout, keyboard input,
and a generic input loop that is suitable for most GUI programs. See [Scenes
and events](scene.md).

**`jlabel`** is a content widget that shows a single-line or multi-line piece
of text. It supports various text alignment options (left/right/center), line
wrapping (letter and word level), and a couple of graphical options.

**`jinput`** is a one-line input field. It supports direct key input, delayed
and instant SHIFT and ALPHA modifiers, as well as modifier locking, with visual
hints.

**`jpainted`** is a very simple wigdet that paints itself with a user-provided
function. It is intended to make custom widgets with very little effort. A
`jpainted` can be positioned, sized and managed by the widget tree while the
user only provides a drawing function.

**`jfkeys`** represents a function key bar, normally at the bottom of the
screen. On fx-9860G, it uses an image to show keys; on fx-CG 50, it supports a
string specification that looks like `"@JUMP;;#ROM;#RAM;#ILRAM;#ADDIN"`. It can
change options dynamically.

**`jframe`** is a scrolling frame. It contains a single child widget that's
larger than the frame, scrolls it around, renders scrollbars, and ensures that
the child widget's rendering is clipped to the frame.

**`jlist`** and **`jscrolledlist`** are browsable lists. The list model is not
forced; the user provides an info function (indicating which elements can be
selected and triggered) and a paint function for rendering them. Custom
rendering is possible at any point and delegate widgets for edition can also be
specified. `jscrolledlist` is a `jlist` in a `jframe`, which provides scrolling
and is usually what one wants.

**`jfilebrowser`** is a `jlist`-like file browser which is used to open and
save files in applications.

## Custom widgets and polymorphic operations

For custom widgets that just have custom rendering a no event management, one
can simply use a `jpainted` instance with well-chosen options. However, for
reusable widgets that have internal state or event handling, a new widget type
should be created.

A custom widget must be a structure that starts with a `jwidget`. The type
itself should be register with `j_register_widget()`, and provide a couple of
polymorphic functions. Here is an example with a very trivial widget that holds
an integer counter.

```c
typedef struct {
	jwidget widget;
	int counter;
} jcounter;
```

To be registered in JustUI, a custom widget type must provide a couple of
functions, in the form of a `jwidget_poly` structure. All of the following
functions are detailed in `<justui/widget-api.h>`, but here is a brief review
of the requirements (the `type_poly_` prefix is customary). All of the
functions receive pointers to `jcounter` structures, but the type is `void *`
because of the limitations mentioned earlier.

```c
/* Receives a (jcounter *). Should set its (w) and (h) attributes to the
   natural dimensions of the widget. Cannot be NULL. */
void jcounter_poly_csize(void *counter);

/* Paint a jcounter at (x,y). */
void jcounter_poly_render(void *counter, int x, int y);

/* Destroy the resources of a jcounter. (If there are not malloc'd pointers in
   the structure, this is generally not needed.) */
void jcounter_poly_destroy(void *counter);
```

The following functions should be implemented if the custom widget needs to
receive events (such as keyboard input). Events are defined in
`<justui/jevent.h>`.

```c
/* Handle event (e) sent to a jcounter. Should return true if the event was
   accepted/used, false if it was ignored/unused. */
bool jcounter_poly_event(void *counter, jevent e);
```

The following function is used if (1) an instance of the custom widget has
children, and (2) this instance does not have a layout. This is rarely needed.
See [Space distribution and layout](layout.md)

```c
/* Receives a (jcounter *) that has its full size set in the (w) and (h)
   attributes. Should determine the position and size of the children. */
void jcounter_poly_layout(void *counter);
```

In general, most of the work consists in specifying the `csize()` and
`render()` functions. `destroy()` just has to release the resources allocated
during widget creation, `event()` is straightforward, and `layout()` is very
rarely needed at all.

Once the required functions are implemented, a polymorphic widget structure can
be defined and registered as a new type. A good way to do this is to register
the widget in a constructor located in the same file as the widget creation
function, so that it runs automatically if the widget is used.

```c
static jwidget_poly type_jcounter = {
	.name    = "jcounter",
	.csize   = jcounter_poly_csize,
	.layout  = NULL,
	.render  = jcounter_poly_render,
	.event   = NULL,
	.destroy = NULL,
};

static int jcounter_type_id;

__attribute__((constructor))
static void j_register_jcounter(void)
{
	jcounter_type_id = j_register_widget(&type_jcounter);
}
```

The type ID returned by `j_register_widget()` is how JustUI differentiates
labels from input fields from custom counters. When creating the widget, you
should initialize the `jwidget` field with `jwidget_init()` and specify the
type ID. Note how the parent is also passed at creation time so that the new
widget can automatically be attached to the tree.

```c
jcounter *jcounter_create(int initial_value, void *parent)
{
	if(jcounter_type_id < 0) return NULL;

	jcounter *c = malloc(sizeof *c);
	jwidget_init(&c->widget, jcounter_type_id, parent);

	c->counter = initial_value;
	return c;
}
```

That's pretty much it. There is a fair amount of boilerplate, but this part
is the same for every widget. See `jpainted` for a simple example and `jinput`
for a full-featured one.
