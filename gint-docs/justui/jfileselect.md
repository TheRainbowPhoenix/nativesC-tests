# jfileselect

JustUI.jfileselect: Basic file selector

## Functions

### `*jfileselect_create`

Event IDs

```c
jfileselect *jfileselect_create(void *parent);
```

---

### `*jfileselect_create`

Create a file selection interface There is no initial folder. The widget will not handle any events nor emit any events in this state; a path must first be set before use.

```c
jfileselect *jfileselect_create(void *parent);
```

---

### `jfileselect_set_saveas`

Select whether a "Save as" option is presented If true, the browser will show a "<Create a new file here>" entry in every directory. The result of jfileselect_selected_file(), in this case, might be a file that does not yet exist.

```c
void jfileselect_set_saveas(jfileselect *fs, bool save_as);
```

---

### `jfileselect_browse`

Browse a folder This function loads the specified folder and allows the user to select a file. (Remember to give the widget focus.) A JFILESELECT_LOADED event is emitted immediately, and further events are emitted based on user inputs. This function resets the selected file to NULL. Returns true on success, false if the path does not exist or cannot be browsed (in that case, check errno).

```c
bool jfileselect_browse(jfileselect *fs, char const *path);
```

---

### `jfileselect_default_filter`

Set a filter function The function is called on each directory entry read when scanning a folder. It should return true to show the entry, false to ignore it. By default, jfileselect_default_filter is used. Note filters in general should really accept folders.

```c
bool jfileselect_default_filter(struct dirent const *entry);
```

---

### `jfileselect_default_filter`

Default filter. Rejects "@MainMem", "SAVE-F", "." and "..".

```c
bool jfileselect_default_filter(struct dirent const *entry);
```

---

### `jfileselect_set_font`

Trivial properties

```c
void jfileselect_set_font(jfileselect *fs, font_t const *font);
```

---

## Data Structures

### `jfileselect`

jfileselect: Basic file selector

   This widget is used to browse the filesystem and select a file. Visually, it
   only consists of a scrolling list of names showing a section of a folder's
   entries.

   Events:
   * JFILESELECT_LOADED when a folder is loaded into the view
   * JFILESELECT_VALIDATED when a file has been selected
   * JFILESELECT_CANCELED when if the user exits from the top-level folder

**Fields**:

- `jwidget widget`

- `/* Folder currently being browsed */
    char *path`

- `/* List of entries */
    void *entries`

- `/* Number of entries in the current folder */
    int entry_count`

- `/* Error code for loaded folder */
    int folder_error`

- `/* Full path to file last selected with EXE */
    char *selected_file`

- `/* "Save As" input field */
    jinput *saveas_input`

- `/* Whether the "Save As" option is shown */
    bool saveas`

- `/* Whether we are currently using the input field */
    bool input_mode`

- `/* Scrollbar with (0 to disable) */
    int8_t scrollbar_width`

- `/* File filter`

- `NULL accepts everything */
    bool (*filter_function)(struct dirent const *entry)`

- `/* Current cursor position (0 .. folder_entries-1) */
    int16_t cursor`

- `/* Index of the currently touch-clicked item, -1 none */
    int touch_cursor`

- `/* Current scroll position */
    int16_t scroll`

- `/* Number of visible lines */
    int8_t visible_lines`

- `/* Additional pixels of spacing per line (base is font->height) */
    int8_t line_spacing`

- `/* Whether to show the file size on the right */
    bool show_file_size`

- `/* Rendering font */
    font_t const *font`

```c
struct jfileselect {
jwidget widget;

    /* Folder currently being browsed */
    char *path;
    /* List of entries */
    void *entries;
    /* Number of entries in the current folder */
    int entry_count;
    /* Error code for loaded folder */
    int folder_error;

    /* Full path to file last selected with EXE */
    char *selected_file;
    /* "Save As" input field */
    jinput *saveas_input;
    /* Whether the "Save As" option is shown */
    bool saveas;
    /* Whether we are currently using the input field */
    bool input_mode;
    /* Scrollbar with (0 to disable) */
    int8_t scrollbar_width;
    /* File filter; NULL accepts everything */
    bool (*filter_function)(struct dirent const *entry);

    /* Current cursor position (0 .. folder_entries-1) */
    int16_t cursor;
    /* Index of the currently touch-clicked item, -1 none */
    int touch_cursor;
    /* Current scroll position */
    int16_t scroll;
    /* Number of visible lines */
    int8_t visible_lines;

    /* Additional pixels of spacing per line (base is font->height) */
    int8_t line_spacing;
    /* Whether to show the file size on the right */
    bool show_file_size;
    /* Rendering font */
    font_t const *font;
};
```

---

## Implementation

Implementation is in the gint source tree.
