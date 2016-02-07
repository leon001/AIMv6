
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stddef.h>
#include <smp.h>
#include <drivers/serial/uart.h>

#define FWSTACKSIZE	(1 << FWSTACKORDER)

unsigned char fwstack[NR_CPUS][FWSTACKSIZE];

extern void slave_entry(void);

void bring_up_slaves(void)
{
	int i;
	for (i = 1; i < NR_CPUS; ++i)
		write32(msim_mailbox(i), (unsigned long)slave_entry);
}

void main(void)
{
	uart_puts("Hello world!\n");
	bring_up_slaves();
	for (;;)
		/* nothing */;
}

void slave_main(void)
{
	uart_puts("Hello from slave!\n");
	for (;;)
		/* nothing */;
}
