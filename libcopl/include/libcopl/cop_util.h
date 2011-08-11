/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/
#ifndef _LIB_LIBCOPL_COP_UTIL_H
#define _LIB_LIBCOPL_COP_UTIL_H

#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>

/**
 * @brief Calculate address from @a a aligned to @s.
 */
#define ALIGN_UP(a,s)	(((a) + ((s) - 1)) & (~((s) - 1)))

#define ROUND_TO_NEXT_QUADWORD(b) (ALIGN_UP(b,16))
#define CEIL_BYTE(s) ((s % 8 == 0) ? s/8 : (s/8 + 1))

/*
 * chip version returned from cop_get_chip_version()
 */
#define COP_CHIP_UNKNOWN	-1
#define	COP_CHIP_DD1		1	/* DD1 level */
#define COP_CHIP_DD2		2	/* DD2 level */

/*
 * Information about instance placement within directive field of CCW.
 * Placement depends on coprocessor.
 */
struct cop_directive_info {
	uint32_t inst_mask;	/* clear the instance part of directive */
	int	inst_shift;	/* num bits to shift instance */
	uint32_t fc_mask;	/* clear the function code part of directive */
};
extern struct cop_directive_info cop_dir_info[];


#define COP_COMP_MTHD_STORE	0	/* Completion store */
#define COP_COMP_MTHD_INTERRUPT	1	/* Interrupt */
#define COP_COMP_MTHD_JOE	2	/* JOE Enqueue */

/**
 * \defgroup cop_crb Control Block (CRB/CSB/CPB) modification routines
 */

/**
 * \ingroup cop_crb
 * @brief Set the completion method field in a CRB
 *
 * This routine simply sets the completion method to the value specified
 * by @a method in the CRB specified by @ crb.  @a method should be one
 * of the values @c COP_COMP_MTHD_STORE, @c COP_COMP_MTHD_INTERRUPT or
 * @c COP_COMP_MTHD_JOE.
 *
 * @param rs Pointer to a previously populated CRB
 * @param mthd completion method
 */
static inline void cop_crb_set_comp_mthd(struct cop_crb *crb, int mthd)
{
	crb->csbp.cv = 1;
	crb->ccb.mthd = mthd;
};


/**
 * @brief Set the coprocessor type in the CCW and return CCW
 * \ingroup cop_crb
 * 
 * This routine sets the coprocessor type field within the CCW.  The
 * value for the type field is obtained from the @a ctx argument.
 * The value will be placed within the CCW contained in the @crb argument,
 * and the CCW value will be returned to the caller.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @param ctx Pointer to previously populated cop_ctx structure
 *
 * @return ccw that contains with the coprocessor type set
 */
static inline int cop_crb_set_ccw_type(struct cop_crb *crb, struct cop_ctx *ctx)
{
	crb->ccw.type = ctx->type;

	return crb->ccw.value;
}


/**
 * @brief Set the coprocessor instance in the CCW and return CCW
 * \ingroup cop_crb
 * 
 * This routine sets the coprocessor instance field within the CCW.  The
 * value for the instance field is obtained from the @a ctx argument.
 * The value will be placed within the CCW contained in the @crb argument,
 * and the CCW value will be returned to the caller.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @param ctx Pointer to previously populated cop_ctx structure
 *
 * @return ccw that contains with the coprocessor instance set
 */
static inline int cop_crb_set_ccw_inst(struct cop_crb *crb, struct cop_ctx *ctx)
{
	/* first clear appropriate bits in the CCW */
	crb->ccw.value &= cop_dir_info[ctx->copid].inst_mask;

	/*
	 * then OR in just those bits that comprise the instance
	 */
	crb->ccw.value |= ((uint32_t)ctx->instance &
				~cop_dir_info[ctx->copid].inst_mask);
	
	return crb->ccw.value;
}


/**
 * @brief Set the function code in the CCW directive and return CCW
 * \ingroup cop_crb
 * 
 * This routine sets the function code within the directive field within
 * the CCW.  The @a ctx argument contains coprocessor specific information
 * used to determine which bits within the directive field the function
 * code should occupy.  The function code will be placed in the appropriate
 * bits of the directive field, and the CCW value will be returned to the
 * caller.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @param ctx Pointer to previously populated cop_ctx structure
 * @param fc function code to be set in the directive field of CCW
 *
 * @return ccw that contains with the coprocessor function code set
 */
static inline int cop_crb_set_ccw_fc(struct cop_crb *crb, struct cop_ctx *ctx, int fc)
{
	/* first clear appropriate bits in the CCW */
	crb->ccw.value &= cop_dir_info[ctx->copid].fc_mask;

	/*
	 * then OR in just those bits that comprise the instance
	 */
	crb->ccw.value |= ((uint32_t)fc &
				~cop_dir_info[ctx->copid].fc_mask);

	return crb->ccw.value;
}

/**
 * @brief Set the function code in the CCW directive based on COPID
 * \ingroup cop_crb
 * 
 * This routine sets the function code within the directive field of the
 * the CCW.  The @a copid argument identifies the coprocessor for which
 * the CRB is being prepared.  @a copid is used to determine which bits
 * the function code should occupy within the directive field.  The function
 * code will be placed in the appropriate bits of the directive field, and
 * the CCW value will be returned to the caller.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @param ctx Pointer to previously populated cop_ctx structure
 * @param fc function code to be set in the directive field of CCW
 *
 * @return ccw that contains with the coprocessor function code set
 */
static inline int cop_crb_set_ccw_fc_copid(struct cop_crb *crb, int copid,
					   int fc)
{
	/* first clear appropriate bits in the CCW */
	crb->ccw.value &= cop_dir_info[copid].fc_mask;

	/*
	 * then OR in just those bits that comprise the instance
	 */
	crb->ccw.value |= ((uint32_t)fc &
				~cop_dir_info[copid].fc_mask);

	return crb->ccw.value;
}


/**
 * @brief Clear the control word within the CSB
 * \ingroup cop_crb
 * 
 * This routine clears the control word within the CSB.  If reusing a CSB,
 * it is important to clear the control word.  This ensures that data present
 * in the control word comes from the new, rather than previous coprocessor
 * invocation.
 *
 * @param csb Pointer to a CSB structure
 *
 * @return ccw that contains with the coprocessor function code set
 */
static inline void cop_csb_reset(struct cop_csb *csb)
{
	csb->cw.word32 = 0;
}


extern int cop_get_chip_version(struct cop_ctx *ctx);
extern void cop_crb_marker_trace_cnt(struct cop_crb *crb);
extern void cop_crb_set_marker_trace(struct cop_crb *crb, struct cop_ctx *ctx);
extern void cop_crb_clear_marker_trace(struct cop_crb *crb);
extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);
extern int cop_open_bind_remote(struct cop_ctx *ctx, int copid);

#endif	/* _LIB_LIBCOPL_COP_UTIL_H */
