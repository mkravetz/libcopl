/* 
 *  
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 *  Copyright 2008,2009, 2010
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
#include <errno.h>

#include <config.h>
#include "libcopmalloc.h"
#include <libcop.h>
#include <stdlib.h>
#include "libcopl/comp.h"

#ifdef VERBOSE_ON
#include <stdio.h>
#endif

extern int cop_ct_to_cop_number(int ct);
extern void cop_give_back_crb(mapped_memory_pool_t p, void *crb,unsigned int type);

cop_session_t *cop_create_session()
{
	cop_session_t *session = NULL;

	session = malloc(sizeof(cop_session_t));
	if (!session) {
		errno = ENOMEM;
		return NULL;
	}

	if (session != memset(session, 0, sizeof(cop_session_t)))
		return NULL;

	session->data = NULL;
	return session;
}

void cop_free_session(cop_session_t * session)
{
	if (session) {
		/* give back crb if the ptrPVT exists */
		mapped_memory_pool_t pool = cop_cc_pool();
		if (session->ptrPVT) {
			uint8_t cop_number =
			    cop_ct_to_cop_number(session->ptrPVT->ccw.type);
			if (cop_number == COP_REGX || cop_number == COP_XML)
				pool = cop_rx_pool();

			/* TODO: need code to look for DDEs to throw back? */
			cop_give_back_crb(pool, session->ptrPVT, cop_number);
		}

		/* free malloced data */
		if (session->data)
			free(session->data);//cop_free(pool, session->data);

		free(session);
	}
}

void cop_print_session_state(cop_session_t * session)
{
#ifdef VERBOSE_ON
	switch (session->state) {
	case COP_SESSION_INVALID:
		printf("invalid");
		break;
	case COP_SESSION_INIT:
		printf("init");
		break;
	case COP_SESSION_INIT_COMPLETE:
		printf("init complete");
		break;
	case COP_SESSION_INFLIGHT:
		printf("inflight");
		break;
	case COP_SESSION_INFLIGHT_NO_RETURN:
		printf("inflight no return");
		break;
	case COP_SESSION_COMPLETE:
		printf("complete");
		break;
	default:
		printf("%d", session->state);
		break;
	}
#endif
}

cop_state_t cop_get_session_state(cop_session_t * session)
{
	return session->state;
}

void cop_set_session_state(cop_session_t * session, cop_state_t newstate)
{
	session->state = newstate;
}

size_t cop_get_tgt_size(cop_session_t * session)
{
	return session->ptrPVT->tgt_dde.bytes;
}

size_t cop_get_src_size(cop_session_t * session)
{
	return session->ptrPVT->src_dde.bytes;
}

size_t cop_get_target_output_size(cop_session_t * session)
{
	if (cop_session_iscomplete(session)) {
		struct cop_csb *CSB =
		    (struct cop_csb *) cop_get_csbaddr(session->ptrPVT);

        uint8_t cop_number =
		    cop_ct_to_cop_number(session->ptrPVT->ccw.type);

        uint8_t can_go = CSB->cw.comp_code == 64 && cop_number == COP_DECOMP; 

        if (CSB->cw.comp_code == 0 || can_go) {
			if ((CSB->cw.comp_ext & 32) > 0) {
				return CSB->bytes;
			} else {
				struct cop_comp_cpb *cpb =
						(struct cop_comp_cpb *)
						cop_get_cpbptr(session->ptrPVT);
				return cpb->tgt_bytes;
			}
		}

	}
	return 0;
}

void cop_fixup_target_output_size(cop_session_t * session, size_t newsize)
{
	if (cop_session_iscomplete(session)) {
		struct cop_csb *CSB =
		    (struct cop_csb *) cop_get_csbaddr(session->ptrPVT);

		if ((CSB->cw.comp_ext & 32) > 0) {
			CSB->bytes = newsize;
			return;
		}

		struct cop_comp_cpb *cpb = (struct cop_comp_cpb *)
					   cop_get_cpbptr(session->ptrPVT);
		cpb->tgt_bytes = newsize;
	}
}

size_t cop_get_src_processed(cop_session_t * session)
{
	if (cop_session_iscomplete(session)) {
		struct cop_csb *CSB =
		    (struct cop_csb *) cop_get_csbaddr(session->ptrPVT);
		return CSB->bytes;
	}
	return 0;
}

cop_session_t *cop_commit_session(cop_session_t * session)
{
	volatile struct cop_csb *csb;

	/* check for a valid session */
	if ((!session) || (!session->ptrPVT)) {
		verbose_print("invalid cop_session_t\n");
		return NULL;
	}

	verbose_print("CRB\n");
	verbose_memprint(((void *) session->ptrPVT), sizeof(struct cop_crb));

	if (session->state >= COP_SESSION_INIT_COMPLETE &&
	    session->state < COP_SESSION_INFLIGHT) {
		cop_set_session_state(session, COP_SESSION_INFLIGHT);
		errno =
		    cop_dispatch_and_wait(session->ptrPVT);
	} else {
		return NULL;
	}
	if (!errno) {
		csb = (struct cop_csb *) cop_get_csbaddr(session->ptrPVT);
		//errno=csb->completion_extension;
	}

	verbose_print("CSB\n");
	verbose_memprint(cop_get_csbaddr
			 (((struct cop_crb *) session->ptrPVT)),
			 sizeof(struct cop_csb));

	verbose_print("CPB\n");
	verbose_memprint(cop_get_cpbptr
			 (((struct cop_crb *) session->ptrPVT)),
			 1024);

	return session;
}

cop_session_t *cop_commit_session_async(cop_session_t * session)
{
	if (session->state >= COP_SESSION_INIT_COMPLETE &&
	    session->state < COP_SESSION_INFLIGHT) {
		cop_set_session_state(session, COP_SESSION_INFLIGHT);
		errno =
		    cop_dispatch(session->ptrPVT);
	} else {
		return NULL;
	}

	return session;
}

// 0 invalid
// // 1 prepared but not submitted
// // 2 in flight
// // 3 complete
int cop_session_iscomplete(cop_session_t * session)
{
	if (session->state == COP_SESSION_COMPLETE)
		return 1;
	else if (session->state >= COP_SESSION_INFLIGHT &&
		 session->state < COP_SESSION_COMPLETE) {
		volatile struct cop_csb *csb = cop_get_csbaddr(session->ptrPVT);
		if (csb->cw.valid) {
			session->state = COP_SESSION_COMPLETE;
			return 1;
		} else
			return 0;
	} else
		return 0;
}
