TOOLCHAIN_PREFIX ?=
CC = $(TOOLCHAIN_PREFIX)gcc
AR = $(TOOLCHAIN_PREFIX)ar
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

CFLAGS=$(CFLAG) -DPS4_7_00 -DKASLR -DNO_SYMTAB -DDO_NOT_REMAP_RWX
CFLAGS += -march=btver2 -masm=intel -std=gnu11 -ffreestanding -fno-common \
	-fPIE -pie -fno-stack-protector -fomit-frame-pointer -nostdlib -nostdinc \
	-fno-asynchronous-unwind-tables \
	-Os -Wall -Werror -Wl,--no-dynamic-linker,--build-id=none,-T,kexec.ld,--nmagic \
	-mcmodel=small -mno-red-zone

SOURCES := kernel.c kexec.c linux_boot.c linux_thunk.S uart.c firmware.c \
	acpi.c crc32.c

OBJS := $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(SOURCES)))
DEPS := $(OBJS) $(SOURCES) $(INCLUDES:%=$(INC_DIR)/%) Makefile kexec.ld

all: libkexec.a kexec.bin

%.o: %.c *.h
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

libkexec.a: $(OBJS)
	$(AR) -rc $@ $(OBJS)

kexec.elf: libkexec.a kexec.ld
	$(CC) $(CFLAGS) -o $@ libkexec.a

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f libkexec.a kexec.elf kexec.bin $(OBJS)
