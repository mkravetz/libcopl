/* 
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 *  Copyright 2010
 *  Mike Kravetz
 *  IBM Corp, All Rights Reserved
 *                      
 *  US Government Users Restricted Rights - Use Duplication or
 *  disclosure restricted by GSA ADP Schedule Contract with IBM
 *  Corp 
 *                                
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libcopl/cop_err.h>

/**
 * \defgroup cop_err Coprocessor error assistnace routines
 */

/**
 * CSB completion code text strings returned by cop_csb_cc_text().
 * Correspond to codes in enum cop_csb_completion_code.
 * \ingroup cop_err
 * @{
 */
#define NO_EXCEPTION_TXT	"No exception condition"
#define INVAL_ALIGNMENT_TXT	"Invalid-Alignment Exception"
#define OPERAND_OVERLAP_TXT	"Operand-Overlap Exception"
#define	DATA_LEN_EXCEPT_TXT	"Data-Length Exception"
#define TRANSLATION_TXT		"CCB/CSB Write Translation Exception"
#define PROTECTION_TXT		"CCB/CBS Write Protection Exception"
#define EXTERNAL_ERROR_READ_TXT	"External Uncorrectable Error on Read"
#define INVAL_OPERAND_TXT	"Invalid-Operand Exception"
#define INSUFFICIENT_PRIV_TXT	"Insufficient-Privilege Exception"
#define INTERNAL_ERROR_TXT	"Internal Uncorrectable Error"
#define EXTERNAL_ERROR_TXT	"External Uncorrectable Error"
#define TGT_SPACE_EXHAUSTED_TXT	"Target-data-space Exhausted"
#define EXCESS_DDE_COUNT_TXT	"Excessive-DDE Count"
#define	UNKNOWN_SUBFUNC_TXT	"Unrecognized-subfunction Exception"
#define FUNCTION_ABORTED_TXT	"Function aborted"
#define SEGMENTED_DDE_TXT	"Segmented-DDE Exception"
#define	PROGRESS_POINT_TXT	"Progress point"
#define DDE_OVERFLOW_TXT	"Source Data DDE Overflow Exception"
#define SESSION_VIOLATION_TXT	"Session-violation Exception"
#define	PROVISION_OVFL_TXT	"Provision Overflow"
#define CHAIN_TXT		"Chain exception"
#define SEQ_NUMBER_TXT		"Sequence-number exception"
#define	CORRECTABLE_HW_TXT	"Correctable Hardware Error"
#define DMA_READ_ABORT_TXT	"DMA Read PCIe Abort response"
#define DMA_READ_NONE_TXT	"DMA Read PCIe Unsupported Request response"
#define PCIE_FCN_MAP_ERR_TXT	"PCIe function mapping error"
#define PCIE_FCN_MAP_ERR2_TXT	"PCIe function mapping error"
#define ADLER_ERR_TXT		"Adler Error"
#define	ISIZE_ERR_TXT		"ISIZE Error"
#define	MULTI_MEMBER_TXT	"Multi-member Exception"
#define	DECOMP_LERR_0_TXT	"Decomp Logical Error #0"
#define	DECOMP_LERR_1_TXT	"Decomp Logical Error #1"
#define	DECOMP_LERR_2_TXT	"Decomp Logical Error #2"
#define	DECOMP_LERR_3_TXT	"Decomp Logical Error #3"
#define DMA_SRC_DDE_XLATE_TXT	"DMA Source DDE Translation Exception"
#define DMA_SRC_DDE_PROT_TXT	"DMA Source DDE Protection Error"
#define DMA_SRC_DDE_PBUS_TXT	"DMA Source DDE PBus Error"
#define DMA_TGT_DDE_XLATE_TXT	"DMA Target DDE Translation Exception"
#define DMA_TGT_DDE_PROT_TXT	"DMA Target DDE Protection Exception"
#define DMA_TGT_DDE_PBUS_TXT	"DMA Target DDE PBus Error"
#define DMA_DATA_FETCH_XLATE_TXT "DMA Data Fetch Translation Exception"
#define DMA_DATA_FETCH_PROT_TXT	"DMA Data Fetch Protection Exception"
#define DMA_DATA_FETCH_PBUS_TXT	"DMA Data Fetch PBus Exception"

#define UNKNOWN_ERR_TXT		"Unknown error"
/* @} */

/**
 * @brief Return the text associated with CSB completion code
 * \ingroup cop_err
 *
 * This routine simply takes a CSB completion code @a cc as an argument
 * and returns a pointer to the corresponding text string.
 *
 * @param cc	CSB completion code
 * @return	Text string corresponding to completion code
 */
char *cop_csb_cc_text(int cc)
{
    switch(cc) {
	case NO_EXCEPTION:	return NO_EXCEPTION_TXT;	break;
	case INVAL_ALIGNMENT:	return INVAL_ALIGNMENT_TXT;	break;
	case OPERAND_OVERLAP:	return OPERAND_OVERLAP_TXT;	break;
	case DATA_LEN_EXCEPT:	return DATA_LEN_EXCEPT_TXT;	break;
	case TRANSLATION:	return TRANSLATION_TXT;		break;
	case PROTECTION:	return PROTECTION_TXT;		break;
	case EXTERNAL_ERROR_READ: return EXTERNAL_ERROR_READ_TXT; break;
	case INVAL_OPERAND:	return INVAL_OPERAND_TXT;	break;
	case INSUFFICIENT_PRIV:	return INSUFFICIENT_PRIV_TXT;	break;
	case INTERNAL_ERROR:	return INTERNAL_ERROR_TXT;	break;
	case EXTERNAL_ERROR:	return EXTERNAL_ERROR_TXT;	break;
	case TGT_SPACE_EXHAUSTED: return TGT_SPACE_EXHAUSTED_TXT; break;
	case EXCESS_DDE_COUNT:	return EXCESS_DDE_COUNT_TXT;	break;
	case UNKNOWN_SUBFUNC:	return UNKNOWN_SUBFUNC_TXT;	break;
	case FUNCTION_ABORTED:	return FUNCTION_ABORTED_TXT;	break;
	case SEGMENTED_DDE:	return SEGMENTED_DDE_TXT;	break;
	case PROGRESS_POINT:	return PROGRESS_POINT_TXT;	break;
	case DDE_OVERFLOW:	return DDE_OVERFLOW_TXT;	break;
	case SESSION_VIOLATION:	return SESSION_VIOLATION_TXT;	break;
	case PROVISION_OVFL:	return PROVISION_OVFL_TXT;	break;
	case CHAIN:		return CHAIN_TXT;		break;
	case SEQ_NUMBER:	return SEQ_NUMBER_TXT;		break;
	case CORRECTABLE_HW:	return CORRECTABLE_HW_TXT;	break;
	case DMA_READ_ABORT:	return DMA_READ_ABORT_TXT;	break;
	case DMA_READ_NONE:	return DMA_READ_NONE_TXT;	break;
	case PCIE_FCN_MAP_ERR:	return PCIE_FCN_MAP_ERR_TXT;	break;
	case PCIE_FCN_MAP_ERR2:	return PCIE_FCN_MAP_ERR2_TXT;	break;
	case ADLER_ERR:		return ADLER_ERR_TXT;		break;
	case ISIZE_ERR:		return ISIZE_ERR_TXT;		break;
	case MULTI_MEMBER:	return MULTI_MEMBER_TXT;	break;
	case DECOMP_LERR_0:	return DECOMP_LERR_0_TXT;	break;
	case DECOMP_LERR_1:	return DECOMP_LERR_1_TXT;	break;
	case DECOMP_LERR_2:	return DECOMP_LERR_2_TXT;	break;
	case DECOMP_LERR_3:	return DECOMP_LERR_3_TXT;	break;
	case DMA_SRC_DDE_XLATE:	return DMA_SRC_DDE_XLATE_TXT;	break;
	case DMA_SRC_DDE_PROT:	return DMA_SRC_DDE_PROT_TXT;	break;
	case DMA_SRC_DDE_PBUS:	return DMA_SRC_DDE_PBUS_TXT;	break;
	case DMA_TGT_DDE_XLATE:	return	DMA_TGT_DDE_XLATE_TXT;	break;
	case DMA_TGT_DDE_PROT:	return DMA_TGT_DDE_PROT_TXT;	break;
	case DMA_TGT_DDE_PBUS:	return DMA_TGT_DDE_PBUS_TXT;	break;
	case DMA_DATA_FETCH_XLATE: return DMA_DATA_FETCH_XLATE_TXT; break;
	case DMA_DATA_FETCH_PROT: return DMA_DATA_FETCH_PROT_TXT; break;
	case DMA_DATA_FETCH_PBUS: return DMA_DATA_FETCH_PBUS_TXT; break;
	default:		return UNKNOWN_ERR_TXT;		break;
    }
    return UNKNOWN_ERR_TXT;
}
