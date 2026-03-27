/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef ELF_H
#define ELF_H

#include "types.h"

#define EI_NIDENT 16

typedef struct {
    u8 e_ident[EI_NIDENT];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u64 e_entry;
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shtrndx;
} Elf64_Ehdr;

typedef struct {
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    void *p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
} Elf64_Phdr;

#define	PT_DYNAMIC  2
#define PT_PHDR     6

#define	DT_NULL     0
#define	DT_STRTAB   5
#define	DT_SYMTAB   6
#define	DT_STRSZ    10

typedef struct {
    s64 d_tag;
    union {
        u64 d_val;
        void *d_ptr;
    } d_un;
} Elf64_Dyn;

typedef struct {
    u32 st_name;
    u8 st_info;
    u8 st_other;
    u16 st_shndx;
    void *st_value;
    u64 st_size;
} Elf64_Sym;

#endif
