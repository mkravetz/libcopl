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

#include <libcop.h>

/* send an icswx to a coprocessor that does exist but is blocked via ACOP */

int main(int argc, char *argv[])
{
	poolid_t mempool;
	void *ptrSrc, *ptrDest;
	coprocessor_request_block *ptrCrb;
	coprocessor_status_block *ptrCsb;
	int i, result, j, z, remaining;
	unsigned long long rndnumber = 0;
	Rngu rngfrc;

	init_cop_mempool();
	mempool = create_cop_allocation_pool(30);	/* 30 pages of 4k currently */
	ptrCrb =
	    malloc_from_pool(mempool, sizeof(coprocessor_request_block), 128);

	memset(ptrCrb, 0x0, sizeof(coprocessor_request_block));

	ptrCrb->ccwu.value = 0;
	ptrCrb->ccwu.ccw.coprocessor_type = RANDOM_NUMBER_COPRO;
	rngfrc.value = 0;
	rngfrc.frc.rndFn = 1;
	rngfrc.frc.bytecount = 1;
	ptrCrb->ccwu.ccw.function_request_code = rngfrc.value;

	ptrCrb->source_bytes = 0;
	ptrCrb->ptrSD = (unsigned long long)malloc_from_pool(mempool, 64, 64);
	ptrCrb->ptrTD = (unsigned long long)malloc_from_pool(mempool, 8, 64);
	ptrCrb->ptrCSB =
	    (unsigned long long)malloc_from_pool(mempool,
						 sizeof
						 (coprocessor_status_block),
						 64);

	ptrCsb = (coprocessor_status_block *) ptrCrb->ptrCSB;
	memset(ptrCsb, 0x0, sizeof(coprocessor_status_block));

	printf("about to dispatch\n");

	/* code to turn off ACOP availability here */

	printf
	    ("testcase needs to turn off ACOP somehow ... hasn't been figured out yet\n");

	ptrCsb->residual_byte_count = 0;

	result = dispatch_to_coprocessor(ptrCrb, ptrCrb->ccwu.value);

	if (result != 0) {
		printf("test returned RC = %d  should return ?? \n", result);
	}

	free_to_pool((void *)ptrCrb->ptrSD);
	free_to_pool((void *)ptrCrb->ptrTD);
	free_to_pool((void *)ptrCrb->ptrCSB);

	release_allocation_pool(mempool);

}
