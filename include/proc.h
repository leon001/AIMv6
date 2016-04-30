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

#ifndef _PROC_H
#define _PROC_H

#include <sys/types.h>

struct proc {
	/*
	 * the kernel stack pointer is used to prepare C runtime, thus accessed
	 * in assembly. Force it here at offset 0 for easy access.
	 */
	void *kstack; /* bottom, or lower address */
	size_t kstack_size;

	/* other stuff go here */
};

#endif /* _PROC_H */

