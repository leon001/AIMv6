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

/* moved from drivers/serial/uart-zynq.c */
#include <console.h>

#if PRIMARY_CONSOLE == uart_zynq

#include <drivers/serial/uart-zynq.h>
#include <drivers/serial/uart-zynq-hw.h>

/* FIXME zedboard uses UART1 only */
#define UART_BASE	UART1_PHYSBASE

int early_console_putchar(unsigned char c)
{
	return uart_zynq_putchar(UART_BASE, c);
}

void early_console_init(void)
{
	uart_zynq_init(UART_BASE);
	uart_zynq_enable(UART_BASE);
	set_console(early_console_putchar, DEFAULT_KPUTS);
}

#endif /* PRIMARY_CONSOLE */


