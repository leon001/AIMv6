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

#include <mmu.h>
#include <util.h>
#include <pmm.h>
#include <vmm.h>

addr_t get_mem_size(void)
{
	/* TODO: handle situations with <256M RAM and make it consistent
	 * with HIGHRAM_SIZE */
	return MEM_SIZE;
}

void mips_add_memory_pages(void)
{
	/* Low RAM */
	extern uint8_t _kern_end;
	uint32_t kern_end = (uint32_t)&_kern_end;
	struct pages *p = kmalloc(sizeof(*p), 0);
	p->paddr = kva2pa(ALIGN_ABOVE(kern_end, PAGE_SIZE));
	p->size = LOWRAM_TOP - p->paddr;	/* TODO: no magic number */
	p->flags = 0;

	free_pages(p);

	/* High RAM */
#if HIGHRAM_SIZE != 0
	p = kmalloc(sizeof(*p), 0);
	p->paddr = HIGHRAM_BASE;
	p->size = HIGHRAM_SIZE;
	p->flags = 0;

	free_pages(p);
#endif
}

int get_tlb_entries(void)
{
	return 48;	/* MSIM preset */
}
