#ifndef _COP_MAP_NODE_H
#define _COP_MAP_NODE_H

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

#include "cop_map_tree.h"

void
cop_setupMappedMemoryBranch(MappedMemoryBranch b, unsigned long alignment);

void
cop_seedMappedMemoryBranch(MappedMemoryBranch b, unsigned int align, unsigned long stride, unsigned long size, unsigned long totalsize, char *address, unsigned long number);

void
cop_insertMappedMemoryNodeToBranch(MappedMemoryBranch b, MappedMemoryNode ptrNode);

/*
MappedMemoryNode
cop_create_MappedMemoryNode_aligned(MappedMemoryBranch b, unsigned long size, 
					unsigned long BlockSize,
				    unsigned long alignment); */
MappedMemoryNode cop_createMappedMemoryNode(cop_key_t newKey,
					    cop_info_t newInfo, unsigned int align);

inline void cop_setMappedMemoryNodeInfo(MappedMemoryNode node, cop_info_t address)
{
	node->address = address;
}
inline void cop_setMappedMemoryNodeKey(MappedMemoryNode node, cop_key_t size)
{
	node->size = size;
}
inline cop_info_t cop_getMappedMemoryNodeInfo(MappedMemoryNode node)
{
	return node->address;
}
inline cop_key_t cop_getMappedMemoryNodeKey(MappedMemoryNode node)
{
	return node->size;
}

// returned node is new and not in tree
MappedMemoryNode cop_splitMappedMemoryNode(MappedMemoryNode node,
					   unsigned long long newSize);
void cop_insertMappedMemoryNode(MappedMemoryNode * root,
				MappedMemoryNode ptrNode);

void cop_insertMappedMemoryNodeToAllocTree(MappedMemoryTree tree,
				MappedMemoryNode ptrNode);

MappedMemoryNode cop_searchForAlignedMappedMemoryNode(MappedMemoryBranch root,
						      size_t size);
MappedMemoryNode cop_searchForExactMappedMemoryNodeAndRemove(MappedMemoryTree tree,
							  void * address);
void cop_removeFromMappedMemoryTree(MappedMemoryNode * root,
				    MappedMemoryNode node);

void printNodes(MappedMemoryNode root);
int totalNodes(MappedMemoryNode root);
int maxNodeDepth(MappedMemoryNode root);

#endif
