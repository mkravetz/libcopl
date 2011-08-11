/* 
 *  
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 *  Copyright 2008,2009
 *  Chris J Arges
 *  Everton Constantino
 *  Tom Gall
 *  Michael Ellerman
 *  IBM Corp, All Rights Reserved
 *                      
 *  US Government Users Restricted Rights - Use Duplication or
 *  disclosure restricted by GSA ADP Schedule Contract with IBM
 *  Corp 
 *                                
 **/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "config.h"
#include "libcopmanage.h"
#include <libcop.h>
#include "cop_map_tree.h"
#include <libcopl/cop_util.h>

extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];
extern unsigned int COP_NUMBER_COPS;

int cop_exists (int cop) {
	if (cop < COP_MINIMUM || cop > COP_NUMBER_KNOWN_COPS)
		return 0;
	return cop_get_status(cop);
}

int cop_get_status(int cop_number) {
	if (COP_CTX_FD(&cop_ct_info_array[cop_number].ctx) != -1)
		return 1;
	return 0;
}

int cop_ct_to_cop_number(int ct) {
	int i;
	for(i=COP_MINIMUM; i<COP_NUMBER_KNOWN_COPS; i++) {
		if (COP_CTX_TYPE(&cop_ct_info_array[i].ctx) == ct)
			return i;
	}
	return -1;
}

int gDD_version;

int cop_get_DD_version() {
	return gDD_version;
}

void get_api_version(struct cop_ctx *ctx) {
	int retv;

	errno = 0;
	retv = cop_get_api_version(ctx);
	if (errno) {
		fprintf(stderr, "cop_get_api_version failure %d (%s)\n",
			errno, strerror(errno));
		exit(errno);
	}

#if 0
/*
 * Removing so that there is no longer a dependency on asm/copro-driver.h.
 * If this functionality is desired (verbose pprint of version #), I would
 * suggest adding to libcopl or a new libcopl interface.  One goal is to
 * have libcoopl be the only code that needs to know about kernel asm
 * header files
 */
	verbose_print("api version = %d (major %d, minor %d)\n", retv, 
		COPRO_API_MAJOR(retv), COPRO_API_MINOR(retv));
#endif
}

void call_cop_get_compatible(int cop) {
	struct cop_ctx *ctx = &(cop_ct_info_array[cop].ctx);

	gDD_version = cop_get_chip_version(ctx);
}
