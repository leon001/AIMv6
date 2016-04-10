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

#include <mm.h>

/*
 * This source file provides upper-level utilities to handle memory mappings.
 * On different systems, memory management unit (MMU)s may look different,
 * may have different names and interfaces, or may even be absent (like MIPS).
 * From the kernel's point of view, we want to unify their access interface,
 * so this wrapper is here.
 */

/*
 * It may take a lot of configuration logic to decide how early mappings should
 * be done. This leads to even more trouble when we mark these mappings
 * in proper kernel data structures later.
 * AIMv6 uses a very simple queue located in .bss to solve the problem:
 * Early initialization routines submit mappings, and the platform-independent
 * routines will call underlying platform-dependent ones to apply them.
 * These data structure are kept in memory, and will later be used to
 * initialize the page allocator and the kmmap subsystem.
 */

#define EARLY_MAPPING_QUEUE_LENGTH	10

/* internal data structure */
static int __early_mapping_queue_size;
static struct early_mapping __early_mapping_queue[EARLY_MAPPING_QUEUE_LENGTH];

void early_mapping_clear(void)
{
	__early_mapping_queue_size = 0;
	/* No need to overwrite anything */
}

/* add a mapping entry */
int early_mapping_add(struct early_mapping *entry)
{
	if (__early_mapping_queue_size > EARLY_MAPPING_QUEUE_LENGTH) {
		/* Bad data structure. Panic immediately to prevent damage. */
		/* FIXME: panic is not yet implemented. */
		while (1);
	}
	if (__early_mapping_queue_size == EARLY_MAPPING_QUEUE_LENGTH) {
		/* Queue full */
		return EOF;
	}
	/* TODO: check for overlap */
	__early_mapping_queue[__early_mapping_queue_size] = *entry;
	__early_mapping_queue_size += 1;
	return 0;
}

/*
 * basic iterator. Caller should not work with internal data structure.
 * If given a pointer to some early mapping entry, return the next one.
 * If given a NULL, return the first entry.
 * If given some invalid entry, or if no more entries are available, return
 * NULL.
 */
struct early_mapping *early_mapping_next(struct early_mapping *base)
{
	struct early_mapping *next;
	int tmp;

	if (base == NULL) {
		next = __early_mapping_queue;
	} else {
		next = base + 1; /* One entry */
	}
	tmp = next - __early_mapping_queue;
	if (tmp < 0 || tmp >= __early_mapping_queue_size) {
		return NULL;
	} else {
		return next;
	}
}
