#ifndef _PID_H
#define _PID_H


#include "stdint.h"

void pid_init();
extern void pid_setpoint(int16_t point);
extern int8_t pid_calculate(int16_t value);
extern void pid_set_consts(int16_t p, int16_t i, int16_t d);

#endif // _PID_H
