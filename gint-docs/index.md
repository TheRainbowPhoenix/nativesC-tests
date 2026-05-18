# gint Documentation

Complete API reference for the gint library. This documentation covers all headers, modules, and implementation details.

## Core Modules

- [gint](gint/gint.md) - Main gint header with initialization and core functions
- [hardware](gint/hardware.md) - Hardware detection and platform information
- [cpu](gint/cpu.md) - CPU identification and features
- [clock](gint/clock.md) - Clock frequency and timing utilities
- [timer](gint/timer.md) - Timer management and sleep functions

## Display and Graphics

- [display](gint/display.md) - Drawing functions for both platforms
- [display-fx](gint/display-fx.md) - fx-9860G display specifics
- [display-cg](gint/display-cg.md) - fx-CG 50 display specifics
- [gray](gint/gray.md) - Gray engine for fx-9860G
- [image](gint/image.md) - Image manipulation and rendering
- [video](gint/video.md) - Video capture and output

## Input

- [keyboard](gint/keyboard.md) - Keyboard scanning and input
- [keycodes](gint/keycodes.md) - Key code definitions
- [touch](gint/touch.md) - Touch screen support (fx-CG 50)

## Memory Management

- [kmalloc](gint/kmalloc.md) - Kernel memory allocator
- [mmu](gint/mmu.md) - Memory management unit control

## File System

- [fs](gint/fs.md) - File system operations
- [bfile](gint/bfile.md) - BFile API for file access

## Drivers

- [drivers](gint/drivers.md) - Driver framework overview
- [asyncio](gint/drivers/asyncio.md) - Asynchronous I/O driver
- [iokbd](gint/drivers/iokbd.md) - Keyboard I/O driver
- [keydev](gint/drivers/keydev.md) - Key device driver
- [r61523](gint/drivers/r61523.md) - R61523 display controller
- [r61524](gint/drivers/r61524.md) - R61524 display controller
- [states](gint/drivers/states.md) - Power state management
- [t6k11](gint/drivers/t6k11.md) - T6K11 touch controller

## MPU (Microprocessor Unit)

- [bsc](gint/mpu/bsc.md) - Bus state controller
- [cpg](gint/mpu/cpg.md) - Clock pulse generator
- [dma](gint/mpu/dma.md) - Direct memory access
- [intc](gint/mpu/intc.md) - Interrupt controller
- [mmu](gint/mpu/mmu.md) - Memory management unit
- [pfc](gint/mpu/pfc.md) - Pin function controller
- [power](gint/mpu/power.md) - Power management
- [rtc](gint/mpu/rtc.md) - Real-time clock
- [scif](gint/mpu/scif.md) - Serial communication interface
- [spu](gint/mpu/spu.md) - Sound processing unit
- [tmu](gint/mpu/tmu.md) - Timer module unit
- [ubc](gint/mpu/ubc.md) - User break controller
- [usb](gint/mpu/usb.md) - USB controller
- [wdt](gint/mpu/wdt.md) - Watchdog timer

## USB

- [usb](gint/usb.md) - USB stack and device management
- [usb-ff-bulk](gint/usb-ff-bulk.md) - Full-speed bulk transfer class

## System Utilities

- [dma](gint/dma.md) - DMA helper functions
- [intc](gint/intc.md) - Interrupt handling
- [rtc](gint/rtc.md) - RTC helper functions
- [serial](gint/serial.md) - Serial port utilities
- [ubc](gint/ubc.md) - Debug breakpoints
- [exc](gint/exc.md) - Exception handling
- [gdb](gint/gdb.md) - GDB stub for debugging
- [prof](gint/prof.md) - Profiling tools

## Standard Library

- [stdio](gint/std/stdio.md) - Standard I/O functions
- [stdlib](gint/std/stdlib.md) - Standard library utilities
- [string](gint/std/string.md) - String manipulation
- [endian](gint/std/endian.md) - Byte order conversion

## Type Definitions and Utilities

- [types](gint/defs/types.md) - Basic type definitions
- [attributes](gint/defs/attributes.md) - Compiler attributes
- [call](gint/defs/call.md) - Indirect call mechanism
- [timeout](gint/defs/timeout.md) - Timeout handling
- [util](gint/defs/util.md) - Utility macros and functions

## JustUI Framework

- [defs](justui/defs.md) - JustUI definitions
- [jbutton](justui/jbutton.md) - Button widget
- [jevent](justui/jevent.md) - Event system
- [jfileselect](justui/jfileselect.md) - File selection dialog
- [jfkeys](justui/jfkeys.md) - Function keys
- [jframe](justui/jframe.md) - Main window frame
- [jinput](justui/jinput.md) - Input widget
- [jlabel](justui/jlabel.md) - Label widget
- [jlayout](justui/jlayout.md) - Layout management
- [jlist](justui/jlist.md) - List widget
- [jpainted](justui/jpainted.md) - Painted widget
- [jscene](justui/jscene.md) - Scene management
- [jscrolledlist](justui/jscrolledlist.md) - Scrolled list widget
- [jwidget](justui/jwidget.md) - Base widget class
- [jwidget-api](justui/jwidget-api.md) - Widget API reference

### JustUI Utilities

- [preproc](justui/p/preproc.md) - Preprocessing utilities
- [vec](justui/p/vec.md) - Vector math utilities

## Build System

- [Linker Script Guide](linker.md) - Memory layout and section organization

## Implementation Details

Each module page includes links to the corresponding source files in the gint repository. This helps you understand how functions are implemented and find usage examples.

## Repository

Source code: [ClasspadDev/gint](https://github.com/ClasspadDev/gint)
