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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libcopmalloc.h>
#include <math.h>
#include <pthread.h>
#include "libcop.h"
#include "config.h"

#define NUM_TESTS 50
#define MAX_TEXT_SIZE 1023

#define CHECK_ERROR() { \
    if( errno && errno != 13  ) { \
        printf("[%s,%d] Error: %d\n", __func__, __LINE__, errno);\
		pthread_mutex_lock(&mutex); \
		fail++; \
		pthread_mutex_unlock(&mutex); \	
		if( session != NULL ) cop_free_session(session); \
		if( session_uncomp != NULL ) cop_free_comp_session(session_uncomp); \
		cop_free(cc_pool, src); \
		cop_free(cc_pool, tgt); \
		return; \
    } else { \
    } \
}

#define CHECK_OUTPUT(tgt,sz,input_sz) { \
    printf("input size = %d\n", (input_sz)); \
    printf("output size = %d\n", (sz)); \
	for (i = 0; i < (sz); i++) { \
		if (i % 8 == 0) \
			printf("\n"); \
		printf("%02x ", (tgt)[i]); \
	} \
	printf("\n"); \
}

typedef struct {
    int id; 
} parameters;

int fail = 0;
pthread_mutex_t mutex;

void comp(void *args){
	parameters *p = (parameters *) args;
	int i;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	size_t test_size = (rand() % (MAX_TEXT_SIZE-32)) + 32;
	char *test_data = (char *)malloc(test_size);
	
	/* Random text generator */
	int range = 'Z' - 'A' + 1;
	for( i = 0; i < test_size; i++ ){
		char c = (rand() % 1000 == 1 ? '\n' : (rand() % 6 == 1 ? ' ' : (rand() % range) + (rand() % 2 == 0 ? 'A' : 'a')));
		test_data[i] = c;
	}
	test_data[test_size-1] = '\0';
	
	//printf("[%d] is sending (%s)\n", p->id, test_data );
	
	size_t size_uncomp = 512;
	
	char *src = (char *)cop_malloc(cc_pool, test_size, 1);
	char *tgt = (char *)cop_malloc(cc_pool, test_size + 512, 1);

	cop_session_t *session = NULL,*session_uncomp = NULL;

	size_t compressed_size, uncompressed_size;

	memcpy(src, test_data, test_size);
	
	memset(tgt, 0x00, test_size + 512);

	session=cop_create_gzip_session(COMPLETION_STORE, test_size, src, test_size + 512, tgt);

	/* DD1 workaround */
	if (gDD_version == 1) {
	    session=cop_psr2_gzip_compression(session,tgt, &compressed_size);
	} else {
	    session=cop_commit_session(session);
	}  
	CHECK_ERROR();

	while(!cop_session_iscomplete(session));

	if (cop_session_iscomplete(session)) {
		compressed_size=cop_get_target_output_size(session);
	}

	if( compressed_size == 0 ) {
		printf("compressed_size was zero!\n");
		goto CLEAN;
	}
    // now decompress and see if we get the same thing

	int max_num_chunks = rand() % MAX_TEXT_SIZE;
	
	max_num_chunks = max_num_chunks < 3 ? 3 : max_num_chunks;

	int num_chunks = max_num_chunks > compressed_size ? compressed_size : max_num_chunks;
	int chunk_size = (int)ceil(compressed_size/num_chunks);

	session_uncomp=cop_create_gunzip_session_with_dde_list(COMPLETION_STORE,1, chunk_size, tgt, size_uncomp);
SEND_ME_FIRST:
	session_uncomp=cop_commit_session(session_uncomp);
	CHECK_ERROR();

	if( errno == 13 ){
        printf("Updating target buffer\n");
		cop_comp_session_t *cdc_session = (cop_comp_session_t *) session_uncomp->data;

        cop_free(cc_pool, cdc_session->last->target_buffer);

        size_uncomp *= 2;

        cdc_session->last->target_buffer = (char *) cop_malloc( cc_pool, size_uncomp, 64 );
		cdc_session->last->size = size_uncomp;

        memset( cdc_session->last->target_buffer, 0x0, size_uncomp );

        session_uncomp = cop_gunzip_add_tgt_buffer(session_uncomp, cdc_session->last->size, cdc_session->last->target_buffer );		
        goto SEND_ME_FIRST;	
	}
 	
	while(!cop_session_iscomplete(session_uncomp));

	if (cop_session_iscomplete(session_uncomp)) {
		uncompressed_size=cop_get_target_output_size(session_uncomp);
	}
    
    //printf("[%d] Decompression step-1 completed %d bytes sent - uncompressed size up till now %d bytes\n", p->id,  chunk_size, uncompressed_size);
    
    for( i = 1; i  < num_chunks; i++ ){
        int isLast = (i + 1) == num_chunks;
       	int bsize = isLast ? compressed_size - i*chunk_size : chunk_size;      	

        session_uncomp = cop_prep_next_decompression(session_uncomp, isLast, bsize, tgt + i*chunk_size, 0, NULL );
SEND_ME:	
		session_uncomp=cop_commit_session(session_uncomp);
		CHECK_ERROR();

		if( errno == 13 ){
            printf("[%d] Adding a new target buffer\n", i);
            
			size_uncomp = 2048;

			session_uncomp = cop_gunzip_add_dde( session_uncomp, size_uncomp );//add_dde( size_uncomp );
			
            uncompressed_size = 0;
			goto SEND_ME;	
		}else if( errno ) {
			pthread_mutex_lock(&mutex);
			fail++;
			pthread_mutex_unlock(&mutex);

			goto CLEAN;
		}	
			
		if (cop_session_iscomplete(session_uncomp)) {
			uncompressed_size=cop_get_target_output_size(session_uncomp);
		}
		    
		//printf("[%d] Decompression step-%d completed %d bytes sent - uncompressed size up till now %d bytes\n", p->id, (i+1), bsize, uncompressed_size);
    }

	char *tgt_uncompressed = (char *) malloc( uncompressed_size );
	cop_comp_copy_dde_list( session_uncomp, tgt_uncompressed, uncompressed_size );

	if( memcmp(src, tgt_uncompressed, uncompressed_size) != 0 ) {
		pthread_mutex_lock(&mutex);
		fail++;
		pthread_mutex_unlock(&mutex);
		printf("input -> compressed -> uncompressd output are not equal\n");
		for(i=0; i<uncompressed_size;i++){
			printf("orig:%02x(%c)   unc:%02x(%c)\n",src[i],src[i],tgt_uncompressed[i],tgt_uncompressed[i]);
			if ( src[i] != tgt_uncompressed[i]) {
				printf("byte %d is different %c orig %c decomp\n",i, src[i], tgt_uncompressed[i]);
			}
		}
	} else {
		printf("[%d] Finished succesfully\n", p->id );
	}

	free(tgt_uncompressed);

CLEAN:	
	free(test_data);
	if( session ) cop_free_comp_session(session);
	if( session_uncomp ) cop_free_comp_session(session_uncomp);
	cop_free(cc_pool, src);
	cop_free(cc_pool, tgt);
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
	printf("=== %s: %d/%d failures ===\n", argv[0], fail, NUM_TESTS);
	return 0;
}
