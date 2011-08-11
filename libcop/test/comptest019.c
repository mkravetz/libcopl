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
#include <stdlib.h>
#include <errno.h>
#include <libcopmalloc.h>
#include <math.h>
#include "libcop.h"
#include "config.h"

#define CHECK_ERROR() { \
    if( errno && errno != 13 ) { \
        printf("Error: %d\n", errno);\
		fail++; \
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

void print_text( char *t, int size ) {
    int i;
    for( i = 0; i < size; i++ ){
        if( i % 128 == 0 ) verbose_print("\n");
        verbose_print("%c", t[i]);
    }
    verbose_print("\n");
}

void dump_crb( cop_session_t *s ){
	struct cop_crb *crb = (struct cop_crb *) s->ptrPVT;   
	int i;

	verbose_print("Dumping crb\n");
	verbose_print("crb->command_word: %02X\n", crb->ccw.value );
	verbose_print("crb->qos: %02X\n", crb->qos );
	verbose_print("crb->mt_tag: %02X\n", crb->mt_tag);
	verbose_print("crb->flags: %02X\n", crb->flags);
	verbose_print("crb->ch: %02X\n", crb->ch);
	verbose_print("crb->seq_num: %02X\n", crb->seq_num);
	verbose_print("crb->ptrCSBValue: %02X\n", crb->cop_addr_value);
	verbose_print("crb->source_DDE\n");
	verbose_print("crb->source_progress: %02X\n", crb->source_progress );
	verbose_print("crb->source_countt: %02X\n", crb->source_countt);
	verbose_print("crb->source_pi: %02X\n", crb->source_pi );
	verbose_print("crb->source_byte_count: %02X\n", crb->source_byte_count );
	verbose_print("crb->ptrSD: %02X\n", crb->ptrSD);
	verbose_print("crb->target_DDE\n");
	verbose_print("crb->target_progress: %02X\n", crb->target_progress);
	verbose_print("crb->target_countt: %02X\n", crb->target_countt);
	verbose_print("crb->target_pi: %02X\n", crb->target_pi);
	verbose_print("crb->target_byte_count: %02X\n", crb->target_byte_count);
	verbose_print("crb->ptrTD: %02X\n", crb->ptrTD);
	verbose_print("\n");
}

void dump_csb( cop_session_t *s ){
    struct cop_csb *CSB = (struct cop_csb *) cop_get_csbaddr(s->ptrPVT);   
	int i;

	verbose_print("Dumping csb\n");
	verbose_print("csb->valid_bit: %02X\n", CSB->valid_bit);
	verbose_print("csb->format: %02X\n", CSB->format );
	verbose_print("csb->ch: %02X\n", CSB->ch );
	verbose_print("csb->crb_seq_number: %02X\n", CSB->crb_seq_number );
	verbose_print("csb->completion_code: %02X\n", CSB->completion_code );
	verbose_print("csb->completion_extension: %02X\n", CSB->completion_extension );
	verbose_print("csb->bytes: %02X\n", CSB->bytes );
	verbose_print("csb->address: %02X\n", CSB->address );
	verbose_print("\n");
}

void print_binary( char *t, int size ) {
    int i;
    for( i = 0; i < size; i ++) {
        if( i % 64 == 0 ) verbose_print("\n");
        verbose_print("%02X ", t[i] );
    }
    verbose_print("\n");
}

int main(int argc, char *argv[])
{
	int i;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	FILE *test_file = fopen( "/bin/busybox", "r" );
	fseek( test_file, 0L, SEEK_END );

	size_t size = ftell(test_file), size_uncomp = 64;
	char *src = (char *)cop_malloc(cc_pool, size, 1);
	char *tgt = (char *)cop_malloc(cc_pool, size, 1);
	cop_session_t *session,*session_uncomp;
	size_t compressed_size, uncompressed_size;
	size_t decomp_size = 0;
	int fail = 0;

	fseek( test_file, 0L, SEEK_SET );
	fread( src, 1, size, test_file );
	fclose( test_file );

	memcpy(tgt, src, size);

	errno=0;

	session=cop_create_zlib_compression_session(COMPLETION_STORE, size, src, size, tgt);
    session=cop_commit_session(session);
	CHECK_ERROR();

	if (cop_session_iscomplete(session)) {
		compressed_size=cop_get_target_output_size(session);
	}

    // now decompress and see if we get the same thing

	int max_num_chunks = 12;
	int num_chunks = max_num_chunks > compressed_size ? compressed_size : max_num_chunks;
	size_t chunk_size = (size_t)floor(compressed_size/num_chunks);
	
	session_uncomp = cop_create_zlib_decompression_session_with_dde_list(COMPLETION_STORE,1, chunk_size, tgt, size_uncomp );
SEND_ME_FIRST:
	dump_crb(session_uncomp);
    session_uncomp = cop_commit_session(session_uncomp);
	CHECK_ERROR();
	dump_csb(session_uncomp);
    
    if( errno == 13 ){
        printf("Updating target buffer\n");
		cop_comp_session_t *cdc_session = (cop_comp_session_t *) session_uncomp->data;

        cop_free(cc_pool, cdc_session->last->target_buffer);

        size_uncomp *= 2;

        cdc_session->last->target_buffer = (char *) cop_malloc( cc_pool, size_uncomp, 64 );
		cdc_session->last->size = size_uncomp;

        memset( cdc_session->last->target_buffer, 0x0, size_uncomp );

        session_uncomp = cop_zlib_decompression_add_tgt_buffer(session_uncomp, cdc_session->last->size, cdc_session->last->target_buffer );		
        goto SEND_ME_FIRST;	
    }
    
    if (cop_session_iscomplete(session_uncomp)) {
		uncompressed_size = cop_get_target_output_size(session_uncomp);
	}

    printf("Decompression step-1 completed %u bytes sent - uncompressed size up till now %u bytes\n",  chunk_size, uncompressed_size);
 
	for( i = 1; i  < num_chunks; i++ ){
		int first_try = 1;
        int isLast = (i + 1) == num_chunks;
       	int bsize = isLast ? compressed_size - i*chunk_size : chunk_size;      	

        session_uncomp = cop_prep_next_decompression(session_uncomp, isLast, bsize, tgt + i*chunk_size, 0, NULL );

SEND_ME:
		dump_crb(session_uncomp);
		session_uncomp = cop_commit_session(session_uncomp);
		CHECK_ERROR();
		dump_csb(session_uncomp);
        
		if( errno == 13 ){
			size_uncomp = size_uncomp < 2048 ? 2048 : size_uncomp;

			if( first_try ){ //We're just adding a new target buffer
				session_uncomp = cop_zlib_decompression_add_dde( session_uncomp, size_uncomp );
			} else { //We haven't made any progress because our buffer is still not big enough
				cop_comp_session_t *cdc_session = (cop_comp_session_t *) session_uncomp->data;

				cop_free(cc_pool, cdc_session->last->target_buffer);

				size_uncomp = size_uncomp + 2048;

				cdc_session->last->target_buffer = (char *) cop_malloc( cc_pool, size_uncomp, 64 );
				cdc_session->last->size = size_uncomp;

				memset( cdc_session->last->target_buffer, 0x0, size_uncomp );

				session_uncomp = cop_zlib_decompression_add_tgt_buffer(session_uncomp, cdc_session->last->size, cdc_session->last->target_buffer );		
			}

			first_try = 0;
            
			printf("[%d] Adding a new target buffer of size %u\n", i, size_uncomp);

            uncompressed_size = 0;
			goto SEND_ME;	
		} else if( errno ) {
			fail++;

			goto CLEAN;
		}
		
		if(cop_session_iscomplete(session_uncomp)) {
			uncompressed_size = cop_get_target_output_size( session_uncomp );
		}
		    
		printf("Decompression step-%d completed %d bytes sent - uncompressed size up till now %d bytes\n", (i+1), bsize, uncompressed_size);
    }
   
	char *tgt_uncompressed = (char *) malloc( uncompressed_size );
	memset( tgt_uncompressed, 0x0, uncompressed_size );

	cop_comp_copy_dde_list( session_uncomp, tgt_uncompressed, uncompressed_size );

    printf("Decompression completed. Output size is %d src data size was %d\n", uncompressed_size, size);

	if ( memcmp(src, tgt_uncompressed, uncompressed_size) != 0 ) {
		fail++;
		printf("input -> compressed -> uncompressd output are not equal\n");
        print_text( src, uncompressed_size );

        print_text( tgt_uncompressed, uncompressed_size );
	} else {
		verbose_print("Output data: \n");
		print_text( tgt_uncompressed, uncompressed_size );
	}

CLEAN:
	cop_free_comp_session(session);
	cop_free_comp_session(session_uncomp);

	cop_free(cc_pool, src);
	cop_free(cc_pool, tgt);
	
	free(tgt_uncompressed);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 3);
	return 0;
}
