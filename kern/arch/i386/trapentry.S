
#include <segment.h>

.globl trapentry
trapentry:
	/*
	 * x86 uses a huge interrupt vector consisting of 256 entries
	 * to route interrupts.  The processor enters the i-th entry
	 * if an IRQ with number i occurs.
	 *
	 * Unfortunately, it's common practice that all entries merely
	 * pushes some arguments (e.g. interrupt number) before calling into
	 * a shared handler (here).  The huge vector therefore becomes such a
	 * burden that the entry code are usually generated automatically
	 * via a helper script (tools/arch/i386/vectors.pl).
	 *
	 * To make things worse, x86 processors always pushes CS, EIP and
	 * EFLAGS onto the stack whenever an interrupt occurs.  Also,
	 * depending on situation, the processor
	 * (1) pushes an error code onto stack under interrupt 8, 10, 11, 12,
	 *     13, 14, 17.  The helper script deals with this behavior by
	 *     manually inserting a dummy error code under other interrupts.
	 * (2) pushes ESP and SS onto stack when crossing rings (i.e.
	 *     switching processor mode).
	 */
	
	/* Push the selectors */
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs

	/* Push general registers */
	pushal

	/* Make sure the data segments are under kernel segments */
	movw	$KERNEL_DS, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs	/* KCPU? */
	movw	%ax, %gs	/* KCPU? */

	pushl	%esp
	call	trap_handler
	addl	$4, %esp

.globl	trapret
trapret:
	popal
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds
	addl	$0x8, %esp
	iret
