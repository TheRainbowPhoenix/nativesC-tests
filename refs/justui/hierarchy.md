# JustUI: Widget hierarchy

## The tree hierarchy

Every widget (either of type `jwidget` or of a structure type that begins with
a `jwidget`) has a parent. This relationship forms a tree. The widgets that
have a common parent *w* are called the children of *w*.

The motivation for the widget hierarchy is to have groups of widgets behave as
one. A complex UI element can have many widgets to implement its complex
interface, but group them together as children of a parent and expose only the
parent. This is an essential tool to build complex interfaces out of smaller
components.

When a widget is created, for instance with `jwidget_create()` or
`jlabel_create()`, its parent is specified as the last parameter.

In the tree one of the widgets is the ancestor of all the others, and is called
the *root* of the scene. In JustUI the root of the tree is normally a `jscene`,
because that is how event handling and keyboard input are managed.

## Managing ownership

Every widget *owns* its children with regards to memory allocation. This means
that when a widget is destroyed, its children are destroyed along with it.
Thus, even though the user program performs a function call for each widget to
allocate in a scene, a single call freeing the root will destroy all of them.

Whenever children are moved around, they change ownership. This is important
because we always want the parent to outlive the children. Specifically, if a
source widget holds a pointer of reference to a target widget, then the source
should make sure that either the target is one of its parents, or it is
informed when the target is destroyed.
