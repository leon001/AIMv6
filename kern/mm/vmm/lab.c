/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#include <sys/types.h>
#include <aim/initcalls.h>
#include <mm.h>
#include <vmm.h>
#include <pmm.h>
#include <console.h>
#include <list.h>
#include <util.h>

/*
 * Large Aligned Object allocator, serving as caching allocator.
 * Not sure if it fully implements SLAB, so named accordingly.
 */

#define LAB_MASK_WORDS	2
#define LAB_ENTRIES	(widthof(LAB_MASK_WORDS)) /* per slab */
#define LAB_MIN_SIZE	(PAGE_SIZE / LAB_ENTRIES)

struct lab_head {
	size_t slab_size;
	struct list_head empty;
	struct list_head partial;
	struct list_head full;
};

struct lab_slab {
	struct list_head node;
	void *vaddr;
	uint32_t used[LAB_MASK_WORDS];
};

static inline bool __is_full(struct lab_slab *slab)
{
	for (int i = 0; i < LAB_MASK_WORDS; i += 1) {
		if (slab->used[i] != 0xFFFFFFFF)
			return false;
	}
	return true;
}

static inline bool __is_empty(struct lab_slab *slab)
{
	for (int i = 0; i < LAB_MASK_WORDS; i += 1) {
		if (slab->used[i] != 0)
			return false;
	}
	return true;
}

/* FIXME not sure whether to expose this to the outside world. */
static int __extend(struct allocator_cache *cache)
{
	void * vaddr;
	struct lab_head *head = cache->head;
	struct lab_slab *slab = kmalloc(sizeof(*slab), 0);
	if (slab == NULL) return EOF;

	/* allocate pages. We can recover the struct so discard after use. */
	struct pages pages = {
		.paddr	= 0,
		.size	= head->slab_size,
		.flags	= cache->flags
	};
	int ret = alloc_pages(&pages);
	if (ret < 0) {
		kfree(slab);
		return EOF;
	}
	vaddr = (void *)pa2kva((size_t)(pages.paddr));
	slab->vaddr = vaddr;

	/* initialize the entries */
	for (int i = 0; i < LAB_MASK_WORDS; i += 1)
		slab->used[i] = 0;
	if (cache->create_obj != NULL) {
		for (size_t off = 0; off < head->slab_size; off += cache->size)
			cache->create_obj(vaddr + off);
	}

	/* add to list, don't sort. */
	list_add_after(&slab->node, &head->empty);

	return 0;
}

static void __trim(struct allocator_cache *cache)
{

}

static int __create(struct allocator_cache *cache)
{
	size_t size = cache->size;
	size_t align = cache->align;

	/* allocate head */
	struct lab_head *head= kmalloc(sizeof(*head), 0);
	if (head == NULL) return EOF;

	/* bitfield has length limit */
	if (size < LAB_MIN_SIZE) size = LAB_MIN_SIZE;
	/* apply alignment */
	size = ALIGN_ABOVE(size, align);
	/* if larger than half a page, use whole pages */
	if (size > (PAGE_SIZE / 2)) size = ALIGN_ABOVE(size, PAGE_SIZE);
	/* fill in struct head */
	head->slab_size = ALIGN_ABOVE(size * LAB_ENTRIES, PAGE_SIZE);
	list_init(&head->empty);
	list_init(&head->partial);
	list_init(&head->full);
	cache->head = head;

	return 0;
}

static int __destroy(struct allocator_cache *cache)
{
	struct lab_head *head = cache->head;
	if (list_empty(&head->partial) == false)
		return EOF;
	if (list_empty(&head->full) == false)
		return EOF;
	/* dispatcher already locked, be careful with this call. */
	__trim(cache);
	/* all the slabs freed by now */
	kfree(head);
	cache->head = NULL; /* play safe */
	return 0;
}

static void *__alloc(struct allocator_cache *cache)
{
	struct lab_head *head = cache->head;
	struct lab_slab *slab;
	int i, j;
	void *obj;

	/* first we try the partial list */
	if (list_empty(&head->partial) == false)
		slab = list_first_entry(&head->partial, typeof(*slab), node);
	else {
		/* we might need to extend */
		if (list_empty(&head->empty)) {
			int ret = __extend(cache);
			if (ret < 0) return NULL; /* out of memory */
		}
		slab = list_first_entry(&head->empty, typeof(*slab), node);
	}

	/* perform a single allocation */
	i = -1;
	do {
		j = get_lowest_0(slab->used[++i]);
	} while (j == -1);
	/* won't go out of bounds */
	slab->used[i] |= (1 << j);
	i = i * sizeof(slab->used[i]) + j;
	obj = slab->vaddr + i * cache->size;
	

	if (__is_full(slab)) {
		/* full after this allocation */
		list_del(&slab->node);
		list_add_after(&slab->node, &head->full);
	} else if (slab->node.prev == &head->empty) {
		/* empty previously */
		list_del(&slab->node);
		list_add_after(&slab->node, &head->partial);
	}

	return obj;
}

static int __free(struct allocator_cache *cache, void *obj)
{
	return EOF;
}

static int __init(void)
{
	kputs("KERN: <lab> Initializing.\n");

	struct caching_allocator allocator = {
		.create		= __create,
		.destroy	= __destroy,
		.alloc		= __alloc,
		.free		= __free,
		.trim		= __trim
	};
	set_caching_allocator(&allocator);

	kputs("KERN: <lab> Done.\n");
	return 0;
}

INITCALL_CORE(__init)

