/*
 * ps4-kexec - a kexec() implementation for Orbis OS / FreeBSD
 *
 * Copyright (C) 2015-2016 shuffle2 <godisgovernment@gmail.com>
 * Copyright (C) 2015-2016 Hector Martin "marcan" <marcan@marcan.st>
 *
 * This code is licensed to you under the 2-clause BSD license. See the LICENSE
 * file for more information.
 */

#include "uart.h"
#include "kernel.h"
#define PHYS_TO_DMAP(size, addr) ((volatile u##size *)(kern.dmap_base | (uintptr_t)(addr)))

#define AEOLIA_UART_BASE 0xD0340000
#define BAIKAL_UART_BASE 0xC890E000

#define UART_REG(size, intf, reg) PHYS_TO_DMAP(size, AEOLIA_UART_BASE + (intf << 12) + (reg << 2))

#define UART_REG_DATA	0
#define UART_REG_IER	1
#define UART_REG_IIR	2
#define UART_REG_LCR	3
#define UART_REG_MCR	4
#define UART_REG_LSR	5
#	define	LSR_TXRDY	0x20
#	define	LSR_TEMT	0x40
#define UART_REG_MSR	6
#define UART0_DATA	UART_REG( 8, 0, UART_REG_DATA)
#define UART0_IER	UART_REG(32, 0, UART_REG_IER)
#define UART0_IIR	UART_REG(32, 0, UART_REG_IIR)
#define UART0_LCR	UART_REG(32, 0, UART_REG_LCR)
#define UART0_MCR	UART_REG(32, 0, UART_REG_MCR)
#define UART0_LSR	UART_REG(32, 0, UART_REG_LSR)
#define UART0_MSR	UART_REG(32, 0, UART_REG_MSR)

void uart_write_byte(u8 b)
{
    int limit;
    u64 barrier;
    limit = 250000;
    while (!(*UART0_LSR & LSR_TXRDY) && --limit)
        ;
    *UART0_DATA = b;
    __sync_fetch_and_add(&barrier, 0);
    limit = 250000;
    while (!(*UART0_LSR & LSR_TEMT) && --limit)
        ;
}

void uart_write_char(char c)
{
    if (c == '\n')
        uart_write_byte('\r');

    uart_write_byte(c);
}

void uart_write_str(const char *s)
{
    while (*s) {
        uart_write_char(*s++);
    }
}
