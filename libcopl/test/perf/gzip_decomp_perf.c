/*
 * Nick Lindberg <nlindber@us.ibm.com>
 * 
 * Performance test showing decompression bandwidth of Canterbury Corpus 
 * benchmark files compressed using  level 9 gzip compression
 *
 * Modified by Chris J Arges <arges@linux.vnet.ibm.com>
 * to work with libcopl
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

#include <libcopl/icswx_util.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

#include <libcopl/comp.h>
#include <libcopl/comp/gzip.h>

#include "bma2libcopl.h"

#define DEBUG 0
#define DEBUG2 0

#define REQS	8

static char type_str[2][20] = { "Decompression", "Compression" };

struct cop_ctx ctx;

struct cop_cdc_cxb {
	struct cop_crb crb;		/* 64 bytes */
	uint8_t _pad[48];
	struct cop_csb csb;		/* 16 bytes */
	struct cop_comp_cpb cpb;	/* 112 bytes */
	uint8_t _pad2[16];
};

static void decomp_file(const char *name, int type,
			void *input, size_t input_sz,
			void *output, size_t output_sz, 
			int do_calc)
{
	if (DEBUG) {
		printf("\nTest function entry for \"%s\": ", name);
		printf("Filesize: %d   Expected Size: %d\n", (int)input_sz,
		       (int)output_sz);
	}

	int rc = 0, i = 0;
	float bytes_per_second = 0;
	float bytes_per_second_upped = 0;
	uint64_t startTime = 0, endTime = 0, totalTime = 0;
	
	struct cop_cdc_cxb *cxb;
	void *dest[REQS];

	/* allocate coprocessor blocks */
	cxb = dmalloc(&ctx, 128, sizeof(struct cop_cdc_cxb) * REQS);
	memset(cxb, 0x00, sizeof(struct cop_cdc_cxb) * REQS);

	/* allocate target space */
	for (i = 0; i < REQS; i++) {
		dest[i] = dmalloc(&ctx, 512, output_sz);
		memset(dest[i], 0x00, output_sz);
	}

	/* ---> start the clock */
	startTime = mftb();

	/* set up the crbs and dispatch */
	for (i = 0; i < REQS; i++) {
		cop_set_csbaddr(&(cxb[i].crb), &(cxb[i].csb));
		if (type == 0)
			cop_gunzip_setup_only(&(cxb[i].crb), &ctx, input, input_sz, dest[i], output_sz);
		else 
			cop_gzip_setup(&(cxb[i].crb), &ctx, input, input_sz, dest[i], output_sz);
		icswx(&(cxb[i].crb));
	}
	
	/* spin on the last csb valid bit */
	while (!cxb[REQS-1].csb.cw.valid) {
		clobber();
	}

	/* <--- stop the clock */
	endTime = mftb();

	/* if this is not the warming run, do bandwidth calculations */
	if (do_calc == 1) {

		/* check the last CC to ensure it is zero */
		if (cxb[REQS - 1].csb.cw.comp_code != 0)
			rc++;

		/* compare result with known correct value */
		if (type == 0) {
		if (memcmp(dest[REQS - 1], output, output_sz) != 0) {
			printf("Memcmp failed.\n");
			rc++;

			printf("dest\n");
			MEMPRINT(dest[REQS-1],output_sz);
			printf("output\n");
			MEMPRINT(output,output_sz);
		}
		}

		totalTime = (endTime - startTime);

		/* TODO: may be an equivalent for ticks_per_sec. For now, hard coding timebase */
		//totalTimeInSeconds = ((float)totalTime / ticks_per_sec(1));

		bytes_per_second = (output_sz * REQS * 33333333) / totalTime;
		bytes_per_second_upped = (output_sz * REQS * 42592592) / totalTime;
		
		if (DEBUG2) {
			printf("startTime: %lu", (long unsigned int)startTime);
			printf("     endTime:   %lu", (long unsigned int)endTime);
			printf("     totalTime:   %lu\n", (long unsigned int)totalTime);
			printf("Throughput (B/s) for \"%s\": %lu\n", name,
			       (long unsigned int) bytes_per_second);
			printf("Throughput Upped (B/s) for \"%s\": %lu\n", name,
                               (long unsigned int) bytes_per_second_upped);
		}

		float GBps_d = 0.0;
		float GBps_d_upped = 0.0;

		/* normalize the number before the decimal point and print it */
		GBps_d = ((float)bytes_per_second) / (1000.0 * 1000.0 * 1000.0);
		GBps_d_upped = ((float)bytes_per_second_upped) / (1000.0 * 1000.0 * 1000.0);
		
		//char GBps_s[100];
		//ftostr(GBps_s, GBps_d, 3);

		printf("%s throughput (GB/s) for \"%s\": %f\n", type_str[type], name, GBps_d);
		printf("%s throughput Upped (GB/s) for \"%s\": %f\n", type_str[type], name, GBps_d_upped);

	}
}

//------------------------------------------------------------------------//

char *input_file(char *filename, int *size) {

	/* open file */
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == 0) {
		fprintf(stderr, "can't open file\n");
		return NULL;
	}

	/* determine size and allocate buffer */
	fseek(fp, 0l, SEEK_END);
	*size = ftell(fp);
	fseek(fp, 0l, SEEK_SET);

	char *data = malloc(sizeof(char) * *size);
	if (!data) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}

	/* read in data */
	if (fread(data, sizeof(char), *size, fp) == 0) {
		fprintf(stderr, "bad fread\n");
		return NULL;
	}

	return data;
}

int main(int argc, char *argv[])
{
	int rc = 0;
	int i = 0;

	/* input compressed file */
	char *input_filename = NULL;
	int input_sz = 0;
	char *input = NULL;

	/* check decompressed file */
	char *check_filename = NULL;
	int check_sz = 0;
	char *check = NULL;

	/* check for appropriate number of arguments */
	if (argc != 3) {
		printf("usage: ./%s file.gz file\n", argv[0]);
		return -1;
	}

	/* input compressed and uncompressed files */
	input_filename = argv[1];
	input = input_file(input_filename, &input_sz);
	if (!input)
		return -1;
	check_filename = argv[2];
	check = input_file(check_filename, &check_sz);
	if (!check)
		return -1;

	/* remove the header from the file */
	void *decapPtr = decapitate(input, FLAG_GZIP);
	unsigned long headerSz = (unsigned long)decapPtr - (unsigned long)input;
	if (!decapPtr) {
		fprintf(stderr, "invalid gzip file\n");
		return -1;
	}

	/* initialize coprocessor structure */
	cop_init(&ctx, COP_DECOMP);

	/* do decompression twice -- first time to warm everything up */
	for (i = 0; i < 2; i++) {
		/* the last parameter calculates and prints bandwidth if it is 1 */
		decomp_file(input_filename, 0,
			decapPtr, input_sz - headerSz,
			check, check_sz, i);
	}
	
	/* do compression twice -- first time to warm everything up */
	for (i = 0; i < 2; i++) {
		/* the last parameter calculates and prints bandwidth if it is 1 */
		decomp_file(check_filename, 1,
			check, check_sz,
			decapPtr, input_sz - headerSz, i);
	}

	return rc;
}

