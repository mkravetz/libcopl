/*
 * BMA test to libcopl compatibility layer
 */
#ifndef _test_perf_bma2libcopl_
#define _test_perf_bma2libcopl_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop.h>

#include "config.h"

#define MEMPRINT(name,length) { \
    long i; \
    for (i = 0; i < length; i++) { \
        if ((i % 16) == 0) { \
            if (i) \
                printf("\n"); \
            printf("%08lx: ", ((unsigned long)name + i));  \
        } \
        printf("%02x", *((char *)name + i)); \
        if (((i + 1) % 2) == 0) \
            printf(" "); \
    } \
    printf("\n"); \
}

#define CONFIG_CACHE_BLOCK_SIZE	128

#define SPRN_TB		0x10c
#define SPRN_TBU	0x10d

#define mfspr(rn) ({unsigned long rval; \
			asm volatile("mfspr %[r]," __stringify(rn)	\
			: [r] "=r" (rval)); rval;})

static inline uint64_t mftb(void)
{
	uint64_t tb;

#ifdef __powerpc64__
	tb = mfspr(SPRN_TB);
#else
	ulong hi;
	ulong lo;
	ulong check;

	do {
		hi = mfspr(SPRN_TBU);
		lo = mfspr(SPRN_TB);
		check = mfspr(SPRN_TBU);
	} while (hi != check);

	tb = hi;
	tb <<= 32;
	tb |= lo;
#endif
	return tb;
}

static inline void isync(void) {
	asm volatile ("isync");
}

#if DEBUG_ON
void tmesg(const char *fmt, ...)
{
	va_list ap;

	puts("  ");

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);

	puts("...");
}

int passfail(int fail, const char *fmt, ...)
{
	if (fail) {
		if (fmt) {
			va_list ap;

			puts("FAIL: ");

			va_start(ap, fmt);
			vprintf(fmt, ap);
			va_end(ap);
			puts("\n");
		} else {
			puts("FAIL\n");
		}
	} else {
		puts("pass\n");
	}

	return fail;
}
#else
void tmesg(const char *fmt, ...) { }
int passfail(int fail, const char *fmt, ...) { return 0; }
#endif

#define ALIGN_UP(a,s)   (((a) + ((s) - 1)) & (~((s) - 1)))

#define FLAG_ZLIB	1
#define FLAG_GZIP	2

#define FHCRC           0x02
#define FEXTRA          0x04
#define FNAME           0x08
#define FCOMMENT        0x10

void *decapitate(const char *addr, unsigned int format)
{
	unsigned char flag;
	const char *ptr;
	unsigned int xlen;

	ptr = NULL;
	switch (format) {
	case FLAG_ZLIB:
		ptr = addr;
		ptr++;
		flag = *ptr;
		ptr++;

		if (flag & 0x20)
			ptr += 4;
		break;
	case FLAG_GZIP:
		ptr = addr;
		if ((*ptr != 0x1f)
		    || (*(ptr + 1) != 0x8b)
		    || (*(ptr + 2) != 0x08))
			return NULL;
		ptr += 3;

		flag = *ptr;
		ptr += 7;

		if (flag & FEXTRA) {
			xlen = *(ptr + 1) & (*ptr << 8);
			ptr += (2 + xlen);
		}

		if (flag & FNAME) {
			while (*ptr != '\0')
				ptr++;
			ptr++;
		}

		if (flag & FCOMMENT) {
			while (*ptr != '\0')
				ptr++;
			ptr++;
		}

		if (flag & FHCRC)
			ptr += 2;
		break;
	}

	return (void *)(unsigned long)ptr;
}

/********************************************************************************/

extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);

void cop_init(struct cop_ctx *ctx, int type) {
	cop_open_bind_local(ctx, type);
}

/* dmalloc - dumb memory allocator
 * its a quick and dirty posix_memalign and 
 * pbic_map call
 */
void *dmalloc(struct cop_ctx *ctx, size_t align, size_t size) {
	void *mem;

	if (posix_memalign(&mem, align, size))
		return NULL;

	if (cop_pbic_map(ctx, mem, size, COP_MAP_ALL_FLAGS))
		return NULL;

	return mem;
}

void dfree(struct cop_ctx *ctx, void *mem, size_t size)
{
	cop_pbic_unmap(ctx, mem, size, COP_MAP_ALL_FLAGS);
	//free(mem);
}

#endif
