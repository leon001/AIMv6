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

#ifndef _DRIVERS_SD_SD_H
#define _DRIVERS_SD_SD_H

#ifdef RAW /* baremetal driver */

int	sd_init(void);
int	sd_init_card(void);
int	sd_read(u32 pa, u16 count, u32 offset);
int	sd_write(u32 pa, u16 count, u32 offset);

#else /* not RAW, or kernel driver */

#endif /* RAW */


#endif /* _DRIVERS_SD_SD_H */

