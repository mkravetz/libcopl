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
#include <pthread.h>
#include <libcopmalloc.h>
#include "libcop.h"

//#define THREAD_NOS 100
int THREAD_NOS;

typedef struct {
	int id;
} parameters;

pthread_mutex_t mutex;

void asym_test(void *args)
{
	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id;

//	pthread_mutex_lock(&mutex);
    BigNumber *op1 = cop_bn_new_int( cop_rand() );
    BigNumber *op2 = cop_bn_new_int( cop_rand() );

    //simple euclidean algorithm
    while( cop_bn_is_zero(op2) != 1 ){ 
        BigNumber *t = cop_bn_new_sz( sizeof(int) );
        cop_bn_copy(t, op2);
        op2 = cop_mred( op1, op2 );
        cop_bn_copy( op1, t );
        cop_bn_free( t );
    }

    cop_bn_print(op1);
    cop_bn_print(op2);

    cop_bn_free( op1 );
    cop_bn_free( op2 );
//	pthread_mutex_unlock(&mutex);
	
	printf("(%X) Finished successfully\n", id);
}

int main(int argc, char *argv[])
{
	int i, ok, rc, fail = 0;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	pthread_t *threads = NULL;
	parameters params;
	
	/* grab parameters */
	if (argc == 2)
		THREAD_NOS =  atoi(argv[1]);
	else 
		THREAD_NOS = 100;
//	pthread_mutex_init(&mutex, NULL);

	//---SPAWNING THREADS---
	ok = 0;
	threads = (pthread_t *) malloc(THREAD_NOS * sizeof(pthread_t *));
	for (i = 0; i < THREAD_NOS; i++) {
		params.id = i;
		rc = pthread_create (&threads[i], NULL, (void *)&asym_test, &params);
		if (rc) {
			printf("fail %d\n", i);
			fail++;
		} else {
			ok++;
		}
	}

	for (i = 0; i < ok; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}

	free(threads);
	printf("=== %s: %d/%d failures ===\n", argv[0], fail, THREAD_NOS);
	return 0;
}
