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
#include <sys/errno.h>
#include "libcop.h"
#include "libcopmalloc.h"
#include "config.h"
#include "libcopcache.h"

#include "./libcopl/regx.h"

#ifdef REGX_SUPPORT
#include <regx_um.h>

#define REGX_UM_GLOBAL_MEMSIZE	10
#define RESULT_BUFFER_SIZE 	4096

regx_um_shared_t *um_shared = NULL;

/********************************************************************************/
/* Private Functions */

inline void print_results_entry(struct cop_regx_results *r)
{
#ifdef VERBOSE_ON
	if (gVerbose) {
		printf("**** Results Entry %p ****\n", r);
		printf("control_flg %d\nlocal_result_processor_flg %d\nselect %d\n", 
			r->control_flg, r->local_result_processor_flg, r->select);
		printf("match_reporting_bank %d\nsearch_command %d\ngroup %d\n",
			r->match_reporting_bank, r->search_command, r->group);
		printf("table %d\ncharacter %d\nbfsm %d\n", r->table,
			r->character, r->table);
		printf("result_processor_information %d\nmatch_end_position %d\n", 
			r->result_processor_information, r->match_end_position);
		printf("***********************\n");
	}
#endif
}

inline void print_search_command(struct cop_regx_sc *c)
{
#ifdef VERBOSE_ON
	if (gVerbose) {
		printf("**** Search Command %p ****\n", c);
		printf("start = %u, stop = %u\n", c->start, c->stop);
		printf("v = %u, cs = %u, pchn = %u, pcid = %u\n", c->ctrl_v,
			c->ctrl_cs, c->ctrl_pchn, c->ctrl_pcid);
		printf("***********************\n");
	}
#endif
}

/********************************************************************************/
/* Public Functions */

/**
 * cop_regx_create_session() - allocate a session structure
 *
 * Returns a pointer to an allocated session structure, NULL on errors,
 * Init UM if it hasn't been done yet.
 */
cop_session_t *cop_regx_create_session()
{
	int i;
	errno = 0;
	mapped_memory_pool_t rx_pool = cop_rx_pool();

	/* allocate space for session information */
	cop_session_t *session = cop_create_session();
	CHECK(session, "cannot create session", ENOMEM, NULL);

	/* allocate space for regx specific data */
	session->data = malloc(sizeof(cop_regx_session_t));
	cop_regx_session_t *regx_session = (cop_regx_session_t *) session->data;
	if (!session->data) {
		free(session);
		fprintf(stderr, "Cannot malloc session data.\n");
		errno = ENOMEM;
		return NULL;
	}

	/* setup the crb */
	session->ptrPVT = (struct cop_crb *)cop_next_crb(rx_pool, COP_REGX);

	/* allocate results buffer */
	for (i = 0; i < REGX_MAX_SEARCHES_PER_SESSION; i++) {
		regx_session->search[i].results =
			cop_malloc(rx_pool, RESULT_BUFFER_SIZE, 1);
		if (!regx_session->search[i].results) {
			free(regx_session);
			free(session);
			fprintf(stderr, "Cannot malloc regx results buffer.\n");
			errno = ENOMEM;
			return NULL;
		}
	}

	regx_session->search_count = regx_session->results_count = 0;

	if (um_shared == NULL) {
		free(regx_session);
		free(session);
		fprintf(stderr, "UM failed to initialize!\n");
		errno = EAGAIN;
		return NULL;
	} else {
		regx_session->um_shared = um_shared;
	}

	/* update session state and return */
	cop_set_session_state(session, COP_SESSION_INIT);
	return session;
}

/**
 * cop_regx_init() - starts up the upload manager
 * 
 * Note that cop_regx_init() and cop_regx_close() must be the first and last 
 * function called regarding the use of the regx cop.
 */
void cop_regx_init()
{
	/* initialize the upload manager */
	if (um_shared == NULL) {
		um_shared = um_regx_init(REGX_UM_GLOBAL_MEMSIZE);
		if (um_shared->um_state != READY) {
			fprintf(stderr, "UM failed to initialize!\n");
		}
	}
}

/**
 * cop_regx_close() - close the upload manager
 */
void cop_regx_close()
{
	um_regx_close();
}

/**
 * cop_regx_free_session() - free a session
 * @ session - pointer to session structure to be freed.
 *
 * Note, we only free stuff if it actually exists.
 */
void cop_regx_free_session(cop_session_t * session)
{
	if (session) {
		if (session->data) {
			int i;
			cop_regx_session_t *regx_session =
				(cop_regx_session_t *) session->data;
			for (i = 0; i < REGX_MAX_SEARCHES_PER_SESSION; i++) {
				cop_free(cop_rx_pool(),
					regx_session->search[i].results);
			}
		}
		cop_free_session(session);
	}
}

/**
 * cop_regx_reset_session() - reuse session memory with new context
 * @ session - pointer to session structure to be reset.
 *
 * Reset session to a zeroed state. Useful for recycling sessions.
 */
void cop_regx_reset_session(cop_session_t * session)
{
	if (session) {
		if (session->data) {
			int i;
			cop_regx_session_t *regx_session =
				(cop_regx_session_t *) session->data;

			regx_session->search_count =
				regx_session->results_count = 0;

			/* reset all searches */
			for (i = 0; i < REGX_MAX_SEARCHES_PER_SESSION; i++) {
				memset(regx_session->search[i].results, 0x00,
					RESULT_BUFFER_SIZE);
				regx_session->search[i].results_total = 0;
			}
		}
	}
}

/**
 * cop_regx_pcs_load() - upload a pcs and map file
 * @session - session pointer.
 * @pcs_file - .pcs binary rules file, created by compiler.
 * @map_file - .map file, maps between match results and pattern, created by compiler.
 * 
 * Returns the load_id, return -1 if invalid
 */
um_pcs_handle_t cop_regx_pcs_load(cop_session_t * session, char *pcs_file)
{
	cop_regx_session_t *regx_session = (cop_regx_session_t *) session->data;

	/* check if UM is ready to load data */
	regx_um_shared_t *um_shared = (regx_um_shared_t *)regx_session->um_shared;
	CHECK(um_shared->um_state == READY,
		"um not initialized", EBUSY, -1);

	/* load the pcs file */
	um_pcs_handle_t id = um_pcs_load(pcs_file);
	CHECK(id != -1, "um returned invalid load_id", EINVAL, -1);

	/* TODO: since upload manager can only handle one PCS file at a time,
	 * it always returns 0 */
	return id;
}

// TODO: ADD cop_regx_replace
// TODO: ADD cop_regx_unload

/**
 * cop_regx_search() - execute a search given a context
 * @ session - an already allocated session handle.
 * @ ctx_id - the desired context number.
 * @ source - Source pointer to the data.
 * @ source_size - length of the source data.
 */
void cop_regx_search(cop_session_t * session, uint32_t ctx_id,
	const void *source, size_t source_size)
{
	cop_regx_search_ex(session, ctx_id, ctx_id, source, source_size, 0,
		source_size - 1, 0, -1);
}

/**
 * cop_regx_search_ex() - execute a search given a context
 * @ session - an already allocated session handle.
 * @ ctx_id - the desired context number for the first search command.
 * @ ctx_id2 - the desired context number for the second search command.
 * @ source - Source pointer to the data.
 * @ source_size - length of the source data.
 * @ start1 - first search command string start.
 * @ stop1 - first search command string stop.
 * @ start2 - second search command strign start.
 * @ stop2 - second search command string stop.
 */
int cop_regx_search_ex(cop_session_t * session, uint32_t ctx_id1,
	uint32_t ctx_id2, const void *source,
	size_t source_size, int start1, int stop1, int start2, int stop2)
{
	int i = 0;
	//mapped_memory_pool_t rx_pool = cop_rx_pool();
	errno = 0;

	verbose_print
		("cop_regx_search_ex( %p, %d, %d, %p, %d, %d, %d, %d, %d)\n",
		session, ctx_id1, ctx_id2, source, source_size, start1, stop1,
		start2, stop2);

	/* perform parameter checks */
	CHECK(session, "invalid session", EINVAL, -1);
	CHECK(source, "invalid source", EINVAL, -1);
	CHECK(session->data, "invalid session data", EINVAL, -1);

	cop_regx_session_t *regx_session = (cop_regx_session_t *) session->data;

	/* TODO: check UM state to make sure PCS file is loaded */

	/* get the search handle */
	search_handle_t *search_handle1 = um_get_search_handle(ctx_id1);
	CHECK(search_handle1, "can't get search handle", EINVAL, -1);

	search_handle_t *search_handle2 = um_get_search_handle(ctx_id2);
	CHECK(search_handle2, "can't get search handle", EINVAL, -1);

	verbose_print("searches_number = %d\n",
		search_handle1->searches_number);
	for (i = 0; i < search_handle1->searches_number; i++) {
		verbose_print("search_handle1->pcid[%d] = %d\n", i,
			search_handle1->pcid[i]);
	}
	verbose_print("searches_number = %d\n",
		search_handle2->searches_number);
	for (i = 0; i < search_handle2->searches_number; i++) {
		verbose_print("search_handle2->pcid[%d] = %d\n", i,
			search_handle2->pcid[i]);
	}

	int lanes =
		search_handle1->searches_number >=
		search_handle2->
		searches_number ? search_handle1->searches_number :
		search_handle2->searches_number;
	regx_session->search_count = lanes;

	//[TODO] Check Erratum 98
	for (i = 0; i < regx_session->search_count; i++) {
		int pcid1 =
			(i <
			search_handle1->
			searches_number) ? search_handle1->pcid[i] : -1;
		int pcid2 =
			(i <
			search_handle2->
			searches_number) ? search_handle2->pcid[i] : -1;

		stop1 = pcid1 < 0 ? -1 : stop1;
		stop2 = pcid2 < 0 ? -1 : stop2;

		//From RegX Spec 1.02 so we avoid a hazard, not exactly an erratum
		pcid1 = pcid1 < 0 ? pcid2 : pcid1;
		pcid2 = pcid2 < 0 ? pcid1 : pcid2;

		/* setup crb for search */
		cop_regx_searchl((struct cop_crb_regx *)session->ptrPVT, NULL,
			pcid1, pcid2, source, source_size,
			regx_session->search[i].results, RESULT_BUFFER_SIZE,
			start1, stop1, start2, stop2);
		cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);

		/* save search commands to session */
		struct cop_crb_regx *crb =
			(struct cop_crb_regx *)session->ptrPVT;
		regx_session->search[i].cmd[0] = crb->sc[0];
		regx_session->search[i].cmd[1] = crb->sc[1];

		cop_commit_session(session);

		//while (!cop_session_iscomplete(session)) ;

		verbose_print("Error = %d\n", errno);

		//[TODO] We should be checking for errors here

		regx_session->search[i].results_total =
			cop_get_src_processed(session) / 8;
		regx_session->results_count +=
			regx_session->search[i].results_total;

		verbose_print("pcid1 = %d; pcid2 = %d\n", pcid1, pcid2);
		verbose_print("regx_session->search[%d].results_total = %d\n",
			i, regx_session->search[i].results_total);
	}

	return 0;
}

/**
 * cop_regx_get_results() - populates an offset array
 * @ session - regx session.
 * @ offset_array - returns a pointer to an array of integer offsets.
 * @ pattern_array - returns a pointer to an array of pattern numbers.
 * @ scommand_array - returns a pointer to an array of scommands 
 *
 * Returns the number of results. User is responsible for freeing
 * all arrays.
 */
int cop_regx_get_results(cop_session_t * session, int **offset_array,
	int **pattern_array, int **scommand_array)
{
	int i = 0;

	/* check parameters */
	CHECK(session, "invalid session", EINVAL, 0);
	CHECK(session->data, "invalid session data", EINVAL, 0);

	cop_regx_session_t *regx_session = (cop_regx_session_t *) session->data;

	/* ensure that the search operation is complete */
	if (cop_get_session_state(session) != COP_SESSION_COMPLETE) {
		errno = EBUSY;
		fprintf(stderr, "no results, session not ready\n");
		return 0;
	}

	/* determine the number of results */
	verbose_print("results_count = %d\n", regx_session->results_count);
	if (regx_session->results_count == 0) {
		return 0;
	}

	regx_matches_t **matches =
		(regx_matches_t **) malloc(sizeof(regx_matches_t *) *
		regx_session->search_count);

	int count_patterns_hit = 0, j = 0;
	for (i = 0; i < regx_session->search_count; i++) {
		if (regx_session->search[i].results_total > 0) {
			int pcid1 = regx_session->search[i].cmd[0].ctrl_pcid;
			int pcid2 = regx_session->search[i].cmd[1].ctrl_pcid;
			int offset;

			matches[i] =
				(regx_matches_t *) malloc(sizeof(regx_matches_t)
				* 2);

			if (regx_session->search[i].cmd[0].ctrl_v == 1) {
				offset = um_get_matches((regx_session->search
						[i].results), &(matches[i][0]),
					0, pcid1, 0);
				verbose_print
					("(%d) matches[%d][0]->matches_num = %d\n",
					pcid1, i, matches[i][0].matches_num);

				/* [TODO] Keep calling um_get_matches until offset == 0 */
			}

			if (regx_session->search[i].cmd[1].ctrl_v == 1) {
				offset = um_get_matches((regx_session->search
						[i].results), &(matches[i][1]),
					0, pcid2, 1);
				verbose_print
					("(%d) matches[%d][1]->matches_num = %d\n",
					pcid2, i, matches[i][1].matches_num);

				/* [TODO] Keep calling um_get_matches until offset == 0 */
			}

			int scmd = 0;
			for (scmd = 0; scmd < 2; scmd++) {
				for (j = 0; j < matches[i][scmd].matches_num;
					j++) {
					count_patterns_hit +=
						matches[i][scmd].match[j].
						patterns_num;
				}
			}
		}
	}

	regx_session->results_count = count_patterns_hit;

	verbose_print("regx_session->results_count = %d\n",
		regx_session->results_count);

	/* allocate space for all necessary arrays */
	if (offset_array != NULL) {
		(*offset_array) =
			malloc(sizeof(int) * regx_session->results_count);
		CHECK((*offset_array), "can't malloc space for offset_array",
			ENOMEM, 0);
	}

	if (pattern_array != NULL) {
		(*pattern_array) =
			malloc(sizeof(int) * regx_session->results_count);
		CHECK((*pattern_array), "can't malloc space for pattern_array",
			ENOMEM, 0);
	}

	if (scommand_array != NULL) {
		(*scommand_array) =
			malloc(sizeof(int) * regx_session->results_count);
		CHECK((*scommand_array),
			"can't malloc space for scommand_array", ENOMEM, 0);
	}

	/* enumerate the offset aray */
	int idx = 0;
	for (i = 0; i < regx_session->search_count; i++) {
		int j = 0;
		int scmd = 0;
		if( regx_session->search[i].results_total > 0 ){
			for (scmd = 0; scmd < 2; scmd++) {
				if( regx_session->search[i].cmd[scmd].ctrl_v == 1 ){
					for (j = 0; j < matches[i][scmd].matches_num; j++) {
						int k = 0;
						/* [TODO] UM restriction, REMOVE THEM IN THE FUTURE!  */
						unsigned int *patterns_ptr = 
							(unsigned int *) matches[i][scmd].
												match[j].
												patterns_ptr;
						for (k = 0;
							k <
							matches[i][scmd].match[j].patterns_num;
							k++) {
							if (offset_array != NULL) {
								(*offset_array)[idx] =
									matches[i][scmd].match[j].location;
							}
							if (pattern_array != NULL) {
								(*pattern_array)[idx] =
									*patterns_ptr;
								patterns_ptr += 2;
							}
							if (scommand_array != NULL) {
								(*scommand_array)[idx] = scmd;
							}
							idx++;
						}
					}
				}
			}	
		}

	}

	for (i = 0; i < regx_session->search_count; i++) {
		if( regx_session->search[i].results_total > 0 ){
			free(matches[i]);
		}
	}
	free(matches);

	return regx_session->results_count;
}
#endif
