/* @brief: rtx.h User API prototype, this is only an example
 * @author: TEAM BLACKJACK
 * @date: 2014/01/17
 */
#ifndef RTX_H_
#define RTX_H_

/* ----- Definitations ----- */
#define RTX_ERR -1
#define NULL 0
#define NUM_TEST_PROCS 9
#define NUM_SYS_PROCS 6

/* Process Priority. The bigger the number is, the lower the priority is*/
#define INTERRUPT -1
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

#define DEFAULT 0
#define KCD_REG 1
#define CRT_DISP 2
/* ----- Types ----- */
typedef unsigned int U32;

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

/* ----- RTX User API ----- */
#define __SVC_0  __svc_indirect(0)

extern void k_rtx_init(void);
#define rtx_init() _rtx_init((U32)k_rtx_init)
extern void __SVC_0 _rtx_init(U32 p_func);

extern unsigned int k_timer_init(unsigned char n_timer);
#define timer_init(n_timer) _timer_init((U32)k_timer_init, n_timer)
extern int __SVC_0 _timer_init(U32 p_func, unsigned char n_timer);

extern int k_release_from_iprocess(void);
#define release_from_iprocess() _release_from_iprocess((U32)k_release_from_iprocess)
extern int __SVC_0 _release_from_iprocess(U32 p_func);

extern int k_release_into_iprocess(void);
#define release_into_iprocess() _release_into_iprocess((U32)k_release_into_iprocess)
extern int __SVC_0 _release_into_iprocess(U32 p_func);

extern int k_release_processor(void);
#define release_processor() _release_processor((U32)k_release_processor)
extern int __SVC_0 _release_processor(U32 p_func);

extern void *k_request_memory_block(void);
#define request_memory_block() _request_memory_block((U32)k_request_memory_block)
extern void *_request_memory_block(U32 p_func) __SVC_0;

extern void *k_request_memory_block_nonblocking(void);
#define request_memory_block_nonblocking() _request_memory_block_nonblocking((U32)k_request_memory_block_nonblocking)
extern void *_request_memory_block_nonblocking(U32 p_func) __SVC_0;

/* __SVC_0 can also be put at the end of the function declaration */

extern int k_release_memory_block(void *);
#define release_memory_block(p_mem_blk) _release_memory_block((U32)k_release_memory_block, p_mem_blk)
extern int _release_memory_block(U32 p_func, void *p_mem_blk) __SVC_0;

extern int k_release_memory_block_nonblocking(void *);
#define release_memory_block_nonblocking(p_mem_blk) _release_memory_block_nonblockingU32)k_release_memory_block_nonblocking, p_mem_blk)
extern int _release_memory_block_nonblocking(U32 p_func, void *p_mem_blk) __SVC_0;

extern int k_set_process_priority(int process_id, int priority);
#define set_process_priority(process_id, priority) _set_process_priority((U32)k_set_process_priority, process_id, priority)
extern int _set_process_priority(U32 p_func, int process_id, int priority) __SVC_0;

extern int k_get_process_priority(int process_id);
#define get_process_priority(process_id) _get_process_priority((U32)k_get_process_priority, process_id)
extern int _get_process_priority(U32 p_func, int process_id) __SVC_0;

extern int k_send_message(int pid, void *p_msg);
#define send_message(pid, p_msg) _send_message((U32)k_send_message, pid, p_msg)
extern int _send_message(U32 p_func, int pid, void *p_msg) __SVC_0;

extern void *k_receive_message(int *p_pid);
#define receive_message(p_pid) _receive_message((U32)k_receive_message, p_pid)
extern void *_receive_message(U32 p_func, void *p_pid) __SVC_0;

extern void *k_receive_message_nonblocking(int *p_pid);
#define receive_message_nonblocking(p_pid) _receive_message_nonblocking((U32)k_receive_message_nonblocking, p_pid)
extern void *_receive_message_nonblocking(U32 p_func, void *p_pid) __SVC_0;

extern int k_delayed_send(int pid, void *p_msg, int delay);
#define delayed_send(pid, p_msg, delay) _delayed_send((U32)k_delayed_send, pid, p_msg, delay)
extern int _delayed_send(U32 p_func, int pid, void *p_msg, int delay) __SVC_0;

#endif /* !RTX_H_ */
