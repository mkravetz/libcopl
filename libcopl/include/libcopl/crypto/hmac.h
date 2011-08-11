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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_HMAC_H
#define _INCLUDE_LIBCOPL_CRYPTO_HMAC_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup HMAC SHA Hashing Message Authentication Code interfaces.
 * @{
 */

/**
 * @brief CPB structure for Hash-based Message Authentication Code using SHA-1, 192 bytes long.
 **/
struct cop_sym_sha1_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl;  /**< @brief Message bit length */
	uint8_t _rsv0[8];
	uint8_t key[64];   /**< @brief Key */
	uint8_t pm[20];	  /**< @brief Partial message */
	uint8_t _rsv1[12];
	uint8_t mac[20];   /**< @brief Message authentication code / partial message */
	uint8_t _rsv2[12];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief sha1_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha1_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[112]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha1_hmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Hash-based Message Authentication Code using SHA-256, 192 bytes long.
 **/
struct cop_sym_sha256_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl;  /**< @brief Message bit length */
	uint8_t _rsv0[8];
	uint8_t key[64];   /**< @brief Key */
	uint8_t pm[32];	  /**< @brief Partial message */
	uint8_t mac[32];   /**< @brief Message authentication code / partial message */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[28];
};

/**
 * @brief sha256_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha256_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[112]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha256_hmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Hash-based Message Authentication Code using SHA-512, 384 bytes long.
 **/
struct cop_sym_sha512_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl[2];  /**< @brief Message bit length */
	uint8_t key[128];   /**< @brief Key */
	uint8_t pm[64];	  /**< @brief Partial message */
	uint8_t _rsv0[32];
	uint8_t mac[64];   /**< @brief Message authentication code / partial message */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[60];
};

/**
 * @brief sha512_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha512_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha512_hmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for sha1_hmac.
 *
 * Setup function for sha1_hmac.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA1_HMAC).
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
static inline int cop_sha1_hmac_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA_HMAC;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_1;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/**
 * @brief Setup required coprocessor request block fields for sha256_hmac.
 *
 * Setup function for sha256_hmac.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA256_HMAC).
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
static inline int cop_sha256_hmac_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA_HMAC;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_256;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/**
 * @brief Setup required coprocessor request block fields for sha512_hmac.
 *
 * Setup function for sha512_hmac.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA512_HMAC).
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
static inline int cop_sha512_hmac_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA_HMAC;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_512;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/** @} HMAC */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_HMAC_H */
