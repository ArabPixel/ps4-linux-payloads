/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef LINUX_BOOT_H
#define LINUX_BOOT_H

#include "types.h"

#define SMAP_TYPE_MEMORY		1
#define SMAP_TYPE_RESERVED		2
#define SMAP_TYPE_ACPI_RECLAIM	3
#define SMAP_TYPE_ACPI_NVS		4
#define SMAP_TYPE_UNUSABLE		5
#define SMAP_TYPE_PMEM			7

#define X86_SUBARCH_PS4 5

struct e820entry {
	u64 addr;     /* start of memory segment */
	u64 size;     /* size of memory segment */
	u32 type;     /* type of memory segment */
} __attribute__((packed));

struct setup_header {
	u8    setup_sects;
	u16   root_flags;
	u32   syssize;
	u16   ram_size;
	u16   vid_mode;
	u16   root_dev;
	u16   boot_flag;
	u16   jump;
	u32   header;
	u16   version;
	u32   realmode_swtch;
	u16   start_sys;
	u16   kernel_version;
	u8    type_of_loader;
	u8    loadflags;
	u16   setup_move_size;
	u32   code32_start;
	u32   ramdisk_image;
	u32   ramdisk_size;
	u32   bootsect_kludge;
	u16   heap_end_ptr;
	u8    ext_loader_ver;
	u8    ext_loader_type;
	u32   cmd_line_ptr;
	u32   initrd_addr_max;
	u32   kernel_alignment;
	u8    relocatable_kernel;
	u8    min_alignment;
	u16   xloadflags;
	u32   cmdline_size;
	u32   hardware_subarch;
	u64   hardware_subarch_data;
	u32   payload_offset;
	u32   payload_length;
	u64   setup_data;
	u64   pref_address;
	u32   init_size;
	u32   handover_offset;
} __attribute__((packed));

#define E820MAX 128             /* number of entries in E820MAP */

OSTRUCT(boot_params, 0x1000)
OFIELD(0x0c0, u32 ext_ramdisk_image);
OFIELD(0x0c4, u32 ext_ramdisk_size);
OFIELD(0x0c8, u32 ext_cmd_line_ptr);
OFIELD(0x1e8, u8 e820_entries);
OFIELD(0x1f1, struct setup_header hdr);
OFIELD(0x2d0, struct e820entry e820_map[E820MAX]);
OSTRUCT_END

void set_nix_info(void *linux_image, struct boot_params *bp, void *initramfs,
                  size_t initramfs_size, char *cmd_line, int v);
void prepare_boot_params(struct boot_params *bp, u8 *linux_image);
int hook_icc_query_nowait(u8 *icc_msg);

#endif
