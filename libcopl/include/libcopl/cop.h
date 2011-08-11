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

#ifndef _LIB_LIBCOPL_COP_H
#define _LIB_LIBCOPL_COP_H

#include <stdint.h>
#include <string.h>

/**
 *  * @defgroup cop_ctl Coprocessor Control Blocks (data structures)
 *   * @{
 *    */

/**
 * @brief Coprocessor Command Word - CCW
 *
 * Used to specify a specific coprocessor.  This word in embedded within the
 * CRB, and is also passed directly via the icswx instruction.
 *
 * - type - coprocessor type
 * - dir - directive. dynamic field containing coprocessor command function and
 *             specific instance of coprocessor.
 * - value - Integer containing type and directive
 */
union cop_ccw {
	int value;
	struct {
		uint32_t _rsv_:8;
		uint32_t type:8;
		uint32_t dir:16;
	};
};

/**
 * @brief Control Word within Coprocessor Status Block (CSB)
 *
 * Written by a coprocessor to indicate status of last request.  This union
 * is embedded withing the CSB.
 *
 * - valid - indicates CSB contents are valid for reading (written by coproc)
 * - format - 0 indicates 16B CSB size (only size supported)
 * - ch - Chaining State
 * - seq_num - Completion sequence number
 * - comp_code - zero, or a code indicating exception with request
 * - comp_ext - completion extension bits
 */
union cop_csbctl {
	uint32_t word32;
	struct {
		uint32_t valid:1;
		uint32_t _rsv_:4;
		uint32_t format:1;
		uint32_t ch:2;
		uint32_t seq_num:8;
		uint32_t comp_code:8;
		uint32_t comp_ext:8;
	};
};

/**
 * @brief Coprocessor Status Block - CSB
 *
 * Status block written by coprocessor at the completion of a request.
 *
 * - cw  - Control Word (see cop_csbctl above)
 * - bytes - bytes processed by coprocessor request
 * - addr - address whose meaning is determined by cw.completion_code
 *           and cw.completion_extension
 */
struct cop_csb {
	union cop_csbctl cw;
	uint32_t bytes;
	uint64_t addr;
};
#define COP_CSB_ALIGN	16	/* required CSB alignment */

/**
 * @brief Address of Coprocessor Status Block (CSB)
 *
 * A CSB must be 16 byte aligned.  So, the low order bits are used for
 * other purposes.
 *
 * - addr - The 16 byte aligned address shifted 4 bits
 * - cv - 0 no extra write, 1 extra write as defined by CM field
 * - type - 0 = effective, 1 = real (hyp only)
 * - marked - 1 = Mark for performance monitor
 * - addr_value = long word containing all values
 */
union cop_csbaddr {
	uint64_t addr_value;
	struct {
		uint64_t addr:60;
		uint64_t cv:1;
		uint64_t _rsv_:1;
		uint64_t type:1;
		uint64_t marked:1;
	};
};

/**
 * @brief Data Descriptor Entry - DDE
 *
 * Description of data parameters passed to coprocessor.  A source and
 * target DDE are embedded within the Coprocessor Request Block (CRB).
 * - pr - progress requested
 * - count - 0 = direct DDE, !0 = indirect DDE with dde_count DDEs
 *                 starting at data.
 * - p_index - pool index
 * - bytes - total bytes pointed to by direct DDE
 * - data - address of data or indirect DDEs
 */
struct cop_dde {
	union {
		uint16_t rc;
		struct {
			uint16_t pr:1;
			uint16_t _rsv_:15;
		};
	};
	uint8_t  countt;
	uint8_t  pi;
	uint32_t bytes;
	uint64_t data;
};

/**
 * @brief Coprocessor Completion Block - CCB
 *
 * The Coprocessor Completion Block (CCB) is primarily used to specify the
 * completion method associated with coprocessor requests.  Some coprocessors
 * make use of the completion_value and completion_address fields.  The CCB
 * Is embedded withing the Coprocessor Request Block (CRB).
 *
 * - value - Completion value
 * - addr -  Completion address
 * - mthd - Specifies Completion store, Interrupt or JOE Enqueue
 */
struct cop_ccb {
	uint8_t  _rsv_:2;
	uint64_t value:62;
	uint64_t addr:61;
	uint8_t  mthd:3;
};

/**
 * @brief Coprocessor Request Block - CRB
 *
 * The Coprocessor Request Block (CSB) is the primary control block passed to
 * a coprocessor.  A CSB must be aligned on a 128 byte boundary.
 *
 * - ccw - Control word, see cop_ccw above
 * - qos - 
 * - marker_trace_tag - Used if marker trace enabled
 * - flags - coprocessor specific
 * - ch - Chain control.  only, first, middle or last CRB.
 * - seq_num - CRB sequence number
 * - csbp - address of CSB, see cop_csbaddr above
 * - src_dde - Source data descriptor.  see cop_dde above
 * - tgt _dde - Target data descriptor. see cop_dde above
 * - ccb - Completion block, see cop_ccb above
 */
struct cop_crb {
	union cop_ccw	ccw;
	uint32_t	_rsv_:1;
	uint32_t	qos:3;
	uint32_t	mt_tag:10;
	uint32_t	flags:8;
	uint32_t	ch:2;
	uint32_t	seq_num:8;
	union cop_csbaddr csbp;
	struct cop_dde	src_dde;
	struct cop_dde	tgt_dde;
	struct cop_ccb	ccb;
};
#define COP_CRB_ALIGN	128	/* required CRB alignment */

/** @} */

/*
 * Routines to get/set CSP address in CRB
 */
#ifdef __powerpc64__	/* 64 bit */
static inline struct cop_csb *cop_get_csbaddr(struct cop_crb *crb)
{
	return (struct cop_csb *)((uint64_t)crb->csbp.addr << (uint64_t)4);
}

static inline void cop_set_csbaddr(struct cop_crb *crb, const void *addr)
{
	crb->csbp.addr = (((uint64_t)addr) >> 4);
}

static inline void *cop_get_cpbptr(struct cop_crb *crb)
{
	return (void *)((crb->csbp.addr << 4) + sizeof(struct cop_csb));
}

static inline void *cop_get_sdptr(struct cop_crb *crb)
{
	return (void *)crb->src_dde.data;
}

static inline void *cop_get_tdptr(struct cop_crb *crb)
{
	return (void *)crb->tgt_dde.data;
}

static inline void cop_set_sdptr(struct cop_crb *crb, const void *p)
{
	crb->src_dde.data = (uint64_t)p;
}

static inline void cop_set_tdptr(struct cop_crb *crb, const void *p)
{
	crb->tgt_dde.data = (uint64_t)p;
}

#else /* 32 bit */

static inline struct cop_csb *cop_get_csbaddr(struct cop_crb *crb)
{
	return (struct cop_csb *)((uintptr_t)crb->csbp.addr << 4);
}

static inline void cop_set_csbaddr(struct cop_crb *crb, const void *addr)
{
	crb->csbp.addr = (((uint64_t)(uintptr_t)addr) >> 4);
}

static inline void *cop_get_cpbptr(struct cop_crb *crb)
{
	return (void *)(((uintptr_t)crb->csbp.addr << 4) +
			sizeof(struct cop_csb));
}

static inline void *cop_get_sdptr(struct cop_crb *crb)
{
	return (void *)((uintptr_t)crb->src_dde.data);
}

static inline void *cop_get_tdptr(struct cop_crb *crb)
{
	return (void *)((uintptr_t)crb->tgt_dde.data);
}

static inline void cop_set_sdptr(struct cop_crb *crb, const void *p)
{
	crb->src_dde.data = (uint64_t)(uintptr_t)p;
}

static inline void cop_set_tdptr(struct cop_crb *crb, const void *p)
{
	crb->tgt_dde.data = (uint64_t)(uintptr_t)p;
}

#endif /* __powerpc64__ */

struct cop_imq{
	uint32_t	valid:1;
	uint32_t	format:1;
	uint32_t	trigger:4;
	uint32_t	state:2;
	uint32_t	vf_overflow:1;
	uint32_t	vf_code:3;
	uint32_t	_rsv_0:20;
	uint32_t	_rsv_1:8;
	uint32_t	as:1;
	uint32_t	gs:1;
	uint32_t	pid:14;
	uint32_t	lpid:8;
	union cop_csbaddr csbp;
	struct cop_csb	csb;
	struct cop_ccb	ccb;
	uint32_t	_rsv_2[4];
};

struct cop_curb {
	union cop_ccw	ccw;
	uint8_t		_rsv_0[3];
	uint8_t		subfunction;
	union cop_csbaddr csbp;
	uint32_t	lpid;
	uint32_t	pid;
	uint64_t	session_id;
	uint64_t	_rsv_1[2];
	struct cop_ccb	ccb;
	uint8_t		_rsv_2[64];
};

#endif /* _LIB_LIBCOPL_COP_H */
