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
	int testsize[] = { 1, 2, 3, 4, 7, 8, 9, 16, 17, 32, 33, 63, 64, 65,
		127, 128, 129, 4096, 4097, 4095, 16192, 16193, 0
	};			/* need to add -1 testcase */
	char testdata[102] =
	    "DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  \0";

	int i, z, remaining, errors;
	char *ptrCDest, *ptrCSrc, *ptrScout;
	void *result;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	cop_trans_t cop_transaction_tag;

	cop_transaction_tag = cop_start();

	for (i = 0; testsize[i]; i++) {
		cop_adm_transaction *ptrAdm;
		cop_adm_transaction *ptrAdm;

		ptrCSrc = cop_malloc_unaligned(cc_pool, testsize[i]);
		ptrCDest = cop_malloc_unaligned(cc_pool, testsize[i]);

		ptrAdm->ptrSource=ptrCSrc;
		ptrAdm->ptrDestination=ptrCDest;
		ptrAdm->size=testsize[i];
		ptrAdm->notification=EPOLL;  // BUSY_WAIT
		ptrAdm->operation=MEMSET; // MEMMOVE, MEMCPY
		cop_trans_adm_add(COP_ADM, ptrAdm, callback_fn, );

		cop_memset(NULL, ptrCSrc, 0x00, testsize[i]);
		cop_memset(NULL, ptrCDest, 0xFF, testsize[i]);

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
		for (errors = 0, z = 0; z < testsize[i]; z++, ptrScout++) {
			if (*(ptrCDest + z) != *ptrScout)
				errors++;
		}
		if (errors) {
			fprintf(stderr,
				"Error: files %s, %d src != dest  test variation %d \n",
				__FILE__, errors, testsize[i]);
			fwrite(ptrCSrc, testsize[i], 1, stderr);
			printf("\n");
			fwrite(ptrCDest, testsize[i], 1, stderr);
			printf("\n\n");
		}

		free_to_trans(cop_transaction_tag, ptrCSrc);
		free_to_trans(cop_transaction_tag, ptrCDest);
	}

	cop_commit_transaction();

	

	cop_end(cop_transaction_tag);
}
