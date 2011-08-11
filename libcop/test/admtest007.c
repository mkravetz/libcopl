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

int main(int argc, char *argv[])
{
	int testsize[] = { 16, 17, 32, 33, 63, 64, 65,
		127, 128, 129, 4096, 4097, 4095, 16192, 16193, 0
	};
	char testdata[102] =
	    "DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  \0";

	int i, z, remaining, errors;
	char *ptrCDest, *ptrCSrc, *ptrScout;
	void *result;

	cop_trans_t cop_transaction_tag;

	cop_transaction_tag = cop_start(ASYNC_DATA_MOVER_COPRO);

	for (i = 0; testsize[i]; i++) {
		ptrCSrc =
		    malloc_from_trans_pool(cop_transaction_tag,
					   testsize[i] * 2);
		ptrCDest = ptrCSrc + (testsize[i] / 2);

		cop_memset(NULL, ptrCSrc, 0xFF, testsize[i] * 2);

		test_print("Src add %p   Dest Add %p\n", ptrCSrc, ptrCDest);
		ptrScout = ptrCSrc;
		for (remaining = testsize[i]; remaining > 0;) {
			z = remaining % 100;
			remaining -= z;
			if (z) {
				memcpy(ptrScout, testdata, z);
				ptrScout += z;
			}
		}

		test_memprint(ptrCSrc, testsize[i]);
		test_print("calling cop_memmove\n");
		result =
		    cop_memmove(cop_transaction_tag, ptrCSrc, ptrCDest,
				testsize[i]);

		test_print("returned from cop_memmove\n");

		if (!result) {
			fprintf(stdout, "test failed testcase %d", testsize[i]);
		} else
			fprintf(stdout,
				"adm testcase %d testsize %d dispatched successfully\n",
				i, testsize[i]);

		ptrScout = ptrCSrc;
		for (z = 0; z < (testsize[i] * 2); z++, ptrScout++) {
			fprintf(stdout, "%c", *ptrScout);
		}
		fprintf(stdout, "\n");
		free_to_trans(cop_transaction_tag, ptrCSrc);
		free_to_trans(cop_transaction_tag, ptrCDest);
	}

	for (i = 0; testsize[i]; i++) {
		ptrCDest =
		    malloc_from_trans_pool(cop_transaction_tag,
					   testsize[i] * 2);
		ptrCSrc = ptrCDest + (testsize[i] / 2);

		cop_memset(NULL, ptrCDest, 0xFF, testsize[i] * 2);

		test_print("Src add %p   Dest Add %p\n", ptrCSrc, ptrCDest);
		ptrScout = ptrCSrc;
		for (remaining = testsize[i]; remaining > 0;) {
			z = remaining % 100;
			remaining -= z;
			if (z) {
				memcpy(ptrScout, testdata, z);
				ptrScout += z;
			}
		}

		test_memprint(ptrCSrc, testsize[i]);
		test_print("calling cop_memmove\n");
		result =
		    cop_memmove(cop_transaction_tag, ptrCSrc, ptrCDest,
				testsize[i]);

		test_print("returned from cop_memmove\n");

		if (!result) {
			fprintf(stdout, "test failed testcase %d", testsize[i]);
		} else
			fprintf(stdout,
				"adm testcase %d testsize %d dispatched successfully\n",
				i, testsize[i]);

		ptrScout = ptrCDest;
		for (z = 0; z < (testsize[i] * 2); z++, ptrScout++) {
			fprintf(stdout, "%c", *ptrScout);
		}
		fprintf(stdout, "\n");

		free_to_trans(cop_transaction_tag, ptrCSrc);
		free_to_trans(cop_transaction_tag, ptrCDest);
	}

	cop_end(cop_transaction_tag);
}
