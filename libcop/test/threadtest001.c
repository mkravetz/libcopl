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
#include <errno.h>

//#define NUM_TESTS	1000
int THREAD_NOS;

typedef struct {
	int id;
} parameters;

void work(void *args)
{
	/* get the parameters */
	parameters *p = (parameters *) args;
	int id = p->id;

	/* print id */
	printf("[%d]", id);
}

int main(int argc, char *argv[])
{
	int i = 0;
	int fail = 0;
	int rc = 0, ok = 0;
	pthread_t *threads = NULL;
	parameters params;
	 
	/* grab parameters */
	if (argc == 2)
		THREAD_NOS =  atoi(argv[1]);
	else 
		THREAD_NOS = 1000;

	threads = (pthread_t *) malloc(THREAD_NOS * sizeof(pthread_t *));

	for (i = 0; i < THREAD_NOS; i++) {
		params.id = i;
		rc = pthread_create(&threads[i], NULL, (void *)&work, &params);
		rc ? fail++ : ok ++;
	}

	for (i = 0; i < ok; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}

	free(threads);
	pthread_exit(NULL);
	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail, THREAD_NOS * 2);
	return fail;
}
