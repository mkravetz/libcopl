/*
   Licensed Materials - Property of IBM
   Restricted Materials of IBM

   Copyright 2008,2009, 2010
   Chris J Arges
   IBM Corp, All Rights Reserved

   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _INCLUDE_LIBCOPL_CRYPTO_HASH_EX_H
#define _INCLUDE_LIBCOPL_CRYPTO_HASH_EX_H

#include "hash.h"

/**
 * @ingroup CRYPTO
 * @defgroup HASH
 * @{
 */

/**
 * @brief Copy parameters as required for the next part of a multi-part hash calculation.
 * @defgroup HASH_ADVANCE
 * @{
 *
 * The coprocessor operation using @a prev must be completed.
 * This function is only useful for continuation type operations.
 * @a prev and @a next may be the same structure; if not,
 * @a next must be initialized using appropriate setup function and using
 * the same parameters as @a prev was.
 *
 * @param[in] prev a valid cpb structure from previous part
 * @param[in] next a valid cpb structure from next part
 * @pre prev->hdr.fdm.c == 1
 * @pre prev->hdr.fdm.i == 0
 */
static inline void cop_md5_cpb_advance(struct cop_sym_md5_cpb * prev,
	struct cop_sym_md5_cpb * next)
{
	if (!prev || !next) return;
	memcpy(next->pd, prev->md, sizeof(prev->md));
}

static inline void cop_sha1_cpb_advance(struct cop_sym_sha1_cpb * prev,
	struct cop_sym_sha1_cpb * next)
{
	if (!prev || !next) return;
	memcpy(next->pd, prev->md, sizeof(prev->md));
}

static inline void cop_sha256_cpb_advance(struct cop_sym_sha256_cpb * prev,
	struct cop_sym_sha256_cpb * next)
{
	if (!prev || !next) return;
	memcpy(next->pd, prev->md, sizeof(prev->md));
}

static inline void cop_sha512_cpb_advance(struct cop_sym_sha512_cpb * prev,
	struct cop_sym_sha512_cpb * next)
{
	if (!prev || !next) return;
	memcpy(next->pd, prev->md, sizeof(prev->md));
}

/** @} HASH_ADVANCE */

/**
 * @brief Calculate message bit length based on number of bytes.
 * @defgroup HASH_CALC_MBL
 * @{
 *
 * @param[in] cpb
 * @param[in] bytes
 */
static inline void cop_md5_cpb_calc_mbl(struct cop_sym_md5_cpb *cpb, 
	uint64_t bytes)
{
	cpb->mbl = bytes << 3ul;
}

static inline void cop_sha1_cpb_calc_mbl(struct cop_sym_sha1_cpb *cpb, 
	uint64_t bytes)
{
	cpb->mbl = bytes << 3ul;
}

static inline void cop_sha256_cpb_calc_mbl(struct cop_sym_sha256_cpb *cpb, 
	uint64_t bytes)
{
	cpb->mbl = bytes << 3ul;
}

static inline void cop_sha512_cpb_calc_mbl(struct cop_sym_sha512_cpb *cpb, 
	uint64_t bytes)
{
	cpb->mbl[0] = 0;
	cpb->mbl[1] = bytes << 3ull;	
}

/** @} HASH_CALC_MBL */

/** @} HASH*/
#endif
