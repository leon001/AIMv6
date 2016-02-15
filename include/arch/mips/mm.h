/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_H
#define _ASM_MM_H

#include <addrspace.h>

static inline unsigned long kv2p(void *x)
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

static inline void *p2kv(unsigned long x)
{
	return (void *)(TO_CAC(x));
}

#endif
