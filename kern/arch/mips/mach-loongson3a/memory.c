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

addr_t get_mem_size(void)
{
	/* TODO: fill in... */
	return 0;
}

void mips_add_memory_pages(void)
{
#ifdef LOONGSON3A_RAM_DETECTION
	int j;
	uint64_t base, mask, mmap, rambase = 0, ramsize = 0;

	/*
	 * This feature is really experimental as it assumes that
	 * the address space mappings are left as default on
	 * Loongson 3A user manual.  Specifically:
	 * 1. Address 0x18000000 - 0x1bffffff -> HT1 LOW I/O
	 * 2. Address 0x1e000000 - 0x1effffff -> HT1 LOW MEM
	 * 3. Base address 0x0c0000000000 -> HT0
	 * 4. Base address 0x0e0000000000 -> HT1
	 * 5. Base address 0x?00000000000 -> NUMA space
	 *
	 * Excluding disabled mappings, usually there is only one
	 * enabled mapping left (with base address denoted B), which
	 * corresponds to the entire RAM.
	 *
	 * Note that address 0 - 256M and address B - (B + 256M)
	 * are mapped to the same RAM region.
	 */
	for (j = 0; j < LOONGSON3A_CORE_WINS; ++j) {
		base = LOONGSON3A_COREx_WINy_BASE(0, j);
		mask = LOONGSON3A_COREx_WINy_MASK(0, j);
		mmap = LOONGSON3A_COREx_WINy_MMAP(0, j);

		/* filter out disabled spaces */
		if (mmap & LOONGSON3A_MMAP_AVAILABLE)
			continue;
		/* filter out NUMA address spaces */
		if (mmap & LOONGSON3A_NUMA_MASK)
			continue;
		/* filter out (3) and (4) */
		if (base == LOONGSON3A_HT0_BASE ||
		    base == LOONGSON3A_HT1_BASE)
			continue;
		/* filter out (1) and (2) */
		if (base == LOONGSON3A_HTIO32 ||
		    base == LOONGSON3A_HTMEM32)
			continue;

		rambase = base;
		ramsize = ~mask + 1;
		break;
	}

	if (rambase == 0)
		for (;;) ;	/* panic */

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
