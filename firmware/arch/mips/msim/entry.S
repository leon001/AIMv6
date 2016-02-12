
#include <asm.h>
#include <regdef.h>
#include <cp0regdef.h>
#include <smp.h>
#include <addrspace.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

.section .text.reset

BEGIN(reset)
	/* Turn off all interrupts, and go into kernel mode */
	MFC032	a0, CP0_STATUS
	or	a0, ST_EXCM
	xor	a0, ST_EXCM
	MTC032	a0, CP0_STATUS
	cpuid	a1, a2
	bnez	a1, 2f
	j	master_entry
2:	j	slave_hangup
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

BEGIN(slave_hangup)
	/* a1 already contains cpuid */
	SLL	t0, a1, MSIM_ORDER_MAILBOX_ORDER
	STORE	zero, TO_UNCAC(MSIM_ORDER_MAILBOX_BASE)(t0)
1:	LOAD	t1, TO_UNCAC(MSIM_ORDER_MAILBOX_BASE)(t0)
	beqz	t1, 1b
	jr	t1
END(slave_hangup)

BEGIN(master_entry)
reloc_data:
	LI	s0, __rodata_end__
	LI	s1, __data_begin__
	LI	s2, __data_end__
1:	bge	s1, s2, 9f
	lw	t0, (s0)
	sw	t0, (s1)
	ADDU	s0, 4
	ADDU	s1, 4
	b	1b
9:
init_bss:
	LI	s1, __bss_begin__
	LI	s2, __bss_end__
1:	bge	s1, s2, 9f
	sw	zero, (s1)
	ADDU	s1, 4
	b	1b
9:
init_master_stack:
	ADDU	a1, 1
	SLL	a3, a1, FWSTACKORDER
	LA	sp, fwstack(a3)
main_entry:
	j	main
END(master_entry)
