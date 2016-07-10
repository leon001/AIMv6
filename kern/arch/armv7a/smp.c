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

/*
 * Needed to compare the NR_CPUS macro. Positive values reserved for
 * real cpu numbers and therefore can't be used.
 */
#define DETECT	-1

static int __nr_cpus;

void detect_cpus()
{
#if NR_CPUS == DETECT
	/* TODO: detect is possible if MACH supports it, which is true
	 * for A9, and can be read from the MPCore's SCU CPU Power Status
	 * Register. Cores outside the cluster cannot be detected.
	 * Avoid using THIS detection
	 */
#else
	__nr_cpus = NR_CPUS;
#endif
}

int nr_cpus()
{
	return __nr_cpus;
}

