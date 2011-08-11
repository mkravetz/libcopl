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
#include <libcopcache.h>

extern union cop_u cop_ct_union;

#define FAIL 0
#define PASS 1


#ifdef REGX_SUPPORT
#define POOLS_SIZE 2
#else
#define POOLS_SIZE 1
#endif

/* What should be the error in case there are no CRBs left? */
int test_case7()
{
	return PASS;
}

int test_case6()
{
	struct cop_crb *crb =
	    (struct cop_crb *) cop_next_crb(cop_cc_pool(), COP_RNG);

	if (crb == NULL) {
		return FAIL;
	}

	crb->ccw.value = 0xFF;

	return PASS;
}

/* malloc and free from pool nearobj functionality test */
int test_case5(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, test_size);

	if (ptr == NULL) {
		return FAIL;
	}

	cop_free(pool, ptr);
	return PASS;
}

/* basic malloc and free from pool unaligned stress test */
int test_case4(mapped_memory_pool_t pool)
{
	int test_size = 10240;
	void *ptr = cop_malloc_unaligned(pool, test_size);
	char *cptr = (char *)ptr;

	if (ptr == NULL) {
		return FAIL;
	}

	memset(cptr, 0x00, sizeof(char) * test_size);

	cop_free(pool, ptr);

	return PASS;
}

/* malloc and free from pool unaligned functionality test */
int test_case3(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc_unaligned(pool, test_size);

	if (ptr == NULL) {
		return FAIL;
	}

	cop_free(pool, ptr);
	return PASS;
}

/* basic malloc and free from pool stress test */
int test_case2(mapped_memory_pool_t pool)
{
	int test_size = 10240;
	void *ptr = cop_malloc(pool, test_size, test_size);
	char *cptr = (char *)ptr;

	if (ptr == NULL) {
		return FAIL;
	}

	memset(cptr, 0x00, sizeof(char) * test_size);

	cop_free(pool, ptr);

	return PASS;
}

/* malloc and free from pool functionality test */
int test_case1(mapped_memory_pool_t pool)
{
	int test_size = 128;
	void *ptr = cop_malloc(pool, test_size, test_size);

	if (ptr == NULL) {
		return FAIL;
	}

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

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, pass);
	return 0;
}
