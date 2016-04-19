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
#include <vmm.h>

#define BOOTSTRAP_POOL_SIZE	1024

void __noreturn master_init(void)
{
	__attribute__ ((aligned))
	uint8_t bootstrap_pool[BOOTSTRAP_POOL_SIZE];

	jump_handlers_apply();
	kputs("KERN: We are in high address!\n");
	simple_allocator_bootstrap(bootstrap_pool, BOOTSTRAP_POOL_SIZE);
	kputs("KERN: Simple allocator bootstrapping!\n");
	void *a, *b, *c;
	a = kmalloc(16, 0);
	b = kmalloc(32, 0);
	c = kmalloc(16, 0);
	kprintf("DEBUG: 0x%08x 0x%08x 0x%08x\n", a, b, c);
	while (1);
}

void __noreturn slave_init(void)
{
	while (1);
}

