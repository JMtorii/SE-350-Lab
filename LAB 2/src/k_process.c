/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: TEAM BLACKJACK
 * @date:   2014/01/17
 * NOTE: This works (hopefully).
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process;

/* process initialization table */
PROC_INIT g_proc_table[NUM_TEST_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

/**
 * @brief: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	U32 *sp;
  
  /* fill out the initialization table */
	add_null_process();

	set_test_procs();	
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i+1].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i+1].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i+1].mpf_start_pc = g_test_procs[i].mpf_start_pc;
		g_proc_table[i+1].m_priority = g_test_procs[i].m_priority;
	}
	
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TEST_PROCS+1; i++ ) {
		int j;
		// Add test procs to pcbs
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_state = NEW;
		(gp_pcbs[i])->prev = NULL;
		
		// Create stack pointer and alloc space
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		
		// Push each pcb into ready_queue
		q_push(&ready_queue[get_process_priority((gp_pcbs[i])->m_pid)],gp_pcbs[i]);
		
		// Assign memory address of stack pointer for each pcb
		(gp_pcbs[i])->mp_sp = sp;
	}
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
PCB *scheduler(void)
{
	// Get next pcb to execute and return
	PCB* tmp_pcb;
	tmp_pcb = q_pop_highest_priority(ready_queue);
	
	// Assign current process to execute to global var
	gp_current_process = tmp_pcb; 
	return tmp_pcb;
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	state = gp_current_process->m_state;
	
	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			p_pcb_old->m_state = RDY;
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */
	if (gp_current_process != p_pcb_old) {
		if (state == RDY){ 		
			p_pcb_old->m_state = RDY; 
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}

/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	// Set pointer to last executed pcb
	PCB *p_pcb_old = NULL;
	p_pcb_old = gp_current_process;
	
	// Push old process back to ready queue
	if ( p_pcb_old != NULL ) {
		q_push(&ready_queue[get_process_priority(p_pcb_old->m_pid)], p_pcb_old);
	}
	
	// Obtain next in execution
	gp_current_process = scheduler();
	
	// Nothing to do
	if ( gp_current_process == NULL  ) {
		gp_current_process = p_pcb_old;
		return RTX_ERR;
	}
	
	// Init for if no current processes
  if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
	}

	process_switch(p_pcb_old);
	return RTX_OK;
}

// Wrapper to obtain process priority of process with arg pid
int get_process_priority(int process_id) {
	return k_get_process_priority(process_id);
}

// Obtain process priority of process with arg pid
int k_get_process_priority(int process_id) {
	int i;
	for (i = 0;i<NUM_TEST_PROCS+1;++i) {
		if (g_proc_table[i].m_pid == process_id) {
			return g_proc_table[i].m_priority;
		}
	}
	return RTX_ERR;
}

// Wrapper to set priority of process with arg PID
int set_process_priority(int process_id, int priority) {
	return k_set_process_priority(process_id, priority);
}

// Set priority of process with arg PID
int k_set_process_priority(int process_id, int priority) {
	int i;
	//Disallow changing null process or iprocess priorities
	if (process_id == 0) {
		return RTX_ERR;
	}
	for (i = 0;i<NUM_TEST_PROCS+1;++i) {
		if (g_proc_table[i].m_pid == process_id) {
			g_proc_table[i].m_priority = priority;
			return RTX_OK;
		}
	}
	return RTX_ERR;
}

PCB* get_pcb_from_pid(int pid) {
	int i;
	for (i = 0;i<NUM_TEST_PROCS+1;++i) {
		if ((gp_pcbs[i])->m_pid == pid) {
			return gp_pcbs[i];
		}
	}
	return NULL;
}

// Proc to add null process
void add_null_process(void) {
	g_proc_table[0].m_pid = (U32)0;
	g_proc_table[0].m_stack_size = 0x100;
	g_proc_table[0].m_priority = 4;
	g_proc_table[0].mpf_start_pc = &null;
}

// Proc for what the null process does
void null (void) {
	int ret_val = 30;
	while (1) {
		#ifdef DEBUG_0
			printf("[proc0]: ret_val=%d\r\n", ret_val);
			printf("NULL PROCESS!!\r\n", ret_val);
		#endif 
		ret_val = release_processor();
	}
}
