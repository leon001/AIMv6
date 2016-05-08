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

#include <libc/string.h>

/* dummy implementations */
static void *__alloc(size_t size, gfp_t flags) { return NULL; }
static void __free(void *obj) {}
static size_t __size(void *obj) { return 0; }

static struct simple_allocator __allocator = {
	.alloc	= __alloc,
	.free	= __free,
	.size	= __size
};

void set_simple_allocator(struct simple_allocator *allocator)
{
	memcpy(&__allocator, allocator, sizeof(*allocator));
}

void *kmalloc(size_t size, gfp_t flags)
{
	return __allocator.alloc(size, flags);
}

void kfree(void *obj)
{
	__allocator.free(obj);
}

size_t ksize(void *obj)
{
	return __allocator.size(obj);
}

void get_simple_allocator(struct simple_allocator *allocator)
{
	memcpy(allocator, &__allocator, sizeof(*allocator));
}

