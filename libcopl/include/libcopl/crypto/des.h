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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_DES_H
#define _INCLUDE_LIBCOPL_CRYPTO_DES_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup DES Data Encryption Standard interfaces.
 * @{
 */

/**
 * @brief CPB structure for DES in ECB mode, 128 bytes long.
 **/
struct cop_sym_des_ecb_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[8];	  /**< @brief Key (56 bits) */
	uint8_t _rsv0[40];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[60];
};

/**
 * @brief des_ecb CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_des_ecb_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_des_ecb_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for DES in Chain-block Cipher mode, 128 bytes long.
 **/
struct cop_sym_des_cbc_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[8];	 /**< @brief Initialization vector / chaining value */
	uint8_t _rsv0[8];
	uint8_t key[8];	  /**< @brief Key */
	uint8_t _rsv1[24];
	uint8_t cv[8];	 /**< @brief Chaining value */
	uint8_t _rsv2[8];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[44];
};

/**
 * @brief des_cbc CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_des_cbc_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_des_cbc_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for TDES in ECB mode, 128 bytes long.
 **/
struct cop_sym_tdes_ecb_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[24];   /**< @brief Key (168 bits) */
	uint8_t _rsv0[24];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[60];
};

/**
 * @brief tdes_ecb CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_tdes_ecb_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_tdes_ecb_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for TDES in Chain-block Cipher mode, 128 bytes long.
 **/
struct cop_sym_tdes_cbc_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[8];	 /**< @brief Initialization vector / chaining value */
	uint8_t _rsv0[8];
	uint8_t key[24];   /**< @brief Key */
	uint8_t _rsv1[8];
	uint8_t cv[8];	 /**< @brief Chaining value */
	uint8_t _rsv2[8];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[44];
};

/**
 * @brief tdes_cbc CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_tdes_cbc_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_tdes_cbc_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for des_ecb.
 *
 * Setup function for des_ecb.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(DES_ECB).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] pad {COP_RPAD_NONE, COP_RPAD_ZERO, COP_RPAD_INC, COP_RPAD_USE}
 * @param[in] rpad byte used per padding option in @a pad
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_des_ecb_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_DES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_DES);
}

/**
 * @brief Setup required coprocessor request block fields for des_cbc.
 *
 * Setup function for des_cbc.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(DES_CBC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] pad {COP_RPAD_NONE, COP_RPAD_ZERO, COP_RPAD_INC, COP_RPAD_USE}
 * @param[in] rpad byte used per padding option in @a pad
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_des_cbc_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_DES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_DES);
}

/**
 * @brief Setup required coprocessor request block fields for tdes_ecb.
 *
 * Setup function for tdes_ecb.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(TDES_ECB).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] pad {COP_RPAD_NONE, COP_RPAD_ZERO, COP_RPAD_INC, COP_RPAD_USE}
 * @param[in] rpad byte used per padding option in @a pad
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_tdes_ecb_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_TDES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_DES);
}

/**
 * @brief Setup required coprocessor request block fields for tdes_cbc.
 *
 * Setup function for tdes_cbc.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(TDES_CBC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] pad {COP_RPAD_NONE, COP_RPAD_ZERO, COP_RPAD_INC, COP_RPAD_USE}
 * @param[in] rpad byte used per padding option in @a pad
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_tdes_cbc_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_TDES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_DES);
}

/** @} DES */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_DES_H */
