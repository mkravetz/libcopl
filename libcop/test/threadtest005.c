/*
 
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009
   Chris J Arges
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

/* a _VERY_ simple pthread test */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <libcop.h>

#define NUM_TESTS 10

typedef struct {
	int id;
} parameters;

sem_t central_mutex;

void work(void *args)
{
	int i;
	int fail = 0;
	long values[1000];

	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id;

	sem_wait(&central_mutex);

	for (i = 0; i < 1000; i++) {
		values[i] = cop_random();
		//if (i % 100 == 0)
		//	printf("id %x  : i %d rnd numbers complete\n", id, i);

		if (errno != 0 || values[i] == 0)
			fail++;
	}

	/* print id */
	printf("%03x ", id);
}

int main(int argc, char *argv[])
{
	int i = 0;
	int fail = 0;
	pthread_t threads[NUM_TESTS];
	parameters params;

	sem_init(&central_mutex, 0, 0);
	for (i = 0; i < NUM_TESTS; i++) {
		params.id = i;
		if (pthread_create(&threads[i], NULL, (void *)&work, &params))
			fail++;
	}

	// release the dogs
	for (i = 0; i < NUM_TESTS; i++) {
		sem_post(&central_mutex);
	}

	for (i = 0; i < NUM_TESTS; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}

	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return fail;
}
