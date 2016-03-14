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

void set_console(putchar_fp putchar, puts_fp puts)
{
	__putchar = putchar;
	__puts = puts;
}

int kprintf(const char *fmt, ...)
{
	return 0;
}

int kputchar(int c)
{
	putchar_fp putchar = __putchar;

	switch(get_addr_space()){
		case 0:
			putchar = early_kva2pa(__putchar);
			break;
		case 1:
			putchar = __putchar;
			break;
		default:
			putchar = NULL;
	}

	if (putchar == NULL) return EOF;
	return putchar(c);
}

int kputs(const char *s)
{
	puts_fp puts = __puts;

	switch(get_addr_space()){
		case 0:
			puts = early_kva2pa(__puts);
			break;
		case 1:
			puts = __puts;
			break;
		default:
			puts = NULL;
	}

	if (puts == NULL) return EOF;
	return puts(s);
}

