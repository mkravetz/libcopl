/* 
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009,2010
   Chris J Arges
   Everton Constantino
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/
#ifndef _LIB_LIBCOPL_COMP_DEFLATE_H
#define _LIB_LIBCOPL_COMP_DEFLATE_H

#include <libcopl/comp.h>

/**
 * @ingroup COMP
 * @defgroup DEFLATE Deflate algorithm setup routines for DC coprocessor
 * @{
 */

/**
 * @brief Setup deflate operation control blocks
 *
 * This routine provides a convenient mechanism to setup the control blocks
 * passed to the Decompression/Compression (DC) coprocessor for a deflate
 * operation.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *            with an open instance of the DC coprocessor
 * @param src pointer to source data area (to be compressed)
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (compressed data)
 * @param t_size size (in bytes) of target area
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_deflate_setup(crb, ctx, src, s_size, tgt, t_size) \
	cop_comp_setup(crb, ctx, src, s_size, tgt, t_size, COP_FF_DEFLATE)

/**
 * @brief Setup inflate operation control blocks if only a single CRB is needed
 *
 * This routine provides a convenient mechanism to setup the control blocks
 * passed to the Decompression/Compression (DC) coprocessor for an inflate
 * operation where only a single CRB is needed.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *            with an open instance of the DC coprocessor
 * @param src pointer to source data area (to be decompressed)
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (decompressed data)
 * @param t_size size (in bytes) of target area
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_inflate_setup_only(crb, ctx, src, s_size, tgt, t_size) \
	cop_decomp_setup_fo(crb, ctx, src, s_size, tgt, t_size, \
			    COP_FF_DEFLATE, COP_CHAINING_ONLY)

/**
 * @brief Setup inflate operation control blocks for first CRB in a sequence
 *
 * This routine provides a convenient mechanism to setup the first control
 * blocks passed to the Decompression/Compression (DC) coprocessor in a
 * sequence of CRBS used to perform an inflate operation.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 * @param ctx pointer to a previously populated cop_ctx structure assoiated
 *            with an open instance of the DC coprocessor
 * @param src pointer to source data area (to be decompressed)
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (decompressed data)
 * @param t_size size (in bytes) of target area
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_inflate_setup_first(crb, ctx, src, s_size, tgt, t_size) \
	cop_decomp_setup_fo(crb, ctx, src, s_size, tgt, t_size, \
			    COP_FF_DEFLATE, COP_CHAINING_FIRST)

/**
 * @brief Setup inflate operation control blocks for a middle CRB in a sequence
 *
 * This routine provides a convenient mechanism to setup the middle control
 * blocks passed to the Decompression/Compression (DC) coprocessor in a
 * sequence of CRBS used to perform an inflate operation.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set.
 *              The CRB should have been previously used in decompression
 *              sequence as either the first CRB or a middle CRB.
 * @param src pointer to source data area (to be decompressed). @a src can
 *              be NULL if data remains in srb buffer from previous call.
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (compressed data).  @a tgt can
 *              be NULL if sufficient room remains from a previous operation.
 * @param t_size size (in bytes) of target area
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_inflate_setup_middle(crb, src, s_size, tgt, t_size) \
	cop_decomp_setup_ml(crb, src, s_size, tgt, t_size, \
			    COP_CHAINING_MIDDLE)

/**
 * @brief Setup inflate operation control blocks for a last CRB in a sequence
 *
 * This routine provides a convenient mechanism to setup the last control
 * blocks passed to the Decompression/Compression (DC) coprocessor in a
 * sequence of CRBs used to perform an inflate operation.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set.
 *              The CRB should have been previously used in decompression
 *              sequence as either the first CRB or a middle CRB.
 * @param src pointer to source data area (to be decompressed). @a src can
 *              be NULL if data remains in srb buffer from previous call.
 * @param s_size size (in bytes) of source data
 * @param tgt pointer to target data area (compressed data).  @a tgt can
 *              be NULL if sufficient room remains from a previous operation.
 * @param t_size size (in bytes) of target area
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_inflate_setup_last(crb, src, s_size, tgt, t_size) \
	cop_decomp_setup_ml(crb, src, s_size, tgt, t_size, \
			    COP_CHAINING_LAST)

/**
 * @brief Setup control blocks to add target buffer and resume inflate operation
 *
 * This routine provides a convenient mechanism to setup control blocks that
 * will add a target buffer to an inflate sequence already in progress
 * and resume the sequence.
 *
 * @param crb pointer to CRB structure with pointers to CSB already set
 *              The CRB should have been previously used in the decompression
 *              sequence
 * @param tgt pointer to target data area (compressed data)
 * @param t_size size (in bytes) of target area
 * @param flop flag that specifies the operation/algorithm to use.  valid
 *              values are COP_FF_DEFLATE, COP_FF_GZIP or COP_FF_ZLIB.
 *
 * @return zero if successful, non-zero on failure
 */
#define cop_inflate_addtgt_rsm_setup(crb, tgt, t_size) \
	cop_decomp_addtgt_rsm_setup(crb, tgt, t_size, COP_FF_DEFLATE)

/** @} */
#endif /* _LIB_LIBCOPL_COMP_DEFLATE_H */
