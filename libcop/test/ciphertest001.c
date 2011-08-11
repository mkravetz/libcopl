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
#include <openssl/bn.h>
#include <openssl/evp.h>
#include "../test/utils.h"

#define NUM_TESTS 100

do_sw_cipher(const EVP_CIPHER * type, unsigned char *input,
	     size_t input_sz, unsigned char *output, int *output_sz,
	     unsigned char *key, unsigned char *iv, int enc)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(ctx);
	EVP_CIPHER_CTX_set_padding(ctx, 0);
	EVP_CipherInit(ctx, type, key, iv, enc);
	EVP_CipherUpdate(ctx, output, output_sz, input, input_sz);
	EVP_CipherFinal(ctx, output + *output_sz, output_sz);
	EVP_CIPHER_CTX_cleanup(ctx);
}

#define TEST_ECB(name, evp, enc) {\
	ciphertext_sw_sz = data_len;\
	do_sw_cipher( evp(), data, data_len, ciphertext_sw, &ciphertext_sw_sz,\
		key, NULL, enc);\
	if (cop_ ##name ( data, data_len,\
		ciphertext_hw, key, enc, COP_RPAD_NONE, 0x00)) {\
		printf(#name ": call failed\n"); fail++;\
	}\
	total++;\
	if (memcmp(ciphertext_hw, ciphertext_sw, data_len)){\
		printf(#name ": memcmp failed\n"); fail++;\
		test_memprint(ciphertext_sw, data_len);\
		printf("--\n");\
		test_memprint(ciphertext_hw, data_len);\
	}\
	total++;\
}

#define TEST_CBC(name, evp, enc) {\
	do_sw_cipher( evp(), data, data_len, ciphertext_sw, &ciphertext_sw_sz,\
		key, iv, enc);\
	if (cop_ ##name ( data, data_len,\
		ciphertext_hw, key, iv, enc, COP_RPAD_NONE, 0x00)) {\
		printf(#name ": call failed\n"); fail++;\
	}\
	total++;\
	if (memcmp(ciphertext_hw, ciphertext_sw, data_len)){\
		printf(#name ": memcmp failed\n"); fail++;\
		test_memprint(ciphertext_sw, data_len);\
		printf("--\n");\
		test_memprint(ciphertext_hw, data_len);\
	}\
	total++;\
}

#define TEST_RC4(name, evp) {\
	do_sw_cipher( evp(), data, data_len, ciphertext_sw, &ciphertext_sw_sz,\
		key, NULL, 0);\
	if (cop_ ## name ( key, data_len, data, ciphertext_hw,\
		NULL, NULL, NULL, COP_FLAG_ONESHOT)) {\
		printf(#name ": call failed\n"); fail++;\
	}\
	total++;\
	if (memcmp(ciphertext_hw, ciphertext_sw, data_len)){\
		printf(#name, ": memcmp failed\n"); fail++;\
	}\
	total++;\
}

int generate_test_data(unsigned char *key, unsigned char *iv,
		       unsigned char *data)
{
	BIGNUM *BN_key = BN_new();
	BIGNUM *BN_iv = BN_new();
	BIGNUM *BN_data = BN_new();
	BN_CTX *ctx = BN_CTX_new();
	BN_pseudo_rand(BN_key, 256, -1, 0);
	BN_pseudo_rand(BN_iv, 256, -1, 0);
	BN_pseudo_rand(BN_data, 65536, -1, 0);
	BN_bn2bin(BN_key, key);
	BN_bn2bin(BN_key, iv);
	BN_bn2bin(BN_data, data);
	BN_free(BN_key);
	BN_free(BN_iv);
	BN_free(BN_data);
	BN_CTX_free(ctx);
}

#define PACKET_SIZE 8192

int main(int argc, char **argv)
{
	int fail = 0;
	int total = 0;
	int i = 0;

	mapped_memory_pool_t cc_pool = cop_cc_pool();
	unsigned char key[32];
	unsigned char iv[32];
	unsigned char ciphertext_sw[PACKET_SIZE];
	unsigned char *data = (unsigned char *)cop_malloc(cc_pool, PACKET_SIZE, 1);
	unsigned char *ciphertext_hw = (unsigned char *)cop_malloc(cc_pool, PACKET_SIZE, 1);
	int ciphertext_sw_sz;
	int ciphertext_hw_sz;
	int data_len = PACKET_SIZE;

	/* TODO: this only tests modulo blocksize operations
	 * in the future we need to do padding tests with non
	 * modulo blocksize inputs */
	for (i = 0; i < NUM_TESTS; i++) {
		data_len = ((rand() % (PACKET_SIZE/16)) * 16);
		generate_test_data(data, key, iv);
		TEST_ECB(aes_ecb_128, EVP_aes_128_ecb, 1)
		TEST_ECB(aes_ecb_192, EVP_aes_192_ecb, 1)
		TEST_ECB(aes_ecb_256, EVP_aes_256_ecb, 1)
		TEST_ECB(aes_ecb_128, EVP_aes_128_ecb, 0)
		TEST_ECB(aes_ecb_192, EVP_aes_192_ecb, 0)
		TEST_ECB(aes_ecb_256, EVP_aes_256_ecb, 0)

		TEST_CBC(aes_cbc_128, EVP_aes_128_cbc, 1)
		TEST_CBC(aes_cbc_192, EVP_aes_192_cbc, 1)
		TEST_CBC(aes_cbc_256, EVP_aes_256_cbc, 1)
		TEST_CBC(aes_cbc_128, EVP_aes_128_cbc, 0)
		TEST_CBC(aes_cbc_192, EVP_aes_192_cbc, 0)
		TEST_CBC(aes_cbc_256, EVP_aes_256_cbc, 0)
		
		TEST_ECB(des_ecb, EVP_des_ecb, 1);
		TEST_ECB(tdes_ecb, EVP_des_ede3, 1);
		TEST_ECB(des_ecb, EVP_des_ecb, 0);
		TEST_ECB(tdes_ecb, EVP_des_ede3, 0);
		
		TEST_CBC(des_cbc, EVP_des_cbc, 1);
		TEST_CBC(tdes_cbc, EVP_des_ede3_cbc, 1);
		TEST_CBC(des_cbc, EVP_des_cbc, 0);
		TEST_CBC(tdes_cbc, EVP_des_ede3_cbc, 0);
		
		/* don't run rc4 test due to DD1 errata */
		if (gDD_version > 1) {
			TEST_RC4(rc4_40, EVP_rc4_40);
			TEST_RC4(rc4_128, EVP_rc4);
		}
	}

	cop_free(cc_pool, data);
	cop_free(cc_pool, ciphertext_hw);
	printf("=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;
}
