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
#include <libcopl/icswx_util.h>
#include <libcop.h>

#define WAITRSV .long 0x7c20007c

/*
 * CR0 defines
 */
#define CR0_MASK        0xf0000000
#define CR0_LT          0x80000000
#define CR0_GT          0x40000000
#define CR0_EQ          0x20000000
#define CR0_SO          0x10000000


int cop_dispatch(struct cop_crb * crb)
{
	int rc = 0;
	volatile struct cop_csb *ptrCSB = cop_get_csbaddr(crb);
	ptrCSB->cw.valid = 0;

	/* Marker Trace */
	cop_crb_marker_trace_cnt(crb);

	/* issue icswx(with retry) and return code */
	rc = icswx_retry(crb, 50, (long)1000000);
	return (-rc);
}

int cop_dispatch_and_wait(struct cop_crb * crb)
{
	int rc = 0;
	struct cop_csb *p_csb = cop_get_csbaddr(crb);

	/* issue icswx */
	rc = cop_dispatch(crb);
	if (rc) {
		cop_crb_clear_marker_trace(crb);
		return (-rc);
	}

	/* wait for valid bit in CSB to be set */
	cop_csb_wait(p_csb);

	cop_crb_clear_marker_trace(crb);
	return p_csb->cw.comp_code;
}
