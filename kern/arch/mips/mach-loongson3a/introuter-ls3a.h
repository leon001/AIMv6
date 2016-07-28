
#ifndef _DRIVERS_INTR_INTROUTER_LS3A_H
#define _DRIVERS_INTR_INTROUTER_LS3A_H

/*
 * Interrupt router destination fields.
 *
 * Here, I'm only defining the fields we will be using in AIMv6, that is,
 * LPC interrupt, and HT Bus #1 interrupts.
 *
 * For the complete set, see Loongson 3A User Manual Part 1, Section 7.
 */
#define IR_SYSINTS	32	/* # interrupts the controller handles */
#define IR_LPC		0xa
#define IR_HT1_INTx(x)	(0x18 + (x))

#define IR_CPU(x)	(1 << (x))
#define IR_IP(x)	(1 << (2 + (x)))

#define IR_INTISR	0x20
#define IR_INTEN	0x24
#define IR_INTENSET	0x28
#define IR_INTENCLR	0x2c
#define IR_INTEDGE	0x38

#define IR_COREx_INTISR(x)	(0x40 + 8 * (x))

#endif

