/**
 * @file:   Heap.h
 * @brief:  Heap implementation header file
 * @author: The coolest guys ever
 * @date:   2014/01/22
 */
 
#include "k_rtx.h"

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Node{
		struct Node* next;
	  void *value;
} Node;

typedef struct Queue{
		U32 size;
	  Node* first;
	  Node* last;
} Queue;


U32* h_pop(void);
void h_push(U32 * mem_blk);

#endif
