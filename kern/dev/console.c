/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

/* from kernel */
#include <sys/types.h>
#include <console.h>
#include <mm.h>

static putchar_fp __putchar = NULL;
static puts_fp __puts = NULL;

/*
 * In most cases,
 * 1. Outputting a string to a console can be decomposed into
 *    outputting characters one-by-one.
 * 2. A new line on a console is a carriage return ('\r') followed
 *    by a line feed ('\n').
 * If your console device satisfies the two characteristics above,
 * you can use default kputs() implementation by
 *
 *     set_console(your_putchar, DEFAULT_KPUTS);
 *
 * Exceptions may exist, e.g. an efficient CGA console driver.  In
 * this case, set up console output functions by
 *
 *     set_console(your_putchar, your_puts);
 */
void set_console(putchar_fp putchar, puts_fp puts)
{
	__putchar = putchar;
	__puts = puts;
}

int kprintf(const char *fmt, ...)
{
	return 0;
}

static putchar_fp __get_kputchar(void)
{
	switch(get_addr_space()) {
	case 0:
		return early_kva2pa(__putchar);
	case 1:
		return __putchar;
	default:
		return NULL;
	}
}

int kputchar(int c)
{
	putchar_fp putchar = __get_kputchar();

	if (putchar == NULL)
		return EOF;
	return putchar(c);
}

static int __kputs(const char *s)
{
	putchar_fp putchar = __get_kputchar();

	if (putchar == NULL)
		return EOF;

	for (; *s != '\0'; ++s) {
		if (*s == '\n')
			putchar('\r');
		putchar(*s);
	}

	return 0;
}

int __default_kputs(const char *s)
{
	return __kputs(s);
}

static puts_fp __get_kputs(void)
{
	switch(get_addr_space()) {
	case 0:
		return early_kva2pa(__puts);
	case 1:
		return __puts;
	default:
		return NULL;
	}
}

int kputs(const char *s)
{
	puts_fp puts = __get_kputs();

	if (puts == NULL)
		return EOF;
	return puts(s);
}

