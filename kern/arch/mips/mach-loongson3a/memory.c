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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pmm.h>
#include <platform.h>
#include <mm.h>
#include <util.h>
#include <bitops.h>

addr_t get_mem_size(void)
{
	/* TODO: fill in... */
	return 0;
}

void mips_add_memory_pages(void)
{
#ifdef LOONGSON3A_RAM_DETECTION
	int j;
	uint64_t base, mask, mmap, rambase = (uint64_t)-1, ramsize = 0;

	/*
	 * This feature is really experimental as it assumes that
	 * the address space mappings are left as default on
	 * Loongson 3A user manual.
	 *
	 * The description of the two-level address switches for
	 * Loongson 3A is rather scattered.  In general, we are
	 * interested in Section 2.5 and Chapter 14 if we are going
	 * to probe RAM on the fly.  Specifically, we need to peek
	 * the mapping configuration of the *secondary* address
	 * switches, whose register addresses are denoted as
	 * LOONGSON3A_CPU_WINx_XXXXADDR.
	 *
	 * We would NEVER change the values there in kernel.
	 *
	 * Loongson Tech. by default arranges the mappings in a
	 * rather counterintuitive manner to ensure DDR efficiency,
	 * while maintaining the standard convention of physical
	 * addresses on MIPS.
	 *
	 * Here's the default configuration on my machine.  We
	 * can infer from Section 2.5 that the high base address of
	 * RAM is 0x80000000, and the size of RAM is 4GB.
	 * BASE             MASK             MMAP
	 * 000000001fc00000 fffffffffff00000 000000001fc000f2
	 * 0000000010000000 fffffffff0000000 0000000010000082
	 * 0000000000000000 fffffffff0100000 00000000000000f0
	 * 0000000000100000 fffffffff0100000 00000000000000f1
	 * 0000000080000000 ffffffff80100000 00000000000000f0
	 * 0000000080100000 ffffffff80100000 00000000000000f1
	 * 0000000100000000 ffffffff80100000 00000000001000f0
	 * 0000000100100000 ffffffff80100000 00000000001000f1
	 *
	 * This solution assumes that for high RAM address mappings,
	 * the mask values are the same while the effective base
	 * addresses are contiguous.
	 *
	 * I can say that the following code can probe the RAM size
	 * and RAM base correctly in most cases, as long as neither you
	 * nor Loongson made funny configurations.  If you fail,
	 * please try the static mapping after finding out RAM
	 * base and size, or contact me for help.
	 *
	 * Of course, a better solution is always welcome...
	 */
	/*******MAGIC STARTS HERE********/
	for (j = 0; j < LOONGSON3A_CORE_WINS; ++j) {
		base = read64(LOONGSON3A_CPU_WINx_BASEADDR(j));
		mask = read64(LOONGSON3A_CPU_WINx_MASKADDR(j));
		mmap = read64(LOONGSON3A_CPU_WINx_MMAPADDR(j));
		kprintf("CPU address mappings: %p %p %p\n", base, mask, mmap);

		if ((base & 0xfffffffff0000000) == 0x10000000)
			/* MIPS convention: devices, not RAM, skip */
			continue;
		if ((base & 0xfffffffff0000000) == 0x00000000)
			/* MIPS convention: Low RAM, trivial, skip */
			continue;

		rambase = min2(rambase, base);
		ramsize += 1 << count_zero_bits(mask);
	}
	/********MAGIC ENDS HERE*********/

	kprintf("RAM base: %p\n", rambase);
	kprintf("RAM size: %p\n", ramsize);

	struct pages *p = kmalloc(sizeof(*p), 0);
	extern uint8_t _kern_end;
	size_t kern_end = (size_t)&_kern_end;
	uint64_t reserved_space = kva2pa(ALIGN_ABOVE(kern_end, PAGE_SIZE));
	/* filter out spaces lower than _kern_end */
	p->paddr = rambase + reserved_space;
	p->size = ramsize - reserved_space;
	p->flags = 0;

	free_pages(p);
#else	/* LOONGSON3A_RAM_DETECTION == no */
#endif	/* LOONGSON3A_RAM_DETECTION */
}
