/**
 * @file:   queue.h
 * @brief:  Queue implementation header file
 * @author: TEAM BLACKJACK (021)
 * @date:   2014/01/22
 */
 
#include "k_process.h"

#ifndef MQUEUE_H_
#define MQUEUE_H_

/*
      ______		 ______
first|______|<--|______|last

*/

typedef struct mQueue{
	  void* first;
	  void* last;		
} mQueue;


void mq_init(mQueue *q);
void* mq_pop(mQueue *q);
void mq_push(mQueue *q, void *val);
void mq_print(mQueue *q);



#endif
