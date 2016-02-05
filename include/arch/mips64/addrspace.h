
#ifndef _MIPS64_ADDRSPACE_H
#define _MIPS64_ADDRSPACE_H

#include <../mips32/addrspace.h>

#define CKSEG0		KSEG0
#define CKSEG1		KSEG1
#define CKSSEG		KSSEG
#define CKSEG3		KSEG3

#define XSSEG		0x4000000000000000
#define XKPHY		0x8000000000000000
#define XKSEG		0xc000000000000000

#define IO_CAC_BASE	(XKPHY + 0x1800000000000000)
#define IO_UNCAC_BASE	(XKPHY + 0x1000000000000000)

#ifdef TO_CAC
#undef TO_CAC
#endif
#ifdef TO_UNCAC
#undef TO_UNCAC
#endif

#define TO_CAC(x)	(IO_CAC_BASE + (x))
#define TO_UNCAC(x)	(IO_UNCAC_BASE + (x))

#endif
