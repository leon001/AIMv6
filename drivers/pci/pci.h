
#ifndef _DRIVERS_PCI_PCI_H
#define _DRIVERS_PCI_PCI_H

#include <drivers/pci/pci_regs.h>

#define make_pci_tag(bus, dev, func) \
	(((bus) << 16) | ((dev) << 11) | ((func) << 8))

#endif
