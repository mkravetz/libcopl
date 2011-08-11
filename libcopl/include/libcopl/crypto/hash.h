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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_HASH_H
#define _INCLUDE_LIBCOPL_CRYPTO_HASH_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup HASH Hashing (SHA1/SHA256/SHA512/MD5) interfaces.
 * @{
 */

/**
 * @brief CPB structure for Message Digest 5, 128 bytes long.
 **/
struct cop_sym_md5_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl;  /**< @brief Message bit length  */
	uint8_t _rsv0[8];
	uint8_t pd[16];	  /**< @brief Partial digest */
	uint8_t _rsv1[16];
	uint8_t md[16];	  /**< @brief Message digest / partial digest */
	uint8_t _rsv2[16];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief md5 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_md5_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_md5_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Secure Hash Algorithm 1, 128 bytes long.
 **/
struct cop_sym_sha1_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl;  /**< @brief Message bit length  */
	uint8_t _rsv0[8];
	uint8_t pd[20];	  /**< @brief Partial digest */
	uint8_t _rsv1[12];
	uint8_t md[20];	  /**< @brief Message digest / partial digest */
	uint8_t _rsv2[12];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief sha1 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha1_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha1_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Secure Hash Algorithm 256, 128 bytes long.
 **/
struct cop_sym_sha256_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl;  /**< @brief Message bit length  */
	uint8_t _rsv0[8];
	uint8_t pd[32];	  /**< @brief Partial digest */
	uint8_t md[32];	  /**< @brief Message digest / partial digest */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[28];
};

/**
 * @brief sha256 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha256_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha256_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for Secure Hash Algorithm 512, 256 bytes long.
 **/
struct cop_sym_sha512_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mbl[2];  /**< @brief Message bit length  */
	uint8_t pd[64];	  /**< @brief Partial digest */
	uint8_t _rsv0[32];
	uint8_t md[64];	  /**< @brief Message digest / partial digest */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[60];
};

/**
 * @brief sha512 CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_sha512_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_sha512_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for md5.
 *
 * Setup function for md5.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(MD5).
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
static inline int cop_md5_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_MD5;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_MD5;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_MD5);
}

/**
 * @brief Setup required coprocessor request block fields for sha1.
 *
 * Setup function for sha1.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA1).
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
static inline int cop_sha1_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_1;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/**
 * @brief Setup required coprocessor request block fields for sha256.
 *
 * Setup function for sha256.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA256).
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
static inline int cop_sha256_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_256;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/**
 * @brief Setup required coprocessor request block fields for sha512.
 *
 * Setup function for sha512.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(SHA512).
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
static inline int cop_sha512_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_SHA;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ds = DS_SHA_512;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_SHA);
}

/** @} HASH */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_HASH_H */
