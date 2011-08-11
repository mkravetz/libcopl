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
#include "libcop.h"

#define NUM_TESTS	500
#define TEST_SIZE	2048

int main(int argc, char *argv[])
{
	int fail = 0;
	int i = 0, j = 0;
	size_t size = TEST_SIZE;
	char number[TEST_SIZE];
	memset(number, 0xA5, size);

	/* allocate bignumbers */
	BigNumber *bn = cop_bn_new(number, size);
	BigNumber *bn2 = cop_bn_new_int(12345);
	BigNumber *bn3 = cop_bn_new_sz(size);
	BigNumber *bn_array[TEST_SIZE];
	for (i = 0; i < NUM_TESTS; i++) {
		bn_array[i] = cop_bn_new(number, size);
		if (bn_array[i] == NULL)
			fail++;
	}

	/* modify bignumbers */
	BigNumber *bn_new = cop_bn_new_int(9999);
	cop_bn_copy(bn, bn_new);
	cop_bn_copy(bn2, bn_new);
	cop_bn_copy(bn3, bn_new);

	BigNumber *bn_zero = cop_bn_new_int(0);
	for (i = 0; i < NUM_TESTS; i++) {
		cop_bn_copy(bn_array[i], bn_zero);
		if (!cop_bn_is_zero(bn_array[i]))
			fail++;
	}

	/* free bignumbers */
	cop_bn_free(bn);
	cop_bn_free(bn2);
	cop_bn_free(bn3);
	for (i = 0; i < NUM_TESTS; i++) {
		cop_bn_free(bn_array[i]);
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS * 2);
	return fail;
}
