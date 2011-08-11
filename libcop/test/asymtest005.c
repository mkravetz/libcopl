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
#include <errno.h>
#include <libcopmalloc.h>
#include "libcop.h"
#include "config.h"

#define NUM_TESTS 300

typedef struct {
	int id;
} parameters;

int valid[NUM_TESTS];
int fail = 0;
pthread_mutex_t mutex;

#define CHECK_ERROR() { \
    if( errno ) { \
        printf("(%x) Error: %d\n", id, errno);\
        pthread_mutex_lock(&mutex); \
        fail++; \
        pthread_mutex_unlock(&mutex); \
    } else { \
    } \
}

void asym_test(void *args)
{
	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id;
   
   /* FIXME: these numbers need to be valid numbers, and this was what
	* was causing the failures before. Please consult the HRM to determine
	* the constraints on operands */
    BigNumber *op1 = cop_bn_new_int( 1 ); //cop_rand() );
    BigNumber *op2 = cop_bn_new_int( 2 ); //cop_rand() );
    BigNumber *op3 = cop_bn_new_int( 3 ); //cop_rand() );
    BigNumber *r = NULL;
	errno = 0;

	/* don't use minv if we're on DD1 */ 
	int switch_num = gDD_version > 1 ? 7: 6;

    switch( id % switch_num ) {
    case 0:
        r = cop_madd( op1, op2, op3 );
        break;
    case 1:
        r = cop_msub( op1, op2, op3 );
        break;
    case 2:
        r = cop_mred( op1, op3 );
        break;
    case 3:
        r = cop_mex( op1, op2, op3 );
        break;
    case 4:
        r = cop_mmul( op1, op2, op3 );
        break;
    case 5:
        r = cop_mgmul( op1, op2, op3 );
        break;
    case 6:
        r = cop_minv( op1, op3 );
        break;
    }

    CHECK_ERROR();

    cop_bn_print(op1);
    cop_bn_print(op2);
    cop_bn_print(op3);

    if( r != NULL )
        cop_bn_print(r);

    cop_bn_free( op1 );
    cop_bn_free( op2 );
    cop_bn_free( op3 );
    
    if( r != NULL )
        cop_bn_free( r );

	//printf("[%d]", id);
}

int main(int argc, char *argv[])
{
	int i, ok, rc;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	pthread_t *threads = NULL;
	parameters params;

	//---SPAWNING THREADS---
	threads = (pthread_t *) malloc(NUM_TESTS * sizeof(pthread_t *));
	for (i = 0; i < NUM_TESTS; i++) {
		params.id = i;
		rc = pthread_create (&threads[i], NULL, (void *)&asym_test, &params);
        pthread_mutex_lock(&mutex);
		if (rc) {
			fail++;
			valid[i] = 0;
		} else {
			valid[i] = 1;
		}
        pthread_mutex_unlock(&mutex);
	}

	for (i = 0; i < NUM_TESTS; i++) {
		if (valid[i])
			if (pthread_join(threads[i], NULL))
				fail++;
	}

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return 0;
}
