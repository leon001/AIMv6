/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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
#include <asm.h>
#include <mmu.h>
#include <mm.h>

void page_index_clear(page_index_head_t * index)
{
}

int page_index_early_map(page_index_head_t * index,
			 addr_t paddr,
			 size_t vaddr,
			 size_t length)
{
	return 0;
}

int mmu_init(page_index_head_t *boot_page_index)
{
	return 0;
}

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 */
int get_addr_space(void)
{
	return (get_pc() > KERN_BASE);
}

void early_mm_init(void)
{
	extern page_index_head_t *boot_page_index;

	page_index_init(boot_page_index);
	mmu_init(boot_page_index);
}
