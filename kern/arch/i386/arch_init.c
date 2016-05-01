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
#include <drivers/io/io-port.h>
#include <util.h>
#include <mm.h>
#include <memlayout.h>
#include <segment.h>
#include <asm.h>

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
			    XPAGE_SIZE
			);
			desc.vaddr = KERN_BASE + desc.paddr;
			vaddr = ALIGN_ABOVE(
			    desc.vaddr + e820map->map[i].size,
			    XPAGE_SIZE
			);
			/* Overflow included */
			
			desc.size = (size_t)min2(
			    (int32_t)KMMAP_BASE,
			    (int32_t)vaddr) - desc.vaddr;
			desc.type = EARLY_MAPPING_MEMORY;

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

void
abs_jump(void *addr)
{
	asm volatile (
		"jmp	*%0"
		: /* no output */
		: "r"(addr)
	);
}

void early_arch_init(void)
{
	//early_mach_init();
	portio_bus_init(&portio_bus);

	probe_memory();
}

/* TODO: put into per-CPU structure */
static struct segdesc gdt[NR_SEGMENTS] = {
	[SEG_KCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0),
	[SEG_KDATA] = SEG(STA_W, 0, 0xffffffff, 0),
	[SEG_UCODE] = SEG(STA_X|STA_R, 0, 0xffffffff, 0),
	[SEG_UDATA] = SEG(STA_W, 0, 0xffffffff, 0)
};

static void segment_init(void)
{
	lgdt(gdt, sizeof(gdt));
#if 0
	uint16_t reg;
	asm volatile (
		"	movw	%2, %0;"
		"	movw	%0, %%ds;"
		"	movw	%0, %%es;"
		"	movw	%0, %%fs;"
		"	movw	%0, %%gs;"
		"	movw	%0, %%ss;"
		"	ljmp	%1, $1f;"
		"1:"
		: "=r"(reg)
		: "i"(SEG_KCODE), "i"(SEG_KDATA)
	);
#endif
}

void arch_init(void)
{
	segment_init();
}

