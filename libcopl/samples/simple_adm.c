/*
 
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libcopl.h>
#include <libcopl/adm.h>
#include <libcopl/cop_util.h>
#include <libcopl/icswx_util.h>


#define TEST_SIZE 4097

#define ADM_CRB_SIZE sizeof(struct cop_crb) + sizeof(struct cop_csb)

int main(int argc, char *argv[])
{

	int i;
	char *src, *dest;
	struct cop_ctx ctx;
	struct cop_crb *crb;
	struct cop_csb *csb;
	char *pad;

	/*
	 * coprocessor open and setup
	 */
	if (cop_open_bind_local(&ctx, COP_ASYNC_DATA_MOVER)) {
		fprintf(stdout, "cop_open(COP_ASYNC_DATA_MOVER) failure\n");
		exit(1);
	}

	/*
	 * allocate src & target buffer
	 */
	src = malloc(TEST_SIZE);
	if (cop_pbic_map(&ctx, src, TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_map(%p, %d bytes) failure\n",
			src, TEST_SIZE);
		exit(1);
	}

	pad = malloc(4096); /* to make sure on separate pages */

	dest = malloc(TEST_SIZE);
	if (cop_pbic_map(&ctx, dest, TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_map(%p, %d bytes) failure\n",
			dest, TEST_SIZE);
		exit(1);
	}

	/*
	 * initialize src buffer with some data, and clear target buffer
	 */
	for (i=0; i < TEST_SIZE; i++)
		src[i] = (char)i;

	bzero(dest, TEST_SIZE);

	/*
	 * allocate control blocks
	 */
	if (posix_memalign((void *)&crb, COP_CRB_ALIGN,
				sizeof(struct cop_crb)) ||
	    posix_memalign((void *)&csb, COP_CSB_ALIGN,
				sizeof(struct cop_csb))) {
		fprintf(stdout, "error allocating control blocks\n");
		exit(1);
	}
	cop_set_csbaddr(crb, csb);

	if (cop_pbic_map(&ctx, crb, sizeof(struct cop_crb),
				COP_MAP_BOLT) ||
	    cop_pbic_map(&ctx, csb, sizeof(struct cop_csb), COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_map failure of status blocks\n");
		exit(1);
	}

	/*
	 * initialize CRB and Call coprocessor to perform data move (copy)
	 */
	if (cop_memcpy_setup(crb, &ctx, dest, src, TEST_SIZE)) {
		fprintf(stdout, "cop_memcpy_setup() failure\n");
			exit(1);
	}

	if (icswx_wait(crb)) {
		fprintf(stdout, "icswx_wait failure\n");
			exit(1);
	}

	/*
	 * now check to ensure target area matches source
	 */
	for (i=0; i < TEST_SIZE; i++) {
		if (dest[i] != src[i]) {
			fprintf(stdout, "data compare after move failed\n");
			exit(1);
		}
	}

	/*
	 * free source and target areas
	 */
	if (cop_pbic_unmap(&ctx, src, TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_unmap(%p, %d bytes) failure\n",
			src, TEST_SIZE);
		exit(1);
	}
	free(src);
	if (cop_pbic_unmap(&ctx, dest, TEST_SIZE, COP_MAP_BOLT)) {
		fprintf(stdout, "cop_pbic_unmap(%p, %d bytes) failure\n",
			dest, TEST_SIZE);
		exit(1);
	}
	free(dest);
	free(pad);

	/*
	 * and free control blocks
	 */
	if (cop_pbic_unmap(&ctx, crb, sizeof(struct cop_crb),
					COP_MAP_BOLT) ||
	    cop_pbic_unmap(&ctx, csb, sizeof(struct cop_csb),
					COP_MAP_BOLT)) {
		fprintf(stdout, "cop_free_adm_cxb() failure\n");
		exit(1);
	}
	free(crb);
	free(csb);

	printf("Copy success\n");
}
