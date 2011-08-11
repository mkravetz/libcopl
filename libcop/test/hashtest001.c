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

#define do_hw_digest_oneshot(type, data, data_sz, md)\
{\
	uint64_t mbl = data_sz * 8;\
	cop_ ## type (data, data_sz, md, mbl, COP_FLAG_ONESHOT);\
}

#define do_hw_digest_multipass(type, data, data_sz, md)\
{\
	size_t bytes_left = data_sz;\
	size_t bytes_done = 0;\
	uint64_t mbl;\
	size_t chunk = 128;\
	cop_ ## type (data, chunk, md, NULL, COP_FLAG_FIRST);\
	bytes_left -= chunk;\
	bytes_done += chunk;\
	while (bytes_left >= chunk){\
		cop_ ## type (data + bytes_done, chunk, md, NULL, COP_FLAG_MIDDLE);\
		bytes_left -= chunk;\
		bytes_done += chunk;\
	}\
	mbl = data_sz * 8;\
	cop_ ## type (data + bytes_done, bytes_left, md, mbl, COP_FLAG_LAST);\
}

#define HASH_TEST(type, size, pass) {\
	memset(digest_sw, 0x00, size);\
	do_sw_digest((const EVP_MD *)EVP_ ##type(), data, data_len, digest_sw);\
	memset(digest_hw, 0x00, size);\
	do_hw_digest_ ##pass(type, data, data_len, digest_hw);\
	if (memcmp(digest_sw, digest_hw, size)) {\
		printf(#type " " #pass " " #size " FAILED!\n");\
		fail++;\
	}\
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
		HASH_TEST(sha1, 20, oneshot);
		HASH_TEST(sha256, 32, oneshot);
		HASH_TEST(sha512, 64, oneshot);
		HASH_TEST(md5, 16, oneshot);
		HASH_TEST(sha1, 20, multipass);
		HASH_TEST(sha256, 32, multipass);
		HASH_TEST(sha512, 64, multipass);
		HASH_TEST(md5, 16, multipass);
	}

	cop_free(cc_pool, data);
	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;
}
