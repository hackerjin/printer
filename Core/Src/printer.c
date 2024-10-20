#include "main.h"
#include "printer.h"
#include "motor.h"
#include <stdio.h>
#include "tim.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <stdbool.h>

#define PRINT_TIME 2600         //��ӡ����ʱ��
#define PRINT_END_TIME 200      //��ȴʱ��
#define LAT_TIME 1              //��������ʱ��


extern void spi_transmit(uint8_t* data,uint8_t len);
extern xQueueHandle print_queue;

// ���ܶ�
uint8_t heat_density = 30;

float add_time[6] = {0};

void set_heat_density(uint8_t density)
{
    printf("��ӡ�ܶ����� %d\n", density);
    heat_density = density;
}

uint16_t stb_pins[6] = { STB1_Pin,STB2_Pin,STB3_Pin,STB4_Pin,STB5_Pin,STB6_Pin};

static void stb_write(uint8_t stb_num,int PinState)
{
	
    HAL_GPIO_WritePin(GPIOB,stb_pins[stb_num],(GPIO_PinState)PinState);
	
}

static void latch_write(int PinState)
{
     HAL_GPIO_WritePin(GPIOB,LAT_Pin,(GPIO_PinState)PinState);    
}

static void power_write(int PinState)
{
	HAL_GPIO_WritePin(GPIOA,VH_EN_Pin,(GPIO_PinState)PinState);
}

static void set_stb_idle()
{
    stb_write(0,RESET);
    stb_write(1,RESET);
    stb_write(2,RESET);
    stb_write(3,RESET);
    stb_write(4,RESET);
    stb_write( 5,RESET);

   
}

static void  init_print()
{
  
    set_stb_idle();
    latch_write(SET);
    power_write(SET);
}


static void deinit_print()
{
  
    set_stb_idle();
    latch_write(SET);
    power_write(RESET);
}




static void send_one_line(uint8_t *data)
{
    //�������ʱ��
    for(int stb_num = 0; stb_num < 6;stb_num++)
    {
        float temp_time = 0;
        for(int byte_num = 0;byte_num < 8;byte_num++)
        {
            temp_time += data[stb_num*8 + byte_num];
            
        }
        
        add_time[stb_num] = temp_time*temp_time*0.001;
        
        
    }
    
    //ͨ��spi��������
    spi_transmit(data,48);
    
    //��������֮�󣬽�������
    latch_write(RESET);
    us_delay(1);
    latch_write(SET);
    
}


static void  print_line()
{
    for(int i=0;i< 6;i++)
    {
        stb_write(i,SET);
        //us_delay ( (PRINT_TIME + add_time[i]) *  ((double)heat_density/100));
        us_delay(5000);
        stb_write(i, RESET);
        us_delay(PRINT_END_TIME);
    }
    
}


void print_and_move(uint8_t* data)
{
   
    send_one_line(data);
    
    motor_run_steps(1);
    print_line();
    motor_run_steps(3);
    
}

uint8_t print_buffer[48];


void print_task(void* arg)
{        
    printf("��ʼ���д�ӡ����\n");
    bool is_start = false;
    while(1)
    {
        if(xQueueReceive(print_queue,print_buffer,5000))
        {
            if(!is_start)
            {          
                //init_print();
                is_start = true;
            }
            print_and_move(print_buffer);
        }
        else
        {
           if(is_start)
               deinit_print();
           is_start = false;
            vTaskDelay(100);
        }
    }
    
}

