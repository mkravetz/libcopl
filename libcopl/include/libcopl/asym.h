/*  
Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/

#ifndef _LIB_LIBCOPL_ASYM_H
#define _LIB_LIBCOPL_ASYM_H

#include <errno.h>
#include <libcopl/cop.h>
#include <libcopl/cop_sys.h>
#include <libcopl/cop_util.h>

/**
 * @ingroup CRYPTO
 * @defgroup ASYM General Asymmetric Cryptographic definitions.
 * @{
 */

enum cop_asym_functions {
/**
 * @brief Integer Modular Multiplication. 
 * Performs the operation <b>R=A*B mod N</b>. Uses cop_asym_imf*_cpb structures. */
	COP_ASYM_MMUL = 0,
/**
 * @brief Integer Modular Montgomery Multiplication. 
 * Performs the operation <b>R=A*B*O^(-1) mod N</b>. Uses cop_asym_imf*_cpb structures. */
	COP_ASYM_MGMUL = 1,
/**
 * @brief Integer Modular Exponentiation. 
 * Performs the operation <b>R=A^B mod N</b>. Uses cop_asym_imf*_cpb structures. */
	COP_ASYM_MEX = 2,
/**
 * @brief Integer Modular Reduction. 
 * Performs the operation <b>R=A mod N</b>. Uses cop_asym_imri*_cpb structures. */
	COP_ASYM_MR = 3,
/**
 * @brief Integer Modular Inverse. 
 * Performs the operation <b>R=A^(-1) mod N</b>. Uses cop_asym_imri*_cpb structures. */
	COP_ASYM_MINV = 4,
/**
 * @brief Integer Modular Addition. 
 * Performs the operation <b>R=A+B mod N</b>. Uses cop_asym_imf*_cpb structures. */
	COP_ASYM_MADD = 5,
/**
 * @brief Integer Modular Subtraction. 
 * Performs the operation <b>R=A-B mod N</b>. Uses cop_asym_imf*_cpb structures. */
	COP_ASYM_MSUB = 6,
/**
 * @brief Integer Modular CRT Exponentiation. 
 * Performs the operation <b>R=A^B mod N, where N=Np*Nq, Dp=B Mod Np-1, Dq=B Mod Nq-1, U=Nq-1 Mod Np 
</b>. Uses cop_asym_imcrt*_cpb structures. */
	COP_ASYM_MEXC = 7,
/**
 * @brief Elliptical Curve Cryptography GF(p) Point Add. 
 * Performs the operation <b>(Xr,Yr)=(Xp,Yp)+(Xq,Yq) (Other Inputs: N=modulus) </b>. Uses cop_asym_eccpadd*_cpb structures. */
	COP_ASYM_PADD = 8,
/**
 * @brief Elliptical Curve Cryptography GF(p) Point Double. 
 * Performs the operation <b>(Xr,Yr)=(Xp,Yp)*2 (Other Inputs: N=modulus) </b>. Uses cop_asym_eccpdbl*_cpb structures. */
	COP_ASYM_PDBL = 9,
/**
 * @brief Elliptical Curve Cryptography GF(p) Point Multiplication. 
 * Performs the operation <b>(Xr,Yr)=(Xp,Yp)*K (Other Inputs: N=modulus) </b>. Uses cop_asym_eccpmul*_cpb structures. */
	COP_ASYM_PMUL = 10,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Multiplication. 
 * Performs the operation <b>R(x)=A(x)*B(x) Mod N(x) </b>. Uses cop_asym_eccm2ame*_cpb structures. */
	COP_ASYM_M2MUL = 11,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Montgomery Multiplication. 
 * Performs the operation <b>R(x)=A(x)*B(x)*O-1 Mod N(x) </b>. Uses cop_asym_eccm2ame*_cpb structures. */
	COP_ASYM_MG2MUL = 12,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Exponentiation. 
 * Performs the operation <b>R(x)=A(x)^B(x) Mod N(x) </b>. Uses cop_asym_eccm2ame*_cpb structures. */
	COP_ASYM_M2EX = 13,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Reduction. 
 * Performs the operation <b>R(x)= A(x) Mod N(x) </b>. Uses cop_asym_eccm2ri*_cpb structures. */
	COP_ASYM_M2R = 14,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Inverse. 
 * Performs the operation <b>R(x)=A(x)-1 Mod N(x) </b>. Uses cop_asym_eccm2ri*_cpb structures. */
	COP_ASYM_M2MI = 15,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Modular Addition. 
 * Performs the operation <b>R(x)=A(x)+B(x) Mod N(x) </b>. Uses cop_asym_eccm2ame*_cpb structures. */
	COP_ASYM_M2ADD = 16,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Point Addition. 
 * Performs the operation <b>[Xr(x),Yr(x)]=[Xp(x),Yp(x)]+[Xq(x),Yq(x)] (Other Inputs: N(x)=irreducible polynomial, a=elliptic curve equation coeffecient) </b>. Uses cop_asym_eccp2add*_cpb structures. */
	COP_ASYM_P2ADD = 17,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Point Doubling. 
 * Performs the operation <b>[Xr(x),Yr(x)]=[Xp(x),Yp(x)]*2 (Other Inputs: N(x)=irreducible polynomial, a=elliptic curve equation coeffecient) </b>. Uses cop_asym_eccp2dbl*_cpb structures. */
	COP_ASYM_P2DBL = 18,
/**
 * @brief Elliptical Curve Cryptography GF(2^m) Point Multiplication. 
 * Performs the operation <b>[Xr(x),Yr(x)]=[Xp(x),Yp(x)]*K (Other Inputs: N(x)=irreducible polynomial, a=elliptic curve equation coeffecient) </b>. Uses cop_asym_eccp2mul*_cpb structures. */
	COP_ASYM_P2MUL = 19,
/**
 * @brief Integer Modular Reduction Negated. 
 * Performs the operation <b>R= -A Mod N= N - (A Mod N) </b>. Uses cop_asym_imri*_cpb structures. */
	COP_ASYM_MNR = 20,
};

/** @} ASYM */

/*
 * cop_asym_rc_encode_table
 *
 * This table corresponds to values specified in the "Asym Operand
 * Size Encoding" table in the Hardware Reference Manual (V 1.91,
 * tables 10-39 and 10-40).
 */
#define COP_ASYM_RC_SIZE         15
#define COP_ASYM_RC_OPS_SIZE     8

#define COP_ASYM_OP_ENCODE       0
#define COP_ASYM_OP_ABNR         1
#define COP_ASYM_OP_DQNQ         2
#define COP_ASYM_OP_DPNPU        3
#define COP_ASYM_OP_XYKN         4
#define COP_ASYM_OP_ABRXYK       5
#define COP_ASYM_OP_N            6
#define COP_ASYM_OP_A            7
extern size_t cop_asym_rc_encode_table[COP_ASYM_RC_SIZE][COP_ASYM_RC_OPS_SIZE];

/*
 * cop_asym_op_class
 *
 * Array that correlates asym operation to operand class
 */
extern uint8_t cop_asym_op_class[];

/*
 * cop_asym_rc_op_type_table
 *
 * Table that describes Asym operands for each operation type
 */
#define COP_ASYM_OP_NUM		6
#define COP_ASYM_OP_OPS		20
extern uint8_t cop_asym_rc_op_type_table[COP_ASYM_OP_OPS][COP_ASYM_OP_NUM];

/** @defgroup ASYM Asymmetric coprocessor interfaces
 * @{ */
/**
 * @brief BigNumber
 *
 * Data structure defining operants to and result of Asym Coprocessor
 * operations.
 * 
 * - size - size of @p number field in bytes
 * - number - bytes containing numeric value
 */
typedef struct {
	size_t size;
	uint8_t *number;
} BigNumber;

/* TODO: get rid of BigNumbers */

/**
 * @deprecated This structure is going to be phased out.
 *
 * @brief  Asymmetric Crypto Coprocessor Parameter Block - CPB
 *
 * The Asym CPB is primarily used to pass parameters for Asym operations
 * to the Asym coprocessor.
 *
 * - cr - Output Condition register
 * - op_bytes - Operand Bytes
 */
typedef struct {
	uint8_t cr:4;
	uint8_t _rsv_0:4;
	uint8_t _rsv_1[15];
	uint8_t op_bytes[1536];  /*TODO get rid of this */
} cop_asymcrypto_cpb;

/**
 * @brief  Asymmetric Crypto Coprocessor Structure
 */
struct cop_asym_cxb {
	struct cop_crb crb; /**< @brief Coprocessor request block. */
	uint8_t _pad[48];
	struct cop_csb csb; /**< @brief Coprocessor status block. */
	uint8_t cpb[16]; /**< @brief Reserved portion of CPB append all operands to this. */
};

/**
 * @brief Simple validation of BigNumber
 *
 * This routine simply validates the passed BigNumber data structure.
 * Minimal checking is done, is simply ensures the pointers are non-NULL.
 *
 * @param bn BigNumber to validate
 *
 * @return zero on success, negative value on failure
 */
static inline int cop_check_bignumber(const BigNumber * x)
{
        if (!x)
		return -EINVAL;
	if (!x->size)
		return -EFAULT;

	return 0;
}

/**
 * @brief Return pointer to operand type information
 *
 * This routine simply takes an Asym operation code @a op and returns
 * a pointer in to the @c cop_asym_rc_op_type_table that describes the
 * operands for the operation.
 *
 * @param op Asym operation code. COP_ASYM_MMUL, COP_ASYM_MGMUL, ...
 *
 * @return pointer to 'row' in cop_asym_rc_op_type_table that describes
 * 		operands to used in the specified operation.
 */
static inline uint8_t *cop_asymops_op_type(int op)
{
	return (cop_asym_rc_op_type_table[op]);
}


/**
 * @brief Return size of result operand
 *
 * This routine is designed to return the required size of the result of
 * a specified Asym operation.  The operation is specifcied by the @a
 * parameter.  The @a rc_index parameter is an inded into the
 * @c cop_asym_rc_encode_table.  @a rc_index must have been obtained from
 * a previous call to @c cop_asymops_to_rcencode().
 *
 * @param op Asym operation code. COP_ASYM_MMUL, COP_ASYM_MGMUL, ...
 * @param rc_index result of a previous call to @c cop_asymops_to_rcencode()
 *
 * @return required size of result operand
 */
static inline size_t cop_asymops_rsize(int op, size_t rc_index)
{
	return (ROUND_TO_NEXT_QUADWORD(
		CEIL_BYTE(cop_asym_rc_encode_table
				[rc_index][cop_asym_op_class[op]])));
}


/**
 * @brief Asymmetric Crypto CRB setup function
 *
 * This routine provides a convenient mechanism for initializing the
 * CRB and CSB control blocks associated with a Asym operation.  A CPB
 * is also required for all Asym operations, and this routine does not
 * initialize the corresponding CPB.  A separate call to @c cop_asym_cpb_setup()
 * must be bade to initialize the CPB structure.

 * @param crb a valid/mapped coprocessor request block
 * @param ctx cop_ctx structure from open/bound copro interface
 * @param target pointer to target operand memory must be 16B aligned
 * @param target_size size in bytes for output parameter
 * @param rc_encode specifies operand bit size, rc_encode value can be
 *	  obtained by calling the @c cop_asymops_to_rcencode() routine.
 * @param op Operation type (COP_ASYM_MMUL, COP_ASYM_MGMUL ...)
 *
 */
static inline int cop_asym_setup(struct cop_crb *crb, struct cop_ctx *ctx, 
	void *target, size_t target_size, uint8_t rc_encode, uint8_t op)
{
	if (!crb) return -1;

	/* setup copro blocks */
	cop_crb_set_ccw_fc_copid(crb, COP_ASYM_CRYPTO, op);
	crb->src_dde.rc = rc_encode;
	cop_set_sdptr(crb, NULL);
	crb->src_dde.bytes = 0;
	cop_set_tdptr(crb, target);
	crb->tgt_dde.bytes = target_size;
	crb->tgt_dde.countt = 0;

	/* reset csb */
	struct cop_csb *csb = cop_get_csbaddr(crb);
	if (!csb) return -1;
	cop_csb_reset(csb);
	
	/* setup context if necessary */
	if (ctx) {
		cop_crb_set_ccw_type(crb, ctx);
		cop_crb_set_ccw_inst(crb, ctx);
	}

	/* success */
	return 0;
}


/**
 * @brief Asymmetric Crypto CPB setup function
 *
 * This routine is designed to assist with initialization of a CPB
 * associated with an Asym operation.  The CPB primarily consists of
 * of the source operands to the Asym operation.  This routine uses
 * information in the @c cop_asym_rc_encode_table to determine how
 * to copy operands to the CPB.
 *
 * @param cpb Pointer to CPB structure to be initialized.  Note that the
 *	CPB must immediately follow the CSB that is pointed to by the CRB.
 * @param op Operation type (COP_ASYM_MMUL, COP_ASYM_MGMUL ...)
 * @param ops Array of operation input operands
 * @param ops_sz Number of operands in @ops array
 * @param rc_tab_index Index into @c cop_asym_rc_encode_table for the operation
 * 	and arguments.  rc_tab_index is obtained by a previous call to the
 *	@c cop_asymops_to_rcencode() routine.
 *
 * @return no return value
 */
static inline void cop_asym_cpb_setup(cop_asymcrypto_cpb *cpb, int op,
					const BigNumber ** ops, size_t ops_sz,
					int rc_tab_index)
{
	size_t op_pos = 0;
	uint8_t *ops_type;
	size_t size_bytes;
	size_t size_qw;
	int i;

	ops_type = cop_asymops_op_type(op);
	for (i = 0; i < ops_sz; i++) {
		size_bytes = CEIL_BYTE(
				cop_asym_rc_encode_table
						[rc_tab_index][ops_type[i]]);
		size_qw = ROUND_TO_NEXT_QUADWORD(size_bytes);

		memcpy(&cpb->op_bytes[op_pos] +
			(size_qw - ops[i]->size), ops[i]->number, ops[i]->size);

		op_pos += size_qw;
	}
}

/** @} */

extern int cop_asymops_to_rcencode(int op, const BigNumber ** ops,
				   size_t ops_sz, int *r_size);

#endif	/* _LIB_LIBCOPL_ASYM_H */
