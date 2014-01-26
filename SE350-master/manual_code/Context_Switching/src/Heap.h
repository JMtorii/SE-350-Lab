/**
 * @file:   Heap.h
 * @brief:  Heap implementation header file
 * @author: The coolest guys ever
 * @date:   2014/01/22
 */
 
#include "k_rtx.h"
 
#ifndef HEAP_H_
#define HEAP_H_


typedef struct MemBlock {
		U32 *next_blk;
} MemBlock;

/*typedef struct Node{
		struct Node* next;
		struct Node* prev;
	  MemBlock value;
} Node;

typedef struct Heap{
		U32 size;
	  Node* first;
} Heap;*/


U32* h_pop();

// TODO: Stuff here.

#endif
