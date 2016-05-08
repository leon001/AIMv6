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

#ifndef _ARCH_REGS_H
#define _ARCH_REGS_H

#include <sys/types.h>

struct regs {
	/*
	 * We are not required to save all of them because some of them
	 * are caller-saved.  However we save all of them regardless just
	 * for convenience.
	 */
	uint32_t eax;
	uint32_t edx;
	uint32_t ecx;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eflags;
	uint32_t eip;
};

struct trapframe {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t oesp;      /* ignored */
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	uint16_t gs;
	uint16_t padding1;
	uint16_t fs;
	uint16_t padding2;
	uint16_t es;
	uint16_t padding3;
	uint16_t ds;
	uint16_t padding4;
	uint32_t trapno;

	/* below are all hardwire defined */
	uint32_t err;
	uint32_t eip;
	uint16_t cs;
	uint16_t padding5;
	uint32_t eflags;

	/* switching rings */
	uint32_t esp;
	uint16_t ss;
	uint16_t padding6;
};

#endif
