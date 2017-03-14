#include "pid.h"

#define SET_POINT (int16_t)0
volatile int16_t set_point = 0;
volatile int16_t error;
volatile int16_t prv_error;
volatile int16_t _i = 0;//10;
volatile int16_t _d = 5000;
volatile int16_t _p = 1000;

volatile int32_t temp;
volatile int16_t integrate = 0;
volatile int16_t integrate_count = 0;
volatile int16_t derivate;
volatile int8_t output;

void pid_init()
{

}

void pid_setpoint(int16_t point)
{
	set_point = point;
}

void pid_set_consts(int16_t p, int16_t i, int16_t d)
{
	_p = p;
	_i = i;
	_d = d;
}

int8_t pid_calculate(int16_t value)
{
	error = set_point - (int16_t)value;
	
//	if(integrate_count == 5)
	{
		integrate += error;
//		integrate_count = 0;
	}
	integrate_count++;

	derivate = (error - prv_error);

	temp = (int32_t)((int32_t)((int32_t)_p * (int32_t)error) + (int32_t)_d * (int32_t)derivate + (int32_t)_i * (int32_t)integrate); 
	if(temp > (int32_t)32767)
	{
		temp = 32767;
	}
	else if(temp < (int32_t) -32767)
	{
		temp = -32767;
	}


	output = (int8_t)(((uint32_t)temp * (uint32_t)127) / (uint32_t)(32767) );
	
	

	prv_error = error;
	return output;
}
