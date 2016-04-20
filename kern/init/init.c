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

/* from kernel */
#include <sys/types.h>
#include <console.h>
#include <mm.h>
#include <pmm.h>
#include <vmm.h>

#define BOOTSTRAP_POOL_SIZE	1024

void __noreturn master_init(void)
{
	__attribute__ ((aligned(16)))
	uint8_t bootstrap_pool[BOOTSTRAP_POOL_SIZE];

	jump_handlers_apply();
	kputs("KERN: We are in high address.\n");
	simple_allocator_bootstrap(bootstrap_pool, BOOTSTRAP_POOL_SIZE);
	kputs("KERN: Simple allocator bootstrapping.\n");
	page_allocator_init();
	kputs("KERN: Page allocator initialized.\n");
	add_memory_pages();
	kputs("KERN: Pages added.\n");
	kprintf("KERN: Free memory: 0x%08x\n", (size_t)get_free_memory());
	struct pages *p1, *p2;
	p1 = alloc_pages(0x1000, 0);
	p2 = alloc_pages(0x2000, 0);
	kprintf("DEBUG: paddr=0x%08x, size=0x%08x\n", (size_t)p1->paddr, p1->size);
	kprintf("DEBUG: paddr=0x%08x, size=0x%08x\n", (size_t)p2->paddr, p2->size);
	free_pages(p1);
	p1 = alloc_pages(0x1000, 0);
	kprintf("DEBUG: paddr=0x%08x, size=0x%08x\n", (size_t)p1->paddr, p1->size);
	kprintf("KERN: Free memory: 0x%08x\n", (size_t)get_free_memory());
	while (1);
}

void __noreturn slave_init(void)
{
	while (1);
}

