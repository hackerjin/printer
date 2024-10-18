#include "motor.h"
#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "tim.h"
/*
4step 行进一点行
对应PIN
A- A B- B
0  1 1  0
0  0 1  0
1  0 1  0
1  0 0  0
1  0 0  1
0  0 0  1
0  1 0  1
0  1 0  0
*/



uint8_t motor_pos = 0;

uint8_t motor_table[8][4] =
    {
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {1, 0, 1, 0},
        {1, 0, 0, 0},
        {1, 0, 0, 1},
        {0, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 1, 0, 0}};

osTimerId conti_move_timer_handle;
		

    
        
static void motor_write(int pin,int PinState)
{
	
    HAL_GPIO_WritePin(GPIOB,pin,(GPIO_PinState)PinState);
	
}


void  conti_move(void const * argument)
{
  
    
    motor_write(MOTOR1_Pin, motor_table[motor_pos][0]);
    motor_write(MOTOR2_Pin, motor_table[motor_pos][1]);
    motor_write(MOTOR3_Pin, motor_table[motor_pos][2]);
    motor_write(MOTOR4_Pin, motor_table[motor_pos][3]);
    motor_pos++;
    if (motor_pos >= 8)
    {
        motor_pos = 0;
    }
}	

void motor_start()
{
    
	if( conti_move_timer_handle == NULL)
    {
        osTimerDef(conti_move_timer,conti_move);
        conti_move_timer_handle =  osTimerCreate(osTimer(conti_move_timer),osTimerPeriodic,NULL);
	}
    //每2毫秒移动一次
	osTimerStart(conti_move_timer_handle,2); 
}

void motor_stop()
{
   
    motor_write(MOTOR1_Pin, 0);
    motor_write(MOTOR2_Pin, 0);
    motor_write(MOTOR3_Pin, 0);
    motor_write(MOTOR4_Pin, 0);
    if(conti_move_timer_handle != NULL)
        osTimerStop(conti_move_timer_handle);
}

void motor_run()
{
    motor_write(MOTOR1_Pin, motor_table[motor_pos][0]);
    motor_write(MOTOR2_Pin, motor_table[motor_pos][1]);
    motor_write(MOTOR3_Pin, motor_table[motor_pos][2]);
    motor_write(MOTOR4_Pin, motor_table[motor_pos][3]);
    motor_pos++;
    if (motor_pos >= 8)
    {
        motor_pos = 0;
    }
}

void motor_run_steps(uint8_t steps)
{
    while (steps)
    {
        motor_write(MOTOR1_Pin, motor_table[motor_pos][0]);
        motor_write(MOTOR2_Pin, motor_table[motor_pos][1]);
        motor_write(MOTOR3_Pin, motor_table[motor_pos][2]);
        motor_write(MOTOR4_Pin, motor_table[motor_pos][3]);
        motor_pos++;
        if (motor_pos >= 8)
        {
            motor_pos = 0;
        }
        us_delay(WAIT_TIME);
        steps--;
    }
}

void init_motor()
{
    motor_write(MOTOR1_Pin, motor_table[motor_pos][0]);
    motor_write(MOTOR2_Pin, motor_table[motor_pos][1]);
    motor_write(MOTOR3_Pin, motor_table[motor_pos][2]);
    motor_write(MOTOR4_Pin, motor_table[motor_pos][3]);
}
