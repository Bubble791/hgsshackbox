# environment setting
# get rid of devkitpro:  if devkitpro is installed, can still use it.  otherwise, default to arm-none-eabi tools
ifeq ($(shell echo $$DEVKITARM),)
ifeq ($(MSYS2), 0)
PREFIX = /mingw64/bin/arm-none-eabi-
AS = $(PREFIX)as
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
else
PREFIX = arm-none-eabi-
AS = $(PREFIX)as
CC = $(PREFIX)gcc
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
endif
else
# support legacy devkitpro instructions
PREFIX = bin/arm-none-eabi-
AS = $(DEVKITARM)/$(PREFIX)as
CC = $(DEVKITARM)/$(PREFIX)gcc
LD = $(DEVKITARM)/$(PREFIX)ld
OBJCOPY = $(DEVKITARM)/$(PREFIX)objcopy
endif
PYTHON = python3

.PHONY: clean all

default: all

####################### Tools #######################
ARMIPS 	:= tools/armips
GFX		:= tools/nitrogfx/nitrogfx

# Compiler/Assembler/Linker settings
LDFLAGS = rom.ld -T linker.ld
ASFLAGS = -mthumb -I ./data
CFLAGS = -mthumb -mno-thumb-interwork -mcpu=arm7tdmi -mtune=arm7tdmi -march=armv4t -Wall -Wextra -Wno-parentheses -Wno-sign-compare -Wno-multichar -Wno-unused-parameter -Wno-builtin-declaration-mismatch -Wno-sequence-point -Wno-ignored-qualifiers -Wno-address-of-packed-member -Wno-old-style-declaration -Os -fira-loop-pressure -fipa-pta
GLB_DEFINES := -DSDK_ARM9 -DSDK_CODE_ARM -DSDK_FINALROM

####################### Output #######################
C_SUBDIR = src
ASM_SUBDIR = asm
INCLUDE_SUBDIR = include
BUILD := build

LINK = $(BUILD)/linked.o
OUTPUT = $(BUILD)/output.bin

INCLUDE_SRCS := $(wildcard $(INCLUDE_SUBDIR)/*.h)
C_SRCS := $(wildcard $(C_SUBDIR)/*.c)
C_OBJS := $(patsubst $(C_SUBDIR)/%.c,$(BUILD)/%.o,$(C_SRCS))

ASM_SRCS := $(wildcard $(ASM_SUBDIR)/*.s)
ASM_OBJS := $(patsubst $(ASM_SUBDIR)/%.s,$(BUILD)/%.d,$(ASM_SRCS))
OBJS     := $(C_OBJS) $(ASM_OBJS)

$(BUILD)/%.d: asm/%.s
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -I lib/include -I include -I include/library $(GLB_DEFINES) -c $< -o $@

$(LINK):$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

include files.mk

all: $(OBJS) hackBox.narc
	$(ARMIPS) main.s -sym2 output.sym
	python3 make.py rom.nds game_patched.nds

clean:
	rm -rf $(BUILD)