FIRMWARES = 505 672 700 900 903 960 1000 1050 1100 1102 1150 1200 1250 1300 1302
SIZES     = 1gb 2gb 3gb 4gb

# Generate payload targets automatically
PAYLOADS = $(foreach fw,$(FIRMWARES), \
             $(foreach sz,$(SIZES), \
               fw$(fw)/payload-$(fw)-$(sz).bin \
               fw$(fw)/payload-$(fw)-$(sz)-pro.bin \
               fw$(fw)/payload-$(fw)-$(sz)-baikal.bin \
               fw$(fw)/payload-$(fw)-$(sz)-pro-baikal.bin))

# Common compiler flags
CC = gcc
CFLAGS = -isystem ../freebsd-headers -nostdinc -nostdlib -fno-stack-protector -static -fPIE -ffreestanding
LDFLAGS = -Wl,-gc-sections

# VRAM size flags - using conditional to avoid empty variable issues
define VRAM_FLAGS
$(if $(filter 2gb,$(1)),-DVRAM_GB_DEFAULT=2,$(if $(filter 3gb,$(1)),-DVRAM_GB_DEFAULT=3,$(if $(filter 4gb,$(1)),-DVRAM_GB_DEFAULT=4)))
endef

# Firmware version formatting helper (handles both 3-digit and 4-digit versions)
# 505 -> 5_05, 1000 -> 10_00, 1302 -> 13_02
define FW_VERSION
$(shell printf '%s' '$(1)' | sed 's/\(.*\)\(..\)/\1_\2/')
endef

.PHONY: all clean

all: $(PAYLOADS)

clean:
	rm -f fw*/*
	@for fw in $(FIRMWARES); do \
		for var in "" "-pro" "-baikal" "-pro-baikal"; do \
			if [ -d "ps4-kexec-$${fw}$${var}" ]; then \
				cd "ps4-kexec-$${fw}$${var}" && $(MAKE) clean && cd ..; \
			fi; \
		done; \
	done
	cd ../lib/ && $(MAKE) clean

# Library dependency
../lib/lib.a:
	cd ../lib && $(MAKE)

# Pattern rules for kexec binaries
ps4-kexec-%/kexec.bin:
	cd ps4-kexec-$* && $(MAKE)

# Convert ELF to BIN
%.bin: %.elf
	objcopy $< --only-section .text --only-section .data --only-section .bss --only-section .rodata -O binary $@
	file $@ | fgrep -q '$@: DOS executable (COM)'

# Template function to generate rules
# Usage: $(call make-payload-rule,firmware,size,main-source,variant-suffix)
define make-payload-rule
fw$(1)/payload-$(1)-$(2)$(4).elf: ../lib/lib.a $(3) ps4-kexec-$(1)$(4)/kexec.bin
	$(CC) $(CFLAGS) ../lib/lib.a -D__$(call FW_VERSION,$(1))__ -DPS4_$(call FW_VERSION,$(1)) $(call VRAM_FLAGS,$(2)) $(3) $(LDFLAGS) -o $$@
endef

# Generate all rules for each firmware
$(foreach fw,$(FIRMWARES), \
  $(foreach sz,$(SIZES), \
    $(eval $(call make-payload-rule,$(fw),$(sz),main.c,)) \
    $(eval $(call make-payload-rule,$(fw),$(sz),main.c,-pro)) \
    $(eval $(call make-payload-rule,$(fw),$(sz),main-baikal.c,-baikal)) \
    $(eval $(call make-payload-rule,$(fw),$(sz),main-baikal.c,-pro-baikal)) \
  ) \
)