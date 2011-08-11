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
#include "libcopmanage.h"
#include "libcopmalloc.h"
#include "libcopcache.h"
#include "libcop.h"
#include "libcopl/crypto.h"
#include "libcopl/asym.h"
#include "libcopl/cop_util.h"

extern struct cop_ct_info cop_ct_info_array[COP_NUMBER_KNOWN_COPS];
extern cop_session_t *cop_create_session();


cop_session_t *cop_create_asym_session()
{
	/* allocate space for the session */
	cop_session_t *session = cop_create_session();
	if (!session)
		return NULL;

	return session;
}

/*
 * @brief asymmetric coprocessor call
 *
 * @param session session structure for storing crb.
 * @param r_type operand group type (OP_ABNR...).
 * @param ops array of BigNumber operands.
 * @param ops_sz number of operands in ops.
 * @param cop_directive function type (MADD, MMUL..).
 *
 * This is the generic asymmetric crypto function that determines the
 * proper operand size and sets up the CRB. It returns a pointer to the
 * output operand. However, that operand will not be valid until
 * the session has been commited and completed.
 */
BigNumber *cop_asym(cop_session_t * session, uint8_t r_type,
		    const BigNumber ** ops, size_t ops_sz,
		    uint8_t op)
{
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	int rc_encode, rc_tab_index;
	size_t r_size;
	struct cop_crb *crb = cop_next_crb(cc_pool, COP_ASYM_CRYPTO);

	/* get rc_encode value and rc_tab_index */
	rc_encode = cop_asymops_to_rcencode(op, ops, ops_sz, &rc_tab_index);
	if (rc_encode < 0) {
		errno = -EINVAL;
		cop_set_session_state(session, COP_SESSION_INVALID);
		return NULL;
	}

	/* allocate result operand */
	r_size = cop_asymops_rsize(op, rc_tab_index);
	BigNumber *r = cop_bn_new_sz(r_size);

	/* setup crb */
	cop_asym_setup(crb, &cop_ct_info_array[COP_ASYM_CRYPTO].ctx,
		r->number, r->size, rc_encode, op);

	/* setup cpb */
	cop_asymcrypto_cpb *cpb = cop_get_cpbptr(crb);
	memset(cpb->op_bytes, 0x00,
	       sizeof(cpb->op_bytes));
	cop_asym_cpb_setup(cpb, op, ops, ops_sz, rc_tab_index);

	/* assign session crb pointer */
	session->ptrPVT = (struct cop_crb *) crb;

	/* update the session state */
	cop_set_session_state(session, COP_SESSION_INIT_COMPLETE);

	/* success */
	return r;
}

inline BigNumber *cop_asym_sync(const BigNumber * ops[3], uint8_t op_type,
				uint8_t ops_sz, uint8_t op_name)
{
	cop_session_t *session = cop_create_asym_session();
	BigNumber *r = cop_asym(session, op_type, ops, ops_sz, op_name);

	cop_commit_session(session);

	cop_free_session(session);
	return r;
}

/* synchronous calls */
BigNumber *cop_madd(const BigNumber * a, const BigNumber * b,
		    const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 3, COP_ASYM_MADD);
}

BigNumber *cop_msub(const BigNumber * a, const BigNumber * b,
		    const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 3, COP_ASYM_MSUB);
}

BigNumber *cop_mred(const BigNumber * a, const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 2, COP_ASYM_MR);
}

BigNumber *cop_mex(const BigNumber * a, const BigNumber * e,
		   const BigNumber * n)
{
	const BigNumber *ops[3] = { a, e, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 3, COP_ASYM_MEX);
}

BigNumber *cop_mmul(const BigNumber * a, const BigNumber * b,
		    const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 3, COP_ASYM_MMUL);
}

BigNumber *cop_mgmul(const BigNumber * a, const BigNumber * b,
		     const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 3, COP_ASYM_MGMUL);
}

BigNumber *cop_minv(const BigNumber * a, const BigNumber * n)
{
	if (gDD_version > 1) {
		const BigNumber *ops[2] = { a, n };
		return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 2, COP_ASYM_MINV);
	} else {
		errno = -ENODEV;
		return NULL;
	}
}

BigNumber *cop_mexc(const BigNumber * a, const BigNumber * dp,
		    const BigNumber * dq, const BigNumber * np,
		    const BigNumber * nq, const BigNumber * u)
{
	const BigNumber *ops[6] = { a, dp, dq, np, nq, u };
	return cop_asym_sync(ops, COP_ASYM_OP_ABNR, 6, COP_ASYM_MEXC);
}

BigNumber *cop_m2add(const BigNumber * a, const BigNumber * b,
		     const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 3, COP_ASYM_M2ADD);
}

BigNumber *cop_m2red(const BigNumber * a, const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 2, COP_ASYM_M2R);
}

BigNumber *cop_m2ex(const BigNumber * a, const BigNumber * e,
		    const BigNumber * n)
{
	const BigNumber *ops[3] = { a, e, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 3, COP_ASYM_M2EX);
}

BigNumber *cop_m2mul(const BigNumber * a, const BigNumber * b,
		     const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 3, COP_ASYM_M2MUL);
}

BigNumber *cop_mg2mul(const BigNumber * a, const BigNumber * b,
		      const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 3, COP_ASYM_MG2MUL);
}

BigNumber *cop_m2mi(const BigNumber * a, const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	return cop_asym_sync(ops, COP_ASYM_OP_ABRXYK, 2, COP_ASYM_M2MI);
}

/* asynchronous calls */
BigNumber *cop_madd_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MADD);
}

BigNumber *cop_msub_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MSUB);
}

BigNumber *cop_mred_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	size_t ops_sz = 2;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MR);
}

BigNumber *cop_mex_async(cop_session_t * session, const BigNumber * a,
			 const BigNumber * e, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, e, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MEX);
}

BigNumber *cop_mmul_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MMUL);
}

BigNumber *cop_mgmul_async(cop_session_t * session, const BigNumber * a,
			   const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MGMUL);
}

BigNumber *cop_minv_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	size_t ops_sz = 2;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MINV);
}

BigNumber *cop_mexc_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * dp, const BigNumber * dq,
			  const BigNumber * np, const BigNumber * nq,
			  const BigNumber * u)
{
	const BigNumber *ops[6] = { a, dp, dq, np, nq, u };
	size_t ops_sz = 6;
	return cop_asym(session, COP_ASYM_OP_ABNR, ops, ops_sz, COP_ASYM_MEXC);
}

BigNumber *cop_m2add_async(cop_session_t * session, const BigNumber * a,
			   const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz,
								COP_ASYM_M2ADD);
}

BigNumber *cop_m2red_async(cop_session_t * session, const BigNumber * a,
			   const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	size_t ops_sz = 2;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz, COP_ASYM_M2R);
}

BigNumber *cop_m2ex_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * e, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, e, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz,
								COP_ASYM_M2EX);
}

BigNumber *cop_m2mul_async(cop_session_t * session, const BigNumber * a,
			   const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz,
								COP_ASYM_M2MUL);
}

BigNumber *cop_mg2mul_async(cop_session_t * session, const BigNumber * a,
			    const BigNumber * b, const BigNumber * n)
{
	const BigNumber *ops[3] = { a, b, n };
	size_t ops_sz = 3;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz,
							COP_ASYM_MG2MUL);
}

BigNumber *cop_m2mi_async(cop_session_t * session, const BigNumber * a,
			  const BigNumber * n)
{
	const BigNumber *ops[2] = { a, n };
	size_t ops_sz = 2;
	return cop_asym(session, COP_ASYM_OP_ABRXYK, ops, ops_sz,
								COP_ASYM_M2MI);
}
