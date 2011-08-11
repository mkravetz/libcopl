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
#include <openssl/bn.h>
#include "libcop.h"

#define NUM_TESTS	20

#define PRINT_BN(bn) {\
	printf(#bn " = ");\
	BN_print_fp(stdout, bn);\
	printf("\n");}

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
	int fail = 0;
	int bitlen = 128;
	BIGNUM *r_mod_exp_crt;
	BIGNUM *r_mod_exp_hw;
	BIGNUM *r_mod_exp = BN_new();

	BIGNUM *I = BN_new();
	BIGNUM *e = BN_new();
	BIGNUM *p = BN_new();
	BIGNUM *p1 = BN_new();
	BIGNUM *q = BN_new();
	BIGNUM *q1 = BN_new();
	BIGNUM *dmp1 = BN_new();
	BIGNUM *dmq1 = BN_new();
	BIGNUM *iqmp = BN_new();
	BIGNUM *temp = BN_new();
	BIGNUM *c = BN_new();
	BIGNUM *d = BN_new();
	BIGNUM *n = BN_new();
	BIGNUM *phi = BN_new();

	BigNumber *bn_a, *bn_d, *bn_n, *bn_r, *bn_dp, *bn_dq, *bn_np, *bn_nq,
	    *bn_u;
	int i = 0;

	BN_CTX *ctx = BN_CTX_new();

	do {
		// Generate p and q as distinct primes
		do {
			BN_generate_prime(p, bitlen, 0, BN_value_one(), NULL,
					  NULL, NULL);
			BN_generate_prime(q, bitlen, 0, BN_value_one(), NULL,
					  NULL, NULL);
		} while (!BN_cmp(p, q));

		// Calculate n = pq
		BN_mul(n, p, q, ctx);

		// Calculate phi = (p-1)(q-1)
		BN_sub(p1, p, BN_value_one());
		BN_sub(q1, q, BN_value_one());
		BN_mul(phi, p1, q1, ctx);

		// Select e s.t. e < n and gcd(e, phi) = 1
		/*do {
		   BN_pseudo_rand_range(e, n);
		   BN_gcd(temp, e, phi, ctx);
		   } while (!BN_is_one(temp));
		 */

		// Use a Fermant prime for e
		BN_set_word(e, 65537);

		// Calculate d = (1/e) mod phi
		BN_mod_inverse(d, e, phi, ctx);

		// Calculate other numbers
		BN_mod_inverse(dmp1, e, p1, ctx);
		BN_mod_inverse(dmq1, e, q1, ctx);
		BN_mod_inverse(iqmp, q, p, ctx);

		// Generate random message
		BN_pseudo_rand(I, bitlen, 0, 0);

		// Perform the operations

		// Test with openssl sw
		BN_mod_exp(r_mod_exp, I, d, n, ctx);

		// Test wsp hw
		bn_a = cop_convert_bignum(I);
		bn_dp = cop_convert_bignum(dmp1);
		bn_dq = cop_convert_bignum(dmq1);
		bn_np = cop_convert_bignum(p);
		bn_nq = cop_convert_bignum(q);
		bn_u = cop_convert_bignum(iqmp);

		bn_d = cop_convert_bignum(d);
		bn_n = cop_convert_bignum(n);

		// Calculate mod_exp_crt
		bn_r = cop_mexc(bn_a, bn_dp, bn_dq, bn_np, bn_nq, bn_u);
		r_mod_exp_crt = cop_convert_bignumber(bn_r);

		// Calculate mod_exp
		bn_r = cop_mex(bn_a, bn_d, bn_n);
		r_mod_exp_hw = cop_convert_bignumber(bn_r);

		// Count number of tests
		i++;

		if (
			BN_cmp(r_mod_exp, r_mod_exp_crt) ||
			BN_cmp(r_mod_exp, r_mod_exp)) {
			PRINT_BN(p);
			PRINT_BN(q);
			PRINT_BN(n);
			PRINT_BN(p1);
			PRINT_BN(q1);
			PRINT_BN(phi);
			PRINT_BN(e);
			PRINT_BN(d);
			PRINT_BN(dmp1);
			PRINT_BN(dmq1);
			PRINT_BN(iqmp);
			PRINT_BN(I);
			PRINT_BN(r_mod_exp);
			PRINT_BN(r_mod_exp_crt);
			PRINT_BN(r_mod_exp_hw);
			printf("failed.\n");
			fail++;
		}

	} while (i < NUM_TESTS);

	// Free memory
	cop_bn_free(bn_r);
	cop_bn_free(bn_a);
	cop_bn_free(bn_dp);
	cop_bn_free(bn_dq);
	cop_bn_free(bn_np);
	cop_bn_free(bn_nq);
	cop_bn_free(bn_u);
	BN_free(r_mod_exp_crt);
	BN_free(r_mod_exp);
	BN_free(I);
	BN_free(e);
	BN_free(p);
	BN_free(p1);
	BN_free(q);
	BN_free(q1);
	BN_free(dmp1);
	BN_free(dmq1);
	BN_free(iqmp);
	BN_free(temp);
	BN_free(c);
	BN_free(d);
	BN_free(n);
	BN_free(phi);
	BN_CTX_free(ctx);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return fail;
}
