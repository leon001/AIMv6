/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <uart-ns16550.h>
#include <uart-ns16550-hw.h>

#include <io.h>
#include <console.h>

/*
 * FIXME:
 * Only an example demonstrating difference between x86 UART and other
 * UART accesses.
 */
#ifdef ARCH_I386
/* Here UART_BASE is a port-mapped base */
#define UART_READ(base, x)		inb((base) + (x))
#define UART_WRITE(base, x, d)		outb((base) + (x), (d))
#else /* !ARCH_I386 */
#define UART_READ(base, x)		read8((base) + (x))
#define UART_WRITE(base, x, d)		write8((base) + (x), (d))
#endif /* ARCH_I386 */

/* internal routines */

static void __uart_ns16550_init(uint32_t base)
{
	/* Currently nothing to be done here */
}

static void __uart_ns16550_enable(uint32_t base)
{
	UART_WRITE(base, UART_MODEM_CONTROL, UART_MCR_RTSC | UART_MCR_DTRC);
}

static void __uart_ns16550_disable(uint32_t base)
{
	UART_WRITE(base, UART_MODEM_CONTROL, 0);
}

static void __uart_ns16550_enable_interrupt(uint32_t base)
{
	UART_WRITE(base, UART_INTR_ENABLE, UART_IER_RBFI);
}

static void __uart_ns16550_disable_interrupt(uint32_t base)
{
	UART_WRITE(base, UART_INTR_ENABLE, 0);
}

static unsigned char __uart_ns16550_getchar(uint32_t base)
{
	while (!(UART_READ(base, UART_LINE_STATUS) & UART_LSR_DATA_READY))
		/* nothing */;
	return UART_READ(base, UART_RCV_BUFFER);
}

static int __uart_ns16550_putchar(uint32_t base, unsigned char c)
{
	while (!(UART_READ(base, UART_LINE_STATUS) & UART_LSR_THRE))
		/* nothing */;
	UART_WRITE(base, UART_TRANS_HOLD, c);
	return 0;
}

#ifdef RAW /* baremetal driver */

void uart_init(void)
{
	__uart_ns16550_init(UART_BASE);
}

void uart_enable(void)
{
	__uart_ns16550_enable(UART_BASE);
}

void uart_disable(void)
{
	__uart_ns16550_disable(UART_BASE);
}

unsigned char uart_getchar(void)
{
	return __uart_ns16550_getchar(UART_BASE);
}

int uart_putchar(unsigned char c)
{
	__uart_ns16550_putchar(UART_BASE, c);
	return 0;
}

#else /* not RAW, or kernel driver */

#if PRIMARY_CONSOLE == uart_ns16550

/* Meant to register to kernel, so this interface routine is static */
static int early_console_putchar(unsigned char c)
{
	__uart_ns16550_putchar(UART_BASE, c);
	return 0;
}

int early_console_init(void)
{
	__uart_ns16550_init(UART_BASE);
	__uart_ns16550_enable(UART_BASE);
	set_console(early_console_putchar, DEFAULT_KPUTS);
	return 0;
}

#endif /* PRIMARY_CONSOLE == uart_ns16550 */

#endif /* RAW */

