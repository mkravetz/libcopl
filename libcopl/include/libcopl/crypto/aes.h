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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_AES_H
#define _INCLUDE_LIBCOPL_CRYPTO_AES_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup AES Advanced Encryption Standard interfaces.
 * @{
 */

/**
 * @brief CPB structure for AES in ECB mode, 128 bytes long.
 **/
struct cop_sym_aes_ecb_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t key[32];   /**< @brief Key */
	uint8_t _rsv0[16];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[60];
};

/**
 * @brief aes_ecb CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_ecb_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_ecb_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in Cipher-Block Chaining mode, 128 bytes long.
 **/
struct cop_sym_aes_cbc_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint8_t key[32];   /**< @brief Key */
	uint8_t cv[16];	  /**< @brief Chaining value */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv0[44];
};

/**
 * @brief aes_cbc CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_cbc_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_cbc_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in Galois MAC mode, 192 bytes long.
 **/
struct cop_sym_aes_gmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t pat[16];   /**< @brief Partial authentication tag */
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint64_t blen_a;  /**< @brief Bit length of aad field */
	uint64_t blen_c;  /**< @brief Bit length of message / cipher input data */
	uint8_t s0_in[16];   /**< @brief S0 input */
	uint8_t key[32];   /**< @brief Key */
	uint8_t _rsv0[16];
	uint8_t mac[16];   /**< @brief Message authentication code */
	uint8_t s0_out[16];   /**< @brief S0 output */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[28];
};

/**
 * @brief aes_gmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_gmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[112]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_gmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in GCA mode, 128 bytes long.
 **/
struct cop_sym_aes_gca_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t pat_in[16];   /**< @brief Partial authentication tag */
	uint8_t key[32];   /**< @brief Key */
	uint8_t pat_out[16];   /**< @brief Partial authentication tag */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv0[44];
};

/**
 * @brief aes_gca CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_gca_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_gca_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in Galois/CTR mode, 192 bytes long.
 **/
struct cop_sym_aes_gcm_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t pat[16];   /**< @brief Partial authentication tag or aad */
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint64_t blen_a;  /**< @brief Bit length of aad field */
	uint64_t blen_c;  /**< @brief Bit length of message / cipher input data */
	uint8_t s0_in[16];   /**< @brief S0 input */
	uint8_t key[32];   /**< @brief Key */
	uint8_t _rsv0[16];
	uint8_t mac[16];   /**< @brief Message authentication code or pat */
	uint8_t s0_out[16];   /**< @brief S0 output */
	uint8_t cnt[16];   /**< @brief Counter output */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[12];
};

/**
 * @brief aes_gcm CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_gcm_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[112]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_gcm_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in CCA mode, 128 bytes long.
 **/
struct cop_sym_aes_cca_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t b0[16];	  /**< @brief B0 */
	uint8_t b1[16];	  /**< @brief B1 */
	uint8_t key[16];   /**< @brief Key */
	uint8_t pat[16];   /**< @brief Partial authentication tag */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv0[44];
};

/**
 * @brief aes_cca CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_cca_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_cca_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in CBC MAC mode, 192 bytes long.
 **/
struct cop_sym_aes_ccm_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t pat[16];   /**< @brief Partial authentication tag */
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint8_t s0_in[16];   /**< @brief S0 input */
	uint8_t key[16];   /**< @brief Key */
	uint8_t _rsv0[48];
	uint8_t u[16];	 /**< @brief Output authentication value or pat */
	uint8_t s0_out[16];   /**< @brief S0 output */
	uint8_t cnt[16];   /**< @brief Counter output */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[12];
};

/**
 * @brief aes_ccm CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_ccm_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[112]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_ccm_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in Counter mode, 128 bytes long.
 **/
struct cop_sym_aes_ctr_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t iv[16];	  /**< @brief Initialization vector or chaining value  */
	uint8_t key[32];   /**< @brief Key */
	uint8_t cv[16];	  /**< @brief Chaining value */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv0[44];
};

/**
 * @brief aes_ctr CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_ctr_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_ctr_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES in XMAC mode, 128 bytes long.
 **/
struct cop_sym_aes_xmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint8_t cv[16];	  /**< @brief Chaining value */
	uint8_t key[16];   /**< @brief Key */
	uint8_t _rsv0[16];
	uint8_t mac[16];   /**< @brief Chaining value / mac */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv1[44];
};

/**
 * @brief aes_xmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_xmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_xmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for aes_ecb.
 *
 * Setup function for aes_ecb.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_ECB).
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
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ecb_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ecb_128.
 *
 * Setup function for aes_ecb_128.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_ECB).
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
static inline int cop_aes_ecb_128_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_128;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ecb_192.
 *
 * Setup function for aes_ecb_192.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_ECB).
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
static inline int cop_aes_ecb_192_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_192;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ecb_256.
 *
 * Setup function for aes_ecb_256.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_ECB).
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
static inline int cop_aes_ecb_256_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_ECB;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_256;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc.
 *
 * Setup function for aes_cbc.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CBC).
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
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_cbc_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_128.
 *
 * Setup function for aes_cbc_128.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CBC).
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
static inline int cop_aes_cbc_128_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_128;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_192.
 *
 * Setup function for aes_cbc_192.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CBC).
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
static inline int cop_aes_cbc_192_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_192;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_256.
 *
 * Setup function for aes_cbc_256.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CBC).
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
static inline int cop_aes_cbc_256_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = KS_AES_256;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_gmac.
 *
 * Setup function for aes_gmac.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_GMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_gmac_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_GMAC;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = ks;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_gca.
 *
 * Setup function for aes_gca.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_GCA).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_gca_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_GCA;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = ks;
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_gcm.
 *
 * Setup function for aes_gcm.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_GCM).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_gcm_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t flag, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_GCM;
	hdr->fdm.ende = ende;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	hdr->ks = ks;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cca.
 *
 * Setup function for aes_cca.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CCA).
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
static inline int cop_aes_cca_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CCA;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ccm.
 *
 * Setup function for aes_ccm.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CCM).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] flag COP_FLAG_ {ONESHOT, FIRST, MIDDLE, LAST}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ccm_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ende,
	uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CCM;
	hdr->fdm.ende = ende;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr.
 *
 * Setup function for aes_ctr.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_CTR).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, void *tgt, size_t tgt_sz, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR;
	hdr->ks = ks;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES);
}

/**
 * @brief Setup required coprocessor request block fields for aes_xmac.
 *
 * Setup function for aes_xmac.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_XMAC).
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
static inline int cop_aes_xmac_setup(struct cop_crb *crb, struct cop_ctx *ctx,
	const void *src, size_t src_sz, uint8_t flag)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_XMAC;
	hdr->fdm.i = IS_I(flag);
	hdr->fdm.c = IS_C(flag);
	return cop_sym_setup(crb, ctx, src, src_sz, NULL, 0, FN_AES);
}

/** @} AES */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_AES_H */
