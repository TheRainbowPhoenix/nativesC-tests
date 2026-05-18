# usb

gint:usb - USB communication

## Functions

### `usb_open`

Open the USB link This function opens the USB link and notifies the host that the device is ready to connect. Usually the host immediately queries the device, and after some exchanges the device can be used. The USB link might not be ready when this function returns, use the callback or usb_open_wait() for that. The first parameters is a NULL-terminated array of interfaces to open. To see available interfaces, please see header files in <gint/usb-*.h>. Each interface can be used independently, however if there are not enough USB resources (buffer memory, pipes or endpoints) for all of them, usb_open() will return an error. The second parameter is a callback to be (asynchronously) invoked when the USB link is ready. Use GINT_CALL() to create one, or pass GINT_CALL_NULL for no callback. You can also use usb_open_wait() to synchronously wait for the link to be ready. @interfaces  NULL-terminate list of interfaces to open @callback    Optional function to be called when the USB link opens

```c
int usb_open(usb_interface_t const **interfaces, gint_call_t callback);
```

---

### `usb_open_wait`

Wait until the USB link is ready When called after usb_open(), this function waits until the communication is established. You should only call this if usb_open() returns 0.

```c
void usb_open_wait(void);
```

---

### `usb_is_open`

Check whether the USB link is active

```c
bool usb_is_open(void);
```

---

### `usb_is_open_interface`

Check whether a particular interface is open

```c
bool usb_is_open_interface(usb_interface_t const *interface);
```

---

### `usb_close`

Close the USB link This function closes the link opened by usb_open(), and notifies the host of the disconnection (if any was established). The USB link can be reopened later to perform more tasks. There are two reasons to close the USB link: to save battery power and to return to the calculator's main menu. You should thus close it if (1) the USB link might not be used for a while, or (2) you want to return to the main menu before using it again.

```c
void usb_close(void);
```

---

### `usb_interface_pipe`

Requested buffer size, should be a multiple of 64 and not more than 2048. Valid only for bulk and isochronous endpoints.

```c
int usb_interface_pipe(usb_interface_t const *interface, int endpoint);
```

---

### `usb_interface_pipe`

Get the pipe associated to an interface endpoint This function returns the pipe number that backs the specified endpoint number (using the local value of the interface, not the concrete one). This function is intended for interface implementations, not users.

```c
int usb_interface_pipe(usb_interface_t const *interface, int endpoint);
```

---

### `usb_write_sync`

Synchronously write to a USB pipe This functions writes (size) bytes of (data) into the specified pipe. If the data fits into the pipe, this function returns right away, and the data is *not* transmitted. Otherwise, data is written until the pipe is full, at which point it is automatically transmitted. After the transfer, this function resumes writing, returning only once everything is written. Even then the last bytes will still not have been transmitted, to allow for other writes to follow. After the last write in a sequence, use usb_commit_sync() or usb_commit_async() to transmit the last bytes. If (use_dma=true), the write is performed with the DMA instead of the CPU. This requires at least 4-byte alignment on: 1. The input data; 2. The size of this write; 3. The amount of data previously written to the pipe not yet committed. This is because using the DMA does not allow any insertion of CPU logic to handle unaligned stuff. This function will use a FIFO controller to access the pipe. The FIFO controller will be reserved for further writes until the contents of the pipe are commited with usb_commit_sync() or usb_commit_async(); when more than two pipes need to operate in parallel, keep the write sequences short and commit regularly to avoid holding the controllers. If the pipe is busy due to an ongoing asynchronous write or commit, or there is no FIFO controller available to perform the operation, this function waits for the ressources to become available then proceeds normally. @pipe       Pipe to write into @data       Source data @size       Size of source @dma        Whether to use the DMA to perform the write

**Returns**: Returns an error code (0 on success).

```c
int usb_write_sync(int pipe, void const *data, int size, bool use_dma);
```

---

### `usb_commit_sync`

Asynchronously write to a USB pipe This function is similar to usb_write_sync(), but it only starts the writing and returns immediately without ever waiting. The writing then occurs in the background of the calling code, and the caller is notified through a callback when it completes. Use GINT_CALL() to create a callback or pass GINT_CALL_NULL. If the pipe is busy due to a previous asynchronous write, this function returns USB_WRITE_BUSY. If no FIFO controller is available for the transfer, it returns USB_WRITE_NOFIFO. When called with (use_dma=true), it returns as soon as the DMA starts, without even a guarantee that the first few bytes have been written. There is no guarantee that the write is complete until the callback is called, however calling again with data=NULL and size=0 can be used to determine whether the write has finished, since it will return 0 if the pipe is idle and USB_WRITE_BUSY otherwise. @pipe       Pipe to write into @data       Source data @size       Size of source @dma        Whether to use the DMA to perform the write @callback   Optional callback to invoke when the write completes

**Returns**: Returns an error code (0 on success).

```c
void usb_commit_sync(int pipe);
```

---

### `usb_commit_sync`

Synchronously commit a write This function waits for any pending write on the pipe to finish, then transfers whatever data is left, and returns when the transfer completes.

```c
void usb_commit_sync(int pipe);
```

---

### `usb_commit_sync_timeout`

Synchronously commit a write, with timeout

```c
int usb_commit_sync_timeout(int pipe, timeout_t const *timeout);
```

---

### `usb_commit_async`

Asynchronously commit a write This function commits the specified pipe, causing the pipe to transfer written data in the pipe. If the pipe is currently busy due to an ongoing write or commit, it returns USB_COMMIT_BUSY. You should call usb_commit_async() when the pipe is ready, which is either when the previous synchronous call returns, or when the callback of the previous asynchronous call is invoked. This function returns immediately and invokes (callback) when the transfer of the remaining data completes.

```c
int usb_commit_async(int pipe, gint_call_t callback);
```

---

### `usb_read_sync`

Synchronously read from a USB pipe This function waits for data to become available on the specified `pipe`, and then reads up to `size` bytes into `data`. It is "synchronized" with USB transactions on the pipe in the following ways: 1. If there is no active transaction, it waits for one to arrive. 2. If the active transaction has no data left to read, it is skipped. 3. If the transaction's data is completely read, the transaction is closed even if `data` is full. (See usb_read_async() for the alternative.) Basically usb_read_sync() returns the next `size` bytes of data that the calculator receives on the pipe, with a single exception: it doesn't read across transactions, so if the active transaction has 100 bytes left and you request 200, you will only get 100. usb_read_sync() only ever returns 0 bytes if there is an empty transaction, which is rare. Because this is a blocking function, a call to usb_read_sync() will *FREEZE* if there is no data to read on the pipe and the host doesn't send any. In addition, it's not possible to detect how much data is left to read with usb_read_sync()'s interface. If you want to avoid or debug a freeze, use use usb_read_async() or set a timeout with usb_read_sync_timeout(). If you want to read a transaction until the end without knowing its size in advance, use usb_read_async(). If `use_dma=true`, uses the DMA for transfers. This requires 4-byte alignment on the buffer, size, and number of bytes previously read in the current transaction. Returns the number of bytes read or a negative error code.

```c
int usb_read_sync(int pipe, void *data, int size, bool use_dma);
```

---

### `usb_read_cancel`

Cancel an asynchronous read Once started, an async read will run in the background and keep writing to the provided buffer. This function cancels the operation so that no further writes to the buffer are made and the associated memory can be safely deallocated.

```c
void usb_read_cancel(int pipe);
```

---

### `usb_log`

Set the logging function for the USB driver

```c
void usb_log(char const *format, ...);
```

---

### `usb_log`

Get the logging function

```c
void usb_log(char const *format, ...);
```

---

### `usb_log`

Send a message to the USB log

```c
void usb_log(char const *format, ...);
```

---

### `usb_trace`

Set the tracing function for the USB driver The function is called atomically, thus cannot be interrupted, therefore it is safe to call usb_trace() in interrupt handlers.

```c
void usb_trace(char const *message);
```

---

### `usb_trace`

Get the tracing function

```c
void usb_trace(char const *message);
```

---

### `usb_trace`

Trace the current state of the driver

```c
void usb_trace(char const *message);
```

---

### `usb_dc_string`

Create a STRING descriptor and return its ID This function registers the provided string in an array of STRING descriptors used to answer GET_DESCRIPTOR requests, and returns the string's ID. USB 2.0 only has provision for 256 strings in the device, so this function will return 0 when out of space. The string should be encoded as UTF-16 (big-endian), which can be achieved with a "u" prefix on the string literal, for instance: u"Hello,World!". If (len) is specified, it should be the number of UTF-16 code points to count in the string. If it is 0, it defaults to the length of the string.

```c
uint16_t usb_dc_string(uint16_t const *literal, size_t len);
```

---

### `*usb_dc_string_get`

Get the descriptor for a STRING id This is mostly used by the driver to answer GET_DESCRIPTOR requests.

```c
usb_dc_string_t *usb_dc_string_get(uint16_t id);
```

---

## Data Structures

### `usb_interface`

usb_open(): Open the USB link

   This function opens the USB link and notifies the host that the device is
   ready to connect. Usually the host immediately queries the device, and after
   some exchanges the device can be used. The USB link might not be ready when
   this function returns, use the callback or usb_open_wait() for that.

   The first parameters is a NULL-terminated array of interfaces to open. To
   see available interfaces, please see header files in <gint/usb-*.h>. Each
   interface can be used independently, however if there are not enough USB
   resources (buffer memory, pipes or endpoints) for all of them, usb_open()
   will return an error.

   The second parameter is a callback to be (asynchronously) invoked when the
   USB link is ready. Use GINT_CALL() to create one, or pass GINT_CALL_NULL for
   no callback. You can also use usb_open_wait() to synchronously wait for the
   link to be ready.

   @interfaces  NULL-terminate list of interfaces to open
   @callback    Optional function to be called when the USB link opens */
int usb_open(usb_interface_t const **interfaces, gint_call_t callback);

/* usb_open_wait(): Wait until the USB link is ready
   When called after usb_open(), this function waits until the communication is
   established. You should only call this if usb_open() returns 0. */
void usb_open_wait(void);

/* usb_is_open(): Check whether the USB link is active */
bool usb_is_open(void);

/* usb_is_open_interface(): Check whether a particular interface is open */
bool usb_is_open_interface(usb_interface_t const *interface);

/* usb_close(): Close the USB link

   This function closes the link opened by usb_open(), and notifies the host of
   the disconnection (if any was established). The USB link can be reopened
   later to perform more tasks.

   There are two reasons to close the USB link: to save battery power and to
   return to the calculator's main menu. You should thus close it if (1) the
   USB link might not be used for a while, or (2) you want to return to the
   main menu before using it again. */
void usb_close(void);

//---
// Interfaces to communicate with USB transfers
//
// These interfaces define how the calculator behaves on the USB connection,
// and can include stuff like:
//
// -> Communicate with a custom protocol and a custom program on the PC
//    (like Protocol 7 does with FA-124, or fxlink)
// -> Exchange text as a Communications and CDC Control (class 0x03) device
//    (like an Internet router)
// -> Share a video stream as a video input (class 0x0e) device (like a webcam)
//
// Normal add-ins that just want to use the USB connection don't need to worry
// about programming the interfaces; they can simply use interfaces that are
// already implemented. Start with usb_open().
//---

/* usb_interface_t: A USB interface that can be enabled in usb_open()

   This driver provides a device that only has one configuration (due to how
   rare it is for devices to have several configurations). However, a number of
   interfaces can be activated independently. This structure describes an
   interface with regards to this driver.

   The driver chooses endpoint numbers and slices of the FIFO buffer for the
   interface to use, therefore the supplied descriptors cannot specify them.
   Instead, the supplied descriptors should use arbitrary endpoint numbers; the
   driver will use them to communicate with the interface, and transparently
   use concrete endpoint numbers internally.

**Fields**:

- `/* NULL-terminated array of descriptors for the interface */
	void const **dc`

- `/* Array of endpoint parameters, see below */
	struct usb_interface_endpoint *params`

- `/* Answer class-specific SETUP requests */
	/* TODO */

	/* Notification that an endpoint has data to be read. This function is
	   called frequently when data is being transmitted`

- `the particular
	   timings depend on low-level details. The notification should be
	   passed down to cause the main thread to read later. Do not read in
	   the notification function! */
	void (*notify_read)(int endpoint)`

```c
struct usb_interface {
/* NULL-terminated array of descriptors for the interface */
	void const **dc;
	/* Array of endpoint parameters, see below */
	struct usb_interface_endpoint *params;

	/* Answer class-specific SETUP requests */
	/* TODO */

	/* Notification that an endpoint has data to be read. This function is
	   called frequently when data is being transmitted; the particular
	   timings depend on low-level details. The notification should be
	   passed down to cause the main thread to read later. Do not read in
	   the notification function! */
	void (*notify_read)(int endpoint);
};
```

---

### `usb_interface_endpoint`

usb_interface_endpoint_t: Parameters for an interface endpoint

   This structure mainly specifies the settings for the pipe associated to the
   endpoint. There 10 pipes, but not all can be used with any transfer type,
   and not all have access to the same amount of memory.

**Fields**:

- `/* Endpoint number as specified in the interface's descriptors
	   (including the IN/OUT bit) */
	uint8_t endpoint`

- `/* Requested buffer size, should be a multiple of 64 and not more than
	   2048. Valid only for bulk and isochronous endpoints. */
	uint16_t buffer_size`

```c
struct usb_interface_endpoint {
/* Endpoint number as specified in the interface's descriptors
	   (including the IN/OUT bit) */
	uint8_t endpoint;
	/* Requested buffer size, should be a multiple of 64 and not more than
	   2048. Valid only for bulk and isochronous endpoints. */
	uint16_t buffer_size;
};
```

---

## Macros

### `USB_LOG`

```c
#define USB_LOG(...) usb_log(__VA_ARGS__)
```

---

### `USB_TRACE`

```c
#define USB_TRACE(...) usb_trace(__VA_ARGS__)
```

---

### `USB_LOG`

```c
#define USB_LOG(...) do {} while(0)
```

---

### `USB_TRACE`

```c
#define USB_TRACE(...) do {} while(0)
```

---

## Implementation

Source files:

- [src/gdb/gdb.c](https://github.com/ClasspadDev/gint/blob/dev/src/gdb/gdb.c)
- [src/usb/usb.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/usb.c)
- [src/usb/pipes.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/pipes.c)
- [src/usb/string.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/string.c)
- [src/usb/configure.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/configure.c)
- [src/usb/setup.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/setup.c)
- [src/usb/classes/ff-bulk-gray.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/classes/ff-bulk-gray.c)
- [src/usb/classes/ff-bulk.c](https://github.com/ClasspadDev/gint/blob/dev/src/usb/classes/ff-bulk.c)
