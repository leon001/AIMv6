/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 * Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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
#endif /* HAVE_CONFIG_H */

/* from uart driver */
#include <uart.h>

#ifdef RAW /* baremetal driver */

int uart_puts(const char *str)
{
	for (; *str != '\0'; ++str)
#ifdef CONSOLE_NEED_CR
		if (*str == '\n')
			uart_putchar('\r');
#endif /* CONSOLE_NEED_CR */
		uart_putchar((unsigned char)*str);
}

#else /* not RAW, or kernel driver */

/*
 * If a serial console requires \r\n to change line, we usually don't
 * want to specify "\r\n" in C string.
 *
 * uart_puts() function should prepend a carriage return
 * before a newline if needed. In this case, set CONSOLE_NEED_CR during
 * configuration
 *
 * NOTE:
 * In disassemblies you can still see the code of this weak function.
 * This is *correct*.  The strong function code appears before the weak
 * function, and therefore the weak code would never be executed.
 */

/* FIXME: using early_console_puts() and device descriptor (in future),
 * should be removed */
#if 0
int uart_puts(const char *str)
{
	for (; *str != '\0'; ++str) {
#ifdef CONSOLE_NEED_CR
		if (*str == '\n')
			uart_putchar('\r');
#endif /* CONSOLE_NEED_CR */
		uart_putchar((unsigned char)*str);
	}
	return 0;
}
#endif

#endif /* RAW */
