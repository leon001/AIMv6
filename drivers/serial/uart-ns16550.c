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

#include <io.h>
#include <uart-ns16550.h>
#include <uart-ns16550-regs.h>

/*
 * FIXME:
 * Only an example demonstrating difference between x86 UART and other
 * UART accesses.
 */
#ifdef X86
/* Here UART_BASE is a port-mapped base */
#define UART_READ(x)		inb(UART_BASE + (x))
#define UART_WRITE(x, d)	outb(UART_BASE + (x), (d))
#else	/* !X86 */
#define UART_READ(x)		read8(UART_BASE + (x))
#define UART_WRITE(x, d)	write8(UART_BASE + (x), (d))
#endif

void uart_init(void)
{
	/*
	 * XXX:
	 * It's probably best to leave NS16550 setup to machine-specific
	 * code.  The initialization routine between Loongson 3A and QEMU-i386
	 * are *VERY* different.
	 */
}

void uart_enable(void)
{
	UART_WRITE(UART_MODEM_CONTROL, UART_MCR_RTSC | UART_MCR_DTRC);
}

void uart_disable(void)
{
	UART_WRITE(UART_MODEM_CONTROL, 0);
}

void uart_enable_interrupt(void)
{
	UART_WRITE(UART_INTR_ENABLE, UART_IER_RBFI);
}

void uart_disable_interrupt(void)
{
	UART_WRITE(UART_INTR_ENABLE, 0);
}

unsigned char uart_getbyte(void)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_DATA_READY))
		/* nothing */;
	return UART_READ(UART_RCV_BUFFER);
}

void uart_putbyte(unsigned char byte)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_THRE))
		/* nothing */;
	UART_WRITE(UART_TRANS_HOLD, byte);
}

