/*  
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_CRYPTO_H
#define _LIB_LIBCOPL_CRYPTO_H

#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

extern struct cop_directive_info cop_dir_info[];

/** @defgroup CRYPTO Cryptographic coprocessor interfaces
 * @{ */

/* TODO: parameters that require use of a flag should
 * only access this type of enumerator */
enum cop_sym_flag
{
	COP_FLAG_FIRST = 0x01,
	COP_FLAG_LAST = 0x02,
	COP_FLAG_MIDDLE = 0x03,
	COP_FLAG_ONESHOT = 0x00,
};

#define COP_RPAD_NONE		0x00
#define COP_RPAD_ZERO		0x01
#define COP_RPAD_INC		0x02
#define COP_RPAD_USE		0x03

#define FN_AES			0x00
#define FN_DES			0x01
#define FN_SHA			0x02
#define FN_MD5			0x03
#define FN_RC4			0x04
#define FN_KAS			0x05
#define FN_AES_HMAC		0x06
#define FN_TDES_HMAC		0x07

#define MODE_MD5		0x00
#define MODE_SHA		0x00
#define MODE_SHA_HMAC		0x01
#define MODE_RC4		0x00
#define MODE_DES_ECB		0x00
#define MODE_DES_CBC		0x01
#define MODE_TDES_ECB		0x02
#define MODE_TDES_CBC		0x03
#define MODE_AES_ECB		0x00
#define MODE_AES_CBC		0x01
#define MODE_AES_GMAC		0x02
#define MODE_AES_GCA		0x03
#define MODE_AES_GCM		0x04
#define MODE_AES_CCA		0x05
#define MODE_AES_CCM		0x06
#define MODE_AES_CTR		0x07
#define MODE_AES_XMAC		0x14
#define MODE_KAS_F8		0x01
#define MODE_KAS_F9		0x02
#define MODE_AES_CBC_ETA	0x08
#define MODE_AES_CBC_ATE	0x09
#define MODE_AES_CBC_EAA	0x0A
#define MODE_AES_CTR_ETA	0x0C
#define MODE_AES_CTR_ATE	0x0D
#define MODE_AES_CTR_EAA	0x0E
#define MODE_TDES_CBC_ETA	0x10
#define MODE_TDES_CBC_ATE	0x11
#define MODE_TDES_CBC_EAA	0x12

#define DS_MD5			0x00
#define DS_SHA_1		0x01
#define DS_SHA_256		0x02
#define DS_SHA_512		0x03
#define DS_HMAC_160		0x01
#define DS_HMAC_256		0x02
#define DS_HMAC_512		0x03

#define KS_RC4_40		0x01
#define KS_RC4_64		0x02
#define KS_RC4_128		0x03
#define KS_AES_128		0x01
#define KS_AES_192		0x02
#define KS_AES_256		0x03
#define KS_AES128_HMAC		0x01
#define KS_AES192_HMAC		0x02
#define KS_AES256_HMAC		0x03

#define IS_C(flag) ( flag & 0x2 ? 1: 0 )
#define IS_I(flag) ( flag & 0x1 ? 1: 0 )

/**
 * @brief FDM byte structure
 */
struct cop_sym_cpb_fdm {
	uint8_t ende:1; /**< @brief 1=encrypt 0=decrypt */
	uint8_t _rsv0:1;
	uint8_t pr:2; /**< @brief padding rules, 01 rpad zeros, 10 rpad incrementally, 11 rpad concatentate */
	uint8_t _rsv1:2;
	uint8_t c:1; /**< @brief continuation pass bit */
	uint8_t i:1; /**< @brief intermediate pass bit */
};

/**
 * @brief symmetric crypto CPB header
 */
struct cop_sym_cpb_hdr {
	uint8_t mode; /**< @brief specific algorithm mode. */
	struct cop_sym_cpb_fdm fdm; /**< @brief FDM structure */
	uint8_t ks:4; /**< @brief keysize number */
	uint8_t ds:4; /**< @brief digest size number */
	uint8_t rpad; /**< @brief rpad padding value */
	uint8_t _rsv0[12];
};

/**
 * @brief Symmetric cryptographic CRB setup function.
 *
 * Takes a valid CRB with a valid CSB address and sets the @a target and 
 * @a source addresses. Sets the coprocessor directive as well.
 *
 * @pre ctx has been setup using @a cop_open_bind_local or a combination
 * 	as described in examples
 *
 * @param crb a valid coprocessor request block
 * @param ctx cop_ctx structure from open/bound copro interface
 * @param source pointer to source data
 * @param source_size size in bytes of source data
 * @param target pointer to target data
 * @param target_size size in bytes of target data
 * @param cd coprocessor directive, describes function type
 *
 */
static inline int cop_sym_setup(struct cop_crb * crb, struct cop_ctx * ctx,
	const void *source, size_t source_size, void *target,
	size_t target_size, uint8_t cd)
{
	if (!crb) return -1;

	/* setup crb */
	cop_crb_set_ccw_fc_copid(crb, COP_SYM_CRYPTO, cd);
	cop_set_sdptr(crb, source);
	crb->src_dde.bytes = (uint64_t) source_size;
	cop_set_tdptr(crb, target);
	crb->tgt_dde.bytes = (uint64_t) target_size;

	/* reset csb */
	struct cop_csb *csb = cop_get_csbaddr(crb);
	if (!csb) return -1;
	cop_csb_reset(csb);

	/* setup context if necessary */
	if (ctx) {
		cop_crb_set_ccw_type(crb, ctx);
		cop_crb_set_ccw_inst(crb, ctx);
	}

	return 0;
}

/** @} */

#endif	/* _LIB_LIBCOPL_CRYPTO_H */
