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
#endif

#include <mmu.h>
#include <sys/types.h>

size_t get_mem_size(void)
{
	return MEM_SIZE;
}

void page_index_clear(pgindex_t * index)
{
}

int page_index_early_map(pgindex_t * index,
			 addr_t paddr,
			 size_t vaddr,
			 size_t length)
{
	return 0;
}

int mmu_init(pgindex_t *boot_page_index)
{
	return 0;
}

int get_addr_space(void)
{
	/* MIPS kernels always run in high address space (0x80000000+);
	 * the kernel space mapping is hardwired by MIPS architecture.
	 * We don't have any choice. */
	return 1;
}

