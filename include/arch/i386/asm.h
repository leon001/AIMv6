/*
 * LICENSE NOTES:
 * Since most i386 code comes from MIT xv6, I wonder how we should put the
 * license information...
 */

#ifndef _ASM_ASM_H
#define _ASM_ASM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

/*
 * C wrappers for i386 instructions.
 */
static inline uint8_t
inb(unsigned short port)
{
	unsigned char data;
	asm volatile (
		"in %1, %0"
		: "=a"(data)
		: "d" (port)
	);
	return data;
}

static inline void
outb(unsigned short port, uint8_t data)
{
	asm volatile (
		"out %0, %1"
		: /* no output */
		: "a"(data), "d"(port)
	);
}

static inline uint16_t
inw(unsigned short port)
{
	unsigned char data;
	asm volatile (
		"in %1, %0"
		: "=a"(data)
		: "d" (port)
	);
	return data;
}

static inline void
outw(unsigned short port, uint16_t data)
{
	asm volatile (
		"out %0, %1"
		: /* no output */
		: "a"(data), "d"(port)
	);
}

static inline uint32_t
ind(unsigned short port)
{
	unsigned char data;
	asm volatile (
		"in %1, %0"
		: "=a"(data)
		: "d" (port)
	);
	return data;
}

static inline void
outd(unsigned short port, uint32_t data)
{
	asm volatile (
		"out %0, %1"
		: /* no output */
		: "a"(data), "d"(port)
	);
}

static inline void
insl(unsigned short port, void *addr, size_t cnt)
{
	asm volatile (
		"cld;"
		"rep insl"
		: "=D"(addr), "=c"(cnt)
		: "d"(port), "0"(addr), "1"(cnt)
		: "memory", "cc"
	);
}

static inline void
outsl(unsigned short port, const void *addr, size_t cnt)
{
	asm volatile (
		"cld;"
		"rep outsl"
		: "=S"(addr), "=c"(cnt)
		: "d"(port), "0"(addr), "1"(cnt)
		: "cc"
	);
}

static inline void
stosb(void *addr, int data, size_t cnt)
{
	asm volatile (
		"cld;"
		"rep stosb"
		: "=D"(addr), "=c"(cnt)
		: "0"(addr), "1"(cnt), "a"(data)
		: "memory", "cc"
	);
}

#include <arch-trap.h>
static inline void
lidt(struct idtentry *idt, uint32_t size)
{
#pragma pack(1)
	volatile struct {
		uint16_t size;
		uint32_t addr;
	} idtdesc;
#pragma pack()
	idtdesc.size = (uint16_t)(size - 1);
	idtdesc.addr = (uint32_t)idt;

	asm volatile (
		"lidt	(%0)"
		: /* no output */
		: "r"(&idtdesc)
	);
}

#include <segment.h>
static inline void
lgdt(struct segdesc *gdt, uint32_t size)
{
#pragma pack(1)
	volatile struct {
		uint16_t size;
		uint32_t addr;
	} gdtdesc;
#pragma pack()
	gdtdesc.size = (uint16_t)(size - 1);
	gdtdesc.addr = (uint32_t)gdt;

	asm volatile (
		"	lgdt	(%0);"
		: /* no output */
		: "r"(&gdtdesc)
	);
}

__noinline unsigned long get_pc(void);

#endif
