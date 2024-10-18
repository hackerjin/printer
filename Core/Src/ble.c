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

//ATָ��
char *ble_enter_at_mode = "+++";
char *ble_close_status = "AT+STATUS=0\r\n"; //�ر��豸״̬��ʾ
char *ble_set_name = "AT+NAME=Mini-Printer\r\n"; //OK ��д��
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
                printf("���ͽ���ATģʽ��ATָ��\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_enter_at_mode,strlen(ble_enter_at_mode),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("�ظ�����ATģʽʧ��\n"); 
                  
                }
                break;
                
            }
            case BLE_IN_AT_MODE:
            {   
                retry_count++;
                printf("���͹ر�״̬��ʾ��ATָ��\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_close_status,strlen(ble_close_status),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("�ر�״̬��ʾʧ��\n"); 
                  
                }
                break;
                
            }
            case BLE_CLOSE_STATUS:
            {   
                retry_count++;
                printf("���������豸����ATָ��\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_set_name,strlen(ble_set_name),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("�����豸��ʧ��\n"); 
                  
                }
                break;
                
            }
            case BLE_SET_NAME:
            {   
                retry_count++;
                printf("�����˳�ATģʽ��ATָ��\n");
                HAL_UART_Transmit(&huart2,(uint8_t*)ble_exit_at_mode,strlen(ble_exit_at_mode),1000); 
                if(retry_count == 10)
                {
                  retry_count = 0;
                  printf("�˳�ATģʽʧ��\n"); 
                  
                }
                break;
                
            }
            case BLE_EXIT_AT_MODE:
            { 
                cur_step = BLE_RUN;
                printf("������ʼ���ɹ�\n");
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
    
    //ֻ������ʽ����ǰ����ble_buffer�е����ݽ���Ϊ�ַ���֮���������Ϊ��ֵ
    if(cur_step != BLE_RUN)
    {
        //������������Ļظ��к���ok����˵��AT�����ɹ�,������Ҫ��ջ��档������һ�������Ƿ񷵻���OK�޷��ж�
        if(strstr((char*)ble_buffer,"OK\r\n"))
        {
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
            
            
            switch(cur_step)
            {
                case BLE_INIT_START:
                    retry_count = 0;
                    cur_step = BLE_IN_AT_MODE;
                    printf("�ɹ�����ATģʽ\n");
                    break;
                case BLE_IN_AT_MODE:
                { 
                    retry_count = 0;
                    cur_step = BLE_CLOSE_STATUS;
                    printf("�ر�״̬��ʾ�ɹ�\n");
                    break;
                    
                }
                case BLE_CLOSE_STATUS:
                { 
                    retry_count = 0;
                    cur_step = BLE_SET_NAME;
                    printf("�����豸���Ƴɹ�\n");
                    break;
                    
                }
                case BLE_SET_NAME:
                { 
                    retry_count = 0;
                    cur_step = BLE_EXIT_AT_MODE;
                    printf("�˳�ATģʽ�ɹ�\n");
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
    //������ʼ���ɹ�֮������ݴ���
    else
    {
        if(ble_data == 0xa6)
            end_byte_num += 1;
        else
            end_byte_num = 0;
        
        //�յ���������״̬���ݣ����
        if(strstr((char*)ble_buffer, "CONNECTED") || strstr((char*)ble_buffer, "DISCONNECTED") || strstr((char*)ble_buffer, "DEVICE ERROR"))
        {
           printf("�޹�״̬��ϢΪ%s\n",(char*)ble_buffer);
           printf("����޹�״̬��Ϣ\n");
           cur_index = 0;
           memset(ble_buffer,0,sizeof(ble_buffer));
                    
        }       
        //���ܶ����û���ͼƬ���ݴ������
        else if(cur_index == 4 && (ble_buffer[0] == 0xa5 && ble_buffer[1] == 0xa5 && ble_buffer[2] == 0xa5 && ble_buffer[3] == 0xa5) )
        {
            printf("��ǰindexΪ%u,����Ϊ%X\n",cur_index,ble_data);
            
               
            if(ble_buffer[4] == 1)                 
                set_heat_density(30);
            else if(ble_buffer[4] == 2)
                set_heat_density(60);
            else
                set_heat_density(100);
            
            cur_index = 0;
            memset(ble_buffer,0,sizeof(ble_buffer));
            
            printf("���ܶ����óɹ�\n");
       }  
         //һ�����ݽ������
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
            
            
            printf("ͼƬ���������������Ϊ%u\n",line_count);
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
    printf("��ʼ������������\n");
    
    printf("��ʼ��ʼ������\n");
    
    init_ble();
    
    printf("������ʼ��������\n");
    
    while(1)
    {
        
        vTaskDelay(1000);
        
        
    }
        
        
        
    
    
    
    
}












