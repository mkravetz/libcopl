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

/*
 * March 2010
 *
 * Testcase developed by Anil Krishna, IBM Corp., based on several example
 * testcases written by Chris Arges and team.
 *
 * This is a testcase to measure performance of the crypto accelerator hardware.
 *
 * This code tries to stress test the crypto hardware with many requests of a
 * given size and for a given cryptographic algorithm.
 *
 * This test can then be used with wrapper code that measures throughput and
 * latency of the crypto unit.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <libcop.h>
#include <openssl/bn.h>
#include <pthread.h>
#include <semaphore.h>

sem_t central_mutex;
int TYPE;
int KEYSIZE;
int NUM_THREADS;
int NUM_TESTS_PER_THREAD;


typedef struct {
  int id;
  int fail;
  int total;
} parameters;

BigNumber *cop_convert_bignum(BIGNUM * bn)
{
        BigNumber *r = cop_bn_new_sz((size_t) BN_num_bytes(bn));
	BN_bn2bin(bn, r->number);
	return r;
}


void work(void* args){
        parameters *parm = (parameters*) args;
	printf("Started Thread %03x: \n", parm->id);
	int i = 0;

	BIGNUM *I = BN_new();
	BIGNUM *e = BN_new();
	BIGNUM *p = BN_new();
	BIGNUM *p1 = BN_new();
	BIGNUM *q = BN_new();
	BIGNUM *q1 = BN_new();
	BIGNUM *dmp1 = BN_new();
	BIGNUM *dmq1 = BN_new();
	BIGNUM *iqmp = BN_new();
	BIGNUM *d = BN_new();
	BIGNUM *n = BN_new();
	BIGNUM *phi = BN_new();

	BigNumber *bn_r, *bn_a, *bn_dp, *bn_dq, *bn_d, *bn_np, *bn_nq, *bn_n, *bn_u;
 	bn_r = cop_bn_new_sz(KEYSIZE);

	BN_CTX* ctx = BN_CTX_new();

	sem_wait(&central_mutex);

	struct cop_crb *crb[NUM_TESTS_PER_THREAD];
	struct cop_csb *csb[NUM_TESTS_PER_THREAD];
	for(i=0; i < NUM_TESTS_PER_THREAD; i++) {
	  	do{
		  BN_generate_prime(p, (KEYSIZE>>1), 0, BN_value_one(), NULL, NULL, NULL);
		  BN_generate_prime(q, (KEYSIZE>>1)+1, 0, BN_value_one(), NULL, NULL, NULL);
	        } while(!BN_cmp(p,q));
	        BN_mul(n,p,q, ctx); //n = p*q
	        BN_sub(p1,p,BN_value_one()); 
	        BN_sub(q1,p,BN_value_one());
	        BN_mul(phi, p1, q1, ctx); //phi = (p-1)*(q-1)
	        BN_set_word(e,65537); //Use a Fermat prime for e
	        BN_mod_inverse(d, e, phi, ctx); //d = (1/e) mod ((p-1)*(q-1))
	        BN_mod_inverse(dmp1, e, p1, ctx); //dmp1 = (1/e) mod (p-1)
	        BN_mod_inverse(dmq1, e, q1, ctx);//dmq1 = (1/e) mod (q-1)
	        BN_mod_inverse(iqmp, q, p, ctx);//iqmp = (1/q) mod p
	        BN_pseudo_rand(I,KEYSIZE,0,0);//pseudo random message

	        bn_a = cop_convert_bignum(I);
	        bn_dp = cop_convert_bignum(dmp1);
	        bn_dq = cop_convert_bignum(dmq1);
	        bn_d = cop_convert_bignum(d);
	        bn_np = cop_convert_bignum(p);
	        bn_nq = cop_convert_bignum(q);
	        bn_n = cop_convert_bignum(n);
	        bn_u = cop_convert_bignum(iqmp);

		switch(TYPE){
	      	case 1:
	        	bn_r = cop_mexc(bn_a, bn_dp, bn_dq,bn_np, bn_nq, bn_u);
				if (!bn_r)	
				{
	          	  printf((": call failed in thread %d\n"),parm->id);
	          	  parm->fail++;
	        	}
	       		parm->total++;
			break;
	      	case 2:
	        	bn_r = cop_mex(bn_a,bn_d,bn_n);
				if (!bn_r) {
	          	  printf((": call failed in thread %d\n"),parm->id);
	          	  parm->fail++;
	        	}
	        	parm->total++;
			break;
	      	default: printf("Incorrect TYPE\n"); break;
		}   
	}
	cop_bn_free(bn_r);
	cop_bn_free(bn_a);
	cop_bn_free(bn_dp);
	cop_bn_free(bn_dq);
	cop_bn_free(bn_d);
	cop_bn_free(bn_np);
	cop_bn_free(bn_nq);
	cop_bn_free(bn_n);
	cop_bn_free(bn_u);
	BN_free(I);
	BN_free(e);
	BN_free(p);
	BN_free(p1);
	BN_free(q);
	BN_free(q1);
	BN_free(dmp1);
	BN_free(dmq1);
	BN_free(iqmp);
	BN_free(d);
	BN_free(n);
	BN_free(phi);
	BN_CTX_free(ctx);
}

int main(int argc, char **argv){
  	if(argc != 5){
	  printf("Usage: %s TYPE(1=mexc, 2=mex) KEYSIZE(1024, 2048) NUM_THREADS NUM_TESTS_PER_THREAD\n", argv[0]);
	  return 0;
	}
	else{
	  TYPE = atoi(argv[1]);
	  KEYSIZE = atoi(argv[2]);
	  NUM_THREADS = atoi(argv[3]);
	  NUM_TESTS_PER_THREAD = atoi(argv[4]);
	  if(TYPE!=1 && TYPE!=2){
	    printf("TYPE can only be 1 or 2\n");
	  }
	}
	int i=0;
	int failTh = 0;
	parameters* params = NULL;
	params = (parameters*)malloc(NUM_THREADS * sizeof(parameters));
	for(i=0; i<NUM_THREADS; i++){
	  params[i].id = i;
	  params[i].fail = 0;
	  params[i].total = 0;
	}

	pthread_t *threads = NULL;
	threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t*));

	//create all the threads; they initialize and wait on a semaphore
	for(i=0;i<NUM_THREADS;i++){
	  if(pthread_create(&threads[i], NULL, (void*)&work, &params[i])){
	    failTh++;
	  }
	}

	//green flag for each of the threads to go
	for(i=0;i<NUM_THREADS;i++){
	  sem_post(&central_mutex);
	}

	//wait for all the threads to finish
	for(i=0; i<NUM_THREADS; i++){
	  if(pthread_join(threads[i], NULL)){
	    failTh++;
	  }
	}

	//report stats on each threads
	for(i=0;i<NUM_THREADS;i++){
	  printf("=== %s: Thread %d: %d/%d failures ===\n", argv[0], i, params[i].fail, params[i].total);
	}

	return failTh;
}

