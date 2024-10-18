#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "printer.h"



extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


extern void set_read_ble_finish(bool finish);

//AT指令
char *ble_enter_at_mode = "+++";
char *ble_close_status = "AT+STATUS=0\r\n"; //关闭设备状态显示
char *ble_set_name = "AT+NAME=Mini-Printer\r\n"; //OK 大写？
char *ble_exit_at_mode = "AT+EXIT\r\n";

typedef enum{
        BLE_INIT_START = 0,
        BLE_IN_AT_MODE,
        BLE_CLOSE_STATUS,
        BLE_SET_NAME,
        BLE_EXIT_AT_MODE,
        BLE_RUN,
}ble_init_step;


ble_init_step cur_step = BLE_INIT_START;

uint8_t retry_count = 0;

uint8_t ble_buffer[48];
uint8_t cur_index = 0;



void init_ble()
{
    while(1)
    {
        
         switch(cur_step)
        {
            case BLE_INIT_START:
            {   retry_count++;
                printf("发送进入AT模式的AT指令\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_enter_at_mode,strlen(ble_enter_at_mode),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("重复进入AT模式失败\n"); 
                  
                }
                break;
                
            }
            case BLE_IN_AT_MODE:
            {   
                retry_count++;
                printf("发送关闭状态显示的AT指令\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_close_status,strlen(ble_close_status),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("关闭状态显示失败\n"); 
                  
                }
                break;
                
            }
            case BLE_CLOSE_STATUS:
            {   
                retry_count++;
                printf("发送设置设备名的AT指令\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_set_name,strlen(ble_set_name),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("设置设备名失败\n"); 
                  
                }
                break;
                
            }
            case BLE_SET_NAME:
            {   
                retry_count++;
                printf("发送退出AT模式的AT指令\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_exit_at_mode,strlen(ble_exit_at_mode),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("退出AT模式失败\n"); 
                  
                }
                break;
                
            }
            case BLE_EXIT_AT_MODE:
            { 
                cur_step = BLE_RUN;
                printf("蓝牙初始化成功\n");
                break;
            }
            default:
                break;
            
        
        }
         
        if(cur_step == BLE_RUN)
            break;
        
        vTaskDelay(1000);       
    }
     
}

extern xQueueHandle print_queue;

uint16_t line_count = 0;

uint8_t end_byte_num = 0;

void ble_data_process(uint8_t ble_data)
{
    
    ble_buffer[cur_index] = ble_data;
    
    //只有在正式运行前，将ble_buffer中的数据解释为字符，之后则将其解释为数值
    if(cur_step != BLE_RUN)
    {
        //如果来自蓝牙的回复中含有ok，则说明AT操作成功,并且需要清空缓存。否则下一步操作是否返回了OK无法判断
        if(strstr((char*)ble_buffer,"OK\r\n"))
        {
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
            
            
            switch(cur_step)
            {
                case BLE_INIT_START:
                    retry_count = 0;
                    cur_step = BLE_IN_AT_MODE;
                    printf("成功进入AT模式\n");
                    break;
                case BLE_IN_AT_MODE:
                { 
                    retry_count = 0;
                    cur_step = BLE_CLOSE_STATUS;
                    printf("关闭状态显示成功\n");
                    break;
                    
                }
                case BLE_CLOSE_STATUS:
                { 
                    retry_count = 0;
                    cur_step = BLE_SET_NAME;
                    printf("设置设备名称成功\n");
                    break;
                    
                }
                case BLE_SET_NAME:
                { 
                    retry_count = 0;
                    cur_step = BLE_EXIT_AT_MODE;
                    printf("退出AT模式成功\n");
                    break;
                    
                }
                default:
                    break;
                
                
             }
            
        }
        else
        {
            cur_index++;
            if(cur_index == sizeof(ble_buffer))
                cur_index = 0;
        }
        
  
    }
    //蓝牙初始化成功之后的数据处理
    else
    {
        if(ble_data == 0xa6)
            end_byte_num += 1;
        else
            end_byte_num = 0;
        
        //收到不期望的状态数据，清空
        if(strstr((char*)ble_buffer, "CONNECTED") || strstr((char*)ble_buffer, "DISCONNECTED") || strstr((char*)ble_buffer, "DEVICE ERROR"))
        {
           printf("无关状态信息为%s\n",(char*)ble_buffer);
           printf("清除无关状态信息\n");
           cur_index = 0;
           memset(ble_buffer,0,sizeof(ble_buffer));
                    
        }       
        //热密度设置或者图片数据传输结束
        else if(cur_index == 4 && (ble_buffer[0] == 0xa5 && ble_buffer[1] == 0xa5 && ble_buffer[2] == 0xa5 && ble_buffer[3] == 0xa5) )
            
        {
            printf("当前index为%u,数据为%X\n",cur_index,ble_data);
            
               
            if(ble_buffer[4] == 1)                 
                set_heat_density(30);
            else if(ble_buffer[4] == 2)
                set_heat_density(60);
            else
                set_heat_density(100);
        
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
            
            printf("热密度设置成功\n");
            
               
            
       }  
         //一行数据接收完毕
        else if(cur_index == 47)
        {
            
            line_count++;
            xQueueSendFromISR(print_queue,ble_buffer,0);
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
        }   
        else
        {
            cur_index++;
            if(cur_index == sizeof(ble_buffer))
                cur_index = 0;
        }
       
        if(end_byte_num == 4 )
        {
            
            
            printf("图片传输结束，总行数为%u\n",line_count);
            end_byte_num = 0;
            line_count = 0;
            
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
            set_read_ble_finish(true);
            
        }                  
        
        
    }
      
}


void ble_task(void* arg)
{
    printf("开始运行蓝牙任务\n");
    
    printf("开始初始化蓝牙\n");
    
    init_ble();
    
    printf("蓝牙开始接收数据\n");
    
    while(1)
    {
        
        vTaskDelay(1000);
        
        
    }
        
        
        
    
    
    
    
}












