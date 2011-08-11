/*  
 *
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 * Copyright 2008,2009, 2010
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <config.h>

#include "deq.h"
#include "cop_map_tree.h"
#include "libcopmanage.h"
#include "libcopmalloc.h"

// from libcopmanage
struct cop_pbic_info cop_pbic_info_array[COP_NUMBER_KNOWN_PBICS];

void *cop_malloc(mapped_memory_pool_t ptr, size_t size,
               unsigned long alignment) {
    mapped_memory_pbic_root * p=(mapped_memory_pbic_root *)ptr;
    char *toBeReturned=NULL;

    if (alignment <= 0x40000000 ) 
        toBeReturned=cop_MMTree_malloc(p, p->Tree, size, alignment,p->bolted);
	else
		fprintf(stderr,"alignment too large\n");
	
	return toBeReturned;
}

void *cop_malloc_unaligned(mapped_memory_pool_t ptr, size_t size) {
    mapped_memory_pbic_root * p=(mapped_memory_pbic_root *)ptr;

    return cop_MMTree_malloc(p, p->Tree, size, 1, p->bolted); 
}

void cop_free(mapped_memory_pool_t ptr, void *ptrMemoryObject) { 
    mapped_memory_pbic_root * p=(mapped_memory_pbic_root *)ptr;

    cop_MMTree_free(p->Tree, ptrMemoryObject);
}
