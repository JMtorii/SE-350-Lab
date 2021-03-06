/**
 * @brief timer.h - Timer header file
 * @author Y. Huang
 * @date 2013/02/12
 */
#ifndef _TIMER_H_
#define _TIMER_H_

#include "k_rtx.h"

extern PCB* get_pcb_from_pid(int pid);
extern uint32_t k_timer_init ( uint8_t n_timer );  /* initialize timer n_timer */
extern PCB *p_pcb_old;
extern PCB *gp_current_process;

#endif /* ! _TIMER_H_ */
