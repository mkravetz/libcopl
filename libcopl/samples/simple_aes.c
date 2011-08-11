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
#include <stdlib.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/aes.h>

#define MAX_PACKETSIZE 8192
#define MAX_KEYSIZE 32

int main(int argc, char *argv[])
{
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
	 * case will be 128 bytes since AES-ECB-256 has the largest CPB */
	size_t cb_size =
		sizeof(struct cop_crb) + sizeof(struct cop_csb) +
		sizeof(struct cop_sym_aes_ecb_cpb);

	/* we need a segment of memory for the plaintext (source) and ciphertext (target) 
	 * and for the c*bs */
	size_t segment_size = MAX_PACKETSIZE * 2 + cb_size;
	void *base = NULL;	/* base memory address */

	/* we align our memory to 128 bytes, because I would like to ensure that the crb 
	 * is aligned, this is heavily dependent on your own memory design */
	posix_memalign(&base, COP_CRB_ALIGN, segment_size);
	if (!base) {
		fprintf(stderr, "posix_memalign failed!\n");
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

	/* setup non-mapped stack memory */
	unsigned char key[MAX_KEYSIZE];
	unsigned int data_len = MAX_PACKETSIZE;

	/* put something in memory */
	memset(key, 0xff, MAX_KEYSIZE);
	memset(data, 0xa5, MAX_PACKETSIZE);

	/********************************************************************************/
	/* perform tests */

	/* always clear out the crb before using */
	bzero(crb, cb_size);

	/* set the csb pointer */
	cop_set_csbaddr(crb, csb);

	/* get the cpb pointer, copy in the key */
	struct cop_sym_aes_ecb_cpb *cpb = cop_get_cpbptr(crb);
	memcpy(cpb->key, key, sizeof(cpb->key));

	/* issue the cop setup function for aes_ecb_256, enc = 1 (encryption), and no padding */
	cop_aes_ecb_256_setup(crb, &ctx, data, data_len, ciphertext_hw, data_len, 1, COP_RPAD_NONE, 0x00);

	/* dispatch */
	icswx(crb);

	/* wait for the result */
	cop_csb_wait(cop_get_csbaddr(crb));

	/* print out the result */
	int i;
	printf("ciphertext = ");
	for (i = 0; i < sizeof(ciphertext_hw); i++) {
		printf("%02x", ciphertext_hw[i]);
	}
	printf("\n");

	/********************************************************************************/
	/* free memory */
	if (cop_pbic_unmap(&ctx, base, segment_size, COP_MAP_BOLT)) {
		fprintf(stderr, "cop_pbic_unmap failed!\n");
		exit(1);
	}
	free(base);

	return 0;
}

