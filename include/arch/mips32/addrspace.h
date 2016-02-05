
#ifndef _MIPS32_ADDRSPACE_H
#define _MIPS32_ADDRSPACE_H

#define USEG		0x00000000
#define KSEG0		0x80000000
#define KSEG1		0xa0000000
#define KSSEG		0xc0000000
#define KSEG3		0xe0000000

#ifdef TO_CAC
#undef TO_CAC
#endif

#ifdef TO_UNCAC
#undef TO_UNCAC
#endif

#define TO_CAC(x)	(KSEG0 + (x))
#define TO_UNCAC(x)	(KSEG1 + (x))

#endif
