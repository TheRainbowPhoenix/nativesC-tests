# Changelog and migration guides

## 1.4.0

**Migration: event functions**

The convention for event functions has changed to allow the removall of the "inheritance" property, which was clumsy in C and only useful for event functions.
Now, instead of returning `false` for events that a widget is not interested in, the event function should explicitly forward the event to the "inherited" function, i.e. usually the one from jwidget, and the inheritance parameter to `j_register_widget()` must be removed. The lack of inheritance also removes the need to order constructors.

```c
//=== Before =================================================================//
static bool mywidget_poly_event(void *w, jevent e) {
    if(event_is_interesting(e)) { /* ... */ }
    return false;
}
__attribute__((constructor(1020)))
static void j_register_mywidget(void) {
	mywidget_type_id = j_register_widget(&type_mywidget, "jwidget");
	/* ... */
}

//=== After ==================================================================//
static bool mywidget_poly_event(void *w, jevent e) {
    if(event_is_interesting(e)) { /* ... */ }
    return jwidget_poly_event(w, e); //< Forward to jwidget
}
__attribute__((constructor)) //< No priority needed
static void j_register_mywidget(void) {
	mywidget_type_id = j_register_widget(&type_mywidget); //< No inheritance
	/* ... */
}
```

The event function should not forward events to focused sub-widgets; `jscene` handles that. Emitting an event at the scene level already leads to multiple event function calls, starting at the focused widget, and bubbling up to parents along the way.

**Migration: widget definitions**

The widget definition system has been improved with macros. This change is optional but removes a lot of boilerplate.

The macro `J_DEFINE_WIDGET(<NAME>, <FUNCTIONS>...)` defines a widget with the given `NAME` that implements the given poly-`FUNCTIONS`. This macro:
- Defines `int NAME_type_id`, which is assigned automatically at startup in a constructor;
- Uses `NAME_poly_FUN` for each `FUN` in the list of `FUNCTIONS`.

For instance (see https://git.planet-casio.com/Lephenixnoir/JustUI/commit/626da6f378fd3763fe51dd9478f85d9f9ea4874c):

```c
//=== Macro version ==========================================================//
#include <justui/jwidget-api.h>
J_DEFINE_WIDGET(jpainted, csize, render)

void jpainted_poly_csize(void *p) { /* ... */ }
void jpainted_poly_render(void *p, int x, int y) { /* ... */ }

//=== Equivalent to ==========================================================//
static int jpainted_type_id = -1;

void jpainted_poly_csize(void *p) { /* ... */ }
void jpainted_poly_render(void *p, int x, int y) { /* ... */ }

static jwidget_poly type_jpainted = {
    .name    = "jpainted",
    .csize   = jpainted_poly_csize,
    .render  = jpainted_poly_render,
};

__attribute__((constructor))
static void j_register_jpainted(void) {
    jpainted_type_id = j_register_widget(&type_jpainted);
}
```

**Migration: event definitions**

To allow the complete removal of explicit constructor functions from widgets, events can also be defined by a macro.

```c
J_DEFINE_EVENTS(<NAMES>...)
```

This simply defines new variables `uint16_t NAME` for every `NAME` in the list of `NAMES` and registers them as events at startup with a constructor.

**Migration: focus system**

The focus system changed; see the [appropriate documentation](scene.md). The main changes are as follow.

Any widget that wants to receive keyboard input must set a focus policy, typically fixed at creation. This is either `J_FOCUS_POLICY_ACCEPT` if the widget has no children or `J_FOCUS_POLICY_SCOPE` if the widget contains children that may themselves want to receive keyboard focus. In most cases this is all that needs to be done (example with `jlist`: https://git.planet-casio.com/Lephenixnoir/JustUI/commit/57a460894f1395bd2b789f32c922fbc61e231a66#diff-94b157d9507dee40d44c23076d92c5ceb0b427d4)

```c
mywidget *mywidget_create(void *parent)
{
    if(mywidget_type_id < 0) return NULL;

    mywidget *w = malloc(sizeof *w);
    if(!w) return NULL;

    jwidget_init(&w->widget, mywidget_type_id, parent);
    jwidget_set_focus_policy(w, J_FOCUS_POLICY_ACCEPT); //< Set policy
    /* ... */
}
```

The events `JWIDGET_FOCUS_IN` and `JWIDGET_FOCUS_OUT` are replaced with a general `JWIDGET_FOCUS_CHANGED` event encompassing their meaning (among other things). The handler for such an event can call `jwidget_has_focus()` (example with `jinput`: https://git.planet-casio.com/Lephenixnoir/JustUI/commit/57a460894f1395bd2b789f32c922fbc61e231a66#diff-492b1daa1b4361eb231c82b4b1fca013601eb54c).

**Breaking features and major features**

* Rework widget focus model https://git.planet-casio.com/Lephenixnoir/JustUI/commit/57a460894f1395bd2b789f32c922fbc61e231a66 https://git.planet-casio.com/Lephenixnoir/JustUI/commit/7a5101360a86bc0d7d28e7a26f460fcea5f80dea
  - Widgets that want keyboard input must now specify a focus policy at creation
  - The events `JWIDGET_FOCUS_IN` and `JWIDGET_FOCUS_OUT` are replaced with a general `JWIDGET_FOCUS_CHANGED`
  - This also removes `JSCENE_KEY` in favor of `JWIDGET_KEY`, which were supposed to be the same but weren't, leading to confusing bugs https://git.planet-casio.com/Lephenixnoir/JustUI/commit/23294b77ac945ac35a2b9571e778f8193fd6f4de
* Replace widget definition process https://git.planet-casio.com/Lephenixnoir/JustUI/commit/216918123fc484793bef2e9d6072b13e129fa655 https://git.planet-casio.com/Lephenixnoir/JustUI/commit/683e89d7257d5d303071f9c63e238c8bb11400a9 https://git.planet-casio.com/Lephenixnoir/JustUI/commit/626da6f378fd3763fe51dd9478f85d9f9ea4874c
  - This removes the inheritance property
  - This modifies the convention for event handling functions, which should now default to `jwidget_poly_event()` https://git.planet-casio.com/Lephenixnoir/JustUI/commit/683e89d7257d5d303071f9c63e238c8bb11400a9 https://git.planet-casio.com/Lephenixnoir/JustUI/commit/93eb0df38aab9ebf1f71a4f27a53ba2039ed946d
  - Results in much less boilerplate
* fx-CP 400 build and basic compatibility (no touch input yet) https://git.planet-casio.com/Lephenixnoir/JustUI/commit/5b092a5a4ec2a74337ef6a98ccb4da3f658ab738 with correctly-sizes (but barely usable) F-keys https://git.planet-casio.com/Lephenixnoir/JustUI/commit/e12a58c1f0cd48cc1abd14630e4d44152e810948s
* WIP: Denotational UIs still a work-in-progress.

**Minor improvemements**

* `jfileselect`: Now displays "(No entries)" for empty folders https://git.planet-casio.com/Lephenixnoir/JustUI/commit/4c44b3e413e81ad443e02d2d136efd3207ffcb54 and error values when a filesystem error occurs https://git.planet-casio.com/Lephenixnoir/JustUI/commit/bea113f09e33fea6e9cb91a78a15a93b8c4842aa
* `jscene`: Now turns off with SHIFT+AC/ON, with option to disable https://git.planet-casio.com/Lephenixnoir/JustUI/commit/ef71bc11c0634fc575164e40bfe4afa6688c3fd9 https://git.planet-casio.com/Lephenixnoir/JustUI/commit/0c8371edceb768de6077f0fef2e2deaa912d2e8c
* `jscene`: Autopaint option https://git.planet-casio.com/Lephenixnoir/JustUI/commit/7f2131d6a03af4930dc85a3900a537940d4689cc
* `jscene`: Defaults to vbox layout instead of no layout https://git.planet-casio.com/Lephenixnoir/JustUI/commit/a2129f1ed208f5e982ae822e230e46a4b7406756
* `jinput`: Add function to customize keymap https://git.planet-casio.com/Lephenixnoir/JustUI/commit/0e5ccf4cc3723f47d592cf0ba2375edd954c24ad
* `jlabel`: Now recognizes the `font_t` property `line_distance` for inter-line spacing https://git.planet-casio.com/Lephenixnoir/JustUI/commit/33e99622096b892ae8406b160d19908e82f854cb
* `jlabel`: Defaults to top alignment https://git.planet-casio.com/Lephenixnoir/JustUI/commit/4728c6ecbe4c1fcfcd11fbab15078bf51828f384
* `jlabel`: No longer gobbles spaces after line breaks if the break is caused by an explicit `\n` https://git.planet-casio.com/Lephenixnoir/JustUI/commit/4728c6ecbe4c1fcfcd11fbab15078bf51828f384
* `jlist`: More fluid user code with default selection https://git.planet-casio.com/Lephenixnoir/JustUI/commit/3488c6515ab1d4f4c664b353362a82aa328decc9, info values https://git.planet-casio.com/Lephenixnoir/JustUI/commit/12b29f8223e91753250ad5e64a730e8c7ff2db19, and a user data pointer for rendering https://git.planet-casio.com/Lephenixnoir/JustUI/commit/7587dfa17cdc643881a3048a0935abe2f31d2936
* `jevent`: Provide functions for identifying key events (still looking for a good design there) https://git.planet-casio.com/Lephenixnoir/JustUI/commit/7b8070f02c01f8eb74487c4e61acf98f5b076cfc
* `jfkeys`: Now returns events instead of leaving keys F1...F6 pass through. This requires jfkeys to get key events, which needs support from the scene (in gintctl, gscreen handles it). https://git.planet-casio.com/Lephenixnoir/JustUI/commit/f28d7a9cb8e9a1249e761a7b97e2e6f5e9faa6b5
* Optimization: turn on LTO https://git.planet-casio.com/Lephenixnoir/JustUI/commit/5a885b541f7aad3dfcc76f522d4deec274cc91e4
* Optimization: full-screen widget with background (usually `jscene`) now renders with `dclear()` https://git.planet-casio.com/Lephenixnoir/JustUI/commit/f32dcc69ce9055606d8efff97150fed291ceadb5

**Fixes**

* Compatibility: less FX vs. CG hardcoding https://git.planet-casio.com/Lephenixnoir/JustUI/commit/e324f9a3a281d9ce29bb6d4953ba39763b52dbed
* `jframe`: Fix crash with NULL child https://git.planet-casio.com/Lephenixnoir/JustUI/commit/5e2488cdf4ac73761bc11612b10a37d0375e5ba9
* `jscene`: Interrupt-safe access to event queue https://git.planet-casio.com/Lephenixnoir/JustUI/commit/550c08e200fd6479e51d1afc7f625657a659d9db
* `jlist`, `jscrolledlist`: Fix parent not being last parameter https://git.planet-casio.com/Lephenixnoir/JustUI/commit/ba7b0a02d0701bd564294456452b897251bb2768
* Add checks to avoid the compiler inserting certain `abort()` calls https://git.planet-casio.com/Lephenixnoir/JustUI/commit/4d6b760b1ae6744332ce929321c6b0d0ea2b8f30
