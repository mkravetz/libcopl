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

#ifndef _LIB_LIBCOPL_COMP_H
#define _LIB_LIBCOPL_COMP_H

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

/*
 * CRB Types
 */
#define COP_CRB_TYPE_FUNCTIONAL     0x0000
#define COP_CRB_TYPE_STORAGE        0x0001
#define COP_CRB_TYPE_FLUSH          0x0002

/* Resume Flag */
#define COP_RESUME                  0x0080

/*
 * Chaining
 */
#define COP_CHAINING_MIDDLE         0x0000
#define COP_CHAINING_LAST           0x0001
#define COP_CHAINING_FIRST          0x0002
#define COP_CHAINING_ONLY           0x0003

/*
 * File Format
 */
#define COP_FF_DEFLATE              0x0000
#define COP_FF_GZIP                 0x0010
#define COP_FF_ZLIB                 0x0020

/*
 * Destination Buffer DDE Presence
 */
#define COP_DDE_NOT_PRESENT         0x0000
#define COP_DDE_PRESENT             0x0008

/*
 * Force cache inject
 */
#define COP_CACHE_NOT_INJECT        0x0000
#define COP_CACHE_INJECT            0x8000

/**
 * @defgroup COMP Decompression/Compression (DC) coprocessor setup routines
 * @{
 */


/**
 * @brief cop_comp_cpb Comp/Decomp Coprocessor Parameter Block
 *
 * Contains information about target buffer usage during decompression
 * operation.
 *
 * - tgt_bytes - Target Processed Byte Count
 * - src_bytes - Source Processed Byte Count
 * - tgt_wptr - Write pointer of Target Buffer. Offset from Target DDE address
 * - tgt_dde_sum - Sum of Target Direct DDE byte count
 * - tgt_dde_cnt - Number of Target DDE in CPB. 2-bit unsigned integer
 *	- 00: One Target DDE
 *	- 01: Two Target DDE
 *	- 10: Three Target DDE
 *	- 11: Four Target DDE
 * - eom - End-Of-Member detected
 * - indir_dde_cnt - DDE number in DDL when Target DDE #0 is Indirect DDE
 * - tgt_dde0-3 - target DDEs tgt_dde_cnt indicated how many DDEs are stored
 *
 * Note: cop_comp_cpb is a universally usable Type B CPB for the Comp/Decomp
 * coprocessor.  As a result, it is large (896 bytes).  Based on coproc
 * usage, a smaller Type A or Type C CPB could be used.
 */ 
struct cop_comp_cpb {
	uint32_t	_rsv_0;
	uint32_t	tgt_bytes;
	uint32_t	_rsv_1;
	uint32_t	src_bytes;		/* end of type A */
	uint32_t	tgt_wptr;
	uint32_t	tgt_dde_sum;
	uint32_t	_rsv_2;
	uint32_t	_rsv_3:21;
	uint32_t	tgt_dde_cnt:2;
	uint32_t	eom:1;
	uint32_t	indir_dde_cnt:8;
	uint64_t	_rsv_4[2];
	struct cop_dde tgt_dde0;
	struct cop_dde tgt_dde1;
	struct cop_dde tgt_dde2;
	struct cop_dde tgt_dde3;		/* end of type C */
	uint64_t	_rsv_5[98];		/* end of type B */
};

/**
 * @brief cop_comp_cpb_a Type A Comp/Decomp Coprocessor Parameter Block
 *
 * Contains information about target buffer usage during decompression
 * operation.
 *
 * See @c cop_comp_cpb for field descriptions
 */
struct cop_comp_cpb_a {
	uint32_t	_rsv_0;
	uint32_t	tgt_bytes;
	uint32_t	_rsv_1;
	uint32_t	src_bytes;
};

/**
 * @brief cop_comp_cpb_c Type C Comp/Decomp Coprocessor Parameter Block
 *
 * Contains information about target buffer usage during decompression
 * operation.
 *
 * See @c cop_comp_cpb for field descriptions
 */
struct cop_comp_cpb_c {
	uint32_t	_rsv_0;
	uint32_t	tgt_bytes;
	uint32_t	_rsv_1;
	uint32_t	src_bytes;
	uint32_t	tgt_wptr;
	uint32_t	tgt_dde_sum;
	uint32_t	_rsv_2;
	uint32_t	_rsv_3:21;
	uint32_t	tgt_dde_cnt:2;
	uint32_t	eom:1;
	uint32_t	indir_dde_cnt:8;
	uint64_t	_rsv_4[2];
	struct cop_dde tgt_dde0;
	struct cop_dde tgt_dde1;
	struct cop_dde tgt_dde2;
	struct cop_dde tgt_dde3;
};


enum cop_cmd_normal_functions {
	COP_COMP_CMD_COMP = 0x00,
	COP_COMP_CMD_DECOMP
};


#define COP_DECOMP_M_TGT_ALIGN		64
#define COP_DECOMP_M_TGT_MINSIZE	2048


/**
 * @brief Compression operation control block setup routine
 *
 * This routine provides a convenient mechanism to setup the control blocks
 * passed to the Decompression/Compression (DC) for a compression operation.
 * Normally, this routine would not be called directly.  Rather, it would
 * be invoked via an algorithm specific wrapper such as: cop_gzip_setup(),
 * cop_zlib_comp_setup() or cop_deflate_setup().
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *	      with an open instance of the DC coprocessor
 * @param src pointer to source data area (to be compressed)
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (compressed data)
 * @param t_size size (in bytes) of target area
 * @param flop flag that specifies the operation/algorithm to use.  valid
 *		values are COP_FF_DEFLATE, COP_FF_GZIP or COP_FF_ZLIB.
 *
 * @return zero if successful, non-zero on failure
 */
static inline int cop_comp_setup(struct cop_crb *crb,
				struct cop_ctx *ctx,
				void *src, size_t s_size,
				void *tgt, size_t t_size,
				int flop)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	/* verify argument pointers */
	if (!crb || !csb || !ctx ) 
		return -EINVAL;

	/* verify data arguments */
	if (!src || !s_size || !tgt || !t_size)
		return -EINVAL;

	crb->seq_num = 0;
        crb->src_dde.bytes = s_size;
        cop_set_sdptr(crb, src);
        crb->tgt_dde.bytes = t_size;
        cop_set_tdptr(crb, tgt);

	csb->cw.valid = 0;

        crb->flags = flop | COP_DDE_PRESENT;
	crb->ch = COP_CHAINING_ONLY;

	(void)cop_crb_set_ccw_fc(crb, ctx, COP_COMP_CMD_COMP);
	(void)cop_crb_set_ccw_inst(crb, ctx);
	(void)cop_crb_set_ccw_type(crb, ctx);

	return 0;
}


/**
 * @brief Decompression operation setup only/first control block
 *
 * This routine provides a convenient mechanism to setup the control blocks
 * passed to the Decompression/Compression (DC) for a decompression operation.
 * Normally, this routine would not be called directly.  Rather, it would
 * be invoked via an algorithm specific wrapper such as:
 * cop_inflate_setup_only/first(), cop_zlib_decomp_setup_only/first(), or
 * cop_gunzip_setup_only/first().
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *	      with an open instance of the DC coprocessor
 * @param src pointer to source data area (to be decompressed)
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (decompressed data)
 * @param t_size size (in bytes) of target area
 * @param flop flag that specifies the operation/algorithm to use.  valid
 *		values are COP_FF_DEFLATE, COP_FF_GZIP or COP_FF_ZLIB.
 * @param ch Chaining value to specify if this is the first or only CRB.
 *		Valid Values are COP_CHAINING_ONLY or COP_CHAINING_FIRST.
 *
 * @return zero if successful, non-zero on failure
 */
static inline int cop_decomp_setup_fo(struct cop_crb *crb,
					struct cop_ctx *ctx,
					void *src, size_t s_size,
					void *tgt, size_t t_size,
					int flop, int ch)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	/* verify argument pointers */
	if (!crb || !csb || !ctx) 
		return -EINVAL;

	/* verify data arguments */
	if (!src || !s_size || !tgt || !t_size)
		return -EINVAL;

#if 0
	/* for a first CRB, 'end' of target area must be aligned */
	if ((ch == COP_CHAINING_FIRST) &&
	    (((size_t)tgt + t_size) & (COP_DECOMP_M_TGT_ALIGN -1)))
		return -EINVAL;
#endif

	crb->seq_num = 0;
	crb->src_dde.countt = 0;
        crb->src_dde.bytes = s_size;
        cop_set_sdptr(crb, src);
	crb->tgt_dde.countt = 0;
        crb->tgt_dde.bytes = t_size;
        cop_set_tdptr(crb, tgt);

        crb->flags = flop | COP_DDE_PRESENT;

	csb->cw.valid = 0;
	csb->cw.ch = COP_CHAINING_LAST;

	(void)cop_crb_set_ccw_fc(crb, ctx, COP_COMP_CMD_DECOMP);
	(void)cop_crb_set_ccw_inst(crb, ctx);
	(void)cop_crb_set_ccw_type(crb, ctx);

	crb->ch = ch;

	return 0;
}


/**
 * @brief Decompression operation setup middle/last control block routine
 *
 * This routine provides a convenient mechanism to setup control blocks
 * passed to the Decompression/Compression (DC) for a decompression
 * operation in the middle of a decompression sequence.  Normally, this
 * routine would not be called directly.  Rather, it would be invoked
 * via an algorithm specific wrapper such as: cop_gunzip_setup_middle/last(),
 * cop_zlib_decomp_setup_middle/last() or cop_inflate_setup_middle/last().
 *
 * @param crb pointer to CRB structure with pointers to CSB already set.
 *		The CRB should have been previously used in decompression
 *		sequence as either the first CRB or a middle CRB.
 * @param src pointer to source data area (to be decompressed). @a src can
 *		be NULL if data remains in srb buffer from previous call.
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (compressed data).  @a tgt can
 *		be NULL if sufficient room remains from a previous operation.
 * @param t_size size (in bytes) of target area
 * @param ch Chaining value to specify if this is the middle or last CRB.
 *		Valid Values are COP_CHAINING_MIDDLE or COP_CHAINING_LAST.
 *
 * @return zero if successful, non-zero on failure
 */
static inline int cop_decomp_setup_ml(struct cop_crb *crb,
					void *src, size_t s_size,
					void *tgt, size_t t_size,
					int ch)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	if (!crb || !csb)
		return -EINVAL;

#if 0
	/*
	 * target area restrictions
	 * start/end on 64b boundary, at least 2K in size
	 */
	if ((unsigned long)tgt & (COP_DECOMP_M_TGT_ALIGN - 1) ||
	    t_size & (COP_DECOMP_M_TGT_ALIGN - 1) ||
	    t_size < COP_DECOMP_M_TGT_MINSIZE)
		return -EINVAL;
#endif

	csb->cw.valid = 0;

	crb->seq_num++;

	if (src && s_size) {
		crb->src_dde.countt = 0;
		crb->src_dde.bytes = s_size;
		cop_set_sdptr(crb, src);
	}

	if (tgt && t_size) {
		crb->tgt_dde.countt = 0;
		crb->tgt_dde.bytes = t_size;
		cop_set_tdptr(crb, tgt);
		crb->flags |= COP_DDE_PRESENT;
	} else {
		crb->flags &= ~COP_DDE_PRESENT;
	}

        crb->flags &= ~COP_RESUME;
	crb->ch = ch;

	return 0;
}


/**
 * @brief Decompression operation add target buffer and resume setup routine
 *
 * This routine provides a convenient mechanism to setup control blocks that
 * will add a target buffer to a decompression sequence already in progress
 * and resume the sequence.  Normally, this routine would not be called
 * directly.  Rather, it would be invoked via an algorithm specific wrapper
 * such as: cop_gunzip_addtgt_rsm_setup(), cop_zlib_decomp_addtgt_resume() or
 * cop_inflate_addtgt_resume().
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 *		The CRB should have been previously used in the decompression
 *		sequence
 * @param tgt pointer to target data area (compressed data)
 * @param t_size size (in bytes) of target area
 * @param flop flag that specifies the operation/algorithm to use.  valid
 *		values are COP_FF_DEFLATE, COP_FF_GZIP or COP_FF_ZLIB.
 *
 * @return zero if successful, non-zero on failure
 */
static inline int cop_decomp_addtgt_rsm_setup(struct cop_crb *crb,
					void *tgt, size_t t_size,
					int flop)
{
	struct cop_csb *csb = cop_get_csbaddr(crb);

	if (!crb || !csb || !tgt || !t_size)
		return -EINVAL;

	crb->flags = COP_RESUME | flop | COP_DDE_PRESENT;
	crb->tgt_dde.countt = 0;
	crb->tgt_dde.bytes = t_size;
	cop_set_tdptr(crb, tgt);
	
	csb->cw.valid = 0;

	return 0;
}

/** @} */
#endif /* _LIB_LIBCOPL_COMP_H */
