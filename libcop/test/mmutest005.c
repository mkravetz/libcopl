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
#include <string.h>
#include <libcopmalloc.h>
#include "libcop.h"

#define NUM_TESTS 100

int main(int argc, char *argv[])
{
	int bitlens[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	char *a;
	char *b;
	char *n;
	char *bn_r;
	char *bn_a;
	char *bn_b;
	char *bn_n;
	int i = 0;
	int j = 0;
	int pass = 0, fail = 0;

	for (j = 0; j < (sizeof(bitlens) / sizeof(int)); j++) {
		// alloc bignumbers (must be mod 8)
		bn_a = cop_malloc(cc_pool, (bitlens[j] / 8), 1);
		bn_b = cop_malloc(cc_pool, (bitlens[j] / 8), 1);
		bn_n = cop_malloc(cc_pool, (bitlens[j] / 8), 1);
		bn_r = cop_malloc(cc_pool, (bitlens[j] / 8), 1);

		for (i = 0; i < NUM_TESTS; i++) {
			//printf("test %d-%d\n", i, j);
			pass++;

			memset(bn_a, 0xAB, bitlens[j] / 8);
			memset(bn_b, 0xAB, bitlens[j] / 8);
			memset(bn_n, 0xAB, bitlens[j] / 8);
			memset(bn_r, 0xAB, bitlens[j] / 8);

		}

		cop_free(cc_pool, bn_a);
		cop_free(cc_pool, bn_b);
		cop_free(cc_pool, bn_n);
		cop_free(cc_pool, bn_r);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, pass);

	return 0;
}
