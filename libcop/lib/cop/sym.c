/*  
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008, 2009, 2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include "config.h"
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcopmanage.h"
#include "libcop.h"

#include "./libcopl/crypto/aes.h"
#include "./libcopl/crypto/des.h"
#include "./libcopl/crypto/rc4.h"
#include "./libcopl/crypto/hash.h"
#include "./libcopl/crypto/hmac.h"

extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];

/* *INDENT-OFF* */

const uint8_t cop_crypto_sha1_length0[] = {
	0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef,
	0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09
};

const uint8_t cop_crypto_sha256_length0[] = {
	0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8,
	0x99, 0x6f, 0xb9, 0x24, 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
	0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
};

const uint8_t cop_crypto_sha512_length0[] = {
	0xcf, 0x83, 0xe1, 0x35, 0x7e, 0xef, 0xb8, 0xbd, 0xf1, 0x54, 0x28, 0x50,
	0xd6, 0x6d, 0x80, 0x07, 0xd6, 0x20, 0xe4, 0x05, 0x0b, 0x57, 0x15, 0xdc,
	0x83, 0xf4, 0xa9, 0x21, 0xd3, 0x6c, 0xe9, 0xce, 0x47, 0xd0, 0xd1, 0x3c,
	0x5d, 0x85, 0xf2, 0xb0, 0xff, 0x83, 0x18, 0xd2, 0x87, 0x7e, 0xec, 0x2f,
	0x63, 0xb9, 0x31, 0xbd, 0x47, 0x41, 0x7a, 0x81, 0xa5, 0x38, 0x32, 0x7a,
	0xf9, 0x27, 0xda, 0x3e
};

const uint8_t cop_crypto_md5_length0[] = {
	0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98,
	0xec, 0xf8, 0x42, 0x7e
};

////////////////////////////////////////////////////////////////////////////////

/* hash functions */

#define SYM_MBL_8 cpb->mbl = mbl;
#define SYM_MBL_16 cpb->mbl[0] = 0; cpb->mbl[1] = mbl;

#define SYM_HASH_SYNC(name, mbl) {\
	if (flag == COP_FLAG_ONESHOT && length == 0) {\
		memcpy(digest, cop_crypto_ ##name## _length0,\
			sizeof(cop_crypto_ ##name## _length0));\
		return 0;\
	}\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ## name ## _setup(crb, NULL, source, length, flag);\
	struct cop_sym_ ## name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if(cpb->hdr.fdm.c)\
		memcpy(cpb->pd, digest, sizeof(cpb->pd));\
	if(!(cpb->hdr.fdm.i)) {\
		SYM_MBL_ ## mbl \
	}\
	int rc = cop_dispatch_and_wait(crb);\
	memcpy(digest, cpb->md, sizeof(cpb->md));\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_md5(const void *source, size_t length, void *digest, uint64_t mbl, uint8_t flag)
{
	SYM_HASH_SYNC(md5, 8)
}

int cop_sha1(const void *source, size_t length, void *digest, uint64_t mbl, uint8_t flag)
{
	SYM_HASH_SYNC(sha1, 8)
}

int cop_sha256(const void *source, size_t length, void *digest, uint64_t mbl, uint8_t flag)
{
	SYM_HASH_SYNC(sha256, 8)
}

int cop_sha512(const void *source, size_t length, void *digest, uint64_t mbl, uint8_t flag)
{
	SYM_HASH_SYNC(sha512, 16)
}

/* sha hmac functions */

#define SYM_SHA_HMAC_SYNC(name, mbl) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, source, length, flag);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if(key && !cpb->hdr.fdm.i)\
		memcpy(cpb->key, key, sizeof(cpb->key));\
	if(mac && cpb->hdr.fdm.c)\
		memcpy(cpb->pm, mac, sizeof(cpb->pm));\
	if(!(cpb->hdr.fdm.i)) {\
		SYM_MBL_ ## mbl \
	}\
	int rc = cop_dispatch_and_wait(crb);\
	memcpy(mac, cpb->mac, sizeof(cpb->mac));\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_sha1_hmac(const void *source, size_t length, void *mac,
		     uint64_t mbl, void *key, uint8_t flag)
{
	SYM_SHA_HMAC_SYNC(sha1_hmac, 8)
}

int cop_sha256_hmac(const void *source, size_t length, void *mac,
		       uint64_t mbl, void *key, uint8_t flag)
{
	SYM_SHA_HMAC_SYNC(sha256_hmac, 8)
}

int cop_sha512_hmac(const void *source, size_t length, void *mac,
		       uint64_t mbl, void *key, uint8_t flag)
{
	SYM_SHA_HMAC_SYNC(sha512_hmac, 16)
}

/* arc4 functions */

#define SYM_RC4_SYNC(name) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, indata, length, outdata, length, flag);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if(state && x_val && y_val && cpb->hdr.fdm.c){\
		cpb->x_val = *x_val;\
		cpb->y_val = *y_val;\
		memcpy(cpb->state, state, sizeof(cpb->state));\
	}\
	if(key && !cpb->hdr.fdm.c)\
		memcpy(cpb->key, key, sizeof(cpb->key));\
	int rc = cop_dispatch_and_wait(crb);\
	if(state && x_val && y_val && cpb->hdr.fdm.i){\
		*x_val = cpb->x_val;\
		*y_val = cpb->y_val;\
		memcpy(state, cpb->state, sizeof(cpb->state));\
	}\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_rc4_40(const void *key, size_t length, const void *indata,
		  void *outdata, void *state, uint8_t *x_val, uint8_t *y_val,
		  uint8_t flag)
{
	SYM_RC4_SYNC(rc4_40)
}

int cop_rc4_64(const void *key, size_t length, const void *indata,
		  void *outdata, void *state, uint8_t *x_val, uint8_t *y_val,
		  uint8_t flag)
{
	SYM_RC4_SYNC(rc4_64)
}

int cop_rc4_128(const void *key, size_t length, const void *indata,
		  void *outdata, void *state, uint8_t *x_val, uint8_t *y_val,
		   uint8_t flag)
{
	SYM_RC4_SYNC(rc4_128)
}

/* des functions */

#define SYM_DES_ECB_SYNC(name) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, input, input_sz, output, input_sz, enc, pad_rules, pad_byte);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if (key)\
		memcpy(cpb->key, key, sizeof(cpb->key));\
	int rc = cop_dispatch_and_wait(crb);\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_des_ecb(const void *input, size_t input_sz, void *output,
		   void *key, uint8_t enc, uint8_t pad_rules,
		   uint8_t pad_byte)
{
	SYM_DES_ECB_SYNC(des_ecb)
}

int cop_tdes_ecb(const void *input, size_t input_sz, void *output,
		    void *key, uint8_t enc, uint8_t pad_rules,
		    uint8_t pad_byte)
{
	SYM_DES_ECB_SYNC(tdes_ecb)
}

#define SYM_DES_CBC_SYNC(name) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, input, input_sz, output, input_sz, enc, pad_rules, pad_bytes);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if (key)\
		memcpy(cpb->key, key, sizeof(cpb->key));\
	if (iv)\
		memcpy(cpb->iv, iv, sizeof(cpb->iv));\
	int rc = cop_dispatch_and_wait(crb);\
	if (iv)\
		memcpy(iv, cpb->cv, sizeof(cpb->cv));\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_des_cbc(const void *input, size_t input_sz, void *output,
		   void *key, void *iv, uint8_t enc,
		   uint8_t pad_rules, uint8_t pad_bytes)
{
	SYM_DES_CBC_SYNC(des_cbc)
}

int cop_tdes_cbc(const void *input, size_t input_sz, void *output,
		    void *key, void *iv, uint8_t enc,
		    uint8_t pad_rules, uint8_t pad_bytes)
{
	SYM_DES_CBC_SYNC(tdes_cbc)
}

/* aes functions */

#define SYM_AES_ECB_SYNC(name, size) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, input, input_sz, output, input_sz, \
		enc, pad_rules, pad_byte, KS_AES_ ## size);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if (key)\
		memcpy(cpb->key, key, size/8);\
	int rc = cop_dispatch_and_wait(crb);\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_aes_ecb_128(const void *input, size_t input_sz, void *output,
		       void *key, uint8_t enc, uint8_t pad_rules,
		       uint8_t pad_byte)
{
	SYM_AES_ECB_SYNC(aes_ecb, 128)
}

int cop_aes_ecb_192(const void *input, size_t input_sz, void *output,
		       void *key, uint8_t enc, uint8_t pad_rules,
		       uint8_t pad_byte)
{
	SYM_AES_ECB_SYNC(aes_ecb, 192)
}

int cop_aes_ecb_256(const void *input, size_t input_sz, void *output,
		       void *key, uint8_t enc, uint8_t pad_rules,
		       uint8_t pad_byte)
{
	SYM_AES_ECB_SYNC(aes_ecb, 256)
}

#define SYM_AES_CBC_SYNC(name, size) {\
	mapped_memory_pool_t cc_pool = cop_cc_pool();\
	struct cop_crb *crb = cop_next_crb(cc_pool,COP_SYM_CRYPTO);\
	cop_ ##name ## _setup(crb, NULL, input, input_sz, output, input_sz, \
		enc, pad_rules, pad_byte, KS_AES_ ## size);\
	struct cop_sym_ ##name ## _cpb *cpb = cop_get_cpbptr(crb);\
	if (key)\
		memcpy(cpb->key, key, size/8);\
	if (iv)\
		memcpy(cpb->iv, iv, sizeof(cpb->iv));\
	int rc = cop_dispatch_and_wait(crb);\
	if (iv)\
		memcpy(iv, cpb->cv, sizeof(cpb->cv));\
	cop_give_back_crb(cc_pool, crb, COP_SYM_CRYPTO);\
	return rc;\
}

int cop_aes_cbc_128(const void *input, size_t input_sz, void *output,
		       void *key, void *iv, uint8_t enc,
		       uint8_t pad_rules, uint8_t pad_byte)
{
	SYM_AES_CBC_SYNC(aes_cbc, 128)
}

int cop_aes_cbc_192(const void *input, size_t input_sz, void *output,
		       void *key, void *iv, uint8_t enc,
		       uint8_t pad_rules, uint8_t pad_byte)
{
	SYM_AES_CBC_SYNC(aes_cbc, 192)
}

int cop_aes_cbc_256(const void *input, size_t input_sz, void *output,
		       void *key, void *iv, uint8_t enc,
		       uint8_t pad_rules, uint8_t pad_byte)
{
	SYM_AES_CBC_SYNC(aes_cbc, 256)
}


/* *INDENT-ON* */
