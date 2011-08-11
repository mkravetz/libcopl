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
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcopmanage.h"
#include "libcop.h"
#include "libcopl/rnd.h"
#include "libcopl/cop_util.h"

struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];
extern cop_session_t *cop_create_session();

/**
 * @brief	function to create an asynchronous RND session
 * @param mode	0 for WAIT, 1 for INTERRUPT completion method
 * @return	a session structure
 */
cop_session_t *cop_create_random_session(int mode)
{

	/* allocate space for session information */
	cop_session_t *session = cop_create_session();

	if (!session)
		return NULL;

	/* save the CRB */
	void *ptrTD = 0;
	struct cop_crb *crb;
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	
	crb = cop_next_crb(cc_pool, COP_RNG);

	ptrTD = cop_malloc_unaligned(cc_pool, sizeof(uint64_t));

	if (cop_random_setup(crb, &cop_ct_info_array[COP_RNG].ctx, ptrTD,
							sizeof(uint64_t))) {
		cop_free(cc_pool, (void *)ptrTD);
		cop_give_back_crb(cc_pool, crb, COP_RNG);
		free(session);
		return NULL;
	}

	if (mode) {
		cop_crb_set_comp_mthd(crb, COP_COMP_MTHD_INTERRUPT);
	}

	session->ptrPVT = crb;
	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);

	return session;
}

/**
 * @brief function to get the random value from a given session
 * @param session structure
 * @return	a long random value
 */
long cop_get_rnd(cop_session_t * session)
{
	//FIXME: why is there a hardcoded value here? -cja
	while (!cop_session_iscomplete(session)) {
		usleep(1000000);
	}

	void *data = cop_get_tdptr(session->ptrPVT);
	return *((long *)data);
}

/*
 * @brief synchronous function to generate a random value
 * @return	a long random value
 */
long cop_random()
{
	int error;
	uint64_t result;
	void *ptrTD = 0;
	struct cop_crb *crb;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_RNG);

	ptrTD = cop_malloc_unaligned(cc_pool, sizeof(result));

	if ((error = cop_random_setup(crb, &cop_ct_info_array[COP_RNG].ctx,
						ptrTD, sizeof(result)))) {
		cop_free(cc_pool, (void *)ptrTD);
		cop_give_back_crb(cc_pool, crb, COP_RNG);
		errno = error;
		return 0;
	}

	error = cop_dispatch_and_wait(crb);

	void *data = cop_get_tdptr(crb);
	result = *((long *)data);

	cop_free(cc_pool, ptrTD);
	cop_give_back_crb(cc_pool, crb, COP_RNG);

	if (error) {
		errno = -error;
		return 0;
	}
	return result;
}

//FIXME: do we need this function? deprecate? -cja
int cop_rand()
{
	return (int)cop_random();
}

