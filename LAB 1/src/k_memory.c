/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
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

Queue blk_q;
Queue rdy_q;
Queue *blocked_queue;
Queue *ready_queue;
Node nodes[NUM_TEST_PROCS+1];

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
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
	U32 *test;
 
	for (i = 0;i < 100;i++) {
		printf("\r\n");
	}
	blocked_queue = &blk_q;
	ready_queue = &rdy_q;
	blocked_queue->first = NULL;
	blocked_queue->last = NULL;
	ready_queue->first = NULL;
	ready_queue->last = NULL;
	for (i=0;i<NUM_TEST_PROCS+1;i++) {
		nodes[i].value = NULL;
		nodes[i].next = NULL;
	}

	//n_print();
	//printf("%x",NULL);
  //while(1) {}
		
	gp_current_process = NULL;
	/* 4 bytes padding */
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += (NUM_TEST_PROCS+1) * sizeof(PCB *);
  
	for ( i = 0; i < NUM_TEST_PROCS + 1; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
#ifdef DEBUG_0  
	printf("gp_pcbs[0] = 0x%x \r\n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \r\n", gp_pcbs[1]);
	printf("gp_pcbs[2] = 0x%x \r\n", gp_pcbs[2]);
#endif
	
	/* allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	/* allocate memory for heap, not implemented yet*/	  
	first_mem_block = (MemBlock *)p_end;
	hp = first_mem_block;
	
	NUM_MEM_BLK = (highaddress - p_end) * 3/4 / SIZE_MEM_BLK;
	
	for(i = 0;i < NUM_MEM_BLK;i++) {
		MemBlock memBlock;
		memBlock.next_blk = (MemBlock *)((U8 *)hp + SIZE_MEM_BLK);
		if (i == 0) {
			(*first_mem_block) = memBlock;
		}
		*hp = memBlock;	
		hp = memBlock.next_blk;		
	}
	
	heap_begin = p_end;
	
	printf("\r\nMemory INIT completed.\r\n\r\n");
	
	// HERE BE TESTING
	if (0) {
		hp = first_mem_block;
		for (i = 0; i < 23; i++) {
			printf("0x%x\r\n", hp->next_blk);
			hp = (MemBlock*)hp->next_blk;		
		}
		
		printf("gp_stack: 0x%x \r\n", gp_stack);
		printf("p_end: 0x%x \r\n", p_end);
		
		printf("pop_first_mem_block: 0x%x ",first_mem_block);
		test = h_pop();
		printf("pop_first_mem_block: 0x%x ",first_mem_block);
		printf("pop_testblock: 0x%x \r\n",test);
		
		printf("push_first_mem_block: 0x%x ",first_mem_block);
		h_push(test);
		printf("push_first_mem_block: 0x%x ",first_mem_block);
		printf("push_testblock: 0x%x \r\n",test);
		
		printf("MEM_NUM: %d \r\n",NUM_MEM_BLK);
		while(1) {
		}
	}
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

void *k_request_memory_block(void) {
	U32 * ret_val;
#ifdef DEBUG_0 
	printf("k_request_memory_block: entering...\r\n");
#endif /* ! DEBUG_0 */
	//_atomic_(0);
	while (first_mem_block == NULL) {
		// set that process state to BLOCKED
		gp_current_process->m_state = BLK;
		
		// put PCB on blocked_resource q
		q_push(blocked_queue, gp_current_process);
		
		// release processor
		release_processor();
	}
	ret_val = h_pop();
	//_endatomic_();
	return (void *) ret_val;
}

int k_release_memory_block(void *p_mem_blk) {
	PCB* tmp;
#ifdef DEBUG_0 
	printf("k_release_memory_block: releasing block @ 0x%x\r\n", p_mem_blk);
#endif /* ! DEBUG_0 */
	//_atomic_(0);
	if (p_mem_blk < heap_begin || p_mem_blk > heap_begin+NUM_MEM_BLK*SIZE_MEM_BLK) {
		return RTX_ERR;
	}
	h_push(p_mem_blk);
	
	if (blocked_queue->first != NULL) {
		tmp = q_pop(blocked_queue);
		q_push(ready_queue, tmp);
	}
	
	//_endatomic_();
	return RTX_OK;
}
