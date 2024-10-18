#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include<stdio.h>



extern void button_task(void* arg);

extern void ble_task(void* arg);

extern void report_task(void* arg);


void start_tasks()
{
    
     xTaskCreate(
        button_task,  // 任务函数
        "button_task", // 任务名
        128,         // 任务栈
        NULL,         // 任务参数
        0,            // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL          // 任务句柄
    );
    
   
     xTaskCreate(
        report_task,  // 任务函数
        "report_task", // 任务名
        128,         // 任务栈
        NULL,         // 任务参数
        3,            // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL          // 任务句柄
    );
    
    
   
    
    xTaskCreate(
        ble_task,  // 任务函数
        "ble_task", // 任务名
        256,         // 任务栈
        NULL,         // 任务参数
        5,            // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL          // 任务句柄
    );
    
    
    
    
}