# gint Documentation

Complete API documentation for the gint library. This documentation covers all public headers and their functions, data structures, and macros.

## Core Modules

- [gint](gint/gint.md) - Main runtime environment
- [bfile](gint/bfile.md) - File operations
- [clock](gint/clock.md) - Clock and timing
- [cpu](gint/cpu.md) - CPU information
- [display](gint/display.md) - Display functions
- [display-fx](gint/display-fx.md) - fx-9860G display
- [display-cg](gint/display-cg.md) - fx-CG50 display
- [dma](gint/dma.md) - DMA controller
- [drivers](gint/drivers.md) - Driver framework
- [exc](gint/exc.md) - Exception handling
- [fs](gint/fs.md) - File system
- [gdb](gint/gdb.md) - GDB stub
- [gray](gint/gray.md) - Gray engine
- [hardware](gint/hardware.md) - Hardware abstraction
- [image](gint/image.md) - Image handling
- [intc](gint/intc.md) - Interrupt controller
- [keyboard](gint/keyboard.md) - Keyboard input
- [keycodes](gint/keycodes.md) - Key code definitions
- [kmalloc](gint/kmalloc.md) - Kernel memory allocator
- [mmu](gint/mmu.md) - Memory management unit
- [rtc](gint/rtc.md) - Real-time clock
- [serial](gint/serial.md) - Serial communication
- [timer](gint/timer.md) - Timer functions
- [ubc](gint/ubc.md) - User breakpoint controller
- [usb](gint/usb.md) - USB support
- [usb-ff-bulk](gint/usb-ff-bulk.md) - USB F-F Bulk
- [video](gint/video.md) - Video capture

## Definitions (defs)

- [attributes](gint/defs/attributes.md) - Compiler attributes
- [call](gint/defs/call.md) - Indirect call mechanism
- [timeout](gint/defs/timeout.md) - Timeout handling
- [types](gint/defs/types.md) - Type definitions
- [util](gint/defs/util.md) - Utility macros

## Drivers

- [asyncio](gint/drivers/asyncio.md) - Asynchronous I/O
- [iokbd](gint/drivers/iokbd.md) - Keyboard I/O driver
- [keydev](gint/drivers/keydev.md) - Key device driver
- [r61523](gint/drivers/r61523.md) - R61523 display driver
- [r61524](gint/drivers/r61524.md) - R61524 display driver
- [states](gint/drivers/states.md) - Driver states
- [t6k11](gint/drivers/t6k11.md) - T6K11 touch driver

## MPU Registers

- [bsc](gint/mpu/bsc.md) - Bus state controller
- [cpg](gint/mpu/cpg.md) - Clock pulse generator
- [dma](gint/mpu/dma.md) - DMA registers
- [intc](gint/mpu/intc.md) - Interrupt controller registers
- [mmu](gint/mpu/mmu.md) - MMU registers
- [pfc](gint/mpu/pfc.md) - Port and function controller
- [power](gint/mpu/power.md) - Power management
- [rtc](gint/mpu/rtc.md) - RTC registers
- [scif](gint/mpu/scif.md) - Serial communication interface
- [spu](gint/mpu/spu.md) - Sound processing unit
- [tmu](gint/mpu/tmu.md) - Timer unit
- [ubc](gint/mpu/ubc.md) - User breakpoint controller registers
- [usb](gint/mpu/usb.md) - USB registers
- [wdt](gint/mpu/wdt.md) - Watchdog timer

## Standard Library (std)

- [endian](gint/std/endian.md) - Byte order functions
- [stdio](gint/std/stdio.md) - Standard I/O
- [stdlib](gint/std/stdlib.md) - Standard library
- [string](gint/std/string.md) - String functions

## JustUI Framework

- [defs](justui/defs.md) - UI definitions
- [jbutton](justui/jbutton.md) - Button widget
- [jevent](justui/jevent.md) - Event system
- [jfileselect](justui/jfileselect.md) - File selection dialog
- [jfkeys](justui/jfkeys.md) - Function keys
- [jframe](justui/jframe.md) - Frame container
- [jinput](justui/jinput.md) - Input widget
- [jlabel](justui/jlabel.md) - Label widget
- [jlayout](justui/jlayout.md) - Layout manager
- [jlist](justui/jlist.md) - List widget
- [jpainted](justui/jpainted.md) - Painted widget
- [jscene](justui/jscene.md) - Scene management
- [jscrolledlist](justui/jscrolledlist.md) - Scrolled list
- [jwidget](justui/jwidget.md) - Base widget
- [jwidget-api](justui/jwidget-api.md) - Widget API

## JustUI Internal (p)

- [preproc](justui/p/preproc.md) - Preprocessor utilities
- [vec](justui/p/vec.md) - Vector utilities
