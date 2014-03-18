#include "linked_list.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

//TODO: Implement this ;_;
/*
Heap* h_init() {
	Heap* h;
	h->size = 0;
	h->first = NULL;
	return h;
}

void h_delete(Heap *heap) {
	//todo
}

void h_push(Heap *h, MemBlock value) {
	Node* n;
	n->value = value;
	n->prev = NULL;
	n->next = h->first;
	
	if (h->size > 0) {
		h->first->prev = n; 
	}
	
	++(h->size);
	h->first = n;
}

MemBlock* h_pop(Heap *h) {
	if (h->size == 0)
		return NULL;
	else {
		Node* n = h->first;
	
		if (n->next) {
			n->next->prev = NULL;
			h->first = h->first->next;
		}
		else {
			h->first = NULL;
		}
		
		--(h->size);
		return n->value;
	}
	*/
//}
