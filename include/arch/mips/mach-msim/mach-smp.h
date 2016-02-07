/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*
 * IMPORTANT: rename this file to "smp.h" after moving to the desired directory.
 */
#ifndef _MACH_SMP_H
#define _MACH_SMP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <io.h>
#include <addrspace.h>

#define MSIM_ORDER_REG_CPUID	0x0
#define MSIM_ORDER_REG_IPI	0x4
#define MSIM_ORDER_MAILBOX_SIZE	(1 << MSIM_ORDER_MAILBOX_ORDER)

/*
 * Each mach/smp.h should implement its own cpuid() and cpuid assembler macro
 * if the machine doesn't support EBASE coprocessor register.
 */

#ifndef __ASSEMBLER__

/*
 * We don't need to implement this under a driver framework even if the core
 * ID# is encoded inside a separate device, because
 * (1) The core ID# will be read quite frequently, and
 * (2) The mechanism on each machine may be different anyway.
 * (3) We already knew that Revision 2 or higher encodes the core ID# in
 *     EBASE register.
 */

static inline unsigned int __cpuid(void)
{
	return read32(MSIM_ORDER_PHYSADDR + MSIM_ORDER_REG_CPUID);
}

static inline unsigned long msim_mailbox(unsigned int cpuid)
{
	return MSIM_ORDER_MAILBOX_BASE + cpuid * MSIM_ORDER_MAILBOX_SIZE;
}

#define cpuid()			__cpuid()

#else	/* !__ASSEMBLER__ */

	.macro	cpuid result temp
	LI	\result, TO_UNCAC(MSIM_ORDER_PHYSADDR)
	lw	\result, MSIM_ORDER_REG_CPUID(\result)
	.endm

#endif	/* __ASSEMBLER__ */

#endif
