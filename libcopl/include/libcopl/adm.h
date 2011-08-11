/* 

Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009,2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_ADM_H
#define _LIB_LIBCOPL_ADM_H

#include <errno.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

#define COP_ADM_COPY_INC	0
#define COP_ADM_COPY_DEC	1

#define COP_ADM_MEMCPY		0
#define COP_ADM_MEMMOVE		1

static inline int _cop_adm(struct cop_crb *crb, struct cop_ctx *ctx,
				void *source, void *target, size_t size,
				int mode)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	/* verify pointers and alignment */
	if (!crb || !csb || !ctx)
		return -EINVAL;

	cop_csb_reset(csb);

	crb->src_dde.bytes = crb->tgt_dde.bytes = size;

	/* for now always use COP_ADM_COPY_INC mode */
	(void)cop_crb_set_ccw_fc(crb, ctx, COP_ADM_COPY_INC);


	cop_set_sdptr(crb, source);
	cop_set_tdptr(crb, target);

	/*
	 * Add coprocessor type and instance to CRB/CCW
	 */
	(void)cop_crb_set_ccw_type(crb, ctx);
	(void)cop_crb_set_ccw_inst(crb, ctx);

	return 0;
}


/**
 * @defgroup ADM Asynchronous Data Mover (ADM) setup routines
 * @{
 *
 * @brief Setup functions for ADM Asynchronous Data Mover control blocks
 * 
 * These routines provide a convenient mechanism to setup the control
 * blocks associated with invocation of the ADM coprocessor.  The two
 * primary operations that can be performed are a memory move
 * @c cop_memmove_setup() and memory copy @c cop_memcpy_setup().
 *
 * A pointer to the primary control block (the @a crb) is passed to this
 * routine.  It is assumed that a pointer to a valid CSB is contained within
 * the CRB.  It is the responsibility of the caller to properly allocate
 * the CRB and CSB control blocks.  The routines will validate the
 * alignment of the control blocks.  The space pointed to by @a crb should
 * be aligned on a COP_ADM_CRB_ALIGN boundary.  The space pointed to by
 * @ a csb should be aligned on a COP_ADM_CSB_ALIGN boundary.  The caller
 * of this routine is also responsible for allocation and proper mapping
 * of source and destination memory areas.
 *
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *            with an open instance of the ADM coprocessor.
 * @param dest pointer to destination data area of move/copy
 * @param src pointer to source data area
 * @param n length of data in bytes
 *
 * @return zero if successful, non-zero if failure
 */
static inline int cop_memcpy_setup(struct cop_crb *crb,
					 struct cop_ctx *ctx, void *dest,
					 void *src, size_t n)
{
	return _cop_adm(crb, ctx, src, dest, n, COP_ADM_MEMCPY);
}

static inline int cop_memmove_setup(struct cop_crb *crb,
					  struct cop_ctx *ctx, void *dest,
					  void *src, size_t n)
{
	return _cop_adm(crb, ctx, src, dest, n, COP_ADM_MEMMOVE);
}

/** @} */

#endif /* _LIB_LIBCOPL_ADM_H */
