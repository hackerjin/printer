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
    printf("Button ����!\n");
    printer_test = true;
    
}

void key_long_click_handle()
{
    
    printf("Button ����!\n");
    
    device_state *pdevice = get_device_state();
            
    // ��ȱֽ�Ҳ��ڴ�ӡ�в�ִ��
    if (pdevice->paper_state == PAPER_STATUS_NORMAL)
    {
        if (pdevice->printer_state == PRINTER_STATUS_FINISH ||
            pdevice->printer_state == PRINTER_STATUS_INIT)
        {
            printf("��ʼ��ֽ\n");
            motor_start();
        }
       
    }
}

void key_long_click_free_handle()
{
    
    printf("ֹͣ��ֽ\n");
    motor_stop();
}



void button_scan()
{
    
    
  if(keyIsPress == false)
  { 
    //��⵽����
    if(btnRead() == 0)
    {
      //������ʱ����
      vTaskDelay(10);
      //����ٴμ�⵽��˵��������
      if(btnRead() == 0)
      {
        keyIsPress = true;
        //��¼��ǰtick
        clicktime = millis();
      }
    }
  }
  
  
  //���µ�����£����ʲôʱ��ſ�
  if(keyIsPress == true)
  {
    //�Ѿ��ſ�����
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
    //û�зſ�����
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
    printf("�������а�������\n");
    
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

