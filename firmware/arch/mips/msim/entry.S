
#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>
#include <smp.h>

#define FIRMWARE_STACK_SIZE	0x1000

.section .text.reset

BEGIN(reset)
	/* Turn off all interrupts, and go into kernel mode */
	MFC032	a0, CP0_STATUS
	or	a0, ST_EXCM
	xor	a0, ST_EXCM
	MTC032	a0, CP0_STATUS
	cpuid	a1, a2
	bnez	a1, slave_pool
	li	sp, 0x80000000 + FIRMWARE_STACK_SIZE
	j	main
END(reset)

.text

	.align	9		/* 0xbfc00200 */
BEGIN(__tlbfill)
	j	__exception_entry
END(__tlbfill)

	.align	8		/* 0xbfc00300 */
BEGIN(__cacheerror)
	j	__exception_entry
END(__cacheerror)

	.align	7		/* 0xbfc00380 */
BEGIN(__exception_entry)
	j	__exception_entry
END(__exception_entry)

slave_pool:
	j	slave_wait
