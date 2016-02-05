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

#include <sys/types.h>
#include <io.h>

#include <mach.h>
#include <timer-a9.h>
#include <timer-a9-hw.h>

// FIXME
#define GT_TPUS 400l
#define GT_TPS	(GT_TPUS * 1000000l)


#ifdef RAW /* baremetal driver */

#define GT_BASE	GT_PHYSBASE

uint64_t timer_read(void)
{
	return gt_read();
}

#else /* not RAW, or kernel driver */

#endif /* RAW */

uint64_t gt_get_tpus(void)
{
	// FIXME
	return GT_TPUS;
}

uint64_t gt_get_tps(void)
{
	// FIXME
	return GT_TPS;
}

uint64_t gt_read(void)
{
	uint64_t time;
	uint64_t hi, lo, tmp;
	/* HI-LO-HI reading because GTC is 64bit */
	do {
		hi = read32(GT_BASE + GT_COUNTER_HI_OFFSET);
		lo = read32(GT_BASE + GT_COUNTER_LO_OFFSET);
		tmp = read32(GT_BASE + GT_COUNTER_HI_OFFSET);
	} while (hi != tmp);
	time = (uint64_t)hi << 32;
	time |= lo;
	return time;
}

