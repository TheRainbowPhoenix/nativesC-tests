SOURCEDIR = src
BUILDDIR = obj
OUTDIR = dist
DEPDIR = .deps

# Compiler prefix based on CI logs
CROSS_COMPILE := sh-elf-

AS:=$(CROSS_COMPILE)gcc
AS_FLAGS:=-gdwarf-5

# The SDK is usually in the sysroot for fxsdk images
SDK_DIR?=/home/dev/.local/share/fxsdk/sysroot/sh3eb-elf

DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
WARNINGS=-Wall -Wextra -pedantic -Werror -pedantic-errors
INCLUDES=-I$(SDK_DIR)/include -Isrc
DEFINES=
FUNCTION_FLAGS=-flto=auto -ffat-lto-objects -fno-builtin -ffunction-sections -fdata-sections -gdwarf-5 -O2
COMMON_FLAGS=$(FUNCTION_FLAGS) $(INCLUDES) $(WARNINGS) $(DEFINES)

CC:=$(CROSS_COMPILE)gcc
CC_FLAGS=-std=c23 $(COMMON_FLAGS)

CXX:=$(CROSS_COMPILE)g++
CXX_FLAGS=-std=c++20 $(COMMON_FLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit

LD:=$(CROSS_COMPILE)g++
LD_FLAGS:=$(FUNCTION_FLAGS) -Wl,--gc-sections
LIBS:=-L$(SDK_DIR)/lib -lsdk -lgint

READELF:=$(CROSS_COMPILE)readelf
OBJCOPY:=$(CROSS_COMPILE)objcopy
STRIP:=$(CROSS_COMPILE)strip

APP_ELF := $(OUTDIR)/CED.elf
APP_HH3 := $(APP_ELF:.elf=.hh3)

CXX_SOURCES:=$(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS := $(addprefix $(BUILDDIR)/,$(CXX_SOURCES:.cpp=.o))

DEPFILES := $(OBJECTS:$(BUILDDIR)/%.o=$(DEPDIR)/%.d)

hh3: $(APP_HH3) Makefile
elf: $(APP_ELF) Makefile

all: elf hh3
.DEFAULT_GOAL := all
.SECONDARY:

.NOTPARALLEL: clean
clean:
	rm -rf $(BUILDDIR) $(OUTDIR) $(DEPDIR)

%.hh3: %.elf
	$(STRIP) -o $@ $^

$(APP_ELF): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -Wl,-Map $@.map -o $@ $(LD_FLAGS) $^ $(LIBS)

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$<)
	$(CXX) -c $< -o $@ $(CXX_FLAGS) $(DEPFLAGS)

.PHONY: elf hh3 all clean
-include $(DEPFILES)
