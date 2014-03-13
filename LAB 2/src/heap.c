#include "heap.h"


#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

MemBlock *first_mem_block = NULL;
int NUM_MEM_BLK = 0;
int SIZE_MEM_BLK = 1024; //128bytes * 8
U8* heap_begin = NULL;

U32 * h_pop(void) {
	U32* ret;
	
	if (first_mem_block == NULL){
		printf("ERROR: OUT OF MEMORY\r\n");
		return NULL;
	}
	
	ret = (U32 *)first_mem_block;
	if (first_mem_block->next_blk != NULL) {
		first_mem_block = first_mem_block->next_blk;
	}
	else {
		first_mem_block = NULL;
	}	
	return ret;
}

void h_push(U32 * mem_blk) {
	((MemBlock *)mem_blk)->next_blk = first_mem_block;
	first_mem_block = (MemBlock *)mem_blk;  
}
