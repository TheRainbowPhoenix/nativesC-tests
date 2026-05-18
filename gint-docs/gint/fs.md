# fs

gint:fs - Filesystem abstraction

## Functions

### `fs_create_descriptor`

Create a new file descriptor This function is used in open() and its variants to allocate new file descriptors. The descriptor's data is created with a copy of the provided structure, which must include a non-NULL type attribute. This function always returns the smallest file descriptor that is unused by the application and is not 0, 1 or 2; unless it runs out of file descriptors, in which case it returns -1 and the caller might want to set errno = ENFILE.

```c
int fs_create_descriptor(fs_descriptor_t const *data);
```

---

### `fs_free_descriptor`

Close a file descriptor's slot This function frees the specified file descriptor. It is automatically called by close() after the descriptor type's close() function, so there is normally no need to call this function directly.

```c
void fs_free_descriptor(int fd);
```

---

### `open_generic`

Open a file descriptor using custom file functions Opens a new file descriptor of the specified type with the provided user data. If reuse_fd < 0, a new file descriptor is allocated, otherwise the exact file descriptor reuse_fd is used. (This is useful to reopen standard streams.) In this case, the only possible return values are -1 and reuse_fd itself.

```c
int open_generic(fs_descriptor_type_t const *type, void *data, int reuse_fd);
```

---

### `*fs_path_normalize`

Normalize a path to eliminate ., .. and redundant / This function creates a copy of the specified path which is an absolute path with redundant / removed and . and .. components resolved. For instance: "" -> "/" "/subfolder/./" -> "/subfolder" "/../subfolder/../subfolder" -> "/subfolder" "/../../x/y/../t" -> "/x/t" The new string is created with malloc() and should be free()'d after use.

```c
char *fs_path_normalize(char const *path);
```

---

### `*fs_path_normalize_fc`

Normalize a path and translate it to FONTCHARACTER This function is similar to fs_path_normalize(), but it creates a 16-bit string that starts with \\fls0\ rather than /, and can be used in direct calls to BFile.

```c
uint16_t *fs_path_normalize_fc(char const *path);
```

---

## Data Structures

### `fs_descriptor_type_t`

Maximum number of file descriptors */
#define FS_FD_MAX 16

/* fs_descriptor_type_t: Overloaded file descriptor functions

   This structure provides the base functions for every type of file
   descriptor. The prototypes are standard, except that the first argument
   (int fd) is replaced by (void *data) which points to the custom data
   allocator for the descriptor.

**Fields**:

- `/* See <unistd.h> for a description of these functions */
    ssize_t (*read)(void *data, void *buf, size_t size)`

- `ssize_t (*write)(void *data, void const *buf, size_t size)`

- `off_t (*lseek)(void *data, off_t offset, int whence)`

- `int (*close)(void *data)`

```c
struct fs_descriptor_type_t {
/* See <unistd.h> for a description of these functions */
    ssize_t (*read)(void *data, void *buf, size_t size);
    ssize_t (*write)(void *data, void const *buf, size_t size);
    off_t (*lseek)(void *data, off_t offset, int whence);
    int (*close)(void *data);
};
```

---

### `fs_descriptor_t`

fs_descriptor_t: File descriptor information
   This internal type describes the entries of the descriptor table.

**Fields**:

- `/* Interface functions */
    fs_descriptor_type_t const *type`

- `/* Custom data (can also be an integer cast to (void *)) */
    void *data`

```c
struct fs_descriptor_t {
/* Interface functions */
    fs_descriptor_type_t const *type;
    /* Custom data (can also be an integer cast to (void *)) */
    void *data;
};
```

---

## Macros

### `FS_FD_MAX`

Maximum number of file descriptors

```c
#define FS_FD_MAX 16
```

---

## Implementation

Source files:

- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/fs/read.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/read.c)
- [src/fs/fs.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/fs.c)
- [src/fs/lseek.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/lseek.c)
- [src/fs/pread.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/pread.c)
- [src/fs/fdopendir.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/fdopendir.c)
- [src/fs/write.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/write.c)
- [src/fs/close.c](https://github.com/ClasspadDev/gint/blob/dev/src/fs/close.c)
