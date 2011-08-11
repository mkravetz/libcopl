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
#include <papi.h>
#include <libcop.h>
#include <openssl/bn.h>
#include <pthread.h>
#include <semaphore.h>

sem_t central_mutex;
int TYPE;
int KEYSIZE;
int NUM_THREADS;
int NUM_TESTS_PER_THREAD;
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
      strcpy(EventCodeStr1,"WSP_CRYPTO_CRB_GOOD_AMF");
      strcpy(EventCodeStr2,"PM_COMP_CRYPTO_PBIC_255");
      break;
    case 2: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_MARKED_CRB_ACT_AMF");
      strcpy(EventCodeStr2,"WSP_CRYPTO_MARKED_CRB_RCV_AMF");
      break;
    case 3: 
      strcpy(EventCodeStr1,"WSP_CRYPTO_IDLE_ENG_AMF_3");
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
     fprintf(stdout, "Throughput (ops/s) = (Packets/PBUS_cycles)*(1.75G PBUS_cycles/sec) = %f\n", values[0]*(double)(1.75)/values[1]);
     break;
   case 2: 
     fprintf(stdout, "LATENCY\n");
     fprintf(stdout, "Packets Marked = %d\n", values[1]);
     fprintf(stdout, "Packets Expected = %d\n", NUM_THREADS*NUM_TESTS_PER_THREAD);
     fprintf(stdout, "PBUS cycles for Marked Requests = %d\n", values[0]);
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

	BigNumber *bn_r[NUM_TESTS_PER_THREAD];
	BigNumber *bn_a[NUM_TESTS_PER_THREAD];
	BigNumber *bn_dp[NUM_TESTS_PER_THREAD];
	BigNumber *bn_dq[NUM_TESTS_PER_THREAD];
	BigNumber *bn_d[NUM_TESTS_PER_THREAD];
	BigNumber *bn_np[NUM_TESTS_PER_THREAD];
	BigNumber *bn_nq[NUM_TESTS_PER_THREAD];
	BigNumber *bn_n[NUM_TESTS_PER_THREAD];
	BigNumber *bn_u[NUM_TESTS_PER_THREAD];

	BN_CTX* ctx = BN_CTX_new();

	sem_wait(&central_mutex);

	struct cop_crb *crb[NUM_TESTS_PER_THREAD];
	struct cop_csb *csb[NUM_TESTS_PER_THREAD];
	cop_session_t *session[NUM_TESTS_PER_THREAD];


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
	
	 	bn_r[i] = cop_bn_new_sz(KEYSIZE);
		bn_a[i] = cop_convert_bignum(I);
		bn_dp[i] = cop_convert_bignum(dmp1);
		bn_dq[i] = cop_convert_bignum(dmq1);
		bn_d[i] = cop_convert_bignum(d);
		bn_np[i] = cop_convert_bignum(p);
		bn_nq[i] = cop_convert_bignum(q);
		bn_n[i] = cop_convert_bignum(n);
		bn_u[i] = cop_convert_bignum(iqmp);

		session[i] = cop_create_session();

		switch(TYPE){
	      	case 1:
	        	bn_r[i] = cop_mexc_async(session[i], bn_a[i], bn_dp[i], bn_dq[i],bn_np[i], bn_nq[i], bn_u[i]);
				if (session[i] != cop_commit_session_async(session[i]))	
				{
	          	  printf((": call failed in thread %d\n"),parm->id);
	          	  parm->fail++;
	        	}
	       		parm->total++;
			break;
	      	case 2:
	        	bn_r[i] = cop_mex_async(session[i], bn_a[i], bn_d[i], bn_n[i]);
				if (session != cop_commit_session_async(session[i]))	
				{
	          	  printf((": call failed in thread %d\n"),parm->id);
	          	  parm->fail++;
	        	}
	        	parm->total++;
			break;
	      	default: printf("Incorrect TYPE\n"); break;
		}
	}
	for(i=0; i < NUM_TESTS_PER_THREAD; i++) {
		while(!cop_session_iscomplete(session[i])) { ;; }
		cop_bn_free(bn_r[i]);
		cop_bn_free(bn_a[i]);
		cop_bn_free(bn_dp[i]);
		cop_bn_free(bn_dq[i]);
		cop_bn_free(bn_d[i]);
		cop_bn_free(bn_np[i]);
		cop_bn_free(bn_nq[i]);
		cop_bn_free(bn_n[i]);
		cop_bn_free(bn_u[i]);
		cop_free_session(session[i]);
	}

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
  	if(argc != 6){
	  printf("Usage: %s TYPE(1=mexc, 2=mex) KEYSIZE(1024, 2048) NUM_THREADS NUM_TESTS_PER_THREAD COUNT(0=none, 1=Throughput 2=latency 3=engine utilization)\n", argv[0]);
	  return 0;
	}
	else{
	  TYPE = atoi(argv[1]);
	  KEYSIZE = atoi(argv[2]);
	  NUM_THREADS = atoi(argv[3]);
	  NUM_TESTS_PER_THREAD = atoi(argv[4]);
	  COUNT =  atoi(argv[5]);
          if(COUNT!=0 && COUNT!=1 && COUNT!=2 && COUNT!=3){
	    printf("COUNT can only be 0, 1, 2 or 3\n");
	    return(0);
	  }
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

	//start the counters
	startPMUCounter(COUNT);

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

	//stop the counters
	stopPMUCounter(COUNT);

	//report stats on each threads
	for(i=0;i<NUM_THREADS;i++){
	  printf("=== %s: Thread %d: %d/%d failures ===\n", argv[0], i, params[i].fail, params[i].total);
	}

	return failTh;
}

