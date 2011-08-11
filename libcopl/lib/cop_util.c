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
#include <errno.h>
#include <sys/types.h>
#include <sched.h>
#include <utmpx.h>
#include <libcopl/cop.h>
#include <libcopl/cop_util.h>
#include <libcopl/cop_sys.h>

/*
 * coprocessor specific information for instance placement within
 * directive field of CCW.
 */
struct cop_directive_info cop_dir_info[] = {
	{0xffffb001, 1, 0xfffffffe},	/* COP_DECOMP */
	{0xffffb007, 3, 0xfffffff8},	/* COP_SYM_CRYPTO */
	{0xffffb01f, 5, 0xffffffe0},	/* COP_ASYM_CRYPTO */
	{0xffffb001, 1, 0xfffffffe},	/* COP_RNG */
	{0xffffb001, 1, 0xfffffffe},	/* COP_ASYNC_DATA_MOVER */
	{0xffffb001, 1, 0xfffffffe},	/* COP_REGX */
	{0xffffb01f, 5, 0xffffffe0},	/* COP_XML */
};
	

#define MT_INCR_WRAP_TAG (__sync_add_and_fetch(&marker_tag_count, 1) & 0x1FF)
static int marker_tag_count = 0;
/**
 * @brief Update marker trace tag count in the CRB
 * \ingroup cop_crb
 * 
 * This routine will simply update the marker traqce tag count within
 * the marker trace tag field.  This routine should only be used if it
 * is known the the other coprocessor specific bits of the marker trace
 * tag field are valid.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @return none - void
 */
void cop_crb_marker_trace_cnt(struct cop_crb *crb)
{
	crb->mt_tag = MT_INCR_WRAP_TAG | (crb->mt_tag & 0x200);
	crb->csbp.marked = 1;
}


/**
 * @brief Set the marker trace tag in the CRB
 * \ingroup cop_crb
 * 
 * This routine will set the marker trace tag information in the passed
 * @a crb argument.  The marker trace tags are specific to the coprocessor
 * type which can be determined by the passed @a ctx argument.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @param ctx Pointer to previously populated cop_ctx structure
 * @return none - void
 */
void cop_crb_set_marker_trace(struct cop_crb *crb, struct cop_ctx *ctx)
{
	crb->mt_tag = MT_INCR_WRAP_TAG |
			(cop_mt_devpbicid[ctx->copid] & 0x200);
	crb->csbp.marked = 1;
}


/**
 * @brief Clear the marker trace tag in the CRB
 * \ingroup cop_crb
 * 
 * This routine will simply clear (zero) the marker trace bit
 * in the passed @a crb argument.
 *
 * @param crb Pointer to a previously populated CRB structure
 * @return none - void
 */
void cop_crb_clear_marker_trace(struct cop_crb *crb)
{
	crb->csbp.marked = 0;
}


static int cop_chip_version = -1;
#define COP_GET_COMPATIBLE_BUF_SIZE 512
/**
 * @brief Get the version of the WSP EN processor chip.
 * \ingroup cop_sys
 *
 * This routines returns an identifier for the WSP EN processor chip
 * associated with the open coprocessor specified in @a ctx.  Software
 * may want to know the chip version to take advantage of features
 * only available on specific versions, or to work around errata present
 * on specific versions.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return @c COP_CHIP_UNKNOWN if chip is unknown
 *         @c COP_CHIP_DD1 for DD1 level chip
 *         @c COP_CHIP_DD2 for DD2 level chip
 */
int cop_get_chip_version(struct cop_ctx *ctx)
{
	char buf[COP_GET_COMPATIBLE_BUF_SIZE];
	int rc;

	if (cop_chip_version != -1)
		return cop_chip_version;

	rc = cop_get_compatible(ctx, buf, COP_GET_COMPATIBLE_BUF_SIZE);
	if (rc < 0)
		return cop_chip_version; /* still -1 */
	
	/*
	 * NOTE: buf will actually contain a list of strings upon
	 * return.  the total length of all strings is rc.
	 * The code below only looks at the FIRST string.
	 */

	/* assume version 2 chip */
	cop_chip_version = 2;
/*
 * Hardcoded until firmware is updated
 */
#if 0
	if (strstr(buf, "1.0.0"))
		cop_chip_version = 1;
#endif
	
	return cop_chip_version;
}

/*
 * Check all coproc instances for a match instead of quitting after first
 * match. This catches multiple matches which should not happen. Do not
 * define in 'production code'.
 *
 * #define DGB_CHECK_ALL_INST 
 */ 

static int cop_open_bind_common(struct cop_ctx *ctx, int copid,
				const char *fn, int local)
{
	cpu_set_t cop_mask;
	int cpu;
	uint64_t instances[4];
	int rc, i, ni;
#ifdef DGB_CHECK_ALL_INST
	int saved_i;
#endif

	/*
	 * open and get instances
	 */
	if ((rc = cop_open(ctx, copid)))
		return rc;

	if ((ni = cop_get_instances(ctx, instances, sizeof(instances))) < 1) {
		cop_close(ctx);
		return -1;
	}
	
	/*
	 * Quick bind/return if only one instance found
	 */
	if (ni == 1) {
		i = 0;	/* bind to zero'th (only) instance */
		goto cobl_bind_now;
	}

	/*
	 * Get current CPU.  If thread is not affinity'ed to CPU, this
	 * value could be invalid/wrond before returning from sched_getcpu().
	 */
	cpu = sched_getcpu();
	if (cpu < 0) {
		fprintf(stderr, "%s: Warning sched_getcpu() error,"
			" binding to first instance of %s coprocessor\n",
			fn, cop_dev_name[copid]); 
		i = 0;	/* bind to zero'th instance */
		goto cobl_bind_now;
	}

	/*
	 * For each instance, get affinity mask and look for current
	 * CPU in mask.  A match indicates 'local'.
	 */
#ifdef DGB_CHECK_ALL_INST
	saved_i = 0;
#endif
	for (i = 0; i < ni; i++) {
		if ((rc = cop_get_instance_affinity(ctx, instances[i],
					sizeof(cpu_set_t), &cop_mask))) {
			cop_close(ctx);
			return rc;
		}


#ifdef DGB_CHECK_ALL_INST
		if (local) {
			if (CPU_ISSET(cpu, &cop_mask)) {
				if (saved_i) {
					fprintf(stderr,
						"%s: Warning current cpu %d matches multiple affinity masks of %s coprocessor\n",
						fn, (int)cpu,
						cop_dev_name[copid]); 
					fprintf(stderr,
						" Binding to first match.\n");
				} else
					saved_i = i+1;
			}
		} else { /* remote */
			if (!CPU_ISSET(cpu, &cop_mask)) {
				if (saved_i) {
					fprintf(stderr,
						"%s: Warning current cpu %d matches multiple affinity masks of %s coprocessor\n",
						fn, (int)cpu,
						cop_dev_name[copid]); 
					fprintf(stderr,
						" Binding to first match.\n");
				} else
					saved_i = i+1;
					saved_i = i+1;
			}
		}
	}

	if (!saved_i) {
		fprintf(stderr, "%s: Warning current CPU '%d' not found "
			"in any coprocessor affinity mask, binding to "
			"first instance of %s coprocessor\n",
			fn, cpu, cop_dev_name[copid]); 
		i = 0;	/* default to zero'th entry */
	} else {
  		i = saved_i-1;
	}
#else /* ! DGB_CHECK_ALL_INST */
		if (local) {
			if (CPU_ISSET(cpu, &cop_mask))
				break; /* coprocessor local for this cpu */
		} else {
			if (!CPU_ISSET(cpu, &cop_mask))
				break; /* coprocessor remote to this cpu */
		}
	}

	if (i == ni) {
		fprintf(stderr, "%s: Warning current CPU '%d' not found "
			"in any coprocessor affinity mask, binding to "
			"first instance of %s coprocessor\n",
			fn, cpu, cop_dev_name[copid]); 
		i = 0;	/* default to zero'th entry */
	}
#endif

cobl_bind_now:
	if ((rc = cop_bind_instance(ctx, instances[i]))) {
		cop_close(ctx);
		return rc;
	}

	return rc;
}


/**
 *  * \defgroup cop_sys_util Utility functions built on System/kernel interfaces
 *   */
/**
 * @brief Open coprocessor and bind to local instance
 * \ingroup cop_sys_util
 *
 * This routine will open the coprocessor specified by @a copid.  It will
 * examine all instances of the coprocessor found in the system to determine
 * which is 'local' to the current CPU on which thread is running.  The
 * cop_ctx structure @a ctx will then be bound to this local instance.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return Zero on success, negative value on failure
 */
int cop_open_bind_local(struct cop_ctx *ctx, int copid)
{
	return cop_open_bind_common(ctx, copid, __func__, 1);
}


/**
 * @brief Open coprocessor and bind to remote instance
 * \ingroup cop_sys_util
 *
 * This routine will open the coprocessor specified by @a copid.  It will
 * examine all instances of the coprocessor found in the system to determine
 * which is not 'local' to the current CPU on which thread is running.  The
 * cop_ctx structure @a ctx will then be bound to this non-local instance.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return Zero on success, negative value on failure
 */
int cop_open_bind_remote(struct cop_ctx *ctx, int copid)
{
	return cop_open_bind_common(ctx, copid, __func__, 0);
}
