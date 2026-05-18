# gdb

gint:gdb - GDB remote serial protocol

## Functions

### `gdb_start`

Start the GDB remote serial protocol server This function starts the GDB stub (program that communicates with GDB). It takes over USB, and returns once communication is established with a GDB instance on the computer. Normally this is called by the panic handler after a crash. It can also be called manually, in which case it should be followed by a call to gdb_main() with the current state (NULL if you don't have any). If the GDB stub is already started, this is a no-op. Returns 0 on success and a negative value if the USB setup fails.

```c
int gdb_start(void);
```

---

### `gdb_main`

Main GDB loop Main loop for when the program is in a paused state. Communicates with GDB over USB and mutates the cpu_state struct, memory and the UBC configuration accordingly. Returns once the program should resume.

```c
void gdb_main(gdb_cpu_state_t *cpu_state);
```

---

### `gdb_start_on_exception`

Set the GDB stub to autostart on crashes This function sets a crash handler that starts the GDB stub whenever a System ERROR occurs.

```c
void gdb_start_on_exception(void);
```

---

### `gdb_redirect_streams`

Select whether to redirect stdout/stderr This function specifies whether stdout and stderr shall be redirected when debugging. If redirected, stdout and stderr will change from their default implementation (or the one supplied by the user via open_generic()) to a stubcall that prints text in the GDB console. This setting must be set before GDB starts, so usually just after gdb_start_on_exception(). This is intended to be used with debug methods that print text. For example, if the program has a status() function that prints information useful when debugging, one might want to invoke it from GDB with "call status()". With default stdout/stderr this would be at best impractical to read the output on the calculator, at worst buggy due to the program being interrupted. Redirecting stdout/stderr allows the result to be printed in GDB. The default is to not redirect stdout/stderr.

```c
void gdb_redirect_streams(bool redirect_stdout, bool redirect_stderr);
```

---

### `gdb_stubcall_write`

Stubcalls *

```c
void gdb_stubcall_write(int fd, void const *buf, size_t size);
```

---

### `gdb_stubcall_write`

Write to a file descriptor on the remote debugger.

```c
void gdb_stubcall_write(int fd, void const *buf, size_t size);
```

---

## Macros

## Implementation

Source files:

- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/ubc/ubc.c](https://github.com/ClasspadDev/gint/blob/dev/src/ubc/ubc.c)
