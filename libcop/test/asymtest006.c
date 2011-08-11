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
#include "utils.h"
#include <openssl/bn.h>

#define NUM_TESTS 10

#define PRINT_BN(bn) {\
	printf(#bn " = ");\
	BN_print_fp(stdout, bn);\
	printf("\n");}

#define TEST_CASE_ASYM(bn_func, cop_func) {\
		BN_ ##bn_func (r_hw, a, b, n, ctx);\
		volatile struct cop_csb *csb;\
		struct cop_crb *crb;\
		cop_session_t *session = cop_create_asym_session();\
		bn_r = cop_ ##cop_func ##_async (session, bn_a, bn_b, bn_n);\
		cop_commit_session(session);\
		while(!cop_session_iscomplete(session));\
		r_sw = cop_convert_bignumber(bn_r);\
		if (BN_cmp(r_hw, r_sw)) {\
			printf("FAIL!\n");\
			printf(#bn_func " = ");\
			BN_print_fp(stdout, r_hw);\
			printf("\n");\
			printf(#cop_func " = ");\
			BN_print_fp(stdout, r_sw);\
			printf("\n");\
			fail++;\
		}\
		cop_free_session(session);\
		total++;\
}

BigNumber *cop_convert_bignum(BIGNUM * bn)
{
	BigNumber *r = cop_bn_new_sz((size_t) BN_num_bytes(bn));
	BN_bn2bin(bn, r->number);
	return r;
}

BIGNUM *cop_convert_bignumber(BigNumber * bn)
{
	BIGNUM *r = BN_new();
	BN_bin2bn(bn->number, bn->size, r);
	return r;
}

int main(int argc, char *argv[])
{
	int bitlens[] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };
	BIGNUM *r_hw = BN_new();
	BIGNUM *r_sw = BN_new();
	BIGNUM *a = BN_new();
	BIGNUM *b = BN_new();
	BIGNUM *n = BN_new();
	BigNumber *bn_r;
	BigNumber *bn_a;
	BigNumber *bn_b;
	BigNumber *bn_n;
	int i = 0;
	int j = 0;
	int fail = 0;
	int total = 0;

	BN_CTX *ctx = BN_CTX_new();
	//bn_r = cop_bn_new_sz(256);

	for (j = 0; j < (sizeof(bitlens) / sizeof(int)); j++) {
		for (i = 0; i < NUM_TESTS; i++) {
			//printf("test %d-%d\n", i, j);

			// Generate random parameters
			BN_pseudo_rand(n, bitlens[j], 0, 1);
			BN_pseudo_rand(b, bitlens[j], 0, 0);
			BN_pseudo_rand_range(a, n);

			// Setup bignumbers
			size_t a_sz = BN_num_bytes(a);
			if (a_sz) {
				bn_a = cop_bn_new_sz(BN_num_bytes(a));
				BN_bn2bin(a, bn_a->number);
			} else {
				bn_a = cop_bn_new_int(0);
			}

			bn_b = cop_bn_new_sz(BN_num_bytes(b));
			bn_n = cop_bn_new_sz(BN_num_bytes(n));
			BN_bn2bin(b, bn_b->number);
			BN_bn2bin(n, bn_n->number);

			// Perform tests
			TEST_CASE_ASYM(mod_add, madd);
			TEST_CASE_ASYM(mod_sub, msub);
			TEST_CASE_ASYM(mod_mul, mmul);
			TEST_CASE_ASYM(mod_exp, mex);

			// Free memory
			cop_bn_free(bn_a);
			cop_bn_free(bn_b);
			cop_bn_free(bn_n);
		}
	}

	cop_bn_free(bn_r);
	BN_free(r_hw);
	BN_free(r_sw);
	BN_free(a);
	BN_free(b);
	BN_free(n);
	BN_CTX_free(ctx);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;
}

