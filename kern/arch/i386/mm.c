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
#endif /* HAVE_CONFIG_H */

/* from kernel */
#include <sys/types.h>
#include <asm.h>
#include <mm.h>
#include <processor-flags.h>
#include <util.h>
#include <libc/string.h>

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 */
int get_addr_space(void)
{
	return (get_pc() > KERN_BASE);
}

void page_index_clear(pgindex_t *index)
{
	memset(index, 0, PAGE_SIZE);
}

/*
 */
int page_index_early_map(pgindex_t *index,
			 addr_t paddr,
			 size_t vaddr,
			 size_t length)
{
	xpte_t *xpte = (xpte_t *)index;
	if (!(IS_ALIGNED(paddr, XPAGE_SIZE) &&
	      IS_ALIGNED(vaddr, XPAGE_SIZE) &&
	      IS_ALIGNED(length, XPAGE_SIZE)))
		return -1;

	for (size_t va = vaddr;
	     va < vaddr + length;
	     va += XPAGE_SIZE, paddr += XPAGE_SIZE) {
		xpte[XPTX(va)] = mkxpte(paddr, PTE_P | PTE_WRITABLE | PTE_S);
	}

	return 0;
}

int mmu_init(pgindex_t *index)
{
	uint32_t reg;
	asm volatile (
		"movl	%%cr4, %[reg];"
		"orl	%[cr4_flags], %[reg];"
		"movl	%[reg], %%cr4;"
		"movl	%[index], %[reg];"
		"movl	%[reg], %%cr3;"
		"movl	%%cr0, %[reg];"
		"orl	%[cr0_flags], %[reg];"
		"movl	%[reg], %%cr0;"
		: [reg]		"+r"(reg)
		: [cr4_flags]	"i" (CR4_PSE),
		  [index]	"r" (index),
		  [cr0_flags]	"i" (CR0_PG | CR0_WP)
		: "memory"
	);
	return 0;
}
