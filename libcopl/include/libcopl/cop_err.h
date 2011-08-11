/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_COP_ERR_H
#define _LIB_LIBCOPL_COP_ERR_H

#include <stdint.h>
#include <string.h>

/**
 * CSB completion codes.  Use cop_csb_cc_text() to get corresponding text
 * \ingroup cop_err
 * @{
 */
enum cop_csb_completion_code {
	NO_EXCEPTION = 0,
	INVAL_ALIGNMENT = 1,
	OPERAND_OVERLAP = 2,
	DATA_LEN_EXCEPT = 3,
	TRANSLATION = 5,
	PROTECTION = 6,
	EXTERNAL_ERROR_READ = 7,
	INVAL_OPERAND = 8,
	INSUFFICIENT_PRIV = 9,
	INTERNAL_ERROR = 10,
	EXTERNAL_ERROR = 12,
	TGT_SPACE_EXHAUSTED = 13,
	EXCESS_DDE_COUNT = 14,
	UNKNOWN_SUBFUNC = 17,
	FUNCTION_ABORTED = 18,
	SEGMENTED_DDE = 31,
	PROGRESS_POINT = 32,
	DDE_OVERFLOW = 33,
	SESSION_VIOLATION = 34,
	PROVISION_OVFL = 36,
	CHAIN = 37,
	SEQ_NUMBER = 38,
	CORRECTABLE_HW = 39,
	DMA_READ_ABORT = 64,
	DMA_READ_NONE = 65,
	PCIE_FCN_MAP_ERR = 66,
	ADLER_ERR = 67,
	ISIZE_ERR = 68,
	MULTI_MEMBER = 69,
	DECOMP_LERR_0 = 70,
	DECOMP_LERR_1 = 71,
	DECOMP_LERR_2 = 72,
	DECOMP_LERR_3 = 73,
	PCIE_FCN_MAP_ERR2 = 79,
	DMA_SRC_DDE_XLATE = 80,
	DMA_SRC_DDE_PROT = 81,
	DMA_SRC_DDE_PBUS = 82,
	DMA_TGT_DDE_XLATE = 83,
	DMA_TGT_DDE_PROT = 84,
	DMA_TGT_DDE_PBUS = 85,
	DMA_DATA_FETCH_XLATE = 86,
	DMA_DATA_FETCH_PROT = 87,
	DMA_DATA_FETCH_PBUS = 88
};
/* @} */

extern char *cop_csb_cc_text(int cc);

#endif /* _LIB_LIBCOPL_COP_ERR_H */
