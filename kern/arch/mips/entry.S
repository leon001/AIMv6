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

#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>

/*
 * IMPORTANT NOTE:
 * Unless explicitly stated (either by filename, assembler directives,
 * or comments), all code inside kern/arch/mips directory should work
 * on both MIPS32 and MIPS64 architectures.
 */

/*
 * Here is the entry of kernel, as stated by vmaim.lds.S.
 *
 * Usually on a multiprocessor machine, only one processor (we call it
 * the master processor) is brought in to the kernel entry, while
 * other processors (called slave processors) are suspended by firmware
 * and wait for some sort of signal from the master.
 */
BEGIN(__start)
	/* Ensure that we are inside kernel mode. */
	MFC032	a0, CP0_STATUS
	or	a0, ST_EXCM
	xor	a0, ST_EXCM
	MTC032	a0, CP0_STATUS
	/*
	 * The kernel is already inside RAM, so .text section, .rodata
	 * section and .data section are in their right position with
	 * correct values.
	 * However, .bss section is still not guaranteed to be initialized
	 * to 0, so we are going to clean it first.
	 */
init_bss:
	LI	s1, __bss_begin__
	LI	s2, __bss_end__
1:	bge	s1, s2, 9f
	sw	zero, (s1)
	ADDU	s1, 4
	b	1b
9:
init_master_stack:
	/* boot_stack points to stack top.  Defined in vmaim.lds.S */
	LA	sp, boot_stack

main_entry:
	/*
	 * We know that we won't return from master_early_init(), so
	 * we don't need to save anything prior to calling the function.
	 */
	jal	master_early_init

die:	j	die
END(__start)
