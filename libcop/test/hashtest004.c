/* 
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp
 */

#include <stdio.h>
#include <libcopmalloc.h>
#include <libcop.h>

#include <openssl/crypto.h>
#include <openssl/evp.h>

#include "../test/utils.h"

#define NUM_TESTS 100

do_sw_digest(const EVP_MD * type, void *data, size_t data_len, void *md)
{
	EVP_MD_CTX *ctx = EVP_MD_CTX_create();
	EVP_DigestInit(ctx, type);
	EVP_DigestUpdate(ctx, data, data_len);
	EVP_DigestFinal(ctx, md, NULL);
	EVP_MD_CTX_cleanup(ctx);
	EVP_MD_CTX_destroy(ctx);
}

#define HASH_TEST(type, size) {\
	struct cop_crb *crb;\
	volatile struct cop_csb *csb;\
	memset(digest_sw, 0x00, size);\
	do_sw_digest((const EVP_MD *)EVP_ ##type(), data, data_len, digest_sw);\
	memset(digest_hw, 0x00, size);\
	cop_ ## type ##_async (data, data_len, digest_hw, NULL, COP_FLAG_ONESHOT, ASYNC_INTERRUPT, &crb);\
	csb=cop_get_csbaddr(crb);\
	while(csb->cw.valid == 0);\
	cop_symcrypto_cpb_ ##type *cpb = cop_get_cpbptr(crb);\
	memcpy(digest_hw, cpb->md, size);\
	if (memcmp(digest_sw, digest_hw, size)) {\
		printf("async " #type " " #size " FAILED!\n");\
		fail++;\
	}\
	cop_give_back_crb(cop_cc_pool(), crb, COP_SYM_CRYPTO);\
	total++;\
}

#define PACKET_SIZE 8192

int main(int argc, char **argv)
{
	int fail = 0;
	int total = 0;
	int i = 0;

	mapped_memory_pool_t cc_pool = cop_cc_pool();
	void *data = (unsigned char *)cop_malloc(cc_pool, PACKET_SIZE, 1);
	unsigned char digest_sw[64];
	unsigned char digest_hw[64];
	memset(data, 0xA5, PACKET_SIZE);
	printf("Please be patient with this test...\n");

	/* pick random size */
	for (i = 0; i < NUM_TESTS; i++) {
		size_t data_len = rand() % 8064 + 128;
		/* FIXME
		HASH_TEST(sha1, 20);
		HASH_TEST(sha256, 32);
		HASH_TEST(sha512, 64);
		HASH_TEST(md5, 16);
		*/
	}

	cop_free(cc_pool, data);
	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;
}
