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

#include "config.h"
#include "libcop.h"
#include "deq.h"

#include <stdlib.h>
#include <stdio.h>

extern __C__ droot create_dec()
{
	droot d = malloc(sizeof(struct dec_root));
	dnode n;
	int i;

	d->root = NULL;
	d->freelist = malloc(sizeof(struct dec_node));
	d->freelist->next = NULL;
	for (i = 0; i < 5; i++) {
		n = malloc(sizeof(struct dec_node));
		n->next = d->freelist;
		d->freelist = n;
	}
	d->size = 0;

	return d;
}

extern __C__ void dec_init(droot d)
{
	dnode n;
	int i;

	d->freelist = malloc(sizeof(struct dec_node));
	d->freelist->next = NULL;
	for (i = 0; i < 5; i++) {
		n = malloc(sizeof(struct dec_node));
		n->next = d->freelist;
		d->freelist = n;
	}
	d->root = NULL;
	d->size = 0;

}

extern __C__ void free_dec(droot d)
{
	dnode n;
	while (d->root) {
		n = d->root;
		free(n);
	}
	while (d->freelist) {
		n = d->freelist->next;
		free(n);
	}
	free(d);
}

extern __C__ void dec_push(droot d, void *nc)
{
	dnode n;
	if (d->freelist) {
		n = d->freelist;
		d->freelist = d->freelist->next;
	} else {
		n = malloc(sizeof(dec_node));
	}

	d->size++;

	//verbose_print("%p size %d contents %p\n", n, d->size, nc);

	n->contents = nc;
	if (nc==NULL)
		verbose_print("%p, contents are NULL\n",n);
	n->next = d->root;
	d->root = n;
}

extern __C__ void *dec_pop(droot d)
{
	dnode n = d->root;
	void *ptr=NULL;

	if (d->size >0) {
		d->root=d->root->next;

		ptr = n->contents;

		//verbose_print("%p size %d contents %p\n", n, d->size, ptr);

		n->next = d->freelist;
		d->freelist = n;
		n->contents = NULL;

		d->size--;
	}

//	verbose_print("%p size %d contents %p\n",n, d->size, ptr);
	return ptr;
}

extern __C__ unsigned int dec_size(droot d)
{
	return d->size;
}
