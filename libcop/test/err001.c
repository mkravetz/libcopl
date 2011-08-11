/* first in a series of test cases to test error cases */

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
#include <sys/errno.h>

#include "config.h"
#include <libcopmalloc.h>
#include <libcop.h>
#include "utils.h"

extern union cop_u cop_ct_union;

int testsize[] = { 1, 2, 3, 4, 7, 8, 9, 16, 17, 32, 33, 63, 64, 65, 0,
	127, 128, 129, 4096, 4097, 4095, 16192, 16193, 0
};				/* need to add -1 testcase */
char testdata[102] =
    "DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  DEADBEEF  \0";

char outputbuffer[4096];

void inject_error1(struct cop_crb * crb);
void inject_error2(struct cop_crb * crb);
void inject_error3(struct cop_crb * crb);
void inject_error4(struct cop_crb * crb);
void inject_error5(struct cop_crb * crb);
void inject_error6(struct cop_crb * crb);
void inject_error7(struct cop_crb * crb);
void inject_error8(struct cop_crb * crb);

int main(int argc, char *argv[])
{
/* This test case needs to be running on DD2 or better hardware */
if (gDD_version > 1) {
	int error;
	int i, z, remaining, errors;
	char *ptrCDest, *ptrCSrc, *ptrScout;
	void *result;
	struct cop_crb *adm_crb;
	struct cop_csb *ptrCSB;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	for (i = 0; testsize[i]; i++) {
		ptrCSrc = cop_malloc_unaligned(cc_pool, testsize[i]);
		ptrCDest = cop_malloc_unaligned(cc_pool, testsize[i]);

		memset(ptrCSrc, 0x00, testsize[i]);
		memset(ptrCDest, 0x00, testsize[i]);
		test_print("Src add %p   Dest Add %p\n", ptrCSrc, ptrCDest);

		ptrScout = ptrCSrc;
		for (remaining = testsize[i]; remaining > 0;) {
			z = remaining % 100;
			remaining -= z;
			if (z) {
				memcpy(ptrScout, testdata, z);
				ptrScout += z;
			}
		}

		test_memprint(ptrCSrc, testsize[i]);
		test_print("calling fakecop_memcpy\n");

		adm_crb = cop_next_crb(cc_pool, COP_ASYNC_DATA_MOVER);

		test_print
		    ("err001 after cop_next_control_block : testcase %d for ct=%d \n",
		     i, adm_crb->ccw.type);

		// according to the 12/12/2008 crypto spec
		// ADM_INCINC mode
		adm_crb->ccw.dir = 0;

		adm_crb->src_dde.bytes =
			adm_crb->tgt_dde.bytes = testsize[i];
		adm_crb->src_dde.data = (uint64_t) ptrCSrc;
		adm_crb->tgt_dde.data = (uint64_t) ptrCDest;

		switch (i) {
			/*           case 0: inject_error1(adm_crb); 
			   break; */
		case 1:
			inject_error2(adm_crb);
			break;
		case 2:
			inject_error3(adm_crb);
			break;
		case 3:
			inject_error4(adm_crb);
			break;
		case 4:
			inject_error5(adm_crb);
			break;
		case 5:
			inject_error6(adm_crb);
			break;
//          case 6: inject_error7(adm_crb); 
//              break;
		case 7:
			inject_error8(adm_crb);
			break;
		default:
			break;
		}

	      restart:
		error = cop_dispatch_and_wait(adm_crb);

		test_print("err001 after dispatch and wait : testcase %d\n", i);

		ptrCSB = cop_get_csbaddr(adm_crb);

		if (error || ptrCSB->cw.comp_code) {
			fprintf(stderr,
				"err001 error on returned %d and cc %d\n",
				error, ptrCSB->cw.comp_code);
			if (error == EAGAIN)
				goto restart;	// resubmit since pipe is full
			else if (error == ENODEV) {
				cop_give_back_crb(cc_pool, adm_crb,
						  COP_ASYNC_DATA_MOVER);
				goto errorcontinue;
			}

			switch (ptrCSB->cw.comp_code) {
			default:
				fprintf(stderr,
					"err001 CC of %x returned with address %llx and processed byte count %d\n",
					ptrCSB->cw.comp_code,
					ptrCSB->addr,
					ptrCSB->bytes);
			}
		} else {
			test_print("no error reported err001 : testcase %d\n",
				   i);
		}

	      errorcontinue:
		cop_give_back_crb(cc_pool, adm_crb, COP_ASYNC_DATA_MOVER);
	}
} else {
	printf("this test should NOT be run on DD1 hardware due to Errata\n");
}
}

void inject_error1(struct cop_crb * crb)
{
	// set type to 0
	crb->ccw.type = 0;
}

void inject_error2(struct cop_crb * crb)
{
	crb->src_dde.data = 0x00;
}

void inject_error3(struct cop_crb * crb)
{
	crb->tgt_dde.data = 0x00;
}

void inject_error4(struct cop_crb * crb)
{
	crb->ccw.dir = 10;

}

void inject_error5(struct cop_crb * crb)
{
	crb->src_dde.data = (uint64_t) testdata;
}

void inject_error6(struct cop_crb * crb)
{
	crb->tgt_dde.data = (uint64_t) outputbuffer;

}

void inject_error7(struct cop_crb * crb)
{
	uint64_t ptr = cop_get_csbaddr(crb);
	ptr = 0x00;
	cop_set_csbaddr(crb, ptr);
}

void inject_error8(struct cop_crb * crb)
{
	crb->csbp.type = 1;
}
