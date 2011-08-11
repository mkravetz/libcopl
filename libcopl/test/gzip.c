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

/** @file gzip.c 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

#include <libcopl/comp.h>
#include <libcopl/comp/gzip.h>

#define MAX_PACKETSIZE 8192
#define MAX_KEYSIZE 32

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

#define CHECK(statement, message, ... ) { \
	if (!statement) {\
		fprintf(stderr, message, ##__VA_ARGS__);\
		exit(1);\
	}\
}

extern int cop_open_bind_local(struct cop_ctx *ctx, int copid);

struct cop_cdc_cxb {
	struct cop_crb crb;		/* 64 bytes */
	uint8_t _pad[48];
	struct cop_csb csb;		/* 16 bytes */
	struct cop_comp_cpb cpb;	/* 112 bytes */
	uint8_t _pad2[16];
};

int main(int argc, char *argv[])
{
	int rc = 0;
	int i = 0;

	/********************************************************************************/
	/* open copro and bind instance */

	struct cop_ctx ctx;
	CHECK(cop_open_bind_local(&ctx, COP_DECOMP) == 0, "cop_open failure\n");

	/********************************************************************************/
	/* allocate memory */

	struct cop_cdc_cxb *cxb;
	rc = posix_memalign((void **)(&cxb), COP_CRB_ALIGN, sizeof(struct cop_cdc_cxb));
	CHECK(cxb, "posix_memalign failed rc = %d\n", rc);

	/********************************************************************************/
	/* zero and setup memory */

	char input[] = "it works!";
	char output[30];
	char check[10];
	memset(output, 0x00, sizeof(output));
	memset(check, 0x00, sizeof(check));
	memset(cxb, 0x00, sizeof(struct cop_cdc_cxb));

	/********************************************************************************/
	/* map memory */

	CHECK(cop_pbic_map(&ctx, cxb, sizeof(struct cop_cdc_cxb), 0) == 0, "map cxb fail\n");
	CHECK(cop_pbic_map(&ctx, input, sizeof(input), 0) == 0, "map input fail\n");
	CHECK(cop_pbic_map(&ctx, output, sizeof(output), 0) == 0, "map output fail\n");
	CHECK(cop_pbic_map(&ctx, check, sizeof(check), 0) == 0, "map check fail\n");

	/********************************************************************************/
	/* perform basic hash operation */

	cop_set_csbaddr(&cxb[i].crb, &cxb[i].csb);

	cop_gzip_setup(&cxb[i].crb, &ctx, input, sizeof(input), output, sizeof(output));
	icswx(&cxb[i].crb);
	cop_csb_wait(&cxb[i].csb);

	cop_gunzip_setup_only(&cxb[i].crb, &ctx, output, sizeof(output), check, sizeof(check));
	icswx(&cxb[i].crb);
	cop_csb_wait(&cxb[i].csb);
	
	/********************************************************************************/
	/* check */
	printf("%s\n", check);

	/********************************************************************************/
	/* free memory */

	CHECK (cop_pbic_unmap(&ctx, cxb, sizeof(struct cop_cdc_cxb),0) == 0, "unmap cxb fail!\n");
	CHECK (cop_pbic_unmap(&ctx, input, sizeof(input),0) == 0, "unmap input fail!\n");
	CHECK (cop_pbic_unmap(&ctx, output, sizeof(output),0) == 0, "unmap output fail!\n");
	CHECK (cop_pbic_unmap(&ctx, check, sizeof(check),0) == 0, "unmap check fail!\n");
	free(cxb);

	return 0;
}
