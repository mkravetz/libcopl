/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009, 2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/
#ifndef _LIB_LIBCOPL_RND_H
#define _LIB_LIBCOPL_RND_H

#include <errno.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>


/**
 * @defgroup RND Random Number (RND) setup routines
 * @{
 *
 * @brief Setup functions for RND Random Number control blocks
 *
 * This routine provides a convenient mechanism to setup the control
 * blocks associated with invocation of the RND coprocessor.
 *
 * The control blocks populated by the routines are pointed to by the
 * @a crb parameter.  Before calling this routine, the CRB must contain
 * a pointer to a valid CSB.  It is the responsibility of the caller to
 * properly allocate the control blocks.  The routines will validate the
 * alignment of the control blocks.  The space pointed to by @a crb should
 * be aligned on a COP_CRB_ALIGN boundary.  The space pointed to by
 * the CSB pointer within the CRB should be aligned on a COP_CSB_ALIGN
 * boundary.  The caller of this routine is also responsible for allocation
 * and proper mapping of the target data area.
 *
 * @param crb pointer to CSB structure containing a valis CSB pointer
 * @param target	target buffer for holding random value 
 * @param tgt_size	size of target buffer (in bytes)
 *
 * @return 		zero if successful, non-zero if failed
 */
static inline int cop_random_setup(struct cop_crb *crb, struct cop_ctx *ctx,
						void *target, size_t tgt_size)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	/* verify pointer arguments */
	if (!crb || !csb )
		return -EINVAL;

	crb->tgt_dde.bytes = tgt_size;
	cop_set_tdptr(crb, target);
	crb->src_dde.bytes = 0;
	cop_set_sdptr(crb, NULL);

	/*
	 * Add coprocessor type and instance to CRB/CCW
	 */
	cop_crb_set_ccw_type(crb, ctx);
	cop_crb_set_ccw_inst(crb, ctx);

	return 0;
}

/** @} */
#endif /* _LIB_LIBCOPL_RND_H */
