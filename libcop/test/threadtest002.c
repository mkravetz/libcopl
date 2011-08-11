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

/* a _VERY_ simple pthread consumer/producer test */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUM_TESTS	10
#define FIFO_SIZE	50

/* fifo */
int fifo[FIFO_SIZE];
size_t at;
pthread_mutex_t mutex;

/* parameters */
typedef struct {
	int id;
} parameters;

void producer(void *args)
{
	int i;
	for (i = 0; i < NUM_TESTS; i++) {
		i++;
		pthread_mutex_lock(&mutex);
		if (at < FIFO_SIZE) {
			fifo[at % FIFO_SIZE] = i;
			at++;
			printf("\n-> produced %03x ", i);
		} else {
			printf("\nfull.");
			usleep(100);
		}
		pthread_mutex_unlock(&mutex);
	}
}

void consumer(void *args)
{
	int i;
	for (i = 0; i < NUM_TESTS; i++) {
		i++;
		pthread_mutex_lock(&mutex);
		if (at > 0) {
			at--;
			int fifo_id = fifo[at % FIFO_SIZE];
			printf("\n<- consumed %03x", fifo_id);
		} else {
			printf("\nempty.");
			usleep(100);
		}
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char *argv[])
{
	int i = 0;
	int fail = 0;
	pthread_t *threads;
	parameters params;
	at = 0;

	pthread_mutex_init(&mutex, NULL);
	threads = (pthread_t *) malloc(sizeof(pthread_t *) * NUM_TESTS * 2);

	for (i = 0; i < NUM_TESTS * 2; i += 2) {
		if (pthread_create(&threads[i], NULL, (void *)&producer, NULL))
			fail++;
		if (pthread_create
		    (&threads[i + 1], NULL, (void *)&consumer, NULL))
			fail++;
	}

	for (i = 0; i < NUM_TESTS * 2; i++) {
		if (pthread_join(threads[i], NULL))
			fail++;
	}
	pthread_mutex_destroy(&mutex);

	printf("\n=== %s: %d/%d failures ===\n", argv[0], fail,
	       NUM_TESTS * 2 * 2);
	return fail;
}
