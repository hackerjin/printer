#include "main.h"
#include "printer.h"
#include "motor.h"
#include <stdio.h>
#include "tim.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"


#define PRINT_TIME 2600         //打印加热时间
#define PRINT_END_TIME 200      //冷却时间
#define LAT_TIME 1              //数据锁存时间


extern void spi_transmit(uint8_t* data,uint8_t len);
extern xQueueHandle print_queue;

// 热密度
uint8_t heat_density = 30;

float add_time[6] = {0};

void set_heat_density(uint8_t density)
{
    printf("打印密度设置 %d\n", density);
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
    //计算加热时间
    for(int stb_num = 0; stb_num < 6;stb_num++)
    {
        float temp_time = 0;
        for(int byte_num = 0;byte_num < 8;byte_num++)
        {
            temp_time += data[stb_num*8 + byte_num];
            
        }
        
        add_time[stb_num] = temp_time*temp_time*0.001;
        
        
    }
    
    //通过spi传输数据
    spi_transmit(data,48);
    
    //传输数据之后，进行锁存
    latch_write(RESET);
    us_delay(1);
    latch_write(SET);
    
}


static void  print_line()
{
    for(int i=0;i< 6;i++)
    {
        stb_write(i,SET);
        us_delay ( (PRINT_TIME + add_time[i]) *  (float)heat_density/100  );
        stb_write(i, RESET);
        us_delay(PRINT_END_TIME);
    }
    
}


void print_and_move(uint8_t* data)
{
   
    send_one_line(data);
    print_line();
    
    motor_run_steps(4);
    
}

void print_in_time()
{        

   



    
        
}



void print(uint8_t* data ,uint16_t len)
{
   for(int i= 0; i< len /48; i++)
   {
      
       print_and_move(data + i*48);
       
   }
    
    
}
