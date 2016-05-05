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

/*
 * MIPS does not have a page index walker like ARM or i386, so we have
 * the freedom to design our own page table.
 *
 * Here, the page table is a standard 2-level hierarchical table.
 *
 * The entries for all directories except leaf page tables are kernel virtual
 * addresses pointing to next-level directories:
 * 33222222222211111111110000000000
 * 10987654321098765432109876543210
 * +--4K-aligned virtual address--+
 *
 * While the leaf page tables stores PFN and additional information in the
 * following format:
 * 33222222222211111111 110 0 0 0 000000
 * 10987654321098765432 109 8 7 6 543210
 * +-------PFN--------+ CCF D V G 00X00P
 *
 * The two types of entries could be distinguished by checking the P bit.
 */

#include <pmm.h>
#include <vmm.h>
#include <mmu.h>
#include <pgtable.h>
#include <libc/string.h>
#include <errno.h>
#include <sys/types.h>
#include <util.h>

int init_pgindex(pgindex_t *pgindex)
{
	struct pages p;
	int retcode = 0;
       
	p.size = PAGE_SIZE;
	p.flags = 0;
	p.paddr = 0;

	if (alloc_pages(&p) < 0)
		return -ENOMEM;

	memset((void *)pa2kva(p.paddr), 0, PAGE_SIZE);

	*pgindex = (pgindex_t)pa2kva(p.paddr);
	return 0;
}

/* Map a single page with size @size from virtual address @vaddr to physical
 * address @paddr */
int map_page(pgindex_t *pgindex, void *vaddr, addr_t paddr, size_t size,
    uint32_t flags);

static void __unmap_page(pgindex_t *pgindex, void *vaddr)
{
	pde_t *pde = (pde_t *)pgindex;
	pte_t *pte = (pte_t *)pde[PDX(vaddr)];
	addr_t paddr = PTE_PADDR(pte[PTX(vaddr)]);
	pgfree(paddr);
}

int unmap_pages(pgindex_t *pgindex, void *vaddr, size_t size)
{
	pte_t *pte;
	pde_t *pde = (pde_t *)pgindex;

	if (!PTR_IS_ALIGNED(vaddr, PAGE_SIZE) ||
	    !IS_ALIGNED(size, PAGE_SIZE) ||
	    (size == 0))	/* TODO: size == 0 */
		return -EINVAL;

	void *vend = (void *)((size_t)vaddr + size - PAGE_SIZE);
	int pdx_start = PDX(vaddr);
	int pdx_end = PDX(vend);
	struct pages p;

	/* Unmap the pages */
	for (; vaddr <= vend; vaddr += PAGE_SIZE)
		__unmap_page(pgindex, vaddr);

	/* Clean up the page sub-tables */
	/* Check if we need to clear the starting leaf page table first */
check_start:
	pte = pde[pdx_start];
	for (int ptx = 0; ptx < NR_PTENTRIES; ++ptx) {
		if (pte[ptx] & PTE_VALID)
			goto clean_middle;
	}
	pgfree(kva2pa(pte));

clean_middle:
	/* leaf page tables between pde[pdx_start] and pde[pdx_end] should
	 * be always empty. */
	for (int pdx = pdx_start + 1; pdx <= pdx_end - 1; ++pdx)
		pgfree(kva2pa((pte_t *)pde[pdx]));

check_end:
	pte = pde[pdx_end];
	for (int ptx = 0; ptx < NR_PTENTRIES; ++ptx) {
		if (pte[ptx] & PTE_VALID)
			goto finish;
	}
	pgfree(kva2pa(pte));

finish:
	return 0;
}

