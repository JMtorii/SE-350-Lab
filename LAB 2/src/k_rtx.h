/** 
 * @file:   k_rtx.h
 * @brief:  kernel deinitiation and data structure header file
 * @auther: TEAM BLACKJACK
 * @date:   2014/01/17
 */

#include "queue.h"
 
#ifndef K_RTX_H_
#define K_RTX_H_

/*----- Definitions -----*/
#define RTX_ERR -1
#define RTX_OK  0

#define NULL 0
#define NUM_TEST_PROCS 7
#define NUM_SYS_PROCS 4

#define NUM_PRIORITIES 5

#define DEFAULT 0
#define KCD_REG 1
#define CRT_DISP 2

#ifdef DEBUG_0
#define USR_SZ_STACK 0x200         /* user proc stack size 512B   */
#else
#define USR_SZ_STACK 0x100         /* user proc stack size 218B  */
#endif /* DEBUG_0 */

/*----- Types -----*/
typedef unsigned char U8;
typedef unsigned int U32;

/* process states, note we only assume three states in this example */
typedef enum {NEW = 0, RDY, BLK, BLK_ON_RCV, RUN, EXT} PROC_STATE_E;  

/*
  PCB data structure definition.
  You may want to add your own member variables
  in order to finish P1 and the entire project 
*/


typedef struct pcb 
{ 
	void *prev;
	U32 *mp_sp;		/* stack pointer of the process */
	U32 m_pid;		/* process id */
	PROC_STATE_E m_state;   /* state of the process */  
	Queue mailbox;
} PCB;

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	       		  /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

#define __SVC_0  __svc_indirect(0)
extern int k_release_processor(void);
extern int __SVC_0 _release_processor(U32 p_func);
#define release_processor() _release_processor((U32)k_release_processor)

extern int k_release_from_iprocess(void);
extern int __SVC_0 _release_from_iprocess(U32 p_func);
#define release_from_iprocess() _release_from_iprocess((U32)k_release_from_iprocess)

extern int k_release_into_iprocess(void);
extern int __SVC_0 _release_into_iprocess(U32 p_func);
#define release_into_iprocess() _release_into_iprocess((U32)k_release_into_iprocess)

#endif // ! K_RTX_H_
