#include <avr/io.h>

#define F_CPU 8000000UL

#include <util/delay.h>

#include "dbg_putchar.h"
#include "mcal_at24.h"
#include "thomson_codes.h"

#define IDLE 		0x0
#define CHECK_SIG	0x3
#define START_RCV	0x1
#define DECODE_MSG	0x2

void init_turbine();

void _sys_init(){

	dbg_tx_init();
	init_turbine();

	SET_SENSOR_INPUT;
	SET_LED;

	LED_ON;
	_delay_ms(50);
	LED_OFF;
	_delay_ms(50);
	LED_ON;
	_delay_ms(100);
	LED_OFF;

};




uint8_t signal_periods[75];
uint32_t signal_code = 0;
uint8_t  signal_shifter = 0;
uint16_t turbine_lvl = 500;

volatile uint8_t sig_idx = 0;
volatile uint8_t dec_state = IDLE;
volatile uint16_t hold_counter = 0;
volatile uint8_t msg_counter = 0;
volatile uint8_t msg_end_qalif = 0;

volatile uint8_t sig_lvl = 0;
volatile uint8_t sig_lvl_prev = 0;
volatile uint8_t idx = 0;




void init_turbine(){

	SET_TIMER_REG1;
	SET_TIMER_REG2;


	SET_TOP_LVL_ICR;

	TURBINE_SET_PORT;

	SET_TOP_LVL_OCR;
	_delay_ms(1000);


	TIMER_COMPARE_REG = 0;
	_delay_ms(1000);

	
	TIMER_COMPARE_REG = 0;
	turbine_lvl = 500;
}

uint8_t set_turbine(uint16_t value){
	
	uint8_t ret_val = 0;
	if( (value >= 500) && (value <= 1000)){
		TIMER_COMPARE_REG = value;
		ret_val = 1;
	}

	return ret_val;
}

void stop_turbine(){
	TIMER_COMPARE_REG = 0x0000;
	turbine_lvl = 500;
}

void max_turbine(){
	TIMER_COMPARE_REG = 700;
	turbine_lvl = 700;

	_delay_ms(1000);

	TIMER_COMPARE_REG = 800;
	turbine_lvl = 800;

	_delay_ms(1000);


	TIMER_COMPARE_REG = 900;
	turbine_lvl = 900;

	_delay_ms(500);


	TIMER_COMPARE_REG = 950;
	turbine_lvl = 950;

	_delay_ms(500);

	TIMER_COMPARE_REG = 1000;
	turbine_lvl = 1000;



}



int main(){
	
	_sys_init();	



	while(1){
	
	sig_lvl = GET_DEC_LVL;

	

	switch(dec_state){
		
		case IDLE:
		{
			
			if(sig_lvl == 0){
				dec_state = CHECK_SIG;
				hold_counter = 0;
	
			
			}
			else{
				dec_state = IDLE;
			}
		}
		break;

		case CHECK_SIG:
		{
			
			if(sig_lvl == 0){
				hold_counter++;
			}
			else{
				dec_state = IDLE;
				hold_counter = 0;
			}
			
			if(hold_counter == START_DEC_THRESHOLD){
				dec_state = START_RCV;
				
				hold_counter = 0;
				sig_lvl = 0;
				idx = 0;
				msg_end_qalif = 0;


			}

		}	
		break;

		case START_RCV:
		{
		while(idx < 75){
		
			sig_lvl = GET_DEC_LVL;

			if((sig_lvl != sig_lvl_prev)  || (hold_counter == SIG_HOLD_THRS)){

				if(hold_counter == SIG_HOLD_THRS){
					msg_end_qalif++;
				}
				
				if( (hold_counter > THOMSON_REPEAT_THR) && (hold_counter < THOMSON_HOLD_THR) ){
				
					signal_periods[idx] = 0xFF;
				}
				else if( (hold_counter > SIG_HIGH_THR) && (hold_counter <= THOMSON_REPEAT_THR) ){
					signal_periods[idx] = 1;
				}
				else{
					signal_periods[idx] = 0;
				}

				idx++;
				hold_counter = 0;
			}
		
			hold_counter++;
			sig_lvl_prev = sig_lvl;

			if(msg_end_qalif >= SIG_QALIF_THR){
				dec_state = DECODE_MSG;
				break;
			}

		}

		dec_state = DECODE_MSG;
		}
		break;

		case DECODE_MSG:
		{
		
		signal_code = 0;
		signal_shifter = 0;

		for(idx = 0; idx < 75; idx++){
			
			
			if( signal_periods[idx] == 0xFF ){
				if(signal_shifter == THOMSON_MAX_BITS){
					signal_shifter = 0;
				}
				else{
					signal_shifter = 0;
					signal_code = 0;
				}
				
			}
			else if( signal_periods[idx] == 1 ){
				signal_code |= ( (uint32_t)0x1 << signal_shifter);
				signal_shifter++;
			}
			else{
				signal_shifter++;
			}

			signal_periods[idx] = 0;
		}

		LED_ON;
		_delay_ms(100);
		LED_OFF;
		msg_counter++;
		dec_state = IDLE;

		if( (uint8_t)(signal_code >> 16) == 0x0A){
			stop_turbine();
		}
		else if( (uint8_t)(signal_code >> 16) == 0x82){
			if(set_turbine( (uint16_t)(turbine_lvl + 50)) ){
				turbine_lvl += 50;
			}
		}
		else if( (uint8_t)(signal_code >> 16) == 0x22){
			if(set_turbine( (uint16_t)(turbine_lvl - 50)) ){
				turbine_lvl -= 50;
			}
		}
		else if( (uint8_t)(signal_code >> 16) == 0x88){
			max_turbine();
		}
		
		dbg_putchar( (uint8_t) signal_code);
		dbg_putchar( (uint8_t) (signal_code >> 8) );
		dbg_putchar( (uint8_t) (signal_code >> 16) );
		dbg_putchar( (uint8_t) (signal_code >> 24) );
		}
		break;
		default:
		{
		}
		break;
	}


	}

}
