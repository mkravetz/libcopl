#ifndef _COP_CACHE_H
#define _COP_CACHE_H

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

struct lists {
unsigned long PBICtype;
struct dec_root memory_block_list[COP_NUMBER_KNOWN_COPS];
struct dec_root DDE_block_list;
};


#endif
