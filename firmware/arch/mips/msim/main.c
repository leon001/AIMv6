
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libc/sys/types.h>
#include <libc/stddef.h>
#include <smp.h>
#include <drivers/serial/uart.h>
#include <drivers/block/msim-ddisk.h>

#define FWSTACKSIZE	(1 << FWSTACKORDER)

unsigned char fwstack[NR_CPUS][FWSTACKSIZE];

void main(void)
{
	char buf[512];
	uart_puts("Hello world!\n");
	msim_dd_init(MSIM_DISK_PHYSADDR);
	msim_dd_read_sector(MSIM_DISK_PHYSADDR, 0, buf, true);
	uart_puts(buf);
	for (;;)
		/* nothing */;
}

