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
ARMIPS := tools/armips/build/armips

# Compiler/Assembler/Linker settings
LDFLAGS = rom.ld -T linker.ld
ASFLAGS = -mthumb -I ./data
CFLAGS = -mthumb -mno-thumb-interwork -mcpu=arm7tdmi -mtune=arm7tdmi -mno-long-calls -march=armv4t -Wall -Wextra -Wno-builtin-declaration-mismatch -Wno-sequence-point -Wno-address-of-packed-member -Os -fira-loop-pressure -fipa-pta

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
	@echo -e "Compiling"
	$(CC) $(CFLAGS) -I include -c $< -o $@

$(LINK):$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

all: $(LINK)
	$(ARMIPS) main.s

clean:
	rm -rf $(BUILD)