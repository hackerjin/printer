#ifndef _MOTOR_H
#define _MOTOR_H
#include<stdint.h>

#define WAIT_TIME 4000 
void init_motor(void);
void motor_start(void);
void motor_stop(void);
void motor_run(void);
void motor_run_steps(uint32_t steps);
#endif

