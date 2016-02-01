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

#include <uart-zynq.h>
#include <uart-zynq-hw.h>

#include <io.h>

#ifdef RAW /* baremetal driver */

/* FIXME zedboard uses UART1 only */
#define UART_BASE	UART1_PHYSBASE

void uart_init(void)
{
	/* Disable interrupts */
	write32(UART_BASE + UART_OFFSET_IDR, UART_IXR_MASK);
	/* Disable TX and RX */
	uart_disable();
	/* Reset TX and RX, Clear FIFO */
	write32(UART_BASE + UART_OFFSET_CR, UART_CR_TXRST | UART_CR_RXRST);
	/* Clear Flags */
	write32(UART_BASE + UART_OFFSET_ISR, UART_IXR_MASK);
	/* Mode Reset to Normal/8-N-1 */
	write32(UART_BASE + UART_OFFSET_MR,
		UART_MR_CHMODE_NORM | UART_MR_CHARLEN_8_BIT |
		UART_MR_PARITY_NONE | UART_MR_STOPMODE_1_BIT);
	/* Trigger Reset */
	write32(UART_BASE + UART_OFFSET_RXWM, UART_RXWM_RESET_VAL);
	write32(UART_BASE + UART_OFFSET_TXWM, UART_TXWM_RESET_VAL);
	/* Disable RX timeout */
	write32(UART_BASE + UART_OFFSET_RXTOUT, UART_RXTOUT_DISABLE);
	/* Reset baud rate generator and divider to genetate 115200 */
	write32(UART_BASE + UART_OFFSET_BAUDGEN, 0x3E);
	write32(UART_BASE + UART_OFFSET_BAUDDIV, 0x06);
	/* Set CR Value */
	write32(UART_BASE + UART_OFFSET_CR,
		UART_CR_RX_DIS | UART_CR_TX_DIS | UART_CR_STOPBRK);
}

#else /* not RAW, or kernel driver */

#endif /* RAW */

