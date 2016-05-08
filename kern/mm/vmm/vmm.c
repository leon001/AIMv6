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

#include <vmm.h>
#include <panic.h>

static struct simple_allocator *__simple_allocator = NULL;

void set_simple_allocator(struct simple_allocator *allocator)
{
	__simple_allocator = allocator;
}

void *kmalloc(size_t size, gfp_t flags)
{
	if (__simple_allocator == NULL)
		panic("kmalloc() called but no allocator available.\n");
	return __simple_allocator->alloc(size, flags);
}

void kfree(void *obj)
{
	if (__simple_allocator == NULL)
		panic("kfree() called but no allocator available.\n");
	if (obj == NULL)
		return;
	__simple_allocator->free(obj);
}

size_t ksize(void *obj)
{
	if (__simple_allocator == NULL)
		panic("ksize() called but no allocator available.\n");
	return __simple_allocator->size(obj);
}

struct simple_allocator *get_simple_allocator(void)
{
	return __simple_allocator;
}

