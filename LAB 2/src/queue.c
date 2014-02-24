/**
 * @file:   queue.c
 * @brief:  Implementation of queues
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#include "queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void q_init(Queue *q) {
	q->first = NULL;
	q->last = NULL;
}

// Pop returns the last item in the queue 
void* q_pop(Queue* q) {
	void* ret;
	if (q->last == NULL) {
		return NULL; 
	}
	ret = q->last;
	q->last = &*(q->last);	//first member of q->last which should be q->last->prev
	if (q->last == NULL) {
		q->first = NULL;
	}
	return ret;
}

// Pops the last PCB of the highest priority
void* q_pop_highest_priority(Queue q[]) {
	void *ret;
	int i;
	for (i=0; i<NUM_PRIORITIES; i++){
		ret = (PCB*)q_pop(&q[i]);
		if (ret != NULL) {
			return ret;
		}
	}
	return NULL; // This should never be reached because we always have NULL process
}

// Pushes element to the front, queue sorted by time inserted
void q_push(Queue* q, void *val) {	
  // Queue currently is not empty
	if (q->first != NULL) {
		//void* tmp_void_ptr;
		//tmp_void_ptr = &*(q->first);
		//tmp_void_ptr = val;
	} else {
		q->last = val;
	}
	q->first = val;
}

// Prints PID contents of queue
/*
void q_print(Queue *q) {
	Node* iter;
	int i = 0;
	iter = q->first;
	printf("\r\nContents of q:\r\n==============\r\n");
	while (iter != NULL) {
		i++;
		printf("%d: ", i);
		printf("%d\r\n",iter->value->m_pid);
		printf("Address of next node: %x. \r\n\r\n", iter->next);
		iter = iter->next;
	}
	printf("\r\nq_print complete:==============\r\n");
}*/

