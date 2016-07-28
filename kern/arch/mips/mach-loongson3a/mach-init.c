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

#include <drivers/io/io-port.h>
#include <drivers/io/io-mem.h>
#include <io.h>
#include <mp.h>
#include <platform.h>
#include <aim/device.h>
#include <mach-conf.h>

#include "introuter-ls3a.h"

dev_t rootdev;

void early_mach_init(void)
{
	/* XXX: maybe unnecessary... */
	portio_bus_connect(&early_portio_bus,
			   &early_memory_bus,
			   LOONGSON3A_PORTIO_BASE);
}

void mach_init(void)
{
	int i;
	/* Enable all IPI bits and clear the IPI bits first */
	for (i = 0; i < nr_cpus(); ++i) {
		write32(LOONGSON3A_COREx_IPI_CLEAR(i), 0xffffffff);
		write32(LOONGSON3A_COREx_IPI_ENABLE(i), 0xffffffff);
	}

	rootdev = makedev(IDE_DISK_MAJOR, ROOT_PARTITION_ID);
}

struct devtree_entry devtree[] = {
	/* memory bus */
	{
		"memory",
		"memory",
		"",
		0,
		{0},
		DEVTREE_IM_NONE,
		"",
		0,
		{0},
	},
	/* Loongson 3A interrupt router */
	{
		"introuter-ls3a",
		"introuter-ls3a",
		"memory",
		1,
		{LOONGSON3A_INTROUTER_BASE},
		DEVTREE_IM_CTRL,
		"cpu",
		DEVTREE_INTR_AUTO,
		{0},
	},
	/* LPC UART */
	{
		"ns16550",
		"ns16550",
		"memory",
		1,
		{LOONGSON3A_UART_BASE},
		DEVTREE_IM_GEN,
		"introuter-ls3a",
		DEVTREE_INTR_AUTO,
		{IR_LPC},
	},
	/* HyperTransport Bus #1 */
	{
		"hypertransport1",
		"hypertransport",
		"memory",
		1,
		{LOONGSON3A_HT1_BASE},
		DEVTREE_IM_CTRL,
		"introuter-ls3a",
		DEVTREE_INTR_AUTO,
		{IR_HT1_INTx(0)},
	},
	/* Port I/O */
	{
		"portio",
		"portio",
		"hypertransport1",
		1,
		{LOONGSON3A_HT_PORTIO_BASE},
		DEVTREE_IM_NONE,
		"",
		0,
		{0},
	},
	/* Intel 8259A interrupt controller */
	{
		"i8259",
		"i8259",
		"portio",
		2,
		{0x20, 0xa0}	/* master and slave */,
		DEVTREE_IM_CTRL,
		"hypertransport1",
		DEVTREE_INTR_AUTO,
		{0},
	},
	/* PCI */
	{
		"pci",
		"pci",
		"hypertransport1",
		1,
		{LOONGSON3A_HT_PCICFG_BASE},
		DEVTREE_IM_CTRL,
		"i8259",
		DEVTREE_INTR_AUTO,
		{0},
	},
	/* the devices attached to PCI bus will be instantiated upon PCI
	 * bus device instantiation. */
};

int ndevtree_entries = ARRAY_SIZE(devtree);

