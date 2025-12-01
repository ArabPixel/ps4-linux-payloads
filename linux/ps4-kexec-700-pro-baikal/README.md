# PS4 kexec implementation

This repo implements a kexec()-style system call for the PS4 Orbis kernel
(FreeBSD derivative). This is designed to boot a Linux kernel directly from
FreeBSD.

This is not an exploit. It is useless without some mechanism of injecting code
into the PS4 OS kernel.

## Building

To build a kexec.bin relocatable binary using the supplied Makefile, just type
`make`. This will also build a kexec.a archive. You can either use the binary
directly, or link the archive into your own project.

If you link kexec.a with your own code, you need to supply the two symbols
`_start` and `_end` in your linker script, as `kernel_init()` will try to remap
all pages covered by that range as RWX (to make global variable accesses work).
Alternatively, you can add `-DDO_NOT_REMAP_RWX` to CFLAGS to disable this
feature, if you have already taken care of page permissions for the code.

If you use a compiler toolchain that have a special prefix you can declare it
by passing TOOLCHAIN_PREFIX option to the Makefile like this:

    make TOOLCHAIN_PREFIX='amd64-marcel-freebsd9.0-'

## Usage

The code is designed to be completely standalone. There is a single entry point:

    int kexec_init(void *early_printf, sys_kexec_t *sys_kexec_ptr);

Simply call `kexec_init(NULL, NULL)`. This will locate all the required kernel
symbols and install the sys_kexec system call. The syscall is registered
as number 153 by default (you can change this in kexec.h). The return value
is 0 on success, or negative on error.

You may pass something other than NULL as `early_printf`. In that case, that
function will be used for debug output during early symbol resolution, before
printf is available.

Since PS4 3.55(?), KASLR(Kernel Address Space Layout Randomization) is
enabled by default, symtab also disappears in newer kernel, we have to
hardcode offsets for some symbols. Currently we use the `early_printf`
given by user to caculate the base address of kernel, then relocate all the
symbols from the kernel base. You could enable this feature like this:

    make CFLAG='-DPS4_4_00 -DKASLR -DNO_SYMTAB'

If you do not want to call the syscall from userspace, you can pass the address
of a function pointer as `sys_kexec_ptr`. `kexec_init` will write to it the
address of `sys_kexec`, so you can invoke it manually (see kexec.h for
its prototype and how the arguments are passed).

If you are using the standalone kexec.bin blob, then the `kexec_init` function
is always located at offset 0, so simply call the base address of the blob.
Don't forget to pass two NULL arguments (or the appropriate pointers).

The injected `sys_kexec` system call takes (userspace) pointers to the kernel
and initramfs blobs, their sizes, and a pointer to the (null-terminated) command
line string. From userspace, this looks like this:

    int kexec(void *kernel_image, size_t image_size,
              void *initramfs, size_t initramfs_size,
              const char *cmdline);

    // syscall() usage:
    syscall(153, kernel_image, image_size, initramfs, initramfs_size, cmdline);

`kexec()` will load the kernel and initramfs into memory, but will not directly
boot them. To boot the loaded kernel, shut down the system. This can be
accomplished by pressing the power button, but can also be done more quickly
and reliably from userspace with the following sequence of system calls (this
kills userspace quickly but still does a controlled filesystem unmount):

    int evf = syscall(540, "SceSysCoreReboot");
    syscall(546, evf, 0x4000, 0);
    syscall(541, evf);
    // should be syscall(37, 1, 30) but only tested via kill symbol
    kill(1, 30);

Note that this software should be loaded into kernel memory space. If you are
running kernel code from userland mappings, you should either switch to kernel
mappings or separately copy kexec.bin to a location in kernel address space.
While syscalls or exploit code may run properly from userland, the shutdown hook
will not, as it will be called from a different process context.

## Features

`kernel_init()` will automatically find the Orbis OS kernel and resolve all
necessary symbols to work. There are no static symbol dependencies. If
`DO_NOT_REMAP_RWX` is not defined (the default), it will also patch
`pmap_protect` to disable the W^X restriction.

In addition to loading the user-supplied initramfs, `kexec` will locate the
Radeon firmware blobs inside Orbis OS, extract them, convert them to a format
suitable for Linux, and append them as an additional initramfs cpio image to
the existing initramfs. This avoids the need to distribute the Radeon firmware
blobs. The `radeon` module, when compiled into the kernel, will automatically
load this firmware on boot. Note however that most typical initramfs scripts
will wipe the initramfs contents while pivoting to the real system, so if you
compile `radeon` as a module you may not be able to access the firmware after
boot. To cover that case, add some code to your initramfs `/init` script to
copy the firmware to a tmpfs mounted on the real filesystem:

    # assuming real root FS is mounted on /mnt

    mkdir -p /mnt/lib/firmware/radeon
    mount -t tmpfs none /mnt/lib/firmware/radeon
    cp /lib/firmware/radeon/* /mnt/lib/firmware/radeon/

    # now switch_root to /mnt

This avoids having to permanently store copies of the Radeon firmware, which
isn't really necessary for most use cases.

There is significant debug logging available, which will appear on the system
UART. Most of the code relies on the kernel `printf` implementation, and
therefore you should patch out the UART output blanker to see it. The final
code that runs on the boot CPU before booting the kernel uses direct UART
writes and is not affected by the blanking feature of Orbis OS.
