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

#ifndef _INCLUDE_LIBCOPL_CRYPTO_AES_HMAC_H
#define _INCLUDE_LIBCOPL_CRYPTO_AES_HMAC_H

#include <libcopl/crypto.h>

/**
 * @ingroup CRYPTO
 * @defgroup AES_HMAC AES and Hashing Message Authentication Code combination mode interfaces.
 * @{
 */

/**
 * @brief CPB structure for  AES(CBC,CTR)/HMAC combo mode using SHA-1, 256 bytes long.
 **/
struct cop_sym_aes_sha1_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mblp;	/**< @brief Bit length of message plus block */
	uint8_t _rsv0[8];
	uint8_t hkey[64];   /**< @brief Hmac key */
	uint8_t _rsv1[32];
	uint8_t aadl[2];   /**< @brief Additional authentication length */
	uint8_t _rsv2[14];
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint8_t ckey[32];   /**< @brief Cipher key */
	uint8_t mac[20];   /**< @brief Message authentication code */
	uint8_t _rsv3[12];
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv4[28];
};

/**
 * @brief aes_sha1_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_sha1_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_sha1_hmac_cpb cpb; /**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for  AES(CBC,CTR)/HMAC combo mode using SHA-256, 256 bytes long.
 **/
struct cop_sym_aes_sha256_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mblp;	/**< @brief Bit length of message plus block */
	uint8_t _rsv0[8];
	uint8_t hkey[64];   /**< @brief Hmac key */
	uint8_t _rsv1[32];
	uint8_t aadl[2];   /**< @brief Additional authentication length */
	uint8_t _rsv2[14];
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint8_t ckey[32];   /**< @brief Cipher key */
	uint8_t mac[32];   /**< @brief Message authentication code */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief aes_sha256_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_sha256_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_sha256_hmac_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief CPB structure for AES(CBC,CTR)/HMAC combo mode using SHA-512, 416 bytes long.
 **/
struct cop_sym_aes_sha512_hmac_cpb {
	struct cop_sym_cpb_hdr hdr; /**< @brief Common symmetric-crypto header. */
	uint64_t mblp[2];  /**< @brief Bit length of message plus block */
	uint8_t hkey[128];   /**< @brief Hmac key */
	uint8_t _rsv0[64];
	uint8_t aadl[2];   /**< @brief Additional authentication length */
	uint8_t _rsv1[14];
	uint8_t iv[16];	  /**< @brief Initialization vector */
	uint8_t ckey[32];   /**< @brief Cipher key */
	uint8_t _rsv2[32];
	uint8_t mac[64];   /**< @brief Message authentication code */
	uint32_t spbc; /**< @brief Source Pointer Byte Count */
	uint8_t _rsv3[28];
};

/**
 * @brief aes_sha512_hmac CRB/CCB/CSB/CPB helper structure.
 **/
struct cop_sym_aes_sha512_hmac_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[16]; /**<  @brief Padding to ensure alignment. */
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	struct cop_sym_aes_sha512_hmac_cpb cpb;	/**< @brief Coprocessor Parameter block. */
};

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha1_hmac_eta.
 *
 * Setup function for aes_cbc_sha1_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
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
static inline int cop_aes_cbc_sha1_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ETA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha256_hmac_eta.
 *
 * Setup function for aes_cbc_sha256_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
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
static inline int cop_aes_cbc_sha256_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ETA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha512_hmac_eta.
 *
 * Setup function for aes_cbc_sha512_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
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
static inline int cop_aes_cbc_sha512_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ETA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha1_hmac_eta.
 *
 * Setup function for aes_ctr_sha1_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha1_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ETA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha256_hmac_eta.
 *
 * Setup function for aes_ctr_sha256_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha256_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ETA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha512_hmac_eta.
 *
 * Setup function for aes_ctr_sha512_hmac_eta.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha512_hmac_eta_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ETA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha1_hmac_ate.
 *
 * Setup function for aes_cbc_sha1_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
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
static inline int cop_aes_cbc_sha1_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ATE;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha256_hmac_ate.
 *
 * Setup function for aes_cbc_sha256_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
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
static inline int cop_aes_cbc_sha256_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ATE;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha512_hmac_ate.
 *
 * Setup function for aes_cbc_sha512_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
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
static inline int cop_aes_cbc_sha512_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_ATE;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha1_hmac_ate.
 *
 * Setup function for aes_ctr_sha1_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha1_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ATE;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha256_hmac_ate.
 *
 * Setup function for aes_ctr_sha256_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha256_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ATE;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha512_hmac_ate.
 *
 * Setup function for aes_ctr_sha512_hmac_ate.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha512_hmac_ate_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_ATE;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha1_hmac_eaa.
 *
 * Setup function for aes_cbc_sha1_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
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
static inline int cop_aes_cbc_sha1_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_EAA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha256_hmac_eaa.
 *
 * Setup function for aes_cbc_sha256_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
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
static inline int cop_aes_cbc_sha256_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_EAA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_cbc_sha512_hmac_eaa.
 *
 * Setup function for aes_cbc_sha512_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
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
static inline int cop_aes_cbc_sha512_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t pad, uint8_t rpad, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CBC_EAA;
	hdr->fdm.ende = ende;
	hdr->fdm.pr = pad;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	hdr->rpad = rpad;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha1_hmac_eaa.
 *
 * Setup function for aes_ctr_sha1_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA1_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha1_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_EAA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_1;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha256_hmac_eaa.
 *
 * Setup function for aes_ctr_sha256_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA256_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha256_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_EAA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_256;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/**
 * @brief Setup required coprocessor request block fields for aes_ctr_sha512_hmac_eaa.
 *
 * Setup function for aes_ctr_sha512_hmac_eaa.
 * @a crb must be setup before and have a valid CSB pointer. The source
 * @a src data must be mapped for that particular coprocessor before.
 *
 * @pre @a crb must be valid, and the csb field must be a valid and 
 * 	mapped block, directly after the csb must follow enough
 *	mapped memory to accommodate sizeof(AES_SHA512_HMAC).
 * @pre @a ctx must be a valid ctx which has already been opened, if NULL is given
 *	this field will not be set, which is useful for reusing CRBs.
 * @param[in] crb a coprocessor request block
 * @param[in] ctx cop_ctx structure from open/bound copro interface
 * @param[in] src pointer to source buffer
 * @param[in] src_sz size in bytes of source data
 * @param[in] tgt pointer to target buffer
 * @param[in] tgt_sz size of target buffer in bytes
 * @param[in] enc 1 = encryption, 0 = decryption
 * @param[in] ks key size  {KS_AES_128, KS_AES_192, KS_AES_256}
 *
 * Returns -1 on failure, 0 on success.
 */
static inline int cop_aes_ctr_sha512_hmac_eaa_setup(struct cop_crb *crb,
	struct cop_ctx *ctx, const void *src, size_t src_sz, void *tgt,
	size_t tgt_sz, uint8_t ende, uint8_t ks)
{
	struct cop_sym_cpb_hdr *hdr =
		(struct cop_sym_cpb_hdr *)cop_get_cpbptr(crb);
	hdr->mode = MODE_AES_CTR_EAA;
	hdr->fdm.ende = ende;
	hdr->ks = ks;
	hdr->ds = DS_SHA_512;
	return cop_sym_setup(crb, ctx, src, src_sz, tgt, tgt_sz, FN_AES_HMAC);
}

/** @} AES_HMAC */
#endif				/* _INCLUDE_LIBCOPL_CRYPTO_AES_HMAC_H */
