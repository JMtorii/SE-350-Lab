/**
 * @file:   q_queue.h
 * @brief:  Queue implementation header file
 * @author: TEAM BLACKJACK (021)
 * @date:   2014/01/22
 */
 
#include "uart_polling.h"

#ifndef Q_QUEUE_H_
#define Q_QUEUE_H_

/*
      ______	   ______
first|______|<--|______|last

*/

typedef struct msg {
	int mtype;
	char *mtext;
	struct msg *prev;
} msg;

typedef struct Q_Queue{
	msg* first;
	msg* last;		
} Q_Queue;

void q_q_init(Q_Queue *q);
msg* q_q_pop(Q_Queue *q);
void q_q_push(Q_Queue *q, msg *val);

#endif
