/**
 * @file:   queue.h
 * @brief:  Queue implementation header file
 * @author: TEAM BLACKJACK (021)
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
	  Node* first;
	  Node* last;		
} Queue;

void n_print(void);

PCB* q_pop(Queue *q);
void q_push(Queue *q, PCB *val);
void q_print(Queue *q);

Node *getFreeNode(void);
void freeNode(Node *n);

#endif
