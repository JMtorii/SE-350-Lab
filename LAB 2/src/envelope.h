/**
 * @file:   envelope.h
 * @brief:  Implementation of envelopes for messages
 * @author: TEAM BLACKJACK (21)
 * @date:   2014/01/17
 */

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "k_rtx.h"
#include "queue.h"
#include "k_process.h"

extern PCB *gp_current_process;
extern int g_timer_count;

typedef struct Message {
	int mtype; /* user defined message type */
	char *mtext; /* body of the message */
} Message;

typedef struct Envelope {
	void* prev_msg;
	int sender_pid;
	int destination_pid;
	int timestamp;
	Message msg;
} Envelope;

void create_envelope(Envelope *env, int sender_pid, int destination_pid, int mtype, char* mtext);

void send_message(int receiving_pid, Envelope* env);

int delayed_send(int receiving_pid, Envelope* env, int delay);

Envelope* receive_message(void);

Envelope* receive_message_nonblocking(void);

int get_num_msg(PCB * pcb);

#endif
