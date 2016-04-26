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

#include <pmm.h>
#include <panic.h>

static struct page_allocator *__page_allocator = NULL;

void set_page_allocator(struct page_allocator *allocator)
{
	__page_allocator = allocator;
}

struct pages * alloc_pages(addr_t count, gfp_t flags)
{
	if (__page_allocator == NULL)
		panic("alloc_page() called but no allocator available.\n");
	return __page_allocator->alloc(count, flags);
}

void free_pages(struct pages *pages)
{
	if (__page_allocator == NULL)
		panic("free_page() called but no allocator available.\n");
	__page_allocator->free(pages);
}

addr_t get_free_memory(void)
{
	if (__page_allocator == NULL)
		panic("get_free_memory() called but no allocator available.\n");
	return __page_allocator->get_free();
}

