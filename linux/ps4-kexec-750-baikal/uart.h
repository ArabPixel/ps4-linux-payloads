/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#ifndef UART_H
#define UART_H

#include "types.h"

void uart_write_byte(u8 b);
void uart_write_char(char c);
void uart_write_str(const char *s);

#endif
