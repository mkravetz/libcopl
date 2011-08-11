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

#include <stdio.h>
#include <libcopmalloc.h>
#include <libcop.h>
#include "utils.h"

int main(int argc, char *argv[])
{
	int testsize[] = { 1, 2, 3, 4, 7, 8, 9, 16, 17, 32, 33, 63, 64, 65,
		127, 128, 129, 4096, 4097, 4095, 16192, 16193, 0
	};			/* need to add -1 testcase */
	char testdata[102] =
	    "DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  \0";

	int i, z, remaining, errors;
	int fail = 0;
	char *ptrCDest, *ptrCSrc, *ptrScout;
	void *result;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	for (i = 0; testsize[i]; i++) {
		ptrCSrc = cop_malloc_unaligned(cc_pool, testsize[i]);
		ptrCDest = cop_malloc_unaligned(cc_pool, testsize[i]);

		memset(ptrCSrc, 0x00, testsize[i]);
		memset(ptrCDest, 0x00, testsize[i]);

//              test_print("Src add %p   Dest Add %p\n", ptrCSrc, ptrCDest);
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

//              test_memprint(ptrCSrc, testsize[i]);
		result = cop_memcpy(ptrCDest, ptrCSrc, testsize[i]);

		if (!result) {
			fprintf(stdout, "test failed testcase %d", testsize[i]);
		}

		ptrScout = ptrCSrc;
		for (errors = 0, z = 0; z < testsize[i]; z++, ptrScout++) {
			if (*(ptrCDest + z) != *ptrScout)
				errors++;
		}
		if (errors) {
			fail++;
			fprintf(stderr,
				"Error: files %s, %d src != dest  test variation %d \n",
				__FILE__, errors, testsize[i]);
			fwrite(ptrCSrc, testsize[i], 1, stderr);
			printf("\n");
			fwrite(ptrCDest, testsize[i], 1, stderr);
			printf("\n\n");
		}

		cop_free(cc_pool, ptrCSrc);
		cop_free(cc_pool, ptrCDest);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, i);
}
