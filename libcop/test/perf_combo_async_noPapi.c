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
January 2010

Testcase developed by Anil Krishna, IBM Corp., based on several example 
testcases written by Chris Arges and team.

This is a testcase to measure performance of the crypto accelerator hardware.

This code tries to stress test the crypto hardware with many requests of a 
given size and for a given cryptographic algorithm. 

This test can then be used with wrapper code that measures throughput and 
latency of the crypto unit.
*/

#include <stdio.h>
#include <stdlib.h>
#include <libcop.h>
#include <openssl/bn.h>
#include <pthread.h>
#include <semaphore.h>

sem_t central_mutex;
int TYPE;
int PKT_SIZE_BYTES;
int NUM_THREADS;
int NUM_TESTS_PER_THREAD;
int DGST_SIZE_BYTES = 64;
int HKEY_SIZE_BYTES = 32;
int CKEY_SIZE_BYTES = 32;
int IV_SIZE_BYTES = 8;


typedef struct {
  int id;
  int fail;
  int total;
} parameters;

int generate_test_data( unsigned char *data, unsigned char* hkey, unsigned char* ckey, unsigned char* iv)
{
	BIGNUM *BN_data = BN_new();
	BIGNUM *BN_hkey = BN_new();
	BIGNUM *BN_ckey = BN_new();
	BIGNUM *BN_iv = BN_new();
	BN_CTX *ctx = BN_CTX_new();
	BN_pseudo_rand(BN_data, PKT_SIZE_BYTES*8, -1, 0);
	BN_pseudo_rand(BN_hkey, HKEY_SIZE_BYTES*8, -1, 0);
	BN_pseudo_rand(BN_ckey, CKEY_SIZE_BYTES*8, -1, 0);
	BN_pseudo_rand(BN_iv, IV_SIZE_BYTES*8, -1, 0);
	BN_bn2bin(BN_data, data);
	BN_bn2bin(BN_hkey, hkey);
	BN_bn2bin(BN_ckey, ckey);
	BN_bn2bin(BN_iv, iv);
	BN_CTX_free(ctx);
}

#define TESTAESHMAC(type, shorttype, size){\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  generate_test_data(indata,hkey,ckey,iv);\
	  if(type(indata, indata_len, outdata, NULL, hkey, NULL, iv, ckey, outdigest, 0x01, COP_RPAD_NONE, 0x00, ASYNC_INTERRUPT, &crb[i])){\
	    printf(": call failed in thread %d\n", p->id);\
	    p->fail++;\
	  }\
	  p->total++;\
	}\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  csb[i]=cop_get_csbaddr(crb[i]);\
	  while(csb[i]->valid_bit == 0);\
	  cop_symcrypto_cpb_ ##shorttype *cpb = cop_get_cpbptr(crb[i]);\
	  memcpy(outdigest, cpb->mac, size);\
	  cop_give_back_crb(cop_cc_pool(), crb[i], COP_SYM_CRYPTO);\
	}\
}

#define TESTTDESHMAC(type, shorttype, size){\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  generate_test_data(indata,hkey,ckey,iv);\
	  if(type(indata, indata_len, outdata, NULL, hkey, NULL, iv, ckey, outdigest, 0x01, COP_RPAD_NONE, 0x00, ASYNC_INTERRUPT, &crb[i])){\
	    printf(": call failed in thread %d\n", p->id);\
	    p->fail++;\
	  }\
	  p->total++;\
	}\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  csb[i]=cop_get_csbaddr(crb[i]);\
	  while(csb[i]->valid_bit == 0);\
	  cop_symcrypto_cpb_ ##shorttype *cpb = cop_get_cpbptr(crb[i]);\
	  memcpy(outdigest, cpb->mac, size);\
	  cop_give_back_crb(cop_cc_pool(), crb[i], COP_SYM_CRYPTO);\
	}\
}

void work(void *args){
	parameters *p = (parameters*) args;
	printf("Started Thread %03x: \n", p->id);
        int i = 0;

	unsigned char* indata;
	indata=(char*)malloc(PKT_SIZE_BYTES);
	unsigned char* hkey;
	hkey=(char*)malloc(HKEY_SIZE_BYTES);
	unsigned char* ckey;
	ckey=(char*)malloc(CKEY_SIZE_BYTES);
	unsigned char* iv;
	iv=(char*)malloc(IV_SIZE_BYTES);
	unsigned char* outdata;
	outdata=(char*)malloc(PKT_SIZE_BYTES);
	unsigned char* outdigest;
	outdigest=(char*)malloc(DGST_SIZE_BYTES);

	int indata_len = PKT_SIZE_BYTES;

	sem_wait(&central_mutex);

        struct cop_crb *crb[NUM_TESTS_PER_THREAD];
	struct cop_csb *csb[NUM_TESTS_PER_THREAD];

	switch(TYPE){
	  case 1: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha1_async, combo, 20);break;
	  case 2: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha256_async, combo, 32);break;
	  case 3: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha512_async, combo512, 64);break;
	  case 4: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha1_async, combo, 20);break;
	  case 5: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha256_async, combo, 32);break;
	  case 6: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha512_async, combo512, 64);break;
	  case 7: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha1_async, combo, 20);break;
	  case 8: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha256_async, combo, 32);break;
	  case 9: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha512_async, combo512, 64);break;
	  case 11: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha1_async, combo, 20);break;
	  case 12: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha256_async, combo, 32);break;
	  case 13: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha512_async, combo512, 64);break;
	  case 14: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha1_async, combo, 20);break;
	  case 15: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha256_async, combo, 32);break;
	  case 16: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha512_async, combo512, 64);break;
	  case 17: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha1_async, combo, 20);break;
	  case 18: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha256_async, combo, 32);break;
	  case 19: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha512_async, combo512, 64);break;
	  case 21: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha1_async, combo, 20);break;
	  case 22: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha256_async, combo, 32);break;
	  case 23: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha512_async, combo512, 64);break;
	  case 24: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha1_async, combo, 20);break;
	  case 25: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha256_async, combo, 32);break;
	  case 26: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha512_async, combo512, 64);break;
	  case 27: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha1_async, combo, 20);break;
	  case 28: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha256_async, combo, 32);break;
	  case 29: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha512_async, combo512, 64);break;
	  case 31: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha1_async, combo, 20);break;
	  case 32: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha256_async, combo, 32);break;
	  case 33: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha512_async, combo512, 64);break;
	  case 34: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha1_async, combo, 20);break;
	  case 35: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha256_async, combo, 32);break;
	  case 36: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha512_async, combo512, 64);break;
	  case 37: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha1_async, combo, 20);break;
	  case 38: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha256_async, combo, 32);break;
	  case 39: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha512_async, combo512, 64);break;
	  case 41: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha1_async, combo, 20);break;
	  case 42: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha256_async, combo, 32);break;
	  case 43: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha512_async, combo512, 64);break;
	  case 44: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha1_async, combo, 20);break;
	  case 45: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha256_async, combo, 32);break;
	  case 46: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha512_async, combo512, 64);break;
	  case 47: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha1_async, combo, 20);break;
	  case 48: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha256_async, combo, 32);break;
	  case 49: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha512_async, combo512, 64);break;
	  case 51: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha1_async, combo, 20);break;
	  case 52: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha256_async, combo, 32);break;
	  case 53: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha512_async, combo512, 64);break;
	  case 54: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha1_async, combo, 20);break;
	  case 55: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha256_async, combo, 32);break;
	  case 56: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha512_async, combo512, 64);break;
	  case 57: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha1_async, combo, 20);break;
	  case 58: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha256_async, combo, 32);break;
	  case 59: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha512_async, combo512, 64);break;
	  case 101: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha1_async, combo, 20);break;
	  case 102: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha256_async, combo, 32);break;
	  case 103: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha512_async, combo512, 64);break;
	  case 111: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha1_async, combo, 20);break;
	  case 112: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha256_async, combo, 32);break;
	  case 113: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha512_async, combo512, 64);break;
	  case 121: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha1_async, combo, 20);break;
	  case 122: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha256_async, combo, 32);break;
	  case 123: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha512_async, combo512, 64);break;
	  default: printf("Incorrect TYPE\n"); break;
	}
}

int main(int argc, char **argv)
{
	if(argc != 5){
	  printf("Usage: %s   TYPE( \
	  1=eta_aes128_cbc_hmac_sha1, \
	  2=eta_aes128_cbc_hmac_sha256, \
	  3=eta_aes128_cbc_hmac_sha512 \
	  4=eta_aes192_cbc_hmac_sha1, \
	  5=eta_aes192_cbc_hmac_sha256, \
	  6=eta_aes192_cbc_hmac_sha512 \
	  7=eta_aes256_cbc_hmac_sha1, \
	  8=eta_aes256_cbc_hmac_sha256, \
	  9=eta_aes256_cbc_hmac_sha512 \
	  11=ate_aes128_cbc_hmac_sha1, \
	  12=ate_aes128_cbc_hmac_sha256, \
	  13=ate_aes128_cbc_hmac_sha512 \
	  14=ate_aes192_cbc_hmac_sha1, \
	  15=ate_aes192_cbc_hmac_sha256, \
	  16=ate_aes192_cbc_hmac_sha512 \
	  17=ate_aes256_cbc_hmac_sha1, \
	  18=ate_aes256_cbc_hmac_sha256, \
	  19=ate_aes256_cbc_hmac_sha512 \
	  21=eaa_aes128_cbc_hmac_sha1, \
	  22=eaa_aes128_cbc_hmac_sha256, \
	  23=eaa_aes128_cbc_hmac_sha512 \
	  24=eaa_aes192_cbc_hmac_sha1, \
	  25=eaa_aes192_cbc_hmac_sha256, \
	  26=eaa_aes192_cbc_hmac_sha512 \
	  27=eaa_aes256_cbc_hmac_sha1, \
	  28=eaa_aes256_cbc_hmac_sha256, \
	  29=eaa_aes256_cbc_hmac_sha512 \
	  31=eta_aes128_ctr_hmac_sha1, \
	  32=eta_aes128_ctr_hmac_sha256, \
	  33=eta_aes128_ctr_hmac_sha512 \
	  34=eta_aes192_ctr_hmac_sha1, \
	  35=eta_aes192_ctr_hmac_sha256, \
	  36=eta_aes192_ctr_hmac_sha512 \
	  37=eta_aes256_ctr_hmac_sha1, \
	  38=eta_aes256_ctr_hmac_sha256, \
	  39=eta_aes256_ctr_hmac_sha512 \
	  41=ate_aes128_ctr_hmac_sha1, \
	  42=ate_aes128_ctr_hmac_sha256, \
	  43=ate_aes128_ctr_hmac_sha512 \
	  44=ate_aes192_ctr_hmac_sha1, \
	  45=ate_aes192_ctr_hmac_sha256, \
	  46=ate_aes192_ctr_hmac_sha512 \
	  47=ate_aes256_ctr_hmac_sha1, \
	  48=ate_aes256_ctr_hmac_sha256, \
	  49=ate_aes256_ctr_hmac_sha512 \
	  51=eaa_aes128_ctr_hmac_sha1, \
	  52=eaa_aes128_ctr_hmac_sha256, \
	  53=eaa_aes128_ctr_hmac_sha512 \
	  54=eaa_aes192_ctr_hmac_sha1, \
	  55=eaa_aes192_ctr_hmac_sha256, \
	  56=eaa_aes192_ctr_hmac_sha512 \
	  57=eaa_aes256_ctr_hmac_sha1, \
	  58=eaa_aes256_ctr_hmac_sha256, \
	  59=eaa_aes256_ctr_hmac_sha512 \
	  101=eta_tdes_cbc_hmac_sha1, \
	  102=eta_tdes_cbc_hmac_sha256, \
	  103=eta_tdes_cbc_hmac_sha512 \
	  111=ate_tdes_cbc_hmac_sha1, \
	  112=ate_tdes_cbc_hmac_sha256, \
	  113=ate_tdes_cbc_hmac_sha512 \
	  121=eaa_tdes_cbc_hmac_sha1, \
	  122=eaa_tdes_cbc_hmac_sha256, \
	  123=eaa_tdes_cbc_hmac_sha512 \
	  ) PKT_SIZE_BYTES   NUM_THREADS   NUM_TESTS_PER_THREAD\n", argv[0]);
	  return 0;
	} 
	else{
	  TYPE =  atoi(argv[1]);
	  PKT_SIZE_BYTES =  atoi(argv[2]);
	  NUM_THREADS =  atoi(argv[3]);
	  NUM_TESTS_PER_THREAD =  atoi(argv[4]);
	}

	int i = 0;
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

	//wait for all threads to finish
	for(i=0;i<NUM_THREADS;i++){
	  if(pthread_join(threads[i], NULL)){
	    failTh++;
	  }
	}

	//report stats on each thread
	for(i=0;i<NUM_THREADS;i++){
	  printf("=== %s: Thread %d: %d/%d failures ===\n", argv[0], i, params[i].fail, params[i].total);
	}

	return failTh;
}
