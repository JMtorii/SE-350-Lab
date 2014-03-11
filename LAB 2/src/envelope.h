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
extern volatile unsigned int g_timer_count;

typedef struct Message {
	int mtype; /* user defined message type */
	char *mtext; /* body of the message */
} Message;

typedef struct Envelope {
	void* prev_msg;
	int sender_pid;
	int destination_pid;
	int timestamp;
	int delay;
	Message msg;
} Envelope;

void send_message(int receiving_pid, void* env);

int delayed_send(int receiving_pid, void* env, int delay);

void* receive_message(int *sender_id);

Envelope* receive_message_nonblocking(void);

void *create_envelope(void *msg, int sender_pid, int destination_pid);

void send_envelope(PCB *receiving_proc, Envelope *env);

int get_num_msg(PCB * pcb);

#endif
