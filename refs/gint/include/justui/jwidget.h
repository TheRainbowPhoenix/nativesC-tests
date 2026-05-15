//---
// JustUI.jwidget: Base object for all widgets
//---

#ifndef _J_JWIDGET
#define _J_JWIDGET

#include <justui/defs.h>
#include <justui/jlayout.h>

/* jwidget: Base object for all widgets.

   Functions that ought to work on any widget take void * parameters; this
   includes jwidget and derived types whose first attribute is a jwidget. A
   void * parameter named "w" is implicitly a widget.

   Widgets for a scene are arranged in a tree hierarchy; each object has a
   unique parent and a number of distinct children, which they own. Widgets
   destroy their children when they are destroyed. The parent of all the
   widgets in the scene is called the root, it's the only one without a parent.

   Every widget has an HTML-like box model described by its geometry object.
   See the jwidget_geometry type below for details. By default widgets have no
   geometry to save memory, it is only created when actually used.

   The size of each widget can be controlled in two ways. Initially, the
   widget's contents suggest a "natural size". The user can then restrict
   acceptable sizes by specifying a size hint along with a policy. See the
   jwidget_size_policy type below for details. Additionally, the user can set
   stretch parameters to allow the widget to grow and occupy available space.

   Widgets usually have one of two roles; either they are "containers" for
   other widgets, or they are "content widgets" that render text and images, or
   take input from the user. In order to make containers easy to design, each
   widget can be equipped with a "layout" that automatically arranges children
   in useful ways, such as vertical lists or grids. Content widgets usually
   don't have layouts.

   Polymorphic operations are defined for each widget type and accessed through
   the type attribute. See the widget extension API at the end of this header
   for details on polymorphic operations.

   The following attributes can be accessed by the user:
     .parent       (read-only)
     .children[]   (read-only)
     .child_count  (read-only) */
typedef struct jwidget {
	/* Parent and children in the widget tree */
	struct jwidget *parent;
	struct jwidget **children;

	/* Location within the content-box of the parent (after layout) */
	int16_t x, y;
	/* Margin-box size in pixels (after layout) */
	int16_t w, h;
	/* Size hints: these are user-provided sizes, which are combined with the
	   size policy to determine acceptable widget dimensions */
	int16_t min_w, min_h;
	int16_t max_w, max_h;

	/* Widget geometry, defaults to a fixed geometry object */
	struct jwidget_geometry *geometry;
	/* Layout data, access with the jlayout_{get,set}_*() functions) */
	union {
		jlayout_box   layout_box;
		jlayout_stack layout_stack;
		jlayout_grid  layout_grid;
	};

	/* Focused subwidget for focus scopes */
	struct jwidget *focus_scope_target;

	/* Widget type, used to find polymorphic operations */
	uint8_t type;
	/* Number of children */
	uint8_t child_count;
	/* Number of pointers allocated in the children array */
	uint8_t child_alloc;
	/* Horizontal and vertical stretch rates */
	uint stretch_x      :4;
	uint stretch_y      :4;

	/* Type of layout (see the jlayout_type enum) */
	uint layout         :4;
	/* Whether stretch can go beyond the maximum size */
	uint stretch_force  :1;
	/* Whether the layout needs to be recomputed */
	uint dirty          :1;
	/* Whether the widget needs to be redrawn */
	uint update         :1;
	/* Whether widget is visible inside its parent */
	uint visible        :1;
	/* Widget is floating outside the layout (and positioned manually) */
	uint floating       :1;
	/* Widget is clipped during rendering */
	uint clipped        :1;
	/* Focus policy */
	uint focus_policy   :2;
	/* Focus flags */
	uint focused        :1;
	uint active_focused :1;

	uint :18;

} jwidget;

/* jwidget_border_style: Styles of widget borders */
typedef enum {
	/* No border */
	J_BORDER_NONE,
	/* Border is a solid color */
	J_BORDER_SOLID,
	/* TODO: More border styles (especially on fx-CG 50) */

} jwidget_border_style;

/* jwidget_focus_policy: Options for receiving and handling keyboard focus */
typedef enum {
	/* The widget does not accept focus or interact with keyboard (default) */
	J_FOCUS_POLICY_REJECT,
	/* The widget accepts keyboard input and hides it from descendants */
	J_FOCUS_POLICY_ACCEPT,
	/* The widget accepts keyboard focus and forwards it to a descendant */
	J_FOCUS_POLICY_SCOPE,

} jwidget_focus_policy_t;

/* jwidget_geometry: Built-in positioning and border geometry

   Every widget has a "geometry", which consists of a border and two layers of
   spacing around the widget:
   * The "padding" is the spacing between the widget's contents and its border
   * The "border" is a visible decoration around the widget's contents
   * The "margin" is the spacing between the border and the widget's edge

   For users familiar with the CSS box model, this is it. Note, however, that
   unlike the common CSS property (box-sizing: border-box), JustUI counts the
   margin as widget-owned space and measures widgets using the margin box. */
typedef struct jwidget_geometry {
	/* Padding (in pixels) on all four sides; access either using padding.top,
	   .right, .bottom and .left, or using paddings[0] through paddings[3] */
	union {
		uint8_t paddings[4];
		jdirs padding;
	};
	/* Width of the border on all four sides */
	union {
		uint8_t borders[4];
		jdirs border;
	};
	/* Size of the margin (in pixel) on all four sides */
	union {
		uint8_t margins[4];
		jdirs margin;
	};
	/* Border color (as in <gint/display.h>) */
	int border_color;
	/* Border style */
	jwidget_border_style border_style;
	/* Background color */
	int background_color;

} jwidget_geometry;

/* Key press event; the widget has active focus and a key was pressed.
    -> .data.key: Key event */
extern uint16_t JWIDGET_KEY;
/* The widget's focus state (either .focused or .active_focused) changed. */
extern uint16_t JWIDGET_FOCUS_CHANGED;
/* The widget is a scope and its target changed. */
extern uint16_t JWIDGET_FOCUS_TARGET_CHANGED;

//---
// Creation and destruction
//---

/* jwidget_create(): Create a widget

   This function creates a type-less widget. If you want to create labels,
   buttons, input fields... you need to use the specific creation functions
   such as jlabel_create(). This function only creates empty widgets, which are
   primarily useful as containers.

   If a non-NULL parent is specified, then the new widget becomes a child of
   that parent. The parent will then handle the positioning and sizing of the
   new widget, and destroy it automatically.

   After creating a container with jwidget_create(), it is common to give it a
   layout and add children to it, either with jwidget_add_child() or through
   the children's constructors.

   @parent  This widget's parent.
   -> Returns the new widget (NULL on error). */
jwidget *jwidget_create(void *parent);

/* jwidget_destroy(): Destroy a widget
   This function destroys the specified widget and its children. If the
   destroyed widget has a parent, the parent is notified, so the widget tree
   cannot become invalid. However, the layout process should be re-run to
   layout the remaining scene elements. */
void jwidget_destroy(void *w);

//---
// Widget tree manipulation
//---

/* jwidget_set_parent(): Change a widget's parent
   Moves the widget from its current parent to another parent. If the widget
   already had a parent, it is notified. If the new parent is NULL, the widget
   is left without a parent. */
void jwidget_set_parent(void *w, void *parent);

/* jwidget_add_child(): Add a child at the end of a widget's child list
   If the widget already had a parent, that parent is notified. */
void jwidget_add_child(void *w, void *child);

/* jwidget_insert_child(): Insert a child in the widget's child list
   Similar to jwidget_add_child(), but the child is added at the requestd
   position in the parent's child list. The position must be in the range
   [0 ... w->child_count]. */
void jwidget_insert_child(void *w, void *child, int position);

/* jwidget_remove_child(): Remove a child from a widget
   (w) must be the parent of (child). The child is left without a parent. */
void jwidget_remove_child(void *w, void *child);

/* jwidget_child_position(): Find the position of a widget in the child list */
int jwidget_child_position(void *w, void *child);

//---
// Sizing and stretching
//---

/* Functions to set the minimum width, minimum height, or both. The minimum
   size can be cleared by specifying 0. */
void jwidget_set_minimum_width(void *w, int min_width);
void jwidget_set_minimum_height(void *w, int min_height);
void jwidget_set_minimum_size(void *w, int min_width, int min_height);

/* Functions to set the maximum width, maximum height, or both. The maximum
   size can be cleared by specifying -1. */
void jwidget_set_maximum_width(void *w, int max_width);
void jwidget_set_maximum_height(void *w, int max_height);
void jwidget_set_maximum_size(void *w, int max_width, int max_height);

/* Functions to set both the minimum and maximum size at the same time. */
void jwidget_set_fixed_width(void *w, int width);
void jwidget_set_fixed_height(void *w, int height);
void jwidget_set_fixed_size(void *w, int width, int height);

/* jwidget_set_stretch(): Set the stretch factors for a widget

   Stretch factors indicate how much a widget wants to grow. Stretch is used in
   all size policies except the fixed one. Due to storage limits, the stretch
   factors should be in the range [0 ... 15].

   The last parameter indicates whether to allow stretching beyond the maximum
   size of the widget. In almost all situations this should be false. However,
   in some cases you might want to completely ignore natural size and allocate
   space based uniquely on stretch factors. In this case, you can set a fixed
   size of 0 and enable stretching beyond limits. */
void jwidget_set_stretch(void *w, int stretch_x, int stretch_y,
	bool stretch_beyond_limits);

//---
// Geometry
//--

/* jwidget_geometry_r(): Get read-only access to a widget's geometry

   This function returns a read-only pointer to the specified widget's
   geometry. Because most widgets don't have customized geometry, JustUI
   doesn't store any data until the geometry is modified, to save memory. This
   is why it makes sense to separate read-only and read-write accesses.

   For widgets without customized geometry, this functions returns a pointer to
   a fixed constant geometry with zero padding, border and margin. */
jwidget_geometry const *jwidget_geometry_r(void *w);

/* jwidget_geometry_rw(): Get read-write access to a widget's geometry

   This function returns a read-write pointer to the specified widget's
   geometry. For widgets that don't have customized geometry yet, this will
   duplicate the default settings. This avoids memory consumption on widgets
   that don't need custom geometry.

   Returns NULL if duplication fails because of memory exhaustion. */
jwidget_geometry *jwidget_geometry_rw(void *w);

/* jwidget_set_border(): Set a uniform border around a widget
   This is a shorthand to set (border_style), (border_color), and a uniform
   border width on a widget's geometry. */
void jwidget_set_border(void *w, jwidget_border_style s, int width, int color);

/* jwidget_set_borders(): Set all four borders */
void jwidget_set_borders(void *w, jwidget_border_style s, int color,
   int top, int right, int bottom, int left);

/* jwidget_set_padding(): Set all padding distances around a widget */
void jwidget_set_padding(void *w, int top, int right, int bottom, int left);

/* jwidget_set_margin(): Set all margin distances around a widget */
void jwidget_set_margin(void *w, int top, int right, int bottom, int left);

/* jwidget_set_background(): Set the widget's background color
   The default is C_NONE. The background covers content and padding. */
void jwidget_set_background(void *w, int color);

//---
// Layout
//---

/* jwidget_layout_dirty(): Check whether the tree needs to be laid out

   This function checks the dirty bit of every widget in the tree. If any
   widget changes size, the whole tree needs to be laid out again (there are
   possible optimizations, but they are not implemented yet). */
bool jwidget_layout_dirty(void *scene_root);

/* jwidget_layout(): Layout a widget tree

   This function lays out the specified widget (computing its size and the
   position of its children) and its children recursively. Because this is a
   two-phase process going from the children to their parents and then from the
   parents to their children, it only makes sense to layout the whole tree at
   once. You should thus call jwidget_layout() only with your scene root.

   A scene's layout should always be up-to-date before rendering. There is no
   need to layout at every frame (this would be a waste of resources), but you
   need to layout after doing any of the following things:

   * Creating the scene
   * Adding, removing, or moving visible children around
   * Changing a widget's contents in a way that affects its natural size
   * Changing geometry or layout parameters on a widget

   The layout process determines the size and position of every widget in the
   tree. Thus, if you need to access this size and position information, you
   need to keep the layout up-to-date before doing it. */
void jwidget_layout(void *scene_root);

/* jwidget_absolute_x(): Absolute x-position of a widget
   jwidget_absolute_y(): Absolute y-position of a widget
   jwidget_absolute_padding_x(): Absolute x-position of a widget's padding box
   jwidget_absolute_padding_y(): Absolute y-position of a widget's padding box
   jwidget_absolute_content_x(): Absolute x-position of a widget's content box
   jwidget_absolute_content_y(): Absolute y-position of a widget's content box

   These are actually based on the root's coordinates, which are typically 0,0
   with a full-screen scene. */
int jwidget_absolute_x(void *w);
int jwidget_absolute_y(void *w);
int jwidget_absolute_padding_x(void *w);
int jwidget_absolute_padding_y(void *w);
int jwidget_absolute_content_x(void *w);
int jwidget_absolute_content_y(void *w);

/* jwidget_width(): With of a widget's content box
   jwidget_height(): Height of a widget's content box

   These functions return the size of the content box of a widget. The content
   box does not comprise the geometry (padding, border and margins). These
   dimensions are known only after layout; calling these functions when the
   layout is not up-to-date will return funny results. */
int jwidget_content_width(void *w);
int jwidget_content_height(void *w);

/* jwidget_width(): With of a widget's padding box
   jwidget_height(): Height of a widget's padding box

   These functions return the size of the padding box of a widget, which
   includes the content and padding. These are the dimensions that appear as
   the widget's size when there is a background. These dimensions are known
   only after layout; calling these functions when the layout is not up-to-date
   will return funny results. */
int jwidget_padding_width(void *w);
int jwidget_padding_height(void *w);

/* jwidget_full_width(): Width of a widget's margin box
   jwidget_full_height(): Height of a widget's margin box

   These functions return the whole size of the margin box of a widgets; this
   includes the contents, padding, border and margins. These functions only
   make sense to call when the layout is up-to-date. */
int jwidget_full_width(void *w);
int jwidget_full_height(void *w);

/* jwidget_floating(): Whether widget is floating
   A floating widget is not laid out by its parent's layout, and can be
   positioned manually. Floating children are rendered after other children. */
bool jwidget_floating(void *w);

/* jwidget_set_floating(): Make a widget floating or non-floating */
void jwidget_set_floating(void *w, bool floating);

//---
// Rendering
//---

/* jwidget_visible(): Whether widget is visible
   A non-visible widget occupies no space and is not rendered, as if it did not
   exist at all. */
bool jwidget_visible(void *w);

/* jwidget_set_visible(): Hide or show a widget */
void jwidget_set_visible(void *w, bool visible);

/* jwidget_clipped(): Whether widget is clipped

   If a widget is clipped then its rendering function cannot draw pixels
   outside of its bounding box. There is no performance cost to this feature
   because it relies on underlying gint rendering functions already supporting
   clipping.

   This is disabled by default because it is convenient to have widgets draw
   outside their bounding box. For instance it is easier to align a single-
   line label by setting the font's bearing as its height, and then drawing
   glyphs' tails outside the bouding box. It is also harder to spot layout
   issues if the widgets are clipped away. */
bool jwidget_clipped(void *w);

/* jwidget_set_clipped(): Set a widget's rendering clipping preference */
void jwidget_set_clipped(void *w, bool clipped);

/* jwidget_needs_update(): Check whether the tree needs to be re-rendered

   If this function returns true, you should re-render the tree. Aditionally,
   if jwidget_layout_dirty() returns true, you should re-layout the tree and
   repaint it. jscene_render() will layout automatically if needed, so you just
   need to call it if either function returns true.

   When using jscene_run(), a JSCENE_REPAINT event will be emitted in this
   exact conditions, so just jscene_render() upon JSCENE_REPAINT. */
bool jwidget_needs_update(void *w);

/* jwidget_render(): Render a widget
   This function renders the widget. The specified location (x,y) is the
   top-left corner of the margin box of the widget. There is no clipping.
   Unlike jscene_render(), this function does not automatically layout the
   widgets if there has been changes. */
void jwidget_render(void *w, int x, int y);

//---
// Keyboard focus
//---

/* Change the widget's focus policy. This is usually done in the constructor
   but can be done anytime. If the policy is changed to JWIDGET_FOCUS_REJECT
   while the widget has focus, the focus will be lost. */
void jwidget_set_focus_policy(void *w, jwidget_focus_policy_t fp);

/* Check whether a widget accepts focus. */
bool jwidget_accepts_focus(void *w);

/* Check whether a widget is currently focused within its surrounding scope. */
GINLINE static bool jwidget_has_focus(void *w)
{
	return ((jwidget *)w)->focused;
}
/* Check whether a widget is current in the key event propagation chain. */
GINLINE static bool jwidget_has_active_focus(void *w)
{
	return ((jwidget *)w)->active_focused;
}

/* Change the target of a focus scope widget. */
void jwidget_scope_set_target(void *fs, void *target);

/* Get the target of a focus scope, NULL if none or not a scope. */
GINLINE static jwidget *jwidget_scope_get_target(void *fs)
{
	return ((jwidget *)fs)->focus_scope_target;
}

/* Clear the target of a focus scope widget. */
GINLINE static void jwidget_scope_clear_focus(void *fs)
{
	return jwidget_scope_set_target(fs, NULL);
}

/* Context function that returns the immediately surrounding scope that owns
   this widget, NULL if there's none. Since jscene is a scope, in general there
   should always be one. */
jwidget *jwidgetctx_enclosing_focus_scope(void *w);

/* Context function that gives w focus within its enclosing focus scope. */
void jwidgetctx_grab_focus(void *w);

/* Context function that drops the focus from w (if it has it) within its
   enclosing focus scope. */
void jwidgetctx_drop_focus(void *w);

//---
// Misc
//---

/* jwidget_type(): Get a widget's human-readable type name
   This is the name specified in the jwidget_poly structure for the type. */
char const *jwidget_type(void *w);

#endif /* _J_JWIDGET */
