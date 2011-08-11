#ifndef _LIB_COPMANAGE_H
#define _LIB_COPMANAGE_H

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
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>

#define COP_NUMBER_KNOWN_PBICS 2
#define COP_RX_PBIC 1
#define COP_CC_PBIC 0

#define COP_NUMBER_KNOWN_COPS 7
#define COP_MINIMUM 0
#define COP_DECOMP 0
#define COP_SYM_CRYPTO 1
#define COP_ASYM_CRYPTO 2
#define COP_RNG 3
#define COP_ASYNC_DATA_MOVER 4
#define COP_REGX 5
#define COP_XML 6

struct cop_ct_info {
	struct cop_ctx ctx;
    	char *ptrDevName;
	int unit_fd;
	unsigned short mt_id;	/* marker trace cop id */
};

struct cop_pbic_info {
	struct cop_ctx ctx;	/* ctx copro instance the PBIC is behind */
};

/* check for existence of coprocessor on hardware
 **/
int cop_exists(int cop);

int cop_get_status(int cop_number);

/* What is the status of the coprocess, is it available, shared or
 * reserved by other process(es)
 **/

/* setup coprocessor use
 * These are specific to the following api's **ONLY**
 **/

#define COP_DEV_PATH "/dev/cop_type/"

void cop_init_cops(void);

void cop_open_and_bind(int cop);
void cop_open_and_bind_pbic(unsigned int PBICnum, char *coprocessor);
void cop_get_cop_type(int cop_instance, char *cop_name, int memory_flag);


#endif /* LIB_COPMANAGE_H */
