
#ifndef _DRIVERS_SERIAL_UART_MSIM_H
#define _DRIVERS_SERIAL_UART_MSIM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#error "Did you ran ./configure?"
#endif

/*
 * Although MSIM take the notion of a keyboard and a line printer.  In our code
 * we treat the keyboard and line printer as a serial.
 */
#define MSIM_UART_OUTPUT	MSIM_LP_PHYSADDR
#define MSIM_UART_INPUT		MSIM_KBD_PHYSADDR

#endif
