#include "main.h"
#include "device.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"

extern UART_HandleTypeDef huart2;
extern void read_sensors();

device_state g_device_state;


static void init_device_state(){
    memset(&g_device_state,0,sizeof(device_state));
    g_device_state.printer_state = PRINTER_STATUS_INIT;
    g_device_state.battery = 100;
    g_device_state.temperature = 30;
    g_device_state.paper_state = PAPER_STATUS_NORMAL;
    g_device_state.read_ble_finish = false;
}


device_state * get_device_state(){
    return &g_device_state;
}

void set_device_paper_status(paper_state status){
    g_device_state.paper_state = status;
}

void set_read_ble_finish(bool finish){
    g_device_state.read_ble_finish = finish;
}



static void report_device_state()
{
   
    device_state *pdevice = get_device_state();
    
    uint8_t status[4];
    status[0] = pdevice->battery;
    status[1] = pdevice->temperature;
    status[2] = pdevice->paper_state;
    status[3] = pdevice->printer_state;
    HAL_UART_Transmit(&huart2,(uint8_t*)&status,sizeof(status),0xffff);
    
}


void report_task(void* arg)
{
    
    printf("开始运行报告任务\n");
    init_device_state();
    while(1)
    {
        read_sensors();
        report_device_state();
        vTaskDelay(5000);
        
    }
    
    
    
}

