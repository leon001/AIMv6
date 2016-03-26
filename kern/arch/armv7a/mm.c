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

/* get_addr_space()
 * determine whether we are running in low address or in high address
 * return values:
 * 0 - low address
 * 1 - high address
 * negative - reserved for errors
 */
int get_addr_space()
{
	uint32_t pc;

	asm volatile (
		"mov	%[pc], pc"
		:[pc] "=r" (pc)
	);
	return (pc > KERN_BASE);
}

void mmu_init()
{

}

