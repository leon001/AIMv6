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
#include <mmu.h>
#include <atomic.h>

struct mm *
mm_new(void)
{
	struct mm *mm = (struct mm *)kmalloc(sizeof(*mm), 0);

	if (mm != NULL) {
		list_init(&(mm->vma_head));
		mm->vma_count = 0;
		if (init_pgindex(&(mm->pgindex)) < 0) {
			kfree(mm);
			return NULL;
		}
	}

	return mm;
}

/* Unmap the virtual memory area */
static void
__clean_vma(struct mm *mm, struct vma *vma)
{
	/* all assertations here are temporary */
	ssize_t unmapped;
	addr_t pa;

	assert(vma->size == vma->pages->size);

	unmapped = unmap_pages(&(mm->pgindex), vma->start, vma->size, &pa);

	assert(pa == vma->pages->paddr);
	assert(unmapped == vma->size);
}

static void
__ref_pages(struct pages *p)
{
	atomic_inc(&(p->refs));
}

#define __PAGES_FREED	1
static int
__unref_and_free_pages(struct pages *p)
{
	atomic_dec(&(p->refs));
	if (p->refs == 0) {
		free_pages(p);
		return __PAGES_FREED;
	}
	return 0;
	
}

void
mm_destroy(struct mm *mm)
{
	struct vma *vma, *vma_next;

	if (mm == NULL)
		return;

	for_each_entry_safe (vma, vma_next, &(mm->vma_head), node) {
		__clean_vma(mm, vma);
		if (__unref_and_free_pages(vma->pages) == __PAGES_FREED)
			kfree(vma->pages);
		kfree(vma);
	}

	destroy_pgindex(&(mm->pgindex));

	kfree(mm);
}

static int
__insert_vma(struct mm *mm, struct vma *new_vma)
{
	struct vma *vma, *vma_prev;

	if (list_empty(&(mm->vma_head))) {
		list_add_before(&(new_vma->node), &(mm->vma_head));
		return 0;
	}
	/* otherwise */
	for_each_entry (vma, &(mm->vma_head), node) {
		if (vma->start >= new_vma->start + new_vma->size)
			break;
	}

	vma_prev = prev_entry(vma, node);
	if (vma->start < vma_prev->start + vma->size)
		/* overlap detected */
		return -EFAULT;

	list_add_before(&(new_vma->node), &(vma->node));
	return 0;
}

int
create_uvm(struct mm *mm, void *addr, size_t len, uint32_t flags)
{
	int retcode = 0;
	struct vma *vma = NULL;
	struct pages *p = NULL;

	vma = (struct vma *)kmalloc(sizeof(*vma), 0);
	p = (struct pages *)kmalloc(sizeof(*p), 0);
	if ((vma == NULL) || (p == NULL)) {
		retcode = -ENOMEM;
		goto rollback_kmalloc;
	}

	if ((retcode = __insert_vma(mm, vma)) < 0)
		goto rollback_kmalloc;

	p->paddr = 0;
	p->size = len;
	p->flags = 0;
	p->refs = 0;
	if (alloc_pages(p) < 0) {
		retcode = -ENOMEM;
		goto rollback_insert;
	}

	retcode = map_pages(&(mm->pgindex), addr, p->paddr, len, flags);
	if (retcode < 0)
		goto rollback_pgalloc;

	__ref_pages(p);

	return 0;

rollback_pgalloc:
	free_pages(p);
rollback_insert:
	list_del(&(vma->node));
rollback_kmalloc:
	kfree(vma);
	kfree(p);

	return retcode;
}

int
destroy_uvm(struct mm *mm, void *addr, size_t len)
{
	/*
	 * TODO: how should we deal with the following scenario:
	 * 1. Process A create a virtual memory mapping VA..VA+200P
	 *    and maps it to PA..PA+200P via a sbrk(2) call.
	 * 2. Process A shrinks the heap to 5 pages via another sbrk(2).
	 * Ideally, we want to free the 195 pages shrunk out.
	 * Things may get more complicated if copy-on-write is involved
	 * (e.g. Process B inherits the virtual mapping via fork(2)), in
	 * which case the page references are different between parts.
	 */
}

