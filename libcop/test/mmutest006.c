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

#define NUM_TESTS 1000
#define TEST_BYTE 0xA5

#ifdef REGX_SUPPORT
#define POOLS_SIZE 2
#else
#define POOLS_SIZE 1
#endif

int main(int argc, char *argv[])
{
	int bitlens[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };

#ifdef REGX_SUPPORT    
	mapped_memory_pool_t pools[POOLS_SIZE] = { cop_cc_pool(), cop_rx_pool() };
#else
    mapped_memory_pool_t pools[POOLS_SIZE] = { cop_cc_pool() };
#endif

    char *x[NUM_TESTS];
	int i, j, k, l;
	int pass = 0, fail = 0;


	for (l = 0; l < POOLS_SIZE; l++) {

		printf("big alloc\n");
		cop_malloc(pools[l], 4194304, 262144);

		for (j = 0; j < (sizeof(bitlens) / sizeof(int)); j++) {
			printf("BITLEN = %d\n", bitlens[j]);

			for (i = 0; i < NUM_TESTS; i++)
				x[i] = cop_malloc(pools[l], (bitlens[j]), 1);

			for (i = 0; i < NUM_TESTS; i++)
				memset(x[i], TEST_BYTE, bitlens[j]);

			for (i = 0; i < NUM_TESTS; i++) {
				for (k = 0; k < (bitlens[j]); k++) {
					if (*((x[i]) + k) != TEST_BYTE) {
						printf("ERROR\n");
						fail++;
					}
				}
			}

			for (i = 0; i < NUM_TESTS; i++)
				cop_free(pools[l], x[i]);
		}
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return 0;
}
