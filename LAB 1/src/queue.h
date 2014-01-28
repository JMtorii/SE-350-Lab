/**
 * @file:   queue.h
 * @brief:  Queue implementation header file
 * @author: The coolest guys ever
 * @date:   2014/01/22
 */
 
#include "k_rtx.h"
#include "k_process.h"

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct Node{
		struct Node *next;
	  PCB *value;
} Node;

extern Node nodes[NUM_TEST_PROCS+1];

typedef struct Queue{
		U32 size;
	  Node* first;
	  Node* last;
		
} Queue;


PCB* q_pop(Queue *q);
void q_push(Queue *q, PCB *val);
Node *getFreeNode(void);


#endif
