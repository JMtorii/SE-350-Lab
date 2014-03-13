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
	void** tmp;
	if (q->last == NULL) {
		return NULL; 
	}
	ret = q->last;
	tmp = (void *)(q->last);
	q->last = *tmp;	//q->last = q->last->prev;
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
	void** tmp2;
	//printf("PUSH: %d\r\n",((PCB*)val)->m_pid);
	tmp2 = (void *)(val);
	*tmp2 = NULL;
  // Queue currently is not empty
	if (q->first != NULL) {
		void** tmp;
		tmp = (void *)(q->first);
		*tmp = val; //q->first->prev = val
	} else {
		q->last = val;
	}
	q->first = val;
}

// Prints PID contents of a single queue
void q_print_process(Queue *q,int priority) {
	PCB *iter;
	int i = 0;
	iter = (PCB *)(q->last);
	
	while (iter != (PCB *)(q->first) && iter != NULL) {
		//printf("Index[%d], pid: %d, priority: %d\r\n", i, iter->m_pid, priority);
		iter = iter->prev;
		i++;
	}
	if (iter != NULL) {
		//printf("Index[%d], pid: %d, priority: %d\r\n", i, iter->m_pid, priority);
  }
}

void q_print_rdy_process(void) {
	int i;
	char buffer[8];
	if (get_num_mem_blk() == 0) {
		i = 0;
	}
	
	uart1_put_string("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
		uart1_put_string("Priority: ");
		itoa(i, buffer);
		uart1_put_string((unsigned char*)buffer);
		uart1_put_string("\r\n");
		//printf("Priority %d:\r\n",i);
		q_print_process(&ready_queue[i],i);
	}
	uart1_put_string("\r\nReady queue print complete:================\r\n");
}

void q_print_blk_mem_process(void) {
	int i;
	printf("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
	  printf("Priority %d:\r\n",i);
		q_print_process(&blocked_queue[i],i);
	}
	printf("\r\nBlocked on mem queue print complete:================\r\n");
}

void q_print_blk_rcv_process(void) {
	int i;
	printf("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
		q_print_process(&blocked_rcv_queue[i],i);
	}
	printf("\r\nBlocked on rcv queue print complete:================\r\n");
}

void q_remove_pid(int pid) {
	int i;
	PCB *iter, *next;
	for (i=0;i<NUM_PRIORITIES;++i) {
		next = NULL;
		iter = blocked_rcv_queue[i].last;
		while (iter != NULL) {
			if (iter->m_pid == pid) {
				if (iter == blocked_rcv_queue[i].last) {
					blocked_rcv_queue[i].last = iter->prev;
				}
				else if (iter == blocked_rcv_queue[i].first) {
					blocked_rcv_queue[i].first = next;
				}
				else {
					next->prev = iter->prev;
				}
				return;
			}
			next = iter;
			iter = iter->prev;
		}
	}
	return;
}
