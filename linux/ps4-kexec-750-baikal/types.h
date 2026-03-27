/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef TYPES_H
#define TYPES_H

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#ifndef TESTING
typedef u8 uint8_t;
typedef u64 size_t;
typedef s64 ssize_t;
typedef u64 uintptr_t;
typedef s64 off_t;
#endif

#define NULL ((void *)0)

#define CAT_(x, y) x ## y
#define CAT(x, y) CAT_(x, y)

#define OPAD(size) u8 CAT(_pad_, __COUNTER__)[size]
#define OSTRUCT(name, size) struct name { union { OPAD(size);
#define OSTRUCT_END };};
#define OFIELD(off, field) struct { OPAD(off); field; }

#define ASSERT_STRSIZE(struc, size) \
    _Static_assert(sizeof( struc ) == (size), "size of " #struc " != " #size )

#define offsetof(type, member)  __builtin_offsetof (type, member)

#define ENOENT 2
#define ENOMEM 12
#define EFAULT 14
#define EINVAL 22
#define ENAMETOOLONG 63

#endif
