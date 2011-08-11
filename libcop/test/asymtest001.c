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
#include "utils.h"

#define NUM_TESTS 2

int main(int argc, char *argv[])
{
	int i;
	int fail = 0;

	BigNumber *a = cop_bn_new_int(33);
	BigNumber *b = cop_bn_new_int(27);
	BigNumber *n = cop_bn_new_int(59);
	BigNumber *r;

	r = cop_madd(a, b, n);
	if (r->number[ r->size - 1 ] != 1) {
    	printf("MADD %d\n", sizeof(int));
    	test_memprint(a->number, a->size);
    	test_memprint(b->number, b->size);
    	test_memprint(n->number, n->size);
		test_memprint(r->number, r->size);
		fail++;
	}

	r = cop_msub(a, b, n);
	if (r->number[ r->size - 1] != 6) {
		printf("MSUB\n");
    	test_memprint(a->number, a->size);
    	test_memprint(b->number, b->size);
    	test_memprint(n->number, n->size);
		test_memprint(r->number, r->size);
		fail++;
	}

/*	printf("MRED\n");
	cop_mred(r, a, n);
	test_memprint(r->number, r->size);
	if (r->number[6] != 6)
		fail++;

    printf("PADD\n");

    BigNumber xp = cop_get_bn( t, 7 );
    BigNumber yp = cop_get_bn( t, 8 );
    BigNumber xq = cop_get_bn( t, 9 );
    BigNumber yq = cop_get_bn( t, 10 );
    BigNumber bb = cop_get_bn( t, 9 );
    BigNumber nn = cop_get_bn( t, 11 );

    BigNumber r2 = cop_padd( t, &xp, &yp, &xq, &yq, &nn,&bb );

    cop_print_bn(&r2);
*/
	cop_bn_free(r);
	cop_bn_free(a);
	cop_bn_free(b);
	cop_bn_free(n);
	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return fail;
}
