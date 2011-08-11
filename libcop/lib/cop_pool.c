/*  
 *
 *  Licensed Materials - Property of IBM
 *  Restricted Materials of IBM"
 *      
 *  Copyright 2010
 *  Chris J Arges
 *  Everton Constantino
 *  Tom Gall
 *  IBM Corp, All Rights Reserved
 *                      
 *  US Government Users Restricted Rights - Use Duplication or
 *  disclosure restricted by GSA ADP Schedule Contract with IBM
 *  Corp
 *
 **/

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "config.h"

#include "deq.h"
#include "cop_map_tree.h"

#include "libcopmanage.h"
#include "libcopmalloc.h"
#include "libcopl/cop_sys.h"

#define COP_DEV_PATH "/dev/cop_type/"

unsigned int cop_process_init_complete = 0;

extern int gDD_version;
int gVerbose;

// from libcopmanage
extern unsigned int COP_NUMBER_COPS;
extern struct cop_pbic_info cop_pbic_info_array[COP_NUMBER_KNOWN_PBICS];

static mapped_memory_pbic_root *mpr_rx_pbic = NULL;
static mapped_memory_pbic_root *mpr_cc_pbic = NULL;

void cop_init_global_mempool(void);
void __attribute__ ((constructor)) cop_init(void);
void __attribute__ ((destructor)) cop_fini(void);

void cop_fork_child(void);
void cop_fork_parent(void);
void cop_remap_pool(mapped_memory_pool_t pool);
void
cop_init_cc_pbic(mapped_memory_pbic_root * mpr, unsigned long newBlockSize, unsigned long cc_size, int cc_bolt);
void
cop_init_rx_pbic(mapped_memory_pbic_root * mpr, unsigned long newBlockSize, unsigned long rx_size, int rx_bolt);

void read_environment_variables();
void call_cop_get_compatible(int cop);

void __attribute__ ((constructor)) cop_init()
{
	if (!cop_process_init_complete) {
		cop_init_cops();

		/* set default memory pool values */
		unsigned long cc_size = 16 * 1024 * 1024;
		int cc_bolt = 0;
		unsigned long rx_size = 16 * 1024 * 1024;
		int rx_bolt = 0;
		read_environment_variables(&cc_size, &cc_bolt, &rx_size, &rx_bolt);

		mpr_rx_pbic = malloc(sizeof(mapped_memory_pbic_root));
		mpr_cc_pbic = malloc(sizeof(mapped_memory_pbic_root));

		if (COP_NUMBER_COPS) {
			/* check for compatible */
			call_cop_get_compatible(COP_SYM_CRYPTO);

			cop_init_rx_pbic(mpr_rx_pbic, pool_block_Size64K, rx_size, rx_bolt);
			cop_init_cc_pbic(mpr_cc_pbic, pool_block_Size64K, cc_size, cc_bolt);

			/* register fork handler */
			pthread_atfork(NULL, cop_fork_parent, cop_fork_child);
		}
		cop_process_init_complete = 1;
	}
}

void __attribute__ ((destructor)) cop_fini()
{
	mapped_memory_pool_t cc_pool = cop_cc_pool();
	mapped_memory_pool_t rx_pool = cop_rx_pool();

	// tear down default memory pool
	cop_free_pool(cc_pool);
	cop_free_pool(rx_pool);
}

mapped_memory_pool_t cop_rx_pool()
{
	return mpr_rx_pbic;
}

mapped_memory_pool_t cop_cc_pool()
{
	return mpr_cc_pbic;
}

void read_environment_variables(unsigned long *CC_size, int *CC_bolt, unsigned long *RX_size, int *RX_bolt) {
	char *value;

#ifdef VERBOSE_ON
	/* check for verbose commandline options */
	if ((value = getenv("V"))) {
		switch(atoi(value)) {
			case 1:
				printf("Turning debug printing on.\n");
				gVerbose = 1;
				break;
			default:
				printf("invalid debug level, use 1 or 2\n");
		}
	}
#endif

	/* check for memory pool options */
	if ((value = getenv("CCBOLT"))) {
		if (atoi(value) == 1) {
			*CC_bolt = 1;
			printf("Bolting the CC memory pool.\n");
		}
	}
	if ((value = getenv("CCSIZE"))) {
		unsigned long pool_size = atoi(value);
		if (pool_size) {
			printf("Setting CC memory pool size to %ld bytes.\n", pool_size);
			*CC_size = pool_size;
		}
	}
	if ((value = getenv("RXBOLT"))) {
		if (atoi(value) == 1) {
			*RX_bolt = 1;
			printf("Bolting the RX memory pool.\n");
		}
	}
	if ((value = getenv("RXSIZE"))) {
		unsigned long pool_size = atoi(value);
		if (pool_size) {
			printf("Setting RX memory pool size to %ld bytes.\n", pool_size);
			*RX_size = pool_size;
		}
	}
}

void cop_init_cc_pbic(mapped_memory_pbic_root * mpr, unsigned long newBlockSize,
	unsigned long cc_size, int cc_bolt)
{
	memory_block_t *mapped_range;
	int i, rc;

	mpr->avail_cops = (1 << COP_DECOMP) | (1 << COP_RNG) |
	    (1 << COP_ASYM_CRYPTO) | (1 << COP_SYM_CRYPTO) |
	    (1 << COP_ASYNC_DATA_MOVER);
	mpr->PBICtype = COP_PBIC_CC;
	mpr->crb_cache = malloc(sizeof(struct dec_root) * 15);
	for (i = 0; i < 15; i++) {
		//verbose_print("%d: plain %p amper %p\n", i, mpr->crb_cache[i], &(mpr->crb_cache[i]));
		dec_init(&(mpr->crb_cache[i]));
	}
	dec_init(&(mpr->DDE_cache));
	dec_init(&(mpr->memory_block_list));
#ifdef REENTRANT_ON
	sem_init(&(mpr->mnode_root_lock), 0, 1);
	sem_init(&(mpr->memory_block_lock), 0, 1);
#endif
	errno = 0;

	mpr->Tree = cop_MMTree_create(newBlockSize);
	mapped_range = malloc(sizeof(memory_block_t));

	if ((rc = posix_memalign((void **)&(mapped_range->base_address), (64 * 1024), cc_size)))
		fprintf(stderr, "posix_memalign failed, %s\n", strerror(rc));

	mapped_range->length = cc_size;

	mpr->bolted = cc_bolt;
	if (mpr->bolted)
		mapped_range->flags = COP_MAP_BOLT | COP_MAP_MLOCK;
	else
		mapped_range->flags = 0;

	cop_MMTree_insertmappedblock(mpr->Tree, mapped_range);
	dec_push(&(mpr->memory_block_list), mapped_range);

	cop_remap_pool(mpr);

}

mapped_memory_pool_t
cop_create_pool(unsigned long initialSize, unsigned long PBICinstance)
{
	return NULL;
}

#if 0
mapped_memory_pool_t
cop_create_pool_by_type(unsigned long initialSize, unsigned long PBICtype,
			int bolted)
{
	mapped_memory_pbic_root *mpr;
	memory_block_t *mapped_range;
	int rc;

	mpr = malloc(sizeof(mapped_memory_pbic_root));

#ifdef REENTRANT_ON
	sem_init(&(mpr->memory_block_lock), 0, 1);
	sem_init(&(mpr->mnode_root_lock), 0, 1);
#endif

	mpr->bolted = bolted;
	if (PBICtype == COP_PBIC_CC) {
		mpr->avail_cops = (1 << COP_DECOMP) | (1 << COP_RNG) |
		    (1 << COP_ASYM_CRYPTO) | (1 << COP_SYM_CRYPTO) |
		    (1 << COP_ASYNC_DATA_MOVER);
	} else if (PBICtype == COP_PBIC_RX) {
		mpr->avail_cops = (1 << COP_XML) | (1 << COP_REGX);
	} else
		return NULL;

	mpr->Tree = cop_MMTree_create(4096);
	mapped_range = malloc(sizeof(memory_block_t));

	if (rc = posix_memalign((void **)&(mapped_range->base_address), (64 * 1024), 16*1024*1024))
		fprintf(stderr, "posix_memalign failed, %s\n", strerror(rc));

	if (mpr->bolted)
		mapped_range->flags = COP_MAP_BOLT;
	else
		mapped_range->flags = 0;

	mapped_range->length = 16 * 1024 * 1024;
	dec_push(&(mpr->memory_block_list), mapped_range);

	// initial map and touching of pages to force 
	// creation of the PTEs
	cop_remap_pool(mpr);
}
#endif

void cop_init_rx_pbic(mapped_memory_pbic_root * mpr, unsigned long newBlockSize, 
	unsigned long rx_size, int rx_bolt)
{
	memory_block_t *mapped_range;
	int i, rc;

	mpr->avail_cops = (1 << COP_XML) | (1 << COP_REGX);
	mpr->PBICtype = COP_PBIC_RX;
	mpr->crb_cache = malloc(sizeof(struct dec_root) * 15);

	for (i = 0; i < 15; i++) {
		//verbose_print("%d: plain %p amper %p\n", i, mpr->crb_cache[i], &(mpr->crb_cache[i]));
		dec_init(&(mpr->crb_cache[i]));
	}
	dec_init(&(mpr->DDE_cache));
	dec_init(&(mpr->memory_block_list));
#ifdef REENTRANT_ON
	sem_init(&(mpr->mnode_root_lock), 0, 1);
	sem_init(&(mpr->memory_block_lock), 0, 1);
#endif
	errno = 0;

	mpr->Tree = cop_MMTree_create(newBlockSize);
	mapped_range = malloc(sizeof(memory_block_t));

	if ((rc = posix_memalign((void **)&(mapped_range->base_address), (512 * 1024 * 1024), rx_size)))
		fprintf(stderr, "posix_memalign failed, %s\n", strerror(rc));

	mapped_range->length = rx_size;

	mpr->bolted = rx_bolt;
	if (mpr->bolted)
		mapped_range->flags = COP_MAP_BOLT | COP_MAP_MLOCK;
	else
		mapped_range->flags = 0;

	cop_MMTree_insertmappedblock(mpr->Tree, mapped_range);
	dec_push(&(mpr->memory_block_list), mapped_range);

	// initial map and touching of pages to force 
	// creation of the PTEs
	cop_remap_pool(mpr);

}

int cop_free_pool(mapped_memory_pool_t p)
{
	mapped_memory_pbic_root *mpr = (mapped_memory_pbic_root *) p;
	memory_block_t *mapped_range;

	//cop_MMTree_free(mpr->Tree);
	mapped_range = (memory_block_t *) dec_pop(&(mpr->memory_block_list));
	for (; mapped_range;) {
		free(mapped_range);
		mapped_range =
		    (memory_block_t *) dec_pop(&(mpr->memory_block_list));
	}
	free(mpr);
	return 1;
}

void cop_remap_pool(mapped_memory_pool_t pool)
{
	mapped_memory_pbic_root *p;
	memory_block_t *mapped_range;
	volatile char *memory;
	uint64_t length;
	char i = 0;
	int retv;
	droot t = create_dec();

	p = (mapped_memory_pbic_root *) pool;

	errno = 0;
	if (COP_CTX_FD(&cop_pbic_info_array[p->PBICtype].ctx) != -1) {
		mapped_range =
		    (memory_block_t *) dec_pop(&(p->memory_block_list));
		for (; mapped_range;) {
			length = 0;
			for (memory = (char *)mapped_range->base_address;
			     length < mapped_range->length;) {
				i = *memory;
				*memory = i;
				memory += 4096;
				length += 4096;
			}

			errno = 0;
			retv = cop_pbic_map(
					&(cop_pbic_info_array[p->PBICtype].ctx),
					(void *)mapped_range->base_address,
					mapped_range->length,
					mapped_range->flags);
			if (errno) {
				fprintf(stderr, "child fork pbic_map fail %d\n",
					errno);
				exit(errno);
			}
			dec_push(t, mapped_range);
			mapped_range =
			    (memory_block_t *) dec_pop(&(p->memory_block_list));
		}
		mapped_range = (memory_block_t *) dec_pop(t);
		for (; mapped_range;) {
			dec_push(&(p->memory_block_list), mapped_range);
			mapped_range = (memory_block_t *) dec_pop(t);
		}
	}
}

void cop_touch_pool(mapped_memory_pool_t pool)
{
	mapped_memory_pbic_root *p;
	memory_block_t *mapped_range;
	volatile char *memory;
	uint64_t length;
	char i = 0;
	droot t = create_dec();

	p = (mapped_memory_pbic_root *) pool;

	errno = 0;
	mapped_range = (memory_block_t *) dec_pop(&(p->memory_block_list));
	for (; mapped_range;) {
		length = 0;
		for (memory = mapped_range->base_address;
		     length < mapped_range->length;) {
			i = *memory;
			*memory = i;
			memory += 4096;
			length += 4096;
		}
		dec_push(t, mapped_range);
		mapped_range =
		    (memory_block_t *) dec_pop(&(p->memory_block_list));
	}
	mapped_range = (memory_block_t *) dec_pop(t);
	for (; mapped_range;) {
		dec_push(&(p->memory_block_list), mapped_range);
		mapped_range = (memory_block_t *) dec_pop(t);
	}
}

void cop_fork_parent(void)
{
	verbose_print("parent fork pid %d\n", getpid());

	mapped_memory_pool_t cc = cop_cc_pool();
	mapped_memory_pool_t rx = cop_rx_pool();
	mapped_memory_pbic_root *p;

	p = (mapped_memory_pbic_root *) cc;
	errno = 0;
	cop_touch_pool(cc);
	p = (mapped_memory_pbic_root *) rx;
	cop_touch_pool(rx);
}

void cop_fork_child(void)
{
	verbose_print("child fork pid %d\n", getpid());

	cop_init_cops();

	mapped_memory_pool_t cc = cop_cc_pool();
	mapped_memory_pool_t rx = cop_rx_pool();
	cop_remap_pool(cc);
	cop_remap_pool(rx);
}

int cop_pool_type(mapped_memory_pool_t ptr)
{
	mapped_memory_pbic_root *p = (mapped_memory_pbic_root *) ptr;

	return p->PBICtype;
}

int cop_setup_pbic(mapped_memory_pool_t p, const void *address, size_t length)
{
	mapped_memory_pbic_root *mpr = (mapped_memory_pbic_root *) p;
	int retv;
	memory_block_t *n = malloc(sizeof(memory_block_t));

	n->base_address = (void *)address;
	n->length = length;

	retv = cop_pbic_map(&(cop_pbic_info_array[mpr->PBICtype].ctx),
			address, length, 0);
	if (errno) {
		fprintf(stderr, "cop_setup_pbic map fail retv %d\n", errno);
		// if we're here it's sort of a game over situation
		errno = 0;
		free(n);
		return -1;
	}
#ifdef REENTRANT_ON
	sem_wait(&(mpr->memory_block_lock));
#endif
	dec_push(&(mpr->memory_block_list), n);
#ifdef REENTRANT_ON
	sem_post(&(mpr->memory_block_lock));
#endif
	return 0;
}
