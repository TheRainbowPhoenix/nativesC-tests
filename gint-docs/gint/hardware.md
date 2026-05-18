# hardware

gint:hardware - Platform information and hardware detection
//
//	This components centralizes detected information about the runtime
//	hardware, including MPU version, peripheral modules, and how drivers
//	configured them.
//
//	The most common use of this header is for the isSH3() and isSH4()
//	macros that let you run MPU-dependent jobs and are used like this:
//	  if(isSH3()) do_sh3();
//	  else do_sh4();


## Functions


### `hw_detect`

Most of the information here is going to be stored in (key, value) pairs for predetermined keys and 32-bits values that are often integers or a set of flags. The data will be filled by gint or its drivers.


```c
void hw_detect(void);
```


---


### `hw_detect`

Basic hardware detection This function probes the hardware and fills in the HWMPU, HWCPUVR and HWCPUPR fields.


```c
void hw_detect(void);
```


---


## Macros


### `HW_KEYS`

flags. The data will be filled by gint or its drivers.


```c
#define HW_KEYS 16
```


---


### `HW_LOADED`

information must be treated as invalid.


```c
#define HW_LOADED 0x80000000
```


---


### `HWMPU`


```c
#define HWMPU 0    /* MPU type */
```


---


### `HWCPUVR`

#define HWMPU    0    MPU type


```c
#define HWCPUVR 1    /* CPU Version Register */
```


---


### `HWCPUPR`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register


```c
#define HWCPUPR 2    /* CPU Product Register */
```


---


### `HWCALC`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register


```c
#define HWCALC 3    /* Calculator model, hardcoded in kernel/inth.S */
```


---


### `HWRAM`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S


```c
#define HWRAM 4    /* Amount of RAM */
```


---


### `HWROM`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM


```c
#define HWROM 5    /* Amount of ROM */
```


---


### `HWURAM`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM


```c
#define HWURAM 6    /* Userspace RAM */
```


---


### `HWETMU`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM #define HWURAM   6    Userspace RAM


```c
#define HWETMU /* Deprecated: use timer_count() (ETMU always load) */
```


---


### `HWKBD`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM #define HWURAM   6    Userspace RAM #define HWETMU        Deprecated: use timer_count() (ETMU always load)


```c
#define HWKBD 8    /* Keyboard */
```


---


### `HWKBDSF`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM #define HWURAM   6    Userspace RAM #define HWETMU        Deprecated: use timer_count() (ETMU always load) #define HWKBD    8    Keyboard


```c
#define HWKBDSF /* Deprecated: use keysc_scan_frequency() */
```


---


### `HWDD`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM #define HWURAM   6    Userspace RAM #define HWETMU        Deprecated: use timer_count() (ETMU always load) #define HWKBD    8    Keyboard #define HWKBDSF       Deprecated: use keysc_scan_frequency()


```c
#define HWDD /* Deprecated: use the T6K11/R61524 API */
```


---


### `HWFS`

#define HWMPU    0    MPU type #define HWCPUVR  1    CPU Version Register #define HWCPUPR  2    CPU Product Register #define HWCALC   3    Calculator model, hardcoded in kernel/inth.S #define HWRAM    4    Amount of RAM #define HWROM    5    Amount of ROM #define HWURAM   6    Userspace RAM #define HWETMU        Deprecated: use timer_count() (ETMU always load) #define HWKBD    8    Keyboard #define HWKBDSF       Deprecated: use keysc_scan_frequency() #define HWDD          Deprecated: use the T6K11/R61524 API


```c
#define HWFS 11    /* Filesystem type */
```


---


### `HWMPU_UNKNOWN`

Unknown MPUs are all assumed to be SH-4A-based


```c
#define HWMPU_UNKNOWN 0
```


---


### `HWMPU_SH7337`

Used on original fx-9860G, SH-3-based


```c
#define HWMPU_SH7337 1
```


---


### `HWMPU_SH7305`

Used on recent fx-9860G derivatives such as the fx-9750G II, and also on the fx-CG 10/20/50. SH-4A-based


```c
#define HWMPU_SH7305 2
```


---


### `HWMPU_SH7355`

Used on the fx-9860G II, SH-3-based


```c
#define HWMPU_SH7355 3
```


---


### `HWMPU_SH7724`

Closest documented match to the SH7305, not used in any known calculator. Detected and included for reference only


```c
#define HWMPU_SH7724 4
```


---


### `HWCALC_FX9860G_SH3`

SH-3-based fx-9860G-family


```c
#define HWCALC_FX9860G_SH3 1
```


---


### `HWCALC_FX9860G_SH4`

Other SH-4A-based fx-9860G-family


```c
#define HWCALC_FX9860G_SH4 2
```


---


### `HWCALC_G35PE2`

Graph 35+E II, an SH-4A French extension of the fx-9860G family


```c
#define HWCALC_G35PE2 3
```


---


### `HWCALC_PRIZM`

fx-CG 10/20, also known as the "Prizm" family


```c
#define HWCALC_PRIZM 4
```


---


### `HWCALC_FXCG50`

fx-CG 50, a late extension to the Prizm family


```c
#define HWCALC_FXCG50 5
```


---


### `HWCALC_FXCG_MANAGER`

fx-CG 50 emulator, hardcoded in kernel/inth.S


```c
#define HWCALC_FXCG_MANAGER 6
```


---


### `HWCALC_FX9860G_SLIM`

fx-9860G Slim, SH-3-based fx-9860G with hardware differences


```c
#define HWCALC_FX9860G_SLIM 7
```


---


### `HWCALC_FXCP400`

fx-CP 400


```c
#define HWCALC_FXCP400 8
```


---


### `HWKBD_IO`

The keyboard uses an I/O-port-based scan method. This is possible on both SH3 and SH4, but gint will normally do it only on SH3.


```c
#define HWKBD_IO 0x01
```


---


### `HWKBD_WDD`

When using the I/O-port scanning method on SH3, whether the watchdog is used to delay I/O operations.


```c
#define HWKBD_WDD 0x02
```


---


### `HWKBD_KSI`

The keyboard uses a KEYSC-based scan method. This is only possible on SH4


```c
#define HWKBD_KSI 0x04
```


---


### `HWFS_NONE`

Unknown or no filesystem.


```c
#define HWFS_NONE 0
```


---


### `HWFS_CASIOWIN`

CASIO's in-house filesystem, now deprecated.


```c
#define HWFS_CASIOWIN 1
```


---


### `HWFS_FUGUE`

Wrapper around Kyoto Software Research's Fugue VFAT implementation.


```c
#define HWFS_FUGUE 2
```


---
