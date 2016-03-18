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

#include <uart.h>
#include <uart-msim.h>
#include <io.h>

void uart_msim_init(void)
{
}

int uart_msim_putchar(unsigned long base, unsigned char c)
{
	write8(base, c);
	return 0;
}

unsigned char uart_msim_getchar(unsigned long base)
{
	unsigned char b;
	while (!(b = read8(base)))
		/* nothing */;
	return b;
}

#ifdef RAW

void uart_init(void)
{
	/* nothing */
}

void uart_enable(void)
{
	/* nothing */
}

void uart_disable(void)
{
	/* nothing */
}

unsigned char uart_getchar(void)
{
	return uart_msim_getchar(MSIM_UART_INPUT);
}

int uart_putchar(unsigned char c)
{
	return uart_msim_putchar(MSIM_UART_OUTPUT, c);
}

#else

#endif
