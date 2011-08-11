/*
 * Licensed Materials - Property of IBM
 * Restricted Materials of IBM
 * Copyright 2010
 * Chris J Arges
 * Everton Constantino
 * Tom Gall
 * IBM Corp, All Rights Reserved
 *                    
 * US Government Users Restricted Rights - Use Duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM
 * Corp
 *
 **/

#include <stdlib.h>

#include "deq.h"
#include "libcopmanage.h"
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcop.h"
#include "cop_cache.h"
#include "libcopl/comp.h"
#include "libcopl/regx.h"
#include "libcopl/asym.h"
#include "libcopl/crypto.h"
#include "libcopl/cop_util.h"

extern unsigned int COP_NUMBER_COPS;
extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];

extern void cop_init();

__thread struct lists *cop_thread_cache=NULL;

cop_cache_fn cache_fns;

void cop_create_and_prime_crbs(mapped_memory_pool_t ptr, unsigned int num, unsigned int cop, droot list);

void __attribute__ ((constructor)) cop_init_tls_cacheat_load(void) {
	int i;
	mapped_memory_pool_t mpr;

	mpr=cop_cc_pool();
	if (!mpr) {
		cop_init();
		mpr=cop_cc_pool();
		}
	unsigned long PBICtype=cop_pool_type(mpr);

	cop_thread_cache=malloc(sizeof (struct lists));
	dec_init(&(cop_thread_cache->DDE_block_list));
	for(i=0; i<COP_NUMBER_KNOWN_COPS; i++) {
		dec_init(&(cop_thread_cache->memory_block_list[i]));
	}
	if (COP_NUMBER_COPS) {
		switch(PBICtype) {
			case COP_PBIC_CC:
				for(i=0; i<=COP_ASYNC_DATA_MOVER; i++) {
					cop_create_and_prime_crbs(mpr, 50, i,&(cop_thread_cache->memory_block_list[i]) );
				}
				break;
			case COP_PBIC_RX:
				mpr=cop_rx_pool();
				for(i=COP_REGX; i<=COP_XML; i++) {
					cop_create_and_prime_crbs(mpr, 50, i,&(cop_thread_cache->memory_block_list[i]));
				}
				break;
		}
	}
cache_fns.cop_available_DDEs=cop_tls_available_DDEs;
cache_fns.cop_available_crbs=cop_tls_available_crbs;
cache_fns.cop_next_DDE=cop_tls_next_DDE;
cache_fns.cop_next_crb=cop_tls_next_crb;
cache_fns.cop_give_back_crb=cop_tls_give_back_crb;
cache_fns.cop_give_back_DDE=cop_tls_give_back_DDE;
cache_fns.cop_set_minimum_num_crbs=cop_tls_set_minimum_num_crbs;
}

void cop_init_tls_cache(mapped_memory_pool_t mpr) {
	int i;
	if (!mpr)
		mpr=cop_cc_pool();

	cop_thread_cache=malloc(sizeof (struct lists));
	dec_init(&(cop_thread_cache->DDE_block_list));
	for(i=0; i<COP_NUMBER_KNOWN_COPS; i++) {
		dec_init(&(cop_thread_cache->memory_block_list[i]));
	}
}

void cop_init_tls_by_type(mapped_memory_pool_t mpr,unsigned int type) {
	if (!mpr)
		mpr=cop_cc_pool();
	unsigned long PBICtype=cop_pool_type(mpr);

	switch(PBICtype) {
		case COP_PBIC_CC:
				cop_create_and_prime_crbs(mpr, 50, type,&(cop_thread_cache->memory_block_list[type]) );
			break;
		case COP_PBIC_RX:
				cop_create_and_prime_crbs(mpr, 50, type,&(cop_thread_cache->memory_block_list[type]));
			break;
	}
/*
cache_fns.cop_available_DDEs=cop_tls_available_DDEs;
cache_fns.cop_available_crbs=cop_tls_available_crbs;
cache_fns.cop_next_DDE=cop_tls_next_DDE;
cache_fns.cop_next_crb=cop_tls_next_crb;
cache_fns.cop_give_back_crb=cop_tls_give_back_crb;
cache_fns.cop_give_back_DDE=cop_tls_give_back_DDE;
cache_fns.cop_set_minimum_num_crbs=cop_tls_set_minimum_num_crbs;
*/
}

cop_cache_fn * cop_init_processwide_cache()
{
	return NULL;
}

cop_cache_fn * cop_get_cache_fns()
{
	return NULL;
}

void
cop_create_and_prime_crbs(mapped_memory_pool_t ptr, unsigned int num, unsigned int cop, droot list) {
    struct cop_crb *crb;
    struct cop_csb *ptrCSB;
    int i;
    size_t alloc_size;
    size_t crb_size = sizeof (struct cop_crb); // default

    for (i = 0; i < num; i++) {
	verbose_print("create crb[%d] type %d\n", i, cop);
	switch(cop) {
	  case COP_XML:
	  case COP_REGX:
	  	alloc_size = sizeof (struct cop_crb_regx) +
				sizeof (struct cop_csb);
		crb_size = sizeof (struct cop_crb_regx); // special
		break;
	  case COP_DECOMP:
	  	alloc_size = sizeof (struct cop_crb) +
				sizeof (struct cop_csb) +
				sizeof (struct cop_comp_cpb);
		break;
	  case COP_RNG:
	  case COP_ASYNC_DATA_MOVER:
	  case COP_SYM_CRYPTO:
	  	alloc_size = sizeof (struct cop_crb) +
				sizeof (struct cop_csb) +
				16*15; //FIXME: this _needs_ to be variable
		break;
	  case COP_ASYM_CRYPTO:
	  	alloc_size = sizeof (struct cop_crb) +
				sizeof (struct cop_csb) +
				sizeof (cop_asymcrypto_cpb);
		break;
	  default:
		return;
		break;
	}

	crb = (struct cop_crb *)cop_malloc(ptr, alloc_size, 128);
	memset(crb, 0x00, alloc_size);

	ptrCSB=(struct cop_csb *) ((char *)crb + crb_size);
	cop_set_csbaddr(crb, ptrCSB);

	(void)cop_crb_set_ccw_type(crb, &cop_ct_info_array[cop].ctx);
	(void)cop_crb_set_ccw_inst(crb, &cop_ct_info_array[cop].ctx);
	crb->mt_tag = cop_ct_info_array[cop].mt_id;

	dec_push(list, crb);
    }
}
