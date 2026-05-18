# jframe

JustUI.jframe: Scrolling frame holding a widget


## Functions


### `*jframe_create`

Create a new frame The frame's inner widget is always its first child. It can be specified by jwidget_set_parent() or by creating the child with the frame as a parent directy. More children can be added, but they will not be rendered.


```c
jframe *jframe_create(void *parent);
```


---


### `jframe_set_align`

Trivial properties


```c
void jframe_set_align(jframe *f, jalign halign, jalign valign);
```


---


### `jframe_scroll_to_region`

Scroll a region of the child into view This functions scrolls the frame to ensure that the specified region of the child widget is visible within the frame (minus the visibility margin). The purpose of the visibility margin is to avoid aligning important regions of the child widget along the edges of the frame unless we reach the edge of the child widget. For example, with a scrolling list, we want the selected item to be somewhat off the edge of the frame so that items around it are visible. Showing the selected item right on the edge of the frame suggests to the user that there are no items beyond it. If either dimension of the provided region is larger than the content size of the frame minus the visibility margin, the center of the region will be shown at the center of the view along that direciton. Otherwise, the view will scroll the minimum amount possible to bring the region into view. If clamp is set to false, the frame will allow scrolling beyond current boundaries, which is helpful as a hack when calling this function while a layout is occuring.


```c
void jframe_scroll_to_region(jframe *f, jrect region, bool clamp);
```


---


## Data Structures


### `jframe`

jframe: Scrolling frame holding a widget

   This widget is used to implement scrolling widgets. It has a single child,
   which is displayed fully if it's smaller than the frame, or partially (with
   scrollbars) otherwise.

   The child widget has horizontal and vertical alignments, which specify its
   position within the frame when smaller than the frame. Its position when
   larger than the frame is determined by the scrolling offsets, which can be
   manipulated manually or left for the frame to control with arrow keys.

   Scrollbars can be set to either render on top of the framed widget, or
   occupy dedicated space.


**Fields**:

- `jwidget widget`

- `/* Horizontal and vertical alignment for the child */
    jalign halign, valign`

- `/* Force scrollbars even if the child is smaller than the frame */
    bool scrollbars_always_visible`

- `/* Scrollbars render on top of the child widget */
    bool floating_scrollbars`

- `/* Scrolling can be handled by the frame itself, with arrow keys */
    bool keyboard_control`

- `/* Force matching the width and/or height of the child widget */
    bool match_width, match_height`

- `/* Scrollbar width, in pixels */
    uint8_t scrollbar_width`

- `/* If floating_scrollbars == false, spacing between scrollbars and child */
    uint8_t scrollbar_spacing`

- `/* Visibility margin (see jframe_scroll_to_region()) */
    uint8_t visibility_margin_x, visibility_margin_y`

- `/* Whether scrollbars are shown */
    bool scrollbar_x, scrollbar_y`

- `/* Current scroll offsets */
    int16_t scroll_x, scroll_y`

- `/* Maximum scroll offsets for the current size of the child widget */
    int16_t max_scroll_x, max_scroll_y`


```c
struct jframe {
jwidget widget;

    /* Horizontal and vertical alignment for the child */
    jalign halign, valign;
    /* Force scrollbars even if the child is smaller than the frame */
    bool scrollbars_always_visible;
    /* Scrollbars render on top of the child widget */
    bool floating_scrollbars;
    /* Scrolling can be handled by the frame itself, with arrow keys */
    bool keyboard_control;
    /* Force matching the width and/or height of the child widget */
    bool match_width, match_height;
    /* Scrollbar width, in pixels */
    uint8_t scrollbar_width;
    /* If floating_scrollbars == false, spacing between scrollbars and child */
    uint8_t scrollbar_spacing;

    /* Visibility margin (see jframe_scroll_to_region()) */
    uint8_t visibility_margin_x, visibility_margin_y;

    /* Whether scrollbars are shown */
    bool scrollbar_x, scrollbar_y;
    /* Current scroll offsets */
    int16_t scroll_x, scroll_y;
    /* Maximum scroll offsets for the current size of the child widget */
    int16_t max_scroll_x, max_scroll_y;
};
```


---
