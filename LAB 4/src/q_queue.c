/**
 * @file:   q_queue.c
 * @brief:  Implementation of queues
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#include "q_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void q_q_init(Q_Queue *q) {
	q->first = NULL;
	q->last = NULL;
}

// Pop returns the last item in the queue 
msg* q_q_pop(Q_Queue* q) {
	msg* ret = NULL;
	
	if (q->last == NULL) {
		return NULL; 
	}
	
	ret = q->last;
	q->last = q->last->prev;
	
	if (q->last == NULL) {
		q->first = NULL;
	}
	return ret;
}

// Pushes element to the front, queue sorted by time inserted
void q_q_push(Q_Queue* q, msg *val) {	
  // Queue currently is not empty
	if (q->first != NULL) {
		q->first->prev = val;
	} else {
		q->last = val;
	}
	q->first = val;
}
