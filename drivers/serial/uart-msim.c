
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <uart-msim.h>
#include <io.h>

#ifdef RAW
void uart_init(void)
{
	/* nothing */
}

void uart_enable(void)
{
	/* nothing */
}

void uart_disable(void)
{
	/* nothing */
}
#else
#endif

unsigned char uart_getbyte(void)
{
	unsigned char b;
	while (b = read8(MSIM_UART_INPUT))
		/* nothing */;
	return b;
}

void uart_putbyte(unsigned char byte)
{
	write8(MSIM_UART_OUTPUT, byte);
}
