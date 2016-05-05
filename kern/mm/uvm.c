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

struct mm *mm_new(void)
{
	struct mm *mm = (struct mm *)kmalloc(sizeof(*mm), 0);
	memset(mm, 0, sizeof(*mm));
	if (mm != NULL) {
		list_init(&(mm->vma_head));
		if (init_pgindex(&(mm->pgindex)) == 0)
			return mm;
	}

fail:
	kfree(mm);
	return NULL;
}

void mm_destroy(struct mm *mm)
{
	struct vma *vma, *vma_next;

	/* Unmap each virtual memory area */
	for_each_entry (vma, &(mm->vma_head), node) {
		if (unmap_and_free_pages(&(mm->pgindex),
					 vma->start,
					 vma->size) < 0)
			/* temporary */
			panic("destroy_uvm: %p %p\n", mm->pgindex, vma->start);
	}

	/* Destroy the page index itself */
	destroy_pgindex(&(mm->pgindex));

	/* Free the struct vma list */
	for_each_entry_safe (vma, vma_next, &(mm->vma_head), node)
		kfree(vma);

	kfree(mm);
}

/* Find struct vma with start address @addr */
static struct vma *vma_find(struct mm *mm, void *addr)
{
	for_each_entry (vma, &(mm->vma_head), node) {
		if (addr >= vma->start &&
		    addr < (vma->start + vma->size))
			return vma;
	}
	return NULL;
}

int destroy_uvm(struct mm *mm, void *addr, size_t len)
{
	/* This function mainly maintains the vma list, and leaves the
	 * actual unmapping job to unmap_and_free_pages(). */
	struct vma *vma = vma_find(mm, addr);
	int retcode;

	if (vma == NULL ||
	    !(addr + len >= vma->start &&
	      addr + len < vma->start + vma->size))
		return -EINVAL;
	if (len == 0)
		return 0;

	void *vma_end = vma->start + vma->size;

	if (addr == vma->start && len == vma->size) {
		if ((retcode = unmap_and_free_pages(&(mm->pgindex),
						    vma->start,
						    vma->size)) < 0)
			return retcode;

		list_del(&(vma->node));
		kfree(vma);
		return 0;
	} else if (addr == vma->start) {
		vma->start += len;
	} else if (addr + len == vma_end) {
		vma->size -= len;
	} else {
		/* split the struct vma into two structs */
		struct vma *newvma = (struct vma *)kmalloc(sizeof(*newvma), 0);
		if (newvma == NULL)
			return -ENOMEM;
		newvma->start = addr + len;
		newvma->size = vma_end - addr;
		newvma->flags = vma->flags;
		newvma->mm = mm;
	}

	if ((retcode = unmap_and_free_pages(&(mm->pgindex), addr, len)) < 0)
		goto rollback_vma;

	vma->size = addr - vma->start;
	list_add_after(newvma, vma);
	return 0;

rollback_vma:
	kfree(newvma);
	return retcode;
}

int create_uvm(struct mm *mm, void *addr, size_t len, uint32_t flags)
{
}

