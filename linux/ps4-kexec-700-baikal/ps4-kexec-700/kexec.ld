/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

ENTRY(kexec_init)

SECTIONS {
    /* We don't do GOT relocation and rely on nothing ending up using the GOT
     * (-fno-common helps here) */
    /DISCARD/ : { *(.comment) *(.got) }
    _start = .;
    .init : { *(.init) *(.init.*) }
    .text : { *(.text) *(.text.*) }
    .data : { *(.data) *(.data.*) }
    .rodata : { *(.rodata) *(.rodata.*) }
    .bss : { *(.bss) *(.bss.*) *(COMMON)}
    .footer : { LONG(0xdeadbeef) } /* make sure .bss is padded out in raw binary */
    _end = .;
}
