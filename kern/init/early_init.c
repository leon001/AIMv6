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
#include <init.h>
#include <console.h>
#include <mm.h>

void __noreturn master_early_init(void)
{
	extern page_index_head_t *boot_page_index;

	early_mapping_clear();

	early_arch_init();
	early_console_init();
	kputs("KERN: Hello, world!\n");

	/* add default mapping last */
	early_mapping_add_memory(
		get_mem_physbase(),
		(size_t)get_mem_size());

	/* dump some debug info */
	kprintf("KERN: Total memory: 0x%08x\n", (size_t)get_mem_size());
	struct early_mapping *mapping = early_mapping_next(NULL);
	for (; mapping != NULL; mapping = early_mapping_next(mapping)) {
		kprintf("KERN: early_mapping(P=0x%08x, V=0x%08x, S=0x%08x, %d)\n",
			(size_t)mapping->paddr, mapping->vaddr, 
			mapping->size, mapping->type);
	}

	/* initialize and apply page index */
	page_index_init(boot_page_index);
	mmu_init(boot_page_index);
	while (1);
}

void __noreturn slave_early_init(void)
{
	while (1);
}


