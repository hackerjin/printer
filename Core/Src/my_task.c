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
        button_task,  // ������
        "button_task", // ������
        128,         // ����ջ
        NULL,         // �������
        0,            // �������ȼ�, with 3 (configMAX_PRIORITIES - 1) ����ߵģ�0����͵�.
        NULL          // ������
    );
    
   
     xTaskCreate(
        report_task,  // ������
        "report_task", // ������
        128,         // ����ջ
        NULL,         // �������
        3,            // �������ȼ�, with 3 (configMAX_PRIORITIES - 1) ����ߵģ�0����͵�.
        NULL          // ������
    );
    
    
   
    
    xTaskCreate(
        ble_task,  // ������
        "ble_task", // ������
        256,         // ����ջ
        NULL,         // �������
        5,            // �������ȼ�, with 3 (configMAX_PRIORITIES - 1) ����ߵģ�0����͵�.
        NULL          // ������
    );
    
    
    
    
}