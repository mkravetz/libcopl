#ifndef _test_perf_threads_posix_h
#define _test_perf_threads_posix_h

//#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <sched.h>

#include "./affinity.h"

static unsigned threads_num;
static pthread_t pthreads[MAX_THREADS];

struct thread_info {
	pthread_t pthread;
	unsigned tid;
};

static struct thread_info t_info[MAX_THREADS];

__thread unsigned int myTID = 0;


unsigned thread_count(void)
{
        /* should change to look at /sys/proc at some point */
	return MAX_THREADS;
}

unsigned thread_self(void)
{

	return myTID;

}

struct thread_info *thread_get(unsigned t)
{
	if (t > threads_num)
		return NULL;
	return &t_info[t];
}

int thread_join(unsigned tid)
{

 	int rc = pthread_join(pthreads[tid], NULL);
	if (rc)
		printf("rc = %d\n", rc);

	return tid;
}

void thread_exit(void)
{
	pthread_exit (NULL);
}

typedef struct
{
  	void (*func) (void *);
  	void *args;
	int tid;
} func_args;

void *
wrapper (void *p)
{
  	func_args *fa = (func_args *) p;
	int start_cpu = fa->tid;
	setaffinity_pthreads(start_cpu);
	//printf("RUNNING WRAPPER on cpu %d\n", getaffinity());
	//FIXME we are crashing here!!!!
  	fa->func (fa->args);
  	return NULL;
}

int thread_start(unsigned tid, void (*ustart) (void *), void *args)
{

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	myTID = tid;

	func_args *fa = (func_args *) malloc (sizeof (func_args));
	fa->func = ustart;
	fa->args = args;
	fa->tid = tid;

	pthread_create (&(pthreads[tid]), &attr, wrapper, (void *) fa);

        // pthread_create(&(pthreads[tid]), &attr, ustart, args);
        setaffinity_pthreads(tid);
        t_info[tid].pthread = pthreads[tid];
	t_info[tid].tid = tid;
        

        return tid;

}

#endif
