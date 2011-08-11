/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2008,2009, 2010
   Chris J Arges
   Everton Constantino
   Tom Gall
   Mike Kravetz
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_REGEX_H
#define _LIB_LIBCOPL_REGEX_H

/** @defgroup REGX Regular Expression coprocessor interfaces
 * @{ */

#define REGX_FC_SEARCH          0x0

#define REGX_CRB_ONLY		0x0
#define REGX_CRB_FIRST		0x1
#define REGX_CRB_MIDDLE		0x2
#define REGX_CRB_LAST		0x3

/**
 * @brief regx search command
 */
struct cop_regx_sc {
	unsigned short start; /**< @brief start position inside input buffer */
	unsigned short stop; /**< @brief stop position inside input buffer */
	unsigned ctrl_v:1; /**< @brief search valid (enable this search command) */
	unsigned ctrl_cs:2; /**< @brief continuous search (00 none, 01 first, 10 middle, 11 last) */
	unsigned ctrl_resv:5;
	unsigned ctrl_pchn:8; /**< @brief pattern context handler number */
	unsigned ctrl_pcid:16; /**< @brief pattern context ID */
};

/**
 * @brief regx coprocessor request block
 */
struct cop_crb_regx {
	union cop_ccw ccw;
	uint32_t _rsv0:1;
	uint32_t qos:3; /**< @brief quality of survives */
	uint32_t mt_tag:10; /**< @brief marker trace tag */
	uint32_t resume:1; /**< @brief set to indicate CRB is the first CRB in a software resumed session */
	uint32_t _rsv1:1;
	uint32_t ch:2; /**< @brief chain control, use REGX_CRB_{ONLY,FIRST,LAST,MIDDLE}*/
	uint32_t _rsv2:6;
	uint32_t seq_num:8; /**< @brief sequence for multi-CRB mode */
	union cop_csbaddr csbp; /**< @brief CSB address */
	struct cop_dde src_dde; /**< @brief source DDE */
	struct cop_dde tgt_dde; /**< @brief target DDE */
	struct cop_regx_sc sc[2]; /**< @brief two search commands*/
	struct cop_ccb ccb; /**< @brief coprocessor control block */
};

/**
 * @brief regx result structure
 */
struct cop_regx_results {
	unsigned control_flg:2; /**< @brief control flags, bit 0 valid, bit 1 not last */
	unsigned local_result_processor_flg:7; /**< @brief local results processor flags */
	unsigned select:1; /**< @brief define the type of info reported by the start/char field */
	unsigned match_reporting_bank:1; /**< @brief match reporting rule */
	unsigned search_command:1; /**< @brief number of the corresponding search command */
	unsigned group:4; /**< @brief group ID */
	unsigned table:5; /**< @brief table ID */
	unsigned short character:9; /**< @brief character/state */
	unsigned bfsm:2; /**< @brief logical number of the BFSM which reported the match */
	unsigned short result_processor_information; /**< @brief info from the result processor */
	unsigned short match_end_position; /**< @brief match end position */
};

/**
 * @brief execute a regx search given context(s)
 *
 * @param crb 		a valid/mapped coprocessor request block
 * @param ctx 		cop_ctx structure from open/bound copro interface
 * @param pcid1 	the pcid for the first search command.
 * @param pcid2 	the pcid for the second search command.
 * @param source 	the source data.
 * @param source_size 	the size of the source data.
 * @param target 	the target data.
 * @param target_size 	the size of the target buffer.
 * @param start1 	the starting index of the first search command.
 * @param stop1 	the ending index of the first search command.
 * @param start2 	the starting index of the second search command.
 * @param stop2 	the ending index of the second search command.
 */
static inline int cop_regx_searchl( struct cop_crb_regx *crb, struct cop_ctx *ctx, int pcid1, int pcid2,
	const void *source, size_t source_size, void *target, size_t target_size,
	int start1, int stop1, int start2, int stop2 ) 
{
	if (!crb) return -1;

	/* setup crb */
	cop_crb_set_ccw_fc_copid((struct cop_crb *)crb, COP_REGX, REGX_FC_SEARCH);
	crb->ch = 0x3;
	cop_set_sdptr((struct cop_crb *)crb,source); 
	crb->src_dde.bytes = source_size;
	cop_set_tdptr((struct cop_crb *)crb,target); 
	crb->tgt_dde.bytes = target_size;

	/* setup search controls */
	if( stop1 >= 0 ) {
		crb->sc[0].ctrl_v = 1;
		crb->sc[0].start = start1;
		crb->sc[0].stop = stop1;
		crb->sc[0].ctrl_pchn = 0;
		crb->sc[0].ctrl_pcid = pcid1;
		crb->sc[0].ctrl_cs = REGX_CRB_ONLY;
	} else {
		crb->sc[0].ctrl_v = 0;
	}

	if (stop2 >= 0 ) {
		crb->sc[1].ctrl_v = 1;
		crb->sc[1].start = start2;
		crb->sc[1].stop = stop2;
		crb->sc[1].ctrl_pchn = 0;
		crb->sc[1].ctrl_pcid = pcid2;
		crb->sc[1].ctrl_cs = REGX_CRB_ONLY;
	} else {
		crb->sc[1].ctrl_v = 0;
	}

	/* reset csb */
	struct cop_csb *csb = cop_get_csbaddr((struct cop_crb *)crb);
	if (!csb) return -1;
	cop_csb_reset(csb);

	/* setup context if necessary */
	if (ctx) {
		cop_crb_set_ccw_type((struct cop_crb *)crb, ctx);
		cop_crb_set_ccw_inst((struct cop_crb *)crb, ctx);
	}

	return 0;
}

/** @} */

#endif /* _LIB_LIBCOPL_REGEX_H */
