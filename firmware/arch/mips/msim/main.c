
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stddef.h>
#include <smp.h>
#include <drivers/serial/uart.h>

#define FWSTACKSIZE	(1 << FWSTACKORDER)

unsigned char fwstack[NR_CPUS][FWSTACKSIZE];

void main(void)
{
	uart_puts("Hello world!\n");
	for (;;)
		/* nothing */;
}

