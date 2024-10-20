#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stdio.h>


xQueueHandle print_queue;


void init_print_queue()
{
    print_queue = xQueueCreate(200,48);
    
    if(print_queue != NULL)
      printf("打印队列创建成功\n");
    else
      printf("打印队列创建失败\n");
}   



