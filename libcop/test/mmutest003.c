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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libcopmalloc.h>
#include <libcop.h>

#define FAIL 0
#define PASS 1

#ifdef REGX_SUPPORT
#define POOLS_SIZE 2
#else
#define POOLS_SIZE 1
#endif

/* malloc and free from pool functionality test */
int test_case1(mapped_memory_pool_t pool)
{
	int test_size = 128;

//	cop_dumpGlobalPool();

	void *ptr = cop_malloc(pool, test_size, 2);

	if (ptr == NULL) {
		return FAIL;
	}
//	cop_dumpGlobalPool();

	if ((unsigned long long)ptr & 0x01)
		return FAIL;

	cop_free(pool, ptr);

//	cop_dumpGlobalPool();

	return PASS;
}

#define DO_TEST(num) {\
	int rtrc, i;\
	for (i = 0; i < POOLS_SIZE; i++) {\
		rtrc = test_case ##num(pools[i]);\
		if (rtrc == FAIL) {\
			printf("Test %d, Pool %d FAILED.\n", num, i);\
			fail++;\
		} else {\
			pass++;\
		}\
	}\
}

int main(int argc, char *argv[])
{
	int pass = 0, fail = 0;
#ifdef REGX_SUPPORT    
	mapped_memory_pool_t pools[POOLS_SIZE] = { cop_cc_pool(), cop_rx_pool() };
#else
    mapped_memory_pool_t pools[POOLS_SIZE] = { cop_cc_pool() };
#endif

	DO_TEST(1);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, pass);
	return 0;
}
