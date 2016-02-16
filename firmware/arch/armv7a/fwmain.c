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

/* from kernel */
#include <sys/types.h>

/* from drivers */
#include <drivers/serial/uart.h>
#include <drivers/serial/uart-zynq.h>
#include <drivers/timer/timer.h>
#include <drivers/timer/timer-a9.h>
#include <drivers/sd/sd.h>
#include <drivers/sd/sd-zynq.h>

void sleep(uint32_t s)
{
	uint64_t time, time1;
	time = timer_read();
	time += gt_get_tps() * s;
	do {
		time1 = timer_read();
	} while (time1 < time);
}

void usleep(uint32_t us)
{
	uint64_t time, time1;
	time = timer_read();
	time += gt_get_tpus() * us;
	do {
		time1 = timer_read();
	} while (time1 < time);
}

__attribute__ ((noreturn))
void fw_main(void)
{
	int ret;
	volatile uint8_t *mbr = (void *)0x100000; /* THIS IS NOT A NULL! */
	void (*mbr_entry)() = (void *)mbr;

	/* Wait for UART fifo to flush */
	sleep(1);
	
	/* Initialize and enable UART */
	uart_init();
	uart_enable();
	uart_puts("FW: Hello!\r\n");

	/* Initialize SDHCI interface */
	sd_init();
	uart_puts("FW: SD Controller initialized.\r\n");

	/* Initialize SD card */
	ret = sd_init_card();
	if (ret == 0)
		uart_puts("FW: SD Card initialized.\r\n");
	else if (ret == 1)
		uart_puts("FW: SDHC Card initialized.\r\n");
	else {
		uart_puts("FW: Card initialization failed.\r\n");
		goto spin;
	}

	/*
	 * We don't turn on SCU now. The kernel should do this.
	 * This CANNOT be done here. DDR in 0x0 to 0xFFFFF is only accessible
	 * to processor cores, not the DMA controller.
	 * See Xilinx UG585, Table 4-1 for details.
	 */

	/* Read MBR */
	ret = sd_read((u32)mbr, 1, 0);
	if (ret == 0) uart_puts("FW: Card read OK.\r\n");
	else {
		uart_puts("FW: Card read failed.\r\n");
		goto spin;
	}

	/* Check MBR */
	if (mbr[510] == 0x55 && mbr[511] == 0xAA) {
		uart_puts("FW: MBR valid.\r\n");
		mbr_entry();
	} else uart_puts("FW: MBR not valid.\r\n");

spin:
	while (1);
}
