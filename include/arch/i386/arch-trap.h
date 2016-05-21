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

#ifndef _ARCH_TRAP_H
#define _ARCH_TRAP_H

struct idtentry {
	unsigned int off_lo:16;
	unsigned int cs:16;
	unsigned int args:5;
	unsigned int rsv1:3;
	unsigned int type:4;
	unsigned int s:1;
	unsigned int dpl:2;
	unsigned int p:1;
	unsigned int off_hi:16;
};

#include <sys/types.h>

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

/* Processor-defined */
#define T_DIVIDE	0	// divide error
#define T_DEBUG		1	// debug exception
#define T_NMI		2	// non-maskable interrupt
#define T_BRKPT		3	// breakpoint
#define T_OFLOW		4	// overflow
#define T_BOUND		5	// bounds check
#define T_ILLOP		6	// illegal opcode
#define T_DEVICE	7	// device not available
#define T_DBLFLT	8	// double fault
// #define T_COPROC	9	// reserved (not used since 486)
#define T_TSS		10	// invalid task switch segment
#define T_SEGNP		11	// segment not present
#define T_STACK		12	// stack exception
#define T_GPFLT		13	// general protection fault
#define T_PGFLT		14	// page fault
// #define T_RES	15	// reserved
#define T_FPERR		16	// floating point error
#define T_ALIGN		17	// aligment check
#define T_MCHK		18	// machine check
#define T_SIMDERR	19	// SIMD floating point error

#define T_MSG_MAX	T_SIMDERR

/* User defined */
#define T_SYSCALL	64	// system call
#define T_DEFAULT	500	// catchall

#define T_IRQ0		32	// IRQ 0 corresponds to int T_IRQ

#define IRQ_TIMER	0
#define IRQ_KBD		1
#define IRQ_COM1	4
#define IRQ_IDE		14
#define IRQ_ERROR	19
#define IRQ_SPURIOUS	31

#endif
