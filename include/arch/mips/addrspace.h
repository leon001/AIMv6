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

#ifndef _ADDRSPACE_H
#define _ADDRSPACE_H

/*
 * Note: only TO_CAC and TO_UNCAC should be used.
 * You probably won't ever need these constants.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(USE_MIPS32)

#define USEG		0x00000000
#define KSEG0		0x80000000
#define KSEG1		0xa0000000
#define KSSEG		0xc0000000
#define KSEG3		0xe0000000

#define IO_CAC_BASE	KSEG0
#define IO_UNCAC_BASE	KSEG1

#elif defined(USE_MIPS64)

#define USEG		0x0000000000000000
#define KSEG0		0xffffffff80000000
#define KSEG1		0xffffffffa0000000
#define KSSEG		0xffffffffc0000000
#define KSEG3		0xffffffffe0000000

#define XSSEG		0x4000000000000000
#define XKPHY		0x8000000000000000
#define XKSEG		0xc000000000000000

#define IO_CAC_BASE	(XKPHY + 0x1800000000000000)
#define IO_UNCAC_BASE	(XKPHY + 0x1000000000000000)

#endif	/* USE_MIPS32 || USE_MIPS64 */

#define TO_CAC(x)	(IO_CAC_BASE + (x))
#define TO_UNCAC(x)	(IO_UNCAC_BASE + (x))

#define __premap_addr(x)	(x)
#define __postmap_addr(x)	(x)

#ifndef __ASSEMBLER__

static inline unsigned long kva2pa(void *x)
{
	unsigned long a = (unsigned long)x;
	if (a > KSEG1)
		return a - KSEG1;
	else if (a > KSEG0)
		return a - KSEG0;
#ifdef USE_MIPS64
	else if (a > IO_CAC_BASE)
		return a - IO_CAC_BASE;
	else if (a > IO_UNCAC_BASE)
		return a - IO_UNCAC_BASE;
#endif
	else
		return -1;	/* should be something like panic() */
}

static inline void *pa2kva(unsigned long x)
{
	return (void *)(TO_CAC(x));
}

#endif	/* !__ASSEMBLER__ */

#endif
