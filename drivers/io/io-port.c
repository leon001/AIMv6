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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>

#include <io.h>
#include <aim/device.h>
#include <util.h>

#include <asm.h>	/* inb() and outb() should be declared there */
#include <io-port.h>
#include <mm.h>

/*
 * To determine whether we should interact with port I/O bus directly or
 * via memory bus, we check if the bus instance is connected to another
 * bus.
 *
 * For architectures without ways to directly interact with port I/O bus,
 * or more precisely, architectures without IN/OUT instructions,
 * functions inb(), outb(), etc. are **REQUIRED** to cause a panic.
 *
 * If the driver framework works correctly, port-mapped I/O through
 * address space should be redirected to memory bus, so the 'else'
 * branches in read/write functions below are never reached.
 */
static int __read8(struct bus_device *inst, addr_t addr, uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read8;

	if (bus) {
		bus_read8 = bus->bus_driver.get_read_fp(bus, 8);
		return bus_read8(bus, inst->base + addr, ptr);
	} else {
		*ptr = inb((uint16_t)addr);
		return 0;
	}
}

static int __write8(struct bus_device *inst, addr_t addr, uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write8;

	if (bus) {
		bus_write8 = bus->bus_driver.get_write_fp(bus, 8);
		return bus_write8(bus, inst->base + addr, val);
	} else {
		outb((uint16_t)addr, val);
		return 0;
	}
}

static int __read16(struct bus_device *inst, addr_t addr, uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read16;

	if (bus) {
		bus_read16 = bus->bus_driver.get_read_fp(bus, 16);
		return bus_read16(bus, inst->base + addr, ptr);
	} else {
		*ptr = inw((uint16_t)addr);
		return 0;
	}
}

static int __write16(struct bus_device *inst, addr_t addr, uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write16;

	if (bus) {
		bus_write16 = bus->bus_driver.get_write_fp(bus, 16);
		return bus_write16(bus, inst->base + addr, val);
	} else {
		outw((uint16_t)addr, val);
		return 0;
	}
}

static int __read32(struct bus_device *inst, addr_t addr, uint64_t *ptr)
{
	struct bus_device *bus = inst->bus;
	bus_read_fp bus_read32;

	if (bus) {
		bus_read32 = bus->bus_driver.get_read_fp(bus, 32);
		return bus_read32(bus, inst->base + addr, ptr);
	} else {
		*ptr = ind((uint32_t)addr);
		return 0;
	}
}

static int __write32(struct bus_device *inst, addr_t addr, uint64_t val)
{
	struct bus_device *bus = inst->bus;
	bus_write_fp bus_write32;

	if (bus) {
		bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
		return bus_write32(bus, inst->base + addr, val);
	} else {
		outd((uint32_t)addr, val);
		return 0;
	}
}

static bus_read_fp __get_read_fp(struct bus_device *inst, int data_width)
{
	if (inst == &portio_bus) {
		switch (data_width) {
		case 8:
			return __read8;
		case 16:
			return __read16;
		case 32:
			return __read32;
		}
	}
	return NULL;
}

static bus_write_fp __get_write_fp(struct bus_device *inst, int data_width)
{
	if (inst == &portio_bus) {
		switch (data_width) {
		case 8:
			return __write8;
		case 16:
			return __write16;
		case 32:
			return __write32;
		}
	}
	return NULL;
}

void portio_bus_connect(struct bus_device *portio,
			struct bus_device *bus,
			addr_t base)
{
	portio->base = base;
	portio->bus = bus;
}

static void __portio_bus_init(struct bus_device *portio)
{
	portio->bus_driver.get_read_fp = __get_read_fp;
	portio->bus_driver.get_write_fp = __get_write_fp;
}

static void __jump_handler(void)
{
	__portio_bus_init(&portio_bus);
}

void portio_bus_init(struct bus_device *portio)
{
	__portio_bus_init(portio);

	if (jump_handlers_add((generic_fp)postmap_addr(__jump_handler)) != 0)
		for (;;) ;	/* panic */
}

/*
 * IMPORTANT NOTE:
 * Port I/O bus structure should be further initialized in
 * machine-dependent code in case of accessing ports via
 * address spaces.
 */
struct bus_device portio_bus = {
	/* FIXME ? */
	.addr_width = 32,
	.class = DEVCLASS_BUS,
};

