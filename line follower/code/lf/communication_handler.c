#include <avr/interrupt.h>
#include <avr/io.h>
#include "communication_handler.h"

#include "scheduler.h"
#include "pwm.h"



uint8_t uart_buffer[UART_RX_MAX];



#define AN_ZERO 			(uint8_t)0
#define AN_COM_PROT			(uint8_t)1
#define AN_CMD_START		(uint8_t)2
#define AN_START1			(uint8_t)3
#define AN_STOP1			(uint8_t)4
#define AN_CMD_STOP			(uint8_t)5

void communication_init();
void _analyse_cmd(uint8_t cmd_char);


void communication_handler_init()
{


	
}

uint8_t an_state = AN_ZERO;
void _analyse_cmd(uint8_t cmd_char)
{



	switch(an_state)
	{
		case AN_ZERO:
		{
			if(cmd_char == 0xA0){
				an_state = AN_COM_PROT;
			}
			else{
				an_state = AN_ZERO;
			}
		}
		break;
		case AN_COM_PROT:
		{
			if( (cmd_char == 0x0) || (cmd_char == 0x2) ){
				an_state = AN_CMD_STOP;
			}
			else if( (cmd_char == 0x82) || (cmd_char == 0x80) ){
				an_state = AN_CMD_START;
			}
			else{
				an_state = AN_ZERO;
			}

		}
		break;
		case AN_CMD_STOP:
		{
			if( cmd_char == 0x0A){
				an_state = AN_STOP1;
			}
			else{
				an_state = AN_ZERO;
			}
		}
		break;
		case AN_STOP1:
		{
			if(cmd_char == 0x00){
				sch_setTaskStatus(TASK_1,DISABLE_TASK);
				pwm_setMotors(MOTOR_FORWARD,0,MOTOR_FORWARD,0);
			}
			else{
				an_state = AN_ZERO;
			}
		}
		break;
		case AN_CMD_START:
		{
			if(cmd_char == 0x2){
				an_state = AN_START1;
			}
			else{
				an_state = AN_ZERO;
			}
		}
		break;
		case AN_START1:
		{
			if(cmd_char == 0x0){
				sch_setTaskStatus(TASK_1,ENABLE_TASK);
				an_state = AN_ZERO;
			}
			else{
				an_state = AN_ZERO;
			}
			
		}
		break;
		default:
		{

		}
		break;
	}

}




void communication_handler_task()
{	
	//get all rx commands
	uint8_t available_data = 0;
	available_data = uart_rx_flush(&uart_buffer[0]);

	for(uint8_t idx = 0; idx < available_data; idx++)
	{
		_analyse_cmd(uart_buffer[idx]);
		uart_buffer[idx] = 0;
	}
	
}
