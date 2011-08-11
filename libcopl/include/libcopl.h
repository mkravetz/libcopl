/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/


#ifndef _LIB_LIBCOPL_H
#define _LIB_LIBCOPL_H

/*
 * Convennient method for including ALL libcopl functionality
 */
#include <libcopl/adm.h>	/* async data mover setup routines */
#include <libcopl/asym.h>	/* asymmetric cryptographic functions */
#include <libcopl/comp.h>	/* compression coproc data structures */
#include <libcopl/cop_err.h>	/* coprocessor related error codes */
#include <libcopl/cop.h>	/* basic coproc data structures */
#include <libcopl/cop_sys.h>	/* system/kernel interfaces */
#include <libcopl/cop_util.h>	/* utility routines for coprocessor access */
#include <libcopl/crypto.h>	/* cryptographic (sym and asym) coprocessors */
#include <libcopl/icswx.h>	/* coprocessor invocation via icswx */
#include <libcopl/icswx_util.h>	/* utility routines built around icswx */
#include <libcopl/regx.h>	/* regular expression coprocessor */
#include <libcopl/rnd.h>	/* random number setup */
#include <libcopl/waitm.h>	/* wait on memory (modification) */

#endif	/* _LIB_LIBCOPL_H */
