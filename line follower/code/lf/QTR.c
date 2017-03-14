#include "QTR.h"
#include <avr/io.h>

/*
	To be used to initialize the sensors

*/

void QTR_init()
{

	DDRA = 0x0;
	DDRB &= 0xFC;

	PORTA = 0xFF;
	PORTB = 0x3;

}

/*
	To be used to read sensors value

*/

uint8_t QTR_compute()
{
	
	uint8_t value = 0;
	value = (PINA & 0xFC) << 1;
	value |= ( (PINB >> 1) & 0x1);

	return value;
}


