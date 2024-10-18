#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <stdint.h>



xQueueHandle print_queue;


void init_print_queue()
{
    print_queue = xQueueCreate(275,48);
    
    
}   



