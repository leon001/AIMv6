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

/*
 * This file implements a first fit algorithm on a free list data structure
 * to work as a simple allocator.
 */

#include <vmm.h>

#define ALLOC_ALIGN 16

__attribute__ ((aligned(16)))
struct block {
	size_t size;
	bool free;
	struct block *prev;
	struct block *next;
};

static struct block *head = NULL;
//static lock_t lock;

static void *__alloc(size_t size, gfp_t flags)
{
	struct block *this, *newblock;
	size_t allocsize, newsize;

	/* Make a good size */
	if ((size & (ALLOC_ALIGN - 1)) != 0) {
		size -= size & (ALLOC_ALIGN - 1);
		size += ALLOC_ALIGN;
	}
	allocsize = size + sizeof(struct block);

	for (this = head; this != NULL; this = this->next) {
		if (this->size >= allocsize)
			break;
	}
	if (this == NULL) return NULL;

	newsize = this->size - allocsize;
	if (newsize > 0) {
		newblock = ((void *)this) + allocsize;
		newblock->size = newsize;
		newblock->free = true;
		newblock->prev = this->prev;
		newblock->next = this->next;
		if (this->prev != NULL)
			this->prev->next = newblock;
		else
			head = newblock;
		if (this->next != NULL)
			this->next->prev = newblock;
	} else {
		if (this->prev != NULL)
			this->prev->next = this->next;
		else
			head = this->next;
		if (this->next != NULL)
			this->next->prev = this->prev;
	}
	this->prev = NULL;
	this->next = NULL;
	this->free = false;
	return (void *)(this + 1);
}

static struct simple_allocator __bootstrap_allocator;

int simple_allocator_bootstrap(void *pt, size_t size)
{
	struct block *block = pt;
	block->size = size;
	block->free = true;
	block->prev = NULL;
	block->next = NULL;
	head = block;
	__bootstrap_allocator.alloc = __alloc;
	set_simple_allocator(&__bootstrap_allocator);
	return 0;
}

int simple_allocator_init()
{
	return 0;
}

