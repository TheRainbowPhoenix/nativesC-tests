# jfkeys

JustUI.jfkeys: Row of function keys


## Functions


### `jfkeys_set2`

Create a set of function keys Both the image and text specification are provided; one of them should generally be NULL (if both are non-NULL, the image takes precedence). For historical reasons the name fkeys_create() is a one-argument version with either the text or image depending on the platform.


```c
void jfkeys_set2(jfkeys *keys, bopti_image_t const *img, char const *labels);
```


---


### `jfkeys_set2`

Replace the definition of function keys This will also reset the level to 0.


```c
void jfkeys_set2(jfkeys *keys, bopti_image_t const *img, char const *labels);
```


---


### `jfkeys_level`

Return the current function key level


```c
int jfkeys_level(jfkeys *keys);
```


---


### `jfkeys_set_level`

Set the function key level


```c
void jfkeys_set_level(jfkeys *keys, int level);
```


---


### `jfkeys_set_override`

The following functions are available only on fx-CG 50 and are no-ops on fx-9860G (you can't generate a good image for a tiny key).


```c
void jfkeys_set_override(jfkeys *keys, int key, char const *override);
```


---


### `jfkeys_set_override`

Get the override for a key


```c
void jfkeys_set_override(jfkeys *keys, int key, char const *override);
```


---


### `jfkeys_set_override`

Override the value of a single key on all levels This functions sets the override on the specified key, which replaces the label for that key on all levels. This is useful to conditionally show functions.


```c
void jfkeys_set_override(jfkeys *keys, int key, char const *override);
```


---


### `jfkeys_set_font`

Change the key and text colors * bg is the background color for MENU, ENTRY and ACTION keys (default C_BLACK), and the border color for SPECIAL keys. * bg_special is the background color for SPECIAL keys (default C_WHITE). * text is the text color for MENU, ENTRY and ACTION keys (default C_WHITE). * text_special is the text color for SPECIAL keys (default C_BLACK).


```c
void jfkeys_set_font(jfkeys *keys, font_t const *font);
```


---


## Data Structures


### `jfkeys`

jfkeys: Functions keys indicating functions for the F1..F6 keys

   This widget is the typical function key bar with a slightly different
   design. There are four types of keys, with conventional guidelines:

   * MENU KEYS are used for functions that open menus or navigate between tabs
     on a same application. The name comes from their primary usage in the
     system apps. Navigation functions should be easily reversible and fairly
     failproof. Menu keys are black rectangular keys with a chipped corner.

   * ENTRY KEYS are used for catalog entries such as the leaves of PRGM's many
     nested input menus. They represent entries to be chosen from. Entry keys
     are black rectangular keys.

   * ACTION KEYS are used for generic safe and unsafe actions. Action keys are
     black round keys.

   * SPECIAL KEYS are used for special functions, such as scrolling to the next
     set of functions keys when there are several pages, important functions
     that should catch attention, or particularly unsafe actions. They are
     round white keys.

   The more flexible option to draw the keys is dynamically with text. In that
   case each key are specified with a strings that combines a type and name:

   * "/NAME" for a menu key;
   * ".NAME" for an entry key;
   * "@NAME" for an action key;
   * "#NAME" for a special key.

   The names are separated by semicolons, eg. "/F1;;/F3;.F4;@F5;#F6". Several
   sets of function keys can be defined if separated by a '|' character. For
   instance, "/F1;#F2|/F1" represents a function bar where the F2
   function can be hidden by switching from level 0 to level 1.

   The other option is to draw the keys with an image. This is most useful at
   small resolutions where hand-drawn keys often look better. On the fx-9860G
   with its 128x64 resolution, the convention is that the image is 128x8, and
   key #i is positioned at x = 21i+2 with width 19. The equivalent of "|"-
   separated levels is allowed by stacking up rows of keys (in which case the
   image is of height 9n-1 for n rows).

   jkfeys will gobble keyboard events for F1..F6 and emit JFKEYS_TRIGGERED
   events instead. However, in general jfkeys doesn't have keyboard focus, so
   you have to give the events manually.


**Fields**:

- `jwidget widget`

- `int8_t level`

- `/* Image version`

- `if specified this overrides all text parameters */

	bopti_image_t const *img`

- `/* Text version */

	char const *labels`

- `char const *overrides[6]`

- `int bg_color, bg_special_color`

- `int text_color, text_special_color`

- `font_t const *font`


```c
struct jfkeys {
jwidget widget;
	int8_t level;

	/* Image version; if specified this overrides all text parameters */

	bopti_image_t const *img;

	/* Text version */

	char const *labels;
	char const *overrides[6];

	int bg_color, bg_special_color;
	int text_color, text_special_color;

	font_t const *font;
};
```


---
