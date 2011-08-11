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

/** @file hash.c 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

#include <libcopl/crypto.h>
#include <libcopl/crypto/hash.h>
#include <libcopl/crypto/hash_ex.h>

#define MAX_PACKETSIZE 8192
#define MAX_KEYSIZE 32

extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);

#define MEMPRINT(name, length) {\
    long i; \
    printf(#name " =\n");\
    for (i = 0; i < length; i++) { \
        if ((i % 16) == 0) { \
            if (i) \
                printf("\n"); \
            printf("%08lx: ", (unsigned long)(name + i));  \
        } \
        printf("%02x", *((char *)name + i)); \
        if (((i + 1) % 2) == 0) \
            printf(" "); \
    } \
    printf("\n"); \
}\

/********************************************************************************/
/* example API */

int main(int argc, char *argv[])
{
	int rc = 0;

	/********************************************************************************/
	/* open copro and bind instance */
	struct cop_ctx ctx;
	if (cop_open_bind_local(&ctx, COP_SYM_CRYPTO)) {
		fprintf(stdout, "cop_open(COP_ASYNC_DATA_MOVER) failure\n");
		exit(1);
	}

	/********************************************************************************/
	/* allocate memory */

	/* the coprocessor block size is the size of the crb/ccb/csb/cpb, the cpb in this 
	 * case will be 128 bytes since AES-CBC-256 has the largest CPB */
	size_t cb_size =
		sizeof(struct cop_crb) + sizeof(struct cop_csb) +
		sizeof(struct cop_sym_sha1_cpb);

	/* we need a segment of memory for the plaintext (source) and ciphertext (target) 
	 * and for the c*bs */
	size_t segment_size = MAX_PACKETSIZE * 2 + cb_size;
	void *base = NULL;	/* base memory address */

	/* we align our memory to 128 bytes, because I would like to ensure that the crb 
	 * is aligned, this is heavily dependent on your own memory design */
	rc = posix_memalign(&base, COP_CRB_ALIGN, segment_size);
	if (!base) {
		fprintf(stderr, "posix_memalign failed!, rc = %d\n", rc);
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

	/* clear memory for crb/csb/cpb */
	bzero(crb, cb_size);

	/* set the csb pointer (required before doing any cpb work */
	cop_set_csbaddr(crb, csb);

	/* point to the cpb */
	struct cop_sym_sha1_cpb *cpb = cop_get_cpbptr(crb);

	unsigned char *data = base + cb_size;

	/* setup non-mapped stack memory */
	unsigned char key[MAX_KEYSIZE];
	unsigned int data_len = MAX_PACKETSIZE;

	/* put something in memory */
	memset(key, 0xff, MAX_KEYSIZE);
	memset(data, 0xa5, MAX_PACKETSIZE);

	/********************************************************************************/
	/* perform basic hash operation */

	cop_sha1_setup(crb, &ctx, data, data_len, COP_FLAG_ONESHOT);
	cop_sha1_cpb_calc_mbl(cpb, data_len);
	icswx(crb);
	cop_csb_wait(csb);

	printf("oneshot:\n");
	MEMPRINT((cpb->md), 20);

	/********************************************************************************/
	/* perform a multi-crb hash operation */

	/* FIRST */
	cop_sha1_setup(crb, NULL, data, data_len / 4, COP_FLAG_FIRST);
	icswx(crb);
	cop_csb_wait(cop_get_csbaddr(crb));

	/* MIDDLE */
	cop_sha1_setup(crb, NULL, data + data_len / 4, data_len / 4, COP_FLAG_MIDDLE);
	cop_sha1_cpb_advance(cpb, cpb);
	icswx(crb);
	cop_csb_wait(cop_get_csbaddr(crb));

	/* MIDDLE */
	cop_sha1_setup(crb, NULL, data + data_len / 2, data_len / 4, COP_FLAG_MIDDLE);
	cop_sha1_cpb_advance(cpb, cpb);
	icswx(crb);
	cop_csb_wait(cop_get_csbaddr(crb));

	/* LAST */
	cop_sha1_setup(crb, NULL, data + 3 * data_len / 4, data_len / 4, COP_FLAG_LAST);
	cop_sha1_cpb_advance(cpb, cpb);
	cop_sha1_cpb_calc_mbl(cpb, data_len);
	icswx(crb);
	cop_csb_wait(cop_get_csbaddr(crb));

	printf("multi:\n");
	MEMPRINT((cpb->md), 20);

	/********************************************************************************/
	/* free memory */

	if (cop_pbic_unmap(&ctx, base, segment_size, COP_MAP_BOLT)) {
		fprintf(stderr, "cop_pbic_unmap failed!\n");
		exit(1);
	}
	free(base);
	return 0;
}
