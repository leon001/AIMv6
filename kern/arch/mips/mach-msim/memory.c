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

#include <pmm.h>
#include <vmm.h>

void mips_add_memory_pages(void)
{
	/* Low RAM */
	struct pages *p = kmalloc(sizeof(*p), 0);
	p->paddr = 0;
	p->size = 0x10000000;	/* TODO: no magic number */
	p->flags = 0;

	free_pages(p);

	/* High RAM */
	p = kmalloc(sizeof(*p), 0);
	p->paddr = HIGHRAM_BASE;
	p->size = HIGHRAM_SIZE;
	p->flags = 0;

	free_pages(p);
}
