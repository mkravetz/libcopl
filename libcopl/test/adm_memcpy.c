/* testcase for asynchronous data mover corprocessor */

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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libcopl.h>
#include <libcopl/adm.h>
#include <libcopl/cop_util.h>
#include <libcopl/icswx_util.h>


extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);

#define MAX_TEST_SIZE 16193
int testsize[] = { 1, 2, 3, 4, 7, 8, 9, 16, 17, 32, 33, 63, 64, 65,
	127, 128, 129, 4096, 4097, 4095, 16192, 16193, 0
};				/* need to add -1 testcase */
char testdata[102] =
    "DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  \0";

#define ADM_CRB_SIZE sizeof(struct cop_crb) + sizeof(struct cop_csb)
#define ADM_CRB_ALIGN 128

int main(int argc, char *argv[])
{

	int i, z, remaining, errors;
	int fail = 0;
	char *ptrCDest, *ptrCSrc, *ptrScout;
	struct cop_crb *crb;
	struct cop_csb *csb;
	struct cop_ctx ctx;
	char *pad;
	size_t result;

	/*
	 * coprocessor open and setup
	 */
	if (cop_open_bind_local(&ctx, COP_ASYNC_DATA_MOVER)) {
		fprintf(stdout, "cop_open(COP_ASYNC_DATA_MOVER) failure\n");
		exit(1);
	}

	/*
	 * allocate control blocks
	 */
	fail = posix_memalign((void *)&crb, COP_CRB_ALIGN,
			sizeof(struct cop_crb)) ||
	       posix_memalign((void *)&csb, COP_CSB_ALIGN,
			sizeof(struct cop_csb));
	if (fail) {
		fprintf(stdout, "failure allocating control blocks\n");
		exit(1);
	}
	cop_set_csbaddr(crb, csb);

	fail = cop_pbic_map(&ctx, crb, sizeof(struct cop_crb),
						COP_MAP_BOLT) ||
	       cop_pbic_map(&ctx, csb, sizeof(struct cop_csb), COP_MAP_BOLT);
	if (fail) {
		fprintf(stdout, "cop_pbic_map failure of status blocks\n");
		exit(1);
	}

	/*
	 * allocate largest needed src/target buffer (only once)
	 */
	ptrCSrc = malloc(MAX_TEST_SIZE);
	if (cop_pbic_map(&ctx, ptrCSrc, MAX_TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_map(%p, %d bytes) failure\n",
			ptrCSrc, MAX_TEST_SIZE);
		exit(1);
	}
	pad = malloc(4096); /* to make sure on separate pages */
	ptrCDest = malloc(MAX_TEST_SIZE);
	if (cop_pbic_map(&ctx, ptrCDest, MAX_TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_map(%p, %d bytes) failure\n",
			ptrCDest, MAX_TEST_SIZE);
		exit(1);
	}

	/*
	 * Now, do a test 'copy' for each test size
	 */
	for (i = 0; testsize[i]; i++) {
		/*
		 * Clean CRB and set CSB pointer on each iteration
		 */
		/* bzero(crb, sizeof(struct cop_crb)); */
		bzero(csb, sizeof(struct cop_csb));

		/*
		 * Clear source/target buffers
		 */
		memset(ptrCSrc, 0x00, testsize[i]);
		memset(ptrCDest, 0x00, testsize[i]);

		/*
		 * Initialize source area
		 */
		ptrScout = ptrCSrc;
		for (remaining = testsize[i]; remaining > 0;) {
			if ((remaining / 100) > 0)
				z = 100;
			else
				z = remaining % 100;

			remaining -= z;
			if (z) {
				memcpy(ptrScout, testdata, z);
				ptrScout += z;
			}
		}

		/*
		 * Call coprocessor to perform data move (copy)
		 */
		if (cop_memcpy_setup(crb, &ctx, ptrCDest, ptrCSrc,
								testsize[i])) {
			fprintf(stdout, "cop_memcpy_setup() failure\n");
				exit(1);
		}

		if (icswx_wait(crb)) {
			fprintf(stdout, "icswx failure\n");
				exit(1);
		}

		/*
		 * now check to ensure target area matches source
		 */
		errors = 0;
		for (z = 0; z < testsize[i]; z++) {
			if (*(ptrCDest + z) != *(ptrCSrc + z))
				errors++;
		}

		/*
		 * print big ugly message in case of errors
		 */
		if (errors) {
			fail++;
			fprintf(stderr,
				"Error: files %s, %d src != dest  test variation %d \n",
				__FILE__, errors, testsize[i]);
			result = fwrite(ptrCSrc, testsize[i], 1, stderr);
			printf("\n");
			result = fwrite(ptrCDest, testsize[i], 1, stderr);
			printf("\n\n");

		}
	}

	/*
	 * free source and target areas
	 */
	if (cop_pbic_unmap(&ctx, ptrCSrc, MAX_TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_unmap(%p, %d bytes) failure\n",
			ptrCSrc, MAX_TEST_SIZE);
		exit(1);
	}
	free(ptrCSrc);
	free(pad);
	if (cop_pbic_unmap(&ctx, ptrCDest, MAX_TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_unmap(%p, %d bytes) failure\n",
			ptrCDest, MAX_TEST_SIZE);
		exit(1);
	}
	free(ptrCDest);

	/*
	 * and free control blocks
	 */
	free(crb);
	free(csb);

	if (cop_pbic_unmap(&ctx, crb, sizeof(struct cop_crb),
				COP_MAP_BOLT) ||
	    cop_pbic_unmap(&ctx, crb, sizeof(struct cop_crb),
				COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_unmap error unmapping ctrl blocks\n");
		exit(1);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, i);

	exit(0);
}
