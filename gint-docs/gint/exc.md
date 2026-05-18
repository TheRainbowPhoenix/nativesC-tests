# exc

gint:exc - Exception handling
//
//	This small module is used to display exceptions and configure when the
//	exception handler displays these messages. This is for advanced users
//	only!


## Functions


### `gint_exc_skip`

Skip pending exception instructions Many exceptions re-execute the offending instruction after the exception is handled. For instance the TLB miss handler is supposed to load the required page into memory, so that the instruction that accessed unmapped memory can be successfully re-executed. When an exception-catching function records an exception without solving it, this re-execution will fail again and the exception handling process will loop. In such a situation, gint_exc_skip() can be used to manually skip the offending instruction, if this is an acceptable resolution. @instructions  Number of instructions to skip (usually only one)


```c
void gint_exc_skip(int instructions);
```


---


## Macros
