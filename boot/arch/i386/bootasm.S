
/*
 * LICENSE NOTES:
 * Since most i386 code comes from MIT xv6, I wonder how we should put the
 * license information...
 */

/*
 * COMMENT NOTES FOR CONTRIBUTORS:
 * Provide as much information as possible in x86 code, because x86
 * architecture is awfully complicated and the reasoning of many designs are
 * not so apparent.  As a result, novices reading x86 code would be easily
 * confused.
 */

/*
 * Yet another note:
 * The code is directly taken from bootasm.S in xv6.
 */

#include <segment.h>
#include <processor-flags.h>

/*
 * Just before entering into the boot loader, i386 runs in 16-bit "real" mode
 * for backward compatibility (i.e. to be able to run creaky stuff like
 * MS-DOS).  Therefore, the very first portion of code should be in 16-bit
 * mode.
 *
 * As an IBM PC/AT standard, the BIOS loads MBR code into physical address
 * 0x7c00.
 */
.code16
.globl	start
start:
	/* Disable interrupts */
	cli

	/* ax <= 0 */
	xorw	%ax, %ax
	/*
	 * Clean data segment, stack segment, and extra segment.
	 *
	 * A memory segment is a continuous chunk of memory.  A x86 segment
	 * register selects a memory segment for a dedicated use.
	 *
	 * i386 has three common segment registers, each for Code Segment (CS),
	 * Data Segment (DS), and Stack Segment (SS).
	 * (1) The instruction pointer (IP) points to segment determined by CS.
	 * (2) The general registers (AX, BX, CX, DX) and index registers
	 *     (SI, DI) points to segment determined by DS when referencing
	 *     memory.
	 * (3) The stack pointer (SP) and stack base pointer (BP) points to
	 *     segment determined by SS.
	 *
	 * Back to days before 32-bit, *segments* are introduced by Intel to
	 * enable the processors to access addresses more than 16-bit, since
	 * at that time 20-bit (1MB) memory are available.  The memory is
	 * usually computed by
	 *     <memory address> = <segment> << 4 + <offset>
	 * and takes the following notation:
	 *     <segment>:<offset>
	 * For example, address F000:FFFF = FFFF:000F = F800:7FFF, all pointing
	 * to memory address 0xFFFFF.
	 * Another example: F800:7FFF + 1 = F800:8000, which points to memory
	 * address 0x100000.  On a machine with 20-bit memory, it points to 0
	 * instead.
	 *
	 * Code segment, data segment and stack segment are responsible of the
	 * "segment" part, while other registers and immediates take the
	 * "offset" part.
	 * E.g.  We (assume that you) are all familiar with the fact that
	 * (1) IP (EIP if 32-bit) points to the address of current instruction.
	 * (2) SP (ESP if 32-bit) points to the address of stack top.
	 * However, with segmentation, the current instruction address is
	 * in fact CS:IP, while the stack top address is actually SS:SP.
	 *
	 * In 32-bit "protected" mode, the segment registers also controls
	 * which memory chunk various registers will be referencing.  However,
	 * the form of segment registers are different.
	 */
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es

	/*
	 * The following code deals with A20 line.
	 *
	 * At the time of 20-bit memory, some programs exploited the ability to
	 * "wrap around" the memory (consider F800:8000 on a 1MB memory machine
	 * again) to work.
	 *
	 * While IBM was trying to shift to the more modern 80286 chip (which
	 * introduced protected mode and was able to address larger memory),
	 * the developers there found a bug that when operating in real mode
	 * to allow old 8086 programs to work, the 21st address line (or A20)
	 * was not always zero.  Therefore, programs relying on the "wrapping"
	 * behavior were broken.
	 *
	 * I guess the number of such programs are so large that IBM eventually
	 * decided to hack around this bug by introducing a gate logic (called
	 * A20 gate) to control A20 (because IBM couldn't do anything to the
	 * chip itself).  BIOS turns A20 gate off before transferring control
	 * to MBR to allow old programs to work in real mode correctly.
	 * Consequently, new programs have to enable A20 gate in order to
	 * correctly address higher memory.
	 *
	 * Strangely, A20 gate is at first connected to Intel 8042 keyboard
	 * controller.  Nowadays it is usually controlled by PS/2 0x92 port
	 * and AT keyboard controller (with port 0x60 and 0x64).
	 *
	 * We can see that A20 gate is no good.
	 *
	 * As we are not coding "old" programs, we will enable A20 gate here.
	 *
	 * More about A20 gate:
	 * https://en.wikipedia.org/wiki/A20_line
	 * A20 - a pain from the past:
	 * https://www.win.tue.nl/~aeb/linux/kbd/A20.html
	 * Keyboard scancodes:
	 * https://www.win.tue.nl/~aeb/linux/kbd/scancodes-11.html
	 * Intel 8042 keyboard controller:
	 * http://wiki.osdev.org/%228042%22_PS/2_Controller
	 */
	/* Poll to wait for not busy */
seta20.1:
	inb	$0x64, %al
	testb	$0x2, %al
	jnz	seta20.1

	/* Write d1 to 0x64, that is, "write to controller output port" */
	movb	$0xd1, %al
	outb	%al, $0x64

	/* Again wait for not busy */
seta20.2:
	inb	$0x64, %al
	testb	$0x2, %al
	jnz	seta20.2

	/* Write df to 0x60, that is, "enable A20" */
	movb	$0xdf, %al
	outb	%al, $0x60

	/*
	 * We would like to switch from real mode to protected mode then.
	 * But before that, we need to set up a Global Descriptor Table (GDT),
	 * to "redefine" the meaning of segments.
	 *
	 * In protected mode, the segment registers (CS, DS, SS, etc.) contains
	 * *segment selectors*.
	 *
	 * A segment selector has the following form:
	 * (<index> << 3) + (<table indicator> << 2) + (<privilege level>)
	 * where
	 *     - <index> is the index to global/local descriptor table
	 *     - <table indicator> should be 1 if reading local descriptor table.
	 *     - <privilege level> is the privilege level (user or kernel) the
	 *       registers will be referencing under.
	 *
	 * The local descriptor tables are not used in our operating system.
	 *
	 * For now, we are going to use a GDT stored at @gdtdesc (discussed
	 * below).
	 *
	 * See also:
	 * Global Descriptor Table:
	 * http://wiki.osdev.org/Global_descriptor_table
	 */
	lgdt	gdtdesc

	/* Time to turn on protected mode. */
	movl	%cr0, %eax
	orl	$CR0_PE, %eax
	movl	%eax, %cr0

	/* A far jump modifies both CS and EIP registers. */
	ljmp	$(SEG_KCODE << 3), $start32

	/* Now we're executing 32-bit code */
.start32
	/* Set up data segment *selectors* */
	movw	$(SEG_KDATA << 3), %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	/* and zero out unused segments */
	xorw	%ax, %ax
	movw	%ax, %fs
	movw	%ax, %gs

	/*
	 * Set up stack pointer.  All C programs requires a stack to run.
	 * For now we are going to put the stack top right before the address of
	 * bootloader code.  This is OK; there are not that many local
	 * variables and nested calls in our bootloader, and we are going to
	 * reset the stack pointer in kernel anyway.
	 */
	movl	$start, %esp

	/* Finally, jump to C code. */
	call	bootmain

	/*
	 * If bootmain returns (it shouldn't), loop here.
	 *
	 * Either you are extremely unlucky or you emulator/toolchain/other
	 * stuff is broken if you got stuck here.
	 */
die:	jmp	die

	/*
	 * The following contains the GDT the bootloader will use.
	 * A GDT is a list of segment descriptor.  For the form of a
	 * segment descriptor, see include/arch/i386/segment.h.
	 *
	 * Here, we are constructing two segments.
	 *
	 * Segment #1 is an executable, readable segment, covering addresses
	 * from 0x0 + 0x0 to 0x0 + 0xffffffff.
	 * Segment #2 is a readable, writable, non-executable segment, also
	 * covering the whole address space.
	 */
.p2align 2			/* Force 4 byte alignment */
gdt:
	SEG_NULLASM
	SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)
	SEG_ASM(STA_W, 0x0, 0xffffffff)

gdtdesc:
	.word	(gdtdesc - gdt - 1)
	.long	gdt
