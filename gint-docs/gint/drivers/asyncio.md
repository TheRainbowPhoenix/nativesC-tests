# asyncio

gint:usb:asyncio - Asynchronous I/O common definitions

## Functions

### `asyncio_op_clear`

Initialize/clear the storage for an I/O operation

```c
void asyncio_op_clear(asyncio_op_t *op);
```

---

### `asyncio_op_busy`

Check whether the transfer is busy for syscalls This function checks whether the transfer is in a state where the CPU is busy wrt. starting a new syscall, ie. read(2), write(2) or fsync(2). Returns true if the CPU is busy and the call has to wait, false if the call can proceed immediately.

```c
bool asyncio_op_busy(asyncio_op_t const *op);
```

---

### `asyncio_op_finish_write`

Start/finish a write(2) call.

```c
void asyncio_op_finish_write(asyncio_op_t *op);
```

---

### `asyncio_op_start_write_round`

Start/finish a single-block write to hardware.

```c
void asyncio_op_start_write_round(asyncio_op_t *op, size_t size);
```

---

### `asyncio_op_start_sync`

Start an fsync(2) operation (after one or more writes) and finish it.

```c
void asyncio_op_start_sync(asyncio_op_t *op, gint_call_t const *callback);
```

---

### `asyncio_op_start_read_hwseg`

Start a read(2) call. The call will finish automatically when the final round finishes. If `autoclose` is set, the current hardware segment will be marked as completed if the round reads it entirely, even if the request is fulfilled.

```c
void asyncio_op_start_read_hwseg(asyncio_op_t *op, size_t size, bool cont);
```

---

### `asyncio_op_start_read_hwseg`

Start a hardware segment. `cont` should be true if there will be another segment in the same transaction. The segment will finish automatically when it is completely consumed by a read round.

```c
void asyncio_op_start_read_hwseg(asyncio_op_t *op, size_t size, bool cont);
```

---

### `asyncio_op_start_read_round`

Start a single-block read from hardware. The requested size is automatically t->size, however the round may of course be smaller depending on how much data is available. Returns the round size.

```c
int asyncio_op_start_read_round(asyncio_op_t *op);
```

---

### `asyncio_op_finish_read_round`

Finish a single-block read from hardware. This function also finishes the current hardware segment and read call if appropriate, *except* that it doesn't invoke the read(2) callback. You should make a copy of it before calling and invoke it manually after. Returns a combination of the above flags indicating what finished along with the round.

```c
int asyncio_op_finish_read_round(asyncio_op_t *op);
```

---

### `asyncio_op_cancel_read`

Cancel a read call. This keeps the hardware segment part intact.

```c
void asyncio_op_cancel_read(asyncio_op_t *op);
```

---

## Data Structures

## Macros

## Implementation

Source files:

- [src/usb/pipes.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/pipes.c)
- [src/usb/asyncio.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/asyncio.c)
