#ifndef _COP_MAP_TREE_H
#define _COP_MAP_TREE_H

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
#include "config.h"

#ifdef REENTRANT_ON
#include <semaphore.h>
#include <pthread.h>
#endif

#include "deq.h"


#ifdef __powerpc64__
typedef unsigned long long cop_key_t;
typedef unsigned long long cop_info_t;
#else
typedef unsigned long cop_key_t;
typedef unsigned long cop_info_t;
#endif

#define UNALIGNED_TREE 0
#define QUADWORD_ALIGNED_TREE 1
#define CACHELINE_ALIGNED_TREE 2
#define PAGE_ALIGNED_TREE 3
#define NUMBER_FREE_TREES 4

#define COP_NO_ALIGNMENT 0x00
#define COP_NO_ALIGNMENT_MASK 0xFFFFFFFF
#define COP_16BYTE_ALIGNMENT 0x0010
#define COP_16BYTE_MASK 0x000F
#define COP_CACHELINE_ALIGNMENT 0x0040
#define COP_CACHELINE_MASK 0x007F
#define COP_PAGE_ALIGNMENT 0x1000
#define COP_PAGE_MASK 0x0FFF

#define COP_SIZE_128_MASK
#define COP_SIZE_4096_MASK
#define COP_SIZE_64K_MASK
#define COP_SIZE_1M_MASK
#define COP_SIZE_OTHER_MASK
#define COP_SLOT_SIZE 5

#ifdef __cplusplus
#define __C__ "C"
#else
#define __C__
#endif
typedef struct memory_block_t {
    void * base_address;
    unsigned long long length;
	unsigned long long flags;
} memory_block_t;

typedef struct MappedMemoryTree_dec *MappedMemoryTree;
typedef struct MappedMemoryNode_dec *MappedMemoryNode;
typedef struct MappedMemoryBranch_dec *MappedMemoryBranch;

typedef struct MappedMemoryTree_dec {
	// key is the size
	// info is the address
	MappedMemoryBranch freeList[4];

#ifdef REENTRANT_ON
//	pthread_rwlock_t allocedTreeRWLock;
    sem_t allocedTreeLock;
#endif
	MappedMemoryNode allocedTree;
	unsigned long blockSize;

#ifdef REENTRANT_ON
    sem_t remainingLock;
#endif
	void *ptrRemainingChunk;
	size_t remainingSize;

} MappedMemoryTree_dec;

typedef struct MappedMemoryBranch_dec {
#ifdef REENTRANT_ON
	sem_t bitBucketLocks[6];
#endif

	droot bitBucket[6];
	unsigned long alignment;
} MappedMemoryBranch_dec;

typedef struct MappedMemoryNode_dec {
	cop_key_t size;
	cop_info_t address;
	unsigned int alignment;
	MappedMemoryNode left, right, parent;
} MappedMemoryNode_dec;

typedef struct mnode_pbic_root {
    MappedMemoryTree Tree;

    unsigned long long avail_cops;// bit_mask
    unsigned long PBICtype;
    int bolted;

#ifdef REENTRANT_ON
    // protects CRBs and DDEs
    sem_t mnode_root_lock;
#endif
	struct dec_root *crb_cache;
	struct dec_root DDE_cache;
   
#ifdef REENTRANT_ON
    // protects block list`
    sem_t memory_block_lock;
#endif
    struct dec_root memory_block_list;
} mapped_memory_pbic_root;


MappedMemoryTree cop_MMTree_create(unsigned long newBlockSize);
/* MappedMemoryTree cop_create_MappedMemoryTreeWithPages(unsigned long
						      newBlockSize,
						      unsigned long
						      numberOfPages); */

// for now this has to be 16 meg chucks!
void cop_MMTree_insertmappedblock(MappedMemoryTree tree,
							memory_block_t *newrange);


void *cop_MMTree_malloc(mapped_memory_pbic_root * p,
					   MappedMemoryTree tree,
				       size_t size,
				       unsigned long alignment,
					   int bolted);
void cop_MMTree_free(MappedMemoryTree tree, void *address);


void * cop_MMTree_allocate_mapped_range(mapped_memory_pbic_root * p,
		MappedMemoryTree tree,size_t blockSize, unsigned long alignment, int bolted);
void * cop_MMTree_refill_and_flush(mapped_memory_pbic_root * p,
		size_t blockSize, size_t allocationRequest, unsigned long alignment, int bolted);

size_t cop_MMTree_freeSize(MappedMemoryTree tree);
size_t cop_MMTree_allocatedSize(MappedMemoryTree tree);
void cop_MMTree_flush_remaining(MappedMemoryTree tree);

void cop_dump_tree(MappedMemoryTree tree);
void cop_dump_tree_ranges(MappedMemoryTree tree);

/*

void *malloc_from_pool(unsigned long size, unsigned long alignment=0);
void free_to_pool(void *address);
unsigned long freeSize();
unsigned long allocatedSize();
*/

extern __C__ unsigned long alignmentToTree(unsigned long alignment);
extern __C__ unsigned long alignmentToMask(unsigned long alignment);
extern __C__ unsigned long sizeToBucket(size_t size);
extern __C__ size_t sizeToBucketSize(size_t size);
extern __C__ unsigned long addressToTree(void *address);

#endif /* _COP_MAP_TREE_H */
