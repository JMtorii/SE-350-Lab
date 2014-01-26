/**
 * @file:   Heap.h
 * @brief:  Heap implementation header file
 * @author: The coolest guys ever
 * @date:   2014/01/22
 */
 
#include "k_rtx.h"

#ifndef HEAP_H_
#define HEAP_H_

struct MemBlock;

typedef struct MemBlock {
		struct MemBlock *next_blk;
} MemBlock;

extern MemBlock *first_mem_block;
extern int NUM_MEM_BLK;
extern int SIZE_MEM_BLK;
extern U8 *heap_begin;

/*typedef struct Node{
		struct Node* next;
		struct Node* prev;
	  MemBlock value;
} Node;

typedef struct Heap{
		U32 size;
	  Node* first;
} Heap;*/


U32* h_pop(void);
void h_push(U32 * mem_blk);

// TODO: Stuff here.

#endif
