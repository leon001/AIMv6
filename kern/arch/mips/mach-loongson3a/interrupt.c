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

#include <arch-trap.h>
#include <cp0regdef.h>
#include <timer.h>
#include <errno.h>

static int __discard(struct trapframe *regs)
{
	return 0;
}

static int __timer_interrupt(struct trapframe *regs)
{
	handle_timer_interrupt();
	return 0;
}

static int (*__dispatch[])(struct trapframe *) = {
	NULL,			/* Soft interrupt 0 */
	NULL,			/* Soft interrupt 1 */
	__discard,		/* LPC/UART interrupt */
	__discard,		/* HT1 interrupt */
	NULL,			/* Unused */
	NULL,			/* Unused */
	NULL,			/* IPI */
	__timer_interrupt	/* Timer */
};

/* TODO: move to mach-specific code */
int handle_interrupt(struct trapframe *regs)
{
	int i;

	for (i = 7; i >= 0; --i) {
		if (regs->cause & CR_IPx(i)) {
			if (__dispatch[i] != NULL)
				return __dispatch[i](regs);
			else
				return -ENOENT;
		}
	}
	/* NOTREACHED */
	return -EINVAL;
}

