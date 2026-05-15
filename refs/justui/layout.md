# JustUI: Space distribution and layout

Layout is the process through which space is allocated to widgets, which are
then sized and positioned.

## Content widgets and containers

Widgets in a scene usually fulfill one of two roles:

* **Content widgets** such as labels, input fields, or menu browsers, display
  the interface and receive events. They often have no children, and do their
  own rendering.
* **Containers** such as rows and columns, stacks, or grids, organize other
  widgets so they align nicely and don't overlap. The widgets they organize are
  their children; they themselves often perform no rendering.

JustUI does not enforce this separation, and a single widget can both handle
contents and organize children. But the features are usually designed with one
of the two roles in mind.

## Layouts

Layouts are parameters that can be attached to widgets to automatically size
and position their widgets in a useful fashion. This is mostly designed for
containers. There are currently 4 types of layouts:

* **Horizontal boxes** and **vertical boxes** arrange children in a row or a
  column, respectively. Each widget gets its desired size; if there is space
  left, widgets can expand according to stretch parameters (more on that
  later).
* **Stacks** arrange all the widgets on top of each other. Only one widget is
  visible at a time. This is useful for tabbed interfaces.
* **Grids** arrange all widgets in a grid. (TODO: WIP)

A widget that does not have a layout needs to manually determine its own
desired size as well as the position its children.

## The layout process

The layout process has two phases.

1. **Size estimation.** In the first phase, each widget declares a desired
   size. This size often depends on the size of the children, so this phase
   proceeds bottom-up: first the children declare their desired sizes, then the
   parents deduce their own desired sizes, and so on.

2. **Space distribution.** In the second phase, space is distributed by the
   parents to the children. If the parents have more available space than the
   children request, extra space is distributed as well. This phase is
   top-down: first the root distributes the available space to its children,
   then these children split their shares between their own children, etc.

All of this proceeds automatically unless some widgets cannot provide a natural
size (or only a useless one), in which case the user should give a hint.

## Internals

During the first phase, the content size of each widget is evaluated by either
the layout's `csize()` function, or the polymorphic widget's `csize()`
override. Then `jwidget_msize()` adds in the geometry and stores the margin-box
size in the `w` and `h` attributes.

During the second phase, `jwidget_layout_apply()` distributes the space by
dispatching to the layout's `apply()` function, or the polymorphic widget's
`apply()` override. It proceeds recursively in a depth-first, prefix order.

## Layout control

The widget type provides a natural content size, but the user has the final say
in the size of any widget. Any widget can have a minimum and maximum size
specified, and every layout guarantees that the allocated size falls in this
range (note that limits are examined during the second phase, and the natural
content size does not need to be in the acceptable range).

Additionally, the user can provide stretch rates indicating whether the widget
can use more space horizontally and vertically. When there is space to
distribute and several widgets are competing to use it, space is allocated in
proportion to stretch rates. For instance, a widget with double the stretch
rate of its competitors will get twice as much space as them.

In certain occasions, one might want to disregard the natural content size
entirely and distribute space based only on stretch rates, for instance to
split a screen in evenly-sized columns even when the contents of the columns
have different natural sizes. In this case, one can set the columns to a fixed
width of 0 while enabling stretching-beyond-limits. Stretching beyond limits
will allow the widgets to grow despite being of fixed size, and because they
all start with the same width of 0, they will all end up with the same size.
