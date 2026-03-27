/*
 * string.h -- standard C string-manipulation functions.
 *
 * Copyright (C) 2008      Segher Boessenkool <segher@kernel.crashing.org>
 * Copyright (C) 2009      Haxx Enterprises <bushing@gmail.com>
 * Copyright (C) 2010-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * Portions taken from the Public Domain C Library (PDCLib).
 * http://pdclib.rootdirectory.de/
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef STRING_H
#define STRING_H

#include "types.h"

static inline int strcmp(const char *s1, const char *s2)
{
    size_t i;

    for (i = 0; s1[i] && s1[i] == s2[i]; i++)
        ;

    return s1[i] - s2[i];
}

static inline void *memset(void *b, int c, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
        ((unsigned char *)b)[i] = c;

    return b;
}

static inline void *memcpy(void *dst, const void *src, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
        ((unsigned char *)dst)[i] = ((unsigned char *)src)[i];

    return dst;
}

static inline int memcmp(const void *s1, const void *s2, size_t len)
{
    size_t i;
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    for (i = 0; i < len; i++)
        if (p1[i] != p2[i])
            return p1[i] - p2[i];

    return 0;
}

static inline size_t strlen(const char *s)
{
    size_t len;

    for (len = 0; s[len]; len++)
        ;

    return len;
}

static inline size_t strnlen(const char *s, size_t count)
{
    size_t len;

    for (len = 0; s[len] && len < count; len++)
        ;

    return len;
}

static inline size_t strlcpy(char *dest, const char *src, size_t maxlen)
{
    size_t len, needed;

    len = needed = strnlen(src, maxlen - 1) + 1;
    if (len >= maxlen)
        len = maxlen - 1;

    memcpy(dest, src, len);
    dest[len] = 0;

    return needed - 1;
}

#endif
