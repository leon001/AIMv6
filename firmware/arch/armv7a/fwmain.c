/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
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

#include <drivers/serial/uart.h>
#include <drivers/timer/timer.h>

/* FIXME */
#include <drivers/serial/uart-zynq.h>
#include <drivers/timer/timer-a9.h>

void sleep(uint32_t s)
{
	uint64_t time, time1;
	time = timer_read();
	time += gt_get_tps() * s;
	do {
		time1 = timer_read();
	} while (time1 < time);
}

__attribute__ ((noreturn))
void fw_main(void)
{
	/* Wait for UART fifo to flush */
	sleep(1);
	
	/* Initialize and enable UART */
	uart_init();
	uart_enable();
	uart_puts("FW: Hello!\r\n");

	while (1);
}
