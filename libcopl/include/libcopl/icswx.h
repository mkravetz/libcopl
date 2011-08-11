/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   IBM Corp, All Rights Reserved

   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

/**
 * @file icswx.h
 * @brief Low-level interface to PowerEN @c icswx instruction
 *
 * The PowerEN processor has access to a number of coprocessors, and its
 * general-purpose CPU cores have a special instruction, @c icswx, to
 * access that.  This header file provides a low-level icswx() wrapper
 * function to invoke that instruction.
 * \defgroup icswx Coprocessor invocation via icswx instruction
 */

#ifndef _LIB_LIBCOPL_ICSWX_H
#define _LIB_LIBCOPL_ICSWX_H

#include <errno.h>
#include <time.h>
#include <libcopl/cop.h>
#include <libcopl/waitm.h>

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef __stringify
/**
 * @brief Convert a macro parameter to a string (without expansion).
 * @param x  Thing to convert to string
 * @return   @a x as a legal C string
 */
#define __stringify_1(x)        #x

/**
 * @brief Convert a macro parameter to a string (with expansion).
 * @param x  Thing (or macro) to convert to string
 * @return   @a x (macro-expanded) as a legal C string
 */
#define __stringify(x)          __stringify_1(x)
#endif

/**
 * @brief Emit the @c icswx instruction.
 *
 * This generally needs to appear inside an @c asm statement.  This is
 * a PowerEN-specific instruction.  If @a RA is 0 then the coprocessor
 * request block (CRB) is at the address in register @a RB; otherwise
 * the CRB is at the offset in register @a RA from the address in register
 * @a RB; the CRB address this calculates must be aligned on a 128-byte
 * boundary.  The register @a RS contains a 6-bit coprocessor type in bits
 * 42-47 and a 16-bit coprocessor directive in bits 48-63 (bit 63 is the
 * least-significant bit).  If @a Rc is non-zero, use the conditional
 * form of the @c icswx. instruction.
 *
 * @param RS  Register number containing CT/CD
 * @param RA  Register number holding byte offset from @a RB, or 0 for no offset
 * @param RB  Register number holding base of CRB
 * @param Rc  If non-zero, set CR0 based on success/failure
 */
#define ICSWX(RS,RA,RB,Rc) .long ((31 << (31 - 5))			| \
				  (((RS) & 0x1f) << (31 - 10))		| \
				  (((RA) & 0x1f) << (31 - 15))		| \
				  (((RB) & 0x1f) << (31 - 20))		| \
				  (406 << (31 - 30))			| \
				  ((Rc) & 1))

/**
 * @brief Generate the @c icswx. instruction for a specific CRB.
 * \ingroup icswx
 *
 * This includes an @c asm block to generate the ::ICSWX() instruction.
 * @a rs has the packed coprocessor type and directive as described for
 * the instruction.  This clobbers CR0, but most C code won't care.
 * The value in CR0 is interpreted for the return value: this function
 * returns @c -EAGAIN if CR0 bit 1 ("busy") is set, and @c -ENODEV if
 * CR0 bit 2 ("reject") is set.
 *
 * @param rs   Packed coprocessor command word with CD/CT
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @return     0 on success, negative ::errno value on failure
 */
static inline int icswx_raw(uint32_t rs, void *crb)
{
	int rc;

	/* *INDENT-OFF* */
	asm volatile(
		"# icswx						\n"
		"	lwsync	# Push the CRB to the cache		\n"
		"	li %[rc],0					\n"
		"	"__stringify(ICSWX(%[rs], 0, %[ra], 1))"	\n"
		"	blt+ 2f		# success			\n"
		"	li %[rc],%[eagain]				\n"
#ifndef DD2_CHIP_ERRATA
		"	bgt- 2f		# eagain			\n"
		"	li %[rc],%[enodev]				\n"
#endif
		"2:							\n"
		: [rc] "=&r" (rc), "=m" (*(uint32_t *)crb)
		: [rs] "r" (rs), [ra] "r" (crb),
		  [eagain] "I" (-EAGAIN), [enodev] "I" (-ENODEV)
		: "cr0");
	/* *INDENT-ON* */

	return rc;
}


/**
 * @brief Identicle to @c icswx_raw() above, but without the @c lwsync
 * \ingroup icswx
 *
 * This function is functionally equivalent to icswx_raw(), but does not
 * issue the @c lwsync instruction before the @c icswx.  It is desirable to
 * perform the lwsync if there has been any modification to the CRB or
 * coprocessor data.  This is the normal case when issuing an icswx.
 * However, in special circumstances such as request retry we know the
 * CRB/data has not been changed since the last icswx invocation.  Hence,
 * the lwsync is unnecessary.
 *
 * @param rs   Packed coprocessor command word with CD/CT
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @return     0 on success, negative ::errno value on failure
 */
static inline int icswx_raw_nosync(uint32_t rs, void *crb)
{
	int rc;

	/* *INDENT-OFF* */
	asm volatile(
		"# icswx						\n"
		"	li %[rc],0					\n"
		"	"__stringify(ICSWX(%[rs], 0, %[ra], 1))"	\n"
		"	blt+ 2f		# success			\n"
		"	li %[rc],%[eagain]				\n"
#ifndef DD2_CHIP_ERRATA
		"	bgt- 2f		# eagain			\n"
		"	li %[rc],%[enodev]				\n"
#endif
		"2:							\n"
		: [rc] "=&r" (rc), "=m" (*(uint32_t *)crb)
		: [rs] "r" (rs), [ra] "r" (crb),
		  [eagain] "I" (-EAGAIN), [enodev] "I" (-ENODEV)
		: "cr0");
	/* *INDENT-ON* */

	return rc;
}


/**
 * @brief Invoke a PowerEN coprocessor specifying type and directive
 * \ingroup icswx
 *
 * This takes the coprocessor type @a ct and coprocessor directive @a cd
 * as parameters and invokes the coprocessor, doing byte-packing magic
 * as required.  Returns @c -EAGAIN if the coprocessor reports back "busy",
 * @c -ENODEV if the coprocessor reports back "rejected" or @c -EFAULT
 * if the passed CRB is improperly aligned.
 *
 * @param ct  Coprocessor type
 * @param cd  Coprocessor directive, specific to @a ct
 * @param ea  Coprocessor request block, must be 128-byte aligned
 * @return    0 on success, negative ::errno value on failure
 */
static inline int icswx_td(int ct, unsigned short cd, void *ea)
{
	union cop_ccw ccwu = { 0 };

	if((unsigned long)ea & 127)
		return -EFAULT;

	ccwu.type = ct;
	ccwu.dir = cd;

	return icswx_raw(ccwu.value, ea);
}


/**
 * @brief Invoke a PowerEN coprocessor directly.
 * \ingroup icswx
 *
 * This takes a pointer to a coprocessor request block @a crb and invokes
 * the coprocessor specified in the block.  Returns @c -EAGAIN if the
 * coprocessor reports back "busy", * @c -ENODEV if the coprocessor reports
 * back "rejected" or @c -EFAULT if the passed CRB is improperly aligned.
 *
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @return    0 on success, negative ::errno value on failure
 */
static inline int icswx(struct cop_crb *crb)
{
	if((unsigned long)crb & 127)
		return -EFAULT;

	return icswx_raw(crb->ccw.value, crb);
}


/**
 * @brief Invoke a PowerEN coprocessor and retry in case busy.
 * \ingroup icswx
 *
 * This takes a pointer to a coprocessor request block @a crb and invokes
 * the coprocessor specified in the block. If the coprocesor is bust, it
 * will retry/reinvoke the coprocessor @a n_retry times while waiting
 * @a nssec nanoseconds between retrys.
 *
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @param n_retry  Number of times to retry
 * @param nsec  Number of nanoseconds to wait between retries
 * @return    0 on success, negative ::errno value on failure
 */
static inline int icswx_retry(struct cop_crb *crb, int n_retry,
							long nsec)
{
	int rc = 0;
	struct timespec ts;
	struct cop_csb *csb;

	if((unsigned long)crb & 127)
		return -EFAULT;

	do {
		if (rc != -EAGAIN)	/* first time ? */
			rc = icswx_raw(crb->ccw.value, crb);
		else
			rc = icswx_raw_nosync(crb->ccw.value, crb);
		
		if (--n_retry && (rc == -EAGAIN)) {
			/*
			 * check for error conditions before retry
			 */
			csb = cop_get_csbaddr(crb);
			if ((csb->cw.comp_code != 0) ||
			    (csb->cw.comp_ext & 0x80))
				break;

			if (nsec) {
				/* sleep for a bit if desired */
				ts.tv_sec = 0;
				ts.tv_nsec = nsec;
				(void)nanosleep(&ts, NULL);
			}
			csb->cw.valid = 0;	/* necessary ? */
		}
	} while (n_retry && (rc == -EAGAIN));

	return rc;
}


#ifdef __cplusplus
}
#endif 

#endif	/* _LIB_LIBCOPL_ICSWX_H */
