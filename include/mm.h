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

#ifndef _MM_H
#define _MM_H

#define early_kva2pa(kva)	((kva) - KERN_BASE)
#define early_pa2kva(pa)	((pa) + KERN_BASE)

/*
 * Data structure to hold early mappings.
 * type indicates how the mapping should be treated after we
 * jump up to kernel address space.
 * EARLY_MAPPING_MEMORY - Nothing will be done.
 * EARLY_MAPPING_KMMAP - Will be translated to an ioremap() result.
 */
struct early_mapping {
	addr_t	phys_addr;
	size_t	virt_addr;
	size_t	size;
	int	type;
#define	EARLY_MAPPING_MEMORY	0
#define EARLY_MAPPING_KMMAP	1
};

void early_mapping_clear(void);
int early_mapping_add(struct early_mapping *entry);
struct early_mapping *early_mapping_next(struct early_mapping *base);

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 *
 * FIXME:
 * Need a better name.  Also, hardwiring return values with
 * literals is probably not a good idea.
 * Can be changed into at_lower() or before_kernmap() or alike.
 */
int get_addr_space(void);

#endif /* _MM_H */

