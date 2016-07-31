
#include <aim/device.h>
#include <sys/types.h>

static uint32_t
__read32(struct bus_device *pci, uint32_t tag, int reg)
{
	uint64_t val;
	struct bus_device *bus = pci->bus;
	bus_read_fp bus_read32 = bus->bus_driver.get_read_fp(bus, 32);

	bus_read32(bus, pci->base, tag + reg, &val);
	return (uint32_t)val;
}

static uint16_t
__read16(struct bus_device *pci, uint32_t tag, int reg)
{
	uint32_t val = __read32(pci, tag, reg & ~3);
	return (uint16_t)(val >> ((reg & 3) * 8));
}

static uint8_t
__read8(struct bus_device *pci, uint32_t tag, int reg)
{
	uint32_t val = __read32(pci, tag, reg & ~3);
	return (uint8_t)(val >> ((reg & 3) * 8));
}

static void
__write32(struct bus_device *pci, uint32_t tag, int reg, uint32_t val)
{
	struct bus_device *bus = pci->bus;
	bus_write_fp bus_write32 = bus->bus_driver.get_write_fp(bus, 32);
	bus_write32(bus, pci->base, tag + reg, val);
}

static void
__write16(struct bus_device *pci, uint32_t tag, int reg, uint16_t val)
{
	uint32_t res = __read32(pci, tag, reg & ~3);
	res &= ~(0xffff << ((reg & 3) * 8));
	res |= (val << ((reg & 3) * 8));
	__write32(pci, tag, reg & ~3, res);
}

static void
__write8(struct bus_device *pci, uint32_t tag, int reg, uint8_t val)
{
	uint32_t res = __read32(pci, tag, reg & ~3);
	res &= ~(0xff << ((reg & 3) * 8));
	res |= (val << ((reg & 3) * 8));
	__write32(pci, tag, reg & ~3, res);
}

#include "drivers/pci/pci.c"

