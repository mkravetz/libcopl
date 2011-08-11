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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include "config.h"
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcop.h"
#include "libcopmanage.h"

#include "./libcopl/adm.h"

struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];

void *cop_memcpy(void *dest, void *src, size_t n)
{
	struct cop_crb *crb;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_ASYNC_DATA_MOVER);

	cop_memcpy_setup(crb, &cop_ct_info_array[COP_ASYNC_DATA_MOVER].ctx,
			 dest, src, n);
	int error = cop_dispatch_and_wait(crb);

	cop_give_back_crb(cc_pool, crb, COP_ASYNC_DATA_MOVER);
	if (error) {
		errno = -error;
		return NULL;
	}
	return dest;
}

void *cop_memmove(void *dest, void *src, size_t n)
{
	struct cop_crb *crb;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	crb = cop_next_crb(cc_pool, COP_ASYNC_DATA_MOVER);

	cop_memmove_setup(crb, &cop_ct_info_array[COP_ASYNC_DATA_MOVER].ctx,
			  dest, src, n);
	int error = cop_dispatch_and_wait(crb);

	cop_give_back_crb(cc_pool, crb, COP_ASYNC_DATA_MOVER);
	if (error) {
		errno = -error;
		return NULL;
	}
	return dest;
}

void *cop_memset(void *s, int c, size_t n)
{
	int i, j;
	mapped_memory_pool_t cc_pool = cop_cc_pool();

	unsigned long pagesz = getpagesize();
	char *tmp = cop_malloc(cc_pool, pagesz, 64);
	memset(tmp, c, pagesz);

	j = n / pagesz;
	for (i = 0; i < j; i++) {
		verbose_print("copy block #%d\n", i);
		cop_memcpy((((char *)s) + (i * pagesz)), tmp, pagesz);
	}

	size_t remain = n % pagesz;
	verbose_print("remain = %zd\n", remain);
	if (remain) {
		cop_memcpy((((char *)s) + (i * pagesz)), tmp, remain);
	}

	return s;
}

