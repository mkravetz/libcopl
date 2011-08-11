#ifndef HESPERORNIS_DEC
#define HESPERORNIS_DEC 1

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

// An embarrassingly "SIMPLE" stack implementation

#ifdef __cplusplus
#define __C__ "C"
#else
#define __C__
#endif

typedef struct dec_node *dnode;
typedef struct dec_root *droot;

typedef struct dec_node {
	dnode next;
	void *contents;
} dec_node;

typedef struct dec_root {
	dnode root;
	dnode freelist;
	unsigned int size;
} dec_root;

extern __C__ droot create_dec();
extern __C__ void dec_init(droot d);
extern __C__ void free_dec(droot d);

extern __C__ void dec_push(droot d, void *nc);
extern __C__ void *dec_pop(droot d);

extern __C__ unsigned int dec_size(droot d);

#endif
