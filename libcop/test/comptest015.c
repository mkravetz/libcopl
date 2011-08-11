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
#include <math.h>
#include "libcop.h"
#include "config.h"

#define CHECK_ERROR() { \
    if( errno ) { \
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

int main(int argc, char *argv[])
{
	int i;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	char test_data[] =
	    "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFD";
	size_t size = sizeof(test_data), size_uncomp = ((sizeof(test_data) / 64)+1)*64 ;
	char *src = (char *)cop_malloc(cc_pool, size, 1);
	char *tgt = (char *)cop_malloc(cc_pool, size, 1);
	char *tgt_multi = (char *)cop_malloc(cc_pool, 6400, 1);
	char *tgt_uncompressed =(char *)cop_malloc(cc_pool, size_uncomp, 1);
	cop_session_t *session,*session_uncomp;
	size_t compressed_size, uncompressed_size;
	size_t decomp_size = 0;
	int fail = 0;

	memcpy(src, test_data, size);
	memcpy(tgt, test_data, size);

	errno=0;

	session=cop_create_gzip_session(COMPLETION_STORE, size, src, size, tgt);

/* DD1 workaround */
if (gDD_version == 1) {
        session=cop_psr2_gzip_compression(session,tgt, &compressed_size);
} else {
        session=cop_commit_session(session);
}  
	CHECK_ERROR();

	if (cop_session_iscomplete(session)) {
		compressed_size=cop_get_target_output_size(session);
		//CHECK_OUTPUT(tgt, (int)compressed_size, size);
	}

    // now decompress and see if we get the same thing
    memset(tgt_uncompressed,0x00, size); 

	session_uncomp=cop_create_gunzip_session(COMPLETION_STORE,1, (int)floor(compressed_size/3), tgt, size_uncomp, tgt_uncompressed);
	session_uncomp=cop_commit_session(session_uncomp);
	CHECK_ERROR();
	if (cop_session_iscomplete(session_uncomp)) {
		uncompressed_size=cop_get_target_output_size(session_uncomp);
		//CHECK_OUTPUT(tgt_uncompressed, (int)uncompressed_size, compressed_size);
	}
    
    printf("Decompression step-1 completed (%d)\n", uncompressed_size);
    
    session_uncomp=cop_prep_next_decompression(session_uncomp, 0, (int)floor(compressed_size/3), tgt + (int)floor(compressed_size/3), 0, NULL );
	session_uncomp=cop_commit_session(session_uncomp);
	CHECK_ERROR();
	if (cop_session_iscomplete(session_uncomp)) {
		uncompressed_size=cop_get_target_output_size(session_uncomp);
		//CHECK_OUTPUT(tgt_uncompressed, (int)uncompressed_size, compressed_size);
	}
        
    printf("Decompression step-2 completed (%d)\n", uncompressed_size);
    	
    session_uncomp=cop_prep_next_decompression(session_uncomp, 1, compressed_size - (int)(2*floor(compressed_size/3)), tgt + (int)(2*floor(compressed_size/3)), 0, NULL );
	session_uncomp=cop_commit_session(session_uncomp);
	CHECK_ERROR();
	if (cop_session_iscomplete(session_uncomp)) {
		uncompressed_size=cop_get_target_output_size(session_uncomp);
		//CHECK_OUTPUT(tgt_uncompressed, (int)uncompressed_size, compressed_size);
	}

    printf("Decompression step-3 completed (%d)\n", uncompressed_size);

	if (0!=memcmp(src, tgt_uncompressed, uncompressed_size)) {
		fail++;
		printf("input -> compressed -> uncompressd output are not equal\n");
		for(i=0; i<uncompressed_size;i++)
			printf("orig:%02x(%c)   unc:%02x(%c)\n",src[i],src[i],tgt_uncompressed[i],tgt_uncompressed[i]);
			if ( src[i] != tgt_uncompressed[i]) {
				printf("byte %d is different %c orig %c decomp\n",i, src[i], tgt_uncompressed[i]);
			}

	}

	cop_free_comp_session(session);
	cop_free_comp_session(session_uncomp);
	cop_free(cc_pool, src);
	cop_free(cc_pool, tgt);
	cop_free(cc_pool, tgt_uncompressed);

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 3);
	return 0;
}
