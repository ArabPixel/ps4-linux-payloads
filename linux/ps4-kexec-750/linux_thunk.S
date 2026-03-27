/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

.intel_syntax noprefix

.equ setup_sects, 0x1f1
.equ shdr_syssize, 0x1f4
.equ pref_address, 0x258

.text

#void jmp_to_linux(
#    uintptr_t image_base,      rdi
#    uintptr_t bootargs,        rsi
#    uintptr_t new_cr3,         rdx
#    uintptr_t gdt_ptr          rcx
#);
.globl jmp_to_linux
jmp_to_linux:
    # switch to new gdt + data segments
    cli
    lgdt [rcx]
    #xor eax, eax
    mov eax, 0x18
    mov ds, eax
    mov ss, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    # switch to our own page tables (in low mem)
    mov cr3, rdx

    # now we're on our own page tables, so we can obliterate the rest of memory
    # TODO make sure we don't inadvertently overwrite (important) smap regions
    #   I think on ps4 we'll actually want to load to 0x700000
    #   since we have tons of free room there.
    #   on 4.00/4.01, bzImage might be allocated at 0x800000 accidently. We
    #   should choose a higher address as pref_address.

    # save args
    mov r12, rdi
    mov r13, rsi

    # memmove(pref_address, , (syssize * 0x10) / 8)
    #mov rdi, [r12 + pref_address]  # dst = [image_base + pref_address] ; where linux image wants to go
    #mov rdi, 0x700000               # where ps4 freebsd kernel is loaded (before relocating itself)
    mov rdi, 0x6000000				# should be far from bzImage and initramfs
    mov r14, rdi                    # r14 = pref_address
    xor edx, edx
    mov dl, [r12 + setup_sects]
    inc rdx
    shl rdx, 9                      # rdx = offsetof(image_base, startup_32)
    lea rsi, [r12 + rdx]            # src = image_base + startup_32
    mov ecx, [r12 + shdr_syssize]
    shl rcx, 4
    add rdi, rcx
    add rsi, rcx
    sub rdi, 8
    sub rsi, 8
    shr rcx, 3
    std
    rep movsq
    cld

    # make a tiny stack - we just need it for the lretq.
    # what we jump to will not use this stack
    lea rsp, [rip + jmp_to_linux_end + 0x200]
    and rsp, -0x10
    #push 0                          # retaddr
    push 0x10                       # cs = GDT[2]
    add r14, 0x200                  # pref_address + startup_64
    push r14                        # rip
    mov rsi, r13                    # bootargs
    lretq
jmp_to_linux_end:

.data

.globl jmp_to_linux_size
jmp_to_linux_size: .quad jmp_to_linux_end - jmp_to_linux

.att_syntax prefix
