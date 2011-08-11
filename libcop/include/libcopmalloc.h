#ifndef _LIBMALLOC_H
#define _LIBMALLOC_H

/*  

Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#include <stdint.h>
#include <string.h>

#include "cop_debug.h"

#ifdef __powerpc64__
#define ASSIGNPTR2INT(ptr) ((uint64_t)ptr);
#else
#define ASSIGNPTR2INT(ptr) ((uint64_t)(uintptr_t)ptr);
#endif


enum BlockSize_t {
	pool_block_Size4K = 0x00001000L,
	pool_block_Size16K = 0x00004000L,
	pool_block_Size64K = 0x00010000L,
	pool_block_Size256K = 0x00040000L,
	pool_block_Size1M = 0x00100000L,
	pool_block_Size4M = 0x00400000L,
	pool_block_Size16M = 0x01000000L,
	pool_block_Size64M = 0x04000000L,
	pool_block_Size256M = 0x10000000L
};

#define COP_PBIC_CC 0
#define COP_PBIC_RX 1

typedef void *mapped_memory_pool_t;

void *cop_malloc(mapped_memory_pool_t p, size_t size,
	unsigned long alignment);
void *cop_malloc_unaligned(mapped_memory_pool_t p, size_t size);
void cop_free(mapped_memory_pool_t p, void *ptr);

/*
mapped_memory_pool_t
cop_create_mapped_allocation_pool(unsigned long blockSize,
	unsigned long numberofpages);
*/

mapped_memory_pool_t 
cop_create_pool(unsigned long initialSize, unsigned long PBICinstance);

mapped_memory_pool_t
cop_create_pool_by_type(unsigned long initialSize, unsigned long PBICtype, int bolted);

int cop_free_pool(mapped_memory_pool_t p);

int cop_setup_pbic(mapped_memory_pool_t p, const void *address, size_t length);

mapped_memory_pool_t 
cop_rx_pool();
mapped_memory_pool_t 
cop_cc_pool();

int
cop_pool_type(mapped_memory_pool_t p);

/*
mapped_memory_pool_t
cop_create_mapped_allocation_pool(unsigned long blockSize,
	unsigned long numberofpages);
*/
#endif /* LIBMALLOC_H */
