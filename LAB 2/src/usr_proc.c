/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: TEAM BLACKJACK
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

typedef struct msg {
	int mtype;
	char *mtext;
} msg;

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
int test_results[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
		test_results[i] = 1;
		
		// Testing with different priorities on processes
		if (i == 3) {  // Memory blocks test
			g_test_procs[i].m_priority=3 ;
  	}
		if (i == 4) {  // Memory blocking processes
			g_test_procs[i].m_priority=4;
  	}
		if (i == 5) { // Send message process
			g_test_procs[i].m_priority=3;
		}
		if (i == 6) { // Recieve message process	
			g_test_procs[i].m_priority=3;
		}
	}

	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &priority_test;
	g_test_procs[3].mpf_start_pc = &memory_block_test;
	g_test_procs[4].mpf_start_pc = &blocked_test;
	g_test_procs[5].mpf_start_pc = &message_send_test;
	g_test_procs[6].mpf_start_pc = &message_receive_test;
	
	uart1_put_string("\n\r");
	uart1_put_string("G021_test: START\r\n");
}

// Proc to print test results and terminate program
void print_test_results() {
	int i = 0;
	int pass = 0;
	for (i = 0;i < NUM_TEST_PROCS;i++) {
		if (test_results[i] == 1) {
			printf("G021_test: test %d OK\r\n",i+1);
			pass++;
		}
		else {
			printf("G021_test: test %d FAIL\r\n",i+1);
		}
	}
	printf("G021_test: %d/%d tests OK\r\n",pass,NUM_TEST_PROCS);
	printf("G021_test: %d/%d tests FAIL\r\n",NUM_TEST_PROCS-pass,NUM_TEST_PROCS);
	printf("G021_test: END");
}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc1(void)
{
	int i = 0;
	int ret_val = 10;
	while (1) {
		if ( i != 0 && i%5 == 0 ) {
			uart1_put_string("\n\r");
	
			ret_val = release_processor();
			
			#ifdef DEBUG_0
				uart1_put_string("Process 1 completed!\r\n");
			#endif /* DEBUG_0 */
			
			if (ret_val == -1) {
				uart1_put_string("G021_test: test 1 FAIL\r\n");
				test_results[0] = 0;
			} else {
				uart1_put_string("G021_test: test 1 OK\r\n");
			}
		}
		
		uart1_put_char('A' + i%26);
		i++;
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc2(void)
{
	int i = 0;
	int ret_val = 20;
	
	while (1) {
		if ( i != 0 && i%5 == 0 ) {
			uart1_put_string("\n\r");
			ret_val = release_processor();
			
			#ifdef DEBUG_0
				uart1_put_string("Process 2 completed!\r\n");
			#endif /* DEBUG_0 */
			
			if (ret_val == -1) {
				uart1_put_string("G021_test: test 2 FAIL\r\n");
				test_results[1] = 0;
			} else {
				uart1_put_string("G021_test: test 2 OK\r\n");
			}
		}
		uart1_put_char('0' + i%10);
		i++;
	}
}

/**
 * @brief: Sets priority for process and returns get value
 */
void priority_test(void)
{	
	int ret_val = 50;
	int i = 0;
	//set_process_priority(5,1);
	
	while(1) {	
		for (i = 1;i < 3;i++) {
			set_process_priority(3,i);
			#ifdef DEBUG_0
				uart1_put_string("PRIORITY SET!\r\n");
			#endif /* DEBUG_0 */
		}
		
		#ifdef DEBUG_0
			uart1_put_string("Process 3 completed!\r\n");
		#endif /* DEBUG_0 */
		
		set_process_priority(3,3);
		ret_val = release_processor();
		//printf("Timer result: %d \r\n",g_timer_count);
		if (ret_val == -1) {
			uart1_put_string("G021_test: test 3 FAIL\r\n");
			test_results[2] = 0;
		} else {
			uart1_put_string("G021_test: test 3 OK\r\n");
		}
	}
}

/**
 * @brief: Requests memory blocks and returns
 */
void memory_block_test(void)
{	
	int ret_val = 60;
	int i = 0;
	int* tmp_int;
	char* tmp_string;
	
	while(1) {
		uart1_put_string("Iterating through proc 4 with\r\n");
		tmp_int = request_memory_block();
		tmp_string = request_memory_block();
		i++;
		*tmp_int = i;
		if (i > 1337) {
			i = 0;
		}
		tmp_string[0] = 'H';
		tmp_string[1] = 'E';
		tmp_string[2] = 'L';
		tmp_string[3] = 'L';
		tmp_string[4] = 'O';
		tmp_string[5] = '\0';
		
		#ifdef DEBUG_0
			uart1_put_string("Proc 4 completed!\r\n");
			/*printf("INT!   Address: 0x%x, Value: %d\r\n",tmp_int,*tmp_int);
			printf("CHAR[]! Address: 0x%x, Value: %s\r\n",&tmp_string,tmp_string);*/
		#endif /* DEBUG_0 */
		
		release_memory_block(tmp_int);
		release_memory_block(tmp_string);
		
		ret_val = release_processor();
		
		if (ret_val == -1) {
			uart1_put_string("G021_test: test 4 FAIL\r\n");
			test_results[3] = 0;
		} else {
			uart1_put_string("G021_test: test 4 OK\r\n");
		}
	}
}

/**
 * @brief: Requests memory blocks and returns, this will block all memory related processes
 */
void blocked_test(void)
{	
	int ret_val = 70;
	int i = 0;
	int* tmp_int;
	
	while(1) {
		tmp_int = request_memory_block();
		
		i++;
		*tmp_int = i;
		if (i > 1337) {
			i = 0;
		}
		#ifdef DEBUG_0
			uart1_put_string("Proc 5 completed!\r\n");
			//printf("KILL!   Address: 0x%x, Value: %d\r\n",tmp_int,*tmp_int);
		#endif /* DEBUG_0 */
		
		//release_memory_block(tmp_int);
		ret_val = release_processor();
		
		if (ret_val == -1) {
			uart1_put_string("G021_test: test 5 FAIL\r\n");
			test_results[4] = 0;
		} else {
			uart1_put_string("G021_test: test 5 OK\r\n");
		}
		
		if (i == 5) {
			//print_test_results();
		}
	}
}
	
void message_send_test(void) {
	int ret_val = 80;
	
	while(1) {
		msg* testMessage = (msg*)request_memory_block();
		testMessage->mtype = 0;
		testMessage->mtext = "Vegetables are my enemy.";
		
		/*msg* testMessage2 = (msg*)request_memory_block();
 		testMessage2->mtype = 0;
		testMessage2->mtext = "MSG is my favourite vitamin.";*/
		
		uart1_put_string("Preparing to send message...");
		send_message(7, testMessage);
		//delayed_send(7, testMessage, 60);
		uart1_put_string("Message sent!\r\n");
		
		//send_message(7, testMessage2);
		//delayed_send(7, testMessage2, 100);
		
		#ifdef DEBUG_0
			uart1_put_string("Process 6 completed!\r\n");
		#endif /* DEBUG_0 */
		
		ret_val = release_processor();
	}
}
	
void message_receive_test(void) {
	int ret_val = 90;
	
	while(1) {
	  int i = 0;
		int sender_id;
	
		msg* rcvMessage;

		rcvMessage = (msg*)(receive_message(&sender_id));
		//printf("SenderID: %d Message:",sender_id);
		while (rcvMessage->mtext[i] != '\0') {
			uart1_put_char(rcvMessage->mtext[i++]);
		}
		release_memory_block(rcvMessage);
		
		#ifdef DEBUG_0
			uart1_put_string("Process 7 completed!\r\n");
		#endif /* DEBUG_0 */
		
		ret_val = release_processor();
	}
}
