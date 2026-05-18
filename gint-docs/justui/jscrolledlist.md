# jscrolledlist

JustUI.jscrolledlist: A jlist inside a jframe


## Data Structures


### `jscrolledlist`

jscrolledlist: A jlist inside a jframe

   jlist as a variabled-size widget which is intended to be used inside a
   scrolling view like a jframe. However this still requires the jframe to
   scroll when the list cursor moves and when the model is refreshed.

   This utility widget does this wrapping. It does not have any specific
   functions, and instead returns the list and frame as its `->list` and
   `->frame` members.


**Fields**:

- `jwidget widget`

- `jframe *frame`

- `jlist *list`


```c
struct jscrolledlist {
jwidget widget;
    jframe *frame;
    jlist *list;
};
```


---
