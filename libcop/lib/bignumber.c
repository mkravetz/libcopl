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
#include <stdlib.h>
#include <string.h>
#include "libcopmalloc.h"
#include "libcop.h"
#include <libcopl/asym.h>

BigNumber *cop_bn_new(uint8_t * number, size_t size)
{
	/* grab cc memory pool */
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	if (!cc_pool)
		fprintf(stderr, "Couldn't get cc pool handle!\n");

	/* malloc the struct */
	BigNumber *bn =
	    (BigNumber *) cop_malloc_unaligned(cc_pool, sizeof(BigNumber));
	if (!bn) {
		fprintf(stderr, "Error allocating bn.\n");
		return NULL;
	}

	/* malloc the number */
	bn->number = (uint8_t *) cop_malloc_unaligned(cc_pool, size);
	if (!bn->number) {
		fprintf(stderr, "Error allocating bn->number.\n");
		return NULL;
	}

	/* set the size */
	if (size) {
		bn->size = size;
	} else {
		fprintf(stderr, "Error bn->size is zero.\n");
		return NULL;
	}

	/* if a number is passed copy its contents otherwise,
	 * zero its value */
	if (number) {
		memcpy(bn->number, number, size);
	} else {
		memset(bn->number, 0x00, size);
	}

	return bn;
}

inline BigNumber *cop_bn_new_int(int number)
{
	return cop_bn_new((uint8_t *)(&number), sizeof(int));
}

inline BigNumber *cop_bn_new_sz(size_t size)
{
	return cop_bn_new(NULL, size);
}

errorn cop_bn_free(BigNumber * bn)
{
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	if (!bn)
		return -EINVAL;

	if (!bn->number)
		return -EFAULT;

	cop_free(cc_pool, bn);
	cop_free(cc_pool, bn->number);

	return 0;
}

void cop_bn_copy(BigNumber * dest, const BigNumber * src)
{
	if (dest->size < src->size) {
		return;
	}

	memset(dest->number, 0x0, dest->size);
	memcpy(dest->number, src->number, src->size);
}

int cop_bn_is_zero(BigNumber * n)
{
	int i = 0;
	for (i = 0; i < n->size; i++) {
		if (n->number[i] != 0) {
			return 0;
		}
	}
	return 1;
}

void cop_bn_print(BigNumber * n)
{
	verbose_memprint(n->number, n->size);
}
