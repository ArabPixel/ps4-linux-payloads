/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef KEXEC_H
#define KEXEC_H

#include "types.h"

#define SYS_KEXEC 153
/*
 * 6 original args + 2 new (fw_dump_buf, fw_dump_size_ptr).
 * Args 7-8 are passed on the user stack per FreeBSD amd64 ABI;
 * the kernel reads them via fuword from td_frame->tf_rsp+8/+16.
 */
#define SYS_KEXEC_NARGS 8

struct sys_kexec_args {
	void   *image;
	size_t  image_size;
	void   *initramfs;
	size_t  initramfs_size;
	char   *cmd_line;
	int     vram_gb;
	/*
	 * Padding to fill the register_t slot for vram_gb on amd64
	 * so the two new pointer fields sit at clean 8-byte offsets.
	 */
	int     _pad;
	/*
	 * Optional firmware CPIO dump path-back to userspace.
	 *
	 * If fw_dump_buf is non-NULL and sys_kexec is called as a real
	 * syscall (td != NULL), the raw CPIO produced by firmware_extract()
	 * is copied out to this buffer and its size is written to
	 * *fw_dump_size_ptr.  Both must point to valid userspace memory.
	 * The buffer must be at least FW_CPIO_SIZE bytes (≈ 96 KB).
	 *
	 * Set both to NULL to keep the original behaviour.
	 */
	void   *fw_dump_buf;
	size_t *fw_dump_size_ptr;
};

typedef int (*sys_kexec_t)(void *td, struct sys_kexec_args *uap);

// Note: td is unused, you can pass NULL if you call this directly.
int sys_kexec(void *td, struct sys_kexec_args *uap);

int kernel_init(void *early_printf);

int kexec_init(void *early_printf, sys_kexec_t *sys_kexec_ptr)
    __attribute__ ((section (".init")));

#endif
