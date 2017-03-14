#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


#include "pwm.h"
#include "QTR.h"
#include "uart.h"
#include "scheduler.h"
#include "pid.h"
#include "communication_handler.h"

void calculate_proportional();

uint8_t temp = 0;
int8_t offset = 8;
uint8_t sensor_value = 0;
uint8_t last_value = 0;

int8_t last_offset = 8;
int8_t output_pid = 0;

uint8_t left_speed = 0;
uint8_t right_speed = 0;

uint8_t speed = 60;



void alg()
{
	char pos =0;
	short mask;
	//center
	sensor_value = QTR_compute();
	
	mask = 1 << last_offset;

	do
	{
		if (sensor_value & (mask << pos))
		{
			offset = last_offset + pos;		
			break;
		}
		else if (sensor_value & (mask >> pos))
		{
			offset = last_offset - pos;		
			break;
		}
		else
		{
			pos++;
		}
		
	}while (pos<8);	
	
	if (8 == pos)
	{
		offset = last_offset;
	}
	
	last_offset = offset;
	output_pid = pid_calculate((int8_t)4 - offset);


	if(output_pid > 0) // increase left motor speed
	{
			left_speed = speed + output_pid;
			if(output_pid > 100){
				pwm_setMotors(MOTOR_BACKWARD,left_speed,MOTOR_FORWARD,left_speed);
			}
			else{
					
				pwm_setMotors(MOTOR_FORWARD,speed - (left_speed * 40/100),MOTOR_FORWARD,left_speed);
			}
	}
	else if(output_pid < 0) //right motor
	{
		output_pid *= (-1);

		right_speed = speed + output_pid;

		if(output_pid > 80){
				pwm_setMotors(MOTOR_FORWARD,right_speed,MOTOR_BACKWARD,right_speed);
		}
		else{
				pwm_setMotors(MOTOR_FORWARD,right_speed,MOTOR_FORWARD,speed- (right_speed * 40/100));
		}

	
	}
	else{
		pwm_setMotors(MOTOR_FORWARD,speed,MOTOR_FORWARD,speed);
	}


}


int main()
{

	pwm_init();
	uart_init(259);
	QTR_init();
	sch_init();
	pid_init();

	


	sch_setTask(alg,0,1);
	sch_setTask(communication_handler_task,1,20);
	sch_setTaskStatus(TASK_2,ENABLE_TASK);
	pid_setpoint(0);
	pid_set_consts(7000,0,15000);

	
	sei(); 	
	

	while(1)
	{

		sch_main();

		
	}


}


