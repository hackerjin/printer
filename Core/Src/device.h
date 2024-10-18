#ifndef _DEVICE_H
#define _DEVICE_H
#include<stdint.h>
#include<stdbool.h>

typedef enum{
    PRINTER_STATUS_INIT = 0,
    PRINTER_STATUS_START,
    PRINTER_STATUS_WORKING,
    PRINTER_STATUS_FINISH,
}printer_state;

typedef enum{
    PAPER_STATUS_NORMAL = 0,
    PAPER_STATUS_LACK,
}paper_state;

typedef struct
{
    uint8_t battery;
    uint8_t temperature;
    paper_state paper_state;
    printer_state printer_state; 
	bool read_ble_finish;
    
}device_state;


void init_device_state();

device_state * get_device_state();

void set_read_ble_finish(bool finish);

void set_device_paper_status(paper_state status);

#endif
