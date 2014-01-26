#include "Heap.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

//TODO: Implement this ;_;

//void h_push(Heap *h, MemBlock value) {
//}

U32 * h_pop() {
	if (first_mem_block == NULL){//crash
		}
	
	U32* ret;
	ret = (U32 *)first_mem_block;
	first_mem_block = first_mem_block->next_blk; 
	
	return ret;
}