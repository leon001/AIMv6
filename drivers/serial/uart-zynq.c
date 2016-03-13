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

/* internal routines */

static void __uart_zynq_enable(uint32_t base)
{
	/* Enable TX and RX */
	write32(base + UART_OFFSET_CR, UART_CR_TX_EN | UART_CR_RX_EN);
}

static void __uart_zynq_disable(uint32_t base)
{
	/* Disable TX and RX */
	write32(base + UART_OFFSET_CR, UART_CR_TX_DIS | UART_CR_RX_DIS);
}

static void __uart_zynq_init(uint32_t base)
{
	/* Disable interrupts */
	write32(base + UART_OFFSET_IDR, UART_IXR_MASK);
	/* Disable TX and RX */
	__uart_zynq_disable(base);
	/* Reset TX and RX, Clear FIFO */
	write32(base + UART_OFFSET_CR, UART_CR_TXRST | UART_CR_RXRST);
	/* Clear Flags */
	write32(base + UART_OFFSET_ISR, UART_IXR_MASK);
	/* Mode Reset to Normal/8-N-1 */
	write32(base + UART_OFFSET_MR,
		UART_MR_CHMODE_NORM | UART_MR_CHARLEN_8_BIT |
		UART_MR_PARITY_NONE | UART_MR_STOPMODE_1_BIT);
	/* Trigger Reset */
	write32(base + UART_OFFSET_RXWM, UART_RXWM_RESET_VAL);
	write32(base + UART_OFFSET_TXWM, UART_TXWM_RESET_VAL);
	/* Disable RX timeout */
	write32(base + UART_OFFSET_RXTOUT, UART_RXTOUT_DISABLE);
	/* Reset baud rate generator and divider to genetate 115200 */
	write32(base + UART_OFFSET_BAUDGEN, 0x3E);
	write32(base + UART_OFFSET_BAUDDIV, 0x06);
	/* Set CR Value */
	write32(base + UART_OFFSET_CR,
		UART_CR_RX_DIS | UART_CR_TX_DIS | UART_CR_STOPBRK);
}

unsigned char __uart_zynq_getchar(uint32_t base)
{
	while (read32(base + UART_OFFSET_SR) & UART_SR_RXEMPTY);
	return read8(base + UART_OFFSET_FIFO);
}

void __uart_zynq_putchar(uint32_t base, unsigned char c)
{
	while (read32(base + UART_OFFSET_SR) & UART_SR_TXFULL);
	write8(base + UART_OFFSET_FIFO, c);
}

#ifdef RAW /* baremetal driver */

/* FIXME zedboard uses UART1 only */
#define UART_BASE	UART1_PHYSBASE

void uart_init(void)
{
	__uart_zynq_init(UART_BASE);
}

void uart_enable(void)
{
	__uart_zynq_enable(UART_BASE);
}

void uart_disable(void)
{
	__uart_zynq_disable(UART_BASE);
}

unsigned char uart_getchar(void)
{
	__uart_zynq_getchar(UART_BASE);
}

void uart_putchar(unsigned char c)
{
	__uart_zynq_putchar(UART_BASE, c);
}

#else /* not RAW, or kernel driver */

#if PRIMARY_CONSOLE == uart_zynq

/* FIXME zedboard uses UART1 only */
#define UART_BASE	UART1_PHYSBASE

void __weak early_console_init()
{
	__uart_zynq_init(UART_BASE);
	__uart_zynq_enable(UART_BASE);
}

static void early_console_putchar(unsigned char c)
{
	__uart_zynq_putchar(UART_BASE, c);
}

static void early_console_puts(const char *str)
{
	for (; *str != '\0'; ++str) {
		if (*str == '\n')
			__uart_zynq_putchar(UART_BASE, '\r');
		__uart_zynq_putchar(UART_BASE, (unsigned char)*str);
	}
}

#endif /* PRIMARY_CONSOLE */

#endif /* RAW */

