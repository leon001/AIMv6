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

#ifndef _DRIVERS_TIMER_TIMER_A9_HW_H
#define _DRIVERS_TIMER_TIMER_A9_HW_H

/* base address */
#define GT_PHYSBASE	(MPCORE_PHYSBASE + 0x200)
#define PT_PHYSBASE	(MPCORE_PHYSBASE + 0x600)
#define SWDT_PHYSBASE	(MPCORE_PHYSBASE + 0x620)

/* register offset */
#define GT_COUNTER_LO_OFFSET		0x00
#define GT_COUNTER_HI_OFFSET		0x04
#define GT_CTRL_OFFSET			0x08
#define GT_INT_OFFSET			0x0C
#define GT_COMPARATOR_LO_OFFSET	0x10
#define GT_COMPARATOR_HI_OFFSET	0x14
#define GT_INCREMENT_OFFSET		0x18

#endif /* _DRIVERS_TIMER_TIMER_A9_HW_H */

