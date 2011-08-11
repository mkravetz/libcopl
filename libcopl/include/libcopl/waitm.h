/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   IBM Corp, All Rights Reserved

   Taken from BLU
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_WAITM_H
#define _LIB_LIBCOPL_WAITM_H

#include <sys/types.h>
#include <libcopl/cop.h>

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef __stringify
#define __stringify_1(x)        #x
#define __stringify(x)          __stringify_1(x)
#endif

#define _WAITM(wc) .long (0x7c00007c | (((wc) & 0x3) << (31 - 10)))
#define WAITM _WAITM(0)
#define WAITMRSV _WAITM(1)

static inline void lwsync(void)
{
	asm volatile ("sync 1");
}

static inline void clobber(void)
{
        asm volatile ("":::"memory");
}

static inline void waitm(void)
{
	asm volatile (__stringify(WAITM));
}

static inline void waitrsv(void)
{
	asm volatile (__stringify(WAITMRSV));
}

/**
 * \defgroup waitm Routines built around waitm instruction
 */
#define _wait_on_mem(LARX, CMP) \
	"1: #LOOP						\n"	\
	"	"LARX"	%[val],0,%[ptr]	# val = (*ptr)	[reserve]\n"	\
	"	"CMP"	%[val],%[orig]	# val = (*ptr)	[reserve]\n"	\
	"	bne-	2f	# if (val != orig) goto OUT	\n"	\
	"	"__stringify(WAITMRSV)"	# wait on reservation	\n"	\
	"	b	1b	# goto LOOP			\n"	\
	"2: #OUT						\n"

/**
 * @brief Wait on a 32 bit memory location to change
 * \ingroup waitm
 *
 * This routine waits for a change (write) to a 32 bit memory location.  
 * It takes as arguments a pointer to the 32 bit memory and the original
 * value at the memory location.  It returns the modified value.
 * 
 * @param ptr pointer to 32 bit memory location
 * @param orig original contents at 32 bit memory location
 * @return new value at 32 bit memory location
 */
static inline uint32_t wait_on_mem32(volatile uint32_t *ptr, uint32_t orig)
{
	uint32_t val;

	/* *INDENT-OFF* */
	asm volatile (
		"# wait_on_mem32					\n"
		_wait_on_mem("lwarx", "cmpw")
		"# end wait_on_mem32					\n"
		: [val] "=&r" (val) 
		: [ptr] "r" (ptr), [orig] "r" (orig)
		: "cc", "cr0", "memory");
	/* *INDENT-ON* */

	/* return the loaded value */
	return val;
}

/**
 * @brief Wait on a 64 bit memory location to change
 * \ingroup waitm
 *
 * This routine waits for a change (write) to a 64 bit memory location.  
 * It takes as arguments a pointer to the 64 bit memory and the original
 * value at the memory location.  It returns the modified value.
 * 
 * @param ptr pointer to 64 bit memory location
 * @param orig original contents at 64 bit memory location
 * @return new value at 64 bit memory location
 */
static inline uint64_t wait_on_mem64(volatile uint64_t *ptr, uint64_t orig)
{
	uint64_t val;

	/* *INDENT-OFF* */
	asm volatile (
		"# wait_on_mem64					\n"
		_wait_on_mem("ldarx", "cmpd")
		"# end wait_on_mem64					\n"
		: [val] "=&r" (val)
		: [ptr] "r" (ptr), [orig] "r" (orig)
		: "cc", "cr0", "memory");
	/* *INDENT-ON* */

	/* return the loaded value */
	return val;
}

#ifdef __powerpc64__
#define wait_on_meml wait_on_mem64
#else
#define wait_on_meml wait_on_mem32
#endif

#ifdef __cplusplus
}
#endif

#endif	/* _LIB_LIBCOPL_WAITM_H */
