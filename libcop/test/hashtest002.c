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
#include <libcopl/cop_err.h>
#include "config.h"
#include "../test/utils.h"

const uint8_t sha1_len0[] = {
	0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef,
	0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09
};

#define PACKET_SIZE 8192

int main(int argc, char **argv)
{
if (cop_get_DD_version() > 1) {
	int fail = 0;
	int total = 0;
	int rc = 0;
	int i = 0;

	mapped_memory_pool_t cc_pool = cop_cc_pool();
	void *data = (unsigned char *)cop_malloc(cc_pool, PACKET_SIZE, 1);
	memset(data, 0xA5, PACKET_SIZE);
	unsigned char digest[64];

	/* test zero length conditions */
	printf("test zero length data...\n");
	rc = cop_sha1(NULL, 0, digest, 0ull, COP_FLAG_ONESHOT);
	if (rc) {
		printf("error rc = %d, should be 0 (we handle this is software)\n", rc);
		fail++;
	}
	total++;
	if (memcmp(digest, sha1_len0, 20)) {
		printf("memcmpy failed for sha1 len 0\n");
		fail++;
	}
	total++;
	rc = cop_sha1(NULL, 0, digest, 0ull, COP_FLAG_FIRST);
	if (rc != 3) {
		printf("error rc = %d, should be 3 (Data Length) \n", rc);
		fail++;
	}
	total++;
	rc = cop_sha1(NULL, 0, digest, 0ull, COP_FLAG_MIDDLE);
	if (rc != 3) {
		printf("error rc = %d, should be 3 (Data Length) \n", rc);
		fail++;
	}
	total++;
	rc = cop_sha1(NULL, 0, digest, 0ull, COP_FLAG_LAST);
	if (rc) {
		printf("error rc = %d, should be 0\n", rc);
		//printf("error rc = %d, should be 5 (Translation Error)) \n", rc);
		fail++;
	}
	total++;

	/* test error conditions */
	printf("test non-block sizes for data length exceptions...\n");
	int invalid_sizes[] = { 17, 63, 65 };
	for (i = 0; i < sizeof(invalid_sizes) / sizeof(int); i++) {
		printf("test FIRST #%d:", i);
		rc = cop_sha1(data, invalid_sizes[i], digest, 0ull,
			      COP_FLAG_FIRST);
		if (rc != DATA_LEN_EXCEPT) {
			printf(" error rc = %d, should be 3\n", rc);
			fail++;
		} else {
			printf(" OK\n");
		}
		total++;

		printf("test MIDDLE #%d:", i);
		rc = cop_sha1(data, invalid_sizes[i], digest, 0ull,
			      COP_FLAG_MIDDLE);
		if (rc != DATA_LEN_EXCEPT) {
			printf(" error rc = %d, should be 3\n", rc);
			fail++;
		} else {
			printf(" OK\n");
		}
		total++;
	}

	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;

} else {
	printf("this test should NOT be run on DD1 hardware due to Errata\n");
}

}
