/**
 * @file:   mqueue.c
 * @brief:  Implementation of message queues
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#include "mqueue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void mq_init(mQueue *q) {
	q->first = NULL;
	q->last = NULL;
}

// Pop returns the last item in the queue 
void* mq_pop(mQueue* q) {
	void* ret;
	if (q->last == NULL) {
		return NULL; 
	}
	
	q->last = *(q->last);	//first member of q->last which should be q->last->prev
	if (q->last == NULL) {
		q->first = NULL;
	}
	return ret;
}

// Pushes element to the front, queue sorted by time inserted
void mq_push(mQueue* q, void *val) {	
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

