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

/* What should be the error in case there are no CRBs left? */

int test_case10(mapped_memory_pool_t pool)
{
	int i;
	int test_size = 12800000;
	void *ptr = cop_malloc(pool, test_size, 4096);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x000fff)
		return FAIL;

	for (i = 0; i < test_size; i++) {
		*((char *)ptr + 1) = 0xff;
	}

	cop_free(pool, ptr);
	return PASS;
}

int test_case9(mapped_memory_pool_t pool)
{
	int test_size = 4096;
	void *ptr = cop_malloc(pool, test_size, 4096);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x000fff)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case8(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 4096);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x0fff)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case7(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 1024);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0xfff)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case6(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 256);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0xff)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case5(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 64);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x3f)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case4(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 16);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x0f)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case3(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 8);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x07)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

int test_case2(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 4);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x03)
		return FAIL;

	cop_free(pool, ptr);
	return PASS;
}

/* malloc and free from pool functionality test */
int test_case1(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, 2);

	if (ptr == NULL) {
		return FAIL;
	}

	if ((unsigned long long)ptr & 0x01)
		return FAIL;

	cop_free(pool, ptr);
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
	DO_TEST(2);
	DO_TEST(3);
	DO_TEST(4);
	DO_TEST(5);
	DO_TEST(6);
	DO_TEST(7);
	DO_TEST(8);
	DO_TEST(9);
	DO_TEST(10);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, pass);
	return 0;
}
