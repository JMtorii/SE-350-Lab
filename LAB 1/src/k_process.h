/**
 * @file:   k_process.h
 * @brief:  process management hearder file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: Assuming there are only two user processes in the system
 */

#ifndef K_PROCESS_H_
#define K_PROCESS_H_

#include "k_rtx.h"
#include "queue.h"

/* ----- Definitions ----- */

#define INITIAL_xPSR 0x01000000        /* user process initial xPSR value */
//PCB *gp_current_process; /* always point to the current RUN process */
/* ----- Functions ----- */

void process_init(void);               /* initialize all procs in the system */
PCB *scheduler(void);                  /* pick the pid of the next to run process */
int k_release_process(void);           /* kernel release_process function */
int k_get_process_priority(int process_id);
int k_set_process_priority(int process_id, int priority);
int get_process_priority(int process_id);
int set_process_priority(int process_id, int priority);


void add_null_process(void);
void null(void);

extern U32 *alloc_stack(U32 size_b);   /* allocate stack for a process */
extern void __rte(void);               /* pop exception stack frame */
extern void set_test_procs(void);      /* test process initial set up */
extern Queue *blocked_queue;
extern Queue *ready_queue;

#endif /* ! K_PROCESS_H_ */
