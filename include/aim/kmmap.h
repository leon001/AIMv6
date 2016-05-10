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

#ifndef _AIM_KMMAP_H
#define _AIM_KMMAP_H

#include <sys/types.h>
#include <mm.h>

#ifndef __ASSEMBLER__

struct kmmap_entry {
	addr_t		paddr;
	void		*vaddr;
	size_t		size;
	uint32_t	flags;
/* use the higher halfword to avoid conflict with VMA_* flags */
#define MAP_TYPE_MASK	0x30000
#define MAP_USER_MEM	0x00000
#define	MAP_KERN_MEM	0x10000
#define MAP_PRIV_DEV	0x20000	/* eg. device on private bus */
#define MAP_SHARED_DEV	0x30000	/* normal devices */
#define MAP_LARGE	0x40000
};

struct kmmap_keeper {
	void (*init)(void);
	int (*map)(struct kmmap_entry *entry);
	size_t (*unmap)(void *vaddr);
	int (*kva2pa)(void *vaddr, addr_t *paddr);
	struct kmmap_entry *(*next)(struct kmmap_entry *base);
};

struct kmmap_allocator {
	void (*init)(struct kmmap_keeper *keeper);
	void *(*alloc)(size_t size);
	void (*free)(void *pt, size_t size);
};

void set_kmmap_allocator(struct kmmap_allocator *allocator);
void set_kmmap_keeper(struct kmmap_keeper *keeper);

void kmmap_init(void);
void *kmmap(void *vaddr, addr_t paddr, size_t size, uint32_t flags);
size_t kmunmap(void *vaddr);
int kmmap_apply(pgindex_t *pgindex);

#endif /* !__ASSEMBLER__ */

#endif /* _AIM_KMMAP_H */
