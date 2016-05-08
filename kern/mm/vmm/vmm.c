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
static void *__simple_alloc(size_t size, gfp_t flags) { return NULL; }
static void __simple_free(void *obj) {}
static size_t __simple_size(void *obj) { return 0; }

static struct simple_allocator __simple_allocator = {
	.alloc	= __simple_alloc,
	.free	= __simple_free,
	.size	= __simple_size
};

void *kmalloc(size_t size, gfp_t flags)
{
	return __simple_allocator.alloc(size, flags);
}

void kfree(void *obj)
{
	__simple_allocator.free(obj);
}

size_t ksize(void *obj)
{
	return __simple_allocator.size(obj);
}

void set_simple_allocator(struct simple_allocator *allocator)
{
	memcpy(&__simple_allocator, allocator, sizeof(*allocator));
}

void get_simple_allocator(struct simple_allocator *allocator)
{
	memcpy(allocator, &__simple_allocator, sizeof(*allocator));
}

/* dummy implementation again */
static int __caching_create(struct allocator_cache *cache) { return EOF; }
static int __caching_destroy(struct allocator_cache *cache) { return EOF; }
static void *__caching_alloc(struct allocator_cache *cache) { return NULL; }
static void __caching_free(struct allocator_cache *cache, void *obj) {}
static void __caching_trim(struct allocator_cache *cache) {}

struct caching_allocator __caching_allocator = {
	.create		= __caching_create,
	.destroy	= __caching_destroy,
	.alloc		= __caching_alloc,
	.free		= __caching_free,
	.trim		= __caching_trim
};

void set_caching_allocator(struct caching_allocator *allocator)
{
	memcpy(&__caching_allocator, allocator, sizeof(*allocator));
}

