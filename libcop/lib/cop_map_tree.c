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
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "libcopmanage.h"
#include "libcopmalloc.h"
#include "libcop.h"
#include "cop_map_tree.h"
//#include "sasio.h"
//#include "sasalloc.h"

extern struct cop_pbic_info cop_pbic_info_array[COP_NUMBER_KNOWN_PBICS];

extern void cop_insertMappedMemoryNodeToBranch(MappedMemoryBranch b, MappedMemoryNode ptrNode);
extern void cop_insertMappedMemoryNodeToAllocTree(MappedMemoryTree tree, MappedMemoryNode n);
extern void cop_seedMappedMemoryBranch(MappedMemoryBranch b, unsigned int align, unsigned long stride, unsigned long size , unsigned long totalsize, char *address, unsigned long number);
extern void cop_setupMappedMemoryBranch(MappedMemoryBranch b, unsigned long alignment);

/* locking not necessary - init code */
MappedMemoryTree 
cop_MMTree_create(unsigned long newBlockSize)
{
	MappedMemoryTree tree = malloc(sizeof(MappedMemoryTree_dec));
	int i;
	if (newBlockSize < 4096)
		newBlockSize = 4096;
	tree->blockSize = newBlockSize;
	
	unsigned long alignment = COP_NO_ALIGNMENT;

#ifdef REENTRANT_ON
//	pthread_rwlock_init(&(tree->allocedTreeRWLock),NULL);
	sem_init(&(tree->allocedTreeLock),0,1);
	sem_init(&(tree->remainingLock),0,1);
#endif
	
	for (i = 0; i < NUMBER_FREE_TREES; i++) {
		switch (i) {
		case 0:
			alignment = COP_NO_ALIGNMENT;
			break;
		case 1:
			alignment = COP_16BYTE_ALIGNMENT;
			break;
		case 2:
			alignment = COP_CACHELINE_ALIGNMENT;
			break;
		case 3:
			alignment = COP_PAGE_ALIGNMENT;
			break;
		}
		tree->freeList[i] = malloc(sizeof(MappedMemoryBranch_dec));
		cop_setupMappedMemoryBranch(tree->freeList[i], alignment);
	}
	tree->allocedTree = NULL;
	return tree;
}

void *
cop_MMTree_topoffmappedblock(MappedMemoryTree tree, memory_block_t *newrange){
	char * ptrMemory=newrange->base_address;

#ifdef REENTRANT_ON
	sem_wait(&(tree->remainingLock));
#endif
	tree->ptrRemainingChunk=ptrMemory;
	tree->remainingSize=newrange->length;
#ifdef REENTRANT_ON
	sem_post(&(tree->remainingLock));
#endif
	return ptrMemory;
}

void
cop_MMTree_insertmappedblock(MappedMemoryTree tree, memory_block_t *newrange) {
char * ptrMemory=newrange->base_address;
char * endaddress;

#ifdef REENTRANT_ON
	sem_wait(&(tree->remainingLock));
#endif
	tree->remainingSize=newrange->length;

	for (endaddress=ptrMemory+newrange->length; ptrMemory < endaddress; 
		ptrMemory += 4096) {
		*ptrMemory=0xC0;
	}

 	ptrMemory=newrange->base_address;

	/* this adds up to 4998400 */
	cop_seedMappedMemoryBranch(tree->freeList[3],3, 64*1024,64*1024, 64*1024*25, ptrMemory, 25);
	ptrMemory += (64*1024 * 25);
	tree->remainingSize-= (64*1024 * 25);
	cop_seedMappedMemoryBranch(tree->freeList[3], 3, 4096, 4096, 4096*25,ptrMemory, 25);
	ptrMemory += (4*1024 * 25);
	tree->remainingSize-= (4*1024 * 25);
	cop_seedMappedMemoryBranch(tree->freeList[2], 2, 4096, 4096, 4096*50,ptrMemory, 50);
	ptrMemory += (4*1024 * 50);
	tree->remainingSize-= (4*1024 * 50);
	cop_seedMappedMemoryBranch(tree->freeList[1], 1, 4096, 4096, 4096*50, ptrMemory, 50);
	ptrMemory += (4*1024 * 50);
	tree->remainingSize-= (4*1024 * 50);
	cop_seedMappedMemoryBranch(tree->freeList[0], 0, 4096, 4096, 4096*50,ptrMemory, 50);
	ptrMemory += (4*1024 * 50);
	tree->remainingSize-= (4*1024 * 50);

	cop_seedMappedMemoryBranch(tree->freeList[3], 3, 4096, 128, 4096*25,ptrMemory, 25);
	ptrMemory += (4096 * 25);
	tree->remainingSize-= (4096 * 25);

	cop_seedMappedMemoryBranch(tree->freeList[3], 3, 4096, 512, 4096*25, ptrMemory, 25);
	ptrMemory += (4096 * 25);
	tree->remainingSize-= (4096 * 25);

	cop_seedMappedMemoryBranch(tree->freeList[2], 2, 512, 512, 512*250,ptrMemory, 250);
	ptrMemory += (512 * 250);
	tree->remainingSize-= (512 * 250);

	cop_seedMappedMemoryBranch(tree->freeList[1], 1, 512, 512, 512*250,ptrMemory, 250);
	ptrMemory += (512 * 250);
	tree->remainingSize-= (512 * 250);

	cop_seedMappedMemoryBranch(tree->freeList[0], 0, 128, 128, 128*250,ptrMemory, 250);
	ptrMemory += (128 * 250);
	tree->remainingSize-= (128 * 250);

	cop_seedMappedMemoryBranch(tree->freeList[1], 1, 128, 128, 128*250,ptrMemory, 250);
	ptrMemory += (128 * 250);
	tree->remainingSize-= (128 * 250);

	cop_seedMappedMemoryBranch(tree->freeList[2], 2, 128, 128, 128*250,ptrMemory, 250);
	ptrMemory += (128 * 250);
	tree->remainingSize-= (128 * 250);

	tree->ptrRemainingChunk=ptrMemory;
#ifdef REENTRANT_ON
	sem_post(&(tree->remainingLock));
#endif
}

#ifdef __powerpc64__
#define ANDMASK(a, b) ((uint64_t)(a) & (uint64_t)b)
#define REMMASK(a, b) ((uint64_t)(a) % b)
#else
#define ANDMASK(a, b) ((uintptr_t)(a) & b)
#define REMMASK(a, b) ((uintptr_t)(a) % b)
#endif

extern MappedMemoryNode cop_searchForAlignedMappedMemoryNode(
					MappedMemoryBranch treeRoot,
					size_t size);
extern cop_info_t cop_getMappedMemoryNodeInfo(MappedMemoryNode node);

extern MappedMemoryNode cop_createMappedMemoryNode(cop_key_t newKey,
                           cop_info_t newInfo, unsigned int align);
extern cop_info_t cop_getMappedMemoryNodeInfo(MappedMemoryNode node);
extern MappedMemoryNode cop_searchForExactMappedMemoryNodeAndRemove(
				MappedMemoryTree tree, void *address);

// cop_searchForAlignedMappedMemoryNode - locks
// cop_insertMappedMemoryNodeToAllocTree - locks
//
void *cop_MMTree_malloc(mapped_memory_pbic_root * p,
					   MappedMemoryTree tree,
				       size_t size,
				       unsigned long alignment,
					   int bolted)
{
	unsigned long whichTree = alignmentToTree(alignment);
	char * toBeReturned;
//	memory_block_t *mapped_range;
//	volatile char *memory;
//	droot t;
	

	if (!tree) {
		fprintf(stderr, "Null memory map tree - check that your coprocessors exists in /dev/cop_type/\n");
		return NULL;
	}

	MappedMemoryNode ptrNode =
	    cop_searchForAlignedMappedMemoryNode((tree->freeList[whichTree]),
						 size);

	// if this comes back NULL, we return NULL which is a signal to the 
	// accell layer that they need to take action, allocate more
	// map it and push the mapped block into the tree and recalli
	if (ptrNode) {
		cop_insertMappedMemoryNodeToAllocTree(tree, ptrNode);

		toBeReturned=(char *)cop_getMappedMemoryNodeInfo(ptrNode);
	} else {
		char *walker;
		unsigned long alignmentmask = alignmentToMask(alignment);
		size_t allocSize=sizeToBucketSize(size);
		size_t offset=0;
#ifdef REENTRANT_ON
		sem_wait(&(tree->remainingLock));
#endif
		walker=tree->ptrRemainingChunk;
		if ((!(ANDMASK(walker,alignmentmask))) &&
			allocSize<= tree->remainingSize) {
			;
		} else {
			offset = REMMASK(walker, alignment);
			if (offset)
				offset=alignment - offset;
		}
		if ((offset + allocSize) <= tree->remainingSize) {
			// golden~
			ptrNode=cop_createMappedMemoryNode(allocSize, (cop_info_t)(walker+offset),whichTree);

			tree->ptrRemainingChunk+=(allocSize+offset);
			tree->remainingSize-=(allocSize+offset);
			cop_insertMappedMemoryNodeToAllocTree(tree, ptrNode);

#ifdef REENTRANT_ON
			sem_post(&(tree->remainingLock));
#endif
			toBeReturned=(char *)cop_getMappedMemoryNodeInfo(ptrNode);
		} else {
			// we're screwed!
		
			if (allocSize < (16*1024*1024))
				allocSize=16*1024*1024;
			
			walker=cop_MMTree_refill_and_flush(p, allocSize,size,alignment,bolted);

#ifdef REENTRANT_ON
			sem_post(&(tree->remainingLock));
#endif
			toBeReturned=walker; 
		}
	}

/* 
	t=create_dec();
#ifdef REENTRANT_ON
	sem_wait(&(p->memory_block_lock));
#endif
    mapped_range=(memory_block_t *)dec_pop(&(p->memory_block_list));
	for(;mapped_range;) {

		memory=(char *)mapped_range->base_address;
		if (toBeReturned <= (memory+mapped_range->length)) {
			// ok have a start let's see if we're in bounds now
			if (toBeReturned+size <= (memory+mapped_range->length))
				; // good allocation
			else {
				fprintf(stderr, "In range but bad length %d\n", size);
				if (ptrNode)
					fprintf(stderr, "node says it's %d length\n",cop_getMappedMemoryNodeKey(ptrNode));
			}
		}
		 dec_push(t,mapped_range);
		 mapped_range=(memory_block_t *)dec_pop(&(p->memory_block_list));
	}
	mapped_range=(memory_block_t *)dec_pop(t);
	for(;mapped_range;) {
		dec_push(&(p->memory_block_list),mapped_range);
		mapped_range=(memory_block_t *)dec_pop(t);
	}
#ifdef REENTRANT_ON
	sem_post(&(p->memory_block_lock));
#endif
*/
	return toBeReturned;
} 

void *
cop_MMTree_allocate_mapped_range(mapped_memory_pbic_root * p,
				MappedMemoryTree tree, size_t blockSize,
				unsigned long alignment, int bolted) {
	memory_block_t *mapped_range;
	char *address,*endaddress;
	int retv;

	mapped_range=malloc(sizeof (memory_block_t));
	mapped_range->length=blockSize;
	if (alignment < 128)
		alignment=128;

	retv=posix_memalign((void **) &(mapped_range->base_address),
			alignment, blockSize);

	if (retv) {
		fprintf(stderr,"posix_memalign failure %d size %d alignment %d error\n",retv, (int)blockSize, (int)alignment);
		free(mapped_range);
		return NULL;
	}

	/* Fault in all pages */
	address=(char *)mapped_range->base_address;
	for (endaddress = address + blockSize ; address < endaddress;
							address += 4096) {
		*address=0xC0;
	}   

	if (bolted)
		mapped_range->flags=COP_MAP_BOLT;
	else
		mapped_range->flags=0;

	// at the time we lock this, we are holding the remaing lock in the tree
#ifdef REENTRANT_ON
	sem_wait(&(p->memory_block_lock));
#endif          

	errno=0;
	retv = cop_pbic_map(&(cop_pbic_info_array[p->PBICtype].ctx),
			(void *)mapped_range->base_address,
			mapped_range->length, mapped_range->flags);

	if (errno) {
		fprintf(stderr, "PBIC maaping for type %d failed for address %p length %x errno %d\n",(int) p->PBICtype, (void *)mapped_range->base_address, (int)mapped_range->length, (int)errno);
		// if we're here it's sort of a game over situation
		free(mapped_range);
		free(mapped_range->base_address);

#ifdef REENTRANT_ON
		sem_post(&(p->memory_block_lock));
#endif          
		return NULL;
	}       

	dec_push(&(p->memory_block_list), mapped_range);

#ifdef REENTRANT_ON
	sem_post(&(p->memory_block_lock));
#endif          

	return mapped_range->base_address;
}

void *
cop_MMTree_refill_and_flush(mapped_memory_pbic_root * p,size_t blockSize, size_t allocationRequest, unsigned long alignment, int bolted) {
	MappedMemoryTree tree=p->Tree;
	char *toBeReturned;
	MappedMemoryNode ptrNode;
	size_t allocation;

	// convert ptrRemainingChunk into something useful
	// posix_memalign with mapping
	cop_MMTree_flush_remaining(tree);
	
	toBeReturned=cop_MMTree_allocate_mapped_range(p,tree,blockSize,alignment,bolted);

	if (toBeReturned) {
		int whichTree=addressToTree(toBeReturned);
		allocation=sizeToBucketSize(allocationRequest);
		if (allocation< allocationRequest)
			allocation=allocationRequest;
		ptrNode=cop_createMappedMemoryNode(allocation,(cop_info_t)toBeReturned,whichTree);
		cop_insertMappedMemoryNodeToAllocTree(tree, ptrNode);

		tree->ptrRemainingChunk=toBeReturned+allocationRequest;
		tree->remainingSize=blockSize-allocationRequest;
		}
	// refill bucket?
	return toBeReturned;
}


// cop_searchForExactMappedMemoryNodeAndRemove locks
// cop_insertMappedMemoryNodeToBranch locks
void cop_MMTree_free(MappedMemoryTree tree, void *address)
{
// find address/node
    unsigned long whichTree;

    MappedMemoryNode ptrNode=cop_searchForExactMappedMemoryNodeAndRemove(tree,address);

	if (ptrNode) {
		whichTree=ptrNode->alignment;
//		fprintf(stderr,"t %x address %p \n",(long int)syscall(207), address);
		cop_insertMappedMemoryNodeToBranch((tree->freeList[whichTree]),ptrNode);
	} else {
		fprintf(stderr,"err t %x address %p did not come from libcop\n",(unsigned int)syscall(207), address);
	}
} 

size_t cop_MMTree_freeSize(MappedMemoryTree tree)
{
	return 0;
}

unsigned long cop_MMTree_alloocatedSize(MappedMemoryTree tree)
{
	return 0;
}

extern void printNodes (MappedMemoryNode root);

void cop_dump_tree(MappedMemoryTree tree) {

// dump allocated tree

fprintf(stdout,"Tree %p\n",tree);
fprintf(stdout,"AllocatedTree\n==================================================\n");

printNodes(tree->allocedTree);
}

void cop_dump_tree_ranges(MappedMemoryTree tree) {
}

int fitToBucket(size_t size);

// caller has remaining portion of tree locked already
void cop_MMTree_flush_remaining(MappedMemoryTree tree) {
	char *address;
	unsigned long long remaining;
	MappedMemoryNode ptrNode;

	address=tree->ptrRemainingChunk;
	tree->ptrRemainingChunk=NULL;
	remaining=tree->remainingSize;
	tree->remainingSize=0;

	if (address) {
		int whichBucket=-1;
		int whichTree=addressToTree(address);
		if (remaining>=128)
			whichBucket=fitToBucket(remaining);
		if (whichBucket>-1) {
			ptrNode=cop_createMappedMemoryNode(remaining, (cop_info_t)address,whichTree);
			cop_insertMappedMemoryNodeToBranch(tree->freeList[whichTree], ptrNode);	
		}
	}
}

/* keep in mind
#define COP_NO_ALIGNMENT 0x00
#define COP_NO_ALIGNMENT_MASK 0xFFFFFFFF
#define COP_16BYTE_ALIGNMENT 0x0010
#define COP_16BYTE_MASK 0x000F
#define COP_CACHELINE_ALIGNMENT 0x0100
#define COP_CACHELINE_MASK 0x00FF
#define COP_PAGE_ALIGNMENT 0x1000
#define COP_PAGE_MASK 0x0FFF
*/
unsigned long alignmentToTree(unsigned long alignment) {
	if (alignment <= 1)
		return COP_NO_ALIGNMENT;

	else if (alignment <= 16)
		return 1;

	else if (alignment <= 128)
		return 2;

	else if (alignment <= 4096)
		return 3;

	else
		return 3;
}

unsigned long alignmentToMask(unsigned long alignment)
{
// need to handle absurd case
	if (alignment <= 1)
		return -1;

	else if (alignment <= 16)
		return 0x0F;

	else if (alignment <= 128)
		return 0x007F;

	else if (alignment <= 4096)
		return 0x0FFF;

	return (alignment-1);
}

int fitToBucket(size_t size) {
	if (size < 128)
		return -1;
	else if (size == 128)
		return 0;
	else if (size<512)
		return 0;
	else if (size==512)
		return 1;
	else if (size < 4096)
		return 1;
	else if (size == 4096)
		return 2;
	else if (size < (64 * 1024))
		return 2;
	else if (size == (64 * 1024))
		return 3;
	else if (size < (1024 * 1024))
		return 3;
	else if (size == (1024 * 1024))
		return 4;
	else
		return 5;
}

unsigned long sizeToBucket(size_t size) {
	if (size <= 128)
		return 0;
	else if (size<=512)
		return 1;
	else if (size <= 4096)
		return 2;
	else if (size <= (64 * 1024))
		return 3;
	else if (size <= (1024 * 1024))
		return 4;
	else
		return 5;
}

#if 0
size_t bucketTobucketSize(unsigned long bucketNumber) {
	switch(bucketNumber) {
		case 0:
			return 128;
		case 1:
			return 512;
		case 2:
			return 4096;
		case 3:
			return (64*1024);
		case 4:
			return (1024*1024);
		case 5:
			return (1024*1024*16);
	}
}
#endif

size_t sizeToBucketSize(size_t size) {
	if (size <= 128)
		return 128;
	else if (size<=512)
		return 512;
	else if (size <= 4096)
		return 4096;
	else if (size <= (64 * 1024))
		return (64*1024);
	else if (size <= (1024 * 1024))
		return (1024*1024);
	else
		return size;
}

unsigned long addressToTree(void *address)
{
	unsigned long long addr = (unsigned long long)(uintptr_t)address;
	if (!( addr &  COP_PAGE_MASK))
		return 3;
	else if (!(addr & COP_CACHELINE_MASK))
		return 2;
	else if (!( addr & COP_16BYTE_MASK))
		return 1;
	else 
		return 0;
}

unsigned long alignmentMapping(unsigned long alignment) {
	if (alignment <16)
		return 8;
	else if (alignment ==16)
		return 16;
	else if (alignment<=128)
		return 128;
	else
		return 4096;
}
