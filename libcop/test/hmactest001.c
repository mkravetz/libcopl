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

 *
 * shatest002
 *
 * This test covers hmac
 *
 */

#include <stdio.h>
#include <libcopmalloc.h>
#include <libcop.h>

#include <openssl/crypto.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>

#include "../test/data/symdata.h"
#include "../test/utils.h"

int digest_size[3] = { 20, 32, 64 };
int block_size[3] = { 64, 64, 128 };

void hmac_test(char *name, int index)
{
	unsigned char digest_sw[64];
	unsigned char digest_hw[64];
	unsigned char digest_hw_multi[64];
	unsigned long mbl[2];
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	char *src = cop_malloc(cc_pool, data_src_len, 1);
	char *orig_src = src;
	memcpy(src, data_src, data_src_len);
	int fail = 0;
	int i;
	char key[128];
	int bs = block_size[index - 1];
	int len;

	/* generate key */
	for (i = 0; i < 128; i++)
		key[i] = i;

	/* calculate openssl sw hash */
	memset(digest_sw, 0, digest_size[index - 1]);
	HMAC(EVP_sha1(), key, 64, src, data_src_len, digest_sw, &len);
	//printf("\nsw %s digest:\n", name);
	//test_memprint(digest_sw, digest_size[index - 1]);

	/* test normal sha-hmac functionality */
	memset(digest_hw, 0, digest_size[index - 1]);
	cop_sha1_hmac(src, data_src_len, digest_hw, NULL, key,
		      COP_FLAG_ONESHOT);
	//printf("\nhw %s digest:\n", name);
	//test_memprint(digest_hw, digest_size[index - 1]);

	/* test normal sha-hmac multi-crb functionality */
	memset(digest_hw_multi, 0, digest_size[index - 1]);
	cop_sha1_hmac(src, bs, digest_hw_multi, NULL, key, COP_FLAG_FIRST);
	src += bs;
	for (i = bs; i < (data_src_len - bs); i += bs) {
		cop_sha1_hmac(src, bs, digest_hw_multi, NULL, key,
			      COP_FLAG_MIDDLE);
		src += bs;
	}
	mbl[0] = data_src_len * 8;
	mbl[1] = 0;
	if (index == 3) {
		mbl[0] = 0;
		mbl[1] = data_src_len * 8;
	}
	cop_sha1_hmac(src, data_src_len - i, digest_hw_multi, &mbl, key,
		      COP_FLAG_LAST);
	//printf("\nhw multi-crb %s digest:\n", name);
	//test_memprint(digest_hw_multi, digest_size[index - 1]);

	/* check against software */
	if (memcmp(digest_sw, digest_hw, digest_size[index - 1]) ||
	    memcmp(digest_sw, digest_hw_multi, digest_size[index - 1]))
		fail++;

	cop_free(cc_pool, orig_src);
}

int main(int argc, char **argv)
{
	int fail = 0;
	hmac_test("hmac-sha1", 1);
	//hmac_test("hmac-sha256", 2);
	//hmac_test("hmac-sha512", 3);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 3);
	return fail;
}
