/**
 * @file:   queue.h
 * @brief:  Queue implementation header file
 * @author: TEAM BLACKJACK (021)
 * @date:   2014/01/22
 */
 
#include "k_process.h"

#ifndef QUEUE_H_
#define QUEUE_H_

/*
      ______		 ______
first|______|<--|______|last

*/

typedef struct Queue{
	  void* first;
	  void* last;		
} Queue;

void n_print(void);

void q_init(Queue *q);
void* q_pop(Queue *q);
void* q_pop_highest_priority(Queue q[]);
void q_push(Queue *q, void *val);
void q_print(Queue *q);
void q_print_process(Queue *q,int priority);
void q_print_rdy_process(void);
void q_print_blk_mem_process(void);
void q_print_blk_rcv_process(void);
void q_remove_pid(int pid);

#endif
