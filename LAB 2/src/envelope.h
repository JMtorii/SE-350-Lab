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

typedef struct Envelope {
	struct Envelope* prev_msg;
	int sender_pid;
	int destination_pid;
	int mtype; /* user defined message type */
	char *mtext; /* body of the message */
} Envelope;

void create_envelope(Envelope *env, int sender_pid, int destination_pid, int mtype, char* mtext);

void send_message(int receiving_pid, Envelope* env);

Envelope* receive_message(void);

#endif
