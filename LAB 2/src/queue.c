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
	void** val_prev;

	val_prev = (void *)(val);
	*val_prev = NULL;
	
  // Queue currently is not empty
	if (q->first != NULL) {
		void** q_first_prev;
		q_first_prev = (void *)(q->first);
		*q_first_prev = val; //q->first->prev = val
	} else {
		q->last = val;
	}
	q->first = val;
}

// Prints PID contents of a single queue
void q_print_process(Queue *q, int priority) {
	PCB *iter;
	int i = 0;
	char pid_buffer[8];
	char pri_buffer[8];
	char ind_buffer[8];
	
	iter = (PCB *)(q->last);
	
	itoa(priority, pri_buffer);
	
	while (iter != NULL) {
		
		itoa(i, ind_buffer);
		itoa(iter->m_pid, pid_buffer);
		
		uart0_put_string("Index[");
		uart0_put_string((unsigned char*)ind_buffer);
		uart0_put_string("], pid: ");
		uart0_put_string((unsigned char*)pid_buffer);
		uart0_put_string(", priority: ");
		uart0_put_string((unsigned char*)pri_buffer);
		uart0_put_string("\r\n");
		
		iter = iter->prev;
		i++;
	}
}

void q_print_rdy_process(void) {
	int i;
	char buffer[8];
	if (get_num_mem_blk() == 0) {
		i = 0;
	}
	
	uart0_put_string("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
		
		uart0_put_string("Priority: ");
		itoa(i, buffer);
		uart0_put_string((unsigned char*)buffer);
		uart0_put_string("\r\n");
		q_print_process(&ready_queue[i],i);
	}
	uart0_put_string("\r\nReady queue print complete:================\r\n");
}

void q_print_blk_mem_process(void) {
	int i;
	char buffer[8];
	
  uart0_put_string("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
	  uart0_put_string("Priority: ");
		itoa(i, buffer);
		uart0_put_string((unsigned char*)buffer);
		uart0_put_string("\r\n");
		q_print_process(&blocked_queue[i],i);
	}
	uart0_put_string("\r\nReady queue print complete:================\r\n");
}

void q_print_blk_rcv_process(void) {
	int i;
	uart0_put_string("\r\nContents of q:\r\n==============\r\n");
	for (i=0;i<NUM_PRIORITIES;++i) {
		q_print_process(&blocked_rcv_queue[i],i);
	}
	uart0_put_string("\r\nBlocked on rcv queue print complete:================\r\n");
}

void q_remove_pid(int pid) {
	int i;
	PCB *iter, *next;
	for (i=0;i<NUM_PRIORITIES;++i) {
		next = NULL;
		iter = blocked_rcv_queue[i].last;
		while (iter != NULL) {
			if (iter->m_pid == pid) {
				if (iter == blocked_rcv_queue[i].last) {				//last case
					blocked_rcv_queue[i].last = iter->prev;
					if (iter == blocked_rcv_queue[i].first) {			//only one
						blocked_rcv_queue[i].first = NULL;
					}
				}
				else if (iter == blocked_rcv_queue[i].first) {	//first case
					blocked_rcv_queue[i].first = next;
					next->prev = NULL;
				}
				else {
					next->prev = iter->prev;											//middle case
				}
				return;
			}
			next = iter;
			iter = iter->prev;
		}
	}
	return;
}

void q_update_priority (int pid, int priority) {
	int i;
	PCB *iter, *next;
	for (i=0;i<NUM_PRIORITIES;++i) {
		next = NULL;
		iter = ready_queue[i].last;
		while (iter != NULL) {
			if (iter->m_pid == pid) {
				
				// remove from other queue
				if (iter == ready_queue[i].last) {
					ready_queue[i].last = iter->prev;
					if (iter == ready_queue[i].first) {			//only one
						ready_queue[i].first = NULL;
					}
				}
				else if (iter == ready_queue[i].first) {
					ready_queue[i].first = next;
					next->prev = NULL;
				}
				else {
					next->prev = iter->prev;
				}
				
				// add to correct queue
				q_push(&ready_queue[priority], iter);
				return;
			}
			next = iter;
			iter = iter->prev;
		}
	}
	return;
}
