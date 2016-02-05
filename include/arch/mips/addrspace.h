
#ifndef _ADDRSPACE_H
#define _ADDRSPACE_H

/*
 * Note: only TO_CAC and TO_UNCAC should be used.
 * You probably won't ever need these constants.
 */

#define USEG		0x00000000
#define KSEG0		0x80000000
#define KSEG1		0xa0000000
#define KSSEG		0xc0000000
#define KSEG3		0xe0000000

#ifdef USE_MIPS32
#define TO_CAC(x)	(KSEG0 + (x))
#define TO_UNCAC(x)	(KSEG1 + (x))
#else

#define CKSEG0		KSEG0
#define CKSEG1		KSEG1
#define CKSSEG		KSSEG
#define CKSEG3		KSEG3

#define XSSEG		0x4000000000000000
#define XKPHY		0x8000000000000000
#define XKSEG		0xc000000000000000

#define IO_CAC_BASE	(XKPHY + 0x1800000000000000)
#define IO_UNCAC_BASE	(XKPHY + 0x1000000000000000)

#define TO_CAC(x)	(KSEG0 + (x))
#define TO_UNCAC(x)	(KSEG1 + (x))

#endif

#endif
