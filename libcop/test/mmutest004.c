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

int temp_stop(void)
{
	printf("temp stop at 500\n");
}

/* malloc and free from pool functionality test */
int test_case1(mapped_memory_pool_t pool)
{
	void *memoryBlock[1024];
	int test_size = 128;
	int i;
	void *ptr;

	for (i = 0; i < 1024; i++) {
		memoryBlock[i] = cop_malloc(pool, test_size, 8);

		if (memoryBlock[i] == NULL) {
			printf("alloc of 128 block # %d failed\n", i);
			return FAIL;
		}

		memset(memoryBlock[i], 0xAB, 128);

	}

	for (i = 0; i < 1024; i++) {
		if (i == 500)
			temp_stop();

		cop_free(pool, memoryBlock[i]);
	}

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
