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
#include "utils.h"

#define NUM_TESTS	10

typedef struct {
	int id;
} parameters;

pthread_mutex_t mutex;

void *work(void *args)
{
	parameters *p = (parameters *) args;
	int id = p->id;
	size_t input_sz = 128;
	unsigned char *digest = malloc(input_sz);
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	unsigned char *input = cop_malloc(cc_pool, input_sz, 1);
	memset(input, 0xa5, input_sz);
	input[0] = id;

	/* hash operation */
	cop_sha1(input, input_sz, digest, NULL, COP_FLAG_ONESHOT);

	cop_free(cc_pool, input);
	return digest;
}

int main(int argc, char *argv[])
{
	int i = 0, j = 0;
	int fail = 0, total = 0;
	pthread_t *threads = NULL;
	parameters params;
	unsigned char *digest;

	threads = (pthread_t *) malloc(NUM_TESTS * sizeof(pthread_t *));

	for (i = 0; i < NUM_TESTS; i++) {
		params.id = i;
		if (pthread_create(&threads[i], NULL, (void *)&work, &params))
			fail++;
		total++;
	}

	for (i = 0; i < NUM_TESTS; i++) {
		if (pthread_join(threads[i], &digest))
			fail++;
		printf("i = %d\n", i);
		test_memprint(digest, 20);
		free(digest);
		total++;
	}

	free(threads);
	threads = NULL;

	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, total);
	return fail;
}
