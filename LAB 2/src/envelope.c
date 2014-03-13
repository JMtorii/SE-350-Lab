/**
 * @file:   envelope.c
 * @brief:  Implementation of envelopes for messages
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#include "envelope.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void *create_envelope(void *msg, int sender_pid, int destination_pid) {
	int mtype;
	char *mtext;
	Envelope *env;
	
	mtype = ((Message*)msg)->mtype;
	mtext = ((Message*)msg)->mtext;
	
	env = (Envelope*)k_request_memory_block();
	
	//overwrite memblock of user envelope and wrap in our envelope
	env->prev_msg = NULL;
	env->sender_pid = sender_pid;
	env->destination_pid = destination_pid;
	env->timestamp = g_timer_count;
	env->delay = 0;
	env->msg.mtype = mtype;
	env->msg.mtext = mtext;
	
	// deallocating old "msg" here
	// TODO: verify whether this should be done here,
	// or by the sending process after the message is no longer needed
	k_release_memory_block(msg); 
	
	return env;
}

void k_send_message(int receiving_pid, void* msg) {
	/*atomic ( on ) ;
	set sender_procid , destination_procid ;
	pcb_t *receiving_proc = get_pcb_from_pid ( receiving_pid ) ;
	enqueue env onto the msg_queue of receiving_proc ;
	if ( receiving_proc->state is BLOCKED_ON_RECEIVE ) {
		set receiving_proc state to ready ;
		rpq_enqueue ( receiving_proc ) ;
	}
	atomic ( off ) ;*/
	
	int this_pid = gp_current_process->m_pid;
	PCB *receiving_proc = get_pcb_from_pid(receiving_pid);
	void* env = create_envelope(msg, this_pid, receiving_proc->m_pid);

	send_envelope(receiving_proc, env);
}

void* k_receive_message(int* sender_id) {
	/*atomic ( on ) ;
	while ( current_process msg_queue is empty ) {
		set current_process state to BLOCKED_ON_RECEIVE ;
		release_processor ( ) ;
	}
	msg_t *env = dequeue current_process msg queue ;
	atomic ( off ) ;
	return env ;*/
	
	Envelope* env;
	Message* returnMessage;
	//atomic(on)
	while( gp_current_process->mailbox.first == NULL) {
		gp_current_process->m_state = BLK_ON_RCV;
		q_push(&blocked_rcv_queue[k_get_process_priority(gp_current_process->m_pid)],gp_current_process);
		release_processor();
	}
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	*sender_id = env->sender_pid;
	printf("PID in env: %d, PID return: %d\r\n",env->sender_pid,*sender_id);
	//atomic(off)
	returnMessage = (Message *)k_request_memory_block();
	*returnMessage = env->msg;
	
	printf("Received at: %d\r\n", g_timer_count);
	
	k_release_memory_block(env);
	return (void *)returnMessage;
}

Envelope* receive_message_nonblocking(void) {
	
	Envelope* env;
	//atomic(on)
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	//atomic(off)
	return env;
	
}

int k_delayed_send(int receiving_pid, void* msg, int delay) {

	int this_pid = gp_current_process->m_pid;
	PCB *receiving_proc = get_pcb_from_pid(receiving_pid);
	void* env = create_envelope(msg, this_pid, receiving_proc->m_pid);
	PCB *timer_i = get_pcb_from_pid(14);
	((Envelope *)env)->delay = delay;
	
	printf("Current Time: %d, Delay: %d\r\n", g_timer_count, delay);
	
	send_envelope(timer_i, env);
	return RTX_OK;
}

void send_envelope(PCB *receiving_proc, Envelope *env) {
	//atomic(on);
	q_push(&(receiving_proc->mailbox), env);
	if (receiving_proc->m_state == BLK_ON_RCV) {
		q_remove_pid(receiving_proc->m_pid);
		receiving_proc->m_state = RDY;
		q_push(&ready_queue[k_get_process_priority(receiving_proc->m_pid)],receiving_proc);
	}
	//atomic(off);
}

int get_num_msg(PCB * pcb) {
	Envelope* env;
	int ret;
	env = pcb->mailbox.last;
	printf("HERE3\r\n");
	if (env == NULL) {
		return 0;
	}
	printf("HERE4\r\n");
	while (env != pcb->mailbox.first) {
		++ret;
		env = (Envelope*)(env->prev_msg);
	}
	printf("HERE5\r\n");
	++ret;
	printf("%d\r\n", ret);
	return ret;
}
