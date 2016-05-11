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

#include <console.h>	/* to be removed */
#include <mm.h>
#include <mmu.h>
#include <atomic.h>
#include <errno.h>
#include <panic.h>

struct mm *
mm_new(void)
{
	struct mm *mm = (struct mm *)kmalloc(sizeof(*mm), 0);

	if (mm != NULL) {
		list_init(&(mm->vma_head));
		mm->vma_count = 0;
		if ((mm->pgindex = init_pgindex()) == NULL) {
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

	unmapped = unmap_pages(mm->pgindex, vma->start, vma->size, &pa);

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

	destroy_pgindex(mm->pgindex);

	kfree(mm);
}

static struct vma *
__find_vma_before(struct mm *mm, void *addr, size_t size)
{
	struct vma *vma, *vma_prev;

	if (list_empty(&(mm->vma_head)))
		return list_entry(&(mm->vma_head), struct vma, node);

	/* otherwise */
	for_each_entry (vma, &(mm->vma_head), node) {
		if (vma->start >= addr + size)
			break;
	}

	vma_prev = prev_entry(vma, node);
	if (addr < vma_prev->start + vma->size)
		/* overlap detected */
		return NULL;

	return vma_prev;
}

static void
__unmap_and_free_vma(struct mm *mm, struct vma *vma_start, size_t size)
{
	struct vma *vma_cur = vma_start;
	for (size_t i = 0; i < size; i += PAGE_SIZE) {
		struct vma *vma = vma_cur;
		vma_cur = next_entry(vma_cur, node);

		list_del(&(vma->node));
		/* temporary in case of typo - assertation will be removed */
		assert(unmap_pages(mm->pgindex, vma->start, vma->size,
		    NULL) == PAGE_SIZE);
		if (__unref_and_free_pages(vma->pages) == __PAGES_FREED)
			kfree(vma->pages);
		kfree(vma);
	}
}

int
create_uvm(struct mm *mm, void *addr, size_t len, uint32_t flags)
{
	int retcode = 0;
	struct vma *vma_start, *vma, *vma_cur;
	struct pages *p;
	void *vcur = addr;
	size_t mapped = 0;

	if (!IS_ALIGNED(len, PAGE_SIZE) ||
	    mm == NULL ||
	    !PTR_IS_ALIGNED(addr, PAGE_SIZE))
		return -EINVAL;

	if (!(vma_start = __find_vma_before(mm, addr, len)))
		return -EFAULT;

	vma_cur = vma_start;
	for (; mapped < len; mapped += PAGE_SIZE, vcur += PAGE_SIZE) {
		vma = (struct vma *)kmalloc(sizeof(*vma), 0);
		if (vma == NULL) {
			retcode = -ENOMEM;
			goto rollback;
		}
		vma->start = vcur;
		vma->size = PAGE_SIZE;
		vma->flags = flags;

		p = (struct pages *)kmalloc(sizeof(*p), 0);
		if (p == NULL) {
			retcode = -ENOMEM;
			goto rollback_vma;
		}
		p->paddr = 0;
		p->flags = 0;
		p->size = PAGE_SIZE;
		p->refs = 0;
		if (alloc_pages(p) < 0) {
			retcode = -ENOMEM;
			goto rollback_pages;
		}

		if ((retcode = map_pages(mm->pgindex, vcur, p->paddr,
		    PAGE_SIZE, flags)) < 0) {
			goto rollback_pgalloc;
		}

		vma->pages = p;
		__ref_pages(p);
		list_add_after(&(vma->node), &(vma_cur->node));
		vma_cur = vma;
		continue;

rollback_pgalloc:
		free_pages(p);
rollback_pages:
		kfree(p);
rollback_vma:
		kfree(vma);
		goto rollback;
	}

	return 0;

rollback:
	__unmap_and_free_vma(mm, vma_start, mapped);
	return retcode;
}

int
destroy_uvm(struct mm *mm, void *addr, size_t len)
{
	struct vma *vma, *vma_start;
	size_t i = 0;

	if (!IS_ALIGNED(len, PAGE_SIZE) ||
	    mm == NULL ||
	    !PTR_IS_ALIGNED(addr, PAGE_SIZE))
		return -EINVAL;

	/* find the vma */
	for_each_entry (vma, &(mm->vma_head), node) {
		if (vma->start == addr)
			break;
	}
	/* not found? */
	if (&(vma->node) == &(mm->vma_head))
		return -EFAULT;
	vma_start = vma;
	i += PAGE_SIZE;
	for (; i < len; i += PAGE_SIZE) {
		vma = next_entry(vma, node);
		if (vma->start != addr + i)
			/* requested region contain unmapped virtual page */
			return -EFAULT;
	}

	__unmap_and_free_vma(mm, vma_start, len);
	return 0;
}

void *
share_uvm(struct mm *mm_src,
	  void *addr_src,
	  struct mm *mm_dst,
	  void *addr_dst,
	  size_t len,
	  uint32_t flags)
{
}

void
mm_test(void)
{
	kprintf("==========mm_test()  started==========\n");
	struct mm *mm = mm_new();
	kprintf("pgindex: %p\n", mm->pgindex);
	kprintf("creating uvm\n");
	assert(create_uvm(mm, (void *)0x100000, 5 * PAGE_SIZE, VMA_READ | VMA_WRITE) == 0);
	kprintf("destroying uvm1\n");
	assert(destroy_uvm(mm, (void *)0x100000, 2 * PAGE_SIZE) == 0);
	kprintf("destroying uvm2\n");
	assert(destroy_uvm(mm, (void *)0x102000, 3 * PAGE_SIZE) == 0);
	kprintf("destroying mm\n");
	mm_destroy(mm);
	kprintf("another mm\n");
	mm = mm_new();
	kprintf("pgindex: %p\n", mm->pgindex);
	mm_destroy(mm);
	kprintf("==========mm_test() finished==========\n");
}

