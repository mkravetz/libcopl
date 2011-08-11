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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "config.h"
#include "libcopl/asym.h"
#include "libcopl/cop_util.h"

#define OP_ENCODE	0
#define OP_ABNR		1
#define OP_DQNQ		2
#define OP_DPNPU	3
#define OP_XYKN		4
#define OP_ABRXYK	5
#define OP_N		6
#define OP_A		7

/**
 * @brief cop_asym_rc_encode_table
 * \ingroup ASYM
 *
 * This table corresponds to values specified in the "Asym Operand
 * Size Encoding" table in the Hardware Reference Manual (V 1.91,
 * tables 10-39 and 10-40).
 */
size_t cop_asym_rc_encode_table[COP_ASYM_RC_SIZE][COP_ASYM_RC_OPS_SIZE] = {
/*      RC,   ABNR,  DNq, DNp,XYKN,2mABR, 2mN,  2mA      */
	{0,     56,   28,   29,  56,  56,  57,  56},
	{8,      0,    0,    0,   0, 163, 164,  56},
	{16,   192,   96,   97, 192,   0,   0,   0},
	{24,   224,  112,  113, 224,   0,   0,   0},
	{32,     0,    0,    0,   0, 233, 234,  56},
	{40,   256,  128,  129, 256,   0,   0,   0},
	{48,     0,    0,    0,   0, 283, 284,  56},
	{56,   384,  192,  193, 384,   0,   0,   0},
	{64,     0,    0,    0,   0, 409, 410,  56},
	{72,   512,  256,  257,   0,   0,   0,   0},
	{80,   521,  261,  262, 521,   0,   0,   0},
	{88,     0,    0,    0,   0, 571, 572,  56},
	{96,  1024,  512,  513,   0,   0,   0,   0},
	{104, 2048, 1024, 1025,   0,   0,   0,   0},
	{112, 4096, 2048, 2049,   0,   0,   0,   0}
};

/**
 * @brief cop_asym_rc_op_type_table
 * \ingroup ASYM
 *
 * Table that describes Asym operands for each operation type
 */
uint8_t cop_asym_rc_op_type_table[COP_ASYM_OP_OPS][COP_ASYM_OP_NUM] = {
	{1, 1, 1, 0, 0, 0}, //MMUL
	{1, 1, 1, 0, 0, 0}, //MGMUL
	{1, 1, 1, 0, 0, 0}, //MEX
	{1, 1, 1, 0, 0, 0}, //MR
	{1, 1, 1, 0, 0, 0}, //MINV
	{1, 1, 1, 0, 0, 0}, //MADD
	{1, 1, 1, 0, 0, 0}, //MSUB
	{1, 3, 2, 3, 2, 3}, //MEXC
	{4, 4, 4, 4, 4, 0}, //PADD
	{4, 4, 4, 0, 0, 0}, //PDBL
	{4, 4, 4, 4, 0, 0}, //PMUL
	{5, 5, 6, 0, 0, 0}, //M2MUL
	{5, 5, 6, 0, 0, 0}, //MG2MUL
	{5, 5, 6, 0, 0, 0}, //M2EX
	{5, 6, 0, 0, 0, 0}, //M2R
	{5, 6, 0, 0, 0, 0}, //M2MI
	{5, 5, 6, 0, 0, 0}, //M2ADD
	{5, 5, 5, 5, 6, 7}, //P2ADD
	{5, 5, 6, 7, 0, 0}, //P2DBL
	{5, 5, 6, 7, 6, 0}, //P2MUL
};

uint8_t cop_asym_op_class[] = {
	OP_ABNR,	/* MMUL */
	OP_ABNR,	/* MGMUL */
	OP_ABNR,	/* MEX */
	OP_ABNR,	/* MR */
	OP_ABNR,	/* MINV */
	OP_ABNR,	/* MADD */
	OP_ABNR,	/* MSUB */
	OP_ABNR,	/* MEXC */
	OP_XYKN,	/* PADD ??? */
	OP_XYKN,	/* PDBL ??? */
	OP_XYKN,	/* PMUL ??? */
	OP_ABRXYK,	/* M2MUL */
	OP_ABRXYK,	/* MG2MUL */
	OP_ABRXYK,	/* M2EX */
	OP_ABRXYK,	/* M2R */
	OP_ABRXYK,	/* M2MI */
	OP_ABRXYK,	/* M2ADD  */
	OP_DPNPU,	/* P2ADD */
	OP_DPNPU,	/* P2DBL */
	OP_DPNPU	/* P2MUL */
};


/*
 * determine_asym_encode_value
 *
 * Internal routine to determine rc encode value based on operation
 * and input parameters.  cop_asym_rc_encode_table is used to determine
 * value.
 */
int determine_asym_encode_value(const BigNumber ** ops,
			    const size_t ops_sz,
			    const uint8_t * ops_type, int * rc_index)
{
	unsigned int i, j;

	/* determine operand bitsizes */
	size_t ops_bits[COP_ASYM_RC_OPS_SIZE];
	for (i = 0; i < ops_sz; i++) {
		ops_bits[i] = ops[i]->size << 3;
	}

	/* determine best encode value */
	uint8_t done = 1;
	for (i = 0; i < COP_ASYM_RC_SIZE; i++) {
		done = 1;
		for (j = 0; j < ops_sz; j++) {
			if (ops_bits[j] > cop_asym_rc_encode_table
							[i][ops_type[j]]) {
				done = 0;
				break;
			}
		}
		if (done)
			break;
	}
	if (!done)
		return -EINVAL;

	/* set the rc index value */
	*rc_index = i;

	return 0;
}


/**
 * @brief Get the rc encode value associated with Asym operantion and operands
 * \ingroup ASYM
 *
 * This routine is used to determine the rc encode value associated with an
 * Asym operation and the corresponding source operands.  The rc encode value
 * must be set in the CRBs that are passed to the Asym coprocessor.  The
 * rc encode value is determined by doing a lookup in the
 * @c cop_asym_rc_encode_table.  The index of 'row' in the
 * @c cop_asym_rc_encode_table will be returned in the @a rc_index as it
 * will be neeeded in calls to subsequent Asym setup/helper routines.
 *
 * @param op Operation type (COP_ASYM_MMUL, COP_ASYM_MGMUL ...)
 * @param ops Array of operation input operands
 * @param ops_sz Number of operands in @ops array
 * @param rc_index Pointer to integer that will be set upon successful exit
 *	of routine.  rc_index is the index into @c cop_asym_rc_encode_table
 *	that can be passed to other setup/helper routines.
 *
 * @return non-negative rc encode value if successful.  negative value if
 * 	failure.
 */
int cop_asymops_to_rcencode(int op, const BigNumber ** ops, size_t ops_sz,
				int *rc_index)
{
	int i;
	uint8_t *ops_type;

	/* validate operands */
	for (i = 0; i < ops_sz; i++) {
		if (cop_check_bignumber(ops[i]))
			return -EINVAL;
	}

	/* determine operand types */
	ops_type = cop_asymops_op_type(op);

	/* determine operand encode values */
	if (determine_asym_encode_value(ops, ops_sz, ops_type, rc_index))
		return -EINVAL;

	return (cop_asym_rc_encode_table[*rc_index][COP_ASYM_OP_ENCODE]);
}
