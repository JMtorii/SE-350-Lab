/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: TEAM BLACKJACK
 * @date:   2014/01/17
 */

#include "k_memory.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
							 /* stack grows down. Fully decremental stack */

Queue blocked_queue[NUM_PRIORITIES];
Queue ready_queue[NUM_PRIORITIES];


/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK    s       |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|<--- p_end, 0x100002ec
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

      Heap|---------------------------|
          |Pointer                    |
          |---------------------------|
          |                           |
          |                           |
          |                           |
          |---------------------------|
          |Pointer                    |
          |---------------------------|
          |                           |
          |                           |
          |                           |
          |---------------------------|
					|Pointer                    |
          |---------------------------|
          |                           |
          |                           |
          |                           |
          |---------------------------|

*/

void memory_init(void)
{
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit;
	U8 *highaddress = (U8 *)0x10008000;
	int i;
	MemBlock *hp;
	MemBlock memBlock;
	
	// Initialize ready and blocked queue
	for (i=0; i<NUM_PRIORITIES; i++) {
		q_init(&blocked_queue[i]);
		q_init(&ready_queue[i]);
	}

	gp_current_process = NULL;
	/* 4 bytes padding */
	p_end += 4;

	//Allocate memory for pcb pointers
	gp_pcbs = (PCB **)p_end;
	p_end += (NUM_TEST_PROCS+1) * sizeof(PCB *);
  
	for ( i = 0; i < NUM_TEST_PROCS + 1; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
	
	// Allocate memory for stack
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	// Allocate memory for heap, implemented
	// Set first mem block to point to first available location
	first_mem_block = (MemBlock *)p_end;
	hp = first_mem_block;
	
	// Calculate number of memory blocks using end addresses of stack and pcb
	NUM_MEM_BLK = (highaddress - p_end) * 3/4 / SIZE_MEM_BLK;
	
	// Create heap (linked list) of memory
	for(i = 0;i < NUM_MEM_BLK;i++) {
		memBlock.next_blk = (MemBlock *)((U8 *)hp + SIZE_MEM_BLK);
		if (i == 0) {
			(*first_mem_block) = memBlock;
		}
		*hp = memBlock;	
		hp = memBlock.next_blk;		
	}
	hp->next_blk = NULL;
	heap_begin = p_end;
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustme	t to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

// Request a memory block from heap
void *k_request_memory_block(void) {
	U32 * ret_val;
	//_atomic_(0);
	while (first_mem_block == NULL) { //Is this correct?
		// Set that process state to BLOCKED
		gp_current_process->m_state = BLK;
		
		// Put PCB on blocked_resource q
		q_push(&blocked_queue[k_get_process_priority(gp_current_process->m_pid)], gp_current_process);
		
		// Release processor
		k_release_processor();
	}
	// Pop from heap
	if (first_mem_block != NULL) {
		ret_val = h_pop();
	}	
	//_endatomic_();
	return (void *) ret_val;
}

// Release a memory block to be used by a process
int k_release_memory_block(void *p_mem_blk) {
	PCB* tmp;
	//_atomic_(0);
	if (p_mem_blk < heap_begin || p_mem_blk > heap_begin+NUM_MEM_BLK*SIZE_MEM_BLK) {
		return RTX_ERR;
	}
	// Push memory block back into heap
	h_push(p_mem_blk);
	
	// If entry in blocked queue after memory is free, then put process back to ready queue
	if (blocked_queue->first != NULL) {
		tmp = (PCB*)q_pop_highest_priority(blocked_queue);
		q_push(&ready_queue[k_get_process_priority(tmp->m_pid)], tmp);
	}
	//_endatomic_();
	return RTX_OK;
}
