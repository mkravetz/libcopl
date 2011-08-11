/* 
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 *  Copyright 2008,2009, 2010
 *  Chris J Arges
 *  Everton Constantino
 *  Tom Gall
 *  Michael Ellerman
 *  IBM Corp, All Rights Reserved
 *                      
 *  US Government Users Restricted Rights - Use Duplication or
 *  disclosure restricted by GSA ADP Schedule Contract with IBM
 *  Corp 
 *                                
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libcopl/cop_sys.h"
#include <sys/mman.h>
#include "asm/copro-driver.h"
#include "asm/copro-regx.h"

#include <sched.h>
#include <sys/ioctl.h>

/**
 * \defgroup cop_sys System/kernel coprocessor interfaces
 */

char *cop_dev_node[] = {
	"/dev/cop_type/cmpx",	/* COP_DECOMP */
	"/dev/cop_type/sacx",	/* COP_SYM_CRYPTO */
	"/dev/cop_type/aacx",	/* COP_ASYM_CRYPTO */
	"/dev/cop_type/rngx",	/* COP_RNG */
	"/dev/cop_type/admx",	/* COP_ASYNC_DATA_MOVER */
	"/dev/cop_type/regx",	/* COP_REGX */
	"/dev/cop_type/xmlx",	/* COP_XML */
};

char *cop_dev_name[] = {
	"cmpx",
	"sacx ",
	"aacx",
	"rngx",
	"admx",
	"regx",
	"xmlx",
};

/*
 * coprocessor IDs for use by Marker Trace.  MUST correspond (same order)
 * as device names in cop_dev_name[] above.
 */
unsigned short cop_mt_devpbicid[] = {0x000, 0x200, 0x200, 0x200,
					0x200, 0x000, 0x200,};

/*
 * Internal helper routine to call copro ioctl() and return errno in case
 * of error.  Should not be caled directly from application code.
 */
static int _copro_ioctl(int fd, int num, void *arg)
{
	int rc;

	rc = ioctl(fd, num, arg);
	if (rc == -1)
		return -errno;
	
	return rc;
}


/**
 * @brief Open a coprocessor (specifying device name) for subsequent operations.
 *
 * This routine opens the coprocessor specified by the device name @a dev_name.
 * Flags specified by the @a flags parameter will be passed to the open system
 * call.  If the open of the coprocessor is successful, the context structure
 * pointed to by @a ctx will be populated with information.  Returns zero
 * if successful, non-zero value otherwise.
 *
 * @param ctx  Pointer to cop_ctx structure to be populated upon success
 * @param dev_name Fully qualified device name of coprocessor
 * @param flags Flags to be passed to the open system call
 * @return     0 on success, non-zero value on failure
 */
static int cop_open_dev(struct cop_ctx *ctx, char *dev_name, int flags)
{
	int rc, fd;
	uint64_t type;

	fd = open(dev_name, flags);
	if (fd < 0)
		return fd;

	rc = _copro_ioctl(fd, COPRO_IOCTL_GET_TYPE, &type);
	if (rc) {
		close(fd);
		return rc;
	}

	ctx->fd = fd;
	ctx->instance = COPRO_INSTANCE_INVALID;
	ctx->type = type;
	ctx->copid = -1; /* if called by cop_open() will be overwritten */

	return rc; /* rc == 0 */
}


/**
 * @brief Open a coprocessor for subsequent operations.
 * \ingroup cop_sys
 *
 * This routine opens the coprocessor specified by the @a copid identifier.  
 * If the open of the coprocessor is successful, the context structure
 * pointed to by @a ctx will be populated with information.  Returns zero
 * if successful, non-zero value otherwise.
 *
 * @param ctx  Pointer to cop_ctx structure to be populated upon success
 * @param copid Coprocessor identifier value
 * @return     0 on success, non-zero value on failure
 */
int cop_open(struct cop_ctx *ctx, int copid)
{
	int rc;

	if (copid < 0 || copid >= COP_NUMBER_KNOWN_COPS)
		return -EINVAL;

	rc = cop_open_dev(ctx, cop_dev_node[copid], O_RDONLY);
	if (!rc)
		ctx->copid = copid;

	return rc;
}


/**
 * @brief Close a coprocessor
 * \ingroup cop_sys
 *
 * This routine closes a coprocessor that was previously opened with a
 * @c cop_open() call.  The only parameter is a context structure @a ctx
 * populated by a successful invocation of @c cop_open().
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return     0 on success, non-zero value on failure
 */
int cop_close(struct cop_ctx *ctx)
{
	/* we could 'unbind', but unnecessary */
	return close(ctx->fd);
}


/**
 * @brief Get the version of the coprocessor API in use by the kernel driver
 * \ingroup cop_sys
 *
 * This routine returns the integer value representing the API in use by
 * the kernel driver associated with the coprocessor specified in @a ctx.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return     Positive integer representing API version on success,
 * 	       non-zero value on failure
 */
int cop_get_api_version(struct cop_ctx *ctx)
{
	return _copro_ioctl(ctx->fd, COPRO_IOCTL_GET_API_VERSION, NULL);
}


/**
 * @brief Return the compatibility strings of the coprocessor
 * \ingroup cop_sys
 *
 * This routine fills in the character array pointed to by @a info with
 * the compatibility information for the processor.  The compatibility
 * can be used to identify specific levels of hardware for compatibility
 * purposes.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param info Pointer to character array (buffer)
 * @param len  Size (in bytes) of info array
 * @return     number of bytes placed in passed buffer on success,
 *	       negative value on failure
 */
int cop_get_compatible(struct cop_ctx *ctx, char *info, size_t len)
{
	struct copro_compat_info *cci;
	int rc;

	cci = malloc(sizeof(struct copro_compat_info));
	if (!cci)
		return -errno;

	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_GET_COMPATIBLE, cci);
	if (rc) {
		free(cci);
		return rc;
	}

	if (len < cci->len) {
		free(cci);
		return -ENOMEM;		/* passed buffer too small */
	}

	rc = cci->len;
	memcpy(info, cci->buf, rc);
	free(cci);

	return rc;
}


/**
 * @brief Get all instances associated with the coprocessor type
 * \ingroup cop_sys
 *
 * This routines returns the number of instances of a specific coprocessor
 * type.  If a buffer is passed to the routine, the instance identifiers
 * (@p len bytes of identifiers) will be written to the passed buffer.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param instances Pointer to an array of (uint64_t sized) instances
 * @param len  Size (in bytes) of of instances array
 * @return     Number of instances (positive int) on success,
 *             negative value on failure
 */
int cop_get_instances(struct cop_ctx *ctx, uint64_t *instances, size_t len)
{
	struct copro_instance_list *cil;
	int rc;
	int total_instances = 0;

	cil = malloc(sizeof(struct copro_instance_list));
	if (!cil)
		return -errno;

	do {
		if ((rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_GET_INSTANCES, cil))){
			free(cil);
			return rc;
		}

		/* count number of instances (in this buffer) */
		for (rc = 0; rc < COPRO_INSTANCE_LIST_SIZE; rc++) {
			if (cil->instances[rc] == COPRO_INSTANCE_INVALID) {
				break;
			}

			/* put as many instances in buffer as will fit */
			if (instances && (len > sizeof(uint64_t))) {
				*instances++ = cil->instances[rc];
				len -= sizeof(uint64_t);
			}
		}

		total_instances += rc;

	} while (rc == COPRO_INSTANCE_LIST_SIZE);

	free(cil);
	return total_instances;
}


/**
 * @brief Get the affinity associated with a coprocessor instance
 * \ingroup cop_sys
 *
 * This routines returns affinity information for a specific coprocessor
 * instance.  The affinity information identifies the preferred set of
 * CPUs for a thread that is making use of the coprocessor instance.  The
 * returned @a mask can be used as input to the @c sched_setaffinity()
 * system call. 
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param instance Coprocessor instance from cop_get_instances() call
 * @param cpusetsize Size (in bytes) of cpu_set_t pointed to by mask
 * @param mask Pointer to Mask representing preferred CPU set for coprocessor
 * @return     Zero on success, negative value on failure
 */
int cop_get_instance_affinity(struct cop_ctx *ctx, uint64_t instance,
					size_t cpusetsize, cpu_set_t *mask)
{
	struct copro_affinity_args args;
	int rc;

	/*
	 * In 'compatibility mode' the ioctl will only write to a subset
	 * of the mask.  For 'convenience' we clear the entire for the
	 * user.
	 */
	CPU_ZERO_S(cpusetsize, mask);

	args.instance = instance;
	args.addr = (uint64_t)(uintptr_t)mask;
	args.len = cpusetsize;

	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_GET_AFFINITY, &args);

	if (rc > 0)	/* positive value indicates size written, success */
		rc = 0;

	return rc;
}


/**
 * @brief Bind the open coprocessor to a specific instance
 * \ingroup cop_sys
 *
 * This routines binds an open coprocessor context spedified by @a ctx to
 * a specific instance of the coprocessor.  This binding can be useful in
 * subsequent PBIC mapping operations and coprocessor invocation.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param instance Coprocessor instance from cop_get_instances() call
 * @return     Zero on success, negative value on failure
 */
int cop_bind_instance(struct cop_ctx *ctx, uint64_t instance)
{
	int rc;

	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_BIND, &instance);
	if (rc)
		return rc;

	ctx->instance = instance;
	return rc; /* rc == 0 */
}


/**
 * @brief Bind the open coprocessor to a specific instance
 * \ingroup cop_sys
 *
 * This routines removes the binding of a open coprocessor context to a
 * specific context.  The coprocessor comtext specified by @a ctx should
 * have been bound to a specific instance.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure that is bound
 *             to a specific instance
 * @return     Zero on success, negative value on failure
 */
int cop_unbind_instance(struct cop_ctx *ctx)
{
	int rc;

	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_UNBIND, NULL);
	if (rc)
		return rc;

	ctx->instance = COPRO_INSTANCE_INVALID;

	return rc;
}


#define _UP_LEN(l, p)	( ((l) + (p) - 1) & ~((p) - 1) )
#define _DOWN_ADD(a, p)	( (a) & ~((p) - 1) )

/**
 * @brief Internal routine to Map multiple address ranges in PBIC
 *
 * This routine is used internally by @c libcopl to map multiple address
 * ranges in a PBIC.  It takes as an argument a data structures used by
 * the kernel driver.  Not directly callable from user application code.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param arg  Pointer to populated copro_map_args structure
 * @return     Zero on success, negative value on failure
 */
static int pbic_map_list(struct cop_ctx *ctx, struct copro_map_args *arg)
{
	int i = 0, j, rc;
	uint64_t s_flags = arg->flags;
	int psize = getpagesize();

	/*
	 * check for flags we handle here
	 */
	if (arg->flags & (COP_MAP_MLOCK | COP_MAP_MADV_DONTFORK)) {
		for (i = 0; i < arg->count; i++) {
			if (arg->flags & COP_MAP_MLOCK) {
#ifdef __powerpc64__
				rc = mlock((void *)arg->entries[i].addr,
#else
				rc = mlock((void *)((uintptr_t)
						arg->entries[i].addr),
#endif
						arg->entries[i].len);
				if (rc) {
					rc = -errno;
					goto pml_out_error_mlock;
				}
			}
			if (arg->flags & COP_MAP_MADV_DONTFORK) {
#ifdef __powerpc64__
				rc = madvise((void *)_DOWN_ADD(
						  arg->entries[i].addr,
						  psize),
#else
				rc = madvise((void *)_DOWN_ADD((uintptr_t)
						  arg->entries[i].addr,
						  psize),
#endif
						_UP_LEN(arg->entries[i].len,
						  psize),
						MADV_DONTFORK);
				if (rc) {
					rc = -errno;
					goto pml_out_error_madvise;
				}
			}
		}
	}

	if (arg->flags & COP_MAP_BOLT)
		arg->flags = COPRO_MAP_BOLT;
	else
		arg->flags = 0;
	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_MAP, arg);
	arg->flags = s_flags;

	if (rc)
		goto pml_out_error_madvise;

	return rc;

	/*
	 * i indicates where we were in the list of addresses/len pairs
	 * need to undo all the mlock's/madvise's done above
	 */
pml_out_error_madvise:
	if (arg->flags & COP_MAP_MLOCK)
#ifdef __powerpc64__
		munlock((void *)arg->entries[i].addr, arg->entries[i].len);
#else
		munlock((void *)((uintptr_t)arg->entries[i].addr),
			arg->entries[i].len);
#endif
pml_out_error_mlock:
	for (j = 0; j < i; j++) {
		if (arg->flags & COP_MAP_MLOCK)
#ifdef __powerpc64__
			munlock((void *)arg->entries[j].addr,
#else
			munlock((void *)((uintptr_t)arg->entries[j].addr),
#endif
					arg->entries[j].len);
		if (arg->flags & COP_MAP_MADV_DONTFORK)
#ifdef __powerpc64__
			madvise((void *)_DOWN_ADD(arg->entries[j].addr, psize),
#else
			madvise((void *)_DOWN_ADD(((uintptr_t)
					arg->entries[j].addr), psize),
#endif
					_UP_LEN(arg->entries[j].len, psize),
					MADV_DOFORK);
	}

	return rc;
}


/**
 * @brief Internal routine to Unmap multiple address ranges in PBIC
 *
 * This routine is used internally by @c libcopl to unmap multiple address
 * ranges from a PBIC.  It takes as an argument a data structures used by
 * the kernel driver.  Not directly callable from user application code.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param arg  Pointer to populated copro_map_args structure
 * @return     Zero on success, negative value on failure
 */
static int pbic_unmap_list(struct cop_ctx *ctx, struct copro_map_args *arg)
{
	int i, rc, rc2;
	uint64_t s_flags = arg->flags;
	int psize = getpagesize();

	if (arg->flags & COP_MAP_BOLT)
		arg->flags = COPRO_MAP_BOLT;
	else
		arg->flags = 0;
	rc = _copro_ioctl(ctx->fd, COPRO_IOCTL_UNMAP, arg);
	s_flags = arg->flags;
	if (rc)
		return rc;

	/*
	 * check for flags we handle here.  after successfully
	 * unbloting/unmapping above, do not abort the call or
	 * try to 'undo the unmap' if munloc or madvise fail.
	 * rather, keep track of any errors encountered and
	 * return to the user so they know 'something' went wrong.
	 */
	if (arg->flags & (COP_MAP_MLOCK | COP_MAP_MADV_DONTFORK)) {
		for (i = 0; i < arg->count; i++) {
			if (arg->flags & COP_MAP_MLOCK) {
#ifdef __powerpc64__
				rc2 = munlock((void *)arg->entries[i].addr,
#else
				rc2 = munlock((void *)((uintptr_t)
						arg->entries[i].addr),
#endif
						arg->entries[i].len);
				if (rc2 && !rc)
					rc = rc2;
			}
			if (arg->flags & COP_MAP_MADV_DONTFORK) {
#ifdef __powerpc64__
				rc2 = madvise((void *)_DOWN_ADD(
						  arg->entries[i].addr,
#else
				rc2 = madvise((void *)_DOWN_ADD(((uintptr_t)
						  arg->entries[i].addr),
#endif
						  psize),
						_UP_LEN(arg->entries[i].len,
						  psize),
						MADV_DOFORK);
				if (rc2 && !rc)
					rc = rc2;
			}
		}
	}

	return rc;
}


/**
 * @brief Map multiple address ranges in PBIC associated with a coprocessor
 * \ingroup cop_sys
 *
 * This routines maps multiple address ranges in a process' address space
 * in a PBIC associated with the coprocessor identified by @a ctx.
 * Memory needs to be mapped in the PBIC so that it can be accessed by the
 * the coprocessor.  The parameter @a flags can be used to specify special
 * types of mappings.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param p    Pointer to an array of addresses is process address space
 * @param len  Pointer to an array of address range lengths pointed to by @a p
 * @param num  Number of address ranges described by @a p and @a len
 * @param flags Flags to specify mapping options
 *              COP_MAP_BOLT - Bolt address range in PBIC
 *              COP_MAP_MLOCK - mlock address range
 *              COP_MAP_MADV_DONTFORK - pages not available in child at fork
 * @return     Zero on success, negative value on failure
 */
int cop_pbic_map_list(struct cop_ctx *ctx, const void **p, size_t *len,
					int num, uint64_t flags)
{
	int i;
	struct copro_map_args arg;

	if (num > COPRO_MAP_MAX_COUNT)
		return -EINVAL;

	bzero(&arg, sizeof (arg));
	for (i = 0; i < num; i++) {
		arg.entries[i].addr = (uint64_t)(uintptr_t)*p++;
		arg.entries[0].len = (uint64_t)(uintptr_t)*len++;
	}
	arg.flags = flags;
	arg.count = (uint64_t)num;

	return (pbic_map_list(ctx, &arg));
}


/**
 * @brief Unmap multiple address ranges from PBIC associated with a coprocessor
 * \ingroup cop_sys
 *
 * This routines unmaps multiple address ranges in a process' address space
 * from a PBIC associated with the coprocessor identified by @a ctx.  These
 * address ranges should have been previously mapped via the
 * @c cop_pbic_map_list() routine.  Memory mapped with the @c COP_MAP_BOLT
 * flag should be unmapped when it will not be used.  The parameter @a
 * flags should match those specified in the previous @c cop_pbic_map_list()
 * operation.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param p    Pointer to an array of addresses is process address space
 * @param len  Pointer to an array of address range lengths pointed to by @a p
 * @param num  Number of address ranges described by @a p and @a len
 * @param flags Flags to specify unmapping options
 *              COP_MAP_BOLT - Bolt address range in PBIC
 *              COP_MAP_MLOCK - mlock address range
 *              COP_MAP_MADV_DONTFORK - pages not available in child at fork
 * @return     Zero on success, negative value on failure
 */
int cop_pbic_unmap_list(struct cop_ctx *ctx, const void **p, size_t *len,
					int num, uint64_t flags)
{
	int i;
	struct copro_map_args arg;

	if (num > COPRO_MAP_MAX_COUNT)
		return -EINVAL;

	bzero(&arg, sizeof (arg));
	for (i = 0; i < num; i++) {
		arg.entries[i].addr = (uint64_t)(uintptr_t)*p++;
		arg.entries[0].len = (uint64_t)(uintptr_t)*len++;
	}
	arg.flags = flags;
	arg.count = (uint64_t)num;

	return (pbic_unmap_list(ctx, &arg));
}


/**
 * @brief Map address range to PBIC associated with a coprocessor
 * \ingroup cop_sys
 *
 * This routines maps an address range in a process' address space to
 * a PBIC associated with the coprocessor identified by @a ctx.  Memory
 * needs to be mapped in the PBIC so that it can be accessed by the
 * the coprocessor.  The parameter @a flags can be used to specify special
 * types of mappings.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param p    Pointer to memory is process address space
 * @param len  Length of address range pointed to by @a p
 * @param flags Flags to specify mapping options
 *              COP_MAP_BOLT - Bolt address range in PBIC
 *              COP_MAP_MLOCK - mlock address range
 *              COP_MAP_MADV_DONTFORK - pages not available in child at fork
 * @return     Zero on success, negative value on failure
 */
int cop_pbic_map(struct cop_ctx *ctx, const void *p, size_t len, uint64_t flags)
{
	struct copro_map_args arg;

	bzero(&arg, sizeof (arg));
	arg.count = (uint64_t)1;
	arg.flags = flags;
	arg.entries[0].addr = (uint64_t)(uintptr_t)p;
	arg.entries[0].len = (uint64_t)(uintptr_t)len;

	return (pbic_map_list(ctx, &arg));
}


/**
 * @brief Unmap address range from PBIC associated with a coprocessor
 * \ingroup cop_sys
 *
 * This routines unmaps an address range in a process' address space from
 * a PBIC associated with the coprocessor identified by @a ctx.  This addres
 * range should have been previously mapped via the @c cop_pbic_map() routine.
 * Memory mapped with the @c COP_MAP_BOLT flag should be unmapped when it
 * will not be used.  The parameter @a flags should match those specified in
 * the previous @c cop_pbic_map() operation.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param p    Pointer to memory is process address space
 * @param len  Length of address range pointed to by @a p
 * @param flags Flags to specify mapping options
 *              COP_MAP_BOLT - Bolt address range in PBIC
 *              COP_MAP_MLOCK - mlock address range
 *              COP_MAP_MADV_DONTFORK - pages not available in child at fork
 * @return     Zero on success, negative value on failure
 */
int cop_pbic_unmap(struct cop_ctx *ctx, const void *p, size_t len,
			uint64_t flags)
{
	struct copro_map_args arg;

	bzero(&arg, sizeof (arg));

	arg.count = (uint64_t)1;
	arg.flags = flags;
	arg.entries[0].addr = (uint64_t)(uintptr_t)p;
	arg.entries[0].len = (uint64_t)(uintptr_t)len;

	return (pbic_unmap_list(ctx, &arg));
}


#ifdef DD2_CHIP_ERRATA
/**
 * @brief Invalidate entries for page in PBIC
 * \ingroup cop_sys
 *
 * Routine provided as part of a work around for chip erratum dealing
 * with duplicate PBIC entries.  This routine is a simple interface to
 * the kernel routine which will flush the entries.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @param p    Pointer to memory is process address space
 * @return     Zero on success, negative value on failure
 * 	       -ENOSYS returned if support is not in kernel.
 */
int cop_pbic_invalidate(struct cop_ctx *ctx, const void *p)
{
#ifdef COPRO_IOCTL_INVALIDATE
	uint64_t addr = (uint64_t)(uintptr_t)p;

	return _copro_ioctl(ctx->fd, COPRO_IOCTL_INVALIDATE, (void *)&addr);
#else
	return -ENOSYS;
#endif
}
#endif


/**
 * @brief Open unit that houses the specified coprocessor
 * \ingroup cop_sys
 *
 * This routine opens the unit that houses the coprocessor specified by
 * @a ctx.  The @c unit would be opened so that one could subsequently
 * get and set unit specific registers.
 *
 * @param ctx  Pointer to previously populated cop_ctx structure
 * @return     Zero on success, negative value on failure
 */
int cop_open_unit(struct cop_ctx *ctx)
{
	return _copro_ioctl(ctx->fd, COPRO_IOCTL_OPEN_UNIT, NULL);
}


/**
 * @brief Request Upload Manager privilege on regex coproc unit
 * \ingroup cop_sys
 *
 * This routine requests Upload Manager privilege on the unit that houses
 * the @c regex coprocessor.  @a unitfd is a unit descriptor that was
 * returned from a successful call to @c cop_open_unit() for the regex
 * coprocessor.
 *
 * @param unitfd  Descriptor returned from a previous @c cop_open_unit() call
 * @return     Zero on success, negative value on failure
 */
int cop_request_um_priv(int unitfd)
{
	return _copro_ioctl(unitfd, REGX_IOCTL_REQUEST_UM_PRIV, NULL);
}


/**
 * @brief Drop Upload Manager privilege on regex coproc unit
 * \ingroup cop_sys
 *
 * This routine drops Upload Manager privilege on the unit that houses
 * the @c regex coprocessor when it is no longer needed.  @a unitfd is
 * a unit descriptor that was used in a previously successful call to
 * @c cop_request_um_priv().
 *
 * @param unitfd  Descriptor used in a previous @c cop_request_um_priv() call
 * @return     Zero on success, negative value on failure
 */
int cop_drop_um_priv(int unitfd)
{
	return _copro_ioctl(unitfd, REGX_IOCTL_DROP_UM_PRIV, NULL);
}


/**
 * @brief Get value in specic register of unit
 * \ingroup cop_sys
 *
 * This routine will get the value of the register specified by @a reg of the
 * unit specified by @a unitfd.  @a unitfd is value returned from a successful
 * @c cop_open_unit() call.  The value of the register is returned in the
 * @c uint64_t size area pointed to by @a value.
 *
 * @param unitfd Descriptor identifying the unit
 * @param reg Register identifier
 * @param value Pointer to area where register value will be stored
 * @return     Zero on success, negative value on failure
 */
int cop_get_unit_reg(int unitfd, uint64_t reg, uint64_t *value)
{
	struct copro_reg_args args;
	int rc;

	args.regnr = reg;
	rc = _copro_ioctl(unitfd, COPRO_UNIT_IOCTL_GET_REG, &args);
	if (rc == -1)
		return -errno;

	*value = args.value;
	return rc;
}


/**
 * @brief Set value in specic register of unit
 * \ingroup cop_sys
 *
 * This routine will set the value of the register specified by @a reg of the
 * unit specified by @a unitfd.  @a unitfd is value returned from a successful
 * @c cop_open_unit() call.  The value of the register is set to the value
 * contained in the @a value parameter.
 *
 * @param unitfd Descriptor identifying the unit
 * @param reg Register identifier
 * @param value Value to be stored in register
 * @return     Zero on success, negative value on failure
 */
int cop_set_unit_reg(int unitfd, uint64_t reg, uint64_t value)
{
	struct copro_reg_args args;
	int rc;

	args.regnr = reg;
        args.value = value;
	rc = _copro_ioctl(unitfd, COPRO_UNIT_IOCTL_SET_REG, &args);
	if (rc == -1)
		return -errno;

	return 0;
}

int cop_alloc_imq(struct cop_ctx *ctx, int nr_imqe)
{
        uint64_t size = nr_imqe;

        return _copro_ioctl(ctx->fd, COPRO_IOCTL_ALLOC_IMQ, &size);
}

int cop_free_imq(struct cop_ctx *ctx)
{
        return _copro_ioctl(ctx->fd, COPRO_IOCTL_FREE_IMQ, NULL);
}

int cop_enable_imq(struct cop_ctx *ctx)
{
        return _copro_ioctl(ctx->fd, COPRO_IOCTL_ENABLE_IMQ, NULL);
}

int cop_disable_imq(struct cop_ctx *ctx)
{
        return _copro_ioctl(ctx->fd, COPRO_IOCTL_DISABLE_IMQ, NULL);
}
