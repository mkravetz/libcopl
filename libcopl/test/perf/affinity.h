#ifndef _test_affinity_h_
#define _test_affinity_h_

#include <stdio.h>
#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_THREADS 128

/* FIXME: Access macros for `cpu_set'. from sched.h */
# define CPU_SET(cpu, cpusetp)	 __CPU_SET_S (cpu, sizeof (cpu_set_t), cpusetp)
# define CPU_ISSET(cpu, cpusetp) __CPU_ISSET_S (cpu, sizeof (cpu_set_t), cpusetp)
# define CPU_ZERO(cpusetp)	 __CPU_ZERO_S (sizeof (cpu_set_t), cpusetp)

/* FIXME: Extern prototypes for sched.h and pthread.h */
extern int sched_setaffinity(__pid_t, size_t, __const cpu_set_t *);
extern int sched_getaffinity (__pid_t, size_t, cpu_set_t *);
extern int pthread_setaffinity_np (pthread_t, size_t, __const cpu_set_t *);

int setaffinity(int num)
{
	cpu_set_t mask;
	int rc;
	if (num > MAX_THREADS || num < 0) {
		return -1;
	}

	CPU_ZERO(&mask);
	CPU_SET(num, &mask);
	rc = sched_setaffinity(0, sizeof (mask), &mask);
	if (rc != 0) {
		return -1;
	}
	return 0;
}

int setaffinity_pthreads(int num)
{
	cpu_set_t mask;

	if (num > MAX_THREADS || num < 0) {
		return -1;
	}

	CPU_ZERO(&mask);
	CPU_SET(num, &mask);
	if (pthread_setaffinity_np(pthread_self(), sizeof (mask), &mask) < 0) {
		fprintf(stderr, "Error: pthread_setaffinity_np in thread %i\n",
			num);
		return -1;
	}

	return 0;
}

int getaffinity(void)
{
	cpu_set_t mask;
	int rc, i;
	CPU_ZERO(&mask);
	rc = sched_getaffinity(0, sizeof (mask), (cpu_set_t *) & mask);
	if (rc != 0) {
		return -2;
	}
	for (i = 0; i < MAX_THREADS; i++) {
		if (CPU_ISSET(i, &mask)) {
			return i;
		}
	}

	return -1;
}

#endif
