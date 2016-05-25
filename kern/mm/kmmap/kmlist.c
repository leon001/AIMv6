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
#include <aim/early_kmmap.h>
#include <aim/kmmap.h>
#include <aim/initcalls.h>
#include <mm.h>
#include <console.h>
#include <panic.h>
#include <list.h>

/*
 * KMList serves as a kmmap keeper, using a linked list sorted by
 * virual address.
 */

struct kmlist_entry {
	struct list_head node;
	struct kmmap_entry data;
};

static struct list_head head = EMPTY_LIST(head);

static int map(struct kmmap_entry *entry)
{
	if (!IS_ALIGNED(entry->paddr, PAGE_SIZE))
		return EOF;
	if (!IS_ALIGNED((size_t)entry->vaddr, PAGE_SIZE))
		return EOF;
	
	return EOF;
}

static void init(void)
{
	struct early_mapping *old;
	struct kmmap_entry new;

	/* Go through early kmmap queue */
	for (
		old = early_mapping_next(NULL); old != NULL;
		old = early_mapping_next(old)
	) {
		switch (old->type) {
			case EARLY_MAPPING_MEMORY:
				/* Linear memory */
				new.flags = MAP_KERN_MEM | MAP_LARGE;
				break;
			case EARLY_MAPPING_KMMAP:
				/* Devices */
				new.flags = MAP_SHARED_DEV | MAP_LARGE;
				break;
			case EARLY_MAPPING_TEMP:
				/* Low mappings */
				continue;
			case EARLY_MAPPING_OTHER:
				/* Reserved memory */
				new.flags = MAP_KERN_MEM | MAP_LARGE;
				break;
		}
		new.paddr = old->paddr;
		new.vaddr = old->vaddr;
		new.size = old->size;
		assert(map(&new) == 0);
	}
}

static size_t unmap(void *vaddr)
{
	return 0;
}

static int __kva2pa(void *vaddr, addr_t *paddr)
{
	return EOF;
}

static struct kmmap_entry *next(struct kmmap_entry *base)
{
	return NULL;
}

static int __init(void)
{
	struct kmmap_keeper this = {
		.init	= init,
		.map	= map,
		.unmap	= unmap,
		.kva2pa	= __kva2pa,
		.next	= next
	};
	set_kmmap_keeper(&this);
	return 0;
}

EARLY_INITCALL(__init)

