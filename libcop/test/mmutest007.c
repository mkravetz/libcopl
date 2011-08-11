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

#define NUM_TESTS 10000

int main(int argc, char *argv[])
{
	int bitlens[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
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

	unsigned long long y=0;

	for (j = 0; j < (sizeof(bitlens) / sizeof(int)); j++) {
		printf("doing %d cop_mallocs on %u bytes\n", NUM_TESTS, bitlens[j]);
		for (i = 0; i < NUM_TESTS; i++) {
			void * x = cop_malloc(cc_pool, bitlens[j], 1);
			if (!x) { fail++; }
			if (y)
				if (y!=(unsigned long long) x)
					fprintf(stderr,"%lx != %p\n",y,x);
			y=(unsigned long long)x;
			cop_free(cc_pool, x);
			pass++;
		}
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, pass);

	return 0;
}
