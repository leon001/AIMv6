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

/*
 * This is the code for RAW driver, that is, for firmware.
 * It is separated from msim-ddisk.c for clarity and is included as is in
 * msim-ddisk.c by #include.
 * The internal routines are already provided in msim-ddisk.c
 */

void msim_dd_init(unsigned long paddr)
{
	__msim_dd_init(paddr);
}

size_t msim_dd_get_sector_count(unsigned long paddr)
{
	return __msim_dd_get_sector_count(paddr);
}

int msim_dd_read_sector(unsigned long paddr, off_t off, void *buf, bool poll)
{
	return __msim_dd_read_sector(paddr, off, buf, poll);
}

int msim_dd_write_sector(unsigned long paddr, off_t off, void *buf, bool poll)
{
	return __msim_dd_write_sector(paddr, off, buf, poll);
}

int msim_dd_check_interrupt(unsigned long paddr)
{
	return __msim_dd_check_interrupt(paddr);
}

void msim_dd_ack_interrupt(unsigned long paddr)
{
	return __msim_dd_ack_interrupt(paddr);
}

