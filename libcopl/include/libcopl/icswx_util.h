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
 * @file icswx_util.h
 * @brief Utility routines built around @c icswx instruction invocation
 * \defgroup icswx_util Utility routines built around icswx invocation
 */

#ifndef _LIB_LIBCOPL_ICSWX_UTIL_H
#define _LIB_LIBCOPL_ICSWX_UTIL_H

#include <errno.h>
#include <time.h>
#include <libcopl/cop.h>
#include <libcopl/icswx.h>
#include <libcopl/waitm.h>

#ifdef __cplusplus
extern "C" {
#endif 


/*
 * cop invocation helper functions
 */
static inline unsigned cop_csb_valid(struct cop_csb *csb)
{
	/* check once */
	unsigned v;
	/* *INDENT-OFF* */
	asm volatile (
		"lbz %[v],0%[csb]; lwsync	\n"
		:[v] "=&r" (v)
		:[csb] "b" (csb));
	/* *INDENT-ON* */
	return v;
}


static inline void cop_csb_spin(struct cop_csb *csb)
{
	/* check forever */
	do {
	} while (!cop_csb_valid(csb));
}


static inline void cop_csb_wait(struct cop_csb *csb)
{
	union cop_csbctl ctl = csb->cw;

	/* wait forever */
	while (!ctl.valid) {
		ctl.word32 = wait_on_mem32(&csb->cw.word32, ctl.word32);
	}

	lwsync();
}


/**
 * @brief Invoke a PowerEN coprocessor specifying type/directive and wait
 * \ingroup icswx_util
 *
 * This routine is similar to @c icswx_td(), except that it waits for
 * completion of the coprocessor call, and returns status/error information.
 * It takes the coprocessor type @a ct and coprocessor directive @a cd
 * as parameters and invokes the coprocessor, doing byte-packing magic
 * as required.  Returns negative values @c -EAGAIN, @c -ENODEV if there
 * was an error involing the coprocessor or @c -EFAULT if the passed CRB
 * is improperly aligned..  It returns a positive (non-zero) value if an
 * error was encountered by the coprocessor while executing the request.
 *
 * @param ct  Coprocessor type
 * @param cd  Coprocessor directive, specific to @a ct
 * @param ea  Coprocessor request block, must be 128-byte aligned
 * @return    0 on success, negative values for invocation error,
 *			    positive values for processing error
 */
static inline int icswx_td_wait(int ct, unsigned short cd, void *ea)
{
	int rc;
	union cop_ccw ccwu = { 0 };
	struct cop_csb *csb;
	struct cop_crb *crb = (struct cop_crb *)ea;

	if((unsigned long)ea & 127)
		return -EFAULT;

	ccwu.type = ct;
	ccwu.dir = cd;

	if ((rc = icswx_raw(ccwu.value, ea)))
		return rc; /* error invoking coprocessor */

	csb = cop_get_csbaddr(crb);
	cop_csb_wait(csb);

	return(csb->cw.comp_code);
}


/**
 * @brief Invoke a PowerEN coprocessor directly and wait
 * \ingroup icswx_util
 *
 * This routine is similar to @c icswx(), except that it waits for
 * completion of the coprocessor call, and returns status/error information.
 * It takes a pointer to a coprocessor request block @a crb and invokes
 * the coprocessor specified in the block.  Returns negative values
 * @c -EAGAIN, @c -ENODEV if there was an error involing the coprocessor or
 * @c -EFAULT if the passed CRB is improperly aligned. It returns a positive
 * (non-zero) value if an error was encountered by the coprocessor while
 * executing the request.
 *
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @return    0 on success, negative values for invocation error,
 *			    positive values for processing error
 */
static inline int icswx_wait(struct cop_crb *crb)
{
	int rc;
	struct cop_csb *csb;

	if((unsigned long)crb & 127)
		return -EFAULT;

	if ((rc = icswx_raw(crb->ccw.value, crb)))
		return rc; /* error invoking coprocessor */

	csb = cop_get_csbaddr(crb);
	cop_csb_wait(csb);

	return(csb->cw.comp_code);
}


/**
 * @brief Invoke a PowerEN coprocessor and retry in case busy.
 * \ingroup icswx_util
 *
 * This routine is similar to @c icswx_retry(), except that it waits for
 * completion of the coprocessor call, and returns status/error information.
 * This takes a pointer to a coprocessor request block @a crb and invokes
 * the coprocessor specified in the block. If the coprocesor is busy, it
 * will retry/reinvoke the coprocessor @a n_retry times while waiting
 * @a nssec nanoseconds between retrys.
 *
 * @param crb  Coprocessor request block, must be 128-byte aligned
 * @param n_retry  Number of times to retry
 * @param nsec  Number of nanoseconds to wait between retries
 * @return    0 on success, negative values for invocation error,
 *			    positive values for processing error
 */
static inline int icswx_retry_wait(struct cop_crb *crb, int n_retry,
							long nsec)
{
	int rc = 0;
	struct timespec ts;
	struct cop_csb *csb;

	if((unsigned long)crb & 127)
		return -EFAULT;

	csb = cop_get_csbaddr(crb);

	do {
		if (rc != -EAGAIN)	/* first time ? */
			rc = icswx_raw(crb->ccw.value, crb);
		else
			rc = icswx_raw_nosync(crb->ccw.value, crb);
		
		if (--n_retry && (rc == -EAGAIN)) {
			/*
			 * check for error conditions before retry
			 */
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

	if (rc)
		return rc; /* error invoking coprocessor */

	cop_csb_wait(csb);

	return(csb->cw.comp_code);
}

#ifdef __cplusplus
}
#endif 

#endif	/* _LIB_LIBCOPL_ICSWX_UTIL_H */
