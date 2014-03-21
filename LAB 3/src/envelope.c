/**
 * @file:   envelope.c
 * @brief:  Implementation of envelopes for messages
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#include "envelope.h"
#include "uart.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void *create_envelope(Envelope* env, void *msg, int sender_pid, int destination_pid) {
	//overwrite memblock of user envelope and wrap in our envelope
	env->prev_msg = NULL;
	env->sender_pid = sender_pid;
	env->destination_pid = destination_pid;
	env->timestamp = g_timer_count;
	env->delay = 0;
	env->msg = (Message*)msg;
	
	return env;
}

int k_send_message(int receiving_pid, void* msg) {
	int this_pid = gp_current_process->m_pid;
	PCB *receiving_proc = get_pcb_from_pid(receiving_pid);
	void* env;
	
	if (((Message *)msg)->mtype == 2) {
			k_set_process_priority(13, 0);
	}
	
	// Allocate for envelope
  env = (Envelope*)k_request_memory_block();
	atomic_on();
	// Create envelope wrapping message 
	env = create_envelope(env, msg, this_pid, receiving_proc->m_pid);
	// Send envelope to receiving proc
	send_envelope(receiving_proc, env);
	atomic_off();
	
	return RTX_OK;
}

void* k_receive_message(int* sender_id) {	
	Envelope* env;
	Message* returnMessage;
	atomic_on();
	
	// Check if any in mailbox
  while( gp_current_process->mailbox.first == NULL) {
  	gp_current_process->m_state = BLK_ON_RCV;
			
  	q_push(&blocked_rcv_queue[k_get_process_priority(gp_current_process->m_pid)],gp_current_process);
  	atomic_off();
 	  k_release_processor();
  }
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	*sender_id = env->sender_pid;
	//printf("PID in env: %d, PID return: %d\r\n",env->sender_pid,*sender_id);
	returnMessage = env->msg;
	atomic_off();
	//printf("Received at: %d\r\n", g_timer_count);
	k_release_memory_block(env);
	
	return (void *)returnMessage;
}

void* k_receive_message_nonblocking(int* sender_id) {	
	Envelope* env;
	Message* returnMessage;
	
	// Check if any in mailbox
  if( gp_current_process->mailbox.first == NULL) {
		return NULL;
  }
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	*sender_id = env->sender_pid;
	//printf("PID in env: %d, PID return: %d\r\n",env->sender_pid,*sender_id);
	returnMessage = env->msg;
	//printf("Received at: %d\r\n", g_timer_count);
	k_release_memory_block_nonblocking(env);
	
	return (void *)returnMessage;
}

int k_delayed_send(int receiving_pid, void* msg, int delay) {
	int this_pid;
	PCB *receiving_proc;
	void* env;
	
	// Allocate for envelope
  env = (Envelope*)k_request_memory_block();
  atomic_on();
  this_pid = gp_current_process->m_pid;
  receiving_proc = get_pcb_from_pid(receiving_pid);
  env = create_envelope(env, msg, this_pid, receiving_proc->m_pid);
  ((Envelope *)env)->delay = delay;

  send_envelope(get_pcb_from_pid(14), env);
  atomic_off();
	
	return RTX_OK;
}

void send_envelope(PCB *receiving_proc, Envelope *env) {
	q_push(&(receiving_proc->mailbox), env);
	if (receiving_proc->m_state == BLK_ON_RCV) {
		receiving_proc->m_state = RDY;
		q_remove_pid(receiving_proc->m_pid);
		q_push(&ready_queue[k_get_process_priority(receiving_proc->m_pid)],receiving_proc);
	}
}

int get_num_msg(PCB * pcb) {
	Envelope* env;
	int ret;
	env = pcb->mailbox.last;
	if (env == NULL) {
		return 0;
	}
	while (env != pcb->mailbox.first) {
		++ret;
		env = (Envelope*)(env->prev_msg);
	}
	++ret;
	return ret;
}
