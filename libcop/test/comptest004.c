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

#define NUMINTERATIONS 13

int main(int argc, char *argv[])
{
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
	int fail = 0, remaining,z;
	char *src;
	char *tgt;
	char *tgt_uncompressed;
	char *ptrScout;

	for (i=0; i< NUMINTERATIONS ; i++) {

		size=seed_sizes[i];
		src = (char *)cop_malloc(cc_pool, size, 1);
		tgt = (char *)cop_malloc(cc_pool, size, 1);
		tgt_uncompressed =(char *)cop_malloc(cc_pool, size, 1);

		ptrScout=src;
		for (remaining = seed_sizes[i]; remaining > 0;) {

			if ((remaining / 100) > 0)
				z=100;
			else
				z = remaining % 100;

			remaining -= z;
			if (z) {
				memcpy(ptrScout, seed_test_data, z);
				ptrScout+=z;
			}
		}		

		errno=0;

		session=cop_create_zlib_compression_session(COMPLETION_STORE, 
			size, src, size, tgt);
		session=cop_commit_session(session);
		if (cop_session_iscomplete(session)) {
			compressed_size=cop_get_target_output_size(session);
		}
		if( errno ) { 
            printf("zlib Comp Error: %d size %d output %d\n",(int) errno,(int)size,(int)compressed_size); 
        	fail++;
            errno=0;
        } 

    // now decompress and see if we get the same thing
    	memset(tgt_uncompressed,0x00, size); 

	session_uncomp=cop_create_zlib_decompression_session(COMPLETION_STORE, 0,
			compressed_size, tgt, size, tgt_uncompressed);
		session_uncomp=cop_commit_session(session_uncomp);
		if (cop_session_iscomplete(session_uncomp)) {
			uncompressed_size=cop_get_target_output_size(session_uncomp);
		}
		if( errno ) { 
            printf("zlib decomp Error: %d size %d output %d\n",(int) errno,(int)size,(int)uncompressed_size); 
        	fail++;
            errno=0;
        } 

      if (size != uncompressed_size) {
            printf("orig size = %d\n", (int)size); 
            printf("comp size = %d\n", (int)compressed_size); 
            printf("uncomp size = %d\n", (int)uncompressed_size); 
            fail++; 
        } else if (0!=memcmp(src, tgt_uncompressed, uncompressed_size)) {
			fail++;
			printf("%d input -> compressed -> uncompressd output are not equal\n", (int)size);
/*			for(j=0; j<uncompressed_size;j++)
				printf("orig:%02x   unc:%02x\n",src[j],tgt_uncompressed[j]);
				if ( src[j] != tgt_uncompressed[j]) {
					printf("byte %d is different %c orig %c decomp\n",j, src[j], tgt_uncompressed[j]);
				} */

		}
	cop_free(cc_pool, src);
	cop_free(cc_pool, tgt);
	cop_free(cc_pool, tgt_uncompressed);
	cop_free_session(session);
	cop_free_session(session_uncomp);
	}


	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 26);
	return 0;
}
