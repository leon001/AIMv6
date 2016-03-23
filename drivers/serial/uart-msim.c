/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
 * Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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
#include <console.h>

/* uart-msim is a combined device, so there's two base address */
static void __uart_msim_init(addr_t lp_base, addr_t kbd_base)
{
	/* nothing */
}

static int __uart_msim_putchar(addr_t base, unsigned char c)
{
	write8(base, c);
	return 0;
}

static unsigned char __uart_msim_getchar(addr_t base)
{
	unsigned char b;
	while (!(b = read8(base)))
		/* nothing */;
	return b;
}

#ifdef RAW /* baremetal driver */

void uart_init(void)
{
	__uart_msim_init();
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

#else /* not RAW, or kernel driver */

#if PRIMARY_CONSOLE == uart_msim

int early_console_init(void)
{
	__uart_msim_init(void);
	set_console(__uart_msim_putchar, DEFAULT_KPUTS);
	return 0;
}

#endif /* PRIMARY_CONSOLE == uart_msim */

#endif /* RAW */

