#ifndef _LIBCOPCACHE_H
#define _LIBCOPCACHE_H

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

typedef struct cop_cache_fn {
	unsigned long (*cop_available_DDEs)(mapped_memory_pool_t p);
	unsigned long (*cop_available_crbs)(mapped_memory_pool_t p, unsigned int type);
	void * (*cop_next_DDE)(mapped_memory_pool_t p);
	void * (*cop_next_crb)(mapped_memory_pool_t p,unsigned int type);
	void (*cop_give_back_crb)(mapped_memory_pool_t p, void *crb, unsigned int type);
	void (*cop_give_back_DDE)(mapped_memory_pool_t p, void *crb, unsigned int type);
	void (*cop_set_minimum_num_crbs)(mapped_memory_pool_t p, void *crb, unsigned int type, unsigned int num);
} cop_cache_fn;

void
cop_enable_tls_cache();

void
cop_set_crb_cache(unsigned int cop, unsigned int number);

void
cop_enable_processwide_cache();

cop_cache_fn *
cop_get_cache_fns();

unsigned long cop_tls_available_DDEs(mapped_memory_pool_t p);
unsigned long cop_tls_available_crbs(mapped_memory_pool_t p, unsigned int type);
void *cop_tls_next_DDE(mapped_memory_pool_t p);
void *cop_tls_next_crb(mapped_memory_pool_t p,unsigned int type);
void cop_tls_give_back_crb(mapped_memory_pool_t p, void *crb, unsigned int type);
void cop_tls_give_back_DDE(mapped_memory_pool_t p, void *crb, unsigned int type);
void cop_tls_set_minimum_num_crbs(mapped_memory_pool_t p, void *crb, unsigned int type, unsigned int num);

unsigned long cop_available_DDEs(mapped_memory_pool_t p);
unsigned long cop_available_crbs(mapped_memory_pool_t p, unsigned int type);
void *cop_next_DDE(mapped_memory_pool_t p);
void *cop_next_crb(mapped_memory_pool_t p,unsigned int type);
void cop_give_back_crb(mapped_memory_pool_t p, void *crb, unsigned int type);
void cop_give_back_DDE(mapped_memory_pool_t p, void *crb, unsigned int type);
void cop_set_minimum_num_crbs(mapped_memory_pool_t p, void *crb, unsigned int type, unsigned int num);

#define COP_NUMBER_KNOWN_COPS 7

#endif
