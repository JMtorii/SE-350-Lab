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

/*
      ______		 ______
first|______|<--|______|last

*/

typedef struct Queue{
	  PCB* first;
	  PCB* last;		
} Queue;

void n_print(void);

void q_init(Queue *q);
PCB* q_pop(Queue *q);
PCB* q_pop_highest_priority(Queue q[]);
void q_push(Queue *q, PCB *val);
void q_print(Queue *q);



#endif
