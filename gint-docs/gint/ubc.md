# ubc

gint:ubc - User Break Controller driver

## Functions

### `ubc_setDBR`

Read and write the DBR register

```c
void ubc_setDBR(void* DBR);
```

---

### `ubc_dbh`

Low level UBC debug handler The handler will backup the current CPU state and call ubc_debug_handler().

```c
void ubc_dbh(void);
```

---

### `ubc_debug_handler`

C level UBC debug handler The execution will be redirected to the handler set by the user or the break will be ignored in case no handler has been set.

```c
void ubc_debug_handler(gdb_cpu_state_t* cpu_state);
```

---

### `ubc_set_breakpoint`

Break before the instruction is executed

```c
bool ubc_set_breakpoint(int channel, void* break_address, ubc_break_mode_t break_mode);
```

---

### `ubc_set_breakpoint`

Break after the instruction is executed : at this point PC will point to the next instruction.

```c
bool ubc_set_breakpoint(int channel, void* break_address, ubc_break_mode_t break_mode);
```

---

### `ubc_set_breakpoint`

Set a breakpoint in a UBC channel and enable it Return false when an invalid channel number is provided, true if the breakpoint was correctly set up.

```c
bool ubc_set_breakpoint(int channel, void* break_address, ubc_break_mode_t break_mode);
```

---

### `ubc_get_break_address`

Get a breakpoint address if it's enabled If the channel is disabled the function will return false and *break_address will not be updated.

```c
bool ubc_get_break_address(int channel, void** break_address);
```

---

### `ubc_disable_channel`

Disable a UBC channel Return true on success. If an invalid channel number is provided, it will return false.

```c
bool ubc_disable_channel(int channel);
```

---

## Data Structures

### `ubc_break_mode_t`

Read and write the DBR register */
void ubc_setDBR(void* DBR);
void* ubc_getDBR(void);

/* ubc_dbh(): Low level UBC debug handler
   The handler will backup the current CPU state and call ubc_debug_handler(). */
void ubc_dbh(void);
/* ubc_debug_handler(): C level UBC debug handler
   The execution will be redirected to the handler set by the user or the break
   will be ignored in case no handler has been set. */
void ubc_debug_handler(gdb_cpu_state_t* cpu_state);
/* ubc_set_debug_handler(): Set user debug handler
   Set a custom debug handler that will be called when a break condition from
   the UBC is reached. */
void ubc_set_debug_handler(void (*h)(gdb_cpu_state_t*));

/* ubc_dbh_lock: Lock set by ubc_dbh() when a UBC break is currently being
   handled. */
extern uint8_t ubc_dbh_lock;

/* UBC Breakpoint types

**Fields**:

- `UBC_BREAK_BEFORE, /* Break before the instruction is executed */
	UBC_BREAK_AFTER,  /* Break after the instruction is executed :
                             at this point PC will point to the next instruction. */`

```c
enum ubc_break_mode_t {
UBC_BREAK_BEFORE, /* Break before the instruction is executed */
	UBC_BREAK_AFTER,  /* Break after the instruction is executed :
                             at this point PC will point to the next instruction. */
};
```

---

## Macros

## Implementation

Source files:

- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/ubc/ubc.c](https://github.com/ClasspadDev/gint/blob/dev/src/ubc/ubc.c)
