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
#include <errno.h>
#include <libcopmalloc.h>
#include "libcop.h"
#include <pthread.h>
#include "config.h"

#define NUM_TESTS 50


typedef struct {
    int id; 
} parameters;

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

// 13
#define NUMINTERATIONS 13
//#define NUMINTERATIONS 4
#define MAX_SEED_SIZE 1024*1024

void comp(void *args)
{
	parameters *p = (parameters *) args;
    int id = p->id;
	int i,j;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	char seed_test_data[] =
	    "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";

	int seed_sizes [] = { 63, 64, 65, 127, 128, 129, 4095, 4096, 4097, (64*1024)-1, (64*1024)+1,
		256*1024, 1024*1024, 0};

	size_t size;
	cop_session_t *session,*session_uncomp;
	size_t compressed_size, uncompressed_size;
	size_t decomp_size = 0;
	int remaining,z;
	char *src;
	char *tgt;
	char *tgt_uncompressed;
	char *ptrScout;

	src = (char *)cop_malloc(cc_pool, MAX_SEED_SIZE, 8);
	tgt = (char *)cop_malloc(cc_pool, MAX_SEED_SIZE*2, 8);
	tgt_uncompressed =(char *)cop_malloc(cc_pool, MAX_SEED_SIZE*2, 8);
	if (src==NULL || tgt==NULL || tgt_uncompressed==NULL) {
		fprintf(stderr, "Thread buffer allocation failure\n");
		return;
	}

	for (i=0; i< NUMINTERATIONS ; i++) {
		size=seed_sizes[i];

		/*
		 * Fill source buffer with data
		 */
		ptrScout=src;
		for (remaining = seed_sizes[i]; remaining > 0;) {

			if ((remaining / sizeof(seed_test_data -1)) > 0)
				z = sizeof(seed_test_data -1);
			else
				z = remaining % sizeof(seed_test_data -1);

			remaining -= z;
			if (z) {
				memcpy(ptrScout, seed_test_data, z);
				ptrScout+=z;
			}
		}		

		errno=0;
		session=cop_create_gzip_session(COMPLETION_STORE, 
			size, src, size, tgt);

		/* DD1 workaround */
		if (gDD_version == 1) {
			session=cop_psr2_gzip_compression(session, tgt,
							&compressed_size);
		} else {
			session=cop_commit_session(session);
		}  

		CHECK_ERROR();
		if (cop_session_iscomplete(session)) {
			compressed_size=cop_get_target_output_size(session);
		}

		// now decompress and see if we get the same thing
		memset(tgt_uncompressed, 0x00, size); 

		session_uncomp=cop_create_gunzip_session(COMPLETION_STORE,  0,
			compressed_size, tgt, size, tgt_uncompressed);
		session_uncomp=cop_commit_session(session_uncomp);
		CHECK_ERROR();
		if (cop_session_iscomplete(session_uncomp)) {
			uncompressed_size =
				cop_get_target_output_size(session_uncomp);
		}

		if (size != uncompressed_size) {
			printf("(%x) orig size = %d\n", id, (int)size); 
			printf("(%x) comp size = %d\n", id,
							(int)compressed_size); 
			printf("(%x) uncomp size = %d\n", id,
							(int)uncompressed_size);

			pthread_mutex_lock(&mutex); 
			fail++; 
			pthread_mutex_unlock(&mutex); 

		} else if (memcmp(src, tgt_uncompressed, size)) {
			int start=0,end=0;
			printf("(%x) FAIL orig size = %d\n", id, (int)size); 
			printf("(%x) FAIL comp size = %d\n", id,
							(int)compressed_size); 
			printf("(%x) FAIL uncomp size = %d\n", id,
							(int)uncompressed_size);

			pthread_mutex_lock(&mutex); 
			fail++; 
			pthread_mutex_unlock(&mutex); 
			printf("(%x) input -> compressed -> uncompressd output"
				" are not equal\n", id);
		}

		cop_free_session(session);
		cop_free_session(session_uncomp);

	}

	cop_free(cc_pool, src);
	cop_free(cc_pool, tgt);
	cop_free(cc_pool, tgt_uncompressed);

	printf("(%X) Finished successfully\n", id);
}

int main(int argc, char *argv[])
{
    int i;
    pthread_t *threads = NULL;
    parameters *params;

    //---SPAWNING THREADS---
    threads = (pthread_t *) malloc(NUM_TESTS * sizeof(pthread_t *));
    for (i = 0; i < NUM_TESTS; i++) {
		params=malloc(sizeof (parameters));
        params->id = i;
        if (pthread_create(&threads[i], NULL, (void *)&comp, params)){
            threads[i] = NULL;
            fail++;
        }
    }

    for (i = 0; i < NUM_TESTS; i++) {
        if( threads[i] ){
            if (pthread_join(threads[i], NULL)){
            fail++;
            }
        }
    }

    printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS * NUMINTERATIONS);
    return 0;
}
