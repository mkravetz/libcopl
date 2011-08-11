/*  
   Licensed Materials - Property of IBM
   Restricted Materials of IBM"
 
   Copyright 2010
   Chris J Arges
   Everton Constantino
   Mike Kravetz
   Tom Gall
   IBM Corp, All Rights Reserved
 
   US Government Users Restricted Rights - Use Duplication or
   disclosure restricted by GSA ADP Schedule Contract with IBM
   Corp

*/


#ifndef _LIB_LIBCOPL_COP_SYS_H
#define _LIB_LIBCOPL_COP_SYS_H

#include <stdint.h>
#include <sys/types.h>
#include <sched.h>
 
/**
 * Coprocessor identifiers (copid) passed to cop_open()
 * \ingroup cop_sys
 * @{
 */
#define COP_NUMBER_KNOWN_COPS 7
#define COP_DECOMP 0		/* /dev/cop_type/cmpx */ 
#define COP_SYM_CRYPTO 1	/* /dev/cop_type/sacx */
#define COP_ASYM_CRYPTO 2	/* /dev/cop_type/aacx */
#define COP_RNG 3		/* /dev/cop_type/rngx */
#define COP_ASYNC_DATA_MOVER 4	/* /dev/cop_type/admx */
#define COP_REGX 5		/* /dev/cop_type/regx */
#define COP_XML 6		/* /dev/cop_type/xmlx */
/* @} */
extern char *cop_dev_node[];
extern char *cop_dev_name[];
extern unsigned short cop_mt_devpbicid[];

/*
 * Interface paramater definitions
 */
struct cop_ctx {
	int fd;
	int copid;
	uint64_t instance;
	uint64_t type;
};

#define COP_CTX_FD(ctx)		((int)(ctx)->fd)
#define COP_CTX_TYPE(ctx)	((int)(ctx)->type)

/* cop_pbic_map* flags */
#define COP_MAP_BOLT		0x00001
#define COP_MAP_MLOCK		0x00002
#define COP_MAP_MADV_DONTFORK	0x00004
#define COP_MAP_ALL_FLAGS	(COP_MAP_BOLT | COP_MAP_MLOCK | \
				 COP_MAP_MADV_DONTFORK)

/*
 * coprocessor interfaces
 */
extern int cop_open(struct cop_ctx *ctx, int copid);
extern int cop_close(struct cop_ctx *ctx);
extern int cop_get_api_version(struct cop_ctx *ctx);
extern int cop_get_compatible(struct cop_ctx *ctx, char *info, size_t len);
extern int cop_get_instances(struct cop_ctx *ctx, uint64_t *instances,
				size_t len);
extern int cop_get_instance_affinity(struct cop_ctx *ctx, uint64_t instance,
					size_t cpusetsize, cpu_set_t *mask);
extern int cop_bind_instance(struct cop_ctx *ctx, uint64_t instance);
extern int cop_unbind_instance(struct cop_ctx *ctx);
extern int cop_pbic_map(struct cop_ctx *ctx, const void *p, size_t len,
			uint64_t flags);
extern int cop_pbic_unmap(struct cop_ctx *ctx, const void *p, size_t len,
			uint64_t flags);
extern int cop_pbic_map_list(struct cop_ctx *ctx, const void **p, size_t *len,
				int num, uint64_t flags);
extern int cop_pbic_unmap_list(struct cop_ctx *ctx, const void **p, size_t *len,
				int num, uint64_t flags);
extern int cop_pbic_invalidate(struct cop_ctx *ctx, const void *p);
extern int cop_open_unit(struct cop_ctx *ctx);
extern int cop_request_um_priv(int unitfd);
extern int cop_drop_um_priv(int unitfd);
extern int cop_get_unit_reg(int unitfd, uint64_t reg, uint64_t *value);
extern int cop_set_unit_reg(int unitfd, uint64_t reg, uint64_t value);
extern int cop_alloc_imq(struct cop_ctx *ctx, int nr_imqe);
extern int cop_free_imq(struct cop_ctx *ctx);
extern int cop_enable_imq(struct cop_ctx *ctx);
extern int cop_disable_imq(struct cop_ctx *ctx);

#endif	/* _LIB_LIBCOPL_COP_SYS_H */
