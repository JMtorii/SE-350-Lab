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

void create_envelope(Envelope *env, int sender_pid, int destination_pid, int mtype, char *mtext) {
	env->prev_msg = NULL;
	env->sender_pid = sender_pid;
	env->destination_pid = destination_pid;
	env->mtype = mtype;
	env->mtext = mtext;
}

void send_message(int receiving_pid, Envelope* env) {
	/*atomic ( on ) ;
	set sender_procid , destination_procid ;
	pcb_t *receiving_proc = get_pcb_from_pid ( receiving_pid ) ;
	enqueue env onto the msg_queue of receiving_proc ;
	if ( receiving_proc->state is BLOCKED_ON_RECEIVE ) {
		set receiving_proc state to ready ;
		rpq_enqueue ( receiving_proc ) ;
	}
	atomic ( off ) ;*/
	
	//atomic(on);
	PCB *receiving_proc = get_pcb_from_pid(receiving_pid);
	q_push(&(receiving_proc->mailbox), env);
	if (receiving_proc->m_state == BLK_ON_RCV) {
		receiving_proc->m_state = RDY;
    q_push(&ready_queue[get_process_priority(receiving_proc->m_pid)],receiving_proc);
  }
	//atomic(off);
}

Envelope* receive_message(void) {
	/*atomic ( on ) ;
	while ( current_process msg_queue is empty ) {
		set current_process state to BLOCKED_ON_RECEIVE ;
		release_processor ( ) ;
	}
	msg_t *env = dequeue current_process msg queue ;
	atomic ( off ) ;
	return env ;*/
	
	Envelope* env;
	//atomic(on)
	while( gp_current_process->mailbox.first == NULL) {
		gp_current_process->m_state = BLK_ON_RCV;
		release_processor();
	}
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	//atomic(off)
	return env;
	
}

Envelope* receive_message_nonblocking(void) {
	
	Envelope* env;
	//atomic(on)
	env = (Envelope*)q_pop(&(gp_current_process->mailbox));
	//atomic(off)
	return env;
	
}

int delayed_send(int receiving_pid, Envelope* env, int delay) {
	int start_time = g_timer_count;
	while (1) {
		if (g_timer_count - start_time >= delay) {
			send_message(receiving_pid, env);
			return RTX_OK;
		}
		else {
			release_processor();
		}
	}
	//return RTX_ERR;
}
