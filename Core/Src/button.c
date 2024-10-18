#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include "button.h"
#include "device.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
extern void motor_start();
extern void motor_stop();


#define SHORT_PRESS_TIME 1000

bool keyIsPress = false;
unsigned long clicktime = 0;
bool longPressIsStart = false;

static int btnRead()
{
	return HAL_GPIO_ReadPin(BTN_GPIO_Port,BTN_Pin);
}

static uint32_t millis(){
    
	return HAL_GetTick();
}


bool printer_test = false;

void key_click_handle()
{
    printf("Button 单击!\n");
    printer_test = true;
    
}

void key_long_click_handle()
{
    
    printf("Button 长按!\n");
    
    device_state *pdevice = get_device_state();
            
    // 不缺纸且不在打印中才执行
    if (pdevice->paper_state == PAPER_STATUS_NORMAL)
    {
        if (pdevice->printer_state == PRINTER_STATUS_FINISH ||
            pdevice->printer_state == PRINTER_STATUS_INIT)
        {
            printf("开始走纸\n");
            motor_start();
        }
       
    }
}

void key_long_click_free_handle()
{
    
    printf("停止走纸\n");
    motor_stop();
}



void button_scan()
{
    
    
  if(keyIsPress == false)
  { 
    //检测到按下
    if(btnRead() == 0)
    {
      //进行延时防抖
      vTaskDelay(10);
      //如果再次检测到则说明按下了
      if(btnRead() == 0)
      {
        keyIsPress = true;
        //记录当前tick
        clicktime = millis();
      }
    }
  }
  
  
  //按下的情况下，检测什么时候放开
  if(keyIsPress == true)
  {
    //已经放开按键
    if(btnRead() == 1)
    {
      if(millis() - clicktime > SHORT_PRESS_TIME)
      {
        printf("long click free\n");
        key_long_click_free_handle();
        longPressIsStart = false;
      }
      else
      {
         
        printf("short click\n");
        key_click_handle();
      }
      keyIsPress = false;
    }
    //没有放开按键
    else
    {
      if(millis() - clicktime > SHORT_PRESS_TIME)
      {
        if(longPressIsStart == false)
        {
          printf("long click\n");
          key_long_click_handle();
          longPressIsStart = true;
        }
      }
    }
  }
  
}




void button_task(void* arg)
{
    printf("开启运行按键任务\n");
    
    unsigned int  count = 0;
    while(1)
    {
        button_scan();
        
        vTaskDelay(20);
        
        count++;
        if(count == 250)
        {
           
            count = 0;
        }
        
    }
    
    
    
}

