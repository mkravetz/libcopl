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

/** @file aes.c 
 * Tests the Data Encyrption Standard (DES) and Advanced Encryption Standard 
 * (AES) functions. Demonstrates how to use the low level interfaces. 
 * Generates random iv/key for each test run, and iterates NUM_TESTS times */ 

#include <stdio.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/aes.h>
#include <libcopl/crypto/des.h>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#define NUM_TESTS 100

#define MAX_PACKETSIZE 8192
#define MAX_KEYSIZE 32
#define MAX_IVSIZE 32

#define MEMPRINT(name,length) { \
    long i; \
    for (i = 0; i < length; i++) { \
        if ((i % 16) == 0) { \
            if (i) \
                printf("\n"); \
            printf("%08lx: ", ((unsigned long)name + i));  \
        } \
        printf("%02x", *((char *)name + i)); \
        if (((i + 1) % 2) == 0) \
            printf(" "); \
    } \
    printf("\n"); \
}


void do_sw_cipher(const EVP_CIPHER * type, unsigned char *input,
	size_t input_sz, unsigned char *output, int *output_sz,
	unsigned char *key, unsigned char *iv, int enc)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(ctx);
	EVP_CipherInit_ex(ctx, type, NULL, key, iv, enc);
	EVP_CipherUpdate(ctx, output, output_sz, input, input_sz);
	EVP_CipherFinal_ex(ctx, output + *output_sz, output_sz);
	EVP_CIPHER_CTX_cleanup(ctx);
}

void generate_test_data(unsigned char *key, unsigned char *iv,
	unsigned char *data)
{
	BIGNUM *BN_key = BN_new();
	BIGNUM *BN_iv = BN_new();
	BIGNUM *BN_data = BN_new();
	BN_pseudo_rand(BN_key, MAX_KEYSIZE, -1, 0);
	BN_pseudo_rand(BN_iv, MAX_IVSIZE * 8, -1, 0);
	BN_pseudo_rand(BN_data, MAX_PACKETSIZE * 8, -1, 0);
	BN_bn2bin(BN_key, key);
	BN_bn2bin(BN_iv, iv);
	BN_bn2bin(BN_data, data);
	BN_free(BN_key);
	BN_free(BN_iv);
	BN_free(BN_data);
}

#define TEST_AES_ECB(name, ks, evp, enc) {\
	struct cop_sym_aes_ecb_cpb *cpb = cop_get_cpbptr(crb);\
	do_sw_cipher( evp(), data, data_len, ciphertext_sw, &ciphertext_sw_sz, key, iv, enc);\
	cop_ ## name ## _setup(crb, &ctx, data, data_len, ciphertext_hw, data_len,\
		enc, COP_RPAD_NONE, 0x00, ks);\
	memcpy(cpb->key, key, sizeof(cpb->key));\
	icswx(crb);\
	cop_csb_wait(cop_get_csbaddr(crb));\
	if (memcmp(ciphertext_hw, ciphertext_sw, data_len)) {\
		MEMPRINT(ciphertext_hw, 16); MEMPRINT(ciphertext_sw, 16);\
		printf(": memcmp failed\n"); fail++;\
	}\
	total++;\
}

#define TEST_AES_CBC(name, ks, evp, enc) {\
	struct cop_sym_aes_cbc_cpb *cpb = cop_get_cpbptr(crb);\
	do_sw_cipher( evp(), data, data_len, ciphertext_sw, &ciphertext_sw_sz, key, iv, enc);\
	cop_ ## name ## _setup(crb, &ctx, data, data_len, ciphertext_hw, data_len,\
		enc, COP_RPAD_NONE, 0x00, ks);\
	memcpy(cpb->key, key, sizeof(cpb->key));\
	memcpy(cpb->iv, iv, sizeof(cpb->iv));\
	icswx(crb);\
	cop_csb_wait(cop_get_csbaddr(crb));\
	if (memcmp(ciphertext_hw, ciphertext_sw, data_len)) {\
		MEMPRINT(ciphertext_hw, 16); MEMPRINT(ciphertext_sw, 16);\
		printf(": memcmp failed\n"); fail++;\
	}\
	memcpy(iv, cpb->cv, sizeof(cpb->cv));\
	total++;\
}

int main(int argc, char *argv[])
{
	int fail = 0;
	int total = 0;
	int rc = 0;

	/********************************************************************************/
	/* open copro and bind instance */

	struct cop_ctx ctx;
	uint64_t instances[4];
	cop_open(&ctx, COP_SYM_CRYPTO);
	cop_get_instances(&ctx, instances, sizeof(instances));
	cop_bind_instance(&ctx, instances[0]);

	/********************************************************************************/
	/* allocate memory */

	/* the coprocessor block size is the size of the crb/ccb/csb/cpb, the cpb in this 
	 * case will be 128 bytes since AES-CBC-256 has the largest CPB */
	size_t cb_size =
		sizeof(struct cop_crb) + sizeof(struct cop_csb) +
		sizeof(struct cop_sym_aes_ecb_cpb);

	/* we need a segment of memory for the plaintext (source) and ciphertext (target) 
	 * and for the c*bs */
	size_t segment_size = MAX_PACKETSIZE * 2 + cb_size;
	void *base = NULL;	/* base memory address */

	/* we align our memory to 128 bytes, because I would like to ensure that the crb 
	 * is aligned, this is heavily dependent on your own memory design */
	rc = posix_memalign(&base, COP_CRB_ALIGN, segment_size);
	if (!base) {
		fprintf(stderr, "posix_memalign failed, rc %d!\n", rc);
		exit(1);
	}

	/********************************************************************************/
	/* map memory */

	/* map our base memory + segment_size into the pbic based on the copro instance,
	 * in our case this will be in the crypto/comp PBIC. We can also specify flags
	 * to BOLT/MLOCK memory (won't be evicted).*/
	rc = cop_pbic_map(&ctx, base, segment_size,
		COP_MAP_BOLT | COP_MAP_MLOCK | COP_MAP_MADV_DONTFORK);
	if (rc) {
		fprintf(stderr, "cop_pbic_map failed!\n");
		exit(1);
	}

	/********************************************************************************/
	/* allocate memory */

	/* pack the crb/ccb/csb/cpb then source/target buffers together */
	struct cop_crb *crb = base;
	struct cop_csb *csb = base + sizeof(struct cop_crb);
	unsigned char *data = base + cb_size;
	unsigned char *ciphertext_hw = base + cb_size + MAX_PACKETSIZE;
	bzero(crb, cb_size);

	/* setup non-mapped stack memory */
	unsigned char key[32];
	unsigned char iv[32];
	unsigned char ciphertext_sw[MAX_PACKETSIZE];
	int ciphertext_sw_sz;
	unsigned int data_len = MAX_PACKETSIZE;

	/* set csb pointer, get cpb pointer */
	cop_set_csbaddr(crb, csb);

	/********************************************************************************/
	/* perform tests */

	int i;
	for (i = 0; i < NUM_TESTS; i++) {
		generate_test_data(key, iv, data);
		TEST_AES_ECB(aes_ecb, KS_AES_128, EVP_aes_128_ecb, 1);
		TEST_AES_ECB(aes_ecb, KS_AES_128, EVP_aes_128_ecb, 0);
		TEST_AES_ECB(aes_ecb, KS_AES_192, EVP_aes_192_ecb, 1);
		TEST_AES_ECB(aes_ecb, KS_AES_192, EVP_aes_192_ecb, 0);
		TEST_AES_ECB(aes_ecb, KS_AES_256, EVP_aes_256_ecb, 1);
		TEST_AES_ECB(aes_ecb, KS_AES_256, EVP_aes_256_ecb, 0);

		TEST_AES_CBC(aes_cbc, KS_AES_128, EVP_aes_128_cbc, 1);
		TEST_AES_CBC(aes_cbc, KS_AES_128, EVP_aes_128_cbc, 0);
		TEST_AES_CBC(aes_cbc, KS_AES_192, EVP_aes_192_cbc, 1);
		TEST_AES_CBC(aes_cbc, KS_AES_192, EVP_aes_192_cbc, 0);
		TEST_AES_CBC(aes_cbc, KS_AES_256, EVP_aes_256_cbc, 1);
		TEST_AES_CBC(aes_cbc, KS_AES_256, EVP_aes_256_cbc, 0);
	}

	/********************************************************************************/
	/* free memory */

	if (cop_pbic_unmap(&ctx, base, segment_size, COP_MAP_BOLT)) {
		fprintf(stderr, "cop_pbic_unmap failed!\n");
		exit(1);
	}
	free(base);

	printf("=== aes: %d/%d failures ===\n", fail, total);
	return 0;
}


