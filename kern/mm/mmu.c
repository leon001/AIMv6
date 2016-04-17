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
#include <mmu.h>

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
static size_t __mem_top;
static size_t __kmmap_top;

void early_mapping_clear(void)
{
	__early_mapping_queue_size = 0;
	__mem_top = KERN_BASE;
	__kmmap_top = KMMAP_BASE;
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
	/* TODO: check for overlap and alignment */
	__early_mapping_queue[__early_mapping_queue_size] = *entry;
	__early_mapping_queue_size += 1;
	return 0;
}

size_t early_mapping_add_memory(addr_t base, size_t size)
{
	/* check available address space */
	if (__mem_top >= KMMAP_BASE)
		return 0;
	if (size > KMMAP_BASE - __mem_top)
		size = KMMAP_BASE - __mem_top;

	/* construct the descriptor and register the mapping */
	struct early_mapping desc = {
		.paddr = base,
		.vaddr = __mem_top,
		.size = (size_t)size,
		.type = EARLY_MAPPING_MEMORY
	};
	int ret = early_mapping_add(&desc);
	if (ret != 0) return 0;/* fail */
	__mem_top += size;
	return size;
}

size_t early_mapping_add_kmmap(addr_t base, size_t size)
{
	/* check available address space */
	if (__kmmap_top >= RESERVED_BASE)
		return EOF;

	/* construct the descriptor and register the mapping */
	struct early_mapping desc = {
		.paddr = base,
		.vaddr = __kmmap_top,
		.size = size,
		.type = EARLY_MAPPING_KMMAP
	};
	int ret = early_mapping_add(&desc);
	if (ret != 0) return 0;
	__kmmap_top += size;
	return __kmmap_top - size;
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

int page_index_init(page_index_head_t *boot_page_index)
{
	struct early_mapping *mapping = early_mapping_next(NULL);
	int ret;

	page_index_clear(boot_page_index);

	for (; mapping != NULL; mapping = early_mapping_next(mapping)) {
		ret = page_index_early_map(boot_page_index, mapping->paddr,
			mapping->vaddr, mapping->size);
		if (ret == EOF) return EOF;
	}
	return 0;
}

/* handlers after mmu start and after jump */
#define MMU_HANDLER_QUEUE_LENGTH	10
static int __mmu_handler_queue_size;
static generic_fp __mmu_handler_queue[MMU_HANDLER_QUEUE_LENGTH];

void mmu_handlers_clear(void)
{
	__mmu_handler_queue_size = 0;
}

int mmu_handlers_add(generic_fp entry)
{
	if (__mmu_handler_queue_size > MMU_HANDLER_QUEUE_LENGTH) {
		/* Bad data structure. Panic immediately to prevent damage. */
		/* FIXME: panic is not yet implemented. */
		while (1);
	}
	if (__mmu_handler_queue_size == MMU_HANDLER_QUEUE_LENGTH) {
		/* Queue full */
		return EOF;
	}
	__mmu_handler_queue[__mmu_handler_queue_size] = entry;
	__mmu_handler_queue_size += 1;
	return 0;
}

void mmu_handlers_apply(void)
{
	for (int i = 0; i < __mmu_handler_queue_size; ++i) {
		__mmu_handler_queue[i]();
	}
}

#define JUMP_HANDLER_QUEUE_LENGTH	10
static int __jump_handler_queue_size;
static generic_fp __jump_handler_queue[JUMP_HANDLER_QUEUE_LENGTH];

void jump_handlers_clear(void)
{
	__jump_handler_queue_size = 0;
}

int jump_handlers_add(generic_fp entry)
{
	if (__jump_handler_queue_size > JUMP_HANDLER_QUEUE_LENGTH) {
		/* Bad data structure. Panic immediately to prevent damage. */
		/* FIXME: panic is not yet implemented. */
		while (1);
	}
	if (__jump_handler_queue_size == JUMP_HANDLER_QUEUE_LENGTH) {
		/* Queue full */
		return EOF;
	}
	__jump_handler_queue[__jump_handler_queue_size] = entry;
	__jump_handler_queue_size += 1;
	return 0;
}

void jump_handlers_apply(void)
{
	for (int i = 0; i < __jump_handler_queue_size; ++i) {
		__jump_handler_queue[i]();
	}
}

