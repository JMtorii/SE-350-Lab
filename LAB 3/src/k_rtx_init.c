/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */
 
#include <LPC17xx.h>
#include "k_rtx_init.h"
#include "uart_polling.h"
#include "uart.h"
#include "k_memory.h"
#include "k_process.h"

void k_rtx_init(void)
{
        __disable_irq();
				uart_irq_init(0);   // uart0, interrupt-driven 
        uart1_init();   
        memory_init();
        process_init();
        __enable_irq();
	
				//NVIC_DisableIRQ(UART0_IRQn); // Remove this when timer interrupts work
				
				uart1_put_string("\n\n\r");
	
	/* start the first process */
        k_release_processor();
}
