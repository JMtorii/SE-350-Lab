#include "queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

// Pop returns the first PCB of highest priority (lowest int)
PCB* q_pop(Queue* q) {
	PCB* ret = NULL;
	int highest_pri = 0;
	int compare;
	Node* iter = NULL;
	Node* prev_iter = NULL;
	
	highest_pri = 4;
	iter = q->first;
	
	// Nothing to pop
	if (iter == NULL) {
		return NULL;
	} 
	// Only one node in queue
	else if (q->first == q->last) {
		ret = q->first->value;
		freeNode(q->first);
		return ret;
	}
	
	// Find the highest priority in the queue
	while(iter != NULL) {
		ret = iter->value;
		compare = get_process_priority(iter->value->m_pid);
		if (compare < highest_pri && compare != -1) {
			highest_pri = compare;
		}
		iter = iter->next;
	}
	printf("Highest priority: %d \n", highest_pri);
	
	iter = q->first;
	prev_iter = NULL;
	
	// Pop the first element with the highest priority	
	while(iter != NULL) {
		if (get_process_priority(iter->value->m_pid) == highest_pri) {
			ret = iter->value;
			
			// Node at beginning
			if (iter == q->first) {
				q->first = q->first->next;
				if(q->first != NULL && q->first->next == NULL) {
					q->last = q->first;
				}
			}
			// Node at end
			else if (iter == q->last) {
				q->last = prev_iter;
				q->last->next = NULL;
			}
			// Node in middle
			else {
				prev_iter->next = iter->next;
			}
			
			// Free deleted node
			freeNode(iter);
			return ret;
		}
		// Iterate through nodes until found
		prev_iter = iter;
		iter = iter->next;
	}
	
	printf("Post-pop (NULL return):\r\n");
	//q_print(q);
	return NULL;
}

// Pushes PCB to end, queue sorted by time inserted
void q_push(Queue* q, PCB *val) {
	Node *n = getFreeNode();
	if (n == NULL) {printf("FAILURE at push\r\n");}

	n->next = NULL;
	n->value = val;

	if (q->last != NULL) {
		q->last->next = n;
	}
	
	q->last = n;
	if (q->first == NULL) {
		q->first = n;
	}
	
	// Tester
	if (0) {
		printf("PID of q->first: %d\r\n",q->first->value->m_pid);
		printf("PID of q->last: %d\r\n",q->last->value->m_pid);
		q_print(q);
		printf("Push completed\r\n");
	}
}

// Prints PID contents of queue
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
}

Node *getFreeNode(void) {
	int i;
	for (i=0;i<NUM_TEST_PROCS+1;++i) {
		if (nodes[i].value == NULL) {
			printf("Return node: %x\r\n",&nodes[i]);
			return &nodes[i];
		}
	}
	printf("Return NULL");
	return NULL;
}

void freeNode(Node *n) {
	printf("Free node called.\r\n");
	n->value = NULL;
	n->next = NULL;
}

void n_print() {
	int i;
	printf("\r\nNode contents print\r\n");
	for (i = 0;i < NUM_TEST_PROCS+1;++i) {
		printf("i: %d, &nodes: %x, &node_val: %x\r\n",i,&nodes[i],(nodes[i].value));
		printf("PID Value: %d\r\n",nodes[i].value->m_pid);
	}
	printf("\r\n");
}
