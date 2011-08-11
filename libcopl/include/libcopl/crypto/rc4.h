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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_RC4_H
#define _INCLUDE_LIBCOPL_CRYPTO_RC4_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup RC4 Alleged Rivest Cipher 4 interfaces.
 * @{
 */

/**
 * @brief CPB structure for 40b key Rivest Cipher 4, 384 bytes long.
 **/
struct cop_sym_rc4_40_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[5];	  /**< @brief Key (5 bytes) */
	uint8_t _rsv0[43];
	uint8_t x_val;	/**< @brief X value in keystream */
	uint8_t y_val;	/**< @brief Y value in keystream */
	uint8_t _rsv1[14];
	uint8_t state[256];   /**< @brief Current state input */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv2[44];
};

/**
 * @brief rc4_40 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_rc4_40_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_rc4_40_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for 64b key Rivest Cipher 4, 384 bytes long.
 **/
struct cop_sym_rc4_64_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[8];	  /**< @brief Key (8 bytes) */
	uint8_t _rsv0[40];
	uint8_t x_val;	/**< @brief X value in keystream */
	uint8_t y_val;	/**< @brief Y value in keystream */
	uint8_t _rsv1[14];
	uint8_t state[256];   /**< @brief Current state input */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv2[44];
};

/**
 * @brief rc4_64 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_rc4_64_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_rc4_64_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for 128b key Rivest Cipher 4, 384 bytes long.
 **/
struct cop_sym_rc4_128_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[16];   /**< @brief Key (16 bytes) */
	uint8_t _rsv0[32];
	uint8_t x_val;	/**< @brief X value in keystream */
	uint8_t y_val;	/**< @brief Y value in keystream */
	uint8_t _rsv1[14];
	uint8_t state[256];   /**< @brief Current state input */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv2[44];
};

/**
 * @brief rc4_128 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_rc4_128_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_rc4_128_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for rc4_40.
 *
 * Setup function for rc4_40.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(RC4).
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
static inline int cop_rc4_40_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_RC4;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = KS_RC4_40;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_RC4);
}

/**
 * @brief Setup required coprocessor request block fields for rc4_64.
 *
 * Setup function for rc4_64.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(RC4).
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
static inline int cop_rc4_64_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_RC4;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = KS_RC4_64;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_RC4);
}

/**
 * @brief Setup required coprocessor request block fields for rc4_128.
 *
 * Setup function for rc4_128.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(RC4).
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
static inline int cop_rc4_128_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_RC4;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = KS_RC4_128;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_RC4);
}

/** @} RC4 */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_RC4_H */
