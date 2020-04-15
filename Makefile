PREFIX = arm-none-eabi-
CXX = g++
CC = gcc
LD = gcc
OBJDUMP = objdump
OBJCOPY = objcopy

OBJS   = $(patsubst system/%,build/system/%.o,$(shell find system/ -name '*.c' -or -name '*.cpp'))
OBJS  += $(patsubst src/%,build/src/%.o,$(shell find src/ -name '*.c' -or -name '*.cpp'))
DEPS   = $(OBJS:%.o=%.d)

DEFS		+= -DSTM32F746xx
DEFS		+= -DUSE_HAL_DRIVER

INCS		+= -Iinclude
INCS		+= -Isystem/include
INCS		+= -Isystem/include/stm32f7-hal
INCS		+= -Isystem/include/cmsis
INCS		+= -Isystem/include/arm
INCS		+= -Isystem/include/components
INCS		+= -Isystem/inclue/cortexm
INCS		+= -Isystem/include/bsp

ARCH_FLAGS   = -mthumb -march=armv7e-m+fpv5

CFLAGS      += -std=gnu11 -Wall -Wpedantic
CPPFLAGS	+= -ggdb3 -MD $(DEFS) $(INCS) $(ARCH_FLAGS)

LDSCRIPT 	 = system/stm32f746-disco.ld

LDFLAGS     += -static -nostartfiles
LDFLAGS		+= $(ARCH_FLAGS)

LDLIBS	 	+= -Wl,-lc -Wl,-lgcc -Wl,-lnosys

MAKEFLAGS += --no-print-directory

.SILENT:
.SECONDARY:
.PHONY: clean all flash

all: build/firmware.elf build/firmware.bin

%.bin: %.elf
	printf "  OBJCOPY $(*).bin\n"
	$(PREFIX)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.list: %.elf
	printf "  OBJDUMP $(*).list\n"
	$(PREFIX)$(OBJDUMP) -S $(*).elf > $(*).list

build/firmware.elf: $(OBJS) $(LDSCRIPT)
	printf "  LD      $(@)\n"
	$(PREFIX)$(LD) $(LDFLAGS) -T$(LDSCRIPT) $(OBJS) $(LDLIBS) -o $@

build/%.c.o: %.c
	@mkdir -p $(@D)
	printf "  CC      $(*).c\n"
	$(PREFIX)$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

build/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	printf "  CXX     $(*).cpp\n"
	$(PREFIX)$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

flash: build/firmware.bin
	$(PREFIX)gdb -ex "set confirm off" -ex "load" -ex "monitor reset run" -ex "quit"

clean:
	printf "  CLEAN\n"
	$(RM) -r build/

-include $(DEPS)
