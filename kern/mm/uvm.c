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
		list_init(&(mm->node));
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
		if (unmap_pages(&(mm->pgindex), vma->start, vma->size))
			panic("unmap pages failed: %p %p %p\n",
			      mm->pgindex,
			      vma->start,
			      vma->size);
	}

	/* Free the struct vma list */
	for_each_entry_safe (vma, vma_next, &(mm->vma_head), node) {
		kfree(vma);
	}

	kfree(mm);
}

