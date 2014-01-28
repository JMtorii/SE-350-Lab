#include "queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

PCB* q_pop(Queue* q) {
	PCB* ret;
	int highest_pri, compare;
	Node* iter, *prev_iter;
	
	highest_pri = 4;
	iter = q->first;
	while(iter != NULL) {
		ret = iter->value;
		compare = get_process_priority(iter->value->m_pid);
		if (compare < highest_pri) {
			highest_pri = compare;
		}
		iter = iter->next;
	}
	
	iter = q->first;
	prev_iter = NULL;
	while(iter != NULL) {
		if (get_process_priority(iter->value->m_pid) == highest_pri) {
			ret = iter->value;
			if (iter->next != NULL) {
				iter->value = iter->next->value;
				iter->next = iter->next->next;
			}
			else {
				prev_iter->next = NULL;
			}
			return ret;
		}
		prev_iter = iter;
		iter = iter->next;
	}

	return NULL;
	
	//ret = q->first->value;
	//delete[] q->first;
	//if (q->first){
	//	q->first = q->first->next;
	//}
	
	//return ret;
}

void q_push(Queue* q, PCB *val) {
	Node *n = getFreeNode();
	if (n == NULL) {return;}

	n->next = NULL;
	n->value = val;
	
	if (q->last) {
		q->last->next = n;	
	}
	
	q->last = n;	
}

Node *getFreeNode(void) {
	int i;
	for (i=0;i<NUM_TEST_PROCS+1;++i) {
		if (nodes[i].value == NULL) {
			return &nodes[i];
		}
	}
	return NULL;
}
