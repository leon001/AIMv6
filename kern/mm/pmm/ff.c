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
#include <list.h>

/*
 * This file implements a first fit algorithm on pages.
 */

#include <mm.h>
#include <pmm.h>
#include <vmm.h>
#include <panic.h>

struct block {
	struct pages;
	struct list_head node;
};

static struct page_allocator __allocator;
static struct list_head __head;
static addr_t __free_space;
//static lock_t lock;

static struct pages *__alloc(addr_t size, gfp_t flags)
{
	struct block *this, *newblock;
	struct pages *ret;
	size_t newsize;

	if (!IS_ALIGNED(size, PAGE_SIZE))
		return NULL;
	if (size > __free_space)
		return NULL;

	for_each_entry(this, &__head, node) {
		if (this->size >= size)
			break;
	}
	if (this == NULL) return NULL;

	newsize = this->size - size;
	if (newsize > 0) {
		newblock = kmalloc(sizeof(struct block), 0);
		newblock->paddr = this->paddr + size;
		newblock->size = newsize;
		list_add_after(&newblock->node, &this->node);
	}
	list_del(&this->node);
	ret = kmalloc(sizeof(struct pages), 0);
	ret->paddr = this->paddr;
	ret->size = size;
	__free_space -= size;
	kfree(this);
	return ret;
}

static void __free(struct pages *pages)
{
	struct block *this, *prev = NULL, *tmp, *next = NULL;

	if (!IS_ALIGNED(pages->paddr, PAGE_SIZE))
		return;

	if (!IS_ALIGNED(pages->size, PAGE_SIZE))
		return;

	__free_space += pages->size;
	this = kmalloc(sizeof(struct block), 0);
	if (this == NULL)
		panic("Out of memory during free_pages().\n");
	this->paddr = pages->paddr;
	this->size = pages->size;
	this->flags = pages->flags;
	kfree(pages);

	for_each_entry(tmp, &__head, node) {
		if (tmp->paddr >= this->paddr)
			break;
		prev = tmp;
	}

	if (prev != NULL)
		list_add_after(&this->node, &prev->node);
	else
		list_add_after(&this->node, &__head);

	/* merge downwards */
	if (prev != NULL && prev->paddr + prev->size == this->paddr) {
		prev->size += this->size;
		list_del(&this->node);
		kfree(this);
		this = prev;
	}

	/* merge upwards */
	if (!list_is_last(&this->node, &__head))
		next = list_next_entry(this, struct block, node);
	if (next != NULL && this->paddr + this->size == next->paddr) {
		this->size += next->size;
		list_del(&next->node);
		kfree(next);
	}
}

static addr_t __get_free(void)
{
	return __free_space;
}

int page_allocator_init(void)
{
	__free_space = 0;
	list_init(&__head);
	__allocator.alloc = __alloc;
	__allocator.free = __free;
	__allocator.get_free = __get_free;
	set_page_allocator(&__allocator);
	return 0;
}

int page_allocator_move(struct simple_allocator *old)
{
	struct block *this, *new;
	for_each_entry(this, &__head, node) {
		new = kmalloc(sizeof(struct block), 0);
		if (new == NULL)
			panic("Out of memory during page_allocator_move().\n");
		new->paddr = this->paddr;
		new->size = this->size;
		list_add_after(&new->node, &this->node);
		list_del(&this->node);
		old->free(this);
		this = new;
	}
	return 0;
}

