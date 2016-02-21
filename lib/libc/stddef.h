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

#ifndef _LIBC_STDDEF_H
#define _LIBC_STDDEF_H

#ifndef NULL
#define NULL	(void *)0
#endif

#ifndef BUFSIZ
#define BUFSIZ		1024
#endif

#define MEMBER_OFFSET(struct, member_name) \
	((unsigned long)&(((struct *)0)->member_name))

#define member_to_struct(addr, struct, member_name) \
	((struct *)((unsigned long)(addr) - MEMBER_OFFSET(struct, member_name)))

#define MIN2(a, b)	(((a) < (b)) ? (a) : (b))

#endif /* _LIBC_STDDEF_H */

