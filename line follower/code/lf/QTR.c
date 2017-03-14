#include "QTR.h"
#include <avr/io.h>

#define S1 PINB5
#define S2 PINC0
#define S3 PINC1
#define S4 PIND3
#define S5 PIND7
#define S6 PINB4
#define S7 PINB3
#define S8 PINB2


void QTR_init()
{

	DDRA = 0x0;
	DDRB &= 0xFC;

	PORTA = 0xFF;
	PORTB = 0x3;

}

void QTR_read(uint8_t *values)
{

	values[0] = (PINB >> S1) & 0x1;
    values[1] = (PINC >> S2) & 0x1;
    values[2] = (PINC >> S3) & 0x1;
    values[3] = (PIND >> S4) & 0x1;
    values[4] = (PIND >> S5) & 0x1;
    values[5] = (PINB >> S6) & 0x1;
    values[6] = (PINB >> S7) & 0x1;
    values[7] = (PINB >> S8) & 0x1;

}


uint8_t QTR_compute()
{
	
	uint8_t value = 0;
	value = (PINA & 0xFC) << 1;
	value |= ( (PINB >> 1) & 0x1);

	return value;
}


