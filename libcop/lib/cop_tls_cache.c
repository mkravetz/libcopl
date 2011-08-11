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

#include "deq.h"
#include <libcopmanage.h>
#include <libcopmalloc.h>
#include <libcopcache.h>
#include <libcop.h>
#include "cop_cache.h"
#include "libcopl/comp.h"
#include "libcopl/regx.h"
#include "libcopl/asym.h"
#include "libcopl/crypto.h"
#include <libcopl/cop_util.h>

extern __thread struct lists *cop_thread_cache;

extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];

extern void cop_create_and_prime_crbs(mapped_memory_pool_t ptr, unsigned int num, unsigned int cop, droot list);
extern void cop_init_tls_cache(mapped_memory_pool_t mpr);
extern void cop_init_tls_by_type(mapped_memory_pool_t mpr,unsigned int type);


unsigned long 
cop_tls_available_DDEs(mapped_memory_pool_t p) {
	if (cop_thread_cache)
		return dec_size(&(cop_thread_cache->DDE_block_list));
	else
		return 0;
}

unsigned long 
cop_tls_available_crbs(mapped_memory_pool_t p, unsigned int type) {
	if (cop_thread_cache)
		return dec_size(&(cop_thread_cache->memory_block_list[type]));	
	return 0;
}

void * cop_tls_next_DDE(mapped_memory_pool_t p)
{
	if (cop_thread_cache)
		return dec_pop(&(cop_thread_cache->DDE_block_list));

	return NULL;
}

void *cop_tls_next_crb(mapped_memory_pool_t p, unsigned int type) {
	if (cop_thread_cache)
		if (dec_size(&(cop_thread_cache->memory_block_list[type])))
			return dec_pop(&(cop_thread_cache->memory_block_list[type]));
		else {
			cop_init_tls_by_type(p,type);
			return dec_pop(&(cop_thread_cache->memory_block_list[type]));
		}
	else  {
		cop_init_tls_cache(p);
		cop_init_tls_by_type(p,type);
	}
	return dec_pop(&(cop_thread_cache->memory_block_list[type]));
}

void 
cop_tls_give_back_crb(mapped_memory_pool_t p, void *crb,unsigned int type) {
	struct cop_csb *ptrCSB;
	struct cop_crb *ptrCRB = (struct cop_crb *)crb;
	size_t zero_size;
	size_t crb_size = sizeof(struct cop_crb); // default

	switch (type) {
	  case COP_XML:
	  case COP_REGX:
	  	zero_size = sizeof(struct cop_crb_regx) +
				sizeof(struct cop_csb);
		crb_size = sizeof(struct cop_crb_regx);	// special
	  	break;
	  case COP_DECOMP:
	  	zero_size = sizeof(struct cop_crb) + sizeof(struct cop_csb) +
				sizeof (struct cop_comp_cpb);
	  	break;
	  case COP_RNG:
	  case COP_ASYNC_DATA_MOVER:
	  case COP_SYM_CRYPTO:
	  	zero_size = sizeof(struct cop_crb) + sizeof(struct cop_csb) +
				16*15; //FIXME: this _needs_ to be variable
	  	break;
	  case COP_ASYM_CRYPTO:
	  	zero_size = sizeof(struct cop_crb) + sizeof(struct cop_csb) +
				sizeof(cop_asymcrypto_cpb);
	  	break;
	  default:
	  	zero_size = sizeof(struct cop_crb) + sizeof(struct cop_csb);
	  	break;
	}

	memset(ptrCRB, 0x00, zero_size);

	ptrCSB=(struct cop_csb *)((char *)ptrCRB + crb_size);
	cop_set_csbaddr(ptrCRB, ptrCSB);

	(void)cop_crb_set_ccw_type(ptrCRB, &cop_ct_info_array[type].ctx);
	(void)cop_crb_set_ccw_inst(ptrCRB, &cop_ct_info_array[type].ctx);
	ptrCRB->mt_tag = cop_ct_info_array[type].mt_id;

	if (cop_thread_cache)
		dec_push(&(cop_thread_cache->memory_block_list[type]),crb);
	else {
		cop_init_tls_cache(p);
		dec_push(&(cop_thread_cache->memory_block_list[type]),crb);
	}
}

void 
cop_tls_give_back_DDE(mapped_memory_pool_t p, void *crb,unsigned  int type) {
}

void cop_tls_set_minimum_num_crbs(mapped_memory_pool_t p, void *crb, unsigned int type, unsigned int num) {
	unsigned int curnum;
	if (!cop_thread_cache)
		cop_init_tls_cache(p);
	curnum=cop_tls_available_crbs(p,type);
	if (curnum < num) {
		cop_create_and_prime_crbs(p,num-curnum, type, &(cop_thread_cache->memory_block_list[type]));
	}
}

unsigned long cop_available_DDEs(mapped_memory_pool_t p) __attribute__ ((weak, alias ("cop_tls_available_DDEs")));

unsigned long cop_available_crbs(mapped_memory_pool_t p, unsigned int type) __attribute__ ((weak, alias ("cop_tls_available_crbs")));

void *cop_next_DDE(mapped_memory_pool_t p) __attribute__ ((weak, alias ("cop_tls_next_DDE")));

void *cop_next_crb(mapped_memory_pool_t p,unsigned int type) __attribute__ ((weak, alias ("cop_tls_next_crb")));

void cop_give_back_crb(mapped_memory_pool_t p, void *crb,unsigned int type) __attribute__ ((weak, alias ("cop_tls_give_back_crb")));

void cop_give_back_DDE(mapped_memory_pool_t p, void *crb,unsigned  int type) __attribute__ ((weak, alias ("cop_tls_give_back_DDE")));

void cop_set_minimum_num_crbs(mapped_memory_pool_t p, void *crb, unsigned int type, unsigned int num)  __attribute__ ((weak, alias ("cop_tls_set_minimum_num_crbs")));
