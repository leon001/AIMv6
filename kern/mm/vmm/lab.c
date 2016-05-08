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
#include <console.h>
#include <list.h>

/*
 * Large Aligned Object allocator, serving as caching allocator.
 */

static int __create(struct allocator_cache *cache)
{
	return EOF;
}

static int __destroy(struct allocator_cache *cache)
{
	return EOF;
}

static void *__alloc(struct allocator_cache *cache)
{
	return NULL;
}

static void __free(struct allocator_cache *cache, void *obj)
{

}

static void __trim(struct allocator_cache *cache)
{

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

