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
#include <papi.h>
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
int COUNT;


//--------- PAPI INITIALIZATION & COUNTER START -----------
static int EventSet = PAPI_NULL;
static unsigned long startPMUCounter(int count, int type)
{
  PAPI_event_code_t EventCode1;
  PAPI_event_code_t EventCode2;
  int retval;
  char EventCodeStr1[128];
  char EventCodeStr2[128];
  if(type<100){
   switch(count){
    case 0: return 0;
    case 1:
      strcpy(EventCodeStr1,"WSP_CRYPTO_CRB_GOOD_AESSHA");
      strcpy(EventCodeStr2,"PM_COMP_CRYPTO_PBIC_255");
      break;
    case 2: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_MARKED_CRB_ACT_AESSHA");
      strcpy(EventCodeStr2,"WSP_CRYPTO_MARKED_CRB_RCV_AESSHA");
      break;
    case 3: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_IDLE_ENG_AESSHA_0");
      strcpy(EventCodeStr2,"WSP_COMP_CRYPTO_PBIC_255");
      break;
    default:
      fprintf(stderr, "Error: Do not understand which events to count");
      exit(1);
   }    
  }
  else{
   switch(count){
    case 0: return 0;
    case 1:
      strcpy(EventCodeStr1,"WSP_CRYPTO_CRB_GOOD_DESSHA");
      strcpy(EventCodeStr2,"PM_COMP_CRYPTO_PBIC_255");
      break;
    case 2: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_MARKED_CRB_ACT_DESSHA");
      strcpy(EventCodeStr2,"WSP_CRYPTO_MARKED_CRB_RCV_DESSHA");
      break;
    case 3: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_IDLE_ENG_DESSHA_0");
      strcpy(EventCodeStr2,"WSP_COMP_CRYPTO_PBIC_255");
      break;
    default:
      fprintf(stderr, "Error: Do not understand which events to count");
      exit(1);
   }    
  }
  /* Initialize the library */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if  (retval != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error! %d\n",retval);
    exit(1);
  }

  /* Get EventCode1*/
  retval = PAPI_event_name_to_code(EventCodeStr1, &EventCode1);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_event_name_to_code failed! %d\n",retval);
    exit(1);
  }
  printf("Event Code for %s is 0x%llX\n",EventCodeStr1,EventCode1);

  /* Get EventCode2*/
  retval = PAPI_event_name_to_code(EventCodeStr2, &EventCode2);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_event_name_to_code failed! %d\n",retval);
    exit(1);
  }
  printf("Event Code for %s is 0x%llX\n",EventCodeStr2,EventCode2);

  /* Create EventSet*/
  retval = PAPI_create_eventset(&EventSet);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_create_eventset failed! %d\n",retval);
    exit(1);
  }

  /* Add EventCode1 to EventSet*/
  retval = PAPI_add_event(EventSet, EventCode1);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_add_event failed! %d\n",retval);
    exit(1);
  }

  /* Add EventCode2 to EventSet*/
  retval = PAPI_add_event(EventSet, EventCode2);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_add_event failed! %d\n",retval);
    exit(1);
  }

  /* Start counting events in EventSet*/
  retval = PAPI_start(EventSet);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_start failed! %d\n",retval);
    exit(1);
  }

  return 0;
}
//-------------------------------------------

//----------------- COUNTER STOP ------------
static unsigned long stopPMUCounter(int count)
{
  long long *values;
  values = malloc(sizeof(long long)*2); //currently only 2 return values expected
  int retval = PAPI_stop(EventSet, values);
  if(retval != PAPI_OK) {
    fprintf(stderr, "PAPI_stop failed! %d\n",retval);
    exit(1);
  }
  switch(count){
   case 0: return 0;
   case 1: 
     fprintf(stdout, "THROUGHPUT\n");
     fprintf(stdout, "Packets Received = %d\n", values[0]);
     fprintf(stdout, "Packets Expected = %d\n", NUM_THREADS*NUM_TESTS_PER_THREAD);
     fprintf(stdout, "PBUS cycles = %d\n", values[1]);
     fprintf(stdout, "Bytes/Packet = %d\n", PKT_SIZE_BYTES);
     fprintf(stdout, "Throughput (GBps) = (Bytes/Packet)*(Packets/PBUS_cycles)*(1.75G PBUS_cycles/sec) = %f\n", values[0]*(double)(1.75)*(double)(PKT_SIZE_BYTES)/values[1]);
     break;
   case 2: 
     fprintf(stdout, "LATENCY\n");
     fprintf(stdout, "Packets Marked = %d\n", values[1]);
     fprintf(stdout, "Packets Expected = %d\n", NUM_THREADS*NUM_TESTS_PER_THREAD);
     fprintf(stdout, "PBUS cycles for Marked Requests = %d\n", values[0]);
     fprintf(stdout, "Bytes/Packet = %d\n", PKT_SIZE_BYTES);
     fprintf(stdout, "Latency (PBUS cycles) = %f\n", values[0]/values[1]);
     break;
   case 3: 
     fprintf(stdout, "UTILIZATION\n");
     fprintf(stdout, "Idle Cycles (for the first channel that supports this algo) = %d\n", values[0]);
     fprintf(stdout, "PBUS cycles = %d\n", values[1]);
     fprintf(stdout, "Utilization (%) = %f\n", 100*(1-(values[0]/values[1])));
     break;
  } 
  return 0;
}
//------------------------------------------



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
	  if(type(indata, indata_len, outdata, NULL, hkey, NULL, iv, ckey, outdigest, 0x01, COP_RPAD_NONE, 0x00)){\
	    printf(": call failed in thread %d\n", p->id);\
	    p->fail++;\
	  }\
	  p->total++;\
	}\
}

#define TESTTDESHMAC(type, shorttype, size){\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  generate_test_data(indata,hkey,ckey,iv);\
	  if(type(indata, indata_len, outdata, NULL, hkey, NULL, iv, ckey, outdigest, 0x01, COP_RPAD_NONE, 0x00)){\
	    printf(": call failed in thread %d\n", p->id);\
	    p->fail++;\
	  }\
	  p->total++;\
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
	  case 1: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha1, combo, 20);break;
	  case 2: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha256, combo, 32);break;
	  case 3: TESTAESHMAC(cop_eta_aes128_cbc_hmac_sha512, combo512, 64);break;
	  case 4: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha1, combo, 20);break;
	  case 5: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha256, combo, 32);break;
	  case 6: TESTAESHMAC(cop_eta_aes192_cbc_hmac_sha512, combo512, 64);break;
	  case 7: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha1, combo, 20);break;
	  case 8: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha256, combo, 32);break;
	  case 9: TESTAESHMAC(cop_eta_aes256_cbc_hmac_sha512, combo512, 64);break;
	  case 11: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha1, combo, 20);break;
	  case 12: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha256, combo, 32);break;
	  case 13: TESTAESHMAC(cop_ate_aes128_cbc_hmac_sha512, combo512, 64);break;
	  case 14: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha1, combo, 20);break;
	  case 15: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha256, combo, 32);break;
	  case 16: TESTAESHMAC(cop_ate_aes192_cbc_hmac_sha512, combo512, 64);break;
	  case 17: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha1, combo, 20);break;
	  case 18: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha256, combo, 32);break;
	  case 19: TESTAESHMAC(cop_ate_aes256_cbc_hmac_sha512, combo512, 64);break;
	  case 21: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha1, combo, 20);break;
	  case 22: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha256, combo, 32);break;
	  case 23: TESTAESHMAC(cop_eaa_aes128_cbc_hmac_sha512, combo512, 64);break;
	  case 24: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha1, combo, 20);break;
	  case 25: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha256, combo, 32);break;
	  case 26: TESTAESHMAC(cop_eaa_aes192_cbc_hmac_sha512, combo512, 64);break;
	  case 27: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha1, combo, 20);break;
	  case 28: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha256, combo, 32);break;
	  case 29: TESTAESHMAC(cop_eaa_aes256_cbc_hmac_sha512, combo512, 64);break;
	  case 31: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha1, combo, 20);break;
	  case 32: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha256, combo, 32);break;
	  case 33: TESTAESHMAC(cop_eta_aes128_ctr_hmac_sha512, combo512, 64);break;
	  case 34: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha1, combo, 20);break;
	  case 35: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha256, combo, 32);break;
	  case 36: TESTAESHMAC(cop_eta_aes192_ctr_hmac_sha512, combo512, 64);break;
	  case 37: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha1, combo, 20);break;
	  case 38: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha256, combo, 32);break;
	  case 39: TESTAESHMAC(cop_eta_aes256_ctr_hmac_sha512, combo512, 64);break;
	  case 41: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha1, combo, 20);break;
	  case 42: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha256, combo, 32);break;
	  case 43: TESTAESHMAC(cop_ate_aes128_ctr_hmac_sha512, combo512, 64);break;
	  case 44: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha1, combo, 20);break;
	  case 45: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha256, combo, 32);break;
	  case 46: TESTAESHMAC(cop_ate_aes192_ctr_hmac_sha512, combo512, 64);break;
	  case 47: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha1, combo, 20);break;
	  case 48: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha256, combo, 32);break;
	  case 49: TESTAESHMAC(cop_ate_aes256_ctr_hmac_sha512, combo512, 64);break;
	  case 51: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha1, combo, 20);break;
	  case 52: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha256, combo, 32);break;
	  case 53: TESTAESHMAC(cop_eaa_aes128_ctr_hmac_sha512, combo512, 64);break;
	  case 54: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha1, combo, 20);break;
	  case 55: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha256, combo, 32);break;
	  case 56: TESTAESHMAC(cop_eaa_aes192_ctr_hmac_sha512, combo512, 64);break;
	  case 57: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha1, combo, 20);break;
	  case 58: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha256, combo, 32);break;
	  case 59: TESTAESHMAC(cop_eaa_aes256_ctr_hmac_sha512, combo512, 64);break;
	  case 101: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha1, combo, 20);break;
	  case 102: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha256, combo, 32);break;
	  case 103: TESTTDESHMAC(cop_eta_tdes_cbc_hmac_sha512, combo512, 64);break;
	  case 111: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha1, combo, 20);break;
	  case 112: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha256, combo, 32);break;
	  case 113: TESTTDESHMAC(cop_ate_tdes_cbc_hmac_sha512, combo512, 64);break;
	  case 121: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha1, combo, 20);break;
	  case 122: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha256, combo, 32);break;
	  case 123: TESTTDESHMAC(cop_eaa_tdes_cbc_hmac_sha512, combo512, 64);break;
	  default: printf("Incorrect TYPE\n"); break;
	}
}

int main(int argc, char **argv)
{
	if(argc != 6){
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
	  ) PKT_SIZE_BYTES   NUM_THREADS   NUM_TESTS_PER_THREAD COUNT(0=none, 1=Throughput 2=latency 3=engine utilization)\n", argv[0]);
	  return 0;
	} 
	else{
	  TYPE =  atoi(argv[1]);
	  PKT_SIZE_BYTES =  atoi(argv[2]);
	  NUM_THREADS =  atoi(argv[3]);
	  NUM_TESTS_PER_THREAD =  atoi(argv[4]);
	  COUNT =  atoi(argv[5]);
	  if(COUNT!=0 && COUNT!=1 && COUNT!=2 && COUNT!=3){
	    printf("COUNT can only be 0, 1, 2 or 3\n");
	    return(0);
	  }
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

	//start the coutners
	startPMUCounter(COUNT,TYPE);

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

	//stop the coutners
	stopPMUCounter(COUNT);

	//report stats on each thread
	for(i=0;i<NUM_THREADS;i++){
	  printf("=== %s: Thread %d: %d/%d failures ===\n", argv[0], i, params[i].fail, params[i].total);
	}

	return failTh;
}
