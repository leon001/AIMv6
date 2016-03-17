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

#include <addrspace.h>
#include <io.h>
#include <config.h>
#include <libc/string.h>
#include <drivers/block/hd.h>
#include <drivers/block/msim-ddisk.h>

unsigned char msim_dd_dma[SECTOR_SIZE];

void msim_dd_init(unsigned long paddr)
{
	write32(MSIM_DD_REG(paddr, MSIM_DD_DMAADDR), kv2p(msim_dd_dma));
}

size_t msim_dd_get_sector_count(unsigned long paddr)
{
	return read32(MSIM_DD_REG(paddr, MSIM_DD_SIZE));
}

/*
 * Read sector, returns 0 if successful.
 */
int msim_dd_read_sector(unsigned long paddr, size_t sect, void *buf, bool poll)
{
	write32(MSIM_DD_REG(paddr, MSIM_DD_DMAADDR), kv2p(msim_dd_dma));
	write32(MSIM_DD_REG(paddr, MSIM_DD_SECTOR), sect);
	write32(MSIM_DD_REG(paddr, MSIM_DD_COMMAND), CMD_READ);
	if (poll) {
		while (!msim_dd_check_interrupt(paddr))
			/* nothing */;
		/* Clear interrupt */
		msim_dd_ack_interrupt(paddr);
		if (read32(MSIM_DD_REG(paddr, MSIM_DD_STAT)) & STAT_ERROR) {
			return -1;
		} else {
			memcpy(buf, msim_dd_dma, SECTOR_SIZE);
			return 0;
		}
	} else {
		return 0;
	}
}

int msim_dd_write_sector(unsigned long paddr, size_t sect, void *buf, bool poll)
{
	memcpy(msim_dd_dma, buf, SECTOR_SIZE);
	write32(MSIM_DD_REG(paddr, MSIM_DD_DMAADDR), kv2p(msim_dd_dma));
	write32(MSIM_DD_REG(paddr, MSIM_DD_SECTOR), sect);
	write32(MSIM_DD_REG(paddr, MSIM_DD_COMMAND), CMD_WRITE);
	if (poll) {
		while (!msim_dd_check_interrupt(paddr))
			/* nothing */;
		/* Clear interrupt */
		msim_dd_ack_interrupt(paddr);
		if (read32(MSIM_DD_REG(paddr, MSIM_DD_STAT)) & STAT_ERROR)
			return -1;
		else
			return 0;
	} else {
		return 0;
	}
}

int msim_dd_check_interrupt(unsigned long paddr)
{
	return !!(read32(MSIM_DD_REG(paddr, MSIM_DD_STAT)) & STAT_INTR);
}

void msim_dd_ack_interrupt(unsigned long paddr)
{
	write32(MSIM_DD_REG(paddr, MSIM_DD_COMMAND), CMD_ACK);
}

