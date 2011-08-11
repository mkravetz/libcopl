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
int KEY_SIZE_BYTES = 32;
int IV_SIZE_BYTES = 8;
int COUNT;


//--------- PAPI INITIALIZATION & COUNTER START -----------
static int EventSet = PAPI_NULL;
static unsigned long startPMUCounter(int count)
{
  PAPI_event_code_t EventCode1;
  PAPI_event_code_t EventCode2;
  int retval;
  char EventCodeStr1[128];
  char EventCodeStr2[128];
  switch(count){
    case 0: return 0;
    case 1: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_CRB_GOOD_AES");
      strcpy(EventCodeStr2,"PM_COMP_CRYPTO_PBIC_255");
      break;
    case 2: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_MARKED_CRB_ACT_AES");
      strcpy(EventCodeStr2,"WSP_CRYPTO_MARKED_CRB_RCV_AES");
      break;
    case 3: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_IDLE_ENG_AES_0");
      strcpy(EventCodeStr2,"WSP_COMP_CRYPTO_PBIC_255");
      break;
    default:
      fprintf(stderr, "Error: Do not understand which events to count");
      exit(1);
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

int generate_test_data(unsigned char *data, unsigned char *key,
		       unsigned char *iv)
{
	BIGNUM *BN_key = BN_new();
	BIGNUM *BN_iv = BN_new();
	BIGNUM *BN_data = BN_new();
	BN_CTX *ctx = BN_CTX_new();
	BN_pseudo_rand(BN_key, KEY_SIZE_BYTES*8, -1, 0);
	BN_pseudo_rand(BN_iv, IV_SIZE_BYTES*8, -1, 0);
	BN_pseudo_rand(BN_data, PKT_SIZE_BYTES*8, -1, 0);
	BN_bn2bin(BN_key, key);
	BN_bn2bin(BN_key, iv);
	BN_bn2bin(BN_data, data);
	BN_CTX_free(ctx);
}

#define TESTECB(type){\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  generate_test_data(indata, key, iv);\
	  if(type(indata, indata_len, outdata, key, 0x01, COP_RPAD_NONE, 0x00, ASYNC_INTERRUPT, &crb[i])){\
	    printf(": call failed in thread %d\n", p->id); \
	    p->fail++;\
	  }\
	  p->total++;\
	}\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  csb[i]=cop_get_csbaddr(crb[i]);\
	  while(csb[i]->valid_bit == 0);\
	  memcpy(outdata, (uint64_t)(crb[i]->ptrTD), indata_len);\
	  cop_give_back_crb(cop_cc_pool(), crb[i], COP_SYM_CRYPTO);\
        }\
}

#define TESTCBC(type){\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  generate_test_data(indata, key, iv);\
	  if(type(indata, indata_len, outdata, key, iv, 0x01, COP_RPAD_NONE, 0x00, ASYNC_INTERRUPT, &crb[i])){\
	    printf(": call failed in thread %d\n", p->id); \
	    p->fail++;\
	  }\
	  p->total++;\
	}\
	for (i = 0; i < NUM_TESTS_PER_THREAD; i++) {\
	  csb[i]=cop_get_csbaddr(crb[i]);\
	  while(csb[i]->valid_bit == 0);\
	  memcpy(outdata, (uint64_t)(crb[i]->ptrTD), indata_len);\
	  cop_give_back_crb(cop_cc_pool(), crb[i], COP_SYM_CRYPTO);\
        }\
}

void work(void *args){
	parameters *p = (parameters*) args;
	printf("Started Thread %03x: \n", p->id);
        int i = 0;

	unsigned char* key;
	key=(char*)malloc(KEY_SIZE_BYTES);
	unsigned char* iv;
	iv=(char*)malloc(IV_SIZE_BYTES);
	unsigned char* indata;
	indata=(char*)malloc(PKT_SIZE_BYTES);
	unsigned char* outdata;
	outdata=(char*)malloc(PKT_SIZE_BYTES);

	int indata_len = PKT_SIZE_BYTES; 

	sem_wait(&central_mutex);

	struct cop_crb *crb[NUM_TESTS_PER_THREAD];
	struct cop_csb *csb[NUM_TESTS_PER_THREAD];

	switch(TYPE){
	  case 1: TESTECB(cop_aes_ecb_128_async); break;
	  case 2: TESTECB(cop_aes_ecb_192_async); break;
  	  case 3: TESTECB(cop_aes_ecb_256_async); break;
	  case 4: TESTCBC(cop_aes_cbc_128_async); break;
	  case 5: TESTCBC(cop_aes_cbc_192_async); break;
	  case 6: TESTCBC(cop_aes_cbc_256_async); break;
	  default: printf("Incorrect TYPE\n"); break;
	}
}

int main(int argc, char **argv)
{
	if(argc != 6){
	  printf("Usage: %s   TYPE(1=aes_ecb_128,2=aes_ecb_192, 3=aes_ecb_256, 4=aes_cbc_128, 5=aes_cbc_192, 6=aes_cbc_256)   PKT_SIZE_BYTES   NUM_THREADS   NUM_TESTS_PER_THREAD  COUNT(0=none, 1=Throughput 2=latency 3=engine utilization)\n", argv[0]);
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
	  if(TYPE!=1 && TYPE!=2 && TYPE!=3 && TYPE!=4 && TYPE!=5 && TYPE!=6){
	    printf("TYPE can only be 1, 2, 3, 4, 5 or 6\n");
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

        //start the counters
	startPMUCounter(COUNT);
	
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

        //stop the counters
	stopPMUCounter(COUNT);
	
	//report stats on each thread
	for(i=0;i<NUM_THREADS;i++){
	  printf("=== %s: Thread %d: %d/%d failures ===\n", argv[0], i, params[i].fail, params[i].total);
	}

	return failTh;
}
