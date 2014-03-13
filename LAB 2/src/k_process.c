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
#include "envelope.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process;
PCB *p_pcb_old;

U32 g_switch_flag = 1;          /* whether to continue to run the process before the UART receive interrupt */
                                /* 1 means to switch to another process, 0 means to continue the current process */
																/* this value will be set by UART handler */

// Holds keyboard characters for KCD when relevant
char next_command_char;
int in_command_mode = 0;

// Current command sent to a process
char current_command[256];

/* process initialization table */
PROC_INIT g_proc_table[NUM_TEST_PROCS+NUM_SYS_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];

/**
 * @brief: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	U32 *sp;
  
	p_pcb_old = NULL;
  /* fill out the initialization table */
	add_system_processes();

	set_test_procs();	
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i+1].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i+1].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i+1].mpf_start_pc = g_test_procs[i].mpf_start_pc;
		g_proc_table[i+1].m_priority = g_test_procs[i].m_priority;
	}
	
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TEST_PROCS+NUM_SYS_PROCS; i++ ) {
		int j;
		// Add test procs to pcbs
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_state = NEW;
		(gp_pcbs[i])->prev = NULL;
		/*
		if ((g_proc_table[i]).m_pid >= 14) {
			(gp_pcbs[i])->m_state = NEW;
		}*/
		
		// Create stack pointer and alloc space
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		
		// Push each pcb into ready_queue
		if (g_test_procs[i].m_priority != -1) {
			q_push(&ready_queue[k_get_process_priority((gp_pcbs[i])->m_pid)],gp_pcbs[i]);
		}
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
	tmp_pcb = (PCB*)q_pop_highest_priority(ready_queue);
	
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

// Obtain process priority of process with arg pid
int k_get_process_priority(int process_id) {
	int i;
	for (i = 0;i<NUM_TEST_PROCS+NUM_SYS_PROCS;++i) {
		if (g_proc_table[i].m_pid == process_id) {
			return g_proc_table[i].m_priority;
		}
	}
	return RTX_ERR;
}

// Wrapper to set priority of process with arg PID
/*int set_process_priority(int process_id, int priority) {
	return k_set_process_priority(process_id, priority);
}*/

// Set priority of process with arg PID
int k_set_process_priority(int process_id, int priority) {
	int i;
	//TODO Disallow changing null process or iprocess priorities
	if (process_id == 0 || priority == -1) {
		return RTX_ERR;
	}
	for (i = 0;i<NUM_TEST_PROCS+NUM_SYS_PROCS;++i) {
		if (g_proc_table[i].m_pid == process_id) {
			q_update_priority(process_id, priority);
			g_proc_table[i].m_priority = priority;
			return RTX_OK;
		}
	}
	return RTX_ERR;
}

PCB* get_pcb_from_pid(int pid) {
	int i;
	for (i = 0;i<NUM_TEST_PROCS+NUM_SYS_PROCS;++i) {
		if ((gp_pcbs[i])->m_pid == pid) {
			return gp_pcbs[i];
		}
	}
	return NULL;
}

/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	// Set pointer to last executed pcb
	p_pcb_old = NULL;
	p_pcb_old = gp_current_process;
	
	//printf("############# Printing current process ############### \r\n");
	//printf("PID current proc: %d\r\n",gp_current_process->m_pid);
	//printf("############# Printing ready queue ################## \r\n");
	//q_print_rdy_process();
	//printf("############# Printing blocked queue ################ \r\n");
	//q_print_blk_mem_process();
	//print_num_mem_blk();
	
	// Push old process back to ready queue
	if ( p_pcb_old != NULL && p_pcb_old->m_state != BLK) {
		q_push(&ready_queue[k_get_process_priority(p_pcb_old->m_pid)], p_pcb_old);
		//q_print_rdy_process();
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

int k_release_from_iprocess(void)
{	
	// Set pointer to last executed pcb
	//printf("iProcess exiting \r\n");
	//uart1_put_string("iProcess exiting \r\n");
	//printf("############# Printing current process ############### \r\n");
	//printf("PID current proc: %d\r\n",gp_current_process->m_pid);
	//printf("############# Printing ready queue ################## \r\n");
	//q_print_rdy_process();
	//printf("############# Printing blocked queue ################ \r\n");
	//q_print_blk_mem_process();
	//print_num_mem_blk();

	p_pcb_old = gp_current_process;
	
	// Obtain next in execution
	gp_current_process = scheduler();
	
	// Init for if no current processes
  if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
	}
	process_switch(p_pcb_old);
	return RTX_OK;
}

int k_release_into_iprocess(void)
{	
	// Set pointer to last executed pcb
	//printf("iProcess beginning \r\n");
	//uart1_put_string("iProcess beginning \r\n");
	//printf("############# Printing current process ############### \r\n");
	//printf("PID current proc: %d\r\n",gp_current_process->m_pid);
	//printf("############# Printing ready queue ################## \r\n");
	//q_print_rdy_process();
	//printf("############# Printing blocked queue ################ \r\n");
	//q_print_blk_mem_process();
	//print_num_mem_blk();
	
	// Push old process back to ready queue
	if ( p_pcb_old != NULL && p_pcb_old->m_state != BLK) {
		q_push(&ready_queue[k_get_process_priority(p_pcb_old->m_pid)], p_pcb_old);
		//q_print_rdy_process();
  }
	
	// Init for if no current processes
  if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
	}
	
	process_switch(p_pcb_old);

	return RTX_OK;
}

// Proc to add null process
void add_system_processes(void) {
	g_proc_table[0].m_pid = (U32)0;
	g_proc_table[0].m_stack_size = 0x100;
	g_proc_table[0].m_priority = 4;
	g_proc_table[0].mpf_start_pc = &null;
	
	g_proc_table[NUM_TEST_PROCS+1].m_pid = (U32)12;
	g_proc_table[NUM_TEST_PROCS+1].m_stack_size = 0x100;
	g_proc_table[NUM_TEST_PROCS+1].m_priority = 4;
	g_proc_table[NUM_TEST_PROCS+1].mpf_start_pc = &KCD;
	
	g_proc_table[NUM_TEST_PROCS+2].m_pid = (U32)13;
	g_proc_table[NUM_TEST_PROCS+2].m_stack_size = 0x100;
	g_proc_table[NUM_TEST_PROCS+2].m_priority = 4;
	g_proc_table[NUM_TEST_PROCS+2].mpf_start_pc = &CRT;
	
	g_proc_table[NUM_TEST_PROCS+3].m_pid = (U32)14;
	g_proc_table[NUM_TEST_PROCS+3].m_stack_size = 0x100;
	g_proc_table[NUM_TEST_PROCS+3].m_priority = -1;
	g_proc_table[NUM_TEST_PROCS+3].mpf_start_pc = &Timer_i;
	
	g_proc_table[NUM_TEST_PROCS+4].m_pid = (U32)15;
	g_proc_table[NUM_TEST_PROCS+4].m_stack_size = 0x100;
	g_proc_table[NUM_TEST_PROCS+4].m_priority = -1;
	g_proc_table[NUM_TEST_PROCS+4].mpf_start_pc = &UART_i;
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

void WallClock_p(void) {
	int ret_val = 0;
	int this_pid = 11;
	int hours;
	int minutes;
	int seconds;
	//TODOTODOTODO
	//INCLUDE IN G_PROC_TABLE AND .H FILE
	while (1) {
		if (current_command == "WT") {
			// reset the wall clock and display it on the CRT
		}
		else if (current_command == "WT") {
			// terminates the wall clock
		}
		else {
			int error = 1;
			// Real men don't need sprintf.
			if (current_command[1] == 'S' && current_command[2] == ' ') {
				// attempt to parse a time
				if (current_command[3] <= '9' && current_command[3] >= '0') {
					hours = 10*(((int)'0')+current_command[3]);
					if (current_command[4] <= '9' && current_command[4] >= '0') {
						hours += (((int)'0')+current_command[4]);
						if (current_command[5] == ':') {
							if (current_command[6] <= '9' && current_command[6] >= '0') {
								minutes += 10*(((int)'0')+current_command[6]);
									if (current_command[7] <= '9' && current_command[7] >= '0') {
										minutes += (((int)'0')+current_command[7]);
										if (current_command[8] == ':') {
											if (current_command[9] <= '9' && current_command[9] >= '0') {
												seconds += 10*(((int)'0')+current_command[9]);
													if (current_command[10] <= '9' && current_command[10] >= '0') {
														seconds += (((int)'0')+current_command[10]);
														if (current_command[11] == '\0') {
															// start CRT
															error = 0;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
			
			// if incorrect input, complain about the bad command
			if (error) {
				uart0_put_string("\r\n\"");
				uart0_put_string((unsigned char*)current_command);
				uart0_put_string("\" is not a valid wall clock command.  Please try again.\r\n");
			}
		}
		
		ret_val = release_processor();
	}
	
	return;
}

void KCD (void) {			//pid 12
	int ret_val = 0;
	int this_pid = 12;
	PCB* this_pcb = get_pcb_from_pid(this_pid);
	int commandIndex = 0;
	
	while (1) {
		
		if(next_command_char != '\r') {
			// read in a character, add it to input
			input[commandIndex++] = next_command_char;
			uart0_put_char(next_command_char);
			
			// abort if we try to enter a command that's too long
			if(commandIndex > 255) {
				uart0_put_string("\r\n");
				in_command_mode = 0;
			}
		}
		else if (next_command_char != '\n') {
			int i = 0;
			
			// attempt to interpret command in input buffer
			if (next_command[0] == 'W' || next_command[0] == 'w') {
				current_command = next_command;
				set_process_priority(11, 0);
			}
			else {
				uart0_put_string("\r\n\"");
				uart0_put_string((unsigned char*)next_command);
				uart0_put_string("\" is not a recognized command.  Please try again.\r\n");
			}
			// clear the command buffer
			for (i = 0; i < 256; ++i) {
				next_command[i] = 0; 
			}
			in_command_mode = 0;
			commandIndex = 0;
		}
		
		// release processor, retreat into the background
		set_process_priority(12, 4);
		ret_val = release_processor();
	}
}

void CRT (void) {			//pid 13
	int ret_val = 0;
	int this_pid = 13;
	PCB* this_pcb = get_pcb_from_pid(this_pid);
	
	while (1) {
		while(get_num_msg(this_pcb) > 0) {
			Envelope* env;
			Message* msg;
			
			env = q_pop(&(this_pcb->mailbox));
			msg = &(env->msg);
			if (msg->mtype == 2) {
				char* iter = msg->mtext;
				while (msg->mtext != '\0' && msg->mtext != NULL) {//is null check required?
					printf("%c", iter);
					++iter;
				}
			}// does not respond to any other msg type
			
			k_release_memory_block(env);
		}
		printf("HERE10\r\n");
		ret_val = release_processor();
	}	
}

void Timer_i (void) {
	int ret_val = 0;
	int this_pid = 14;
	PCB* this_pcb = get_pcb_from_pid(this_pid);
	
	while (1) {
		Envelope *env = this_pcb->mailbox.last;
		uart1_put_string("We are in the timer_i process\r\n");
		while (env != NULL) {
			int time_to_send;
			PCB* send_to;
			time_to_send = env->delay + env->timestamp;
			if (time_to_send >= g_timer_count) {
				send_to = get_pcb_from_pid(env->destination_pid);
				send_envelope(send_to, env);
			}
			env = (Envelope *)(env->prev_msg);
		}
		release_from_iprocess();
	}
}

void UART_i (void) {
	int ret_val = 0;
	int this_pid = 15;
	uint8_t g_char_in;
	uint8_t g_char_out;
	uint8_t g_buffer[]= "You Typed a Q\n\r";
	uint8_t *gp_buffer = g_buffer;
	uint8_t g_send_char = 0;
	
	PCB* this_pcb = get_pcb_from_pid(this_pid);

	while (1) {
		
		uint8_t IIR_IntId;	    // Interrupt ID from IIR 		 
		LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
		
	#ifdef DEBUG_0
		//uart0_put_string("Entering c_UART0_IRQHandler\n\r");
	#endif // DEBUG_0

		IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR
			
		if (IIR_IntId & IIR_RDA) { // Receive Data Avaialbe
			/* read UART. Read RBR will clear the interrupt */
			g_char_in = pUart->RBR;
			g_buffer[12] = g_char_in; // nasty hack
			g_send_char = 1;
			
			// If we're in command mode, send keystrokes to process
			if (in_command_mode) {
				next_command_char = g_char_in;
				set_process_priority(12, 0);
				//q_print_rdy_process();
				//uart0_put_string("=================== BLOCKED QUEUE ===============================\r\n");
				//q_print_blk_mem_process();
			} else {
				#ifdef DEBUG_0
				//uart0_put_string("Reading a char = ");
				//uart0_put_char(g_char_in);
				//uart0_put_string("\n\r");
				#endif // DEBUG_0
				
				// If not in command mode, look for commands
				if ( g_char_in == 'r' ) {
					 q_print_rdy_process();
				} else if ( g_char_in == 'm' ) {
					q_print_blk_mem_process();
				} else if	( g_char_in == 'c' ) {
					q_print_blk_rcv_process();
				}	else if ( g_char_in == '%' ) {
					in_command_mode = 1;
					uart0_put_string("%");
				}
			}	
		} else if (IIR_IntId & IIR_THRE) {
		/* THRE Interrupt, transmit holding register becomes empty */

			if (*gp_buffer != '\0' ) {
				g_char_out = *gp_buffer;
	#ifdef DEBUG_0
				uart0_put_string("Writing a char = ");
				uart0_put_char(g_char_out);
				uart0_put_string("\n\r");
				
				// you could use the printf instead
				//printf("Writing a char = %c \n\r", g_char_out);
	#endif // DEBUG_0
				pUart->THR = g_char_out;
				gp_buffer++;
			} else {
	#ifdef DEBUG_0
				uart0_put_string("Finish writing. Turning off IER_THRE\n\r");
	#endif // DEBUG_0
				pUart->IER ^= IER_THRE; // toggle the IER_THRE bit 
				pUart->THR = '\0';
				g_send_char = 0;
				gp_buffer = g_buffer;		
			}
					
		} else {  /* not implemented yet */
	#ifdef DEBUG_0
				//uart0_put_string("Should not get here!\n\r");
	#endif // DEBUG_0
			//return;
		}	

		//uart0_put_string("THIS WORKS\r\n");
		release_from_iprocess();
	}
}
