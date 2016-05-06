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
 * The leaf page tables stores PFN and additional information in the
 * following format:
 * 33222222222211111111 110 0 0 0 000000
 * 10987654321098765432 109 8 7 6 543210
 * +-------PFN--------+ CCF D V G 00X000
 *
 * pgindex_t is the type of *physical* address to the page table structure.
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
	uint32_t	pdep;	/* page directory physical address */
	uint32_t	ptep;	/* leaf page table physical address */
	int		pdx;
	int		ptx;
};

/*
 * Get a page index descriptor (PDE, PTE in our case) for the page table,
 * possibly creating leaf page tables if needed.
 */
static int 
__getpagedesc(pgindex_t *pgindex,
	      void *addr,
	      bool create,
	      struct pagedesc *pd)
{
	pde_t *pde = (pde_t *)pa2kva(*pgindex);
	pd->pdep = *pgindex;
	pd->pdx = PDX(addr);
	if (pde[pd->pdx] != 0) {
		/* We already have the intermediate directory */
		pd->ptep = pde[pd->pdx];
	} else {
		/* We don't have it, fail or create one */
		if (!create) {
			return -ENOENT;
		}
		pd->ptep = (uint32_t)pgalloc();
		if (pd->ptep == -1)
			return -ENOMEM;
		pde[pd->pdx] = pd->ptep;
	}
	pd->ptx = PTX(addr);
	return 0;
}

int
init_pgindex(pgindex_t *pgindex)
{
	addr_t paddr = pgalloc();
	if (paddr == -1)
		return -ENOMEM;

	*pgindex = paddr;
	memset(pa2kva(*pgindex), 0, PAGE_SIZE);

	return 0;
}

void
destroy_pgindex(pgindex_t *pgindex)
{
	pgfree(*pgindex);
}

/* TODO: put this into a header */
extern uint32_t __mach_pgtable_perm(uint32_t vma_flags);

static uint32_t
__pgtable_perm(uint32_t vma_flags)
{
	/*
	 * Some machines (e.g. Loongson 2F) may specify additional flags
	 * (e.g. NOEXEC) to supplement the flags defined by MIPS
	 * standard.
	 *
	 * We will have to rely on machine-specific internal function, so
	 * I prepended the function with double underscores to indicate
	 * that this function should not be called elsewhere.
	 */
	uint32_t flags = PTE_VALID | PTE_GLOBAL | PTE_CACHEABLE |
		((vma_flags & VMA_WRITE) ? PTE_DIRTY : 0) |
		__mach_pgtable_perm(vma_flags);
}

int
map_pages(pgindex_t *pgindex,
	  void *vaddr,
	  addr_t paddr,
	  size_t size,
	  uint32_t flags)
{
	struct pagedesc pd;
	int retcode;
	pte_t *pte;
	pde_t *pde;
	addr_t pend = paddr + size;
	addr_t pcur = paddr;
	void *vcur = vaddr;

	if (!IS_ALIGNED(paddr, PAGE_SIZE) ||
	    !IS_ALIGNED(size, PAGE_SIZE) ||
	    !PTR_IS_ALIGNED(vaddr, PAGE_SIZE))
		return -EINVAL;

	for (; pcur < pend; pcur += PAGE_SIZE, vcur += PAGE_SIZE) {
		retcode = __getpagedesc(pgindex, vcur, true, &pd);
		if (retcode == -ENOMEM)
			goto rollback;

		pte = (pte_t *)pa2kva(pd.ptep);
		if (pte[pd.ptx] != 0) {
			/* we are mapping on the exact same virtual
			 * page which is either valid or invalid (paged
			 * out), fail */
			retcode = -EEXIST;
			goto rollback;
		}
		pte[pd.ptx] = pcur | __pgtable_perm(flags);
	}

	return 0;

rollback:
	/* Clean the mapped entries first */
	for (; paddr < pcur; paddr += PAGE_SIZE, vaddr += PAGE_SIZE) {
		__getpagedesc(pgindex, vaddr, false, &pd);
		pte = (pte_t *)pa2kva(pd.ptep);
		pte[pd.ptx] = 0;
	}
	/* TODO: free up the leaf page table pages we've just created */
	return retcode;
}

