# gint

gint - An alternative runtime environment for fx9860g and fxcg50

## Functions

### `gint_world_switch`

Switch out of gint to execute a function This function can be used to leave gint, restore the OS's hardware state, and execute code there before returning to gint. By doing this one can effectively interleave gint with the standard OS execution. gint drivers will be inactive during this time but OS features such as BFile or the main menu are available. This main uses for this switch are going back to the main menu and using BFile function. You can go back to the main menu easily by calling getkey() (or getkey_opt() with the GETKEY_MENU flag set) and pressing the MENU key, or by calling gint_osmenu() below which uses this switch. The code to execute while in OS mode is passed as a gint call; you can use GINT_CALL() to create one. This allows you to pass arguments to your function, as well as return an int. A GINT_CALL() to execute while in OS mode

**Returns**: Returns the return value of (function), if any, 0 if function is NULL.

```c
int gint_world_switch(gint_call_t function);
```

---

### `gint_world_sync`

This function is an older version of gint_world_switch() which only accepts functions with no arguments and no return value. It will be removed in gint 3.

```c
void gint_world_sync(void);
```

---

### `gint_world_sync`

Synchronize asynchronous drivers This function waits for asynchronous tasks to complete by unbinding all drivers. This is useful in certain hardware operations while remaining in gint.

```c
void gint_world_sync(void);
```

---

### `gint_osmenu`

Call the calculator's main menu This function safely invokes the calculator's main menu with gint_switch(). If the user selects the gint application again in the menu, this function reloads gint and returns. Otherwise, the add-in is fully unloaded by the system and the application terminates. This function is typically called when the [MENU] key is pressed during a call to getkey(), but can also be called manually.

```c
void gint_osmenu(void);
```

---

### `gint_osmenu_native`

Like gint_osmenu() without the world switch This is a replacement for gint_osmenu() which can be used when the current kernel is already the native OS kernel.

```c
void gint_osmenu_native(void);
```

---

### `gint_setrestart`

Set whether to restart the add-in after exiting An add-in that returns from its main() function automatically exits to the OS' main menu. However, when this happens the OS does not allow the add-in to be restarted unless another add-in is launched first. (This is because the OS tries to *resume* the current add-in, which then proceeds to exit again immediately.) This function enables a gint trick where after main() returns the add-in will invoke the main menu with gint_osmenu() rather than exiting. If the add-in is selected again, gint will jump back to the entry point, creating the illusion that the add-in exited and was then restarted. @restart  0 to exit, 1 to restart by using gint_osmenu()

```c
void gint_setrestart(int restart);
```

---

### `gint_poweroff`

World switch and power off the calculator Shows the CASIO logo / poweroff screen if show_logo is true. The program will resume execution normally after powering on again. Don't call this function with show_logo=false as a result of pressing AC/ON because the calculator will restart immediately unless the user releases the AC/ON key extremely quickly.

```c
void gint_poweroff(bool show_logo);
```

---

### `gint_set_onchip_save_mode`

Specify memory save policy for world switches World switches can cause corruption in on-chip memory in two ways. First, if the calculator powers off while in the OS world, on-chip memory will be wiped because it's not powered when the calculator is off. Second, the OS may run code that overwrites on-chip memory (we don't have any examples of that but it's not part of the add-in interface). As a result, the best option is to backup on-chip memory and restore it when loading back into the add-in. The issue is that there's 20 kB of on-chip memory (4 kB ILRAM + 16 kB XYRAM) and on some machines (fx-9860G-like) or in some applications we don't have that much memory lying around. This function selects between three modes for handling this save: * [Reinitialization mode] Don't save on-chip memory at all, instead just reinitialize the globals and leave the rest corrupted. This is useful if on-chip memory is used only for temporaries (e.g. frames in Azur), for which we don't care if they're corrupted, or code (e.g gint interrupt handling code), which will be reloaded and is otherwise a constant. * [Backup mode] Save on-chip memory to a user-provided buffer of size GINT_ONCHIP_BUFSIZE. * [Nothing mode] Don't do anything, and let the world switch function choose its preferred saving method. This allows application-specific compromises. This is not a problem on SH3 because on-chip memory is only used on SH4.

```c
void gint_set_onchip_save_mode(int mode, void *ptr);
```

---

### `gint_get_onchip_save_mode`

Get the current on-chip memory save policy

```c
int gint_get_onchip_save_mode(void **ptr);
```

---

### `intc_handler`

This function has been moved to the INTC driver

```c
return intc_handler(code, h, size);
```

---

### `gint_set_quit_handler`

Setup a call to be invoked when leaving the add-in This function sets up the provided GINT_CALL() to be invoked when the add-in is unloaded, which is either when we exit from main() or when starting another application from the main menu. Crucially, this is only *after* selecting an application, not before opening the main menu. The quit handler is not invoked if the user re-enters the add-in. This is based on the SetQuitHandler() syscall, and therefore the callback runs in the OS world by default. If [run_in_os_world] is set to false, a world switch will be performed to run the callback in the gint world. TODO: Currently the quit handler is not called when exiting from main(). TODO: Detail how this interacts with destructor functions! TODO: [run_in_os_world == false] is not honored yet (because unstable) @call             Callback to be performed when leaving add-in @run_in_os_world  true to stay in OS world, false to use gint world

```c
void gint_set_quit_handler(gint_call_t gcall, bool run_in_os_world);
```

---

## Data Structures

### `gint_inth_callback_context_t`

gint_get_onchip_save_mode(): Get the current on-chip memory save policy */
int gint_get_onchip_save_mode(void **ptr);

/* This function has been moved to the INTC driver */
__attribute__((deprecated("Use intc_handler() instead")))
static GINLINE void *gint_inthandler(int code, void const *h, size_t size) {
	return intc_handler(code, h, size);
}

/* gint_inth_callback(): Callback from interrupt handler to userland

   This function performs an indirect call as with gint_call(), afters saving
   the user context, enabling interrupts and going to user bank. This is useful
   to call user code from interrupt handlers. You can think of it as a kernel-
   space escape to virtualized userland during interrupt handling.

   This function can only be useful in an interrupt handler's assembler code.
   It is loaded at a runtime-determined address and accessed through a function
   pointer, like this:

        mov.l	.callback, r0
        mov.l	@r0, r0 # because function pointer
        mov	<address of gint_call_t object>, r4
        jsr	@r0
        nop
   .callback:
        .long	_gint_inth_callback

   The gint_call_t object can be built with GINT_CALL_FLAG to specify that the
   called function should get a pointer to a gint_inth_callback_context_t
   strcture as its first argument.

   @call  Address of a gint_call_t object
   Returns the return value of the callback. */
extern int (*gint_inth_callback)(gint_call_t const *call);

/* gint_inth_callback_context_t: Context of the interrupted function when an
   interrupt is handled by gint_inth_callback.

**Fields**:

- `uint32_t ssr`

- `uint32_t spc`

- `uint32_t r7`

- `uint32_t r6`

- `uint32_t r5`

- `uint32_t r4`

- `uint32_t r3`

- `uint32_t r2`

- `uint32_t r1`

- `uint32_t r0`

```c
struct gint_inth_callback_context_t {
uint32_t ssr;
	uint32_t spc;
	uint32_t r7;
	uint32_t r6;
	uint32_t r5;
	uint32_t r4;
	uint32_t r3;
	uint32_t r2;
	uint32_t r1;
	uint32_t r0;
};
```

---

## Macros

### `GINT_ONCHIP_BUFSIZE`

```c
#define GINT_ONCHIP_BUFSIZE (20 << 10)
```

---

## Implementation

Source files:

- [src/kernel/kernel.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/kernel.c)
- [src/kernel/syscall.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/syscall.c)
- [src/kernel/exch.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/exch.c)
- [src/kernel/osmenu.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/osmenu.c)
- [src/kernel/start.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/start.c)
- [src/kernel/world.c](https://github.com/ClasspadDev/gint/blob/dev/src/kernel/world.c)
- [src/intc/intc.c](https://github.com/ClasspadDev/gint/blob/dev/src/intc/intc.c)
- [src/cpg/overclock.c](https://github.com/ClasspadDev/gint/blob/dev/src/cpg/overclock.c)
