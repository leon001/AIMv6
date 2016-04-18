/* Copyright (C) 2016 Xiaofei Bai <xffbai@gmail.com>
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
#include <init.h>
#include <drivers/io/io_port.h>
#include <util.h>
#include <mm.h>
#include <memlayout.h>

/* FIXME: put in mm.c? */
static size_t mem_size = 0;

/* FIXME: put in mm.c? */
addr_t get_mem_size(void)
{
	return mem_size;
}

/* FIXME: put in mm.c? */
static void probe_memory(void)
{
	struct e820map *e820map = (struct e820map *)BOOT_E820MAP;
	struct early_mapping desc;
	uint32_t vaddr = KERN_BASE;

	for (int i = 0; i < e820map->num; ++i) {
		if (e820map->map[i].type == E820_RAM) {
			desc.paddr = ALIGN_BELOW(
			    (uint32_t)e820map->map[i].start,
			    PAGE_SIZE
			);
			desc.vaddr = vaddr;
			vaddr = ALIGN_ABOVE(
			    vaddr + e820map->map[i].size,
			    PAGE_SIZE
			);
			/* Overflow included */
			
			desc.size = (size_t)min2(
			    (int32_t)KMMAP_BASE,
			    (int32_t)vaddr) - desc.vaddr;

			early_mapping_add(&desc);

			/* identity mapping */
			desc.vaddr = desc.paddr;
			early_mapping_add(&desc);

			mem_size = max2(mem_size,
			    (size_t)e820map->map[i].start +
			    (size_t)e820map->map[i].size);

			if ((int32_t)vaddr > (int32_t)KMMAP_BASE)
				break;
		}
	}
}

void early_arch_init(void)
{
	//early_mach_init();
	portio_bus_init(&portio_bus);

	probe_memory();
}

