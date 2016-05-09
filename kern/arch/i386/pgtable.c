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

#include <mm.h>
#include <pmm.h>
#include <vmm.h>
#include <mmu.h>
#include <libc/string.h>
#include <errno.h>
#include <sys/types.h>
#include <util.h>
#include <panic.h>

struct pagedesc {
	pde_t	*pdep;
	pte_t	*ptep;
	int	pdx;
	int	ptx;
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
	addr_t paddr;
	pde_t *pde = (pde_t *)pgindex;
	pd->pdep = pde;
	pd->pdx = PDX(addr);
	if (pde[pd->pdx] & PTE_P) {
		/* We already have the intermediate directory */
		pd->ptep = (pte_t *)pa2kva(PTE_PADDR(pde[pd->pdx]));
	} else {
		/* We don't have it, fail or create one */
		if (!create) {
			return -ENOENT;
		}
		if ((paddr = pgalloc()) == -1)
			return -ENOMEM;
		pd->ptep = (pte_t *)pa2kva(paddr);
		memset((void *)pd->ptep, 0, PAGE_SIZE);
		pde[pd->pdx] = mkpte(paddr, PTE_P | PTE_R | PTE_U);
	}
	pd->ptx = PTX(addr);
	return 0;
}

/*
 * This function assumes that:
 * 1. (Leaf) page table entries for vaddr..vaddr+size are already zero.
 * 2. @vaddr and @size are page-aligned.
 */
static void
__free_intermediate_pgtable(pgindex_t *pgindex, void *vaddr, size_t size)
{
	pde_t *pde = (pde_t *)pgindex;
	int pdx = PDX(vaddr), pdx_end = PDX(vaddr + size - PAGE_SIZE);
	for (; pdx <= pdx_end; ++pdx) {
		pte_t *pte = (pte_t *)pa2kva(PTE_PADDR(pde[pdx]));
		for (int i = 0; i < NR_PTENTRIES; ++i) {
			if (pte[i] & PTE_P)
				goto rollback_next_pde;
		}
		pgfree(kva2pa(PTE_PADDR(pde[pdx])));
		pde[pdx] &= ~PTE_P;
rollback_next_pde:
		/* nothing */;
	}
}

pgindex_t *
init_pgindex(void)
{
	addr_t paddr = pgalloc();
	if (paddr == -1)
		return NULL;

	memset(pa2kva(paddr), 0, PAGE_SIZE);

	return pa2kva(paddr);
}

void
destroy_pgindex(pgindex_t *pgindex)
{
	pgfree(kva2pa(pgindex));
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
	uint32_t flags = PTE_P | PTE_U |
		((vma_flags & VMA_WRITE) ? PTE_R : 0);
	return flags;
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
	addr_t pend = paddr + size;
	addr_t pcur = paddr;
	void *vcur = vaddr;

	if (!IS_ALIGNED(paddr, PAGE_SIZE) ||
	    !IS_ALIGNED(size, PAGE_SIZE) ||
	    !PTR_IS_ALIGNED(vaddr, PAGE_SIZE))
		return -EINVAL;

	/*
	 * 1st pass: allocate leaf page tables if needed, and validate
	 * if there are any conflicts or memory shortage, in either case
	 * we need to rollback.
	 */
	for (; pcur < pend; pcur += PAGE_SIZE, vcur += PAGE_SIZE) {
		retcode = __getpagedesc(pgindex, vcur, true, &pd);
		if (retcode == -ENOMEM)
			goto rollback;

		if (pd.ptep[pd.ptx] & PTE_P) {
			/* we are mapping on the exact same virtual
			 * page which is either valid or invalid (paged
			 * out), fail */
			retcode = -EEXIST;
			goto rollback;
		}
	}

	/* 2nd pass: fill the entries as there shouldn't be any failure */
	pcur = paddr;
	vcur = vaddr;
	for (; pcur < pend; pcur += PAGE_SIZE, vcur += PAGE_SIZE) {
		__getpagedesc(pgindex, vcur, false, &pd);
		pd.ptep[pd.ptx] = pcur | __pgtable_perm(flags);
	}

	return 0;

rollback:
	__free_intermediate_pgtable(pgindex, vaddr, vcur - vaddr);
	return retcode;
}

ssize_t
unmap_pages(pgindex_t *pgindex,
	    void *vaddr,
	    size_t size,
	    addr_t *paddr)
{
	void *vcur = vaddr, *vend = vaddr + size;
	ssize_t unmapped_bytes = 0;
	struct pagedesc pd = {0};	/* suppresses warning */
	addr_t pcur = 0;

	for (; vcur < vend; vcur += PAGE_SIZE,
			    unmapped_bytes += PAGE_SIZE,
			    pcur += PAGE_SIZE) {
		if (__getpagedesc(pgindex, vcur, false, &pd) < 0)
			/* may return -ENOENT? */
			panic("unmap_pages non-existent: %p %p\n",
			    pgindex, vcur);
		if (unmapped_bytes == 0) {
			/* unmapping the first page: store the physical
			 * address */
			pcur = PTE_PADDR(pd.ptep[pd.ptx]);
			if (paddr != NULL)
				*paddr = pcur;
		} else if (pd.ptep[pd.ptx] != pcur) {
			break;
		}
		pd.ptep[pd.ptx] = 0;
	}

	__free_intermediate_pgtable(pgindex, vaddr, unmapped_bytes);

	return unmapped_bytes;
}

