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

__noinline unsigned long get_pc(void);

#endif
