# bfile

gint:bfile - BFile interface
//
// BFile is the OS's native interface to the filesystem. It has quite a bit of
// legacy, so it's not very easy to use. Please note that BFile cannot be used
// from within gint, so do a gint_world_switch() (<gint/gint.h>) to call BFile
// functions; otherwise the add-in is likely to crash.
//
// There are two different filesystems:
// * An older, in-house one made by CASIO and informally called CASIOWIN (which
//   is the name of the OS itself), which is incomplete, hard to use, and very
//   limiting, but also very fast.
// * A newer one, designed by Kyoto Software Research and called Fugue, which
//   is mostly complete and behaves as a regular filesystem, though very slow.
//
// You can detect which one is used by querying gint[HWFS] (<gint/hardware.h>):
// * HWFS_FUGUE is the newer one; it is found in:
//   - The fx-CG series (in France: Prizm and Graph 90+E)
//   - The fx G-III series (in France: Graph 35+E II)
// * HWFS_CASIOWIN is the older one; it is found in all older black-and-white
//   model that have a storage memory.
//
// Wherever Fugue is used, gint supports the Unix file API (open, read, write,
// etc) and the C99 standard file API (fopen, fread, fwrite, etc), so there is
// no need to call into BFile directly (you should still do a world switch
// before using these functions).
//
// You should only have to use BFile if you're dealing with the CASIOWIN
// filesystem. With Fugue, not only are the Unix/C99 APIs easier to use, but
// the meanings of arguments and return values in BFile calls also change
// compared to the CASIOWIN filesystem, so there's little to gain anyway.
//
// If you're here for the CASIOWIN filesystem, be aware of its limitations. In
// general reading files will work fine; expect no issues with that. Modifying
// files is where things get difficult. Keep the following in mind:
//
// * Non-standard meanings of arguments and return values (eg. BFile_Read()
//   returns the number of readable bytes between the new position and EOF).
// * Files must be created with a fixed size indicated in BFile_Create() and
//   are initialized with 0xff.
// * A write to a file can only replace 1's with 0's, meaning in practice a
//   file can only be written to once. (That's why it's created with 0xff.)
// * All writes must be of even size; writing an odd number of bytes can mess
//   up the file and confuse the filesystem.
// * Only one level of folders is supported; attempting to create second-level
//   subfolders results in weird recursive directories (don't do that).
//
// File paths in the OS use the non-standard FONTCHARACTER encoding, which is a
// 16-bit fixed-width encoding. Most users don't care about special characters;
// in GCC, you can get a 16-bit literal string with the u"" prefix, eg.
//   u"\\\\fls0\\data.bin"
// which is what you usually supply as the [uint16_t const *path] argument.
//
// All functions return nonnegative values on success. If no return value is
// described then a successful call returns 0. Unless specified otherwise, all
// functions can also return a negative error code as documented near the end
// of this header.


## Functions


### `BFile_Remove`

Remove a file or folder (also works if the entry does not exist).


```c
int BFile_Remove(uint16_t const *path);
```


---


### `BFile_Rename`

Rename a file (can move folders; Fugue only).


```c
int BFile_Rename(uint16_t const *oldpath, uint16_t const *newpath);
```


---


### `BFile_Create`

Create a new file or folder The file or directory must not exist. For a file the size pointer must point to the desired file size (which is fixed), for a folder it is ignored. With CASIOWIN this is the final size of the file. With Fugue the file can be resized dynamically and is usually created with initial size 0. @path  FONTCHARACTER file path @type  Entry type (BFile_File or BFile_Folder) @size  Pointer to file size if type is BFile_File, use NULL otherwise


```c
int BFile_Create(uint16_t const *path, int type, int *size);
```


---


### `BFile_Open`

Open a file for reading or writing The file must exist, even when opening in writing mode. The meaning of the [Share] flag isn't clear; I believe it's simply ignored in the CASIOWIN filesystem. @path  FONTCHARACTER file path @mode  Desired access mode Returns a file descriptor (or a negative error code).


```c
int BFile_Open(uint16_t const *path, int mode);
```


---


### `BFile_Close`

Close an open file descriptor.


```c
int BFile_Close(int fd);
```


---


### `BFile_Size`

Get the size of an open file.


```c
int BFile_Size(int fd);
```


---


### `BFile_Write`

Write data to an open file WARNING: The CASIOWIN fs is known to become inconsistent if an odd number of bytes is written with BFile_Write(). Always keep it even! With CASIOWIN, returns the new file offset after writing (or an error code). With Fugue, returns the amount of data written (or an error code).


```c
int BFile_Write(int fd, void const *data, int even_size);
```


---


### `BFile_Read`

Read data from an open file The extra argument [whence] specifies where data is read from in the style of pread(2), and is supported with both filesystems. * If [whence >= 0], it is taken as an absolute location within the file; * If [whence == -1], BFile_Read() reads from the current position. With Fugue this function can read past end of file and return the requested amount of bytes even when the file does not have enough data to read that much. It seems that extra bytes read as zeros. Reading past the end does *not* extend the file size. With CASIOWIN, returns how much data can be read from the updated file offset (ie. how many bytes until end of file), or an error code. With Fugue, returns the amount of data read (or an error code).


```c
int BFile_Read(int handle, void *data, int size, int whence);
```


---


### `BFile_Seek`

Seek to a relative or absolute position within an open file With CASIOWIN, moves [offset] bytes relative to the current position, and returns how much data can be read from the new position. BFile_Seek(fd, 0) combined with BFile_Size(fd) can be used to determine the current position. With Fugue, moves to the absolute position [offset] within the file, and returns the amount of allocated space following the new position (usually larger than the amount of data until end-of-file because files are allocated in units of 4096 bytes). There is no way to seek relative to the current position unless the target is precomputed with BFile_GetPos().


```c
int BFile_Seek(int fd, int offset);
```


---


### `BFile_GetPos`

Get the current position in an open file This call does not exist in the CASIOWIN interface, so this function always returns -1 on models with a CASIOWIN filesystem. This call exists in Fugue, however some Fugue models have their syscall list inherited from the CASIOWIN era and don't have an entry point for it (or if there's one it's escape scrutiny so far). * Prizm / Graph 90+E / fx-CG series: this function works as intended. * Graph 35+E II / G-III series: the call is missing, returns -1. For the latter models there is no easily reliable way of knowing the current position within an open file!


```c
int BFile_GetPos(int fd);
```


---


### `BFile_FindClose`

Continue a search Continues the search for matches. The search handle is the value set in *shandle in BFile_FindFirst(). As before, *foundfile receives the matching entry's path and *fileinfo its metadata. Returns 0 on success. The negative error code BFile_EnumerateEnd should be interpreted as the end of the search (ie. all matching files have been returned) rather than an error.


```c
int BFile_FindClose(int shandle);
```


---


### `BFile_FindClose`

Close a search handle (with or without exhausting the matches).


```c
int BFile_FindClose(int shandle);
```


---


### `BFile_Ext_Stat`

Stat an entry for type and size


```c
int BFile_Ext_Stat(uint16_t const *path, int *type, int *size);
```


---


## Macros


### `BFile_File`


```c
#define BFile_File 1
```


---


### `BFile_Folder`


```c
#define BFile_Folder 5
```


---


### `BFile_ReadOnly`


```c
#define BFile_ReadOnly 0x01
```


---


### `BFile_WriteOnly`


```c
#define BFile_WriteOnly 0x02
```


---


### `BFile_ReadWrite`


```c
#define BFile_ReadWrite (BFile_ReadOnly | BFile_WriteOnly)
```


---


### `BFile_Share`


```c
#define BFile_Share 0x80
```


---


### `BFile_ReadShare`


```c
#define BFile_ReadShare (BFile_ReadOnly | BFile_Share)
```


---


### `BFile_ReadWriteShare`


```c
#define BFile_ReadWriteShare (BFile_ReadWrite | BFile_Share)
```


---


### `BFile_EntryNotFound`


```c
#define BFile_EntryNotFound -1
```


---


### `BFile_IllegalParam`


```c
#define BFile_IllegalParam -2
```


---


### `BFile_IllegalPath`


```c
#define BFile_IllegalPath -3
```


---


### `BFile_DeviceFull`


```c
#define BFile_DeviceFull -4
```


---


### `BFile_IllegalDevice`


```c
#define BFile_IllegalDevice -5
```


---


### `BFile_IllegalFilesystem`


```c
#define BFile_IllegalFilesystem -6
```


---


### `BFile_IllegalSystem`


```c
#define BFile_IllegalSystem -7
```


---


### `BFile_AccessDenied`


```c
#define BFile_AccessDenied -8
```


---


### `BFile_AlreadyLocked`


```c
#define BFile_AlreadyLocked -9
```


---


### `BFile_IllegalTaskID`


```c
#define BFile_IllegalTaskID -10
```


---


### `BFile_PermissionError`


```c
#define BFile_PermissionError -11
```


---


### `BFile_EntryFull`


```c
#define BFile_EntryFull -12
```


---


### `BFile_AlreadyExists`


```c
#define BFile_AlreadyExists -13
```


---


### `BFile_ReadOnlyFile`


```c
#define BFile_ReadOnlyFile -14
```


---


### `BFile_IllegalFilter`


```c
#define BFile_IllegalFilter -15
```


---


### `BFile_EnumerateEnd`


```c
#define BFile_EnumerateEnd -16
```


---


### `BFile_DeviceChanged`


```c
#define BFile_DeviceChanged -17
```


---


### `BFile_IllegalSeekPos`

#define BFile_NotRecordFile     -18     // Not used


```c
#define BFile_IllegalSeekPos -19
```


---


### `BFile_IllegalBlockFile`


```c
#define BFile_IllegalBlockFile -20
```


---


### `BFile_NotMountDevice`

#define BFile_NoSuchDevice      -21     // Not used #define BFile_EndOfFile         -22     // Not used


```c
#define BFile_NotMountDevice -23
```


---


### `BFile_NotUnmountDevice`


```c
#define BFile_NotUnmountDevice -24
```


---


### `BFile_CannotLockSystem`


```c
#define BFile_CannotLockSystem -25
```


---


### `BFile_RecordNotFound`


```c
#define BFile_RecordNotFound -26
```


---


### `BFile_NoAlarmSupport`

#define BFile_NotDualRecordFile -27     // Not used


```c
#define BFile_NoAlarmSupport -28
```


---


### `BFile_CannotAddAlarm`


```c
#define BFile_CannotAddAlarm -29
```


---


### `BFile_FileFindUsed`


```c
#define BFile_FileFindUsed -30
```


---


### `BFile_DeviceError`


```c
#define BFile_DeviceError -31
```


---


### `BFile_SystemNotLocked`


```c
#define BFile_SystemNotLocked -32
```


---


### `BFile_DeviceNotFound`


```c
#define BFile_DeviceNotFound -33
```


---


### `BFile_FileTypeMismatch`


```c
#define BFile_FileTypeMismatch -34
```


---


### `BFile_NotEmpty`


```c
#define BFile_NotEmpty -35
```


---


### `BFile_BrokenSystemData`


```c
#define BFile_BrokenSystemData -36
```


---


### `BFile_MediaNotReady`


```c
#define BFile_MediaNotReady -37
```


---


### `BFile_TooManyAlarms`


```c
#define BFile_TooManyAlarms -38
```


---


### `BFile_SameAlarmExists`


```c
#define BFile_SameAlarmExists -39
```


---


### `BFile_AccessSwapArea`


```c
#define BFile_AccessSwapArea -40
```


---


### `BFile_MultimediaCard`


```c
#define BFile_MultimediaCard -41
```


---


### `BFile_CopyProtection`


```c
#define BFile_CopyProtection -42
```


---


### `BFile_IllegalFileData`


```c
#define BFile_IllegalFileData -43
```


---


### `BFile_Type_Directory`


```c
#define BFile_Type_Directory 0x0000
```


---


### `BFile_Type_File`


```c
#define BFile_Type_File 0x0001
```


---


### `BFile_Type_Addin`


```c
#define BFile_Type_Addin 0x0002
```


---


### `BFile_Type_Eact`


```c
#define BFile_Type_Eact 0x0003
```


---


### `BFile_Type_Language`


```c
#define BFile_Type_Language 0x0004
```


---


### `BFile_Type_Bitmap`


```c
#define BFile_Type_Bitmap 0x0005
```


---


### `BFile_Type_MainMem`


```c
#define BFile_Type_MainMem 0x0006
```


---


### `BFile_Type_Temp`


```c
#define BFile_Type_Temp 0x0007
```


---


### `BFile_Type_Dot`


```c
#define BFile_Type_Dot 0x0008
```


---


### `BFile_Type_DotDot`


```c
#define BFile_Type_DotDot 0x0009
```


---


### `BFile_Type_Volume`


```c
#define BFile_Type_Volume 0x000a
```


---


### `BFile_Type_Archived`


```c
#define BFile_Type_Archived 0x0041
```


---
