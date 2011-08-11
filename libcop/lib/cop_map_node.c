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
#include <stdlib.h>
#include <stdio.h>

#ifdef REENTRANT_ON
#include <semaphore.h>
#endif

#include "libcop.h"
#include "cop_map_node.h"
#include "deq.h"
//#include "sasio.h"
//#include "sasalloc.h"

void
cop_setupMappedMemoryBranch(MappedMemoryBranch b, unsigned long alignment)
{
	// setup 128, 512, 4096, 64 and 1M & other lists
	b->bitBucket[0]=create_dec();
	b->bitBucket[1]=create_dec();
	b->bitBucket[2]=create_dec();
	b->bitBucket[3]=create_dec();
	b->bitBucket[4]=create_dec();
	b->bitBucket[5]=create_dec();
	b->alignment=alignment;

#ifdef REENTRANT_ON
	sem_init(&(b->bitBucketLocks[0]),0,1);
	sem_init(&(b->bitBucketLocks[1]),0,1);
	sem_init(&(b->bitBucketLocks[2]),0,1);
	sem_init(&(b->bitBucketLocks[3]),0,1);
	sem_init(&(b->bitBucketLocks[4]),0,1);
	sem_init(&(b->bitBucketLocks[5]),0,1);
#endif
}

void
cop_seedMappedMemoryBranch(MappedMemoryBranch b, unsigned int align, unsigned long stride, unsigned long size , unsigned long totalsize, char *address, unsigned long number)
{
	// setup 128, 512, 4096, 64 and 1M & other lists
	int i;
	MappedMemoryNode n;
	int whichList;
	
	// alloc enough space for MappedMemoryNodes
	char *ptrNodeSpace=malloc (number*(sizeof (MappedMemoryNode_dec)));
	whichList=sizeToBucket(size);	
		
#ifdef REENTRANT_ON
	sem_wait(&(b->bitBucketLocks[whichList]));
#endif
	for (i=0;i<number; i++) {
		n=(MappedMemoryNode)ptrNodeSpace;
		n->address=(cop_info_t)address;
		n->size=size;
		n->left=n->right=n->parent=NULL;
		n->alignment=align;
		dec_push((b->bitBucket[whichList]),n);
		ptrNodeSpace+= sizeof (MappedMemoryNode_dec);
		address+=stride;
	}
#ifdef REENTRANT_ON
	sem_post(&(b->bitBucketLocks[whichList]));
#endif
}

int fitToBucket(size_t size);

void
cop_insertMappedMemoryNodeToBranch(MappedMemoryBranch b, MappedMemoryNode ptrNode)
{
	unsigned int bucket=fitToBucket(ptrNode->size);
	dnode n, last,walker;
	droot d;
	MappedMemoryNode mn=ptrNode;
	
#ifdef REENTRANT_ON
	sem_wait(&(b->bitBucketLocks[bucket]));
#endif
	d=b->bitBucket[bucket];

	if (d->freelist) {
		n = d->freelist;
		d->freelist = d->freelist->next;
	} else {
		n = malloc(sizeof(dec_node));
	}

	verbose_print("node %p size %d contents %p\n", n, d->size, mn);
	d->size++;

	n->contents = mn;
	walker = d->root;
	last=NULL;
	
	while(walker && ((MappedMemoryNode)walker->contents)->size >mn->size && (walker->next)) {
		if (walker->next) {
			last=walker;
			walker=walker->next;
		}
	}
	if (!(walker)) {
		n->next=NULL; 
		if (last) {
			/* add to end */
			last->next=n;
		} else {
			d->root=n;
		}
	} else {
		/* insert into existing spot */
		if (last) {
			n->next=last->next;
			last->next=n; 
		} else {
			n->next=d->root;
			d->root=n;
			
		}
	}

#ifdef REENTRANT_ON
	sem_post(&(b->bitBucketLocks[bucket]));
#endif
}

// no locks ncessary
MappedMemoryNode
cop_createMappedMemoryNode(cop_key_t newKey,
			   cop_info_t newInfo, unsigned int align)
{
	MappedMemoryNode node = malloc(sizeof(MappedMemoryNode_dec));
	node->size = newKey;
	node->address = newInfo;
	node->left=node->right=node->parent=NULL;
	node->alignment=align;

	return node;
}

MappedMemoryNode
cop_searchForAlignedMappedMemoryNode(MappedMemoryBranch treeRoot,
				     size_t size)
{
	MappedMemoryNode n=NULL;
	// k == size in the free tree
	// alignment is implied by starting with this bucket so it is just 
	// a search based on size
	// returns NULL if nothing fits
	
	unsigned int whichList=sizeToBucket(size);
	
	if (treeRoot->bitBucket[whichList]->size>0) {
#ifdef REENTRANT_ON
		sem_wait(&(treeRoot->bitBucketLocks[whichList]));
#endif
		n=dec_pop(treeRoot->bitBucket[whichList]);
#ifdef REENTRANT_ON
		sem_post(&(treeRoot->bitBucketLocks[whichList]));
#endif	
	} else {
		for(;whichList<6;whichList++)
			if (treeRoot->bitBucket[whichList]->size>0) {
#ifdef REENTRANT_ON
				sem_wait(&(treeRoot->bitBucketLocks[whichList]));
#endif	
				if (treeRoot->bitBucket[whichList]->size>0) {
					n=dec_pop(treeRoot->bitBucket[whichList]);
#ifdef REENTRANT_ON
					sem_post(&(treeRoot->bitBucketLocks[whichList]));
#endif	
					if (cop_getMappedMemoryNodeKey(n) < size) {
						fprintf(stderr,"node size %d is less than %d size\n",(int)cop_getMappedMemoryNodeKey(n), (int)size);
						return NULL;
					}
					return n;
				} else {
#ifdef REENTRANT_ON
					sem_post(&(treeRoot->bitBucketLocks[whichList]));
#endif	
				}
			}
		// if we get here we just don't have it
		//verbose_print("cop_searchForAlignedMappedMemoryNode not in list\n");
		return NULL;
	}

	return n;
}


MappedMemoryNode
cop_searchForExactMappedMemoryNodeAndRemove(MappedMemoryTree tree,
				     void *address)
{
	MappedMemoryNode root;
	
#ifdef REENTRANT_ON
//	pthread_rwlock_rdlock(&(tree->allocedTreeRWLock));
	sem_wait(&(tree->allocedTreeLock));
#endif
	root = tree->allocedTree;
	// k == address in the allocated tree
	// just a search based on address
	// returns NULL if nothing matches
	for (;;)
		if (root) {
			if (root->address == (cop_info_t)address) {
#ifdef REENTRANT_ON
//				pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
//				pthread_rwlock_wrlock(&(tree->allocedTreeRWLock));
#endif 
				cop_removeFromMappedMemoryTree(&(tree->allocedTree), root);
#ifdef REENTRANT_ON
				sem_post(&(tree->allocedTreeLock));
//				pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
#endif
				return root;
			} else if ((cop_info_t)address < root->address) {
				root=root->left;
			} else {
				root=root->right;
			}
		} else {
#ifdef REENTRANT_ON
			sem_post(&(tree->allocedTreeLock));
//			pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
#endif
			return NULL;
		}
}

// in the ONE place this is used, the allocedTreeLock is held
//      this is in cop_searchForExactMappedMemoryNodeAndRemove
void
cop_removeFromMappedMemoryTree(MappedMemoryNode * root, MappedMemoryNode node)
{
	MappedMemoryNode newChildForParent;
	MappedMemoryNode walker;

	if (node->right == NULL && node->left==NULL) 
		newChildForParent = NULL;
	else if (node->right && node->left==NULL) 
		newChildForParent = node->right;
	else if (node->left && node->right==NULL)
		newChildForParent = node->left;
	else {
		// left and right are both occupied
		// promote right  and left becomes child of right, pushed left
		newChildForParent = node->right;
		walker=node->right;
		while (walker->left)
			walker=walker->left;

		walker->left = node->left;
		node->left->parent = walker;
	}

	if (node->parent && newChildForParent) {
		if (node->parent->left == node) {
			node->parent->left = newChildForParent;
		} else {
			node->parent->right = newChildForParent;
		}
		newChildForParent->parent=node->parent;
	} else  if (node->parent==NULL && newChildForParent) {
		// no parent but have a child.. they're the new parent
		*root=newChildForParent;
		newChildForParent->parent=NULL;
	} else  if (node->parent && newChildForParent==NULL) {
		if (node->parent->left == node) 
			node->parent->left=NULL;
		else
			node->parent->right=NULL;
	} else {
		// if no parent, and no children, this is the last
		// node in the tree
		*root = NULL;
	}

	node->parent=node->left=node->right=NULL;
}

void
listNodes (MappedMemoryNode node, int indent);
void
printNodes (MappedMemoryNode root)
{
	int indent=0;

	if (root)
		listNodes (root,indent);
	else
		fprintf(stdout,"<Empty Tree>\n");
}

void
listNodes (MappedMemoryNode node, int indent)
{
	int i;

    if ( node->left  ) listNodes (node->left, indent+1);

	for (i = 0; i < indent; i++) 
		fprintf(stdout,"  ");
    fprintf(stdout,"<Node size:%p Addr:%p>\n",(void*)node->size,(void*)node->address);
    if ( node->right ) listNodes (node->right,indent+1);

}

int
walkTotalNodes (MappedMemoryNode node, int count);
int
totalNodes (MappedMemoryNode root)
{
    int	count = 0;
    count = walkTotalNodes ( root, count );
    return count;
}

int
walkTotalNodes (MappedMemoryNode node, int count)
{
    if ( node->left  )
		count = walkTotalNodes (node->left, count);

	count++;

	if ( node->right )
	    count = walkTotalNodes (node->right,count);

    return count;
} 

int
walkNodeDepth (MappedMemoryNode node, int max, int depth);

int
maxNodeDepth (MappedMemoryNode root)
{
    int	max = 0;
    max = walkNodeDepth (root, max, 0 );
    return max;
}

int
walkNodeDepth (MappedMemoryNode node, int max, int depth)
{
    if ( node->left ) 
		max = walkNodeDepth (node->left,max, depth + 1);

    if ( max < depth ) 
		max = depth;

    if ( node->right ) 
		max = walkNodeDepth (node->right,max, depth + 1);

    return max;
}

// root is ALWAYS the root node of the respective tree
void cop_insertMappedMemoryNodeToAllocTree(MappedMemoryTree tree, MappedMemoryNode n)
{
	MappedMemoryNode p;
	cop_info_t k = cop_getMappedMemoryNodeInfo(n);

#ifdef REENTRANT_ON
//	pthread_rwlock_rdlock(&(tree->allocedTreeRWLock));
	sem_wait(&(tree->allocedTreeLock));
#endif

	p = tree->allocedTree;
	if (p != NULL) {
		do {
			if (k < p->address) {
				if (p->left) {
					p = p->left;
				} else {
#ifdef REENTRANT_ON
//					pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
//					pthread_rwlock_wrlock(&(tree->allocedTreeRWLock));
#endif
					p->left = n;
					n->parent = p;
					p = NULL;
				}
			} else {
				if (p->right) {
					p = p->right;
				} else {
#ifdef REENTRANT_ON
//					pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
//					pthread_rwlock_wrlock(&(tree->allocedTreeRWLock));
#endif
					p->right = n;
					n->parent = p;
					p = NULL;
				}
			}
		} while (p);
	} else {
		tree->allocedTree = n;
	}
#ifdef REENTRANT_ON
//	pthread_rwlock_unlock(&(tree->allocedTreeRWLock));
	sem_post(&(tree->allocedTreeLock));
#endif
}

void cop_rotate_left(MappedMemoryNode *root, MappedMemoryNode node) {
}

void cop_rotate_right(MappedMemoryNode *root, MappedMemoryNode node) {
}
