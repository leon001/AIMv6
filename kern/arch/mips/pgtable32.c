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
 * +-4K-aligned physical address--+
 *
 * While the leaf page tables stores PFN and additional information in the
 * following format:
 * 33222222222211111111 110 0 0 0 000000
 * 10987654321098765432 109 8 7 6 543210
 * +-------PFN--------+ CCF D V G 00X000
 */

#include <pmm.h>
#include <vmm.h>
#include <mmu.h>
#include <pgtable.h>
#include <libc/string.h>
#include <errno.h>
#include <sys/types.h>
#include <util.h>

struct pagedesc {
	pde_t	*pde;
	pte_t	*pte;
	int	pdx;
	int	ptx;
};

/*
 * Get a page index descriptor (PDE, PTE in our case) for the page table,
 * possibly creating leaf page tables if needed.
 */
static int 
__pgindex_getdesc(pgindex_t *pgindex,
			     void *addr,
			     bool create,
			     struct pagedesc *pd)
{
	pd->pde = (pde_t *)*pgindex;
	pd->pdx = PDX(addr);
	if (pd->pde[pd->pdx] != 0) {
		/* We already have the intermediate directory */
		pd->pte = (pte_t *)pd->pde[pd->pdx];
	} else {
		/* We don't have it, fail or create one */
		if (!create) {
			return -ENOENT;
		}
		pd->pte = (pte_t *)pgalloc();
		if (pd->pte == -1)
			return -ENOMEM;
		pd->pde[pd->pdx] = pd->pte;
	}
	pd->ptx = PTX(addr);
	return 0;
}

int
init_pgindex(pgindex_t *pgindex)
{
	int retcode = 0;
	addr_t paddr = pgalloc();

	if (addr_t == -1)
		return -ENOMEM;

	memset((void *)pa2kva(paddr), 0, PAGE_SIZE);

	*pgindex = (pgindex_t)pa2kva(paddr);
	return 0;
}

void
destroy_pgindex(pgindex_t *pgindex)
{
	pgfree(*pgindex);
	*pgindex = NULL;
}

static void
__unmap_and_free_page(pgindex_t *pgindex, void *vaddr)
{
	struct pagedesc pd;
	if (__pgindex_getdesc(pgindex, vaddr, false, &pd) < 0)
		panic("__unmap_page fail: %p %p\n", *pgindex, vaddr);
	addr_t paddr = PTE_PADDR(pd->pte[pd->ptx]);
	pgfree(paddr);
	pd->pte[pd->ptx] = 0;
}

/* This function cleans intermediate page tables by itself, leaving actual
 * page unmapping to __unmap_page() */
int
unmap_and_free_pages(pgindex_t *pgindex, void *vaddr, size_t size)
{
	pte_t *pte;
	pde_t *pde = (pde_t *)*pgindex;

	if (!PTR_IS_ALIGNED(vaddr, PAGE_SIZE) ||
	    !IS_ALIGNED(size, PAGE_SIZE) ||
	    (size == 0))	/* TODO: size == 0 */
		return -EINVAL;

	void *vend = (void *)((size_t)vaddr + size - PAGE_SIZE);
	int pdx_start = PDX(vaddr);
	int pdx_end = PDX(vend);

	/* Unmap the pages */
	for (; vaddr <= vend; vaddr += PAGE_SIZE)
		__unmap_and_free_page(pgindex, vaddr);

	/* Clean up the page sub-tables */
	/* Check if we need to clear the starting leaf page table first */
check_start:
	pte = pa2kva(pde[pdx_start]);
	for (int ptx = 0; ptx < NR_PTENTRIES; ++ptx) {
		if (pte[ptx] != 0)
			goto clean_middle;
	}
	pgfree(kva2pa(pte));
	pde[pdx_start] = 0;

clean_middle:
	/* leaf page tables between pde[pdx_start] and pde[pdx_end] should
	 * be always empty. */
	for (int pdx = pdx_start + 1; pdx <= pdx_end - 1; ++pdx) {
		pgfree((pte_t *)pde[pdx]);
		pde[pdx] = 0;
	}

check_end:
	pte = pa2kva(pde[pdx_end]);
	for (int ptx = 0; ptx < NR_PTENTRIES; ++ptx) {
		if (pte[ptx] & PTE_VALID)
			goto finish;
	}
	pgfree(kva2pa(pte));
	pde[pdx_end] = 0;

finish:
	return 0;
}

static uint32_t
page_perm(uint32_t vma_flags)
{
	uint32_t perm = PTE_VALID | PTE_CACHEABLE;
	if (!(vma_flags & VMA_EXEC))
		perm |= PTE_NOEXEC;
	if (vma_flags & VMA_WRITE)
		perm |= PTE_DIRTY;
}

static int
__map_page(pgindex_t *pgindex, void *vaddr, addr_t paddr, uint32_t perm)
{
	struct pagedesc pd;
	if (__pgindex_getdesc(pgindex, vaddr, true, &pd) < 0)
		panic("__map_page fail: %p %p\n", *pgindex, vaddr);
}

