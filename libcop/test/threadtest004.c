/*
 
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009
   Chris J Arges
   Everton R Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libcopmalloc.h>
#include <libcop.h>

#define NUM_TESTS	10

typedef struct {
	int id;
} parameters;

int *add_me = NULL;
pthread_mutex_t mutex;

void work(void *args)
{
	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id, i = 0;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	int *local_add = (int *)cop_malloc_unaligned(cc_pool, sizeof(int));

	printf("%03d ", cop_rand());

	pthread_mutex_lock(&mutex);
	cop_memcpy(local_add, add_me, sizeof(int));
	*local_add = *local_add + 1;
	cop_memcpy(add_me, local_add, sizeof(int));
	pthread_mutex_unlock(&mutex);

	cop_free(cc_pool, local_add);
}

int main(int argc, char *argv[])
{
	int i = 0, j = 0;
	int fail = 0;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	pthread_t *threads = NULL;
	parameters params;

	threads = (pthread_t *) malloc(NUM_TESTS * sizeof(pthread_t *));

	add_me = (int *)cop_malloc_unaligned(cc_pool, sizeof(int));
	*add_me = 0;

	for (i = 0; i < NUM_TESTS; i++) {
		params.id = i;
		if (pthread_create(&threads[i], NULL, (void *)&work, &params))
			fail++;
	}

	for (i = 0; i < NUM_TESTS; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}

	printf("Add me %d\n", *add_me);
	if (*add_me != NUM_TESTS)
		fail++;

	free(threads);
	cop_free(cc_pool, add_me);
	threads = NULL;
	add_me = NULL;

	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, 1);
	return fail;
}
