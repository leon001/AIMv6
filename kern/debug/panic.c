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

#include <sys/types.h>

#include <console.h>

#include <libc/stdarg.h>
#include <libc/stddef.h>
#include <libc/stdio.h>

__noreturn
void __panic(void)
{
	/*
	 * We cannot simply do tight loops at panic. Modern kernels turn down
	 * processors and other devices to keep energy consumption and heat
	 * generation low.
	 * Later on there will be interfaces for the targets and drivers.
	 * We currently do a tight loop.
	 */
	 while (1);
}

__noreturn
void panic(const char *fmt, ...)
{
	/*
	 * We might be in some strange state with limited stack, use a static
	 * buffer
	 */
	static char __buf[BUFSIZ];
	va_list args;
	int result;

	/* TODO: disable interrupt and stop other processors */

	va_start(args, fmt);
	result = vsnprintf(__buf, BUFSIZ, fmt, args);
	va_end(args);

	kputs("----- KERNEL PANIC -----\n");
	if (result >= BUFSIZ) {
		kputs("PANIC: message is truncated.");
	}
	kputs(__buf);
	
	__panic();
}

