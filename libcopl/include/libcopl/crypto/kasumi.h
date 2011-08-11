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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_KASUMI_H
#define _INCLUDE_LIBCOPL_CRYPTO_KASUMI_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup KASUMI Kasumi F8/F9 interfaces.
 * @{
 */

/**
 * @brief CPB structure for Kasumi F8, 128 bytes long.
 **/
struct cop_sym_kasumi_f8_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[8];	 /**< @brief Initialization vector / a */
	uint8_t _rsv0[8];
	uint8_t key[16];   /**< @brief Key */
	uint8_t bc_in[8];   /**< @brief Bc (last,middle) */
	uint8_t ks_in[8];   /**< @brief Ks (last,middle) */
	uint8_t a[8];	/**< @brief A (last,middle) */
	uint8_t _rsv1[8];
	uint8_t bc_out[8];   /**< @brief Bc (last,middle) */
	uint8_t ks_out[8];   /**< @brief Ks (last,middle) */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv2[28];
};

/**
 * @brief kasumi_f8 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_kasumi_f8_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_kasumi_f8_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Kasumi F9, 128 bytes long.
 **/
struct cop_sym_kasumi_f9_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[8];	 /**< @brief Initialization vector / partial message */
	uint8_t pad[8];	  /**< @brief Pad */
	uint8_t key[16];   /**< @brief Key */
	uint8_t b_in[8];   /**< @brief B */
	uint8_t _rsv0[8];
	uint8_t mac[8];	  /**< @brief Message authentication code (only, last) partial message (first, middle) */
	uint8_t _rsv1[8];
	uint8_t b_out[8];   /**< @brief B */
	uint8_t _rsv2[8];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief kasumi_f9 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_kasumi_f9_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_kasumi_f9_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for kasumi_f8.
 *
 * Setup function for kasumi_f8.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(KASUMI_F8).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_kasumi_f8_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_KAS_F8;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_KAS);
}

/**
 * @brief Setup required coprocessor request block fields for kasumi_f9.
 *
 * Setup function for kasumi_f9.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(KASUMI_F9).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_kasumi_f9_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_KAS_F9;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_KAS);
}

/** @} KASUMI */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_KASUMI_H */
