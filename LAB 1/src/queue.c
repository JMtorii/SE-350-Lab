#include "queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void* q_pop(Queue* q) {
	void* ret;
	ret = q->first;
	//delete[] q->first;
	q->first = q->first->next;
	return ret;
}

void q_push(Queue* q, void *val) {
	Node n;
	n.next = NULL;
	n.value = val;
	
	if (q->last)
		q->last->next = &n;
	
	q->last = &n;
}


