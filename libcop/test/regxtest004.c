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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "libcop.h"
#include "libcopmalloc.h"
#include "config.h"

#define CHECK_INPUT()  \
	char *filename_pcs = NULL; \
	if( argc != 2 ) { \
		filename_pcs = "test.pcs"; \
	} else { \
		filename_pcs = argv[1]; \
	} 

int main(int argc, char *argv[])
{
	int fail = 0;
	CHECK_INPUT();

#ifdef REGX_SUPPORT
	char input_text[] = "anchored_pattern unanchored_pat cASe_INSENs_pat prefixghisuffix \
        char_Class15 negated_bbc quatiiifie card wild fgdf card double.hash1 \
        double_hash2 double0hash3 double hash4 doubleAhash5 double hash6! \
        anchored---wild_card !double?hash1 ";

    mapped_memory_pool_t rx_pool = cop_rx_pool();
    
	/* setup src and target buffers */
	size_t src_size;
	src_size = sizeof(input_text);
	char *src = (char *)cop_malloc(rx_pool, src_size, 1);
	memcpy(src, input_text, src_size);

    cop_regx_init();

	/* create regx session */
	cop_session_t *s = cop_regx_create_session();
	cop_regx_session_t *regx_s = (cop_regx_session_t *)s->data;

	/* load pcs/map files */
	int rc = cop_regx_pcs_load(s, filename_pcs);
	printf("cop_regx_pcs_load returned %d\n", rc);
	if (rc == -1) {
		fprintf(stderr, "Could not load the pcs file\n");
		goto END;
	}
	printf("--- pcs file loaded.\n");

	/* setup session to search */
	printf("--- cop_regx_search\n");
	cop_regx_search(s, 901, src, src_size);

	/* commit session syncronously and check if we are complete */
	printf("--- commit session\n");
	//cop_commit_session(s);
	//cop_session_iscomplete(s);

	/* print out results */
	int *offset_array, *pattern_array, *scommand_array;
	int rn =
	    cop_regx_get_results(s, &offset_array, &pattern_array, &scommand_array);
	printf("--- results_total = %d search_count = %d\n",
		regx_s->results_count, regx_s->search_count);
	if (rn == 0) {
		goto END;
	}

	int i = 0;
	for (i = 0; i < rn; i++) {
		printf("[%d] => off: %d pat: %d scmd: %d = ", i, offset_array[i], pattern_array[i], scommand_array[i]);
		int j = offset_array[i];

		//Just to give us an image
		char c = ' ';
		int start_point = 0;
		while (input_text[j] != c) {
			j--;
			if (j < 0)
				break;
		}
		start_point = j;
		for (; j <= offset_array[i]; j++) {
			printf("%c", input_text[j]);
		}

		printf("\n");
	}

	free(offset_array);
	free(pattern_array);
	free(scommand_array);

 END:
	cop_free(rx_pool, src);

	cop_regx_free_session(s);

	printf("--- shutdown UM\n");
	cop_regx_close();
#else
	printf("libcop was not compiled with RegEx support\n");
#endif

	printf("=== %s: %d/%d failures ===\n", argv[0], fail, 1);
	return fail;
}
