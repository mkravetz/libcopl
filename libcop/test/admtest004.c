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
#include <errno.h>
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

	// this test is source is less than destination address
	for (i = 0; testsize[i]; i++) {
		ptrCSrc = cop_malloc_unaligned(cc_pool, (testsize[i] * 2) + 10);
		ptrCDest = ptrCSrc + (testsize[i] / 2 + 1);

		memset(ptrCSrc, 0xFF, (testsize[i] * 2) + 10);

//              test_print("Src add %p   Dest Add %p\n", ptrCSrc, ptrCDest);

		memset(ptrCSrc, 0xAA, testsize[i]);

//              test_memprint(ptrCSrc, testsize[i]);
//              test_print("calling cop_memcpy\n");
		errno = 0;
		cop_memmove(ptrCDest, ptrCSrc, testsize[i]);

		if (errno) {
			fail++;
			fprintf(stdout, "test failed testcase %d, errno %d",
				testsize[i], errno);
		}

		for (errors = 0, z = 0; z < testsize[i]; z++) {
			if (*(ptrCDest + z) != 0xAA)
				errors++;
		}
		if (errors) {
			fail++;
			fprintf(stderr,
				"Error: files %s, %d src != dest  test variation %d \n",
				__FILE__, errors, testsize[i]);
		}

		cop_free(cc_pool, ptrCSrc);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, i);
}
